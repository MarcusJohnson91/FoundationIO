#include "../../include/UnicodeIO/FormatIO.h"    /* Included for the Formatter */
#include "../../include/UnicodeIO/LogIO.h"       /* Included for the Log declarations */
#include "../../include/UnicodeIO/StringIO.h"    /* Included for StringIO's declarations */
#include <stdarg.h>                              /* Included for va_list, va_copy, va_start, va_end */
#include <stdlib.h>                              /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, realloc, and free */

#ifdef __cplusplus
extern "C" {
#endif
    
    static FILE  *Log_LogFile      = NULL;
    static UTF32 *Log_ProgramName  = NULL;
    
    void Log_SetProgramName(UTF8 *ProgramName) {
        if (ProgramName != NULL) {
            Log_ProgramName        = UTF8_Decode(ProgramName);
        }
    }
    
    void Log_UTF8_OpenFile(UTF8 *LogFilePath) {
        if (LogFilePath != NULL) {
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS) || (FoundationIOTargetOS == FoundationIOAppleOS)
            bool PathHasBOM      = UTF8_HasBOM(LogFilePath);
            if (PathHasBOM) {
                UTF8 *BOMLess    = UTF8_RemoveBOM(LogFilePath);
                Log_LogFile      = FoundationIO_File_Open(BOMLess, UTF8String("a+"));
                free(BOMLess);
            } else {
                Log_LogFile      = FoundationIO_File_Open(LogFilePath, UTF8String("a+"));
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            UTF32 *Path32        = UTF8_Decode(LogFilePath);
            bool   PathHasBOM    = UTF32_HasBOM(Path32);
            bool   PathHasPrefix = UTF32_IsUNCPath(Path32);
            if (PathHasBOM == Yes && PathHasPrefix == No) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF32 *Prefixed  = UTF32_Insert(BOMLess, UNCPathPrefix32, 0);
                free(BOMLess);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == Yes) {
                UTF16 *Path16    = UTF16_Encode(Path32);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            } else if (PathHasBOM == Yes && PathHasPrefix == Yes) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF16 *Path16    = UTF16_Encode(BOMLess);
                free(BOMLess);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == No) {
                UTF32 *Prefixed  = UTF32_Insert(Path32, UNCPathPrefix32, 0);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            }
#endif
        }
    }
    
    void Log_UTF16_OpenFile(UTF16 *LogFilePath) {
        if (LogFilePath != NULL) {
            UTF32 *Path32        = UTF16_Decode(LogFilePath);
            bool   PathHasBOM    = UTF32_HasBOM(Path32);
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS) || (FoundationIOTargetOS == FoundationIOAppleOS)
            if (PathHasBOM) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF8 *Path8      = UTF8_Encode(BOMLess);
                free(BOMLess);
                Log_LogFile      = FoundationIO_File_Open(Path8, UTF8String("rb"));
                free(Path8);
            } else {
                UTF8 *Path8      = UTF8_Encode(Path32);
                Log_LogFile      = FoundationIO_File_Open(Path8, UTF8String("rb"));
                free(Path8);
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            bool   PathHasPrefix = UTF32_IsUNCPath(Path32);
            if (PathHasBOM == Yes && PathHasPrefix == No) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF32 *Prefixed  = UTF32_Insert(BOMLess, UNCPathPrefix32, 0);
                free(BOMLess);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == Yes) {
                UTF16 *Path16    = UTF16_Encode(Path32);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            } else if (PathHasBOM == Yes && PathHasPrefix == Yes) {
                UTF32 *BOMLess   = UTF32_RemoveBOM(Path32);
                UTF16 *Path16    = UTF16_Encode(BOMLess);
                free(BOMLess);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            } else if (PathHasBOM == No && PathHasPrefix == No) {
                UTF32 *Prefixed  = UTF32_Insert(Path32, UNCPathPrefix32, 0);
                UTF16 *Path16    = UTF16_Encode(Prefixed);
                free(Prefixed);
                Log_LogFile      = FoundationIO_File_Open(Path16, UTF16String("rb"));
                free(Path16);
            }
#endif
            free(Path32);
        }
    }
    
    void Log(LogTypes Severity, UTF8 *FunctionName, UTF8 *Description, ...) {
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        UTF8 *VariadicString  = UTF8_Format(Description, VariadicArguments);
        
        UTF8 *FormattedString = NULL;
        
        UTF8 *ErrorType[2]    = {UTF8String("ERROR"), UTF8String("Mistake")};
        if (Log_ProgramName != NULL) {
            FormattedString   = UTF8_Format(UTF8String("%Us in %s's %s: %s%s"), *Log_ProgramName, ErrorType[Severity - 1], FunctionName, VariadicString, FoundationIONewLine8);
        } else {
            //FormattedString   = UTF8_Format(UTF8String("%s in %s: %s%s"), ErrorType[Severity - 1], FunctionName, VariadicString, FoundationIONewLine8);
            printf(UTF8String("%s in %s: %s%s"), ErrorType[Severity - 1], FunctionName, VariadicString, FoundationIONewLine8);
        }
        
        if (Severity == Log_USER) {
            UTF8_WriteLine(stdout, FormattedString);
            fflush(stdout);
        } else if (Severity == Log_DEBUG) {
            UTF8_WriteLine(stderr, FormattedString);
            fflush(stderr);
        }
        
        if (Log_LogFile != NULL) {
            UTF8_WriteLine(Log_LogFile, FormattedString);
            fflush(Log_LogFile);
        }
        free(FormattedString);
        va_end(VariadicArguments);
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
