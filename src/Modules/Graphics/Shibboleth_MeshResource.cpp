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
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_LogManager.h>

#include <assimp/mesh.h>

SHIB_REFLECTION_DEFINE(MeshResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(MeshResource)
	.classAttrs(CreatableAttribute())

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(MeshResource)

bool MeshResource::createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiMesh& mesh)
{
	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	_meshes.reserve(devices.size());

	Gleam::IBuffer::BufferSettings settings{ nullptr, 0, 0, Gleam::IBuffer::BT_VERTEX_DATA, Gleam::IBuffer::MT_NONE, true, 0 };

	// Options for packing here?

	constexpr size_t PosSize = sizeof(float) * 3;
	constexpr size_t NrmSize = sizeof(float) * 3;
	constexpr size_t TanSize = sizeof(float) * 3;
	constexpr size_t UVSize = sizeof(float);
	constexpr size_t ClrSize = sizeof(float) * 4;

	// Calculate total buffer size.
	if (mesh.HasPositions()) {
		settings.size += PosSize * mesh.mNumVertices;
	}

	if (mesh.HasNormals()) {
		settings.size += NrmSize * mesh.mNumVertices;
	}

	if (mesh.HasTangentsAndBitangents()) {
		settings.size += TanSize * 2 * mesh.mNumVertices;
	}

	for (int32_t j = 0; j < static_cast<int32_t>(mesh.GetNumUVChannels()); ++j) {
		settings.size += UVSize * mesh.mNumUVComponents[j] * mesh.mNumVertices;
	}

	for (int32_t j = 0; j < static_cast<int32_t>(mesh.GetNumColorChannels()); ++j) {
		settings.size += ClrSize * mesh.mNumVertices;
	}

	settings.stride = static_cast<int32_t>(settings.size / mesh.mNumVertices);

	void* const data = SHIB_ALLOC_POOL(settings.size, GetAllocator().getPoolIndex("Graphics"), GetAllocator());
	int8_t* curr = reinterpret_cast<int8_t*>(data);

	settings.data = data;

	for (int32_t j = 0; j < static_cast<int32_t>(mesh.mNumVertices); ++j) {
		if (mesh.HasPositions()) {
			memcpy(curr, &mesh.mVertices[j], PosSize);
			curr += PosSize;
		}

		if (mesh.HasNormals()) {
			memcpy(curr, &mesh.mNormals[j], NrmSize);
			curr += NrmSize;
		}

		if (mesh.HasTangentsAndBitangents()) {
			memcpy(curr, &mesh.mTangents[j], TanSize);
			curr += TanSize;

			memcpy(curr, &mesh.mBitangents[j], TanSize);
			curr += TanSize;
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumUVChannels()); ++k) {
			memcpy(curr, &mesh.mTextureCoords[k], UVSize * mesh.mNumUVComponents[k]);
			curr += UVSize * mesh.mNumUVComponents[k];
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumColorChannels()); ++k) {
			memcpy(curr, &mesh.mColors[k], ClrSize);
			curr += ClrSize;
		}
	}

	bool succeeded = true;

	const U8String res_name = getFilePath().getString() + ":mesh_buffer";
	_buffers = res_mgr.createResourceT<BufferResource>(res_name.data());

	if (!_buffers->createBuffer(devices, settings)) {
		LogErrorResource("Failed to load mesh '%s'.", getFilePath().getBuffer());
		succeeded = false;
	}

	for (int32_t j = 0; j < static_cast<int32_t>(devices.size()); ++j) {
		Gleam::IRenderDevice* const rd = devices[j];
		Gleam::IBuffer* const buffer = _buffers->getBuffer(*rd);

		if (!buffer) {
			continue;
		}

		Gleam::IMesh* const gpu_mesh = render_mgr.createMesh();
		_meshes[rd].reset(gpu_mesh);

		gpu_mesh->setTopologyType(Gleam::IMesh::TRIANGLE_LIST);

		// Add the buffer with all the offsets to the mesh.
		uint32_t offset = 0;

		if (mesh.HasPositions()) {
			gpu_mesh->addBuffer(buffer, offset);
			offset += static_cast<uint32_t>(PosSize);
		}

		if (mesh.HasNormals()) {
			gpu_mesh->addBuffer(buffer, offset);
			offset += static_cast<uint32_t>(NrmSize);
		}

		if (mesh.HasTangentsAndBitangents()) {
			gpu_mesh->addBuffer(buffer, offset);
			offset += static_cast<uint32_t>(TanSize);

			gpu_mesh->addBuffer(buffer, offset);
			offset += static_cast<uint32_t>(TanSize);
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumUVChannels()); ++k) {
			gpu_mesh->addBuffer(buffer, offset);
			offset += static_cast<uint32_t>(UVSize * mesh.mNumUVComponents[k]);
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumColorChannels()); ++k) {
			gpu_mesh->addBuffer(buffer, offset);
			offset += static_cast<uint32_t>(ClrSize);
		}
	}

	SHIB_FREE(data, GetAllocator());
	return succeeded;
}

bool MeshResource::createMesh(Gleam::IRenderDevice& device, const aiMesh& mesh)
{
	Vector<Gleam::IRenderDevice*> devices(1, &device, ProxyAllocator("Graphics"));
	return createMesh(devices, mesh);
}

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

NS_END
