#ifndef SHORTCUTEDITOR_H
#define SHORTCUTEDITOR_H

#include <QDialog>

namespace Ui {
class ShortcutEditor;
}

class ContrivanceWindow;

class ShortcutEditor : public QDialog
{
	Q_OBJECT

public:
	explicit ShortcutEditor(ContrivanceWindow& window, QWidget* parent = 0);
	~ShortcutEditor();

	void refreshShortcuts(void);

private:
	ContrivanceWindow& _window;
	Ui::ShortcutEditor* _ui;

public slots:
	void accept(void);
	void reject(void);
};

#endif // SHORTCUTEDITOR_H
