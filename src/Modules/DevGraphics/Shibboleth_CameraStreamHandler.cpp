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

#include "Shibboleth_CameraStreamHandler.h"
#include "Shibboleth_NVENCHelpers.h"
#include <Shibboleth_RenderManagerBase.h>
#include <Shibboleth_DevWebAttributes.h>
#include <Shibboleth_DevWebUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(CameraStreamHandler)
	.classAttrs(DevWebCommandAttribute("/camera"))

	.BASE(IDevWebHandler)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(CameraStreamHandler)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(CameraStreamHandler)

CameraStreamHandler::CameraStreamHandler(void)
{
}

bool CameraStreamHandler::init(void)
{
	auto& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	const auto* const devices = render_mgr.getDevicesByTag("main");

	if (!devices) {
		// $TODO: Log error.
		return false;
	}

	if (devices->size() <= 0) {
		// $TODO: Log error.
		return false;
	}

	// Unsupported renderer type. Just ignore.
	if (devices->front()->getRendererType() != Gleam::RendererType::Direct3D11) {
		_active = false;
		return true;
	}

	const auto& nvenc_funcs = GetNVENCFuncs();

	NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS params =
	{
		NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER,
		NV_ENC_DEVICE_TYPE_DIRECTX,
		devices->front()->getUnderlyingDevice(), // IDevice
		0,
		NVENCAPI_VERSION,
		{ 0 },
		{ 0 }
	};

	const NVENCSTATUS result = nvenc_funcs.nvEncOpenEncodeSessionEx(&params, &_encoder);
	
	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		return false;
	}

	_active = true;
	return true;
}

bool CameraStreamHandler::handleGet(CivetServer* /*server*/, mg_connection* conn)
{
	GAFF_REF(conn);

	//const mg_request_info* const req = mg_get_request_info(conn);
	//int32_t id = -1;

	//if (req->query_string) {
	//	sscanf(req->query_string, "id=%i", &id);
	//}

	//mg_printf(conn, "HTTP/1.1 200 OK\r\n");
	//mg_printf(conn, "Content-Type: text/html; charset=utf-8\r\n");
	//mg_printf(conn, "Connection: close\r\n\r\n");

	return true;
}

bool CameraStreamHandler::handlePut(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (req->content_length <= 0) {
		return true;
	}

	const Gaff::JSON req_data = ReadJSON(*conn, *req);

	if (req_data.isNull()) {
		// $TODO: Log error.
		return true;
	}

	return true;
}

NS_END
