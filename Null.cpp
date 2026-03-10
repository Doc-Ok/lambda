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

#include "Null.h"

#include <iostream>

namespace Lambda {

/*****************************
Static elements of class Null:
*****************************/

Null Null::the;

/*********************
Methods of class Null:
*********************/

const char* Null::classIsA(void)
	{
	return "a Null";
	}

std::ostream& Null::print(std::ostream& os) const
	{
	/* Print the empty list token: */
	os<<"()";
	
	return os;
	}

}
