#include "Contrivance_ShortcutEditor.h"
#include "ui_Contrivance_ShortcutEditor.h"
#include "Contrivance_ShortcutKeySelector.h"
#include "Contrivance_ContrivanceWindow.h"
#include <QPushButton>

ShortcutEditor::ShortcutEditor(ContrivanceWindow& window, QWidget* parent):
	QDialog(parent), _window(window), _ui(new Ui::ShortcutEditor)
{
	_ui->setupUi(this);

	QStringList labels;
	labels << "Action" << "Shortcut";

	_ui->tableWidget->setHorizontalHeaderLabels(labels);
	_ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	const QList<Shortcut>& shortcuts = _window.retrieveShortcuts();
	_ui->tableWidget->setRowCount(shortcuts.size());

	for (int i = 0; i < shortcuts.size(); ++i) {
		QTableWidgetItem* action = new QTableWidgetItem(shortcuts[i].action);

		action->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		action->setFlags(Qt::NoItemFlags);

		_ui->tableWidget->setItem(i, 0, action);

		ShortcutKeySelector* shortcut = new ShortcutKeySelector(_ui->tableWidget);

		_ui->tableWidget->setCellWidget(i, 1, shortcut);
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
		//QTableWidgetItem* shortcut = _ui->tableWidget->item(i, 1);
		//shortcuts.push_back(shortcut->text());

		ShortcutKeySelector* selector = (ShortcutKeySelector*)_ui->tableWidget->cellWidget(i, 1);
	}

	_window.setNewShortcuts(shortcuts);
	QDialog::accept();
}

void ShortcutEditor::reject(void)
{
	QDialog::reject();
}
