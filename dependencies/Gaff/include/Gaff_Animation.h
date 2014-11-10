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

#include "Gaff_NodeAnimation.h"
#include "Gaff_MeshAnimation.h"

NS_GAFF

/*!
	\brief
		Class that wraps animation data extracted from a loaded Scene.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_animation.html">aiAnimation Documentation</a> for more details.
*/
class Animation
{
public:
	Animation(const Animation& animation);
	Animation(void);
	~Animation(void);

	INLINE const char* getName(void) const;
	INLINE double getDuration(void) const;
	INLINE double getFramesPerSecond(void) const;
	INLINE unsigned int getNumNodeChannels(void) const;
	INLINE unsigned int getNumMeshChannels(void) const;
	INLINE NodeAnimation getNodeAnimation(unsigned int channel) const;
	INLINE MeshAnimation getMeshAnimation(unsigned int channel) const;

	INLINE bool valid(void) const;

	INLINE const Animation& operator=(const Animation& rhs);

	INLINE bool operator==(const Animation& rhs) const;
	INLINE bool operator!=(const Animation& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiAnimation* _animation;

	friend class Scene;
	Animation(const aiAnimation* animation);
};

NS_END
