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

#pragma once

#include <Reflection/Shibboleth_Reflection.h>
#include <QThread>
#include <QMutex>
#include <QFrame>

class QTableWidget;
class QPushButton;
class QLineEdit;

NS_SHIBBOLETH

class SearchThread final : public QThread
{
	Q_OBJECT

public:
	SearchThread(QObject* parent = nullptr);
	~SearchThread(void);

	bool search(const QString& dir, const QString& filters);
	void cancel(void);

signals:
	void resultFound(const QString& file, size_t line, const QString& text);
	void finished(void);

protected:
	void run(void) override;

private:
	struct Filter final
	{
		QString directory;
		QString extension;
	};

	QList<Filter> _filters;
	QString _directory;
	QMutex _lock;
	bool _cancel = false;
};


class ToDoWindow final : public QFrame
{
	Q_OBJECT

public:
	ToDoWindow(QWidget* parent = nullptr);
	~ToDoWindow();

private:
	SearchThread _search_thread;

	QPushButton* _search_button = nullptr;
	QTableWidget* _results = nullptr;
	QLineEdit* _filters = nullptr;

	void updateResults(const QString& file, size_t line, const QString& text);
	void performOrCancelSearch(void);
	void searchFinished(void);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ToDoWindow)
