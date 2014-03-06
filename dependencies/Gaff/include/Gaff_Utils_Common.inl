/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

template <class Callback>
void ForEachInDirectory(const char* directory, Callback&& callback)
{
	DIR* dir = opendir(directory);

	if (!dir) {
		return;
	}

	dirent* entry = readdir(dir);

	while (entry) {
		if (callback(entry->d_name, entry->d_namlen, (DataType)entry->d_type)) {
			break;
		}

		entry = readdir(dir);
	}

	closedir(dir);
}

template <DataType type, class Callback>
void ForEachTypeInDirectory(const char* directory, Callback&& callback)
{
	DIR* dir = opendir(directory);

	if (!dir) {
		return;
	}

	dirent* entry = readdir(dir);

	while (entry) {
		if (entry->d_type == type) {
			if (callback(entry->d_name, entry->d_namlen)) {
				break;
			}
		}

		entry = readdir(dir);
	}

	closedir(dir);
}

#ifdef _UNICODE
template <class Callback>
void ForEachInDirectory(const wchar_t* directory, Callback&& callback)
{
	WDIR* dir = wopendir(directory);

	if (!dir) {
		return;
	}

	wdirent* entry = wreaddir(dir);

	while (entry) {
		if (callback(entry->d_name, entry->d_namlen, (DataType)entry->d_type)) {
			break;
		}

		entry = wreaddir(dir);
	}

	wclosedir(dir);
}

template <DataType type, class Callback>
void ForEachTypeInDirectory(const wchar_t* directory, Callback&& callback)
{
	WDIR* dir = wopendir(directory);

	if (!dir) {
		return;
	}

	wdirent* entry = wreaddir(dir);

	while (entry) {
		if (entry->d_type == type) {
			if (callback(entry->d_name, entry->d_namlen)) {
				break;
			}
		}

		entry = wreaddir(dir);
	}

	wclosedir(dir);
}
#endif
