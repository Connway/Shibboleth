/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#pragma once

#include <Gaff_Vector.h>
#include <Gaff_String.h>

#ifdef PLATFORM_WINDOWS
	#include "Gaff_FileWatcher_Windows.h"
#elif defined(PLATFORM_LINUX)
	#include "Gaff_FileWatcher_Linux.h"
#elif defined(PLATFORM_MAC)
	#include "Gaff_FileWatcher_Mac.h"
#else
	#error Platform not supported
#endif

NS_GAFF

template <class Allocator>
class FileWatcherManager final
{
public:
	using Callback = void (*)(const char* path);

	FileWatcherManager(Allocator allocator = Allocator());

	bool addWatch(const char* path, Flags<FileWatcher::NotifyChangeFlag> flags, Callback callback);
	bool removeWatch(const char* path, Callback callback);
	bool removeWatch(Callback callback);
	bool removeWatch(const char* path);

	void update(void);

private:
	struct Entry final
	{
		Vector<Callback, Allocator> callbacks;
		U8String<Allocator> path;
		FileWatcher watcher;
	};

	Vector<Entry, Allocator> _watches;
	Allocator _allocator;
};

#include "Gaff_FileWatcher.inl"

NS_END
