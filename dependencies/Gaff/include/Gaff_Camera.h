/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

struct aiCamera;

NS_GAFF

/*!
	\brief
		Class that wraps camera data extracted from a loaded Scene.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_camera.html">aiCamera Documentation</a> for more details.
*/
class Camera
{
public:
	Camera(const Camera& camera);
	Camera(void);
	~Camera(void);

	INLINE const char* getName(void) const;
	INLINE const float* getPosition(void) const;
	INLINE const float* getDirection(void) const;
	INLINE const float* getUp(void) const;
	INLINE float getHorizFOV(void) const;
	INLINE float getClipNear(void) const;
	INLINE float getClipFar(void) const;
	INLINE float getAspectRatio(void) const;

	INLINE bool valid(void) const;

	INLINE const Camera& operator=(const Camera& rhs);

	INLINE bool operator==(const Camera& rhs) const;
	INLINE bool operator!=(const Camera& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiCamera* _camera;

	friend class Scene;
	Camera(const aiCamera* camera);
};

NS_END
