/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_Transform_SIMD.h"

NS_GLEAM

TransformSIMD::TransformSIMD(const Vector4SIMD& scale, const QuaternionSIMD& rotation, const Vector4SIMD& translation):
	_rotation(rotation), _translation(translation), _scale(scale)
{
}

TransformSIMD::TransformSIMD(void):
	_rotation(QuaternionSIMD::Identity), _translation(0.0f, 0.0f, 0.0f, 1.0f), _scale(1.0f, 1.0f, 1.0f, 1.0f)
{
}

TransformSIMD::~TransformSIMD(void)
{
}

const TransformSIMD& TransformSIMD::operator=(const TransformSIMD& rhs)
{
	_scale = rhs._scale;
	_rotation = rhs._rotation;
	_translation = rhs._translation;
	return *this;
}

bool TransformSIMD::operator==(const TransformSIMD& rhs) const
{
	return _scale == rhs._scale &&
			_rotation == rhs._rotation &&
			_translation == rhs._translation;
}

bool TransformSIMD::operator!=(const TransformSIMD& rhs) const
{
	return !(*this == rhs);
}

const TransformSIMD& TransformSIMD::operator+=(const TransformSIMD& rhs)
{
	concatThis(rhs);
	return *this;
}

TransformSIMD TransformSIMD::operator+(const TransformSIMD& rhs) const
{
	TransformSIMD temp(_scale, _rotation, _translation);
	temp.concatThis(rhs);
	return temp;
}

const Vector4SIMD& TransformSIMD::getScale(void) const
{
	return _scale;
}

void TransformSIMD::setScale(const Vector4SIMD& scale)
{
	_scale = scale;
}

const QuaternionSIMD& TransformSIMD::getRotation(void) const
{
	return _rotation;
}

void TransformSIMD::setRotation(const QuaternionSIMD& rotation)
{
	_rotation = rotation;
}

const Vector4SIMD& TransformSIMD::getTranslation(void) const
{
	return _translation;
}

void TransformSIMD::setTranslation(const Vector4SIMD& translation)
{
	_translation = translation;
}

TransformSIMD TransformSIMD::concat(const TransformSIMD& rhs) const
{
	return TransformSIMD(_scale * rhs._scale, _rotation * rhs._rotation, _translation + rhs._translation);
}

TransformSIMD TransformSIMD::inverse(void) const
{
	return TransformSIMD(Vector4SIMD(1.0f) / _scale, _rotation.conjugate(), -_translation);
}

void TransformSIMD::concatThis(const TransformSIMD& rhs)
{
	_scale *= rhs._scale;
	_rotation *= rhs._rotation;
	_translation += rhs._translation;
}

void TransformSIMD::inverseThis(void)
{
	_scale = Vector4SIMD(1.0f) / _scale;
	_rotation.conjugateThis();
	_translation = -_translation;
}

Vector4SIMD TransformSIMD::transform(const Vector4SIMD& rhs) const
{
	Vector4SIMD temp = rhs * _scale;
	temp = _rotation.transform(temp);
	temp += _translation;
	return temp;
}

Matrix4x4SIMD TransformSIMD::matrix(void) const
{
	Matrix4x4SIMD temp = _rotation.matrix();
	temp.setTranslate(_translation);
	temp *= Matrix4x4SIMD::MakeScale(_scale);
	return temp;
}

TransformSIMD TransformSIMD::lerp(const TransformSIMD& end, const Vector4SIMD& t)
{
	return TransformSIMD(
		_scale.lerp(end._scale, t),
		_rotation.slerp(end._rotation, t),
		_translation.lerp(end._translation, t)
	);
}

void TransformSIMD::lerpThis(const TransformSIMD& end, const Vector4SIMD& t)
{
	_scale.lerpThis(end._scale, t);
	_rotation.slerpThis(end._rotation, t);
	_translation.lerpThis(end._translation, t);
}

TransformSIMD TransformSIMD::lerp(const TransformSIMD& end, float t)
{
	return lerp(end, Vector4SIMD(t));
}

void TransformSIMD::lerpThis(const TransformSIMD& end, float t)
{
	lerpThis(end, Vector4SIMD(t));
}

NS_END