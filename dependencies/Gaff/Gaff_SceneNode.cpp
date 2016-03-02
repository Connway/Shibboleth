/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gaff_SceneNode.h"
#include "Gaff_Assert.h"
#include <assimp/scene.h>

NS_GAFF

SceneNode::SceneNode(const SceneNode& node):
_node(node._node)
{
}

SceneNode::SceneNode(const aiNode* node):
_node(node)
{
}

SceneNode::SceneNode(void):
_node(nullptr)
{
}

SceneNode::~SceneNode(void)
{
}

SceneNode SceneNode::findNode(const char* name) const
{
	GAFF_ASSERT(_node);
	return SceneNode(_node->FindNode(name));
}

SceneNode SceneNode::getParent(void) const
{
	GAFF_ASSERT(_node);
	return SceneNode(_node->mParent);
}

bool SceneNode::valid(void) const
{
	return _node != nullptr;
}

const SceneNode& SceneNode::operator=(const SceneNode& rhs)
{
	_node = rhs._node;
	return *this;
}

bool SceneNode::operator==(const SceneNode& rhs) const
{
	return _node == rhs._node;
}

bool SceneNode::operator!=(const SceneNode& rhs) const
{
	return _node != rhs._node;
}

unsigned int SceneNode::getNumChildren(void) const
{
	GAFF_ASSERT(_node);
	return _node->mNumChildren;
}

SceneNode SceneNode::getChild(unsigned int index) const
{
	GAFF_ASSERT(_node && index < _node->mNumChildren);
	return SceneNode(_node->mChildren[index]);
}

const float* SceneNode::getTransform(void) const
{
	GAFF_ASSERT(_node);
	return &_node->mTransformation.a1;
}

const char* SceneNode::getName(void) const
{
	GAFF_ASSERT(_node);
	return _node->mName.C_Str();
}

MetaData SceneNode::getMetaData(void) const
{
	GAFF_ASSERT(_node);
	return MetaData(_node->mMetaData);
}

NS_END
