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

#ifndef FUNCTION_INCLUDED
#define FUNCTION_INCLUDED

#include <stddef.h>
#include <vector>

#include "Atom.h"

namespace Lambda {

class Function:public Atom
	{
	/* Protected methods: */
	protected:
	static size_t getArity(ThingPtr arguments); // Throws an exception if the given argument is not a proper list; returns number of arguments otherwise
	static void checkArity(size_t arity,ThingPtr arguments); // Throws an exception if the given argument is not a proper list with the given number of elements
	static Thing& getArg(size_t n,ThingPtr arguments); // Returns the n-th argument from the given list of arguments, which is already known to be a proper list with at least n elements
	template <class DestThingParam>
	static DestThingParam& getArg(size_t n,ThingPtr arguments) // Ditto, but casts the argument to the requested type
		{
		/* Defer to the other method and cast its result: */
		return to<DestThingParam>(getArg(n,arguments));
		}
	static ThingPtr evalArg(size_t n,ThingPtr arguments,Context& context); // Returns the result of evaluating the n-th argument from the given list of arguments, which is already known to be a proper list with at least n elements
	static std::vector<Thing*> getArgs(ThingPtr arguments); // Returns the list of arguments as a vector; throws exception if the given argument is not a proper list
	static std::vector<ThingPtr> evalArgs(ThingPtr arguments,Context& context); // Returns the list of results of evaluating the arguments as a vector; throws exception if the given argument is not a proper list
	
	/* Methods from class Thing: */
	public:
	static const char* classIsA(void);
	virtual std::string isA(void) const
		{
		return classIsA();
		}
	
	/* New methods: */
	public:
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context) =0; // Evaluates the function with the given argument(s) in the given context
	static ThingPtr evaluate(Thing& thing,ThingPtr arguments,Context& context) // Evaluates the given thing as a function if it is a Function; throws exception otherwise
		{
		#if LAMBDA_CONFIG_INSTRUMENT
		++thingsEvaluated;
		#endif
		
		/* Try casting the given thing to a Function and return the result of evaluating that Function: */
		return to<Function>(thing).evaluate(arguments,context);
		}
	};

typedef Misc::Autopointer<Function> FunctionPtr; // Type for pointers to Function objects

}

#endif
