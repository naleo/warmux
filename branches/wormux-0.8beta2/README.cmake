
Short CMake tutorial
====================

IMPORTANT NOTE:
Using cmake with wormux is optional!
It is still possible to use the old autotools system.


Introduction:
-------------

cmake has to be called with the path to the source-dir as a parameter.
e.g.:

     cmake /home/foobar/source/wormux

This makes it possible to have source and build directories seperated.
For example, you can build in /tmp/wormux-build and leave the source-dir untouched,
which makes it easier to remove all files generated by the build-system.

After running cmake (which just generates the Makefiles for the native make-system)
you have to call make from the build-dir (which can be the source-dir) in order
to get Wormux compiled:

     make

To install Wormux just call

     make install

This command install everything needed to run wormux in $CMAKE_INSTALL_PREFIX
which defaults to /usr/local (see below how to change that).


Further cmake options:
----------------------

To install wormux in another prefix, pass the following parameter to cmake:
     -DPREFIX=/foo/bar
(this corresponds to --prefix=/foo/bar with autotools-configure)

To specify another build-type, pass the following parameter:
     -DBUILD=XXXXX
where XXXXX is one of the following:
     None (Default), Debug, Release, RelWithDebInfo, MinSizeRel

To get a more verbose compilation output use:
     make VERBOSE=1

To enable profiling support (e.g. for gprof) add the option:
    -DPROFILE=1


Compilation outside of source-dir:
----------------------------------

     cd /path/to/wormux-src
     mkdir ../wormux-build
     cd ../wormux-build
     cmake ../wormux-src
     make
     make install

The Makefiles and CMake control files will be generated inside the new
wormux-build dir, while the source-dir stays clean.


Wormux development:
-------------------

If you add a new C++ sourcefile that needs to be compiled, you just have
to add a line with the relative path in src/SourceFiles.cmake.
If you create a new translation in the po subdirectory, you have to add
it in po/CMakeLists.txt.
After adding new data or source files you can also run the script SearchFiles.sh
in the cmake/ subdir.


Package generation:
-------------------

With CMake it is possible to generate .tar.gz archives of Wormux.
It is possible to generate ready-to-use binary archives (that only have to
get extracted to play wormux) and source archives for compilation.
To create binary archives issue the command:
     make package
Note that you should use build-type 'Release' with binary packages.
To create source achives you need to have a clean source tree and build
Wormux outside the source-dir (see above):
     make package_source


Win32 compilation:
------------------

I assume that Mingw is installed in /mingw.
Export the following variables so that CMake can find the rigth paths:
	export CMAKE_INCLUDE_PATH=/mingw/include
	export CMAKE_LIBRARY_PATH=/mingw/lib
Then run CMake:
	cmake ../wormux-src -G "MSYS Makefiles"


Problems:
---------

On problems with CMake remove the CMakeCache.txt files and CMakeFiles/
directories manually and call "cmake ." again.
If there are still problems, contact the Wormux Development Team by
email to <wormux-dev@gna.org> or via IRC at #wormux in Freenode.
