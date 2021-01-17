/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#pragma once

#include "Shibboleth_MeshResource.h"

struct aiScene;

NS_SHIBBOLETH

class ModelResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	Vector<Gleam::IRenderDevice*> getDevices(void) const;

	bool createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiScene& scene, const Vector<int32_t>& centering_meshes);
	bool createMesh(Gleam::IRenderDevice& device, const aiScene& scene, const Vector<int32_t>& centering_meshes);
	bool createMesh(const Vector<MeshResourcePtr>& meshes, const Vector<int32_t>& centering_meshes);
	bool createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiScene& scene);
	bool createMesh(Gleam::IRenderDevice& device, const aiScene& scene);
	bool createMesh(const Vector<MeshResourcePtr>& meshes);

	const MeshResourcePtr& getMesh(int32_t index) const;
	int32_t getNumMeshes(void) const;

	const Gleam::Vec3& getCenteringVector(void) const;

private:
	Vector<MeshResourcePtr> _meshes{ ProxyAllocator("Graphics") };
	Gleam::Vec3 _centering_vector = glm::zero<Gleam::Vec3>();

	void loadModel(IFile* file, uintptr_t thread_id_int);

	SHIB_REFLECTION_CLASS_DECLARE(ModelResource);
};

using ModelResourcePtr = Gaff::RefPtr<ModelResource>;

NS_END

SHIB_REFLECTION_DECLARE(ModelResource)
