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

#include "Gaff_File.h"
#include <cstdarg>

#if defined(_WIN32) || defined(_WIN64)
	#include <io.h>
#else
	#include <sys/io.h>
#endif


NS_GAFF

static const char* _open_modes_a[12] = {
		"r", "w", "a", "r+", "w+", "a+",
		"rb", "wb", "ab", "rb+", "wb+", "ab+"
};

/*!
	\brief Checks of the \a file_name ends with \a extension. This version takes string sizes to avoid calls to strlen().

	\param file_name The name of the file we are checking the extension of.
	\param file_name_size The length of \a file_name.
	\param extension The extension we are checking for.
	\param extension_size The length of \a extension.

	\return Whether \a file_name has \a extension.
*/
bool File::checkExtension(const char* file_name, size_t file_name_size, const char* extension, size_t extension_size)
{
	assert(file_name && extension && file_name_size > extension_size);
	return strcmp(file_name + file_name_size - extension_size, extension) == 0;
}

/*!
	\brief Checks of the \a file_name ends with \a extension.

	\param file_name The name of the file we are checking the extension of.
	\param extension The extension we are checking for.

	\return Whether \a file_name has \a extension.

	\note This function makes calls to strlen().
*/
bool File::checkExtension(const char* file_name, const char* extension)
{
	assert(file_name && extension);
	return checkExtension(file_name, strlen(file_name), extension, strlen(extension));
}


bool File::remove(const char* file_name)
{
	assert(file_name);
	return !::remove(file_name);
}

bool File::rename(const char* old_file_name, const char* new_file_name)
{
	assert(old_file_name && new_file_name);
	return !::rename(old_file_name, new_file_name);
}

#ifdef _UNICODE
static const wchar_t* _open_modes_u[12] = {
	L"r", L"w", L"a", L"r+", L"w+", L"a+",
	L"rb", L"wb", L"ab", L"rb+", L"wb+", L"ab+"
};

bool File::remove(const wchar_t* file_name)
{
	assert(file_name);
	return !::_wremove(file_name);
}

bool File::rename(const wchar_t* old_file_name, const wchar_t* new_file_name)
{
	assert(old_file_name && new_file_name);
	return !::_wrename(old_file_name, new_file_name);
}

#endif

File::File(const char* file_name, OPEN_MODE mode):
	_file(nullptr)
{
	assert(file_name);
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

const File& File::operator=(File&& rhs)
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

/*!
	\brief Get's the mode that this file was opened with.
*/
File::OPEN_MODE File::getMode(void) const
{
	return _mode;
}

bool File::open(const char* file_name, OPEN_MODE mode)
{
	assert(file_name);

	if (_file) {
		return false;
	}

	_mode = mode;
	_file = fopen(file_name, _open_modes_a[mode]);
	return _file != NULL;
}

/*!
	\brief Redirect's the \a file stream to the file found at \a file_name.

	\param file The file stream to redirect.
	\param file_name The file to redirect the \a file stream to.
	\param mode The mode to open \a file_name with.

	\return Whether the redirect succeeded.
*/
bool File::redirect(FILE* file, const char* file_name, OPEN_MODE mode)
{
	assert(!_file && file_name);
	_file = freopen(file_name, _open_modes_a[mode], file);
	return _file && _file == file;
}

/*!
	\brief Redirects the file stream that this File currently has open to \a file_name.

	\param file_name The file to redirect the currently opened file stream to.
	\param mode The mode to open \a file_name with.

	\return Whether the redirect succeeded.
*/
bool File::redirect(const char* file_name, OPEN_MODE mode)
{
	assert(_file && file_name);
	FILE* out = freopen(file_name, _open_modes_a[mode], _file);
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
	return _file != NULL && _file != nullptr;
}

/*!
	\brief Not-End-of-File.
	\return Returns true if we ARE NOT at the end of the file, false otherwise.
*/
bool File::neof(void) const
{
	assert(_file);
	return !feof(_file);
}

/*!
	\brief End-of-File.
	\return Returns true if we ARE at the end of the file, false otherwise.
*/
bool File::eof(void) const
{
	assert(_file);
	return feof(_file) != 0;
}

/*!
	\brief Flushes the current file stream buffer to disk.
	\bool Returns whether flushing succeeded.
*/
bool File::flush(void)
{
	assert(_file);
	return !fflush(_file);
}

/*!
	\brief Sets the internal buffer to use for I/O operations performed on the file stream.
*/
void File::setBuffer(char* buffer)
{
	assert(_file && buffer);
	setbuf(_file, buffer);
}

size_t File::read(void* buffer, size_t element_size, size_t element_count)
{
	assert(_file && buffer && element_size && element_count);
	return fread(buffer, element_size, element_count, _file);
}

size_t File::write(void* buffer, size_t element_size, size_t element_count)
{
	assert(_file && buffer && element_size && element_count);
	return fwrite(buffer, element_size, element_count, _file);
}

void File::printfVA(const char* format_string, va_list vl)
{
	assert(_file && format_string && vl);
	vfprintf(_file, format_string, vl);
}

void File::printf(const char* format_string, ...)
{
	assert(_file && format_string);

	va_list vl;
	va_start(vl, format_string);
	vfprintf(_file, format_string, vl);
	va_end(vl);
}

bool File::writeChar(char c)
{
	assert(_file);
	return fputc(c, _file) != EOF;
}

int File::readChar(void)
{
	assert(_file);
	return fgetc(_file);
}

bool File::writeString(const char* s)
{
	assert(_file && s);
	return fputs(s, _file) != EOF;
}

bool File::readString(char* buffer, int max_count)
{
	assert(_file && buffer && max_count > -1);
	char* tmp = fgets(buffer, max_count, _file);
	return tmp != NULL && tmp != nullptr;
}

/*!
	\brief Gets the current position in the file stream.
	\return The current position in the file stream.
*/
long File::getFilePos(void) const
{
	assert(_file);
	return ftell(_file);
}

bool File::seek(long offset, SEEK_ORIGIN origin)
{
	assert(_file);
	return !fseek(_file, offset, origin);
}

void File::rewind(void)
{
	assert(_file);
	::rewind(_file);
}

/*!
	\brief Opens a temporary file using the name returned by tmpfile().
	\return Whether the file successfully opened.
*/
bool File::openTempFile(void)
{
	assert(!_file);
	_file = tmpfile();
	return _file != NULL && _file != nullptr;
}

long File::getFileSize(void)
{
	assert(_file);
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
	assert(buffer && _file);
	long size = getFileSize();
	size_t bytes_read = 0;

	while (bytes_read < (size_t)size && !feof(_file)) {
		size_t this_read = fread(buffer, sizeof(char), size, _file);
		bytes_read += this_read;

		// something went wrong
		if ((this_read == 0 && bytes_read < (size_t)size) ||
			(bytes_read < (size_t)size && feof(_file))) {

				return false;
		}
	}

	return true;
}

// Unicode functions
#ifdef _UNICODE
bool File::checkExtension(const wchar_t* file_name, size_t file_name_size, const wchar_t* extension, size_t extension_size)
{
	assert(file_name && extension && file_name_size > extension_size);
	return wcscmp(file_name + file_name_size - extension_size, extension) == 0;
}

bool File::checkExtension(const wchar_t* file_name, const wchar_t* extension)
{
	assert(file_name && extension);
	return checkExtension(file_name, wcslen(file_name), extension, wcslen(extension));
}

File::File(const wchar_t* file_name, OPEN_MODE mode):
	_file(nullptr)
{
	assert(file_name);
	open(file_name, mode);
}

bool File::open(const wchar_t* file_name, OPEN_MODE mode)
{
	assert(file_name);

	if (_file) {
		return false;
	}

	_mode = mode;
	_file = _wfopen(file_name, _open_modes_u[mode]);
	return _file != NULL;
}

bool File::redirect(FILE* file, const wchar_t* file_name, OPEN_MODE mode)
{
	assert(!_file && file_name);
	_file = _wfreopen(file_name, _open_modes_u[mode], file);
	return _file && _file == file;
}

bool File::redirect(const wchar_t* file_name, OPEN_MODE mode)
{
	assert(_file && file_name);
	FILE* out = _wfreopen(file_name, _open_modes_u[mode], _file);
	return out && out == _file;
}

void File::printfVA(const wchar_t* format_string, va_list vl)
{
	assert(_file && format_string && vl);
	vfwprintf(_file, format_string, vl);
}

void File::printf(const wchar_t* format_string, ...)
{
	assert(_file && format_string);

	va_list vl;
	va_start(vl, format_string);
	vfwprintf(_file, format_string, vl);
	va_end(vl);
}

bool File::writeChar(wchar_t c)
{
	assert(_file);
	return fputwc(c, _file) != WEOF;
}

unsigned short File::readWChar(void)
{
	assert(_file);
	return fgetwc(_file);
}

bool File::writeString(const wchar_t* s)
{
	assert(_file && s);
	return fputws(s, _file) != WEOF;
}

bool File::readString(wchar_t* buffer, int max_count)
{
	assert(_file && buffer && max_count > -1);
	wchar_t* tmp = fgetws(buffer, max_count, _file);
	return tmp != NULL && tmp != nullptr;
}
#endif

NS_END
