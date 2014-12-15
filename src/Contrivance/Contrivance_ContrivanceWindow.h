#ifndef CONTRIVANCEWINDOW_H
#define CONTRIVANCEWINDOW_H

#include <QMainWindow>
#include "Contrivance_Shortcut.h"

namespace Ui {
class ContrivanceWindow;
}

class ShortcutEditor;

class ContrivanceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ContrivanceWindow(QWidget *parent = 0);
    ~ContrivanceWindow();

	void registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QString& shortcut = QString());
	void setNewShortcuts(const QList<QString>& shortcuts); // List should match 1-to-1 with _shortcuts.
	const QList<Shortcut>& retrieveShortcuts(void) const;

private:
	Ui::ContrivanceWindow* _ui;
	ShortcutEditor* _shortcutEditor;

	QList<Shortcut> _shortcuts;

private slots:
	void shortcutEditor(void);
	void aboutQt(void);
	void newTab(void);
    void exit(void);
};

#endif // CONTRIVANCEWINDOW_H
