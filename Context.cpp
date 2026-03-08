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

#include "Context.h"

#include <stdio.h>

#include "Error.h"

namespace Lambda {

/************************
Methods of class Context:
************************/

Context::Context(void)
	:thingMap(101),numOpenFrames(0)
	{
	}

Context* Context::clone(void) const
	{
	/* Create a new context: */
	Context* result=new Context;
	
	/* Clone all name definitions: */
	for(ThingMap::ConstIterator tmIt=thingMap.begin();!tmIt.isFinished();++tmIt)
		result->thingMap.setEntry(*tmIt);
	
	return result;
	}

std::vector<String>* Context::getMatchingNames(const char* begin,const char* end) const
	{
	/* Collect a list of matching names: */
	std::vector<String>* result=new std::vector<String>;
	
	/* Iterate over all mappings: */
	size_t length=end-begin;
	for(ThingMap::ConstIterator tmIt=thingMap.begin();!tmIt.isFinished();++tmIt)
		{
		/* Check if the defined name is at least as long as the matching prefix: */
		if(tmIt->getSource().length()>=length)
			{
			/* Check if the matching prefix completely matches the defined name: */
			if(memcmp(tmIt->getSource().c_str(),begin,length)==0)
				{
				/* Add the defined name to the match list: */
				result->push_back(tmIt->getSource());
				}
			}
		}
	
	return result;
	}

size_t Context::openFrame(void)
	{
	/* Increment the stack frame counter and return the number of mappings in the stack: */
	++numOpenFrames;
	return stack.size();
	}

void Context::setThing(const String& name,Thing& thing)
	{
	/* Check if the name is already in the map: */
	ThingMap::Iterator tmIt=thingMap.findEntry(name);
	if(!tmIt.isFinished())
		{
		/* Push the name's current mapping onto the stack if there is an open frame: */
		if(numOpenFrames>0)
			stack.push_back(Mapping(name,*tmIt->getDest()));
		
		/* Change the mapping in the map: */
		tmIt->getDest()=&thing;
		}
	else
		{
		/* Push the name's current empty mapping onto the stack if there is an open frame: */
		if(numOpenFrames>0)
			stack.push_back(Mapping(name));
		
		/* Add the mapping to the map: */
		thingMap.setEntry(ThingMap::Entry(name,&thing));
		}
	}

void Context::removeThing(const String& name)
	{
	/* Check if the name is actually in the map: */
	ThingMap::Iterator tmIt=thingMap.findEntry(name);
	if(!tmIt.isFinished())
		{
		/* Push the name's current mapping onto the stack if there is an open frame: */
		if(numOpenFrames>0)
			stack.push_back(Mapping(name,*tmIt->getDest()));
		
		/* Remove the mapping from the map: */
		thingMap.removeEntry(tmIt);
		}
	}

void Context::closeFrame(size_t frameBase)
	{
	/* Undo all mappings from the top of the stack to the given frame base: */
	while(stack.size()>frameBase)
		{
		/* Get the most recent mapping: */
		Mapping& top=stack.back();
		
		/* Undo the mapping: */
		if(top.thing!=0)
			thingMap.setEntry(ThingMap::Entry(top.name,top.thing));
		else
			thingMap.removeEntry(top.name);
		
		/* Remove the most recent mapping from the stack: */
		stack.pop_back();
		}
	
	/* Decrement the stack frame counter: */
	--numOpenFrames;
	}

Thing& Context::getThing(const String& name)
	{
	/* Find the name in the map and return the associated thing: */
	ThingMap::Iterator tmIt=thingMap.findEntry(name);
	if(tmIt.isFinished())
		{
		char buffer[1024];
		snprintf(buffer,sizeof(buffer),"Name %s is not defined",name.c_str());
		throw Error(buffer);
		}
	
	return *tmIt->getDest();
	}

}
