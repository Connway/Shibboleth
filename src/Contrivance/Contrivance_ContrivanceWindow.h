#ifndef CONTRIVANCEWINDOW_H
#define CONTRIVANCEWINDOW_H

#include "Contrivance_Shortcut.h"
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
		typedef bool (*SaveInstanceDataFunc)(const QString&, QJsonObject&, QWidget*);
		typedef bool (*LoadInstanceDataFunc)(const QString&, const QJsonObject&, QWidget*);
		typedef QWidget* (*CreateInstance)(const QString&);
		typedef void (*DestroyInstance)(const QString&, QWidget*);
		typedef QList<QString> (*GetExtensionsFunc)(void);

		QList<QString> extension_names;
		QLibrary library;
	};

	QList<Shortcut> _shortcuts;

	Ui::ContrivanceWindow* _ui;
	ExtensionSpawner* _extension_spawner;
	ShortcutEditor* _shortcut_editor;
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

private slots:
	void currentTabChanged(int index);
	void tabDoubleClicked(int index);
	void shortcutEditor(void);
	void aboutQt(void);
	void newTab(void);
	void exit(void);
};

#endif // CONTRIVANCEWINDOW_H
