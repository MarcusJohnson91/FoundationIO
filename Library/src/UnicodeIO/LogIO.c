#pragma warning(push, 0)        
#include <stdarg.h>  /* Included for the variadic argument support macros */
#include <stdio.h>   /* Included for the printf family */
#include <stdlib.h>  /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, and free */
#include <string.h>  /* Included for atoll, memset */
#pragma warning(pop)

#include "../include/BitIOMacros.h"
#include "../include/BitIOLog.h"
#include "../include/StringIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#define BitIOLogNULLStringSize 1
    
    /* BitIOLog */
    static FILE       *BitIOLogFile     = NULL;
    static UTF8String  BitIOProgramName = NULL;
    
    void BitIOLog_OpenFile(UTF8String LogFilePath) {
        if (LogFilePath != NULL) {
#if   (BitIOTargetOS == BitIOWindowsOS)
            uint64_t    LogFilePathSize = UTF8String_GetNumCodePoints(LogFilePath);
            UTF32String LogFilePath32   = UTF8String_Decode(LogFilePath, LogFilePathSize);
            UTF16String LogFilePath16   = UTF16String_Encode(LogFilePath32, LogFilePathSize);
            free(LogFilePath32);
            BitIOLogFile = _wfopen(LogFilePath16, "a+");
            free(LogFilePath16);
#elif (BitIOTargetOS == BitIOPOSIXOS)
            BitIOLogFile = fopen(LogFilePath, "a+");
#endif
        }
    }
    
    void BitIOLog_SetProgramName(UTF8String ProgramName) {
        if (ProgramName != NULL) {
            BitIOProgramName = ProgramName;
        }
    }
    
    void BitIOLog(BitIOLogTypes ErrorSeverity, UTF8Constant LibraryOrProgram, UTF8Constant FunctionName, UTF8Constant Description, ...) {
        // Get the size of the Variadic arguments, on windows use `_vscprintf`; on POSIX use
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
#if (BitIOTargetOS == BitIOWindowsOS)
        // Get the size of the variadic arguments with _vscprintf
        int32_t    VarArgSize = _vscprintf(Description, VariadicArguments) + BitIOLogNULLStringSize;
        UTF8String HardString = (UTF8String) calloc(VarArgSize, sizeof(UTF8String));
        vsprintf(HardString, Description, VariadicArguments);
#elif (BitIOTargetOS == BitIOPOSIXOS)
        UTF8String StringSizeString = calloc(1, 1);
        uint64_t StringSize = vsnprintf(StringSizeString, 1, Description, VariadicArguments);
        free(StringSizeString);
        
        UTF8String HardString = calloc(StringSize + BitIOLogNULLStringSize, sizeof(UTF8String));
        vsnprintf(HardString, StringSize, Description, VariadicArguments);
        /*
        UTF8String HardString = NULL;
        vasprintf(&HardString, Description, VariadicArguments);
         */
#endif
        va_end(VariadicArguments);
        
        if (BitIOLogFile != NULL) {
            if (ErrorSeverity == BitIOLog_ERROR) {
                fprintf(BitIOLogFile, "%s %s: %s - %s:, %s%s", BitIOProgramName, u8"ERROR", LibraryOrProgram, FunctionName, HardString, BitIONewLine);
            } else {
                fprintf(BitIOLogFile, "%s %s: %s - %s:, %s%s", BitIOProgramName, u8"DEBUG", LibraryOrProgram, FunctionName, HardString, BitIONewLine);
            }
        } else {
            if (ErrorSeverity == BitIOLog_ERROR) {
                fprintf(stderr, "%s %s %s - %s: %s%s", BitIOProgramName, u8"ERROR", LibraryOrProgram, FunctionName, HardString, BitIONewLine);
            } else {
                fprintf(stderr, "%s %s %s - %s: %s%s", BitIOProgramName, u8"DEBUG", LibraryOrProgram, FunctionName, HardString, BitIONewLine);
            }
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
