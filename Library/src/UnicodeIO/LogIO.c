#include "../../include/UnicodeIO/LogIO.h"    /* Included for the Log declarations */
#include "../../include/UnicodeIO/FormatIO.h" /* Included for the Formatter */
#include "../../include/UnicodeIO/StringIO.h" /* Included for StringIO's declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static FILE  *Log_LogFile      = NULL;
    static UTF32 *Log_ProgramName  = NULL;
    
    void Log_SetProgramName(PlatformIO_Immutable(UTF8 *) ProgramName) {
        if (ProgramName != NULL) {
            Log_ProgramName        = UTF8_Decode(ProgramName);
        }
    }
    
    void Log_SetLogFile(FILE *File) {
        if (File != NULL) {
            Log_LogFile = File;
        }
    }
    
    void Log(LogIO_Severities Severity, PlatformIO_Immutable(UTF8 *) FunctionName, PlatformIO_Immutable(UTF8 *) Description, ...) {
        if (Log_LogFile == NULL) {
            Log_LogFile = stderr;
        }

        PlatformIO_Immutable(UTF32 *) Error   = UTF32String("ERROR");
        PlatformIO_Immutable(UTF32 *) Mistake = UTF32String("Mistake");
        
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
        
        UTF8_WriteSentence(Severity == Severity_USER ? stdout : stderr, FormattedString8);
        fflush(stdout);
        
        if (Log_LogFile != NULL) {
            UTF8_WriteSentence(Log_LogFile, FormattedString8);
            fflush(Log_LogFile);
        }
        free(FormattedString32);
        free(FormattedString8);
        va_end(VariadicArguments);
    }
    
    void Log_Deinit(void) {
        if (Log_LogFile != NULL) {
            PlatformIO_Close(Log_LogFile);
        }
        if (Log_ProgramName != NULL) {
            free(Log_ProgramName);
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
