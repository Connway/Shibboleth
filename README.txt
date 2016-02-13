==============================
    Shibboleth Game Engine
==============================

Shibboleth is my pet project at developing a fully featured game engine. Shibboleth comes in two parts,
a simple application framework and the engine itself. Essentially, the game engine portion is just extensions
of the application framework.

=============
    GOALS
=============

The primary goal is to develop a game engine and editor that is highly extensible and as data driven as possible (where it makes sense).

I am always looking for feedback from code reviews! If you wish to spend your valuable time giving feedback,
send me a message on Bitbucket!

===============
    LICENSE
===============

Shibboleth is copyrighted under The MIT License. See LICENSE.txt for the full text of the license.

==========================
    BUILD INSTRUCTIONS
==========================

Includes Visual Studio 2015 solution/project files and should compile out of the box.
Once done building, run workingdir/App/copy_dlls_and_exe.bat and
"workingdir/App/copy_similar_named_dlls.bat [debug/release] [32/64]" before running.
Otherwise the engine won't be able to find the DLLs it needs in order to run.

If you wish to generate your own solution files, just run "premake5 [vs2013/vs2015]".

If you want to use Brofiler, you will have to run the Python script "python.py [vs2013/vs2015] --brofiler".
The Python script takes the exact same command line arguments as Premake.

=============
    NOTES
=============

============================
    KNOWN ISSUES / TODOS
============================

A lot of data structures' member variables are organized for memory instead of hotness. This could
have performance implications if cache misses are frequent.

GameStates DLL takes a while to link sometimes.
