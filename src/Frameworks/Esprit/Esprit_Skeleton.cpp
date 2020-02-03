/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Esprit_Skeleton.h"
#include <Gaff_Assert.h>

NS_ESPRIT

Skeleton::Skeleton(void)
{
}

Skeleton::~Skeleton(void)
{
}

int32_t Skeleton::getNumBones(void) const
{
	return static_cast<int32_t>(_parent_indices.size());
}

int32_t Skeleton::getParentIndex(int32_t bone_index) const
{
	GAFF_ASSERT(bone_index < static_cast<int32_t>(_parent_indices.size()));
	return _parent_indices[bone_index];
}

const Gaff::Hash32& Skeleton::getNameHash(int32_t bone_index) const
{
	GAFF_ASSERT(bone_index < static_cast<int32_t>(_bone_hashes.size()));
	return _bone_hashes[bone_index];
}

int32_t Skeleton::getBoneIndex(Gaff::Hash32 name) const
{
	const int32_t size = static_cast<int32_t>(_bone_hashes.size());

	for (int32_t i = 0; i < size; ++i) {
		if (_bone_hashes[i] == name) {
			return i;
		}
	}

	return -1;
}

void Skeleton::setReferenceTransform(int32_t bone_index, const Gleam::Transform& transform)
{
	GAFF_ASSERT(bone_index < static_cast<int32_t>(_parent_indices.size()));
	_default_pose.getLocalTransforms()[bone_index] = transform;
}

void Skeleton::addBone(int32_t parent_index, Gaff::Hash32 name)
{
	_parent_indices.push_back(parent_index);
	_bone_hashes.push_back(name);
	_default_pose.setNumBones(_parent_indices.size());
}

void Skeleton::calculateModelTransform(Pose& pose, int32_t bone_index)
{
	GAFF_ASSERT(bone_index < static_cast<int32_t>(_parent_indices.size()));

	Gleam::Transform& bone_transform = pose.getModelTransforms()[bone_index];
	int32_t parent_index = _parent_indices[bone_index];

	if (parent_index > -1) {
		bone_transform = pose.getModelTransforms()[parent_index] + pose.getLocalTransforms()[bone_index];
	} else {
		bone_transform = pose.getLocalTransforms()[bone_index];
	}
}

void Skeleton::calculateModelTransform(Pose& pose)
{
	const int32_t size = static_cast<int32_t>(_parent_indices.size());

	for (int32_t i = 0; i < size; ++i) {
		calculateModelTransform(pose, i);
	}
}

NS_END
