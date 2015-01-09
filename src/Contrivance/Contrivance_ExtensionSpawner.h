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

#ifndef CONTRIVANCE_EXTENSIONSPAWNER_H
#define CONTRIVANCE_EXTENSIONSPAWNER_H

#include <QAbstractItemModel>
#include <QLibrary>
#include <QWidget>

namespace Ui
{
	class ExtensionSpawner;
}

class IContrivanceWindow;
class ContrivanceWindow;
class QDockWidget;

class ExtensionSpawner : public QWidget
{
	Q_OBJECT

public:
	struct ExtensionData
	{
		// Widget identifiers are QStrings instead of IDs.
		// I'm expecting this system to not be called very frequently.
		typedef bool (*InitExtensionModuleFunc)(IContrivanceWindow&);
		typedef void (*ShutdownExtensionModuleFunc)(void);
		typedef bool (*SaveInstanceDataFunc)(const QString&, QJsonObject&, QWidget*);
		typedef bool (*LoadInstanceDataFunc)(const QString&, const QJsonObject&, QWidget*);
		typedef QWidget* (*CreateInstanceFunc)(const QString&, QDockWidget*); // Passing in QDockWindow for changing the window title
		typedef void (*GetExtensionsFunc)(QStringList&);

		QStringList extension_names;
		QLibrary* library;

		InitExtensionModuleFunc init_func;
		ShutdownExtensionModuleFunc shutdown_func;
		SaveInstanceDataFunc save_func;
		LoadInstanceDataFunc load_func;
		CreateInstanceFunc create_func;
		GetExtensionsFunc get_exts_func;
	};

	explicit ExtensionSpawner(ContrivanceWindow& window, QWidget* parent = nullptr);
	~ExtensionSpawner();

	void loadExtensions(void);

	//bool eventFilter(QObject* object, QEvent* event);

private:
	struct ExtensionSpawnData
	{
		QString name;
		QWidget* instance;
	};

	Ui::ExtensionSpawner* _ui;
	ContrivanceWindow& _window;

	QList<ExtensionData> _extension_modules;
	QHash<QString, int> _extension_indices;

	const QList<ExtensionData>& claimExtensionData(void) const;
	friend class ContrivanceWindow;

private slots:
	void itemDoubleClicked(QModelIndex index);
};

#endif // CONTRIVANCE_EXTENSIONSPAWNER_H
