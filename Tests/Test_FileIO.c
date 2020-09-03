#include "../Library/include/TestIO.h"
#include "../Library/include/FileIO.h"
#include "../Library/include/CryptographyIO.h"

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

