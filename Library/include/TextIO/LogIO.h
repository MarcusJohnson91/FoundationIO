#include <stdio.h>   /* Included for the printf family */
#include <string.h>  /* Included for atoll, memset */

#pragma  once

#ifndef  LIBBITIO_BitIOLog_H
#define  LIBBITIO_BitIOLog_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    static const char          *BitIOLogLibraryName             = "libBitIO";
    
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
    void                        BitIOLog_OpenFile(const char *LogFilePath);
    
    /*!
     @abstract                                                  "Logs to BitIOLogFile, which can be a user specified path, otherwise it's STDERR".
     @param                     ErrorSeverity                   "Any of the types provided by BitIOLogTypes".
     @param                     LibraryOrProgram                "Name of the program or library at fault".
     @param                     FunctionName                    "Which function is calling BitIOLog?".
     @param                     Description                     "String describing what went wrong".
     */
    void                        BitIOLog(BitIOLogTypes ErrorSeverity, const char *restrict LibraryOrProgram, const char *restrict FunctionName, const char *restrict Description, ...);
    
    /*!
     @abstract                                                  "Closes the BitIOLogFile".
     */
    void                        BitIOLog_CloseFile(void);
    
#ifdef   __cplusplus
}
#endif

#endif   /* LIBBITIO_BitIOLog_H */
