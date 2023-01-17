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

#if defined(_WIN32) || defined(_WIN64)

#include "Gaff_FileWatcher_Windows.h"
#include "Gaff_String.h"

NS_GAFF

FileWatcher::FileWatcher(const char* path, Flags<NotifyChangeFlag> flags):
	_flags(flags)
{
	memset(&_overlapped, 0, sizeof(OVERLAPPED));

	_dir_handle = CreateFileA(
		path,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);
}

FileWatcher::~FileWatcher(void)
{
}

const char* FileWatcher::processEvents(void)
{
	if (!isValid()) {
		return nullptr;
	}

	DWORD bytes_written = 0;

	// File has changed. Notify
	if (GetOverlappedResult(_dir_handle, &_overlapped, &bytes_written, false)) {
		const FILE_NOTIFY_INFORMATION* const entry = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(_buffer);

		if (entry->FileNameLength > 0) {
			const wchar_t* filename = entry->FileName;
			CONVERT_STRING(char, buffer, filename);
			strncpy(_buffer, buffer, eastl::CharStrlen(buffer) + 1);

			return _buffer;
		}
	}

	return nullptr;
}

bool FileWatcher::listen(bool watch_subtree)
{
	if (!isValid()) {
		return false;
	}

	if (_overlapped.hEvent && !ResetEvent(_overlapped.hEvent)) {
		// $TODO: Log error.
		return false;
	}

	DWORD bytes_written = 0;

	const BOOL result = ReadDirectoryChangesW(
		_dir_handle,
		_buffer,
		sizeof(_buffer),
		watch_subtree,
		_flags.getStorage(),
		&bytes_written,
		&_overlapped,
		nullptr
	);

	return (result) ? true : false;
}

bool FileWatcher::isValid(void) const
{
	return _dir_handle != INVALID_HANDLE_VALUE;
}

NS_END

#endif
