#include "../../include/TestIO.h"           /* Included for testing */
#include "../../include/AsynchronousIO.h"   /* Included for our declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    int main(const int argc, const char *argv[]) {
        bool   TestSuitePassed      = true;
        return TestSuitePassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
