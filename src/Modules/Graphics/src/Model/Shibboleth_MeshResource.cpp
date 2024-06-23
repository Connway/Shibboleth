/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Resources/Shibboleth_MeshResource.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Gleam_Mesh.h>
#include <assimp/mesh.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::MeshResource)
	.classAttrs(Shibboleth::CreatableAttribute())

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::MeshResource)

namespace
{
	static Shibboleth::ProxyAllocator g_allocator("Graphics");
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(MeshResource)

Vector<Gleam::RenderDevice*> MeshResource::getDevices(void) const
{
	Vector<Gleam::RenderDevice*> out{ GRAPHICS_ALLOCATOR };

	for (const auto& pair : _meshes) {
		out.emplace_back(const_cast<Gleam::RenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool MeshResource::createMesh(const Vector<Gleam::RenderDevice*>& devices, const aiMesh& mesh)
{
	if (!mesh.HasFaces()) {
		LogErrorResource("Failed to load mesh '%s'. Mesh does not have an index list.", getFilePath().getBuffer());
		return false;
	}

	ResourceManager& res_mgr = GetManagerTFast<ResourceManager>();

	_meshes.reserve(devices.size());

	Gleam::IBuffer::Settings settings{ nullptr, 0, 0, 0, Gleam::IBuffer::Type::VertexData, Gleam::IBuffer::MapType::None, true };

	// Options for packing here?

	constexpr size_t k_pos_size = sizeof(float) * 3;
	constexpr size_t k_norm_size = sizeof(float) * 3;
	constexpr size_t k_tan_size = sizeof(float) * 3;
	constexpr size_t k_uv_size = sizeof(float);
	constexpr size_t k_color_size = sizeof(float) * 4;

	_aabb.setMax(Gleam::Vec3(mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z));
	_aabb.setMin(Gleam::Vec3(mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z));

	// Calculate total buffer size.
	if (mesh.HasPositions()) {
		settings.size += k_pos_size * mesh.mNumVertices;
		settings.element_size += k_pos_size;
	}

	if (mesh.HasNormals()) {
		settings.size += k_norm_size * mesh.mNumVertices;
		settings.element_size += k_norm_size;
	}

	if (mesh.HasTangentsAndBitangents()) {
		settings.size += k_tan_size * 2 * mesh.mNumVertices;
		settings.element_size += k_tan_size;
	}

	for (int32_t j = 0; j < static_cast<int32_t>(mesh.GetNumUVChannels()); ++j) {
		settings.size += k_uv_size * mesh.mNumUVComponents[j] * mesh.mNumVertices;
		settings.element_size += k_uv_size * mesh.mNumUVComponents[j];
	}

	for (int32_t j = 0; j < static_cast<int32_t>(mesh.GetNumColorChannels()); ++j) {
		settings.size += k_color_size * mesh.mNumVertices;
		settings.element_size += k_color_size;
	}

	settings.stride = static_cast<int32_t>(settings.size / mesh.mNumVertices);

	void* data = SHIB_ALLOC_POOL(settings.size, GetAllocator().getPoolIndex("Graphics"), GetAllocator());
	int8_t* curr = reinterpret_cast<int8_t*>(data);

	settings.data = data;

	for (int32_t j = 0; j < static_cast<int32_t>(mesh.mNumVertices); ++j) {
		if (mesh.HasPositions()) {
			memcpy(curr, &mesh.mVertices[j], k_pos_size);
			curr += k_pos_size;
		}

		if (mesh.HasNormals()) {
			const auto normal = mesh.mNormals[j].NormalizeSafe();
			memcpy(curr, &normal, k_norm_size);
			curr += k_norm_size;
		}

		if (mesh.HasTangentsAndBitangents()) {
			const auto tan = mesh.mTangents[j].NormalizeSafe();
			const auto bitan = mesh.mBitangents[j].NormalizeSafe();

			memcpy(curr, &tan, k_tan_size);
			curr += k_tan_size;

			memcpy(curr, &bitan, k_tan_size);
			curr += k_tan_size;
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumUVChannels()); ++k) {
			memcpy(curr, &mesh.mTextureCoords[k][j], k_uv_size * mesh.mNumUVComponents[k]);
			curr += k_uv_size * mesh.mNumUVComponents[k];
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumColorChannels()); ++k) {
			memcpy(curr, &mesh.mColors[k], k_color_size);
			curr += k_color_size;
		}
	}

	bool succeeded = true;

	U8String res_name = getFilePath().getString() + u8":vertex_buffer";
	_vertex_data = res_mgr.createResourceT<BufferResource>(res_name.data());

	if (!_vertex_data->createBuffer(devices, settings)) {
		LogErrorResource("Failed to load mesh '%s'. Failed to create vertex buffer.", getFilePath().getBuffer());
		succeeded = false;
	}

	SHIB_FREE(data, GetAllocator());

	// Create indice data.
	settings = Gleam::IBuffer::Settings{ nullptr, static_cast<size_t>(mesh.mNumFaces) * 3 * sizeof(uint32_t), 0, sizeof(uint32_t), Gleam::IBuffer::Type::IndexData, Gleam::IBuffer::MapType::None, true };
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

	res_name = getFilePath().getString() + u8":indice_buffer";
	_indice_data = res_mgr.createResourceT<BufferResource>(res_name.data());

	if (!_indice_data->createBuffer(devices, settings)) {
		LogErrorResource("Failed to load mesh '%s'. Failed to create indice buffer.", getFilePath().getBuffer());
		succeeded = false;
	}

	SHIB_FREE(data, GetAllocator());

	for (int32_t j = 0; j < static_cast<int32_t>(devices.size()); ++j) {
		Gleam::RenderDevice* const rd = devices[j];
		Gleam::Buffer* const vertex_buffer = _vertex_data->getBuffer(*rd);
		Gleam::Buffer* const indice_buffer = _indice_data->getBuffer(*rd);

		if (!vertex_buffer || !indice_buffer) {
			continue;
		}

		Gleam::Mesh* const gpu_mesh = SHIB_ALLOCT(Gleam::Mesh, g_allocator);
		_meshes[rd].reset(gpu_mesh);

		gpu_mesh->setTopology(Gleam::IMesh::Topology::TriangleList);
		gpu_mesh->setIndiceBuffer(indice_buffer);
		gpu_mesh->setIndexCount(static_cast<int32_t>(mesh.mNumFaces * 3));

		// Add the buffer with all the offsets to the mesh.
		uint32_t offset = 0;

		if (mesh.HasPositions()) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(k_pos_size);
		}

		if (mesh.HasNormals()) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(k_norm_size);
		}

		if (mesh.HasTangentsAndBitangents()) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(k_tan_size);

			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(k_tan_size);
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumUVChannels()); ++k) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(k_uv_size * mesh.mNumUVComponents[k]);
		}

		for (int32_t k = 0; k < static_cast<int32_t>(mesh.GetNumColorChannels()); ++k) {
			gpu_mesh->addBuffer(vertex_buffer, offset);
			offset += static_cast<uint32_t>(k_color_size);
		}
	}

	if (!succeeded) {
		return false;
	}

	return true;
}

bool MeshResource::createMesh(Gleam::RenderDevice& device, const aiMesh& mesh)
{
	const Vector<Gleam::RenderDevice*> devices(1, &device, GRAPHICS_ALLOCATOR);
	return createMesh(devices, mesh);
}

const Gleam::Mesh* MeshResource::getMesh(const Gleam::RenderDevice& rd) const
{
	const auto it = _meshes.find(&rd);
	return (it != _meshes.end()) ? it->second.get() : nullptr;
}

Gleam::Mesh* MeshResource::getMesh(const Gleam::RenderDevice& rd)
{
	const auto it = _meshes.find(&rd);
	return (it != _meshes.end()) ? it->second.get() : nullptr;
}

const Gleam::AABB& MeshResource::getAABB(void) const
{
	return _aabb;
}

NS_END
