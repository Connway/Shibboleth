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
class IContrivanceExtension;
class QDockWidget;
class QMainWindow;

class ExtensionSpawner : public QWidget
{
	Q_OBJECT

public:
	explicit ExtensionSpawner(IContrivanceWindow& window, QWidget* parent = nullptr);
	~ExtensionSpawner();

	void loadExtensions(void);
	void close(void);

	void spawnExtension(const QString& ext_name, QMainWindow* parent_window, QSize size = QSize(), const QString* name = nullptr, int id = 0);

	void saveGlobalData(QJsonObject& object) const;
	void loadGlobalData(const QJsonObject& object);
	void saveOpenExtensions(QMainWindow* tab, QJsonArray& widgets) const;
	void loadOpenExtensions(QMainWindow* tab, const QJsonArray& widgets);

	//bool eventFilter(QObject* object, QEvent* event);

	void destroyExtension(QDockWidget* dock_widget);
	void addFreeID(int id);

private:
	struct ExtensionData
	{
		// Widget identifiers are QStrings instead of IDs.
		// I'm expecting this system to not be called very frequently.
		typedef bool (*InitExtensionModuleFunc)(IContrivanceWindow&);
		typedef void (*ShutdownExtensionModuleFunc)(void);
		typedef bool (*SaveInstanceDataFunc)(const QString&, QJsonObject&, QWidget*);
		typedef bool (*LoadInstanceDataFunc)(const QString&, const QJsonObject&, QWidget*);
		typedef IContrivanceExtension* (*CreateInstanceFunc)(const QString&);
		typedef void (*DestroyInstanceFunc)(IContrivanceExtension*);
		typedef void (*GetExtensionsFunc)(QStringList&);

		QStringList extension_names;
		QLibrary* library;

		InitExtensionModuleFunc init_func;
		ShutdownExtensionModuleFunc shutdown_func;
		SaveInstanceDataFunc save_func;
		LoadInstanceDataFunc load_func;
		CreateInstanceFunc create_func;
		DestroyInstanceFunc destroy_func;
		GetExtensionsFunc get_exts_func;
	};

	struct SpawnedExtension
	{
		IContrivanceExtension* extension;
		QDockWidget* dock_widget;
		ExtensionData* ext_data;
	};

	Ui::ExtensionSpawner* _ui;
	IContrivanceWindow& _window;

	QVector<ExtensionData> _extension_modules;
	QHash<QString, int> _extension_indices;
	QVector<SpawnedExtension> _spawned_widgets;
	QVector<int> _free_ids;

	int _num_spawned_extensions;

private slots:
	void itemDoubleClicked(QModelIndex index);
	void widgetDestroyed(QObject* dock_widget);
};

#endif // CONTRIVANCE_EXTENSIONSPAWNER_H
