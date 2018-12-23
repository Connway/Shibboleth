/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <wx/panel.h>

class wxScrollWinEvent;
class wxCommandEvent;
class wxTextCtrl;

NS_SHIBBOLETH

class NumberInspector final : public Gaff::IReflectionObject, public wxPanel
{
public:
	NumberInspector(
		void* value,
		const Gaff::IReflectionDefinition& ref_def,
		const Gaff::IReflectionDefinition* parent_ref_def,
		wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize
	);

private:
	wxTextCtrl* _text = nullptr;
	void* _value = nullptr;
	double _step = 1.0f;

	void (*_step_func)(void* value, double step) = nullptr;

	wxValidator* createValidator(void* value, const Gaff::IReflectionDefinition& ref_def, const Gaff::IReflectionDefinition* parent_ref_def);
	void onTextChange(const wxCommandEvent&);
	void onScrollDown(const class wxScrollWinEvent&);
	void onScrollUp(const class wxScrollWinEvent&);

	SHIB_REFLECTION_CLASS_DECLARE(NumberInspector);
};

NS_END

SHIB_REFLECTION_DECLARE(NumberInspector);
