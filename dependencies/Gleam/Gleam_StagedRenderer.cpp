/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_StagedRenderer.h"
#include "Gleam_IRenderStage.h"
#include "Gleam_Scene.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

StagedRenderer::StagedRenderer(void)
{

}

StagedRenderer::~StagedRenderer(void)
{
	destroy();
}

void StagedRenderer::destroy(void)
{
	for (unsigned int i = 0; i < _stages.size(); ++i) {
		_stages[i]->release();
	}

	_stages.clear();
}

void StagedRenderer::render(IRenderDevice& rd, Scene& scene)
{
	assert(scene.getCamera());

	for (unsigned int i = 0; i < _stages.size(); ++i) {
		_stages[i]->render(rd, scene);
	}
}

void StagedRenderer::addStage(IRenderStage* stage)
{
	_stages.push(stage);
	stage->addRef();
}

bool StagedRenderer::removeStage(const IRenderStage* stage)
{
	assert(stage);
	int index = _stages.linearFind((IRenderStage* const)stage);

	if (index > -1) {
		stage->release();
		_stages.erase(index);
		return true;
	}

	return false;
}

void StagedRenderer::removeStage(unsigned int index)
{
	assert(index < _stages.size());
	_stages[index]->release();
	_stages.erase(index);
}

NS_END
