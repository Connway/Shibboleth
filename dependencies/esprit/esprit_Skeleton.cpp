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

#include "esprit_Skeleton.h"

NS_ESPRIT

Skeleton::Skeleton(void)
{
}

Skeleton::~Skeleton(void)
{
}

unsigned int Skeleton::getNumBones(void) const
{
	return _parent_indices.size();
}

unsigned int Skeleton::getParentIndex(unsigned int bone_index) const
{
	assert(bone_index < _parent_indices.size());
	return _parent_indices[bone_index];
}

const AString& Skeleton::getName(unsigned int bone_index) const
{
	assert(bone_index < _names.size());
	return _names[bone_index];
}

unsigned int Skeleton::getBoneIndex(const char* name) const
{
	for (unsigned int i = 0; i < _names.size(); ++i) {
		if (_names[i] == name) {
			return i;
		}
	}

	return UINT_FAIL;
}

void Skeleton::setReferenceTransform(unsigned int bone_index, const Gleam::Transform& transform)
{
	assert(bone_index < _parent_indices.size());
	_default_pose.getLocalTransforms()[bone_index] = transform;
}

void Skeleton::addBone(unsigned int parent_index, const char* name)
{
	AString str_name;

	if (name) {
		str_name = name;
	}

	_parent_indices.push(parent_index);
	_names.push(str_name);
	_default_pose.setNumBones(_parent_indices.size());
}

void Skeleton::calculateModelTransform(Pose& pose, unsigned int bone_index)
{
	assert(bone_index < _parent_indices.size());

	Gleam::Transform& bone_transform = pose.getModelTransforms()[bone_index];
	unsigned int parent_index = _parent_indices[bone_index];

	if (parent_index != UINT_FAIL) {
		bone_transform = pose.getModelTransforms()[parent_index] + pose.getLocalTransforms()[bone_index];
	} else {
		bone_transform = pose.getLocalTransforms()[bone_index];
	}
}

void Skeleton::calculateModelTransform(Pose& pose)
{
	for (unsigned int i = 0; i < _parent_indices.size(); ++i) {
		calculateModelTransform(pose, i);
	}
}

NS_END
