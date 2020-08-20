#include "../../Library/include/TestIO.h"
#include "../../Library/include/TextIO/StringIO.h"
#include "../../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     Colorize Text API:
     
     Colorize(Color, String)
     
     Where Color is an enum contining a list of possible colors.
     
     Let's put it in CommandLineIO
     */
    
    void Test_UTF8_EncodeDecode(SecureRNG *Secure) {
        if (Secure != NULL) {
            uint64_t  NumCodePoints    = SecureRNG_GenerateInteger(Secure, 16);
            PlatformIO_Immutable(UTF32*) GeneratedString  = UTF32_GenerateString(Secure, NumCodePoints);
            UTF8     *Generated8       = UTF8_Encode(GeneratedString);
            PlatformIO_Immutable(UTF32*) Decoded8         = UTF8_Decode(Generated8);
            bool      StringsMatch     = UTF32_Compare(GeneratedString, Decoded8);
            if (StringsMatch == No) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Strings DO NOT match!"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
    }
    
    bool Test_UTF16_EncodeDecode(SecureRNG *Secure) {
        bool TestPassed = true;
        if (Secure != NULL) {
            uint64_t  NumCodePoints                      = SecureRNG_GenerateInteger(Secure, 16);
            PlatformIO_Immutable(UTF32*) GeneratedString = UTF32_GenerateString(Secure, NumCodePoints);
            PlatformIO_Immutable(UTF16*) Generated16     = UTF16_Encode(GeneratedString);
            PlatformIO_Immutable(UTF32*) Decoded16       = UTF16_Decode(Generated16);
            TestPassed                                   = UTF32_CompareSubString(GeneratedString, Decoded16, 0, 0);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return TestPassed;
    }
    
    void Test_UTF8_Insert(void) {
        PlatformIO_Immutable(UTF8 *) Original = UTF8String("Original");
        PlatformIO_Immutable(UTF8 *) Insertee = UTF8String("Insert");
        PlatformIO_Immutable(UTF8 *) Inserted = UTF8_Insert(Original, Insertee, 0);
        PlatformIO_Immutable(UTF8 *) Correct  = UTF8String("InsertOriginal");
        bool Matches   = UTF8_Compare(Inserted, Correct);
        if (Matches == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Strings do not match"));
        }
    }
    
    bool Test_UTF8_Properties(void) {
        bool      TestPassed     = true;
        PlatformIO_Immutable(UTF8 *) TestString8    = UTF8String("Size: 7");
        PlatformIO_Immutable(UTF32*) TestString32   = UTF8_Decode(TestString8);
        uint64_t  TestStringSize = UTF32_GetStringSizeInCodePoints(TestString32);
        if (TestStringSize != 7) {
            TestPassed           = false;
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String \"%s\" is supposed to be 7 CodePoints long, but is actually %llu"), TestString8, TestStringSize);
        }
        return TestPassed;
    }
    
    bool Test_SubstituteSubString() {
        bool TestPassed = No;
        
        PlatformIO_Immutable(UTF32*) Replacement = UTF32String("987654321");
        PlatformIO_Immutable(UTF32*) String      = UTF32String("123456789");
        UTF32                       *Replaced    = UTF32_SubstituteSubString(String, Replacement, 0, 0);
        
        return TestPassed;
    }
    
    bool Test_UTF8_StitchSubString(void) {
        bool TestPassed = No;
        PlatformIO_Immutable(UTF8*) BananaBread = UTF8_StitchSubString(UTF8String("Banana WAT Bread"), 8, 4);
        TestPassed        = UTF8_Compare(BananaBread, UTF8String("Banana Bread"));
        return TestPassed;
    }
    
    bool Test_UTF8_Reverse(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Clone(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Trim(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Strip(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_String2Decimal(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Decimal2String(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_String2Integer(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Integer2String(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Split(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_CompareSubString(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_FindSubString(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_ExtractSubString(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_RemoveSubString(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
    }
    
    int main(int argc, const char *argv[]) {
        bool TestSuitePassed      = false;
        SecureRNG *Random         = SecureRNG_Init(16 * 1024);
        TestSuitePassed           = Test_UTF16_EncodeDecode(Random);
        return TestSuitePassed;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
