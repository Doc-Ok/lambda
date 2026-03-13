/***********************************************************************
FileLoader - Class to load and watch input files for the Lambda
Programming language.
Copyright (c) 2017-2026 Oliver Kreylos

This file is part of the Lambda Programming Language.

The Lambda Programming Language is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The Lambda Programming Language is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with the Lambda Programming Language; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#include "FileLoader.h"

#include <string.h>
#include <poll.h>
#include <iostream>
#include <Threads/FunctionCalls.h>

#include "Config.h"
#include "Context.h"
#include "Thing.h"
#include "InputStream.h"
#include "Parser.h"

namespace Lambda {

/***********************************************
Declaration of class FileLoader::ReloadCallback:
***********************************************/

class FileLoader::ReloadCallback:public Threads::FunctionCall<IO::FileMonitor::Event&>
	{
	/* Elements: */
	private:
	Context& context; // The evaluation context in which the monitored file will be reloaded
	const char*& reloadPrompt; // Reference to the file loader's current reload prompt
	std::string fileName; // The name of the monitored file inside its parent directory
	bool modified; // Flag if the monitored file has been modified since the last time it was reloaded
	
	/* Constructors and destructors: */
	public:
	ReloadCallback(Context& sContext,const char*& sReloadPrompt,const std::string& sFileName) // Elementwise constructor
		:context(sContext),reloadPrompt(sReloadPrompt),
		 fileName(sFileName),modified(false)
		{
		}
	
	/* Methods from class Threads::FunctionCall<IO::FileMonitor::Event& event>: */
	virtual void operator()(IO::FileMonitor::Event& event) // Method called when an event occurs on the monitored file
		{
		/* Bail out if the event does not apply to the monitored file: */
		if(event.name==0||fileName!=event.name)
			return;
		
		#if 0
		// DEBUGGING
		std::cout<<"Received an event from directory "<<event.fileMonitor.getWatchedPath(event.cookie);
		if(event.name!=0)
			std::cout<<" for file "<<event.name;
		std::cout<<" with event mask "<<event.eventMask<<std::endl;
		#endif
		
		/* Remember if the monitored file was modified: */
		if((event.eventMask&IO::FileMonitor::Modified)!=0x0U)
			modified=true;
		
		/* Check if it's time to reload the file: */
		if((event.eventMask&IO::FileMonitor::MovedTo)!=0x0U||(modified&&(event.eventMask&IO::FileMonitor::ClosedWrite)!=0x0U))
			{
			/* Construct the absolute file name of the monitored file: */
			std::string path=event.fileMonitor.getWatchedPath(event.cookie);
			path+=fileName;
			
			/* Start a new line: */
			std::cout<<std::endl;
			
			try
				{
				/* Load the file in the root context: */
				std::cout<<"Reloading file "<<path<<std::endl;
				load(path,context);
				}
			catch(const std::runtime_error& err)
				{
				std::cout<<"Error: "<<err.what()<<std::endl;
				}
			
			#if LAMBDA_CONFIG_INSTRUMENT
			
			/* Print the evaluation counters: */
			Thing::printCounters(std::cout);
			std::cout<<std::endl;
			
			#endif
			
			/* Print the current reload prompt: */
			if(reloadPrompt!=0)
				std::cout<<reloadPrompt<<std::flush;
			
			/* Mark the monitored file as no longer modified: */
			modified=false;
			}
		}
	};

/***************************
Methods of class FileLoader:
***************************/

void FileLoader::load(const std::string& path,Context& context)
	{
	/* Create an InputStream to read the file: */
	InputStream inputStream(path.c_str());
	
	/* Parse the input stream until end-of-file: */
	while(true)
		{
		/* Parse an expression from the input stream and bail out if the expression is null at end-of-file: */
		inputStream.skipWs();
		ThingPtr expression=parse(inputStream);
		if(expression==0)
			break;
		
		/* Evaluate the expression and ignore the result: */
		expression->evaluate(context);
		}
	}

std::string FileLoader::normalizeFileName(const char* fileName) const
	{
	/* Check if the file name has an extension: */
	std::string path;
	const char* extPtr=0;
	for(const char* fnPtr=fileName;*fnPtr!='\0';++fnPtr)
		{
		if(*fnPtr=='.')
			extPtr=fnPtr;
		else if(*fnPtr=='/')
			extPtr=0;
		path.push_back(*fnPtr);
		}
	if(extPtr==0)
		{
		/* Append the .lambda extension to the file name: */
		path.append(".lambda");
		}
	
	/* Normalize and return the path name: */
	return currentDirectory->getPath(path.c_str());
	}

void FileLoader::monitorFile(const std::string& path)
	{
	/* Split the path into containing directory and contained file: */
	std::string::const_iterator fnIt=path.begin();
	for(std::string::const_iterator pIt=path.begin();pIt!=path.end();++pIt)
		if(*pIt=='/')
			fnIt=pIt+1;
	std::string directory(path.begin(),fnIt);
	std::string fileName(fnIt,path.end());
	
	/* Add the containing directory to the file monitor: */
	IO::FileMonitor::EventMask eventMask=0x0U;
	eventMask|=IO::FileMonitor::Modified;
	eventMask|=IO::FileMonitor::ClosedWrite;
	eventMask|=IO::FileMonitor::MovedTo;
	fileMonitor.addPath(directory.c_str(),eventMask,*new ReloadCallback(rootContext,reloadPrompt,fileName));
	}

FileLoader::FileLoader(Context& sRootContext)
	:rootContext(sRootContext),
	 currentDirectory(IO::Directory::getCurrent()),
	 reloadPrompt(0)
	{
	}

void FileLoader::block(int blockFd,const char* newReloadPrompt)
	{
	/* Remember the reload prompt: */
	reloadPrompt=newReloadPrompt;
	
	/* Poll the given file descriptor and the file monitor until the former has data available for reading: */
	struct pollfd pollRequests[2];
	memset(pollRequests,0,2*sizeof(struct pollfd));
	pollRequests[0].fd=blockFd;
	pollRequests[0].events=POLLIN;
	pollRequests[1].fd=fileMonitor.getBlockFd();
	pollRequests[1].events=POLLIN;
	
	do
		{
		/* Block until either file descriptor becomes ready: */
		poll(pollRequests,2,-1);
		
		/* Check if there are file monitoring events: */
		if(pollRequests[1].revents!=0x0)
			fileMonitor.processEvents();
		}
	while(pollRequests[0].revents==0x0);
	
	/* Forget the reload prompt: */
	reloadPrompt=0;
	}

void FileLoader::loadFile(const char* fileName)
	{
	try
		{
		/* Turn the file name into a normalized absolute file name: */
		std::string path=normalizeFileName(fileName);
		
		/* Monitor the normalized absolute file name for changes: */
		monitorFile(path);
		
		/* Load the file in the root context: */
		std::cout<<"Loading file "<<path<<std::endl;
		load(path,rootContext);
		}
	catch(const std::runtime_error& err)
		{
		std::cout<<"Error: "<<err.what()<<std::endl;
		}
	
	#if LAMBDA_CONFIG_INSTRUMENT
	
	/* Print the evaluation counters: */
	Thing::printCounters(std::cout);
	std::cout<<std::endl;
	
	#endif
	}

void FileLoader::loadFile(const char* fileName,Context& context)
	{
	/* Turn the file name into a normalized absolute file name: */
	std::string path=normalizeFileName(fileName);
	
	/* Check if the Builtin::Load was evaluated within the root context: */
	if(&context==&rootContext)
		{
		/* Monitor the normalized absolute file name for changes: */
		monitorFile(path);
		}
	
	/* Load the file in the given context: */
	std::cout<<"Loading file "<<path<<std::endl;
	load(path,context);
	}

}
