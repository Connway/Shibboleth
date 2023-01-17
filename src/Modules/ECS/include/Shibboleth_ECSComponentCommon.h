/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_ECSComponentBase.h"
#include <Shibboleth_Math.h>

NS_SHIBBOLETH

SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN_WITH_DEFAULT(Position, Gleam::Vec3, ECSComponentBaseBoth, glm::zero<Gleam::Vec3>())
	static Gleam::Vec4SIMD GetX(const void* component_begin);
	static Gleam::Vec4SIMD GetY(const void* component_begin);
	static Gleam::Vec4SIMD GetZ(const void* component_begin);

	static void CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void SetInternal(void* component, int32_t page_index, const Position& value);
	static Position GetInternal(const void* component, int32_t page_index);
SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_END(Position)

SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN_WITH_DEFAULT(Rotation, Gleam::Vec3, ECSComponentBaseBoth, glm::zero<Gleam::Vec3>())
	static Gleam::Vec4SIMD GetPitch(const void* component_begin);
	static Gleam::Vec4SIMD GetYaw(const void* component_begin);
	static Gleam::Vec4SIMD GetRoll(const void* component_begin);

	static void CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void SetInternal(void* component, int32_t page_index, const Rotation& value);
	static Rotation GetInternal(const void* component, int32_t page_index);
SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_END(Rotation)

SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN_WITH_DEFAULT(Scale, Gleam::Vec3, ECSComponentBaseBoth, glm::one<Gleam::Vec3>())
	static Gleam::Vec4SIMD GetX(const void* component_begin);
	static Gleam::Vec4SIMD GetY(const void* component_begin);
	static Gleam::Vec4SIMD GetZ(const void* component_begin);

	static void CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void SetInternal(void* component, int32_t page_index, const Scale& value);
	static Scale GetInternal(const void* component, int32_t page_index);
SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_END(Scale)

SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN_WITH_DEFAULT(PlayerOwner, int32_t, ECSComponentBaseBoth, -1)
	static void CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void SetInternal(void* component, int32_t page_index, const PlayerOwner& value);
	static PlayerOwner GetInternal(const void* component, int32_t page_index);
SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_END(PlayerOwner)


SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_WITH_DEFAULT(PageSize, int32_t, ECSComponentBaseShared, -1)
SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE(Scene, Gaff::Hash32, ECSComponentBaseShared)
SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE(Layer, Gaff::Hash32, ECSComponentBaseShared)

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::PlayerOwner)
SHIB_REFLECTION_DECLARE(Shibboleth::PageSize)
SHIB_REFLECTION_DECLARE(Shibboleth::Position)
SHIB_REFLECTION_DECLARE(Shibboleth::Rotation)
SHIB_REFLECTION_DECLARE(Shibboleth::Scale)
SHIB_REFLECTION_DECLARE(Shibboleth::Scene)
SHIB_REFLECTION_DECLARE(Shibboleth::Layer)
