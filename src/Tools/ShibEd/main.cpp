#include "Windows/Shibboleth_EditorMainWindow.h"
#include ".generated/Gen_ReflectionInit.h"
#include <Shibboleth_GraphicsConfigs.h>
#include <Shibboleth_AppConfigs.h>
#include <Shibboleth_App.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>

namespace ShibEd
{
	bool Initialize(Shibboleth::IApp& /*app*/, Shibboleth::InitMode mode)
	{
		if (mode == Shibboleth::InitMode::EnumsAndFirstInits) {
		#ifdef SHIB_RUNTIME_VAR_ENABLED
			Shibboleth::RegisterRuntimeVars();
		#endif

		} else if (mode == Shibboleth::InitMode::Regular) {
			// Initialize Enums.
			Refl::InitEnumReflection();

			// Initialize Attributes.
			Refl::InitAttributeReflection();
		}

		ShibEd::Gen::InitReflection(mode);

		if (mode == Shibboleth::InitMode::Regular) {
			// Initialize regular classes.
			Refl::InitClassReflection();
		}

		return true;
	}
}

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

	//if (!app.init()) {
	//	qApp->exit(-1);
	//}

	for (int32_t mode_count = 0; mode_count < static_cast<int32_t>(Shibboleth::InitMode::Count); ++mode_count) {
		const Shibboleth::InitMode mode = static_cast<Shibboleth::InitMode>(mode_count);
		ShibEd::Initialize(app, mode);
	}


	EditorMainWindow w;
	w.show();

	const int ret = a.exec();

	//app.destroy();

	return ret;
}
