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
#include <readline/readline.h>
#include <readline/history.h>
#include <stdexcept>
#include <iostream>
#include <Misc/Pipe.h>
#include <Threads/Thread.h>

#include "Context.h"
#include "DefBuiltins.h"
#include "InputStream.h"
#include "Parser.h"

namespace {

Lambda::Context* context=0; // The root expression evaluation context

/****************
Helper functions:
****************/

/* Inserts a thing into a std::ostream: */
std::ostream& operator<<(std::ostream& os,const Lambda::Thing& thing)
	{
	return thing.print(os);
	}

/* Runs a parser and evaluator in a background thread: */
void* parserThreadFunction(Lambda::InputStream* inputStream)
	{
	while(true)
		{
		try
			{
			/* Parse the next expression from the input stream: */
			inputStream->skipWs();
			Lambda::ThingPtr expression=Lambda::parse(*inputStream);
			
			/* Bail out if the expression is null at end-of-file: */
			if(expression==0)
				break;
			
			/* Evaluate the expression: */
			Lambda::ThingPtr result=expression->evaluate(*context);
			
			/* Send the result to the main thread if it is not empty: */
			if(result!=0)
				inputStream->signalResult(*result);
			}
		catch(const std::runtime_error& err)
			{
			/* Print an error message and continue: */
			std::cout<<"Oopsie: "<<err.what()<<std::endl;
			}
		}
	
	return 0;
	}

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

}

int main(void)
	{
	/* Create an evaluation context: */
	context=new Lambda::Context;
	
	/* Define the Lambda Programming Language's built-in primitives and functions: */
	defBuiltins(*context);
	
	/* Create a pipe to send input from stdin to the input stream: */
	Misc::Pipe inputPipe;
	
	/* Write a dummy character to the input pipe to prime the pump, so to speak: */
	inputPipe.write<char>(0);
	
	/* Attach an input stream to the read end of the input pipe: */
	Lambda::InputStream inputStream(inputPipe.getReadFd());
	
	/* Create a signal pipe to read results from the parser: */
	Misc::Pipe signalPipe;
	inputStream.setSignalFd(signalPipe.getWriteFd());
	
	/* Start a background thread to parse and evaluate expressions: */
	Threads::Thread parserThread(parserThreadFunction,&inputStream);
	
	/* Wait for the first blocking signal from the parser thread: */
	Lambda::Thing* result;
	signalPipe.read(result);
	
	/* Set up the readline library: */
	rl_readline_name="Lambda"; // Name of this application, for per-application readline configuration
	using_history();
	rl_attempted_completion_function=complete;
	
	/* Read, evaluate, and print expressions until the user closes the connection: */
	std::cout<<"Welcome to the Lambda Programming Language!"<<std::endl;
	std::cout<<"Press Ctrl+D at the prompt to say goodbye."<<std::endl<<std::endl;
	while(true)
		{
		/* Read a line of input from the readline library and check for end-of-file: */
		char* input=readline("Lambda> ");
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
		
		/* Now read all evaluation results from the parser thread until it blocks for more input: */
		while(true)
			{
			/* Read a result pointer and bail out if there are no more results: */
			Lambda::Thing* result;
			signalPipe.read(result);
			if(result==0)
				break;
			
			/* Print the result: */
			std::cout<<"= "<<*result<<std::endl;
			
			/* Drop the reference to the result that was held by the pipe message: */
			result->unref();
			}
		}
	
	/* Clean up after readline as much as possible: */
	rl_clear_history();
	
	/* Say goodbye and close the write end of the input pipe, then wait for the parser thread to shut down: */
	std::cout<<"Goodbye!"<<std::endl;
	inputPipe.closeWrite();
	parserThread.join();
	
	/* Destroy the evaluation context: */
	delete context;
	
	return 0;
	}
