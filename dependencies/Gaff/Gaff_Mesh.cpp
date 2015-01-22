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

#include "Gaff_Mesh.h"
#include "Gaff_IncludeAssert.h"
#include <assimp/mesh.h>

NS_GAFF
////////////////
//    Face    //
////////////////
Face::Face(const aiFace* face):
	_face(face)
{
}

Face::Face(const Face& face):
	_face(face._face)
{
}

Face::Face(void):
	_face(nullptr)
{
}

Face::~Face(void)
{
}

unsigned int Face::getNumIndices(void) const
{
	assert(_face);
	return _face->mNumIndices;
}

unsigned int Face::getIndice(unsigned int index) const
{
	assert(_face && index < _face->mNumIndices);
	return _face->mIndices[index];
}

bool Face::valid(void) const
{
	return _face != nullptr;
}

const Face& Face::operator=(const Face& rhs)
{
	_face = rhs._face;
	return *this;
}

bool Face::operator==(const Face& rhs) const
{
	return _face == rhs._face || *_face == *rhs._face;
}

bool Face::operator!=(const Face& rhs) const
{
	return _face != rhs._face && *_face != *rhs._face;
}


/////////////////////////
//    Vertex Weight    //
/////////////////////////
VertexWeight::VertexWeight(const aiVertexWeight* weight):
	_weight(weight)
{
}

VertexWeight::VertexWeight(const VertexWeight& weight):
	_weight(weight._weight)
{
}

VertexWeight::VertexWeight(void):
	_weight(nullptr)
{
}

VertexWeight::~VertexWeight(void)
{
}

unsigned int VertexWeight::getVertexIndex(void) const
{
	assert(_weight);
	return _weight->mVertexId;
}

float VertexWeight::getWeight(void) const
{
	assert(_weight);
	return _weight->mWeight;
}

bool VertexWeight::valid(void) const
{
	return _weight != nullptr;
}

const VertexWeight& VertexWeight::operator=(const VertexWeight& rhs)
{
	_weight = rhs._weight;
	return *this;
}

bool VertexWeight::operator==(const VertexWeight& rhs) const
{
	return _weight == rhs._weight;
}

bool VertexWeight::operator!=(const VertexWeight& rhs) const
{
	return _weight != rhs._weight;
}


////////////////
//    Bone    //
////////////////
Bone::Bone(const aiBone* bone):
	_bone(bone)
{
	setPosRot();
}

Bone::Bone(const Bone& bone):
	_bone(bone._bone)
{
	setPosRot();
}

Bone::Bone(void):
	_bone(nullptr)
{
}

Bone::~Bone(void)
{
}

const char* Bone::getName(void) const
{
	assert(_bone);
	return _bone->mName.C_Str();
}

unsigned int Bone::getNumWeights(void) const
{
	assert(_bone);
	return _bone->mNumWeights;
}

VertexWeight Bone::getWeight(unsigned int index) const
{
	assert(_bone && index < _bone->mNumWeights);
	return VertexWeight(_bone->mWeights + index);
}

const float* Bone::getTransformMatrix(void) const
{
	assert(_bone);
	return &_bone->mOffsetMatrix.a1;
}

const float* Bone::getPosition(void) const
{
	assert(_bone);
	return _pos;
}

const float* Bone::getRotationMatrix(void) const
{
	assert(_bone);
	return _rotation;
}

bool Bone::valid(void) const
{
	return _bone != nullptr;
}

const Bone& Bone::operator=(const Bone& rhs)
{
	_bone = rhs._bone;
	setPosRot();
	return *this;
}

bool Bone::operator==(const Bone& rhs) const
{
	return _bone == rhs._bone;
}

bool Bone::operator!=(const Bone& rhs) const
{
	return _bone != rhs._bone;
}

void Bone::setPosRot(void)
{
	if (!_bone) {
		return;
	}

	_pos[0] = _bone->mOffsetMatrix.a4;
	_pos[1] = _bone->mOffsetMatrix.b4;
	_pos[2] = _bone->mOffsetMatrix.c4;

	_rotation[0] = _bone->mOffsetMatrix.a1;
	_rotation[1] = _bone->mOffsetMatrix.a2;
	_rotation[2] = _bone->mOffsetMatrix.a3;
	_rotation[3] = _bone->mOffsetMatrix.b1;
	_rotation[4] = _bone->mOffsetMatrix.b2;
	_rotation[5] = _bone->mOffsetMatrix.b3;
	_rotation[6] = _bone->mOffsetMatrix.c1;
	_rotation[7] = _bone->mOffsetMatrix.c2;
	_rotation[8] = _bone->mOffsetMatrix.c3;
}


////////////////
//    Mesh    //
////////////////
Mesh::Mesh(const aiMesh* mesh):
	_mesh(mesh)
{
}

Mesh::Mesh(const Mesh& mesh):
	_mesh(mesh._mesh)
{
}

Mesh::Mesh(void):
	_mesh(nullptr)
{
}

Mesh::~Mesh(void)
{
}

bool Mesh::hasFaces(void) const
{
	assert(_mesh);
	return _mesh->HasFaces();
}

bool Mesh::hasBones(void) const
{
	assert(_mesh);
	return _mesh->HasBones();
}

bool Mesh::hasVertices(void) const
{
	assert(_mesh);
	return _mesh->HasPositions();
}

bool Mesh::hasNormals(void) const
{
	assert(_mesh);
	return _mesh->HasNormals();
}

bool Mesh::hasTangentsAndBitangents(void) const
{
	assert(_mesh);
	return _mesh->HasTangentsAndBitangents();
}

bool Mesh::hasColors(unsigned int set) const
{
	assert(_mesh && set < AI_MAX_NUMBER_OF_COLOR_SETS);
	return _mesh->HasVertexColors(set);
}

bool Mesh::hasUVs(unsigned int set) const
{
	assert(_mesh && set < AI_MAX_NUMBER_OF_TEXTURECOORDS);
	return _mesh->HasTextureCoords(set);
}

const char* Mesh::getName(void) const
{
	assert(_mesh);
	return _mesh->mName.C_Str();
}

unsigned int Mesh::getPrimitiveTypes(void) const
{
	assert(_mesh);
	return _mesh->mPrimitiveTypes;
}

unsigned int Mesh::getNumVertices(void) const
{
	assert(_mesh);
	return _mesh->mNumVertices;
}

unsigned int Mesh::getNumFaces(void) const
{
	assert(_mesh);
	return _mesh->mNumFaces;
}

unsigned int Mesh::getNumBones(void) const
{
	assert(_mesh);
	return _mesh->mNumBones;
}

//unsigned int Mesh::getNumAnimMeshes(void) const
//{
//	assert(_mesh);
//	return _mesh->mNumAnimMeshes;
//}

//unsigned int Mesh::getMaterialIndex(void) const
//{
//	assert(_mesh);
//	return _mesh->mMaterialIndex;
//}

unsigned int Mesh::getNumUVComponents(unsigned int channel) const
{
	assert(_mesh && channel < AI_MAX_NUMBER_OF_TEXTURECOORDS);
	return _mesh->mNumUVComponents[channel];
}

unsigned int Mesh::getNumUVChannels(void) const
{
	assert(_mesh);
	return _mesh->GetNumUVChannels();
}

unsigned int Mesh::getNumColorChannels(void) const
{
	assert(_mesh);
	return _mesh->GetNumColorChannels();
}

const float* Mesh::getVertex(unsigned int index) const
{
	assert(_mesh && index < _mesh->mNumVertices);
	return &_mesh->mVertices[index].x;
}

const float* Mesh::getNormals(unsigned int index) const
{
	assert(_mesh && index < _mesh->mNumVertices);
	return &_mesh->mNormals[index].x;
}

const float* Mesh::getTangents(unsigned int index) const
{
	assert(_mesh && index < _mesh->mNumVertices);
	return &_mesh->mTangents[index].x;
}

const float* Mesh::getBitangents(unsigned int index) const
{
	assert(_mesh && index < _mesh->mNumVertices);
	return &_mesh->mBitangents[index].x;
}

const float* Mesh::getVertexColor(unsigned int set, unsigned int index) const
{
	assert(_mesh);
	return (_mesh->mColors[set]) ? &_mesh->mColors[set][index].r : nullptr;
}

const float* Mesh::getUV(unsigned int set, unsigned int index) const
{
	// I'm assuming that if a texture channel doesn't exist, then
	// _mesh->mNumUVComponents[set] will be zero.
	assert(_mesh && _mesh->mTextureCoords[set]);
	return &_mesh->mTextureCoords[set][index].x;
}

Face Mesh::getFace(unsigned int index) const
{
	assert(_mesh && index < _mesh->mNumFaces);
	return Face(_mesh->mFaces + index);
}

Bone Mesh::getBone(unsigned int index) const
{
	assert(_mesh && index < _mesh->mNumBones);
	return Bone(_mesh->mBones[index]);
}

bool Mesh::valid(void) const
{
	return _mesh != nullptr;
}

const Mesh& Mesh::operator=(const Mesh& rhs)
{
	_mesh = rhs._mesh;
	return *this;
}

bool Mesh::operator==(const Mesh& rhs) const
{
	return _mesh == rhs._mesh;
}

bool Mesh::operator!=(const Mesh& rhs) const
{
	return _mesh != rhs._mesh;
}

NS_END
