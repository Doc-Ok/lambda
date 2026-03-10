/***********************************************************************
Turtle - Class representing drawing turtles and their display windows.
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

#include "Turtle.h"

#include <string.h>
#include <math.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <Misc/MessageLogger.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/GLContext.h>
#include <GL/GLWindow.h>

#include "Error.h"

namespace Lambda {

/*****************************************
Declaration of struct Turtle::PipeCommand:
*****************************************/

struct Turtle::PipeCommand
	{
	/* Elements: */
	public:
	int command; // The command
	unsigned int sequenceNumber; // The command sequence number
	double parameter; // The command's parameter
	};

/***********************************
Declaration of struct Turtle::Point:
***********************************/

struct Turtle::Point
	{
	/* Elements: */
	public:
	double x,y; // The point's x and y coordinates
	
	/* Constructors and destructors: */
	Point(double sX,double sY) // Constructs a point from the given coordinates
		:x(sX),y(sY)
		{
		}
	};

/***********************
Methods of class Turtle:
***********************/

void Turtle::resetTurtle(void)
	{
	/* Clear the display: */
	polylines.clear();
	
	/* Move the turtle back to its initial state: */
	x=0.0;
	y=0.0;
	angleDeg=0.0;
	angleRad=0.0;
	pen=true;
	
	/* Start the first polyline: */
	polylines.push_back(Polyline());
	polylines.back().push_back(Point(x,y));
	}

void Turtle::windowRectChangedCallback(Misc::CallbackData* cbData)
	{
	/* We must redraw the display window: */
	mustRedraw=true;
	}

void Turtle::windowClosedCallback(Misc::CallbackData*)
	{
	/* Destroy the window: */
	delete window;
	window=0;
	
	/* Remove the window's X connection from the poll request structures: */
	pollRequests[0].fd=-1;
	}

void Turtle::redrawDisplay(void)
	{
	/* Set up the viewport and clear the frame buffer: */
	const GLWindow::Size& size=window->getWindowSize();
	glViewport(0,0,size[0],size[1]);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	/* Set up OpenGL matrices: */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,GLdouble(size[0]),0.0,GLdouble(size[1]),-0.5,0.5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.5*GLdouble(size[0]),0.5*GLdouble(size[1]),0.0);
	glScaled(32.0,32.0,32.0);
	
	/* Draw the turtle: */
	glPushMatrix();
	glTranslated(x,y,0.0);
	glRotated(angleDeg,0.0,0.0,1.0);
	
	glLineWidth(1.0f);
	glColor3f(0.0f,0.5f,0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2d(0.0,0.667);
	glVertex2d(-0.25,-0.333);
	glVertex2d(0.25,-0.333);
	glEnd();
	
	glPopMatrix();
	
	/* Draw all polylines: */
	for(PolylineSet::iterator psIt=polylines.begin();psIt!=polylines.end();++psIt)
		{
		if(psIt->size()>1)
			{
			/* Draw the polyline: */
			glBegin(GL_LINE_STRIP);
			for(Polyline::iterator pIt=psIt->begin();pIt!=psIt->end();++pIt)
				glVertex2d(pIt->x,pIt->y);
			glEnd();
			}
		else
			{
			/* Draw the polyline as a point: */
			glBegin(GL_POINTS);
			glVertex2d(psIt->front().x,psIt->front().y);
			glEnd();
			}
		}
	}

void Turtle::handleXEvents(int numEventsInQueue)
	{
	for(int i=0;window!=0&&i<numEventsInQueue;++i)
		{
		/* Retrieve the next event (this is guaranteed not to block): */
		XEvent event;
		window->nextEvent(event);
		
		/* Handle it: */
		switch(event.type)
			{
			case Expose:
				/* We must redraw the display window: */
				mustRedraw=true;
				
				break;
			
			default:
				/* Send the event to the window to handle: */
				window->processEvent(event);
			}
		}
	}

void Turtle::executeCommand(const Turtle::PipeCommand& command)
	{
	switch(command.command)
		{
		case Forward:
			/* Move forward along the current turtle angle: */
			x-=command.parameter*sin(angleRad);
			y+=command.parameter*cos(angleRad);
			
			/* Add to the current polyline if the pen is down: */
			if(pen)
				polylines.back().push_back(Point(x,y));
			
			/* We must update the display: */
			mustRedraw=true;
			
			break;
		
		case Back:
			/* Move forward back the current turtle angle: */
			x+=command.parameter*sin(angleRad);
			y-=command.parameter*cos(angleRad);
			
			/* Add to the current polyline if the pen is down: */
			if(pen)
				polylines.back().push_back(Point(x,y));
			
			/* We must update the display: */
			mustRedraw=true;
			
			break;
		
		case Left:
			/* Increment the turtle's angle in degrees: */
			angleDeg+=command.parameter;
			
			/* Wrap the new angle to the [-180, 180) interval: */
			while(angleDeg<-180.0)
				angleDeg+=360.0;
			while(angleDeg>=180.0)
				angleDeg-=360.0;
			
			/* Convert the angle from degrees to radians: */
			angleRad=angleDeg*M_PI/180.0;
			
			/* We must update the display: */
			mustRedraw=true;
			
			break;
		
		case Right:
			/* Decrement the turtle's angle in degrees: */
			angleDeg-=command.parameter;
			
			/* Wrap the new angle to the [-180, 180) interval: */
			while(angleDeg<-180.0)
				angleDeg+=360.0;
			while(angleDeg>=180.0)
				angleDeg-=360.0;
			
			/* Convert the angle from degrees to radians: */
			angleRad=angleDeg*M_PI/180.0;
			
			/* We must update the display: */
			mustRedraw=true;
			
			break;
		
		case PenUp:
			/* Lift the pen: */
			pen=false;
			
			break;
		
		case PenDown:
			/* Start a new polyline if the pen was up: */
			if(!pen)
				{
				polylines.push_back(Polyline());
				polylines.back().push_back(Point(x,y));
				}
			
			/* Lower the pen: */
			pen=true;
			
			/* We must update the display: */
			mustRedraw=true;
			
			break;
		
		case Reset:
			/* Reset the turtle: */
			resetTurtle();
			
			/* We must update the display: */
			mustRedraw=true;
			
			break;
		
		case NumCommands:
			// break out...
			break;
		}
	}

void Turtle::init(void)
	{
	/* Open the display window: */
	GLContext::Properties contextProperties;
	contextProperties.depthBufferSize=0; // We neither need nor want a depth buffer
	window=new GLWindow("Lambda Turtle",GLWindow::Size(1024,768),true,contextProperties);
	
	/* Register the close callback: */
	window->getCloseCallbacks().add(this,&Turtle::windowClosedCallback);
	
	/* Set up poll request structures to handle X events on the display window and turtle commands: */
	memset(pollRequests,0,2*sizeof(struct pollfd));
	
	pollRequests[0].fd=window->getConnectionNumber();
	pollRequests[0].events=POLLIN;
	
	pollRequests[1].fd=commandPipe.getReadFd();
	pollRequests[1].events=POLLIN;
	
	/* Initialize the turtle state: */
	resetTurtle();
	}

void Turtle::deinit(void)
	{
	/* Close the display window: */
	delete window;
	}

void* Turtle::controlThreadMethod(void)
	{
	try
		{
		/* Initialize the turtle: */
		init();
		
		/* Signal completion to the main thread: */
		statusPipe.write(nextSequenceNumber);
		}
	catch(const std::runtime_error& err)
		{
		/* Print an error message: */
		Misc::formattedUserError("Error %s while initializing the turtle",err.what());
		
		/* Signal completion or error to the main thread and bail out: */
		statusPipe.write((unsigned int)(0));
		return 0;
		}
	
	/* Cache the display window's display connection: */
	Display* dpy=window->getContext().getDisplay();
	
	/* Loop until shut down: */
	while(true)
		{
		/* Set up the call to poll(): */
		int pollTimeout=-1; // Assume we are going to block forever
		
		/* Don't block if there is a display window and unhandled X events in its X event queue: */
		int numEventsInQueue=window!=0?XQLength(dpy):0;
		
		if(numEventsInQueue>0)
			pollTimeout=0;
		else
			{
			/* Set a time-out for animation, or whatever: */
			// ...
			}
		
		/* Call poll(): */
		poll(pollRequests,2,pollTimeout);
		
		/* Handle all X events that either were in the queue before we polled, or can be read from the display connection after we polled (we can't handle both at the same time): */
		if(numEventsInQueue>0||(pollRequests[0].revents&POLLIN)!=0x0)
			handleXEvents(XEventsQueued(dpy,QueuedAfterReading));
		
		/* Handle all turtle commands from the command pipe: */
		if((pollRequests[1].revents&POLLIN)!=0x0)
			{
			/* Read a batch of commands and execute them: */
			PipeCommand commands[512/sizeof(PipeCommand)];
			size_t numCommands=commandPipe.read(commands,sizeof(commands))/sizeof(PipeCommand);
			for(size_t i=0;i<numCommands;++i)
				{
				/* Execute the command: */
				executeCommand(commands[i]);
				
				/* Send a completion notification to the main thread: */
				statusPipe.write(commands[i].sequenceNumber);
				}
			}
		
		/* Check if the display window must be redrawn: */
		if(window!=0&&mustRedraw)
			{
			/* Redraw the display window: */
			glXWaitX();
			redrawDisplay();
			window->swapBuffers();
			glXWaitGL();
			
			/* Mark the display window as up-to-date: */
			mustRedraw=false;
			}
		}
	
	/* De-initialize the turtle: */
	deinit();
	
	return 0;
	}

Turtle::Turtle(void)
	:nextSequenceNumber(1),completedSequenceNumber(0),
	 window(0),detached(false)
	{
	/* Start the control thread: */
	controlThread.start(this,&Turtle::controlThreadMethod);
	
	/* Wait for confirmation from the control thread: */
	unsigned int status;
	statusPipe.read(status);
	if(status!=nextSequenceNumber)
		throw Error("Cannot establish connection to turtle");
	completedSequenceNumber=nextSequenceNumber++;
	}

Turtle::~Turtle(void)
	{
	/* Tell the control thread to shut down by sending the secret shutdown command: */
	PipeCommand pc;
	pc.command=NumCommands;
	pc.sequenceNumber=nextSequenceNumber++;
	pc.parameter=0.0;
	commandPipe.write(pc);
	
	/* Wait for the control thread to shut down: */
	controlThread.join();
	}

unsigned int Turtle::sendCommand(int command,double parameter)
	{
	/* Check if the command is legal: */
	if(command>=NumCommands)
		throw Error("Invalid turtle command");
	
	/* Create a pipe command structure: */
	PipeCommand pc;
	pc.command=command;
	pc.sequenceNumber=nextSequenceNumber++;
	pc.parameter=parameter;
	
	/* Send the pipe command structure on the command pipe: */
	commandPipe.write(pc);
	
	/* Return the command's sequence number: */
	return pc.sequenceNumber;
	}

void Turtle::waitForCompletion(unsigned int sequenceNumber)
	{
	/* Loop and block until the given sequence number has been completed: */
	while(completedSequenceNumber<sequenceNumber)
		{
		/* Read a batch of completed sequence numbers from the status pipe: */
		unsigned int sequenceNumbers[512/sizeof(unsigned int)];
		size_t numSequenceNumbers=statusPipe.read(sequenceNumbers,sizeof(sequenceNumbers))/sizeof(unsigned int);
		for(size_t i=0;i<numSequenceNumbers;++i)
			if(completedSequenceNumber<sequenceNumbers[i])
				completedSequenceNumber=sequenceNumbers[i];
		}
	}

void Turtle::detach(void)
	{
	/* Close the write end of the command pipe and the read end of the status pipe: */
	commandPipe.closeWrite();
	statusPipe.closeRead();
	}

}
