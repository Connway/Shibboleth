mkdir ..\workingdir\App\bin
mkdir ..\workingdir\App\Components
mkdir ..\workingdir\App\Managers
mkdir ..\workingdir\App\States
mkdir "..\workingdir\App\Render Pipelines"

copy ..\build\output\x86\Debug\ShibbolethComponents32d.dll ..\workingdir\App\Components\ShibbolethComponents32d.dll /Y
copy ..\build\output\x86\Debug\ShibbolethManagers32d.dll ..\workingdir\App\Managers\ShibbolethManagers32d.dll /Y
copy ..\build\output\x86\Debug\ShibbolethStates32d.dll ..\workingdir\App\States\ShibbolethStates32d.dll /Y
copy ..\build\output\x86\Debug\Graphics_OpenGL32d.dll ..\workingdir\App\bin\Graphics_OpenGL32d.dll /Y
copy ..\build\output\x86\Debug\Graphics_Direct3D32d.dll ..\workingdir\App\bin\Graphics_Direct3D32d.dll /Y
copy ..\build\output\x86\Debug\ShibbolethRenderPipelines32d.dll "..\workingdir\App\Render Pipelines\ShibbolethRenderPipelines32d.dll" /Y
copy ..\build\output\x86\Debug\Memory32d.dll ..\workingdir\App\bin\Memory32d.dll /Y
copy ..\build\output\x86\Debug\ResIL32d.dll ..\workingdir\App\bin\ResIL32d.dll /Y
copy ..\build\output\x86\Debug\ResILU32d.dll ..\workingdir\App\bin\ResILU32d.dll /Y
copy ..\build\output\x86\Debug\App32d.exe ..\workingdir\App\App32d.exe /Y

copy ..\build\output\x64\Debug\ShibbolethComponents64d.dll ..\workingdir\App\Components\ShibbolethComponents64d.dll /Y
copy ..\build\output\x64\Debug\ShibbolethManagers64d.dll ..\workingdir\App\Managers\ShibbolethManagers64d.dll /Y
copy ..\build\output\x64\Debug\ShibbolethStates64d.dll ..\workingdir\App\States\ShibbolethStates64d.dll /Y
copy ..\build\output\x64\Debug\Graphics_OpenGL64d.dll ..\workingdir\App\bin\Graphics_OpenGL64d.dll /Y
copy ..\build\output\x64\Debug\Graphics_Direct3D64d.dll ..\workingdir\App\bin\Graphics_Direct3D64d.dll /Y
copy ..\build\output\x64\Debug\ShibbolethRenderPipelines64d.dll "..\workingdir\App\Render Pipelines\ShibbolethRenderPipelines64d.dll" /Y
copy ..\build\output\x64\Debug\Memory64d.dll ..\workingdir\App\bin\Memory64d.dll /Y
copy ..\build\output\x64\Debug\ResIL64d.dll ..\workingdir\App\bin\ResIL64d.dll /Y
copy ..\build\output\x64\Debug\ResILU64d.dll ..\workingdir\App\bin\ResILU64d.dll /Y
copy ..\build\output\x64\Debug\App64d.exe ..\workingdir\App\App64d.exe /Y

copy ..\build\output\x86\Release\ShibbolethComponents32.dll ..\workingdir\App\Components\ShibbolethComponents32.dll /Y
copy ..\build\output\x86\Release\ShibbolethManagers32.dll ..\workingdir\App\Managers\ShibbolethManagers32.dll /Y
copy ..\build\output\x86\Release\ShibbolethStates32.dll ..\workingdir\App\States\ShibbolethStates32.dll /Y
copy ..\build\output\x86\Release\Graphics_OpenGL32.dll ..\workingdir\App\bin\Graphics_OpenGL32.dll /Y
copy ..\build\output\x86\Release\Graphics_Direct3D32.dll ..\workingdir\App\bin\Graphics_Direct3D32.dll /Y
copy ..\build\output\x86\Release\ShibbolethRenderPipelines32.dll "..\workingdir\App\Render Pipelines\ShibbolethRenderPipelines32.dll" /Y
copy ..\build\output\x86\Release\Memory32.dll ..\workingdir\App\bin\Memory32.dll /Y
copy ..\build\output\x86\Release\ResIL32.dll ..\workingdir\App\bin\ResIL32.dll /Y
copy ..\build\output\x86\Release\ResILU32.dll ..\workingdir\App\bin\ResILU32.dll /Y
copy ..\build\output\x86\Release\App32.exe ..\workingdir\App\App32.exe /Y

copy ..\build\output\x64\Release\ShibbolethComponents64.dll ..\workingdir\App\Components\ShibbolethComponents64.dll /Y
copy ..\build\output\x64\Release\ShibbolethManagers64.dll ..\workingdir\App\Managers\ShibbolethManagers64.dll /Y
copy ..\build\output\x64\Release\ShibbolethStates64.dll ..\workingdir\App\States\ShibbolethStates64.dll /Y
copy ..\build\output\x64\Release\Graphics_OpenGL64.dll ..\workingdir\App\bin\Graphics_OpenGL64.dll /Y
copy ..\build\output\x64\Release\Graphics_Direct3D64.dll ..\workingdir\App\bin\Graphics_Direct3D64.dll /Y
copy ..\build\output\x64\Release\ShibbolethRenderPipelines64.dll "..\workingdir\App\Render Pipelines\ShibbolethRenderPipelines64.dll" /Y
copy ..\build\output\x64\Release\Memory64.dll ..\workingdir\App\bin\Memory64.dll /Y
copy ..\build\output\x64\Release\ResIL64.dll ..\workingdir\App\bin\ResIL64.dll /Y
copy ..\build\output\x64\Release\ResILU64.dll ..\workingdir\App\bin\ResILU64.dll /Y
copy ..\build\output\x64\Release\App64.exe ..\workingdir\App\App64.exe /Y

copy ..\build\output\x64\Debug\Contrivance64d.exe ..\workingdir\Editor\Contrivance64d.exe /Y
copy ..\build\output\x64\Debug\ShibbolethExtensions64d.dll ..\workingdir\Editor\extensions\ShibbolethExtensions64d.dll /Y

copy ..\build\output\x64\Release\Contrivance64.exe ..\workingdir\Editor\Contrivance64.exe /Y
copy ..\build\output\x64\Release\ShibbolethExtensions64.dll ..\workingdir\Editor\extensions\ShibbolethExtensions64.dll /Y
