**BitIO:**
========
* BitIO (Pronounced Bit Eye Oh) is a library for Reading, and Writing bits (and all the tasks associated with doing so, like swapping byte and bit order, etc).
* BitIO is designed for and tested on 64 bit CPUs (AMD64 and ARM64 to be specific), 32 bit operating systems and CPUs are not tested, and never will be.
* I've tried keeping BitIO modern, minimalist, and easy to use. we require a C99 compiler.
* BitIO is compatible with C++ through the C API.
* Currently, there's no stable API, let alone ABI. I break shit constantly because i'm very indecisive.

License:
=======
libBitIO is released under the terms of the 3 clause [`Revised BSD`](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29) license.

Here's a tl;dr of my license:

* **Do** If you fork BitIO, change the name.
* **Do** include in your acknowledgments a link to [BitIO on GitHub](https://www.github.com/MarcusJohnson91/FoundationIO).
* **Don't** plagiarize BitIO.
* **Don't** relicense BitIO.
* **Don't** sue me if something goes wrong. I never have and never will guarantee BitIO works for anything, you figure out if it's right for you.

Compiling:
=========
* to use BitIO in your project, add it as a submodule with `git submodule add Dependencies/BitIO BITIO_URL`, then include whatever sub library headers you want to use.
* call `cmake` with `-G "Unix Makefiles"`, the cmake file is in the Projects folder.
* Then compile the makefiles with make like usual.

How To Use libBitIO:
===================
In order to use libBitIO, you need to include CommandLineIO.h, or BitIO.h.
Tips:

* You can have as many instances of `BitInput`, `BitOutput`, and `BitBuffer` as you want, for instance, reading/writing multiple files.

StringIO:
----------
* StringIO provides BitIO with Unicode string handling, it supports decoding and encoding both UTF-8 (NOT CESU-8, WTF-8, or Java's Modified UTF-8) and UTF-16 (actual UTF-16 with surrogate pairs, not just UCS-2).
* If you need any of those formats (CESU-8, WTF-8, or Java's mUTF-8, or UCS-2), feel free to send a pull request.
* The functions in StringIO use UTF-32 internally, that way we can use ANY codepoint in Unicode, StringIO is not just limited to the BMP.
* StringIO is meant to provide basic support for Unicode strings, and some of the more advanced operations on them, like case mapping, and de/normalization, but some things will never be on the map, like REGEX. StringIO's primary purpose is suppoing input and output, not everything Unicode.

CommandLineIO:
-------------
* **Independent/Dependent Switches**
===============================
* A dependent switch is one that provides further context to another, indepedent switch.
* Only independent switches can have dependent switches.

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
-----
* To use BitIO in `main()` call `InitBitInput` and `InitBitOutput` to read from and write to files and sockets.
* Reading and writing from/to files/sockets, requires a `BitBuffer` Init it with `InitBitBuffer`.
* To open a file call `BitInputOpenFile` or `BitOutputOpenFile` (except for the logging file, which uses `OpenLogFile`).
* To measure the size of the file in`BitInput`, call `BitInputGetFileSize`.
* To get the size of a file from `BitInput` call `GetBitInputFileSize`.
* To read bits from a BitBuffer, use the `ReadBits`/`PeekBits` generic macro. to write bits to a BitBuffer use the `WriteBits` generic macro.
* To pull in fresh data from a file, call `BitBufferReadFromBitInput`, and to write out fresh data call `BitBufferWrite2BitOutput`.
* When you're all done, call `DeinitBitInput`, `DeinitBitOutput`, and `DeinitBitBuffer`.


BitIOLog:
-------
* Writes to a file you open with `BitIOLogOpenFile`, if it's unset or otherwise inaccessible, all logs are printed to stderr.
* The interface requires UTF8 which you can coerce your strings to be with the `u8` string literal prefix.
* Works on any platform that provides `stderr`, and `fprintf`.

GUUIDs:
----------
* A `GUUID` is just a handy way to refer to UUID/GUIDStrings, and BinaryUUID/GUIDs as one object.
* A GUUIDString is 21 bytes long, and includes a NULL terminator,  and the dashes.
* A BinaryGUUID is 16 bytes long, and does not contain a NULL terminator or the dashes.
* A UUID is most significant byte first for each section.
* A GUID is least significant byte first for each section, except the last 6 bytes which are most significant.

* GUUIDType: BitIOBinaryUUID, BitIOBinaryGUID, BitIOUUIDString, BitIOGUIDString

`ReadGUUID`:  Reads a GUUID of GUUIDType from the BitBuffer, and returns a pointer to the new GUUID.

`WriteGUUID`: Writes a GUUID of GUUIDType from a pointer into the BitBuffer.

`CompareGUUIDs`: Compares GUUIDs of the same GUUIDType to each other

`ConvertGUUIDString2BinaryGUUID`: Converts a GUUIDString to a BinaryGUUID (without swapping).

`ConvertBinaryGUUID2GUUIDString`: Converts a BinaryGUUID to a GUUIDString (without swapping).

`SwapGUUIDString`: Converts a GUUIDString to a GUUIDString of the opposite byte order.

`SwapBinaryGUUID`: Converts a BinaryGUUID to a BinaryGUUID of the opposite byte order.

`DeinitGUUID`: When you've had enough of GUUID's nonsense, call this function to make your problems disappear.

TODO:
-----
* Write a WriteArray2BitBuffer and ReadBitBuffer2Array functions, that way I could theoretically at least use SIMD operations on the data.
* Finish writing the UCD parsing script so I can add full support for case-mapping and de/normalization to StringIO.
* Finalize the cmake build script.
