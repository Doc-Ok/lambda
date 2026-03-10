/***********************************************************************
Begin - Class for a function that evaluates a sequence of arguments and
returns the result of evaluating its last argument.
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

#include "Begin.h"

#include <iostream>

#include "Void.h"
#include "Cons.h"

namespace Lambda {

/**********************
Methods of class Begin:
**********************/

std::ostream& Begin::print(std::ostream& os) const
	{
	os<<"(BuiltinBegin expr1 ... exprn) |-> (eval exprn)";
	
	return os;
	}

ThingPtr Begin::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	size_t arity=getArity(arguments);
	
	/* Evaluate all arguments in order: */
	ThingPtr result=&Void::the;
	Thing* argPtr=arguments.getPointer();
	while(arity>0)
		{
		/* Evaluate the argument: */
		Cons* cons=toKnownPtr<Cons>(*argPtr);
		result=cons->car().evaluate(context);
		
		/* Go to the next argument: */
		argPtr=&cons->cdr();
		--arity;
		}
	
	return result;
	}

}
