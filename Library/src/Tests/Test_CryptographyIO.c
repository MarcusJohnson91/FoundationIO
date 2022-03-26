#include "../../include/CryptographyIO.h"         /* Included for our declarations */
#include "../../include/TestIO.h"                 /* Included for testing */
#include "../../include/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static const UTF8 *MD5TestStrings[NumMD5TestStrings] = {
        UTF8String(""),
        UTF8String("a"),
        UTF8String("abc"),
        UTF8String("message digest"),
        UTF8String("abcdefghijklmnopqrstuvwxyz"),
        UTF8String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
        UTF8String("12345678901234567890123456789012345678901234567890123456789012345678901234567890")
    };

    static const UTF8 *MD5TestHashes[NumMD5TestStrings] = {
        UTF8String("D41D8CD98F00B204E9800998ECF8427E"),
        UTF8String("0CC175B9C0F1B6A831C399E269772661"),
        UTF8String("900150983CD24FB0D6963F7D28E17F72"),
        UTF8String("F96B697D7CB7938D525A2F31AAF161D0"),
        UTF8String("C3FCD3D76192E4007DFB496CCA67E13B"),
        UTF8String("D174AB98D277D9F5A5611C2C9F419D9F"),
        UTF8String("57EDF4A22BE3C955AC49DA2E2107B67A"),
    };
    
    bool VerifyMD5(void) {
        bool TestPassed = No;
        MD5Context MD5;
        MD5Context_Init(MD5);
        for (uint8_t TestString = 0; TestString < NumMD5TestStrings; TestString++) {
            MD5Update(MD5, MD5TestStrings[TestString], UTF8_GetStringSizeInCodeUnits(MD5TestStrings[TestString]));
            MD5Hash Hash;
            MD5Finalize(MD5, Hash);
            UTF8 *HashString = MD5ToString(Hash);
            if (!UTF8_Compare(MD5TestHashes[TestString], HashString)) {
                // Error, failed
            }
        }
        
        return TestPassed;
    }
    
    int main(void) {
        // First lets test InsecurePRNG
        InsecurePRNG *Insecure = InsecurePRNG_Init(0);

        return EXIT_SUCCESS;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
