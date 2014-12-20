#include "Contrivance_ShortcutKeySelector.h"
#include "ui_Contrivance_ShortcutKeySelector.h"

ShortcutKeySelector::ShortcutKeySelector(QWidget* parent):
	QWidget(parent), _ui(new Ui::ShortcutKeySelector)
{
	_ui->setupUi(this);
}

ShortcutKeySelector::~ShortcutKeySelector()
{
	delete ui;
}
