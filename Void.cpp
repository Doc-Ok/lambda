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

#include "Void.h"

#include <iostream>

namespace Lambda {

/*****************************
Static elements of class Void:
*****************************/

Void Void::the;

/*********************
Methods of class Void:
*********************/

const char* Void::classIsA(void)
	{
	return "the Void";
	}

std::ostream& Void::print(std::ostream& os) const
	{
	/* Print the void token: */
	os<<"void";
	
	return os;
	}

}
