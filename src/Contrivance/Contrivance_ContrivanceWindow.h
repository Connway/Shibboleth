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

#ifndef CONTRIVANCEWINDOW_H
#define CONTRIVANCEWINDOW_H

#include "Contrivance_Shortcut.h"
#include "Contrivance_Defines.h"
#include <QMainWindow>
#include <QTextEdit>
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

class ContrivanceWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ContrivanceWindow(QWidget* parent = nullptr);
	~ContrivanceWindow();

	void registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut = QKeySequence());
	void setNewShortcuts(const QList<QKeySequence>& shortcuts); // List should match 1-to-1 with _shortcuts. Should only really be called by the Shortcut Editor.
	const QList<Shortcut>& retrieveShortcuts(void) const;

	void registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member, const QString& toolbar_name, const QString& group_name);

	void spawnExtension(const QString& extension_name);

	void printToConsole(const QString& message, ConsoleMessageType type = CMT_NORMAL);

private:
	struct ToolbarData
	{
		QHash<QString, QAction*> separators;
		QString lastGroup;
		QToolBar* toolbar;
	};

	struct ExtensionSpawnData
	{
		QString name;
		QWidget* instance;
	};

	struct ExtensionData
	{
		// Widget identifiers are QStrings instead of IDs.
		// I'm expecting this system to not be called very frequently.
		typedef bool (*InitExtensionModuleFunc)(void);
		typedef void (*ShutdownExtensionModuleFunc)(void);
		typedef bool (*SaveInstanceDataFunc)(const QString&, QJsonObject&, QWidget*);
		typedef bool (*LoadInstanceDataFunc)(const QString&, const QJsonObject&, QWidget*);
		typedef QWidget* (*CreateInstanceFunc)(const QString&);
		typedef void (*DestroyInstanceFunc)(const QString&, QWidget*);
		typedef void (*GetExtensionsFunc)(QStringList&);

		QStringList extension_names;
		QLibrary* library;

		InitExtensionModuleFunc init_func;
		ShutdownExtensionModuleFunc shutdown_func;
		SaveInstanceDataFunc save_func;
		LoadInstanceDataFunc load_func;
		CreateInstanceFunc create_func;
		DestroyInstanceFunc destroy_func;
		GetExtensionsFunc get_exts_func;
	};

	QHash<QString, unsigned int> _extension_indices;
	QList<ExtensionData> _extension_modules;
	QList<Shortcut> _shortcuts;

	Ui::ContrivanceWindow* _ui;
	ExtensionSpawner* _extension_spawner;
	ShortcutEditor* _shortcut_editor;
	Console* _console;
	QTextEdit* _tab_renamer;
	int _tab_being_renamed;

	QHash<QString, ToolbarData> _toolbars;

	bool saveShortcuts(const QJsonObject& shortcuts, const QString& file);
	bool loadShortcuts(const QString& file);

	bool eventFilter(QObject* object, QEvent* event);
	bool event(QEvent* event);

	void setupTabRenamer(void);
	void hideTabRenamer(void);

	void setupExtensionSpawner(void);
	void setupConsole(void);

	void loadExtensions(void);

private slots:
	void currentTabChanged(int index);
	void tabDoubleClicked(int index);
	void shortcutEditor(void);
	void aboutQt(void);
	void newTab(void);
	void exit(void);
};

#endif // CONTRIVANCEWINDOW_H
