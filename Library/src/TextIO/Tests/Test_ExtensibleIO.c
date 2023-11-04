#include "../../../include/TestIO.h"       /* Included for testing */
#include "../../../include/TextIO/LogIO.h" /* Included for our declaations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    int main(const int argc, const char *argv[]) {
        bool TestSuitePassed      = Yes;
        Log(Severity_WARNING, PlatformIO_FunctionName, UTF8String("1024 = '%d'"), 1024);
        return TestSuitePassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
