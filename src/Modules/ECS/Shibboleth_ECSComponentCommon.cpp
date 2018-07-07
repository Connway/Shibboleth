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

#include "Shibboleth_ECSComponentCommon.h"
#include "Shibboleth_ECSAttributes.h"

SHIB_REFLECTION_DEFINE(Position)
SHIB_REFLECTION_DEFINE(Rotation)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Position)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::vec3>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(Position)

void Position::Set(const glm::vec3& value)
{
	GAFF_REF(value);
}

glm::vec3 Position::Get()
{
	return glm::vec3();
}

glm_vec4 Position::GetX()
{
	return glm_vec4();
}

glm_vec4 Position::GetY()
{
	return glm_vec4();
}

glm_vec4 Position::GetZ()
{
	return glm_vec4();
}



SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Rotation)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::quat>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(Rotation)

void Rotation::Set(const glm::quat& value)
{
	GAFF_REF(value);
}

glm::quat Rotation::Get()
{
	return glm::quat();
}

glm_vec4 Rotation::GetX()
{
	return glm_vec4();
}

glm_vec4 Rotation::GetY()
{
	return glm_vec4();
}

glm_vec4 Rotation::GetZ()
{
	return glm_vec4();
}

glm_vec4 Rotation::GetW()
{
	return glm_vec4();
}

NS_END
