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
Once done building, run workingdir/App/copy_dlls.bat before running. Otherwise the engine
won't be able to find the DLLs it needs in order to run.

=============
    NOTES
=============

============================
    KNOWN ISSUES / TODOS
============================

A lot of data structures' member variables are organized for memory instead of hotness. This could have performance implications if cache misses are frequent.

GameStates DLL takes a while to link sometimes.
