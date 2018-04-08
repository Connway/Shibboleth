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

#include <Shibboleth_App.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/vector_map.h>
#include <EASTL/string.h>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/menu.hpp>

namespace nana
{
	class form;
}

NS_SHIBBOLETH

class Editor
{
public:
	Editor(void);
	~Editor(void);

	bool init(void);

	void run(void);
	void destroy(void);

	void close(void);

	nana::menu* addMenu(const char* name);
	nana::menu* getMenu(const char* name);

private:
	eastl::vector_map<eastl::string, nana::menu*> _menu_entries;
	eastl::vector< eastl::unique_ptr<nana::form> > _sub_forms;
	eastl::unique_ptr<nana::form> _main_form = nullptr;

	nana::menubar _menu_bar;

	App _engine_instance;

	void addBuiltInMenus(void);
};

NS_END
