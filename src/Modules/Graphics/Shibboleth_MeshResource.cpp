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
		ResExtAttribute(".mesh"),
		MakeLoadFileCallbackAttribute(&MeshResource::loadMesh)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(MeshResource)

const Gleam::IMesh& MeshResource::getMesh(void) const
{
	return *_mesh;
}

Gleam::IMesh& MeshResource::getMesh(void)
{
	return *_mesh;
}

void MeshResource::loadMesh(IFile* file)
{
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

	const RenderManager& render_mgr = GetApp().getManagerTFast<RenderManager>();
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	// Get render device list.

	// If returned zero render devices,

	_mesh.reset(render_mgr.createMesh());

	_buffers.resize(scene->mNumMeshes);

	for (int32_t i = 0; i < static_cast<int32_t>(scene->mNumMeshes); ++i) {
		const U8String res_name = getFilePath().getString() + ":" + scene->mMeshes[i]->mName.C_Str();		
		_buffers[i] = res_mgr.createResourceT<BufferResource>(res_name.data());

		Gleam::IBuffer& buffer = _buffers[i]->getBuffer();
		const aiMesh* const mesh = scene->mMeshes[i];

		Gleam::IBuffer::BufferSettings settings { nullptr, 0, 0, Gleam::IBuffer::BT_VERTEX_DATA, Gleam::IBuffer::MT_NONE, true, 0 };

		// Options for packing here?

		// Calculate total buffer size.
		if (mesh->HasPositions()) {
			settings.size += 3 * sizeof(float) * mesh->mNumVertices;
		}

		if (mesh->HasNormals()) {
			settings.size += 3 * sizeof(float) * mesh->mNumVertices;
		}

		if (mesh->HasTangentsAndBitangents()) {
			settings.size += 6 * sizeof(float) * mesh->mNumVertices;
		}

		for (int32_t j = 0; j < static_cast<int32_t>(mesh->GetNumUVChannels()); ++j) {
			settings.size += mesh->mNumUVComponents[j] * sizeof(float) * mesh->mNumVertices;
		}

		for (int32_t j = 0; j < static_cast<int32_t>(mesh->GetNumColorChannels()); ++j) {
			settings.size += 4 * sizeof(float) * mesh->mNumVertices;
		}

		//buffer.init(rd, settings);

		_mesh->addBuffer(&buffer);
	}
}

NS_END
