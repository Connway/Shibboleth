/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include <Shibboleth_AppUtils.h>
#include <Shibboleth_Memory.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::DevWebServerManager)
	.base<Shibboleth::IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::DevWebServerManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DevWebServerManager)


DevWebServerManager::DevWebServerManager(void)
{
}

DevWebServerManager::~DevWebServerManager(void)
{
	for (auto& handler : _handlers) {
		handler->destroy();
	}
}

bool DevWebServerManager::init(void)
{
	const char* options[] =
	{
		"listening_ports", "8888", 0
	};

	ProxyAllocator allocator("DevWeb");
	CivetCallbacks callbacks;

	_server.reset(SHIB_ALLOCT(CivetServer, allocator, options, &callbacks, this));

	return true;
}

bool DevWebServerManager::initAllModulesLoaded(void)
{
	const auto ref_defs = GetApp().getReflectionManager().getReflectionWithAttribute<DevWebCommandAttribute>();
	ProxyAllocator allocator("DevWeb");

	_server->addHandler("", _default_handler);

	for (const Refl::IReflectionDefinition* ref_def : ref_defs) {
		if (!ref_def->hasInterface(CLASS_HASH(Shibboleth::IDevWebHandler))) {
			// $TODO: Log error.
			continue;
		}

		IDevWebHandler* const handler = ref_def->createT<IDevWebHandler>(CLASS_HASH(Shibboleth::IDevWebHandler), allocator);

		if (!handler) {
			// $TODO: Log error.
			continue;
		}

		if (!handler->init()) {
			// $TODO: Log error.

			SHIB_FREET(handler, allocator);
			continue;
		}

		const DevWebCommandAttribute* const attr = ref_def->getClassAttr<DevWebCommandAttribute>();

		_server->addHandler(reinterpret_cast<const char*>(attr->getURI().data()), handler);
		_handlers.emplace_back(handler);
	}

	return true;
}

void DevWebServerManager::update(void)
{
	for (auto& handler : _handlers) {
		handler->update();
	}
}

void DevWebServerManager::ConnectionClosed(const mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetManagerTFast<DevWebServerManager>();

	for (const auto& handlers : web_mgr._handlers) {
		handlers->handleConnectionClosed(conn);
	}
}

NS_END
