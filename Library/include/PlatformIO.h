/*!
 @header          PlatformIO.h
 @author          Marcus Johnson
 @copyright       2017+
 @version         2.0.0
 @brief           This header contains preprocessor macros for generic functions in FoundationIO, and cross-platform compatibility.
 */

#pragma once

#ifndef FoundationIO_PlatformIO_H
#define FoundationIO_PlatformIO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef             Yes
#define             Yes                                                                 (1)
#else
#undef              Yes
#define             Yes                                                                 (1)
#endif

#ifndef             No
#define             No                                                                  (0)
#else
#undef              No
#define             No                                                                  (0)
#endif

#ifndef             True
#define             True                                                                (1)
#else
#undef              True
#define             True                                                                (1)
#endif

#ifndef             False
#define             False                                                               (0)
#else
#undef              False
#define             False                                                               (0)
#endif

#ifndef             PlatformIO_TargetOSIsUnknown
#define             PlatformIO_TargetOSIsUnknown                                        (0)
#endif

#ifndef             PlatformIO_TargetOSIsClassicMac
#define             PlatformIO_TargetOSIsClassicMac                                     (1)
#endif

#ifndef             PlatformIO_TargetOSIsWindows
#define             PlatformIO_TargetOSIsWindows                                        (2)
#endif

#ifndef             PlatformIO_TargetOSIsPOSIX
#define             PlatformIO_TargetOSIsPOSIX                                          (4)
#endif

#ifndef             PlatformIO_TargetOSIsApple
#define             PlatformIO_TargetOSIsApple                                          (8)
#endif              /* MacOS, iOS, iPadOS, TVOS, WatchOS, etc */

#ifndef             PlatformIO_TargetOSIsBSD
#define             PlatformIO_TargetOSIsBSD                                            (16)
#endif              /* FreeBSD, OpenBSD, NetBSD, DragonFlyBSD, etc */

#ifndef             PlatformIO_TargetOSIsLinux
#define             PlatformIO_TargetOSIsLinux                                          (32)
#endif              /* Ubuntu, CentOS, Android, etc */

#ifndef             PlatformIO_ArchIsAMD64
#define             PlatformIO_ArchIsAMD64                                              (1)
#endif

#ifndef             PlatformIO_ArchIsARM64
#define             PlatformIO_ArchIsARM64                                              (2)
#endif

#ifndef             PlatformIO_ArchIsMIPS64
#define             PlatformIO_ArchIsMIPS64                                             (3)
#endif

#ifndef             PlatformIO_ArchIsRISCV64
#define             PlatformIO_ArchIsRISCV64                                            (4)
#endif

#ifndef             PlatformIO_LanguageVersionC99
#define             PlatformIO_LanguageVersionC99                                       (199901UL)
#endif

#ifndef             PlatformIO_LanguageVersionC11
#define             PlatformIO_LanguageVersionC11                                       (201112UL)
#endif

#ifndef             PlatformIO_LanguageVersionC18
#define             PlatformIO_LanguageVersionC18                                       (201710UL)
#endif

#ifndef             PlatformIO_LanguageVersionC23
#define             PlatformIO_LanguageVersionC23                                       (202302UL)
#endif

#ifndef             PlatformIO_LanguageVersionCXX98
#define             PlatformIO_LanguageVersionCXX98                                     (199711UL)
#endif

#ifndef             PlatformIO_LanguageVersionCXX11
#define             PlatformIO_LanguageVersionCXX11                                     (201103UL)
#endif

#ifndef             PlatformIO_LanguageVersionCXX14
#define             PlatformIO_LanguageVersionCXX14                                     (201402UL)
#endif

#ifndef             PlatformIO_LanguageVersionCXX17
#define             PlatformIO_LanguageVersionCXX17                                     (201703UL)
#endif

#ifndef             PlatformIO_LanguageVersionCXX20
#define             PlatformIO_LanguageVersionCXX20                                     (202002UL)
#endif

#ifndef             PlatformIO_POSIX1
#define             PlatformIO_POSIX1                                                   (199009UL)
#endif

#ifndef             PlatformIO_POSIX2
#define             PlatformIO_POSIX2                                                   (199209UL)
#endif

#ifndef             PlatformIO_POSIX1993
#define             PlatformIO_POSIX1993                                                (199309UL)
#endif

#ifndef             PlatformIO_POSIX1995
#define             PlatformIO_POSIX1995                                                (199506UL)
#endif

#ifndef             PlatformIO_POSIX2001
#define             PlatformIO_POSIX2001                                                (200112UL)
#endif

#ifndef             PlatformIO_POSIX2008
#define             PlatformIO_POSIX2008                                                (200809UL)
#endif

#ifndef             PlatformIO_CompilerIsUnknown
#define             PlatformIO_CompilerIsUnknown                                        (0)
#endif

#ifndef             PlatformIO_CompilerIsClang
#define             PlatformIO_CompilerIsClang                                          (1)
#endif

#ifndef             PlatformIO_CompilerIsMSVC
#define             PlatformIO_CompilerIsMSVC                                           (2)
#endif

#ifndef             PlatformIO_CompilerIsGCC
#define             PlatformIO_CompilerIsGCC                                            (3)
#endif

#ifndef             PlatformIO_LanguageIsC
#define             PlatformIO_LanguageIsC                                              (1)
#endif

#ifndef             PlatformIO_LanguageIsCXX
#define             PlatformIO_LanguageIsCXX                                            (2)
#endif

#ifndef             PlatformIO_BuildTypeIsUnknown
#define             PlatformIO_BuildTypeIsUnknown                                       (0)
#endif

#ifndef             PlatformIO_BuildTypeIsDebug
#define             PlatformIO_BuildTypeIsDebug                                         (1)
#endif

#ifndef             PlatformIO_BuildTypeIsRelease
#define             PlatformIO_BuildTypeIsRelease                                       (2)
#endif

#ifndef             PlatformIO_ByteOrder_Unknown
#define             PlatformIO_ByteOrder_Unknown                                        (0)
#endif

#ifndef             PlatformIO_ByteOrder_BE
#define             PlatformIO_ByteOrder_BE                                             (1)
#endif

#ifndef             PlatformIO_ByteOrder_LE
#define             PlatformIO_ByteOrder_LE                                             (2)
#endif

#ifndef             PlatformIO_NULLTerminator
#define             PlatformIO_NULLTerminator                                           (0)
#endif

#ifndef             PlatformIO_NULLTerminatorSize
#define             PlatformIO_NULLTerminatorSize                                       (1)
#endif

#ifndef             PlatformIO_UNCPathPrefix8
#define             PlatformIO_UNCPathPrefix8                                           UTF8String("//?/")
#endif

#ifndef             PlatformIO_InvisibleString32
#define             PlatformIO_InvisibleString32                                        UTF32String("\u00A0")
#endif

#ifndef             PlatformIO_WideCharTypeIsUTF32
#define             PlatformIO_WideCharTypeIsUTF32                                      (1)
#endif

#ifndef             PlatformIO_WideCharTypeIsUTF16
#define             PlatformIO_WideCharTypeIsUTF16                                      (2)
#endif

#ifndef             PlatformIO_LanguageVersionC
#define             PlatformIO_LanguageVersionC                                         (__STDC_VERSION__)
#endif

#ifndef             PlatformIO_LanguageVersionCXX
#define             PlatformIO_LanguageVersionCXX                                       (__cplusplus)
#endif

#ifndef             PlatformIO_Language
#ifdef              __cplusplus
#define             PlatformIO_Language                                                 PlatformIO_LanguageIsCXX
#else
#define             PlatformIO_Language                                                 PlatformIO_LanguageIsC
#endif /* __cplusplus */
#endif /* PlatformIO_Language */

#ifndef             PlatformIO_BuildType
#if defined(_DEBUG) || defined(DEBUG)
#define             PlatformIO_BuildType                                                PlatformIO_BuildTypeIsDebug
#else
#define             PlatformIO_BuildType                                                PlatformIO_BuildTypeIsRelease
#endif /* Debug defined */
#endif /* PlatformIO_BuildType */

#ifndef             PlatformIO_Architecture
#if   defined(__amd64__) || defined(__x86_64__)
#define             PlatformIO_Architecture                                             (PlatformIO_ArchIsAMD64)
#elif defined(__aarch64__) && (__ARM_64BIT_STATE == 1)
#define             PlatformIO_Architecture                                             (PlatformIO_ArchIsARM64)
#elif defined(__mips64__)
#define             PlatformIO_Architecture                                             (PlatformIO_ArchIsMIPS64)
#elif defined(__riscv) && (__riscv_xlen == 64)
#define             PlatformIO_Architecture                                             (PlatformIO_ArchIsRISCV64)
#endif /* Target checking */
#endif /* PlatformIO_Architecture */

#ifndef             PlatformIO_ArchHasSIMDExtensions
#if   defined(__ARM_NEON)
#define             PlatformIO_ArchHasSIMDExtensions                                    (1)
#elif defined(__AVX__) || defined(__AVX2__) || defined(__AVX512__)
#define             PlatformIO_ArchHasSIMDExtensions                                    (1)
#elif defined(__MIPS_3D__)
#define             PlatformIO_ArchHasSIMDExtensions                                    (1)
#endif /* Target checks */
#endif /* PlatformIO_ArchHasSIMDExtensions */

    /*!
     @abstract      PlatformIO_Immutable is for pointers and arrays.
     @remark        Makes the pointer and the data it points to constant.
     */
#ifndef             PlatformIO_Immutable
#define             PlatformIO_Immutable(PointerTypeWithStar)                           const PointerTypeWithStar const
#endif /* PlatformIO_Immutable */

#ifndef             PlatformIO_Mutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#define             PlatformIO_Mutable(PointerType, Variable)                           (PointerType) Variable
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define             PlatformIO_Mutable(PointerType, Variable)                           const_cast<PointerType>(Variable)
#endif /* PlatformIO_Language */
#endif /* PlatformIO_Mutable */

    /*!
     @abstract      PlatformIO_Volatile is for pointers and arrays.
     @remark        Makes the pointer and the data it points to volatile.
     */
#ifndef             PlatformIO_Volatile
#define             PlatformIO_Volatile(PointerType)                                    volatile PointerType volatile
#endif /* PlatformIO_Volatile */

#ifndef             PlatformIO_Literal
#define             PlatformIO_Literal(NewType, UnconstType, Literal)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#undef              PlatformIO_Literal
#define             PlatformIO_Literal(NewType, UnconstType, Literal)                   (NewType const) Literal
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#undef              PlatformIO_Literal
#define             PlatformIO_Literal(NewType, UnconstType, Literal)                   reinterpret_cast<NewType>(const_cast<NewType>(Literal))
#endif /* PlatformIO_Language */
#endif /* PlatformIO_Literal */

#ifndef             PlatformIO_FunctionName
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#define             PlatformIO_FunctionName                                             (ImmutableString_UTF8) __func__
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define             PlatformIO_FunctionName                                             reinterpret_cast<const UTF8*>(__PRETTY_FUNCTION__)
#endif /* Language */
#endif /* PlatformIO_FunctionName */

#if  !defined(PlatformIO_TargetOS)
#if   defined(__APPLE__) && defined(__MACH__)
#define             PlatformIO_TargetOS                                                 (PlatformIO_TargetOSIsPOSIX | PlatformIO_TargetOSIsApple)
#elif defined(Macintosh) || defined(macintosh)
#define             PlatformIO_TargetOS                                                 (PlatformIO_TargetOSIsClassicMac)
#elif defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
#define             PlatformIO_TargetOS                                                 (PlatformIO_TargetOSIsWindows)
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#define             PlatformIO_TargetOS                                                 (PlatformIO_TargetOSIsPOSIX | PlatformIO_TargetOSIsBSD)
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__ANDROID__)
#define             PlatformIO_TargetOS                                                 (PlatformIO_TargetOSIsPOSIX | PlatformIO_TargetOSIsLinux)
#elif defined(unix) || defined(__unix) || defined(__unix__)
#define             PlatformIO_TargetOS                                                 (PlatformIO_TargetOSIsPOSIX)
#endif /* Apple && Mach defined */
#endif /* PlatformIO_TargetOS undefined */

#ifndef             PlatformIO_POSIXVersion
#define             PlatformIO_POSIXVersion                                             _POSIX_C_SOURCE
#endif

#ifndef             PlatformIO_Compiler
#if   defined(__clang__)
#define             PlatformIO_Compiler                                                 (PlatformIO_CompilerIsClang)
#elif defined(_MSC_VER)
#define             PlatformIO_Compiler                                                 (PlatformIO_CompilerIsMSVC)
#elif defined(__GNUC__) || defined(__GNUG__)
#define             PlatformIO_Compiler                                                 (PlatformIO_CompilerIsGCC)
#endif
#endif /* PlatformIO_Compiler */

#ifndef             PlatformIO_CompilerVersion
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
#define             PlatformIO_CompilerVersion                                          __clang_major__
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#define             PlatformIO_CompilerVersion                                          __GNUC__
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_CompilerVersion                                          _MSC_VER
#endif /* Compiler */
#endif /* CompilerVersion */

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang || PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#if   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define             PlatformIO_TargetByteOrder                                          (PlatformIO_ByteOrder_BE)
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define             PlatformIO_TargetByteOrder                                          (PlatformIO_ByteOrder_LE)
#endif /* __BYTE_ORDER__ */
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_TargetByteOrder                                          (PlatformIO_ByteOrder_LE)
#endif /* PlatformIO_Compiler */

#ifndef             PlatformIO_NewLine8
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#define             PlatformIO_NewLine8                                                 UTF8String("\n")
#define             PlatformIO_NewLine8Size                                             (1)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#define             PlatformIO_NewLine8                                                 UTF8String("\r\n")
#define             PlatformIO_NewLine8Size                                             (2)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsClassicMac)
#define             PlatformIO_NewLine8                                                 UTF8String("\r")
#define             PlatformIO_NewLine8Size                                             (1)
#endif /* PlatformIO_TargetOS */
#endif /* PlatformIO_NewLine8 */

#ifndef             PlatformIO_NewLine16
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#define             PlatformIO_NewLine16                                                UTF16String("\n")
#define             PlatformIO_NewLine16Size                                            (1)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#define             PlatformIO_NewLine16                                                UTF16String("\r\n")
#define             PlatformIO_NewLine16Size                                            (2)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsClassicMac)
#define             PlatformIO_NewLine16                                                UTF16String("\r")
#define             PlatformIO_NewLine16Size                                            (1)
#endif /* PlatformIO_TargetOS */
#endif /* PlatformIO_NewLine16 */


#ifndef             PlatformIO_NewLine32
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#define             PlatformIO_NewLine32                                                UTF32String("\n")
#define             PlatformIO_NewLine32Size                                            (1)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#define             PlatformIO_NewLine32                                                UTF32String("\r\n")
#define             PlatformIO_NewLine32Size                                            (2)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsClassicMac)
#define             PlatformIO_NewLine32                                                UTF32String("\r")
#define             PlatformIO_NewLine32Size                                            (1)
#endif /* PlatformIO_TargetOS */
#endif /* PlatformIO_NewLine32 */


#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#ifndef             _LARGEFILE_SOURCE
#define             _LARGEFILE_SOURCE                                                   (1)
#endif

#ifndef             _LARGEFILE64_SOURCE
#define             _LARGEFILE64_SOURCE                                                 (1)
#endif

#ifndef             _TIME_BITS
#define             _TIME_BITS                                                          (64)
#else
#undef              _TIME_BITS
#define             _TIME_BITS                                                          (64)
#endif

#ifndef             __TIMESIZE
#define             __TIMESIZE                                                          (64)
#else
#undef              __TIMESIZE
#define             __TIMESIZE                                                          (64)
#endif

#ifndef             __USE_TIME_BITS64
#define             __USE_TIME_BITS64                                                   (1)
#endif

#ifdef              _FILE_OFFSET_BITS
#undef              _FILE_OFFSET_BITS
#define             _FILE_OFFSET_BITS                                                   (64)
#else
#define             _FILE_OFFSET_BITS                                                   (64)
#endif

#ifndef            __STDC_WANT_LIB_EXT1__
#define            __STDC_WANT_LIB_EXT1__                                               (1)
#endif
#endif /* PlatformIO_TargetOSIsPOSIX */

#if  ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) == PlatformIO_TargetOSIsApple)
#define             _DARWIN_USE_64_BIT_INODE                                            (1)
#endif /* Apple specific macros */

#if (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#ifndef             WIN32_LEAN_AND_MEAN
#define             WIN32_LEAN_AND_MEAN                                                 (1)
#endif

#ifndef             VC_EXTRALEAN
#define             VC_EXTRALEAN                                                        (1)
#endif

#ifndef             WIN32_EXTRA_LEAN
#define             WIN32_EXTRA_LEAN                                                    (1)
#endif

#ifndef             NOGDI
#define             NOGDI                                                               (1)
#endif

#ifndef             UNICODE
#define             UNICODE                                                             (1)
#endif

#ifndef             _UNICODE
#define             _UNICODE                                                            (1)
#endif

#ifndef             _ATL_ALLOW_CHAR_UNSIGNED
#define             _ATL_ALLOW_CHAR_UNSIGNED                                            (1)
#endif

#ifndef             __STDC_WANT_LIB_EXT1__
#define             __STDC_WANT_LIB_EXT1__                                              (1)
#endif

#endif /* PlatformIO_TargetOS is some kind of Windows */

#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsLinux) == PlatformIO_TargetOSIsLinux)
#define _GNU_SOURCE /* Needed for syscalls like getrandom */
#endif /* Linux */

#include <stdarg.h>     /* Included for va_list, va_copy */
#include <stdbool.h>    /* Included for bool */
#include <stdint.h>     /* Included for u/intX_t */
#include <stdio.h>      /* Included for FILE, SEEK SET/END/CUR macros */
#include <stdlib.h>     /* Included for calloc/free */
#include <wchar.h>      /* Included for WCHAR_MAX */

#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#include <dlfcn.h>      /* Included for shared library support */
#include <unistd.h>     /* Included for stdin/stdout/stderr */
#include <sys/socket.h> /* Included for socket support */
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#include <Windows.h>    /* Included for Shared Library support, WinCon, QueryPerformanceCounter, etc */
#endif /* PlatformIO_TargetOSIsPOSIX */

#if   (WCHAR_MAX == 0x7FFFFFFF || WCHAR_MAX == 0xFFFFFFFF)
#define             PlatformIO_WideCharType                                             PlatformIO_WideCharTypeIsUTF32
#elif (WCHAR_MAX == 0x7FFF || WCGAR_MAX == 0xFFFF)
#define             PlatformIO_WideCharType                                             PlatformIO_WideCharTypeIsUTF16
#endif /* WCHAR_MAX */

#ifndef             PlatformIO_HiddenSymbol
#if (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_HiddenSymbol
#else
#define             PlatformIO_HiddenSymbol                                             __attribute__((visibility("hidden")))
#endif /* Compiler is MSVC */
#endif /* PlatformIO_HiddenSymbol */

#ifndef             PlatformIO_Enum2Index
#define             PlatformIO_Enum2Index(EnumName)                                     (EnumName - 1)
#endif /* PlatformIO_Enum2Index */


    /*!
     @abstract      Gets the total amount of memory in the system.
     */
    uint64_t        PlatformIO_GetTotalMemoryInBytes(void);
    
#ifdef __cplusplus
}
#endif /* Extern C */

#endif /* FoundationIO_PlatformIO_H */
