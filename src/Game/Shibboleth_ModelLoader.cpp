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

#include "Shibboleth_ModelLoader.h"
#include "Shibboleth_ModelAnimResources.h"
#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Gleam_IRenderDevice.h>
#include <esprit_Skeleton.h>
#include <Gleam_ILayout.h>
#include <Gleam_IModel.h>
#include <Gleam_IMesh.h>
#include <Gaff_ScopedExit.h>
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
static const char* d3d11_blend_indices_shader_chunk = "uint4 blend_indices : BLENDINDICES";
static const char* d3d11_blend_weights_shader_chunk = "float4 blend_weights : BLENDWEIGHT";

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


ModelLoader::ModelLoader(RenderManager& render_mgr, ResourceManager& res_mgr, IFileSystem& file_system):
	_render_mgr(render_mgr), _res_mgr(res_mgr), _file_system(file_system),
	_esprit_proxy_allocator("esprit")
{
	esprit::SetAllocator(&_esprit_proxy_allocator);
}

ModelLoader::~ModelLoader(void)
{
}

Gaff::IVirtualDestructor* ModelLoader::load(const char* file_name, uint64_t, HashMap<AString, IFile*>& file_map)
{
	GAFF_ASSERT(file_map.hasElementWithKey(AString(file_name)));

	GAFF_SCOPE_EXIT([&]()
	{
		for (auto it = file_map.begin(); it != file_map.end(); ++it) {
			if (*it) {
				_file_system.closeFile(*it);
				*it = nullptr;
			}
		}
	});

	IFile* file = file_map[AString(file_name)];

	Gaff::JSON json;
	
	if (!json.parse(file->getBuffer())) {
		// log error
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
	if (!lod_tags.isArray() && lod_tags) {
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

		if (check_fail) {
			return nullptr;
		}
	}

	ModelData* data = GetAllocator()->template allocT<ModelData>();

	if (data) {
		data->holding_data = _res_mgr.loadResourceImmediately(mesh_file.getString(), generateLoadingFlags(json), file_map);

		// Loop until loaded. Just in case we got a resource reference requested from somewhere else.
		while (!data->holding_data.getResourcePtr()->isLoaded() && !data->holding_data.getResourcePtr()->hasFailed()) {
		}

		if (!data->holding_data) {
			// log error
			GetAllocator()->freeT(data);
			data = nullptr;

		} else if (!data->holding_data->scene) {
			// log error
			GetAllocator()->freeT(data);
			data = nullptr;

		} else if (!data->holding_data->scene.hasMeshes()) {
			// log error
			GetAllocator()->freeT(data);
			data = nullptr;

		} else {
			Gaff::JSON display_tags = json["display_tags"];
			unsigned short disp_tags = DT_ALL;
			bool any_display_tags = (json["any_display_with_tags"] && json["any_display_tags"].isTrue());

			if (!display_tags.isNull()) {
				if (!display_tags.isArray()) {
					// log error
					GetAllocator()->freeT(data);
					return nullptr;
				}

				disp_tags = 0;

				if (EXTRACT_DISPLAY_TAGS(display_tags, disp_tags)) {
					GetAllocator()->freeT(data);
					return nullptr;
				}
			}

			if (lod_tags && data->holding_data->scene.getNumMeshes() != lod_tags.size()) {
				// log error
				GetAllocator()->freeT(data);
				data = nullptr;

			} else if (!loadMeshes(data, lod_tags, json, disp_tags, any_display_tags)) {
				// log error
				GetAllocator()->freeT(data);
				data = nullptr;
			}
		}
	}

	return data;
}

bool ModelLoader::loadMeshes(ModelData* data, const Gaff::JSON& lod_tags, const Gaff::JSON& model_prefs, unsigned short display_tags, bool any_display_tags)
{
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	size_t num_lods = (!lod_tags) ? 1 : lod_tags.size();
	unsigned int num_bone_weights = 0;

	Array< Array<VertSkeletonData> > vert_skeleton_data;

	// First dimension is for each device.
	data->models.resize(num_lods, rd.getNumDevices());

	Array<unsigned int> devices = (any_display_tags) ?
		_render_mgr.getDevicesWithTagsAny(display_tags) :
		_render_mgr.getDevicesWithTags(display_tags);

	// Second dimension is number of LODs. If we don't define LOD tags in
	// the JSON file, then we assume all meshes are of LOD 0.
	for (auto it = devices.begin(); it != devices.end(); ++it) {
		rd.setCurrentDevice(*it);

		// Pre-create all the model data structures.
		for (size_t i = 0; i < num_lods; ++i) {
			ModelPtr model(_render_mgr.createModel());

			if (!model) {
				// Log error
				return false;
			}

			data->models[*it][i] = model;
		}
	}

	if (model_prefs["blend_indices_weights"] && model_prefs["blend_indices_weights"].isTrue()) {
		if (!model_prefs["skeleton_root"] || !model_prefs["skeleton_root"].isString()) {
			// log error
			return false;
		}

		//if (!model_prefs["bone_tag"] || !model_prefs["bone_tag"].isString()) {
		//	// log warning
		//}

		if (!loadSkeleton(data, model_prefs, num_bone_weights, vert_skeleton_data)) {
			// log error
			return false;
		}
	}


	// Do this for each device
	//for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
	for (auto it = devices.begin(); it != devices.end(); ++it) {
		rd.setCurrentDevice(*it);

		// Iterate over all the meshes
		for (unsigned int j = 0; j < data->holding_data->scene.getNumMeshes(); ++j) {
			Gaff::Mesh scene_mesh = data->holding_data->scene.getMesh(j);

			if (!scene_mesh) {
				// Log Error
				return false;
			}

			// Determine the LOD level of this mesh
			size_t lod = SIZE_T_FAIL;

			if (!lod_tags) {
				lod = 0;

			} else {
				lod_tags.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
				{
					if (Gaff::FindFirstOf(scene_mesh.getName(), value.getString()) != SIZE_T_FAIL) {
						lod = index;
						return true;
					}

					return false;
				});

				if (lod == SIZE_T_FAIL) {
					// log warning saying we found a mesh
					// that was not specified with an LOD level
					continue;
				}
			}

			// Only calculate AABBs for LOD 0
			if (it == devices.begin() && lod == 0) {
				Gleam::AABBCPU aabb;

				for (unsigned int k = 0; k < scene_mesh.getNumVertices(); ++k) {
					aabb.addPoint(Gleam::Vector4CPU(scene_mesh.getVertex(k)[0], scene_mesh.getVertex(k)[1], scene_mesh.getVertex(k)[2], 1.0f));
				}

				data->aabbs.emplacePush(aabb);
			}

			if (!createMeshAndLayout(rd, scene_mesh, data->models[*it][lod].get(), model_prefs, num_bone_weights, vert_skeleton_data[j])) {
				return false;
			}

			// load blend weights
		}
	}

	data->aabbs.trim();

	// Calculate the total AABB
	for (auto it = data->aabbs.begin(); it != data->aabbs.end(); ++it) {
		data->combined_aabb.addAABB(*it);
	}

	return true;
}

bool ModelLoader::createMeshAndLayout(Gleam::IRenderDevice& rd, const Gaff::Mesh& scene_mesh, Gleam::IModel* model, const Gaff::JSON& model_prefs, unsigned int num_bone_weights, const Array<VertSkeletonData>& vert_skeleton_data)
{
	bool uvs = false;
	bool normals = false;
	bool tangents = false;
	bool blend_data = false;
	bool vertex_color = false;
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

	if (model_prefs["blend_indices_weights"] && model_prefs["blend_indices_weights"].isTrue()) {
		blend_data = true;
		vert_size += 8 * num_bone_weights;

		for (unsigned int i = 0; i < num_bone_weights; ++i) {
			// Iterate and do this for every 4 blend indices/weights
			Gleam::LayoutDescription desc = {
				Gleam::SEMANTIC_BLEND_INDICES,
				i,
				Gleam::ITexture::RGBA_32_UI,
				0,
				APPEND_ALIGNED,
				Gleam::PDT_PER_VERTEX
			};

			layout_desc.push(desc);
		}

		for (unsigned int i = 0; i < num_bone_weights; ++i) {
			// Iterate and do this for every 4 blend indices/weights
			Gleam::LayoutDescription desc = {
				Gleam::SEMANTIC_BLEND_WEIGHT,
				i,
				Gleam::ITexture::RGBA_32_F,
				0,
				APPEND_ALIGNED,
				Gleam::PDT_PER_VERTEX
			};

			layout_desc.push(desc);
		}
	}

	if (model_prefs["vertex_color"]) {
		vertex_color = true;

		for (unsigned int i = 0; i < scene_mesh.getNumColorChannels(); ++i) {
			Gleam::LayoutDescription desc = {
				Gleam::SEMANTIC_COLOR,
				i,
				Gleam::ITexture::RGBA_32_F,
				0,
				APPEND_ALIGNED,
				Gleam::PDT_PER_VERTEX
			};

			layout_desc.push(desc);
		}
	}

	Gleam::IMesh* mesh = model->createMesh();

	if (!mesh) {
		// Log error
		return false;
	}

	float* vertices = reinterpret_cast<float*>(GetAllocator()->alloc(sizeof(float) * scene_mesh.getNumVertices() * vert_size));

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
			unsigned int* blend_indices = reinterpret_cast<unsigned int*>(current_vertex);
			float* blend_weights = current_vertex + 4 * num_bone_weights;

			// Initialize to zero
			for (unsigned int j = 0; j < 8 * num_bone_weights; ++j) {
				current_vertex[j] = 0.0f;
			}

			const VertSkeletonData& vsd = vert_skeleton_data[(current_vertex - vertices) / vert_size];

			for (size_t j = 0; j < vsd.bone_indices.size(); ++j) {
				blend_indices[j] = vsd.bone_indices[j];
				blend_weights[j] = vsd.bone_weights[j];
			}

			current_vertex += 8 * num_bone_weights;
		}

		if (vertex_color) {
			for (unsigned int j = 0; j < scene_mesh.getNumColorChannels(); ++j) {
				memcpy(current_vertex, scene_mesh.getVertexColor(j, i), sizeof(float) * 4);
				current_vertex += 4;
			}
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

	if (!mesh->addVertData(rd, vertices, scene_mesh.getNumVertices(), sizeof(float) * vert_size, indices.getArray(), static_cast<unsigned int>(indices.size()))) {
		return false;
	}

	GetAllocator()->free(vertices);

	Gleam::IShader* shader = nullptr;

	// Generate bogus shader for use with model->createLayout()
	if (rd.getRendererType() == Gleam::RENDERER_DIRECT3D) {
		shader = generateEmptyD3D11Shader(rd, model_prefs, scene_mesh, num_bone_weights);

		if (!shader) {
			return false;
		}
	}

	if (!model->createLayout(rd, layout_desc.getArray(), static_cast<unsigned int>(layout_desc.size()), shader)) {
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

	if (model_prefs["remove_degenerates"] && model_prefs["remove_degenerates"].isTrue()) {
		flags |= Gaff::SI_FIND_DEGENERATES;
	}

	return flags;
}

Gleam::IShader* ModelLoader::generateEmptyD3D11Shader(Gleam::IRenderDevice& rd, const Gaff::JSON& model_prefs, const Gaff::Mesh& scene_mesh, size_t num_bone_weights) const
{
	Gleam::IShader* shader = _render_mgr.createShader();

	if (!shader) {
		// Log error
		return nullptr;
	}

	shader->addRef(); // Count is initialized to zero. Increment so we clean up properly.

	AString shader_code(d3d11_begin_shader_chunk);

	if (model_prefs["normals"] && model_prefs["normals"].isTrue()) {
		shader_code += d3d11_normal_shader_chunk;
	}

	if (model_prefs["tangents"] && model_prefs["tangents"].isTrue()) {
		shader_code += d3d11_tangents_shader_chunk;
	}

	if (model_prefs["uvs"] && model_prefs["uvs"].isTrue()) {
		char temp[3] = { 0,0, 0 };

		for (unsigned int i = 0; i < scene_mesh.getNumUVChannels(); ++i) {
			_itoa(static_cast<int>(i), temp, 10); // values should be in [1, 9]

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

	if (model_prefs["blend_indices_weights"] && model_prefs["blend_indices_weights"].isTrue()) {

		char temp[3] = { 0, 0, 0 }; // I imagine our bone count will always be below 100. :)

		for (size_t i = 0; i < num_bone_weights; ++i) {
			shader_code += d3d11_blend_indices_shader_chunk;
			shader_code += _itoa(static_cast<int>(i), temp, 10);
			shader_code += ';';
		}

		for (size_t i = 0; i < num_bone_weights; ++i) {
			shader_code += d3d11_blend_weights_shader_chunk;
			shader_code += _itoa(static_cast<int>(i), temp, 10);
			shader_code += ';';
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

bool ModelLoader::loadSkeleton(ModelData* data, const Gaff::JSON& model_prefs, unsigned int& num_bone_weights, Array< Array<VertSkeletonData> >& vert_skeleton_data)
{
	// Determine number of float4's for vertex data
	unsigned int max_vertex_bones = 0;
	num_bone_weights = 0;

	vert_skeleton_data.resize(data->holding_data->scene.getNumMeshes());

	// Count which vertex has the most bones associated with it
	for (unsigned int k = 0; k < data->holding_data->scene.getNumMeshes(); ++k) {
		Gaff::Mesh scene_mesh = data->holding_data->scene.getMesh(k);

		// Temporarily to hold vertex bone weight count
		Array<unsigned int> num_bones(scene_mesh.getNumVertices(), 0);

		for (unsigned int i = 0; i < scene_mesh.getNumBones(); ++i) {
			Gaff::Bone bone = scene_mesh.getBone(i);

			if (bone) {
				for (unsigned int j = 0; j < bone.getNumWeights(); ++j) {
					Gaff::VertexWeight weight = bone.getWeight(j);

					if (weight) {
						++num_bones[weight.getVertexIndex()];
					}
				}
			}
		}

		vert_skeleton_data[k].resize(scene_mesh.getNumVertices());

		for (size_t i = 0; i < num_bones.size(); ++i) {
			max_vertex_bones = Gaff::Max(max_vertex_bones, num_bones[i]);

			vert_skeleton_data[k][i].bone_indices.reserve(num_bones[i]);
			vert_skeleton_data[k][i].bone_weights.reserve(num_bones[i]);
		}
	}

	num_bone_weights = static_cast<unsigned int>(ceilf(static_cast<float>(max_vertex_bones) / 4.0f));

	if (!num_bone_weights) {
		return true;
	}

	// Load skeleton hierarchy
	// This should load a skeleton that is sorted by parent index. Root being bone 0.
	Gaff::SceneNode root = data->holding_data->scene.getNode(model_prefs["skeleton_root"].getString());

	if (!root) {
		// log error
		return false;
	}

	const char* bone_tag = (model_prefs["bone_tag"]) ? model_prefs["bone_tag"].getString() : nullptr;
	Array<Gaff::SceneNode> nodes(1, root);

	while (!nodes.empty()) {
		for (size_t i = 0; i < nodes.size();) {
			if (!nodes[i]) {
				// log error
				return false;
			}

			// If we don't have a bone tag, we assume everything in the hierarchy is a bone
			if (bone_tag) {
				AString name(nodes[i].getName());

				// If this is not a bone, just ignore it
				if (name.findFirstOf(bone_tag) != 0) {
					nodes.erase(i);
					continue;
				}
			}

			size_t parent_index = SIZE_T_FAIL;

			if (nodes[i].getParent()) {
				parent_index = data->skeleton.getBoneIndex(nodes[i].getParent().getName());
			}

			data->skeleton.addBone(parent_index, nodes[i].getName());

			// If we do not have a parent index, then we are the root and just use identity transform
			if (parent_index != SIZE_T_FAIL) {
				size_t my_index = data->skeleton.getNumBones() - 1;

				Gleam::Matrix4x4SIMD matrix(nodes[i].getTransform());
				Gleam::TransformSIMD transform;

				// We lose scale information when we convert this to our transform class.
				// Most likely fine. Bones probably shouldn't have scale information anyways.
				matrix.transposeThis(); // Make assimp's matrix match our major
				transform.setTranslation(matrix.getColumn(3));
				transform.setRotation(Gleam::QuaternionSIMD::MakeFromMatrix(matrix));

				data->skeleton.setReferenceTransform(my_index, transform);
			}

			++i;
		}

		// Make the new list of nodes to add
		Array<Gaff::SceneNode> new_nodes;

		for (auto it = nodes.begin(); it != nodes.end(); ++it) {
			for (unsigned int i = 0; i < it->getNumChildren(); ++i) {
				new_nodes.push(it->getChild(i));
			}
		}

		nodes = std::move(new_nodes);
	}

	// Cache some bone vert data for later
	for (size_t i = 0; i < data->holding_data->scene.getNumMeshes(); ++i) {
		Gaff::Mesh scene_mesh = data->holding_data->scene.getMesh(static_cast<unsigned int>(i));

		for (unsigned int j = 0; j < scene_mesh.getNumBones(); ++j) {
			Gaff::Bone bone = scene_mesh.getBone(j);

			if (bone) {
				for (unsigned int k = 0; k < bone.getNumWeights(); ++k) {
					Gaff::VertexWeight weight = bone.getWeight(k);

					if (weight && weight.getVertexIndex() == static_cast<unsigned int>(i)) {
						vert_skeleton_data[i][weight.getVertexIndex()].bone_indices.push(static_cast<unsigned int>(data->skeleton.getBoneIndex(bone.getName())));
						vert_skeleton_data[i][weight.getVertexIndex()].bone_weights.push(weight.getWeight());
					}
				}
			}
		}
	}

	return true;
}

NS_END
