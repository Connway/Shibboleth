//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/07/2009
//
// Filename: src-IL/src/il_raw.c
//
// Description: "Raw" file functions
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_RAW


ILboolean iLoadRawInternal(void);
ILboolean iSaveRawInternal(void);


//! Reads a raw file
/*ILboolean ilLoadRaw(ILconst_string FileName)
{
	ILHANDLE	RawFile;
	ILboolean	bRaw = IL_FALSE;

	RawFile = gIO.openReadOnly(FileName);
	if (RawFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bRaw;
	}

	bRaw = ilLoadRawF(RawFile);
	gIO.close(RawFile);

	return bRaw;
}


//! Reads an already-opened raw file
ILboolean ilLoadRawF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = gIO.tell(gIO.handle);
	bRet = iLoadRawInternal();
	gIO.seek(gIO.handle, FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a raw memory "lump"
ILboolean ilLoadRawL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadRawInternal();
}*/


// Internal function to load a raw image
ILboolean iLoadRawInternal()
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}


	iCurImage->Width = GetLittleUInt();

	iCurImage->Height = GetLittleUInt();

	iCurImage->Depth = GetLittleUInt();

	iCurImage->Bpp = (ILubyte)gIO.getc(gIO.handle);

	if (gIO.read(gIO.handle, &iCurImage->Bpc, 1, 1) != 1)
		return IL_FALSE;

	if (!ilTexImage(iCurImage->Width, iCurImage->Height, iCurImage->Depth, iCurImage->Bpp, 0, ilGetTypeBpc(iCurImage->Bpc), NULL)) {
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;

	// Tries to read the correct amount of data
	if (gIO.read(gIO.handle, iCurImage->Data, 1, iCurImage->SizeOfData) < iCurImage->SizeOfData)
		return IL_FALSE;

	if (ilIsEnabled(IL_ORIGIN_SET)) {
		iCurImage->Origin = ilGetInteger(IL_ORIGIN_MODE);
	}
	else {
		iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;
	}

	if (iCurImage->Bpp == 1)
		iCurImage->Format = IL_LUMINANCE;
	else if (iCurImage->Bpp == 3)
		iCurImage->Format = IL_RGB;
	else  // 4
		iCurImage->Format = IL_RGBA;

	return ilFixImage();
}


//! Writes a Raw file
/*ILboolean ilSaveRaw(const ILstring FileName)
{
	ILHANDLE	RawFile;
	ILuint		RawSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	RawFile = gIO.openWrite(FileName);
	if (RawFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	RawSize = ilSaveRawF(RawFile);
	gIO.close(RawFile);

	if (RawSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes Raw to an already-opened file
ILuint ilSaveRawF(ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = gIO.tell(gIO.handle);
	if (iSaveRawInternal() == IL_FALSE)
		return 0;  // Error occurred
	return gIO.tell(gIO.handle) - Pos;  // Return the number of bytes written.
}


//! Writes Raw to a memory "lump"
ILuint ilSaveRawL(void *Lump, ILuint Size)
{
	ILuint Pos;
	iSetOutputLump(Lump, Size);
	Pos = gIO.tell(gIO.handle);
	if (iSaveRawInternal() == IL_FALSE)
		return 0;  // Error occurred
	return gIO.tell(gIO.handle) - Pos;  // Return the number of bytes written.
}*/


// Internal function used to load the raw data.
ILboolean iSaveRawInternal()
{
	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	SaveLittleUInt(iCurImage->Width);
	SaveLittleUInt(iCurImage->Height);
	SaveLittleUInt(iCurImage->Depth);
	gIO.putc(iCurImage->Bpp, gIO.handle);
	gIO.putc(iCurImage->Bpc, gIO.handle);
	gIO.write(iCurImage->Data, 1, iCurImage->SizeOfData, gIO.handle);

	return IL_TRUE;
}


#endif//IL_NO_RAW
