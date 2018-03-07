#include <stdarg.h>                    /* Included for va_end, va_list, va_start */
#include <stdint.h>                    /* Included for u/intX_t */
#include <stdio.h>                     /* Included for FILE */
#include <stdlib.h>                    /* Included for calloc, free */

#include "../include/Log.h"            /* Included for the Log declarations */
#include "../include/Macros.h"         /* Included for NewLineWithNULLSize, FoundationIOTargetOS */
#include "../include/StringIO.h"       /* Included for UTF8 */

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
            Log_LogFile   = fopen(LogFilePath, U8("a+"));
#elif (FoundationIOTargetOS == WindowsOS)
            UTF32 *Path32 = UTF8_Decode(LogFilePath);
            UTF16 *Path16 = UTF16_Encode(Path32, UseNativeByteOrder);
            free(Path32);
            Log_LogFile   = _wfopen(Path16, U16("rb"));
            free(Path16);
#endif
        }
    }
    
    void Log(LogTypes Severity, const UTF8 *FunctionName, const UTF8 *Description, ...) {
        UTF8 Error[] = U8("ERROR");
        UTF8 Debug[] = U8("DEBUG");
        
        va_list VariadicArguments;
        va_start(VariadicArguments, Description);
        uint64_t   StringSize       = vsnprintf(NULL, 0, Description, VariadicArguments) + NewLineWithNULLSize;
        UTF8 *VariadicString        = calloc(StringSize, sizeof(UTF8));
        vsnprintf(VariadicString, StringSize, Description, VariadicArguments);
        va_end(VariadicArguments);
        
        /*
         Ok, well take Description and the Variadic arguments and send it all to FormatString in StringIO
         After the string has been formatted, go ahead and do the platform dependent stuff, on POSIX encode it as UTF-8 and output it.
         on Windows encode it as UTF-16, and output it.
         
         First things first, how do we get the variadic arguments? then we need to get their type from Description.
         
         Well, what we should do is count the number of printf placeholders in the Description String.
         
         Where should that go? in the string formatter it's self?
         
         What is the process?
         
         Read the Description string for printf placeholders, and count them up, then return an array of the various placeholders type to send off to that one function?
         */
        
#if (FoundationIOTargetOS == POSIXOS)
        // Encode the formatted string as UTF-8 and send to Log_LogFile or STDERR.
#elif (FoundationIOTargetOS == WindowsOS)
        // Encode the formatted string as UTF-16 and send to Log_LogFile or STDERR.
#endif
        
        if (Log_LogFile == NULL) {
            // Log to STDERR
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
