/***********************************************************************
PolylineRenderer - Class to render polylines with geometric line width
and high-quality anti-aliasing.
Copyright (c) 2016-2025 Oliver Kreylos

This file is part of the Lambda Programming Language and imported from
the SketchPad vector drawing package.

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

#ifndef LAMBDA_POLYLINERENDERER_INCLUDED
#define LAMBDA_POLYLINERENDERER_INCLUDED

#include <GL/GLObject.h>

#include "Geometry.h"

/* Forward declarations: */
class GLContextData;

namespace Lambda {

class PolylineRenderer:public GLObject
	{
	/* Embedded classes: */
	private:
	struct DataItem; // Forward declaration of per-context data structure
	
	/* Elements: */
	Scalar scaleFactor; // Scale factor from line widths to model space units
	
	/* Constructors and destructors: */
	public:
	PolylineRenderer(void);
	virtual ~PolylineRenderer(void);
	
	/* Methods from class GLObject: */
	virtual void initContext(GLContextData& contextData) const;
	
	/* New methods: */
	GLObject::DataItem* activate(Scalar newPixelSize,GLContextData& contextData) const; // Activates the polyline renderer in the given OpenGL context
	void deactivate(GLObject::DataItem* dataItem) const; // Deactivates the polyline renderer
	void setScaleFactor(Scalar newScaleFactor); // Updates the scale factor from line widths to model space units
	void draw(const Polyline& polyline,const Color& color,Scalar lineWidth,GLObject::DataItem* dataItem) const; // Renders the given polyline with the given color and line width
	void draw(const void* cacheId,unsigned int version,const Polyline& polyline,const Color& color,Scalar lineWidth,GLObject::DataItem* dataItem) const; // Caches the given polyline and renders it with the given color and line width
	bool draw(const void* cacheId,unsigned int version,const Color& color,Scalar lineWidth,GLObject::DataItem* dataItem) const; // Renders an already cached polyline or prepares to upload polyline vertices one at a time; returns true if vertices need to be uploaded
	void addVertex(const Point& vertex,GLObject::DataItem* dataItem) const; // Uploads an additional vertex to the polyline being uploaded
	void setPixelSizeRange(Scalar pixelSizeMin,Scalar pixelSizeMax,GLObject::DataItem* dataItem) const; // Sets the valid pixel size range of the polyline currently being uploaded
	void finish(GLObject::DataItem* dataItem) const; // Finishes uploading and draws the polyline being uploaded
	};

}

#endif
