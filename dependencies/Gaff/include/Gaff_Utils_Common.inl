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
		if (callback(entry->d_name, _D_EXACT_NAMLEN(entry), (FileDataType)entry->d_type)) {
			break;
		}

		entry = readdir(dir);
	}

	closedir(dir);
}

template <FileDataType type, class Callback>
void ForEachTypeInDirectory(const char* directory, Callback&& callback)
{
	DIR* dir = opendir(directory);

	if (!dir) {
		return;
	}

	dirent* entry = readdir(dir);

	while (entry) {
		if (entry->d_type == type) {
			if (callback(entry->d_name, _D_EXACT_NAMLEN(entry))) {
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
		if (callback(entry->d_name, _D_EXACT_NAMLEN(entry), (FileDataType)entry->d_type)) {
			break;
		}

		entry = wreaddir(dir);
	}

	wclosedir(dir);
}

template <FileDataType type, class Callback>
void ForEachTypeInDirectory(const wchar_t* directory, Callback&& callback)
{
	WDIR* dir = wopendir(directory);

	if (!dir) {
		return;
	}

	wdirent* entry = wreaddir(dir);

	while (entry) {
		if (entry->d_type == type) {
			if (callback(entry->d_name, _D_EXACT_NAMLEN(entry))) {
				break;
			}
		}

		entry = wreaddir(dir);
	}

	wclosedir(dir);
}
#endif

template <class Allocator>
HashMap<AHashString<Allocator>, AString<Allocator>, Allocator> ParseCommandLine(int argc, char** argv)
{
	HashMap<AHashString<Allocator>, AString<Allocator>, Allocator> opt_flag_map;

	if (argc == 1) {
		return opt_flag_map;
	}

	AHashString<Allocator> option;
	AString<Allocator> values;

	for (int i = 1; i < argc; ++i) {
		unsigned int option_begin = 0;
		const char* value = argv[i];

		// If it doesn't start with - or -- then skip it!
		while (value[option_begin] == '-') {
			++option_begin;
		}

		if (!option_begin) {
			if (option.size()) {
				if (values.size()) {
					values += ' ';
				}

				values += value;
			}

		} else {
			if (option.size()) {
				opt_flag_map.insert(option, values);
				values.clear();
			}

			option = value + option_begin;
		}
	}

	if (option.size()) {
		opt_flag_map.insert(option, values);
	}

	return opt_flag_map;
}
