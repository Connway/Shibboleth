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
	bool CastNumberToFloat(const Gaff::IReflectionDefinition::StackEntry& entry, float& out)
	{
		if (entry.first == &Shibboleth::Reflection<double>::GetReflectionDefinition()) {
			out = static_cast<float>(*reinterpret_cast<double*>(entry.second));
			return true;
		} else if (entry.first == &Shibboleth::Reflection<float>::GetReflectionDefinition()) {
			out = *reinterpret_cast<float*>(entry.second);
			return true;
		} else if (entry.first == &Shibboleth::Reflection<int64_t>::GetReflectionDefinition()) {
			out = static_cast<float>(*reinterpret_cast<int64_t*>(entry.second));
			return true;
		} else if (entry.first == &Shibboleth::Reflection<int32_t>::GetReflectionDefinition()) {
			out = static_cast<float>(*reinterpret_cast<int32_t*>(entry.second));
			return true;
		} else if (entry.first == &Shibboleth::Reflection<int16_t>::GetReflectionDefinition()) {
			out = static_cast<float>(*reinterpret_cast<int16_t*>(entry.second));
			return true;
		} else if (entry.first == &Shibboleth::Reflection<int8_t>::GetReflectionDefinition()) {
			out = static_cast<float>(*reinterpret_cast<int8_t*>(entry.second));
			return true;
		}

		return false;
	}

	void Vec3StackCtor(void* instance, const Gaff::IReflectionDefinition::CtorStack& stack)
	{
		auto* const vec = reinterpret_cast<glm::vec3*>(instance);
		const size_t size = stack.size();

		if (size == 3) {
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;

			const bool x_success = CastNumberToFloat(stack[0], x);
			const bool y_success = CastNumberToFloat(stack[1], y);
			const bool z_success = CastNumberToFloat(stack[2], z);

			if (!x_success) {
				// $TODO: Log error.
			}

			if (!y_success) {
				// $TODO: Log error.
			}

			if (!z_success) {
				// $TODO: Log error.
			}

			new(vec) glm::vec3(x, y, z);

		} else if (size == 2) {
			if (stack[0].first == &Shibboleth::Reflection<glm::vec2>::GetReflectionDefinition()) {
				const glm::vec2& vec2 = *reinterpret_cast<glm::vec2*>(stack[0].second);

				if (stack[1].first == &Shibboleth::Reflection<glm::vec3>::GetReflectionDefinition()) {
					new(vec) glm::vec3(*reinterpret_cast<glm::vec3*>(stack[1].second));
				} else if (float value; CastNumberToFloat(stack[1], value)) {
					new(vec) glm::vec3(vec2, value);
				} else {
					// $TODO: Log error.
					new(vec) glm::vec3(vec2, 0.0f);
				}

			} else {
				// $TODO: Log error.
				new(vec) glm::vec3();
			}

		} else if (size == 1) {
			if (stack[0].first == &Shibboleth::Reflection<glm::vec3>::GetReflectionDefinition()) {
				new(vec) glm::vec3(*reinterpret_cast<glm::vec3*>(stack[0].second));
			} else if (stack[0].first == &Shibboleth::Reflection<glm::vec2>::GetReflectionDefinition()) {
				new(vec) glm::vec3(*reinterpret_cast<glm::vec2*>(stack[0].second), 0.0f);
			} else if (float value; CastNumberToFloat(stack[0], value)) {
				new(vec) glm::vec3(value);
			} else {
				// $TODO: Log error.
				new(vec) glm::vec3();
			}

		} else if (size == 0) {
			new(vec) glm::vec3();
		}
	}
}

SHIB_REFLECTION_DEFINE_BEGIN(glm::quat)
	.var("x", &glm::quat::x)
	.var("y", &glm::quat::y)
	.var("z", &glm::quat::z)
	.var("w", &glm::quat::w)
SHIB_REFLECTION_DEFINE_END(glm::quat)

SHIB_REFLECTION_DEFINE_BEGIN(glm::vec4)
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
	.stackCtor(Vec3StackCtor)

	.ctor<const glm::vec2&, float>()
	.ctor<const glm::vec3&>()
	.ctor<float, float, float>()
	.ctor<float>()
	.ctor<>()

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
	.var("x", &glm::vec2::x, OptionalAttribute())
	.var("y", &glm::vec2::y, OptionalAttribute())

	.var("r", &glm::vec2::r, OptionalAttribute())
	.var("g", &glm::vec2::g, OptionalAttribute())

	.var("s", &glm::vec2::s, OptionalAttribute())
	.var("t", &glm::vec2::t, OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(glm::vec2)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::Transform)
	.var("rotation", &Gleam::Transform::getRotation, &Gleam::Transform::setRotation)
	.var("translation", &Gleam::Transform::getTranslation, &Gleam::Transform::setTranslation)
	.var("scale", &Gleam::Transform::getScale, &Gleam::Transform::setScale)
SHIB_REFLECTION_DEFINE_END(Gleam::Transform)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::AABB)
	.var("min", &Gleam::AABB::getMin, &Gleam::AABB::setMin)
	.var("max", &Gleam::AABB::getMax, &Gleam::AABB::setMax)
SHIB_REFLECTION_DEFINE_END(Gleam::AABB)
