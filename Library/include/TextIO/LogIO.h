/*!
 @header:                  LogIO.h
 @author:                  Marcus Johnson
 @copyright:               2017+
 @version:                 1.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains code for logging errors and testingR information.
 */

#pragma once

#ifndef FoundationIO_TextIO_LogIO_H
#define FoundationIO_TextIO_LogIO_H

#include "TextIOTypes.h"    /* Included for Text types */

#if PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#include <sal.h>
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @enum       LogIO_Severities
     @constant   Severity_DEBUG                  A internal error happened because there's a bug.
     @constant   Severity_USER                   An error occured because the user misused something.
     @constant   Severity_WARNING                Just an informational message.
     */
    typedef enum LogIO_Severities : uint8_t {
                 Severity_DEBUG                  = 1,
                 Severity_USER                   = 2,
                 Severity_WARNING                = 3,
    } LogIO_Severities;
    
    /*!
     @abstract                                   Sets the name or path of the program to give the logs more context.
     @param      ProgramName                     String containing either the name or the path of the program that this library was linked into.
     */
    void         Log_SetProgramName(UTF8 *ProgramName);

    /*!
     @abstract                                   Sets the Log_LogFile global variable to File.
     @remark                                     If unset stderr is the default location.
     @param      File                            Opened file to write logs to.
     */
    void         Log_SetLogFile(FILE *File);
    
    /*!
     @abstract                                   Logs to the LogFile, which can be a user specified path, otherwise it's STDERR.
     @param      Severity                        Any of the types provided by LogIO_Severities.
     @param      FunctionName                    Which function is calling Log?
     @param      Description                     String describing what went wrong.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    void         Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *) FunctionName, PlatformIO_Immutable(UTF8 *) Description, ...) __attribute__((format(printf, 3, 4)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    void         Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *) FunctionName, PlatformIO_Immutable(UTF8 *)  Description, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
    void         Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *)  FunctionName, __format_string PlatformIO_Immutable(UTF8 *) Description, ...);
#elif    (_MSC_VER >= 1500)
    void         Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *)  FunctionName, _Printf_format_string_ PlatformIO_Immutable(UTF8 *) Description, ...);
#endif /* MSVC Version */
#endif /* Compiler */
    
    /*!
     @abstract                                   Closes the LogFile.
     */
    void         Log_Deinit(void);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_LogIO_H */
