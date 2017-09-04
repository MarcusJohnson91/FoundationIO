**BitIO:** [![Build Status](https://travis-ci.org/MarcusJohnson91/FoundationIO.svg?branch=master)](https://travis-ci.org/MarcusJohnson91/FoundationIO)
========================
* BitIO (Pronounced Bit Eye Oh) is a library for Reading, and Writing bits (and all the tasks associated with doing so, like swapping byte and bit order, etc).
* BitIO is designed for and tested on 64 bit CPUs (ARM64 and AMD64 to be specific), 32 bit OSes and CPUs are not tested, and never will be.
* I've gone to great lengths to make sure BitIO is as fast as it can be, my bit readers and writers are generic functions to move branching to compile time instead of runtime for exmaple.
* Also, I've tried keeping BitIO modern, minimalist, and easy to use. we require C99, along with a preprocessor that supports _Generic programming.
* BitIO is compatible with C++ through the C API, there are no wrappers because there's really no point in adding the additional baggage.
* Currently, there's no stable API, let alone ABI. I break shit constantly to keep it easy to use.

License:
--------
libBitIO is released under the terms of the 3 clause [`Revised BSD`](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29) license.

Here's a tl;dr of my license:

* **Do** include in your acknowledgments a link to [BitIO on GitHub](https://www.github.com/MarcusJohnson91/FoundationIO).
* **Don't** plagiarize BitIO.
* **Don't** relicense BitIO.
* **Don't** sue me if something goes wrong. I never have and never will gurantee BitIO works for anything, you figure out if it's right for you.

Compiling:
----------
* libBitIO alone won't do a whole lot for you, it's real use is in being a library other libraries rely on.
* When compiling you need to set the variables `ByteOrder` and `BitOrder` to `LSByte/MSByte` and `LSBit/MSBit` for the byte and bit order your CPU targets.
* The makefile by defaults builds the library as a static library, in release form, to change this set `BUILDTYPE` to DEBUG.
* BitIO is included as a submodule in git, so you don't need to install it if you're using one of my libraries, ModernPNG, ModernFLAC, ModernAVC, etc.
* On my Mac for AMD64, the libBitIO static library is about 200kb, if that's too big for your use, enable link time optimization on your program to trim unused functions.

How To Use libBitIO:
--------------------
In order to use libBitIO, you need to include CommandLineIO.h, or BitIO.h.
Tips:

* You can have as many instances of `BitInput`, `BitOutput`, and `BitBuffer` as you want, for instances, reading/writing multiple files.

CommandLineIO:
--------------

* To use CommandLineIO, in `main()` call `InitCommandLineIO` to create a pointer to the CommandLineIO struct.
* Create an enum to hold your command line switches. Number them starting at 0. your **help** option MUST be the last one.
* Call the `SetCLI*` group of functions to set generic, program wide information like your program's name, the license, etc.
* Call the `SetCLISwitch*` group of functions to set Switch specific options, like it's' flag, description, etc.
* We support dependent switches;
- To create a parent/child relationship between 2 switches, call `SetCLISwitchAsChild`.
- To create a Master switch (that is a switch that depends on nothing but multiple switches depend on it) call `SetCLISwitchAsMaster`.
* Call `ParseCommandLineIO` to parse argv's arguments for any matching switches.
* At the end of `main()` call `DeinitCommandLineIO` to clean everything up.

BitIO:
------
* To use BitIO in `main()` call `InitBitInput` and `InitBitOutput` to read from and write to files and sockets.
* Reading and writing from/to files/sockets, requires a `BitBuffer` Init that with `InitBitBuffer`.
* To open a file into BitInput call `OpenInputFile` and `OpenOutputFile` (except for the logging file, which uses `OpenLogFile`).
* to measure the size of the file in`BitInput`, call `GetFileSize`.
* To get the size of a file from `BitInput` call `GetBitInputFileSize`.
* To Read bits from a BitBuffer, use the `ReadBits` generic macro. to write bits to a BitBuffer use the `WriteBits` generic macro.
* To pull in fresh data from a file, call `ReadBitInput2BitBuffer`, and to write out fresh data call `WriteBitBuffer2BitOutput`.
* When you're all done, call `DeinitBitInput`, `DeinitBitOutput`, and `DeinitBitBuffer`.


Logging:
--------
`Log()` 

* Writes to a file you set as `BitIOGlobalLogFile`, if it's unset or otherwise inaccessible, all logs are printed to stderr.
* Works on any platform that provides `stderr`, and `vsprintf`.

UUID/GUIDs:
-----------

`ReadUUID`:

* UUIDs and GUIDs have the same size, but use a different format,  so `ReadUUID` can also read GUIDs.
* The only difference is that GUIDs are little endian.
* To convert a UUID to a GUID or vice versa call `ConvertUUID2GUID` or `ConvertGUID2UUID`.


TODO:
-----
* Add format specifier support to `OpenInputFile` and `OpenOutputFile`, and add a function to increment the file.
* Write a WriteArray2BitBuffer and ReadBitBuffer2Array functions.
* Make the makefile able to extract the TargetByteOrder and TargetBitOrder from the target architecture.
