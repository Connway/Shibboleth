==============================
    Shibboleth Game Engine
==============================

Shibboleth is my pet project at developing a fully featured game engine. Shibboleth comes in two parts,
a simple application framework and the engine itself. Essentially, the game engine portion is just extensions
of the application framework.

=============
    GOALS
=============

The primary goal is to develop a game engine and editor that is highly extensible and as data driven
as possible (where it makes sense).

===============
    LICENSE
===============

Shibboleth is copyrighted under The MIT License. See LICENSE.txt for the full text of the license.

==========================
    BUILD INSTRUCTIONS
==========================

1. Install the Meson build system. (https://mesonbuild.com)
2. Install the Ninja build system. (https://ninja-build.org)
    a. Or install Visual Studio 2022.
3. Run "meson setup build/debug"
    a. If on Windows, run either: "meson setup build/debug --vsenv" or "meson setup build/debug_vs --vsenv --backend vs2022"
4. Run "meson compile -C build/debug".
    a. If using "vs2022" backend, then you can "Shibboleth.sln" in "build/debug_vs" and build from Visual Studio.
