#include "../include/BitIOLog.h"

#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(push, 0)
#endif
#include <stdarg.h>  /* Included for the variadic argument support macros */
#include <stdio.h>   /* Included for the printf family */
#include <stdlib.h>  /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, and free */
#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(pop)
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
#define BitIOLogNULLStringSize 1
    
    static FILE       *BitIOLogFile     = NULL;
    static UTF8String  BitIOProgramName = NULL;
    
    void BitIOLog_SetProgramName(UTF8String ProgramName) {
        if (ProgramName != NULL) {
            BitIOProgramName = ProgramName;
        }
    }
    
    void BitIOLog_OpenFile(UTF8String LogFilePath) {
        if (LogFilePath != NULL) {
#if   (BitIOTargetOS == BitIOPOSIXOS)
            BitIOLogFile                = fopen(LogFilePath, "a+");
#elif (BitIOTargetOS == BitIOWindowsOS)
            uint64_t    LogFilePathSize = UTF8String_GetNumCodePoints(LogFilePath);
            UTF32String LogFilePath32   = UTF8String_Decode(LogFilePath, LogFilePathSize);
            UTF16String LogFilePath16   = UTF16String_Encode(LogFilePath32, LogFilePathSize);
            free(LogFilePath32);
            BitIOLogFile                = _wfopen(LogFilePath16, "a+");
            free(LogFilePath16);
#endif
        }
    }
    
    void BitIOLog(BitIOLogTypes ErrorSeverity, UTF8Constant LibraryOrProgram, UTF8Constant FunctionName, UTF8Constant Description, ...) {
        UTF8Constant ErrorString = u8"ERROR";
        UTF8Constant DebugString = u8"DEBUG";
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        UTF8String StringSizeString = calloc(2, sizeof(UTF8String));
        uint64_t StringSize = vsnprintf(StringSizeString, 2, Description, VariadicArguments);
        free(StringSizeString);
        UTF8String HardString = calloc(StringSize + BitIOLogNULLStringSize, sizeof(UTF8String));
        vsnprintf(HardString, StringSize, Description, VariadicArguments);
        va_end(VariadicArguments);
        
        fprintf(BitIOLogFile == NULL ? stderr : BitIOLogFile, "%s: %s in %s\'s %s: \"%s\"%s", BitIOProgramName, ErrorSeverity == BitIOLog_ERROR ? ErrorString : DebugString, LibraryOrProgram, FunctionName, HardString, BitIONewLine);
        free(HardString);
    }
    
    void BitIOLog_CloseFile(void) {
        if (BitIOLogFile != NULL) {
            fclose(BitIOLogFile);
        }
    }
    
#ifdef __cplusplus
}
#endif
