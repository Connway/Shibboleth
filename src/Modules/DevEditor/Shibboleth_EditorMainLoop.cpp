/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_EditorMainLoop.h"
#include "Shibboleth_EditorMainWindow.h"
#include <Gaff_ArrayString.h>
#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include <QLibraryInfo>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EditorMainLoop)
	.BASE(Shibboleth::IMainLoop)
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::EditorMainLoop)


namespace
{
	static intptr_t QtThread(void*)
	{
		Gaff::ArrayString plat_plug_path("-platformpluginpath");
		Gaff::ArrayString<char8_t, 1024> working_dir;

		if (Gaff::GetWorkingDir(working_dir.data.data(), working_dir.data.size())) {
			const size_t len = eastl::CharStrlen(working_dir.data.data());

			// Can't fit "/bin" at the end.
			if ((len + 1) >= (working_dir.data.size() - 5)) {
				// $TODO: Log error.
				return -1;
			}

			working_dir.data.data()[len] = u8'/';
			working_dir.data.data()[len + 1] = u8'b';
			working_dir.data.data()[len + 2] = u8'i';
			working_dir.data.data()[len + 3] = u8'n';
			working_dir.data.data()[len + 4] = 0;

		} else {
			// $TODO: Log error.
			return -1;
		}

		// Element 0 is nullptr because Qt skips over it.
		char* argv[] = { nullptr, plat_plug_path.data.data(), reinterpret_cast<char*>(working_dir.data.data()) };
		int argc = static_cast<int>(std::size(argv));

		QApplication a(argc, argv); // Why the heck does this take an int&?
		QTranslator translator;

		const QStringList uiLanguages = QLocale::system().uiLanguages();

		for (const QString& locale : uiLanguages) {
			const QString baseName = "ShibEditor_" + QLocale(locale).name();
			if (translator.load(":/i18n/" + baseName)) {
				a.installTranslator(&translator);
				break;
			}
		}

		EditorMainWindow w;
		w.show();

		const int ret = a.exec();

		Shibboleth::GetApp().quit();

		return ret;
	}
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EditorMainLoop)

bool EditorMainLoop::init(void)
{
	const QString bin_path = QLibraryInfo::path(QLibraryInfo::BinariesPath);
	const QString lib_path = QLibraryInfo::path(QLibraryInfo::LibrariesPath);
	const QString plugins_path = QLibraryInfo::path(QLibraryInfo::PluginsPath);
	GAFF_REF(plugins_path, lib_path, bin_path);

	EA::Thread::ThreadParameters thread_params;
	thread_params.mbDisablePriorityBoost = false;
	thread_params.mnAffinityMask = EA::Thread::kThreadAffinityMaskAny;
	thread_params.mnPriority = EA::Thread::kThreadPriorityDefault;
	thread_params.mnProcessor = EA::Thread::kProcessorDefault;
	thread_params.mnStackSize = 0;
	thread_params.mpName = "QtApp Thread";
	thread_params.mpStack = nullptr;

	const EA::Thread::ThreadId thread_id = _qt_thread.Begin(QtThread, nullptr, &thread_params);

	return thread_id != EA::Thread::kThreadIdInvalid && _main_loop.init();
}

void EditorMainLoop::destroy(void)
{
	_main_loop.destroy();
}

void EditorMainLoop::update(void)
{
	_main_loop.update();
}

NS_END
