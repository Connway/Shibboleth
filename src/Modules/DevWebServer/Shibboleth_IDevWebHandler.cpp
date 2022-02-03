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

#include "Shibboleth_IDevWebHandler.h"
#include "Shibboleth_DevWebServerManager.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

bool IDevWebHandler::init(void)
{
	return true;
}

void IDevWebHandler::handleConnectionClosed(const mg_connection* /*conn*/)
{
}

bool IDevWebHandler::handleGet(CivetServer* server, mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetApp().getManagerTFast<DevWebServerManager>();
	return web_mgr._default_handler.handleGet(server, conn);
}

bool IDevWebHandler::handlePost(CivetServer* server, mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetApp().getManagerTFast<DevWebServerManager>();
	return web_mgr._default_handler.handlePost(server, conn);
}

bool IDevWebHandler::handleHead(CivetServer* server, mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetApp().getManagerTFast<DevWebServerManager>();
	return web_mgr._default_handler.handleHead(server, conn);
}

bool IDevWebHandler::handlePut(CivetServer* server, mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetApp().getManagerTFast<DevWebServerManager>();
	return web_mgr._default_handler.handlePut(server, conn);
}

bool IDevWebHandler::handleDelete(CivetServer* server, mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetApp().getManagerTFast<DevWebServerManager>();
	return web_mgr._default_handler.handleDelete(server, conn);
}

bool IDevWebHandler::handleOptions(CivetServer* server, mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetApp().getManagerTFast<DevWebServerManager>();
	return web_mgr._default_handler.handleOptions(server, conn);
}

bool IDevWebHandler::handlePatch(CivetServer* server, mg_connection* conn)
{
	DevWebServerManager& web_mgr = GetApp().getManagerTFast<DevWebServerManager>();
	return web_mgr._default_handler.handlePatch(server, conn);
}

NS_END
