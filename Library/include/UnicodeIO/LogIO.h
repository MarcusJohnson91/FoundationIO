/*!
 @header                    Log.h
 @author                    Marcus Johnson
 @copyright                 2017+
 @version                   1.0.0
 @brief                     This header contains code for logging errors and testingR information.
 */

#include <stdbool.h>
#include <stdint.h>

#include "../include/Macros.h"

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
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
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
#if (defined __STDC_UTF_16__ && defined __CHAR16_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
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
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char32_t         UTF32;
#else
    typedef               uint_least32_t   UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
#ifndef                   FoundationIO_Unicodize8
#define                   FoundationIO_Unicodize8              (1)
#define                   UTF8String(Literal)                  (const UTF8*) u8##Literal
#define                   UTF8Character(Literal)               (const UTF8)  u8##Literal
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                   FoundationIO_Unicodize16
#define                   FoundationIO_Unicodize16            (2)
#define                   UTF16String(Literal)                (UTF16*) u##Literal
#define                   UTF16Character(Literal)             (UTF16)  u##Literal
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32            (4)
#define                   UTF32String(Literal)                (UTF32*) U##Literal
#define                   UTF32Character(Literal)             (UTF32) U##Literal
#endif /* FoundationIO_Unicodize32 */
    
#if (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
#undef LOG_EMERG
#undef LOG_ALERT
#undef LOG_CRIT
#undef LOG_ERR
#undef LOG_WARNING
#undef LOG_NOTICE
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_KERN
#undef LOG_USER
#undef LOG_MAIL
#undef LOG_DAEMON
#undef LOG_AUTH
#undef LOG_SYSLOG
#undef LOG_LPR
#undef LOG_NEWS
#undef LOG_UUCP
#undef LOG_CRON
#undef LOG_AUTHPRIV
#undef LOG_FTP
#undef LOG_NTP
#undef LOG_LOCAL0
#undef LOG_LOCAL1
#undef LOG_LOCAL2
#undef LOG_LOCAL3
#undef LOG_LOCAL4
#undef LOG_LOCAL5
#undef LOG_LOCAL6
#undef LOG_LOCAL7
#endif
    
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
    void                        Log(LogTypes Severity, const UTF8 *FunctionName, UTF8 *Description, ...) __attribute__((__format__(__printf__, 3, 4)));
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
