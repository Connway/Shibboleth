/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Gaff_Assert.h"
#include <cstring>
#include <cstdio>

NS_GAFF

class File
{
public:
	static bool Remove(const char* file_name);
	static bool Rename(const char* old_file_name, const char* new_file_name);

	enum class OpenMode
	{
		Read = 0, // Open a file for read-only.
		Write, // Open a file for write-only. Deletes existing content.
		Append, // Open a file and append to already existing content.
		ReadExt, // Open's a file for read/write. Errors if file doesn't exist.
		WriteExt, // Opens a file for read/write. Delete's existing content.
		AppendExt, // Opens a file for read/write. Appends to already existing content.
		ReadBinary, // Same as Read, but in binary mode instead of text mode.
		WriteBinary, // Same as Write, but in binary mode instead of text mode.
		AppendBinary, // Same as Append, but in binary mode instead of text mode.
		ReadExtBinary, // Same as ReadExt, but in binary mode instead of text mode.
		WriteExtBinary, // Same as WriteExt, but in binary mode instead of text mode.
		AppendExtBinary // Same as AppendExt, but in binary mode instead of text mode.
	};

	enum class SeekOrigin
	{
		Beginning = SEEK_SET, // Starts seeking from the beginning of the file.
		Current = SEEK_CUR, // Starts seeking from the current position in the file.
		End = SEEK_END // Starts seeking from the end of the file.
	};

	File(const char8_t* file_name, OpenMode mode = OpenMode::Read);
	File(const char* file_name, OpenMode mode = OpenMode::Read);
	File(File&& rhs);
	File(FILE* rhs);
	File(void);
	~File(void);

	File& operator=(File&& rhs);

	const FILE* getFile(void) const;
	FILE* getFile(void);

	FILE* release(void);

	OpenMode getMode(void) const;

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

	bool open(const char8_t* file_name, OpenMode mode = OpenMode::Read);
	bool open(const char* file_name, OpenMode mode = OpenMode::Read);

	bool redirect(FILE* file, const char8_t* file_name, OpenMode mode = OpenMode::Write);
	bool redirect(const char8_t* file_name, OpenMode mode = OpenMode::Write);
	bool redirect(FILE* file, const char* file_name, OpenMode mode = OpenMode::Write);
	bool redirect(const char* file_name, OpenMode mode = OpenMode::Write);

	bool close(void);
	bool isOpen(void) const;
	bool neof(void) const;
	bool eof(void) const;

	bool flush(void);
	void setBuffer(char* buffer);

	size_t read(void* buffer, size_t element_size, size_t element_count);

	size_t write(void* buffer, size_t element_size, size_t element_count);
	bool write(const char8_t* str);
	bool write(const char* str);
	bool write(char8_t c);
	bool write(char c);

	void printfVA(const char8_t* format_string, va_list vl);
	void printf(const char8_t* format_string, ...);
	void printfVA(const char* format_string, va_list vl);
	void printf(const char* format_string, ...);

	bool writeChar(char8_t c);
	bool writeChar(char c);
	int32_t readChar(void);

	bool writeString(const char8_t* s);
	bool writeString(const char* s);
	bool readString(char8_t* buffer, int32_t max_byte_count);
	bool readString(char* buffer, int32_t max_byte_count);

	int32_t getFilePos(void) const;
	bool seek(long offset, SeekOrigin origin = SeekOrigin::Beginning);
	void rewind(void);

	bool openTempFile(void);

	// Only useful in BINARY mode
	int32_t getFileSize(void);
	bool readEntireFile(char8_t* buffer);
	bool readEntireFile(char* buffer);

private:
	FILE* _file = nullptr;
	OpenMode _mode = OpenMode::Read;

	GAFF_NO_COPY(File);
};

NS_END
