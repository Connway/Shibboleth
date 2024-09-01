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

#include "Shibboleth_LayerResource.h"
#include <Shibboleth_DeferredResourcePtr.h>

NS_SHIBBOLETH

class SceneResource final : public IResource
{
public:
	int32_t getNumDeferredLayers(void) const;
	int32_t getNumLayers(void) const;

	const LayerResource* getDeferredLayer(const HashStringView64<>& name) const;
	LayerResource* getDeferredLayer(const HashStringView64<>& name);
	const LayerResource* getDeferredLayer(Gaff::Hash64 name) const;
	LayerResource* getDeferredLayer(Gaff::Hash64 name);
	const LayerResource* getDeferredLayer(int32_t index) const;
	LayerResource* getDeferredLayer(int32_t index);


	const LayerResource* getLayer(const HashStringView64<>& name) const;
	LayerResource* getLayer(const HashStringView64<>& name);
	const LayerResource* getLayer(Gaff::Hash64 name) const;
	LayerResource* getLayer(Gaff::Hash64 name);
	const LayerResource* getLayer(int32_t index) const;
	LayerResource* getLayer(int32_t index);

	LayerResource* loadLayer(const HashStringView64<>& name);
	LayerResource* loadLayer(Gaff::Hash64 name);
	void unloadLayer(const HashStringView64<>& name);
	void unloadLayer(Gaff::Hash64 name);

private:
	VectorMap< HashString64<>, DeferredResourcePtr<LayerResource> > _deferred_layers{ SCENE_ALLOCATOR };
	VectorMap< HashString64<>, ResourcePtr<LayerResource> > _layers{ SCENE_ALLOCATOR };

	SHIB_REFLECTION_CLASS_DECLARE(SceneResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::SceneResource)
