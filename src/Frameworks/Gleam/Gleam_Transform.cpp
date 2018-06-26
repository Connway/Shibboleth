/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gleam_Transform.h"
#include <gtc/matrix_transform.hpp>

NS_GLEAM

TransformRT::TransformRT(const glm::vec3& translation, const glm::quat& rotation):
	_translation(translation), _rotation(rotation)
{
}

TransformRT::TransformRT(const TransformRT& tform):
	_translation(tform.getTranslation()), _rotation(tform.getRotation())
{
}

TransformRT::TransformRT(const Transform& tform):
	_translation(tform.getTranslation()), _rotation(tform.getRotation())
{
}

TransformRT& TransformRT::operator=(const TransformRT& rhs)
{
	_translation = rhs.getTranslation();
	_rotation = rhs.getRotation();
	return *this;
}

bool TransformRT::operator==(const TransformRT& rhs) const
{
	return _translation == rhs._translation &&
		_rotation == rhs._rotation;
}

bool TransformRT::operator!=(const TransformRT& rhs) const
{
	return _translation != rhs._translation &&
		_rotation != rhs._rotation;
}

TransformRT& TransformRT::operator+=(const TransformRT& rhs)
{
	return concatThis(rhs);
}

TransformRT TransformRT::operator+(const TransformRT& rhs) const
{
	return concat(rhs);
}

const glm::quat& TransformRT::getRotation(void) const
{
	return _rotation;
}

void TransformRT::setRotation(const glm::quat& rotation)
{
	_rotation = rotation;
}

const glm::vec3& TransformRT::getTranslation(void) const
{
	return _translation;
}

void TransformRT::setTranslation(const glm::vec3& translation)
{
	_translation = translation;
}

TransformRT TransformRT::concat(const TransformRT& rhs) const
{
	return TransformRT(
		_translation + rhs._translation,
		_rotation * rhs._rotation
	);
}

TransformRT TransformRT::inverse(void) const
{
	return Transform(
		-_translation,
		glm::inverse(_rotation)
	);
}

TransformRT& TransformRT::concatThis(const TransformRT& rhs)
{
	_translation += rhs._translation;
	_rotation *= rhs._rotation;
	return *this;
}

TransformRT& TransformRT::inverseThis(void)
{
	_translation = -_translation;
	_rotation = glm::inverse(_rotation);
	return *this;
}

glm::vec3 TransformRT::transformVector(const glm::vec3& rhs) const
{
	return _rotation * rhs;
}

glm::vec3 TransformRT::transformPoint(const glm::vec3& rhs) const
{
	return _translation * (_rotation * rhs);
}

glm::mat4x4 TransformRT::toMatrix(void) const
{
	glm::mat4x4 matrix = glm::mat4_cast(_rotation);
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



Transform::Transform(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale):
	_translation(translation), _rotation(rotation), _scale(scale)
{
}

Transform::Transform(const TransformRT& tform, const glm::vec3& scale):
	_translation(tform.getTranslation()), _rotation(tform.getRotation()), _scale(scale)
{
}

Transform::Transform(const Transform& tform):
	_translation(tform._translation), _rotation(tform._rotation), _scale(tform._scale)
{
}

Transform& Transform::operator=(const Transform& rhs)
{
	_translation = rhs._translation;
	_rotation = rhs._rotation;
	_scale = rhs._scale;
	return *this;
}

bool Transform::operator==(const Transform& rhs) const
{
	return _translation == rhs._translation &&
			_rotation == rhs._rotation &&
			_scale == rhs._scale;
}

bool Transform::operator!=(const Transform& rhs) const
{
	return _translation != rhs._translation &&
		_rotation != rhs._rotation &&
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

const glm::vec3& Transform::getScale(void) const
{
	return _scale;
}

void Transform::setScale(const glm::vec3& scale)
{
	_scale = scale;
}

const glm::quat& Transform::getRotation(void) const
{
	return _rotation;
}

void Transform::setRotation(const glm::quat& rotation)
{
	_rotation = rotation;
}

const glm::vec3& Transform::getTranslation(void) const
{
	return _translation;
}

void Transform::setTranslation(const glm::vec3& translation)
{
	_translation = translation;
}

Transform Transform::concat(const Transform& rhs) const
{
	return Transform(
		_translation + rhs._translation,
		_rotation * rhs._rotation,
		_scale * rhs._scale
	);
}

Transform Transform::inverse(void) const
{
	return Transform(
		-_translation,
		glm::inverse(_rotation),
		glm::vec3(1.0f) / _scale
	);
}

Transform& Transform::concatThis(const Transform& rhs)
{
	_translation += rhs._translation;
	_rotation *= rhs._rotation;
	_scale *= rhs._scale;
	return *this;
}

Transform& Transform::inverseThis(void)
{
	_translation = -_translation;
	_rotation = glm::inverse(_rotation);
	_scale = glm::vec3(1.0f) / _scale;
	return *this;
}

glm::vec3 Transform::transformVector(const glm::vec3& rhs) const
{
	return _rotation * (_scale * rhs);
}

glm::vec3 Transform::transformPoint(const glm::vec3& rhs) const
{
	return _translation * (_rotation * (_scale * rhs));
}

glm::mat4x4 Transform::toMatrix(void) const
{
	glm::mat4x4 matrix = glm::mat4_cast(_rotation);
	matrix[3][0] = _translation.x;
	matrix[3][1] = _translation.y;
	matrix[3][2] = _translation.z;

	return glm::scale(glm::mat4x4(1.0f), _scale) * matrix;
}

Transform Transform::lerp(const Transform& end, float t)
{
	return Transform(
		_translation  + t * (end._translation - _translation),
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
