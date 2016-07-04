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

I am always looking for feedback from code reviews! If you wish to spend your valuable time giving
feedback, send me a message on Bitbucket!

===============
    LICENSE
===============

Shibboleth is copyrighted under The MIT License. See LICENSE.txt for the full text of the license.

==========================
    BUILD INSTRUCTIONS
==========================

Includes Visual Studio 2015 solution/project files and should compile out of the box.
Once done building, run "utils/copy_dlls_and_exe.bat" and
"utils/copy_similar_named_dlls.bat [debug/release] [32/64]" before running.
Otherwise the engine won't be able to find the DLLs it needs in order to run.

If you wish to generate your own solution files, just run "utils/premake5 [vs2013/vs2015]".

For building the editor, you must will have to install Qt 5.6.0 or later and define either QTDIR or
QT_DIR environment variables. Or you can run 'utils/premake5 [vs2013/vs2015] --qt_dir="<dir_to_qt>"'
to generate solutions with your version of Qt. The Premake script also accepts
"--qt_prefix=<my_version_prefix>" and will default to Qt5.


If you wish to use any of the libraries mentioned below (Brofiler, PhysX), you will have to use the
"premake5.py" Python script, as some vcxproj fixes are applied post solution/project generation.
The Python script takes the exact same arguments as Premake.

================
    BROFILER
================
If you want to use Brofiler, you will have to run the Python script "utils/premake5.py [vs2013/vs2015] --brofiler".
Currently, the code has not been instrumented, but all the compilation and linking setup for Brofiler is finished.

=============
    PHYSX
=============
If you want to use PhysX, you will have to download your own copy of the PhysX source code and unzip
to dependencies/PhysX.

Folder structure will look like this:
dependencies/PhysX
  - APEXSDK
  - PHYSXSDK
  - PhysXVisualDebugger

Currently there are no PhysX managers or components. Building the PhysX SDK is currently set up.
If you want to build the Apex SDK, you will have to set that up yourself.

PhysX is not currently supported. You will have to write all the managers and components yourself if
you wish to use PhysX.

In order to use PhysX, you will need to run "utils/premake5.py [vs2013/vs2015] --physx".


============================
    KNOWN ISSUES / TODOS
============================

A lot of data structures' member variables are organized for memory instead of hotness. This could
have performance implications if cache misses are frequent.
