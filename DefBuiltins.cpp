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

#include <math.h>
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
#include "Parser.h"

namespace Lambda {

namespace Builtin {

/*******************
Functions with Atom:
*******************/

class AtomP:public Function // Predicate function returning true if the argument is an Atom
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::AtomP expr): (a . b) |-> #f, expr |-> #t";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return a new Boolean with true if the first argument evaluates to an Atom, false otherwise: */
		return new Boolean(is<Atom>(*evalArg(0,arguments,context)));
		}
	};

/*******************
Functions with Null:
*******************/

class NullP:public Function // Predicate function returning true if the argument is a Null
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::NullP expr): '() |-> #t, expr |-> #f";
		
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

class ConsP:public Function // Predicate function returning true if the argument is a Cons
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::ConsP expr): (a . b) |-> #t, expr |-> #f";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return a new Boolean with true if the first argument evaluates to a Null, false otherwise: */
		return new Boolean(is<Cons>(*evalArg(0,arguments,context)));
		}
	};

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

class BooleanP:public Function // Predicate function returning true if the argument is a Boolean
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::BooleanP expr): #t |-> #t, #f |-> #t, expr |-> #f";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return a new Boolean with true if the first argument evaluates to a Boolean, false otherwise: */
		return new Boolean(is<Boolean>(*evalArg(0,arguments,context)));
		}
	};

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

/***********************
Functions with Function:
***********************/

class FunctionP:public Function // Predicate function returning true if the argument is a Function
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::FunctionP expr): (lambda (...) ...) |-> #t, expr |-> #f";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Return a new Boolean with true if the first argument evaluates to a Function, false otherwise: */
		return new Boolean(is<Function>(*evalArg(0,arguments,context)));
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

/********************************************************
Use a macro to create the numerical comparison functions:
********************************************************/

#define COMPFUNC(NAME,OPERATOR) \
class NAME:public Function \
	{ \
	/* Methods from class Thing: */ \
	public: \
	virtual std::ostream& print(std::ostream& os) const \
		{ \
		os<<"(Builtin::"<<#NAME<<" expr1 expr2) |-> expr1"<<#OPERATOR<<"expr2"; \
		 \
		return os; \
		} \
	 \
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context) \
		{ \
		/* Check the argument list: */ \
		checkArity(2,arguments); \
		 \
		/* Evaluate both arguments: */ \
		ThingPtr value0=evalArg(0,arguments,context); \
		ThingPtr value1=evalArg(1,arguments,context); \
		 \
		/* Check if both values are integer or floating-points: */ \
		Integer* i0=toPtr<Integer>(*value0); \
		Integer* i1=toPtr<Integer>(*value1); \
		FloatingPoint* f0=toPtr<FloatingPoint>(*value0); \
		FloatingPoint* f1=toPtr<FloatingPoint>(*value1); \
		if(i0!=0) \
			{ \
			if(i1!=0) \
				return new Boolean(i0->getValue() OPERATOR i1->getValue()); \
			else if(f1!=0) \
				return new Boolean(double(i0->getValue()) OPERATOR f1->getValue()); \
			else \
				throw IsNotAError(*value1,"an Integer"); \
			} \
		else if(f0!=0) \
			{ \
			if(i1!=0) \
				return new Boolean(f0->getValue() OPERATOR double(i1->getValue())); \
			else if(f1!=0) \
				return new Boolean(f0->getValue() OPERATOR f1->getValue()); \
			else \
				throw IsNotAError(*value1,"a FloatingPoint"); \
			} \
		else \
			{ \
			if(i1!=0) \
				throw IsNotAError(*value0,"an Integer"); \
			else if(f1!=0) \
				throw IsNotAError(*value0,"a FloatingPoint"); \
			else \
				throw IsNotAError(*value0,"a Number"); \
			} \
		} \
	}

COMPFUNC(Equal,==);
COMPFUNC(Unequal,!=);
COMPFUNC(LessThan,<);
COMPFUNC(LessEqual,<=);
COMPFUNC(GreaterEqual,>=);
COMPFUNC(Greater,>);

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
		else if(values.size()>1)
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
		else
			throw Error("Not enough function arguments");
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
		else if(values.size()>1)
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
		else
			throw Error("Not enough function arguments");
		}
	};

class Mod:public Function // Class to calculate the modulus of two Integers or FloatingPoints
	{
	/* Private methods: */
	private:
	static long posMod(long a,long b) // Calculates positive modulo
		{
		long result=a%b;
		if(result<0)
			result+=b>0?b:-b;
		return result;
		}
	static double posMod(double a,double b) // Calculates positive modulo
		{
		double result=fmod(a,b);
		if(result<0.0)
			result+=b>0.0?b:-b;
		return result;
		}
	
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Mod expr1 expr2) |-> expr1%expr2";
		
		return os;
		}
	
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(2,arguments);
		
		/* Evaluate the first two arguments: */
		ThingPtr value0=evalArg(0,arguments,context);
		ThingPtr value1=evalArg(1,arguments,context);
		
		/* Check if both values are integer or floating-points: */
		Integer* i0=toPtr<Integer>(*value0);
		Integer* i1=toPtr<Integer>(*value1);
		FloatingPoint* f0=toPtr<FloatingPoint>(*value0);
		FloatingPoint* f1=toPtr<FloatingPoint>(*value1);
		if((i1!=0&&i1->getValue()==0)||(f1!=0&&f1->getValue()==0.0))
			throw Error("Division by zero");
		if(i0!=0)
			{
			if(i1!=0)
				return new Integer(posMod(i0->getValue(),i1->getValue()));
			else if(f1!=0)
				return new FloatingPoint(posMod(double(i0->getValue()),f1->getValue()));
			else
				throw IsNotAError(*value1,"an Integer");
			}
		else if(f0!=0)
			{
			if(i1!=0)
				return new FloatingPoint(posMod(f0->getValue(),double(i1->getValue())));
			else if(f1!=0)
				return new FloatingPoint(posMod(f0->getValue(),f1->getValue()));
			else
				throw IsNotAError(*value1,"a FloatingPoint");
			}
		else
			{
			if(i1!=0)
				throw IsNotAError(*value0,"an Integer");
			else if(f1!=0)
				throw IsNotAError(*value0,"a FloatingPoint");
			else
				throw IsNotAError(*value0,"a Number");
			}
		}
	};

class Abs:public Function // Class to get the absolute value
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Abs expr) |-> abs(expr)";
		
		return os;
		}
	
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Evaluate the first argument: */
		ThingPtr value=evalArg(0,arguments,context);
		
		/* Check if the value is an Integer: */
		Integer* i=toPtr<Integer>(*value);
		if(i!=0)
			{
			/* Return a new Integer with the absolute value: */
			return new Integer(labs(i->getValue()));
			}
		else
			{
			/* Check if the value is a FloatingPoint: */
			FloatingPoint* f=toPtr<FloatingPoint>(*value);
			if(f!=0)
				{
				/* Return a new FloatingPoint with the absolute value: */
				return new FloatingPoint(fabs(f->getValue()));
				}
			else
				throw IsNotAError(*value,"a Number");
			}
		}
	};

/********************************************************
Use a macro to create the unary floating-point functions:
********************************************************/

#define UNARYFLOATFUNC(NAME,FUNCTION) \
class NAME:public Function \
	{ \
	/* Methods from class Thing: */ \
	public: \
	virtual std::ostream& print(std::ostream& os) const \
		{ \
		os<<"(Builtin::"<<#NAME<<" expr) |-> "<<#FUNCTION<<"(expr)"; \
		 \
		return os; \
		} \
	 \
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context) \
		{ \
		/* Check the argument list: */ \
		checkArity(1,arguments); \
		 \
		/* Convert the result of evaluating the first argument to a FloatingPoint and then evaluate the FUNCTION function: */ \
		return new FloatingPoint(FUNCTION(FloatingPoint::convertValue(*evalArg(0,arguments,context)))); \
		} \
	}

UNARYFLOATFUNC(Floor,floor);
UNARYFLOATFUNC(Ceil,ceil);
UNARYFLOATFUNC(Sqrt,sqrt);
UNARYFLOATFUNC(Exp,exp);
UNARYFLOATFUNC(Log,log);
UNARYFLOATFUNC(Exp10,exp10);
UNARYFLOATFUNC(Log10,log10);
UNARYFLOATFUNC(Sin,sin);
UNARYFLOATFUNC(Cos,cos);
UNARYFLOATFUNC(Tan,tan);
UNARYFLOATFUNC(Asin,asin);
UNARYFLOATFUNC(Acos,acos);
UNARYFLOATFUNC(Atan,atan);

class Pow:public Function // Class to raise one value to the power of another
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Pow expr1 expr2) |-> expr1**expr2";
		
		return os;
		}
	
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(2,arguments);
		
		/* Convert the results of evaluating the first two arguments to floating-point: */
		double value0=FloatingPoint::convertValue(*evalArg(0,arguments,context));
		double value1=FloatingPoint::convertValue(*evalArg(1,arguments,context));
		
		/* Return the result of raising the first value to the power of the second: */
		return new FloatingPoint(pow(value0,value1));
		}
	};

class Atan2:public Function // Class to calculate the full-circle arctangent
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Atan2 expr1 expr2) |-> atan2(expr1,expr2)";
		
		return os;
		}
	
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(2,arguments);
		
		/* Convert the results of evaluating the first two arguments to floating-point: */
		double value0=FloatingPoint::convertValue(*evalArg(0,arguments,context));
		double value1=FloatingPoint::convertValue(*evalArg(1,arguments,context));
		
		/* Return the result of the atan2 function: */
		return new FloatingPoint(atan2(value0,value1));
		}
	};

void forgetMath(Context& context);
void rememberMath(Context& context);

class ForgetMath:public Function // Class to make the Lambda Programming Language forget all about math
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::ForgetMath) |-> partial amnesia";
		
		return os;
		}
	
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(0,arguments);
		
		forgetMath(context);
		
		return 0;
		}
	};

class RememberMath:public Function // Class to make the Lambda Programming Language remember all about math
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::RememberMath) |-> total clarity";
		
		return os;
		}
	
	/* Methods from class Function: */ \
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(0,arguments);
		
		rememberMath(context);
		
		return 0;
		}
	};

void forgetMath(Context& context)
	{
	/* Make the parser forget numbers: */
	parserKnowsNumbers=false;
	
	/* Undefine all math functions in the context: */
	context.removeThing("=");
	context.removeThing("/=");
	context.removeThing("<");
	context.removeThing("<=");
	context.removeThing(">=");
	context.removeThing(">");
	context.removeThing("+");
	context.removeThing("-");
	context.removeThing("*");
	context.removeThing("/");
	context.removeThing("%");
	context.removeThing("pi");
	context.removeThing("e");
	context.removeThing("abs");
	context.removeThing("floor");
	context.removeThing("ceil");
	context.removeThing("sqrt");
	context.removeThing("exp");
	context.removeThing("log");
	context.removeThing("exp10");
	context.removeThing("log10");
	context.removeThing("sin");
	context.removeThing("cos");
	context.removeThing("tan");
	context.removeThing("asin");
	context.removeThing("acos");
	context.removeThing("atan");
	context.removeThing("pow");
	context.removeThing("atan2");
	
	context.removeThing("forget-math");
	context.setThing("remember-math",*new Builtin::RememberMath);
	}

void rememberMath(Context& context)
	{
	/* Make the parser remember numbers: */
	parserKnowsNumbers=true;
	
	/* Redefine all math functions in the context: */
	context.setThing("=",*new Builtin::Equal);
	context.setThing("/=",*new Builtin::Unequal);
	context.setThing("<",*new Builtin::LessThan);
	context.setThing("<=",*new Builtin::LessEqual);
	context.setThing(">=",*new Builtin::GreaterEqual);
	context.setThing(">",*new Builtin::Greater);
	context.setThing("+",*new Builtin::Add);
	context.setThing("-",*new Builtin::Sub);
	context.setThing("*",*new Builtin::Mult);
	context.setThing("/",*new Builtin::Div);
	context.setThing("%",*new Builtin::Mod);
	context.setThing("pi",*new FloatingPoint(M_PI));
	context.setThing("e",*new FloatingPoint(M_E));
	context.setThing("abs",*new Builtin::Abs);
	context.setThing("floor",*new Builtin::Floor);
	context.setThing("ceil",*new Builtin::Ceil);
	context.setThing("sqrt",*new Builtin::Sqrt);
	context.setThing("exp",*new Builtin::Exp);
	context.setThing("log",*new Builtin::Log);
	context.setThing("exp10",*new Builtin::Exp10);
	context.setThing("log10",*new Builtin::Log10);
	context.setThing("sin",*new Builtin::Sin);
	context.setThing("cos",*new Builtin::Cos);
	context.setThing("tan",*new Builtin::Tan);
	context.setThing("asin",*new Builtin::Asin);
	context.setThing("acos",*new Builtin::Acos);
	context.setThing("atan",*new Builtin::Atan);
	context.setThing("pow",*new Builtin::Pow);
	context.setThing("atan2",*new Builtin::Atan2);
	
	context.removeThing("remember-math");
	context.setThing("forget-math",*new Builtin::ForgetMath);
	}

}

void defBuiltins(Context& context)
	{
	context.setThing("atom?",*new Builtin::AtomP);
	
	context.setThing("null?",*new Builtin::NullP);
	
	context.setThing("cons?",*new Builtin::ConsP);
	context.setThing("cons",*new Builtin::Cons);
	context.setThing("car",*new Builtin::Car);
	context.setThing("cdr",*new Builtin::Cdr);
	
	context.setThing("quote",*new Builtin::Quote);
	context.setThing("eval",*new Eval);
	context.setThing("begin",*new Begin);
	
	context.setThing("boolean?",*new Builtin::BooleanP);
	context.setThing("#f",*new Boolean(false));
	context.setThing("#t",*new Boolean(true));
	context.setThing("not",*new Builtin::Not);
	context.setThing("and",*new Builtin::And);
	context.setThing("or",*new Builtin::Or);
	
	context.setThing("function?",*new Builtin::FunctionP);
	
	context.setThing("if",*new If);
	context.setThing("def",*new Def);
	context.setThing("load",*new Load);
	
	context.setThing("lambda",*new Builtin::Lambda);
	
	#if 0
	
	context.setThing("=",*new Builtin::Equal);
	context.setThing("/=",*new Builtin::Unequal);
	context.setThing("<",*new Builtin::LessThan);
	context.setThing("<=",*new Builtin::LessEqual);
	context.setThing(">=",*new Builtin::GreaterEqual);
	context.setThing(">",*new Builtin::Greater);
	
	context.setThing("+",*new Builtin::Add);
	context.setThing("-",*new Builtin::Sub);
	context.setThing("*",*new Builtin::Mult);
	context.setThing("/",*new Builtin::Div);
	context.setThing("%",*new Builtin::Mod);
	
	context.setThing("pi",*new FloatingPoint(M_PI));
	context.setThing("e",*new FloatingPoint(M_E));
	
	context.setThing("abs",*new Builtin::Abs);
	context.setThing("floor",*new Builtin::Floor);
	context.setThing("ceil",*new Builtin::Ceil);
	context.setThing("sqrt",*new Builtin::Sqrt);
	context.setThing("exp",*new Builtin::Exp);
	context.setThing("log",*new Builtin::Log);
	context.setThing("exp10",*new Builtin::Exp10);
	context.setThing("log10",*new Builtin::Log10);
	context.setThing("sin",*new Builtin::Sin);
	context.setThing("cos",*new Builtin::Cos);
	context.setThing("tan",*new Builtin::Tan);
	context.setThing("asin",*new Builtin::Asin);
	context.setThing("acos",*new Builtin::Acos);
	context.setThing("atan",*new Builtin::Atan);
	
	context.setThing("pow",*new Builtin::Pow);
	context.setThing("atan2",*new Builtin::Atan2);
	
	context.setThing("forget-math",*new Builtin::ForgetMath);
	
	#else
	
	parserKnowsNumbers=false;
	context.setThing("remember-math",*new Builtin::RememberMath);
	
	#endif
	}

}
