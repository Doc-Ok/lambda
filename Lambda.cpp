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

#include "Lambda.h"

#include <iostream>

#include "Error.h"
#include "Context.h"
#include "Null.h"
#include "Cons.h"
#include "Name.h"

namespace Lambda {

/***********************************
Methods of class Lambda::Expression:
***********************************/

Lambda::Expression::Expression(ThingPtr arguments,Context& sClosure)
	:closure(sClosure.clone())
	{
	/* Check that the arguments thing is a non-empty proper list: */
	Cons* cons=toPtr<Cons>(*arguments);
	if(cons==0||!cons->isList())
		throw IsNotAError(*arguments,"a non-empty proper list");
	
	/* Check if there are argument names: */
	if(!is<Null>(cons->car()))
		{
		/* Check that the list of argument names is a proper list: */
		Cons* argCons=toPtr<Cons>(cons->car());
		if(argCons==0||!argCons->isList())
			throw IsNotAError(cons->car(),"a list of argument names");
		
		/* Process all arguments names: */
		do
			{
			/* Store the argument name: */
			argumentNames.push_back(Name::getName(argCons->car()));
			
			/* Go to the next argument: */
			argCons=toPtr<Cons>(argCons->cdr());
			}
		while(argCons!=0);
		}
	
	/* Process all function body expressions: */
	while((cons=dynamic_cast<Cons*>(&cons->cdr()))!=0)
		body.push_back(&cons->car());
	}

const char* Lambda::Expression::classIsA(void)
	{
	return "a Lambda Expression";
	}

std::ostream& Lambda::Expression::print(std::ostream& os) const
	{
	os<<"(lambda";
	
	/* Print the list of argument names: */
	for(std::vector<std::string>::const_iterator aIt=argumentNames.begin();aIt!=argumentNames.end();++aIt)
		os<<' '<<*aIt;
	os<<") |->";
	
	/* Print the list of body expressions: */
	for(std::vector<ThingPtr>::const_iterator bIt=body.begin();bIt!=body.end();++bIt)
		{
		os<<' ';
		(*bIt)->print(os);
		}
	
	return os;
	}

ThingPtr Lambda::Expression::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(argumentNames.size(),arguments);
	
	/* Evaluate all arguments: */
	std::vector<ThingPtr> argValues;
	Cons* cons=toKnownPtr<Cons>(*arguments);
	for(size_t i=0;i<argumentNames.size();++i,cons=toKnownPtr<Cons>(cons->cdr()))
		argValues.push_back(cons->car().evaluate(context));
	
	/* Open a stack frame in the closure and define all argument names to the evaluated arguments: */
	Context::Frame frame(*closure);
	for(size_t i=0;i<argumentNames.size();++i)
		closure->setThing(argumentNames[i],*argValues[i]);
	
	/* If we have a name, we need to define it in the stack frame for recursion to work: */
	if(!name.empty())
		closure->setThing(name,*this);
	
	/* Evaluate the body expressions: */
	ThingPtr result;
	for(std::vector<ThingPtr>::iterator bIt=body.begin();bIt!=body.end();++bIt)
		result=(*bIt)->evaluate(*closure);
	
	/* Return the final expression's result: */
	return result;
	}

void Lambda::Expression::setName(const std::string& newName)
	{
	/* Only take the given name if no name has been assigned yet: */
	if(name.empty())
		name=newName;
	}

/***********************
Methods of class Lambda:
***********************/

std::ostream& Lambda::print(std::ostream& os) const
	{
	os<<"(BuiltinLambda argList bodyList) |-> (lambda argList) |-> (bodyList)"<<std::endl;
	return os;
	}

ThingPtr Lambda::evaluate(ThingPtr arguments,Context& context)
	{
	/* Create a new lambda expression from the given argument: */
	return new Expression(arguments,context);
	}

}
