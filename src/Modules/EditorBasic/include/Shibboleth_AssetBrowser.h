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

class wxGenericDirCtrl;
class wxTreeEvent;

NS_SHIBBOLETH

class DirectoryControl;

class AssetBrowser final : public wxPanel, public Gaff::IReflectionObject
{
public:
	AssetBrowser(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize
	);

	~AssetBrowser(void);

private:
	DirectoryControl* _file_ctrl = nullptr;
	DirectoryControl* _dir_ctrl = nullptr;

	void onDirectorySelection(wxTreeEvent& event);
	void onFileRightClick(wxTreeEvent& event);
	void onFileSelection(wxTreeEvent& event);
	void onFileActivated(wxTreeEvent& event);

	SHIB_REFLECTION_CLASS_DECLARE(AssetBrowser);
};

NS_END

SHIB_REFLECTION_DECLARE(AssetBrowser)
