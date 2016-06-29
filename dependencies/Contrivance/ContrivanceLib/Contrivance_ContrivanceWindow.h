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

#ifndef CONTRIVANCEWINDOW_H
#define CONTRIVANCEWINDOW_H

#include "contrivance_IContrivanceWindow.h"
#include "Contrivance_Shortcut.h"
#include "Contrivance_Defines.h"
#include <QMainWindow>
#include <QLibrary>
#include <QHash>

namespace Ui
{
	class ContrivanceWindow;
}

class ExtensionSpawner;
class ShortcutEditor;
class Console;
class QJsonObject;
class QLineEdit;

class ContrivanceWindow : public QMainWindow, public IContrivanceWindow
{
	Q_OBJECT

public:
	explicit ContrivanceWindow(QWidget* parent = nullptr);
	~ContrivanceWindow();

	void registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut = QKeySequence());
	void registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member, const QString& toolbar_name, const QString& group_name);
	void registerNewMenuAction(QAction* action, const QString& menu_name, const QString& group_name);
	void registerNewMenuMenu(QMenu* menu, const QString& menu_name, const QString& group_name);

	void setNewShortcuts(const QVector<QKeySequence>& shortcuts); // List should match 1-to-1 with _shortcuts. Should only really be called by the Shortcut Editor.
	const QVector<Shortcut>& retrieveShortcuts(void) const;

	void printToConsole(const QString& message, ConsoleMessageType type = CMT_NORMAL);

	ExtensionSpawner* getExtensionSpawner(void);
	QMainWindow* getCurrentTabWindow(void);
	QMainWindow* getThisWindow(void);

	void addSpawnedWindowMenuEntry(QAction* spawn_window_action);
	void removeSpawnedWindowMenuEntry(QAction* spawn_window_action);

private:
	struct ToolbarData
	{
		QHash<QString, QAction*> separators;
		QAction* lastSeparator = nullptr;
		QToolBar* toolbar = nullptr;
	};

	struct MenuData
	{
		QHash<QString, QAction*> separators;
		QAction* lastSeparator = nullptr;
		QAction* insertBefore = nullptr;
		QMenu* menu = nullptr;
	};

	QHash<QString, ToolbarData> _toolbars;
	QHash<QString, MenuData> _menus;
	QVector<Shortcut> _shortcuts;

	Ui::ContrivanceWindow* _ui;
	ExtensionSpawner* _extension_spawner;
	ShortcutEditor* _shortcut_editor;
	Console* _console;
	QLineEdit* _tab_renamer;
	int _tab_being_renamed;
	QDockWidget* _active_dock_widget;

	int _left;
	int _top;
	int _width;
	int _height;
	Qt::WindowStates _state;

	bool saveShortcuts(const QJsonObject& shortcuts, const QString& file);
	bool loadShortcuts(const QString& file);

	bool eventFilter(QObject* object, QEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

	void setupTabRenamer(void);
	void hideTabRenamer(void);

	void setupExtensionSpawner(void);
	void setupConsole(void);
	void setupMenus(void);

	bool findDockWidgetParent(QObject* object);
	bool findDockWidget(QObject* object);

	void restoreSaveWindowState(bool save);

private slots:
	void tabDoubleClicked(int index);
	void shortcutEditor(void);
	void swapDockParent(void);
	void closeTab(int index);
	void aboutQt(void);
	void newTab(void);
	void exit(void);

	void windowStateRestore(void);
};

#endif // CONTRIVANCEWINDOW_H
