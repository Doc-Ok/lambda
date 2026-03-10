/***********************************************************************
Mu - Class representing mu expressions. A mu expression is a non-closure
variadic lambda expression.
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

#ifndef MU_INCLUDED
#define MU_INCLUDED

#include <vector>

#include "String.h"
#include "Function.h"

namespace Lambda {

class Mu:public Function
	{
	/* Elements: */
	String argumentName; // The name of the mu expression's single argument, which is the unevaluated cdr of the list in which the mu expression was evaluated
	std::vector<ThingPtr> body; // The list of expressions comprising the mu expression's body
	
	/* Constructors and destructors: */
	public:
	Mu(ThingPtr arguments); // Constructs a mu expression from the given thing, which needs to be a list consisting of a single argument name followed by one or more body expressions
	
	/* Methods from class Thing: */
	static const char* classIsA(void);
	std::string isA(void) const
		{
		return classIsA();
		}
	virtual std::ostream& print(std::ostream& os) const;
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context);
	};

}

#endif
