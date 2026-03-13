/***********************************************************************
Def - Class for a function that defines a mapping from a name to a thing
in the current stack frame.
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

#include "Def.h"

#include <iostream>

#include "Error.h"
#include "Context.h"
#include "Void.h"
#include "Cons.h"
#include "Name.h"
#include "Lambda.h"

namespace Lambda {

/********************
Methods of class Def:
********************/

std::ostream& Def::print(std::ostream& os) const
	{
	os<<"(Builtin::Def name thing) |->"<<std::endl;
	
	return os;
	}

ThingPtr Def::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	size_t arity=getArity(arguments);
	if(arity==0)
		throw IsNotAError(*arguments,"a valid definition");
	
	/* Check whether it's a straight definition, or a function definition: */
	Cons* args=toKnownPtr<Cons>(*arguments);
	Thing& arg0=args->car();
	Name* name;
	Cons* cons;
	if(arity==2&&(name=toPtr<Name>(arg0))!=0)
		{
		/* Map the result of evaluating the thing to the name: */
		ThingPtr thing=evalArg(1,arguments,context);
		context.setThing(name->getName(),*thing);
		
		/* Check if the thing is a lambda expression: */
		Lambda* lambda=toPtr<Lambda>(*thing);
		if(lambda!=0)
			{
			/* Define the lambda expression's own name to allow recursion: */
			lambda->setName(name->getName());
			}
		}
	else if(arity>=1&&(cons=toPtr<Cons>(arg0))!=0)
		{
		/* Check the lambda definition argument list: */
		size_t lambdaArity=getArity(cons);
		if(lambdaArity==0)
			throw IsNotAError(*arguments,"a valid function declaration");
		
		/* Get the function name: */
		const String& name=Name::getName(cons->car());
		
		/* Get the function argument name list: */
		Thing* funcArgNames=&cons->cdr();
		
		/* Get the function body expressions: */
		Thing* funcBody=&args->cdr();
		
		/* Remove the current definition of the name: */
		context.removeThing(name);
		
		/* Map a new lambda expression to the name: */
		Misc::Autopointer<Lambda> lambda=new Lambda(new Cons(*funcArgNames,*funcBody),context);
		context.setThing(name,*lambda);
		
		/* Define the lambda expression's own name to allow recursion: */
		lambda->setName(name);
		}
	else
		throw IsNotAError(*arguments,"a valid definition");
	
	/* Return nothing: */
	return Void::get();
	}

}
