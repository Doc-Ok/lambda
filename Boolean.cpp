/***********************************************************************
Boolean - Class representing boolean values.
Copyright (c) 2017-2026 Oliver Kreylos
***********************************************************************/

#include "Boolean.h"

#include <iostream>

#include "Error.h"

namespace Lambda {

/************************
Methods of class Boolean:
************************/

const char* Boolean::classIsA(void)
	{
	return "a Boolean";
	}

std::ostream& Boolean::print(std::ostream& os) const
	{
	/* Print this thing's boolean value: */
	os<<(value?"#t":"#f");
	
	return os;
	}

}
