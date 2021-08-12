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

# Overview:
* FoundationIO is a library for handling all the basics every application needs, like Unicode handling, math functions, command line parsing, file and network operations, etc.

# FoundationIO:
* AsynchronousIO: File and network operations in an asynchronous manner.
* BufferIO: operations on 1D arrays
* CryptographyIO: Hashing and encryption functions.
* FileIO: Built on top of AsynchronousIO, specialized for filesystem access.
* MathIO: Math functions.
* NetworkIO: Built on top of AsynchronousIO, specialized for network access.
* PlatformIO: Macros to abstract away the platform it's compiled for/on.
* TestIO: unit testing framework.

TextIO:
--------
TextIO is not a sub library, it's all part of FoundationIO; it exists just to concentrate the text handling components of FoundationIO.

* TextIOTypes: Macros and types for text support, basically PlatformIO, but for Unicode.
* TextIOTables: For internal use only, the tables, their values, and even names are intentionally changed; do not use TextIOTables directly.
* CommandLineIO: Building a command line interface.
* FormatIO: Printf-like string formatting.
* LocalizationIO: Localization, like how to format numbers for each culture.
* LogIO: Generic logging functionality.
* StringIO: Generic Unicode handling facilities, number to string and vice versa conversions.
* SyntaxIO: XML parsing, work in progress.

# TODO:
* StringIO:
----
- Finish the `Format/Deformat` work.
- Finish the `Decimal2String` and `String2Decimal` :  Based on Ryu, but it's a lot of work, so it's taking a while.
----
* FormatIO:
- l16 and l32 modifiers being standardized with ISO WG14, and being accepted into Clang.
----
* SyntaxIO:
- Finish the parsing, the main complication is having the tokenizer create string references instead of copying everything everywhere.
