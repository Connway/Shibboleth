@set arch=
@set cfg=
@set cfg2=

@IF [%1] == [debug] (
	@set cfg=debug
	@set cfg2=Debug
) ELSE IF [%1] == [release] (
	@set cfg=release
	@set cfg2=Rebug
) ELSE (
	@echo Did not specify configuration or we don't know what %1 is!
	@exit /b %errorlevel%
)

@IF [%2] == [32] (
	@set arch=x86
) ELSE IF [%2] == [64] (
	@set arch=x86_64
) ELSE (
	@echo Did not specify architecture or we don't know what %2 is!
	@exit /b %errorlevel%
)

copy ..\dependencies\prebuilt\EasyHook\%cfg2%\EasyHook32.dll ..\workingdir\App\bin\EasyHook32.dll
copy ..\dependencies\prebuilt\EasyHook\%cfg2%\EasyHook32.dll ..\workingdir\App\bin\EasyHook64.dll
