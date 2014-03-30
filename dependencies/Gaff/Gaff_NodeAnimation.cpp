/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gaff_NodeAnimation.h"
#include "Gaff_IncludeAssert.h"

NS_GAFF

////////////////////////
//    Rotation Key    //
////////////////////////
RotationKey::RotationKey(const aiQuatKey* key):
	_key(key)
	//_time(key->mTime)
{
	//_quat[0] = key->mValue.w;
	//_quat[1] = key->mValue.x;
	//_quat[2] = key->mValue.y;
	//_quat[3] = key->mValue.z;
}

RotationKey::RotationKey(const RotationKey& key):
	_key(key._key)
	//_time(key._time)
{
	//_quat[0] = rhs._quat[0];
	//_quat[1] = rhs._quat[1];
	//_quat[2] = rhs._quat[2];
	//_quat[3] = rhs._quat[3];
}

RotationKey::RotationKey(void):
	_key(nullptr)
	//_time(0.0)
{
	//_quat[0] = 0.0f;
	//_quat[1] = 0.0f;
	//_quat[2] = 0.0f;
	//_quat[3] = 0.0f;
}

RotationKey::~RotationKey(void)
{
}

double RotationKey::getTime(void) const
{
	assert(_key);
	return _key->mTime;
	//return _time;
}

const float* RotationKey::getQuaternion(void) const
{
	assert(_key);
	return &_key->mValue.w;
	//return _quat;
}

bool RotationKey::valid(void) const
{
	return _key != nullptr;
}

const RotationKey& RotationKey::operator=(const RotationKey& rhs)
{
	_key = rhs._key;
	//_time = rhs._time;
	//_quat[0] = rhs._quat[0];
	//_quat[1] = rhs._quat[1];
	//_quat[2] = rhs._quat[2];
	//_quat[3] = rhs._quat[3];
	return *this;
}

bool RotationKey::operator==(const RotationKey& rhs) const
{
	assert(_key);
	return *_key == *rhs._key;
	//return _quat[0] == rhs._quat[0] &&
	//	_quat[1] == rhs._quat[1] &&
	//	_quat[2] == rhs._quat[2] &&
	//	_quat[3] == rhs._quat[3];
}

bool RotationKey::operator!=(const RotationKey& rhs) const
{
	assert(_key);
	return *_key != *rhs._key;
	//return _quat[0] != rhs._quat[0] ||
	//	_quat[1] != rhs._quat[1] ||
	//	_quat[2] != rhs._quat[2] ||
	//	_quat[3] != rhs._quat[3];
}

bool RotationKey::operator<(const RotationKey& rhs) const
{
	assert(_key);
	return *_key < *rhs._key;
	//return _time < rhs._time;
}

bool RotationKey::operator>(const RotationKey& rhs) const
{
	assert(_key);
	return *_key > *rhs._key;
	//return _time > rhs._time;
}

//////////////////////
//    Vector Key    //
//////////////////////
VectorKey::VectorKey(const aiVectorKey* key):
	_key(key)
	//_time(key->mTime)
{
	//_vec[0] = key->mValue.w;
	//_vec[1] = key->mValue.x;
	//_vec[2] = key->mValue.y;
}

VectorKey::VectorKey(const VectorKey& key):
	_key(key._key)
	//_time(key._time)
{
	//_vec[0] = rhs._vec[0];
	//_vec[1] = rhs._vec[1];
	//_vec[2] = rhs._vec[2];
}

VectorKey::VectorKey(void):
	_key(nullptr)
	//_time(0.0)
{
	//_vec[0] = 0.0f;
	//_vec[1] = 0.0f;
	//_vec[2] = 0.0f;
}

VectorKey::~VectorKey(void)
{
}

double VectorKey::getTime(void) const
{
	assert(_key);
	return _key->mTime;
	//return _time;
}

const float* VectorKey::getVector(void) const
{
	assert(_key);
	return &_key->mValue.x;
	//return _vec;
}

bool VectorKey::valid(void) const
{
	return _key != nullptr;
}

const VectorKey& VectorKey::operator=(const VectorKey& rhs)
{
	_key = rhs._key;
	//_time = rhs._time;
	//_vec[0] = rhs._vec[0];
	//_vec[1] = rhs._vec[1];
	//_vec[2] = rhs._vec[2];
	return *this;
}

bool VectorKey::operator==(const VectorKey& rhs) const
{
	assert(_key);
	return *_key == *rhs._key;
	//return _vec[0] == rhs._vec[0] &&
	//	_vec[1] == rhs._vec[1] &&
	//	_vec[2] == rhs._vec[2];
}

bool VectorKey::operator!=(const VectorKey& rhs) const
{
	assert(_key);
	return *_key != *rhs._key;
	//return _vec[0] != rhs._vec[0] ||
	//	_vec[1] != rhs._vec[1] ||
	//	_vec[2] != rhs._vec[2];
}

bool VectorKey::operator<(const VectorKey& rhs) const
{
	assert(_key);
	return *_key < *rhs._key;
	//return _time < rhs._time;
}

bool VectorKey::operator>(const VectorKey& rhs) const
{
	assert(_key);
	return *_key > *rhs._key;
	//return _time > rhs._time;
}


//////////////////////////
//    Node Animation    //
//////////////////////////
NodeAnimation::NodeAnimation(const aiNodeAnim* node_animation):
	_node_animation(node_animation)
{
}

NodeAnimation::NodeAnimation(const NodeAnimation& node_animation):
	_node_animation(node_animation._node_animation)
{
}

NodeAnimation::NodeAnimation(void):
	_node_animation(nullptr)
{
}

NodeAnimation::~NodeAnimation(void)
{
}

const char* NodeAnimation::getName(void) const
{
	assert(_node_animation);
	return _node_animation->mNodeName.C_Str();
}

unsigned int NodeAnimation::getNumPositionKeys(void) const
{
	assert(_node_animation);
	return _node_animation->mNumPositionKeys;
}

unsigned int NodeAnimation::getNumRotationKeys(void) const
{
	assert(_node_animation);
	return _node_animation->mNumRotationKeys;
}

unsigned int NodeAnimation::getNumScaleKeys(void) const
{
	assert(_node_animation);
	return _node_animation->mNumScalingKeys;
}

AnimationBehavior NodeAnimation::getPreAnimState(void) const
{
	assert(_node_animation);
	return (AnimationBehavior)_node_animation->mPreState;
}

AnimationBehavior NodeAnimation::getPostAnimState(void) const
{
	assert(_node_animation);
	return (AnimationBehavior)_node_animation->mPostState;
}

VectorKey NodeAnimation::getPositionKey(unsigned int key) const
{
	assert(_node_animation && key < _node_animation->mNumPositionKeys);
	return VectorKey(_node_animation->mPositionKeys + key);
}

RotationKey NodeAnimation::getRotationKey(unsigned int key) const
{
	assert(_node_animation && key < _node_animation->mNumRotationKeys);
	return RotationKey(_node_animation->mRotationKeys + key);
}

VectorKey NodeAnimation::getScaleKey(unsigned int key) const
{
	assert(_node_animation && key < _node_animation->mNumScalingKeys);
	return VectorKey(_node_animation->mScalingKeys + key);
}

bool NodeAnimation::valid(void) const
{
	return _node_animation != nullptr;
}

const NodeAnimation& NodeAnimation::operator=(const NodeAnimation& rhs)
{
	_node_animation = rhs._node_animation;
	return *this;
}

bool NodeAnimation::operator==(const NodeAnimation& rhs) const
{
	return _node_animation == rhs._node_animation;
}

bool NodeAnimation::operator!=(const NodeAnimation& rhs) const
{
	return _node_animation != rhs._node_animation;
}

NS_END
