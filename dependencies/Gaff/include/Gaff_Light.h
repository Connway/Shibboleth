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

/*! \file */

#pragma once

#include "Gaff_Defines.h"
#include <assimp/light.h>

NS_GAFF

enum LightType
{
	LT_UNKNOWN = aiLightSource_UNDEFINED,
	LT_DIRECTIONAL = aiLightSource_DIRECTIONAL,
	LT_POINT = aiLightSource_POINT,
	LT_SPOT = aiLightSource_SPOT
};

/*!
	\brief
		Class that wraps light data extracted from a loaded Scene.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_light.html">aiLight Documentation</a> for more details.
*/
class Light
{
public:
	Light(const Light& light);
	Light(void);
	~Light(void);

	INLINE const char* getName(void) const;
	INLINE LightType getType(void) const;
	INLINE const float* getPosition(void) const;
	INLINE const float* getDirection(void) const;
	INLINE float getAttenuationConstant(void) const;
	INLINE float getAttenuationLinear(void) const;
	INLINE float getAttenuationQuadratic(void) const;
	INLINE const float* getDiffuseColor(void) const;
	INLINE const float* getSpecularColor(void) const;
	INLINE const float* getAmbientColor(void) const;
	INLINE float getInnerConeAngle(void) const;
	INLINE float getOuterConeAngle(void) const;

	INLINE bool valid(void) const;

	INLINE const Light& operator=(const Light& rhs);

	INLINE bool operator==(const Light& rhs) const;
	INLINE bool operator!=(const Light& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiLight* _light;

	friend class Scene;
	Light(const aiLight* light);
};

NS_END
