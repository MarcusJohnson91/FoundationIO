/*!
 @header              LogIO.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for logging errors and testingR information.
 */

#include "../PlatformIO.h"
#include "UnicodeIOTypes.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_LogIO_H
#define FoundationIO_UnicodeIO_LogIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum                      LogIO_Severities
     @constant                  Log_DEBUG                        A internal error happened because there's a bug.
     @constant                  Log_USER                         An error occured because the user misused something.
     */
    typedef enum LogIO_Severities {
                                Severity_DEBUG                  = 1,
                                Severity_USER                   = 2,
    } LogIO_Severities;
    
    /*!
     @abstract                                                   Sets the name or path of the program to give the logs more context.
     @param                     ProgramName                      String containing either the name or the path of the program that this library was linked into.
     */
    extern void                 Log_SetProgramName(PlatformIO_Immutable(UTF8 *) ProgramName);
    
    /*!
     @abstract                                                   Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in LogFile.
     @param                     LogFilePath                      Path to the log file to open/create.
     */
    extern void                 Log_UTF8_OpenFile(PlatformIO_Immutable(UTF8 *) LogFilePath);
    
    /*!
     @abstract                                                   Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in LogFile.
     @param                     LogFilePath                      Path to the log file to open/create.
     */
    extern void                 Log_UTF16_OpenFile(PlatformIO_Immutable(UTF16 *)LogFilePath);
    
    /*!
     @abstract                                                   Logs to the LogFile, which can be a user specified path, otherwise it's STDERR.
     @param                     Severity                         Any of the types provided by LogIO_Severities.
     @param                     FunctionName                     Which function is calling Log?
     @param                     Description                      String describing what went wrong.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    void                        Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *) FunctionName, PlatformIO_Immutable(UTF8 *) Description, ...) __attribute__((__format__(__printf__, 3, 4)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    void                        Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *) FunctionName, PlatformIO_Immutable(UTF8 *) Description, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#include <sal.h>
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
    void                        Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *) FunctionName, __format_string PlatformIO_Immutable(UTF8 *) Description, ...);
#elif    (_MSC_VER >= 1500)
    void                        Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *) FunctionName, _Printf_format_string_ PlatformIO_Immutable(UTF8 *) Description, ...);
#endif /* MSVC Version */
#endif /* Compiler */
    
    /*!
     @abstract                                                   Closes the LogFile.
     */
    void                        Log_Deinit(void);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_UnicodeIO_LogIO_H */
