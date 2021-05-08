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
#include "Shibboleth_DevWebUtils.h"
#include <Shibboleth_InputReflection.h>
#include <Shibboleth_InputManager.h>
#include <Gaff_Math.h>
#include <Gaff_JSON.h>
#include <EAStdC/EAString.h>

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

void WebInputHandler::update(void)
{
	// Add new input devices.
	{
		const EA::Thread::AutoMutex queue_lock(_new_device_queue_lock);
		const EA::Thread::AutoMutex mgr_lock(_input_mgr_lock);

		for (const NewDeviceEntry& entry : _new_device_queue) {
			int32_t player_id = entry.player_id;

			if (entry.player_id == -1) {
				player_id = _input_mgr->addPlayer();
			}

			for (Gleam::IInputDevice* const device : entry.devices) {
				_input_mgr->addInputDevice(device, player_id);
			}
		}

		_new_device_queue.clear();
	}

	// Process input queue and do input message pumps.
	{
		const EA::Thread::AutoMutex lock(_input_queue_lock);

		for (const InputEntry& entry : _input_queue) {
			const auto keyboard_it = _keyboards.find(entry.player_id);
			const auto mouse_it = _mice.find(entry.player_id);

			if (Gaff::Between(entry.message.base.type, Gleam::EventType::InputKeyDown, Gleam::EventType::InputCharacter)) {
				if (keyboard_it != _keyboards.end()) {
					keyboard_it->second->handleMessage(entry.message);

				} else {
					// $TODO: Log error.
				}

			} else if (Gaff::Between(entry.message.base.type, Gleam::EventType::InputMouseMove, Gleam::EventType::InputMouseWheelVertical)) {
				if (mouse_it != _mice.end()) {
					mouse_it->second->handleMessage(entry.message);

				} else {
					// $TODO: Log error.
				}
			}
		}

		_input_queue.clear();
	}
}

bool WebInputHandler::handlePost(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (req->content_length <= 0) {
		return true;
	}

	const Gaff::JSON req_data = ReadJSON(*conn, *req);
	const Gaff::JSON inputs = req_data["inputs"];

	if (!inputs.isArray()) {
		// $TODO: Log error.
		return true;
	}

	inputs.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		const Gaff::JSON mouse_pos_state = value["mouse_pos_state"];
		const Gaff::JSON mouse_code = value["mouse_code"];
		const Gaff::JSON char_code = value["char_code"];
		const Gaff::JSON key_code = value["key_code"];

		InputEntry entry;
		entry.message.base.window = nullptr;

		// Parse each input and put into input queue.
		entry.player_id = value["player_id"].getInt32(-1);
		const float input_value = value["value"].getFloat(0.0f);

		char buffer[64] = { 0 };

		if (mouse_pos_state.isObject()) {
			entry.message.base.type = Gleam::EventType::InputMouseMove;
			entry.message.mouse_move.abs_x = mouse_pos_state["abs_x"].getInt32(0);
			entry.message.mouse_move.abs_y = mouse_pos_state["abs_y"].getInt32(0);
			entry.message.mouse_move.rel_x = mouse_pos_state["rel_x"].getInt32(0);
			entry.message.mouse_move.rel_y = mouse_pos_state["rel_y"].getInt32(0);
			entry.message.mouse_move.dx = mouse_pos_state["dx"].getInt32(0);
			entry.message.mouse_move.dy = mouse_pos_state["dy"].getInt32(0);

		} else if (mouse_code.isString()) {
			const char* const code_name = mouse_code.getString(buffer, sizeof(buffer));
			const auto& ref_def = Reflection<Gleam::MouseCode>::GetReflectionDefinition();

			if (!ref_def.entryExists(code_name)) {
				// $TODO: Log error.
				return false;
			}

			const Gleam::MouseCode gleam_mouse_code = ref_def.getEntry(code_name);

			if (gleam_mouse_code < Gleam::MouseCode::ButtonCount) {
				entry.message.base.type = input_value > 0.0f ? Gleam::EventType::InputMouseDown : Gleam::EventType::InputMouseUp;
				entry.message.mouse_state.button = gleam_mouse_code;

			} else if (Gaff::Between(gleam_mouse_code, Gleam::MouseCode::WheelVertical, Gleam::MouseCode::WheelDown)) {
				entry.message.base.type = Gleam::EventType::InputMouseWheelVertical;
				entry.message.mouse_state.wheel = static_cast<int16_t>(input_value);

			} else if (Gaff::Between(gleam_mouse_code, Gleam::MouseCode::WheelHorizontal, Gleam::MouseCode::WheelRight)) {
				entry.message.base.type = Gleam::EventType::InputMouseWheelHorizontal;
				entry.message.mouse_state.wheel = static_cast<int16_t>(input_value);

			} else {
				// $TODO: Log error.
			}

		} else if (char_code.isString()) {
			entry.message.base.type = Gleam::EventType::InputCharacter;

			// All this nonsense because eastl::DecodePart() takes pointer references ... for some reason.
			const char* character = char_code.getString(buffer, sizeof(buffer));

			char32_t out_character;
			char32_t* out_character_begin = &out_character;

			eastl::DecodePart(
				character,
				EA::StdC::Strend(character),
				out_character_begin,
				out_character_begin + 1
			);

			// Decode first character into UTF-32
			entry.message.key_char.character = static_cast<uint32_t>(out_character);

		} else if (key_code.isString()) {
			const char* const code_name = key_code.getString(buffer, sizeof(buffer));
			const auto& ref_def = Reflection<Gleam::KeyCode>::GetReflectionDefinition();

			if (!ref_def.entryExists(code_name)) {
				// $TODO: Log error.
				return false;
			}

			entry.message.base.type = input_value > 0.0f ? Gleam::EventType::InputKeyDown : Gleam::EventType::InputKeyUp;
			entry.message.key_char.key = ref_def.getEntry(code_name);
		}

		const EA::Thread::AutoMutex lock(_input_queue_lock);
		_input_queue.emplace_back(entry);

		return false;
	});

	return true;
}

bool WebInputHandler::handlePut(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (req->content_length <= 0) {
		return true;
	}

	const Gaff::JSON req_data = ReadJSON(*conn, *req);
	const bool create_keyboard = req_data["create_keyboard"].getBool(false);
	const bool create_mouse = req_data["create_mouse"].getBool(false);

	NewDeviceEntry entry;
	entry.player_id = req_data["player_id"].getInt32(-1);

	Gaff::JSON response = Gaff::JSON::CreateObject();

	if (create_keyboard && _keyboards.find(entry.player_id) == _keyboards.end()) {
		KeyboardWeb* const keyboard = SHIB_ALLOCT(KeyboardWeb, g_allocator);

		if (keyboard->init()) {
			entry.devices.emplace_back(keyboard);

			response.setObject("keyboard", Gaff::JSON::CreateBool(true));

			const EA::Thread::AutoMutex lock(_device_lock);
			_keyboards[entry.player_id].reset(keyboard);

		} else {
			SHIB_FREET(keyboard, g_allocator);
		}
	}

	if (create_mouse && _mice.find(entry.player_id) == _mice.end()) {
		MouseWeb* const mouse = SHIB_ALLOCT(MouseWeb, g_allocator);

		if (mouse->init()) {
			entry.devices.emplace_back(mouse);

			response.setObject("mouse", Gaff::JSON::CreateBool(true));

			const EA::Thread::AutoMutex lock(_device_lock);
			_mice[entry.player_id].reset(mouse);

		} else {
			SHIB_FREET(mouse, g_allocator);
		}
	}

	if (!entry.devices.empty()) {
		const EA::Thread::AutoMutex lock(_new_device_queue_lock);
		_new_device_queue.emplace_back(std::move(entry));
	}

	char response_buffer[1024] = { 0 };
	response.dump(response_buffer, sizeof(response_buffer));

	WriteResponse(*conn, response_buffer);

	return true;
}

bool WebInputHandler::handleDelete(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (req->content_length <= 0) {
		return true;
	}

	const Gaff::JSON req_data = ReadJSON(*conn, *req);

	int32_t player_id = req_data["player_id"].getInt32(-1);

	const EA::Thread::AutoMutex lock(_input_mgr_lock);
	_input_mgr->removePlayer(player_id);

	return true;
}

bool WebInputHandler::handleOptions(CivetServer* /*server*/, mg_connection* conn)
{
	const mg_request_info* const req = mg_get_request_info(conn);

	if (req->content_length <= 0) {
		return true;
	}

	const Gaff::JSON req_data = ReadJSON(*conn, *req);
	const Gaff::JSON req_type = req_data["request_type"];

	char req_type_buffer[32] = { 0 };
	req_type.getString(req_type_buffer, sizeof(req_type_buffer));

	// $TODO: Refactor this if more GET request type are introduced.
	if (strncmp(req_type_buffer, "player_devices", sizeof(req_type_buffer))) {
		return true;
	}

	Vector<const Gleam::IInputDevice*> devices(ProxyAllocator("DevWeb"));
	Vector<int32_t> player_ids(ProxyAllocator("DevWeb"));
	Gaff::JSON response = Gaff::JSON::CreateArray();

	{
		const EA::Thread::AutoMutex lock(_input_mgr_lock);
		_input_mgr->getPlayerIDs(player_ids);
	}

	devices.reserve(3);

	for (int32_t player_id : player_ids) {
		devices.clear();

		{
			const EA::Thread::AutoMutex lock(_input_mgr_lock);
			_input_mgr->getInputDevices(player_id, devices);
		}

		Gaff::JSON devices_json = Gaff::JSON::CreateArray();

		for (const Gleam::IInputDevice* device : devices) {
			Gaff::JSON device_name = Gaff::JSON::CreateString(device->getDeviceName());
			devices_json.push(std::move(device_name));
		}

		const int32_t new_size = player_id + 1;

		// Empty slots are null.
		while (response.size() < new_size) {
			response.push(Gaff::JSON::CreateNull());
		}

		response.setObject(player_id, std::move(devices_json));
	}

	char response_buffer[1024] = { 0 };
	response.dump(response_buffer, sizeof(response_buffer));

	WriteResponse(*conn, response_buffer);
	return true;
}

NS_END
