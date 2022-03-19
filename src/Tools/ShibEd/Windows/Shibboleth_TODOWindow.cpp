/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_TODOWindow.h"
#include <Gaff_Utils.h>
#include <QCoreApplication>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>

TODOWindow::TODOWindow(QWidget* parent):
	QFrame(parent)
{
	QPushButton* const search_button = new QPushButton(tr("Search"), this);
	QLineEdit* const line_edit = new QLineEdit(this);

	QListWidget* const list_widget = new QListWidget(this);

	QGridLayout* const grid_layout = new QGridLayout(this);
	grid_layout->addWidget(line_edit, 0, 0/*, 1, 3*/);
	grid_layout->addWidget(search_button, 0, 3/*, 1, 1*/);
	grid_layout->addWidget(list_widget, 1, 0);

	setLayout(grid_layout);
}

TODOWindow::~TODOWindow()
{
}
