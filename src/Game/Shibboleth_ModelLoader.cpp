/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_ModelLoader.h"
#include "Shibboleth_ModelAnimResources.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_Allocator.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_ILayout.h>
#include <Gleam_IMesh.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

static const char* d3d11_begin_shader_chunk =
"struct VertexInputType"
"{"
	"float3 position : POSITION0;";
static const char* d3d11_normal_shader_chunk = "float3 normal : NORMAL0;";
static const char* d3d11_tangents_shader_chunk = "float3 tangent : TANGENT0; float3 bitangent : BINORMAL0;";
static const char* d3d11_uv_1_shader_chunk = "float uv : TEXCOORD";
static const char* d3d11_uv_2_shader_chunk = "float2 uv : TEXCOORD";
static const char* d3d11_uv_3_shader_chunk = "float3 uv : TEXCOORD";
static const char* d3d11_uv_4_shader_chunk = "float4 uv : TEXCOORD";
static const char* d3d11_end_shader_chunk = 
"};" // For end of VertexInputType struct
// This struct is empty, since the dummy shader doesn't need it
"struct PixelInputType"
"{"
"};"

"PixelInputType VertexMain(VertexInputType input)"
"{"
	"PixelInputType output;"
	"return output;"
"}"
;


ModelLoader::ModelLoader(RenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

ModelLoader::~ModelLoader(void)
{
}

Gaff::IVirtualDestructor* ModelLoader::load(const char* file_name, unsigned long long)
{
	Gaff::JSON json;
	
	if (!json.parseFile(file_name)) {
		return nullptr;
	}

	if (!json.isObject()) {
		// log error
		return nullptr;
	}

	Gaff::JSON mesh_file = json["mesh_file"];
	Gaff::JSON lod_tags = json["lod_tags"];

	if (!mesh_file.isString()) {
		// log error
		return nullptr;
	}

	// We either specify mesh LODs, or we don't. If we don't, then we assume all meshes are LOD 0.
	// If we specify LOD tags and run across a mesh without the tag, we write a warning to the log
	// and ignore the mesh.
	if (!lod_tags.isArray() && !lod_tags.isNull()) {
		// log error
		return nullptr;
	}

	// Check LOD tags are correct data type.
	if (lod_tags.isArray()) {
		bool check_fail = lod_tags.forEachInArray([](size_t, const Gaff::JSON& value) -> bool
		{
			if (!value.isString()) {
				// log error
				return true;
			}

			return false;
		});

		if (!check_fail) {
			return nullptr;
		}
	}

	ModelData* data = GetAllocator()->template allocT<ModelData>();

	if (data) {
		data->scene = data->importer.loadFile(mesh_file.getString(), generateLoadingFlags(json));

		if (!data->scene) {
			GetAllocator()->freeT(data);
			data = nullptr;
		}

		// Log warnings if we have any
		//if (data->scene.hasWarnings()) {
		//}

		if (data && data->scene.hasMeshes()) {
			if (data->scene.getNumMeshes() != lod_tags.size()) {
				// log error
				GetAllocator()->freeT(data);
				data = nullptr;

			} else if (!loadMeshes(data, lod_tags, json)) {
				GetAllocator()->freeT(data);
				data = nullptr;
			}
		}
	}

	return data;
}

bool ModelLoader::loadMeshes(ModelData* data, const Gaff::JSON& lod_tags, const Gaff::JSON& model_prefs)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_render_mgr.getSpinLock());
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	unsigned int num_lods = (lod_tags.isNull()) ? 1 : lod_tags.size();

	// First dimension is for each device.
	data->models.resize(rd.getNumDevices());
	data->aabbs.resize(num_lods);

	// Second dimension is number of LODs. If we don't define LOD tags in
	// the JSON file, then we assume all meshes are of LOD 0.
	for (auto it1 = data->models.begin(); it1 != data->models.end(); ++it1) {
		rd.setCurrentDevice(it1 - data->models.begin());
		it1->resize(num_lods);

		// Pre-create all the model data structures.
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
			ModelPtr model(_render_mgr.createModel());

			if (!model) {
				// Log error
				return false;
			}

			*it2 = model;
		}
	}

	// Do this for each device
	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		rd.setCurrentDevice(i);

		// Iterate over all the meshes
		for (unsigned int j = 0; j < data->scene.getNumMeshes(); ++j) {
			Gaff::Mesh scene_mesh = data->scene.getMesh(j);

			if (!scene_mesh) {
				// Log Error
				return false;
			}

			// Determine the LOD level of this mesh
			int lod = -1;

			if (lod_tags.isNull()) {
				lod = 0;

			} else {
				lod_tags.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
				{
					AString mesh_name(scene_mesh.getName());

					if (mesh_name.findFirstOf(value.getString()) != AString::npos) {
						lod = index;
						return true;
					}

					return false;
				});

				if (lod == -1) {
					// log warning saying we found a mesh
					// that was not specified with an LOD level
					continue;
				}
			}

			// Only need to do this once
			if (i == 0) {
				Gleam::AABB aabb;

				for (unsigned int k = 0; k < scene_mesh.getNumVertices(); ++k) {
					aabb.addPoint(Gleam::Vec4(scene_mesh.getVertex(k)[0], scene_mesh.getVertex(k)[1], scene_mesh.getVertex(k)[2], 1.0f));
				}

				data->aabbs[lod].push(aabb);
			}

			if (!createMeshAndLayout(rd, scene_mesh, data->models[i][lod].get(), model_prefs)) {
				return false;
			}

			// load blend weights
		}
	}

	// Resize the AABB arrays to be exact
	for (auto it = data->aabbs.begin(); it != data->aabbs.end(); ++it) {
		it->trim();
	}

	return true;
}

bool ModelLoader::createMeshAndLayout(Gleam::IRenderDevice& rd, const Gaff::Mesh& scene_mesh, Gleam::IModel* model, const Gaff::JSON& model_prefs)
{
	bool uvs = false;
	bool normals = false;
	bool tangents = false;
	bool blend_data = false;
	unsigned int vert_size = 3; // defaults to 3 for position
	unsigned int uv_size = 0;

	Array<Gleam::LayoutDescription> layout_desc;

	{
		Gleam::LayoutDescription desc = {
			Gleam::SEMANTIC_POSITION,
			0,
			Gleam::ITexture::RGB_32_F,
			0,
			0,
			Gleam::PDT_PER_VERTEX
		};

		layout_desc.push(desc);
	}

	if (model_prefs["normals"] && model_prefs["normals"].isTrue() && scene_mesh.hasVertices()) {
		normals = true;
		vert_size += 3;

		Gleam::LayoutDescription desc = {
			Gleam::SEMANTIC_NORMAL,
			0,
			Gleam::ITexture::RGB_32_F,
			0,
			APPEND_ALIGNED,
			Gleam::PDT_PER_VERTEX
		};

		layout_desc.push(desc);
	}

	if (model_prefs["tangents"] && model_prefs["tangents"].isTrue() && scene_mesh.hasTangentsAndBitangents()) {
		tangents = true;
		vert_size += 6;

		Gleam::LayoutDescription desc = {
			Gleam::SEMANTIC_TANGENT,
			0,
			Gleam::ITexture::RGB_32_F,
			0,
			APPEND_ALIGNED,
			Gleam::PDT_PER_VERTEX
		};

		layout_desc.push(desc);

		desc.semantic = Gleam::SEMANTIC_BITANGENT;
		layout_desc.push(desc);
	}

	if (model_prefs["uvs"] && model_prefs["uvs"].isTrue() && scene_mesh.hasUVs(0)) {
		uvs = true;

		for (unsigned int i = 0; i < scene_mesh.getNumUVChannels(); ++i) {
			uv_size += scene_mesh.getNumUVComponents(i);

			Gleam::LayoutDescription desc = {
				Gleam::SEMANTIC_TEXCOORD,
				i,
				Gleam::ITexture::RG_32_F,
				0,
				APPEND_ALIGNED,
				Gleam::PDT_PER_VERTEX
			};

			switch (scene_mesh.getNumUVComponents(i)) {
				case 1:
					desc.format = Gleam::ITexture::R_32_F;
					break;

				case 3:
					desc.format = Gleam::ITexture::RGB_32_F;
					break;

				case 4:
					desc.format = Gleam::ITexture::RGBA_32_F;
					break;

				default:
					break;
			}

			layout_desc.push(desc);
		}

		vert_size += uv_size;
	}

	if (model_prefs["blend_indices_weights"] && model_prefs["blend_indices_weights"].isTrue() && scene_mesh.hasBones()) {
		//blend_data = true;
		//vert_size += 8;

		//Gleam::LayoutDescription desc = {
		//	Gleam::SEMANTIC_BLEND_INDICES,
		//	0,
		//	//Gleam::ITexture::RGBA_32_F,
		//	Gleam::ITexture::RGBA_32_UI,
		//	0,
		//	APPEND_ALIGNED,
		//	Gleam::PDT_PER_VERTEX
		//};

		//layout_desc.push(desc);

		//desc.semantic = Gleam::SEMANTIC_BLEND_WEIGHT;
		//desc.format = Gleam::ITexture::RGBA_32_F;
		//layout_desc.push(desc);

		// load skeleton
	}

	Gleam::IMesh* mesh = model->createMesh();

	if (!mesh) {
		// Log error
		return false;
	}

	float* vertices = (float*)GetAllocator()->alloc(sizeof(float) * scene_mesh.getNumVertices() * vert_size);

	if (!vertices) {
		// Log error
		return false;
	}

	float* current_vertex = vertices;

	for (unsigned int i = 0; i < scene_mesh.getNumVertices(); ++i) {
		memcpy(current_vertex, scene_mesh.getVertex(i), sizeof(float) * 3);
		current_vertex += 3;

		if (normals) {
			memcpy(current_vertex, scene_mesh.getNormals(i), sizeof(float) * 3);
			current_vertex += 3;
		}

		if (tangents) {
			memcpy(current_vertex, scene_mesh.getTangents(i), sizeof(float) * 3);
			memcpy(current_vertex + 3, scene_mesh.getBitangents(i), sizeof(float) * 3);
			current_vertex += 6;
		}

		if (uvs) {
			for (unsigned int j = 0; j < scene_mesh.getNumUVChannels(); ++j) {
				uv_size = scene_mesh.getNumUVComponents(j);
				memcpy(current_vertex, scene_mesh.getUV(j, i), sizeof(float) * uv_size);
				current_vertex += uv_size;
			}
		}

		if (blend_data) {
			//unsigned int* blend_indices = (unsigned int*)current_vertex;
			//float* blend_weights = current_vertex + 4;
			//unsigned int curr_ind = 0;

			//blend_indices[0] = blend_indices[1] = blend_indices[2] = blend_indices[3] = 0;
			//blend_weights[0] = blend_weights[1] = blend_weights[2] = blend_weights[3] = 0.0f;

			//for (unsigned int j = 0; j < scene_mesh.getNumBones(); ++j) {
			//	Gaff::Bone bone = scene_mesh.getBone(j);

			//	if (bone) {
			//		for (unsigned int k = 0; k < bone.getNumWeights(); ++k) {
			//			Gaff::VertexWeight weight = bone.getWeight(k);

			//			if (weight && weight.getVertexIndex() == i) {
			//				blend_indices[curr_ind] = weight.getVertexIndex();
			//				blend_weights[curr_ind] = weight.getWeight();
			//				++curr_ind;
			//			}
			//		}
			//	}
			//}

			//current_vertex += 8;
		}
	}

	Array<unsigned int> indices;

	for (unsigned int i = 0; i < scene_mesh.getNumFaces(); ++i) {
		Gaff::Face face = scene_mesh.getFace(i);

		if (!face) {
			return false;
		}

		for (unsigned int j = 0; j < face.getNumIndices(); ++j) {
			indices.push(face.getIndice(j));
		}
	}

	if (!mesh->addVertData(rd, vertices, scene_mesh.getNumVertices(), sizeof(float) * vert_size, indices.getArray(), indices.size())) {
		return false;
	}

	GetAllocator()->free(vertices);

	Gleam::IShader* shader = nullptr;

	// Generate bogus shader for use with model->createLayout()
	if (rd.isD3D()) {
		shader = generateEmptyD3D11Shader(rd, model_prefs, scene_mesh);

		if (!shader) {
			return false;
		}
	}

	if (!model->createLayout(rd, layout_desc.getArray(), layout_desc.size(), shader)) {
		// Log error
		return false;
	}

	if (shader) {
		shader->release();
	}

	return true;
}

unsigned int ModelLoader::generateLoadingFlags(const Gaff::JSON& model_prefs)
{
	// Assumes that the model will only have primitive types that are of type triangle or polygon.
	unsigned int flags = Gaff::SI_LIMIT_BONE_WEIGHTS; // Limits bone weights to 4.

	if (model_prefs["generate_smooth_normals"] && model_prefs["generate_smooth_normals"].isTrue()) {
		flags |= Gaff::SI_GENERATE_SMOOTH_NORMALS;
	} else if (model_prefs["normals"] && model_prefs["normals"].isTrue()) {
		flags |= Gaff::SI_GENERATE_NORMALS;
	}

	if (model_prefs["uvs"] && model_prefs["uvs"].isTrue()) {
		flags |= Gaff::SI_GENERATE_UVS;
	}

	if (model_prefs["tangents"] && model_prefs["tangents"].isTrue()) {
		flags |= Gaff::SI_CALC_TANGENTS;
	}

	if (model_prefs["join_identical_verts"] && model_prefs["join_identical_verts"].isTrue()) {
		flags |= Gaff::SI_JOIN_IDENTICAL_VERTS;
	}

	if (model_prefs["make_left_handed"] && model_prefs["make_left_handed"].isTrue()) {
		flags |= Gaff::SI_MAKE_LEFT_HANDED;
	}

	if (model_prefs["triangulate"] && model_prefs["triangulate"].isTrue()) {
		flags |= Gaff::SI_TRIANGULATE;
	}

	if (model_prefs["fix_in_facing_normals"] && model_prefs["fix_in_facing_normals"].isTrue()) {
		flags |= Gaff::SI_FIX_IN_FACING_NORMALS;
	}

	if (model_prefs["optimize_meshes"] && model_prefs["optimize_meshes"].isTrue()) {
		flags |= Gaff::SI_OPTIMIZE_MESHES;
	}

	if (model_prefs["optimize_graph"] && model_prefs["optimize_graph"].isTrue()) {
		flags |= Gaff::SI_OPTIMIZE_GRAPH;
	}

	if (model_prefs["flip_uvs"] && model_prefs["flip_uvs"].isTrue()) {
		flags |= Gaff::SI_FLIP_UVS;
	}

	if (model_prefs["flip_winding"] && model_prefs["flip_winding"].isTrue()) {
		flags |= Gaff::SI_FLIP_WINDING;
	}

	return flags;
}

Gleam::IShader* ModelLoader::generateEmptyD3D11Shader(Gleam::IRenderDevice& rd, const Gaff::JSON& model_prefs, const Gaff::Mesh& scene_mesh) const
{
	Gleam::IShader* shader = _render_mgr.createShader();

	if (!shader) {
		// Log error
		return nullptr;
	}

	AString shader_code(d3d11_begin_shader_chunk);

	if (model_prefs["normals"] && model_prefs["normals"].isTrue()) {
		shader_code += d3d11_normal_shader_chunk;
	}

	if (model_prefs["tangents"] && model_prefs["tangents"].isTrue()) {
		shader_code += d3d11_tangents_shader_chunk;
	}

	if (model_prefs["uvs"] && model_prefs["uvs"].isTrue()) {
		char temp[2] = { 0, 0 };

		for (unsigned int i = 0; i < scene_mesh.getNumUVChannels(); ++i) {
			shader_code += _itoa(scene_mesh.getNumUVComponents(i), temp, 10); // values should be in [1, 4]

			switch (scene_mesh.getNumUVComponents(i)) {
				case 1:
					shader_code += d3d11_uv_1_shader_chunk;
					shader_code += temp;
					shader_code += ';';
					break;

				case 2:
					shader_code += d3d11_uv_2_shader_chunk;
					shader_code += temp;
					shader_code += ';';
					break;

				case 3:
					shader_code += d3d11_uv_3_shader_chunk;
					shader_code += temp;
					shader_code += ';';
					break;

				case 4:
					shader_code += d3d11_uv_4_shader_chunk;
					shader_code += temp;
					shader_code += ';';
					break;

				default:
					break;
			}
		}
	}

	shader_code += d3d11_end_shader_chunk;

	if (!shader->initVertexSource(rd, shader_code.getBuffer())) {
		// Log error
		shader->release();
		shader = nullptr;
	}

	return shader;
}

NS_END
