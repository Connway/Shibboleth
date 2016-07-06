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

#pragma once

#include "Contrivance_Shortcut.h"
#include <QKeySequence>

class QMainWindow;
class QWidget;
class QAction;
class QObject;
class QMenu;
class QIcon;

NS_CONTRIVANCE

class ExtensionSpawner;

class IContrivanceWindow
{
public:
	IContrivanceWindow(void) {}
	virtual ~IContrivanceWindow(void) {}

	virtual void registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut = QKeySequence()) = 0;
	virtual void registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member, const QString& toolbar_name, const QString& group_name) = 0;
	virtual void registerNewMenuAction(QAction* action, const QString& menu_name, const QString& group_name) = 0;
	virtual void registerNewMenuMenu(QMenu* menu, const QString& menu_name, const QString& group_name) = 0;

	virtual void setNewShortcuts(const QVector<QKeySequence>& shortcuts) = 0; // List should match 1-to-1 with _shortcuts. Should only really be called by the Shortcut Editor.
	virtual const QVector<Shortcut>& retrieveShortcuts(void) const = 0;

	virtual void printToConsole(const QString& message, ConsoleMessageType type = CMT_NORMAL) = 0;

	virtual ExtensionSpawner* getExtensionSpawner(void) = 0;
	virtual QMainWindow* getCurrentTabWindow(void) = 0;
	virtual QMainWindow* getThisWindow(void) = 0;

	virtual void addSpawnedWindowMenuEntry(QAction* spawn_window_action) = 0;
	virtual void removeSpawnedWindowMenuEntry(QAction* spawn_window_action) = 0;
};

NS_END
