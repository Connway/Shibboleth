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

#include "Gaff_Scene.h"
#include "Gaff_IncludeAssert.h"
#include <assimp/scene.h>

NS_GAFF

Scene::Scene(const aiScene* scene):
	_scene(scene)
{
}

Scene::Scene(const Scene& scene):
	_scene(scene._scene)
{
}

Scene::Scene(void):
	_scene(nullptr)
{
}

Scene::~Scene(void)
{
}

bool Scene::hasMeshes(void) const
{
	assert(_scene);
	return _scene->HasMeshes();
}

//bool Scene::hasMaterials(void) const
//{
//	assert(_scene);
//	return _scene->HasMaterials();
//}

bool Scene::hasLights(void) const
{
	assert(_scene);
	return _scene->HasLights();
}

//bool Scene::hasTextures(void) const
//{
//	assert(_scene);
//	return _scene->HasTextures();
//}

bool Scene::hasCameras(void) const
{
	assert(_scene);
	return _scene->HasCameras();
}

bool Scene::hasAnimations(void) const
{
	assert(_scene);
	return _scene->HasAnimations();
}

unsigned int Scene::getNumMeshes(void) const
{
	assert(_scene);
	return _scene->mNumMeshes;
}

//unsigned int Scene::getNumMaterials(void) const
//{
//	assert(_scene);
//	return _scene->mNumMaterials;
//}

unsigned int Scene::getNumLights(void) const
{
	assert(_scene);
	return _scene->mNumLights;
}

//unsigned int Scene::getNumTextures(void) const
//{
//	assert(_scene);
//	return _scene->mNumTextures;
//}

unsigned int Scene::getNumCameras(void) const
{
	assert(_scene);
	return _scene->mNumCameras;
}

unsigned int Scene::getNumAnimations(void) const
{
	assert(_scene);
	return _scene->mNumAnimations;
}

Mesh Scene::getMesh(unsigned int index) const
{
	assert(_scene && index < _scene->mNumMeshes);
	return Mesh(_scene->mMeshes[index]);
}

//Material Scene::getMaterial(unsigned int index) const
//{
//	assert(_scene && index < _scene->mNumMaterials);
//	return Material(_scene->mMaterials[index]);
//}

Light Scene::getLight(unsigned int index) const
{
	assert(_scene && index < _scene->mNumLights);
	return Light(_scene->mLights[index]);
}

//Texture Scene::getTexture(unsigned int index) const
//{
//	assert(_scene && index < _scene->mNumTextures);
//	return Texture(_scene->mTextures[index]);
//}

Camera Scene::getCamera(unsigned int index) const
{
	assert(_scene && index < _scene->mNumCameras);
	return Camera(_scene->mCameras[index]);
}

Animation Scene::getAnimation(unsigned int index) const
{
	assert(_scene && index < _scene->mNumAnimations);
	return Animation(_scene->mAnimations[index]);
}

SceneNode Scene::getRootNode(void) const
{
	assert(_scene);
	return SceneNode(_scene->mRootNode);
}

SceneNode Scene::getNode(const char* name) const
{
	assert(_scene);
	return SceneNode(_scene->mRootNode->FindNode(name));
}

bool Scene::hasWarnings(void) const
{
	return (_scene->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING) != 0;
}

bool Scene::valid(void) const
{
	return _scene != nullptr && !(_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE);
}

const Scene& Scene::operator=(const Scene& rhs)
{
	_scene = rhs._scene;
	return *this;
}

bool Scene::operator==(const Scene& rhs) const
{
	return _scene == rhs._scene;
}

bool Scene::operator!=(const Scene& rhs) const
{
	return _scene != rhs._scene;
}

NS_END
