/***********************************************************************
Lambda - Class for a function that creates lambda expressions.
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

#ifndef LAMBDA_INCLUDED
#define LAMBDA_INCLUDED

#include <string>
#include <vector>
#include <Misc/SelfDestructPointer.h>

#include "Function.h"

namespace Lambda {

class Lambda:public Function // Class representing lambda expressions
	{
	/* Elements: */
	private:
	Misc::SelfDestructPointer<Context> closure; // The lambda expression's closure evaluation context
	std::vector<std::string> argumentNames; // The names of the lambda expression's arguments
	std::vector<ThingPtr> body; // The list of expressions comprising the lambda expression's body
	std::string name; // The name that was given to this lambda expression by a def or let, or the empty string
	
	/* Constructors and destructors: */
	public:
	Lambda(ThingPtr arguments,Context& sClosure); // Constructs a lambda expression from the given thing, which needs to be a list consisting of a list of argument names followed by one or more body expressions, inside the given context
	
	/* Methods from class Thing: */
	static const char* classIsA(void);
	std::string isA(void) const
		{
		return classIsA();
		}
	virtual std::ostream& print(std::ostream& os) const;
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context);
	
	/* New methods: */
	void setName(const std::string& newName); // Gives the lambda expression a name
	};

}

#endif
