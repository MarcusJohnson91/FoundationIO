#include <stdbool.h>                  /* Included for bool true/false, Yes/No are in BitIOMacros */
#include <stdint.h>                   /* Included for u/intX_t */

#include "../include/StringIO.h"

#pragma once

#ifndef LIBBITIO_BitIOLog_H
#define LIBBITIO_BitIOLog_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header    BitIOLog.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2017+
     @version   1.0.0
     @brief     This header contains code for logging errors and debugging information.
     */
    
    /*!
     @enum                      BitIOLogTypes
     @constant                  BitIOLog_ERROR                  "An error occurred".
     @constant                  BitIOLog_DEBUG                  "Information for debugging".
     */
    typedef enum BitIOLogTypes {
                                BitIOLog_ERROR                  = 1,
                                BitIOLog_DEBUG                  = 2,
    } BitIOLogTypes;
    
    /*!
     @abstract                                                  "Sets the name or path of the program to give the logs more context".
     @param                     ProgramName                     "String containing either the name or the path of the program that this library was linked into".
     */
    extern void                 BitIOLog_SetProgramName(UTF8 *ProgramName);
    
    /*!
     @abstract                                                  "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitIOLogFile".
     @param                     LogFilePath                     "Path to the log file to open/create".
     */
    extern void                 BitIOLog_OpenFile(UTF8 *restrict LogFilePath);
    
    /*!
     @abstract                                                  "Logs to BitIOLogFile, which can be a user specified path, otherwise it's STDERR".
     @param                     Severity                        "Any of the types provided by BitIOLogTypes".
     @param                     FunctionName                    "Which function is calling BitIOLog?".
     @param                     Description                     "String describing what went wrong".
     */
    extern void                 BitIOLog(BitIOLogTypes Severity, const UTF8 FunctionName[], const UTF8 Description[], ...);
    
    /*!
     @abstract                                                  "Closes the BitIOLogFile".
     */
    void                        BitIOLog_Deinit(void);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_BitIOLog_H */
