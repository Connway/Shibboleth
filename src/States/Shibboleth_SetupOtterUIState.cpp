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

#include "Shibboleth_SetupOtterUIState.h"
#include "Shibboleth_OtterUIManager.h"
#include "Shibboleth_IApp.h"

#include "Shibboleth_Object.h"

NS_SHIBBOLETH

SetupOtterUIState::SetupOtterUIState(IApp& app):
	_file_system(*app.getFileSystem()), _renderer(app), _app(app)
{
}

SetupOtterUIState::~SetupOtterUIState(void)
{
}

bool SetupOtterUIState::init(unsigned int /*state_id*/)
{
	// Make sure we have a transition for when we finish loading.
	// If we have more than one transition, you're using this state wrong
	// and should fix it.
	GAFF_ASSERT(_transitions.size() == 1);
	//_state_id = state_id;
	return true;
}

void SetupOtterUIState::enter(void)
{
}

void SetupOtterUIState::update(void)
{
	OtterUIManager& otterui_manager = _app.getManagerT<OtterUIManager>("OtterUI Manager");

	//otterui_manager.setSoundSystem();
	otterui_manager.setFileSystem(&_file_system);
	otterui_manager.setRenderer(&_renderer);

	_app.switchState(_transitions[0]);
}

void SetupOtterUIState::exit(void)
{
}

NS_END
