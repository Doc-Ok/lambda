/***********************************************************************
Thing - Base class for all objects.
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

#ifndef THING_INCLUDED
#define THING_INCLUDED

#include <stddef.h>
#include <string>
#include <iosfwd>
#include <Misc/Autopointer.h>
#include <Threads/RefCounted.h>

#include "Config.h"
#include "Error.h"

/* Forward declarations: */
namespace Lambda {
class Context;
}

namespace Lambda {

/* Forward declaration: */
class Thing;

typedef Misc::Autopointer<Thing> ThingPtr; // Type for pointers to Thing objects

class Thing:public Threads::RefCounted
	{
	/* Elements: */
	public:
	#if LAMBDA_CONFIG_INSTRUMENT
	static size_t thingsEvaluated; // Counter to keep track of evaluation's time use
	static size_t thingsCreated,thingsDestroyed; // Counters to keep track of evaluation's space use
	#endif
	
	/* Constructors and destructors: */
	public:
	Thing(void) // Default constructor
		{
		#if LAMBDA_CONFIG_INSTRUMENT
		++thingsCreated;
		#endif
		}
	virtual ~Thing(void) // Destructor
		{
		#if LAMBDA_CONFIG_INSTRUMENT
		++thingsDestroyed;
		#endif
		}
	
	/* New methods: */
	public:
	static const char* classIsA(void); // Returns a string describing the type of the class, in the form "a thing"
	virtual std::string isA(void) const // Returns a string describing the type of the thing, in the form "a thing"
		{
		return classIsA();
		}
	virtual std::ostream& print(std::ostream& os) const =0; // Prints this thing to the given output stream; returns the output stream
	virtual ThingPtr evaluate(Context& context); // Returns the thing resulting from evaluating this thing in the given context; by default, a thing evaluates to itself
	#if LAMBDA_CONFIG_INSTRUMENT
	static void resetCounters(void); // Resets the performance counters before a parsing/evaluation sequence
	static std::ostream& printCounters(std::ostream& os); // Prints the performance counters after a parsing/evaluation sequence
	#endif
	};

/****************************
Generic operations on things:
****************************/

template <class DestThingParam>
bool is(const Thing& thing) // Returns true if the given thing is compatible with the requested type
	{
	/* Try casting the thing to the requested type: */
	return dynamic_cast<const DestThingParam*>(&thing)!=0;
	}

template <class DestThingParam>
const DestThingParam& to(const Thing& thing) // Tries converting the given thing of any type to the requested type; throws exception if the types are incompatible
	{
	/* Try casting the thing to the requested type: */
	const DestThingParam* result=dynamic_cast<const DestThingParam*>(&thing);
	if(result==0)
		throw IsNotAError(thing,DestThingParam::classIsA());
	
	return *result;
	}

template <class DestThingParam>
DestThingParam& to(Thing& thing) // Ditto, for mutable thing
	{
	/* Try casting the thing to the requested type: */
	DestThingParam* result=dynamic_cast<DestThingParam*>(&thing);
	if(result==0)
		throw IsNotAError(thing,DestThingParam::classIsA());
	
	return *result;
	}

template <class DestThingParam>
const DestThingParam* toPtr(const Thing& thing) // Tries converting the given thing of any type to a standard pointer to the requested type; returns null if the types are incompatible
	{
	/* Try casting the thing to the requested type: */
	return dynamic_cast<const DestThingParam*>(&thing);
	}

template <class DestThingParam>
DestThingParam* toPtr(Thing& thing) // Ditto, for mutable thing
	{
	/* Try casting the thing to the requested type: */
	return dynamic_cast<DestThingParam*>(&thing);
	}

template <class DestThingParam>
const DestThingParam* toKnownPtr(const Thing& thing) // Converts the given thing of any type to a standard pointer to the requested type; assumes that the thing is of a compatible type
	{
	/* Cast the thing to the requested type: */
	return static_cast<const DestThingParam*>(&thing);
	}

template <class DestThingParam>
DestThingParam* toKnownPtr(Thing& thing) // Ditto, for mutable thing
	{
	/* Cast the thing to the requested type: */
	return static_cast<DestThingParam*>(&thing);
	}

}

#endif
