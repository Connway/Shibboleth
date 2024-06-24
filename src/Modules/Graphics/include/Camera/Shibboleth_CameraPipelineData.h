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

#include "Pipelines/Shibboleth_IRenderPipelineData.h"
#include "Camera/Shibboleth_CameraView.h"
#include "Shibboleth_GraphicsDefines.h"
#include <Containers/Shibboleth_SparseStack.h>

NS_SHIBBOLETH

class CameraPipelineData final : public IRenderPipelineData
{
public:
	const CameraView& getView(int32_t id) const;
	CameraView& getView(int32_t id);

	int32_t createView(void);
	void removeView(int32_t id);

private:
	SparseStack<CameraView> _views{ GRAPHICS_ALLOCATOR };

	// $TODO: Move G-Buffers into here.

	SHIB_REFLECTION_CLASS_DECLARE(CameraPipelineData);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::CameraPipelineData)
