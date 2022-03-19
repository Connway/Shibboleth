#include "Windows/Shibboleth_EditorMainWindow.h"
#include <Shibboleth_GraphicsConfigs.h>
#include <Shibboleth_AppConfigs.h>
#include <Shibboleth_App.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
	//while (true) {
	//	int i = 0;
	//	i += 5;
	//}

	QApplication a(argc, argv);
	QTranslator translator;

	const QStringList uiLanguages = QLocale::system().uiLanguages();

	for (const QString &locale : uiLanguages) {
		const QString baseName = "ShibEditor_" + QLocale(locale).name();
		if (translator.load(":/i18n/" + baseName)) {
			a.installTranslator(&translator);
			break;
		}
	}

	Shibboleth::App app;

	Gaff::JSON& configs = app.getConfigs();
	configs.setObject(Shibboleth::k_config_app_working_dir, Gaff::JSON::CreateString(u8".."));
	configs.setObject(Shibboleth::k_config_app_log_dir, Gaff::JSON::CreateString(u8"./tools/logs"));
	configs.setObject(Shibboleth::k_config_graphics_no_windows, Gaff::JSON::CreateTrue());

	//if (!_app.init()) {
	//	qApp->exit(-1);
	//}

	EditorMainWindow w;
	w.show();

	const int ret = a.exec();

	//app.destroy();

	return ret;
}
