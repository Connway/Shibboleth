//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 01/04/2009
//
// Filename: src-IL/src/il_files.c
//
// Description: File handling for DevIL
//
//-----------------------------------------------------------------------------


#define __FILES_C
#include "il_internal.h"
#include <stdarg.h>

// Global variable: set of file access functions
// @todo: for multi-threading support, these function pointers would have to be
// moved to the image object ILimage in devil_internal_exports
SIO gIO;

// Lump read functions
ILboolean	ILAPIENTRY iEofLump(ILHANDLE h);
ILint		ILAPIENTRY iGetcLump(ILHANDLE h);
ILuint		ILAPIENTRY iReadLump(ILHANDLE h, void *Buffer, const ILuint Size, const ILuint Number);
ILint		ILAPIENTRY iSeekLump(ILHANDLE h, ILint64 Offset, ILuint Mode);
ILuint		ILAPIENTRY iTellLump(ILHANDLE h);
ILint		ILAPIENTRY iPutcLump(ILubyte Char, ILHANDLE h);
ILint		ILAPIENTRY iWriteLump(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE h);

// "Fake" size functions, used to determine the size of write lumps
// Definitions are in il_size.cpp
ILint		ILAPIENTRY iSizeSeek(ILHANDLE h, ILint64 Offset, ILuint Mode);
ILuint		ILAPIENTRY iSizeTell(ILHANDLE h);
ILint		ILAPIENTRY iSizePutc(ILubyte Char, ILHANDLE h);
ILint		ILAPIENTRY iSizeWrite(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE h);

// Next 7 functions are the default read functions

ILHANDLE ILAPIENTRY iDefaultOpenR(ILconst_string FileName)
{
#ifndef _UNICODE
	return (ILHANDLE)fopen((char*)FileName, "rb");
#else
	// Windows has a different function, _wfopen, to open UTF16 files,
	//  whereas Linux just uses fopen for its UTF8 files.
	#ifdef _WIN32
		return (ILHANDLE)_wfopen(FileName, L"rb");
	#else
		return (ILHANDLE)fopen((char*)FileName, "rb");
	#endif
#endif//UNICODE
}


void ILAPIENTRY iDefaultCloseR(ILHANDLE Handle)
{
	fclose((FILE*)Handle);
	return;
}


ILboolean ILAPIENTRY iDefaultEof(ILHANDLE Handle)
{
	ILuint OrigPos, FileSize;

	// Find out the filesize for checking for the end of file
	OrigPos = gIO.tell(gIO.handle);
	gIO.seek(gIO.handle, 0, IL_SEEK_END);
	FileSize = gIO.tell(gIO.handle);
	gIO.seek(gIO.handle, OrigPos, IL_SEEK_SET);

	if (gIO.tell(gIO.handle) >= FileSize)
		return IL_TRUE;
	return IL_FALSE;
}


ILint ILAPIENTRY iDefaultGetc(ILHANDLE Handle)
{
	ILint Val;

	Val = 0;
	if (gIO.read(gIO.handle, &Val, 1, 1) != 1)
		return IL_EOF;
	return Val;
}


// @todo: change this back to have the handle in the last argument, then
// fread can be passed directly to ilSetRead, and iDefaultRead is not needed
ILuint ILAPIENTRY iDefaultRead(ILHANDLE h, void *Buffer, ILuint Size, ILuint Number)
{
	return (ILint)fread(Buffer, Size, Number, (FILE*) h);
}


ILint ILAPIENTRY iDefaultSeek(ILHANDLE Handle, ILint64 Offset, ILuint Mode)
{
	return fseek((FILE*)Handle, Offset, Mode);
}


ILuint ILAPIENTRY iDefaultTell(ILHANDLE Handle)
{
	return ftell((FILE*)Handle);
}


ILHANDLE ILAPIENTRY iDefaultOpenW(ILconst_string FileName)
{
#ifndef _UNICODE
	return (ILHANDLE)fopen((char*)FileName, "wb");
#else
	// Windows has a different function, _wfopen, to open UTF16 files,
	//  whereas Linux just uses fopen.
	#ifdef _WIN32
		return (ILHANDLE)_wfopen(FileName, L"wb");
	#else
		return (ILHANDLE)fopen((char*)FileName, "wb");
	#endif
#endif//UNICODE
}


void ILAPIENTRY iDefaultCloseW(ILHANDLE Handle)
{
	fclose((FILE*)Handle);
	return;
}


ILint ILAPIENTRY iDefaultPutc(ILubyte Char, ILHANDLE Handle)
{
	return fputc(Char, (FILE*)Handle);
}


ILint ILAPIENTRY iDefaultWrite(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle)
{
	return (ILint)fwrite(Buffer, Size, Number, (FILE*)Handle);
}


void ILAPIENTRY ilResetRead()
{
	ilSetRead(iDefaultOpenR, iDefaultCloseR, iDefaultEof, iDefaultGetc, 
				iDefaultRead, iDefaultSeek, iDefaultTell);
	return;
}


void ILAPIENTRY ilResetWrite()
{
	ilSetWrite(iDefaultOpenW, iDefaultCloseW, iDefaultPutc,
				iDefaultSeek, iDefaultTell, iDefaultWrite);
	return;
}


//! Allows you to override the default file-reading functions.
void ILAPIENTRY ilSetRead(fOpenProc aOpen, fCloseProc aClose, fEofProc aEof, fGetcProc aGetc, 
	fReadProc aRead, fSeekProc aSeek, fTellProc aTell)
{
	memset(&gIO, 0, sizeof(SIO));
	gIO.openReadOnly    = aOpen;
	gIO.close   = aClose;
	gIO.eof   = aEof;
	gIO.getc  = aGetc;
	gIO.read  = aRead;
	gIO.seek = aSeek;
	gIO.tell = aTell;

	return;
}


//! Allows you to override the default file-writing functions.
void ILAPIENTRY ilSetWrite(fOpenProc Open, fCloseProc Close, fPutcProc Putc, fSeekProc Seek, 
	fTellProc Tell, fWriteProc Write)
{
	memset(&gIO, 0, sizeof(SIO));
	gIO.openWrite = Open;
	gIO.close = Close;
	gIO.putc  = Putc;
	gIO.write = Write;
	gIO.seek  = Seek;
	gIO.tell  = Tell;

	return;
}


// Tells DevIL that we're reading from a file, not a lump
void iSetInputFile(ILHANDLE File)
{
	memset(&gIO, 0, sizeof(SIO));
	gIO.eof  = iDefaultEof;
	gIO.getc = iDefaultGetc;
	gIO.read = iDefaultRead;
	gIO.seek = iDefaultSeek;
	gIO.tell = iDefaultTell;
	gIO.handle = File;
	gIO.ReadFileStart = gIO.tell(gIO.handle);
}


// Tells DevIL that we're reading from a lump, not a file
void iSetInputLump(const void *Lump, ILuint Size)
{
	memset(&gIO, 0, sizeof(SIO));
	gIO.eof  = iEofLump;
	gIO.getc = iGetcLump;
	gIO.read = iReadLump;
	gIO.seek = iSeekLump;
	gIO.tell = iTellLump;
	gIO.lump = Lump;
	gIO.lumpPos = 0;
	gIO.lumpSize = Size;
}


// Tells DevIL that we're writing to a file, not a lump
void iSetOutputFile(ILHANDLE File)
{
	memset(&gIO, 0, sizeof(SIO));
	gIO.eof  = iEofLump;
	gIO.putc  = iDefaultPutc;
	gIO.seek = iDefaultSeek;
	gIO.tell = iDefaultTell;
	gIO.write = iDefaultWrite;
	gIO.handle = File;
}


// This is only called by ilDetermineSize.  Associates iputc, etc. with
//  "fake" writing functions in il_size.c.
void iSetOutputFake(void)
{
	memset(&gIO, 0, sizeof(SIO));
	gIO.putc = iSizePutc;
	gIO.seek = iSizeSeek;
	gIO.tell = iSizeTell;
	gIO.write = iSizeWrite;
	return;
}


// Tells DevIL that we're writing to a lump, not a file
void iSetOutputLump(void *Lump, ILuint Size)
{
	// In this case, ilDetermineSize is currently trying to determine the
	//  output buffer size.  It already has the write functions it needs.
	if (Lump == NULL)
		return;

	gIO.putc  = iPutcLump;
	gIO.seek = iSeekLump;
	gIO.tell = iTellLump;
	gIO.write = iWriteLump;
	gIO.lump = Lump;
	gIO.lumpPos = 0;
	gIO.lumpSize = Size;
}


ILuint64 ILAPIENTRY ilGetLumpPos()
{
	if (gIO.lump != 0)
		return gIO.lumpPos;
	return 0;
}


ILuint ILAPIENTRY ilprintf(const char *Line, ...)
{
	char	Buffer[2048];  // Hope this is large enough
	va_list	VaLine;
	ILuint	i;

	va_start(VaLine, Line);
	vsprintf(Buffer, Line, VaLine);
	va_end(VaLine);

	i = ilCharStrLen(Buffer);
	gIO.write(Buffer, 1, i, gIO.handle);

	return i;
}


// To pad zeros where needed...
void ipad(ILuint NumZeros)
{
	ILuint i = 0;
	for (; i < NumZeros; i++)
		gIO.putc(0, gIO.handle);
	return;
}


//
// The rest of the functions following in this file are quite
//	self-explanatory, except where commented.
//

// Next 12 functions are the default write functions

ILboolean ILAPIENTRY iEofFile(ILHANDLE h)
{
	return feof((FILE*)h);
}


ILboolean ILAPIENTRY iEofLump(ILHANDLE h)
{
	if (gIO.lumpSize != 0)
		return (gIO.lumpPos >= gIO.lumpSize);
	return IL_FALSE;
}


ILint ILAPIENTRY iGetcLump(ILHANDLE h)
{
	// If ReadLumpSize is 0, don't even check to see if we've gone past the bounds.
	if (gIO.lumpSize > 0) {
		if (gIO.lumpPos + 1 > gIO.lumpSize) {
			gIO.lumpPos--;
			ilSetError(IL_FILE_READ_ERROR);
			return IL_EOF;
		}
	}

	return *((ILubyte*)gIO.lump + gIO.lumpPos++);
}


ILuint ILAPIENTRY iReadLump(ILHANDLE h, void *Buffer, const ILuint Size, const ILuint Number)
{
	ILuint i, ByteSize = IL_MIN( Size*Number, gIO.lumpSize-gIO.lumpPos);

	for (i = 0; i < ByteSize; i++) {
		*((ILubyte*)Buffer + i) = *((ILubyte*)gIO.lump + gIO.lumpPos + i);
		if (gIO.lumpSize > 0) {  // ReadLumpSize is too large to care about apparently
			if (gIO.lumpPos + i > gIO.lumpSize) {
				gIO.lumpPos += i;
				if (i != Number)
					ilSetError(IL_FILE_READ_ERROR);
				return i;
			}
		}
	}

	gIO.lumpPos += i;
	if (Size != 0)
		i /= Size;
	if (i != Number)
		ilSetError(IL_FILE_READ_ERROR);
	return i;
}


ILint ILAPIENTRY iSeekFile(ILHANDLE h, ILint64 Offset, ILuint Mode)
{
	if (Mode == IL_SEEK_SET)
		Offset += gIO.ReadFileStart;  // This allows us to use IL_SEEK_SET in the middle of a file.
	return fseek((FILE*) gIO.handle, Offset, Mode);
}


// Returns 1 on error, 0 on success
ILint ILAPIENTRY iSeekLump(ILHANDLE h, ILint64 Offset, ILuint Mode)
{
	switch (Mode)
	{
		case IL_SEEK_SET:
			if (Offset > (ILint)gIO.lumpSize)
				return 1;
			gIO.lumpPos = Offset;
			break;

		case IL_SEEK_CUR:
			if (gIO.lumpPos + Offset > gIO.lumpSize || gIO.lumpPos+Offset < 0)
				return 1;
			gIO.lumpPos += Offset;
			break;

		case IL_SEEK_END:
			if (Offset > 0)
				return 1;
			// Should we use >= instead?
			if (abs(Offset) > (ILint)gIO.lumpSize)  // If ReadLumpSize == 0, too bad
				return 1;
			gIO.lumpPos = gIO.lumpSize + Offset;
			break;

		default:
			return 1;
	}

	return 0;
}


ILHANDLE ILAPIENTRY iGetFile(void)
{
	return gIO.handle;
}


const ILubyte* ILAPIENTRY iGetLump(void) {
	return (ILubyte*)gIO.lump;
}



// Next 4 functions are the default write functions

ILint ILAPIENTRY iPutcFile(ILubyte Char, ILHANDLE h)
{
	return putc(Char, (FILE*)gIO.handle);
}


ILint ILAPIENTRY iPutcLump(ILubyte Char, ILHANDLE h)
{
	if (gIO.lumpPos >= gIO.lumpSize)
		return IL_EOF;  // IL_EOF
	*((ILubyte*)(gIO.lump) + gIO.lumpPos++) = Char;
	return Char;
}


ILint ILAPIENTRY iWriteFile(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE h)
{
	ILuint NumWritten;
	NumWritten =fwrite(Buffer, Size, Number, (FILE*) gIO.handle);
	if (NumWritten != Number) {
		ilSetError(IL_FILE_WRITE_ERROR);
		return 0;
	}
	return NumWritten;
}


ILint ILAPIENTRY iWriteLump(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE h)
{
	ILuint SizeBytes = Size * Number;
	ILuint i = 0;

	for (; i < SizeBytes; i++) {
		if (gIO.lumpSize > 0) {
			if (gIO.lumpPos + i >= gIO.lumpSize) {  // Should we use > instead?
				ilSetError(IL_FILE_WRITE_ERROR);
				gIO.lumpPos += i;
				return i;
			}
		}

		*((ILubyte*)gIO.lump + gIO.lumpPos + i) = *((ILubyte*)Buffer + i);
	}

	gIO.lumpPos += SizeBytes;
	
	return SizeBytes;
}


ILuint ILAPIENTRY iTellFile(ILHANDLE h)
{
	return ftell((FILE*) h);
}


ILuint ILAPIENTRY iTellLump(ILHANDLE h)
{
	return gIO.lumpPos;
}
