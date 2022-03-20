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
#include <Shibboleth_EditorAttributesCommon.h>
#include <Shibboleth_AppConfigs.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <QCoreApplication>
#include <QDirIterator>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <filesystem>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ToDoWindow)
	.classAttrs(Shibboleth::EditorWindowAttribute())
	.friendlyName(u8"ToDo")

	.BASE(QWidget)

	.CTOR(QWidget*)
	.ctor<>()
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

	_directory = dir;
	_filters.clear();

	const QStringList filter_strings = filters.split(';');

	for (const QString& filter : filter_strings) {
		const QRegularExpression reg_exp(filter);

		if (!reg_exp.isValid()) {
			// $TODO: Log error.
			continue;
		}

		_filters.emplaceBack(reg_exp);
	}

	if (_filters.isEmpty()) {
		// $TODO: Log error.
		return false;
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

		for (const QRegularExpression& filter : _filters) {
			const QRegularExpressionMatch match = filter.match(file_name);

			if (match.hasMatch()) {
				passes_filters = true;
				break;
			}
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
				emit resultFound(file_name, line_num, line);
			}
		}
	}
}



ToDoWindow::ToDoWindow(QWidget* parent):
	QFrame(parent)
{
	_search_button = new QPushButton(tr("Find"), this);
	_filters = new QLineEdit(".*\.h;.*\.cpp;.*\.lua", this);

	_filters->setPlaceholderText(tr("Search Filter RegExp, separated by ; (eg. .*\.h;.*\.cpp"));

	QListWidget* const list_widget = new QListWidget(this);

	QGridLayout* const grid_layout = new QGridLayout(this);
	grid_layout->addWidget(_filters, 0, 0);
	grid_layout->addWidget(_search_button, 0, 3);
	grid_layout->addWidget(list_widget, 1, 0, -1, 4);

	setLayout(grid_layout);

	connect(_search_button, &QPushButton::clicked, this, &ToDoWindow::performOrCancelSearch);
	connect(&_search_thread, &SearchThread::resultFound, this, &ToDoWindow::updateResults);
}

ToDoWindow::~ToDoWindow()
{
}

void ToDoWindow::updateResults(const QString& file, size_t line, const QString& text)
{
	GAFF_REF(file, line, text);
}

void ToDoWindow::performOrCancelSearch(void)
{
	if (_search_thread.isRunning()) {
		_search_thread.cancel();
		_search_thread.wait();

		_search_button->setText(tr("Find"));
		return;
	}

	//const Gaff::JSON working_dir = GetApp().getConfigs()[k_config_app_working_dir];
	//QString dir;

	//if (working_dir.isString()) {
	//	const char8_t* const temp_dir = working_dir.getString();
	//	dir = temp_dir;
	//	working_dir.freeString(temp_dir);

	//} else {
	//	char8_t temp[2048] = { 0 };
	//	Gaff::GetWorkingDir(temp, ARRAY_SIZE(temp));
	//	dir = temp;
	//}

	const QString filter_text = _filters->text();

	if (filter_text.isEmpty()) {
		// $TODO: Log error.
		return;
	}

	if (_search_thread.search(/*dir*/ "../..", filter_text)) {
		_search_button->setText(tr("Cancel"));
	}
}

NS_END
