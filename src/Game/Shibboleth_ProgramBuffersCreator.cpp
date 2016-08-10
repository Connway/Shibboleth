/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_ProgramBuffersCreator.h"
#include "Shibboleth_ResourceDefines.h"
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>

NS_SHIBBOLETH

ProgramBuffersCreator::ProgramBuffersCreator(void):
	_render_mgr(GetApp().getManagerT<IRenderManager>())
{
}

ProgramBuffersCreator::~ProgramBuffersCreator(void)
{
}

ResourceLoadData ProgramBuffersCreator::load(const IFile*, ResourceContainer*)
{
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();

	ProgramBuffersData* data = SHIB_ALLOCT(ProgramBuffersData, *GetAllocator());

	if (!data) {
		// log error
		return { nullptr };
	}

	data->data.reserve(rd.getNumDevices());

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		ProgramBuffersPtr program_buffers(_render_mgr.createProgramBuffers());

		if (!program_buffers) {
			// log error
			SHIB_FREET(data, *GetAllocator());
			return { nullptr };
		}

		data->data.push(program_buffers);
	}

	return { data };
}

NS_END
