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

#include "Shibboleth_DebugDrawWebHandler.h"
#include <Shibboleth_DevWebAttributes.h>
#include <Shibboleth_AppUtils.h>
#include <Gaff_JSON.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::DebugDrawWebHandler)
	.classAttrs(Shibboleth::DevWebCommandAttribute(u8"/debug_draw"))

	.template BASE(Shibboleth::IDevWebHandler)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::DebugDrawWebHandler)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugDrawWebHandler)

static void ReturnError(mg_connection* conn)
{
	mg_printf(conn, "HTTP/1.1 400 Bad Request\r\n");
	mg_printf(conn, "Content-Type: application/json; charset=utf-8\r\n");
	mg_printf(conn, "Connection: close\r\n\r\n");
}

DebugDrawWebHandler::DebugDrawWebHandler(void):
	_debug_mgr(GETMANAGERT(Shibboleth::IDebugManager, Shibboleth::DebugManager))
{
}

//bool DebugDrawWebHandler::handleGet(CivetServer* /*server*/, mg_connection* conn)
//{
//	mg_printf(conn, "HTTP/1.1 200 OK\r\n");
//	mg_printf(conn, "Content-Type: text/html; charset=utf-8\r\n");
//	mg_printf(conn, "Connection: close\r\n\r\n");
//
//	mg_printf(conn, "<html><body>\r\n");
//
//	////if (_ecs.isValid(id)) {
//	////	mg_printf(conn, "<h1>Valid Entity ID: %i\r\n", id);
//	////} else {
//	////	mg_printf(conn, "<h1>Invalid Entity ID: %i\r\n", id);
//	////}
//
//	mg_printf(conn, "</body></html>\r\n");
//
//	return true;
//}

bool DebugDrawWebHandler::handlePut(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);
	IDebugManager::DebugRenderType type = IDebugManager::DebugRenderType::Count;

	if (req->query_string) {
		//sscanf(req->query_string, "id=%i", &id);
	}

	if (type == IDebugManager::DebugRenderType::Count) {
		// $TODO: Log error.
		ReturnError(conn);
		return true;
	}

	auto it = _debug_handles.find(conn);

	if (it == _debug_handles.end()) {
		it = _debug_handles.emplace(conn, HandleVector{ ProxyAllocator("Dev") }).first;
	}

	IDebugManager::DebugRenderHandle handle;

	switch (type) {
		case IDebugManager::DebugRenderType::Line:
			break;
		case IDebugManager::DebugRenderType::Plane:
			break;
		case IDebugManager::DebugRenderType::Sphere:
			break;
		case IDebugManager::DebugRenderType::Box:
			break;
		case IDebugManager::DebugRenderType::Cone:
			break;
		case IDebugManager::DebugRenderType::Capsule:
			break;
		case IDebugManager::DebugRenderType::Arrow:
			break;
		case IDebugManager::DebugRenderType::Cylinder:
			break;
		case IDebugManager::DebugRenderType::Model:
			break;
	}

	if (!handle.isValid()) {
		// $TODO: Log error
		ReturnError(conn);
		return true;
	}

	Gaff::JSON response = Gaff::JSON::CreateObject();
	response.setObject(u8"id", Gaff::JSON::CreateUInt64(handle.getHash().getHash()));

	char8_t buffer[64] = { 0 };
	response.dump(buffer, sizeof(buffer));

	mg_printf(conn, "HTTP/1.1 201 Created\r\n");
	mg_printf(conn, "Content-Type: application/json; charset=utf-8\r\n");
	mg_printf(conn, "Connection: close\r\n\r\n");
	mg_printf(conn, "%s", buffer);

	return true;
}

void DebugDrawWebHandler::handleConnectionClosed(const mg_connection* conn)
{
	_debug_handles.erase(conn);
}

NS_END
