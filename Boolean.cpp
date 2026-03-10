/***********************************************************************
Boolean - Class representing boolean values.
Copyright (c) 2017-2026 Oliver Kreylos
***********************************************************************/

#include "Boolean.h"

#include <iostream>

#include "Error.h"

namespace Lambda {

/********************************
Static elements of class Boolean:
********************************/

Boolean Boolean::theTrue(true);
Boolean Boolean::theFalse(false);

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
