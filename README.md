# FoundationIO:
- FoundationIO is a library containing many components, namely BitIO, CommandLineIO, StringIO, LogIO, and TestIO.
- BitIO is a component for reading, and writing bits from/to files/sockets.
- CommandLineIO is a component for parsing command line arguments.
- StringIO is a component for Unicode, including UTF-8 and UTF-16 en/de coding, as well as normalization, casefolding, string formatting, string splitting, etc.
- Currently, there's no stable API, let alone ABI. I break shit constantly because i'm very indecisive.

# Versioning:
- FoundationIO's version scheme is based on Semantic versioning.
- What is Semantic Versioning?
* Semantic versioning is a scheme for versioning software.
* There are three digits, seperated by periods, that denote the Major, Minor, and Patch fields respectively in Semantic versioning, we add a fourth field, the CommitID; "Major.Minor.Patch:CommitID".
* The CommitID field is seperated from the rest of the version string with a colon ':'.
* The CommitID field should be entirely ignored by third party developers and users, unless you're filing a bug report.
* The purpose of the CommitID field is to track where exactly issues appeared and to help fix issues more quickly.
* So if you're using FoundationIO in one of your projects, feel free to use the Major, Minor, and Patch fields in the version string, but do not rely on the CommitID for anything.

# License:
FoundationIO is released under the terms of the 3 clause [`Revised BSD`](https://tldrlegal.com/license/bsd-3-clause-license-%28revised%29) license.

# Compiling:
* To use FoundationIO in your project, add it as a submodule with `git submodule add git@github.com:MarcusJohnson91/FoundationIO.git Dependencies/FoundationIO`, then include whatever component headers you want to use.
* Call `cmake` with `-GNinja -B"$(PWD)/../BUILD"`, the cmake file is in `/Projects`
* Call `ctest` after compiling to automatically test that everything works correctly.
* There is an Xcode and Visual Studio project in the `/Projects` folder as well, the VS projects are updated much less frequently than the Xcode ones.

# How To Use FoundationIO:

BitIO:
-----
* To use BitIO call `BitInput_Init` and `BitOutput_Init` for each file or socket you want to read from or write to.
* Reading and writing from/to files/sockets, requires a `BitBuffer` Init it with `BitBuffer_Init`.
* To open a file call `Bit(Input|Output)_UTF(8|16)_OpenFile` (except for the log file, which uses `Log_SetLogFile`).
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

`GUUIDTypes`: BinaryUUID, BinaryGUID, UUIDString, GUIDString

`ReadGUUID`:  Reads a GUUID of GUUIDType from the BitBuffer, and returns a pointer to the new GUUID.

`WriteGUUID`: Writes a GUUID of GUUIDType from a pointer into the BitBuffer.

`CompareGUUIDs`: Compares GUUIDs of the same GUUIDType to each other

`ConvertGUUIDString2BinaryGUUID`: Converts a GUUIDString to a BinaryGUUID (without swapping).

`ConvertBinaryGUUID2GUUIDString`: Converts a BinaryGUUID to a GUUIDString (without swapping).

`SwapGUUIDString`: Converts a GUUIDString to a GUUIDString of the opposite byte order.

`SwapBinaryGUUID`: Converts a BinaryGUUID to a BinaryGUUID of the opposite byte order.

`GUUID_Deinit`: When you've had enough of GUUID's nonsense, call this function to make your problems disappear.

TextIO:
------------
* TextIO isn't a sub-library or actually a real thing at all, it's just a way to help organize all the files, so the TextIO folders contain FoundationIO's text related components.

StringIO:
----------
* StringIO is a component for Unicode, it supports UTF-8, UTF-16, and UTF-32; but NOT CESU-8, WTF-8, or Java's Modified UTF-8.
* To create a Unicode string with StringIO simply include the StringIO header, and declare a string or character literal with the `UTF8`, `UTF16`, or `UTF32` type; then use the `UTF8String()`, `UTF16String()`, or `UTF32String()` macro to ensure the literal is encoded by the compiler correctly.
* StringIO's types `UTF8`, `UTF16`, `UTF32` are declared in the `TextIOTypes.h` header.
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

LogIO:
-----
* Writes to a file you open with `Log_SetLogFile`, if it's unset or otherwise inaccessible, all logs are printed to `stderr`.
* Works on any platform that provides `stderr`.

FormatIO:
-----------
* FormatIO is composed of the UTF(8|16|32)_Format and UTF(8|16|32)_Deformat functions.
* FormatIO's API is a bit different than the standard library.
- *_Format returns a pointer to the formatted string.
- *_Deformat returns a StringArray (just like argv), where each string starts at the offset of the specifier and ends whenever the type and modifier make sense for it to end
* Supported Extensions:
- Wide Character Support: POSIX extensions `lc`/`ls` and Microsoft extensions `C`/`S`.
- Positional Formatters: %X$Y format, where X is the position, and Y is the rest of the specifier (flags, modifiers, type specifier, etc)
- Pointer Formatters: %p/%P lower/upper case hexadecimal pointer.
- Binary Formatters: %b/%B 0bx/0BX prefixed binary strings.
* Custom Extensions:
- `U16`/`U32` length modifiers to c/C and s/S character and string format specifiers that accept a UTF-16 or UTF-32 character or string respectively.
- I'm in the process of standardizing the U16 and U32 length modifiers with ISO's WG14 and WG21, the C and C++ committees respectively.
* Unsupported Extensions:
- Microsoft's `Z` type specifier for "ASCIIString" or "UnicodeString" is not and likely will not be supported.
- the `n` specifier is unsupported because FormatIO's API doesn't require the user to allocate a destination pointer, though the parser will remove any `n` specifiers it finds from the output string.

TODO:
====
* StringIO:
- Finish the `Format/Deformat` work.
- Finish the `Decimal2String` and `String2Decimal` :  Based on Ryu, but it's a lot of work, so it's taking a while.
