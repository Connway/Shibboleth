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

#include "Gleam_ISceneNode.h"
//#include "Gleam_IAnimator.h"
#include "Gleam_Scene.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

ISceneNode::ISceneNode(Scene& scene, const Transform& model_transform):
	_parent(NULLPTR), _model_transform(model_transform),
	_node_type(0), _changed(true), _enabled(true),
	_scene(scene)
{
}

ISceneNode::ISceneNode(Scene& scene):
	_parent(NULLPTR), _node_type(0),
	_changed(true), _enabled(true),
	_scene(scene)
{
}

ISceneNode::~ISceneNode(void)
{
}

void ISceneNode::destroy(void)
{
}

void ISceneNode::render(IRenderDevice&)
{
}

const Transform& ISceneNode::getWorldTransform(void) const
{
	return _world_transform;
}

const Transform& ISceneNode::getModelTransform(void) const
{
	return _model_transform;
}

Transform& ISceneNode::getModelTransform(void)
{
	return _model_transform;
}

void ISceneNode::setModelTransform(const Transform& transform)
{
	_model_transform = transform;
	_changed = true;
}

unsigned int ISceneNode::getNodeType(void) const
{
	return _node_type;
}

void ISceneNode::setNodeType(unsigned int node_type)
{
	_node_type = node_type;
}

const OBB& ISceneNode::getBoundingBox(void) const
{
	return _bounding_box;
}

void ISceneNode::setBoundingBox(const OBB& bounding_box)
{
	_bounding_box = bounding_box;
}

const GleamArray(ISceneNode*)& ISceneNode::getChildren(void) const
{
	return _children;
}

GleamArray(ISceneNode*)& ISceneNode::getChildren(void)
{
	return _children;
}

void ISceneNode::addChild(ISceneNode* child)
{
	assert(child);
	_children.push(child);
	child->addRef();
}

void ISceneNode::removeChild(const ISceneNode* child)
{
	assert(child && _children.size());

	int index = _children.linearFind((ISceneNode* const)child);
	assert(index > -1);

	_children.erase(index);
	child->release();
}

const ISceneNode* ISceneNode::getParent(void) const
{
	return _parent;
}

void ISceneNode::setParent(ISceneNode* parent)
{
	if (_parent) {
		removeFromParent();
	}

	_parent = parent;
	parent->addRef();
}

void ISceneNode::removeFromParent(void)
{
	assert(_parent);
	_parent->removeChild(this);
	_parent->release();
	_parent = NULLPTR;
}

bool ISceneNode::hasChanged(void) const
{
	return _changed;
}

// Responsibility of caller to add node to the change list
void ISceneNode::setChanged(bool changed)
{
	_changed = changed;
}

bool ISceneNode::isEnabled(void) const
{
	return _enabled;
}

void ISceneNode::setEnabled(bool enabled)
{
	_enabled = enabled;

	for (unsigned int i = 0; i < _children.size(); ++i) {
		_children[i]->setEnabled(enabled);
	}
}

void ISceneNode::updateTransform(void)
{
	Transform transform;

	if (_parent) {
		transform = _parent->getWorldTransform();
		transform.concat(_model_transform);
		_world_transform = transform;
	} else {
		_world_transform = _model_transform;
	}
}

void ISceneNode::update(float dt)
{
	if (_enabled) {
		//for (unsigned int i = 0; i < _animators.size(); ++i) {
		//	_animators[i]->animate(_scene, this, dt);
		//}

		if (_changed || (_parent && _parent->hasChanged())) {
			_scene.getChangeList().push(this);
			updateTransform();
			_changed = true;
		}

		for (unsigned int i = 0; i < _children.size(); ++i) {
			_children[i]->update(dt);
		}
	}
}

NS_END
