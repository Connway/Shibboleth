//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 10/10/2006
//
// Filename: src-IL/src/il_cut.c
//
// Description: Reads a Dr. Halo .cut file
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_CUT
#include "il_manip.h"
#include "il_pal.h"
#include "il_bits.h"


// Wrap it just in case...
#ifdef _MSC_VER
#pragma pack(push, packed_struct, 1)
#endif
typedef struct CUT_HEAD
{
	ILushort	Width;
	ILushort	Height;
	ILint		Dummy;
} IL_PACKSTRUCT CUT_HEAD;
#ifdef _MSC_VER
#pragma pack(pop,  packed_struct)
#endif


//	Note:  .Cut support has not been tested yet!
// A .cut can only have 1 bpp.
//	We need to add support for the .pal's PSP outputs with these...
ILboolean iLoadCutInternal(ILimage* image)
{
	CUT_HEAD	Header;
	ILuint		Size, i = 0, j;
	ILubyte		Count, Run;

	if (iCurImage == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Header.Width = GetLittleShort();
	Header.Height = GetLittleShort();
	Header.Dummy = GetLittleInt();

	if (Header.Width == 0 || Header.Height == 0) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	if (!ilTexImage(Header.Width, Header.Height, 1, 1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, NULL)) {  // always 1 bpp
		return IL_FALSE;
	}
	iCurImage->Origin = IL_ORIGIN_LOWER_LEFT;

	Size = Header.Width * Header.Height;

	while (i < Size) {
		Count = gIO.getc(gIO.handle);
		if (Count == 0) { // end of row
			gIO.getc(gIO.handle);  // Not supposed to be here, but
			gIO.getc(gIO.handle);  //  PSP is putting these two bytes here...WHY?!
			continue;
		}
		if (Count & BIT_7) {  // rle-compressed
			ClearBits(Count, BIT_7);
			Run = gIO.getc(gIO.handle);
			for (j = 0; j < Count; j++) {
				iCurImage->Data[i++] = Run;
			}
		}
		else {  // run of pixels
			for (j = 0; j < Count; j++) {
				iCurImage->Data[i++] = gIO.getc(gIO.handle);
			}
		}
	}

	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;  // Not sure

	/*iCurImage->Pal.Palette = SharedPal.Palette;
	iCurImage->Pal.PalSize = SharedPal.PalSize;
	iCurImage->Pal.PalType = SharedPal.PalType;*/

	return ilFixImage();
}

/* ?????????
void ilPopToast() {
	ILstring flipCode = IL_TEXT("#flipCode and www.flipCode.com rule you all.");
	flipCode[0] = flipCode[0];
}
*/



#endif//IL_NO_CUT
