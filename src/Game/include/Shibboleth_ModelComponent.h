/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>
#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ResourceWrapper.h"
#include "Shibboleth_IComponent.h"

NS_SHIBBOLETH

class ResourceManager;
class LoadingMessage;
class RenderManager;
struct ModelData;

class ModelComponent : public IComponent
{
public:
	ModelComponent(IApp& app);
	~ModelComponent(void);

	bool load(const Gaff::JSON& json);
	bool save(Gaff::JSON& json);

	void allComponentsLoaded(void);

	static const char* getComponentName(void)
	{
		return "Model Component";
	}

	void* rawRequestInterface(unsigned int class_id) const;

	void render(void); // Temporary test function

private:
	AString _material_filename;
	AString _model_filename;
	ResourceWrapper<ProgramData> _material_res;
	ResourceWrapper<ModelData> _model_res;
	ProgramBuffersPtr _program_buffers;

	RenderManager& _render_mgr;
	ResourceManager& _res_mgr;
	IApp& _app;

	unsigned char _flags;

	void LoadCallback(const AHashString& resource, bool success);
	void HandleLoadingMessage(const LoadingMessage& msg);

	template <unsigned int bit>
	bool GetFlag(void) const
	{
		return (_flags & (1 << bit)) != 0;
	}

	void SetReleaseHoldingFlag(bool value);
	void SetLoadOnlyHoldingFlag(bool value);

	REF_DEF_SHIB(ModelComponent);
};

NS_END
