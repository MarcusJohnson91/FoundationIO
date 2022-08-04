#include "../../include/TestIO.h"                 /* Included for testing */
#include "../../include/FileIO.h"                 /* Included for our declarations */

#include "../../include/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    bool Test_FileOpenWriteReadClose8(InsecurePRNG *Insecure) {
        bool TestPassed      = No;
        AsynchronousIOStream *Write = AsynchronousIOStream_Init();
        AsynchronousIOStream_OpenPathUTF8(Write, UTF8String("\xEF\xBB\xBF/Users/Marcus/Desktop/FileIO_Test.bin"), FileMode_Write | FileMode_Binary);
        AsynchronousIOStream_Write(Write, UTF8String("Wat"), 1, 4);
        UTF8  String[4];
        AsynchronousIOStream *Read  = AsynchronousIOStream_Init();
        AsynchronousIOStream_OpenPathUTF8(Read, UTF8String("\xEF\xBB\xBF/Users/Marcus/Desktop/FileIO_Test.bin"), FileMode_Read | FileMode_Binary);
        AsynchronousIOStream_Read(Read, &String, 1, 3);
        if (String[0] == 'W' && String[1] == 'a' && String[2] == 't') {
            TestPassed  = Yes;
        }
        FileIO_Seek(Read, -1, SeekType_Current);
        UTF8 Seeked = 0;
        FileIO_Read(Read, &Seeked, 1, 1);
        if (Seeked == 't') {
            TestPassed = Yes;
        }
        return TestPassed;
    }

    int main(const int argc, const char *argv[]) {
        bool   TestSuitePassed      = Test_FileOpenWriteReadClose8(NULL);
        int    ExitCode             = EXIT_FAILURE;
        if (TestSuitePassed) {
            ExitCode                = EXIT_SUCCESS;
        }
        return ExitCode;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

