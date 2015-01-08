/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include "Contrivance_ContrivanceWindow.h"
#include "ui_Contrivance_ExtensionSpawner.h"
#include <QDockWidget>
#include <QMouseEvent>
#include <QDir>

ExtensionSpawner::ExtensionSpawner(ContrivanceWindow& window, QWidget* parent):
	QWidget(parent), _ui(new Ui::ExtensionSpawner),
	_window(window)
{
	_ui->setupUi(this);
	//_ui->listWidget->installEventFilter(this);

	connect(_ui->listWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleClicked(QModelIndex)));
}

ExtensionSpawner::~ExtensionSpawner()
{
	for (auto it = _extension_modules.begin(); it != _extension_modules.end(); ++it) {
		if (it->shutdown_func) {
			it->shutdown_func();
		}

		delete it->library;
	}

	delete _ui;
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

void ExtensionSpawner::loadExtensions(void)
{
	_window.printToConsole("Loading extensions from modules found with filter 'extensions/*" SHARED_LIBRARY_SUFFIX SHARED_LIBRARY_EXTENSION "'");

	QString nameFilter = "*" SHARED_LIBRARY_SUFFIX SHARED_LIBRARY_EXTENSION;
	QDir directory("./extensions");
	QStringList entryList = directory.entryList(QStringList{nameFilter}, QDir::Files, QDir::Name);

	for (auto it_dll = entryList.begin(); it_dll != entryList.end(); ++it_dll) {
		ExtensionData ext_data;
		ext_data.library = new QLibrary;
		ext_data.library->setFileName(*it_dll);

		if (!ext_data.library->load()) {
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
		if (!ext_data.save_func || !ext_data.load_func || !ext_data.create_func ||
			!ext_data.destroy_func || !ext_data.get_exts_func) {

			QString missing_func;

			if (!ext_data.save_func) {
				missing_func = "SaveInstanceData";
			} else if (!ext_data.load_func) {
				missing_func = "LoadInstanceData";
			} else if (!ext_data.create_func) {
				missing_func = "CreateInstance";
			} else if (!ext_data.destroy_func) {
				missing_func = "DestroyInstance";
			} else if (!ext_data.get_exts_func) {
				missing_func = "GetExtensions";
			}

			_window.printToConsole("'" + *it_dll + "' is missing core function '" + missing_func + "'", CMT_ERROR);
			continue;
		}

		// If we have an init func and it failed
		if (ext_data.init_func && !ext_data.init_func()) {
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

void ExtensionSpawner::itemDoubleClicked(QModelIndex index)
{
	QMainWindow* parent_window = &_window;

	// Spawn on the currently open tab.
	if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
		parent_window = _window.getCurrentTabWindow();
	}

	// create extension
	QDockWidget* dock_widget = new QDockWidget(_ui->listWidget->item(index.row())->text());
	//dock_widget->setWidget(new ExtensionSpawner(_window));

	parent_window->addDockWidget(Qt::RightDockWidgetArea, dock_widget);
	dock_widget->setFloating(true);
	dock_widget->resize(400, 300);

	_window.addSpawnedWindowMenuEntry(dock_widget->toggleViewAction());
}
