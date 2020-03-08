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

SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Position, nullptr, "Transform", OptionalAttribute())
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Rotation, nullptr, "Transform", OptionalAttribute())
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Scale, nullptr, "Transform", OptionalAttribute())
SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(Layer, nullptr, "Scene", HashStringAttribute())

NS_SHIBBOLETH

glm_vec4 Position::GetX(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
}

glm_vec4 Position::GetY(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Position::GetZ(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
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

	return Position(glm::vec3(
		comp[0],
		comp[4],
		comp[8]
	));
}



glm_vec4 Rotation::GetPitch(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
}

glm_vec4 Rotation::GetYaw(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Rotation::GetRoll(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
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

	return Rotation(glm::vec3(
		comp[0],
		comp[4],
		comp[8]
	));
}



glm_vec4 Scale::GetX(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
}

glm_vec4 Scale::GetY(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Scale::GetZ(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
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

	return Scale(glm::vec3(
		comp[0],
		comp[4],
		comp[8]
	));
}

NS_END
