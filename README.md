# FoundationIO:
- FoundationIO is a library containing many components, namely BitIO, CommandLineIO, StringIO, LogIO, and TestIO.
- BitIO is a component for reading, and writing bits from/to files/sockets.
- CommandLineIO is a component for parsing command line arguments.
- StringIO is a component for Unicode, including UTF-8 and UTF-16 en/de coding, as well as normalization, casefolding, string formatting, string splitting, etc.
- Currently, there's no stable API, let alone ABI. I break shit constantly because i'm very indecisive.

# Versioning:
- FoundationIO's version scheme is based on Semantic versioning; there's an API level and ABI level for each component in FoundationIO.
* Format: APILevel.ABILevel.CommitID

# License:
=======
* Contributions are accepted only under the terms of the [‘Unlicense’](https://tldrlegal.com/license/unlicense) license.
* By contributing to this project, you agree that the project can relicense at any time for any reason, including your contributions.
* FoundationIO is released under the terms of the Apache 2.0 license, see LICENSE for more information.


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
* ExtensibleIO: XML pull parser, work in progress.

# TODO:
* StringIO:
----
- Finish the `Format/Deformat` work.
- Finish the `Decimal2String` and `String2Decimal` :  Based on Ryu, but it's a lot of work, so it's taking a while.
----
* FormatIO:
- l16 and l32 modifiers being standardized with ISO WG14, and being accepted into Clang.
----
* ExtensibleIO:
- Finish the parsing, the main complication is having the tokenizer create string references instead of copying everything everywhere.
