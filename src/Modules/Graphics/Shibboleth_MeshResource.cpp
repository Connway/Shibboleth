/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_MeshResource.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

SHIB_REFLECTION_DEFINE(MeshResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(MeshResource)
	.classAttrs(
		CreatableAttribute(),
		ResExtAttribute(".mesh.bin"),
		ResExtAttribute(".mesh"),
		MakeLoadFileCallbackAttribute(&MeshResource::loadMesh)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(MeshResource)

const Gleam::IMesh* MeshResource::getMesh(const Gleam::IRenderDevice& rd) const
{
	const auto it = _meshes.find(&rd);
	return (it != _meshes.end()) ? it->second.get() : nullptr;
}

Gleam::IMesh* MeshResource::getMesh(const Gleam::IRenderDevice& rd)
{
	const auto it = _meshes.find(&rd);
	return (it != _meshes.end()) ? it->second.get() : nullptr;
}

void MeshResource::loadMesh(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load mesh '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const RenderManager& render_mgr = GetApp().getManagerTFast<RenderManager>();
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();
	const Vector<Gleam::IRenderDevice*>* devices = nullptr;
	IFile* model_file = nullptr;

	{
		const auto guard = reader.enterElementGuard("devices_tag");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed mesh '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char* const tag = reader.readString("main");
		devices = render_mgr.getDevicesByTag(tag);
		reader.freeString(tag);
	}

	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load mesh '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer());
		failed();
		return;
	}

	{
		const auto guard = reader.enterElementGuard("model_file");

		if (!reader.isString()) {
			LogErrorResource("Malformed mesh '%s'. 'model_file' element is not a string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char* const path = reader.readString();
		model_file = GetApp().getFileSystem().openFile(path);
		reader.freeString(path);
	}

	Assimp::Importer importer;

	// Only load mesh data.
	importer.SetPropertyInteger(
		AI_CONFIG_PP_RVC_FLAGS,
		aiComponent_ANIMATIONS | aiComponent_TEXTURES |
		aiComponent_LIGHTS | aiComponent_CAMERAS |
		aiComponent_MATERIALS
	);

	const aiScene* const scene = importer.ReadFileFromMemory(
		file->getBuffer(),
		file->size(),
		aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded
	);

	if (!scene) {
		LogErrorResource("Failed to load mesh '%s' with error '%s'", getFilePath().getBuffer(), importer.GetErrorString());
		failed();
		return;
	}

	if (!scene->HasMeshes()) {
		LogErrorResource("Failed to load mesh '%s'. Assimp scene has no meshes.", getFilePath().getBuffer());
		failed();
		return;
	}

	_buffers.reserve(devices->size());
	_meshes.reserve(devices->size());

	for (Gleam::IRenderDevice* rd : *devices) {
		auto& buffers = _buffers[rd];
		buffers.resize(scene->mNumMeshes);

		_meshes[rd].reset(render_mgr.createMesh());
	}

	for (int32_t i = 0; i < static_cast<int32_t>(scene->mNumMeshes); ++i) {
		const aiMesh* const mesh = scene->mMeshes[i];

		Gleam::IBuffer::BufferSettings settings { nullptr, 0, 0, Gleam::IBuffer::BT_VERTEX_DATA, Gleam::IBuffer::MT_NONE, true, 0 };

		// Options for packing here?

		constexpr size_t PosSize = sizeof(float) * 3;
		constexpr size_t NrmSize = sizeof(float) * 3;
		constexpr size_t TanSize = sizeof(float) * 3;
		constexpr size_t UVSize = sizeof(float);
		constexpr size_t ClrSize = sizeof(float) * 4;

		// Calculate total buffer size.
		if (mesh->HasPositions()) {
			settings.size += PosSize * mesh->mNumVertices;
		}

		if (mesh->HasNormals()) {
			settings.size += NrmSize * mesh->mNumVertices;
		}

		if (mesh->HasTangentsAndBitangents()) {
			settings.size += TanSize * 2 * mesh->mNumVertices;
		}

		for (int32_t j = 0; j < static_cast<int32_t>(mesh->GetNumUVChannels()); ++j) {
			settings.size += UVSize * mesh->mNumUVComponents[j] * mesh->mNumVertices;
		}

		for (int32_t j = 0; j < static_cast<int32_t>(mesh->GetNumColorChannels()); ++j) {
			settings.size += ClrSize * mesh->mNumVertices;
		}

		settings.stride = static_cast<int32_t>(settings.size / mesh->mNumVertices);

		void* const data = SHIB_ALLOC_POOL(settings.size, GetAllocator().getPoolIndex("Graphics"), GetAllocator());
		int8_t* curr = reinterpret_cast<int8_t*>(data);

		settings.data = data;

		for (int32_t j = 0; j < static_cast<int32_t>(mesh->mNumVertices); ++j) {
			if (mesh->HasPositions()) {
				memcpy(curr, &mesh->mVertices[j], PosSize);
				curr += PosSize;
			}

			if (mesh->HasNormals()) {
				memcpy(curr, &mesh->mNormals[j], NrmSize);
				curr += NrmSize;
			}

			if (mesh->HasTangentsAndBitangents()) {
				memcpy(curr, &mesh->mTangents[j], TanSize);
				curr += TanSize;

				memcpy(curr, &mesh->mBitangents[j], TanSize);
				curr += TanSize;
			}

			for (int32_t k = 0; k < static_cast<int32_t>(mesh->GetNumUVChannels()); ++k) {
				memcpy(curr, &mesh->mTextureCoords[k], UVSize * mesh->mNumUVComponents[k]);
				curr += UVSize * mesh->mNumUVComponents[k];
			}

			for (int32_t k = 0; k < static_cast<int32_t>(mesh->GetNumColorChannels()); ++k) {
				memcpy(curr, &mesh->mColors[k], ClrSize);
				curr += ClrSize;
			}
		}

		for (int32_t j = 0; j < static_cast<int32_t>(devices->size()); ++j) {
			Gleam::IRenderDevice* const rd = devices->at(j);
			auto& buffers = _buffers[rd];

			const U8String res_name = getFilePath().getString() + ":" + scene->mMeshes[i]->mName.C_Str() + ":" + ('0' + static_cast<char>(j));
			buffers[i] = res_mgr.createResourceT<BufferResource>(res_name.data());

			Gleam::IBuffer& buffer = buffers[i]->getBuffer();

			if (!buffer.init(*rd, settings)) {
				LogErrorResource("Failed to load mesh '%s'. Failed to create buffer %i.", getFilePath().getBuffer(), i);
				failed();
				continue;
			}

			_meshes[rd]->addBuffer(&buffer);
		}

		SHIB_FREE(data, GetAllocator());
	}
}

NS_END
