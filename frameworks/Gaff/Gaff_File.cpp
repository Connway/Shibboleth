/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gaff_File.h"

#ifdef PLATFORM_WINDOWS
	#include "Gaff_String.h"
	#include <io.h>
#elif defined(PLATFORM_LINUX)
	#include <sys/io.h>
#elif defined(PLATFORM_MAC)
	#include <sys/uio.h>
#endif

#include <cstdarg>

NS_GAFF

#ifdef PLATFORM_WINDOWS
	static const wchar_t* gOpenModesW[] = {
		L"r", L"w", L"a", L"r+", L"w+", L"a+",
		L"rb", L"wb", L"ab", L"rb+", L"wb+", L"ab+"
	};
#else
	static const char* gOpenModes[] = {
		"r", "w", "a", "r+", "w+", "a+",
		"rb", "wb", "ab", "rb+", "wb+", "ab+"
	};
#endif

bool File::CheckExtension(const char* file_name, size_t file_name_size, const char* extension, size_t extension_size)
{
	GAFF_ASSERT(file_name && extension && file_name_size > extension_size);
	return strcmp(file_name + file_name_size - extension_size, extension) == 0;
}

bool File::CheckExtension(const char* file_name, const char* extension)
{
	GAFF_ASSERT(file_name && extension && strlen(file_name) && strlen(extension));
	return CheckExtension(file_name, strlen(file_name), extension, strlen(extension));
}


bool File::Remove(const char* file_name)
{
	GAFF_ASSERT(file_name && strlen(file_name));

#ifdef PLATFORM_WINDOWS
	CONVERT_STRING(wchar_t, temp, file_name);
	return !_wremove(temp);
#else
	return !remove(file_name);
#endif
}

bool File::Rename(const char* old_file_name, const char* new_file_name)
{
	GAFF_ASSERT(old_file_name && new_file_name && strlen(old_file_name) && strlen(new_file_name));

#ifdef PLATFORM_WINDOWS
	CONVERT_STRING(wchar_t, temp1, old_file_name);
	CONVERT_STRING(wchar_t, temp2, new_file_name);
	return !_wrename(temp1, temp2);
#else
	return !rename(old_file_name, new_file_name);
#endif
}

File::File(const char* file_name, OpenMode mode):
	_file(nullptr)
{
	GAFF_ASSERT(file_name);
	open(file_name, mode);
}

File::File(File&& rhs):
	_file(rhs._file), _mode(rhs._mode)
{
	rhs._file = nullptr;
}

File::File(void):
	_file(nullptr)
{
}

File::~File(void)
{
	close();
}

File& File::operator=(File&& rhs)
{
	_file = rhs._file;
	_mode = rhs._mode;
	rhs._file = nullptr;
	return *this;
}

const FILE* File::getFile(void) const
{
	return _file;
}

FILE* File::getFile(void)
{
	return _file;
}

File::OpenMode File::getMode(void) const
{
	return _mode;
}

bool File::open(const char* file_name, OpenMode mode)
{
	GAFF_ASSERT(file_name && strlen(file_name));

	if (_file) {
		return false;
	}

	_mode = mode;

#ifdef PLATFORM_WINDOWS
	CONVERT_STRING(wchar_t, temp, file_name);
	_wfopen_s(&_file, temp, gOpenModesW[mode]);
#else
	_file = fopen(file_name, gOpenModes[mode]);
#endif

	return _file != NULL;
}

bool File::redirect(FILE* file, const char* file_name, OpenMode mode)
{
	GAFF_ASSERT(!_file && file_name);

#ifdef PLATFORM_WINDOWS
	CONVERT_STRING(wchar_t, temp, file_name);
	_wfreopen_s(&_file, temp, gOpenModesW[mode], file);
#else
	_file = freopen(file_name, gOpenModes[mode], file);
#endif

	return _file && _file == file;
}

bool File::redirect(const char* file_name, OpenMode mode)
{
	GAFF_ASSERT(_file && file_name);

#ifdef PLATFORM_WINDOWS
	CONVERT_STRING(wchar_t, temp, file_name);
	FILE* out = nullptr;
	_wfreopen_s(&out, temp, gOpenModesW[mode], _file);
#else
	FILE* out = freopen(file_name, gOpenModes[mode], _file);
#endif

	return out && out == _file;
}

bool File::close(void)
{
	bool ret = true;

	if (_file) {
		ret = !fclose(_file);
		_file = nullptr;
	}

	return ret;
}

bool File::isOpen(void) const
{
	return _file != nullptr;
}

bool File::neof(void) const
{
	GAFF_ASSERT(_file);
	return !feof(_file);
}

bool File::eof(void) const
{
	GAFF_ASSERT(_file);
	return feof(_file) != 0;
}

bool File::flush(void)
{
	GAFF_ASSERT(_file);
	return !fflush(_file);
}

void File::setBuffer(char* buffer)
{
	GAFF_ASSERT(_file && buffer);
	setbuf(_file, buffer);
}

size_t File::read(void* buffer, size_t element_size, size_t element_count)
{
	GAFF_ASSERT(_file && buffer && element_size && element_count);
	return fread(buffer, element_size, element_count, _file);
}

size_t File::write(void* buffer, size_t element_size, size_t element_count)
{
	GAFF_ASSERT(_file && buffer && element_size && element_count);
	return fwrite(buffer, element_size, element_count, _file);
}

void File::printfVA(const char* format_string, va_list vl)
{
	GAFF_ASSERT(_file && format_string && vl);
	vfprintf_s(_file, format_string, vl);
}

void File::printf(const char* format_string, ...)
{
	GAFF_ASSERT(_file && format_string);

	va_list vl;
	va_start(vl, format_string);
	vfprintf_s(_file, format_string, vl);
	va_end(vl);
}

bool File::writeChar(char c)
{
	GAFF_ASSERT(_file);
	return fputc(c, _file) != EOF;
}

int File::readChar(void)
{
	GAFF_ASSERT(_file);
	return fgetc(_file);
}

bool File::writeString(const char* s)
{
	GAFF_ASSERT(_file && s);
	return fputs(s, _file) != EOF;
}

bool File::readString(char* buffer, int max_byte_count)
{
	GAFF_ASSERT(_file && buffer && max_byte_count > -1);
	char* tmp = fgets(buffer, max_byte_count, _file);
	return tmp != nullptr;
}

long File::getFilePos(void) const
{
	GAFF_ASSERT(_file);
	return ftell(_file);
}

bool File::seek(long offset, SEEK_ORIGIN origin)
{
	GAFF_ASSERT(_file);
	return !fseek(_file, offset, origin);
}

void File::rewind(void)
{
	GAFF_ASSERT(_file);
	::rewind(_file);
}

bool File::openTempFile(void)
{
	GAFF_ASSERT(!_file);
	_file = tmpfile();
	return _file != nullptr;
}

long File::getFileSize(void)
{
	GAFF_ASSERT(_file);
	long size;

	if (fseek(_file, 0, Gaff::File::END)) {
		return -1;
	}

	size = ftell(_file);
	::rewind(_file);

	return size;
}

bool File::readEntireFile(char* buffer)
{
	GAFF_ASSERT(buffer && _file);
	size_t size = static_cast<size_t>(getFileSize());
	size_t bytes_read = 0;

	while (bytes_read < size && !feof(_file)) {
		size_t this_read = fread(buffer, sizeof(char), size, _file);
		bytes_read += this_read;

		// something went wrong
		if ((this_read == 0 && bytes_read < size) ||
			(bytes_read < size && feof(_file))) {

				return false;
		}
	}

	return true;
}

NS_END