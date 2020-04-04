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
	void VecStackCtor(void* instance, const Gaff::FunctionStackEntry* stack, int32_t size)
	{
		T* const vec = reinterpret_cast<T*>(instance);

		if (size == 4) {
			if constexpr (std::is_same<T, glm::vec4>::value || std::is_same<T, glm::quat>::value) {
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
				float w = 0.0f;

				const bool x_success = Gaff::CastNumberToType<float>(stack[0], x);
				const bool y_success = Gaff::CastNumberToType<float>(stack[1], y);
				const bool z_success = Gaff::CastNumberToType<float>(stack[2], z);
				const bool w_success = Gaff::CastNumberToType<float>(stack[3], w);

				if (!x_success) {
					// $TODO: Log error.
				}

				if (!y_success) {
					// $TODO: Log error.
				}

				if (!z_success) {
					// $TODO: Log error.
				}

				if (!w_success) {
					// $TODO: Log error.
				}

				if constexpr (std::is_same<T, glm::quat>::value) {
					new(vec) T(w, x, y, z);
				} else {
					new(vec) T(x, y, z, w);
				}

			} else {
				// $TODO: Log error.
				new(vec) T();
			}

		} else if (size == 3) {
			if constexpr (std::is_same<T, glm::vec4>::value) {
				if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec2>::GetReflectionDefinition()) {
					const glm::vec2& vec2 = *reinterpret_cast<glm::vec2*>(stack[0].value.vp);

					float z = 0.0f;
					float w = 0.0f;
					const bool z_success = Gaff::CastNumberToType<float>(stack[1], z);
					const bool w_success = Gaff::CastNumberToType<float>(stack[2], w);

					if (!z_success) {
						// $TODO: Log error.
					}

					if (!w_success) {
						// $TODO: Log error.
					}

					new(vec) T(vec2, z, w);

				} else {
					// $TODO: Log error.
					new(vec) T();
				}

			} else if constexpr (std::is_same<T, glm::vec3>::value) {
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;

				const bool x_success = Gaff::CastNumberToType<float>(stack[0], x);
				const bool y_success = Gaff::CastNumberToType<float>(stack[1], y);
				const bool z_success = Gaff::CastNumberToType<float>(stack[2], z);

				if (!x_success) {
					// $TODO: Log error.
				}

				if (!y_success) {
					// $TODO: Log error.
				}

				if (!z_success) {
					// $TODO: Log error.
				}

				new(vec) T(x, y, z);
			}

		} else if (size == 2) {
			if constexpr (std::is_same<T, glm::quat>::value) {
				if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec3>::GetReflectionDefinition()) {
					const glm::vec3& v1 = *reinterpret_cast<glm::vec3*>(stack[0].value.vp);

					if (stack[1].ref_def == &Shibboleth::Reflection<glm::vec3>::GetReflectionDefinition()) {
						const glm::vec3& v2 = *reinterpret_cast<glm::vec3*>(stack[1].value.vp);
						new(vec) T(v1, v2);

					} else {
						float w = 0.0f;
						const bool w_success = Gaff::CastNumberToType<float>(stack[1], w);

						if (!w_success) {
							// $TODO: Log error.
						}

						new(vec) T(w, v1);
					}

				} else {
					// $TODO: Log error.
					new(vec) T();
				}

			} else if constexpr (std::is_same<T, glm::vec4>::value) {
				if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec3>::GetReflectionDefinition()) {
					const glm::vec3& vec3 = *reinterpret_cast<glm::vec3*>(stack[0].value.vp);

					float w = 0.0f;
					const bool w_success = Gaff::CastNumberToType<float>(stack[1], w);

					if (!w_success) {
						// $TODO: Log error.
					}

					new(vec) T(vec3, w);

				} else {
					// $TODO: Log error.
					new(vec) T();
				}

			} else if constexpr (std::is_same<T, glm::vec3>::value) {
				if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec2>::GetReflectionDefinition()) {
					const glm::vec2& vec2 = *reinterpret_cast<glm::vec2*>(stack[0].value.vp);

					float z = 0.0f;
					const bool z_success = Gaff::CastNumberToType<float>(stack[1], z);

					if (!z_success) {
						// $TODO: Log error.
					}

					new(vec) T(vec2, z);

				} else {
					// $TODO: Log error.
					new(vec) T();
				}

			} else if constexpr (std::is_same<T, glm::vec2>::value) {
				float x = 0.0f;
				float y = 0.0f;

				const bool x_success = Gaff::CastNumberToType<float>(stack[0], x);
				const bool y_success = Gaff::CastNumberToType<float>(stack[1], y);

				if (!x_success) {
					// $TODO: Log error.
				}

				if (!y_success) {
					// $TODO: Log error.
				}

				new(vec) T(x, y);
			}

		} else if (size == 1) {
			if constexpr (std::is_same<T, glm::quat>::value) {
				/*if (stack[0].ref_def == &Shibboleth::Reflection<glm::mat4x4>::GetReflectionDefinition()) {
					new(vec) T(*reinterpret_cast<glm::mat4x4*>(stack[0].value.vp));
					return;
				} else if (stack[0].ref_def == &Shibboleth::Reflection<glm::mat3x3>::GetReflectionDefinition()) {
					new(vec) T(*reinterpret_cast<glm::mat3x3*>(stack[0].value.vp));
					return;
				} else*/ if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec3>::GetReflectionDefinition()) {
					new(vec) T(*reinterpret_cast<glm::vec3*>(stack[0].value.vp));
					return;
				}

			} else if constexpr (std::is_same<T, glm::vec4>::value) {
				if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec2>::GetReflectionDefinition()) {
					new(vec) T(*reinterpret_cast<glm::vec2*>(stack[0].value.vp), 0.0f, 0.0f);
					return;
				} else if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec3>::GetReflectionDefinition()) {
					new(vec) T(*reinterpret_cast<glm::vec3*>(stack[0].value.vp), 0.0f);
					return;
				}
			} else if constexpr (std::is_same<T, glm::vec3>::value) {
				if (stack[0].ref_def == &Shibboleth::Reflection<glm::vec2>::GetReflectionDefinition()) {
					new(vec) T(*reinterpret_cast<glm::vec2*>(stack[0].value.vp), 0.0f);
					return;
				}
			}

			if constexpr (!std::is_same<T, glm::quat>::value) {
				if (float value; Gaff::CastNumberToType<float>(stack[0], value)) {
					new(vec) T(value);
					return;
				}
			}

			if (stack[0].ref_def == &Shibboleth::Reflection<T>::GetReflectionDefinition()) {
				new(vec) T(*reinterpret_cast<T*>(stack[0].value.vp));
			} else {
				// $TODO: Log error.
				new(vec) T();
			}

		} else if (size == 0) {
			new(vec) T();

		} else {
			// $TODO: Log error.
			new(vec) T();
		}
	}

	template <class T>
	int32_t VecToString(const T& value, char* buffer, int32_t size)
	{
		if constexpr (std::is_same<T, glm::quat>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "quat(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, glm::vec4>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "vec4(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
		} else if constexpr (std::is_same<T, glm::vec3>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "vec3(%f, %f, %f)", value.x, value.y, value.z);
		} else if constexpr (std::is_same<T, glm::vec2>::value) {
			return snprintf(buffer, static_cast<size_t>(size), "vec2(%f, %f)", value.x, value.y);
		}
	}
}

SHIB_REFLECTION_DEFINE_BEGIN(glm::quat)
	.classAttrs(RegisterWithScriptAttribute())
	.friendlyName("Quat")

	.stackCtor(VecStackCtor<glm::quat>)
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
	.classAttrs(RegisterWithScriptAttribute())
	.friendlyName("Vec4")

	.stackCtor(VecStackCtor<glm::vec4>)
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
	.classAttrs(RegisterWithScriptAttribute())
	.friendlyName("Vec3")

	.stackCtor(VecStackCtor<glm::vec3>)
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
	.classAttrs(RegisterWithScriptAttribute())
	.friendlyName("Vec2")

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
	.var("rotation", &Gleam::Transform::getRotation, &Gleam::Transform::setRotation)
	.var("translation", &Gleam::Transform::getTranslation, &Gleam::Transform::setTranslation)
	.var("scale", &Gleam::Transform::getScale, &Gleam::Transform::setScale)
SHIB_REFLECTION_DEFINE_END(Gleam::Transform)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::AABB)
	.var("min", &Gleam::AABB::getMin, &Gleam::AABB::setMin)
	.var("max", &Gleam::AABB::getMax, &Gleam::AABB::setMax)
SHIB_REFLECTION_DEFINE_END(Gleam::AABB)
