/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_ViewportWindow.h"
#include <Shibboleth_EditorWindowAttribute.h>
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IManager.h>

SHIB_REFLECTION_DEFINE(ViewportWindow)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ViewportWindow)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("&Viewport", "Viewport")
	)
SHIB_REFLECTION_CLASS_DEFINE_END(ViewportWindow)

ViewportWindow::ViewportWindow(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size)
{
#ifdef PLATFORM_WINDOWS
	_window.init(GetHWND());
#endif

	IRenderManager& rm = GetApp().GETMANAGERT(RenderManager);
	Gleam::IRenderDevice* const rd = rm.getRenderDevice("main");

	_output = rm.createRenderOutput();
	_output->init(*rd, _window);

	Bind(wxEVT_SIZE, &ViewportWindow::onResize, this, GetId());
}

ViewportWindow::~ViewportWindow(void)
{
	SHIB_FREET(_output, GetAllocator());
}

void ViewportWindow::onResize(wxSizeEvent& event)
{
	const wxSize size = event.GetSize();
	_window.setDimensions(size.GetWidth(), size.GetHeight());
}

NS_END
