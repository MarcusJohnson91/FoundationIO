**BitIO:** [![Build Status](https://travis-ci.org/MarcusJohnson91/FoundationIO.svg?branch=master)](https://travis-ci.org/MarcusJohnson91/FoundationIO)
========================
* BitIO (Pronounced Bit Eye Oh) is a library for Reading, and Writing bits (and all the tasks associated with doing so, like swapping endian, processing various entropy encoded streams, etc).
* BitIO uses HeaderDoc style formatting, which is compatible with Doxygen.
* BitIO is 64 bit native, but if a function is limited to a fewer number of bits (For example, Adler32 is limited to 32 bits), there's no reason to use more, so I don't.
* I've tried optimizing the memory and CPU usage of each function to be as low as possible, while still doing everything as well as possible.
* Also, I've tried keeping BitIO modern, by using the preprocessor as little as possible. Currently, it's exclusively used for including files, for the pragma once directive, and to ensure C++ compilers don't mistake it as C++.
* Currently, there's no stable API, let alone ABI. I constantly refactor BitIO to keep everything as simple as possible.

License:
--------

BitIO (meaning the libBitIO library) uses the BSD 3 clause (aka Revised BSD) license.

Before You Compile:
-------------------
* Make sure the values in `BitIOConstants` make sense to you.

Compiling:
----------
* The Makefile's default installation location is: `/usr/local/Packages/libBitIO`.
* You can change that by calling `make install` with: `PREFIX="/your/path/here" make install`, or just editing the makefile.
* The makefile by defaults builds the library as a static library, in release form, for the computer it's building on with -march=native.
* BitIO is included as a submodule in git, so you don't need to install it if you're using one of my libraries
* The makefile by defaults builds the library as a static library, in release form, for the computer it's building on with `-march=native`.
* to run the unit tests, simply call `make debug && make test && sudo make install` then `Test-BitIO <InputFile> <OutputFile>` if Test-BitIO doesn't print anything, all the tests passed.
* On my Mac, the BitIO library is about 75kb, if that's too big for your use, enable link time optimization on your program to trim unused functions.

How To Use BitIO:
-----------------
In order to use BitIO, you need to include BitIO.h.

* At the end of `main`, call `CloseBitInput`, `CloseBitOutput`, and `CloseBitBuffer` to dealloc all your instances of `BitInput`, `BitOutput`, and `BitBuffer`.

Input/Output Handling:
----------------------
* Tips:
** You can have multiple input and/or output files, at the same time, and/or you can iterate over format specified files with a single instance of them.
* `BitInput` and `BitOutput` contain input and output files, respectively; and data to manage them.
* `BitInput` and `BitOutput` are limited to dealing with files/sockets, for actual reading and writing to/from them, BitIO utilizes `BitBuffer`'s.
* To read data from or write data to a file, you can use `ReadBitInput2BitBuffer` or `WriteBitBuffer2BitOutput`.

Command Line Parsing:
---------------------
* Tips:
** `ParseCommandLineOptions` is zero indexed, your first switch/option should be 0.
** `ParseCommandLineOptions` handles flag prefixes `-`,` --`,` /` for you, do not include them in your flag's name.
** `ParseCommandLineOptions`'s flags are case insensitive.
** Your `Help` switch/option **MUST** be the last one; that way BitIO can automatically handle printing your help options for you.

* Initialize CommandLineOptions (and CommandLineSwitches) with `InitCommandLineOptions`.
* Then set your program wide options with:
** `SetCMDName`,
** `SetCMDVersion`,
** `SetCMDDescription`,
** `SetCMDAuthor`,
** `SetCMDCopyright`,
** `SetCMDLicense`,
** `SetCMDLicenseURL`,
** `SetCMDMinSwitches`.

* Set each switch/option with:
** `SetCMDSwitchFlag`,
** `SetCMDSwitchDescription`
** `SetCMDSwitchDependency`,
** `SetCMDSwitchResultStatus`,

* Get the result of your switches with:
** `GetCMDSwitchPresence`,
** `GetCMDSwitchResult`

* Then call `ParseCommandLineArguments`.
* The Help option **MUST** be the last one, that way BitIO can automatically handle responding to help calls; **It's inaccessible otherwise**.

`ReadUUID`:
* UUIDs and GUIDs have the same size, but use a different format, so `ReadUUID` can also read GUIDs.
* The only difference is that GUIDs are little endian.
* To convert a UUID to a GUID or vice versa call `ConvertUUID2GUID` or `ConvertGUID2UUID`.

`Log`:
* Log calls `openlog` on POSIX compatible platforms.
* Log calls `ReportEvent`on Windows.

License:
--------
BitIO is released under the 3 clause simplified BSD license. (I may relicense or dual license under the MIT license tho, not sure)

Here's a tl;dr of my license:
* **Don't** plagiarize BitIO.
* **Don't** relicense BitIO.
* **Don't** sue me if something goes wrong, or for using patented algorithms; I'm a programmer, not a patent clerk.
* **Do** Include in your acknowledgments a link to [BitIO on GitHub](https://www.github.com/MarcusJohnson91/FoundationIO).

Todo:
-----
* Add format specifier support to `OpenInputFile` and `OpenOutputFile`, and add a function to increment the file.
* Finish the CRC generator and verifier.
* Finish MD5 generator and verifier.
* Finish Deflate/Inflate (which includes Huffman and LZ77)
* Write a Reed-Soloman Error correction verifier.
* Write a LDPC encoder/decoder
* Write a Asymmetric Binary System encoder and decoder
* Change ReadFromMSB/WriteFromMSB functions to ReadFromLittleEndian/ReadFromBigEndian and WriteFromBigEndian/WriteFromLittleEndian
