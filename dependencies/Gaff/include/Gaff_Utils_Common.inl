	/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

/*!
	\brief Traverses all filesystem entries in a specific directory.

	\tparam
		Callback The type used for the callback.
		Callbacks take the signature bool CB(const char* name, size_t name_len, FileDataType type)
		Returning true will end the loop early.

	\return Returns whether the loop was terminated early.
*/
template <class Callback>
bool ForEachInDirectory(const char* directory, Callback&& callback)
{
	DIR* dir = opendir(directory);

	if (!dir) {
		return true;
	}

	dirent* entry = readdir(dir);
	bool early_out = false;

	while (entry) {
		if (callback(entry->d_name, _D_EXACT_NAMLEN(entry), (FileDataType)entry->d_type)) {
			early_out = true;
			break;
		}

		entry = readdir(dir);
	}

	closedir(dir);
	return early_out;
}

/*!
	\brief Traverses filesystem entries of \a type in a specific directory.

	\tparam type The filesystem entry type we are filtering to the callback.
	\tparam
		Callback The type used for the callback.
		Callbacks take the signature bool CB(const char* name, size_t name_len)
		Returning true will end the loop early.

	\return Returns whether the loop was terminated early.
*/
template <FileDataType type, class Callback>
bool ForEachTypeInDirectory(const char* directory, Callback&& callback)
{
	DIR* dir = opendir(directory);

	if (!dir) {
		return true;
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

#ifdef _UNICODE
//! wchar_t version of ForEachInDirectory().
template <class Callback>
bool ForEachInDirectory(const wchar_t* directory, Callback&& callback)
{
	WDIR* dir = wopendir(directory);

	if (!dir) {
		return true;
	}

	wdirent* entry = wreaddir(dir);
	bool early_out = false;

	while (entry) {
		if (callback(entry->d_name, _D_EXACT_NAMLEN(entry), (FileDataType)entry->d_type)) {
			early_out = true;
			break;
		}

		entry = wreaddir(dir);
	}

	wclosedir(dir);
	return early_out;
}

//! whcar_t version of ForEachTypeInDirectory.
template <FileDataType type, class Callback>
bool ForEachTypeInDirectory(const wchar_t* directory, Callback&& callback)
{
	WDIR* dir = wopendir(directory);

	if (!dir) {
		return true;
	}

	wdirent* entry = wreaddir(dir);
	bool early_out = false;

	while (entry) {
		if (entry->d_type == type) {
			if (callback(entry->d_name, _D_EXACT_NAMLEN(entry))) {
				early_out = true;
				break;
			}
		}

		entry = wreaddir(dir);
	}

	wclosedir(dir);
	return early_out;
}
#endif

/*!
	\brief Parses the command line into option/value(s) pairs using a hashmap.
	\return A HashMap of option/value strings.
	\note
		Options start with either a '-' or '--'. Option values are space separated.

		Example (using JSON style notation for result):

		(cmdline): --option1 value1 value2 -option2 value3 -flag0

		(result): { "option1": "value1 value2", "option2": "value3", "flag0": "" }
*/
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

/*!
	\brief Same as ParseCommandLine, but the user provides the HashMap we are outputting to.
*/
template <class Allocator>
void ParseCommandLine(int argc, char** argv, HashMap<AHashString<Allocator>, AString<Allocator>, Allocator>& out)
{
	if (argc == 1) {
		return;
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
				out.insert(option, values);
				values.clear();
			}

			option = value + option_begin;
		}
	}

	if (option.size()) {
		out.insert(option, values);
	}
}
