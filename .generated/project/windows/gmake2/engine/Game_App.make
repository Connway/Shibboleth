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

INCLUDES += -I../../../../../src/Engine/Engine/include -I../../../../../src/Engine/Memory/include -I../../../../../src/Frameworks/Gaff/include -I../../../../../src/Dependencies/rapidjson -I../../../../../src/Dependencies/EASTL/include -I../../../../../src/Dependencies/dirent
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
TARGET = $(TARGETDIR)/Game_App64d.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Debug/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -Og -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -Og -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug/Gaff.lib ../../../../build/windows/gmake2/output/x64/Debug/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug/Gaff.lib ../../../../build/windows/gmake2/output/x64/Debug/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Debug\Game_App64d.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug\Game_App64d.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug\Game_App64d.exe ..\..\..\..\..\workingdir > nul)
endef
endif

ifeq ($(config),release_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Release
TARGET = $(TARGETDIR)/Game_App64.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Release/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Release/Engine.lib ../../../../build/windows/gmake2/output/x64/Release/Gaff.lib ../../../../build/windows/gmake2/output/x64/Release/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Release/Engine.lib ../../../../build/windows/gmake2/output/x64/Release/Gaff.lib ../../../../build/windows/gmake2/output/x64/Release/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Release\Game_App64.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release\Game_App64.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release\Game_App64.exe ..\..\..\..\..\workingdir > nul)
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
TARGET = $(TARGETDIR)/Game_App64d.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Debug_Clang/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -O0 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -O0 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/Memory64d.lib ../../../../build/windows/gmake2/output/x64/Debug_Clang/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Debug_Clang\Game_App64d.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug_Clang\Game_App64d.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Debug_Clang\Game_App64d.exe ..\..\..\..\..\workingdir > nul)
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
TARGET = $(TARGETDIR)/Game_App64.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Release_Clang/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Release_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Release_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/Memory64.lib ../../../../build/windows/gmake2/output/x64/Release_Clang/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Release_Clang\Game_App64.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release_Clang\Game_App64.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Release_Clang\Game_App64.exe ..\..\..\..\..\workingdir > nul)
endef
endif

ifeq ($(config),profile_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Profile
TARGET = $(TARGETDIR)/Game_App64p.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Profile/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DSHIB_PROFILE -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Profile/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile/Gaff.lib ../../../../build/windows/gmake2/output/x64/Profile/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Profile/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile/Gaff.lib ../../../../build/windows/gmake2/output/x64/Profile/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Profile\Game_App64p.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile\Game_App64p.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile\Game_App64p.exe ..\..\..\..\..\workingdir > nul)
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
TARGET = $(TARGETDIR)/Game_App64p.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Profile_Clang/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Profile_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Profile_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/Memory64p.lib ../../../../build/windows/gmake2/output/x64/Profile_Clang/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Profile_Clang\Game_App64p.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile_Clang\Game_App64p.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Profile_Clang\Game_App64p.exe ..\..\..\..\..\workingdir > nul)
endef
endif

ifeq ($(config),optimized_debug_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Optimized_Debug
TARGET = $(TARGETDIR)/Game_App64od.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Optimized_Debug/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Gaff.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Gaff.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug\Game_App64od.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug\Game_App64od.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug\Game_App64od.exe ..\..\..\..\..\workingdir > nul)
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
TARGET = $(TARGETDIR)/Game_App64od.exe
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Optimized_Debug_Clang/Game_App
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
LIBS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/EASTL.lib -liphlpapi.lib -lpsapi.lib -luserenv.lib -lDbghelp
LDDEPS += ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Engine.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Gaff.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/Memory64od.lib ../../../../build/windows/gmake2/output/x64/Optimized_Debug_Clang/EASTL.lib
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -mwindows
define POSTBUILDCMDS
	@echo Running postbuild commands
	IF EXIST ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug_Clang\Game_App64od.exe\ (xcopy /Q /E /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug_Clang\Game_App64od.exe ..\..\..\..\..\workingdir > nul) ELSE (xcopy /Q /Y /I ..\..\..\..\build\windows\gmake2\output\x64\Optimized_Debug_Clang\Game_App64od.exe ..\..\..\..\..\workingdir > nul)
endef
endif

# Per File Configurations
# #############################################


# File sets
# #############################################

OBJECTS :=

OBJECTS += $(OBJDIR)/Game_Main.o

# Rules
# #############################################

all: prebuild prelink $(TARGET) | $(TARGETDIR) $(OBJDIR)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	@echo Linking Game_App
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
	@echo Cleaning Game_App
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

$(OBJDIR)/Game_Main.o: ../../../../../src/Engine/Game_App/Game_Main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include "$(PCH_PLACEHOLDER).d"
endif