#include <stdarg.h>  /* Included for the variadic argument support macros */
#include <stdio.h>   /* Included for the printf family */
#include <stdlib.h>  /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, and free */
#include <string.h>  /* Included for atoll, memset */

#include "../include/BitIOMacros.h"
#include "../include/BitIOLog.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    #define BitIOLogNULLStringSize 1
    
    /* BitIOLog */
    static FILE *BitIOLogFile     = NULL;
    static char *BitIOProgramName = NULL;
    
    void BitIOLog_OpenFile(const char *LogFilePath) {
        if (LogFilePath != NULL) {
            BitIOLogFile = fopen(LogFilePath, "a+");
        }
    }
    
    void BitIOLog_SetProgramName(char *ProgramName) {
        if (ProgramName != NULL) {
            BitIOProgramName = ProgramName;
        }
    }
    
    void BitIOLog(BitIOLogTypes ErrorSeverity, const char *restrict LibraryOrProgram, const char *restrict FunctionName, const char *restrict Description, ...) {
        static const char *ErrorCodeString = NULL;
        if (ErrorSeverity == BitIOLog_ERROR) {
            ErrorCodeString      = "ERROR";
        } else if (ErrorSeverity == BitIOLog_DEBUG) {
            ErrorCodeString      = "DEBUG";
        }
        
        // Get the size of the Variadic arguments, on windows use `_vscprintf`; on POSIX use
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
#if (BitIOTargetOS == BitIOWindowsOS)
        // Get the size of the variadic arguments with _vscprintf
        int32_t  VarArgSize = _vscprintf(Description, VariadicArguments) + BitIOLogNULLStringSize;
        char    *HardString = calloc(VarArgSize, sizeof(uint8_t));
        vsprintf(HardString, Description, VariadicArguments);
#elif (BitIOTargetOS == BitIOPOSIXOS)
        char *HardString         = NULL;
        vasprintf(&HardString, Description, VariadicArguments);
#endif
        va_end(VariadicArguments);
        
        if (BitIOLogFile == NULL) {
            fprintf(stderr, "%s %s %s - %s: %s%s", BitIOProgramName, ErrorCodeString, LibraryOrProgram, FunctionName, HardString, BitIONewLine);
        } else {
            fprintf(BitIOLogFile, "%s %s: %s - %s:, %s%s", BitIOProgramName, ErrorCodeString, LibraryOrProgram, FunctionName, HardString, BitIONewLine);
        }
        free(HardString);
    }
    
    void BitIOLog_CloseFile(void) {
        if (BitIOLogFile != NULL) {
            fclose(BitIOLogFile);
        }
    }
    /* BitIOLog */
    
#ifdef __cplusplus
}
#endif
