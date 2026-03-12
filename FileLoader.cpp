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

std::string FileLoader::normalizeFileName(const char* fileName)
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

void FileLoader::reloadCallback(IO::FileMonitor::Event& event)
	{
	/* Bail out if the file didn't actually change: */
	if((event.eventMask&IO::FileMonitor::ClosedWrite)==0x0U)
		return;
	
	#if LAMBDA_CONFIG_INSTRUMENT
	
	/* Reset the evaluation counters: */
	Thing::resetCounters();
	
	#endif
	
	try
		{
		/* Retrieve the absolute file name from the file monitor: */
		std::string path=event.fileMonitor.getWatchedPath(event.cookie);
		
		/* Load the file in the root context: */
		std::cout<<"Reloading file "<<path<<std::endl;
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

FileLoader::FileLoader(Context& sRootContext)
	:rootContext(sRootContext),
	 currentDirectory(IO::Directory::getCurrent())
	{
	}

void FileLoader::block(int blockFd)
	{
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
	}

void FileLoader::loadFile(const char* fileName)
	{
	#if LAMBDA_CONFIG_INSTRUMENT
	
	/* Reset the evaluation counters: */
	Thing::resetCounters();
	
	#endif
	
	try
		{
		/* Turn the file name into a normalized absolute file name: */
		std::string path=normalizeFileName(fileName);
		
		/* Watch the absolute file name for changes: */
		fileMonitor.addPath(path.c_str(),IO::FileMonitor::AllEvents,*Threads::createFunctionCall(this,&FileLoader::reloadCallback));
		
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
		/* Watch the absolute file name for changes: */
		fileMonitor.addPath(path.c_str(),IO::FileMonitor::AllEvents,*Threads::createFunctionCall(this,&FileLoader::reloadCallback));
		}
	
	/* Load the file in the given context: */
	std::cout<<"Loading file "<<path<<std::endl;
	load(path,context);
	}

}
