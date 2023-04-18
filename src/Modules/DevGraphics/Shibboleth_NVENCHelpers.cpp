/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_NVENCHelpers.h"
#include <Shibboleth_DynamicLoader.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

static NV_ENCODE_API_FUNCTION_LIST g_nvenc_funcs;

const NV_ENCODE_API_FUNCTION_LIST& GetNVENCFuncs(void)
{
	return g_nvenc_funcs;
}

bool InitNVENC(void)
{
	DynamicLoader::ModulePtr nvenc_module = GetApp().getDynamicLoader().loadModule(u8"nvEncodeAPI" DYNAMIC_MODULE_EXTENSION_U8, u8"nvenc");

	if (!nvenc_module) {
		nvenc_module = GetApp().getDynamicLoader().loadModule(u8"nvEncodeAPI64" DYNAMIC_MODULE_EXTENSION_U8, u8"nvenc");

		if (!nvenc_module) {
			return false;
		}
	}

	using NvEncodeAPIGetMaxSupportedVersionFunc = NVENCSTATUS (NVENCAPI*)(uint32_t*);
	const NvEncodeAPIGetMaxSupportedVersionFunc get_max_supported_version = nvenc_module->getFunc<NvEncodeAPIGetMaxSupportedVersionFunc>("NvEncodeAPIGetMaxSupportedVersion");

	if (!get_max_supported_version) {
		// $TODO: Log error.
		return false;
	}

	uint32_t max_supported_version = 0;

	NVENCSTATUS status = get_max_supported_version(&max_supported_version);

	if (status != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		return false;
	}

	// $TODO: Fix this.
	//if (max_supported_version < NVENCAPI_VERSION) {
	//	// $TODO: Log error.
	//	return false;
	//}

	using NvEncodeAPICreateInstanceFunc = NVENCSTATUS (NVENCAPI*)(NV_ENCODE_API_FUNCTION_LIST*);
	const NvEncodeAPICreateInstanceFunc load_api = nvenc_module->getFunc<NvEncodeAPICreateInstanceFunc>("NvEncodeAPICreateInstance");

	if (!load_api) {
		// $TODO: Log error.
		return false;
	}

	memset(&g_nvenc_funcs, 0, sizeof(g_nvenc_funcs));
	g_nvenc_funcs.version = NV_ENCODE_API_FUNCTION_LIST_VER;

	status = load_api(&g_nvenc_funcs);

	if (status != NV_ENC_SUCCESS) {
		// $TODO: Log error.
		return false;
	}

	return true;
}

NS_END
