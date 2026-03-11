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

#ifndef TURTLE_INCLUDED
#define TURTLE_INCLUDED

#include <poll.h>
#include <vector>
#include <Misc/Pipe.h>
#include <Threads/Thread.h>

#include "Geometry.h"

/* Forward declarations: */
namespace Misc {
class CallbackData;
}
class GLWindow;
namespace Lambda {
class PolylineRenderer;
}

namespace Lambda {

class Turtle
	{
	/* Embedded classes: */
	public:
	enum CommandType // Enumerated type for turtle commands
		{
		Forward,Back, // Take a "step" parameter in turtle units (the turtle is one unit across)
		Left,Right, // Take an "angle" parameter in degrees
		PenUp,PenDown, // Raises or lowers the turtle's pen
		Reset, // Resets the turtle, i.e., clears the display and moves the turtle back to the original position and lowers the pen
		NumCommands
		};
	
	private:
	struct PipeCommand; // Structure to send commands to a turtle via its command pipe
	struct CachedPolyline; // Structure representing a polyline that is cached on the GPU
	typedef std::vector<CachedPolyline> PolylineSet; // Type for sets of polylines that are cached on the GPU
	
	/* Elements: */
	Misc::Pipe commandPipe; // A pipe to send commands to the turtle
	unsigned int nextSequenceNumber; // The sequence number to assign to the next turtle command
	unsigned int completedSequenceNumber; // The sequence number of the most recently completed turtle command
	Misc::Pipe statusPipe; // A pipe to receive status updates from the turtle
	bool runControlThread; // Flag to keep the control thread running
	Threads::Thread controlThread; // Background thread controlling the turtle and its display
	GLWindow* window; // The turtle's display window
	PolylineRenderer* polylineRenderer; // A renderer for high-quality anti-aliased polylines
	struct pollfd pollRequests[2]; // Array of poll request structures for the display window's X connection and the command pipe
	bool mustRedraw; // Flag whether the display window must be redrawn
	bool detached; // Flag whether the turtle has been detached from its controller
	bool windowClosed; // Flag whether the turtle's display window has been closed
	double x,y,angleDeg,angleRad; // Current turtle position and orientation
	bool pen; // Current pen down state
	Polyline turtle; // Polyline defining the turtle's graphical representation
	PolylineSet polylines; // The set of polylines drawn by the turtle
	double transX,transY,scale; // The current display transformation
	bool dragging; // Flag if the display screen is currently being dragged
	int dragX,dragY; // Window position at which the dragging operation started
	
	/* Private methods: */
	void resetTurtle(void);
	void windowRectChangedCallback(Misc::CallbackData* cbData); // Called when the user moves or resizes the display window
	void windowClosedCallback(Misc::CallbackData* cbData); // Called when the user closes the display window
	void redrawDisplay(void); // Redraws the turtle's display window
	void handleXEvents(int numEventsInQueue); // Handles all queued-up X events on the display window
	void executeCommand(const PipeCommand& command); // Executes the given turtle command
	void init(void); // Initializes the turtle after construction
	void closeWindow(void); // Closes the turtle's display window
	void* controlThreadMethod(void); // The method controlling the turtle and its display in a background thread
	
	/* Constructors and destructors: */
	public:
	Turtle(void); // Creates a turtle on the default X display
	~Turtle(void); // Destroys the turtle and its display
	
	/* Methods: */
	unsigned int sendCommand(int command,double parameter =0.0); // Sends a command to the turtle; returns the command's sequence number
	void waitForCompletion(unsigned int sequenceNumber); // Blocks until the turtle command with the given sequence number has been completed
	void detach(void); // Detaches the turtle from its controller; display window will stay up until closed
	};

}

#endif
