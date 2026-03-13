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

#include <string>
#include <Misc/Autopointer.h>
#include <Misc/StringHashFunctions.h>
#include <Misc/HashTable.h>
#include <Threads/FunctionCalls.h>
#include <IO/Directory.h>
#include <IO/FileMonitor.h>

/* Forward declarations: */
namespace Lambda {
class Context;
}

namespace Lambda {

class FileLoader
	{
	/* Embedded classes: */
	private:
	class ReloadCallback:public Threads::FunctionCall<IO::FileMonitor::Event&> // Functor class called when an event occurs on a monitored file or directory
		{
		/* Embedded classes: */
		private:
		typedef Misc::HashTable<std::string,bool> MonitoredFileHashTable; // Type for hash tables mapping the names of monitored files to their monitoring states
		
		/* Elements: */
		private:
		Context& context; // The evaluation context in which the monitored file will be reloaded
		const char*& reloadPrompt; // Reference to the file loader's current reload prompt
		MonitoredFileHashTable monitoredFileMap; // Hash table mapping the names of monitored files to their monitoring states
		
		/* Constructors and destructors: */
		public:
		ReloadCallback(Context& sContext,const char*& sReloadPrompt); // Elementwise constructor
		
		/* Methods from class Threads::FunctionCall<IO::FileMonitor::Event& event>: */
		virtual void operator()(IO::FileMonitor::Event& event); // Method called when an event occurs on the monitored file
		
		/* New methods: */
		void monitorFile(const std::string& fileName); // Starts monitoring the file of the given name inside the monitored directory
		};
	
	friend class ReloadCallback;
	
	typedef Misc::Autopointer<ReloadCallback> ReloadCallbackPtr;
	typedef Misc::HashTable<std::string,ReloadCallbackPtr> ReloadHashTable; // Type for hash tables mapping absolute directory paths to the ReloadCallback objects monitoring them
	
	/* Elements: */
	Context& rootContext; // The root evaluation context; only files loaded in this context are monitored for changes
	IO::DirectoryPtr currentDirectory; // The current directory at the time the file loader was created
	IO::FileMonitor fileMonitor; // A monitor to keep track of changes to loaded files
	ReloadHashTable directoryMap; // Hash table mapping absolute directory paths to the ReloadCallback objects monitoring them
	const char* reloadPrompt; // The current reload prompt to be printed after reloading a file
	
	/* Private methods: */
	static void load(const std::string& path,Context& context); // Loads, parses, and evaluates a Lambda Programming Language file in the given evaluation context
	std::string normalizeFileName(const char* fileName) const; // Returns the normalized absolute path to a Lambda Programming Language file of the given base name
	void monitorFile(const std::string& path); // Monitors the file at the given normalized absolute path for changes
	
	/* Constructors and destructors: */
	public:
	FileLoader(Context& sRootContext); // Creates a file loader for the current directory and the given root evaluation context
	
	/* Methods: */
	void block(int blockFd,const char* newReloadPrompt); // Blocks until data becomes available of the given file descriptor; reloads changed files in the meantime, and prints the reload prompt after doing so
	void loadFile(const char* fileName); // Loads the file of the given absolute or relative path in the root context
	void loadFile(const char* fileName,Context& context); // Loads the file of the given absolute or relative path in the given context, due to an evaluation of Builtin::Load
	};

}

#endif
