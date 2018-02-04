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

#include "Shibboleth_Editor.h"
#include <Gaff_JSON.h>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui.hpp>

NS_SHIBBOLETH

Editor::Editor(void)
{
}

Editor::~Editor(void)
{
}

bool Editor::init(void)
{
	nana::rectangle rect(0, 0, 800, 600);
	Gaff::JSON config;
	
	if (config.parseFile("cfg/editor.cfg")) {
		rect.x = config["window_x"].getInt32();
		rect.y = config["window_y"].getInt32();
		rect.width = config["window_width"].getUInt32();
		rect.height = config["window_height"].getUInt32();
	}

	_main_form.reset(new nana::form(rect));
	return _engine_instance.init(0, nullptr);
}

void Editor::run(void)
{
	nana::label label(*_main_form, nana::rectangle(10, 10, 100, 100));
	label.caption("Hello, World!");

	_main_form->show();
	nana::exec();
}

void Editor::destroy(void)
{
}

NS_END
