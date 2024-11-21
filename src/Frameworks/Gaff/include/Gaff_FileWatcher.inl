/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

template <class Allocator>
FileWatcherManager<Allocator>::FileWatcherManager(Allocator allocator):
	_watches(allocator), _allocator(allocator)
{
}

template <class Allocator>
bool FileWatcherManager<Allocator>::addWatch(const char* path, Flags<FileWatcher::NotifyChangeFlag> flags, Callback callback)
{
	for (Entry& entry : _watches) {
		if (entry.path == path) {
			if (Gaff::Find(entry.callbacks, callback) != entry.callbacks.end()) {
				return false;
			}

			entry.callbacks.emplace_back(callback);
			return true;
		}
	}

	FileWatcher watcher(path, flags);

	if (!watcher.isValid()) {
		return false;
	}

	_watches.emplace_back(Entry
	{
		Vector<Callback, Allocator>(_allocator),
		U8String<Allocator>(path, _allocator),
		watcher
	});

	_watches.back().callbacks.emplace_back(callback);
	return true;
}

template <class Allocator>
bool FileWatcherManager<Allocator>::removeWatch(const char* path, Callback callback)
{
	bool removed = false;

	for (const auto it = _watches.begin(); it != _watches.end();) {
		if (it->patch == path && it->callback == callback) {
			it = _watches.erase(it);
			removed = true;
		} else {
			++it;
		}
	}

	return removed;
}

template <class Allocator>
bool FileWatcherManager<Allocator>::removeWatch(Callback callback)
{
	bool removed = false;

	for (const auto it = _watches.begin(); it != _watches.end();) {
		if (it->callback == callback) {
			it = _watches.erase(it);
			removed = true;
		} else {
			++it;
		}
	}

	return removed;
}

template <class Allocator>
bool FileWatcherManager<Allocator>::removeWatch(const char* path)
{
	bool removed = false;

	for (const auto it = _watches.begin(); it != _watches.end();) {
		if (it->patch == path) {
			it = _watches.erase(it);
			removed = true;
		} else {
			++it;
		}
	}

	return removed;
}

template <class Allocator>
void FileWatcherManager<Allocator>::update(void)
{
	for (auto& entry : _watches) {
		if (!entry.watcher.listen()) {
			// $TODO: Log error once.
			continue;
		}

		if (const char* changed_file = entry.watcher.processEvents(); changed_file) {
			U8String<Allocator> file_path(entry.path, _allocator);
			file_path.append_sprintf("/%s", changed_file);

			for (Callback callback : entry.callbacks) {
				callback(file_path.data());
			}
		}
	}
}
