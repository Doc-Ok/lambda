/***********************************************************************
Thing - Base class for all objects.
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

#include "Thing.h"

#include <iostream>

namespace Lambda {

#if LAMBDA_CONFIG_INSTRUMENT

/******************************
Static elements of class Thing:
******************************/

size_t Thing::thingsEvaluated=0;
size_t Thing::thingsCreated=0;
size_t Thing::thingsDestroyed=0;

#endif

/**********************
Methods of class Thing:
**********************/

const char* Thing::classIsA(void)
	{
	return "a Thing";
	}

ThingPtr Thing::evaluate(Context& context)
	{
	#if LAMBDA_CONFIG_INSTRUMENT
	++thingsEvaluated;
	#endif
	
	return this;
	}

#if LAMBDA_CONFIG_INSTRUMENT

std::ostream& Thing::printCounters(std::ostream& os)
	{
	/* Print the performance counters: */
	std::cout<<"("<<thingsEvaluated<<" evaluations, +"<<thingsCreated<<", -"<<thingsDestroyed<<", = "<<ssize_t(thingsCreated)-ssize_t(thingsDestroyed)<<" cells)";
	
	/* Reset the performance counters to zero: */
	thingsEvaluated=0;
	thingsCreated=0;
	thingsDestroyed=0;
	
	return os;
	}

#endif

}
