#include <stdarg.h>                  /* Included for va_end, va_list, va_start */
#include <stdint.h>                  /* Included for uint64_t */
#include <stdio.h>                   /* Included for FILE */
#include <stdlib.h>                  /* Included for calloc, free */

#include "../include/StringIO.h"     /* Included for UTF8 */
#include "../include/BitIOLog.h"     /* Included for BitIOLogTypes */
#include "../include/BitIOMacros.h"  /* Included for BitIONewLineWithNULLSize, BitIOTargetOS */

#define WindowsUTF8CodePage 65001

#ifdef __cplusplus
extern "C" {
#endif
    
    static FILE  *BitIOLog_LogFile     = NULL;
    static UTF8 *BitIOLog_ProgramName  = NULL;
    
    void BitIOLog_SetProgramName(UTF8 *ProgramName) {
        if (ProgramName != NULL) {
            BitIOLog_ProgramName = ProgramName;
        }
    }
    
    void BitIOLog_OpenFile(UTF8 *restrict LogFilePath) {
        if (LogFilePath != NULL) {
#if   (BitIOTargetOS == BitIOPOSIXOS)
            BitIOLog_LogFile            = fopen(LogFilePath, "a+");
#elif (BitIOTargetOS == BitIOWindowsOS)
            uint64_t    LogFilePathSize = UTF8_GetSizeInCodePoints(LogFilePath);
            UTF32 LogFilePath32         = UTF8_Decode(LogFilePath, LogFilePathSize);
            UTF16 LogFilePath16         = UTF16_Encode(LogFilePath32, LogFilePathSize);
            free(LogFilePath32);
            BitIOLogFile                = _wfopen(LogFilePath16, "a+");
            free(LogFilePath16);
#endif
        }
    }
    
    void BitIOLog(BitIOLogTypes Severity, const UTF8 FunctionName[], const UTF8 Description[], ...) {
        UTF8 Error[] = u8"ERROR";
        UTF8 Debug[] = u8"DEBUG";
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        uint64_t   StringSize       = vsnprintf(NULL, 0, Description, VariadicArguments) + BitIONewLineWithNULLSize;
        UTF8 *VariadicString        = calloc(StringSize, sizeof(UTF8));
        vsnprintf(VariadicString, StringSize, Description, VariadicArguments);
        va_end(VariadicArguments);
        
        if (BitIOLog_LogFile == NULL) {
#if (BitIOTargetOS == BitIOWindowsOS)
            if (GetConsoleOutputCP() != WindowsUTF8CodePage) {
                SetConsoleOutputCP(WindowsUTF8CodePage);
            }
#endif
        }
        if (BitIOLog_ProgramName != NULL) {
            fprintf(BitIOLog_LogFile == NULL ? stderr : BitIOLog_LogFile, "%s: %s in %s: \"%s\"%s", BitIOLog_ProgramName, Severity == BitIOLog_ERROR ? Error : Debug, FunctionName, VariadicString, BitIONewLine);
        } else {
            fprintf(BitIOLog_LogFile == NULL ? stderr : BitIOLog_LogFile, "%s in %s: \"%s\"%s", Severity == BitIOLog_ERROR ? Error : Debug, FunctionName, VariadicString, BitIONewLine);
        }
        free(VariadicString);
    }
    
    void BitIOLog_Deinit(void) {
        if (BitIOLog_LogFile != NULL) {
            fclose(BitIOLog_LogFile);
        }
        if (BitIOLog_ProgramName != NULL) {
            free(BitIOLog_ProgramName);
        }
    }
    
#ifdef __cplusplus
}
#endif

