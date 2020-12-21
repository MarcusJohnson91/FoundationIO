#include "../../Library/include/TestIO.h"
#include "../../Library/include/TextIO/LogIO.h"

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
