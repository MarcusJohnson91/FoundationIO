#include "../include/BitIOMacros.h"
#include "../include/StringIO.h"

#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(push, 0)
#endif
#include <stdbool.h>
#include <stdint.h>
#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(pop)
#endif

#pragma  once

#ifndef  LIBBITIO_BitIOLog_H
#define  LIBBITIO_BitIOLog_H

#ifdef   __cplusplus
extern   "C" {
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
     @constant                  BitIOLog_DEBUG                  "Used for debugging".
     */
    typedef enum BitIOLogTypes {
                                BitIOLog_ERROR                  = 1,
                                BitIOLog_DEBUG                  = 2,
    } BitIOLogTypes;
    
    /*!
     @abstract                                                  "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitIOLogFile".
     @param                     LogFilePath                     "Path to the log file to open/create".
     */
    void                        BitIOLog_OpenFile(UTF8String LogFilePath);
    
    /*!
     @abstract                                                  "Sets the name or path of the program to give the logs more context".
     @param                     ProgramName                     "String containing either the name or the path of the program that this library was linked into".
     */
    void                        BitIOLog_SetProgramName(UTF8String ProgramName);
    
    /*!
     @abstract                                                  "Logs to BitIOLogFile, which can be a user specified path, otherwise it's STDERR".
     @param                     ErrorSeverity                   "Any of the types provided by BitIOLogTypes".
     @param                     LibraryOrProgram                "Name of the program or library at fault".
     @param                     FunctionName                    "Which function is calling BitIOLog?".
     @param                     Description                     "String describing what went wrong".
     */
    void                        BitIOLog(BitIOLogTypes ErrorSeverity, UTF8Constant LibraryOrProgram, UTF8Constant FunctionName, UTF8Constant Description, ...);
    
    /*!
     @abstract                                                  "Closes the BitIOLogFile".
     */
    void                        BitIOLog_CloseFile(void);
    
#ifdef   __cplusplus
}
#endif

#endif   /* LIBBITIO_BitIOLog_H */
