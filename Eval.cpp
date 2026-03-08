/***********************************************************************
Eval - Class for a function that evaluates and returns its arguments.
Thus, it is the inverse of Quote.
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

#include "Eval.h"

#include <iostream>

namespace Lambda {

/*********************
Methods of class Eval:
*********************/

std::ostream& Eval::print(std::ostream& os) const
	{
	os<<"(BuiltinEval expr) |-> (eval expr)";
	
	return os;
	}

ThingPtr Eval::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Evaluate the argument twice: */
	return evalArg(0,arguments,context)->evaluate(context);
	}

}
