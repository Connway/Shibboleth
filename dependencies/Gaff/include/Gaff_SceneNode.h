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

#include "Gaff_MetaData.h"

struct aiNode;

NS_GAFF

/*!
	\brief
		Class that wraps scene node data extracted from a loaded Scene.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_node.html">aiNode Documentation</a> for more details.
*/
class SceneNode
{
public:
	SceneNode(const SceneNode& node);
	SceneNode(void);
	~SceneNode(void);

	INLINE SceneNode findNode(const char* name) const;
	INLINE SceneNode getParent(void) const;

	INLINE unsigned int getNumChildren(void) const;
	INLINE SceneNode getChild(unsigned int index) const;

	INLINE const float* getTransform(void) const;
	INLINE const char* getName(void) const;

	INLINE MetaData getMetaData(void) const;

	INLINE bool valid(void) const;

	INLINE const SceneNode& operator=(const SceneNode& rhs);

	INLINE bool operator==(const SceneNode& rhs) const;
	INLINE bool operator!=(const SceneNode& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiNode* _node;

	friend class Scene;
	SceneNode(const aiNode* node);
};

NS_END
