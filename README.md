# FoundationIO:
- FoundationIO is a library containing many modules, namely BitIO, CommandLineIO, ContainerIO, StringIO, Log, and TestIO.
- BitIO is a module for reading, and writing bits from/to files/sockets.
- CommandLineIO is a module for parsing command line arguments.
- StringIO is a module for Unicode, including UTF-8 and UTF-16 en/de coding, as well as normalization, casefolding, string formatting, string splitting, etc.
- Currently, there's no stable API, let alone ABI. I break shit constantly because i'm very indecisive.

# Versioning:
- When complete, FoundationIO will use Semantic Versioning (Version X.Y.Z)
- X is the major version number, it will only be incremented when API breaking changes occur.
- Y is the minor version number, it will only be incremented when new features are added.
- Z is the patch version number, it reflects bug fixes and optimizations.
Al of this is theoretical until FoundationIO is at least version 1.0.0, it's currently beta, and I'm changing things fairly often.

# License:
FoundationIO is released under the terms of the 3 clause [`Revised BSD`](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29) license.

Here's a tl;dr of my license:

* **Do** If you permanently fork FoundationIO, change the name.
* **Do** include in your acknowledgments a link to [FoundationIO on GitHub](https://github.com/MarcusJohnson91/FoundationIO).
* **Don't** plagiarize FoundationIO, you didn't write it, don't claim that you did.
* **Don't** relicense FoundationIO, as the copyright holder, it's exclusively my choice what license to use.
* **Don't** sue me if something goes wrong, it's your responsibility to determine if FoundationIO is right for you.

# Compiling:
* to use FoundationIO in your project, add it as a submodule with `git submodule add Dependencies/FoundationIO git@gitlab.com:MarcusJohnson91/FoundationIO.git`, then include whatever module headers you want to use.
* call `cmake` with `-G "Unix Makefiles"`, the cmake file is in `/libFoundationIO`
* Then compile the makefile with make like usual.
* There is an Xcode and Visual Studio project in the `Projects` folder, tho the Visual Studio project may be out of date.

# How To Use FoundationIO:

StringIO:
----------
* StringIO is a module for Unicode, it supports UTF-8, UTF-16, and UTF-32; but NOT CESU-8, WTF-8, or Java's Modified UTF-8.
* To create a Unicode string with StringIO simply include the StringIO header, and declare a string or character literal with the `UTF8`, `UTF16`, or `UTF32` type; then use the `U8()`, `U16()`, or `U32()` macro to ensure the literal is encoded by the compiler correctly.
* StringIO's types `UTF8`, `UTF16`, `UTF32` are forward declared in the `BitIO`, `CommandLineIO`, `Log` modules; but not the Unicode stringization macros, those are only declared in StringIO.
* The functions in StringIO use UTF-32 internally; the vast majority of UTF-8 and UTF-16 functions are simply wrappers around the UTF-32 version.
* StringIO is meant to provide basic support for Unicode strings, and some of the more advanced operations on them, like case mapping, and de/normalization, but some things will never be on the map, like REGEX. StringIO's primary purpose is supporting input and output, not everything Unicode.

CommandLineIO:
-------------
* The CommandLineIO API is split in 2, there's UTF8, and UTF16 parallel functions. 
* POSIX derived systems tend to support UTF-8, and Windows derived systems tend to support UTF-16; Apple's Cocoa supports both, but prefers UTF-8.
* Options are zero indexed.
* Initialize CommandLineIO with `CommandLineIO_Init`
* CommandLineIO_UTF(8|16)_Set*, and CommandLineIO_Set* functions are for the overall program.
* CommandLineIO_UTF(8|16)_Switch*, and CommandLineIO_Switch* functions are for individual switches.
* Once you've created all of the options you need, and you've called your function to create the options at runtime, you need to call CommandLineIO_UTF(8|16)_ParseOptions.
* If the program you're creating is for POSIX compatible systems, just use argc and argv as the arguments to CommandLineIO_UTF(8|16)_ParseOptions
* If the program you're creating is for Windows, use the global variables __argc and __wargv, or you can call `GetCommandLineW` or `CommandLineToArgvW`
* There is one caveat for Windows, and that is that CommandLineIO assumes that the first argument is the path to the currently running executable, and therefore skips it. For Windows, the arguments need to be padded to align with that.

* **CommandLineIO's Interaction Model: Switches and Options**:
* **Switches: A Switch Is Behavior The Programmer Defines**
- Some switches require further arguments, like the user to enter a number, or a path to a file, or whatever, those are created by setting the switches type to `OptionHasArgMayHaveSlaves` or `OptionHasArgNoSlaves`, simplier switches that do not accept an argument from the user can be set to `OptionNoArgMayHaveSlaves` or `OptionNoArgNoSlaves`.
- You set a switches type by calling `CommandLineIO_SetSwitchType`
- A Switch's argument may also have a type, you can set that with `CLISetSwitchArgumentType`

* **Options: An Option Is Behavior The User Selects To Instruct The Program**

* A slave is an option that provides further context to another, indepedent switch.
* Only independent switches can have dependent switches.

* The types of Switches are:
- Switches may have slave switches, that is, a switch that can only be called in the context of it's master
- SwitchHasArgMayHaveSlave (This type of switch is used when you require the user to give additional information beyond the mere existence of the switch)

* The type of Options are:
- To create a Master switch (that is a switch that depends on nothing) call `SetCLISwitchAsMaster`.
* Call `ParseCommandLineIO` to parse argv's arguments for any matching switches.
* At the end of `main()` call `CommandLineIO_Deinit` to clean everything up.

BitIO:
-----
* To use BitIO call `BitInput_Init` and `BitOutput_Init` for each file or socket you want to read from or write to.
* Reading and writing from/to files/sockets, requires a `BitBuffer` Init it with `BitBuffer_Init`.
* To open a file call `Bit(Input|Output)_UTF(8|16)_OpenFile` (except for the log file, which uses `Log_OpenFile`).
* To get the size of a file from `BitInput` call `BitInput_GetFileSize`.
* To read bits from a BitBuffer, use the `ReadBits`/`PeekBits/ReadUnary`.
* To write bits to a BitBuffer use the `WriteBits/WriteUnary`.
* To pull in fresh data from a file, call `BitBuffer_Update`.
* To read a UTF-8 or UTF-16 string, call `ReadUTF(8|16)`. (You need to know the size of the string when you call ReadUTF(8|16)).
* To write a UTF-8 or UTF-16 string, call `WriteUTF(8|16)`. (The NULL terminator is NOT written, for that just call `BitBuffer_Skip`).
* When you're all done, call `BitInput_Deinit`, `BitOutput_Deinit`, and `BitBuffer_Deinit`.
- GUUIDs:
* A `GUUID` is just a handy way to refer to UUID/GUIDStrings, and BinaryUUID/GUIDs as one object.
* A GUUIDString is 21 bytes long, and includes a NULL terminator,  and dashes.
* A BinaryGUUID is 16 bytes long, and does not contain a NULL terminator or the dashes.
* A UUID is most significant byte first for each section.
* A GUID is least significant byte first for each section, except the last 6 bytes which are most significant.
* GUID's are used by Windows, while UUIDs are used by Unix derivatives.

* `GUUIDTypes`: BinaryUUID, BinaryGUID, UUIDString, GUIDString

`ReadGUUID`:  Reads a GUUID of GUUIDType from the BitBuffer, and returns a pointer to the new GUUID.

`WriteGUUID`: Writes a GUUID of GUUIDType from a pointer into the BitBuffer.

`CompareGUUIDs`: Compares GUUIDs of the same GUUIDType to each other

`ConvertGUUIDString2BinaryGUUID`: Converts a GUUIDString to a BinaryGUUID (without swapping).

`ConvertBinaryGUUID2GUUIDString`: Converts a BinaryGUUID to a GUUIDString (without swapping).

`SwapGUUIDString`: Converts a GUUIDString to a GUUIDString of the opposite byte order.

`SwapBinaryGUUID`: Converts a BinaryGUUID to a BinaryGUUID of the opposite byte order.

`GUUID_Deinit`: When you've had enough of GUUID's nonsense, call this function to make your problems disappear.

Log:
-----
* Writes to a file you open with `Log_OpenFile`, if it's unset or otherwise inaccessible, all logs are printed to `stderr`.
* Works on any platform that provides `stderr`, and for ones that don't open a user specified file with `Log_OpenFile`

TODO:
====
* StringIO:
- Finish the `FormatString` work.
- Finish the `Decimal2String` and `String2Decimal` functions in StringIO.
