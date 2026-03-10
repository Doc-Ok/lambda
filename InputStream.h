/***********************************************************************
InputStream - Class providing an input stream with one-character read-
ahead for expression parsers.
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

#ifndef INPUTSTREAM_INCLUDED
#define INPUTSTREAM_INCLUDED

#include <stddef.h>

#include "Thing.h"

namespace Lambda {

class InputStream
	{
	/* Embedded classes: */
	private:
	enum CharacterClasses // Enumerated type for character class bit masks to speed up tokenization
		{
		NONE=0x0,
		WHITESPACE=0x1, // Class for characters skipped by skipWs()
		PUNCTUATION=0x2, // User-definable class for syntactic punctuation, i.e., single-character tokens
		DIGIT=0x4, // Class for characters allowed in numbers
		STRING=0x8, // Class for characters allowed in unquoted strings
		QUOTE=0x10, // Class for characters that start / end quoted strings
		QUOTEDSTRING=0x20 // Class for characters allowed in quoted strings
		};
	
	/* Elements: */
	unsigned char characterClasses[257]; // Array of character type bit flags for quicker classification, with extra space for EOF
	unsigned char* cc; // Pointer into character classes array to account for EOF==-1
	int fd; // File descriptor from which the input stream will read
	size_t bufferSize; // Size of the stream's read buffer
	char* buffer; // The stream's read buffer
	char* bufferEnd; // Pointer to the end of the available data in the read buffer
	char* bufPtr; // Pointer to the next unread character in the input buffer
	int escapeChar; // Escape character for quoted and non-quoted strings; -1 if escape sequences should be ignored
	int lastChar; // Last character read from the input file; -1 at end-of-file
	bool parsing; // Flag if a parser reading from this input stream is currently in the middle of parsing
	int signalFd; // Optional file descriptor on which to send notifications about stream state to a client
	
	/* Private methods: */
	void fillBuffer(void); // Reads more data from the input file
	void resetCharacterClasses(void); // Resets the character classification table to the default for the Lambda Programming Language
	void init(void); // Initializes an input stream during construction
	
	/* Constructors and destructors: */
	public:
	InputStream(int sFd); // Attaches an input stream to the given file descriptor
	InputStream(const char* fileName); // Attaches an input stream to a file of the given name
	~InputStream(void); // Destroys the input stream
	
	/* Methods: */
	void setSignalFd(int newSignalFd); // Sets a file descriptor on which to signal input stream and parser state
	void setParsing(bool newParsing); // Enters or leaves parsing mode
	void signalResult(Thing& result); // Lets a parser transmit an evaluation result to someone listening on the signal file descriptor
	bool eof(void) const // Returns true if the input stream has been exhausted
		{
		return lastChar<0;
		}
	bool token(void) const // Returns true if the next character is part of a token
		{
		return (cc[lastChar]&STRING)!=0x0U;
		}
	bool digit(void) const // Returns true if the next character is a digit
		{
		return (cc[lastChar]&DIGIT)!=0x0U;
		}
	int peek(void) const // Returns the next character without removing it from the input stream
		{
		/* Return the next character: */
		return lastChar;
		}
	int get(void) // Returns the next character and removes it from the input stream
		{
		/* Return the next character: */
		int result=lastChar;
		
		/* Read the next next character from the input buffer or input file: */
		if(bufPtr!=bufferEnd)
			lastChar=*(bufPtr++);
		else
			fillBuffer();
		
		return result;
		}
	void skipWs(void) // Skips whitespace in the input stream
		{
		/* Skip the next character while it is whitespace: */
		while((cc[lastChar]&WHITESPACE)!=0x0U)
			get();
		}
	int getAndSkipWs(void) // Returns the next character, removes it from the input stream, then skips whitespace in the input stream
		{
		/* Return the next character and read the next next character: */
		int result=get();
		
		/* Skip the next character while it is whitespace: */
		while((cc[lastChar]&WHITESPACE)!=0x0U)
			get();
		
		return result;
		}
	};

}

#endif
