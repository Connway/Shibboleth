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

#include "Shibboleth_SamplerStateResource.h"
#include "Shibboleth_ImageResource.h"
#include <Config/Shibboleth_Config.h>
#include <Shibboleth_DeferredResourcePtr.h>

NS_SHIBBOLETH

struct GraphicsConfigWindow final
{
	int32_t monitor_id = 0;

	int32_t width = -1;
	int32_t height = -1;
	int32_t refresh_rate = -1;
	bool vsync = false;

	int32_t pos_x = -1;
	int32_t pos_y = -1;

	// $TODO: Replace this with window mode type.
	bool windowed = false;

	Vector< HashString32<> > tags{ ProxyAllocator("Graphics") };
};

class GraphicsConfig final : public IConfig
{
public:
	DeferredResourcePtr<SamplerStateResource> texture_filtering_sampler;
	DeferredResourcePtr<ImageResource> icon;

	VectorMap<HashString32<>, GraphicsConfigWindow> windows{ ProxyAllocator("Graphics") };

	SHIB_REFLECTION_CLASS_DECLARE(GraphicsConfig);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::GraphicsConfigWindow)
SHIB_REFLECTION_DECLARE(Shibboleth::GraphicsConfig)
