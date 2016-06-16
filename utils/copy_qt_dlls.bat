if not "%1" == "" (
	set dir=%1\msvc2015_64\bin

	copy %dir%\Qt5Widgets.dll ..\workingdir\Editor\Qt5Widgets.dll /Y
	copy %dir%\Qt5Widgetsd.dll ..\workingdir\Editor\Qt5Widgetsd.dll /Y
	copy %dir%\Qt5Gui.dll ..\workingdir\Editor\Qt5Gui.dll /Y
	copy %dir%\Qt5Guid.dll ..\workingdir\Editor\Qt5Guid.dll /Y
	copy %dir%\Qt5Core.dll ..\workingdir\Editor\Qt5Core.dll /Y
	copy %dir%\Qt5Cored.dll ..\workingdir\Editor\Qt5Cored.dll /Y
)

copy ..\build\ShibbolethExtensions\Release\release\ShibbolethExtensions64.dll ..\workingdir\Editor\extensions\ShibbolethExtensions64.dll /Y
copy ..\build\ShibbolethExtensions\Debug\debug\ShibbolethExtensions64d.dll ..\workingdir\Editor\extensions\ShibbolethExtensions64d.dll /Y
