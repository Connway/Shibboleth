/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

NS_GAFF

#ifndef _D_EXACT_NAMLEN
	#define _D_EXACT_NAMLEN(entry) strlen(entry->d_name)
#endif

template <class Callback>
bool ForEachInDirectory(const char* directory, Callback&& callback)
{
	DIR* dir = opendir(directory);

	if (!dir) {
		return false;
	}

	dirent* entry = readdir(dir);
	bool early_out = false;

	while (entry) {
		if (callback(entry->d_name, _D_EXACT_NAMLEN(entry), static_cast<FileDataType>(entry->d_type))) {
			early_out = true;
			break;
		}

		entry = readdir(dir);
	}

	closedir(dir);
	return early_out;
}

template <FileDataType type, class Callback>
bool ForEachTypeInDirectory(const char* directory, Callback&& callback)
{
	DIR* dir = opendir(directory);

	if (!dir) {
		return false;
	}

	dirent* entry = readdir(dir);
	bool early_out = false;

	while (entry) {
		if (entry->d_type == type) {
			if (callback(entry->d_name, _D_EXACT_NAMLEN(entry))) {
				early_out = true;
				break;
			}
		}

		entry = readdir(dir);
	}

	closedir(dir);
	return early_out;
}

NS_END
