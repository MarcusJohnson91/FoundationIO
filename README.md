
**FoundationIO:**
========
* FoundationIO is a library for reading, and writing bits
* FoundationIO is designed for and tested on 64 bit CPUs (AMD64 and ARM64 to be specific), 32 bit operating systems and CPUs are not tested, and never will be.
* I've tried keeping FoundationIO modern, minimalist, and easy to use. we require a C99 compiler.
* FoundationIO is compatible with C++ through the C API.
* Currently, there's no stable API, let alone ABI. I break shit constantly because i'm very indecisive.

License:
=======
FoundationIO is released under the terms of the 3 clause [`Revised BSD`](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29) license.

Here's a tl;dr of my license:

* **Do** If you permanently fork FoundationIO, change the name.
* **Do** include in your acknowledgments a link to [FoundationIO on GitHub](https://www.github.com/MarcusJohnson91/FoundationIO).
* **Don't** plagiarize FoundationIO.
* **Don't** relicense FoundationIO.
* **Don't** sue me if something goes wrong. I never have and never will guarantee FoundationIO works for anything, you figure out if it's right for you.

Compiling:
=========
* to use FoundationIO in your project, add it as a submodule with `git submodule add Dependencies/FoundationIO https://github.com/MarcusJohnson91/FoundationIO.git`, then include whatever library headers you want to use.
* call `cmake` with `-G "Unix Makefiles"`, the cmake file is in the Projects folder.
* Then compile the makefile with make like usual.

How To Use FoundationIO:
===================

StringIO:
----------
* StringIO provides FoundationIO with Unicode string handling, it supports decoding and encoding both UTF-8 (NOT CESU-8, WTF-8, or Java's Modified UTF-8) and UTF-16 (actual UTF-16 with surrogate pairs, not just UCS-2).
* If you need any of those formats (CESU-8, WTF-8, or Java's mUTF-8, or UCS-2), feel free to send a pull request.
* The functions in StringIO use UTF-32 internally.
* StringIO is meant to provide basic support for Unicode strings, and some of the more advanced operations on them, like case mapping, and de/normalization, but some things will never be on the map, like REGEX. StringIO's primary purpose is suppoing input and output, not everything Unicode.

CommandLineIO:
-------------
* **CommandLineIO's Interaction Model: Switches and Options**:
* **Switches: A Switch Is Behavior The Programmer Defines**
- Some switches require further arguments, like the user to enter a number, or a path to a file, or whatever, those are created by setting the switches type to `SwitchHasArgMayHaveSlaves` or `SwitchHasArgNoSlaves`, simplier switches that do not accept an argument from the user can be set to `SwitchNoArgMayHaveSlaves` or `SwitchNoArgNoSlaves`.
- You set a switches type by calling `CLISetSwitchType`
- A Switch's argument may also have a type, you can set that with `CLISetSwitchArgumentType`

* **Options: An Option Is Behavior The User Selects To Instruct The Program**

* A slave is an option that provides further context to another, indepedent switch.
* Only independent switches can have dependent switches.

* To use CommandLineIO, in `main()` call `CommandLineIO_Init` to create a CommandLineIO instance.
* Create an enum to hold your command line switches, it REALLY simplifies things.
* Call the `CLISet*` group of functions to set generic, program wide information like your program's name, the license, etc.
* Call the `CLISetSwitch*` group of functions to set Switch specific options, like it's flag, description, etc.
* In CommandLineIO's model, there is a Switch which is an interface the programmer provides to the user, and an Option, which the user provides to the programmer.

* The types of Switches are:
- Switches may have slave switches, that is, a switch that can only be called in the context of it's master
- SwitchHasArgMayHaveSlave (This type of switch is used when you require the user to give additional information beyond the mere existence of the switch)
* The type of Options are:
-
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

Log:
-------
* Writes to a file you open with `Log_OpenFile`, if it's unset or otherwise inaccessible, all logs are printed to stderr.
* The interface requires UTF8 which you can coerce your strings to be with the `u8` string literal prefix, or just calling the `U8` macro
* Works on any platform that provides `stderr`, and `fprintf`.

GUUIDs:
----------
* A `GUUID` is just a handy way to refer to UUID/GUIDStrings, and BinaryUUID/GUIDs as one object.
* A GUUIDString is 21 bytes long, and includes a NULL terminator,  and the dashes.
* A BinaryGUUID is 16 bytes long, and does not contain a NULL terminator or the dashes.
* A UUID is most significant byte first for each section.
* A GUID is least significant byte first for each section, except the last 6 bytes which are most significant.

* GUUIDTypes: BinaryUUID, BinaryGUID, UUIDString, GUIDString

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

