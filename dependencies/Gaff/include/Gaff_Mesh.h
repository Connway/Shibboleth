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
#include <assimp/mesh.h>

NS_GAFF

#define MAX_NUM_VERT_COLORS AI_MAX_NUMBER_OF_COLOR_SETS
#define MAX_NUM_UVS AI_MAX_NUMBER_OF_TEXTURECOORDS

enum MeshPrimitives
{
	MP_POINT = aiPrimitiveType_POINT,
	MP_LINE = aiPrimitiveType_LINE,
	MP_TRIANGLE = aiPrimitiveType_TRIANGLE,
	MP_POLYGON = aiPrimitiveType_POLYGON
};

/*!
	\brief
		Class that wraps face data extracted from a loaded Mesh.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_face.html">aiFace Documentation</a> for more details.
*/
class Face
{
public:
	Face(const Face& face);
	Face(void);
	~Face(void);

	INLINE unsigned int getNumIndices(void) const;
	INLINE unsigned int getIndice(unsigned int index) const;

	INLINE bool valid(void) const;

	INLINE const Face& operator=(const Face& rhs);

	INLINE bool operator==(const Face& rhs) const;
	INLINE bool operator!=(const Face& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiFace* _face;

	friend class Mesh;
	Face(const aiFace* face);
};

/*!
	\brief
		Class that wraps vertex weight data extracted from a loaded Bone.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_vertex_weight.html">aiVertexWeight Documentation</a> for more details.
*/
class VertexWeight
{
public:
	VertexWeight(const VertexWeight& weight);
	VertexWeight(void);
	~VertexWeight(void);

	INLINE unsigned int getVertexIndex(void) const;
	INLINE float getWeight(void) const;

	INLINE bool valid(void) const;

	INLINE const VertexWeight& operator=(const VertexWeight& rhs);

	INLINE bool operator==(const VertexWeight& rhs) const;
	INLINE bool operator!=(const VertexWeight& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiVertexWeight* _weight;

	friend class Bone;
	VertexWeight(const aiVertexWeight* weight);
};

/*!
	\brief
		Class that wraps bone data extracted from a loaded Mesh.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_bone.html">aiBone Documentation</a> for more details.
*/
class Bone
{
public:
	Bone(const Bone& bone);
	Bone(void);
	~Bone(void);

	INLINE const char* getName(void) const;
	INLINE unsigned int getNumWeights(void) const;
	INLINE VertexWeight getWeight(unsigned int index) const;

	INLINE const float* getTransformMatrix(void) const;
	INLINE const float* getPosition(void) const;
	INLINE const float* getRotationMatrix(void) const;

	INLINE bool valid(void) const;

	INLINE const Bone& operator=(const Bone& rhs);

	INLINE bool operator==(const Bone& rhs) const;
	INLINE bool operator!=(const Bone& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	float _rotation[9];
	float _pos[3];

	const aiBone* _bone;

	void setPosRot(void);

	friend class Mesh;
	Bone(const aiBone* bone);
};

/*!
	\brief
		Class that wraps mesh data extracted from a loaded Scene.
		See <a href="http://assimp.sourceforge.net/lib_html/structai_mesh.html">aiMesh Documentation</a> for more details.
*/
class Mesh
{
public:
	Mesh(const Mesh& mesh);
	Mesh(void);
	~Mesh(void);

	INLINE bool hasFaces(void) const;
	INLINE bool hasBones(void) const;
	INLINE bool hasVertices(void) const;
	INLINE bool hasNormals(void) const;
	INLINE bool hasTangentsAndBitangents(void) const;
	INLINE bool hasColors(unsigned int set) const;
	INLINE bool hasUVs(unsigned int set) const;

	INLINE const char* getName(void) const;
	INLINE unsigned int getPrimitiveTypes(void) const;
	INLINE unsigned int getNumVertices(void) const;
	INLINE unsigned int getNumFaces(void) const;
	INLINE unsigned int getNumBones(void) const;
	//INLINE unsigned int getNumAnimMeshes(void) const;
	//INLINE unsigned int getMaterialIndex(void) const;
	INLINE unsigned int getNumUVComponents(unsigned int channel) const;
	INLINE unsigned int getNumUVChannels(void) const;
	INLINE unsigned int getNumColorChannels(void) const;

	INLINE const float* getVertex(unsigned int index) const;
	INLINE const float* getNormals(unsigned int index) const;
	INLINE const float* getTangents(unsigned int index) const;
	INLINE const float* getBitangents(unsigned int index) const;
	INLINE const float* getVertexColor(unsigned int set, unsigned int index) const;
	INLINE const float* getUV(unsigned int set, unsigned int index) const;

	INLINE Face getFace(unsigned int index) const;
	INLINE Bone getBone(unsigned int index) const;

	INLINE bool valid(void) const;

	INLINE const Mesh& operator=(const Mesh& rhs);

	INLINE bool operator==(const Mesh& rhs) const;
	INLINE bool operator!=(const Mesh& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiMesh* _mesh;

	friend class Scene;
	Mesh(const aiMesh* Mesh);
};

NS_END
