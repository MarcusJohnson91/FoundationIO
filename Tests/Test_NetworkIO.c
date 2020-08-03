#include "../Library/include/CryptographyIO.h"
#include "../Library/include/TestIO.h"
#include "../Library/include/NetworkIO.h"
#include "../Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    int main(int argc, const char *argv[]) {
        bool   TestSuitePassed      = true;
        return TestSuitePassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
