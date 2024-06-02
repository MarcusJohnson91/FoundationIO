#include "../../include/TextIO/LogIO.h"    /* Included for our declarations */

#include "../../include/AssertIO.h"        /* Included for Assertions */
#include "../../include/FileIO.h"          /* Included for FileIO_Close */
#include "../../include/TextIO/FormatIO.h" /* Included for the Formatter */
#include "../../include/TextIO/StringIO.h" /* Included for StringIO's declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static FILE          *Log_LogFile       = NULL;
    static AsynchronousIOStream *Async_LogFile     = NULL;
    static UTF8          *Log_ProgramName8  = NULL;
    
    void Log_SetProgramName(UTF8 *ProgramName) {
        AssertIO(ProgramName != NULL);
        Log_ProgramName8 = ProgramName;
    }
    
    void Log_SetLogFile(FILE *File) {
        AssertIO(File != NULL);
        Log_LogFile = File;
    }
    
    static void Log_OpenLogFilePath(ImmutableString_UTF8 Path) {
        AssertIO(Path != NULL);

        UTF8 *FoldedPath = UTF8_CaseFold(Path);
        Async_LogFile    = AsynchronousIOStream_Init();

        if (UTF8_Compare(FoldedPath, UTF8String("stdin")) ||
            UTF8_Compare(FoldedPath, UTF8String("zero")) ||
            UTF8_Compare(FoldedPath, UTF8String("0"))
            ) { // STDIN
        } else if (UTF8_Compare(FoldedPath, UTF8String("stdout")) ||
                   UTF8_Compare(FoldedPath, UTF8String("one")) ||
                   UTF8_Compare(FoldedPath, UTF8String("1"))
                   ) { // STDOUT
        } else if (UTF8_Compare(FoldedPath, UTF8String("stderr")) ||
                   UTF8_Compare(FoldedPath, UTF8String("two")) ||
                   UTF8_Compare(FoldedPath, UTF8String("2"))
                   ) { // STDERR
        }
    }
    
    void Log(LogIO_Severities Severity, ImmutableString_UTF8 FunctionName, ImmutableString_UTF8 Description, ...) {
        AssertIO(FunctionName != NULL);
        AssertIO(Description != NULL);
        if (Log_LogFile == NULL) {
            Log_LogFile  = stderr;
        }

        ImmutableString_UTF8 Severities[3] = {
            [0] = UTF8String("ERROR"),
            [1] = UTF8String("Mistake"),
            [2] = UTF8String("Warning"),
        };

        UTF8 *SecurityName8 = NULL;
        ImmutableString_UTF8 WarnString = Severities[Severity - 1];
        if (Log_ProgramName8 != NULL) {
            int Size           = snprintf(NULL, 0, "%s's %s in %s: ", Log_ProgramName8, WarnString, FunctionName);
            SecurityName8      = UTF8_Init((size_t) Size);
            snprintf((char*) SecurityName8, Size, "%s's %s in %s: ", Log_ProgramName8, WarnString, FunctionName);
        } else {
            int Size           = snprintf(NULL, 0, "%s in %s: ", WarnString, FunctionName);
            SecurityName8      = UTF8_Init((size_t) Size);
            snprintf((char*) SecurityName8, Size, "%s in %s: ", WarnString, FunctionName);
        }

        va_list Arguments;
        va_start(Arguments, Description);
        int Size2           = vsnprintf(NULL, 0, (char*) Description, Arguments);
        UTF8 *FormattedArgs = UTF8_Init((size_t) Size2);
        AssertIO(FormattedArgs != NULL);
        vsnprintf((char*) FormattedArgs, Size2, (char*) Description, Arguments);
        va_end(Arguments);

        // Now we need to combine the parts
        UTF8 *Combined   = NULL;
        int SizeCombined = snprintf(NULL, 0, "%s %s", SecurityName8, FormattedArgs);
        Combined         = UTF8_Init((size_t) SizeCombined);
        AssertIO(Combined != NULL);
        snprintf((char*) Combined, SizeCombined, "%s %s", SecurityName8, FormattedArgs);
        
        free(FormattedArgs);
        free(SecurityName8);

        if (Log_LogFile != NULL) {
            UTF8_File_WriteString(Log_LogFile, Combined);
            fflush(Log_LogFile);
        } else {
            printf("%s", Combined);
        }
    }
    
    void Log_Deinit(void) {
        if (Log_LogFile != NULL) {
            FileIO_Close(Log_LogFile);
        }
        if (Log_ProgramName8 != NULL) {
            free(Log_ProgramName8);
        }
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
