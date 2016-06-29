set qt_dir=%1\5.6\msvc2015_64\bin
set qt_tools_dir=%1\Tools\QtCreator\bin

cd ..\build\ContrivanceLib\%2
if "%3" == "rebuild" %qt_tools_dir%\jom.exe clean
%qt_tools_dir%\jom.exe

cd ..\..\ShibbolethExtensions\%2
if "%3" == "rebuild" %qt_tools_dir%\jom.exe clean
%qt_tools_dir%\jom.exe

cd ..\..\Contrivance\%2
if "%3" == "rebuild" %qt_tools_dir%\jom.exe clean
%qt_tools_dir%\jom.exe

cd ..\..\..\utils
