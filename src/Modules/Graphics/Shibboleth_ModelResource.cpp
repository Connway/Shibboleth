/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

SHIB_REFLECTION_DEFINE_BEGIN(ModelResource)
	.classAttrs(
		CreatableAttribute(),
		ResExtAttribute(".model.bin"),
		ResExtAttribute(".model"),
		MakeLoadFileCallbackAttribute(&ModelResource::loadModel)
	)

	.base<IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(ModelResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ModelResource)

template <int32_t flag, size_t size>
static int32_t GetIgnoreFlag(const char (&field)[size], const Gaff::ISerializeReader& reader)
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

bool ModelResource::createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiScene& scene)
{
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	if (!scene.HasMeshes()) {
		LogErrorResource("Failed to load mesh '%s'. Assimp scene has no meshes.", getFilePath().getBuffer());
		return false;
	}

	bool succeeded = true;

	for (int32_t i = 0; i < static_cast<int32_t>(scene.mNumMeshes); ++i) {
		const aiMesh* const mesh = scene.mMeshes[i];
		U8String mesh_name = getFilePath().getString() + ":";

		if (mesh->mName.length) {
			mesh_name += mesh->mName.C_Str();
		}
		else {
			mesh_name += '0' + static_cast<char>(i);
		}

		auto mesh_res = res_mgr.createResourceT<MeshResource>(mesh_name.data());

		if (mesh_res->createMesh(devices, *mesh)) {
			_meshes.emplace_back(std::move(mesh_res));
		} else {
			LogErrorResource("Failed to create model '%s'. Failed to load submesh '%s'.", getFilePath().getBuffer(), mesh_name.data());
			succeeded = false;
		}
	}

	return succeeded;
}

bool ModelResource::createMesh(Gleam::IRenderDevice& device, const aiScene& scene)
{
	const Vector<Gleam::IRenderDevice*> devices(1, &device, ProxyAllocator("Graphics"));
	return createMesh(devices, scene);
}

bool ModelResource::createMesh(const Vector<MeshResourcePtr>& meshes)
{
	for (const auto& mesh : meshes) {
		if (mesh->getDevices() != meshes[0]->getDevices()) {
			LogErrorResource("Failed to create model '%s'. Mesh list was not all made from the same device tag.", getFilePath().getBuffer());
			return false;
		}
	}

	_meshes = meshes;
	return true;
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

void ModelResource::loadModel(IFile* file, uintptr_t thread_id_int)
{
	SerializeReaderWrapper readerWrapper;
	
	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load mesh '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();
	const Vector<Gleam::IRenderDevice*>* devices = nullptr;
	const IFile* model_file = nullptr;
	U8String model_file_path;
	U8String device_tag;

	{
		const auto guard = reader.enterElementGuard("devices_tag");
	
		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed mesh '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}
	
		const char* const tag = reader.readString("main");
		device_tag = tag;
		devices = render_mgr.getDevicesByTag(tag);
		reader.freeString(tag);
	}
	
	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load mesh '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer(), device_tag.data());
		failed();
		return;
	}
	
	// Should we put out a read job instead of loading here?
	// Or just assume nowadays most people have SSDs or decent speed hard drives?
	{
		const auto guard = reader.enterElementGuard("model_file");
	
		if (!reader.isString()) {
			LogErrorResource("Malformed mesh '%s'. 'model_file' element is not a string.", getFilePath().getBuffer());
			failed();
			return;
		}
	
		const char* const path = reader.readString();

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


	const size_t index = model_file_path.find_last_of('.');
	
	Assimp::Importer importer;
	
	// Only load mesh data.
	importer.SetPropertyInteger(
		AI_CONFIG_PP_RVC_FLAGS,
		ignore_flags
	);
	
	const aiScene* const scene = importer.ReadFileFromMemory(
		model_file->getBuffer(),
		model_file->size(),
		aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded,
		model_file_path.data() + index
	);
	
	if (!scene) {
		LogErrorResource("Failed to load mesh '%s' with error '%s'", getFilePath().getBuffer(), importer.GetErrorString());
		failed();
		return;
	}

	if (createMesh(*devices, *scene)) {
		succeeded();
	} else {
		failed();
	}
}

NS_END
