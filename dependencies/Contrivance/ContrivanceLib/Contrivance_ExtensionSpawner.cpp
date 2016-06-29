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

#include "Contrivance_ExtensionSpawner.h"
#include "Contrivance_IContrivanceWindow.h"
#include "Contrivance_DockWidgetWrapper.h"
#include "Contrivance_ContrivanceExtension.h"
#include "ui_Contrivance_ExtensionSpawner.h"
#include <QAbstractButton>
#include <QMainWindow>
#include <QDockWidget>
#include <QMouseEvent>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>
#include <QDir>

ExtensionSpawner::ExtensionSpawner(IContrivanceWindow& window, QWidget* parent):
	QWidget(parent), _ui(new Ui::ExtensionSpawner),
	_window(window), _num_spawned_extensions(0)
{
	_ui->setupUi(this);
	//_ui->listWidget->installEventFilter(this);

	connect(_ui->listWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleClicked(QModelIndex)));
}

ExtensionSpawner::~ExtensionSpawner()
{
	delete _ui;
}

void ExtensionSpawner::loadExtensions(void)
{
	_window.printToConsole("Loading extensions from modules found with filter 'extensions/*" SHARED_LIBRARY_SUFFIX SHARED_LIBRARY_EXTENSION "'");

	QString nameFilter = "*" SHARED_LIBRARY_SUFFIX SHARED_LIBRARY_EXTENSION;
	QDir directory("./extensions");
	QStringList entryList = directory.entryList(QStringList{nameFilter}, QDir::Files, QDir::Name);

	for (auto it_dll = entryList.begin(); it_dll != entryList.end(); ++it_dll) {
		ExtensionData ext_data;
		ext_data.library = new QLibrary;
		ext_data.library->setFileName("./extensions/" + *it_dll);

		if (!ext_data.library->load()) {
			_window.printToConsole("Failed to load module '" + *it_dll + "' for reason '" + ext_data.library->errorString() + "'");
			delete ext_data.library;
			continue;
		}

		ext_data.init_func = (ExtensionData::InitExtensionModuleFunc)ext_data.library->resolve("InitExtensionModule");
		ext_data.shutdown_func = (ExtensionData::ShutdownExtensionModuleFunc)ext_data.library->resolve("ShutdownExtensionModule");
		ext_data.save_func = (ExtensionData::SaveInstanceDataFunc)ext_data.library->resolve("SaveInstanceData");
		ext_data.load_func = (ExtensionData::LoadInstanceDataFunc)ext_data.library->resolve("LoadInstanceData");
		ext_data.create_func = (ExtensionData::CreateInstanceFunc)ext_data.library->resolve("CreateInstance");
		ext_data.destroy_func = (ExtensionData::DestroyInstanceFunc)ext_data.library->resolve("DestroyInstance");
		ext_data.get_exts_func = (ExtensionData::GetExtensionsFunc)ext_data.library->resolve("GetExtensions");

		// If we're missing an essential function
		if (!ext_data.save_func || !ext_data.load_func || !ext_data.create_func || !ext_data.get_exts_func) {
			QString missing_func;

			if (!ext_data.save_func) {
				missing_func = "SaveInstanceData";
			} else if (!ext_data.load_func) {
				missing_func = "LoadInstanceData";
			} else if (!ext_data.create_func) {
				missing_func = "CreateInstance";
			} else if (!ext_data.get_exts_func) {
				missing_func = "GetExtensions";
			}

			_window.printToConsole("'" + *it_dll + "' is missing core function '" + missing_func + "'", CMT_ERROR);
			continue;
		}

		// If we have an init func and it failed
		if (ext_data.init_func && !ext_data.init_func(_window)) {
			_window.printToConsole("Call to 'InitExtensionModule' failed for '" + *it_dll + "'", CMT_ERROR);
			continue;
		}

		ext_data.get_exts_func(ext_data.extension_names);

		for (auto it_ext = ext_data.extension_names.begin(); it_ext != ext_data.extension_names.end(); ++it_ext) {
			// Maybe change this to an if and then fail to add this element and skip over it.
			Q_ASSERT(!_extension_indices.contains(*it_ext));
			_extension_indices[*it_ext] = _extension_modules.size();
			_ui->listWidget->addItem(*it_ext);

			_window.printToConsole("Loaded extension with name '" + *it_ext + "' from '" + *it_dll + "'");
		}

		_extension_modules.push_back(ext_data);
	}
}

// Until I figure out how to do the TODO mentioned at line 74, this is commented out.
/*bool ExtensionSpawner::eventFilter(QObject* object, QEvent* event)
{
	// The first time this event hits, we have dragged outside of the Extension Spawner.
	if (event->type() == QEvent::DragEnter) {
		QMainWindow* parent_window = &_window;

		// Spawn on the currently open tab.
		if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
			parent_window = _window.getCurrentTabWindow();
		}

		// create extension
		QDockWidget* dock_widget = new QDockWidget(_ui->listWidget->item(_ui->listWidget->currentRow())->text());
		//dock_widget->setWidget(new ExtensionSpawner(_window));

		parent_window->addDockWidget(Qt::RightDockWidgetArea, dock_widget);
		dock_widget->setFloating(true);
		dock_widget->move(QCursor::pos() - QPoint(200, 0));
		dock_widget->resize(400, 300);

		// TODO: Somehow tell the application to stop the drag and drop and start dragging the newly spawned window.
		//QMouseEvent fake_mouse_event(QEvent::MouseButtonPress, dock_widget->mapToGlobal(QCursor::pos()), Qt::LeftButton, QApplication::mouseButtons(), QApplication::keyboardModifiers());
		//dock_widget->mousePressEvent(&fake_mouse_event);
	}

	return QWidget::eventFilter(object, event);
}*/

void ExtensionSpawner::close(void)
{
	// Copying the array because apparently disconnect() is not working ...
	auto spawned_widgets = std::move(_spawned_widgets);
	_spawned_widgets.clear();

	for (auto it = spawned_widgets.begin(); it != spawned_widgets.end(); ++it) {
		//disconnect(*it);
		it->dock_widget->setWidget(nullptr);
		it->ext_data->destroy_func(it->extension);
		delete it->dock_widget;
	}

	// Shutdown and unload all the modules
	for (auto it = _extension_modules.begin(); it != _extension_modules.end(); ++it) {
		if (it->shutdown_func) {
			it->shutdown_func();
		}

		delete it->library;
	}
}

void ExtensionSpawner::saveGlobalData(QJsonObject& object) const
{
	object.insert("Extensions Spawned", QJsonValue(_num_spawned_extensions));
	QJsonArray id_free_list;

	for (auto it = _free_ids.begin(); it != _free_ids.end(); ++it) {
		id_free_list.push_back(QJsonValue(*it));
	}

	object.insert("Extensions Free IDs", id_free_list);
}

void ExtensionSpawner::loadGlobalData(const QJsonObject& object)
{
	_num_spawned_extensions = object["Extensions Spawned"].toInt();
	QJsonArray id_free_list = object["Extensions Free IDs"].toArray();

	_free_ids.resize(id_free_list.size());

	for (int i = 0; i < id_free_list.size(); ++i) {
		_free_ids[i] = id_free_list[i].toInt();
	}
}

void ExtensionSpawner::saveOpenExtensions(QMainWindow* tab, QJsonArray& widgets) const
{
	for (auto it = _spawned_widgets.begin(); it != _spawned_widgets.end(); ++it) {
		if (it->dock_widget->parentWidget() == tab) {
			QRect geometry = it->dock_widget->geometry();

			QJsonObject widget_data;
			widget_data.insert("Extension", QJsonValue(it->dock_widget->windowTitle()));
			widget_data.insert("Name", QJsonValue(it->dock_widget->objectName()));
			widget_data.insert("Width", QJsonValue(geometry.width()));
			widget_data.insert("Height", QJsonValue(geometry.height()));
			widget_data.insert("ID", QJsonValue(it->extension->getID()));

			widgets.push_back(widget_data);
		}
	}
}

void ExtensionSpawner::loadOpenExtensions(QMainWindow* tab, const QJsonArray& widgets)
{
	QString extension;
	QString name;
	int width = 0;
	int height = 0;
	int id = 0;

	for (auto it = widgets.begin(); it != widgets.end(); ++it) {
		QJsonObject data = (*it).toObject();
		extension = data["Extension"].toString();
		name = data["Name"].toString();
		width = data["Width"].toInt();
		height = data["Height"].toInt();
		id = data["ID"].toInt();

		spawnExtension(extension, tab, QSize(width, height), &name, id);
	}
}

void ExtensionSpawner::spawnExtension(const QString& ext_name, QMainWindow* parent_window, QSize size, const QString* name, int id)
{
	auto it_ind = _extension_indices.find(ext_name);

	if (it_ind == _extension_indices.end() || *it_ind >= _extension_modules.size()) {
		_window.printToConsole("Could not find entry for extension '" + ext_name + "'", CMT_ERROR);
		return;
	}

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);

	QDockWidget* dock_widget = new QDockWidget(ext_name);
	dock_widget->setUserData(0, new DockWidgetWrapper(_window, dock_widget));
	dock_widget->setSizePolicy(sizePolicy);
	dock_widget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dock_widget->setAllowedAreas(Qt::AllDockWidgetAreas);

	if (name) {
		dock_widget->setObjectName(*name);

	} else {
		QString new_name;
		QTextStream name_stream(&new_name);
		name_stream << "Extension Dock ";

		if (_free_ids.isEmpty()) {
			id = _num_spawned_extensions;
			++_num_spawned_extensions;
		} else {
			id = _free_ids.last();
			_free_ids.pop_back();
		}

		name_stream << id;
		dock_widget->setObjectName(new_name);
	}

	ExtensionData& ext_data = _extension_modules[*it_ind];
	IContrivanceExtension* ext = ext_data.create_func(ext_name);

	if (!ext) {
		_window.printToConsole("Failed to create extension '" + ext_name + "'", CMT_ERROR);
		delete dock_widget;
		return;
	}

	ext->setSizeHint(size);
	ext->setID(id);

	dock_widget->setWidget(ext->getWidget());

	parent_window->addDockWidget(Qt::LeftDockWidgetArea, dock_widget);
	dock_widget->setFloating(true);
	dock_widget->resize(400, 300);

	SpawnedExtension se = { ext, dock_widget, &ext_data };
	_spawned_widgets.push_back(se);

	connect(dock_widget, SIGNAL(destroyed(QObject*)), SLOT(widgetDestroyed(QObject*)));
}

void ExtensionSpawner::destroyExtension(QDockWidget* dock_widget)
{
	for (auto it = _spawned_widgets.begin(); it != _spawned_widgets.end(); ++it) {
		if (it->dock_widget == dock_widget) {
			dock_widget->setWidget(nullptr);
			it->ext_data->destroy_func(it->extension);
			_spawned_widgets.erase(it);
			break;
		}
	}
}

void ExtensionSpawner::addFreeID(int id)
{
	_free_ids.push_back(id);
}

void ExtensionSpawner::itemDoubleClicked(QModelIndex index)
{
	QString ext_name = _ui->listWidget->item(index.row())->text();
	bool dock_to_main_window = (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0;
	QMainWindow* parent_window = (dock_to_main_window) ? _window.getThisWindow() : _window.getCurrentTabWindow();

	spawnExtension(ext_name, parent_window);
}

void ExtensionSpawner::widgetDestroyed(QObject* dock_widget)
{
	QDockWidget* dw = reinterpret_cast<QDockWidget*>(dock_widget);
	destroyExtension(dw);
}
