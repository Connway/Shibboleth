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

#pragma once

#include "Gaff_Assert.h"
#include <cstring>
#include <cstdio>

NS_GAFF

/*!
	\brief Wraps standard file operations.
*/
class File
{
public:
	INLINE static bool CheckExtension(const char* file_name, size_t file_name_size, const char* extension, size_t extension_size);
	INLINE static bool CheckExtension(const char* file_name, const char* extension);

	INLINE static bool Remove(const char* file_name);
	INLINE static bool Rename(const char* old_file_name, const char* new_file_name);

	/*!
		\brief Modes in which we can open a file.
	*/
	enum OpenMode
	{
		READ = 0, //!< Open a file for read-only.
		WRITE, //!< Open a file for write-only. Deletes existing content.
		APPEND, //!< Open a file and append to already existing content.
		READ_EXT, //!< Open's a file for read/write. Errors if file doesn't exist.
		WRITE_EXT, //!< Open's a file for read/write. Delete's existing content.
		APPEND_EXT, //!< Open's a file for read/write. Appends to already existing content.
		READ_BINARY, //!< Same as READ, but in binary mode instead of text mode.
		WRITE_BINARY, //!< Same as WRITE, but in binary mode instead of text mode.
		APPEND_BINARY, //!< Same as APPEND, but in binary mode instead of text mode.
		READ_EXT_BINARY, //!< Same as READ_EXT, but in binary mode instead of text mode.
		WRITE_EXT_BINARY, //!< Same as WRITE_EXT, but in binary mode instead of text mode.
		APPEND_EXT_BINARY //!< Same as APPEND_EXT, but in binary mode instead of text mode.
	};

	/*!
		\brief Positions to seek from when calling seek().
	*/
	enum SEEK_ORIGIN
	{
		BEGINNING = SEEK_SET, //!< Starts seeking from the beginning of the file.
		CURRENT = SEEK_CUR, //!< Starts seeking from the current position in the file.
		END = SEEK_END //!< Starts seeking from the end of the file.
	};

	File(const char* file_name, OpenMode mode = READ);
	File(File&& rhs);
	File(void);
	~File(void);

	File& operator=(File&& rhs);

	INLINE const FILE* getFile(void) const;
	INLINE FILE* getFile(void);

	INLINE OpenMode getMode(void) const;

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

	bool open(const char* file_name, OpenMode mode = READ);

	INLINE bool redirect(FILE* file, const char* file_name, OpenMode mode = WRITE);
	INLINE bool redirect(const char* file_name, OpenMode mode = WRITE);

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
	INLINE bool readString(char* buffer, int max_byte_count);

	INLINE long getFilePos(void) const;
	INLINE bool seek(long offset, SEEK_ORIGIN origin = BEGINNING);
	INLINE void rewind(void);

	INLINE bool openTempFile(void);

	// Only useful in BINARY mode
	long getFileSize(void);
	bool readEntireFile(char* buffer);

private:
	FILE* _file;
	OpenMode _mode;

	GAFF_NO_COPY(File);
};

NS_END
