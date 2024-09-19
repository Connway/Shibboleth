/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include "Attributes/Shibboleth_EngineAttributesCommon.h"

namespace
{
	template <class T>
	int32_t VecToString(const T& value, char8_t* buffer, int32_t size)
	{
		if constexpr (std::is_same<T, Gleam::Quat>::value) {
			return snprintf(reinterpret_cast<char*>(buffer), static_cast<size_t>(size), "Gleam::Quat(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, Gleam::Vec4>::value) {
			return snprintf(reinterpret_cast<char*>(buffer), static_cast<size_t>(size), "Gleam::Vec4(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, Gleam::Vec3>::value) {
			return snprintf(reinterpret_cast<char*>(buffer), static_cast<size_t>(size), "Gleam::Vec3(%f, %f, %f)", value.x, value.y, value.z);
		} else if constexpr (std::is_same<T, Gleam::Vec2>::value) {
			return snprintf(reinterpret_cast<char*>(buffer), static_cast<size_t>(size), "Gleam::Vec2(%f, %f)", value.x, value.y);
		} else if constexpr (std::is_same<T, Gleam::IVec4>::value) {
			return snprintf(reinterpret_cast<char*>(buffer), static_cast<size_t>(size), "Gleam::Vec4(%i, %i, %i, %i)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, Gleam::IVec3>::value) {
			return snprintf(reinterpret_cast<char*>(buffer), static_cast<size_t>(size), "Gleam::Vec3(%i, %i, %i)", value.x, value.y, value.z);
		} else if constexpr (std::is_same<T, Gleam::IVec2>::value) {
			return snprintf(reinterpret_cast<char*>(buffer), static_cast<size_t>(size), "Gleam::Vec2(%i, %i)", value.x, value.y);
		}
	}
}

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Quat)
	//.template ctor<const Gleam::Mat4x4&>()
	//.template ctor<const Gleam::Mat3x3&>()
	.template ctor<const Gleam::Vec3&>()
	.template ctor<float, const Gleam::Vec3&>()
	.template ctor<float, float, float, float>()
	.template ctor<>()

	.template opMul<Gleam::Vec4>()
	.template opMul<Gleam::Vec3>()

	.template opIndex<const float&, glm::length_t>()
	.template opIndex<float&, glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()

	.opEqual()

	.opMinus()
	.opPlus()

	.template opToString< VecToString<Gleam::Quat> >()

	.var("x", &Gleam::Quat::x)
	.var("y", &Gleam::Quat::y)
	.var("z", &Gleam::Quat::z)
	.var("w", &Gleam::Quat::w)
SHIB_REFLECTION_DEFINE_END(Gleam::Quat)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Vec4)
	.template ctor<const Gleam::Vec4&>()
	.template ctor<const Gleam::Vec2&, float, float>()
	.template ctor<const Gleam::Vec3&, float>()
	.template ctor<float, float, float, float>()
	.template ctor<float>()
	.template ctor<>()

	.template opAdd<float>()
	.template opSub<float>()
	.template opMul<float>()
	.template opDiv<float>()

	.template opIndex<const float&, glm::length_t>()
	.template opIndex<float&, glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.template opToString< VecToString<Gleam::Vec4> >()

	.var("x", &Gleam::Vec4::x, Shibboleth::OptionalAttribute())
	.var("y", &Gleam::Vec4::y, Shibboleth::OptionalAttribute())
	.var("z", &Gleam::Vec4::z, Shibboleth::OptionalAttribute())
	.var("w", &Gleam::Vec4::w, Shibboleth::OptionalAttribute())

	.var("r", &Gleam::Vec4::r, Shibboleth::OptionalAttribute())
	.var("g", &Gleam::Vec4::g, Shibboleth::OptionalAttribute())
	.var("b", &Gleam::Vec4::b, Shibboleth::OptionalAttribute())
	.var("a", &Gleam::Vec4::a, Shibboleth::OptionalAttribute())

	.var("s", &Gleam::Vec4::s, Shibboleth::OptionalAttribute())
	.var("t", &Gleam::Vec4::t, Shibboleth::OptionalAttribute())
	.var("p", &Gleam::Vec4::p, Shibboleth::OptionalAttribute())
	.var("q", &Gleam::Vec4::q, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::Vec4)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Vec3)
	.template ctor<const Gleam::Vec2&, float>()
	.template ctor<const Gleam::Vec3&>()
	.template ctor<float, float, float>()
	.template ctor<float>()
	.template ctor<>()

	.template opAdd<float>()
	.template opSub<float>()
	.template opMul<float>()
	.template opDiv<float>()

	.template opIndex<const float&, glm::length_t>()
	.template opIndex<float&, glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.template opToString< VecToString<Gleam::Vec3> >()

	.var("x", &Gleam::Vec3::x, Shibboleth::OptionalAttribute())
	.var("y", &Gleam::Vec3::y, Shibboleth::OptionalAttribute())
	.var("z", &Gleam::Vec3::z, Shibboleth::OptionalAttribute())

	.var("r", &Gleam::Vec3::r, Shibboleth::OptionalAttribute())
	.var("g", &Gleam::Vec3::g, Shibboleth::OptionalAttribute())
	.var("b", &Gleam::Vec3::b, Shibboleth::OptionalAttribute())

	.var("s", &Gleam::Vec3::s, Shibboleth::OptionalAttribute())
	.var("t", &Gleam::Vec3::t, Shibboleth::OptionalAttribute())
	.var("p", &Gleam::Vec3::p, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::Vec3)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Vec2)
	.template ctor<const Gleam::Vec2&>()
	.template ctor<float, float>()
	.template ctor<float>()
	.template ctor<>()

	.template opAdd<float>()
	.template opSub<float>()
	.template opMul<float>()
	.template opDiv<float>()

	.template opIndex<const float&, glm::length_t>()
	.template opIndex<float&, glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.template opToString< VecToString<Gleam::Vec2> >()

	.var("x", &Gleam::Vec2::x, Shibboleth::OptionalAttribute())
	.var("y", &Gleam::Vec2::y, Shibboleth::OptionalAttribute())

	.var("r", &Gleam::Vec2::r, Shibboleth::OptionalAttribute())
	.var("g", &Gleam::Vec2::g, Shibboleth::OptionalAttribute())

	.var("s", &Gleam::Vec2::s, Shibboleth::OptionalAttribute())
	.var("t", &Gleam::Vec2::t, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::Vec2)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IVec4)
	.template ctor<const Gleam::IVec4&>()
	.template ctor<const Gleam::IVec2&, int32_t, int32_t>()
	.template ctor<const Gleam::IVec3&, int32_t>()
	.template ctor<int32_t, int32_t, int32_t, int32_t>()
	.template ctor<int32_t>()
	.template ctor<>()

	.template opAdd<int32_t>()
	.template opSub<int32_t>()
	.template opMul<int32_t>()
	.template opDiv<int32_t>()

	.template opIndex<const int32_t&, glm::length_t>()
	.template opIndex<int32_t&, glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.template opToString< VecToString<Gleam::IVec4> >()

	.var("x", &Gleam::IVec4::x, Shibboleth::OptionalAttribute())
	.var("y", &Gleam::IVec4::y, Shibboleth::OptionalAttribute())
	.var("z", &Gleam::IVec4::z, Shibboleth::OptionalAttribute())
	.var("w", &Gleam::IVec4::w, Shibboleth::OptionalAttribute())

	.var("r", &Gleam::IVec4::r, Shibboleth::OptionalAttribute())
	.var("g", &Gleam::IVec4::g, Shibboleth::OptionalAttribute())
	.var("b", &Gleam::IVec4::b, Shibboleth::OptionalAttribute())
	.var("a", &Gleam::IVec4::a, Shibboleth::OptionalAttribute())

	.var("s", &Gleam::IVec4::s, Shibboleth::OptionalAttribute())
	.var("t", &Gleam::IVec4::t, Shibboleth::OptionalAttribute())
	.var("p", &Gleam::IVec4::p, Shibboleth::OptionalAttribute())
	.var("q", &Gleam::IVec4::q, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::IVec4)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IVec3)
	.template ctor<const Gleam::IVec2&, int32_t>()
	.template ctor<const Gleam::IVec3&>()
	.template ctor<int32_t, int32_t, int32_t>()
	.template ctor<int32_t>()
	.template ctor<>()

	.template opAdd<int32_t>()
	.template opSub<int32_t>()
	.template opMul<int32_t>()
	.template opDiv<int32_t>()

	.template opIndex<const int32_t&, glm::length_t>()
	.template opIndex<int32_t&, glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.template opToString< VecToString<Gleam::IVec3> >()

	.var("x", &Gleam::IVec3::x, Shibboleth::OptionalAttribute())
	.var("y", &Gleam::IVec3::y, Shibboleth::OptionalAttribute())
	.var("z", &Gleam::IVec3::z, Shibboleth::OptionalAttribute())

	.var("r", &Gleam::IVec3::r, Shibboleth::OptionalAttribute())
	.var("g", &Gleam::IVec3::g, Shibboleth::OptionalAttribute())
	.var("b", &Gleam::IVec3::b, Shibboleth::OptionalAttribute())

	.var("s", &Gleam::IVec3::s, Shibboleth::OptionalAttribute())
	.var("t", &Gleam::IVec3::t, Shibboleth::OptionalAttribute())
	.var("p", &Gleam::IVec3::p, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::IVec3)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::IVec2)
	.template ctor<const Gleam::IVec2&>()
	.template ctor<int32_t, int32_t>()
	.template ctor<int32_t>()
	.template ctor<>()

	.template opAdd<int32_t>()
	.template opSub<int32_t>()
	.template opMul<int32_t>()
	.template opDiv<int32_t>()

	.template opIndex<const int32_t&, glm::length_t>()
	.template opIndex<int32_t&, glm::length_t>()

	.opAdd()
	.opSub()
	.opMul()
	.opDiv()

	.opEqual()

	.opMinus()
	.opPlus()

	.template opToString< VecToString<Gleam::IVec2> >()

	.var("x", &Gleam::IVec2::x, Shibboleth::OptionalAttribute())
	.var("y", &Gleam::IVec2::y, Shibboleth::OptionalAttribute())

	.var("r", &Gleam::IVec2::r, Shibboleth::OptionalAttribute())
	.var("g", &Gleam::IVec2::g, Shibboleth::OptionalAttribute())

	.var("s", &Gleam::IVec2::s, Shibboleth::OptionalAttribute())
	.var("t", &Gleam::IVec2::t, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Gleam::IVec2)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::TransformRTEuler)
	.varFunc("translation", &Gleam::TransformRTEuler::getTranslation, &Gleam::TransformRTEuler::setTranslation)
	.varFunc("rotation", &Gleam::TransformRTEuler::getRotation, &Gleam::TransformRTEuler::setRotation)
	.varFunc("rotationQuatTurns", &Gleam::TransformRTEuler::getRotationQuatTurns, &Gleam::TransformRTEuler::setRotationQuatTurns)
	.varFunc("rotationQuat", &Gleam::TransformRTEuler::getRotationQuat, &Gleam::TransformRTEuler::setRotationQuat)
SHIB_REFLECTION_DEFINE_END(Gleam::TransformRT)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::TransformRT)
	.varFunc("translation", &Gleam::TransformRT::getTranslation, &Gleam::TransformRT::setTranslation)
	.varFunc("rotation", &Gleam::TransformRT::getRotation, &Gleam::TransformRT::setRotation)
	.varFunc("rotationEulerTurns", &Gleam::TransformRT::getRotationEulerTurns, &Gleam::TransformRT::setRotationEulerTurns)
	.varFunc("rotationEuler", &Gleam::TransformRT::getRotationEuler, &Gleam::TransformRT::setRotationEuler)
SHIB_REFLECTION_DEFINE_END(Gleam::TransformRT)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Transform)
	.varFunc("translation", &Gleam::Transform::getTranslation, &Gleam::Transform::setTranslation)
	.varFunc("rotation", &Gleam::Transform::getRotation, &Gleam::Transform::setRotation)
	.varFunc("rotationEulerTurns", &Gleam::Transform::getRotationEulerTurns, &Gleam::Transform::setRotationEulerTurns)
	.varFunc("rotationEuler", &Gleam::Transform::getRotationEuler, &Gleam::Transform::setRotationEuler)
	.varFunc(
		"scale",
		&Gleam::Transform::getScale,
		static_cast<void (Gleam::Transform::*)(const Gleam::Vec3&)>(&Gleam::Transform::setScale)
	)
SHIB_REFLECTION_DEFINE_END(Gleam::Transform)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Plane)
	.varFunc("normal", &Gleam::Plane::getNormal, &Gleam::Plane::setNormal)
	.varFunc("distance", &Gleam::Plane::getDistance, &Gleam::Plane::setDistance)
SHIB_REFLECTION_DEFINE_END(Gleam::Plane)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::AABB)
	.varFunc("min", &Gleam::AABB::getMin, &Gleam::AABB::setMin)
	.varFunc("max", &Gleam::AABB::getMax, &Gleam::AABB::setMax)
SHIB_REFLECTION_DEFINE_END(Gleam::AABB)
