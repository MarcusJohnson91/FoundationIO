#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/Log.h"            /* Included for the Log declarations */
#include "../include/StringIO.h"       /* Included for StringIO's declarations */

#include <stdarg.h>                    /* Included for va_list, va_copy, va_start, va_end */

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
    
    void Log_UTF8_OpenFile(UTF8 *LogFilePath) {
        if (LogFilePath != NULL) {
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
            bool PathHasBOM      = UTF8_StringHasBOM(LogFilePath);
            if (PathHasBOM) {
                UTF8 *BOMLess    = UTF8_RemoveBOM(LogFilePath);
                Log_LogFile      = FoundationIO_File_Open(BOMLess, U8("a+"));
                free(BOMLess);
            } else {
                Log_LogFile      = FoundationIO_File_Open(LogFilePath, U8("a+"));
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            UTF32 *Path32        = UTF8_Decode(LogFilePath);
            bool   PathHasBOM    = UTF32_StringHasBOM(Path32);
            bool   PathHasPrefix = UTF32_StringHasWinPathPrefix(Path32);
            if (PathHasBOM == Yes && PathHasPrefix == No) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF32 *Prefixed  = UTF32_Insert(BOMLess, U32("\\\\\?\\"), 0);
                free(BOMLess);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == Yes) {
                UTF16 *Path16    = UTF16_Encode(Path32);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            } else if (PathHasBOM == Yes && PathHasPrefix == Yes) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF16 *Path16    = UTF16_Encode(BOMLess);
                free(BOMLess);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == No) {
                UTF32 *Prefixed  = UTF32_Insert(Path32, U32("\\\\\?\\"), 0);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            }
#endif
        }
    }
    
    void Log_UTF16_OpenFile(UTF16 *LogFilePath) {
        if (LogFilePath != NULL) {
            UTF32 *Path32        = UTF16_Decode(LogFilePath);
            bool   PathHasBOM    = UTF32_StringHasBOM(Path32);
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
            if (PathHasBOM) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF8 *Path8      = UTF8_Encode(BOMLess);
                free(BOMLess);
                Log_LogFile      = FoundationIO_File_Open(Path8, U8("rb"));
                free(Path8);
            } else {
                UTF8 *Path8      = UTF8_Encode(Path32);
                Log_LogFile      = FoundationIO_File_Open(Path8, U8("rb"));
                free(Path8);
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            bool   PathHasPrefix = UTF32_StringHasWinPathPrefix(Path32);
            if (PathHasBOM == Yes && PathHasPrefix == No) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF32 *Prefixed  = UTF32_Insert(BOMLess, U32("\\\\\?\\"), 0);
                free(BOMLess);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == Yes) {
                UTF16 *Path16    = UTF16_Encode(Path32);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            } else if (PathHasBOM == Yes && PathHasPrefix == Yes) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF16 *Path16    = UTF16_Encode(BOMLess);
                free(BOMLess);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == No) {
                UTF32 *Prefixed  = UTF32_Insert(Path32, U32("\\\\\?\\"), 0);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, U16("rb"));
                free(Path16);
            }
#endif
            free(Path32);
        }
    }
    
    void Log(const LogTypes Severity, const UTF8 *restrict FunctionName, UTF8 *restrict Description, ...) {
        UTF8 Error[] = U8("ERROR");
        UTF8 Test[]  = U8("TEST");
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        UTF8 *VariadicString = UTF8_FormatString(Description, VariadicArguments);
        va_end(VariadicArguments);
        
        if (Log_ProgramName != NULL) {
            UTF8  *FormattedString = UTF8_FormatString(U8("%s: %s in %s: \"%s\"%s"), Log_ProgramName, (Severity == Log_ERROR ? Error : Test), FunctionName, VariadicString, NewLineUTF8);
            UTF8_WriteLine(FormattedString, Log_LogFile == NULL ? stderr : Log_LogFile);
            free(FormattedString);
        } else {
            UTF8  *FormattedString = UTF8_FormatString(U8("%s in %s: \"%s\"%s"), (Severity == Log_ERROR ? Error : Test), FunctionName, VariadicString, NewLineUTF8);
            UTF8_WriteLine(FormattedString, Log_LogFile == NULL ? stderr : Log_LogFile);
            free(FormattedString);
        }
        free(VariadicString);
    }
    
    void Log_Deinit(void) {
        if (Log_LogFile != NULL) {
            FoundationIO_File_Close(Log_LogFile);
        }
        if (Log_ProgramName != NULL) {
            free(Log_ProgramName);
        }
    }
    
#ifdef __cplusplus
}
#endif
