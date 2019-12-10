/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include "Gleam_IncludeQuaternion.h"

NS_GLEAM

class Transform;

class TransformRT
{
public:
	TransformRT(const glm::vec3& translation = glm::vec3(), const glm::quat& rotation = glm::quat());
	TransformRT(const TransformRT& tform);
	TransformRT(const Transform& tform);

	TransformRT& operator=(const TransformRT& rhs);
	bool operator==(const TransformRT& rhs) const;
	bool operator!=(const TransformRT& rhs) const;

	TransformRT& operator+=(const TransformRT& rhs);
	TransformRT operator+(const TransformRT& rhs) const;

	const glm::quat& getRotation(void) const;
	void setRotation(const glm::quat& rotation);
	const glm::vec3& getTranslation(void) const;
	void setTranslation(const glm::vec3& translation);

	TransformRT concat(const TransformRT& rhs) const;
	TransformRT inverse(void) const;
	TransformRT& concatThis(const TransformRT& rhs);
	TransformRT& inverseThis(void);

	glm::vec3 transformVector(const glm::vec3& rhs) const;
	glm::vec3 transformPoint(const glm::vec3& rhs) const;
	glm::mat4x4 toMatrix(void) const;

	TransformRT lerp(const TransformRT& end, float t);
	TransformRT& lerpThis(const TransformRT& end, float t);

private:
	glm::vec3 _translation;
	glm::quat _rotation;
};

class Transform
{
public:
	Transform(const glm::vec3& translation = glm::vec3(), const glm::quat& rotation = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f));
	Transform(const TransformRT& tform, const glm::vec3& scale = glm::vec3(1.0f));
	Transform(const Transform& tform);

	Transform& operator=(const Transform& rhs);
	bool operator==(const Transform& rhs) const;
	bool operator!=(const Transform& rhs) const;

	Transform& operator+=(const Transform& rhs);
	Transform operator+(const Transform& rhs) const;

	const glm::vec3& getScale(void) const;
	void setScale(const glm::vec3& scale);
	const glm::quat& getRotation(void) const;
	void setRotation(const glm::quat& rotation);
	const glm::vec3& getTranslation(void) const;
	void setTranslation(const glm::vec3& translation);

	Transform concat(const Transform& rhs) const;
	Transform inverse(void) const;
	Transform& concatThis(const Transform& rhs);
	Transform& inverseThis(void);

	glm::vec3 transformVector(const glm::vec3& rhs) const;
	glm::vec3 transformPoint(const glm::vec3& rhs) const;
	glm::mat4x4 toMatrix(void) const;

	Transform lerp(const Transform& end, float t);
	Transform& lerpThis(const Transform& end, float t);

private:
	glm::vec3 _translation;
	glm::quat _rotation;
	glm::vec3 _scale;
};

NS_END
