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

INCLUDES += -I../../../../../src/Modules/EditorMainLoop/include -I../../../../../src/Engine/Engine/include -I../../../../../src/Engine/Memory/include -I../../../../../src/Frameworks/Gaff/include -I../../../../../src/Frameworks/Gleam/include -I../../../../../src/Dependencies/EASTL/include -I../../../../../src/Modules/Graphics/include
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LIBS +=
LDDEPS +=
LINKCMD = $(AR) -rcs "$@" $(OBJECTS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),debug_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Debug
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Debug/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -Og -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -Og -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64
endif

ifeq ($(config),release_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Release
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Release/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto
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
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Debug_Clang/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -O0 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -ffast-math -O0 -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64
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
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Release_Clang/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto
endif

ifeq ($(config),profile_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Profile
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Profile/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DSHIB_PROFILE -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto
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
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Profile_Clang/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -DNDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto
endif

ifeq ($(config),optimized_debug_x64)
RESCOMP = windres
TARGETDIR = ../../../../build/windows/gmake2/output/x64/Optimized_Debug
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Optimized_Debug/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto
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
TARGET = $(TARGETDIR)/EditorMainLoop.lib
OBJDIR = ../../../../build/windows/gmake2/intermediate/x64/Optimized_Debug_Clang/EditorMainLoop
DEFINES += -DUNICODE -D_UNICODE -DEA_COMPILER_NO_RTTI -D_DEBUG -DDEBUG -DWIN32 -D_WINDOWS -DWIN64
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -Werror -ffast-math -O3 -g -Wall -Wextra -std=c++1z -fno-rtti
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto
endif

# Per File Configurations
# #############################################


# File sets
# #############################################

OBJECTS :=

OBJECTS += $(OBJDIR)/Shibboleth_EditorMainLoop.o

# Rules
# #############################################

all: prebuild prelink $(TARGET) | $(TARGETDIR) $(OBJDIR)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	@echo Linking EditorMainLoop
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
	@echo Cleaning EditorMainLoop
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

$(OBJDIR)/Shibboleth_EditorMainLoop.o: ../../../../../src/Modules/EditorMainLoop/Shibboleth_EditorMainLoop.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include "$(PCH_PLACEHOLDER).d"
endif