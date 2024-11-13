header_files = [
  'public/common/TracyAlign.hpp',
  'public/common/TracyAlloc.hpp',
  'public/common/TracyColor.hpp',
  'public/common/TracyForceInline.hpp',
  'public/common/TracyMutex.hpp',
  'public/common/TracyProtocol.hpp',
  'public/common/TracyQueue.hpp',
  'public/common/TracySocket.hpp',
  'public/common/TracyStackFrames.hpp',
  'public/common/TracySystem.hpp',
  'public/common/TracyUwp.hpp',
  'public/common/TracyVersion.hpp',
  'public/common/TracyYield.hpp',
  'public/common/tracy_lz4.hpp',
  'public/common/tracy_lz4hc.hpp',
  'public/common/TracyApi.h',
  'public/client/TracyCallstack.h',
  'public/client/tracy_SPSCQueue.h',
  'public/client/tracy_concurrentqueue.h',
  'public/libbacktrace/config.h',
  'public/tracy/TracyC.h',
  'public/client/TracyArmCpuTable.hpp',
  'public/client/TracyCallstack.hpp',
  'public/client/TracyCpuid.hpp',
  'public/client/TracyDebug.hpp',
  'public/client/TracyDxt1.hpp',
  'public/client/TracyFastVector.hpp',
  'public/client/TracyKCore.hpp',
  'public/client/TracyLock.hpp',
  'public/client/TracyProfiler.hpp',
  'public/client/TracyRingBuffer.hpp',
  'public/client/TracyScoped.hpp',
  'public/client/TracyStringHelpers.hpp',
  'public/client/TracySysPower.hpp',
  'public/client/TracySysTime.hpp',
  'public/client/TracySysTrace.hpp',
  'public/client/TracyThread.hpp',
  'public/client/tracy_rpmalloc.hpp',
  'public/libbacktrace/backtrace.hpp',
  'public/libbacktrace/filenames.hpp',
  'public/libbacktrace/internal.hpp',
  'public/tracy/Tracy.hpp',
  'public/tracy/TracyD3D11.hpp',
  'public/tracy/TracyD3D12.hpp',
  'public/tracy/TracyLua.hpp',
  'public/tracy/TracyOpenCL.hpp',
  'public/tracy/TracyOpenGL.hpp',
  'public/tracy/TracyVulkan.hpp',
]

source_files = [
  'public/client/TracyAlloc.cpp',
  'public/client/TracyCallstack.cpp',
  'public/client/TracyDxt1.cpp',
  'public/client/TracyKCore.cpp',
  'public/client/TracyOverride.cpp',
  'public/client/TracyProfiler.cpp',
  'public/client/TracySysPower.cpp',
  'public/client/TracySysTime.cpp',
  'public/client/TracySysTrace.cpp',
  'public/client/tracy_rpmalloc.cpp',
  'public/common/TracySocket.cpp',
  'public/common/TracyStackFrames.cpp',
  'public/common/TracySystem.cpp',
  'public/common/tracy_lz4.cpp',
  'public/common/tracy_lz4hc.cpp',
]

extra_files = [
]

include_dirs = [
]

build_args = [
  '-DTRACY_EXPORTS',
]

tracy_defines = [
  '-DTRACY_ENABLE',
  '-DTTRACY_CALLSTACK=10',
  '-DTTRACY_FIBERS',
]

dependencies = [
]

if is_windows
  build_args += [
    '-DWIN32_LEAN_AND_MEAN',
    '-DNOMINMAX',
    '-D_CRT_SECURE_NO_WARNINGS',
    '-D_SCL_SECURE_NO_WARNINGS',
  ]

  dependencies += [
    compiler.find_library('ws2_32'),
    compiler.find_library('Dbghelp'),
  ]

else
  source_files += [
    'public/libbacktrace/alloc.cpp',
    'public/libbacktrace/dwarf.cpp',
    'public/libbacktrace/fileline.cpp',
    'public/libbacktrace/mmapio.cpp',
    'public/libbacktrace/posix.cpp',
    'public/libbacktrace/sort.cpp',
    'public/libbacktrace/state.cpp',
  ]

  if is_linux
    source_files += ['public/libbacktrace/elf.cpp']
  elif is_mac
    source_files += ['public/libbacktrace/macho.cpp']
  endif
endif

tracy_client_static = static_library(
  'TracyClientStatic',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + tracy_defines,
  dependencies: dependencies,
  override_options: {
    'werror': false,
  },
)

tracy_client = shared_library(
  'TracyClient',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args + tracy_defines,
  dependencies: dependencies,
  override_options: {
    'werror': false,
  },
  install: true,
  install_dir: 'bin',
)

if is_profile
  tracy_client_static_dep = declare_dependency(
    include_directories: ['.'],
    compile_args: tracy_defines,
    link_with: [tracy_client_static],
  )

  tracy_client_dep = declare_dependency(
    include_directories: ['.'],
    compile_args: tracy_defines + ['-DTRACY_IMPORTS'],
    link_with: [tracy_client],
  )
else
  tracy_client_static_dep = declare_dependency(
    include_directories: ['.'],
  )

  tracy_client_dep = declare_dependency(
    include_directories: ['.'],
  )
endif

all_deps += tracy_client



header_files = [
  'imgui/imconfig.h',
  'imgui/imgui.h',
  'imgui/imgui_internal.h',
  'imgui/imstb_rectpack.h',
  'imgui/imstb_textedit.h',
  'imgui/imstb_truetype.h',
  'imgui/misc/freetype/imgui_freetype.h',
  'dtl/Diff.hpp',
  'dtl/Diff3.hpp',
  'dtl/Lcs.hpp',
  'dtl/Sequence.hpp',
  'dtl/Ses.hpp',
  'dtl/dtl.hpp',
  'dtl/functors.hpp',
  'dtl/variables.hpp',
  'nfd/nfd.h',
  'server/tracy_pdqsort.h',
  'server/tracy_robin_hood.h',
  'server/tracy_xxhash.h',
  'server/TracyCharUtil.hpp',
  'server/TracyEvent.hpp',
  'server/TracyFileHeader.hpp',
  'server/TracyFileMeta.hpp',
  'server/TracyFileRead.hpp',
  'server/TracyFileWrite.hpp',
  'server/TracyMemory.hpp',
  'server/TracyMmap.hpp',
  'server/TracyPopcnt.hpp',
  'server/TracyPrint.hpp',
  'server/TracyShortPtr.hpp',
  'server/TracySlab.hpp',
  'server/TracySort.hpp',
  'server/TracySortedVector.hpp',
  'server/TracyStringDiscovery.hpp',
  'server/TracySysUtil.hpp',
  'server/TracyTaskDispatch.hpp',
  'server/TracyTextureCompression.hpp',
  'server/TracyThreadCompress.hpp',
  'server/TracyVarArray.hpp',
  'server/TracyVector.hpp',
  'server/TracyWorker.hpp',
  'profiler/src/imgui/imgui_impl_glfw.h',
  'profiler/src/imgui/imgui_impl_opengl3.h',
  'profiler/src/imgui/imgui_impl_opengl3_loader.h',
  'profiler/src/ini.h',
  'profiler/src/profiler/IconsFontAwesome6.h',
  'profiler/src/stb_image.h',
  'profiler/src/stb_image_resize.h',
  'profiler/src/Backend.hpp',
  'profiler/src/ConnectionHistory.hpp',
  'profiler/src/Filters.hpp',
  'profiler/src/Fonts.hpp',
  'profiler/src/HttpRequest.hpp',
  'profiler/src/ImGuiContext.hpp',
  'profiler/src/IsElevated.hpp',
  'profiler/src/ResolvService.hpp',
  'profiler/src/RunQueue.hpp',
  'profiler/src/WindowPosition.hpp',
  'profiler/src/font/DroidSans.hpp',
  'profiler/src/font/FiraCodeRetina.hpp',
  'profiler/src/font/FontAwesomeSolid.hpp',
  'profiler/src/icon.hpp',
  'profiler/src/profiler/TracyAchievements.hpp',
  'profiler/src/profiler/TracyBadVersion.hpp',
  'profiler/src/profiler/TracyBuzzAnim.hpp',
  'profiler/src/profiler/TracyColor.hpp',
  'profiler/src/profiler/TracyConfig.hpp',
  'profiler/src/profiler/TracyDecayValue.hpp',
  'profiler/src/profiler/TracyEventDebug.hpp',
  'profiler/src/profiler/TracyFileselector.hpp',
  'profiler/src/profiler/TracyFilesystem.hpp',
  'profiler/src/profiler/TracyImGui.hpp',
  'profiler/src/profiler/TracyLockHelpers.hpp',
  'profiler/src/profiler/TracyMicroArchitecture.hpp',
  'profiler/src/profiler/TracyMouse.hpp',
  'profiler/src/profiler/TracyProtoHistory.hpp',
  'profiler/src/profiler/TracySourceContents.hpp',
  'profiler/src/profiler/TracySourceTokenizer.hpp',
  'profiler/src/profiler/TracySourceView.hpp',
  'profiler/src/profiler/TracyStorage.hpp',
  'profiler/src/profiler/TracyTexture.hpp',
  'profiler/src/profiler/TracyTimelineContext.hpp',
  'profiler/src/profiler/TracyTimelineController.hpp',
  'profiler/src/profiler/TracyTimelineDraw.hpp',
  'profiler/src/profiler/TracyTimelineItem.hpp',
  'profiler/src/profiler/TracyTimelineItemCpuData.hpp',
  'profiler/src/profiler/TracyTimelineItemGpu.hpp',
  'profiler/src/profiler/TracyTimelineItemPlot.hpp',
  'profiler/src/profiler/TracyTimelineItemThread.hpp',
  'profiler/src/profiler/TracyUserData.hpp',
  'profiler/src/profiler/TracyUtility.hpp',
  'profiler/src/profiler/TracyView.hpp',
  'profiler/src/profiler/TracyViewData.hpp',
  'profiler/src/profiler/TracyWeb.hpp',
  'profiler/src/zigzag01.hpp',
  'profiler/src/zigzag02.hpp',
  'profiler/src/zigzag04.hpp',
  'profiler/src/zigzag08.hpp',
  'profiler/src/zigzag16.hpp',
  'profiler/src/zigzag32.hpp',
  'public/common/TracyAlign.hpp',
  'public/common/TracyAlloc.hpp',
  'public/common/TracyColor.hpp',
  'public/common/TracyForceInline.hpp',
  'public/common/TracyMutex.hpp',
  'public/common/TracyProtocol.hpp',
  'public/common/TracyQueue.hpp',
  'public/common/TracySocket.hpp',
  'public/common/TracyStackFrames.hpp',
  'public/common/TracySystem.hpp',
  'public/common/TracyUwp.hpp',
  'public/common/TracyVersion.hpp',
  'public/common/TracyYield.hpp',
  'public/common/tracy_lz4.hpp',
  'public/common/tracy_lz4hc.hpp',
  'public/common/TracyApi.h',
]

source_files = [
  'imgui/imgui.cpp',
  'imgui/imgui_demo.cpp',
  'imgui/imgui_draw.cpp',
  'imgui/imgui_tables.cpp',
  'imgui/imgui_widgets.cpp',
  'server/TracyMemory.cpp',
  'server/TracyMmap.cpp',
  'server/TracyPrint.cpp',
  'server/TracySysUtil.cpp',
  'server/TracyTaskDispatch.cpp',
  'server/TracyTextureCompression.cpp',
  'server/TracyThreadCompress.cpp',
  'server/TracyWorker.cpp',
  'profiler/src/ini.c',
  'profiler/src/BackendGlfw.cpp',
  'profiler/src/ConnectionHistory.cpp',
  'profiler/src/Filters.cpp',
  'profiler/src/Fonts.cpp',
  'profiler/src/HttpRequest.cpp',
  'profiler/src/ImGuiContext.cpp',
  'profiler/src/IsElevated.cpp',
  'profiler/src/ResolvService.cpp',
  'profiler/src/RunQueue.cpp',
  'profiler/src/WindowPosition.cpp',
  'profiler/src/imgui/imgui_impl_glfw.cpp',
  'profiler/src/imgui/imgui_impl_opengl3.cpp',
  'profiler/src/main.cpp',
  'profiler/src/profiler/TracyAchievementData.cpp',
  'profiler/src/profiler/TracyAchievements.cpp',
  'profiler/src/profiler/TracyBadVersion.cpp',
  'profiler/src/profiler/TracyColor.cpp',
  'profiler/src/profiler/TracyEventDebug.cpp',
  'profiler/src/profiler/TracyFileselector.cpp',
  'profiler/src/profiler/TracyFilesystem.cpp',
  'profiler/src/profiler/TracyImGui.cpp',
  'profiler/src/profiler/TracyMicroArchitecture.cpp',
  'profiler/src/profiler/TracyMouse.cpp',
  'profiler/src/profiler/TracyProtoHistory.cpp',
  'profiler/src/profiler/TracySourceContents.cpp',
  'profiler/src/profiler/TracySourceTokenizer.cpp',
  'profiler/src/profiler/TracySourceView.cpp',
  'profiler/src/profiler/TracyStorage.cpp',
  'profiler/src/profiler/TracyTexture.cpp',
  'profiler/src/profiler/TracyTimelineController.cpp',
  'profiler/src/profiler/TracyTimelineItem.cpp',
  'profiler/src/profiler/TracyTimelineItemCpuData.cpp',
  'profiler/src/profiler/TracyTimelineItemGpu.cpp',
  'profiler/src/profiler/TracyTimelineItemPlot.cpp',
  'profiler/src/profiler/TracyTimelineItemThread.cpp',
  'profiler/src/profiler/TracyUserData.cpp',
  'profiler/src/profiler/TracyUtility.cpp',
  'profiler/src/profiler/TracyView.cpp',
  'profiler/src/profiler/TracyView_Annotations.cpp',
  'profiler/src/profiler/TracyView_Callstack.cpp',
  'profiler/src/profiler/TracyView_Compare.cpp',
  'profiler/src/profiler/TracyView_ConnectionState.cpp',
  'profiler/src/profiler/TracyView_ContextSwitch.cpp',
  'profiler/src/profiler/TracyView_CpuData.cpp',
  'profiler/src/profiler/TracyView_FindZone.cpp',
  'profiler/src/profiler/TracyView_FrameOverview.cpp',
  'profiler/src/profiler/TracyView_FrameTimeline.cpp',
  'profiler/src/profiler/TracyView_FrameTree.cpp',
  'profiler/src/profiler/TracyView_GpuTimeline.cpp',
  'profiler/src/profiler/TracyView_Locks.cpp',
  'profiler/src/profiler/TracyView_Memory.cpp',
  'profiler/src/profiler/TracyView_Messages.cpp',
  'profiler/src/profiler/TracyView_Navigation.cpp',
  'profiler/src/profiler/TracyView_NotificationArea.cpp',
  'profiler/src/profiler/TracyView_Options.cpp',
  'profiler/src/profiler/TracyView_Playback.cpp',
  'profiler/src/profiler/TracyView_Plots.cpp',
  'profiler/src/profiler/TracyView_Ranges.cpp',
  'profiler/src/profiler/TracyView_Samples.cpp',
  'profiler/src/profiler/TracyView_Statistics.cpp',
  'profiler/src/profiler/TracyView_Timeline.cpp',
  'profiler/src/profiler/TracyView_TraceInfo.cpp',
  'profiler/src/profiler/TracyView_Utility.cpp',
  'profiler/src/profiler/TracyView_ZoneInfo.cpp',
  'profiler/src/profiler/TracyView_ZoneTimeline.cpp',
  'profiler/src/profiler/TracyWeb.cpp',
  'profiler/src/winmain.cpp',
  'profiler/src/winmainArchDiscovery.cpp',
  'public/common/TracySocket.cpp',
  'public/common/TracyStackFrames.cpp',
  'public/common/TracySystem.cpp',
  'public/common/tracy_lz4.cpp',
  'public/common/tracy_lz4hc.cpp',
]

include_dirs = [
  'profiler',
  'server',
  'imgui',
  '../../Dependencies/capstone/include/capstone/',
  '../../Dependencies/capstone/include',
  '../../Dependencies/glfw/include',
]

if is_windows
  source_files += ['nfd/nfd_win.cpp']

elif is_linux
  source_files += ['nfd/nfd_gtk.cpp']

  if is_wayland
    build_args += ['-DDISPLAY_SERVER_WAYLAND']
  else
    dependencies += [
      dependency('dl'),
      dependency('pthread'),
      dependency('gtk+-3.0')
    ]

    build_args += ['-DDISPLAY_SERVER_X11']
    # $TODO: Figure out how to do this with Meson.
    # buildoptions { "`pkg-config --cflags-only-I gtk+-3.0`" }
    # linkoptions { "`pkg-config --libs gtk+-3.0`", "-lpthread", "-ldl" }
  endif

elif is_mac
  source_files += ['nfd/nfd_cocoa.m']
  dependencies += dependency('appleframeworks', modules: ['Cocoa', 'UniformTypeIdentifiers'])
endif

tracy_profiler = executable(
  'TracyProfiler',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
  link_with: [capstone, glfw, zstd],
  override_options: {
    'werror': false,
  },
  install: true,
  install_dir: 'tools',
)
