/***********************************************************************
FloatingPoint - Class representing floating-point values.
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

#ifndef FLOATINGPOINT_INCLUDED
#define FLOATINGPOINT_INCLUDED

#include "Thing.h"

namespace Lambda {

class FloatingPoint:public Thing
	{
	/* Elements: */
	private:
	double value; // The floating-point value
	
	/* Constructors and destructors: */
	public:
	FloatingPoint(double sValue) // Elementwise constructor
		:value(sValue)
		{
		}
	
	/* Methods from class Thing: */
	static const char* classIsA(void);
	virtual std::string isA(void) const
		{
		return classIsA();
		}
	virtual std::ostream& print(std::ostream& os) const;
	
	/* New methods: */
	double getValue(void) const // Returns the floating-point value
		{
		return value;
		}
	static double getValue(const Thing& thing) // Returns the floating-point value of the given thing if it is a FloatingPoint; throws exception otherwise
		{
		return to<FloatingPoint>(thing).value;
		}
	static double convertValue(const Thing& thing); // Returns the floating-point value of the given thing if it is or can be converted to a FloatingPoint; throws exception otherwise
	};

}

#endif
