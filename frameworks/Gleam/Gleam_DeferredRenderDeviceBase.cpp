/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gleam_DeferredRenderDeviceBase.h"

NS_GLEAM

DeferredRenderDeviceBase::DeferredRenderDeviceBase(void)
{
}

DeferredRenderDeviceBase::~DeferredRenderDeviceBase(void)
{
}

IRenderDevice::AdapterList DeferredRenderDeviceBase::getDisplayModes(int)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return IRenderDevice::AdapterList();
}

bool DeferredRenderDeviceBase::initThreadData(unsigned int*, size_t)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return false;
}

bool DeferredRenderDeviceBase::init(const IWindow&, unsigned int, unsigned int, unsigned int, bool)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return false;
}

bool DeferredRenderDeviceBase::isVsync(unsigned int, unsigned int) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return false;
}

void DeferredRenderDeviceBase::setVsync(bool, unsigned int, unsigned int)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
}

void DeferredRenderDeviceBase::setClearColor(float, float, float, float)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
}

void DeferredRenderDeviceBase::beginFrame(void)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
}

void DeferredRenderDeviceBase::endFrame(void)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
}

bool DeferredRenderDeviceBase::resize(const IWindow&)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return false;
}

bool DeferredRenderDeviceBase::handleFocusGained(const IWindow&)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return false;
}

void DeferredRenderDeviceBase::resetRenderState(void)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
}

unsigned int DeferredRenderDeviceBase::getViewportWidth(unsigned int, unsigned int) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

unsigned int DeferredRenderDeviceBase::getViewportHeight(unsigned int, unsigned int) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

unsigned int DeferredRenderDeviceBase::getActiveViewportWidth(void)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

unsigned int DeferredRenderDeviceBase::getActiveViewportHeight(void)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

unsigned int DeferredRenderDeviceBase::getNumOutputs(unsigned int) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

unsigned int DeferredRenderDeviceBase::getNumDevices(void) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

IRenderTargetPtr DeferredRenderDeviceBase::getOutputRenderTarget(unsigned int, unsigned int)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return IRenderTargetPtr();
}

IRenderTargetPtr DeferredRenderDeviceBase::getActiveOutputRenderTarget(void)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return IRenderTargetPtr();
}

bool DeferredRenderDeviceBase::setCurrentOutput(unsigned int)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return false;
}

unsigned int DeferredRenderDeviceBase::getCurrentOutput(void) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

bool DeferredRenderDeviceBase::setCurrentDevice(unsigned int)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return false;
}

unsigned int DeferredRenderDeviceBase::getCurrentDevice(void) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

unsigned int DeferredRenderDeviceBase::getDeviceForAdapter(unsigned int) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

unsigned int DeferredRenderDeviceBase::getDeviceForMonitor(unsigned int) const
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return UINT_FAIL;
}

IRenderDevice* DeferredRenderDeviceBase::createDeferredRenderDevice(void)
{
	GAFF_ASSERT_MSG(false, "Calling an immediate render device function on a deferred render device");
	return nullptr;
}

NS_END
