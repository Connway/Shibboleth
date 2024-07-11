/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

class TransformRT;
class Transform;

class TransformRTEuler
{
public:
	TransformRTEuler(const Vec3& translation = glm::zero<Vec3>(), const Vec3& rotation = glm::zero<Vec3>());
	TransformRTEuler(const TransformRTEuler& tform) = default;
	explicit TransformRTEuler(const TransformRT& tform);
	explicit TransformRTEuler(const Transform& tform);

	TransformRTEuler& operator=(const TransformRTEuler& rhs) = default;
	bool operator==(const TransformRTEuler& rhs) const;
	bool operator!=(const TransformRTEuler& rhs) const;

	TransformRTEuler& operator+=(const TransformRTEuler& rhs);
	TransformRTEuler operator+(const TransformRTEuler& rhs) const;

	const Vec3& getRotation(void) const;
	void setRotation(const Vec3& rotation);
	Quat getRotationQuatTurns(void) const;
	Quat getRotationQuat(void) const;
	void setRotationQuatTurns(const Quat& rotation);
	void setRotationQuat(const Quat& rotation);
	const Vec3& getTranslation(void) const;
	void setTranslation(const Vec3& translation);

	TransformRTEuler append(const TransformRTEuler& rhs) const;
	TransformRTEuler concat(const TransformRTEuler& rhs) const;
	TransformRTEuler inverse(void) const;
	TransformRTEuler& appendThis(const TransformRTEuler& rhs);
	TransformRTEuler& concatThis(const TransformRTEuler& rhs);
	TransformRTEuler& inverseThis(void);

	Vec3 transformVector(const Vec3& rhs) const;
	Vec3 transformPoint(const Vec3& rhs) const;
	Mat4x4 toMatrixTurns(void) const;
	Mat4x4 toMatrix(void) const;

	TransformRTEuler lerp(const TransformRTEuler& end, float t);
	TransformRTEuler& lerpThis(const TransformRTEuler& end, float t);

private:
	Vec3 _translation = glm::zero<Vec3>();
	Vec3 _rotation = glm::zero<Vec3>();
};

class TransformRT
{
public:
	TransformRT(const Vec3& translation = glm::zero<Vec3>(), const Quat& rotation = glm::identity<Quat>());
	TransformRT(const TransformRTEuler& tform);
	TransformRT(const TransformRT& tform) = default;
	explicit TransformRT(const Transform& tform);

	TransformRT& operator=(const TransformRT& rhs) = default;
	bool operator==(const TransformRT& rhs) const;
	bool operator!=(const TransformRT& rhs) const;

	TransformRT& operator+=(const TransformRT& rhs);
	TransformRT operator+(const TransformRT& rhs) const;

	const Quat& getRotation(void) const;
	void setRotation(const Quat& rotation);
	Vec3 getRotationEulerTurns(void) const;
	Vec3 getRotationEuler(void) const;
	void setRotationEulerTurns(const Vec3& rotation);
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
	Vec3 _translation = glm::zero<Vec3>();
	Quat _rotation = glm::identity<Quat>();
};

class Transform
{
public:
	Transform(const Vec3& translation = glm::zero<Vec3>(), const Quat& rotation = glm::identity<Quat>(), const Vec3& scale = Vec3(1.0f));
	explicit Transform(const TransformRTEuler& tform, const Vec3& scale = Vec3(1.0f));
	explicit Transform(const TransformRT& tform, const Vec3& scale = Vec3(1.0f));
	Transform(const Transform& tform) = default;

	Transform& operator=(const Transform& rhs) = default;
	bool operator==(const Transform& rhs) const;
	bool operator!=(const Transform& rhs) const;

	Transform& operator+=(const Transform& rhs);
	Transform operator+(const Transform& rhs) const;

	const Vec3& getScale(void) const;
	void setScale(const Vec3& scale);
	void setScale(float scale);
	const Quat& getRotation(void) const;
	void setRotation(const Quat& rotation);
	Vec3 getRotationEulerTurns(void) const;
	Vec3 getRotationEuler(void) const;
	void setRotationEulerTurns(const Vec3& rotation);
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
	Vec3 _translation = glm::zero<Vec3>();
	Quat _rotation = glm::identity<Quat>();
	Vec3 _scale{ 1.0f };
};

NS_END
