/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_IResourceLoader.h"
#include <Gleam_IShader.h>

NS_GAFF
	class JSON;
NS_END

NS_SHIBBOLETH

class IResourceManager;
class ISchemaManager;
class IRenderManager;
class IFileSystem;
struct ProgramData;
struct ShaderData;

class ShaderProgramLoader : public IResourceLoader
{
public:
	ShaderProgramLoader(IResourceManager& res_mgr, ISchemaManager& schema_mgr, IRenderManager& render_mgr);
	~ShaderProgramLoader(void);

	ResourceLoadData load(const IFile* file, ResourceContainer* res_cont) override;

private:
	IResourceManager& _res_mgr;
	ISchemaManager& _schema_mgr;
	IRenderManager& _render_mgr;

	bool loadShader(ProgramData* program_data, ShaderData* shader_data);
	bool createPrograms(ProgramData* data);
	bool createRasterStates(ProgramData* data, const Gaff::JSON& json);

	friend class ShaderLoadedFunctor;

	GAFF_NO_COPY(ShaderProgramLoader);
	GAFF_NO_MOVE(ShaderProgramLoader);
};

NS_END