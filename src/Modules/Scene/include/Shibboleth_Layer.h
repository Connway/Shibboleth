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
#include <Containers/Shibboleth_InstancedArray.h>
#include <Shibboleth_DeferredResourcePtr.h>

NS_SHIBBOLETH

class Layer final
{
public:
	void init(const DeferredResourcePtr<LayerResource>& layer_resource);
	void init(LayerResource& layer_resource);

	void setName(const HashStringView64<>& name);
	const HashString64<>& getName(void) const;

	bool hasRequestedLoad(void) const;
	bool isDeferred(void) const;

	void unload(void);
	void load(void);

	void start(void);
	void end(void);

private:
	enum class Flag
	{
		RequestedLoad,

		Count
	};

	InstancedArray<Entity> _entities{ SCENE_ALLOCATOR };

	DeferredResourcePtr<LayerResource> _layer_resource;

	HashString64<> _name;

	Gaff::Flags<Flag> _flags;

	SHIB_REFLECTION_ALLOW_PRIVATE_ACCESS(Layer);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::Layer)
