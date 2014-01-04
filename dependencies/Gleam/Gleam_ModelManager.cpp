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

#include "Gleam_ModelManager.h"
#include "Gleam_ILayout.h"
#include "Gleam_Model.h"
#include "Gleam_IMesh.h"
#include <cstdio>

NS_GLEAM

ModelManager::ModelManager(void)
{
}

ModelManager::~ModelManager(void)
{
	destroy();
}

void ModelManager::destroy(void)
{
	_name_map.clear();

	for (unsigned int i = 0; i < _models.size(); ++i) {
		_models[i]->release();
	}

	_models.clear();
}

int ModelManager::addModel(IModel* model, const GleamAString& name)
{
	assert(model);

	_models.push(model);
	addEntry(name);
	model->addRef();

	return _models.size() - 1;;
}

bool ModelManager::removeModel(const GleamAString& name)
{
	assert(name.size());
	int index = getIndex(name);

	if (index > -1) {
		_models[index]->release();
		_models.erase(index);

		_name_map.erase(name);

		return true;
	}

	return false;
}

bool ModelManager::removeModel(const IModel* model)
{
	assert(model);
	int index = _models.linearFind((IModel* const)model);

	if (index > -1) {
		removeModel(index);
		return true;
	}

	return false;
}

void ModelManager::removeModel(int index)
{
	assert(index > -1 && (unsigned int)index < _models.size());
	_models[index]->release();
	_models.erase(index);

	for (unsigned int i = 0; i < _name_map.capacity(); ++i) {
		if (_name_map.isOccupied(i) && _name_map.valueAt(i) == index) {
			_name_map.erase(i);
			break;
		}
	}
}

const IModel* ModelManager::getModel(const GleamAString& name) const
{
	assert(name.size());
	int index = _name_map.indexOf(name);

	if (index > -1) {
		return _models[index];
	}

	return NULLPTR;
}

IModel* ModelManager::getModel(const GleamAString& name)
{
	assert(name.size());
	int index = _name_map.indexOf(name);

	if (index > -1) {
		return _models[index];
	}

	return NULLPTR;
}

const IModel* ModelManager::getModel(int index) const
{
	assert(index > -1 && (unsigned int)index < _models.size());
	return _models[index];
}

IModel* ModelManager::getModel(int index)
{
	assert(index > -1 && (unsigned int)index < _models.size());
	return _models[index];
}

int ModelManager::getIndex(const GleamAString& name) const
{
	assert(name.size() && _name_map.indexOf(name) > -1);
	return _name_map[name];
}

int ModelManager::getIndex(const IModel* model) const
{
	assert(model);
	return _models.linearFind((IModel*)model);
}

bool ModelManager::createPlaneModel(const IRenderDevice& rd, unsigned int subdivisions, unsigned int dtg_flags, const IShader* shader, IModel* model)
{
	unsigned int size = 0;

	GleamArray(LayoutDescription) layout_desc;
	GleamAString name = "plane";

	calculateLayoutDescSizeAndNameString(dtg_flags, subdivisions, layout_desc, size, name);

	assert(_name_map.indexOf(name) == -1);

	GleamArray(unsigned int) indices;
	unsigned int num_verts = (subdivisions + 2) * (subdivisions + 2);
	float* vertices = (float*)GleamAllocate(sizeof(float) * num_verts * size);
	float* curr_vert = vertices;
	float x, y;

	if (!vertices) {
		return NULLPTR;
	}

	for (unsigned int j = 0; j < subdivisions + 2; ++j) {
		y = (float)j / (float)(subdivisions + 1);

		for (unsigned int i = 0; i < subdivisions + 2; ++i) {
			x = (float)i / (float)(subdivisions + 1);

			if (dtg_flags & DTG_VERTS) {
				curr_vert[0] = 0.5f - x;
				curr_vert[1] = 0.5f - y;
				curr_vert[2] = 0.0f;
				curr_vert += 3;
			} if (dtg_flags & DTG_NORMALS) {
				curr_vert[0] = 0.0f;
				curr_vert[1] = 0.0f;
				curr_vert[2] = -1.0f;
				curr_vert += 3;
			} if (dtg_flags & DTG_UVS) {
				curr_vert[0] = 1.0f - x;
				curr_vert[1] = y;
				curr_vert += 2;
			} if (dtg_flags & DTG_TANGENTS) {
				curr_vert[0] = 1.0f;
				curr_vert[1] = 0.0f;
				curr_vert[2] = 0.0f;
				curr_vert += 3;
			} if (dtg_flags & DTG_BITANGENTS) {
				curr_vert[0] = 0.0f;
				curr_vert[1] = 1.0f;
				curr_vert[2] = 0.0f;
				curr_vert += 3;
			}
		}
	}

	for (unsigned int j = 0; j < subdivisions + 1; ++j) {
		for (unsigned int i = 0; i < subdivisions + 1; ++i) {
			unsigned int t1 = j * (subdivisions + 2); 
			unsigned int t2 = (j + 1) * (subdivisions + 2); 

			indices.push(t1 + i);
			indices.push(t2 + i);
			indices.push(t1 + i + 1);

			indices.push(t1 + i + 1);
			indices.push(t2 + i);
			indices.push(t2 + i + 1);
		}
	}

	IMesh* mesh = model->createMesh();

	if (!mesh) {
		GleamFree(vertices);
		return false;
	}

	if (!mesh->addVertData(rd, vertices, num_verts, sizeof(float) * size, indices.getArray(), indices.size())) {
		return false;
	}

	GleamFree(vertices);

	if (!model->createLayout(rd, layout_desc.getArray(), layout_desc.size(), shader)) {
		return false;
	}

	_name_map[name] = _models.size();
	_models.push(model);
	model->addRef();

	return true;
}

bool ModelManager::createSphereModel(const IRenderDevice& rd, unsigned int subdivisions, unsigned int dtg_flags, const IShader* shader, IModel* model)
{
	unsigned int size = 0;

	GleamArray(LayoutDescription) layout_desc;
	GleamAString name = "sphere";

	calculateLayoutDescSizeAndNameString(dtg_flags, subdivisions, layout_desc, size, name);

	assert(_name_map.indexOf(name) == -1);

	unsigned int uv_offset = 0, tan_offset = 0, bitan_offset = 0;

	if (dtg_flags & DTG_VERTS) {
		uv_offset += 3;
		tan_offset += 3;
		bitan_offset += 3;
	} if (dtg_flags & DTG_NORMALS) {
		uv_offset += 3;
		tan_offset += 3;
		bitan_offset += 3;
	} if (dtg_flags & DTG_UVS) {
		tan_offset += 2;
		bitan_offset += 2;
	} if (dtg_flags & DTG_TANGENTS) {
		bitan_offset += 3;
	}

	GleamArray(unsigned int) indices;
	unsigned int num_verts = (subdivisions + 2) * (subdivisions + 2);
	float* vertices = (float*)GleamAllocate(sizeof(float) * num_verts * size);
	float* curr_vert = vertices;
	float alpha, beta;
	float sa, sb, ca, cb;
	float x, y;

	if (!vertices) {
		return NULLPTR;
	}

	for (unsigned int j = 0; j < subdivisions + 2; ++j) {
		y = (float)j / (float)(subdivisions + 1);
		alpha = y * Gaff::Pi;
		ca = cosf(alpha);
		sa = sinf(alpha);

		for (unsigned int i = 0; i < subdivisions + 2; ++i) {
			x = (float)i / (float)(subdivisions + 1);
			beta = x * (2.0f * Gaff::Pi);
			cb = cosf(beta);
			sb = sinf(beta);

			float vx = 0.5f * sa * cb;
			float vy = 0.5f * ca;
			float vz = 0.5f * sa * sb;

			if (dtg_flags & DTG_VERTS) {
				curr_vert[0] = vx;
				curr_vert[1] = vy;
				curr_vert[2] = vz;
				curr_vert += 3;

			} if (dtg_flags & DTG_NORMALS) {
				Vec4 nrm(vx, vy, vz, 0.0f);
				nrm.normalize();

				curr_vert[0] = nrm[0];
				curr_vert[1] = nrm[1];
				curr_vert[2] = nrm[2];
				curr_vert += 3;

			} if (dtg_flags & DTG_UVS) {
				curr_vert[0] = x;
				curr_vert[1] = 1.0f - y;
				curr_vert += 2;

			} if (dtg_flags & DTG_TANGENTS) {
				curr_vert[0] = curr_vert[1] = curr_vert[2] = 0.0f;
				curr_vert += 3;
			} if (dtg_flags & DTG_BITANGENTS) {
				curr_vert[0] = curr_vert[1] = curr_vert[2] = 0.0f;
				curr_vert += 3;
			}
		}
	}

	for (unsigned int j = 0; j < subdivisions + 1; ++j) {
		for (unsigned int i = 0; i < subdivisions + 1; ++i) {
			unsigned int t1 = j * (subdivisions + 2); 
			unsigned int t2 = (j + 1) * (subdivisions + 2); 

			indices.push(t1 + i);
			indices.push(t1 + i + 1);
			indices.push(t2 + i);

			indices.push(t1 + i + 1);
			indices.push(t2 + i + 1);
			indices.push(t2 + i);
		}
	}

	if ((dtg_flags & DTG_TANGENTS) || (dtg_flags & DTG_BITANGENTS)) {
		// Not gonna allow tangents and bitangents without having vertices and uvs.
		// There's not much of a reason for you to not have verts and uvs anyways.
		assert((dtg_flags & DTG_VERTS) && (dtg_flags & DTG_UVS));

		generateTangents(vertices, uv_offset, tan_offset, bitan_offset, num_verts, size, indices, dtg_flags);
	}

	IMesh* mesh = model->createMesh();

	if (!mesh) {
		GleamFree(vertices);
		return false;
	}

	if (!mesh->addVertData(rd, vertices, num_verts, sizeof(float) * size, indices.getArray(), indices.size())) {
		return false;
	}

	GleamFree(vertices);

	if (!model->createLayout(rd, layout_desc.getArray(), layout_desc.size(), shader)) {
		return false;
	}

	_name_map[name] = _models.size();
	_models.push(model);
	model->addRef();

	return true;
}

bool ModelManager::createCubeModel(const IRenderDevice& rd, unsigned int subdivisions, unsigned int dtg_flags, const IShader* shader, IModel* model)
{
	unsigned int size = 0;

	GleamArray(LayoutDescription) layout_desc;
	GleamAString name = "plane";

	calculateLayoutDescSizeAndNameString(dtg_flags, subdivisions, layout_desc, size, name);

	assert(_name_map.indexOf(name) == -1);

	GleamArray(unsigned int) indices;
	unsigned int num_verts = (subdivisions + 2) * (subdivisions + 2);
	float* vertices = (float*)GleamAllocate(sizeof(float) * num_verts * size * 6);
	float* curr_vert = vertices;
	float x, y;
	float rot = 0.0f;

	if (!vertices) {
		return NULLPTR;
	}

	for (unsigned int side = 0; side < 6; ++side) {
		Matrix4x4 rot_matrix;

		if (side == 4) {
			rot_matrix.setRotationX(Gaff::Pi / 2.0f);
		} else if (side == 5) {
			rot_matrix.setRotationX(-Gaff::Pi / 2.0f);
		} else {
			rot_matrix.setRotationY(rot);
			rot -= Gaff::Pi / 2.0f;
		}

		for (unsigned int j = 0; j < subdivisions + 2; ++j) {
			y = (float)j / (float)(subdivisions + 1);

			for (unsigned int i = 0; i < subdivisions + 2; ++i) {
				x = (float)i / (float)(subdivisions + 1);

				Vec4 vec;

				if (dtg_flags & DTG_VERTS) {
					vec.set(0.5f - x, 0.5f - y, -0.5f, 1.0f);
					vec = rot_matrix * vec;

					curr_vert[0] = vec[0];
					curr_vert[1] = vec[1];
					curr_vert[2] = vec[2];
					curr_vert += 3;

				} if (dtg_flags & DTG_NORMALS) {
					vec.set(0.0f, 0.0f, -1.0f, 0.0f);
					vec = rot_matrix * vec;

					curr_vert[0] = vec[0];
					curr_vert[1] = vec[1];
					curr_vert[2] = vec[2];
					curr_vert += 3;

				} if (dtg_flags & DTG_UVS) {
					curr_vert[0] = 1.0f - x;
					curr_vert[1] = y;
					curr_vert += 2;

				} if (dtg_flags & DTG_TANGENTS) {
					vec.set(1.0f, 0.0f, 0.0f, 0.0f);
					vec = rot_matrix * vec;

					curr_vert[0] = vec[0];
					curr_vert[1] = vec[1];
					curr_vert[2] = vec[2];
					curr_vert += 3;

				} if (dtg_flags & DTG_BITANGENTS) {
					vec.set(0.0f, 1.0f, 0.0f, 0.0f);
					vec = rot_matrix * vec;

					curr_vert[0] = vec[0];
					curr_vert[1] = vec[1];
					curr_vert[2] = vec[2];
					curr_vert += 3;
				}
			}
		}
	}

	for (unsigned int j = 0; j < subdivisions + 1; ++j) {
		for (unsigned int i = 0; i < subdivisions + 1; ++i) {
			unsigned int t1 = j * (subdivisions + 2); 
			unsigned int t2 = (j + 1) * (subdivisions + 2); 

			indices.push(t1 + i);
			indices.push(t2 + i);
			indices.push(t1 + i + 1);

			indices.push(t1 + i + 1);
			indices.push(t2 + i);
			indices.push(t2 + i + 1);
		}
	}

	// add the indices for the rest of the cube faces
	unsigned int num_indices = indices.size();

	for (unsigned int side = 0; side < 5; ++side) {
		for (unsigned int i = 0; i < num_indices; ++i) {
			indices.push(indices[i] + (side + 1) * num_verts);
		}
	}

	IMesh* mesh = model->createMesh();

	if (!mesh) {
		GleamFree(vertices);
		return false;
	}

	if (!mesh->addVertData(rd, vertices, num_verts * 6, sizeof(float) * size, indices.getArray(), indices.size())) {
		return false;
	}

	GleamFree(vertices);

	if (!model->createLayout(rd, layout_desc.getArray(), layout_desc.size(), shader)) {
		return false;
	}

	_name_map[name] = _models.size();
	_models.push(model);
	model->addRef();

	return true;
}

void ModelManager::calculateLayoutDescSizeAndNameString(unsigned int dtg_flags, unsigned int subdivisions,
														GleamArray(LayoutDescription)& layout_desc,
														unsigned int& size, GleamAString& name) const
{
	bool first = true;
	size = 0;

	if (dtg_flags & DTG_VERTS) {
		name += "v";
		size += 3;

		LayoutDescription desc = {
			SEMANTIC_POSITION,
			0,
			ITexture::RGB_32_F,
			0,
			(first) ? 0 : APPEND_ALIGNED
		};

		layout_desc.push(desc);

		first = false;

	} if (dtg_flags & DTG_NORMALS) {
		name += "n";
		size += 3;

		LayoutDescription desc = {
			SEMANTIC_NORMAL,
			0,
			ITexture::RGB_32_F,
			0,
			(first) ? 0 : APPEND_ALIGNED
		};

		layout_desc.push(desc);

		first = false;

	} if (dtg_flags & DTG_UVS) {
		name += "u";
		size += 2;

		LayoutDescription desc = {
			SEMANTIC_TEXCOORD,
			0,
			ITexture::RG_32_F,
			0,
			(first) ? 0 : APPEND_ALIGNED
		};

		layout_desc.push(desc);

		first = false;

	} if (dtg_flags & DTG_TANGENTS) {
		name += "t";
		size += 3;

		LayoutDescription desc = {
			SEMANTIC_TANGENT,
			0,
			ITexture::RGB_32_F,
			0,
			(first) ? 0 : APPEND_ALIGNED
		};

		layout_desc.push(desc);

		first = false;

	} if (dtg_flags & DTG_BITANGENTS) {
		name += "b";
		size += 3;

		LayoutDescription desc = {
			SEMANTIC_BITANGENT,
			0,
			ITexture::RGB_32_F,
			0,
			(first) ? 0 : APPEND_ALIGNED
		};

		layout_desc.push(desc);
	}

	char buf[4];
	std::sprintf(buf, "%i", subdivisions);

	name += buf;
}

void ModelManager::generateTangents(float* vertices, unsigned int uv_offset, unsigned int tan_offset,
									unsigned int bitan_offset, unsigned int num_verts, unsigned int vert_size,
									const GleamArray(unsigned int)& indices, unsigned int dtg_flags)
{
	GleamArray(unsigned int) face_count(num_verts, (unsigned int)0);

	Vec4 v1, v2, v3;
	Vec4 st1, st2, st3;
	Vec4 temp1, temp2, temp3;

	unsigned int index;

	for (unsigned int i = 0; i < indices.size(); i += 3) {
		index = indices[i] * vert_size;
		v1.set(vertices[index], vertices[index + 1], vertices[index + 2], 0.0f);
		st1.set(vertices[index + uv_offset], vertices[index + uv_offset + 1], 0.0f, 0.0f);

		index = indices[i + 1] * vert_size;
		v2.set(vertices[index], vertices[index + 1], vertices[index + 2], 0.0f);
		st2.set(vertices[index + uv_offset], vertices[index + uv_offset + 1], 0.0f, 0.0f);

		index = indices[i + 2] * vert_size;
		v3.set(vertices[index], vertices[index + 1], vertices[index + 2], 0.0f);
		st3.set(vertices[index + uv_offset], vertices[index + uv_offset + 1], 0.0f, 0.0f);

		temp1 = v2 - v1;
		temp2 = v3 - v1;
		temp3.set(st2[0] - st1[0], st2[1] - st2[1], st3[0] - st1[0], st3[1] - st1[1]);

		float r = 1.0f / (temp3[0] * temp3[3] - temp3[2] * temp3[1]);

		if (dtg_flags & DTG_TANGENTS) {
			v1.set((temp3[3] * temp1[0] - temp3[1] * temp2[0]) * r, (temp3[3] * temp1[1] - temp3[1] * temp2[1]) * r, (temp3[3] * temp1[2] - temp3[1] * temp2[2]) * r, 0.0f);
			vertices[index + tan_offset] += v1[0];
			vertices[index + tan_offset + 1] += v1[1];
			vertices[index + tan_offset + 2] += v1[2];
		}

		if (dtg_flags & DTG_BITANGENTS) {
			v2.set((temp3[0] * temp2[0] - temp3[2] * temp1[0]) * r, (temp3[0] * temp2[1] - temp3[2] * temp1[1]) * r, (temp3[0] * temp2[2] - temp3[2] * temp1[2]) * r, 0.0f);
			vertices[index + bitan_offset] += v2[0];
			vertices[index + bitan_offset + 1] += v2[1];
			vertices[index + bitan_offset + 2] += v2[2];
		}

		++face_count[indices[i]];
		++face_count[indices[i + 1]];
		++face_count[indices[i + 2]];
	}

	unsigned int count;

	for (unsigned int i = 0; i < num_verts; ++i) {
		count = face_count[i];
		index = i * vert_size;

		if (dtg_flags & DTG_TANGENTS) {
			temp1.set(vertices[index + tan_offset], vertices[index + tan_offset + 1], vertices[index + tan_offset + 2], 0.0f);
			temp1 /= (float)count;
			temp1.normalize();

			vertices[index + tan_offset] = temp1[0];
			vertices[index + tan_offset + 1] = temp1[1];
			vertices[index + tan_offset + 2] = temp1[2];
		}

		if (dtg_flags & DTG_BITANGENTS) {
			temp2.set(vertices[index + bitan_offset], vertices[index + bitan_offset + 1], vertices[index + bitan_offset + 2], 0.0f);
			temp2 /= (float)count;
			temp2.normalize();

			vertices[index + bitan_offset] = temp2[0];
			vertices[index + bitan_offset + 1] = temp2[1];
			vertices[index + bitan_offset + 2] = temp2[2];
		}
	}
}

void ModelManager::addEntry(const GleamAString& name)
{
	if (!name.size()) {
		return;
	}

	int index = _models.size() - 1;
	assert(_name_map.indexOf(name) == -1);
	_name_map[name] = index;
}

NS_END
