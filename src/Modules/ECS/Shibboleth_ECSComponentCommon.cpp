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

#include "Shibboleth_ECSComponentCommon.h"
#include <Shibboleth_EngineAttributesCommon.h>

SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(PlayerOwner, nullptr, "Player")
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(PageSize, nullptr, "Memory")
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Position, nullptr, "Transform", OptionalAttribute())
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Rotation, nullptr, "Transform", OptionalAttribute())
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Scale, nullptr, "Transform", OptionalAttribute())
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Scene, nullptr, "Scene", HashStringAttribute())
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Layer, nullptr, "Scene", HashStringAttribute())

NS_SHIBBOLETH

Gleam::Vec4SIMD Position::GetX(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin);
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Position::GetY(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin) + 4;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Position::GetZ(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin) + 8;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

void Position::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const float* const old_values = reinterpret_cast<const float*>(old_begin) + old_index;
	float* const new_values = reinterpret_cast<float*>(new_begin) + new_index;

	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
}

void Position::SetInternal(void* component, int32_t page_index, const Position& value)
{
	float* const comp = reinterpret_cast<float*>(component) + page_index;
	comp[0] = value.value.x;
	comp[4] = value.value.y;
	comp[8] = value.value.z;
}

Position Position::GetInternal(const void* component, int32_t page_index)
{
	const float* const comp = reinterpret_cast<const float*>(component) + page_index;

	return Position(Gleam::Vec3(
		comp[0],
		comp[4],
		comp[8]
	));
}



Gleam::Vec4SIMD Rotation::GetPitch(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin);
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Rotation::GetYaw(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin) + 4;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Rotation::GetRoll(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin) + 8;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

void Rotation::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const float* const old_values = reinterpret_cast<const float*>(old_begin) + old_index;
	float* const new_values = reinterpret_cast<float*>(new_begin) + new_index;

	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
}

void Rotation::SetInternal(void* component, int32_t page_index, const Rotation& value)
{
	float* const comp = reinterpret_cast<float*>(component) + page_index;
	comp[0] = value.value.x;
	comp[4] = value.value.y;
	comp[8] = value.value.z;
}

Rotation Rotation::GetInternal(const void* component, int32_t page_index)
{
	const float* const comp = reinterpret_cast<const float*>(component) + page_index;

	return Rotation(Gleam::Vec3(
		comp[0],
		comp[4],
		comp[8]
	));
}



Gleam::Vec4SIMD Scale::GetX(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin);
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Scale::GetY(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin) + 4;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Scale::GetZ(const void* component_begin)
{
	const float* const vec = reinterpret_cast<const float*>(component_begin) + 8;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

void Scale::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const float* const old_values = reinterpret_cast<const float*>(old_begin) + old_index;
	float* const new_values = reinterpret_cast<float*>(new_begin) + new_index;

	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
}

void Scale::SetInternal(void* component, int32_t page_index, const Scale& value)
{
	float* const comp = reinterpret_cast<float*>(component) + page_index;
	comp[0] = value.value.x;
	comp[4] = value.value.y;
	comp[8] = value.value.z;
}

Scale Scale::GetInternal(const void* component, int32_t page_index)
{
	const float* const comp = reinterpret_cast<const float*>(component) + page_index;

	return Scale(Gleam::Vec3(
		comp[0],
		comp[4],
		comp[8]
	));
}



void PlayerOwner::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const int32_t* const old_values = reinterpret_cast<const int32_t*>(old_begin) + old_index;
	int32_t* const new_values = reinterpret_cast<int32_t*>(new_begin) + new_index;

	*new_values = *old_values;
}

void PlayerOwner::SetInternal(void* component, int32_t page_index, const PlayerOwner& value)
{
	int32_t* const comp = reinterpret_cast<int32_t*>(component) + page_index;
	*comp = value.value;
}

PlayerOwner PlayerOwner::GetInternal(const void* component, int32_t page_index)
{
	const int32_t* const comp = reinterpret_cast<const int32_t*>(component) + page_index;
	return PlayerOwner(*comp);
}

NS_END
