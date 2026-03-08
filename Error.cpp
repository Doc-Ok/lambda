/***********************************************************************
Error - Exception class to signal Logo errors in a friendly(?) way.
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

#include "Error.h"

#include <stdarg.h>
#include <stdio.h>
#include <sstream>

#include "Thing.h"

namespace Lambda {

/****************************
Methods of class IsNotAError:
****************************/

namespace {

std::string makeIsNotAErrorMsg(const Thing& thing,const char* notAWhat)
	{
	/* Print the given thing into a string: */
	std::stringstream stros;
	stros<<'"';
	thing.print(stros);
	stros<<'"';
	
	/* Append the actual type and the wanted type: */
	stros<<" is "<<thing.isA()<<", not "<<notAWhat;
	
	/* Return the message string: */
	return std::move(stros.str());
	}

}

IsNotAError::IsNotAError(const Thing& thing,const char* notAWhat)
	:Error(makeIsNotAErrorMsg(thing,notAWhat))
	{
	}

/*********************
Convenience functions:
*********************/

Error makeError(const char* fmtString,...)
	{
	/* Create the error message in a fixed-size on-stack buffer: */
	va_list ap;
	va_start(ap,fmtString);
	char buffer[1024];
	vsnprintf(buffer,sizeof(buffer),fmtString,ap);
	va_end(ap);
	
	/* Create and return the exception object: */
	return Error(buffer);
	}

}
