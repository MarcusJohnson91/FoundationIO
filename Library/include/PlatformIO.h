#if (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__ANDROID__) || defined(__minix) || defined(__linux__) || defined(__unix__) || defined(_POSIX_C_SOURCE)
#ifdef  _FILE_OFFSET_BITS
#undef  _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#else
#define _FILE_OFFSET_BITS 64
#endif /* _FILE_OFFSET_BITS */
#endif /* Various UNIX Platforms */

#include <stdarg.h>                   /* Included for va_list, va_copy, va_start, va_end */
#include <stdbool.h>                  /* Included for bool */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for FILE, SEEK SET/END/CUR macros */
#include <stdlib.h>                   /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, realloc, and free */
#include <string.h>                   /* Included for memset and memmove */

#pragma once

#ifndef FoundationIO_Macros_H
#define FoundationIO_Macros_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header        Macros.h
     @author        Marcus Johnson
     @copyright     2017+
     @version       1.0.0
     @brief         This header contains preprocessor macros for generic functions in FoundationIO, and cross-platform compatibility.
     */
    
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
    
#ifndef             FoundationIOOSPOSIX
#define             FoundationIOOSPOSIX        1
#endif           /* POSIX */
    
#ifndef             FoundationIOOSWindows
#define             FoundationIOOSWindows      2
#endif           /* Windows */
    
#ifndef             FoundationIOOSMacClassic
#define             FoundationIOOSMacClassic   3
#endif           /* MacClassicOS */
    
#if defined(macintosh) || defined(Macintosh)
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS FoundationIOOSMacClassic
#endif           /* FoundationIOTargetOS */
    
#ifndef             NewLineWithNULLSize
#define             NewLineWithNULLSize    1
#endif
    
#ifndef             NewLineUTF8
#define             NewLineUTF8  u8"\r"
#endif
    
#ifndef             NewLineUTF16
#define             NewLineUTF16  u"\r"
#endif
    
#ifndef             NewLineUTF32
#define             NewLineUTF32  U"\r"
#endif
    
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__ANDROID__) || defined(__minix) || defined(__linux__) || defined(__unix__) || defined(_POSIX_C_SOURCE)
    
#include <sys/socket.h>
#include <unistd.h>
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS FoundationIOOSPOSIX
#endif
    
#ifndef             NewLineWithNULLSize
#define             NewLineWithNULLSize    1
#endif
    
#ifndef             NewLineUTF8
#define             NewLineUTF8  u8"\n"
#endif
    
#ifndef             NewLineUTF16
#define             NewLineUTF16  u"\n"
#endif
    
#ifndef             NewLineUTF32
#define             NewLineUTF32  U"\n"
#endif
    
#ifndef             typeof
#define             typeof(Variable)   __typeof__(Variable)
#endif
    
#ifndef             FoundationIO_FileOpen
#define             FoundationIO_FileOpen(File2OpenPath, FileModeString) fopen(File2OpenPath, FileModeString)
#endif
    
#ifndef             FoundationIO_FileSeek
#define             FoundationIO_FileSeek(File, Offset, Origin) fseeko(File, Offset, Origin)
#endif
    
#ifndef             FoundationIO_FileGetSize
#define             FoundationIO_FileGetSize(File) ftello(File)
#endif
    
#ifndef             FoundationIO_FileRead
#define             FoundationIO_FileRead(Output, ElementSize, NumElements2Read, File2Read) fread(Output, ElementSize, NumElements2Read, File2Read)
#endif
    
#ifndef             FoundationIO_FileWrite
#define             FoundationIO_FileWrite(Input, ElementSize, NumElements2Read, File2Write) fwrite(Input, ElementSize, NumElements2Read, File2Write)
#endif
    
#ifndef             FoundationIO_FileClose
#define             FoundationIO_FileClose(File2Close) fclose(File2Close)
#endif
    
#ifndef             FoundationIO_SocketCreate
#define             FoundationIO_SocketCreate(Family, Type, Protocol) socket(Family, Type, Protocol)
#endif
    
#ifndef             FoundationIO_SocketBind
#define             FoundationIO_SocketBind(Socket2Bind, Name, NameSize) bind(Socket2Bind, Name, NameSize)
#endif
    
#ifndef             FoundationIO_SocketListen
#define             FoundationIO_SocketListen(BoundUnconnectedSocket, BacklogSize) listen(BoundUnconnectedSocket, BacklogSize)
#endif
    
#ifndef             FoundationIO_SocketAccept
#define             FoundationIO_SocketAccept(Socket2Accept, Name, NameSize) accept(Socket2Accept, Name, NameSize)
#endif
    
#ifndef             FoundationIO_SocketConnect
#define             FoundationIO_SocketConnect(Socket2Connect, Name, NameSize) connect(Socket2Connect, Name, NameSize)
#endif
    
#ifndef             FoundationIO_SocketRead
#define             FoundationIO_SocketRead(Socket2Read, Buffer2Write, NumBytes) read(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_SocketWrite
#define             FoundationIO_SocketWrite(Socket2Read, Buffer2Write, NumBytes) write(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_SocketClose
#define             FoundationIO_SocketClose(Socket2Close) close(Socket2Close)
#endif
    
#elif    defined(WIN32) || defined(WIN32) || defined(WINNT) || defined(_WIN32) ||  defined(_WIN64)
    
#include <io.h>                       /* Included because WinSock needs it */
#include <winsock.h>                  /* Included for the socket support on Windows */
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS FoundationIOOSWindows
#endif
    
#ifndef             NewLineWithNULLSize
#define             NewLineWithNULLSize    2
#endif
    
#ifndef             NewLineUTF8
#define             NewLineUTF8  u8"\r\n"
#endif
    
#ifndef             NewLineUTF16
#define             NewLineUTF16  u"\r\n"
#endif
    
#ifndef             NewLineUTF32
#define             NewLineUTF32  U"\r\n"
#endif
    
#ifndef             typeof
#define             typeof(Variable)        __typeof(Variable)
#endif
    
#ifndef             restrict
#define             restrict                __restrict
#endif
    
#ifndef             FoundationIO_FileOpen
#pragma warning(disable: 4996)
#pragma warning(push)
#define             FoundationIO_FileOpen(File2Open, FileModeString) _wfopen(File2Open, FileModeString)
#pragma warning(pop)
#endif
    
#ifndef             FoundationIO_FileSeek
#define             FoundationIO_FileSeek(File, Offset, Origin) _fseeki64(File, Offset, Origin)
#endif
    
#ifndef             FoundationIO_FileGetSize
#define             FoundationIO_FileGetSize(File)  _ftelli64(File)
#endif
    
#ifndef             FoundationIO_FileRead
#define             FoundationIO_FileRead(Output, ElementSize, NumElements2Read, File2Read) fread(Output, ElementSize, NumElements2Read, File2Read)
#endif
    
#ifndef             FoundationIO_FileWrite
#define             FoundationIO_FileWrite(Input, ElementSize, NumElements2Read, File2Write) fwrite(Input, ElementSize, NumElements2Read, File2Write)
#endif
    
#ifndef             FoundationIO_FileClose
#define             FoundationIO_FileClose(File2Close) fclose(File2Close)
#endif
    
#ifndef             FoundationIO_SocketCreate
#define             FoundationIO_SocketCreate(Family, Type, Protocol) socket(Family, Type, Protocol)
#endif
    
#ifndef             FoundationIO_SocketBind
#define             FoundationIO_SocketBind(Socket2Bind, Name, NameSize) bind(Socket2Bind, Name, NameSize)
#endif
    
#ifndef             FoundationIO_SocketListen
#define             FoundationIO_SocketListen(BoundUnconnectedSocket, BacklogSize) listen(BoundUnconnectedSocket, BacklogSize)
#endif
    
#ifndef             FoundationIO_SocketAccept
#define             FoundationIO_SocketAccept(Socket2Accept, Name, NameSize) accept(Socket2Accept, Name, NameSize)
#endif
    
#ifndef             FoundationIO_SocketConnect
#define             FoundationIO_SocketConnect(Socket2Connect, Name, NameSize) connect(Socket2Connect, Name, NameSize)
#endif
    
#ifndef             FoundationIO_SocketRead
#define             FoundationIO_SocketRead(Socket2Read, Buffer2Write, NumBytes) read(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_SocketWrite
#define             FoundationIO_SocketWrite(Socket2Read, Buffer2Write, NumBytes) write(Socket2Read, Buffer2Write, NumBytes)
#endif
    
#ifndef             FoundationIO_SocketClose
#define             FoundationIO_SocketClose(Socket2Close) close(Socket2Close)
#endif
    
#ifndef             FoundationIOFormatStringAttribute
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
#include <sal.h>
#define             FoundationIOFormatStringAttribute(ParamBeforeVarArgs, VarArgsParam) __format_string
#elif    (_MSC_VER >= 1500)
#define             FoundationIOFormatStringAttribute(ParamBeforeVarArgs, VarArgsParam) _Printf_format_string_
#else
#define             FoundationIOFormatStringAttribute(ParamBeforeVarArgs, VarArgsParam)
#endif /* _MSC_VER */
#endif /* FoundationIOFormatStringAttribute */
    
#else
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS FoundationIOOSUnknown
#endif
    
#endif   /* End OS detection */
    
#ifndef FoundationIOCompileTimeByteOrderUnknown
#define FoundationIOCompileTimeByteOrderUnknown 0
#endif
    
#ifndef FoundationIOCompileTimeByteOrderBE
#define FoundationIOCompileTimeByteOrderBE      1
#endif
    
#ifndef FoundationIOCompileTimeByteOrderLE
#define FoundationIOCompileTimeByteOrderLE      2
#endif
    
#ifndef FoundationIOCompilerIsUnknown
#define FoundationIOCompilerIsUnknown           0
#endif
    
#ifndef FoundationIOCompilerIsClang
#define FoundationIOCompilerIsClang             1
#endif
    
#ifndef FoundationIOCompilerIsMSVC
#define FoundationIOCompilerIsMSVC              2
#endif
    
#ifndef FoundationIOCompiler
#if   defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#define FoundationIOCompiler (FoundationIOCompilerIsClang)
#elif defined(_MSC_VER)
#define FoundationIOCompiler (FoundationIOCompilerIsMSVC)
#endif
#endif /* FoundationIOCompiler */
    
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
#if   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define FoundationIOTargetByteOrder FoundationIOCompileTimeByteOrderBE
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define FoundationIOTargetByteOrder FoundationIOCompileTimeByteOrderLE
#endif /* __BYTE_ORDER__ */
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
#define FoundationIOTargetByteOrder FoundationIOCompileTimeByteOrderLE
#endif /* FoundationIOTargetOS */
    
#ifdef  FoundationIOPrivateCountVariadicArguments
#undef  FoundationIOPrivateCountVariadicArguments
#define FoundationIOPrivateCountVariadicArguments(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, Count, ...)(Count)
#elif !defined(FoundationIOPrivateCountVariadicArguments)
#define FoundationIOPrivateCountVariadicArguments(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, Count, ...)(Count)
#endif
    
#ifdef FoundationIOCountVariadicArguments
#undef FoundationIOCountVariadicArguments
#define FoundationIOCountVariadicArguments FoundationIOPrivateCountVariadicArguments(0, __VA_ARGS__,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#elif !defined(FoundationIOCountVariadicArguments)
#define FoundationIOCountVariadicArguments(...) FoundationIOPrivateCountVariadicArguments(0,__VA_ARGS__,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#endif
    
    /*!
     @enum                      FoundationIOConstants
     @constant                  NULLTerminator             "The value of a NULL terminator".
     @constant                  NULLTerminatorSize         "The size  of a NULL terminator".
     */
    typedef enum FoundationIOConstants {
                                NULLTerminator             = 0,
                                NULLTerminatorSize         = 1,
    } FoundationIOConstants;
    
#ifdef __cplusplus
}
#endif

#endif  /* FoundationIO_Macros_H */
