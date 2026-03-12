/***********************************************************************
FileLoader - Class to load and watch input files for the Lambda
Programming language.
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

#ifndef FILELOADER_INCLUDED
#define FILELOADER_INCLUDED

#include <IO/Directory.h>
#include <IO/FileMonitor.h>

/* Forward declarations: */
namespace Lambda {
class Context;
}

namespace Lambda {

class FileLoader
	{
	/* Elements: */
	private:
	Context& rootContext; // The root evaluation context; only files loaded in this context are monitored for changes
	IO::DirectoryPtr currentDirectory; // The current directory at the time the file loader was created
	IO::FileMonitor fileMonitor; // A monitor to keep track of changes to loaded files
	
	/* Private methods: */
	void load(const std::string& path,Context& context); // Loads, parses, and evaluates a Lambda Programming Language file in the given evaluation context
	std::string normalizeFileName(const char* fileName); // Returns the normalized absolute path to a Lambda Programming Language file of the given base name
	void reloadCallback(IO::FileMonitor::Event& event); // Callback called when a previously loaded file is changed asynchronously
	
	/* Constructors and destructors: */
	public:
	FileLoader(Context& sRootContext); // Creates a file loader for the current directory and the given root evaluation context
	
	/* Methods: */
	void block(int blockFd); // Blocks until data becomes available of the given file descriptor; reloads changed files in the meantime
	void loadFile(const char* fileName); // Loads the file of the given absolute or relative path in the root context
	void loadFile(const char* fileName,Context& context); // Loads the file of the given absolute or relative path in the given context, due to an evaluation of Builtin::Load
	};

}

#endif
