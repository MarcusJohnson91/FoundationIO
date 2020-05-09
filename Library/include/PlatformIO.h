/*!
 @header              PlatformIO.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             2.0.0
 @brief               This header contains preprocessor macros for generic functions in FoundationIO, and cross-platform compatibility.
 */

#pragma once

#ifndef FoundationIO_PlatformIO_H
#define FoundationIO_PlatformIO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef             PlatformIO_UnknownOS
#define             PlatformIO_UnknownOS                                                     (0)
#endif              /* UnknownOS */

#ifndef             PlatformIO_ClassicMacOS
#define             PlatformIO_ClassicMacOS                                                  (1)
#endif              /* MacClassicOS */

#ifndef             PlatformIO_WindowsOS
#define             PlatformIO_WindowsOS                                                     (2)
#endif              /* Windows */

#ifndef             PlatformIO_POSIXOS
#define             PlatformIO_POSIXOS                                                       (4)
#endif              /* POSIX */

#ifndef             PlatformIO_AppleOS
#define             PlatformIO_AppleOS                                                       (8)
#endif              /* AppleOS (MacOS, iOS, iPadOS, TVOS, WatchOS, etc) */

#ifndef             PlatformIO_BSDOS
#define             PlatformIO_BSDOS                                                         (16)
#endif              /* BSD (FreeBSD, OpenBSD, NetBSD, DragonFlyBSD, etc) */

#ifndef             PlatformIO_LinuxOS
#define             PlatformIO_LinuxOS                                                       (32)
#endif              /* BSD (Linux, Ubuntu, Android, etc) */

#if ((defined Macintosh) || (defined macintosh))
#define             PlatformIO_TargetOS                                                      (PlatformIO_ClassicMacOS)
#endif

#if ((defined _WIN16) || (defined _WIN32) || (defined _WIN64))
#define             PlatformIO_TargetOS                                                      (PlatformIO_WindowsOS)
#endif

#if (defined(__APPLE__) && defined(__MACH__))
#define             PlatformIO_TargetOS                                                      (PlatformIO_POSIXOS + PlatformIO_AppleOS)
#endif

#if (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__))
#define             PlatformIO_TargetOS                                                      (PlatformIO_POSIXOS + PlatformIO_BSDOS)
#endif

#if (defined(__linux__) || defined(__ANDROID__) || defined(__gnu_linux__))
#define             PlatformIO_TargetOS                                                      (PlatformIO_POSIXOS + PlatformIO_LinuxOS)
#endif

#if (defined(__unix__) || defined(unix))
#define             PlatformIO_TargetOS                                                      (PlatformIO_POSIXOS)
#endif

#if ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#ifdef              _FILE_OFFSET_BITS
#undef              _FILE_OFFSET_BITS
#define             _FILE_OFFSET_BITS                                                         (64)
#else
#define             _FILE_OFFSET_BITS                                                         (64)
#endif /* _FILE_OFFSET_BITS */
#endif /* PlatformIO_TargetOS */

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
    
#ifndef             PlatformIO_LanguageVersionC99
#define             PlatformIO_LanguageVersionC99                                            (199901L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC11
#define             PlatformIO_LanguageVersionC11                                            (201112L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC18
#define             PlatformIO_LanguageVersionC18                                            (201710L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC2X
#define             PlatformIO_LanguageVersionC2X                                            (202102L)
#endif
    
#ifndef             PlatformIO_LanguageVersionC
#define             PlatformIO_LanguageVersionC                                              (__STDC_VERSION__)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX98
#define             PlatformIO_LanguageVersionCXX98                                          (199711L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX11
#define             PlatformIO_LanguageVersionCXX11                                          (201103L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX14
#define             PlatformIO_LanguageVersionCXX14                                          (201402L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX17
#define             PlatformIO_LanguageVersionCXX17                                          (201703L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX20
#define             PlatformIO_LanguageVersionCXX20                                          (202002L)
#endif
    
#ifndef             PlatformIO_LanguageVersionCXX
#define             PlatformIO_LanguageVersionCXX                                            (__cplusplus)
#endif
    
#ifndef             PlatformIO_LanguageIsC
#define             PlatformIO_LanguageIsC                                                   (1)
#endif
    
#ifndef             PlatformIO_LanguageIsCXX
#define             PlatformIO_LanguageIsCXX                                                 (2)
#endif
    
#ifndef             PlatformIO_Language
#if (defined __cplusplus)
#define             PlatformIO_Language                                                      PlatformIO_LanguageIsCXX
#else
#define             PlatformIO_Language                                                      PlatformIO_LanguageIsC
#endif
#endif
    
#ifndef             PlatformIO_CompilerIsUnknown
#define             PlatformIO_CompilerIsUnknown                                             (0)
#endif
    
#ifndef             PlatformIO_CompilerIsClang
#define             PlatformIO_CompilerIsClang                                               (1)
#endif
    
#ifndef             PlatformIO_CompilerIsMSVC
#define             PlatformIO_CompilerIsMSVC                                                (2)
#endif
    
#ifndef             PlatformIO_CompilerIsGCC
#define             PlatformIO_CompilerIsGCC                                                 (3)
#endif
    
#ifndef             PlatformIO_Compiler
#if   defined(__clang__)
#define             PlatformIO_Compiler                                                      (PlatformIO_CompilerIsClang)
#elif defined(_MSC_VER)
#define             PlatformIO_Compiler                                                      (PlatformIO_CompilerIsMSVC)
#elif defined(__GNUC__) || defined(__GNUG__)
#define             PlatformIO_Compiler                                                      (PlatformIO_CompilerIsGCC)
#endif
#endif /* PlatformIO_Compiler */
    
    /* POSIX shared stuff */
#if ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#include <dlfcn.h>      /* Included for shared library support */
#include <sys/socket.h> /* Included for socket support */
#include <sys/sysctl.h> /* Included for getting the number of CPU cores */
#include <unistd.h>     /* Included for stdin/stdout/stderr */
    
#ifndef             PlatformIO_File_Open
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#define             PlatformIO_File_Open(UTF8FilePath, UTF8FileMode)                         fopen((const char*) UTF8FilePath, (const char*) UTF8FileMode)
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define             PlatformIO_File_Open(UTF8FilePath, UTF8FileMode)                         fopen(reinterpret_cast<const char * __restrict>(UTF8FilePath), reinterpret_cast<const char * __restrict>(UTF8FileMode))
#endif
#endif
    
#ifndef             PlatformIO_File_Seek
#define             PlatformIO_File_Seek(File, Offset, Origin)                               fseeko(File, Offset, Origin)
#endif
    
#ifndef             PlatformIO_File_GetSize
#define             PlatformIO_File_GetSize(File)                                            ftello(File)
#endif
    
#ifndef             PlatformIO_File_Read
#define             PlatformIO_File_Read(Output, ElementSize, NumElements2Read, File2Read)   fread(Output, ElementSize, NumElements2Read, File2Read)
#endif
    
#ifndef             PlatformIO_File_Write
#define             PlatformIO_File_Write(Input, ElementSize, NumElements2Write, File2Write) fwrite(Input, ElementSize, NumElements2Write, File2Write)
#endif
    
#ifndef             PlatformIO_File_Close
#define             PlatformIO_File_Close(File2Close)                                        fclose(File2Close)
#endif
    
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
    
#if (PlatformIO_TargetOS == PlatformIO_WindowsOS)
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
    
#ifndef             PlatformIO_File_Open
#define             PlatformIO_File_Open(UTF16FilePath, UTF16FileMode)                       _wfopen(UTF16FilePath, UTF16FileMode)
#endif
    
#ifndef             PlatformIO_File_Seek
#define             PlatformIO_File_Seek(File, Offset, Origin)                               _fseeki64(File, Offset, Origin)
#endif
    
#ifndef             PlatformIO_File_GetSize
#define             PlatformIO_File_GetSize(File)                                            _ftelli64(File)
#endif
    
#ifndef             PlatformIO_File_Read
#define             PlatformIO_File_Read(Output, ElementSize, NumElements2Read, File2Read)   fread(Output, ElementSize, NumElements2Read, File2Read)
#endif
    
#ifndef             PlatformIO_File_Write
#define             PlatformIO_File_Write(Input, ElementSize, NumElements2Read, File2Write)  fwrite(Input, ElementSize, NumElements2Read, File2Write)
#endif
    
#ifndef             PlatformIO_File_Close
#define             PlatformIO_File_Close(File2Close)                                        fclose(File2Close)
#endif
    
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
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#define             PlatformIO_NewLine8                                                      (u8"\n")
#define             PlatformIO_NewLine8Size                                                  (1)
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
#define             PlatformIO_NewLine8                                                      (u8"\r\n")
#define             PlatformIO_NewLine8Size                                                  (2)
#elif (PlatformIO_TargetOS == PlatformIO_ClassicMacOS)
#define             PlatformIO_NewLine8                                                      (u8"\r")
#define             PlatformIO_NewLine8Size                                                  (1)
#endif /* TargetOS */
#endif /* PlatformIO_NewLine8 */
    
#ifndef             PlatformIO_NewLine16
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#define             PlatformIO_NewLine16                                                     (u"\n")
#define             PlatformIO_NewLine16Size                                                 (1)
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
#define             PlatformIO_NewLine16                                                     (u"\r\n")
#define             PlatformIO_NewLine16Size                                                 (2)
#elif (PlatformIO_TargetOS == PlatformIO_ClassicMacOS)
#define             PlatformIO_NewLine16                                                     (u"\r")
#define             PlatformIO_NewLine16Size                                                 (1)
#endif /* TargetOS */
#endif /* PlatformIO_NewLine16 */
    
    
#ifndef             PlatformIO_NewLine32
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#define             PlatformIO_NewLine32                                                     (U"\n")
#define             PlatformIO_NewLine32Size                                                 (1)
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
#define             PlatformIO_NewLine32                                                     (U"\r\n")
#define             PlatformIO_NewLine32Size                                                 (2)
#elif (PlatformIO_TargetOS == PlatformIO_ClassicMacOS)
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
    
#ifndef             PlatformIO_UNCPathPrefix16
#define             PlatformIO_UNCPathPrefix16                                               (u"//?/")
#endif

#ifndef             PlatformIO_UNCPathPrefix32
#define             PlatformIO_UNCPathPrefix32                                               (U"//?/")
#endif
    
#ifndef             PlatformIO_InvisibleString8
#define             PlatformIO_InvisibleString8                                              (u8"\u00A0")
#endif
    
#ifndef             PlatformIO_InvisibleCharacter16
#define             PlatformIO_InvisibleCharacter16                                          (u'\u00A0')
#endif
    
#ifndef             PlatformIO_InvisibleString16
#define             PlatformIO_InvisibleString16                                             (u"\u00A0")
#endif
    
#ifndef             PlatformIO_InvisibleCharacter8
#define             PlatformIO_InvisibleCharacter8                                          (U'\u00A0')
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
   @abstract        PlatformIO_Immutable is for pointers and arrays.
   @remark          Makes the pointer and the data it points to constant.
   */
#ifndef             PlatformIO_Immutable
#define             PlatformIO_Immutable(PointerType) const PointerType const
#endif
    
#ifndef             PlatformIO_Mutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#define             PlatformIO_Mutable(PointerType, Variable) (PointerType, Variable)
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define             PlatformIO_Mutable(PointerType, Variable) std::remove_const<PointerType>(Variable)
#endif
#endif
    
    /*!
     @abstract        PlatformIO_Volatile is for pointers and arrays.
     @remark          Makes the pointer and the data it points to volatile.
     */
#ifndef             PlatformIO_Volatile
#define             PlatformIO_Volatile(PointerType) volatile PointerType volatile
#endif
  
  /*!
   @abstract        PlatformIO_Constant is for variables, constants, and values.
   @remark          Makes the pointer and the data it points to constant.
   */
#ifndef             PlatformIO_Constant
#define             PlatformIO_Constant(Type) const Type
#endif
    
#ifndef             PlatformIO_MakeStringSet
#define             PlatformIO_MakeStringSet(StringSetSize, ...) {__VA_ARGS__,};
#endif
    
    uint64_t        PlatformOS_GetNumCPUCores(void);
    
    uint64_t        PlatformOS_GetTotalMemoryInBytes(void);
    
#ifdef __cplusplus
}
#endif

#endif  /* FoundationIO_PlatformIO_H */
