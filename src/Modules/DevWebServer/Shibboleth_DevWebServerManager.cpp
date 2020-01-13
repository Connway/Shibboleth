/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_DevWebServerManager.h"
#include <Shibboleth_Memory.h>


SHIB_REFLECTION_DEFINE(DevWebServerManager)

NS_SHIBBOLETH

static void* InitThread(const mg_context*, int)
{
	AllocatorThreadInit();
	return nullptr;
}

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(DevWebServerManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(DevWebServerManager)

DevWebServerManager::DevWebServerManager(void)
{
}

DevWebServerManager::~DevWebServerManager(void)
{
}

bool DevWebServerManager::init(void)
{
	ProxyAllocator allocator("DevWeb");

	const char* options[] =
	{
		"document_root", "WebRoot", "listening_ports", "8888", 0
	};

	CivetCallbacks callbacks;

	callbacks.init_thread = InitThread;

	_server.reset(SHIB_ALLOCT(CivetServer, allocator, options, &callbacks, this));

	// add other handlers.

	_server->addHandler("", _default_handler);

	return true;
}

NS_END
