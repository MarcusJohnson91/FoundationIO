/*!
 @header        Macros.h
 @author        Marcus Johnson
 @copyright     2017+
 @version       1.0.0
 @brief         This header contains preprocessor macros for generic functions in FoundationIO, and cross-platform compatibility.
 */

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__ANDROID__) || defined(__minix) || defined(__linux__) || defined(__unix__) || defined(_POSIX_C_SOURCE) || (defined(__APPLE__) && defined(__MACH__))
#ifdef  _FILE_OFFSET_BITS
#undef  _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#else
#define _FILE_OFFSET_BITS 64
#endif /* _FILE_OFFSET_BITS */
#endif /* Various UNIX Platforms */

#include <stdbool.h>                  /* Included for bool */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for FILE, SEEK SET/END/CUR macros */
#include <stdlib.h>                   /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, realloc, and free */

#pragma once

#ifndef FoundationIO_Macros_H
#define FoundationIO_Macros_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef             Yes
#define             Yes          1
#endif           /* Yes */
#ifndef             No
#define             No           0
#endif           /* No */
#ifndef             True
#define             True         1
#endif           /* True */
#ifndef             False
#define             False        0
#endif           /* False */
    
#ifndef             FoundationIOOSUnknown
#define             FoundationIOOSUnknown      0
#endif           /* UnknownOS */
    
#ifndef             FoundationIOPOSIXOS
#define             FoundationIOPOSIXOS        1
#endif           /* POSIX */
    
#ifndef             FoundationIOWindowsOS
#define             FoundationIOWindowsOS      2
#endif           /* Windows */
    
#ifndef             FoundationIOMacClassicOS
#define             FoundationIOMacClassicOS   4
#endif           /* MacClassicOS */
    
#if defined(macintosh) || defined(Macintosh)
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS FoundationIOMacClassicOS
#endif           /* FoundationIOTargetOS */
    
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__ANDROID__) || defined(__minix) || defined(__linux__) || defined(__unix__) || defined(_POSIX_C_SOURCE)
    
#include <dlfcn.h>      /* Included for shared library support */
#include <sys/socket.h> /* Included for socket support */
#include <sys/sysctl.h> /* Included for getting the number of CPU cores */
#include <unistd.h>     /* Included for stdin/stdout/stderr */
    
#define FoundationIOTargetOS (FoundationIOPOSIXOS)

#ifndef             FoundationIO_File_Open
#define             FoundationIO_File_Open(File2OpenPath, FileModeString)                      fopen(File2OpenPath, FileModeString)
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
    
#ifndef             FoundationIO_SharedLibrary_Open
#define             FoundationIO_SharedLibrary_Open(PathUTF8, Mode)                            dlopen(PathUTF8, Mode)
#endif
    
#ifndef             FoundationIO_SharedLibrary_GetSymbol
#define             FoundationIO_SharedLibrary_GetSymbol(LibraryAddress, Symbol)               dlsym(LibraryAddress, Symbol)
#endif
    
#ifndef             FoundationIO_SharedLibrary_Close
#define             FoundationIO_SharedLibrary_Close(LibraryAddress)                           dlclose(LibraryAddress)
#endif
    
#elif    defined(WIN32) || defined(WIN32) || defined(WINNT) || defined(_WIN32) ||  defined(_WIN64)
    
#ifndef   WIN32_LEAN_AND_MEAN
#define   WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */
#ifndef   VC_EXTRALEAN
#define   VC_EXTRALEAN
#endif /* VC_EXTRALEAN */
#include <Windows.h>                  /* Included for Shared Library support, Winsock, WinCon */
#include <WinSock2.h>                 /* Included for the socket support on Windows */
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS (FoundationIOWindowsOS)
#endif
    
#ifndef             typeof
#define             typeof(Variable)                                                           __typeof(Variable)
#endif
    
#ifndef             restrict
#define             restrict                                                                   __restrict
#endif
    
#ifndef             FoundationIO_File_Open
#define             FoundationIO_File_Open(File2Open, FileModeString)                          _wfopen(File2Open, FileModeString)
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
    
#ifndef             FoundationIO_SharedLibrary_Open
#define             FoundationIO_SharedLibrary_Open(PathUTF16, Mode)                           LoadLibraryEx(PathUTF16, NULL, Mode)
#endif
    
#ifndef             FoundationIO_SharedLibrary_GetSymbol
#define             FoundationIO_SharedLibrary_GetSymbol(LibraryAddress, Symbol)               GetProcAddress(LibraryAddress, Symbol)
#endif
    
#ifndef             FoundationIO_SharedLibrary_Close
#define             FoundationIO_SharedLibrary_Close(LibraryAddress)                           FreeLibrary(LibraryAddress)
#endif
    
#else
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS FoundationIOOSUnknown
#endif
    
#endif   /* End OS detection */
    
#ifndef             FoundationIOCompileTimeByteOrderUnknown
#define             FoundationIOCompileTimeByteOrderUnknown                (0)
#endif
    
#ifndef             FoundationIOCompileTimeByteOrderBE
#define             FoundationIOCompileTimeByteOrderBE                     (1)
#endif
    
#ifndef             FoundationIOCompileTimeByteOrderLE
#define             FoundationIOCompileTimeByteOrderLE                     (2)
#endif
    
#ifndef             FoundationIOCompilerIsUnknown
#define             FoundationIOCompilerIsUnknown                          (0)
#endif
    
#ifndef             FoundationIOCompilerIsClang
#define             FoundationIOCompilerIsClang                            (1)
#endif
    
#ifndef             FoundationIOCompilerIsMSVC
#define             FoundationIOCompilerIsMSVC                             (2)
#endif
    
#ifndef             FoundationIOCompilerIsGCC
#define             FoundationIOCompilerIsGCC                              (3)
#endif
    
#ifndef             FoundationIOCompiler
#if   defined(__clang__)
#define             FoundationIOCompiler                                   (FoundationIOCompilerIsClang)
#elif defined(_MSC_VER)
#define             FoundationIOCompiler                                   (FoundationIOCompilerIsMSVC)
#elif defined(__GNUC__) || defined(__GNUG__)
#define             FoundationIOCompiler                                   (FoundationIOCompilerIsGCC)
#endif
#endif /* FoundationIOCompiler */
    
#if   (FoundationIOCompiler == FoundationIOCompilerIsClang || FoundationIOCompiler == FoundationIOCompilerIsGCC)
#if   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define             FoundationIOTargetByteOrder                            (FoundationIOCompileTimeByteOrderBE)
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define             FoundationIOTargetByteOrder                            (FoundationIOCompileTimeByteOrderLE)
#endif /* __BYTE_ORDER__ */
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#define             FoundationIOTargetByteOrder                            (FoundationIOCompileTimeByteOrderLE)
#endif /* FoundationIOCompiler */
    
#ifndef NewLineUTF8
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
#define             NewLineUTF8                                            (u8"\n")
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#define             NewLineUTF8                                            (u8"\r\n")
#elif (FoundationIOTargetOS == FoundationIOMacClassicOS)
#define             NewLineUTF8                                            (u8"\r")
#endif /* TargetOS */
#endif /* Ifndef NewLineUTF8 */
    
#ifndef NewLineUTF16
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
#define             NewLineUTF16                                           (u"\n")
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#define             NewLineUTF16                                           (u"\r\n")
#elif (FoundationIOTargetOS == FoundationIOMacClassicOS)
#define             NewLineUTF16                                           (u"\r")
#endif /* TargetOS */
#endif /* Ifndef NewLineUTF16 */
    
#ifndef NewLineUTF32
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
#define             NewLineUTF32                                           (U"\n")
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#define             NewLineUTF32                                           (U"\r\n")
#elif (FoundationIOTargetOS == FoundationIOMacClassicOS)
#define             NewLineUTF32                                           (U"\r")
#endif /* TargetOS */
#endif /* Ifndef NewLineUTF32 */
    
    uint64_t FoundationIO_GetNumCPUCores(void);
    
#ifdef __cplusplus
}
#endif

#endif  /* FoundationIO_Macros_H */
