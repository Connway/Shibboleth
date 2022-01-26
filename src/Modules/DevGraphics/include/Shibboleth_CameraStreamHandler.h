/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include <Shibboleth_IDevWebHandler.h>
#include <Shibboleth_Reflection.h>

namespace Gleam
{
	class IRenderOutput;
}

NS_SHIBBOLETH

class ECSManager;

class CameraStreamHandler final : public IDevWebHandler, public Refl::IReflectionObject
{
public:
	CameraStreamHandler(void);

	void destroy(void) override;

	bool createEncoder(Gleam::IRenderOutput& output, int32_t& out_id);

	bool handleGet(CivetServer* server, mg_connection* conn) override;
	bool handlePut(CivetServer* server, mg_connection* conn) override;

	SHIB_REFLECTION_CLASS_DECLARE(CameraStreamHandler);

private:
	struct StreamData final
	{
		void* encoder;
		void* input_buffer;
		void* output_buffer;
		void* gleam_input;
	};

	VectorMap<int32_t, StreamData> _stream_data/*{ ProxyAllocator() }*/;
	int32_t _next_id = 0;

	bool createEncoder(uint32_t width, uint32_t height, int32_t& out_id, bool create_input_buffer = false);
	bool registerVulkanResource(Gleam::IRenderOutput& output, int32_t out_id);
	bool registerD3D11Resource(Gleam::IRenderOutput& output, int32_t out_id);
	int32_t getNextID(void);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::CameraStreamHandler)
