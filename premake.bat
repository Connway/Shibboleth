premake5 %1 %2 %3 %4 %5

@if exist Temp.sln (
	del Temp.sln
)

@if exist Makefile (
	del Makefile
)
