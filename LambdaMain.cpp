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

#include <ctype.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <Misc/StdError.h>

#include "Context.h"
#include "Null.h"
#include "Boolean.h"
#include "Cons.h"
#include "If.h"
#include "Def.h"
#include "Lambda.h"
#include "Boolean.h"
#include "Parser.h"
#include "Load.h"

int main(int argc,char* argv[])
	{
	/* Create an evaluation context: */
	Lambda::Context context;
	context.setThing("null",*new Lambda::Null);
	context.setThing("null?",*new Lambda::NullP);
	context.setThing("#f",*new Lambda::Boolean(false));
	context.setThing("#t",*new Lambda::Boolean(true));
	context.setThing("cons",*new Lambda::MakeCons);
	context.setThing("car",*new Lambda::Car);
	context.setThing("cdr",*new Lambda::Cdr);
	context.setThing("if",*new Lambda::If);
	context.setThing("def",*new Lambda::Def);
	context.setThing("lambda",*new Lambda::Lambda);
	context.setThing("not",*new Lambda::Not);
	context.setThing("and",*new Lambda::And);
	context.setThing("or",*new Lambda::Or);
	context.setThing("load",*new Lambda::Load);
	
	/* Read, evaluate, and print expressions until the user closes the connection: */
	std::cout<<"Welcome to the Lambda Programming Language!"<<std::endl;
	std::cout<<"Quit by pressing Ctrl+D at the prompt."<<std::endl<<std::endl;
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
			
			/* Print the result if it is not empty, otherwise just confirm that an expression has been parsed: */
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
