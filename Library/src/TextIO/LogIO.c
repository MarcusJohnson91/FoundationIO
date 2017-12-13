#include <stdarg.h>  /* Included for the variadic argument support macros */
#include <stdbool.h> /* Included for the bool type */
#include <stdint.h>  /* Included for the u/intX_t types */
#include <stdio.h>   /* Included for the FILE type, STD IN/OUT/ERR, SEEK SET/END/CUR macros */
#include <stdlib.h>  /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, and free */

#include "../include/BitIOMacros.h"
#include "../include/BitIOLog.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /* BitIOLog */
    static FILE *BitIOLogFile = NULL;
    
    void BitIOLog_OpenFile(const char *LogFilePath) {
        if (LogFilePath != NULL) {
            BitIOLogFile = fopen(LogFilePath, "a+");
        }
    }
    
    void BitIOLog(BitIOLogTypes ErrorSeverity, const char *restrict LibraryOrProgram, const char *restrict FunctionName, const char *restrict Description, ...) {
        static const char *ErrorCodeString = NULL;
        if (ErrorSeverity == BitIOLog_ERROR) {
            ErrorCodeString      = "ERROR";
        } else if (ErrorSeverity == BitIOLog_DEBUG) {
            ErrorCodeString      = "DEBUG";
        }
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        char *HardString         = NULL;
        vasprintf(&HardString, Description, VariadicArguments);
        va_end(VariadicArguments);
        
        if (BitIOLogFile == NULL) {
            fprintf(stderr, "%s %s - %s: %s%s", ErrorCodeString, LibraryOrProgram, FunctionName, HardString, BitIONewLine);
        } else {
            fprintf(BitIOLogFile, "%s: %s - %s:, %s%s", ErrorCodeString, LibraryOrProgram, FunctionName, HardString, BitIONewLine);
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
