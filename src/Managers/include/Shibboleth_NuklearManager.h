/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_INuklearManager.h"
#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_IncludeNuklear.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Map.h>

namespace Gleam
{
	class IInputDevice;
	class IKeyboard;
	class IWindow;
}

NS_SHIBBOLETH

class NuklearManager : public IManager, public IUpdateQuery, public INuklearManager
{
public:
	static const char* GetFriendlyName(void);

	NuklearManager(void);
	~NuklearManager(void);

	const char* getName(void) const override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	bool init(void) override;

private:
	struct NuklearData
	{
		nk_context context;
		//nk_font_atlas atlas;
		nk_buffer cmds;
		nk_draw_null_texture null_texture;
		size_t max_vert_buffer;
		size_t max_index_buffer;

		// viewport
		// raster state
		// shader program
		// const buffer
		// blend state
		// vert buffer
		// index buffer
		// sampler state
	};

	Map<Gleam::IWindow*, NuklearData> _nuklear_data;
	nk_allocator _allocator;
	nk_context _context;
	bool _init = false;

	void generateDrawCommands(double dt, void* frame_data);
	void submitDrawCommands(double, void* frame_data);

	void handleKeyboard(Gleam::IInputDevice* input_device, unsigned int input_code, float value);
	void handleMouse(Gleam::IInputDevice* input_device, unsigned int input_code, float value);
	void handleCharacter(Gleam::IKeyboard*, unsigned int char_code);

	GAFF_NO_COPY(NuklearManager);
	GAFF_NO_MOVE(NuklearManager);

	SHIB_REF_DEF(NuklearManager);
	REF_DEF_REQ;
};

NS_END
