/***********************************************************************
Context - Class associating names with things and providing stack frames
for recursion.
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

#ifndef CONTEXT_INCLUDED
#define CONTEXT_INCLUDED

#include <stddef.h>
#include <vector>
#include <Misc/HashTable.h>

#include "String.h"
#include "Thing.h"

namespace Lambda {

class Context
	{
	/* Embedded classes: */
	private:
	typedef Misc::HashTable<String,ThingPtr,String> ThingMap; // Hash table mapping names to things
	
	struct Mapping // Structure representing a previous mapping in a thing map, i.e., an undo buffer for defining mappings
		{
		/* Elements: */
		public:
		String name; // The mapped name
		ThingPtr thing; // The thing to which the name is mapped, or null if the name does not map to anything
		
		/* Constructors and destructors: */
		Mapping(const String& sName,Thing& sThing) // Elementwise constructor
			:name(sName),thing(&sThing)
			{
			}
		Mapping(const String& sName) // Ditto, for a name that does not map to anything
			:name(sName)
			{
			}
		};
	
	typedef std::vector<Mapping> MappingList; // Type for lists of mappings
	
	public:
	class Frame // Helper class to manage stack frames via RAII
		{
		/* Elements: */
		private:
		Context& context; // Reference to the context to which this stack frame belongs
		size_t frameBase; // The frame base returned by openFrame();
		
		/* Constructors and destructors: */
		public:
		Frame(Context& sContext) // Opens a new stack frame in the given context
			:context(sContext),frameBase(context.openFrame())
			{
			}
		~Frame(void) // Closes the stack frame
			{
			context.closeFrame(frameBase);
			}
		};
	
	friend class Frame;
	
	/* Elements: */
	private:
	ThingMap thingMap; // The map from names to things
	MappingList stack; // The stack of mappings to be undone
	size_t numOpenFrames; // Number of currently open stack frames
	void closeFrame(size_t frameBase); // Closes the current stack frame and unwinds any mappings made inside that frame; the frameBase value must be the value returned by the matching openFrame call
	
	/* Private methods: */
	size_t openFrame(void); // Opens a new stack frame and returns a frame base counter that must be passed to the closeFrame method to unwind the frame
	
	/* Constructors and destructors: */
	public:
	Context(void); // Creates an empty context with no mappings
	
	/* Methods: */
	Context* clone(void) const; // Clones the current state of this context to create a closure
	std::vector<String>* getMatchingNames(const char* begin,const char* end) const; // Returns the list of defined names whose prefix matches the given character sequence
	void setThing(const String& name,Thing& thing); // Maps the given name to the given thing inside the current stack frame; replaces any previous mapping
	void removeThing(const String& name); // Removes the thing that was mapped to the given name inside the current stack frame
	Thing& getThing(const String& name); // Returns the thing mapped to the given name; throws exception if the name has no mapping
	};

}

#endif
