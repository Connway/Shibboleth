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

#pragma once

#include "Shibboleth_BufferResource.h"
#include <Shibboleth_SmartPtrs.h>
#include <Gleam_IMesh.h>

struct aiMesh;

NS_SHIBBOLETH

class MeshResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	bool createMesh(const Vector<Gleam::IRenderDevice*>& devices, const aiMesh& mesh);
	bool createMesh(Gleam::IRenderDevice& device, const aiMesh& mesh);

	const Gleam::IMesh* getMesh(const Gleam::IRenderDevice& rd) const;
	Gleam::IMesh* getMesh(const Gleam::IRenderDevice& rd);

private:
	VectorMap< const Gleam::IRenderDevice*, UniquePtr<Gleam::IMesh> > _meshes{ ProxyAllocator("Graphics") };
	BufferResourcePtr _vertex_data;
	BufferResourcePtr _indice_data;

	void loadMesh(IFile* file);

	SHIB_REFLECTION_CLASS_DECLARE(MeshResource);
};

using MeshResourcePtr = Gaff::RefPtr<MeshResource>;

NS_END

SHIB_REFLECTION_DECLARE(MeshResource)
