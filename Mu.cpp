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

#include "Mu.h"

#include <iostream>

#include "Error.h"
#include "Context.h"
#include "Void.h"
#include "Name.h"
#include "Cons.h"

namespace Lambda {

/*******************
Methods of class Mu:
*******************/

Mu::Mu(ThingPtr arguments)
	{
	/* Check that the arguments thing is a non-empty proper list: */
	Cons* cons=toPtr<Cons>(*arguments);
	if(cons==0||!cons->isList())
		throw IsNotAError(*arguments,"a non-empty proper list");
	
	/* Retrieve the argument name: */
	argumentName=Name::getName(cons->car());
	
	/* Process all function body expressions: */
	while((cons=dynamic_cast<Cons*>(&cons->cdr()))!=0)
		body.push_back(&cons->car());
	}

const char* Mu::classIsA(void)
	{
	return "a Mu";
	}

std::ostream& Mu::print(std::ostream& os) const
	{
	/* Print the mu identifier and the argument name: */
	os<<"(mu "<<argumentName.c_str()<<") |->";
	
	/* Print the list of body expressions: */
	for(std::vector<ThingPtr>::const_iterator bIt=body.begin();bIt!=body.end();++bIt)
		{
		os<<' ';
		(*bIt)->print(os);
		}
	
	return os;
	}

ThingPtr Mu::evaluate(ThingPtr arguments,Context& context)
	{
	/* Open a stack frame in the given context and define the argument name to the unevaluated arguments thing: */
	Context::Frame frame(context);
	context.setThing(argumentName,*arguments);
	
	/* Evaluate the body expressions: */
	ThingPtr result=Void::get();
	for(std::vector<ThingPtr>::iterator bIt=body.begin();bIt!=body.end();++bIt)
		result=(*bIt)->evaluate(context);
	
	/* Return the final expression's result: */
	return result;
	}

}
