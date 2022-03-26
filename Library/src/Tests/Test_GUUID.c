#include "../../include/GUUID.h"                  /* Included for our declarations */
#include "../../include/TestIO.h"                 /* Included for testing */
#include "../../include/include/CryptographyIO.h"

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
