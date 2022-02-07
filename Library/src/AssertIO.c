#include "../include/AssertIO.h"        /* Included for our declarations */
#include "../include/TextIO/FormatIO.h" /* Included for Format */
#include "../include/TextIO/StringIO.h" /* Included for UTF8_WriteString */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void AssertIO_Message(const UTF8 *FileName, const UTF8 *FunctionName, UTF8 *ExpressionString, ...) {
        // Check if there's any specifiers in Expression
        uint64_t NumSpecifiers = UTF8_GetNumFormatSpecifiers(ExpressionString);
        UTF8    *ExpressionVA  = (UTF8*) ExpressionString;
        if (NumSpecifiers > 0) {
            va_list Arguments;
            va_start(Arguments, ExpressionString);
            ExpressionVA = UTF8_Format(ExpressionVA, Arguments);
            va_end(Arguments);
        }
        UTF8 *Formatted = UTF8_Format(UTF8String("Assertion '%s' in %s::%s Failed%s"), ExpressionVA, FileName, FunctionName, TextIO_NewLine8);
        UTF8_File_WriteString(stderr, Formatted);
        UTF8_Deinit(Formatted);
#if PlatformIO_AbortFailedAsserts == true
        abort();
#endif
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
