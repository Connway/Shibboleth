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

#pragma once

#include "Gleam_Defines.h"
#include <mat4x4.hpp>
#include <vec3.hpp>

NS_GLEAM

class Transform;
class AABB;

class OBB
{
public:
	OBB(const glm::vec3& center, const glm::vec3& right, const glm::vec3& up, const glm::vec3& forward);
	OBB(const AABB& aabb, const glm::mat4x4& transform);
	OBB(const AABB& aabb, const Transform& transform);
	OBB(const AABB& aabb);
	OBB(const OBB& obb);
	OBB(void);
	~OBB(void);

 	const glm::vec3& getCenter(void) const;
	glm::vec3 getExtent(void) const;

	void setAxis(int axis, const glm::vec3& vec);
	const glm::vec3& getAxis(int axis) const;
	const glm::vec3* getAxes(void) const;

	glm::vec3* generatePoints(glm::vec3* out) const;

	void transform(const glm::mat4x4& transform);
	void transform(const Transform& transform);
	bool contains(const glm::vec3& point) const;

private:
	glm::vec3 _center;
	glm::vec3 _axes[3];
};

NS_END
