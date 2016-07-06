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

#include "Shibboleth_InspectionPanel.h"
#include "Shibboleth_ComponentList.h"
#include <QStringList>
#include <QJsonObject>
#include <QWidget>
#include <QHash>

Contrivance::IContrivanceWindow* gWindow = nullptr;

template <class T>
Contrivance::IExtension* CreateWidget(Contrivance::IContrivanceWindow& window)
{
	return new T(window);
}

typedef Contrivance::IExtension* (*CreateInstanceFunc)(Contrivance::IContrivanceWindow&);

QHash<QString, CreateInstanceFunc> create_funcs;

extern "C" Q_DECL_EXPORT bool InitExtensionModule(Contrivance::IContrivanceWindow& window)
{
	create_funcs["Component List"] = &CreateWidget<Shibboleth::ComponentList>;
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

extern "C" Q_DECL_EXPORT Contrivance::IExtension* CreateInstance(const QString& widget_name)
{
	Q_ASSERT(create_funcs.contains(widget_name));
	return create_funcs[widget_name](*gWindow);
}

extern "C" Q_DECL_EXPORT void DestroyInstance(Contrivance::IExtension* ext)
{
	delete ext;
}

 extern "C" Q_DECL_EXPORT void GetExtensions(QStringList& extensions)
{
	extensions.clear();

	for (auto it = create_funcs.begin(); it != create_funcs.end(); ++it) {
		extensions.push_back(it.key());
	}
}
