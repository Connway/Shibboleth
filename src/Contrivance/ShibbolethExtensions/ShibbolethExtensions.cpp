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

#include <QStringList>
#include <QJsonObject>
#include <QWidget>
#include <QHash>

#include "TestExtension.h"

template <class T>
QWidget* CreateWidget(void)
{
	return T();
}

template <class T>
void DestroyWidget(QWidget* widget)
{
	T* casted_widget = (T*)widget;
	delete casted_widget;
}

typedef QWidget* (*CreateInstanceFunc)(void);
typedef void (*DestroyInstanceFunc)(QWidget*);

QHash<QString, CreateInstanceFunc> create_funcs;
QHash<QString, DestroyInstanceFunc> destroy_funcs;

extern "C" Q_DECL_EXPORT bool InitExtensionModule(void)
{
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
	return create_funcs[widget_name]();
}

extern "C" Q_DECL_EXPORT void DestroyInstance(const QString& widget_name, QWidget* widget)
{
	Q_ASSERT(destroy_funcs.contains(widget_name));
	destroy_funcs[widget_name](widget);
}

 extern "C" Q_DECL_EXPORT void GetExtensions(QStringList& extensions)
{
	extensions.clear();
	extensions.push_back("Test Extension");
}
