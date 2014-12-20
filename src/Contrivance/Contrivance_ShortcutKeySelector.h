#ifndef SHORTCUTKEYSELECTOR_H
#define SHORTCUTKEYSELECTOR_H

#include <QWidget>

namespace Ui {
class ShortcutKeySelector;
}

class ShortcutKeySelector : public QWidget
{
	Q_OBJECT

public:
	explicit ShortcutKeySelector(QWidget* parent = 0);
	~ShortcutKeySelector();

private:
	Ui::ShortcutKeySelector* _ui;
};

#endif // SHORTCUTKEYSELECTOR_H
