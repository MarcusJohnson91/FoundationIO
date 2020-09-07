#include "../Library/include/TestIO.h"
#include "../Library/include/FileIO.h"
#include "../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    // Test BOM

    bool Test_FileOpenWriteReadClose(SecureRNG *Secure) {
        bool TestPassed = No;
        FILE *Write     = FileIO_OpenUTF8(UTF8String("/Users/Marcus/Desktop/FileIO_Test.bin"), FileMode_Write | FileMode_Binary);
        FileIO_Write(Write, UTF8String("Wat"), 1, 4);
        UTF8  String[4];
        FILE *Read      = FileIO_OpenUTF8(UTF8String("/Users/Marcus/Desktop/FileIO_Test.bin"), FileMode_Read | FileMode_Binary);
        FileIO_Read(Read, &String, 1, 3);
        if (String[0] == 'W' && String[1] == 'a' && String[2] == 't') {
            TestPassed  = Yes;
        }
        FileIO_Seek(Read, -1, SeekType_Current);
        UTF8 Seeked = 0;
        FileIO_Read(Read, &Seeked, 1, 1);

        return TestPassed;
    }

    int main(const int argc, const char *argv[]) {
        bool   TestSuitePassed      = Test_FileOpenWriteReadClose(NULL);
        return TestSuitePassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

