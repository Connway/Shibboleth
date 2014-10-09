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

#include "Shibboleth_ModelComponent.h"
#include "Shibboleth_ModelAnimResources.h"
#include <Shibboleth_ReflectionDefinitions.h>

NS_SHIBBOLETH

COMP_REF_DEF_SAVE(ModelComponent, g_Ref_Def);
REF_IMPL_REQ(ModelComponent);
REF_IMPL_SHIB(ModelComponent);

ModelComponent::ModelComponent(IApp& app):
	_app(app)
{
}

ModelComponent::~ModelComponent(void)
{
}

bool ModelComponent::load(const Gaff::JSON& json)
{
	g_Ref_Def.read(json, this);
	assert(_model_filename.size());

	_model_res = _app.getManagerT<ResourceManager>("Resource Manager").requestResource(_model_filename.getBuffer());
	_model_res.getResourcePtr()->addCallback(Gaff::Bind(this, &ModelComponent::ModelCallback));

	while (!_model_res.getResourcePtr()->isLoaded() && !_model_res.getResourcePtr()->hasFailed()) {
	}

	return true;
}

void ModelComponent::allComponentsLoaded(void)
{
}

void ModelComponent::ModelCallback(const AHashString&, bool success)
{
	if (!success) {
		// complain about something
		return;
	}

	// process loaded data
}

void ModelComponent::InitReflectionDefinition(void)
{
	if (!g_Ref_Def.isDefined()) {
		g_Ref_Def.setAllocator(ProxyAllocator());

		g_Ref_Def.addString("Model File", &ModelComponent::_model_filename);
		g_Ref_Def.addBaseClassInterfaceOnly<ModelComponent>();

		g_Ref_Def.markDefined();
	}
}

NS_END
