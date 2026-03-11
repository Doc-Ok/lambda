/***********************************************************************
Undef - Class for a function that removes a mapping from a name to a
thing in the current stack frame.
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

#include "Undef.h"

#include <iostream>

#include "Context.h"
#include "Void.h"
#include "Name.h"

namespace Lambda {

/**********************
Methods of class Undef:
**********************/

std::ostream& Undef::print(std::ostream& os) const
	{
	os<<"(Builtin::Undef name) |->"<<std::endl;
	
	return os;
	}

ThingPtr Undef::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Get the first argument's name: */
	const String& name=Name::getName(getArg(0,arguments));
	
	/* Remove the name's mapping from the context: */
	context.removeThing(name);
	
	/* Return nothing: */
	return Void::get();
	}

}
