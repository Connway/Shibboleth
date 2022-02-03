/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include "Gleam_IncludeColor.h"

NS_GLEAM

namespace Color
{
	using RGBA = glm::vec4;
	using HSVA = glm::vec4;
	using RGB = glm::vec3;
	using HSV = glm::vec3;

	static const RGBA Red(1.0f, 0.0f, 0.0f, 1.0f);
	static const RGBA Green(0.0f, 1.0f, 0.0f, 1.0f);
	static const RGBA Blue(0.0f, 0.0f, 1.0f, 1.0f);
	static const RGBA Black(0.0f, 0.0f, 0.0f, 1.0f);
	static const RGBA White(1.0f, 1.0f, 1.0f, 1.0f);
	static const RGBA Yellow(1.0f, 1.0f, 0.0f, 1.0f);

	static const HSVA RedHSV = HSVA(glm::hsvColor(static_cast<RGB>(Red)), 1.0f);
	static const HSVA GreenHSV = HSVA(glm::hsvColor(static_cast<RGB>(Green)), 1.0f);
	static const HSVA BlueHSV = HSVA(glm::hsvColor(static_cast<RGB>(Blue)), 1.0f);
	static const HSVA BlackHSV = HSVA(glm::hsvColor(static_cast<RGB>(Black)), 1.0f);
	static const HSVA WhiteHSV = HSVA(glm::hsvColor(static_cast<RGB>(White)), 1.0f);
	static const HSVA YellowHSV = HSVA(glm::hsvColor(static_cast<RGB>(Yellow)), 1.0f);
}

NS_END
