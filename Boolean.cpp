/***********************************************************************
Boolean - Class representing boolean values.
Copyright (c) 2017-2026 Oliver Kreylos
***********************************************************************/

#include "Boolean.h"

#include <iostream>

#include "Error.h"

namespace Lambda {

/*********************
Methods of class Boolean:
*********************/

const char* Boolean::classIsA(void)
	{
	return "a Boolean";
	}

std::ostream& Boolean::print(std::ostream& os) const
	{
	/* Print this thing's boolean value: */
	os<<(value?"#t":"#f");
	
	return os;
	}

/********************
Methods of class Not:
********************/

std::ostream& Not::print(std::ostream& os) const
	{
	os<<"(BuiltinNot (bool) |-> !bool";
	
	return os;
	}

ThingPtr Not::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Return a new Boolean with the inverted value of result of evaluating the first argument: */
	return new Boolean(!Boolean::getValue(*evalArg(0,arguments,context)));
	}

/********************
Methods of class And:
********************/

std::ostream& And::print(std::ostream& os) const
	{
	os<<"(BuiltinAnd (bool1 bool2) |-> bool1&&bool2";
	
	return os;
	}

ThingPtr And::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(2,arguments);
	
	/* Return a new Boolean with the conjunction of the result of evaluating the two arguments: */
	return new Boolean(Boolean::getValue(*evalArg(0,arguments,context))&&Boolean::getValue(*evalArg(1,arguments,context)));
	}

/*******************
Methods of class Or:
*******************/

std::ostream& Or::print(std::ostream& os) const
	{
	os<<"(BuiltinOr (bool1 bool2) |-> bool1||bool2";
	
	return os;
	}

ThingPtr Or::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(2,arguments);
	
	/* Return a new Boolean with the disjunction of the result of evaluating the two arguments: */
	return new Boolean(Boolean::getValue(*evalArg(0,arguments,context))||Boolean::getValue(*evalArg(1,arguments,context)));
	}

}
