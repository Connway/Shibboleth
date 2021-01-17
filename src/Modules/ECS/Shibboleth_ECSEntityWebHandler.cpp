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

#include "Shibboleth_ECSEntityWebHandler.h"
#include "Shibboleth_ECSManager.h"
#include <Shibboleth_DevWebAttributes.h>

SHIB_REFLECTION_DEFINE_BEGIN(ECSEntityWebHandler)
	.classAttrs(DevWebCommandAttribute("/entity"))

	.BASE(CivetHandler)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(ECSEntityWebHandler)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ECSEntityWebHandler)

ECSEntityWebHandler::ECSEntityWebHandler(void):
	_ecs(GetApp().getManagerTFast<ECSManager>())
{
}

bool ECSEntityWebHandler::handleGet(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);
	int32_t id = -1;

	if (req->query_string) {
		sscanf(req->query_string, "id=%i", &id);
	}

	mg_printf(conn, "HTTP/1.1 200 OK\r\n");
	mg_printf(conn, "Content-Type: text/html; charset=utf-8\r\n");
	mg_printf(conn, "Connection: close\r\n\r\n");

	mg_printf(conn, "<html><body>\r\n");

	if (_ecs.isValid(id)) {
		mg_printf(conn, "<h1>Valid Entity ID: %i\r\n", id);
	} else {
		mg_printf(conn, "<h1>Invalid Entity ID: %i\r\n", id);
	}

	mg_printf(conn, "</body></html>\r\n");

	return true;
}

NS_END
