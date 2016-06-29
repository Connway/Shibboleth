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

#include "Contrivance_ShortcutEditor.h"
#include "ui_Contrivance_ShortcutEditor.h"
#include "Contrivance_IContrivanceWindow.h"

#include <QKeySequenceEdit>

ShortcutEditor::ShortcutEditor(IContrivanceWindow& window, QWidget* parent):
	QDialog(parent), _window(window), _ui(new Ui::ShortcutEditor)
{
	_ui->setupUi(this);

	QStringList labels;
	labels << "Action" << "Shortcut";

	_ui->tableWidget->setHorizontalHeaderLabels(labels);
	_ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

ShortcutEditor::~ShortcutEditor()
{
	delete _ui;
}

void ShortcutEditor::refreshShortcuts(void)
{
	// Populate our table view with the action/key sequence pairs.
	const QVector<Shortcut>& shortcuts = _window.retrieveShortcuts();
	_ui->tableWidget->setRowCount(shortcuts.size());

	for (int i = 0; i < shortcuts.size(); ++i) {
		QTableWidgetItem* action = new QTableWidgetItem(shortcuts[i].action);

		action->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		action->setFlags(Qt::NoItemFlags);

		_ui->tableWidget->setItem(i, 0, action);

		QKeySequenceEdit* shortcut = new QKeySequenceEdit(shortcuts[i].shortcut, _ui->tableWidget);
		_ui->tableWidget->setCellWidget(i, 1, shortcut);
	}
}

void ShortcutEditor::accept(void)
{
	// Tell the main application about or new shortcuts.
	QVector<QKeySequence> shortcuts;
	shortcuts.reserve(_ui->tableWidget->rowCount());

	for (int i = 0; i < _ui->tableWidget->rowCount(); ++i) {
		QKeySequenceEdit* selector = (QKeySequenceEdit*)_ui->tableWidget->cellWidget(i, 1);
		shortcuts.push_back(selector->keySequence());
	}

	_window.setNewShortcuts(shortcuts);
	QDialog::accept();
}

void ShortcutEditor::reject(void)
{
	QDialog::reject();
}
