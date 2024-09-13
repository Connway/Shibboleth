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

#include "Model/Shibboleth_ModelComponent.h"
#include "Model/Shibboleth_ModelPipelineData.h"
#include "Shibboleth_RenderManager.h"
#include <Ptrs/Shibboleth_ManagerRef.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ModelComponent)
	.template base<Shibboleth::EntitySceneComponent>()
	.template ctor<>()

	.var("data", &Type::_model_instance_data)
SHIB_REFLECTION_DEFINE_END(Shibboleth::ModelComponent)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ModelComponent)

bool ModelComponent::init(void)
{
	// Nothing to register.
	if (!_model_instance_data.model) {
		return true;
	}

	ManagerRef<RenderManager> render_mgr;
	ModelPipelineData* const model_data = render_mgr->getRenderPipeline().getOrAddRenderData<ModelPipelineData>();

	_handle = model_data->registerModel(_model_instance_data, *this);

	return true;
}

void ModelComponent::destroy(void)
{
	if (!_model_instance_data.model) {
		return;
	}

	ManagerRef<RenderManager> render_mgr;
	ModelPipelineData* const model_data = render_mgr->getRenderPipeline().getRenderData<ModelPipelineData>();

	GAFF_ASSERT(model_data);

	model_data->unregisterModel(_handle);
	_handle = ModelInstanceHandle{};
}

NS_END
