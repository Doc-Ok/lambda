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

#include "Parser.h"

#include <math.h>
#include <string>

#include "Error.h"
#include "Null.h"
#include "Cons.h"
#include "Quote.h"
#include "Boolean.h"
#include "Name.h"
#include "Integer.h"
#include "FloatingPoint.h"
#include "InputStream.h"

namespace Lambda {

namespace {

/****************
Helper functions:
****************/

ThingPtr parseCons(InputStream& is) // Function to parse a Cons expression
	{
	/* Check if the next character is end-of-file or already a closing parenthesis: */
	if(is.eof())
		throw Error("End-of-file while parsing Cons expression");
	else if(is.peek()==')')
		{
		/* Return an empty list: */
		return new Null;
		}
	
	/* Parse the Cons's left child: */
	ThingPtr left=parse(is);
	
	/* Check if the next character is a cons token: */
	is.skipWs();
	if(is.peek()=='.')
		{
		/* Parse the Cons's right child: */
		is.getAndSkipWs();
		ThingPtr right=parse(is);
		
		return new Cons(*left,*right);
		}
	else if(!is.eof())
		{
		/* Parse another Cons and make a list: */
		ThingPtr right=parseCons(is);
		
		return new Cons(*left,*right);
		}
	else
		throw Error("End-of-file while parsing Cons expression");
	}

}

ThingPtr parse(InputStream& is)
	{
	ThingPtr result;
	
	/* Let's see what's up: */
	if(is.peek()=='(')
		{
		/* Parse a Cons: */
		is.getAndSkipWs();
		result=parseCons(is);
		
		/* Check for the closing parenthesis: */
		is.skipWs();
		if(is.peek()!=')')
			throw Error("Missing ')' while parsing Cons expression");
		is.get();
		}
	else if(is.peek()=='\'')
		{
		/* Parse a Quote: */
		is.getAndSkipWs();
		result=new Quote(*parse(is));
		}
	else if(is.token())
		{
		/* Parse a name, or maybe an integer or a floating-point: */
		static char nameBuf[128];
		char* namePtr=nameBuf;
		char* nameEnd=nameBuf+sizeof(nameBuf)-2;
		bool first=true;
		bool maybeInteger=true;
		bool maybeFloatingPoint=true;
		bool negate=false;
		long integer=0;
		double floatingPoint=0.0;
		double floatingPointBase=1.0;
		long periodDigits=0;
		bool haveDigits=false;
		bool havePeriod=false;
		bool haveExponent=false;
		bool exponentFirst=true;
		bool negateExponent=false;
		long exponent=0;
		bool haveExponentDigits=false;
		
		/* Handle the token's characters: */
		do
			{
			/* Get the next character and append it to the name if there is room: */
			int next=is.get();
			if(namePtr!=nameEnd)
				*(namePtr++)=char(next);
			
			/* Accumulate a potential integer: */
			if(maybeInteger)
				{
				if(next=='+'||next=='-')
					{
					maybeInteger=first;
					negate=next=='-';
					}
				else if(next>='0'&&next<='9')
					{
					integer=integer*10L+long(next-'0');
					haveDigits=true;
					}
				else
					maybeInteger=false;
				}
			
			/* Accumulate a potential floating-point: */
			if(maybeFloatingPoint)
				{
				if(haveExponent)
					{
					if(next=='+'||next=='-')
						{
						maybeFloatingPoint=exponentFirst;
						negateExponent=next=='-';
						}
					else if(next>='0'&&next<='9')
						{
						exponent=exponent*10L+long(next-'0');
						haveExponentDigits=true;
						}
					else
						maybeFloatingPoint=false;
					
					exponentFirst=false;
					}
				else if(havePeriod)
					{
					if(next>='0'&&next<='9')
						{
						floatingPoint=floatingPoint*10.0+double(next-'0');
						floatingPointBase*=10.0;
						++periodDigits;
						haveDigits=true;
						}
					else if(next=='e'||next=='E')
						haveExponent=true;
					else
						maybeFloatingPoint=false;
					}
				else
					{
					if(next=='+'||next=='-')
						{
						maybeFloatingPoint=first;
						negate=next=='-';
						}
					else if(next>='0'&&next<='9')
						{
						floatingPoint=floatingPoint*10.0+double(next-'0');
						haveDigits=true;
						}
					else if(next=='.')
						havePeriod=true;
					else if(next=='e'||next=='E')
						haveExponent=true;
					else
						maybeFloatingPoint=false;
					}
				}
			
			first=false;
			}
		while(is.token());
		
		/* Return an integer, or a floating-point, or a name: */
		if(maybeInteger&&haveDigits)
			{
			/* Apply any modifiers to the final integer value: */
			if(negate)
				integer=-integer;
			
			result=new Integer(integer);
			}
		else if(maybeFloatingPoint&&haveDigits&&(!haveExponent||haveExponentDigits))
			{
			/* Apply any modifiers to the final floating-point value: */
			if(haveExponent)
				{
				if(negateExponent)
					exponent=-exponent;
				exponent-=periodDigits;
				if(exponent!=0)
					floatingPoint*=pow(10.0,double(exponent));
				}
			else if(havePeriod)
				floatingPoint/=floatingPointBase;
			if(negate)
				floatingPoint=-floatingPoint;
			result=new FloatingPoint(floatingPoint);
			}
		else
			{
			*namePtr='\0';
			result=new Name(nameBuf);
			}
		}
	else if(!is.eof())
		throw makeError("Unexpected '%c' while parsing",is.get());
	
	return result;
	}

}
