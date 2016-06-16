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

#include "Contrivance_ContrivanceWindow.h"
#include "Contrivance_ExtensionSpawner.h"
#include "Contrivance_ShortcutEditor.h"
#include "Contrivance_Console.h"
#include "ui_Contrivance_ContrivanceWindow.h"
#include <QJsonDocument>
#include <QTextStream>
#include <QJsonObject>
#include <QMouseEvent>
#include <QDockWidget>
#include <QPushButton>
#include <QJsonArray>
#include <QLineEdit>
#include <QShortcut>
#include <QLabel>
#include <QTimer>
#include <QFile>

ContrivanceWindow::ContrivanceWindow(QWidget* parent):
	QMainWindow(parent), _ui(new Ui::ContrivanceWindow),
	_shortcut_editor(nullptr), _tab_renamer(nullptr),
	_tab_being_renamed(0), _active_dock_widget(nullptr)
{
	_ui->setupUi(this);

	setupTabRenamer();

	// Register the main toolbar with the toolbar management system
	_toolbars["main"].toolbar = _ui->mainToolBar;

	setupMenus();

	// Register the new tab keyboard shortcut and toolbar button. Also create the Shortcut Editor modal widget.
	registerNewShortcut(this, SLOT(newTab()), tr("New Tab"), QKeySequence(Qt::CTRL + Qt::Key_N));
	registerNewShortcut(this, SLOT(swapDockParent()), tr("Swap Dock Parent"), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L));

	_shortcut_editor = new ShortcutEditor(*this);

	registerNewToolbarAction(QIcon("icons/tab_add.png"), this, SLOT(newTab()), "main", "group_a");

	setupConsole();
	setupExtensionSpawner();
	loadShortcuts("keyboard_shortcuts.json");

	// if we have a saved config file with the editor's spawned extensions and layout, use that to populate the editor, otherwise, use defaults.
	addDockWidget(Qt::RightDockWidgetArea, qobject_cast<QDockWidget*>(_extension_spawner->parentWidget()));
	addDockWidget(Qt::BottomDockWidgetArea, qobject_cast<QDockWidget*>(_console->parentWidget()));

	connect(_ui->tabWidget, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(tabDoubleClicked(int)));
	connect(_ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	newTab(); // Start with one tab by default

	qApp->installEventFilter(this);

	// Re-create plugins

	restoreSaveWindowState(false);

	_ui->menuSpawned_Windows->setEnabled(!_ui->menuSpawned_Windows->actions().isEmpty());
}

ContrivanceWindow::~ContrivanceWindow()
{
	delete _ui;
}

void ContrivanceWindow::registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut)
{
	// Check if there is another shortuct using this key sequence.
	for (auto it = _shortcuts.begin(); it != _shortcuts.end(); ++it) {
		if (!it->shortcut.isEmpty() && it->shortcut == shortcut) {
			// Use empty key sequence if another shortcut is already using it.
			printToConsole(
				"Shortcut for action '" + action + "' with key sequence '" +
				shortcut.toString() + "' is already in use by action '" + it->action + "'",
				CMT_WARNING
			);

			_shortcuts.push_back(Shortcut(parent, member, action, QKeySequence()));
			return;
		}
	}

	if (shortcut.isEmpty()) {
		_shortcuts.push_back(Shortcut(parent, member, action, shortcut));
	} else {
		Shortcut data(parent, member, action, shortcut);
		data.instance = new QShortcut(shortcut, parent, member);
		_shortcuts.push_back(data);
	}
}

void ContrivanceWindow::setNewShortcuts(const QVector<QKeySequence>& shortcuts)
{
	printToConsole("Setting new shortcuts");

	QJsonObject root_obj;

	// Add the new shortcuts and construct the JSON object.
	for (int i = 0; i < shortcuts.count(); ++i) {
		QKeySequence key_sequence = shortcuts[i];

		// Check for duplicates
		for (int j = 0; j < i; ++j) {
			// Use empty key sequence if another shortcut is already using it.
			if (!shortcuts[j].isEmpty() && shortcuts[i] == shortcuts[j]) {
				printToConsole(
					"Shortcut for action '" + _shortcuts[j].action + "' with key sequence '" +
					key_sequence.toString() + "' is already in use by action '" + _shortcuts[i].action + "'",
					CMT_WARNING
				);

				key_sequence = QKeySequence();
				break;
			}

		}

		root_obj.insert(_shortcuts[i].action, QJsonValue(key_sequence.toString()));
		_shortcuts[i].instance->setKey(key_sequence);
		_shortcuts[i].shortcut = key_sequence;
	}

	saveShortcuts(root_obj, "keyboard_shortcuts.json");
}

const QVector<Shortcut>& ContrivanceWindow::retrieveShortcuts(void) const
{
	return _shortcuts;
}

void ContrivanceWindow::registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member, const QString& toolbar_name, const QString& group_name)
{
	// See if the toolbar we are adding ourselves to is already registered.
	auto it_tb = _toolbars.find(toolbar_name);

	// If not, add a new toolbar.
	if (it_tb == _toolbars.end()) {
		QToolBar* toolbar = new QToolBar(this);
		toolbar->setAllowedAreas(Qt::AllToolBarAreas);
		toolbar->setObjectName(toolbar_name);
		toolbar->setFloatable(true);
		toolbar->setMovable(true);
		addToolBar(Qt::TopToolBarArea, toolbar);

		ToolbarData data;
		data.toolbar = toolbar;

		it_tb = _toolbars.insert(toolbar_name, data);
	}

	// Check that toolbar's separator groups for the group we want to be added to.
	auto it_sep = it_tb->separators.find(group_name);

	// If it doesn't exist, add the separator we made for the previous group to the toolbar
	// and make a new separator for our group, but don't add it to the toolbar.
	if (it_sep == it_tb->separators.end()) {
		QAction* separator = it_tb->toolbar->addSeparator();
		it_tb->toolbar->removeAction(separator);

		it_sep = it_tb->separators.insert(group_name, separator);

		if (it_tb->lastSeparator) {
			it_tb->toolbar->addAction(it_tb->lastSeparator);
		}

		it_tb->lastSeparator = separator;
	}

	// Add ourselves to the toolbar.
	QAction* action = new QAction(icon, "", this);
	connect(action, SIGNAL(triggered()), receiver, member);

	it_tb->toolbar->insertAction(*it_sep, action);
}

void ContrivanceWindow::registerNewMenuAction(QAction* action, const QString& menu_name, const QString& group_name)
{
	// See if the toolbar we are adding ourselves to is already registered.
	auto it_m = _menus.find(menu_name);

	// If not, add a new toolbar.
	if (it_m == _menus.end()) {
		QMenu* menu = new QMenu(menu_name, this);
		MenuData data;
		data.menu = menu;

		it_m = _menus.insert(menu_name, data);
	}

	// Check that menu's separator groups for the group we want to be added to.
	auto it_sep = it_m->separators.find(group_name);

	// If it doesn't exist, add the separator we made for the previous group to the toolbar
	// and make a new separator for our group, but don't add it to the toolbar.
	if (it_sep == it_m->separators.end()) {
		QAction* separator = it_m->menu->addSeparator();
		it_m->menu->removeAction(separator);

		it_sep = it_m->separators.insert(group_name, separator);

		// If we have an insertBefore, always insert our separator
		if (it_m->insertBefore) {
			it_m->menu->insertAction(it_m->insertBefore, separator);
		} else if (it_m->lastSeparator) {
			it_m->menu->addAction(it_m->lastSeparator);
		}

		it_m->lastSeparator = separator;
	}

	it_m->menu->insertAction(*it_sep, action);
}

void ContrivanceWindow::registerNewMenuMenu(QMenu* menu, const QString& menu_name, const QString& group_name)
{
	// See if the toolbar we are adding ourselves to is already registered.
	auto it_m = _menus.find(menu_name);

	// If not, add a new toolbar.
	if (it_m == _menus.end()) {
		QMenu* menu = new QMenu(menu_name, this);
		MenuData data;
		data.menu = menu;

		it_m = _menus.insert(menu_name, data);
	}

	// Check that menu's separator groups for the group we want to be added to.
	auto it_sep = it_m->separators.find(group_name);

	// If it doesn't exist, add the separator we made for the previous group to the toolbar
	// and make a new separator for our group, but don't add it to the toolbar.
	if (it_sep == it_m->separators.end()) {
		QAction* separator = it_m->menu->addSeparator();
		it_m->menu->removeAction(separator);

		it_sep = it_m->separators.insert(group_name, separator);

		// If we have an insertBefore, always insert our separator
		if (it_m->insertBefore) {
			it_m->menu->insertAction(it_m->insertBefore, separator);
		} else if (it_m->lastSeparator) {
			it_m->menu->addAction(it_m->lastSeparator);
		}

		it_m->lastSeparator = separator;
	}

	it_m->menu->insertMenu(*it_sep, menu);
}

void ContrivanceWindow::printToConsole(const QString& message, ConsoleMessageType type)
{
	_console->print(message, type);
}

ExtensionSpawner* ContrivanceWindow::getExtensionSpawner(void)
{
	return _extension_spawner;
}

QMainWindow* ContrivanceWindow::getCurrentTabWindow(void)
{
	return qobject_cast<QMainWindow*>(_ui->tabWidget->currentWidget());
}

void ContrivanceWindow::addSpawnedWindowMenuEntry(QAction* spawn_window_action)
{
	_ui->menuSpawned_Windows->setEnabled(true);
	_ui->menuSpawned_Windows->addAction(spawn_window_action);
}

void ContrivanceWindow::removeSpawnedWindowMenuEntry(QAction* spawn_window_action)
{
	auto& actions = _ui->menuSpawned_Windows->actions();
	int index = actions.indexOf(spawn_window_action);

	if (index > -1) {
		actions.removeAt(index);
	}

	_ui->menuSpawned_Windows->setEnabled(!actions.isEmpty());
}

bool ContrivanceWindow::saveShortcuts(const QJsonObject& shortcuts, const QString& file)
{
	QFile config(file);

	if (config.open(QFile::WriteOnly | QFile::Text)) {
		QJsonDocument json_doc(shortcuts);
		return config.write(json_doc.toJson()) > 0;
	}

	return false;
}

bool ContrivanceWindow::loadShortcuts(const QString& file)
{
	printToConsole("Loading shortcuts from '" + file + "'");

	QFile config(file);

	if (config.open(QFile::ReadOnly | QFile::Text)) {
		QJsonDocument json_doc = QJsonDocument::fromJson(config.readAll());
		QJsonObject root_obj = json_doc.object();

		// Find the associated action and set it's shortcut to the value from the JSON file.
		for (auto it = _shortcuts.begin(); it != _shortcuts.end(); ++it) {
			auto value = root_obj.find(it->action);

			if (value != root_obj.end()) {
				it->shortcut = QKeySequence::fromString((*value).toString());
				it->instance->setKey(it->shortcut);
			}
		}

		return true;
	}

	return false;
}

#include <QWindow>

bool ContrivanceWindow::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::FocusIn) {
		QDockWidget* old_dock_widget = _active_dock_widget;
		_active_dock_widget = nullptr;

		findDockWidget(object);
		//printToConsole("Focus: " + object->objectName());

		QWindow* window = qobject_cast<QWindow*>(object);
		if (window) {
			//printToConsole("Foo");
		}

		if (!_active_dock_widget) {
			_active_dock_widget = old_dock_widget;
		} //else {
			//printToConsole("New Dock Widget: " + _active_dock_widget->objectName());
		//}
	}

	if (_tab_renamer->isVisible()) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent* key_event = reinterpret_cast<QKeyEvent*>(event);

			if ((key_event->key() == Qt::Key_Enter) || (key_event->key() == Qt::Key_Return)) {
				hideTabRenamer();
				return true;
			}
		} else if (event->type() == QEvent::MouseButtonPress) {
			QPoint cursor_pos = _tab_renamer->mapFromGlobal(QCursor::pos());

			if (!_tab_renamer->rect().contains(cursor_pos)) {
				hideTabRenamer();
			}
		}
	}

	return QMainWindow::eventFilter(object, event);
}

void ContrivanceWindow::closeEvent(QCloseEvent*)
{
	exit();
}

void ContrivanceWindow::setupTabRenamer(void)
{
	// Create the Tab Renamer widget
	_tab_renamer = new QLineEdit(this);
	_tab_renamer->setEnabled(false);
	_tab_renamer->hide();
}

void ContrivanceWindow::hideTabRenamer(void)
{
	_ui->tabWidget->setTabText(_tab_being_renamed, _tab_renamer->text());
	_ui->tabWidget->widget(_tab_being_renamed)->setObjectName(_tab_renamer->text());
	_tab_renamer->setEnabled(false);
	_tab_renamer->hide();
}

void ContrivanceWindow::setupExtensionSpawner(void)
{
	// Create the initial dock widget with the Extension Spawner.
	QDockWidget* dw = new QDockWidget(tr("Extension Spawner"));
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	dw->setSizePolicy(sizePolicy);
	dw->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dw->setAllowedAreas(Qt::AllDockWidgetAreas);
	dw->setObjectName("Extension Spawner Dock");

	_extension_spawner = new ExtensionSpawner(*this, dw);
	dw->setWidget(_extension_spawner);

	// Add the window hide checkbox for the Extension Spawner to the "Window" menu.
	_ui->menu_Window->addAction(dw->toggleViewAction());
	_ui->menuSpawned_Windows->setEnabled(!_ui->menuSpawned_Windows->children().empty());
	_extension_spawner->loadExtensions();
}

void ContrivanceWindow::setupConsole(void)
{
	// Create the initial dock widget with the Extension Spawner.
	QDockWidget* dw = new QDockWidget(tr("Console"));
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	dw->setSizePolicy(sizePolicy);
	dw->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dw->setAllowedAreas(Qt::AllDockWidgetAreas);
	dw->setObjectName("Console Dock");

	_console = new Console(*this, dw);
	dw->setWidget(_console);

	// Add the window hide checkbox for the Extension Spawner to the "Window" menu.
	_ui->menu_Window->addAction(dw->toggleViewAction());

	_ui->menuSpawned_Windows->setEnabled(false);
}

void ContrivanceWindow::setupMenus(void)
{
	// Register the menus with the menu management system.
	_menus["&File"].menu = _ui->menu_File;
	_menus["&File"].insertBefore = _ui->actionE_xit;
	_menus["&Edit"].menu = _ui->menu_Edit;
	_menus["&Window"].menu = _ui->menu_Window;
	_menus["&Help"].menu = _ui->menu_Help;
	_menus["&Help"].insertBefore = _ui->actionAbout_Qt;

	// Connect menu actions and tab renaming actions.
	connect(_ui->action_Shortcut_Editor, SIGNAL(triggered()), this, SLOT(shortcutEditor()));
	connect(_ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect(_ui->actionE_xit, SIGNAL(triggered()), this, SLOT(exit()));

	// Remove some of the built-in UI.
	_ui->menu_File->removeAction(_ui->action_New);
	_ui->menu_File->removeAction(_ui->action_Open);

	_ui->menu_Edit->removeAction(_ui->action_Shortcut_Editor);

	// Re-add them back so the system knows about them.
	registerNewMenuAction(_ui->action_New, "&File", "Docs");
	registerNewMenuAction(_ui->action_Open, "&File", "Docs");

	registerNewMenuAction(_ui->action_Shortcut_Editor, "&Edit", "Shortcuts");
}

bool ContrivanceWindow::findDockWidgetParent(QObject* object)
{
	if (!object) {
		return false;
	}

	_active_dock_widget = qobject_cast<QDockWidget*>(object);

	if (!_active_dock_widget) {
		return findDockWidgetParent(object->parent());
	}

	return _active_dock_widget != nullptr;
}

bool ContrivanceWindow::findDockWidget(QObject* object)
{
	if (!object) {
		return false;
	}

	_active_dock_widget = qobject_cast<QDockWidget*>(object);

	if (!_active_dock_widget) {
		if (findDockWidgetParent(object->parent())) {
			return true;
		}

		_active_dock_widget = object->findChild<QDockWidget*>();
	}

	return _active_dock_widget != nullptr;
}

void ContrivanceWindow::restoreSaveWindowState(bool save)
{
	QIODevice::OpenModeFlag open_flag = (save) ? QFile::WriteOnly : QFile::ReadOnly;
	QFile editor_state("editor.state");

	if (!editor_state.open(open_flag)) {
		// log error
		return;
	}

	QDataStream stream(&editor_state);

	if (save) {
		// Write JSON data.
		QJsonObject root_obj;
		QJsonArray mw_extensions;
		QJsonObject mw_state;

		mw_state.insert("State", QJsonValue(windowState()));
		mw_state.insert("Left", QJsonValue(pos().x()));
		mw_state.insert("Top", QJsonValue(pos().y()));
		mw_state.insert("Width", QJsonValue(frameSize().width()));
		mw_state.insert("Height", QJsonValue(frameSize().height()));

		_extension_spawner->saveGlobalData(root_obj);
		_extension_spawner->saveOpenExtensions(this, mw_extensions);

		root_obj.insert("Main Window Extensions", mw_extensions);
		root_obj.insert("Main Window State", mw_state);

		QJsonArray tab_data;

		for (int i = 0; i < _ui->tabWidget->count(); ++i) {
			QMainWindow* window = qobject_cast<QMainWindow*>(_ui->tabWidget->widget(i));
			QJsonArray extensions;
			QJsonObject tab_info;

			_extension_spawner->saveOpenExtensions(window, extensions);

			tab_info.insert("Name", _ui->tabWidget->tabText(i));
			tab_info.insert("Extensions", extensions);
			tab_data.push_back(tab_info);
		}

		root_obj.insert("Tabs", tab_data);

		QByteArray data = QJsonDocument(root_obj).toJson();
		stream << data.size();
		stream.writeRawData(data.data(), data.size());

		// Write main window state.
		data = saveState();
		stream << data.size();
		stream.writeRawData(data.data(), data.size());

		// Write tab window state.
		for (int i = 0; i < _ui->tabWidget->count(); ++i) {
			QMainWindow* window = qobject_cast<QMainWindow*>(_ui->tabWidget->widget(i));

			data = window->saveState();
			stream << data.size();
			stream.writeRawData(data.data(), data.size());
		}

	} else {
		QByteArray data;
		int size = 0;

		// Read JSON data.
		stream >> size;
		data.resize(size);
		stream.readRawData(data.data(), size);

		QJsonObject root_obj = QJsonDocument::fromJson(data).object();
		_extension_spawner->loadGlobalData(root_obj);
		_extension_spawner->loadOpenExtensions(this, root_obj["Main Window Extensions"].toArray());

		QJsonObject mw_state = root_obj["Main Window State"].toObject();
		QJsonArray tab_data = root_obj["Tabs"].toArray();

		_left = mw_state["Left"].toInt();
		_top = mw_state["Top"].toInt();
		_width = mw_state["Width"].toInt();
		_height = mw_state["Height"].toInt();
		_state = Qt::WindowStates(mw_state["State"].toInt());

		for (int i = 0; i < tab_data.count(); ++i) {
			QJsonObject td = tab_data[i].toObject();

			if (i) {
				newTab();
			}

			_ui->tabWidget->setTabText(i, td.value("Name").toString());

			QMainWindow* window = qobject_cast<QMainWindow*>(_ui->tabWidget->widget(i));
			_extension_spawner->loadOpenExtensions(window, td.value("Extensions").toArray());
		}

		// Read main window state.
		stream >> size;
		data.resize(size);
		stream.readRawData(data.data(), size);

		restoreState(data);

		// Read tab window states.
		for (int i = 0; i < tab_data.count(); ++i) {
			stream >> size;
			data.resize(size);
			stream.readRawData(data.data(), size);

			_ui->tabWidget->setCurrentIndex(i);
			QMainWindow* window = qobject_cast<QMainWindow*>(_ui->tabWidget->currentWidget());
			window->restoreState(data);
		}

		// For some reason multiple window state changes occur between now
		// and next frame. So queue our window state restore for next frame-ish.
		QTimer::singleShot(1, this, SLOT(windowStateRestore()));
	}

	editor_state.close();
}

void ContrivanceWindow::tabDoubleClicked(int index)
{
	// Show the tab renamer widget at the location of the tab
	QPoint pos = _ui->centralWidget->mapToParent(QPoint(0, 0)); // Get our position in the window
	// Add the tab widget and tab bar offsets
	pos += _ui->tabWidget->mapToParent(QPoint(0, 0));
	pos += _ui->tabWidget->tabBar()->mapToParent(QPoint(0, 0));

	QRect rect = _ui->tabWidget->tabBar()->tabRect(index);
	_tab_renamer->setText(_ui->tabWidget->tabText(index));
	_tab_renamer->move(rect.topLeft() + pos); // Add relative tab position to our window position
	_tab_renamer->resize(rect.size());
	_tab_renamer->setEnabled(true);
	_tab_renamer->selectAll();
	_tab_renamer->setFocus();
	_tab_renamer->show();

	_tab_being_renamed = index;
}

void ContrivanceWindow::shortcutEditor(void)
{
	if (_shortcut_editor->isHidden()) {
		_shortcut_editor->refreshShortcuts();
		_shortcut_editor->show();
	}
}

void ContrivanceWindow::swapDockParent(void)
{
	if (_active_dock_widget) {
		QMainWindow* parent_window = qobject_cast<QMainWindow*>(_active_dock_widget->parent());

		if (parent_window) {
			QMainWindow* new_window = (parent_window == this) ? getCurrentTabWindow() : this;
			bool isFloating = _active_dock_widget->isFloating();

			_active_dock_widget->setFloating(false);
			parent_window->removeDockWidget(_active_dock_widget);
			new_window->addDockWidget(Qt::RightDockWidgetArea, _active_dock_widget);

			if (isFloating) {
				_active_dock_widget->setFloating(true);
			}

			_active_dock_widget->show();
		}
	}
}
void ContrivanceWindow::closeTab(int index)
{
	int curr_index = _ui->tabWidget->currentIndex();
	_ui->tabWidget->setCurrentIndex(index);

	QWidget* page = _ui->tabWidget->currentWidget();
	_ui->tabWidget->setCurrentIndex(curr_index);

	_ui->tabWidget->removeTab(index);
	delete page;
}

void ContrivanceWindow::aboutQt(void)
{
	qApp->aboutQt();
}

void ContrivanceWindow::newTab(void)
{
	QString tab_text;
	QTextStream text_stream(&tab_text);
	text_stream << tr("Tab") << " " << _ui->tabWidget->count() + 1;

	QMainWindow* tab_window = new QMainWindow;
	tab_window->setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AnimatedDocks | QMainWindow::GroupedDragging | QMainWindow::AllowNestedDocks);
	tab_window->setDockNestingEnabled(true);
	tab_window->setDocumentMode(true);
	tab_window->setAnimated(true);
	tab_window->setObjectName(tab_text + " Window");

	_ui->tabWidget->addTab(tab_window, tab_text);
}

void ContrivanceWindow::exit(void)
{
	restoreSaveWindowState(true);
	_extension_spawner->close();
	qApp->exit();
}

void ContrivanceWindow::windowStateRestore(void)
{
	setWindowState(_state);
	move(_left, _top);
	resize(_width, _height);
}
