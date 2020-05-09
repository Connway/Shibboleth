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

#pragma once

#include <Shibboleth_Reflection.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_SmartPtrs.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_ISystem.h>
#include <Gleam_IKeyboard.h>
#include <Gleam_IMouse.h>
#include <EASTL/chrono.h>

NS_SHIBBOLETH

static constexpr int32_t g_max_local_players = 4;

class InputManager final : public IManager
{
public:
	template <size_t size>
	float getAliasValue(const char (&alias_name)[size], int32_t player_id)
	{
		return getAliasValue(Gaff::FNV1aHash32Const(alias_name), player_id);
	}

	template <size_t size>
	int32_t getAliasIndex(const char (&alias_name)[size])
	{
		return getAliasIndex(Gaff::FNV1aHash32Const(alias_name));
	}

	bool initAllModulesLoaded(void) override;
	void update(void);
	void resetTimer(void);

	float getAliasValue(Gaff::Hash32 alias_name, int32_t player_id) const;
	float getAliasValue(const char* alias_name, int32_t player_id) const;
	float getAliasValue(int32_t index, int32_t player_id) const;

	int32_t getAliasIndex(Gaff::Hash32 alias_name) const;
	int32_t getAliasIndex(const char* alias_name) const;

	void setKeyboardMousePlayerID(int32_t player_id);
	int32_t getKeyboardMousePlayerID(void) const;

	Gleam::IKeyboard* getKeyboard(void);
	Gleam::IMouse* getMouse(void);

private:
	struct Binding final
	{
		Vector<Gleam::MouseCode> mouse_codes{ ProxyAllocator("Input") };
		Vector<Gleam::KeyCode> key_codes{ ProxyAllocator("Input") };
		int32_t alias_index;
		float curr_tap_time = 0.0f;
		float tap_interval = 0.1f;
		float scale = 1.0f;
		int8_t count = 0;
		int8_t curr_tap = 0;
		int8_t taps = 0;
		bool first_frame = false;
	};

	struct Alias final
	{
		float value = 0.0f;
	};

	VectorMap<Gaff::Hash32, Alias> _alias_values[g_max_local_players] = {
		VectorMap<Gaff::Hash32, Alias>{ ProxyAllocator("Input") },
		VectorMap<Gaff::Hash32, Alias>{ ProxyAllocator("Input") },
		VectorMap<Gaff::Hash32, Alias>{ ProxyAllocator("Input") },
		VectorMap<Gaff::Hash32, Alias>{ ProxyAllocator("Input") }
	};

	Vector<Binding> _bindings[g_max_local_players] = {
		Vector<Binding>{ ProxyAllocator("Input") },
		Vector<Binding>{ ProxyAllocator("Input") },
		Vector<Binding>{ ProxyAllocator("Input") },
		Vector<Binding>{ ProxyAllocator("Input") }
	};

	UniquePtr<Gleam::IKeyboard> _keyboard;
	UniquePtr<Gleam::IMouse> _mouse;

	// Maintaining our own timer to avoid dependencies, as we are using real-time and don't need
	// anything from GameTime.
	eastl::chrono::time_point<eastl::chrono::high_resolution_clock> _start;
	eastl::chrono::time_point<eastl::chrono::high_resolution_clock> _end;

	int32_t _km_player_id = 0;

	void handleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value);
	void handleMouseInput(Gleam::IInputDevice*, int32_t mouse_code, float value);

	SHIB_REFLECTION_CLASS_DECLARE(InputManager);
};

class InputSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(void) override;

private:
	InputManager* _input_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(InputSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(InputManager)
SHIB_REFLECTION_DECLARE(InputSystem)
