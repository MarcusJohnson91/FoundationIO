/*!
 @header                    Log.h
 @author                    Marcus Johnson
 @copyright                 2017+
 @version                   1.0.0
 @brief                     This header contains code for logging errors and testingR information.
 */

#include <stdbool.h>
#include <stdint.h>

#include "Macros.h"

#pragma once

#ifndef FoundationIO_Log_H
#define FoundationIO_Log_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef   char8_t        UTF8;
#else
    typedef   unsigned char  UTF8;
#endif /* __CHAR8_TYPE__ */
#endif /* FoundationIO_StringType8 */
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
#if (defined __STDC_UTF_16__ && defined __CHAR16_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef                     char16_t                        UTF16;
#else
    typedef                     uint_least16_t                  UTF16;
#endif /* __CHAR16_TYPE__ */
#endif /* FoundationIO_StringType16 */
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char32_t         UTF32;
#else
    typedef               uint_least32_t   UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
    /* Define StringIO's Unicodeization macros */
#ifndef                         FoundationIO_Unicodize8
#define                         FoundationIO_Unicodize8         (1)
#define                         U8(QuotedLiteral)               u8##QuotedLiteral
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                         FoundationIO_Unicodize16
#define                         FoundationIO_Unicodize16        (2)
#define                         U16(QuotedLiteral)              u##QuotedLiteral
#endif /* FoundationIO_Unicodize16 */
    /* Define StringIO's Unicodeization macros */
    
    /*!
     @enum                      LogTypes
     @constant                  Log_DEBUG                       "A internal error happened because there's a bug".
     @constant                  Log_USER                        "An error occured because the user misused something".
     */
    typedef enum LogTypes {
                                Log_DEBUG                       = 1,
                                Log_USER                        = 2,
    } LogTypes;
    
    /*!
     @abstract                                                  "Sets the name or path of the program to give the logs more context".
     @param                     ProgramName                     "String containing either the name or the path of the program that this library was linked into".
     */
    extern void                 Log_SetProgramName(UTF8 *ProgramName);
    
    /*!
     @abstract                                                  "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in LogFile".
     @param                     LogFilePath                     "Path to the log file to open/create".
     */
    extern void                 Log_UTF8_OpenFile(UTF8 *LogFilePath);
    
    /*!
     @abstract                                                  "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in LogFile".
     @param                     LogFilePath                     "Path to the log file to open/create".
     */
    extern void                 Log_UTF16_OpenFile(UTF16 *LogFilePath);
    
    /*!
     @abstract                                                  "Logs to the LogFile, which can be a user specified path, otherwise it's STDERR".
     @param                     Severity                        "Any of the types provided by LogTypes".
     @param                     FunctionName                    "Which function is calling Log?".
     @param                     Description                     "String describing what went wrong".
     */
#if   (FoundationIOCompiler == FoundationIOCompilerIsClang || FoundationIOCompiler == FoundationIOCompilerIsGCC)
    void                        Log(LogTypes Severity, const UTF8 *FunctionName, UTF8 *Description, ...);// __attribute__((__format__(__printf__, 3, 4)));
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#include <sal.h>
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
    void                        Log(LogTypes Severity, const UTF8 *FunctionName, __format_string UTF8 *Description, ...);
#elif    (_MSC_VER >= 1500)
    void                        Log(LogTypes Severity, const UTF8 *FunctionName, _Printf_format_string_ UTF8 *Description, ...);
#endif /* MSVC Version */
#endif /* Compiler */
    
    /*!
     @abstract                                                  "Closes the LogFile".
     */
    void                        Log_Deinit(void);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_Log_H */
