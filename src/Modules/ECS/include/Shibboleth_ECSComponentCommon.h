/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_ECSEntity.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_Math.h>
#include <simd/geometric.h>

NS_SHIBBOLETH

class Position final : public Gaff::IReflectionObject
{
public:
	// Slow versions for posterity.
	static void Set(EntityID id, const glm::vec3& value);
	static glm::vec3 Get();

	static glm_vec4 GetX();
	static glm_vec4 GetY();
	static glm_vec4 GetZ();

	SHIB_REFLECTION_CLASS_DECLARE(Position);
};

class Rotation final : public Gaff::IReflectionObject
{
public:
	// Slow versions for posterity.
	static void Set(const glm::quat& value);
	static glm::quat Get();

	static glm_vec4 GetX();
	static glm_vec4 GetY();
	static glm_vec4 GetZ();
	static glm_vec4 GetW();

	SHIB_REFLECTION_CLASS_DECLARE(Rotation);
};

class Scale final : public Gaff::IReflectionObject
{
public:
	// Slow versions for posterity.
	static void Set(const glm::vec3& value);
	static glm::vec3 Get();

	static glm_vec4 GetX();
	static glm_vec4 GetY();
	static glm_vec4 GetZ();

	SHIB_REFLECTION_CLASS_DECLARE(Scale);
};

NS_END

SHIB_REFLECTION_DECLARE(Position)
SHIB_REFLECTION_DECLARE(Rotation)
SHIB_REFLECTION_DECLARE(Scale)
