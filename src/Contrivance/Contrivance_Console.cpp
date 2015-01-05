/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Contrivance_Console.h"
#include "ui_Contrivance_Console.h"

Console::Console(ContrivanceWindow& window, QWidget *parent):
	QWidget(parent), _ui(new Ui::Console), _window(window),
	_maximum_log_entries(100000)
{
	_ui->setupUi(this);

	connect(_ui->consoleInput, SIGNAL(returnPressed()), this, SLOT(handleCommand()));
}

Console::~Console()
{
	delete _ui;
}

void Console::print(const QString& message, ConsoleMessageType type)
{
	QColor color(0, 0, 0);

	switch (type) {
		case CMT_WARNING:
			color.setRgb(255, 127, 0);
			addOutput(message, color, _ui->warningOutput);
			break;

		case CMT_ERROR:
			color.setRgb(255, 0, 0);
			addOutput(message, color, _ui->errorOutput);
			break;

		case CMT_NORMAL:
		default:
			break;
	}

	addOutput(message, color, _ui->allOutput);
}

void Console::addOutput(const QString& message, const QColor& color, QListWidget* list)
{
	if (list->count() == _maximum_log_entries) {
		delete list->item(0);
	}

	list->addItem(message);
	list->item(list->count() - 1)->setTextColor(color);
	list->scrollToBottom();
}

void Console::handleCommand(void)
{
	QString command = _ui->consoleInput->text();
	_ui->consoleInput->clear();

	print("\nHandling command. Actually implement this.", CMT_NORMAL);
	print(command, CMT_NORMAL);

	// implement command registration system
}
