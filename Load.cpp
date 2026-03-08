/***********************************************************************
Load - Class for a function that loads a file and evaluates all
expressions in it.
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

#include "Load.h"

#include <iostream>
#include <fstream>

#include "Name.h"
#include "Parser.h"

namespace Lambda {

/*********************
Methods of class Load:
*********************/

std::ostream& Load::print(std::ostream& os) const
	{
	os<<"(BuiltinLoad filename) |->";
	
	return os;
	}

ThingPtr Load::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Get the first argument: */
	std::string fileName(Name::getName(getArg(0,arguments)).c_str());
	
	/* Check if the file name has an extension: */
	std::string::iterator extIt=fileName.end();
	for(std::string::iterator fnIt=fileName.begin();fnIt!=fileName.end();++fnIt)
		{
		if(*fnIt=='.')
			extIt=fnIt;
		else if(*fnIt=='/')
			extIt=fileName.end();
		}
	if(extIt==fileName.end())
		{
		/* Append the .lambda extension to the file name: */
		fileName.append(".lambda");
		}
	
	/* Open the input file and wrap in InputStream around it: */
	std::ifstream file(fileName.c_str());
	InputStream inputStream(file);
	
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
	
	/* We don't return anything: */
	return 0;
	}

}
