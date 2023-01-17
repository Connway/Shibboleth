/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Gleam_Matrix4x4.h"
#include "Gleam_Quaternion.h"
#include "Gleam_Vec3.h"

NS_GLEAM

class Transform;

class TransformRT
{
public:
	TransformRT(const Vec3& translation = glm::zero<Vec3>(), const Quat& rotation = glm::identity<Quat>());
	TransformRT(const TransformRT& tform);
	TransformRT(const Transform& tform);

	TransformRT& operator=(const TransformRT& rhs);
	bool operator==(const TransformRT& rhs) const;
	bool operator!=(const TransformRT& rhs) const;

	TransformRT& operator+=(const TransformRT& rhs);
	TransformRT operator+(const TransformRT& rhs) const;

	const Quat& getRotation(void) const;
	void setRotation(const Quat& rotation);
	Vec3 getRotationEuler(void) const;
	void setRotationEuler(const Vec3& rotation);
	const Vec3& getTranslation(void) const;
	void setTranslation(const Vec3& translation);

	TransformRT concat(const TransformRT& rhs) const;
	TransformRT inverse(void) const;
	TransformRT& concatThis(const TransformRT& rhs);
	TransformRT& inverseThis(void);

	Vec3 transformVector(const Vec3& rhs) const;
	Vec3 transformPoint(const Vec3& rhs) const;
	Mat4x4 toMatrix(void) const;

	TransformRT lerp(const TransformRT& end, float t);
	TransformRT& lerpThis(const TransformRT& end, float t);

private:
	Vec3 _translation;
	Quat _rotation;
};

class Transform
{
public:
	Transform(const Vec3& translation = glm::zero<Vec3>(), const Quat& rotation = glm::identity<Quat>(), const Vec3& scale = Vec3(1.0f));
	Transform(const TransformRT& tform, const Vec3& scale = Vec3(1.0f));
	Transform(const Transform& tform);

	Transform& operator=(const Transform& rhs);
	bool operator==(const Transform& rhs) const;
	bool operator!=(const Transform& rhs) const;

	Transform& operator+=(const Transform& rhs);
	Transform operator+(const Transform& rhs) const;

	const Vec3& getScale(void) const;
	void setScale(const Vec3& scale);
	void setScale(float scale);
	const Quat& getRotation(void) const;
	void setRotation(const Quat& rotation);
	Vec3 getRotationEuler(void) const;
	void setRotationEuler(const Vec3& rotation);
	const Vec3& getTranslation(void) const;
	void setTranslation(const Vec3& translation);

	Transform concat(const Transform& rhs) const;
	Transform inverse(void) const;
	Transform& concatThis(const Transform& rhs);
	Transform& inverseThis(void);

	Vec3 transformVector(const Vec3& rhs) const;
	Vec3 transformPoint(const Vec3& rhs) const;
	Mat4x4 toMatrix(void) const;

	Transform lerp(const Transform& end, float t);
	Transform& lerpThis(const Transform& end, float t);

private:
	Vec3 _translation;
	Quat _rotation;
	Vec3 _scale;
};

NS_END
