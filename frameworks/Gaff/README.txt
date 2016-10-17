============================
    Gaff Utility Library
============================

Gaff is a general purpose utility library that encompasses many functions. It was originally developed
to be cross platform, but development currently occurs primarily on Windows, with the intention of easily being ported to Linux and Mac.
I am trying to keep platform specific dependencies to a minimum, but when they do occur, I am isolating the platform dependencies.
This should hopefully make porting to other potential systems easier if necessary.

NOTE: Development primarily occurs in Shibboleth and occasionally gets backported to this repository!
For the latest version of Gaff, head to https://bitbucket.org/Connway/shibboleth.

=============
    GOALS
=============

With Gaff, and all my projects for that matter, I strive for simple, clean, and easy to read code with a simple,
yet organized file/folder structure.

Project files that are up-to-date on a platform should compile straight out of the box with no user setup,
unless library dependencies prohibit me from redistributing source. Under such circumstances, it should be as
easy as unzipping files into a specific folder.

I am always looking for feedback from code reviews! If you wish to spend your valuable time giving feedback,
send me a message on Bitbucket!

===============
    LICENSE
===============

Gaff is copyrighted under The MIT License. See LICENSE.txt for the full text of the license.

==========================
    BUILD INSTRUCTIONS
==========================

Includes Visual Studio 2013 solution/project and GCC makefiles. Both should compile out of the box.

GCC compiler version must support C++11. So unless you're running on something super ancient, you should be fine.

You may also easily drag and drop the source files into your project.

=============
    NOTES
=============

====================
    KNOWN ISSUES
====================

Saving TIFF files is broken in 64-bit builds. This is not in Gaff code, but in DevIL/ResIL code.
