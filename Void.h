/***********************************************************************
Void - Class representing nothing at all, the Lambda Programming
Language's equivalent of a null pointer.
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

#ifndef VOID_INCLUDED
#define VOID_INCLUDED

#include "Atom.h"

namespace Lambda {

class Void:public Atom
	{
	/* Elements: */
	public:
	static Void the; // The single Void object in existence
	
	/* Constructors and destructors: */
	private:
	Void(void) // Not publicly accessible
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
	};

}

#endif
