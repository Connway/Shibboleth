/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_DeferredRenderDevice_Direct3D.h"
#include "Gleam_CommandList_Direct3D.h"

NS_GLEAM

DeferredRenderDeviceD3D::DeferredRenderDeviceD3D(void)
{
}

DeferredRenderDeviceD3D::~DeferredRenderDeviceD3D(void)
{
}

void DeferredRenderDeviceD3D::destroy(void)
{
	_context = nullptr;
}

bool DeferredRenderDeviceD3D::isDeferred(void) const
{
	return true;
}

bool DeferredRenderDeviceD3D::isD3D(void) const
{
	return true;
}

const float* DeferredRenderDeviceD3D::getClearColor(void) const
{
	assert(0 && "Calling a immediate render device function on a deferred render device");
	return nullptr;
}

void DeferredRenderDeviceD3D::executeCommandList(ICommandList* command_list)
{
	assert(command_list->isD3D() && _context);
	CommandListD3D* cmd_list = (CommandListD3D*)command_list;
	_context->ExecuteCommandList(cmd_list->getCommandList(), FALSE);
}

bool DeferredRenderDeviceD3D::finishCommandList(ICommandList* command_list)
{
	assert(command_list->isD3D() && _context);
	CommandListD3D* cmd_list = (CommandListD3D*)command_list;

	ID3D11CommandList* cl = nullptr;
	
	if (FAILED(_context->FinishCommandList(FALSE, &cl))) {
		return false;
	}

	cmd_list->setCommandList(cl);
	return true;
}

ID3D11DeviceContext* DeferredRenderDeviceD3D::getDeviceContext(unsigned int)
{
	assert(0 && "Calling a immediate render device function on a deferred render device");
	return nullptr;
}

ID3D11Device* DeferredRenderDeviceD3D::getDevice(unsigned int)
{
	assert(0 && "Calling a immediate render device function on a deferred render device");
	return nullptr;
}

ID3D11DeviceContext* DeferredRenderDeviceD3D::getActiveDeviceContext(void)
{
	return _context.get();
}

ID3D11Device* DeferredRenderDeviceD3D::getActiveDevice(void)
{
	assert(0 && "Calling a immediate render device function on a deferred render device");
	return nullptr;
}

NS_END
