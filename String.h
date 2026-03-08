/***********************************************************************
String - Class representing immutable strings that can be shared.
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

#ifndef STRING_INCLUDED
#define STRING_INCLUDED

#include <string.h>
#include <string>
#include <Threads/Atomic.h>

namespace Lambda {

class String
	{
	/* Embedded classes: */
	private:
	struct Rep // The internal representation of the string
		{
		/* Elements: */
		public:
		static size_t totalMem;
		Threads::Atomic<unsigned int> refCount; // Current number of autopointers referencing this string representation
		size_t length; // Length of the string, not counting the terminating NUL character
		
		/* Constructors and destructors: */
		Rep(size_t sLength,const char* string) // Create a new representation with a reference count of 1
			:refCount(1),length(sLength)
			{
			/* Copy the string into this representation: */
			memcpy(this+1,string,length+1);
			}
		};
	
	/* Elements: */
	Rep* rep; // Pointer to the string representation
	
	/* Private methods: */
	void ref(void) // References the string's current representation
		{
		if(rep!=0)
			rep->refCount.preAdd(1);
		}
	void unref(void) // Unreferences the string's current representation and destroys it if the reference count reaches zero
		{
		if(rep!=0&&rep->refCount.preSub(1)==0)
			free(rep);
		}
	
	/* Constructors and destructors: */
	public:
	String(void) // Creates a null string, it't not even empty
		:rep(0)
		{
		}
	String(const char* string) // Creates a string from the given C string or an invalid string if the C string is null
		:rep(0)
		{
		/* Check if the C string is valid: */
		if(string!=0)
			{
			/* Allocate a representation for the C string: */
			size_t length=strlen(string);
			rep=new (malloc(sizeof(Rep)+length+1)) Rep(length,string); // Allocate space for the representation header, the string's characters, and the terminating NUL
			}
		}
	String(const std::string& string) // Creates a string from the given C++ string
		:rep(0)
		{
		/* Allocate a representation for the C++ string: */
		size_t length=string.size();
		rep=new (malloc(sizeof(Rep)+length+1)) Rep(length,string.c_str()); // Allocate space for the representation header, the string's characters, and the terminating NUL
		}
	String(const String& source) // Copy constructor
		:rep(source.rep)
		{
		/* Add another reference to the source's representation: */
		ref();
		}
	~String(void) // Destroys this string
		{
		unref();
		}
	
	/* Methods: */
	String& operator=(const char* string) // Assignment with C string
		{
		/* Unreference the current representation: */
		unref();
		
		/* Check if the C string is valid: */
		if(string!=0)
			{
			/* Allocate a representation for the C string: */
			size_t length=strlen(string);
			rep=new (malloc(sizeof(Rep)+length+1)) Rep(length,string); // Allocate space for the representation header, the string's characters, and the terminating NUL
			}
		else
			rep=0;
		
		return *this;
		}
	String& operator=(const std::string& string) // Assignment with C++ string
		{
		/* Unreference the current representation: */
		unref();
		
		/* Allocate a representation for the C++ string: */
		size_t length=string.size();
		rep=new (malloc(sizeof(Rep)+length+1)) Rep(length,string.c_str()); // Allocate space for the representation header, the string's characters, and the terminating NUL
		
		return *this;
		}
	String& operator=(const String& source) // Copy assignment operator
		{
		if(rep!=source.rep)
			{
			/* Unreference the current representation: */
			unref();
			
			/* Attach to and add another reference to the source's representation: */
			rep=source.rep;
			ref();
			}
		
		return *this;
		}
	bool operator!=(const String& other) const // Inequality operator
		{
		if(rep!=0&&other.rep!=0)
			{
			return rep->length!=other.rep->length||memcmp(rep+1,other.rep+1,rep->length)!=0;
			}
		else
			return rep!=0||other.rep!=0;
		}
	bool valid(void) const // Returns true if this string has a representation
		{
		return rep!=0;
		}
	bool empty(void) const // Returns true if this string is invalid or empty
		{
		return rep==0||rep->length==0;
		}
	size_t length(void) const // Returns the length of a valid string, or 0 for an invalid string
		{
		return rep!=0?rep->length:0;
		}
	const char* c_str(void) const // Returns the string as a C string, or null for an invalid string
		{
		return rep!=0?reinterpret_cast<const char*>(rep+1):0;
		}
	static size_t hash(const String& source,size_t tableSize)
		{
		size_t result=0;
		if(source.rep!=0)
			{
			for(const char* sPtr=reinterpret_cast<const char*>(source.rep+1);*sPtr!='\0';++sPtr)
				result=result*37+size_t(*sPtr);
			}
		
		return result%tableSize;
		}
	};

}

#endif
