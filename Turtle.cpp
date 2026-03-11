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
#include "PolylineRenderer.h"

// DEBUGGING
#include <iostream>

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

/********************************************
Declaration of struct Turtle::CachedPolyline:
********************************************/

struct Turtle::CachedPolyline
	{
	/* Elements: */
	public:
	Color color; // Polyline's color
	Scalar lineWidth; // Polyline's line width
	Polyline polyline; // The current version of the polyline
	unsigned int version; // The version number of the current polyline
	
	/* Constructors and destructors: */
	CachedPolyline(const Point& start) // Creates a polyline containing a single vertex
		:color(0,192,0),lineWidth(1.5),
		 version(1)
		{
		polyline.push_back(start);
		}
	};

/***********************
Methods of class Turtle:
***********************/

void Turtle::resetTurtle(void)
	{
	/* Clear the display: */
	polylines.clear();
	if(window!=0&&polylineRenderer!=0)
		; // polylineRenderer->clearCache(window->getContextData());
	
	/* Move the turtle back to its initial state: */
	x=0.0;
	y=0.0;
	angleDeg=0.0;
	angleRad=0.0;
	pen=true;
	
	/* Start the first polyline: */
	polylines.push_back(CachedPolyline(Point(x,y)));
	
	if(window!=0)
		{
		/* Initialize the display transformation: */
		transX=double(window->getWindowWidth())*0.5;
		transY=double(window->getWindowHeight())*0.5;
		scale=32.0;
		dragging=false;
		}
	}

void Turtle::windowRectChangedCallback(Misc::CallbackData* cbData)
	{
	GLWindow::RectChangedCallbackData* myCbData=static_cast<GLWindow::RectChangedCallbackData*>(cbData);
	if(myCbData->isSizeChanged())
		{
		/* Update the current display transformation to keep the display center in the center: */
		transX+=(double(myCbData->newRect.size[0])-double(myCbData->oldRect.size[0]))*0.5;
		transY+=(double(myCbData->newRect.size[1])-double(myCbData->oldRect.size[1]))*0.5;
		}
	
	/* We must redraw the display window: */
	mustRedraw=true;
	}

void Turtle::windowClosedCallback(Misc::CallbackData*)
	{
	/* Mark the window as closed: */
	windowClosed=true;
	}

void Turtle::redrawDisplay(void)
	{
	/* Set up the viewport and clear the frame buffer: */
	const GLWindow::Size& size=window->getWindowSize();
	glViewport(0,0,size[0],size[1]);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	/* Enable alpha blending: */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	/* Activate the polyline renderer: */
	GLObject::DataItem* polylineRendererDataItem=polylineRenderer->activate(Scalar(1.25/scale),window->getContextData());
	polylineRenderer->setScaleFactor(Scalar(1.0/scale));
	
	/* Set up OpenGL matrices: */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,GLdouble(size[0]),0.0,GLdouble(size[1]),-0.5,0.5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(transX,transY,0.0);
	glScaled(scale,scale,scale);
	
	/* Draw all polylines: */
	for(PolylineSet::iterator psIt=polylines.begin();psIt!=polylines.end();++psIt)
		polylineRenderer->draw(&*psIt,psIt->version,psIt->polyline,psIt->color,psIt->lineWidth,polylineRendererDataItem);
	
	/* Draw the turtle: */
	glPushMatrix();
	glTranslated(x,y,0.0);
	glRotated(angleDeg,0.0,0.0,1.0);
	
	polylineRenderer->draw(&turtle,1,turtle,Color(255,255,255),Scalar(2.5),polylineRendererDataItem);
	
	glPopMatrix();
	
	/* Deactivate the polyline renderer: */
	polylineRenderer->deactivate(polylineRendererDataItem);
	
	/* Disable alpha blending: */
	glDisable(GL_BLEND);
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
			case ButtonPress:
				if(event.xbutton.button==Button1)
					{
					/* Start dragging: */
					dragging=true;
					dragX=event.xbutton.x;
					dragY=event.xbutton.y;
					}
				else if(event.xbutton.button==4||event.xbutton.button==5)
					{
					/* Zoom in around the mouse position: */
					double scalep=event.xbutton.button==4?scale*1.25:scale/1.25;
					transX=(double(event.xbutton.x)*(scale-scalep)+transX*scalep)/scale;
					transY=(double((window->getWindowHeight()-1)-event.xbutton.y)*(scale-scalep)+transY*scalep)/scale;
					scale=scalep;
					
					/* We must redraw the display window: */
					mustRedraw=true;
					}
				break;
			
			case ButtonRelease:
				if(event.xbutton.button==Button1)
					{
					/* Stop dragging: */
					dragging=false;
					}
				break;
			
			case MotionNotify:
				if(dragging)
					{
					transX+=double(event.xmotion.x-dragX);
					dragX=event.xmotion.x;
					transY-=double(event.xmotion.y-dragY);
					dragY=event.xmotion.y;
					
					/* We must redraw the display window: */
					mustRedraw=true;
					}
				break;
			
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
				{
				polylines.back().polyline.push_back(Point(x,y));
				++polylines.back().version;
				}
			
			/* We must update the display: */
			mustRedraw=true;
			
			break;
		
		case Back:
			/* Move forward back the current turtle angle: */
			x+=command.parameter*sin(angleRad);
			y-=command.parameter*cos(angleRad);
			
			/* Add to the current polyline if the pen is down: */
			if(pen)
			if(pen)
				{
				polylines.back().polyline.push_back(Point(x,y));
				++polylines.back().version;
				}
			
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
				polylines.push_back(CachedPolyline(Point(x,y)));
			
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
			/* Shut down: */
			runControlThread=false;
			break;
		}
	}

void Turtle::init(void)
	{
	/* Open the display window: */
	GLContext::Properties contextProperties;
	contextProperties.depthBufferSize=0; // We neither need nor want a depth buffer
	contextProperties.nonlinear=true; // Select linear rendering for best anti-aliasing quality
	window=new GLWindow("Lambda Turtle",GLWindow::Size(1024,768),true,contextProperties);
	
	/* Register the close callback: */
	window->getRectChangedCallbacks().add(this,&Turtle::windowRectChangedCallback);
	window->getCloseCallbacks().add(this,&Turtle::windowClosedCallback);
	
	/* Create a polyline renderer and associate it with the display window: */
	polylineRenderer=new PolylineRenderer;
	polylineRenderer->initContext(window->getContextData());
	
	/* Create a polyline to render the turtle: */
	turtle.push_back(Point(0.0,0.667));
	turtle.push_back(Point(-0.25,-0.333));
	turtle.push_back(Point(0.25,-0.333));
	turtle.push_back(Point(0.0,0.667));
	
	/* Set up poll request structures to handle X events on the display window and turtle commands: */
	memset(pollRequests,0,2*sizeof(struct pollfd));
	
	pollRequests[0].fd=window->getConnectionNumber();
	pollRequests[0].events=POLLIN;
	
	pollRequests[1].fd=commandPipe.getReadFd();
	pollRequests[1].events=POLLIN;
	
	/* Initialize the turtle state: */
	resetTurtle();
	}

void Turtle::closeWindow(void)
	{
	/* Destroy the polyline renderer: */
	delete polylineRenderer;
	polylineRenderer=0;
	
	/* Close the display window: */
	delete window;
	window=0;
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
	while(runControlThread)
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
			{
			/* Handle all queued-up X events: */
			handleXEvents(XEventsQueued(dpy,QueuedAfterReading));
			
			/* Check if the window was closed: */
			if(windowClosed)
				{
				// DEBUGGING
				std::cout<<"Window has been closed"<<std::endl;
				
				/* Close the window: */
				closeWindow();
				
				/* Remove the window's X connection from the poll request structures: */
				pollRequests[0].fd=-1;
				pollRequests[0].revents=0x0;
				
				/* Stop the thread if the turtle is already detached: */
				runControlThread=runControlThread&&!detached;
				}
			}
		
		/* Handle all turtle commands from the command pipe: */
		if(pollRequests[1].revents!=0x0)
			{
			/* Read a batch of commands and execute them: */
			PipeCommand commands[512/sizeof(PipeCommand)];
			size_t numCommands=commandPipe.read(commands,sizeof(commands))/sizeof(PipeCommand);
			for(size_t i=0;i<numCommands;++i)
				{
				/* Execute the command: */
				executeCommand(commands[i]);
				
				/* Send a completion notification to the main thread: */
				// statusPipe.write(commands[i].sequenceNumber);
				}
			
			/* Check for end-of-file on the command pipe: */
			if(commandPipe.eof())
				{
				/* The turtle is now detached: */
				detached=true;
				
				// DEBUGGING
				std::cout<<"Turtle has been detached"<<std::endl;
				
				/* Remove the command pipe from the poll request structures: */
				pollRequests[1].fd=-1;
				pollRequests[1].revents=0x0;
				
				/* Stop the thread if the display window is already closed: */
				runControlThread=runControlThread&&!windowClosed;
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
	closeWindow();
	
	// DEBUGGING
	std::cout<<"Shutting down turtle"<<std::endl;
	
	return 0;
	}

Turtle::Turtle(void)
	:nextSequenceNumber(1),completedSequenceNumber(0),
	 window(0),polylineRenderer(0),
	 detached(false),windowClosed(false)
	{
	/* Start the control thread: */
	runControlThread=true;
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
	
	// DEBUGGING
	std::cout<<"Turtle destroyed"<<std::endl;
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
	// DEBUGGING
	std::cout<<"Detaching turtle"<<std::endl;
	
	/* Check if the turtle's display window has already been closed: */
	if(windowClosed)
		{
		/* Destroy the turtle: */
		delete this;
		}
	else
		{
		/* Close the write end of the command pipe and the read end of the status pipe: */
		commandPipe.closeWrite();
		statusPipe.closeRead();
		}
	}

}
