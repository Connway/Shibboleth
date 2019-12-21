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

#include "Shibboleth_Math.h"
#include "Shibboleth_OptionalAttribute.h"

SHIB_REFLECTION_EXTERNAL_DEFINE(glm::quat)
SHIB_REFLECTION_BUILDER_BEGIN(glm::quat)
	.var("x", &glm::quat::x)
	.var("y", &glm::quat::y)
	.var("z", &glm::quat::z)
	.var("w", &glm::quat::w)
SHIB_REFLECTION_BUILDER_END(glm::quat)

SHIB_REFLECTION_EXTERNAL_DEFINE(glm::vec4)
SHIB_REFLECTION_BUILDER_BEGIN(glm::vec4)
	.var("x", &glm::vec4::x, OptionalAttribute())
	.var("y", &glm::vec4::y, OptionalAttribute())
	.var("z", &glm::vec4::z, OptionalAttribute())
	.var("w", &glm::vec4::w, OptionalAttribute())

	.var("r", &glm::vec4::r, OptionalAttribute())
	.var("g", &glm::vec4::g, OptionalAttribute())
	.var("b", &glm::vec4::b, OptionalAttribute())
	.var("a", &glm::vec4::a, OptionalAttribute())

	.var("s", &glm::vec4::s, OptionalAttribute())
	.var("t", &glm::vec4::t, OptionalAttribute())
	.var("p", &glm::vec4::p, OptionalAttribute())
	.var("q", &glm::vec4::q, OptionalAttribute())
SHIB_REFLECTION_BUILDER_END(glm::vec4)

SHIB_REFLECTION_EXTERNAL_DEFINE(glm::vec3)
SHIB_REFLECTION_BUILDER_BEGIN(glm::vec3)
	.var("x", &glm::vec3::x, OptionalAttribute())
	.var("y", &glm::vec3::y, OptionalAttribute())
	.var("z", &glm::vec3::z, OptionalAttribute())

	.var("r", &glm::vec3::r, OptionalAttribute())
	.var("g", &glm::vec3::g, OptionalAttribute())
	.var("b", &glm::vec3::b, OptionalAttribute())

	.var("s", &glm::vec3::s, OptionalAttribute())
	.var("t", &glm::vec3::t, OptionalAttribute())
	.var("p", &glm::vec3::p, OptionalAttribute())
SHIB_REFLECTION_BUILDER_END(glm::vec3)

SHIB_REFLECTION_EXTERNAL_DEFINE(glm::vec2)
SHIB_REFLECTION_BUILDER_BEGIN(glm::vec2)
	.var("x", &glm::vec2::x, OptionalAttribute())
	.var("y", &glm::vec2::y, OptionalAttribute())

	.var("r", &glm::vec2::r, OptionalAttribute())
	.var("g", &glm::vec2::g, OptionalAttribute())

	.var("s", &glm::vec2::s, OptionalAttribute())
	.var("t", &glm::vec2::t, OptionalAttribute())
SHIB_REFLECTION_BUILDER_END(glm::vec2)

SHIB_REFLECTION_EXTERNAL_DEFINE(Gleam::Transform)
SHIB_REFLECTION_BUILDER_BEGIN(Gleam::Transform)
	.var("rotation", &Gleam::Transform::getRotation, &Gleam::Transform::setRotation)
	.var("translation", &Gleam::Transform::getTranslation, &Gleam::Transform::setTranslation)
	.var("scale", &Gleam::Transform::getScale, &Gleam::Transform::setScale)
SHIB_REFLECTION_BUILDER_END(Gleam::Transform)

SHIB_REFLECTION_EXTERNAL_DEFINE(Gleam::AABB)
SHIB_REFLECTION_BUILDER_BEGIN(Gleam::AABB)
	.var("min", &Gleam::AABB::getMin, &Gleam::AABB::setMin)
	.var("max", &Gleam::AABB::getMax, &Gleam::AABB::setMax)
SHIB_REFLECTION_BUILDER_END(Gleam::AABB)
