/***********************************************************************
Cons - Class representing an interior node in a binary tree, primarily
used to construct lists.
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

#ifndef CONS_INCLUDED
#define CONS_INCLUDED

#include <vector>

#include "Thing.h"

namespace Lambda {

class Cons:public Thing
	{
	/* Elements: */
	private:
	ThingPtr children[2]; // The two children
	
	/* Constructors and destructors: */
	public:
	Cons(Thing& sCar,Thing& sCdr) // Creates a Cons from the two given children
		{
		children[0]=&sCar;
		children[1]=&sCdr;
		}
	
	/* Methods from class Thing: */
	static const char* classIsA(void);
	virtual std::string isA(void) const
		{
		return classIsA();
		}
	virtual ThingPtr evaluate(Context& context);
	virtual std::ostream& print(std::ostream& os) const;
	
	/* New methods: */
	Thing& car(void) // Returns the left child
		{
		return *children[0];
		}
	Thing& cdr(void) // Returns the right child
		{
		return *children[1];
		}
	bool isList(void) const; // Returns true if this Cons object represents a "proper" list
	size_t checkList(void) const; // Checks if this Cons object represents a "proper" list and returns the list's length if so; throws exception otherwise
	std::vector<ThingPtr> getList(void); // Returns the elements of a proper list as a vector
	static Thing& car(Thing& thing) // Returns the left child of the given thing if it is a Cons; throws exception otherwise
		{
		/* Try casting the given thing to a Cons and return that Cons's car: */
		return *to<Cons>(thing).children[0];
		}
	static Thing& cdr(Thing& thing) // Returns the right child of the given thing if it is a Cons; throws exception otherwise
		{
		/* Try casting the given thing to a Cons and return that Cons's cdr: */
		return *to<Cons>(thing).children[1];
		}
	static bool isList(const Thing& thing); // Returns true if the given thing is a "proper" list
	static size_t checkList(const Thing& thing); // Checks if the given thing is a "proper" list and returns the list's length if so; throws exception otherwise
	};

}

#endif
