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

#include "Gleam_Transform.h"
#include <Gaff_Math.h>
#include <glm/gtx/euler_angles.hpp>

NS_GLEAM

TransformRTEuler::TransformRTEuler(const Vec3& translation , const Vec3& rotation):
	_translation(translation), _rotation(rotation)
{
}

TransformRTEuler::TransformRTEuler(const TransformRT& tform):
	_translation(tform.getTranslation()), _rotation(tform.getRotationEuler())
{
}

TransformRTEuler::TransformRTEuler(const Transform& tform):
	_translation(tform.getTranslation()), _rotation(tform.getRotationEuler())
{
}

bool TransformRTEuler::operator==(const TransformRTEuler& rhs) const
{
	return _translation == rhs._translation && _rotation == rhs._rotation;
}

bool TransformRTEuler::operator!=(const TransformRTEuler& rhs) const
{
	return _translation != rhs._translation || _rotation != rhs._rotation;
}

TransformRTEuler& TransformRTEuler::operator+=(const TransformRTEuler& rhs)
{
	return concatThis(rhs);
}

TransformRTEuler TransformRTEuler::operator+(const TransformRTEuler& rhs) const
{
	return concat(rhs);
}

const Vec3& TransformRTEuler::getRotation(void) const
{
	return _rotation;
}

void TransformRTEuler::setRotation(const Vec3& rotation)
{
	_rotation = rotation;
}

Quat TransformRTEuler::getRotationQuatTurns(void) const
{
	return Quat(_rotation * Gaff::TurnsToRadians);
}

Quat TransformRTEuler::getRotationQuat(void) const
{
	return Quat(_rotation);
}

void TransformRTEuler::setRotationQuatTurns(const Quat& rotation)
{
	_rotation = glm::eulerAngles(rotation) * Gaff::RadiansToTurns;
}

void TransformRTEuler::setRotationQuat(const Quat& rotation)
{
	_rotation = glm::eulerAngles(rotation);
}

const Vec3& TransformRTEuler::getTranslation(void) const
{
	return _translation;
}

void TransformRTEuler::setTranslation(const Vec3& translation)
{
	_translation = translation;
}

TransformRTEuler TransformRTEuler::append(const TransformRTEuler& rhs) const
{
	return TransformRTEuler {
		_translation + rhs._translation,
		_rotation + rhs._rotation
	};
}

TransformRTEuler TransformRTEuler::concat(const TransformRTEuler& rhs) const
{
	return TransformRTEuler{ TransformRT{ *this }.concat(TransformRT{ rhs }) };
}

TransformRTEuler TransformRTEuler::inverse(void) const
{
	return TransformRTEuler(-_translation, -_rotation);
}

TransformRTEuler& TransformRTEuler::appendThis(const TransformRTEuler& rhs)
{
	_translation += rhs._translation;
	_rotation += rhs._rotation;
	return *this;
}

TransformRTEuler& TransformRTEuler::concatThis(const TransformRTEuler& rhs)
{
	*this = concat(rhs);
	return *this;
}

TransformRTEuler& TransformRTEuler::inverseThis(void)
{
	_translation = -_translation;
	_rotation = -_rotation;
}

Vec3 TransformRTEuler::transformVector(const Vec3& rhs) const
{
	return TransformRT{ *this }.transformVector(rhs);
}

Vec3 TransformRTEuler::transformPoint(const Vec3& rhs) const
{
	return TransformRT{ *this }.transformPoint(rhs);
}

Mat4x4 TransformRTEuler::toMatrix(void) const
{
	return glm::yawPitchRoll(_rotation.y, _rotation.x, _rotation.z);
}

TransformRTEuler TransformRTEuler::lerp(const TransformRTEuler& end, float t)
{
	return TransformRTEuler{
		_translation + t * (end._translation - _translation),
		// $TODO: Shortest path rotation.
		_rotation + t * (end._rotation - _rotation)
	};
}

TransformRTEuler& TransformRTEuler::lerpThis(const TransformRTEuler& end, float t)
{
	_translation += t * (end._translation - _translation);
	// $TODO: Shortest path rotation.
	_rotation += t * (end._rotation - _rotation);

	return *this;
}



TransformRT::TransformRT(const Vec3& translation, const Quat& rotation):
	_translation(translation), _rotation(rotation)
{
}

TransformRT::TransformRT(const TransformRTEuler& tform):
	_translation(tform.getTranslation()), _rotation(tform.getRotationQuat())
{
}

TransformRT::TransformRT(const Transform& tform):
	_translation(tform.getTranslation()), _rotation(tform.getRotation())
{
}

bool TransformRT::operator==(const TransformRT& rhs) const
{
	return _translation == rhs._translation && _rotation == rhs._rotation;
}

bool TransformRT::operator!=(const TransformRT& rhs) const
{
	return _translation != rhs._translation || _rotation != rhs._rotation;
}

TransformRT& TransformRT::operator+=(const TransformRT& rhs)
{
	return concatThis(rhs);
}

TransformRT TransformRT::operator+(const TransformRT& rhs) const
{
	return concat(rhs);
}

const Quat& TransformRT::getRotation(void) const
{
	return _rotation;
}

void TransformRT::setRotation(const Quat& rotation)
{
	_rotation = rotation;
}

Vec3 TransformRT::getRotationEulerTurns(void) const
{
	return getRotationEuler() * Gaff::RadiansToTurns;
}

Vec3 TransformRT::getRotationEuler(void) const
{
	return glm::eulerAngles(_rotation);
}

void TransformRT::setRotationEulerTurns(const Vec3& rotation)
{
	setRotationEuler(rotation * Gaff::TurnsToRadians);
}

void TransformRT::setRotationEuler(const Vec3& rotation)
{
	_rotation = Quat(rotation);
}

const Vec3& TransformRT::getTranslation(void) const
{
	return _translation;
}

void TransformRT::setTranslation(const Vec3& translation)
{
	_translation = translation;
}

TransformRT TransformRT::concat(const TransformRT& rhs) const
{
	return TransformRT(
		_translation + _rotation * rhs._translation,
		_rotation * rhs._rotation
	);
}

TransformRT TransformRT::inverse(void) const
{
	return TransformRT(
		-_translation,
		glm::inverse(_rotation)
	);
}

TransformRT& TransformRT::concatThis(const TransformRT& rhs)
{
	_translation += _rotation * rhs._translation;
	_rotation *= rhs._rotation;
	return *this;
}

TransformRT& TransformRT::inverseThis(void)
{
	_translation = -_translation;
	_rotation = glm::inverse(_rotation);
	return *this;
}

Vec3 TransformRT::transformVector(const Vec3& rhs) const
{
	return _rotation * rhs;
}

Vec3 TransformRT::transformPoint(const Vec3& rhs) const
{
	return _translation * (_rotation * rhs);
}

Mat4x4 TransformRT::toMatrix(void) const
{
	Mat4x4 matrix = glm::mat4_cast(_rotation);
	matrix[3][0] = _translation.x;
	matrix[3][1] = _translation.y;
	matrix[3][2] = _translation.z;

	return matrix;
}

TransformRT TransformRT::lerp(const TransformRT& end, float t)
{
	return TransformRT(
		_translation + t * (end._translation - _translation),
		glm::slerp(_rotation, end._rotation, t)
	);
}

TransformRT& TransformRT::lerpThis(const TransformRT& end, float t)
{
	_translation += t * (end._translation - _translation);
	_rotation = glm::slerp(_rotation, end._rotation, t);
	return *this;
}



Transform::Transform(const Vec3& translation, const Quat& rotation, const Vec3& scale):
	_translation(translation), _rotation(rotation), _scale(scale)
{
}

Transform::Transform(const TransformRTEuler& tform, const Vec3& scale):
	_translation(tform.getTranslation()), _rotation(tform.getRotationQuat()), _scale(scale)
{
}

Transform::Transform(const TransformRT& tform, const Vec3& scale):
	_translation(tform.getTranslation()), _rotation(tform.getRotation()), _scale(scale)
{
}

bool Transform::operator==(const Transform& rhs) const
{
	return _translation == rhs._translation &&
			_rotation == rhs._rotation &&
			_scale == rhs._scale;
}

bool Transform::operator!=(const Transform& rhs) const
{
	return _translation != rhs._translation ||
		_rotation != rhs._rotation ||
		_scale != rhs._scale;
}

Transform& Transform::operator+=(const Transform& rhs)
{
	return concatThis(rhs);
}

Transform Transform::operator+(const Transform& rhs) const
{
	return concat(rhs);
}

const Vec3& Transform::getScale(void) const
{
	return _scale;
}

void Transform::setScale(const Vec3& scale)
{
	_scale = scale;
}

void Transform::setScale(float scale)
{
	_scale = Vec3(scale);
}

const Quat& Transform::getRotation(void) const
{
	return _rotation;
}

void Transform::setRotation(const Quat& rotation)
{
	_rotation = rotation;
}

Vec3 Transform::getRotationEulerTurns(void) const
{
	return getRotationEuler() * Gaff::RadiansToTurns;
}

Vec3 Transform::getRotationEuler(void) const
{
	return glm::eulerAngles(_rotation);
}

void Transform::setRotationEulerTurns(const Vec3& rotation)
{
	setRotationEuler(rotation * Gaff::TurnsToRadians);
}

void Transform::setRotationEuler(const Vec3& rotation)
{
	_rotation = Quat(rotation);
}

const Vec3& Transform::getTranslation(void) const
{
	return _translation;
}

void Transform::setTranslation(const Vec3& translation)
{
	_translation = translation;
}

Transform Transform::concat(const Transform& rhs) const
{
	return Transform(
		// Need to rotate translation so it is appropriately relative to the parent transform.
		_translation + _rotation * rhs._translation,
		_rotation * rhs._rotation,
		_scale * rhs._scale
	);
}

Transform Transform::inverse(void) const
{
	return Transform(
		-_translation,
		glm::inverse(_rotation),
		Vec3(1.0f) / _scale
	);
}

Transform& Transform::concatThis(const Transform& rhs)
{
	// Need to rotate translation so it is appropriately relative to the parent transform.
	_translation += _rotation * rhs._translation;
	_rotation *= rhs._rotation;
	_scale *= rhs._scale;
	return *this;
}

Transform& Transform::inverseThis(void)
{
	_translation = -_translation;
	_rotation = glm::inverse(_rotation);
	_scale = Vec3(1.0f) / _scale;
	return *this;
}

Vec3 Transform::transformVector(const Vec3& rhs) const
{
	return _rotation * (_scale * rhs);
}

Vec3 Transform::transformPoint(const Vec3& rhs) const
{
	return _translation + transformVector(rhs);
}

Mat4x4 Transform::toMatrix(void) const
{
	Mat4x4 matrix = glm::mat4_cast(_rotation);
	matrix[3] = Vec4(_translation, 1.0f);
	return glm::scale(matrix, _scale);
}

Transform Transform::lerp(const Transform& end, float t)
{
	return Transform(
		_translation + t * (end._translation - _translation),
		glm::slerp(_rotation, end._rotation, t),
		_scale + t * (end._scale - _scale)
	);
}

Transform& Transform::lerpThis(const Transform& end, float t)
{
	_translation += t * (end._translation - _translation);
	_rotation = glm::slerp(_rotation, end._rotation, t);
	_scale += t * (end._scale - _scale);
	return *this;
}

NS_END
