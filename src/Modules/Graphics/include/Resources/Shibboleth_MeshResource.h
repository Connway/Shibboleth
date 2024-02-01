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

#pragma once

#include "Shibboleth_BufferResource.h"
#include <Shibboleth_ResourcePtr.h>
#include <Gleam_AABB.h>

struct aiMesh;

NS_GLEAM
	class Mesh;
NS_END

NS_SHIBBOLETH

class MeshResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	Vector<Gleam::RenderDevice*> getDevices(void) const;

	bool createMesh(const Vector<Gleam::RenderDevice*>& devices, const aiMesh& mesh);
	bool createMesh(Gleam::RenderDevice& device, const aiMesh& mesh);

	const Gleam::Mesh* getMesh(const Gleam::RenderDevice& rd) const;
	Gleam::Mesh* getMesh(const Gleam::RenderDevice& rd);

	const Gleam::AABB& getAABB(void) const;

private:
	VectorMap< const Gleam::RenderDevice*, UniquePtr<Gleam::Mesh> > _meshes{ ProxyAllocator("Graphics") };
	ResourcePtr<BufferResource> _vertex_data;
	ResourcePtr<BufferResource> _indice_data;

	Gleam::AABB _aabb;

	void loadMesh(IFile* file);

	SHIB_REFLECTION_CLASS_DECLARE(MeshResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::MeshResource)
