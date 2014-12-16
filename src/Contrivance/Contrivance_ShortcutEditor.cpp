#include "Contrivance_ShortcutEditor.h"
#include "ui_Contrivance_ShortcutEditor.h"
#include "Contrivance_ContrivanceWindow.h"

ShortcutEditor::ShortcutEditor(ContrivanceWindow& window, QWidget *parent):
	QDialog(parent), _window(window), _ui(new Ui::ShortcutEditor)
{
	_ui->setupUi(this);

	QStringList labels;
	labels << "Action" << "Shortcut";

	_ui->tableWidget->setHorizontalHeaderLabels(labels);

	const QList<Shortcut>& shortcuts = _window.retrieveShortcuts();
	_ui->tableWidget->setRowCount(shortcuts.size());

	for (int i = 0; i < shortcuts.size(); ++i) {
		QTableWidgetItem* action = new QTableWidgetItem(shortcuts[i].action);
		QTableWidgetItem* shortcut = new QTableWidgetItem(shortcuts[i].shortcut);

		action->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		shortcut->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		_ui->tableWidget->setItem(i, 0, action);
		_ui->tableWidget->setItem(i, 1, shortcut);
	}
}

ShortcutEditor::~ShortcutEditor()
{
	delete _ui;
}

void ShortcutEditor::accept(void)
{
	QList<QString> shortcuts;

	for (int i = 0; i < _ui->tableWidget->rowCount(); ++i) {
		QTableWidgetItem* shortcut = _ui->tableWidget->item(i, 1);

		shortcuts.push_back(shortcut->text());
	}

	_window.setNewShortcuts(shortcuts);
	QDialog::accept();
}

void ShortcutEditor::reject(void)
{
	QDialog::reject();
}
