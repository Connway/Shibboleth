# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug_x64
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild prelink

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

# Configurations
# #############################################

INCLUDES += -I../../../../../src/Dependencies/wxWidgets/include -I../../../../../src/Dependencies/wxWidgets/include/msvc
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef

ifeq ($(config),debug_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Debug
TARGET = $(TARGETDIR)/wxXRC64d.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Debug/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -ffast-math -Og -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -ffast-math -Og -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxBase64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxCore64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxAdv64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxHTML64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxXML64d.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxBase64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxCore64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxAdv64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxHTML64d.lib ../../../../build/windows/gmake2/output/x64/Debug/wxXML64d.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Debug/wxXRC64d.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Debug\wxXRC64d.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug\wxXRC64d.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug\wxXRC64d.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

ifeq ($(config),release_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Release
TARGET = $(TARGETDIR)/wxXRC64.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Release/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Release/Engine.lib ../../../../build/windows/gmake2/output/x64/Release/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release/wxBase64.lib ../../../../build/windows/gmake2/output/x64/Release/wxCore64.lib ../../../../build/windows/gmake2/output/x64/Release/wxAdv64.lib ../../../../build/windows/gmake2/output/x64/Release/wxHTML64.lib ../../../../build/windows/gmake2/output/x64/Release/wxXML64.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Release/Engine.lib ../../../../build/windows/gmake2/output/x64/Release/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release/wxBase64.lib ../../../../build/windows/gmake2/output/x64/Release/wxCore64.lib ../../../../build/windows/gmake2/output/x64/Release/wxAdv64.lib ../../../../build/windows/gmake2/output/x64/Release/wxHTML64.lib ../../../../build/windows/gmake2/output/x64/Release/wxXML64.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Release/wxXRC64.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Release\wxXRC64.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release\wxXRC64.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release\wxXRC64.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

ifeq ($(config),debug_clang_x64)
ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = ar
endif
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Debug_Clang
TARGET = $(TARGETDIR)/wxXRC64d.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Debug_Clang/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -ffast-math -O0 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -ffast-math -O0 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxBase64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxCore64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxAdv64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxHTML64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxXML64d.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxBase64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxCore64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxAdv64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxHTML64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/wxXML64d.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Debug_Clang/wxXRC64d.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Debug_Clang\wxXRC64d.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug_Clang\wxXRC64d.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug_Clang\wxXRC64d.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

ifeq ($(config),release_clang_x64)
ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = ar
endif
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Release_Clang
TARGET = $(TARGETDIR)/wxXRC64.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Release_Clang/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Release_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxBase64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxCore64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxAdv64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxHTML64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxXML64.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Release_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxBase64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxCore64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxAdv64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxHTML64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/wxXML64.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Release_Clang/wxXRC64.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Release_Clang\wxXRC64.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release_Clang\wxXRC64.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release_Clang\wxXRC64.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

ifeq ($(config),profile_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Profile
TARGET = $(TARGETDIR)/wxXRC64p.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Profile/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DSHIB_PROFILE -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Profile/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxBase64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxCore64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxAdv64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxHTML64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxXML64p.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Profile/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxBase64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxCore64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxAdv64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxHTML64p.lib ../../../../build/windows/gmake2/output/x64/Profile/wxXML64p.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Profile/wxXRC64p.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Profile\wxXRC64p.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile\wxXRC64p.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile\wxXRC64p.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

ifeq ($(config),profile_clang_x64)
ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = ar
endif
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Profile_Clang
TARGET = $(TARGETDIR)/wxXRC64p.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Profile_Clang/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Profile_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxBase64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxCore64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxAdv64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxHTML64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxXML64p.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Profile_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxBase64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxCore64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxAdv64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxHTML64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/wxXML64p.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Profile_Clang/wxXRC64p.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Profile_Clang\wxXRC64p.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile_Clang\wxXRC64p.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile_Clang\wxXRC64p.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

ifeq ($(config),optimized_debug_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Optimized_Debug
TARGET = $(TARGETDIR)/wxXRC64od.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Optimized_Debug/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxBase64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxCore64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxAdv64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxHTML64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxXML64od.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxBase64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxCore64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxAdv64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxHTML64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxXML64od.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Optimized_Debug/wxXRC64od.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug\wxXRC64od.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug\wxXRC64od.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug\wxXRC64od.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

ifeq ($(config),optimized_debug_clang_x64)
ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = ar
endif
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang
TARGET = $(TARGETDIR)/wxXRC64od.dll
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Optimized_Debug_Clang/wxXRC
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64 -DNOPCH -DWXBUILDING -DwxUSE_ZLIB_H_IN_PATH -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DXML_STATIC -DWXMAKINGDLL_XRC -DWXUSINGDLL -D__WXMSW__
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxBase64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxCore64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxAdv64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxHTML64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxXML64od.lib
LDDEPS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxBase64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxCore64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxAdv64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxHTML64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxXML64od.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -shared -Wl,--out-implib="../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/wxXRC64od.lib"
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF NOT EXIST ..\..\..\..\..\workingdir\bin (mkdir ..\..\..\..\..\workingdir\bin)
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug_Clang\wxXRC64od.dll\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug_Clang\wxXRC64od.dll ..\..\..\..\..\workingdir\bin > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug_Clang\wxXRC64od.dll ..\..\..\..\..\workingdir\bin > nul)
endef
endif

# Per File Configurations
# #############################################


# File sets
# #############################################

OBJECTS :=

OBJECTS += $(OBJDIR)/dummy.o
OBJECTS += $(OBJDIR)/xh_animatctrl.o
OBJECTS += $(OBJDIR)/xh_bannerwindow.o
OBJECTS += $(OBJDIR)/xh_bmp.o
OBJECTS += $(OBJDIR)/xh_bmpbt.o
OBJECTS += $(OBJDIR)/xh_bmpcbox.o
OBJECTS += $(OBJDIR)/xh_bttn.o
OBJECTS += $(OBJDIR)/xh_cald.o
OBJECTS += $(OBJDIR)/xh_chckb.o
OBJECTS += $(OBJDIR)/xh_chckl.o
OBJECTS += $(OBJDIR)/xh_choic.o
OBJECTS += $(OBJDIR)/xh_choicbk.o
OBJECTS += $(OBJDIR)/xh_clrpicker.o
OBJECTS += $(OBJDIR)/xh_cmdlinkbn.o
OBJECTS += $(OBJDIR)/xh_collpane.o
OBJECTS += $(OBJDIR)/xh_combo.o
OBJECTS += $(OBJDIR)/xh_comboctrl.o
OBJECTS += $(OBJDIR)/xh_datectrl.o
OBJECTS += $(OBJDIR)/xh_dirpicker.o
OBJECTS += $(OBJDIR)/xh_dlg.o
OBJECTS += $(OBJDIR)/xh_editlbox.o
OBJECTS += $(OBJDIR)/xh_filectrl.o
OBJECTS += $(OBJDIR)/xh_filepicker.o
OBJECTS += $(OBJDIR)/xh_fontpicker.o
OBJECTS += $(OBJDIR)/xh_frame.o
OBJECTS += $(OBJDIR)/xh_gauge.o
OBJECTS += $(OBJDIR)/xh_gdctl.o
OBJECTS += $(OBJDIR)/xh_grid.o
OBJECTS += $(OBJDIR)/xh_html.o
OBJECTS += $(OBJDIR)/xh_htmllbox.o
OBJECTS += $(OBJDIR)/xh_hyperlink.o
OBJECTS += $(OBJDIR)/xh_listb.o
OBJECTS += $(OBJDIR)/xh_listbk.o
OBJECTS += $(OBJDIR)/xh_listc.o
OBJECTS += $(OBJDIR)/xh_mdi.o
OBJECTS += $(OBJDIR)/xh_menu.o
OBJECTS += $(OBJDIR)/xh_notbk.o
OBJECTS += $(OBJDIR)/xh_odcombo.o
OBJECTS += $(OBJDIR)/xh_panel.o
OBJECTS += $(OBJDIR)/xh_propdlg.o
OBJECTS += $(OBJDIR)/xh_radbt.o
OBJECTS += $(OBJDIR)/xh_radbx.o
OBJECTS += $(OBJDIR)/xh_scrol.o
OBJECTS += $(OBJDIR)/xh_scwin.o
OBJECTS += $(OBJDIR)/xh_simplebook.o
OBJECTS += $(OBJDIR)/xh_sizer.o
OBJECTS += $(OBJDIR)/xh_slidr.o
OBJECTS += $(OBJDIR)/xh_spin.o
OBJECTS += $(OBJDIR)/xh_split.o
OBJECTS += $(OBJDIR)/xh_srchctrl.o
OBJECTS += $(OBJDIR)/xh_statbar.o
OBJECTS += $(OBJDIR)/xh_stbmp.o
OBJECTS += $(OBJDIR)/xh_stbox.o
OBJECTS += $(OBJDIR)/xh_stlin.o
OBJECTS += $(OBJDIR)/xh_sttxt.o
OBJECTS += $(OBJDIR)/xh_text.o
OBJECTS += $(OBJDIR)/xh_tglbtn.o
OBJECTS += $(OBJDIR)/xh_timectrl.o
OBJECTS += $(OBJDIR)/xh_toolb.o
OBJECTS += $(OBJDIR)/xh_toolbk.o
OBJECTS += $(OBJDIR)/xh_tree.o
OBJECTS += $(OBJDIR)/xh_treebk.o
OBJECTS += $(OBJDIR)/xh_unkwn.o
OBJECTS += $(OBJDIR)/xh_wizrd.o
OBJECTS += $(OBJDIR)/xmladv.o
OBJECTS += $(OBJDIR)/xmlres.o
OBJECTS += $(OBJDIR)/xmlrsall.o

# Rules
# #############################################

all: prebuild prelink $(TARGET) | $(TARGETDIR) $(OBJDIR)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	@echo Linking wxXRC
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning wxXRC
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) $(PCH) | $(OBJDIR) $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | $(OBJDIR)
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
	$(SILENT) touch "$@"
else
$(OBJECTS): | $(OBJDIR)
endif


# File Rules
# #############################################

$(OBJDIR)/dummy.o: ../../../../../src/Dependencies/wxWidgets/src/common/dummy.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_animatctrl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_animatctrl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_bannerwindow.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_bannerwindow.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_bmp.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_bmp.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_bmpbt.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_bmpbt.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_bmpcbox.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_bmpcbox.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_bttn.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_bttn.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_cald.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_cald.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_chckb.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_chckb.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_chckl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_chckl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_choic.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_choic.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_choicbk.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_choicbk.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_clrpicker.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_clrpicker.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_cmdlinkbn.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_cmdlinkbn.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_collpane.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_collpane.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_combo.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_combo.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_comboctrl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_comboctrl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_datectrl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_datectrl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_dirpicker.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_dirpicker.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_dlg.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_dlg.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_editlbox.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_editlbox.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_filectrl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_filectrl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_filepicker.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_filepicker.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_fontpicker.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_fontpicker.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_frame.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_frame.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_gauge.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_gauge.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_gdctl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_gdctl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_grid.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_grid.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_html.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_html.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_htmllbox.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_htmllbox.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_hyperlink.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_hyperlink.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_listb.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_listb.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_listbk.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_listbk.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_listc.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_listc.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_mdi.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_mdi.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_menu.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_menu.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_notbk.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_notbk.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_odcombo.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_odcombo.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_panel.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_panel.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_propdlg.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_propdlg.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_radbt.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_radbt.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_radbx.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_radbx.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_scrol.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_scrol.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_scwin.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_scwin.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_simplebook.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_simplebook.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_sizer.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_sizer.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_slidr.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_slidr.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_spin.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_spin.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_split.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_split.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_srchctrl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_srchctrl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_statbar.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_statbar.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_stbmp.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_stbmp.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_stbox.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_stbox.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_stlin.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_stlin.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_sttxt.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_sttxt.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_text.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_text.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_tglbtn.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_tglbtn.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_timectrl.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_timectrl.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_toolb.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_toolb.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_toolbk.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_toolbk.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_tree.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_tree.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_treebk.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_treebk.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_unkwn.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_unkwn.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xh_wizrd.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xh_wizrd.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xmladv.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xmladv.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xmlres.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xmlres.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/xmlrsall.o: ../../../../../src/Dependencies/wxWidgets/src/xrc/xmlrsall.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include "$(PCH_PLACEHOLDER).d"
endif