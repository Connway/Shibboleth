/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gaff_Flags.h"
#include "Gaff_IncludeWindows.h"

NS_GAFF

class FileWatcher final
{
public:
	enum class NotifyChangeFlag
	{
		FileName = FILE_NOTIFY_CHANGE_FILE_NAME,
		DirName = FILE_NOTIFY_CHANGE_DIR_NAME,
		Attributes = FILE_NOTIFY_CHANGE_ATTRIBUTES,
		Size = FILE_NOTIFY_CHANGE_SIZE,
		LastWrite = FILE_NOTIFY_CHANGE_LAST_WRITE,
		LastAccess = FILE_NOTIFY_CHANGE_LAST_ACCESS,
		Creation = FILE_NOTIFY_CHANGE_CREATION,
		//Security = FILE_NOTIFY_CHANGE_SECURITY,

		Count
	};

	FileWatcher(const char* path, bool watch_sub_tree, Flags<NotifyChangeFlag> flags);
	FileWatcher(const FileWatcher& watcher) = default;
	FileWatcher(void) = default;
	~FileWatcher(void);

	FileWatcher& operator=(const FileWatcher& rhs) = default;

	bool wait(int32_t wait_time_ms = 0) const;

	bool isValid(void) const;

private:
	HANDLE _change_handle = INVALID_HANDLE_VALUE;
};

NS_END
