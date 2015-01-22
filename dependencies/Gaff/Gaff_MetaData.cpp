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

#include "Gaff_MetaData.h"
#include <assimp/types.h>
#include <assimp/metadata.h>

NS_GAFF

MetaData::MetaData(const aiMetadata* meta_data):
	_meta_data(meta_data)
{
}

MetaData::MetaData(const MetaData& meta_data):
	_meta_data(meta_data._meta_data)
{
}

MetaData::MetaData(void):
	_meta_data(nullptr)
{
}

MetaData::~MetaData(void)
{
}

unsigned int MetaData::getNumProperties(void) const
{
	assert(_meta_data);
	return _meta_data->mNumProperties;
}

const char* MetaData::getKey(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties);
	return _meta_data->mKeys[index].C_Str();
}

MetaData::ValueType MetaData::getValueType(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties);
	return (ValueType)_meta_data->mValues[index].mType;
}

bool MetaData::getBool(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties && _meta_data->mValues[index].mType == AI_BOOL);
	return *((bool*)_meta_data->mValues[index].mData);
}

int MetaData::getInt(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties && _meta_data->mValues[index].mType == AI_INT);
	return *((int*)_meta_data->mValues[index].mData);
}

unsigned long long MetaData::getUInt64(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties && _meta_data->mValues[index].mType == AI_UINT64);
	return *((unsigned long long*)_meta_data->mValues[index].mData);
}

float MetaData::getFloat(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties && _meta_data->mValues[index].mType == AI_FLOAT);
	return *((float*)_meta_data->mValues[index].mData);
}

const char* MetaData::getString(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties && _meta_data->mValues[index].mType == AI_AISTRING);
	return ((aiString*)_meta_data->mValues[index].mData)->C_Str();
}

const float* MetaData::getVec3(unsigned int index) const
{
	assert(_meta_data && index < _meta_data->mNumProperties && _meta_data->mValues[index].mType == AI_AIVECTOR3D);
	return &((aiVector3D*)_meta_data->mValues[index].mData)->x;
}

bool MetaData::valid(void) const
{
	return _meta_data != nullptr;
}

const MetaData& MetaData::operator=(const MetaData& rhs)
{
	_meta_data = rhs._meta_data;
	return *this;
}

bool MetaData::operator==(const MetaData& rhs) const
{
	return _meta_data == rhs._meta_data;
}

bool MetaData::operator!=(const MetaData& rhs) const
{
	return _meta_data != rhs._meta_data;
}

NS_END
