#include "../include/Log.h"            /* Included for the Log declarations */

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
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            Log_LogFile   = FoundationIO_FileOpen(LogFilePath, U8("a+"));
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            UTF32 *Path32 = UTF8_Decode(LogFilePath);
            UTF16 *Path16 = UTF16_Encode(Path32);
            Log_LogFile   = FoundationIO_FileOpen(Path16, U16("rb"));
            free(Path32);
            free(Path16);
#endif
        }
    }
    
    void Log(LogTypes Severity, const UTF8 *restrict FunctionName, const UTF8 *restrict Description, ...) {
        UTF8 Error[] = U8("ERROR");
        UTF8 Debug[] = U8("DEBUG");
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        uint64_t   StringSize       = vsnprintf(NULL, 0, Description, VariadicArguments) + NewLineWithNULLSize;
        UTF8      *VariadicString   = calloc(StringSize, sizeof(UTF8));
        vsnprintf(VariadicString, StringSize, Description, VariadicArguments);
        va_end(VariadicArguments);
        
        if (Log_ProgramName != NULL) {
            UTF8  *FormattedString = UTF8_FormatString(U8("%s: %s in %s: \"%s\"%s"), Log_ProgramName, (Severity == Log_ERROR ? Error : Debug), FunctionName, VariadicString, NewLineUTF8);
            UTF8_WriteString2File(FormattedString, Log_LogFile == NULL ? stderr : Log_LogFile);
            free(FormattedString);
        } else {
            UTF8  *FormattedString = UTF8_FormatString(U8("%s in %s: \"%s\"%s"), (Severity == Log_ERROR ? Error : Debug), FunctionName, VariadicString, NewLineUTF8);
            UTF8_WriteString2File(FormattedString, Log_LogFile == NULL ? stderr : Log_LogFile);
            free(FormattedString);
        }
        free(VariadicString);
    }
    
    void Log_Deinit(void) {
        if (Log_LogFile != NULL) {
            FoundationIO_FileClose(Log_LogFile);
        }
        if (Log_ProgramName != NULL) {
            free(Log_ProgramName);
        }
    }
    
#ifdef __cplusplus
}
#endif
