#include "../Library/include/TestIO.h"
#include "../Library/include/GUUID.h"
#include "../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    int main(const int argc, const char *argv[]) {
        bool   TestSuitePassed      = true;
        int    ExitCode             = EXIT_FAILURE;
        if (TestSuitePassed) {
            ExitCode                = EXIT_SUCCESS;
        }
        return ExitCode;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
