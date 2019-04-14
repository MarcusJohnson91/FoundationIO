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
    
    /* Forward declare StringIO's types */
#ifndef               UTF8
#if (FoundationIOSTDVersion >= FoundationIOSTDVersionC2X && FoundationIOTargetOS != FoundationIOAppleOS)
typedef               char8_t                              UTF8;
#else
typedef               unsigned char                        UTF8;
#endif /* char8_t */
#endif /* UTF8 */
    
#ifndef               UTF16
#if (FoundationIOSTDVersion >= FoundationIOSTDVersionC11 && FoundationIOTargetOS != FoundationIOAppleOS)
typedef               char16_t                             UTF16;
#else
typedef               uint_least16_t                       UTF16;
#endif /* char16_t */
#endif /* UTF16 */
    
#ifndef               UTF32
#if (FoundationIOSTDVersion >= FoundationIOSTDVersionC11 && FoundationIOTargetOS != FoundationIOAppleOS)
typedef               char32_t                             UTF32;
#else
typedef               uint_least32_t                       UTF32;
#endif /* char32_t */
#endif /* UTF32 */
    /* Forward declare StringIO's types */
    
    /*!
     @enum                      LogTypes
     @constant                  Log_ERROR                       "An error occurred".
     @constant                  Log_TEST                        "TestIO failed".
     */
    typedef enum LogTypes {
                                Log_ERROR                       = 1,
                                Log_TEST                        = 2,
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
    void                 Log(const LogTypes Severity, const UTF8 *restrict FunctionName, UTF8 *restrict Description, ...) __attribute__((__format__(__printf__, 3, 4)));
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#include <sal.h>
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
    void                 Log(LogTypes Severity, const UTF8 *restrict FunctionName, __format_string UTF8 *restrict Description, ...);
#elif    (_MSC_VER >= 1500)
    void                 Log(LogTypes Severity, const UTF8 *restrict FunctionName, _Printf_format_string_ UTF8 *restrict Description, ...);
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
