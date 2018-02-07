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
#include <nana/gui/widgets/listbox.hpp>
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
	_menu_bar.create(*_main_form);

	_main_form->caption("Shibboleth Editor");
	_main_form->events().destroy([&]() -> void { close(); });

	addBuiltInMenus();

	return _engine_instance.init(0, nullptr);
}

void Editor::run(void)
{
	_main_form->show();
	nana::exec();
}

void Editor::destroy(void)
{
	_engine_instance.destroy();
}

void Editor::close(void)
{
	for (auto& sub_form : _sub_forms) {
		sub_form->close();
	}

	_sub_forms.clear();
	_main_form->close();
}

nana::menu* Editor::addMenu(const char* name)
{
	GAFF_ASSERT(_menu_entries.find(name) == _menu_entries.end());
	nana::menu* const menu = &_menu_bar.push_back(name);
	_menu_entries[name] = menu;

	return menu;
}

nana::menu* Editor::getMenu(const char* name)
{
	auto it = _menu_entries.find(name);
	return it != _menu_entries.end() ? it->second : nullptr;
}

void Editor::addBuiltInMenus(void)
{
	nana::menu* const file_menu = addMenu("&File");

	file_menu->append("E&xit", std::bind([&](nana::menu::item_proxy&) -> void
	{
		close();
	},
	std::placeholders::_1));


	nana::menu* const window_menu = addMenu("&Window");

	window_menu->append("Modules", std::bind([&](nana::menu::item_proxy&) -> void
	{
		nana::rectangle rect;
		rect.x = _main_form->pos().x + 300;
		rect.y = _main_form->pos().y + 200;
		rect.width = 600;
		rect.height = 400;

		_sub_forms.emplace_back(new nana::form(rect));
		nana::form& popup = *_sub_forms.back();
		popup.caption("Loaded Modules");

		nana::listbox modules(popup);
		modules.show_header(false);

		// Makes the assumption that the entity module and the resource module are loaded.
		// People who replace these modules will need to edit this.
		const Vector<Gaff::Hash64>* const component_names = _engine_instance.getTypeBucket(Gaff::FNV1aHash64Const("Component"));
		const Vector<Gaff::Hash64>* const manager_names = _engine_instance.getTypeBucket(Gaff::FNV1aHash64Const("IManager"));
		const Vector<Gaff::Hash64>* const attribute_names = _engine_instance.getTypeBucket(Gaff::FNV1aHash64Const("Gaff::IAttribute"));
		const Vector<Gaff::Hash64>* const resource_names = _engine_instance.getTypeBucket(Gaff::FNV1aHash64Const("IResource"));
		const Vector<Gaff::Hash64>* const misc_names = _engine_instance.getTypeBucket(Gaff::FNV1aHash64Const("**"));

		modules.append_header("Components", 500);
		modules.append_header("Managers", 0);
		modules.append_header("Attributes", 0);
		modules.append_header("Resources", 0);
		modules.append_header("Enums", 0);
		modules.append_header("Misc", 0);
		modules.append_header("Modules", 0);

		nana::listbox::cat_proxy components = modules.append("Components");
		nana::listbox::cat_proxy managers = modules.append("Managers");
		nana::listbox::cat_proxy attributes = modules.append("Attributes");
		nana::listbox::cat_proxy resources = modules.append("Resources");
		nana::listbox::cat_proxy enums = modules.append("Enums");
		nana::listbox::cat_proxy misc = modules.append("Misc");
		nana::listbox::cat_proxy mods = modules.append("Modules");

		for (Gaff::Hash64 name : *component_names) {
			components.push_back(_engine_instance.getReflection(name)->getReflectionInstance().getName());
		}

		for (Gaff::Hash64 name : *manager_names) {
			managers.push_back(_engine_instance.getReflection(name)->getReflectionInstance().getName());
		}

		for (Gaff::Hash64 name : *attribute_names) {
			attributes.push_back(_engine_instance.getReflection(name)->getReflectionInstance().getName());
		}

		for (Gaff::Hash64 name : *resource_names) {
			resources.push_back(_engine_instance.getReflection(name)->getReflectionInstance().getName());
		}

		for (Gaff::Hash64 name : *misc_names) {
			misc.push_back(_engine_instance.getReflection(name)->getReflectionInstance().getName());
		}

		for (const U8String& module_name : _engine_instance.getLoadedModuleNames()) {
			mods.push_back(module_name.data());
		}

		for (const auto& enum_entry : _engine_instance.getEnumReflectionDefinitions()) {
			enums.push_back(enum_entry.second->getReflectionInstance().getName());
		}

		popup.div("<a>");
		popup["a"] << modules;

		popup.collocate();
		popup.modality();
	},
	std::placeholders::_1));
}

NS_END
