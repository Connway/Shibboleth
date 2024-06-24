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

#include "Shibboleth_IRenderPipelineStage.h"
#include "Shibboleth_IRenderPipelineData.h"
#include "Shibboleth_GraphicsDefines.h"
#include <Containers/Shibboleth_InstancedArray.h>
#include <Shibboleth_Error.h>

NS_SHIBBOLETH

class RenderManager;

class RenderPipeline final
{
public:
	template <class T>
	const T* getRenderStagePtr(void) const
	{
		return const_cast<RenderPipeline>(this)->template getRenderStagePtr<T>();
	}

	template <class T>
	T* getRenderStagePtr(void)
	{
		return static_cast<T*>(getRenderStagePtr(Refl::Reflection<T>::GetReflectionDefinition()));
	}

	template <class T>
	const T& getRenderStage(void) const
	{
		return const_cast<RenderPipeline>(this)->template getRenderStagePtr<T>();
	}

	template <class T>
	T& getRenderStage(void)
	{
		return static_cast<T&>(getRenderStage(Refl::Reflection<T>::GetReflectionDefinition()));
	}

	template <class T>
	Vector<const T*> getRenderStages(void) const
	{
		Vector<const T*> out;

		getRenderStagesInternal<T>(out);
		return out;
	}

	template <class T>
	Vector<T*> getRenderStages(void)
	{
		Vector<T*> out;

		getRenderStagesInternal<T>(out);
		return out;
	}

	template <class T>
	T* getOrAddRenderData(void)
	{
		return static_cast<T*>(getOrAddRenderData(Refl::Reflection<T>::GetReflectionDefinition()));
	}

	template <class T>
	const T* getRenderData(void) const
	{
		return const_cast<RenderPipeline*>(this)->template getRenderData<T>();
	}

	template <class T>
	T* getRenderData(void)
	{
		return static_cast<T*>(getRenderData(Refl::Reflection<T>::GetReflectionDefinition()));
	}

	Error init(RenderManager& render_mgr);

	Vector<const IRenderPipelineStage*> getRenderStages(const Refl::IReflectionDefinition& ref_def) const;
	Vector<IRenderPipelineStage*> getRenderStages(const Refl::IReflectionDefinition& ref_def);

	const IRenderPipelineStage* getRenderStagePtr(const Refl::IReflectionDefinition& ref_def) const;
	IRenderPipelineStage* getRenderStagePtr(const Refl::IReflectionDefinition& ref_def);
	const IRenderPipelineStage& getRenderStage(const Refl::IReflectionDefinition& ref_def) const;
	IRenderPipelineStage& getRenderStage(const Refl::IReflectionDefinition& ref_def);

	IRenderPipelineData* getOrAddRenderData(const Refl::IReflectionDefinition& ref_def);
	const IRenderPipelineData* getRenderData(const Refl::IReflectionDefinition& ref_def) const;
	IRenderPipelineData* getRenderData(const Refl::IReflectionDefinition& ref_def);

	const Vector< UniquePtr<IRenderPipelineData> >& getRenderData(void) const;

private:
	Vector< UniquePtr<IRenderPipelineData> > _render_data{ GRAPHICS_ALLOCATOR };
	InstancedArray<IRenderPipelineStage> _stages{ GRAPHICS_ALLOCATOR };

	template <class T, class U>
	void getRenderStagesInternal(Vector<U*>& out)
	{
		for (IRenderPipelineStage& stage : _stages) {
			const Refl::IReflectionDefinition& ref_def = stage.getReflectionDefinition();

			T* const interface_ptr = ref_def.template getInterface<T>(stage.getBasePointer());

			if (interface_ptr) {
				out.emplace_back(interface_ptr);
			}
		}
	}

	SHIB_REFLECTION_ALLOW_PRIVATE_ACCESS(RenderPipeline);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderPipeline)
