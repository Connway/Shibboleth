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

#pragma once

#include "Gaff_Defines.h"
#include "Gaff_String.h"
#include <cstring>
#include <cstdio>

NS_GAFF

class File
{
public:
	template <class Allocator>
	INLINE static bool checkExtension(const Gaff::AString<Allocator>& file_name, const Gaff::AString<Allocator>& extension)
	{
		return checkExtension(file_name.getBuffer(), extension.getBuffer());
	}

	INLINE static bool checkExtension(const char* file_name, const char* extension)
	{
		size_t len = strlen(file_name);
		assert(len > strlen(extension));
		return strcmp(file_name + len - strlen(extension), extension) == 0;
	}

	INLINE static bool remove(const char* file_name);
	INLINE static bool rename(const char* old_file_name, const char* new_file_name);

	enum OPEN_MODE
	{
		READ = 0,
		WRITE,
		APPEND,
		READ_EXT,
		WRITE_EXT,
		APPEND_EXT,
		READ_BINARY,
		WRITE_BINARY,
		APPEND_BINARY,
		READ_EXT_BINARY,
		WRITE_EXT_BINARY,
		APPEND_EXT_BINARY
	};

	enum SEEK_ORIGIN
	{
		BEGINNING = SEEK_SET,
		CURRENT = SEEK_CUR,
		END = SEEK_END
	};

	File(const char* file_name, OPEN_MODE mode = READ);
	File(void);
	~File(void);

	INLINE const FILE* getFile(void) const;
	INLINE FILE* getFile(void);

	INLINE OPEN_MODE getMode(void) const;

	template <class Allocator>
	bool open(const Gaff::AString<Allocator>& file_name, OPEN_MODE mode = READ)
	{
		assert(file_name.size());
		return open(file_name.getBuffer(), mode);
	}

	template <class Allocator>
	bool redirect(FILE* file, const Gaff::AString<Allocator>& file_name, OPEN_MODE mode = WRITE)
	{
		assert(!_file && file_name.size());
		return redirect(file, file_name.getBuffer(), mode);
	}

	template <class Allocator>
	bool redirect(const Gaff::AString<Allocator>& file_name, OPEN_MODE mode = WRITE)
	{
		assert(file_name.size() && _file);
		return redirect(file_name.getBuffer(), mode);
	}

	template <class Allocator>
	bool writeString(const Gaff::AString<Allocator>& s)
	{
		assert(_file && s.size());
		return writeString(s.getBuffer());
	}

	template <class T>
	size_t writeT(const T* data, size_t element_count)
	{
		return write(data, sizeof(T), element_count);
	}

	template <class T>
	size_t writeT(const T& data)
	{
		return write(&data, sizeof(T), 1);
	}

	bool open(const char* file_name, OPEN_MODE mode = READ);

	INLINE bool redirect(FILE* file, const char* file_name, OPEN_MODE mode = WRITE);
	INLINE bool redirect(const char* file_name, OPEN_MODE mode = WRITE);

	bool close(void);
	INLINE bool isOpen(void) const;
	INLINE bool neof(void) const;
	INLINE bool eof(void) const;

	INLINE bool flush(void);
	INLINE void setBuffer(char* buffer);

	INLINE size_t read(void* buffer, size_t element_size, size_t element_count);
	INLINE size_t write(void* buffer, size_t element_size, size_t element_count);

	INLINE void printfVA(const char* format_string, va_list vl);
	void printf(const char* format_string, ...);

	INLINE bool writeChar(char c);
	INLINE int readChar(void);

	INLINE bool writeString(const char* s);
	INLINE bool readString(char* buffer, int max_count);

	INLINE long getFilePos(void) const;
	INLINE bool seek(long offset, SEEK_ORIGIN origin = BEGINNING);
	INLINE void rewind(void);

	INLINE bool openTempFile(void);

	// Only useful in BINARY mode
	long getFileSize(void);
	bool readEntireFile(char* buffer);

	// Unicode functions
#ifdef _UNICODE
	template <class Allocator>
	static bool checkExtension(const Gaff::WString<Allocator>& file_name, const Gaff::WString<Allocator>& extension)
	{
		return checkExtension(file_name.getBuffer(), extension.getBuffer());
	}

	static bool checkExtension(const wchar_t* file_name, const wchar_t* extension)
	{
		size_t len = wcslen(file_name);
		assert(len > wcslen(extension));
		return wcscmp(file_name + len - wcslen(extension), extension) == 0;
	}

	INLINE static bool remove(const wchar_t* file_name);
	INLINE static bool rename(const wchar_t* old_file_name, const wchar_t* new_file_name);

	// File(const Gaff::WString& file_name, OPEN_MODE mode = READ);
	File(const wchar_t* file_name, OPEN_MODE mode = READ);

	template <class Allocator>
	bool open(const Gaff::WString<Allocator>& file_name, OPEN_MODE mode = READ)
	{
		assert(file_name.size());
		return open(file_name.getBuffer(), mode);
	}

	template <class Allocator>
	bool redirect(FILE* file, const Gaff::WString<Allocator>& file_name, OPEN_MODE mode = WRITE)
	{
		assert(!_file && file_name.size());
		return redirect(file, file_name.getBuffer(), mode);
	}

	template <class Allocator>
	bool redirect(const Gaff::WString<Allocator>& file_name, OPEN_MODE mode = WRITE)
	{
		assert(file_name.size() && _file);
		return redirect(file_name.getBuffer(), mode);
	}

	template <class Allocator>
	bool writeString(const Gaff::WString<Allocator>& s)
	{
		assert(_file && s.size());
		return writeString(s.getBuffer());
	}

	bool open(const wchar_t* file_name, OPEN_MODE mode = READ);

	INLINE bool redirect(FILE* file, const wchar_t* file_name, OPEN_MODE mode = WRITE);
	INLINE bool redirect(const wchar_t* file_name, OPEN_MODE mode = WRITE);

	INLINE void printfVA(const wchar_t* format_string, va_list vl);
	void printf(const wchar_t* format_string, ...);

	INLINE bool writeChar(wchar_t c);
	INLINE unsigned short readWChar(void);

	INLINE bool writeString(const wchar_t* s);
	INLINE bool readString(wchar_t* buffer, int max_count);
#endif

	File(File&& rhs);
	const File& operator=(File&& rhs);

private:
	FILE* _file;
	OPEN_MODE _mode;

	GAFF_NO_COPY(File);
};

NS_END
