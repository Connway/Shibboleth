/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_ModelResource.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_IFileSystem.h>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ModelResource)
	.classAttrs(
		Shibboleth::CreatableAttribute(),
		Shibboleth::ResExtAttribute(u8".model.bin"),
		Shibboleth::ResExtAttribute(u8".model"),
		Shibboleth::MakeLoadFileCallbackAttribute(&Shibboleth::ModelResource::loadModel)
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ModelResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ModelResource)

template <int32_t flag, size_t size>
static int32_t GetIgnoreFlag(const char (&field)[size], const ISerializeReader& reader)
{
	const auto guard = reader.enterElementGuard(field);
	return (reader.readBool(false)) ? flag : 0;
}

Vector<Gleam::IRenderDevice*> ModelResource::getDevices(void) const
{
	if (!_meshes.empty()) {
		return _meshes[0]->getDevices();
	}

	return Vector<Gleam::IRenderDevice*>{ ProxyAllocator("Graphics") };
}

bool ModelResource::createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiScene& scene, const Vector<int32_t>& centering_meshes)
{
	ResourceManager& res_mgr = GetManagerTFast<ResourceManager>();

	if (!scene.HasMeshes()) {
		LogErrorResource("Failed to load mesh '%s'. Assimp scene has no meshes.", getFilePath().getBuffer());
		return false;
	}

	bool succeeded = true;

	for (int32_t i = 0; i < static_cast<int32_t>(scene.mNumMeshes); ++i) {
		const aiMesh* const mesh = scene.mMeshes[i];
		U8String mesh_name = getFilePath().getString() + u8":";

		if (mesh->mName.length) {
			CONVERT_STRING(char8_t, temp_name, mesh->mName.C_Str());
			mesh_name += temp_name;
		} else {
			mesh_name.append_sprintf(u8"%i", i);
		}

		auto mesh_res = res_mgr.createResourceT<MeshResource>(mesh_name.data());

		if (mesh_res->createMesh(devices, *mesh)) {
			_meshes.emplace_back(std::move(mesh_res));

		} else {
			LogErrorResource("Failed to create model '%s'. Failed to load submesh '%s'.", getFilePath().getBuffer(), mesh_name.data());
			succeeded = false;
		}
	}

	Gleam::AABB aabb;

	for (int32_t index : centering_meshes) {
		if (index >= static_cast<int32_t>(_meshes.size())) {
			// $TODO: Log error.
			continue;
		}

		aabb.addAABB(_meshes[index]->getAABB());
	}

	_centering_vector = -aabb.getCenter();
	return succeeded;
}

bool ModelResource::createMesh(Gleam::IRenderDevice& device, const aiScene& scene, const Vector<int32_t>& centering_meshes)
{
	const Vector<Gleam::IRenderDevice*> devices(1, &device, ProxyAllocator("Graphics"));
	return createMesh(devices, scene, centering_meshes);
}

bool ModelResource::createMesh(const Vector<MeshResourcePtr>& meshes, const Vector<int32_t>& centering_meshes)
{
	for (const auto& mesh : meshes) {
		if (mesh->getDevices() != meshes[0]->getDevices()) {
			LogErrorResource("Failed to create model '%s'. Mesh list was not all made from the same device tag.", getFilePath().getBuffer());
			return false;
		}
	}

	Gleam::AABB aabb;

	for (int32_t index : centering_meshes) {
		if (index >= static_cast<int32_t>(meshes.size())) {
			// $TODO: Log error.
			continue;
		}

		aabb.addAABB(meshes[index]->getAABB());
	}

	_centering_vector = -aabb.getCenter();
	_meshes = meshes;
	return true;
}

bool ModelResource::createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiScene& scene)
{
	Vector<int32_t> centering_meshes;
	return createMesh(devices, scene, centering_meshes);
}

bool ModelResource::createMesh(Gleam::IRenderDevice& device, const aiScene& scene)
{
	Vector<int32_t> centering_meshes;
	return createMesh(device, scene, centering_meshes);
}

bool ModelResource::createMesh(const Vector<MeshResourcePtr>& meshes)
{
	Vector<int32_t> centering_meshes;
	return createMesh(meshes, centering_meshes);
}

const MeshResourcePtr& ModelResource::getMesh(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_meshes.size()));
	return _meshes[index];
}

int32_t ModelResource::getNumMeshes(void) const
{
	return static_cast<int32_t>(_meshes.size());
}

const Gleam::Vec3& ModelResource::getCenteringVector(void) const
{
	return _centering_vector;
}

void ModelResource::loadModel(IFile* file, uintptr_t thread_id_int)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load mesh '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const RenderManagerBase& render_mgr = GETMANAGERT(Shibboleth::RenderManagerBase, Shibboleth::RenderManager);
	ResourceManager& res_mgr = GetManagerTFast<ResourceManager>();
	const ISerializeReader& reader = *readerWrapper.getReader();
	const Vector<Gleam::IRenderDevice*>* devices = nullptr;
	const IFile* model_file = nullptr;
	U8String model_file_path;
	U8String device_tag;

	{
		const auto guard = reader.enterElementGuard(u8"devices_tag");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed mesh '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char8_t* const tag = reader.readString(u8"main");
		device_tag = tag;
		devices = render_mgr.getDevicesByTag(tag);
		reader.freeString(tag);
	}

	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load mesh '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer(), device_tag.data());
		failed();
		return;
	}

	{
		const auto guard = reader.enterElementGuard(u8"model_file");

		if (!reader.isString()) {
			LogErrorResource("Malformed mesh '%s'. 'model_file' element is not a string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char8_t* const path = reader.readString();

		model_file_path = path;
		model_file = res_mgr.loadFileAndWait(path, thread_id_int);

		reader.freeString(path);
	}

	if (!model_file) {
		LogErrorResource("Failed to load mesh '%s'. Unable to find or load file '%s'.", getFilePath().getBuffer(), model_file_path.data());
		failed();
		return;
	}

	const int32_t ignore_flags =	aiComponent_ANIMATIONS | aiComponent_CAMERAS |
									aiComponent_LIGHTS | aiComponent_MATERIALS |
									aiComponent_TEXTURES |
									GetIgnoreFlag<aiComponent_BONEWEIGHTS>("ignore_blend_weights", reader) |
									GetIgnoreFlag<aiComponent_COLORS>("ignore_colors", reader) |
									GetIgnoreFlag<aiComponent_NORMALS>("ignore_normals", reader) |
									GetIgnoreFlag<aiComponent_TANGENTS_AND_BITANGENTS>("ignore_tangents", reader) |
									GetIgnoreFlag<aiComponent_TEXCOORDS>("ignore_uvs", reader);


	const size_t index = model_file_path.rfind(u8'.');

	Assimp::Importer importer;

	// Only load mesh data.
	importer.SetPropertyInteger(
		AI_CONFIG_PP_RVC_FLAGS,
		ignore_flags
	);

	const aiScene* const scene = importer.ReadFileFromMemory(
		model_file->getBuffer(),
		model_file->size(),
		aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded | static_cast<unsigned int>(aiProcess_GenBoundingBoxes),
		reinterpret_cast<const char*>(model_file_path.data() + index)
	);

	if (!scene) {
		LogErrorResource("Failed to load mesh '%s' with error '%s'", getFilePath().getBuffer(), importer.GetErrorString());
		failed();
		return;
	}

	Vector<int32_t> centering_meshes(ProxyAllocator("Resource"));

	{
		const auto guard = reader.enterElementGuard(u8"center");

		if (reader.isTrue()) {
			centering_meshes.set_capacity(scene->mNumMeshes);

			for (int32_t i = 0; i < static_cast<int32_t>(scene->mNumMeshes); ++i) {
				centering_meshes.emplace_back(i);
			}

		} else if (reader.isArray()) {
			reader.forEachInArray([&](int32_t) -> bool
			{
				if (!reader.isInt32()) {
					// $TODO: Log error.
					return false;
				}

				centering_meshes.emplace_back(reader.readInt32());
				return false;
			});
		}
	}

	if (createMesh(*devices, *scene, centering_meshes)) {
		succeeded();
	} else {
		failed();
	}
}

NS_END
