#include <stdint.h>                   /* Included for u/intX_t */

#include "../include/StringIO.h"      /* Included for UTF8 */

#pragma once

#ifndef FoundationIO_Log_H
#define FoundationIO_Log_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header    Log.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2017+
     @version   1.0.0
     @brief     This header contains code for logging errors and debugging information.
     */
    
    /*!
     @enum                      LogTypes
     @constant                  Log_ERROR                       "An error occurred".
     @constant                  Log_DEBUG                       "Information for debugging".
     */
    typedef enum LogTypes {
        Log_ERROR                       = 1,
        Log_DEBUG                       = 2,
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
    extern void                 Log_OpenFile(UTF8 *restrict LogFilePath);
    
    /*!
     @abstract                                                  "Logs to the LogFile, which can be a user specified path, otherwise it's STDERR".
     @param                     Severity                        "Any of the types provided by LogTypes".
     @param                     FunctionName                    "Which function is calling Log?".
     @param                     Description                     "String describing what went wrong".
     */
    extern void                 Log(LogTypes Severity, const UTF8 FunctionName[], const UTF8 Description[], ...);
    
    /*!
     @abstract                                                  "Closes the LogFile".
     */
    void                        Log_Deinit(void);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_Log_H */
