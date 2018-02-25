#include <stdarg.h>                   /* Included for va_end, va_list, va_start */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for FILE, NULL, fprintf, fclose, fopen, stderr */
#include <stdlib.h>                   /* Included for calloc, free */

#include "../include/Macros.h"        /* Included for NewLineWithNULLSize, FoundationIOTargetOS */
#include "../include/StringIO.h"      /* Included for UTF8 */
#include "../include/Log.h"           /* Included for LogTypes */

#ifdef __cplusplus
extern "C" {
#endif
    
    static FILE *Log_LogFile      = NULL;
    static UTF8 *Log_ProgramName  = NULL;
    
    void Log_SetProgramName(UTF8 *ProgramName) {
        if (ProgramName != NULL) {
            Log_ProgramName = ProgramName;
        }
    }
    
    void Log_OpenFile(UTF8 *restrict LogFilePath) {
        if (LogFilePath != NULL) {
#if   (FoundationIOTargetOS == POSIXOS)
            Log_LogFile                 = fopen(LogFilePath, "a+");
#elif (FoundationIOTargetOS == WindowsOS)
            uint64_t    LogFilePathSize = UTF8_GetSizeInCodePoints(LogFilePath);
            UTF32 LogFilePath32         = UTF8_Decode(LogFilePath, LogFilePathSize);
            UTF16 LogFilePath16         = UTF16_Encode(LogFilePath32, LogFilePathSize);
            free(LogFilePath32);
            LogFile                     = _wfopen(LogFilePath16, "a+");
            free(LogFilePath16);
#endif
        }
    }
    
    void Log(LogTypes Severity, const UTF8 FunctionName[], const UTF8 Description[], ...) {
        UTF8 Error[] = u8"ERROR";
        UTF8 Debug[] = u8"DEBUG";
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        uint64_t   StringSize       = vsnprintf(NULL, 0, Description, VariadicArguments) + NewLineWithNULLSize;
        UTF8 *VariadicString        = calloc(StringSize, sizeof(UTF8));
        vsnprintf(VariadicString, StringSize, Description, VariadicArguments);
        va_end(VariadicArguments);
        
        if (Log_LogFile == NULL) {
#if (FoundationIOTargetOS == WindowsOS)
            if (GetConsoleOutputCP() != WindowsUTF8CodePage) {
                SetConsoleOutputCP(WindowsUTF8CodePage);
            }
#endif
        }
        if (Log_ProgramName != NULL) {
            fprintf((Log_LogFile == NULL ? stderr : Log_LogFile), "%s: %s in %s: \"%s\"%s", Log_ProgramName, (Severity == Log_ERROR ? Error : Debug), FunctionName, VariadicString, NewLine);
        } else {
            fprintf((Log_LogFile == NULL ? stderr : Log_LogFile), "%s in %s: \"%s\"%s", (Severity == Log_ERROR ? Error : Debug), FunctionName, VariadicString, NewLine);
        }
        free(VariadicString);
    }
    
    void Log_Deinit(void) {
        if (Log_LogFile != NULL) {
            fclose(Log_LogFile);
        }
        if (Log_ProgramName != NULL) {
            free(Log_ProgramName);
        }
    }
    
#ifdef __cplusplus
}
#endif
