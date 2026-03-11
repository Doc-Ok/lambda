/***********************************************************************
Geometry - Declarations for drawing-related data types.
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

#ifndef GEOMETRY_INCLUDED
#define GEOMETRY_INCLUDED

#include <vector>
#include <GL/gl.h>
#include <GL/GLColor.h>

namespace Lambda {

typedef float Scalar; // Scalar type for points

struct Point // Structure defining a 2D point
	{
	/* Elements: */
	public:
	Scalar coords[2]; // Point coordinates
	
	/* Constructors and destructors: */
	Point(void) // Dummy constructor
		{
		}
	Point(Scalar sX,Scalar sY) // Elementwise constructor
		{
		coords[0]=sX;
		coords[1]=sY;
		}
	};

typedef GLColor<GLubyte,4> Color; // Type for RGB colors with opacity
typedef std::vector<Point> Polyline; // Type for polylines defined by lists of points

}

#endif
