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

#include "Gaff_SceneNode.h"
#include "Gaff_Animation.h"
#include "Gaff_Camera.h"
#include "Gaff_Light.h"
#include "Gaff_Mesh.h"

struct aiScene;

NS_GAFF

/*!
	\brief
		Class that wraps scene loaded from a SceneImporter.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_scene.html">aiScene Documentation</a> for more details.
*/
class Scene
{
public:
	// Does not copy scene, just the pointer, which the importer controls when it is destroyed.
	// Every class used in scene only copies pointers and never takes ownership. SceneImporter owns scene.
	Scene(const Scene& scene);
	Scene(void);
	~Scene(void);

	INLINE bool hasMeshes(void) const;
	//INLINE bool hasMaterials(void) const;
	INLINE bool hasLights(void) const;
	//INLINE bool hasTextures(void) const;
	INLINE bool hasCameras(void) const;
	INLINE bool hasAnimations(void) const;

	INLINE unsigned int getNumMeshes(void) const;
	//INLINE unsigned int getNumMaterials(void) const;
	INLINE unsigned int getNumLights(void) const;
	//INLINE unsigned int getNumTextures(void) const;
	INLINE unsigned int getNumCameras(void) const;
	INLINE unsigned int getNumAnimations(void) const;

	INLINE Mesh getMesh(unsigned int index) const;
	//INLINE Material getMaterial(unsigned int index) const;
	INLINE Light getLight(unsigned int index) const;
	//INLINE Texture getTexture(unsigned int index) const;
	INLINE Camera getCamera(unsigned int index) const;
	INLINE Animation getAnimation(unsigned int index) const;

	INLINE SceneNode getRootNode(void) const;
	INLINE SceneNode getNode(const char* name) const;

	INLINE bool hasWarnings(void) const;
	INLINE bool valid(void) const;

	INLINE const Scene& operator=(const Scene& rhs);

	INLINE bool operator==(const Scene& rhs) const;
	INLINE bool operator!=(const Scene& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiScene* _scene;

	friend class SceneImporter;
	Scene(const aiScene* scene);
};

NS_END
