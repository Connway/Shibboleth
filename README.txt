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

Includes Visual Studio 2017 solution/project files and should compile out of the box.

If you wish to generate your own solution files, just run "utils/premake5 vs2017".


========================
    WORK IN PROGRESS
========================

Researching replacing Editor_App. Going from C++ using wxWidgets to using C# and Avalonia. 
Getting runtime scene loading with archetype overrides.

Things to think about: C# calling into C++ code and doing operations on C++ side. I don't want to have to make managed C++ wrappers to have interop with C#. That's a giant sink of time and not exactly a great architectural decision.
