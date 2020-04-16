/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include "Shibboleth_DevWebAttributes.h"
#include <Shibboleth_Memory.h>

SHIB_REFLECTION_DEFINE_BEGIN(DevWebServerManager)
	.base<IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DevWebServerManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DevWebServerManager)

static void* InitThread(const mg_context*, int)
{
	AllocatorThreadInit();
	return nullptr;
}

DevWebServerManager::DevWebServerManager(void)
{
}

DevWebServerManager::~DevWebServerManager(void)
{
}

bool DevWebServerManager::init(void)
{
	const char* options[] =
	{
		"document_root", "WebRoot", "listening_ports", "8888", 0
	};

	ProxyAllocator allocator("DevWeb");
	CivetCallbacks callbacks;

	callbacks.init_thread = InitThread;

	_server.reset(SHIB_ALLOCT(CivetServer, allocator, options, &callbacks, this));

	return true;
}

bool DevWebServerManager::initAllModulesLoaded(void)
{
	const auto ref_defs = GetApp().getReflectionManager().getReflectionWithAttribute<DevWebCommandAttribute>();
	ProxyAllocator allocator("DevWeb");

	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		if (!ref_def->hasInterface(CLASS_HASH(CivetHandler))) {
			// $TODO: Log error.
			continue;
		}

		CivetHandler* const handler = ref_def->createT<CivetHandler>(CLASS_HASH(CivetHandler), allocator);

		if (!handler) {
			// $TODO: Log error.
			continue;
		}

		const DevWebCommandAttribute* const attr = ref_def->getClassAttr<DevWebCommandAttribute>();

		_server->addHandler(attr->getURI().data(), handler);
		_handlers.emplace_back(handler);
	}

	_server->addHandler("", _default_handler);
	return true;
}

NS_END
