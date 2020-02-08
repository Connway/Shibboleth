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

#include "Shibboleth_MeshResource.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_LogManager.h>

#include <assimp/mesh.h>

SHIB_REFLECTION_DEFINE_BEGIN_NEW(MeshResource)
	.classAttrs(CreatableAttribute())

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END_NEW(MeshResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_NEW(MeshResource)

Vector<Gleam::IRenderDevice*> MeshResource::getDevices(void) const
{
	Vector<Gleam::IRenderDevice*> out{ ProxyAllocator("Graphics") };

	for (const auto& pair : _meshes) {
		out.emplace_back(const_cast<Gleam::IRenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool MeshResource::createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiMesh& mesh)
{
	if (!mesh.HasFaces()) {
		LogErrorResource("Failed to load mesh '%s'. Mesh does not have an index list.", getFilePath().getBuffer());
		return false;
	}

	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	_meshes.reserve(devices.size());

	Gleam::IBuffer::BufferSettings settings{ nullptr, 0, 0, Gleam::IBuffer::BT_VERTEX_DATA, Gleam::IBuffer::MT_NONE, true };

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

	void* data = SHIB_ALLOC_POOL(settings.size, GetAllocator().getPoolIndex("Graphics"), GetAllocator());
	int8_t* curr = reinterpret_cast<int8_t*>(data);

	settings.data = data;

	for (int32_t j = 0; j < static_cast<int32_t>(mesh.mNumVertices); ++j) {
		if (mesh.HasPositions()) {
			memcpy(curr, &mesh.mVertices[j], PosSize);
			curr += PosSize;
		}

		if (mesh.HasNormals()) {
			const auto normal = mesh.mNormals[j].NormalizeSafe();
			memcpy(curr, &normal, NrmSize);
			curr += NrmSize;
		}

		if (mesh.HasTangentsAndBitangents()) {
			const auto tan = mesh.mTangents[j].NormalizeSafe();
			const auto bitan = mesh.mBitangents[j].NormalizeSafe();

			memcpy(curr, &tan, TanSize);
			curr += TanSize;

			memcpy(curr, &bitan, TanSize);
			curr += TanSize;
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumUVChannels()); ++k) {
			memcpy(curr, &mesh.mTextureCoords[k][j], UVSize * mesh.mNumUVComponents[k]);
			curr += UVSize * mesh.mNumUVComponents[k];
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumColorChannels()); ++k) {
			memcpy(curr, &mesh.mColors[k], ClrSize);
			curr += ClrSize;
		}
	}

	bool succeeded = true;

	U8String res_name = getFilePath().getString() + ":vertex_buffer";
	_vertex_data = res_mgr.createResourceT<BufferResource>(res_name.data());

	if (!_vertex_data->createBuffer(devices, settings)) {
		LogErrorResource("Failed to load mesh '%s'. Failed to create vertex buffer.", getFilePath().getBuffer());
		succeeded = false;
	}

	SHIB_FREE(data, GetAllocator());

	// Create indice data.
	settings = Gleam::IBuffer::BufferSettings{ nullptr, static_cast<size_t>(mesh.mNumFaces) * 3 * sizeof(uint32_t), 0, Gleam::IBuffer::BT_INDEX_DATA, Gleam::IBuffer::MT_NONE, true };
	data = SHIB_ALLOC_POOL(settings.size, GetAllocator().getPoolIndex("Graphics"), GetAllocator());
	uint32_t* index = reinterpret_cast<uint32_t*>(data);

	for (int32_t i = 0; i < static_cast<int32_t>(mesh.mNumFaces); ++i) {
		const aiFace& face = mesh.mFaces[i];

		if (face.mNumIndices != 3) {
			LogErrorResource("Failed to load mesh '%s'. Face is %u vertices instead of 3.", getFilePath().getBuffer(), face.mNumIndices);
			continue;
		}

		index[i * 3] = face.mIndices[0];
		index[i * 3 + 1] = face.mIndices[1];
		index[i * 3 + 2] = face.mIndices[2];
	}

	settings.data = data;

	res_name = getFilePath().getString() + ":indice_buffer";
	_indice_data = res_mgr.createResourceT<BufferResource>(res_name.data());

	if (!_indice_data->createBuffer(devices, settings)) {
		LogErrorResource("Failed to load mesh '%s'. Failed to create indice buffer.", getFilePath().getBuffer());
		succeeded = false;
	}

	SHIB_FREE(data, GetAllocator());

	for (int32_t j = 0; j < static_cast<int32_t>(devices.size()); ++j) {
		Gleam::IRenderDevice* const rd = devices[j];
		Gleam::IBuffer* const vertex_buffer = _vertex_data->getBuffer(*rd);
		Gleam::IBuffer* const indice_buffer = _indice_data->getBuffer(*rd);

		if (!vertex_buffer || !indice_buffer) {
			continue;
		}

		Gleam::IMesh* const gpu_mesh = render_mgr.createMesh();
		_meshes[rd].reset(gpu_mesh);

		gpu_mesh->setTopologyType(Gleam::IMesh::TRIANGLE_LIST);
		gpu_mesh->setIndiceBuffer(indice_buffer);
		gpu_mesh->setIndexCount(static_cast<int32_t>(mesh.mNumFaces * 3));

		// Add the buffer with all the offsets to the mesh.
		uint32_t offset = 0;

		if (mesh.HasPositions()) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(PosSize);
		}

		if (mesh.HasNormals()) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(NrmSize);
		}

		if (mesh.HasTangentsAndBitangents()) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(TanSize);

			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(TanSize);
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumUVChannels()); ++k) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(UVSize * mesh.mNumUVComponents[k]);
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumColorChannels()); ++k) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(ClrSize);
		}
	}

	if (!succeeded) {
		return false;
	}

	return true;
}

bool MeshResource::createMesh(Gleam::IRenderDevice& device, const aiMesh& mesh)
{
	const Vector<Gleam::IRenderDevice*> devices(1, &device, ProxyAllocator("Graphics"));
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
