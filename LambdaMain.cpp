/***********************************************************************
LambdaMain - Main program for the Lambda Programming Language.
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdexcept>
#include <iostream>
#include <Misc/Autopointer.h>
#include <Misc/Pipe.h>
#include <Threads/Thread.h>
#include <Threads/FunctionCalls.h>

#include "Config.h"
#if LAMBDA_CONFIG_USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
#include <string>
#endif
#include "Context.h"
#include "Void.h"
#include "Load.h"
#include "DefBuiltins.h"
#include "DefTurtleBuiltins.h"
#include "FileLoader.h"
#include "InputStream.h"
#include "Parser.h"

namespace {

Lambda::Context* context=0; // The root expression evaluation context
Lambda::FileLoader* fileLoader=0; // A loader for Lambda Programming Language files

/**************
Helper classes:
**************/

class InputStreamBlocker:public Threads::FunctionCall<int> // Function call class to manage an interactive InputStream
	{
	/* Elements: */
	private:
	Lambda::InputStream& inputStream; // Reference to the input stream
	bool parsing; // Flag if the parser thread is currently parsing an expression
	int signalFd; // File descriptor on which to send status messages to the main thread
	
	/* Constructors and destructors: */
	public:
	InputStreamBlocker(Lambda::InputStream& sInputStream,int sSignalFd) // Elementwise constructor
		:inputStream(sInputStream),
		 parsing(false),signalFd(sSignalFd)
		{
		/* Register this object as the blocking function with the input stream: */
		inputStream.setBlockFunction(*this);
		}
	
	/* Methods from class Threads::FunctionCall<int>: */
	void operator()(int fd)
		{
		/* Signal the main thread that we are blocking, and tell it whether we need more input to finish parsing: */
		char signal=parsing?1:0;
		write(signalFd,&signal,sizeof(signal));
		
		/* Let the file loader block on the input stream's file descriptor: */
		fileLoader->block(fd,parsing?"     ?> ":"Lambda> ");
		}
	
	/* New methods: */
	Lambda::InputStream& getInputStream(void) // Returns the input stream
		{
		return inputStream;
		}
	void setParsing(bool newParsing) // Sets the parsing mode flag
		{
		parsing=newParsing;
		}
	};

typedef Misc::Autopointer<InputStreamBlocker> InputStreamBlockerPtr;

/****************
Helper functions:
****************/

/* Inserts a thing into a std::ostream: */
std::ostream& operator<<(std::ostream& os,const Lambda::Thing& thing)
	{
	return thing.print(os);
	}

/* Runs a parser and evaluator in a background thread: */
void* parserThreadFunction(InputStreamBlocker* threadArg)
	{
	/* Hold a reference to the input stream blocker and retrieve the input stream reference: */
	InputStreamBlockerPtr inputStreamBlocker(threadArg);
	Lambda::InputStream& inputStream=inputStreamBlocker->getInputStream();
	
	while(true)
		{
		try
			{
			/* Parse the next expression from the input stream: */
			inputStream.skipWs();
			inputStreamBlocker->setParsing(true);
			Lambda::ThingPtr expression=Lambda::parse(inputStream);
			inputStreamBlocker->setParsing(false);
			
			/* Bail out if the expression is null at end-of-file: */
			if(expression==0)
				break;
			
			/* Evaluate the expression: */
			Lambda::ThingPtr result=expression->evaluate(*context);
			
			/* Print the result if it is something other than the Void: */
			if(!Lambda::Void::is(*result))
				std::cout<<"= "<<*result<<std::endl;
			}
		catch(const std::runtime_error& err)
			{
			/* Print an error message and continue: */
			std::cout<<"Oopsie: "<<err.what()<<std::endl;
			}
		
		#if LAMBDA_CONFIG_INSTRUMENT
		
		/* Print the evaluation counters: */
		std::cout<<"  ";
		Lambda::Thing::printCounters(std::cout);
		std::cout<<std::endl;
		
		#endif
		}
	
	return 0;
	}

#if LAMBDA_CONFIG_USE_READLINE

/* Second-stage completion function called by the readline library: */
char* generateMatches(const char* text,int state)
	{
	/* List of matching names retrieved from the evaluation context: */
	static std::vector<Lambda::String>* matches=0;
	
	if(state==0)
		{
		/* Retrieve all matching names from the root context's bindings: */
		matches=context->getMatchingNames(text,text+strlen(text));
		}
	
	/* Check if there are more matching names to return: */
	if(state<matches->size())
		{
		/* Copy the string into a new buffer and return it: */
		char* result=static_cast<char*>(malloc((*matches)[state].length()+1));
		memcpy(result,(*matches)[state].c_str(),(*matches)[state].length()+1);
		return result;
		}
	else
		{
		/* Release the list of matching names and return an invalid string: */
		delete matches;
		return 0;
		}
	}

/* First-stage completion function called by the readline library: */
char** complete(const char* text,int start,int end)
	{
	/* Call readline's completion function with the second-stage completion function: */
	return rl_completion_matches(text,generateMatches);
	}

#endif

}

int main(int argc,char* argv[])
	{
	std::cout<<"Welcome to the Lambda Programming Language!"<<std::endl;
	
	/* Create an evaluation context and a file loader: */
	context=new Lambda::Context;
	fileLoader=new Lambda::FileLoader(*context);
	Lambda::Load::setFileLoader(fileLoader);
	
	/* Define the Lambda Programming Language's built-in primitives and functions: */
	std::cout<<"Loading built-ins"<<std::endl;
	Lambda::defBuiltins(*context);
	Lambda::defTurtleBuiltins(*context);
	
	#if LAMBDA_CONFIG_INSTRUMENT
	
	/* Print the evaluation counters: */
	Lambda::Thing::printCounters(std::cout);
	std::cout<<std::endl;
	
	#endif
	
	/* Load all Lambda Programming Language files passed on the command line: */
	for(int i=1;i<argc;++i)
		fileLoader->loadFile(argv[i]);
	
	/* Create a pipe to send input from stdin to the input stream: */
	Misc::Pipe inputPipe;
	
	/* Write a dummy character to the input pipe to prime the pump, so to speak: */
	inputPipe.write<char>(0);
	
	/* Attach an input stream to the read end of the input pipe: */
	Lambda::InputStream inputStream(inputPipe.getReadFd());
	
	/* Create a signal pipe to read results from the parser: */
	Misc::Pipe signalPipe;
	
	/* Create an input stream blocker to communicate with the input stream: */
	InputStreamBlockerPtr inputStreamBlocker=new InputStreamBlocker(inputStream,signalPipe.getWriteFd());
	
	/* Start a background thread to parse and evaluate expressions: */
	Threads::Thread parserThread(parserThreadFunction,inputStreamBlocker.getPointer());
	
	/* Wait for the first blocking signal from the parser thread: */
	char signal;
	signalPipe.read(signal);
	
	#if LAMBDA_CONFIG_USE_READLINE
	
	/* Set up the readline library: */
	rl_readline_name="Lambda"; // Name of this application, for per-application readline configuration
	using_history();
	rl_attempted_completion_function=complete;
	
	#endif
	
	/* Read, evaluate, and print expressions until the user closes the connection: */
	std::cout<<"Press Ctrl+D at the prompt to say goodbye."<<std::endl<<std::endl;
	bool parsing=false;
	while(true)
		{
		#if LAMBDA_CONFIG_USE_READLINE
		
		/* Read a line of input from the readline library and check for end-of-file: */
		char* input=readline(parsing?"     ?> ":"Lambda> "); // Indicate whether the parser is idle or waiting for more input
		if(input==0)
			break;
		
		/* Add the line of input to readline's history: */
		add_history(input);
		
		/* Send the line of input to the parser thread: */
		char* iPtr=input;
		char* iEnd=input+strlen(input)+1; // Send the NUL character at the end of the input string to work around the InputStream's read-ahead
		while(iPtr!=iEnd)
			{
			/* Write as much of the input line as possible and advance the input pointer: */
			iPtr+=inputPipe.write(iPtr,iEnd-iPtr);
			}
		
		/* Release the line of input: */
		free(input);
		
		#else
		
		/* Read a line of input from std::cin: */
		std::cout<<(parsing?"     ?> ":"Lambda> ")<<std::flush;
		std::string input;
		std::getline(std::cin,input);
		if(std::cin.eof())
			break;
		
		/* Send the line of input to the parser thread: */
		const char* iPtr=input.c_str();
		const char* iEnd=iPtr+input.size()+1; // Send the NUL character at the end of the input string to work around the InputStream's read-ahead
		while(iPtr!=iEnd)
			{
			/* Write as much of the input line as possible and advance the input pointer: */
			iPtr+=inputPipe.write(iPtr,iEnd-iPtr);
			}
		
		#endif
		
		/* Wait for the parsing thread to have read all the input: */
		char parserStatus;
		signalPipe.read(parserStatus);
		parsing=parserStatus!=0;
		}
	
	#if LAMBDA_CONFIG_USE_READLINE
	
	/* Clean up after readline as much as possible: */
	rl_clear_history();
	
	#endif
	
	/* Say goodbye and close the write end of the input pipe, then wait for the parser thread to shut down: */
	std::cout<<"Goodbye!"<<std::endl;
	inputPipe.closeWrite();
	parserThread.join();
	
	/* If there is an active turtle, destroy it: */
	Lambda::destroyTurtle();
	
	/* Destroy the file loader and evaluation context: */
	delete fileLoader;
	delete context;
	
	#if LAMBDA_CONFIG_INSTRUMENT
	
	/* Print the evaluation counters: */
	Lambda::Thing::printCounters(std::cout);
	std::cout<<std::endl;
	
	#endif
	
	return 0;
	}
