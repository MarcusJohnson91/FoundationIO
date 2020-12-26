#include "../../include/TextIO/LogIO.h"    /* Included for the Log declarations */
#include "../../include/TextIO/FormatIO.h" /* Included for the Formatter */
#include "../../include/TextIO/StringIO.h" /* Included for StringIO's declarations */
#include "../../include/FileIO.h"          /* Included for FileIO_Close */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static FILE          *Log_LogFile     = NULL;
    static AsyncIOStream *Async_LogFile   = NULL;
    static UTF32         *Log_ProgramName = NULL;
    
    void Log_SetProgramName(PlatformIO_Immutable(UTF8 *) ProgramName) {
        if (ProgramName != NULL) {
            Log_ProgramName  = UTF8_Decode(ProgramName);
        }
    }
    
    void Log_SetLogFile(FILE *File) {
        if (File != NULL) {
            Log_LogFile = File;
        }
    }
    
    void Log_OpenLogFilePath(PlatformIO_Immutable(UTF8 *) Path) {
        if (Path != NULL && Async_LogFile == NULL) {
            UTF8 *FoldedPath = UTF8_CaseFold(Path);
            Async_LogFile    = AsyncIOStream_Init();
            
            if (UTF8_Compare(FoldedPath, UTF8String("stdin")) ||
                UTF8_Compare(FoldedPath, UTF8String("zero")) ||
                UTF8_Compare(FoldedPath, UTF8String("0"))
                ) {
                // STDIN
            } else if (UTF8_Compare(FoldedPath, UTF8String("stdout")) ||
                UTF8_Compare(FoldedPath, UTF8String("one")) ||
                UTF8_Compare(FoldedPath, UTF8String("1"))
                ) {
                // STDOUT
            } else if (UTF8_Compare(FoldedPath, UTF8String("stderr")) ||
                UTF8_Compare(FoldedPath, UTF8String("two")) ||
                UTF8_Compare(FoldedPath, UTF8String("2"))
                ) {
                // STDERR
            }
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
        PlatformIO_Immutable(UTF8*) WarnString = Severities[Severity - 1];
        if (Log_ProgramName != NULL) {
            uint64_t Size      = snprintf(NULL, 0, UTF8String("%s's %s in %s: "), ProgramName8, WarnString, FunctionName);
            SecurityName8      = UTF8_Init(Size);
            snprintf(SecurityName8, Size, UTF8String("%s's %s in %s: "), ProgramName8, WarnString, FunctionName);
        } else {
            uint64_t Size      = snprintf(NULL, 0, UTF8String("%s in %s: "), WarnString, FunctionName);
            SecurityName8      = UTF8_Init(Size);
            snprintf(SecurityName8, Size, UTF8String("%s in %s: "), WarnString, FunctionName);
        }
        free(ProgramName8);

        va_list Arguments;
        va_start(Arguments, Description);
        int Size2           = vsnprintf(NULL, 0, Description, Arguments);
        UTF8 *FormattedArgs = UTF8_Init(Size2);
        vsnprintf(FormattedArgs, Size2, Description, Arguments);
        va_end(Arguments);

        // Now we need to combine the parts
        UTF8 *Combined   = NULL;
        int SizeCombined = snprintf(NULL, 0, "%s %s", SecurityName8, FormattedArgs);
        Combined         = UTF8_Init(SizeCombined);
        snprintf(Combined, SizeCombined, "%s %s", SecurityName8, FormattedArgs);
        
        free(FormattedArgs);
        free(SecurityName8);

        if (Log_LogFile != NULL) {
            UTF8_WriteSentence(Log_LogFile, Combined);
            fflush(Log_LogFile);
        } else {
            printf("%s", Combined);
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
