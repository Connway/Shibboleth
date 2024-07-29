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

#include "Shibboleth_GraphicsConfig.h"
#include <Attributes/Shibboleth_EngineAttributesCommon.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::GraphicsConfigWindow)
	.var("monitor_id", &Shibboleth::GraphicsConfigWindow::monitor_id)

	.var("width", &Shibboleth::GraphicsConfigWindow::width)
	.var("height", &Shibboleth::GraphicsConfigWindow::height)
	.var("refresh_rate", &Shibboleth::GraphicsConfigWindow::refresh_rate)
	.var("vsync", &Shibboleth::GraphicsConfigWindow::vsync)

	.var("pos_x", &Shibboleth::GraphicsConfigWindow::pos_x)
	.var("pos_y", &Shibboleth::GraphicsConfigWindow::pos_y)
	.var("windowed", &Shibboleth::GraphicsConfigWindow::windowed)

	.var("tags", &Shibboleth::GraphicsConfigWindow::tags)
SHIB_REFLECTION_DEFINE_END(Shibboleth::GraphicsConfigWindow)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::GraphicsConfig)
	.classAttrs(
		Shibboleth::ConfigFileAttribute(u8"graphics/graphics"),
		Shibboleth::GlobalConfigAttribute()
	)

	.var("texture_filtering_sampler", &Shibboleth::GraphicsConfig::texture_filtering_sampler)
	.var("icon", &Shibboleth::GraphicsConfig::icon, Shibboleth::OptionalAttribute())

	.var("windows", &Shibboleth::GraphicsConfig::windows, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Shibboleth::GraphicsConfig)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(GraphicsConfig)

NS_END

