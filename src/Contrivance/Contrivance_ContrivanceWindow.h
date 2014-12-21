#ifndef CONTRIVANCEWINDOW_H
#define CONTRIVANCEWINDOW_H

#include "Contrivance_Shortcut.h"
#include <QMainWindow>
#include <QTextEdit>

namespace Ui {
class ContrivanceWindow;
}

class ShortcutEditor;
class QJsonObject;

class ContrivanceWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ContrivanceWindow(QWidget *parent = 0);
	~ContrivanceWindow();

	void registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut = QKeySequence());
	void setNewShortcuts(const QList<QKeySequence>& shortcuts); // List should match 1-to-1 with _shortcuts.
	const QList<Shortcut>& retrieveShortcuts(void) const;

	void registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member);

private:
	QList<Shortcut> _shortcuts;

	Ui::ContrivanceWindow* _ui;
	ShortcutEditor* _shortcutEditor;
	QTextEdit* _tab_renamer;
	int _tab_being_renamed;

	bool saveShortcuts(const QJsonObject& shortcuts, const QString& file);
	bool loadShortcuts(const QString& file);

	bool eventFilter(QObject* object, QEvent* event);
	bool event(QEvent* event);

	void hideTabRenamer(void);

private slots:
	void currentTabChanged(int index);
	void tabDoubleClicked(int index);
	void shortcutEditor(void);
	void aboutQt(void);
	void newTab(void);
	void exit(void);
};

#endif // CONTRIVANCEWINDOW_H
