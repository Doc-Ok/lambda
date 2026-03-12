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

#include "Load.h"

#include <iostream>

#include "Void.h"
#include "Name.h"
#include "FileLoader.h"

namespace Lambda {

/*****************************
Static elements of class Load:
*****************************/

FileLoader* Load::fileLoader=0;

/*********************
Methods of class Load:
*********************/

std::ostream& Load::print(std::ostream& os) const
	{
	os<<"(Builtin::Load filename) |->";
	
	return os;
	}

ThingPtr Load::evaluate(ThingPtr arguments,Context& context)
	{
	/* Check the argument list: */
	checkArity(1,arguments);
	
	/* Get the first argument: */
	std::string fileName(Name::getName(getArg(0,arguments)).c_str());
	
	/* Load the file via the file loader: */
	fileLoader->loadFile(fileName.c_str(),context);
	
	/* Return nothing: */
	return Void::get();
	}

void Load::setFileLoader(FileLoader* newFileLoader)
	{
	fileLoader=newFileLoader;
	}

}
