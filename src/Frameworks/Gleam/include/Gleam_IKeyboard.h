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

#include "Gleam_Window_Defines.h"
#include "Gleam_IInputDevice.h"

NS_GLEAM

class IKeyboard : public IInputDevice
{
public:
	using CharacterHandler = eastl::function<void (IKeyboard*, uint32_t)>;

	IKeyboard(void) {}
	virtual ~IKeyboard(void) {}

	virtual bool init(IWindow& window, bool no_windows_key) = 0;
	virtual bool init(bool no_windows_key) = 0;
	virtual bool init(IWindow& window) = 0;
	virtual bool init(void) = 0;

	bool isKeyDown(KeyCode key) const { return _data[static_cast<int32_t>(key)]; }
	bool isKeyUp(KeyCode key) const { return !_data[static_cast<int32_t>(key)]; }

	const bool* getKeyboardData(void) const { return _data; }

	bool isKeyboard(void) const override { return true; }
	bool isMouse(void) const override { return false; }

	int32_t addCharacterHandler(const CharacterHandler& handler)
	{
		const int32_t id = _next_id++;
		_character_handlers.emplace(id, handler);
		return id;
	}

	int32_t addCharacterHandler(CharacterHandler&& handler)
	{
		const int32_t id = _next_id++;
		_character_handlers.emplace(id, std::move(handler));
		return id;
	}

	bool removeCharacterHandler(int32_t id)
	{
		const auto it = _character_handlers.find(id);

		if (it != _character_handlers.end()) {
			_character_handlers.erase(it);
			return true;
		}

		return false;
	}

protected:
	VectorMap<int32_t, CharacterHandler> _character_handlers;
	bool _data[256] = { false };

private:
	int32_t _next_id = 0;
};

NS_END
