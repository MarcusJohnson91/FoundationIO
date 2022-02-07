/*!
 @header          PlatformIO.h
 @author          Marcus Johnson
 @copyright       2017+
 @version         2.0.0
 @brief           This header contains preprocessor macros for generic functions in FoundationIO.
 */

#pragma once

#ifndef FoundationIO_PlatformIO_H
#define FoundationIO_PlatformIO_H

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

#ifndef             PlatformIO_ExecutableFormatIsMachO
#define             PlatformIO_ExecutableFormatIsMachO                                  (1)
#endif

#ifndef             PlatformIO_ExecutableFormatIsPE
#define             PlatformIO_ExecutableFormatIsPE                                     (2)
#endif

#ifndef             PlatformIO_ExecutableFormatIsELF
#define             PlatformIO_ExecutableFormatIsELF                                    (4)
#endif

#ifndef             PlatformIO_ByteOrderIsUnknown
#define             PlatformIO_ByteOrderIsUnknown                                       (0)
#endif

#ifndef             PlatformIO_TargetByteOrderIsBE
#define             PlatformIO_TargetByteOrderIsBE                                            (1)
#endif

#ifndef             PlatformIO_TargetByteOrderIsLE
#define             PlatformIO_TargetByteOrderIsLE                                            (2)
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

#ifndef             PlatformIO_Cast
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#define             PlatformIO_Cast(Type, Thing2Cast) ((Type) Thing2Cast)
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define             PlatformIO_Cast(Type, Thing2Cast) reinterpret_cast<Type>(Thing2Cast)
#endif /* Language */
#endif /* PlatformIO_Cast */

#ifndef             PlatformIO_FunctionName
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#define             PlatformIO_FunctionName                                             PlatformIO_Cast(ImmutableString_UTF8, __func__)
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define             PlatformIO_FunctionName                                             PlatformIO_Cast(ImmutableString_UTF8, __PRETTY_FUNCTION__)
#endif /* Language */
#endif /* PlatformIO_FunctionName */

#ifndef             PlatformIO_FileName
#define             PlatformIO_FileName                                                 PlatformIO_Cast(ImmutableString_UTF8, __FILE__)
#endif /* PlatformIO_FunctionName */

#ifndef PlatformIO_TargetOS
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

#ifndef            PlatformIO_Is
#define            PlatformIO_Is(Macro2Check, Value2Check) ((Macro2Check & Value2Check) == Value2Check)
#endif /* PlatformIO_Is */

#ifndef             PlatformIO_ExecutableFormat
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
#define             PlatformIO_ExecutableFormat                                              (PlatformIO_ExecutableFormatIsMachO)
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#define             PlatformIO_ExecutableFormat                                              (PlatformIO_ExecutableFormatIsPE)
#else
#define             PlatformIO_ExecutableFormat                                              (PlatformIO_ExecutableFormatIsELF)
#endif /* TargetOS */
#endif /* PlatformIO_ExecutableFormat */

#ifndef             PlatformIO_ArraySet
#define             PlatformIO_ArraySet(Type)                      (const Type *const *const)
#endif /* PlatformIO_ArraySet */

#ifndef             PlatformIO_Redefine
#define             PlatformIO_Redefine(Macro2Redefine, Value2Assign) PlatformIO_SaveMacro(Macro2Redefine)
#undef  Macro2Redefine
#define Macro2Redefine Value2Assign
#endif /* PlatformIO_Redefine */

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang || PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#if   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define             PlatformIO_TargetByteOrder                                          (PlatformIO_TargetByteOrderIsBE)
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define             PlatformIO_TargetByteOrder                                          (PlatformIO_TargetByteOrderIsLE)
#endif /* __BYTE_ORDER__ */
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_TargetByteOrder                                          (PlatformIO_TargetByteOrderIsLE)
#endif /* PlatformIO_Compiler */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
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

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
#define             _DARWIN_USE_64_BIT_INODE                                            (1)
#endif /* Apple specific macros */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
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

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsLinux)
#define _GNU_SOURCE /* Needed for syscalls like getrandom */
#endif /* Linux */

#include <stdarg.h>     /* Included for va_list, va_copy */
#include <stdbool.h>    /* Included for bool */
#include <stdint.h>     /* Included for u/intX_t */
#include <stdio.h>      /* Included for FILE, SEEK SET/END/CUR macros */
#include <stdlib.h>     /* Included for calloc/free */
#include <wchar.h>      /* Included for WCHAR_MAX */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#include <dlfcn.h>      /* Included for shared library support */
#include <unistd.h>     /* Included for stdin/stdout/stderr */
#include <sys/socket.h> /* Included for socket support */
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#include <Windows.h>    /* Included for Shared Library support, WinCon, QueryPerformanceCounter, etc */
#endif /* PlatformIO_TargetOSIsPOSIX */

#ifndef             PlatformIO_AnnexK
#ifdef              __STDC_LIB_EXT1__
#define             PlatformIO_AnnexK                                                  (1)
#endif /* __STDC_LIB_EXT1__ */
#endif /* PlatformIO_AnnexK */

#ifndef             PlatformIO_Public
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang) || (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#define             PlatformIO_Public                                                   __attribute__((visibility("default")))
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_Public                                                   __declspec(dllexport)
#endif /* PlatformIO_Compiler */
#endif /* PlatformIO_Public */

#ifndef             PlatformIO_Private
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang) || (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#define             PlatformIO_Private                                                  __attribute__((visibility("hidden")))
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_Private                                                  __declspec(dllexport)
#endif /* PlatformIO_Compiler */
#endif /* PlatformIO_Private */

#ifndef             PlatformIO_Enum2Index
#define             PlatformIO_Enum2Index(EnumName)                                     (EnumName - 1)
#endif /* PlatformIO_Enum2Index */

#ifndef             PlatformIO_Unused
#if (PlatformIO_Language == PlatformIO_LanguageIsC) && (PlatformIO_LanguageVersionC >= PlatformIO_LanguageVersionC23)
#define             PlatformIO_Unused(Type)                                             Type [[maybe_unused]]
#elif (PlatformIO_Language == PlatformIO_LanguageIsC) && (PlatformIO_LanguageVersionC < PlatformIO_LanguageVersionC23)
#if (PlatformIO_Compiler == PlatformIO_CompilerIsClang) || (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#define             PlatformIO_Unused(Type)                                             Type __attribute__((unused))
#else
#define             PlatformIO_Unused(Type)                                             Type
#endif /* Compiler */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX) && (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX17)
#define             PlatformIO_Unused(Type)                                             Type [[maybe_unused]]
#endif /* Language */
#endif /* PlatformIO_Unused */

#ifndef             PlatformIO_Concat
#define             PlatformIO_Concat1(A, B) A##B
#define             PlatformIO_Concat(A, B)  PlatformIO_Concat1(A, B)
#endif /* PlatformIO_Concat */

#ifndef             PlatformIO_Stringify8
#define             PlatformIO_Stringify8(X) UTF8String(#X)
#endif /* PlatformIO_Stringify8 */

#ifndef             PlatformIO_Expand
#define             PlatformIO_Expand(...) __VA_ARGS__
#endif /* PlatformIO_Expand */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @abstract      Gets the total amount of memory in the system.
     */
    size_t          PlatformIO_GetTotalMemoryInBytes(void);

    /*!
     @abstract      Error message for assert; Do not use directly.
     */
    void            PlatformIO_AssertFail(const char *FileName, const char *FunctionName, const unsigned char *Expression, ...);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_PlatformIO_H */
