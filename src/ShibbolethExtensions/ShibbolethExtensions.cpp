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

#include "ComponentList.h"
#include "ObjectEditor.h"

#include <QDockWidget>
#include <QStringList>
#include <QJsonObject>
#include <QWidget>
#include <QHash>

class IContrivanceWindow;
class QDockWidget;

IContrivanceWindow* gWindow = nullptr;

template <class T>
QWidget* CreateWidget(IContrivanceWindow& window)
{
	return new T(window);
}

typedef QWidget* (*CreateInstanceFunc)(IContrivanceWindow&);

QHash<QString, CreateInstanceFunc> create_funcs;

extern "C" Q_DECL_EXPORT bool InitExtensionModule(IContrivanceWindow& window)
{
	create_funcs["Component List"] = &CreateWidget<ComponentList>;
	create_funcs["Object Editor"] = &CreateWidget<ObjectEditor>;
	gWindow = &window;
	return true;
}

extern "C" Q_DECL_EXPORT void ShutdownExtensionModule(void)
{
}

extern "C" Q_DECL_EXPORT bool SaveInstanceData(const QString&, QJsonObject&, QWidget*)
{
	return true;
}

extern "C" Q_DECL_EXPORT bool LoadInstanceData(const QString&, const QJsonObject&, QWidget*)
{
	return true;
}

extern "C" Q_DECL_EXPORT QWidget* CreateInstance(const QString& widget_name)
{
	Q_ASSERT(create_funcs.contains(widget_name));
	return create_funcs[widget_name](*gWindow);
}

 extern "C" Q_DECL_EXPORT void GetExtensions(QStringList& extensions)
{
	extensions.clear();
	extensions.push_back("Component List");
	extensions.push_back("Object Editor");
}
