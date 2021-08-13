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
	// Should do this per-video we are encoding.
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

	NVENCSTATUS result = nvenc_funcs.nvEncOpenEncodeSessionEx(&params, &_encoder);
	
	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	uint32_t encode_guid_count = 0;
	result = nvenc_funcs.nvEncGetEncodeGUIDCount(_encoder, &encode_guid_count);

	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	Vector<GUID> guids(encode_guid_count, GUID());
	result = nvenc_funcs.nvEncGetEncodeGUIDs(_encoder, guids.data(), encode_guid_count, &encode_guid_count);

	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	if (encode_guid_count == 0) {
		// $TODO: Log error.
		return false;
	}

	const GUID encode_guid = (Gaff::Find(guids, NV_ENC_CODEC_HEVC_GUID) != guids.end()) ?
		NV_ENC_CODEC_HEVC_GUID :
		NV_ENC_CODEC_H264_GUID;

	uint32_t preset_guid_count = 0;
	result = nvenc_funcs.nvEncGetEncodePresetCount(_encoder, encode_guid, &preset_guid_count);

	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	Vector<GUID> preset_guids(preset_guid_count, GUID());
	result = nvenc_funcs.nvEncGetEncodePresetGUIDs(_encoder, encode_guid, preset_guids.data(), preset_guid_count, &preset_guid_count);

	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	if (preset_guid_count == 0) {
		// $TODO: Log error.
		return false;
	}

	const GUID preset_guid_priority[] =
	{
		NV_ENC_PRESET_P7_GUID,
		NV_ENC_PRESET_P6_GUID,
		NV_ENC_PRESET_P5_GUID,
		NV_ENC_PRESET_P4_GUID,
		NV_ENC_PRESET_P3_GUID,
		NV_ENC_PRESET_P2_GUID,
		NV_ENC_PRESET_P1_GUID,
	};


	GUID preset_guid = NV_ENC_PRESET_P7_GUID;
	NV_ENC_PRESET_CONFIG preset_config;
	bool found = false;

	for (const GUID& preset_guid_entry : preset_guid_priority) {
		if (Gaff::Find(preset_guids, preset_guid_entry) == preset_guids.end()) {
			continue;
		}

		NV_ENC_PRESET_CONFIG temp_preset_config =
		{
			NV_ENC_PRESET_CONFIG_VER,
			{
				NV_ENC_CONFIG_VER
			},
			{ 0 },
			{ 0 }
		};

		result = nvenc_funcs.nvEncGetEncodePresetConfigEx(_encoder, encode_guid, preset_guid_entry, NV_ENC_TUNING_INFO_HIGH_QUALITY, &temp_preset_config);

		if (result != NV_ENC_SUCCESS) {
			// $TODO: Log error.
			return false;
		}

		preset_config = temp_preset_config;
		preset_guid = preset_guid_entry;
		found = true;
		break;
	}

	if (!found) {
		// $TODO: Log error.
		return false;
	}

	uint32_t supported_input_formats_count = 0;
	result = nvenc_funcs.nvEncGetInputFormatCount(_encoder, encode_guid, &supported_input_formats_count);

	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	Vector<NV_ENC_BUFFER_FORMAT> supported_input_formats(supported_input_formats_count, NV_ENC_BUFFER_FORMAT_UNDEFINED);
	result = nvenc_funcs.nvEncGetInputFormats(_encoder, encode_guid, supported_input_formats.data(), supported_input_formats_count, &supported_input_formats_count);

	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	if (Gaff::Find(supported_input_formats, NV_ENC_BUFFER_FORMAT_ARGB) == supported_input_formats.end()) {
		// $TODO: Log error.
		return false;
	}

	// Checking capabilities.
	//NV_ENC_CAPS_PARAM caps_params =
	//{
	//	NV_ENC_CAPS_PARAM_VER ,
	//	f,
	//	{ 0 },
	//};

	//result = nvenc_funcs.nvEncGetEncodeCaps(_encoder, encode_guid, &caps_params, &caps_val);

	//if (result != NV_ENC_SUCCESS) {
	//	// $TODO: Log error.
	//	//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
	//	return false;
	//}

	NV_ENC_INITIALIZE_PARAMS create_encode_params =
	{
		NV_ENC_INITIALIZE_PARAMS_VER,
		encode_guid,
		preset_guid,
		800, // encode width (CHANGE ME)
		600, // encode height (CHANGE ME)
		800, // display aspect ratio width
		600, // display aspect ratio height
		60, // framerate numerator
		1, // framerate denominator
		1, // async
		0, // PTD
		0, // slice offsets
		0, // subframe write
		0, // external ME hints
		0, // ME only mode
		0, // weighted prediction
		0, // output in video memory
		0,
		0,
		NULL,
		&preset_config.presetCfg,
		0, // encode max width
		0, // encode max height
		{},
		NV_ENC_TUNING_INFO_HIGH_QUALITY,
		{ 0 },
		{ 0 }
	};

	result = nvenc_funcs.nvEncInitializeEncoder(_encoder, &create_encode_params);

	if (result != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		//const char* const error = nvenc_funcs.nvEncGetLastErrorString(_encoder);
		return false;
	}

	_active = true;
	return true;
}

void CameraStreamHandler::destroy(void)
{
	if (_encoder) {
		const auto& nvenc_funcs = GetNVENCFuncs();
		nvenc_funcs.nvEncDestroyEncoder(_encoder);
	}

	_encoder = nullptr;
	_active = false;
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
