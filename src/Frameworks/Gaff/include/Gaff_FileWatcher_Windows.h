/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#if defined(_WIN32) || defined(_WIN64)

#include "Gaff_Flags.h"
#include "Gaff_IncludeWindows.h"

NS_GAFF

class FileWatcher final
{
public:
	enum class NotifyChangeFlag
	{
		FileName = 0,
		DirName,
		Attributes,
		Size,
		LastWrite,
		LastAccess,
		Creation,
		Security = 9,

		Count
	};

	FileWatcher(const char* path, Flags<NotifyChangeFlag> flags);
	FileWatcher(const FileWatcher& watcher) = default;
	FileWatcher(void) = default;
	~FileWatcher(void);

	FileWatcher& operator=(const FileWatcher& rhs) = default;

	const char* processEvents(void);
	bool listen(bool watch_subtree = true);

	bool isValid(void) const;

private:
	char _buffer[256] = { 0 };
	HANDLE _dir_handle = INVALID_HANDLE_VALUE;
	Flags<NotifyChangeFlag> _flags;
	OVERLAPPED _overlapped;
};

NS_END

#endif
