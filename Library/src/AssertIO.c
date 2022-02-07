#include "../include/AssertIO.h"        /* Included for our declarations */
#include "../include/TextIO/FormatIO.h" /* Included for Format */
#include "../include/TextIO/StringIO.h" /* Included for UTF8_WriteString */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void AssertIO_Message(const UTF8 *FileName, const UTF8 *FunctionName, UTF8 *ExpressionString) {
        UTF8 *AssertMessage = UTF8_Format("Assertion Failed! (%s) in %s::%s\n", ExpressionString, FileName, FunctionName);
        UTF8_WriteGrapheme(stderr, AssertMessage);
        UTF8_Deinit(AssertMessage);
#if PlatformIO_AbortFailedAsserts == true
        abort();
#endif
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
