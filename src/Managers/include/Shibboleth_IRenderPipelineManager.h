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

#pragma once

#include <Shibboleth_ReflectionDefinitions.h>

NS_SHIBBOLETH

class ICameraComponent;

class IRenderPipelineManager
{
public:
	IRenderPipelineManager(void) {}
	virtual ~IRenderPipelineManager(void) {}

	virtual bool init(const char* initial_pipeline) = 0;

	virtual void setOutputCamera(ICameraComponent* camera) = 0;
	virtual ICameraComponent* getOutputCamera(unsigned int monitor) const = 0;
	virtual void refreshMonitors(void) = 0;

	virtual size_t getActivePipeline(void) const = 0;
	virtual void setActivePipeline(size_t pipeline) = 0;

	virtual size_t getPipelineIndex(const char* name) const = 0;

	SHIB_INTERFACE_REFLECTION(IRenderPipelineManager)
	SHIB_INTERFACE_NAME("Render Pipeline Manager")
};

NS_END
