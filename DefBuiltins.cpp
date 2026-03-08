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
#include "FloatingPoint.h"

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

/*******************************************
Functions with Integer and/or FloatingPoint:
*******************************************/

class Add:public Function // Class to add any number of Integers and/or FloatingPoints
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
		double floatingPoint=0.0;
		long integer=0;
		bool maybeInteger=true;
		for(std::vector<ThingPtr>::iterator vIt=values.begin();vIt!=values.end();++vIt)
			{
			if(maybeInteger)
				{
				Integer* i=toPtr<Integer>(**vIt);
				if(i!=0)
					{
					integer+=i->getValue();
					floatingPoint+=double(i->getValue());
					}
				else
					maybeInteger=false;
				}
			
			if(!maybeInteger)
				floatingPoint+=FloatingPoint::getValue(**vIt);
			}
		
		if(maybeInteger)
			return new Integer(integer);
		else
			return new FloatingPoint(floatingPoint);
		}
	};

class Sub:public Function // Class to subtract any number of Integers and/or FloatingPoints
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
			Integer* i=toPtr<Integer>(*values.front());
			if(i!=0)
				return new Integer(-i->getValue());
			else
				return new FloatingPoint(-FloatingPoint::getValue(*values.front()));
			}
		else
			{
			/* Subtract all following arguments from the first argument: */
			std::vector<ThingPtr>::iterator vIt=values.begin();
			double floatingPoint;
			long integer;
			bool maybeInteger=true;
			Integer* i=toPtr<Integer>(**vIt);
			if(i!=0)
				{
				integer=i->getValue();
				floatingPoint=double(i->getValue());
				}
			else
				{
				floatingPoint=FloatingPoint::getValue(**vIt);
				maybeInteger=false;
				}
			for(++vIt;vIt!=values.end();++vIt)
				{
				if(maybeInteger)
					{
					Integer* i=toPtr<Integer>(**vIt);
					if(i!=0)
						{
						integer-=i->getValue();
						floatingPoint-=double(i->getValue());
						}
					else
						maybeInteger=false;
					}
				
				if(!maybeInteger)
					floatingPoint-=FloatingPoint::getValue(**vIt);
				}
			
			if(maybeInteger)
				return new Integer(integer);
			else
				return new FloatingPoint(floatingPoint);
			}
		}
	};

class Mult:public Function // Class to multiply any number of Integers and/or FloatingPoints
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
		double floatingPoint=1.0;
		long integer=1;
		bool maybeInteger=true;
		for(std::vector<ThingPtr>::iterator vIt=values.begin();vIt!=values.end();++vIt)
			{
			if(maybeInteger)
				{
				Integer* i=toPtr<Integer>(**vIt);
				if(i!=0)
					{
					integer*=i->getValue();
					floatingPoint*=double(i->getValue());
					}
				else
					maybeInteger=false;
				}
			
			if(!maybeInteger)
				floatingPoint*=FloatingPoint::getValue(**vIt);
			}
		
		if(maybeInteger)
			return new Integer(integer);
		else
			return new FloatingPoint(floatingPoint);
		}
	};

class Div:public Function // Class to divide any number of Integers and/or FloatingPoints
	{
	/* Methods from class Thing: */
	public:
	std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Div expr1 ... exprn) |-> expr1 / ... / exprn";
		
		return os;
		}
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Evaluate all arguments: */
		std::vector<ThingPtr> values=evalArgs(arguments,context);
		
		if(values.size()==1)
			{
			/* Invert the first argument: */
			Integer* i=toPtr<Integer>(*values.front());
			if(i!=0)
				{
				if(i->getValue()==0)
					throw Error("Division by zero");
				else if(i->getValue()==1)
					return values.front();
				else
					return new FloatingPoint(1.0/double(i->getValue()));
				}
			else
				{
				double value=FloatingPoint::getValue(*values.front());
				if(value==0.0)
					throw Error("Division by zero");
				else
					return new FloatingPoint(1.0/value);
				}
			}
		else
			{
			/* Divide the first argument by all subsequent arguments: */
			std::vector<ThingPtr>::iterator vIt=values.begin();
			double floatingPoint;
			long integer;
			bool maybeInteger=true;
			Integer* i=toPtr<Integer>(**vIt);
			if(i!=0)
				{
				integer=i->getValue();
				floatingPoint=double(i->getValue());
				}
			else
				{
				floatingPoint=FloatingPoint::getValue(**vIt);
				maybeInteger=false;
				}
			for(++vIt;vIt!=values.end();++vIt)
				{
				if(maybeInteger)
					{
					Integer* i=toPtr<Integer>(**vIt);
					if(i!=0)
						{
						if(i->getValue()==0)
							throw Error("Division by zero");
						else if(integer%i->getValue()==0)
							integer/=i->getValue();
						else
							{
							floatingPoint=double(integer)/double(i->getValue());
							maybeInteger=false;
							}
						}
					else
						{
						floatingPoint=double(integer)/FloatingPoint::getValue(**vIt);
						maybeInteger=false;
						}
					}
				else
					{
					double value=FloatingPoint::getValue(**vIt);
					if(value==0.0)
						throw Error("Division by zero");
					floatingPoint/=value;
					}
				}
			
			if(maybeInteger)
				return new Integer(integer);
			else
				return new FloatingPoint(floatingPoint);
			}
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
	
	context.setThing("+",*new Builtin::Add);
	context.setThing("-",*new Builtin::Sub);
	context.setThing("*",*new Builtin::Mult);
	context.setThing("/",*new Builtin::Div);
	}

}
