mkdir bin
mkdir Components
mkdir Managers
mkdir States
mkdir "Render Pipelines"

copy ..\..\build\output\x86\Debug\ShibbolethComponents32d.dll Components\ShibbolethComponents32d.dll /Y
copy ..\..\build\output\x86\Debug\ShibbolethManagers32d.dll Managers\ShibbolethManagers32d.dll /Y
copy ..\..\build\output\x86\Debug\ShibbolethStates32d.dll States\ShibbolethStates32d.dll /Y
copy ..\..\build\output\x86\Debug\Graphics_OpenGL32d.dll bin\Graphics_OpenGL32d.dll /Y
copy ..\..\build\output\x86\Debug\Graphics_Direct3D32d.dll bin\Graphics_Direct3D32d.dll /Y
copy ..\..\build\output\x86\Debug\ShibbolethRenderPipelines32d.dll "Render Pipelines\ShibbolethRenderPipelines32d.dll" /Y
copy ..\..\build\output\x86\Debug\Memory32d.dll bin\Memory32d.dll /Y
copy ..\..\build\output\x86\Debug\ResIL32d.dll bin\ResIL32d.dll /Y
copy ..\..\build\output\x86\Debug\ResILU32d.dll bin\ResILU32d.dll /Y
copy ..\..\build\output\x86\Debug\App32d.exe App32d.exe /Y

copy ..\..\build\output\x64\Debug\ShibbolethComponents64d.dll Components\ShibbolethComponents64d.dll /Y
copy ..\..\build\output\x64\Debug\ShibbolethManagers64d.dll Managers\ShibbolethManagers64d.dll /Y
copy ..\..\build\output\x64\Debug\ShibbolethStates64d.dll States\ShibbolethStates64d.dll /Y
copy ..\..\build\output\x64\Debug\Graphics_OpenGL64d.dll bin\Graphics_OpenGL64d.dll /Y
copy ..\..\build\output\x64\Debug\Graphics_Direct3D64d.dll bin\Graphics_Direct3D64d.dll /Y
copy ..\..\build\output\x64\Debug\ShibbolethRenderPipelines64d.dll "Render Pipelines\ShibbolethRenderPipelines64d.dll" /Y
copy ..\..\build\output\x64\Debug\Memory64d.dll bin\Memory64d.dll /Y
copy ..\..\build\output\x64\Debug\ResIL64d.dll bin\ResIL64d.dll /Y
copy ..\..\build\output\x64\Debug\ResILU64d.dll bin\ResILU64d.dll /Y
copy ..\..\build\output\x64\Debug\App64d.exe App64d.exe /Y

copy ..\..\build\output\x86\Release\ShibbolethComponents32.dll Components\ShibbolethComponents32.dll /Y
copy ..\..\build\output\x86\Release\ShibbolethManagers32.dll Managers\ShibbolethManagers32.dll /Y
copy ..\..\build\output\x86\Release\ShibbolethStates32.dll States\ShibbolethStates32.dll /Y
copy ..\..\build\output\x86\Release\Graphics_OpenGL32.dll bin\Graphics_OpenGL32.dll /Y
copy ..\..\build\output\x86\Release\Graphics_Direct3D32.dll bin\Graphics_Direct3D32.dll /Y
copy ..\..\build\output\x86\Release\ShibbolethRenderPipelines32.dll "Render Pipelines\ShibbolethRenderPipelines32.dll" /Y
copy ..\..\build\output\x86\Release\Memory32.dll bin\Memory32.dll /Y
copy ..\..\build\output\x86\Release\ResIL32.dll bin\ResIL32.dll /Y
copy ..\..\build\output\x86\Release\ResILU32.dll bin\ResILU32.dll /Y
copy ..\..\build\output\x86\Release\App32.exe App32.exe /Y

copy ..\..\build\output\x64\Release\ShibbolethComponents64.dll Components\ShibbolethComponents64.dll /Y
copy ..\..\build\output\x64\Release\ShibbolethManagers64.dll Managers\ShibbolethManagers64.dll /Y
copy ..\..\build\output\x64\Release\ShibbolethStates64.dll States\ShibbolethStates64.dll /Y
copy ..\..\build\output\x64\Release\Graphics_OpenGL64.dll bin\Graphics_OpenGL64.dll /Y
copy ..\..\build\output\x64\Release\Graphics_Direct3D64.dll bin\Graphics_Direct3D64.dll /Y
copy ..\..\build\output\x64\Release\ShibbolethRenderPipelines64.dll "Render Pipelines\ShibbolethRenderPipelines64.dll" /Y
copy ..\..\build\output\x64\Release\Memory64.dll bin\Memory64.dll /Y
copy ..\..\build\output\x64\Release\ResIL64.dll bin\ResIL64.dll /Y
copy ..\..\build\output\x64\Release\ResILU64.dll bin\ResILU64.dll /Y
copy ..\..\build\output\x64\Release\App64.exe App64.exe /Y
