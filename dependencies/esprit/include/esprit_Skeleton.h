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

#pragma once

#include "esprit_String.h"
#include "esprit_Array.h"
#include "esprit_Pose.h"

NS_ESPRIT

class Skeleton
{
public:
	Skeleton(void);
	~Skeleton(void);

	unsigned int getNumBones(void) const;

	INLINE unsigned int getParentIndex(unsigned int bone_index) const;
	INLINE const AString& getName(unsigned int bone_index) const;
	unsigned int getBoneIndex(const char* name) const;

	void setReferenceTransform(unsigned int bone_index, const Gleam::Transform& transform);
	void addBone(unsigned int parent_index, const char* name = nullptr);

	// Function assumes that parent's model-space transform has already been calculated
	void calculateModelTransform(Pose& pose, unsigned int bone_index);
	// Assumes that bones were pushed in order, starting with root
	void calculateModelTransform(Pose& pose);

private:
	Pose _default_pose;
	Array<unsigned int> _parent_indices;
	Array<AString> _names;

	GAFF_NO_COPY(Skeleton);
	GAFF_NO_MOVE(Skeleton);
};

NS_END