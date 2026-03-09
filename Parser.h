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

#include "Thing.h"

/* Forward declarations: */
namespace Lambda {
class InputStream;
}

namespace Lambda {

extern bool parserKnowsNumbers; // Flag to enable or disable the parser's knowledge of numeric types

ThingPtr parse(InputStream& is); // Main parser function; reads from the given input stream until an expression is complete, then returns it

}

#endif
