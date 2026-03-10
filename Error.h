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

#ifndef ERROR_INCLUDED
#define ERROR_INCLUDED

#include <string>
#include <stdexcept>

/* Forward declarations: */
namespace Lambda {
class Thing;
}

namespace Lambda {

class Error:public std::runtime_error
	{
	/* Constructors and destructors: */
	public:
	Error(const char* msg) // Creates an error from an error message
		:std::runtime_error(msg)
		{
		}
	Error(const std::string& msg) // Ditto, using a C++ string
		:std::runtime_error(msg)
		{
		}
	};

class IsNotAError:public Error // Class for type conversion errors
	{
	/* Constructors and destructors: */
	public:
	IsNotAError(const Thing& thing,const char* notAWhat); // Indicates that the given thing is not of the type indicated by the notAWhat string in the form of "a Foo"
	};

/*********************
Convenience functions:
*********************/

std::string printThing(const Thing& thing); // Prints a string to a C++ string
Error makeError(const char* fmtString,...); // Returns an error exception with an error message created by a printf format string and its arguments

}

#endif
