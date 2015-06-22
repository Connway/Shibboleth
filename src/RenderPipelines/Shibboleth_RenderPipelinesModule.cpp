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

#include "Shibboleth_InGameRenderPipeline.h"
#include <Shibboleth_RenderPipelineManager.h>
#include <Shibboleth_IRenderPipeline.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>

template <class T>
Shibboleth::IRenderPipeline* CreateRenderPipelineT(void)
{
	return Shibboleth::GetAllocator()->allocT<T>();
}

enum RenderPipelines
{
	IN_GAME_RP = 0,
	NUM_RPS
};

using RPFunc = Shibboleth::IRenderPipeline* (*)(void);

RPFunc rp_funcs[] = {
	&CreateRenderPipelineT<Shibboleth::InGameRenderPipeline>
};

DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Gaff::JSON::SetMemoryFunctions(&Shibboleth::ShibbolethAllocate, &Shibboleth::ShibbolethFree);
	Gaff::JSON::SetHashSeed(app.getSeed());

	Shibboleth::SetApp(app);

	return true;
}

DYNAMICEXPORT_C void ShutdownModule(void)
{
}

DYNAMICEXPORT_C size_t GetNumRenderPipelines(void)
{
	return NUM_RPS;
}

DYNAMICEXPORT_C Shibboleth::IRenderPipeline* CreateRenderPipeline(size_t id)
{
	assert(id < NUM_RPS);
	return rp_funcs[id]();
}

DYNAMICEXPORT_C void DestroyRenderPipeline(Shibboleth::IRenderPipeline* render_pipeline)
{
	Shibboleth::GetAllocator()->freeT(render_pipeline);
}
