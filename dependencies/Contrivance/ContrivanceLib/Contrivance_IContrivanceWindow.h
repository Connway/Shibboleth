#ifndef CONTRIVANCE_ICONTRIVANCEWINDOW_H
#define CONTRIVANCE_ICONTRIVANCEWINDOW_H

#include "Contrivance_Shortcut.h"
#include "Contrivance_Defines.h"
#include <QKeySequence>

class ExtensionSpawner;
class QMainWindow;
class QWidget;
class QAction;
class QObject;
class QMenu;
class QIcon;

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

#endif // CONTRIVANCE_ICONTRIVANCEWINDOW_H
