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

#include "InputStream.h"

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include "Error.h"

namespace Lambda {

/****************************
Methods of class InputStream:
****************************/

void InputStream::fillBuffer(void)
	{
	/* Mark the buffer as empty: */
	bufferEnd=buffer;
	bufPtr=buffer;
	
	/* Do a quick check to see if there is more data available on the input file: */
	struct pollfd check;
	memset(&check,0,sizeof(struct pollfd));
	check.fd=fd;
	check.events=POLLIN;
	poll(&check,1,0);
	
	/* Check if we are about to block and there is a client who cares about that: */
	if((check.revents&POLLIN)==0x0&&signalFd>=0)
		{
		/* Tell the client that we are going to block: */
		Thing* signal=0; // Sending a null pointer means there's nothing to send
		if(write(signalFd,&signal,sizeof(Thing*))!=sizeof(Thing*))
			throw Error("Cannot signal blocking");
		}
	
	/* Read data and block until something is available: */
	bool notAtEof=true;
	while(bufferEnd==buffer&&notAtEof)
		{
		ssize_t readResult=read(fd,buffer,bufferSize);
		if(readResult>0)
			{
			/* Mark the buffer as filled: */
			bufferEnd=buffer+readResult;
			}
		else if(readResult==0)
			{
			/* The input file has been read completely: */
			notAtEof=false;
			}
		else if(errno!=EAGAIN&&errno!=EWOULDBLOCK&&errno!=EINTR)
			throw Error("Cannot read from an input stream");
		}
	
	/* Read the first character from the new buffer: */
	lastChar=notAtEof?*(bufPtr++):-1;
	}

void InputStream::resetCharacterClasses(void)
	{
	cc[-1]=NONE; // EOF is nothing; nothing, I tell you!
	cc[0]=WHITESPACE; // NUL is always whitespace
	
	/* Set up the basic character classes: */
	for(int i=1;i<256;++i)
		{
		cc[i]=NONE;
		if(isspace(i))
			cc[i]|=WHITESPACE;
		else
			cc[i]|=STRING; // Everything that's not a space can be a string for now
		if(isdigit(i))
			cc[i]|=DIGIT;
		cc[i]|=QUOTEDSTRING; // Pretty much everything is a quoted string
		}
	
	/* Set string terminators: */
	cc['\n']&=~QUOTEDSTRING; // Newlines terminate quoted strings
	
	/* Set syntactic delimiters: */
	cc['(']&=~STRING;
	cc[')']&=~STRING;
	
	/* Set the escape character: */
	escapeChar='\\';
	}

void InputStream::init(void)
	{
	/* Initialize the character classes: */
	cc=characterClasses+1; // Go one up so that EOF (-1) has an entry in the cc table
	resetCharacterClasses();
	
	/* Allocate the input buffer: */
	bufferSize=8192;
	buffer=new char[bufferSize];
	lastChar=0;
	
	/* We have to catch exceptions here in order to release the input buffer should some problem occur: */
	try
		{
		/* Fill the input buffer: */
		fillBuffer();
		}
	catch(...)
		{
		/* Release the input buffer and re-throw the exception: */
		delete[] buffer;
		throw;
		}
	}

InputStream::InputStream(int sFd)
	:fd(sFd),buffer(0),signalFd(-1)
	{
	/* Initialize the stream: */
	init();
	}

InputStream::InputStream(const char* fileName)
	:fd(open(fileName,O_RDONLY)),buffer(0),signalFd(-1)
	{
	/* Check if the input file was opened successfully: */
	if(fd<0)
		throw makeError("Cannot open input file \"%s\"",fileName);
	
	/* Initialize the stream: */
	init();
	}

InputStream::~InputStream(void)
	{
	/* Release the input buffer: */
	delete[] buffer;
	
	/* Close the input file: */
	close(fd);
	}

void InputStream::setSignalFd(int newSignalFd)
	{
	signalFd=newSignalFd;
	}

void InputStream::signalResult(Thing& result)
	{
	/* Bail out if noone's listening: */
	if(signalFd<0)
		return;
	
	/* Grab another reference to the thing and send its pointer on the signal file descriptor: */
	result.ref();
	Thing* thingPtr=&result;
	if(write(signalFd,&thingPtr,sizeof(Thing*))!=sizeof(Thing*))
		throw Error("Cannot signal an evaluation result");
	}

}
