/***********************************************************************
DefBuiltIns - Function to define the built-in functions in an evaluation
context.
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

#include "DefBuiltins.h"

#include <iostream>

#include "Context.h"
#include "Function.h"
#include "Null.h"
#include "Cons.h"
#include "Quote.h"
#include "Eval.h"
#include "Begin.h"
#include "Boolean.h"
#include "If.h"
#include "Def.h"
#include "Load.h"
#include "Lambda.h"
#include "Integer.h"

namespace Lambda {

namespace Builtin {

/*******************
Functions with Null:
*******************/

class NullP:public Function // Predicate function returning true if the argument is a Null
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Null? expr): '() |-> #t, expr |-> #f";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return a new Boolean with true if the first argument evaluates to a Null, false otherwise: */
		return new Boolean(is<Null>(*evalArg(0,arguments,context)));
		}
	};

/*******************
Functions with Cons:
*******************/

class Cons:public Function // Class to make a Cons from a car and a cdr
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Cons car cdr) |-> (car . cdr)";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(2,arguments);
		
		/* Create a new Cons from the result of evaluating the two arguments: */
		return new ::Lambda::Cons(*evalArg(0,arguments,context),*evalArg(1,arguments,context));
		}
	};

class Car:public Function // Class to return the car of a Cons
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Car (car . cdr)) |-> car";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return the car of the result of evaluating the first argument: */
		return &to<::Lambda::Cons>(*evalArg(0,arguments,context)).car();
		}
	};

class Cdr:public Function // Class to return the cdr of a Cons
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Cdr (car . cdr)) |-> cdr";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return the cdr of the result of evaluating the first argument: */
		return &to<::Lambda::Cons>(*evalArg(0,arguments,context)).cdr();
		}
	};

/********************
Functions with Quote:
********************/

class Quote:public Function // Class to make a Quote from an expression
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Quote expr) |-> 'expr";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Do not evaluate the argument, just return it: */
		return &getArg(0,arguments);
		}
	};

/**********************
Functions with Boolean:
**********************/

class Not:public Function // Logical not
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Not expr): #t |-> #f, #f |-> #t";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return a new Boolean with the logical not of the result of evaluating the first argument: */
		return new Boolean(!Boolean::getValue(*evalArg(0,arguments,context)));
		}
	};

class And:public Function // Logical and
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::And expr1 expr2): #t #t |-> #t, expr1 expr2 |-> #f";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(2,arguments);
		
		/* Return a new Boolean with the logical and of the result of evaluating the first two arguments: */
		return new Boolean(Boolean::getValue(*evalArg(0,arguments,context))&&Boolean::getValue(*evalArg(1,arguments,context)));
		}
	};

class Or:public Function // Logical or
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Or expr1 expr2): #f #f |-> #f, expr1 expr2 |-> #t";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(2,arguments);
		
		/* Return a new Boolean with the logical or of the result of evaluating the first two arguments: */
		return new Boolean(Boolean::getValue(*evalArg(0,arguments,context))||Boolean::getValue(*evalArg(1,arguments,context)));
		}
	};

/*********************
Functions with Lambda:
*********************/

class Lambda:public Function // Class to make a Lambda from an argument list
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Lambda (arg1 ... argn) body1 ... bodyn) |-> lambda";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Create a new Lambda from the argument list: */
		return new ::Lambda::Lambda(arguments,context);
		}
	};

/**********************
Functions with Integer:
**********************/

class IntegerAdd:public Function // Class to add any number of Integers
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Add expr1 ... exprn) |-> expr1 + ... + exprn";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Evaluate all arguments: */
		std::vector<ThingPtr> values=evalArgs(arguments,context);
		
		/* Sum up all arguments: */
		long result=0;
		for(std::vector<ThingPtr>::iterator vIt=values.begin();vIt!=values.end();++vIt)
			result+=Integer::getValue(**vIt);
		
		return new Integer(result);
		}
	};

class IntegerSub:public Function // Class to subtract any number of Integers
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Sub expr1 ... exprn) |-> expr1 - ... - exprn";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Evaluate all arguments: */
		std::vector<ThingPtr> values=evalArgs(arguments,context);
		
		if(values.size()==1)
			{
			/* Negate the first argument: */
			return new Integer(-Integer::getValue(*values.front()));
			}
		else
			{
			/* Subtract all following arguments from the first argument: */
			std::vector<ThingPtr>::iterator vIt=values.begin();
			long result=Integer::getValue(**vIt);
			for(++vIt;vIt!=values.end();++vIt)
				result-=Integer::getValue(**vIt);
			
			return new Integer(result);
			}
		}
	};

class IntegerMult:public Function // Class to multiply any number of Integers
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Mult expr1 ... exprn) |-> expr1 * ... * exprn";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Evaluate all arguments: */
		std::vector<ThingPtr> values=evalArgs(arguments,context);
		
		/* Multiply up all arguments: */
		long result=1;
		for(std::vector<ThingPtr>::iterator vIt=values.begin();vIt!=values.end();++vIt)
			result*=Integer::getValue(**vIt);
		
		return new Integer(result);
		}
	};

}

void defBuiltins(Context& context)
	{
	context.setThing("null?",*new Builtin::NullP);
	
	context.setThing("cons",*new Builtin::Cons);
	context.setThing("car",*new Builtin::Car);
	context.setThing("cdr",*new Builtin::Cdr);
	
	context.setThing("quote",*new Builtin::Quote);
	context.setThing("eval",*new Eval);
	context.setThing("begin",*new Begin);
	
	context.setThing("#f",*new Boolean(false));
	context.setThing("#t",*new Boolean(true));
	context.setThing("not",*new Builtin::Not);
	context.setThing("and",*new Builtin::And);
	context.setThing("or",*new Builtin::Or);
	
	context.setThing("if",*new If);
	context.setThing("def",*new Def);
	context.setThing("load",*new Load);
	
	context.setThing("lambda",*new Builtin::Lambda);
	
	context.setThing("+",*new Builtin::IntegerAdd);
	context.setThing("-",*new Builtin::IntegerSub);
	context.setThing("*",*new Builtin::IntegerMult);
	}

}
