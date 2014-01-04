/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_Quaternion.h"
#include "Gleam_Matrix4x4.h"
#include "Gleam_Vector4.h"

NS_GLEAM

class Transform
{
public:
	Transform(const Vec4& scale, const Quaternion& rotation, const Vec4& translation);
	Transform(void);
	~Transform(void);

	const Transform& operator=(const Transform& rhs);
	bool operator==(const Transform& rhs) const;
	bool operator!=(const Transform& rhs) const;

	const Transform& operator+=(const Transform& rhs);
	Transform operator+(const Transform& rhs) const;

	const Vec4& getScale(void) const;
	void setScale(const Vec4& scale);
	const Quaternion& getRotation(void) const;
	void setRotation(const Quaternion& rotation);
	const Vec4& getTranslation(void) const;
	void setTranslation(const Vec4& translation);

	void concat(const Transform& rhs);
	void inverse(void);

	Matrix4x4 matrix(void) const;

private:
	Quaternion _rotation;
	Vec4 _translation;
	Vec4 _scale;
};

NS_END
