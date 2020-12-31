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

#include "Gaff_FileWatcher_Windows.h"

NS_GAFF

FileWatcher::FileWatcher(const char* path, bool watch_sub_tree, Flags<NotifyChangeFlag> flags):
	_change_handle(FindFirstChangeNotificationA(path, watch_sub_tree, static_cast<DWORD>(flags.getStorage())))
{
}

FileWatcher::~FileWatcher(void)
{
	if (isValid()) {
		FindCloseChangeNotification(_change_handle);
	}
}

bool FileWatcher::wait(int32_t wait_time_ms) const
{
	wait_time_ms = (wait_time_ms < 0) ? 0 : wait_time_ms;
	const bool changed = WaitForSingleObject(_change_handle, wait_time_ms) == WAIT_OBJECT_0;

	FindNextChangeNotification(_change_handle);

	return changed;
}

bool FileWatcher::isValid(void) const
{
	return _change_handle != INVALID_HANDLE_VALUE;
}

NS_END
