#include "../../include/TextIO/LogIO.h"    /* Included for the Log declarations */
#include "../../include/TextIO/FormatIO.h" /* Included for the Formatter */
#include "../../include/TextIO/StringIO.h" /* Included for StringIO's declarations */
#include "../../include/FileIO.h"          /* Included for FileIO_Close */

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
        UTF8 *ProgramName8 = NULL;
        if (Log_LogFile == NULL) {
            Log_LogFile  = stderr;
        } else {
            ProgramName8 = UTF8_Encode(Log_ProgramName);
        }

        PlatformIO_Immutable(UTF8*) Severities[3] = {
            [0] = UTF8String("ERROR"),
            [1] = UTF8String("Mistake"),
            [2] = UTF8String("Warning"),
        };

        UTF8 *SecurityName8 = NULL;
        if (Log_ProgramName != NULL) {
            uint64_t Size      = sprintf(NULL, 0, UTF8String("%s's %s in %s: "), ProgramName8, Severities[Severity - 1], FunctionName);
            SecurityName8      = UTF8_Init(Size);
            sprintf(SecurityName8, UTF8String("%s's %s in %s: "), ProgramName8, Severities[Severity - 1], FunctionName);
        } else {
            uint64_t Size      = sprintf(NULL, 0, UTF8String("%s in %s: "), Severities[Severity - 1], FunctionName);
            SecurityName8      = UTF8_Init(Size);
            sprintf(SecurityName8, UTF8String("%s in %s: "), Severities[Severity - 1], FunctionName);
        }

        UTF8 *FormattedArgs = NULL;
        va_list Arguments   = {0};
        int Size2           = vsnprintf(FormattedArgs, 0, Description, Arguments);
        FormattedArgs       = UTF8_Init(Size2);
        vsnprintf(FormattedArgs, Size2, Description, Arguments);
        va_end(Arguments);

        // Now we need to combine the parts
        UTF8 *Combined   = NULL;
        int SizeCombined = snprintf(Combined, 0, "%s %s", SecurityName8, FormattedArgs);
        Combined         = UTF8_Init(SizeCombined);
        snprintf(Combined, SizeCombined, "%s %s", SecurityName8, FormattedArgs);

        if (Log_LogFile != NULL) {
            UTF8_WriteSentence(Log_LogFile, FormattedArgs);
            fflush(Log_LogFile);
        } else {
            printf("%s", FormattedArgs);
        }
    }
    
    void Log_Deinit(void) {
        if (Log_LogFile != NULL) {
            FileIO_Close(Log_LogFile);
        }
        if (Log_ProgramName != NULL) {
            free(Log_ProgramName);
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
