/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gleam_Transform_CPU.h"

NS_GLEAM

TransformCPU::TransformCPU(const Vector4CPU& scale, const QuaternionCPU& rotation, const Vector4CPU& translation):
	_rotation(rotation), _translation(translation), _scale(scale)
{
}

TransformCPU::TransformCPU(void):
	_rotation(QuaternionCPU::Identity), _translation(0.0f, 0.0f, 0.0f, 1.0f), _scale(1.0f, 1.0f, 1.0f, 1.0f)
{
}

TransformCPU::~TransformCPU(void)
{
}

const TransformCPU& TransformCPU::operator=(const TransformCPU& rhs)
{
	_scale = rhs._scale;
	_rotation = rhs._rotation;
	_translation = rhs._translation;
	return *this;
}

bool TransformCPU::operator==(const TransformCPU& rhs) const
{
	return _scale == rhs._scale &&
			_rotation == rhs._rotation &&
			_translation == rhs._translation;
}

bool TransformCPU::operator!=(const TransformCPU& rhs) const
{
	return !(*this == rhs);
}

const TransformCPU& TransformCPU::operator+=(const TransformCPU& rhs)
{
	concatThis(rhs);
	return *this;
}

TransformCPU TransformCPU::operator+(const TransformCPU& rhs) const
{
	TransformCPU temp(_scale, _rotation, _translation);
	temp.concatThis(rhs);
	return temp;
}

const Vector4CPU& TransformCPU::getScale(void) const
{
	return _scale;
}

void TransformCPU::setScale(const Vector4CPU& scale)
{
	_scale = scale;
}

const QuaternionCPU& TransformCPU::getRotation(void) const
{
	return _rotation;
}

void TransformCPU::setRotation(const QuaternionCPU& rotation)
{
	_rotation = rotation;
}

const Vector4CPU& TransformCPU::getTranslation(void) const
{
	return _translation;
}

void TransformCPU::setTranslation(const Vector4CPU& translation)
{
	_translation = translation;
}

TransformCPU TransformCPU::concat(const TransformCPU& rhs) const
{
	return TransformCPU(_scale * rhs._scale, _rotation * rhs._rotation, _translation + rhs._translation);
}

TransformCPU TransformCPU::inverse(void) const
{
	return TransformCPU(Vector4CPU(1.0f) / _scale, _rotation.conjugate(), -_translation);
}

void TransformCPU::concatThis(const TransformCPU& rhs)
{
	_scale *= rhs._scale;
	_rotation *= rhs._rotation;
	_translation += rhs._translation;
}

void TransformCPU::inverseThis(void)
{
	_scale = Vector4CPU(1.0f) / _scale;
	_rotation.conjugateThis();
	_translation = -_translation;
}

Vector4CPU TransformCPU::transformVector(const Vector4CPU& rhs) const
{
	Vector4CPU temp = rhs * _scale;
	temp = _rotation.transform(temp);
	return temp;
}

Vector4CPU TransformCPU::transformPoint(const Vector4CPU& rhs) const
{
	Vector4CPU temp = rhs * _scale;
	temp = _rotation.transform(temp);
	temp += _translation;
	return temp;
}

Matrix4x4CPU TransformCPU::matrix(void) const
{
	Matrix4x4CPU temp = _rotation.matrix();
	temp.setTranslate(_translation);
	temp *= Matrix4x4CPU::MakeScale(_scale);
	return temp;
}

TransformCPU TransformCPU::lerp(const TransformCPU& end, const Vector4CPU& t)
{
	return TransformCPU(
		_scale.lerp(end._scale, t),
		_rotation.slerp(end._rotation, t),
		_translation.lerp(end._translation, t)
	);
}

void TransformCPU::lerpThis(const TransformCPU& end, const Vector4CPU& t)
{
	_scale.lerpThis(end._scale, t);
	_rotation.slerpThis(end._rotation, t);
	_translation.lerpThis(end._translation, t);
}

TransformCPU TransformCPU::lerp(const TransformCPU& end, float t)
{
	return TransformCPU(
		_scale.lerp(end._scale, t),
		_rotation.slerp(end._rotation, t),
		_translation.lerp(end._translation, t)
	);
}

void TransformCPU::lerpThis(const TransformCPU& end, float t)
{
	_scale.lerpThis(end._scale, t);
	_rotation.slerpThis(end._rotation, t);
	_translation.lerpThis(end._translation, t);
}

NS_END
