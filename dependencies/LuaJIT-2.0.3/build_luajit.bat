@set arch=
@set cfg=
@set buildarg=

@IF [%1] == [x86] (
	@set arch=x86
) ELSE IF [%1] == [x86_64] (
	@set arch=x86_64
) ELSE (
	@echo Did not specify architecture or we don't know what %1 is!
	@exit /b %errorlevel%
)

@IF [%2] == [debug] (
	@set cfg=debug
	@set buildarg=debug
) ELSE IF [%2] == [release] (
	@set cfg=release
) ELSE (
	@echo Did not specify configuration or we don't know what %2 is!
	@exit /b %errorlevel%
)

@IF NOT EXIST lib\%cfg%\%arch%\lua51.lib (
	mkdir lib\%cfg%\%arch%
	cd src
	msvcbuild.bat %buildarg%

	cd ..\..\dependencies\LuaJIT-2.0.3\src

	copy /D lua51.lib ..\lib\%cfg%\%arch%
	copy /D lua51.dll ..\lib\%cfg%\%arch%

	del lua51.lib
	del lua51.dll

	@IF [%cfg%] == [debug] (
		copy /D lua51.pdb ..\lib\%cfg%\%arch%
		del lua51.pdb
	)

	cd ..

) ELSE (
	@echo LuaJIT %arch% %cfg% already built!
)

@IF NOT [%3] == [] (
	copy /D lib\%cfg%\%arch% %3
)
