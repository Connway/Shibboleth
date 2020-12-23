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
		if constexpr (std::is_same<T, Gleam::Quat>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "Gleam::Quat(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, Gleam::Vec4>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "Gleam::Vec4(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, Gleam::Vec3>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "Gleam::Vec3(%f, %f, %f)", value.x, value.y, value.z);
		} else if constexpr (std::is_same<T, Gleam::Vec2>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "Gleam::Vec2(%f, %f)", value.x, value.y);
		}
	}
}

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Quat)
	//.ctor<const Gleam::Mat4x4&>()
	//.ctor<const Gleam::Mat3x3&>()
	.ctor<const Gleam::Vec3&>()
	.ctor<float, const Gleam::Vec3&>()
	.ctor<float, float, float, float>()
	.ctor<>()

	.opMul<Gleam::Vec4>()
	.opMul<Gleam::Vec3>()

	.opIndex<glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()

	.opEqual()

	.opMinus()
	.opPlus()

	.opToString< VecToString<Gleam::Quat> >()

	.var("x", &Gleam::Quat::x)
	.var("y", &Gleam::Quat::y)
	.var("z", &Gleam::Quat::z)
	.var("w", &Gleam::Quat::w)
SHIB_REFLECTION_DEFINE_END(Gleam::Quat)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Vec4)
	.ctor<const Gleam::Vec4&>()
	.ctor<const Gleam::Vec2&, float, float>()
	.ctor<const Gleam::Vec3&, float>()
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

	.opToString< VecToString<Gleam::Vec4> >()

	.var("x", &Gleam::Vec4::x, OptionalAttribute())
	.var("y", &Gleam::Vec4::y, OptionalAttribute())
	.var("z", &Gleam::Vec4::z, OptionalAttribute())
	.var("w", &Gleam::Vec4::w, OptionalAttribute())

	.var("r", &Gleam::Vec4::r, OptionalAttribute())
	.var("g", &Gleam::Vec4::g, OptionalAttribute())
	.var("b", &Gleam::Vec4::b, OptionalAttribute())
	.var("a", &Gleam::Vec4::a, OptionalAttribute())

	.var("s", &Gleam::Vec4::s, OptionalAttribute())
	.var("t", &Gleam::Vec4::t, OptionalAttribute())
	.var("p", &Gleam::Vec4::p, OptionalAttribute())
	.var("q", &Gleam::Vec4::q, OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::Vec4)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Vec3)
	.ctor<const Gleam::Vec2&, float>()
	.ctor<const Gleam::Vec3&>()
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

	.opToString< VecToString<Gleam::Vec3> >()

	.var("x", &Gleam::Vec3::x, OptionalAttribute())
	.var("y", &Gleam::Vec3::y, OptionalAttribute())
	.var("z", &Gleam::Vec3::z, OptionalAttribute())

	.var("r", &Gleam::Vec3::r, OptionalAttribute())
	.var("g", &Gleam::Vec3::g, OptionalAttribute())
	.var("b", &Gleam::Vec3::b, OptionalAttribute())

	.var("s", &Gleam::Vec3::s, OptionalAttribute())
	.var("t", &Gleam::Vec3::t, OptionalAttribute())
	.var("p", &Gleam::Vec3::p, OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::Vec3)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Vec2)
	.ctor<const Gleam::Vec2&>()
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

	.opToString< VecToString<Gleam::Vec2> >()

	.var("x", &Gleam::Vec2::x, OptionalAttribute())
	.var("y", &Gleam::Vec2::y, OptionalAttribute())

	.var("r", &Gleam::Vec2::r, OptionalAttribute())
	.var("g", &Gleam::Vec2::g, OptionalAttribute())

	.var("s", &Gleam::Vec2::s, OptionalAttribute())
	.var("t", &Gleam::Vec2::t, OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::Vec2)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Transform)
	.var("translation", &Gleam::Transform::getTranslation, &Gleam::Transform::setTranslation)
	.var("rotation", &Gleam::Transform::getRotation, &Gleam::Transform::setRotation)
	.var(
		"scale",
		static_cast<const Gleam::Vec3& (Gleam::Transform::*)() const>(&Gleam::Transform::getScale),
		static_cast<void (Gleam::Transform::*)(const Gleam::Vec3&)>(&Gleam::Transform::setScale)
	)
SHIB_REFLECTION_DEFINE_END(Gleam::Transform)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Plane)
	.var("normal", &Gleam::Plane::getNormal, &Gleam::Plane::setNormal)
	.var("distance", &Gleam::Plane::getDistance, &Gleam::Plane::setDistance)
SHIB_REFLECTION_DEFINE_END(Gleam::Plane)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::AABB)
	.var("min", &Gleam::AABB::getMin, &Gleam::AABB::setMin)
	.var("max", &Gleam::AABB::getMax, &Gleam::AABB::setMax)
SHIB_REFLECTION_DEFINE_END(Gleam::AABB)
