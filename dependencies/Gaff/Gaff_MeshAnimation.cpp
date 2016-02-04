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

#include "Gaff_MeshAnimation.h"
#include "Gaff_IncludeAssert.h"
#include <assimp/anim.h>

NS_GAFF

////////////////////
//    Mesh Key    //
////////////////////
MeshKey::MeshKey(const aiMeshKey* key):
	_key(key)
	//_time(key->mTime), _index(key->mValue)
{
	assert(key);
}

MeshKey::MeshKey(const MeshKey& key):
	_key(key._key)
	//_time(key._time), _index(key._index)
{
}

MeshKey::MeshKey(void):
	_key(nullptr)
	//_time(0.0), _index(0)
{
}

MeshKey::~MeshKey(void)
{
}

double MeshKey::getTime(void) const
{
	assert(_key);
	return _key->mTime;
	//return _time;
}

unsigned int MeshKey::getIndex(void) const
{
	assert(_key);
	return _key->mValue;
	//return _index;
}

bool MeshKey::valid(void) const
{
	return _key != nullptr;
}

const MeshKey& MeshKey::operator=(const MeshKey& rhs)
{
	_key = rhs._key;
	//_time = rhs._time;
	//_index = rhs._index;
	return *this;
}

bool MeshKey::operator==(const MeshKey& rhs) const
{
	assert(_key);
	return *_key == *rhs._key;
	//return _index == rhs._index;
}

bool MeshKey::operator!=(const MeshKey& rhs) const
{
	assert(_key);
	return *_key != *rhs._key;
	//return _index != rhs._index;
}

bool MeshKey::operator<(const MeshKey& rhs) const
{
	assert(_key);
	return *_key < *rhs._key;
	//return _time < rhs._time;
}

bool MeshKey::operator>(const MeshKey& rhs) const
{
	assert(_key);
	return *_key > *rhs._key;
	//return _time > rhs._time;
}


//////////////////////////
//    Mesh Animation    //
//////////////////////////
MeshAnimation::MeshAnimation(const aiMeshAnim* mesh_animation):
	_mesh_animation(mesh_animation)
{
}

MeshAnimation::MeshAnimation(const MeshAnimation& mesh_animation):
	_mesh_animation(mesh_animation._mesh_animation)
{
}

MeshAnimation::MeshAnimation(void):
	_mesh_animation(nullptr)
{
}

MeshAnimation::~MeshAnimation(void)
{
}

const char* MeshAnimation::getName(void) const
{
	assert(_mesh_animation);
	return _mesh_animation->mName.C_Str();
}

unsigned int MeshAnimation::getNumKeys(void) const
{
	assert(_mesh_animation);
	return _mesh_animation->mNumKeys;
}

MeshKey MeshAnimation::getKey(unsigned int key) const
{
	assert(_mesh_animation && key < _mesh_animation->mNumKeys);
	return MeshKey(_mesh_animation->mKeys + key);
}

bool MeshAnimation::valid(void) const
{
	return _mesh_animation != nullptr;
}

const MeshAnimation& MeshAnimation::operator=(const MeshAnimation& rhs)
{
	_mesh_animation = rhs._mesh_animation;
	return *this;
}

bool MeshAnimation::operator==(const MeshAnimation& rhs) const
{
	return _mesh_animation == rhs._mesh_animation;
}

bool MeshAnimation::operator!=(const MeshAnimation& rhs) const
{
	return _mesh_animation != rhs._mesh_animation;
}

NS_END
