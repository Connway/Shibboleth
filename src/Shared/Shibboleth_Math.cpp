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

#pragma once

#include "Shibboleth_Math.h"

SHIB_REFLECTION_BUILDER_BEGIN(glm::quat)
	.var("x", &glm::quat::x)
	.var("y", &glm::quat::y)
	.var("z", &glm::quat::z)
	.var("w", &glm::quat::w)
SHIB_REFLECTION_BUILDER_END(glm::quat)

SHIB_REFLECTION_BUILDER_BEGIN(glm::vec4)
	.var("x", &glm::vec4::x)
	.var("y", &glm::vec4::y)
	.var("z", &glm::vec4::z)
	.var("w", &glm::vec4::w)
SHIB_REFLECTION_BUILDER_END(glm::vec4)

SHIB_REFLECTION_BUILDER_BEGIN(glm::vec3)
	.var("x", &glm::vec3::x)
	.var("y", &glm::vec3::y)
	.var("z", &glm::vec3::z)
SHIB_REFLECTION_BUILDER_END(glm::vec3)

SHIB_REFLECTION_BUILDER_BEGIN(glm::vec2)
	.var("x", &glm::vec2::x)
	.var("y", &glm::vec2::y)
SHIB_REFLECTION_BUILDER_END(glm::vec2)
