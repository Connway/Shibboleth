premake5 %*

@if exist Temp.sln (
	del Temp.sln
)

@if exist Makefile (
	del Makefile
)
