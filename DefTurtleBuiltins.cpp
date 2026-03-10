/***********************************************************************
DefTurtleBuiltIns - Function to define the built-in turtle control
functions in an evaluation context.
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

#include "DefTurtleBuiltins.h"

#include <iostream>

#include "Context.h"
#include "Void.h"
#include "FloatingPoint.h"
#include "Function.h"
#include "Turtle.h"

namespace Lambda {

namespace Builtin {

/************
Global state:
************/

Turtle* activeTurtle=0; // Pointer to the currently active turtle

/*********************
Functions with Turtle:
*********************/

class Turtle:public Function // Function to create a new turtle
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Turtle) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(0,arguments);
		
		/* Detach from the currently active turtle if there is one: */
		if(activeTurtle!=0)
			activeTurtle->detach();
		activeTurtle=0;
		
		/* Create a new turtle: */
		activeTurtle=new ::Lambda::Turtle;
		
		return Void::get();
		}
	};

class Forward:public Function // Function to move the turtle forward by a number of steps
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Forward steps) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Evaluate the first argument and convert it to a floating-point number: */
		ThingPtr arg0=evalArg(0,arguments,context);
		double steps=FloatingPoint::convertValue(*arg0);
		
		/* Check that there is an active turtle: */
		if(activeTurtle==0)
			throw Error("There is no active turtle");
		
		/* Send a command to the active turtle: */
		activeTurtle->sendCommand(::Lambda::Turtle::Forward,steps);
		
		return Void::get();
		}
	};

class Back:public Function // Function to move the turtle back by a number of steps
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Back steps) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Evaluate the first argument and convert it to a floating-point number: */
		ThingPtr arg0=evalArg(0,arguments,context);
		double steps=FloatingPoint::convertValue(*arg0);
		
		/* Check that there is an active turtle: */
		if(activeTurtle==0)
			throw Error("There is no active turtle");
		
		/* Send a command to the active turtle: */
		activeTurtle->sendCommand(::Lambda::Turtle::Back,steps);
		
		return Void::get();
		}
	};

class Left:public Function // Function to rotate the turtle left by a number of degrees
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Left degrees) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Evaluate the first argument and convert it to a floating-point number: */
		ThingPtr arg0=evalArg(0,arguments,context);
		double degrees=FloatingPoint::convertValue(*arg0);
		
		/* Check that there is an active turtle: */
		if(activeTurtle==0)
			throw Error("There is no active turtle");
		
		/* Send a command to the active turtle: */
		activeTurtle->sendCommand(::Lambda::Turtle::Left,degrees);
		
		return Void::get();
		}
	};

class Right:public Function // Function to rotate the turtle right by a number of degrees
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Right degrees) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(1,arguments);
		
		/* Evaluate the first argument and convert it to a floating-point number: */
		ThingPtr arg0=evalArg(0,arguments,context);
		double degrees=FloatingPoint::convertValue(*arg0);
		
		/* Check that there is an active turtle: */
		if(activeTurtle==0)
			throw Error("There is no active turtle");
		
		/* Send a command to the active turtle: */
		activeTurtle->sendCommand(::Lambda::Turtle::Right,degrees);
		
		return Void::get();
		}
	};

class PenUp:public Function // Function to lift the turtle's pen
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::PenUp) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(0,arguments);
		
		/* Check that there is an active turtle: */
		if(activeTurtle==0)
			throw Error("There is no active turtle");
		
		/* Send a command to the active turtle: */
		activeTurtle->sendCommand(::Lambda::Turtle::PenUp);
		
		return Void::get();
		}
	};

class PenDown:public Function // Function to lower the turtle's pen
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::PenDown) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(0,arguments);
		
		/* Check that there is an active turtle: */
		if(activeTurtle==0)
			throw Error("There is no active turtle");
		
		/* Send a command to the active turtle: */
		activeTurtle->sendCommand(::Lambda::Turtle::PenDown);
		
		return Void::get();
		}
	};

class Reset:public Function // Function to reset the turtle
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const
		{
		os<<"(Builtin::Reset) |->";
		
		return os;
		}
	
	/* Methods from class Function: */
	virtual ThingPtr evaluate(ThingPtr arguments,Context& context)
		{
		/* Check the argument list: */
		checkArity(0,arguments);
		
		/* Check that there is an active turtle: */
		if(activeTurtle==0)
			throw Error("There is no active turtle");
		
		/* Send a command to the active turtle: */
		activeTurtle->sendCommand(::Lambda::Turtle::Reset);
		
		return Void::get();
		}
	};

}

void defTurtleBuiltins(Context& context)
	{
	context.setThing("turtle",*new Builtin::Turtle);
	context.setThing("forward",*new Builtin::Forward);
	context.setThing("back",*new Builtin::Back);
	context.setThing("left",*new Builtin::Left);
	context.setThing("right",*new Builtin::Right);
	context.setThing("penup",*new Builtin::PenUp);
	context.setThing("pendown",*new Builtin::PenDown);
	context.setThing("reset",*new Builtin::Reset);
	}

}
