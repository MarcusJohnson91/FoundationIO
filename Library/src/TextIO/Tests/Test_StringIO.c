#include "../libFoundationIO/include/StringIO.h"

#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"

#include "../libFoundationIO/include/CryptographyIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    UTF32 UTF32_GenerateCodePoint(Entropy *Random) {
        UTF32 CodePoint          = 0UL;
        if (Random != NULL) {
            UTF32  CodePointHigh = Entropy_GenerateIntegerInRange(Random, 1, UTF16HighSurrogateStart - 1); // 1..D7FF
            UTF32  CodePointLow  = Entropy_GenerateIntegerInRange(Random, UTF16LowSurrogateEnd + 1, UnicodeMaxCodePoint); //  E000..10FFFF
            CodePoint            = CodePointLow | CodePointHigh; // D7FE..101FFF = 0xF4801
        } else {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF32 *UTF32_GenerateString(Entropy *Random) {
        UTF32 *String                 = 0UL;
        if (Random != NULL) {
            uint16_t NumCodePoints    = Entropy_GenerateIntegerInRange(Random, 1, 8192);
            String                    = calloc(NumCodePoints + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (String != NULL) {
                for (uint16_t CodePoint = 0; CodePoint < NumCodePoints - 1; CodePoint++) {
                    String[CodePoint] = UTF32_GenerateCodePoint(Random);
                }
                WriteString(stderr, String);
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate string with %lu CodePoints"), NumCodePoints);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        }
        return String;
    }
    
    void UTF8_EncodeDecodeTest(Entropy *Random) {
        if (Random != NULL) {
            UTF32    *GeneratedString  = UTF32_GenerateString(Random);
            UTF8     *Generated8       = UTF8_Encode(GeneratedString);
            uint64_t  Generated8Units  = UTF8_GetStringSizeInCodeUnits(Generated8);
            uint64_t  Generated8Points = UTF8_GetStringSizeInCodePoints(Generated8);
            printf("StringSizeInCodeUnits: %llu\nStringSizeInCodePoints %llu\n", Generated8Units, Generated8Points);
            UTF32    *Decoded8         = UTF8_Decode(Generated8);
            bool      StringsMatch     = UTF32_Compare(GeneratedString, Decoded8);
            if (StringsMatch == No) {
                Log(Log_DEBUG, __func__, U8("Strings DO NOT match!"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    void UTF16_EncodeDecodeTest(Entropy *Random) {
        if (Random != NULL) {
            UTF32    *GeneratedString = UTF32_GenerateString(Random);
            UTF16    *Generated16     = UTF16_Encode(GeneratedString);
            UTF32    *Decoded16       = UTF16_Decode(Generated16);
            bool      StringsMatch    = UTF32_CompareSubString(GeneratedString, Decoded16, 0, 0);
            if (StringsMatch == No) {
                Log(Log_DEBUG, __func__, U8("Strings DO NOT match!"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    void FormatTest(Entropy *Random) {
        if (Random != NULL) {
            uint8_t       X = 27;
            UTF8 *Formatted = UTF8_Format(U8("X equals: %d in hex: %#x\nMemory address for x: (%p) \n"), X, X, &X);
            // "X equals: %d in hex: %#x\nMemory address for x: (%p) \n"
            UTF8_WriteLine(stdout, Formatted);
        } else {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    bool UTF8_BasicTest(void) {
        bool      TestPassed     = true;
        UTF8     *TestString8    = U8("Size: 7");
        UTF32    *TestString32   = UTF8_Decode(TestString8);
        uint64_t  TestStringSize = UTF32_GetStringSizeInCodePoints(TestString32);
        if (TestStringSize != 7) {
            TestPassed           = false;
            Log(Log_DEBUG, __func__, U8("String \"%s\" is supposed to be 7 CodePoints long, but is actually %llu"), TestString8, TestStringSize);
        }
        return TestPassed;
    }
    
    int main(int argc, const char *argv[]) {
        bool TestSuitePassed      = true;
        Entropy  *Random          = Entropy_Init(64);
        
        FormatTest(Random);
        
        //UTF8_EncodeDecodeTest(Random);
        //UTF16_EncodeDecodeTest(Random);
        
        return TestSuitePassed;
    }
    
#ifdef __cplusplus
}
#endif
