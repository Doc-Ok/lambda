/***********************************************************************
Parser - A class and functions to parse expressions entered via the
console or read from a file.
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

#include "Parser.h"

#include <string>

#include "Error.h"
#include "Null.h"
#include "Cons.h"
#include "Quote.h"
#include "Boolean.h"
#include "Name.h"

namespace Lambda {

namespace {

/****************
Helper functions:
****************/

ThingPtr parseCons(InputStream& is) // Function to parse a Cons expression
	{
	/* Check if the next character is end-of-file or already a closing parenthesis: */
	if(is.eof())
		throw Error("End-of-file while parsing Cons expression");
	else if(is.peek()==')')
		{
		/* Return an empty list: */
		return new Null;
		}
	
	/* Parse the Cons's left child: */
	ThingPtr left=parse(is);
	
	/* Check if the next character is a cons token: */
	is.skipWs();
	if(is.peek()=='.')
		{
		/* Parse the Cons's right child: */
		is.getAndSkipWs();
		ThingPtr right=parse(is);
		
		return new Cons(*left,*right);
		}
	else if(!is.eof())
		{
		/* Parse another Cons and make a list: */
		ThingPtr right=parseCons(is);
		
		return new Cons(*left,*right);
		}
	else
		throw Error("End-of-file while parsing Cons expression");
	}

}

ThingPtr parse(InputStream& is)
	{
	ThingPtr result;
	
	/* Let's see what's up: */
	if(is.peek()=='(')
		{
		/* Parse a Cons: */
		is.getAndSkipWs();
		result=parseCons(is);
		
		/* Check for the closing parenthesis: */
		is.skipWs();
		if(is.peek()!=')')
			throw Error("Missing ')' while parsing Cons expression");
		is.get();
		}
	else if(is.peek()=='\'')
		{
		/* Parse a Quote: */
		is.getAndSkipWs();
		result=new Quote(*parse(is));
		}
	else if(is.token())
		{
		/* Parse a name: */
		std::string name;
		do
			name.push_back(is.get());
		while(is.token());
		
		result=new Name(name);
		}
	else if(!is.eof())
		throw makeError("Unexpected '%c' while parsing",is.get());
	
	return result;
	}

}
