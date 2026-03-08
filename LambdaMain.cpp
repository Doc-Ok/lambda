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

#include <stdexcept>
#include <iostream>

#include "Context.h"
#include "DefBuiltins.h"
#include "Parser.h"

int main(void)
	{
	/* Create an evaluation context: */
	Lambda::Context context;
	
	/* Define the Lambda Programming Language's built-in primitives and functions: */
	defBuiltins(context);
	
	/* Read, evaluate, and print expressions until the user closes the connection: */
	std::cout<<"Welcome to the Lambda Programming Language!"<<std::endl;
	std::cout<<"Press Ctrl+D at the prompt to say goodbye."<<std::endl<<std::endl;
	std::cout<<"Lambda> "<<std::flush;
	Lambda::InputStream inputStream(std::cin);
	while(true)
		{
		try
			{
			/* Parse an expression from the input stream and bail out if the expression is null at end-of-file: */
			inputStream.skipWs();
			Lambda::ThingPtr expression=Lambda::parse(inputStream);
			if(expression==0)
				break;
			
			/* Evaluate the expression: */
			Lambda::ThingPtr result=expression->evaluate(context);
			
			/* Print the result if it is not empty: */
			if(result!=0)
				{
				std::cout<<"= ";
				result->print(std::cout);
				std::cout<<std::endl;
				}
			}
		catch(const std::runtime_error& err)
			{
			std::cout<<"Oopsie: "<<err.what()<<std::endl;
			}
		
		std::cout<<"Lambda> "<<std::flush;
		}
	
	std::cout<<"Goodbye!"<<std::endl;
	
	return 0;
	}
