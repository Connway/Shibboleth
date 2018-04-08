/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IManager.h>

struct ImGuiContext;

namespace Gleam
{
	class IKeyboard;
}

NS_SHIBBOLETH

class ImGuiManager : public IManager, public IUpdateQuery
{
public:
	static const char* GetFriendlyName(void);

	ImGuiManager(void);
	~ImGuiManager(void);

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	bool init(void);

	void setImGuiContext(void);

private:
	ImGuiContext* _imgui_context = nullptr;

	void generateDrawCommands(double dt, void* frame_data);
	void submitDrawCommands(double, void* frame_data);

	void handleCharacter(Gleam::IKeyboard*, unsigned int char_code);

	GAFF_NO_COPY(ImGuiManager);
	GAFF_NO_MOVE(ImGuiManager);

	SHIB_REF_DEF(ImGuiManager);
	REF_DEF_REQ;
};

NS_END
