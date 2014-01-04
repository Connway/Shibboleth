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

#include "Gleam_Scene.h"

NS_GLEAM

Scene::Scene(unsigned int num_stages):
	_node_types(num_stages, GleamArray(ISceneNode*)()), _camera(NULLPTR), _root(NULLPTR)
{
}

Scene::~Scene(void)
{
	destroy();
}

bool Scene::init(void)
{
	assert(!_root);

	_root = GleamAllocateT(ISceneNode);
	Gaff::construct<ISceneNode, Scene&>(_root, *this);

	if (!_root) {
		return false;
	}

	_root->addRef();
	return true;
}

void Scene::destroy(void)
{
	if (_camera) {
		_camera->release();
		_camera = NULLPTR;
	}

	for (unsigned int i = 0; i < _node_types.size(); ++i) {
		GleamArray(ISceneNode*)& nodes = _node_types[i];

		for (unsigned int j = 0; j < nodes.size(); ++j) {
			nodes[j]->removeFromParent();
			nodes[j]->release();
		}
	}

	_node_types.clear();

	_root->release();
	_root = NULLPTR;
}

const ISceneNode* Scene::getRoot(void) const
{
	assert(_root);
	return _root;
}

ISceneNode* Scene::getRoot(void)
{
	assert(_root);
	return _root;
}

const ISceneNode* Scene::getCamera(void) const
{
	return _camera;
}

ISceneNode* Scene::getCamera(void)
{
	return _camera;
}

void Scene::setCamera(ISceneNode* camera)
{
	SAFEGLEAMRELEASE(_camera)
	_camera = camera;
	camera->addRef();
}

const GleamArray(ISceneNode*)& Scene::getNodesOfType(unsigned int type) const
{
	assert(type < _node_types.size());
	return _node_types[type];
}

GleamArray(ISceneNode*)& Scene::getNodesOfType(unsigned int type)
{
	assert(type < _node_types.size());
	return _node_types[type];
}

void Scene::addNode(ISceneNode* node, ISceneNode* parent)
{
	assert(node && parent);
	node->setParent(parent);
	parent->addChild(node);

	unsigned int node_type = node->getNodeType();

	while (_node_types.size() < node_type + 1) {
		_node_types.push(GleamArray(ISceneNode*)());
	}

	_node_types[node_type].push(node);
	node->addRef();
}

void Scene::addNode(ISceneNode* node)
{
	assert(node);
	addNode(node, _root);
}

void Scene::removeNode(ISceneNode* node)
{
	assert(node && node->getNodeType() < _node_types.size());
	int index = _node_types[node->getNodeType()].linearFind(node);
	assert(index > -1);

	node->removeFromParent();
	_node_types[node->getNodeType()].erase(index);
	node->release();
}

const GleamArray(ISceneNode*)& Scene::getChangeList(void) const
{
	return _change_list;
}

GleamArray(ISceneNode*)& Scene::getChangeList(void)
{
	return _change_list;
}

void Scene::update(float dt)
{
	assert(_root);
	_root->update(dt);

	// reset the changed nodes' changed flag
	for (unsigned int i = 0; i < _change_list.size(); ++i) {
		_change_list[i]->setChanged(false);
	}

	_change_list.clear();
}

NS_END
