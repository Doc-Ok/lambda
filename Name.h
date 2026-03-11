/***********************************************************************
Name - Class representing names of things.
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

#ifndef NAME_INCLUDED
#define NAME_INCLUDED

#include "String.h"
#include "Atom.h"

namespace Lambda {

class Name:public Atom
	{
	/* Elements: */
	private:
	String name; // This thing's name
	
	/* Constructors and destructors: */
	public:
	Name(const char* sName) // Creates a name from a C string
		:name(sName)
		{
		}
	Name(const std::string& sName) // Creates a name from a C++ string
		:name(sName)
		{
		}
	
	/* Methods from class Thing: */
	static const char* classIsA(void);
	virtual std::string isA(void) const
		{
		return classIsA();
		}
	virtual std::ostream& print(std::ostream& os) const;
	virtual ThingPtr evaluate(Context& context);
	
	/* New methods: */
	const String& getName(void) const // Returns this thing's name
		{
		return name;
		}
	static const String& getName(const Thing& thing) // Returns the given thing's name if it is a Name; throws exception otherwise
		{
		/* Try casting the given thing to a Name and return that Name's name: */
		return to<Name>(thing).name;
		}
	};

}

#endif
