/***********************************************************************
Function - Base class for functions.
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

#include "Function.h"

#include "Error.h"
#include "Null.h"
#include "Cons.h"

namespace Lambda {

/*************************
Methods of class Function:
*************************/

size_t Function::getArity(ThingPtr arguments)
	{
	/* Count the number of arguments in the list: */
	Thing* thing=arguments.getPointer();
	size_t result=0;
	while(true)
		{
		/* Check that the current thing is a Cons: */
		Cons* cons=dynamic_cast<Cons*>(thing);
		if(cons==0)
			break;
		
		/* Go to the thing's cdr, and count an argument: */
		thing=&cons->cdr();
		++result;
		}
	
	/* Check that the remaining thing is a Null: */
	if(dynamic_cast<Null*>(thing)==0)
		throw IsNotAError(*arguments,"a function argument list");
	
	return result;
	}

void Function::checkArity(size_t arity,ThingPtr arguments)
	{
	/* Count off the list arguments: */
	Thing* thing=arguments.getPointer();
	while(arity>0)
		{
		/* Check that the current thing is a Cons: */
		Cons* cons=dynamic_cast<Cons*>(thing);
		if(cons==0)
			break;
		
		/* Go to the thing's cdr, and check off an argument: */
		thing=&cons->cdr();
		--arity;
		}
	
	/* Check that all arguments have been checked off, and that the remaining thing is a Null: */
	if(arity>0)
		throw Error("Not enough function arguments");
	else if(dynamic_cast<Null*>(thing)==0)
		{
		if(dynamic_cast<Cons*>(thing)!=0)
			throw Error("Too many function arguments");
		else
			throw IsNotAError(*arguments,"a function argument list");
		}
	}

Thing& Function::getArg(size_t n,ThingPtr arguments)
	{
	/* Count to the requested argument: */
	Cons* cons=static_cast<Cons*>(arguments.getPointer());
	while(n>0)
		{
		cons=static_cast<Cons*>(&cons->cdr());
		--n;
		}
	
	/* Return the argument: */
	return cons->car();
	}

ThingPtr Function::evalArg(size_t n,ThingPtr arguments,Context& context)
	{
	/* Count to the requested argument: */
	Cons* cons=static_cast<Cons*>(arguments.getPointer());
	while(n>0)
		{
		cons=static_cast<Cons*>(&cons->cdr());
		--n;
		}
	
	/* Evaluate the argument and return the result: */
	return cons->car().evaluate(context);
	}

std::vector<Thing*> Function::getArgs(ThingPtr arguments)
	{
	/* Collect all arguments in the list: */
	Thing* thing=arguments.getPointer();
	std::vector<Thing*> result;
	while(true)
		{
		/* Check that the current thing is a Cons: */
		Cons* cons=dynamic_cast<Cons*>(thing);
		if(cons==0)
			break;
		
		/* Retrieve an argument, and go to the thing's cdr: */
		result.push_back(&cons->car());
		thing=&cons->cdr();
		}
	
	/* Check that the remaining thing is a Null: */
	if(dynamic_cast<Null*>(thing)==0)
		throw IsNotAError(*arguments,"a function argument list");
	
	return result;
	}

std::vector<ThingPtr> Function::evalArgs(ThingPtr arguments,Context& context)
	{
	/* Collect the results of evaluating all arguments in the list: */
	Thing* thing=arguments.getPointer();
	std::vector<ThingPtr> result;
	while(true)
		{
		/* Check that the current thing is a Cons: */
		Cons* cons=dynamic_cast<Cons*>(thing);
		if(cons==0)
			break;
		
		/* Retrieve and evaluate an argument, and go to the thing's cdr: */
		result.push_back(cons->car().evaluate(context));
		thing=&cons->cdr();
		}
	
	/* Check that the remaining thing is a Null: */
	if(dynamic_cast<Null*>(thing)==0)
		throw IsNotAError(*arguments,"a function argument list");
	
	return result;
	}

const char* Function::classIsA(void)
	{
	return "a Function";
	}

}
