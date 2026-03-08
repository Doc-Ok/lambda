/***********************************************************************
Quote - Class representing a quoted thing.
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

#ifndef QUOTE_INCLUDED
#define QUOTE_INCLUDED

#include "Thing.h"

namespace Lambda {

class Quote:public Thing
	{
	/* Elements: */
	private:
	ThingPtr quoted; // The quoted thing
	
	/* Constructors and destructors: */
	public:
	Quote(Thing& sQuoted) // Elementwise constructor
		:quoted(&sQuoted)
		{
		}
	
	/* Methods from class Thing: */
	static const char* classIsA(void);
	virtual std::string isA(void) const
		{
		return classIsA();
		}
	virtual ThingPtr evaluate(Context& context);
	virtual std::ostream& print(std::ostream& os) const;
	
	/* New methods: */
	Thing& getQuoted(void) // Returns the quoted thing
		{
		return *quoted;
		}
	static Thing& getQuoted(Thing& thing) // Returns the quoted thing of the given thing if it is a Quote; throws exception otherwise
		{
		/* Try casting the given thing to a Quote and return that Quote's quoted thing: */
		return *to<Quote>(thing).quoted;
		}
	};

}

#endif
