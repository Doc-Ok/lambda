/***********************************************************************
Null - Class representing the empty list, or null thing.
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

#include "Null.h"

#include <iostream>

#include "Boolean.h"

namespace Lambda {

/*********************
Methods of class Null:
*********************/

const char* Null::classIsA(void)
	{
	return "a Null";
	}

std::ostream& Null::print(std::ostream& os) const
	{
	/* Print the empty list token: */
	os<<"()";
	
	return os;
	}

/**********************
Methods of class NullP:
**********************/

std::ostream& NullP::print(std::ostream& os) const
	{
	os<<"(BuiltinNull? thing) |-> Boolean";
	
	return os;
	}

ThingPtr NullP::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Return a new Boolean with true if the argument evaluates to a Null: */
	return new Boolean(is<Null>(*evalArg(0,arguments,context)));
	}

}
