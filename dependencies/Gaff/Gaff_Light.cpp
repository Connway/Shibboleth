/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gaff_Light.h"
#include "Gaff_IncludeAssert.h"
#include <assimp/light.h>

NS_GAFF

Light::Light(const aiLight* light):
	_light(light)
{
}

Light::Light(const Light& light):
	_light(light._light)
{
}

Light::Light(void):
	_light(nullptr)
{
}

Light::~Light(void)
{
}

const char* Light::getName(void) const
{
	assert(_light);
	return _light->mName.C_Str();
}

LightType Light::getType(void) const
{
	assert(_light);
	return (LightType)_light->mType;
}

const float* Light::getPosition(void) const
{
	assert(_light);
	return &_light->mPosition.x;
}

const float* Light::getDirection(void) const
{
	assert(_light);
	return &_light->mDirection.x;
}

float Light::getAttenuationConstant(void) const
{
	assert(_light);
	return _light->mAttenuationConstant;
}

float Light::getAttenuationLinear(void) const
{
	assert(_light);
	return _light->mAttenuationLinear;
}

float Light::getAttenuationQuadratic(void) const
{
	assert(_light);
	return _light->mAttenuationQuadratic;
}

const float* Light::getDiffuseColor(void) const
{
	assert(_light);
	return &_light->mColorDiffuse.r;
}

const float* Light::getSpecularColor(void) const
{
	assert(_light);
	return &_light->mColorSpecular.r;
}

const float* Light::getAmbientColor(void) const
{
	assert(_light);
	return &_light->mColorAmbient.r;
}

float Light::getInnerConeAngle(void) const
{
	assert(_light);
	return _light->mAngleInnerCone;
}

float Light::getOuterConeAngle(void) const
{
	assert(_light);
	return _light->mAngleOuterCone;
}

bool Light::valid(void) const
{
	return _light != nullptr;
}

const Light& Light::operator=(const Light& rhs)
{
	_light = rhs._light;
	return *this;
}

bool Light::operator==(const Light& rhs) const
{
	return _light == rhs._light;
}

bool Light::operator!=(const Light& rhs) const
{
	return _light != rhs._light;
}

NS_END
