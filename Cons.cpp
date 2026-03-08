/***********************************************************************
Cons - Class representing an interior node in a binary tree, primarily
used to construct lists.
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

#include "Cons.h"

#include <iostream>

#include "Null.h"
#include "Function.h"

namespace Lambda {

/*********************
Methods of class Cons:
*********************/

const char* Cons::classIsA(void)
	{
	return "a Cons";
	}

ThingPtr Cons::evaluate(Context& context)
	{
	/* Evaluate the car and cast the result to a Function: */
	FunctionPtr function=toPtr<Function>(*children[0]->evaluate(context));
	
	/* Evaluate the function with the cdr as the argument: */
	return function->evaluate(children[1],context);
	}

std::ostream& Cons::print(std::ostream& os) const
	{
	os<<'(';
	
	/* Check if we are a proper list: */
	if(isList())
		{
		/* Print this Cons as a list: */
		const Cons* consPtr=this;
		
		/* Print the first list element: */
		consPtr->children[0]->print(os);
		
		/* Print the rest of the list elements: */
		while((consPtr=dynamic_cast<Cons*>(consPtr->children[1].getPointer()))!=0)
			{
			/* Print the next list element: */
			os<<' ';
			consPtr->children[0]->print(os);
			}
		}
	else
		{
		/* Print this Cons as a binary tree interior node: */
		children[0]->print(os);
		os<<" . ";
		children[1]->print(os);
		}
	
	os<<')';
	
	return os;
	}

bool Cons::isList(void) const
	{
	/* Walk along a chain of cdrs until we encounter something other than a Cons: */
	const Thing* cdr=this->children[1].getPointer();
	while(true)
		{
		/* Check if the cdr is a Cons: */
		const Cons* cdrCons=dynamic_cast<const Cons*>(cdr);
		if(cdrCons==0)
			break;
		
		/* Go to the next cdr: */
		cdr=cdrCons->children[1].getPointer();
		}
	
	/* It's a proper list if the non-Cons cdr we found is a Null: */
	return dynamic_cast<const Null*>(cdr)!=0;
	}

void Cons::checkList(void) const
	{
	/* Walk along a chain of cdrs until we encounter something other than a Cons: */
	const Thing* cdr=this->children[1].getPointer();
	while(true)
		{
		/* Check if the cdr is a Cons: */
		const Cons* cdrCons=dynamic_cast<const Cons*>(cdr);
		if(cdrCons==0)
			break;
		
		/* Go to the next cdr: */
		cdr=cdrCons->children[1].getPointer();
		}
	
	/* It's not proper list if the non-Cons cdr we found is not a Null: */
	if(dynamic_cast<const Null*>(cdr)==0)
		throw IsNotAError(*this,"a proper list");
	}

std::vector<ThingPtr> Cons::getList(void)
	{
	/* Add all list elements to a vector: */
	std::vector<ThingPtr> result;
	Cons* consPtr=this;
	do
		{
		result.push_back(consPtr->children[0]);
		consPtr=dynamic_cast<Cons*>(consPtr->children[1].getPointer());
		}
	while(consPtr!=0);
	
	return result;
	}

bool Cons::isList(const Thing& thing)
	{
	/* Check if the thing is a Null: */
	if(dynamic_cast<const Null*>(&thing)!=0)
		return true;
	
	/* Try casting the thing to a Cons: */
	const Cons* cons=dynamic_cast<const Cons*>(&thing);
	
	/* Return true if the thing is a Cons and a proper list: */
	return cons!=0&&cons->isList();
	}

void Cons::checkList(const Thing& thing)
	{
	/* Check if the thing is not a Null: */
	if(dynamic_cast<const Null*>(&thing)==0)
		{
		/* Try casting the thing to a Cons: */
		const Cons* cons=dynamic_cast<const Cons*>(&thing);
		
		/* Check if the thing is a Cons and a proper list: */
		if(cons==0||!cons->isList())
			throw IsNotAError(thing,"a proper list");
		}
	}

/*************************
Methods of class MakeCons:
*************************/

std::ostream& MakeCons::print(std::ostream& os) const
	{
	os<<"(BuiltinCons car cdr) |-> (car . cdr)";
	
	return os;
	}

ThingPtr MakeCons::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(2,arguments);
	
	/* Create a new Cons from the result of evaluating the two arguments: */
	return new Cons(*evalArg(0,arguments,context),*evalArg(1,arguments,context));
	}

/********************
Methods of class Car:
********************/

std::ostream& Car::print(std::ostream& os) const
	{
	os<<"(BuiltinCar (car . cdr)) |-> car";
	
	return os;
	}

ThingPtr Car::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Return the car of the result of evaluating the first argument: */
	return &to<Cons>(*evalArg(0,arguments,context)).car();
	}

/********************
Methods of class Cdr:
********************/

std::ostream& Cdr::print(std::ostream& os) const
	{
	os<<"(BuiltinCdr (car . cdr)) |-> cdr";
	
	return os;
	}

ThingPtr Cdr::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Return the cdr of the result of evaluating the first argument: */
	return &to<Cons>(*evalArg(0,arguments,context)).cdr();
	}

}
