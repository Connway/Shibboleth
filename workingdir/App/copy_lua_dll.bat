@set arch=
@set cfg=

@IF [%1] == [debug] (
	@set cfg=debug
) ELSE IF [%1] == [release] (
	@set cfg=release
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

copy ..\..\dependencies\LuaJIT-2.0.3\lib\%cfg%\%arch%\lua51.dll lua51.dll /Y
