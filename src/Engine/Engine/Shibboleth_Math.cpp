/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include "Shibboleth_EngineAttributesCommon.h"

namespace
{
	template <class T>
	int32_t VecToString(const T& value, char* buffer, int32_t size)
	{
		if constexpr (std::is_same<T, glm::quat>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "glm::quat(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, glm::vec4>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "glm::vec4(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, glm::vec3>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "glm::vec3(%f, %f, %f)", value.x, value.y, value.z);
		} else if constexpr (std::is_same<T, glm::vec2>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "glm::vec2(%f, %f)", value.x, value.y);
		}
	}
}

SHIB_REFLECTION_DEFINE_BEGIN(glm::quat)
	//.ctor<const glm::mat4x4&>()
	//.ctor<const glm::mat3x3&>()
	.ctor<const glm::vec3&>()
	.ctor<float, const glm::vec3&>()
	.ctor<float, float, float, float>()
	.ctor<>()

	.opMul<glm::vec4>()
	.opMul<glm::vec3>()

	.opIndex<glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()

	.opEqual()

	.opMinus()
	.opPlus()

	.opToString< VecToString<glm::quat> >()

	.var("x", &glm::quat::x)
	.var("y", &glm::quat::y)
	.var("z", &glm::quat::z)
	.var("w", &glm::quat::w)
SHIB_REFLECTION_DEFINE_END(glm::quat)

SHIB_REFLECTION_DEFINE_BEGIN(glm::vec4)
	.ctor<const glm::vec4&>()
	.ctor<const glm::vec2&, float, float>()
	.ctor<const glm::vec3&, float>()
	.ctor<float, float, float, float>()
	.ctor<float>()
	.ctor<>()

	.opAdd<float>()
	.opSub<float>()
	.opMul<float>()
	.opDiv<float>()

	.opIndex<glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.opToString< VecToString<glm::vec4> >()

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
SHIB_REFLECTION_DEFINE_END(glm::vec4)

SHIB_REFLECTION_DEFINE_BEGIN(glm::vec3)
	.ctor<const glm::vec2&, float>()
	.ctor<const glm::vec3&>()
	.ctor<float, float, float>()
	.ctor<float>()
	.ctor<>()

	.opAdd<float>()
	.opSub<float>()
	.opMul<float>()
	.opDiv<float>()

	.opIndex<glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.opToString< VecToString<glm::vec3> >()

	.var("x", &glm::vec3::x, OptionalAttribute())
	.var("y", &glm::vec3::y, OptionalAttribute())
	.var("z", &glm::vec3::z, OptionalAttribute())

	.var("r", &glm::vec3::r, OptionalAttribute())
	.var("g", &glm::vec3::g, OptionalAttribute())
	.var("b", &glm::vec3::b, OptionalAttribute())

	.var("s", &glm::vec3::s, OptionalAttribute())
	.var("t", &glm::vec3::t, OptionalAttribute())
	.var("p", &glm::vec3::p, OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(glm::vec3)

SHIB_REFLECTION_DEFINE_BEGIN(glm::vec2)
	.ctor<const glm::vec2&>()
	.ctor<float, float>()
	.ctor<float>()
	.ctor<>()

	.opAdd<float>()
	.opSub<float>()
	.opMul<float>()
	.opDiv<float>()

	.opIndex<glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.opToString< VecToString<glm::vec2> >()

	.var("x", &glm::vec2::x, OptionalAttribute())
	.var("y", &glm::vec2::y, OptionalAttribute())

	.var("r", &glm::vec2::r, OptionalAttribute())
	.var("g", &glm::vec2::g, OptionalAttribute())

	.var("s", &glm::vec2::s, OptionalAttribute())
	.var("t", &glm::vec2::t, OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(glm::vec2)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Transform)
	.var("translation", &Gleam::Transform::getTranslation, &Gleam::Transform::setTranslation)
	.var("rotation", &Gleam::Transform::getRotation, &Gleam::Transform::setRotation)
	.var("scale", &Gleam::Transform::getScale, &Gleam::Transform::setScale)
SHIB_REFLECTION_DEFINE_END(Gleam::Transform)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Plane)
	.var("normal", &Gleam::Plane::getNormal, &Gleam::Plane::setNormal)
	.var("distance", &Gleam::Plane::getDistance, &Gleam::Plane::setDistance)
SHIB_REFLECTION_DEFINE_END(Gleam::Plane)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::AABB)
	.var("min", &Gleam::AABB::getMin, &Gleam::AABB::setMin)
	.var("max", &Gleam::AABB::getMax, &Gleam::AABB::setMax)
SHIB_REFLECTION_DEFINE_END(Gleam::AABB)
