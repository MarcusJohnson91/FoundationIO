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

#ifndef             PlatformIO_TargetOSIsUnknown
#define             PlatformIO_TargetOSIsUnknown                                                (0)
#endif              /* UnknownOS */

#ifndef             PlatformIO_TargetOSIsClassicMac
#define             PlatformIO_TargetOSIsClassicMac                                             (1)
#endif              /* MacClassicOS */

#ifndef             PlatformIO_TargetOSIsWindows
#define             PlatformIO_TargetOSIsWindows                                                (2)
#endif              /* Windows */

#ifndef             PlatformIO_TargetOSIsPOSIX
#define             PlatformIO_TargetOSIsPOSIX                                                  (4)
#endif              /* POSIX */

#ifndef             PlatformIO_TargetOSIsApple
#define             PlatformIO_TargetOSIsApple                                                  (8)
#endif              /* AppleOS (MacOS, iOS, iPadOS, TVOS, WatchOS, etc) */

#ifndef             PlatformIO_TargetOSIsBSD
#define             PlatformIO_TargetOSIsBSD                                                    (16)
#endif              /* BSD (FreeBSD, OpenBSD, NetBSD, DragonFlyBSD, etc) */

#ifndef             PlatformIO_TargetOSIsLinux
#define             PlatformIO_TargetOSIsLinux                                                  (32)
#endif              /* Linux (Ubuntu, Android, etc) */

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

#ifndef             PlatformIO_Architecture
#if                 ((__amd64__ == 1) || (__x86_64__ == 1))
#define             PlatformIO_Architecture (PlatformIO_ArchIsAMD64)
#elif               ((__aarch64__ == 1) || (__ARM_64BIT_STATE == 1))
#define             PlatformIO_Architecture (PlatformIO_ArchIsARM64)
#elif               (__mips64__)
#define             PlatformIO_Architecture (PlatformIO_ArchIsMIPS64)
#elif               (__riscv) && (__riscv_xlen == 64)
#define             PlatformIO_Architecture (PlatformIO_ArchIsRISCV64)
#endif
#endif

#ifndef             PlatformIO_ArchHasSIMDExtensions
#if                 (defined __ARM_NEON)
#define             PlatformIO_ArchHasSIMDExtensions (1)
#elif               (defined __AVX__)
#define             PlatformIO_ArchHasSIMDExtensions (1)
#elif               (defined __MIPS_3D__)
#define             PlatformIO_Architecture (PlatformIO_ArchIsMIPS64)
#endif
#endif

#if  !defined(PlatformIO_TargetOS)
#if   defined(__APPLE__) && defined(__MACH__)
#define             PlatformIO_TargetOS                                                 (PlatformIO_TargetOSIsPOSIX | PlatformIO_TargetOSIsBSD | PlatformIO_TargetOSIsApple)
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

#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#ifdef              _FILE_OFFSET_BITS
#undef              _FILE_OFFSET_BITS
#define             _FILE_OFFSET_BITS                                                   (64)
#else
#define             _FILE_OFFSET_BITS                                                   (64)
#endif /* _FILE_OFFSET_BITS */
#endif /* PlatformIO_TargetOS */
#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif 
#include <stdio.h>                    /* Included for FILE, SEEK SET/END/CUR macros */
#include <stdarg.h>                   /* Included for va_list, va_copy */
#include <stdbool.h>                  /* Included for bool */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdlib.h>                   /* Included for calloc/free */
#include <wchar.h>                    /* Included for WCHAR_MAX */
    
#ifndef             Yes
#define             Yes                                                                 (1)
#elif
#undef              Yes
#define             Yes                                                                 (1)
#endif           /* Yes */
    
#ifndef             No
#define             No                                                                  (0)
#elif
#undef              No
#define             No                                                                  (0)
#endif           /* No */
    
#ifndef             True
#define             True                                                                (1)
#elif
#undef              True
#define             True                                                                (1)
#endif           /* True */
    
#ifndef             False
#define             False                                                               (0)
#elif
#undef              False
#define             False                                                               (0)
#endif           /* False */
    
#ifndef             PlatformIO_LanguageVersionC99
#define             PlatformIO_LanguageVersionC99                                       (199901L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC11
#define             PlatformIO_LanguageVersionC11                                       (201112L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC18
#define             PlatformIO_LanguageVersionC18                                       (201710L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC2X
#define             PlatformIO_LanguageVersionC2X                                       (202102L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC
#define             PlatformIO_LanguageVersionC                                         (__STDC_VERSION__)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX98
#define             PlatformIO_LanguageVersionCXX98                                     (199711L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX11
#define             PlatformIO_LanguageVersionCXX11                                     (201103L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX14
#define             PlatformIO_LanguageVersionCXX14                                     (201402L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX17
#define             PlatformIO_LanguageVersionCXX17                                     (201703L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX20
#define             PlatformIO_LanguageVersionCXX20                                     (202002L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX
#define             PlatformIO_LanguageVersionCXX                                       (__cplusplus)
#endif
    
#ifndef             PlatformIO_LanguageIsC
#define             PlatformIO_LanguageIsC                                              (1)
#endif
    
#ifndef             PlatformIO_LanguageIsCXX
#define             PlatformIO_LanguageIsCXX                                            (2)
#endif
    
#ifndef             PlatformIO_Language
#ifdef              __cplusplus
#define             PlatformIO_Language                                                 PlatformIO_LanguageIsCXX
#else
#define             PlatformIO_Language                                                 PlatformIO_LanguageIsC
#endif
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
#define             PlatformIO_CompilerVersion __clang_major__
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#define             PlatformIO_CompilerVersion __GNUC__
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_CompilerVersion _MSC_VER
#endif /* Compiler */
#endif /* CompilerVersion */
    
    /* POSIX shared stuff */
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#include <dlfcn.h>      /* Included for shared library support */
#include <sys/socket.h> /* Included for socket support */
#include <unistd.h>     /* Included for stdin/stdout/stderr */
    
#ifndef             PlatformIO_Socket_Create
#define             PlatformIO_Socket_Create(Family, Type, Protocol)                         socket(Family, Type, Protocol)
#endif
    
#ifndef             PlatformIO_Socket_Bind
#define             PlatformIO_Socket_Bind(Socket2Bind, Name, NameSize)                      bind(Socket2Bind, Name, NameSize)
#endif
    
#ifndef             PlatformIO_Socket_Listen
#define             PlatformIO_Socket_Listen(BoundUnconnectedSocket, BacklogSize)            listen(BoundUnconnectedSocket, BacklogSize)
#endif
    
#ifndef             PlatformIO_Socket_Accept
#define             PlatformIO_Socket_Accept(Socket2Accept, Name, NameSize)                  accept(Socket2Accept, Name, NameSize)
#endif
    
#ifndef             PlatformIO_Socket_Connect
#define             PlatformIO_Socket_Connect(Socket2Connect, Name, NameSize)                connect(Socket2Connect, Name, NameSize)
#endif
    
#ifndef             PlatformIO_Socket_Read
#define             PlatformIO_Socket_Read(Socket2Read, Buffer2Write, NumBytes)              read(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             PlatformIO_Socket_Write
#define             PlatformIO_Socket_Write(Socket2Write, Buffer2Write, NumBytes)            write(Socket2Write, Buffer2Write, NumBytes)
#endif
    
#ifndef             PlatformIO_Socket_Close
#define             PlatformIO_Socket_Close(Socket2Close)                                    close(Socket2Close)
#endif
    
#endif /* PlatformIO_TargetOS is some kind of POSIX */
    
#if (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#ifndef             WIN32_LEAN_AND_MEAN
#define             WIN32_LEAN_AND_MEAN
#endif             /* WIN32_LEAN_AND_MEAN */
#ifndef             VC_EXTRALEAN
#define             VC_EXTRALEAN
#endif             /* VC_EXTRALEAN */
#ifndef             NOGDI
#define             NOGDI
#endif             /* NOGDI */
#include <Windows.h>  /* Included for Shared Library support, WinCon, QueryPerformanceCounter, etc */
#include <WinSock2.h> /* Included for socket support, Windows.h MUST be included before WinSock2 */
    
#ifndef             PlatformIO_Socket_Create
#define             PlatformIO_Socket_Create(Family, Type, Protocol)                         socket(Family, Type, Protocol)
#endif
    
#ifndef             PlatformIO_Socket_Bind
#define             PlatformIO_Socket_Bind(Socket2Bind, Name, NameSize)                      bind(Socket2Bind, Name, NameSize)
#endif
    
#ifndef             PlatformIO_Socket_Listen
#define             PlatformIO_Socket_Listen(BoundUnconnectedSocket, BacklogSize)            listen(BoundUnconnectedSocket, BacklogSize)
#endif
    
#ifndef             PlatformIO_Socket_Accept
#define             PlatformIO_Socket_Accept(Socket2Accept, Name, NameSize)                  accept(Socket2Accept, Name, NameSize)
#endif
    
#ifndef             PlatformIO_Socket_Connect
#define             PlatformIO_Socket_Connect(Socket2Connect, Name, NameSize)                connect(Socket2Connect, Name, NameSize)
#endif
    
#ifndef             PlatformIO_Socket_Read
#define             PlatformIO_Socket_Read(Socket2Read, Buffer2Write, NumBytes)              ReadFile(Socket2Read, Buffer2Write, NumBytes, )
#endif
    
#ifndef             PlatformIO_Socket_Write
#define             PlatformIO_Socket_Write(Socket2Read, Buffer2Write, NumBytes)             write(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             PlatformIO_Socket_Close
#define             PlatformIO_Socket_Close(Socket2Close)                                    close(Socket2Close)
#endif
    
#endif /* PlatformIO_TargetOS is some kind of Windows */
    
#ifndef             PlatformIO_ByteOrder_Unknown
#define             PlatformIO_ByteOrder_Unknown                                              (0)
#endif
    
#ifndef             PlatformIO_ByteOrder_BE
#define             PlatformIO_ByteOrder_BE                                                   (1)
#endif
    
#ifndef             PlatformIO_ByteOrder_LE
#define             PlatformIO_ByteOrder_LE                                                   (2)
#endif
    
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang || PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#if   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define             PlatformIO_TargetByteOrder                                               (PlatformIO_ByteOrder_BE)
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define             PlatformIO_TargetByteOrder                                               (PlatformIO_ByteOrder_LE)
#endif /* __BYTE_ORDER__ */
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define             PlatformIO_TargetByteOrder                                               (PlatformIO_ByteOrder_LE)
#endif /* PlatformIO_Compiler */
    
#ifndef             PlatformIO_NewLine8
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#define             PlatformIO_NewLine8                                                      (u8"\n")
#define             PlatformIO_NewLine8Size                                                  (1)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#define             PlatformIO_NewLine8                                                      (u8"\r\n")
#define             PlatformIO_NewLine8Size                                                  (2)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsClassicMac)
#define             PlatformIO_NewLine8                                                      (u8"\r")
#define             PlatformIO_NewLine8Size                                                  (1)
#endif /* TargetOS */
#endif /* PlatformIO_NewLine8 */
    
#ifndef             PlatformIO_NewLine16
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#define             PlatformIO_NewLine16                                                     (u"\n")
#define             PlatformIO_NewLine16Size                                                 (1)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#define             PlatformIO_NewLine16                                                     (u"\r\n")
#define             PlatformIO_NewLine16Size                                                 (2)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsClassicMac)
#define             PlatformIO_NewLine16                                                     (u"\r")
#define             PlatformIO_NewLine16Size                                                 (1)
#endif /* TargetOS */
#endif /* PlatformIO_NewLine16 */
    
    
#ifndef             PlatformIO_NewLine32
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#define             PlatformIO_NewLine32                                                     (U"\n")
#define             PlatformIO_NewLine32Size                                                 (1)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#define             PlatformIO_NewLine32                                                     (U"\r\n")
#define             PlatformIO_NewLine32Size                                                 (2)
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsClassicMac)
#define             PlatformIO_NewLine32                                                     (U"\r")
#define             PlatformIO_NewLine32Size                                                 (1)
#endif /* TargetOS */
#endif /* PlatformIO_NewLine32 */
    
#ifndef             PlatformIO_NULLTerminator
#define             PlatformIO_NULLTerminator                                                (0)
#endif
#ifndef             PlatformIO_NULLTerminatorSize
#define             PlatformIO_NULLTerminatorSize                                            (1)
#endif
    
#ifndef             PlatformIO_UNCPathPrefix8
#define             PlatformIO_UNCPathPrefix8                                                (u8"//?/")
#endif
    
#ifndef             PlatformIO_InvisibleString32
#define             PlatformIO_InvisibleString32                                             (U"\u00A0")
#endif
    
#if                (WCHAR_MIN < 0)
#define             PlatformIO_WideCharRange ((WCHAR_MAX * 2) + 1)
#else
#define             PlatformIO_WideCharRange (WCHAR_MAX)
#endif

    /*!
     @abstract      PlatformIO_Immutable is for pointers and arrays.
     @remark        Makes the pointer and the data it points to constant.
     */
#ifndef             PlatformIO_Immutable
#define             PlatformIO_Immutable(PointerType) const PointerType const
#endif


#define             class                                                           struct

    
#ifndef             PlatformIO_Mutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#define             PlatformIO_Mutable(PointerType, Variable) (PointerType, Variable)
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define             PlatformIO_Mutable(PointerType, Variable) (std::remove_const<PointerType>(Variable))
#endif
#endif
    
    /*!
     @abstract      PlatformIO_Volatile is for pointers and arrays.
     @remark        Makes the pointer and the data it points to volatile.
     */
#ifndef             PlatformIO_Volatile
#define             PlatformIO_Volatile(PointerType) volatile PointerType volatile
#endif

    /*!
     @abstract      PlatformIO_Constant is for variables, constants, and values.
     @remark        Makes the pointer and the data it points to constant.
     */
#ifndef             PlatformIO_Constant
#define             PlatformIO_Constant(Type) const Type
#endif

#ifndef             PlatformIO_Literal
#define             PlatformIO_Literal(NewType, UnconstType, Literal)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#undef              PlatformIO_Literal
#define             PlatformIO_Literal(NewType, UnconstType, Literal) (NewType const) Literal
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#undef              PlatformIO_Literal
#define             PlatformIO_Literal(NewType, UnconstType, Literal) reinterpret_cast<NewType>(const_cast<UnconstType>(Literal))
#endif
#endif

#ifndef             PlatformIO_FunctionName
#define             PlatformIO_FunctionName           (PlatformIO_Immutable(UTF8 *)) __func__
#endif /* PlatformIO_FunctionName */

    /*!
     @abstract      Gets the total amount of memory in the system.
     */
    uint64_t        PlatformIO_GetTotalMemoryInBytes(void);
    
#ifdef __cplusplus
}
#endif /* Extern C */

#endif /* FoundationIO_PlatformIO_H */
