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

#include "Components/Shibboleth_ModelComponent.h"
#include "Shibboleth_RenderManager.h"
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_Texture.h>
#include <Gleam_Mesh.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ModelComponent)
	.template base<Shibboleth::EntitySceneComponent>()
	.template ctor<>()

	//.var("data", &Shibboleth::ModelComponent::_model_instance_data)
SHIB_REFLECTION_DEFINE_END(Shibboleth::ModelComponent)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ModelComponent)

bool ModelComponent::init(void)
{
	ManagerRef<RenderManager> render_mgr;

	//const auto model_stages = render_mgr->getRenderPipeline().getRenderStages<IModelStageRegistration>();
	//_handles.reserve(model_stages.size());

	//for (IModelStageRegistration* registrar : model_stages) {
	//	_handles.emplace_back(registrar->registerModel(_model_data, *this));
	//}

	return true;
}

void ModelComponent::destroy(void)
{
	ManagerRef<RenderManager> render_mgr;

	//const auto model_stages = render_mgr->getRenderPipeline().getRenderStages<IModelStageRegistration>();

	//for (int32_t i = 0; i < static_cast<int32_t>(_handles.size()); ++i) {
	//	model_stages[i]->unregisterModel(_handles[i]);
	//}

	//_handles.clear();
}

NS_END
