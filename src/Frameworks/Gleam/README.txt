===============================
    Gleam Graphics Library
===============================

Gleam is a DirectX 11 and OpenGL 4.3 graphics rendering library. It also contains cross-platform
window and input classes for getting basic windows up and running and ready to be drawn into.

Gleam currently only supports Windows. Linux was supported at one time, and hopefully will be again
in the future, but right now I just don't have the time. OpenGL 4.3 support is also lagging a little
behind and is currently not as fully featured as the DirectX 11 implementation.

NOTE: Development primarily occurs in Shibboleth and occasionally gets backported to this repository!
For the latest version of Gleam, head to https://bitbucket.org/Connway/shibboleth.

=========================
    WHAT GLEAM IS NOT
=========================

Gleam is not a full-blown graphics engine! It does not auto-magically do everything for you.

It does not:
    * Load a bunch of image formats and convert them into textures
    * Load a bunch of model formats
    * Do <some rendering technique> automatically for you
    * Load and render fonts

HOWEVER, Gleam has a set of building blocks to help build those things.
Gleam provides functionality that you would get by calling appropriate DirectX/OpenGL calls, but
wrapped up in a cross-platform API. So, instead of worrying about doing specific API calls, you
only need to deal with the concept that those API calls encompass.

If you think of the where Gleam lives in your application:

--------------------    ---------    -------------------
| DirectX / OpenGL | -> | Gleam | -> | Graphics Engine |
--------------------    ---------    -------------------

=============
    GOALS
=============

With Gleam, and all my projects for that matter, I strive for simple, clean, and easy to read code with a simple,
yet organized file/folder structure.

Project files that are up-to-date on a platform should compile straight out of the box with no user setup,
unless library dependencies prohibit me from redistributing source. Under such circumstances, it should be as
easy as unzipping files into a specific folder.

I am always looking for feedback from code reviews! If you wish to spend your valuable time giving feedback,
send me a message on Bitbucket!

===============
    LICENSE
===============

Gleam is copyrighted under The MIT License. See LICENSE.txt for the full text of the license.

==========================
    BUILD INSTRUCTIONS
==========================

Includes Visual Studio 2015 solution/project and a GCC makefile. Both should compile out of the box.

GCC compiler version must support C++11. So unless you're running on something super ancient, you should be fine.

You may also easily drag and drop the source files into your project.

LINUX PACKAGE DEPENDENCIES:
    libx11-dev
    mesa-common-dev
    libglu1-mesa-dev
    libxrandr-dev
    libxi-dev

=============
    NOTES
=============
