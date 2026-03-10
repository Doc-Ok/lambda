/***********************************************************************
Load - Class for a function that loads a file and evaluates all
expressions in it.
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

#ifndef LOAD_INCLUDED
#define LOAD_INCLUDED

#include "Function.h"

namespace Lambda {

class Load:public Function
	{
	/* Methods from class Thing: */
	public:
	virtual std::ostream& print(std::ostream& os) const;
	
	/* Methods from class Function: */
	ThingPtr evaluate(ThingPtr arguments,Context& context);
	
	/* New methods: */
	static void load(const char* fileName,Context& context); // Loads the given file inside the given context
	};

}

#endif
