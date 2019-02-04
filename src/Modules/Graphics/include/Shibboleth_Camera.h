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

#include <Shibboleth_Defines.h>
#include <Gleam_Transform.h>
#include <Gaff_Math.h>

NS_SHIBBOLETH

class Camera final
{
public:
	const Gleam::TransformRT& getTransform(void) const { return _transform; }
	Gleam::TransformRT& getTransform(void) { return _transform; }
	void setTransform(const Gleam::TransformRT& transform) { _transform = transform; }

	float getHFOVRadians(void) const { return _h_fov * Gaff::RadToDeg; }
	float getHFOV(void) const { return _h_fov; }
	void setHFOV(float h_fov) { _h_fov = h_fov; }

private:
	Gleam::TransformRT _transform;
	float _h_fov = 90.0f;
};

NS_END
