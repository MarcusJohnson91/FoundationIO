#include "../../include/UnicodeIO/LogIO.h"       /* Included for the Log declarations */
#include "../../include/UnicodeIO/FormatIO.h"    /* Included for the Formatter */
#include "../../include/UnicodeIO/StringIO.h"    /* Included for StringIO's declarations */

#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
extern "C" {
#endif
    
    static FILE  *Log_LogFile      = NULL;
    static UTF32 *Log_ProgramName  = NULL;
    static UTF8  *Log_Path         = UTF8String("/Users/Marcus/Desktop/FormatIO_Test.log");
    
    void Log_SetProgramName(UTF8 *ProgramName) {
        if (ProgramName != NULL) {
            Log_ProgramName        = UTF8_Decode(ProgramName);
        }
    }
    
    void Log_UTF8_OpenFile(UTF8 *LogFilePath) {
        if (LogFilePath != NULL) {
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
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
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
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
    
    void Log(LogIO_Severities Severity, const UTF8 *FunctionName, UTF8 *Description, ...) {
        if (Log_Path != NULL && Log_LogFile == NULL) {
            Log_UTF8_OpenFile(Log_Path);
        }
        
        UTF32 *Error    = UTF32String("ERROR");
        UTF32 *Mistake  = UTF32String("Mistake");
        
        UTF32 *SecurityName   = NULL;
        if (Log_ProgramName != NULL) {
            SecurityName      = UTF32_Format(UTF32String("%Us's %Us in %s: "), Log_ProgramName, Severity == Severity_DEBUG ? Error : Mistake, FunctionName);
        } else {
            SecurityName      = UTF32_Format(UTF32String("%Us in %s: "), Severity == Severity_DEBUG ? Error : Mistake, FunctionName);
            // "ERROR in %s: "
            // "ERRORn %s: "; Ate " i" aka 2 codepoints too far?
        }
        
        UTF32 *Description32  = UTF8_Decode(UTF8_MakeStringMutable(Description));
        UTF32 *Description2   = UTF32_Insert(Description32, SecurityName, 0);
        free(SecurityName);
        free(Description32);
        
        uint64_t NumVariadicArguments = UTF32_GetNumFormatSpecifiers(Description2);
        FormatSpecifiers *Specifiers  = FormatSpecifiers_Init(NumVariadicArguments);
        UTF32_ParseFormatSpecifiers(Description2, Specifiers, StringType_UTF32);
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        Format_Specifiers_RetrieveArguments(Specifiers, VariadicArguments);
        va_end(VariadicArguments);
        
        uint64_t FormattedStringSize  = UTF32_GetFormattedStringSize(Description2, Specifiers);
        UTF32 *FormattedString32      = FormatString_UTF32(Description2, Specifiers, FormattedStringSize);
        UTF8  *FormattedString8       = UTF8_Encode(FormattedString32);
        
        UTF8_WriteLine(Severity == Severity_USER ? stdout : stderr, FormattedString8);
        fflush(stdout);
        
        if (Log_LogFile != NULL) {
            UTF8_WriteLine(Log_LogFile, FormattedString8);
            fflush(Log_LogFile);
        }
        free(FormattedString32);
        free(FormattedString8);
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
    
#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
}
#endif
