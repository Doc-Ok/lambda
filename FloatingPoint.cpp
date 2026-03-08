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

#include "FloatingPoint.h"

#include <iostream>

#include "Integer.h"

namespace Lambda {

/******************************
Methods of class FloatingPoint:
******************************/

const char* FloatingPoint::classIsA(void)
	{
	return "a FloatingPoint";
	}

std::ostream& FloatingPoint::print(std::ostream& os) const
	{
	os<<value;
	
	return os;
	}

double FloatingPoint::convertValue(const Thing& thing)
	{
	/* First check if it's an Integer: */
	const Integer* i=toPtr<Integer>(thing);
	if(i!=0)
		return double(i->getValue());
	
	/* Then assume it's a FloatingPoint: */
	return to<FloatingPoint>(thing).value;
	}

}
