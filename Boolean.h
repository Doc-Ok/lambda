/***********************************************************************
Boolean - Class representing boolean values.
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

#ifndef BOOLEAN_INCLUDED
#define BOOLEAN_INCLUDED

#include "Atom.h"

namespace Lambda {

class Boolean:public Atom
	{
	/* Elements: */
	private:
	bool value; // The boolean value
	
	/* Constructors and destructors: */
	public:
	Boolean(bool sValue) // Elementwise constructor
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
	bool getValue(void) const // Returns the boolean value
		{
		return value;
		}
	static bool getValue(const Thing& thing) // Returns the boolean value of the given thing if it is a Boolean; throws exception otherwise
		{
		/* Try casting the given thing to a Boolean and return that Boolean's value: */
		return to<Boolean>(thing).value;
		}
	};

}

#endif
