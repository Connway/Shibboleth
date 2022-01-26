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

#pragma once

#include "Shibboleth_Keyboard_Web.h"
#include "Shibboleth_Mouse_Web.h"
#include <Shibboleth_IDevWebHandler.h>
#include <Shibboleth_Reflection.h>
#include <Gaff_Flags.h>
#include <EAThread/eathread_mutex.h>

NS_SHIBBOLETH

class InputManager;

class WebInputHandler final : public IDevWebHandler, public Refl::IReflectionObject
{
public:
	WebInputHandler(void);

	void update(void) override;

	bool handlePost(CivetServer* server, mg_connection* conn) override;
	bool handlePut(CivetServer* server, mg_connection* conn) override;
	bool handleDelete(CivetServer* server, mg_connection* conn) override;
	bool handleOptions(CivetServer* server, mg_connection* conn) override;

	SHIB_REFLECTION_CLASS_DECLARE(WebInputHandler);

private:
	struct NewDeviceEntry final
	{
		Vector<Gleam::IInputDevice*> devices{ ProxyAllocator("DevWeb") };
		int32_t player_id = -1;
	};

	struct InputEntry final
	{
		Gleam::AnyMessage message;
		int32_t player_id;
	};

	Vector<NewDeviceEntry> _new_device_queue{ ProxyAllocator("DevWeb") };
	Vector<InputEntry> _input_queue{ ProxyAllocator("DevWeb") };

	VectorMap<int32_t, UniquePtr<KeyboardWeb> > _keyboards{ ProxyAllocator("DevWeb") };
	VectorMap<int32_t, UniquePtr<MouseWeb> > _mice{ ProxyAllocator("DevWeb") };

	EA::Thread::Mutex _new_device_queue_lock;
	EA::Thread::Mutex _input_queue_lock;
	EA::Thread::Mutex _device_lock;
	EA::Thread::Mutex _input_mgr_lock;

	InputManager* _input_mgr = nullptr;
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::WebInputHandler)
