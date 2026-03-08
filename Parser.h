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

#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#include <ctype.h>
#include <iostream>

#include "Thing.h"

namespace Lambda {

class InputStream // Class to read input from streams with one character look-ahead
	{
	/* Elements: */
	private:
	std::istream& stream; // The input stream
	int next; // The next character to be returned by get()
	
	/* Constructors and destructors: */
	public:
	InputStream(std::istream& sStream) // Creates an input stream from the given std::istream
		:stream(sStream),next(stream.get())
		{
		}
	
	/* Methods: */
	bool eof(void) const // Returns true if the input stream has been exhausted
		{
		return next<0;
		}
	bool token(void) const // Returns true if the next character is part of a token
		{
		return next>=0&&next!='('&&next!=')'&&!isspace(next);
		}
	int peek(void) const // Returns the next character without removing it from the input stream
		{
		return next;
		}
	int get(void) // Returns the next character and removes it from the input stream
		{
		int result=next;
		next=stream.get();
		
		return result;
		}
	void skipWs(void) // Skips whitespace in the input stream
		{
		while(next>=0&&isspace(next))
			next=stream.get();
		}
	int getAndSkipWs(void) // Returns the next character, removes it from the input stream, then skips whitespace in the input stream
		{
		int result=next;
		next=stream.get();
		
		while(next>=0&&isspace(next))
			next=stream.get();
		
		return result;
		}
	};

ThingPtr parse(InputStream& is); // Main parser function; reads from the given input stream until an expression is complete, then returns it

}

#endif
