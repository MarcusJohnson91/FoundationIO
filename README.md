**BitIO:**
========
* BitIO (Pronounced Bit Eye Oh) is a library for Reading, and Writing bits (and all the tasks associated with doing so, like swapping byte and bit order, etc).
* BitIO is designed for and tested on 64 bit CPUs (ARM64 and AMD64 to be specific), 32 bit operating systems and CPUs are not tested, and never will be.
* I've gone to great lengths to make sure BitIO is as fast as it can be, my bit readers and writers are generic functions to move branching to compile time instead of runtime for example.
* Also, I've tried keeping BitIO modern, minimalist, and easy to use. we require C99, along with a preprocessor that supports _Generic programming.
* BitIO is compatible with C++ through the C API, there are no wrappers because there's really no point in adding the additional baggage.
* Currently, there's no stable API, let alone ABI. I break shit constantly to keep it easy to use.

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
* libBitIO alone won't do a whole lot for you, it's real use is in being a library other libraries rely on.
* The makefile by defaults builds the library as a static library, for RELEASE, to change this set `BUILDTYPE` to `DEBUG`.
* BitIO is included as a submodule in git, so you don't need to install it if you're using one of my libraries, ModernPNG, ModernFLAC, ModernAVC, etc.
* On my Mac for AMD64, the libBitIO static library is about 200kb, if that's too big for your use, enable link time optimization on your program to trim unused functions.
*  **Microsoft** has decided to not support C11's `_Generic` preprocessor keyword, so you can't directly use the `ReadBits`/`PeekBits`/`ReadUnary`/`ReadExpGolomb`/`ReadGUUID`/`WriteBits`/`WriteUnary`/`WriteExpGolomb`/`WriteGUUID` macros, I've chosen to rely on this preprocessor keyword to ensure that there is minimal branching during runtime, to gain performance on accelerators like GPUs.
To solve this problem, you can hardcode it with the `Read`/`Write``As``ByteOrder``BitOrder` functions, or pester Microsoft to fix their support for the C11 standard.

How To Use libBitIO:
===================
In order to use libBitIO, you need to include CommandLineIO.h, or BitIO.h.
Tips:

* You can have as many instances of `BitInput`, `BitOutput`, and `BitBuffer` as you want, for instances, reading/writing multiple files.

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


Logging:
-------
`Log()`

* Writes to a file you set as `BitIOGlobalLogFile`, if it's unset or otherwise inaccessible, all logs are printed to stderr.
* Works on any platform that provides `stderr`, and `vsprintf`.

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

`CompareGUUIDs`: Compares GUUIDs of the same GUUIDType to each other.

`ConvertGUUIDString2BinaryGUUID`: Converts a GUUIDString to a BinaryGUUID (without swapping).

`ConvertBinaryGUUID2GUUIDString`: Converts a BinaryGUUID to a GUUIDString (without swapping).

`SwapGUUIDString`: Converts a GUUIDString to a GUUIDString of the opposite byte order.

`SwapBinaryGUUID`: Converts a BinaryGUUID to a BinaryGUUID of the opposite byte order.

`DeinitGUUID`: When you've had enough of GUUID nonsense, call this function to make your problems disappear.

TODO:
-----
* Write a WriteArray2BitBuffer and ReadBitBuffer2Array functions.

