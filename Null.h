/***********************************************************************
Null - Class representing the empty list, or null thing.
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

#ifndef NULL_INCLUDED
#define NULL_INCLUDED

#include "Atom.h"

namespace Lambda {

class Null:public Atom
	{
	/* Elements: */
	private:
	static Null the; // The single Null object in existence
	
	/* Constructors and destructors: */
	Null(void) // Not publicly accessible
		{
		/* Take a reference to this object so it will never be destroyed: */
		ref();
		}
	
	/* Methods from class Thing: */
	public:
	static const char* classIsA(void);
	virtual std::string isA(void) const
		{
		return classIsA();
		}
	virtual std::ostream& print(std::ostream& os) const;
	
	/* New methods: */
	static Null* get(void) // Returns a pointer to the Null object
		{
		return &the;
		}
	static bool is(const Thing& thing) // Returns true if the given thing is the Null object
		{
		return &thing==&the;
		}
	};

}

#endif
