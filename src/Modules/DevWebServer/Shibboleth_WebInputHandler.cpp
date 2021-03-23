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

#include "Shibboleth_WebInputHandler.h"
#include "Shibboleth_DevWebAttributes.h"
#include <Shibboleth_InputManager.h>
#include <Gaff_JSON.h>

SHIB_REFLECTION_DEFINE_BEGIN(WebInputHandler)
	.classAttrs(DevWebCommandAttribute("/input"))

	.BASE(IDevWebHandler)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(WebInputHandler)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(WebInputHandler)

static ProxyAllocator g_allocator("DevWeb");

WebInputHandler::WebInputHandler(void):
	_input_mgr(&GetApp().getManagerTFast<InputManager>())
{
}

bool WebInputHandler::handlePost(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (!req->query_string) {
		return false;
	}

	// Read inputs as JSON blob.
	// Parse and forward as appropriate.

	//sscanf(req->query_string, "id=%i", &id);

	return true;
}

bool WebInputHandler::handlePut(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (!req->query_string) {
		return true;
	}

	int32_t create_keyboard = 0;
	int32_t create_mouse = 0;
	int32_t player_id = -1;

	sscanf(req->query_string, "keyboard=%i", &create_keyboard);
	sscanf(req->query_string, "mouse=%i", &create_mouse);
	sscanf(req->query_string, "id=%i", &player_id);

	if (player_id == -1) {
		player_id = _input_mgr->addPlayer();
	}

	Gaff::JSON response = Gaff::JSON::CreateObject();

	if (create_keyboard && _keyboards.find(player_id) == _keyboards.end()) {
		KeyboardWeb* const keyboard = SHIB_ALLOCT(KeyboardWeb, g_allocator);

		if (keyboard->init()) {
			_keyboards[player_id].reset(keyboard);
			_input_mgr->addInputDevice(keyboard, player_id);

			response.setObject("keyboard", Gaff::JSON::CreateBool(true));

		} else {
			SHIB_FREET(keyboard, g_allocator);
		}
	}

	if (create_mouse /*&& _mice.find(player_id) == _mice.end()*/) {
		//MouseWeb* const mouse = SHIB_ALLOCT(MouseWeb, g_allocator);

		//if (mouse->init()) {
		//	_mice[player_id].reset(mouse);
		//	_input_mgr->addInputDevice(mouse, player_id);

		//	response.setObject("mouse", Gaff::JSON::CreateBool(true));

		//} else {
		//	SHIB_FREET(mouse, g_allocator);
		//}
	}

	char response_buffer[1024] = { 0 };
	response.dump(response_buffer, sizeof(response_buffer));

	mg_printf(conn, "HTTP/1.1 200 OK\r\n");
	mg_printf(conn, "Content-Type: application/json; charset=utf-8\r\n");
	mg_printf(conn, "Connection: close\r\n\r\n");
	mg_printf(conn, "%s", response_buffer);

	return true;
}

bool WebInputHandler::handleDelete(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (!req->query_string) {
		return true;
	}

	int32_t player_id = -1;
	sscanf(req->query_string, "id=%i", &player_id);

	_input_mgr->removePlayer(player_id);

	return true;
}

NS_END
