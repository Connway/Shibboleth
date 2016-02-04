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

/*! \file */

#pragma once

#include "Gaff_Defines.h"

struct aiMetadata;

NS_GAFF

/*!
	\brief
		Class that wraps metadata data extracted from a loaded SceneNode.
		Unfortunately, assimp has no documentation on this structure.
*/
class MetaData
{
public:
	enum ValueType
	{
		VT_INT = 0,
		VT_UINT64,
		VT_FLOAT,
		VT_STRING,
		VT_VEC3
	};

	MetaData(const MetaData& meta_data);
	MetaData(void);
	~MetaData(void);

	INLINE unsigned int getNumProperties(void) const;
	INLINE const char* getKey(unsigned int index) const;
	INLINE ValueType getValueType(unsigned int index) const;

	INLINE bool getBool(unsigned int index) const;
	INLINE int getInt(unsigned int index) const;
	INLINE unsigned long long getUInt64(unsigned int index) const;
	INLINE float getFloat(unsigned int index) const;
	INLINE const char* getString(unsigned int index) const;
	INLINE const float* getVec3(unsigned int index) const;

	INLINE bool valid(void) const;

	INLINE const MetaData& operator=(const MetaData& rhs);

	INLINE bool operator==(const MetaData& rhs) const;
	INLINE bool operator!=(const MetaData& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiMetadata* _meta_data;

	friend class SceneNode;
	MetaData(const aiMetadata* meta_data);
};

NS_END
