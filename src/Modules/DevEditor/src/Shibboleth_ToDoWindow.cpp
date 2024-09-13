/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_ToDoWindow.h"
#include "Shibboleth_EditorAttributesCommon.h"
#include "Shibboleth_QtReflection.h"
#include <Shibboleth_AppConfigs.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <QCoreApplication>
#include <QDirIterator>
#include <QTableWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <filesystem>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ToDoWindow)
	.classAttrs(Shibboleth::EditorWindowAttribute())

	.BASE(QWidget)

	.CTOR(QWidget*)
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ToDoWindow)


NS_SHIBBOLETH

SearchThread::SearchThread(QObject* parent):
	QThread(parent)
{
}

SearchThread::~SearchThread(void)
{
	cancel();
	wait();
}

bool SearchThread::search(const QString& dir, const QString& filters)
{
	if (isRunning()) {
		return false;
	}

	if (!QDir(dir).exists()) {
		// $TODO: Log error.
		return false;
	}

	const QStringList string_filters = filters.split(';');
	_directory = dir;

	if (string_filters.isEmpty()) {
		// $TODO: Log error.
		return false;
	}

	_filters.resize(string_filters.size());

	for (qsizetype i = 0; i < string_filters.size(); ++i) {
		const qsizetype extension_index = string_filters[i].lastIndexOf('.');
		const qsizetype folder_index = string_filters[i].indexOf('*');

		if (folder_index != -1 && extension_index != -1) {
			_filters[i].directory = string_filters[i].first(folder_index);
			_filters[i].extension = string_filters[i].last(extension_index);

		} else if (extension_index != -1) {
			_filters[i].extension = string_filters[i];

		} else {
			// $TODO: Log error.
		}
	}

	start(LowPriority);
	return true;
}

void SearchThread::cancel(void)
{
	QMutexLocker lock(&_lock);
	_cancel = true;
}

void SearchThread::run(void)
{
	QDirIterator dir_it(_directory, QDir::Filter::Files, QDirIterator::IteratorFlag::Subdirectories);

	while (dir_it.hasNext()) {
		// Check if we're canceling the search.
		{
			QMutexLocker lock(&_lock);

			if (_cancel) {
				_cancel = false;
				break;
			}
		}

		const QString file_name = dir_it.next();
		bool passes_filters = false;

		for (const Filter& filter : _filters) {
			if (!file_name.endsWith(filter.extension)) {
				continue;
			}

			if (!filter.directory.isEmpty() && file_name.indexOf(filter.directory) != 0) {
				continue;
			}

			passes_filters = true;
			break;
		}

		if (!passes_filters) {
			continue;
		}

		QFile file(file_name);

		if (!file.open(QIODevice::ReadOnly)) {
			// $TODO: Log error.
			continue;
		}

		const QString file_text = file.readAll().trimmed();
		const QStringList lines = file_text.split('\n');
		size_t line_num = 0;

		for (const QString& line : lines) {
			++line_num;

			if (line.contains("$TODO")) {
				// Skip the above line.
				if (file_name.contains("Shibboleth_ToDoWindow.cpp") && line_num == (__LINE__ - 2)) {
					continue;
				}

				emit resultFound(QDir(file_name).absolutePath(), line_num, line.trimmed());
			}
		}
	}

	emit finished();
}



ToDoWindow::ToDoWindow(QWidget* parent):
	QFrame(parent)
{
	_search_button = new QPushButton(tr("Find"), this);
	_filters = new QLineEdit(".h;.cpp;.lua", this);

	_filters->setPlaceholderText(tr("Search Filter, separated by ';' (eg. .h;.cpp;Resources/*.lua"));

	_results = new QTableWidget(this);

	const QStringList headers = { tr("File"), tr("Line"), tr("Text") };
	_results->setColumnCount(3);
	_results->setHorizontalHeaderLabels(headers);

	QGridLayout* const grid_layout = new QGridLayout(this);
	grid_layout->addWidget(_filters, 0, 0);
	grid_layout->addWidget(_search_button, 0, 3);
	grid_layout->addWidget(_results, 1, 0, -1, 4);

	setLayout(grid_layout);

	connect(_search_button, &QPushButton::clicked, this, &ToDoWindow::performOrCancelSearch);
	connect(&_search_thread, &SearchThread::resultFound, this, &ToDoWindow::updateResults);
	connect(&_search_thread, &SearchThread::finished, this, &ToDoWindow::searchFinished);
}

ToDoWindow::~ToDoWindow()
{
}

void ToDoWindow::updateResults(const QString& file, size_t line, const QString& text)
{
	const QByteArray file_utf8 = file.toUtf8();
	const QByteArray text_utf8 = text.toUtf8();

	QTableWidgetItem* const file_item = new QTableWidgetItem(file);
	QTableWidgetItem* const line_item = new QTableWidgetItem(QString("%1").arg(line));
	QTableWidgetItem* const text_item = new QTableWidgetItem(text);

	file_item->setFlags(file_item->flags() & ~Qt::ItemIsEditable);
	line_item->setFlags(line_item->flags() & ~Qt::ItemIsEditable);
	text_item->setFlags(text_item->flags() & ~Qt::ItemIsEditable);

	const int32_t num_rows = _results->rowCount();
	_results->setRowCount(num_rows + 1);

	_results->setItem(num_rows, 0, file_item);
	_results->setItem(num_rows, 1, line_item);
	_results->setItem(num_rows, 2, text_item);
}

void ToDoWindow::performOrCancelSearch(void)
{
	if (_search_thread.isRunning()) {
		_search_button->setEnabled(false);
		_search_thread.cancel();
		return;
	}

	const QString filter_text = _filters->text();

	if (filter_text.isEmpty()) {
		// $TODO: Log error.
		return;
	}

	// .. to get out of bin directory.
	if (_search_thread.search("..", filter_text)) {
		_search_button->setText(tr("Cancel"));
		_results->setRowCount(0);
	}
}

void ToDoWindow::searchFinished(void)
{
	_search_button->setText(tr("Find"));
	_search_button->setEnabled(true);
}

NS_END
