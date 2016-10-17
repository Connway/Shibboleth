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

#pragma once

#include "Esprit_HashString.h"
#include "Esprit_Vector.h"
#include "Esprit_Pose.h"

NS_ESPRIT

class Skeleton
{
public:
	Skeleton(void);
	~Skeleton(void);

	INLINE size_t getNumBones(void) const;

	INLINE size_t getParentIndex(size_t bone_index) const;
	INLINE const HashString32& getName(size_t bone_index) const;
	size_t getBoneIndex(const char* name) const;

	void setReferenceTransform(size_t bone_index, const Gleam::TransformSIMD& transform);
	void addBone(size_t parent_index, const char* name = nullptr);

	// Function assumes that parent's model-space transform has already been calculated
	void calculateModelTransform(Pose& pose, size_t bone_index);
	// Assumes that bones were pushed in order, starting with root
	void calculateModelTransform(Pose& pose);

private:
	Pose _default_pose;
	Vector<size_t> _parent_indices;
	Vector<HashString32> _bone_hashes;

	GAFF_NO_COPY(Skeleton);
	GAFF_NO_MOVE(Skeleton);
};

NS_END
