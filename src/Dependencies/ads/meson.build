#####################################################################################
# Copyright (C) by Nicholas LaCroix
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#####################################################################################

header_files = [
  'ads_globals.h',
  'AutoHideDockContainer.h',
  'AutoHideSideBar.h',
  'AutoHideTab.h',
  'DockAreaTabBar.h',
  'DockAreaTitleBar.h',
  'DockAreaTitleBar_p.h',
  'DockAreaWidget.h',
  'DockComponentsFactory.h',
  'DockContainerWidget.h',
  'DockFocusController.h',
  'DockingStateReader.h',
  'DockManager.h',
  'DockOverlay.h',
  'DockSplitter.h',
  'DockWidget.h',
  'DockWidgetTab.h',
  'ElidingLabel.h',
  'FloatingDockContainer.h',
  'FloatingDragPreview.h',
  'IconProvider.h',
  'PushButton.h',
  'ResizeHandle.h',
]

source_files = [
  'ads_globals.cpp',
  'AutoHideDockContainer.cpp',
  'AutoHideSideBar.cpp',
  'AutoHideTab.cpp',
  'DockAreaTabBar.cpp',
  'DockAreaTitleBar.cpp',
  'DockAreaWidget.cpp',
  'DockComponentsFactory.cpp',
  'DockContainerWidget.cpp',
  'DockFocusController.cpp',
  'DockingStateReader.cpp',
  'DockManager.cpp',
  'DockOverlay.cpp',
  'DockSplitter.cpp',
  'DockWidget.cpp',
  'DockWidgetTab.cpp',
  'ElidingLabel.cpp',
  'FloatingDockContainer.cpp',
  'FloatingDragPreview.cpp',
  'IconProvider.cpp',
  'PushButton.cpp',
  'ResizeHandle.cpp',
]

extra_files = [
]

include_dirs = [
]

build_args = [
  '-DADS_SHARED_EXPORT',
]

dependencies = [
  dependency('qt6', modules: ['core', 'gui', 'widgets'])
]

if is_debug
  build_args += '-DADS_DEBUG_PRINT'
endif

if is_linux
  header_files += 'linux/FloatingWidgetTitleBar.h'
  source_files += 'linux/FloatingWidgetTitleBar.cpp'
endif

source_files += [
  qt.compile_resources(sources: ['ads.qrc']),
  qt.compile_moc(headers: header_files),
]

ads = shared_library(
  'ads',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  extra_files: extra_files,
  dependencies: dependencies,
  override_options: {
    'werror': false
  },
  install: true,
  install_dir: 'bin',
)

ads_dep = declare_dependency(
  include_directories: ['.'],
  dependencies: dependencies,
  link_with: [ads],
)

all_deps += ads

