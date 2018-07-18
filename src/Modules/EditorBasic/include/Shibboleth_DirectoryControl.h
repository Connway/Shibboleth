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

#include <Shibboleth_Defines.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <wx/window.h>
#include <wx/dirctrl.h>

class wxDir;

NS_SHIBBOLETH

enum
{
	DC_SINGLE_DEPTH = 0x0400
};

// Forces the default directory to be the root of the tree.
class DirectoryControl : public wxGenericDirCtrl
{
public:
	DirectoryControl(
		wxWindow *parent,
		const wxWindowID id = wxID_ANY,
		const wxString& dir = wxDirDialogDefaultFolderStr,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDIRCTRL_3D_INTERNAL,
		const wxString& filter = wxEmptyString,
		int defaultFilter = 0,
		const wxString& name = wxTreeCtrlNameStr
	);

	void reset(const wxString& path);

private:
	void SetupSections(void) override;
	void ExpandRoot(void) override;
	bool openDir(wxDir& dir);
};

NS_END
