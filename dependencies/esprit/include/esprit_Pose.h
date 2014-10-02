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

#include "esprit_Array.h"
#include <Gleam_Transform.h>

NS_ESPRIT

class Pose
{
public:
	Pose(void);
	~Pose(void);

	void setNumBones(unsigned int num_bones);
	unsigned int getNumBones(void) const;

	const Array<Gleam::Transform>& getLocalTransforms(void) const;
	Array<Gleam::Transform>& getLocalTransforms(void);
	const Array<Gleam::Transform>& getModelTransforms(void) const;
	Array<Gleam::Transform>& getModelTransforms(void);

private:
	Array<Gleam::Transform> _local_bone_transforms;
	Array<Gleam::Transform> _model_bone_transforms;

	GAFF_NO_COPY(Pose);
	GAFF_NO_MOVE(Pose);
};

NS_END
