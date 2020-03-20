/*!
 @header              Macros.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains preprocessor macros for generic functions in FoundationIO, and cross-platform compatibility.
 */

#pragma once

#ifndef FoundationIO_Macros_H
#define FoundationIO_Macros_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef             FoundationIOOSUnknown
#define             FoundationIOOSUnknown                                                     (0)
#endif              /* UnknownOS */

#ifndef             FoundationIOMacClassicOS
#define             FoundationIOMacClassicOS                                                  (1)
#endif              /* MacClassicOS */

#ifndef             FoundationIOWindowsOS
#define             FoundationIOWindowsOS                                                     (2)
#endif              /* Windows */

#ifndef             FoundationIOPOSIXOS
#define             FoundationIOPOSIXOS                                                       (4)
#endif              /* POSIX */

#ifndef             FoundationIOAppleOS
#define             FoundationIOAppleOS                                                       (8)
#endif              /* AppleOS (MacOS, iOS, TVOS, etc) */

#ifndef             FoundationIOBSDOS
#define             FoundationIOBSDOS                                                         (16)
#endif              /* BSD (FreeBSD, OpenBSD, NetBSD, DragonFlyBSD, etc) */

#ifndef             FoundationIOLinuxOS
#define             FoundationIOLinuxOS                                                       (32)
#endif              /* BSD (Linux, Ubuntu, Android, etc) */

#if ((defined Macintosh) || (defined macintosh))
#define             FoundationIOTargetOS                                                      (FoundationIOMacClassicOS)
#endif

#if ((defined _WIN16) || (defined _WIN32) || (defined _WIN64))
#define             FoundationIOTargetOS                                                      (FoundationIOWindowsOS)
#endif

#if (defined(__APPLE__) && defined(__MACH__))
#define             FoundationIOTargetOS                                                      (FoundationIOPOSIXOS + FoundationIOAppleOS)
#endif

#if (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__))
#define             FoundationIOTargetOS                                                      (FoundationIOPOSIXOS + FoundationIOBSDOS)
#endif

#if (defined(__linux__) || defined(__ANDROID__) || defined(__gnu_linux__))
#define             FoundationIOTargetOS                                                      (FoundationIOPOSIXOS + FoundationIOLinuxOS)
#endif

#if (defined(__unix__) || defined(unix))
#define             FoundationIOTargetOS                                                      (FoundationIOPOSIXOS)
#endif

#if ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
#ifdef              _FILE_OFFSET_BITS
#undef              _FILE_OFFSET_BITS
#define             _FILE_OFFSET_BITS                                                         (64)
#else
#define             _FILE_OFFSET_BITS                                                         (64)
#endif /* _FILE_OFFSET_BITS */
#endif /* FoundationIOTargetOS */

#include <stdarg.h>                   /* Included for va_list, va_copy */
#include <stdbool.h>                  /* Included for bool */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for FILE, SEEK SET/END/CUR macros */
#include <stdlib.h>                   /* Included for calloc/free */
#include <wchar.h>                    /* Included for WCHAR_MAX */
    
#ifndef             Yes
#define             Yes                                                                       (1)
#elif
#undef              Yes
#define             Yes                                                                       (1)
#endif           /* Yes */
    
#ifndef             No
#define             No                                                                        (0)
#elif
#undef              No
#define             No                                                                        (0)
#endif           /* No */
    
#ifndef             True
#define             True                                                                      (1)
#elif
#undef              True
#define             True                                                                      (1)
#endif           /* True */
    
#ifndef             False
#define             False                                                                     (0)
#elif
#undef              False
#define             False                                                                     (0)
#endif           /* False */
    
#ifndef             FoundationIOStandardVersionC99
#define             FoundationIOStandardVersionC99                                            (199901L)
#endif
    
#ifndef             FoundationIOStandardVersionC11
#define             FoundationIOStandardVersionC11                                            (201112L)
#endif
    
#ifndef             FoundationIOStandardVersionC18
#define             FoundationIOStandardVersionC18                                            (201710L)
#endif
    
#ifndef             FoundationIOStandardVersionC2X
#define             FoundationIOStandardVersionC2X                                            (202102L)
#endif
    
#ifndef             FoundationIOStandardVersion
#define             FoundationIOStandardVersion                                               (__STDC_VERSION__)
#endif
    
#ifndef             FoundationIOCompilerIsUnknown
#define             FoundationIOCompilerIsUnknown                                             (0)
#endif
    
#ifndef             FoundationIOCompilerIsClang
#define             FoundationIOCompilerIsClang                                               (1)
#endif
    
#ifndef             FoundationIOCompilerIsMSVC
#define             FoundationIOCompilerIsMSVC                                                (2)
#endif
    
#ifndef             FoundationIOCompilerIsGCC
#define             FoundationIOCompilerIsGCC                                                 (3)
#endif
    
#ifndef             FoundationIOCompiler
#if   defined(__clang__)
#define             FoundationIOCompiler                                                      (FoundationIOCompilerIsClang)
#elif defined(_MSC_VER)
#define             FoundationIOCompiler                                                      (FoundationIOCompilerIsMSVC)
#elif defined(__GNUC__) || defined(__GNUG__)
#define             FoundationIOCompiler                                                      (FoundationIOCompilerIsGCC)
#endif
#endif /* FoundationIOCompiler */
    
    /* POSIX shared stuff */
#if ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
#include <dlfcn.h>      /* Included for shared library support */
#include <sys/socket.h> /* Included for socket support */
#include <sys/sysctl.h> /* Included for getting the number of CPU cores */
#include <unistd.h>     /* Included for stdin/stdout/stderr */
    
#ifndef             FoundationIO_File_Open
#define             FoundationIO_File_Open(UTF8FilePath, UTF8FileMode)                         fopen(UTF8FilePath, UTF8FileMode)
#endif
    
#ifndef             FoundationIO_File_Seek
#define             FoundationIO_File_Seek(File, Offset, Origin)                               fseeko(File, Offset, Origin)
#endif
    
#ifndef             FoundationIO_File_GetSize
#define             FoundationIO_File_GetSize(File)                                            ftello(File)
#endif
    
#ifndef             FoundationIO_File_Read
#define             FoundationIO_File_Read(Output, ElementSize, NumElements2Read, File2Read)   fread(Output, ElementSize, NumElements2Read, File2Read)
#endif
    
#ifndef             FoundationIO_File_Write
#define             FoundationIO_File_Write(Input, ElementSize, NumElements2Write, File2Write) fwrite(Input, ElementSize, NumElements2Write, File2Write)
#endif
    
#ifndef             FoundationIO_File_Close
#define             FoundationIO_File_Close(File2Close)                                        fclose(File2Close)
#endif
    
#ifndef             FoundationIO_Socket_Create
#define             FoundationIO_Socket_Create(Family, Type, Protocol)                         socket(Family, Type, Protocol)
#endif
    
#ifndef             FoundationIO_Socket_Bind
#define             FoundationIO_Socket_Bind(Socket2Bind, Name, NameSize)                      bind(Socket2Bind, Name, NameSize)
#endif
    
#ifndef             FoundationIO_Socket_Listen
#define             FoundationIO_Socket_Listen(BoundUnconnectedSocket, BacklogSize)            listen(BoundUnconnectedSocket, BacklogSize)
#endif
    
#ifndef             FoundationIO_Socket_Accept
#define             FoundationIO_Socket_Accept(Socket2Accept, Name, NameSize)                  accept(Socket2Accept, Name, NameSize)
#endif
    
#ifndef             FoundationIO_Socket_Connect
#define             FoundationIO_Socket_Connect(Socket2Connect, Name, NameSize)                connect(Socket2Connect, Name, NameSize)
#endif
    
#ifndef             FoundationIO_Socket_Read
#define             FoundationIO_Socket_Read(Socket2Read, Buffer2Write, NumBytes)              read(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_Socket_Write
#define             FoundationIO_Socket_Write(Socket2Write, Buffer2Write, NumBytes)            write(Socket2Write, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_Socket_Close
#define             FoundationIO_Socket_Close(Socket2Close)                                    close(Socket2Close)
#endif
    
#endif /* FoundationIOTargetOS is some kind of POSIX */
    
#if (FoundationIOTargetOS == FoundationIOWindowsOS)
#ifndef             WIN32_LEAN_AND_MEAN
#define             WIN32_LEAN_AND_MEAN
#endif             /* WIN32_LEAN_AND_MEAN */
#ifndef             VC_EXTRALEAN
#define             VC_EXTRALEAN
#endif             /* VC_EXTRALEAN */
#ifndef             NOGDI
#define             NOGDI
#endif             /* NOGDI */
#include <Windows.h>                  /* Included for Shared Library support, WinCon, QueryPerformanceCounter, etc */
#include <WinSock2.h>                 /* Included for socket support, Windows.h MUST be included before WinSock2 */
    
#ifndef             FoundationIO_File_Open
#define             FoundationIO_File_Open(UTF16FilePath, UTF16FileMode)                       _wfopen(UTF16FilePath, UTF16FileMode)
#endif
    
#ifndef             FoundationIO_File_Seek
#define             FoundationIO_File_Seek(File, Offset, Origin)                               _fseeki64(File, Offset, Origin)
#endif
    
#ifndef             FoundationIO_File_GetSize
#define             FoundationIO_File_GetSize(File)                                            _ftelli64(File)
#endif
    
#ifndef             FoundationIO_File_Read
#define             FoundationIO_File_Read(Output, ElementSize, NumElements2Read, File2Read)   fread(Output, ElementSize, NumElements2Read, File2Read)
#endif
    
#ifndef             FoundationIO_File_Write
#define             FoundationIO_File_Write(Input, ElementSize, NumElements2Read, File2Write)  fwrite(Input, ElementSize, NumElements2Read, File2Write)
#endif
    
#ifndef             FoundationIO_File_Close
#define             FoundationIO_File_Close(File2Close)                                        fclose(File2Close)
#endif
    
#ifndef             FoundationIO_Socket_Create
#define             FoundationIO_Socket_Create(Family, Type, Protocol)                         socket(Family, Type, Protocol)
#endif
    
#ifndef             FoundationIO_Socket_Bind
#define             FoundationIO_Socket_Bind(Socket2Bind, Name, NameSize)                      bind(Socket2Bind, Name, NameSize)
#endif
    
#ifndef             FoundationIO_Socket_Listen
#define             FoundationIO_Socket_Listen(BoundUnconnectedSocket, BacklogSize)            listen(BoundUnconnectedSocket, BacklogSize)
#endif
    
#ifndef             FoundationIO_Socket_Accept
#define             FoundationIO_Socket_Accept(Socket2Accept, Name, NameSize)                  accept(Socket2Accept, Name, NameSize)
#endif
    
#ifndef             FoundationIO_Socket_Connect
#define             FoundationIO_Socket_Connect(Socket2Connect, Name, NameSize)                connect(Socket2Connect, Name, NameSize)
#endif
    
#ifndef             FoundationIO_Socket_Read
#define             FoundationIO_Socket_Read(Socket2Read, Buffer2Write, NumBytes)              read(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_Socket_Write
#define             FoundationIO_Socket_Write(Socket2Read, Buffer2Write, NumBytes)             write(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_Socket_Close
#define             FoundationIO_Socket_Close(Socket2Close)                                    close(Socket2Close)
#endif
    
#endif /* FoundationIOTargetOS is some kind of Windows */
    
#ifndef             FoundationIOCompileTimeByteOrderUnknown
#define             FoundationIOCompileTimeByteOrderUnknown                                   (0)
#endif
    
#ifndef             FoundationIOByteOrderBE
#define             FoundationIOByteOrderBE                                                   (1)
#endif
    
#ifndef             FoundationIOByteOrderLE
#define             FoundationIOByteOrderLE                                                   (2)
#endif
    
#if   (FoundationIOCompiler == FoundationIOCompilerIsClang || FoundationIOCompiler == FoundationIOCompilerIsGCC)
#if   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define             FoundationIOTargetByteOrder                                               (FoundationIOByteOrderBE)
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define             FoundationIOTargetByteOrder                                               (FoundationIOByteOrderLE)
#endif /* __BYTE_ORDER__ */
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#define             FoundationIOTargetByteOrder                                               (FoundationIOByteOrderLE)
#endif /* FoundationIOCompiler */
    
#ifndef             FoundationIONewLine8
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
#define             FoundationIONewLine8                                                      (u8"\n")
#define             FoundationIONewLine8Size                                                  (1)
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#define             FoundationIONewLine8                                                      (u8"\r\n")
#define             FoundationIONewLine8Size                                                  (2)
#elif (FoundationIOTargetOS == FoundationIOMacClassicOS)
#define             FoundationIONewLine8                                                      (u8"\r")
#define             FoundationIONewLine8Size                                                  (1)
#endif /* TargetOS */
#endif /* FoundationIONewLine8 */
    
#ifndef             FoundationIONewLine16
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
#define             FoundationIONewLine16                                                     (u"\n")
#define             FoundationIONewLine16Size                                                 (1)
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#define             FoundationIONewLine16                                                     (u"\r\n")
#define             FoundationIONewLine16Size                                                 (2)
#elif (FoundationIOTargetOS == FoundationIOMacClassicOS)
#define             FoundationIONewLine16                                                     (u"\r")
#define             FoundationIONewLine16Size                                                 (1)
#endif /* TargetOS */
#endif /* FoundationIONewLine16 */
    
    
#ifndef             FoundationIONewLine32
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
#define             FoundationIONewLine32                                                     (U"\n")
#define             FoundationIONewLine32Size                                                 (1)
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#define             FoundationIONewLine32                                                     (U"\r\n")
#define             FoundationIONewLine32Size                                                 (2)
#elif (FoundationIOTargetOS == FoundationIOMacClassicOS)
#define             FoundationIONewLine32                                                     (U"\r")
#define             FoundationIONewLine32Size                                                 (1)
#endif /* TargetOS */
#endif /* FoundationIONewLine32 */
    
#ifndef             FoundationIONULLTerminator
#define             FoundationIONULLTerminator                                                (0)
#endif
#ifndef             FoundationIONULLTerminatorSize
#define             FoundationIONULLTerminatorSize                                            (1)
#endif
    
#ifndef             UNCPathPrefix8
#define             UNCPathPrefix8                                                            (u8"//?/")
#endif
    
#ifndef             UNCPathPrefix16
#define             UNCPathPrefix16                                                           (u"//?/")
#endif

#ifndef             UNCPathPrefix32
#define             UNCPathPrefix32                                                           (U"//?/")
#endif
    
#ifndef             FoundationIO_ImmutablePointer2MutableData
#define             FoundationIO_ImmutablePointer2MutableData(Type, VariableName)              Type *const VariableName
#endif
    
#ifndef             FoundationIO_MutablePointer2ImmutableData
#define             FoundationIO_MutablePointer2ImmutableData(Type, VariableName)              Type const *VariableName
#endif
    
#ifndef             FoundationIO_ImmutablePointer2ImmutableData
#define             FoundationIO_ImmutablePointer2ImmutableData(Type, VariableName)            Type const *const VariableName
#endif
    
#if                (WCHAR_MIN < 0)
#define             FoundationIOWideCharSize ((WCHAR_MAX * 2) + 1)
#else
#define             FoundationIOWideCharSize (WCHAR_MAX)
#endif
    
    uint64_t        FoundationIO_GetNumCPUCores(void);
    
    uint64_t        FoundationIO_GetTotalMemoryInBytes(void);
    
#ifdef __cplusplus
}
#endif

#endif  /* FoundationIO_Macros_H */
