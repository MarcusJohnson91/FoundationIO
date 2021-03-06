#include "../Library/include/TestIO.h"
#include "../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#define NumMD5TestStrings 7
#define MD5HashSizeInBytes 16
    
    static const UTF8 *MD5TestStrings[NumMD5TestStrings] = {
        UTF8String(""),
        UTF8String("a"),
        UTF8String("abc"),
        UTF8String("message digest"),
        UTF8String("abcdefghijklmnopqrstuvwxyz"),
        UTF8String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
        UTF8String("12345678901234567890123456789012345678901234567890123456789012345678901234567890")
    };
    
    static const uint8_t MD5TestHashes[NumMD5TestStrings][MD5HashSizeInBytes] = {
        [0] = {0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04, 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E},
        [1] = {0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8, 0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61},
        [2] = {0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0, 0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72},
        [3] = {0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D, 0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0},
        [4] = {0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00, 0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B},
        [5] = {0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5, 0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F},
        [6] = {0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55, 0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A},
    };
    
    bool VerifyMD5(void) {
        bool TestPassed = No;
        MD5 *MD5Data = MD5_Init();
        
        return TestPassed;
    }

    bool Test_InsecurePRNG(void) {
        bool TestPassed = No;
        uint64_t InitConstants[4] = {0xF61E2562BEBFBC70, 0xFDE5380CA3014314, 0xE9B6C7AAD9D4D039, 0x21E1CDE62AD7D2BB}; // Used Random.org data as index into MD5GeneratedConstants, 17, 40, 36, 59, 20, 45, 25, 63
        InsecurePRNG *Insecure = InsecurePRNG_Init(InitConstants);
        /*
        uint8_t 
        InsecurePRNG_Generate(Insecure, <#uint8_t *Buffer#>, <#uint64_t BufferSize#>);
*/
        return TestPassed;
    }
    
    int main(void) {
        // First lets test InsecureRNG
        SecureRNG *Random = SecureRNG_Init(12288);

        return EXIT_SUCCESS;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
