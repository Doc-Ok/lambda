/***********************************************************************
Integer - Class representing integer values.
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

#ifndef INTEGER_INCLUDED
#define INTEGER_INCLUDED

#include "Thing.h"

namespace Lambda {

class Integer:public Thing
	{
	/* Elements: */
	private:
	long value; // The integer value
	
	/* Constructors and destructors: */
	public:
	Integer(long sValue) // Elementwise constructor
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
	long getValue(void) const // Returns the integer value
		{
		return value;
		}
	static long getValue(const Thing& thing) // Returns the integer value of the given thing if it is an Integer; throws exception otherwise
		{
		/* Try casting the given thing to an Integer and return that Integer's value: */
		return to<Integer>(thing).value;
		}
	};

}

#endif
