#include "../../Library/include/TestIO.h"
#include "../../Library/include/TextIO/StringIO.h"
#include "../../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    bool Test_UTF8_EncodeDecode(SecureRNG *Secure) {
        bool TestPassed                = No;
        if (Secure != NULL) {
            uint64_t  NumCodePoints    = SecureRNG_GenerateInteger(Secure, 16);
            PlatformIO_Immutable(UTF32*) GeneratedString  = UTF32_GenerateString(Secure, NumCodePoints);
            UTF8     *Generated8       = UTF8_Encode(GeneratedString);
            PlatformIO_Immutable(UTF32*) Decoded8         = UTF8_Decode(Generated8);
            TestPassed                 = UTF32_Compare(GeneratedString, Decoded8);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return TestPassed;
    }

    TestCase UTF8_EncodeDecode_Test = {
        .TestState      = TestState_Enabled,
        .Function       = Test_UTF8_EncodeDecode,
        .ExpectedResult = ExpectedResult_Passed,
    };
    
    bool Test_UTF16_EncodeDecode(SecureRNG *Secure) {
        bool TestPassed = Yes;
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
    
    bool Test_UTF8_Insert(SecureRNG *Secure) {
        bool TestPassed = No;
        PlatformIO_Immutable(UTF8 *) Original = UTF8String("Original");
        PlatformIO_Immutable(UTF8 *) Insertee = UTF8String("Insert");
        PlatformIO_Immutable(UTF8 *) Inserted = UTF8_Insert(Original, Insertee, 0);
        PlatformIO_Immutable(UTF8 *) Correct  = UTF8String("InsertOriginal");
        TestPassed                            = UTF8_Compare(Inserted, Correct);
        return TestPassed;
    }
    
    bool Test_UTF8_Properties(SecureRNG *Secure) {
        bool      TestPassed     = Yes;
        PlatformIO_Immutable(UTF8 *) TestString8    = UTF8String("Size: 7");
        PlatformIO_Immutable(UTF32*) TestString32   = UTF8_Decode(TestString8);
        uint64_t  TestStringSize = UTF32_GetStringSizeInCodePoints(TestString32);
        if (TestStringSize != 7) {
            TestPassed           = No;
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String \"%s\" is supposed to be 7 CodePoints long, but is actually %llu"), TestString8, TestStringSize);
        }
        return TestPassed;
    }
    
    bool Test_SubstituteSubString(SecureRNG *Secure) {
        bool TestPassed = No;
        
        PlatformIO_Immutable(UTF32*) Replacement = UTF32String("987654321");
        PlatformIO_Immutable(UTF32*) String      = UTF32String("123456789");
        UTF32                       *Replaced    = UTF32_SubstituteSubString(String, Replacement, 0, 0);
        
        return TestPassed;
    }
    
    bool Test_UTF8_StitchSubString(SecureRNG *Secure) {
        bool TestPassed                         = No;
        PlatformIO_Immutable(UTF8*) BananaBread = UTF8_StitchSubString(UTF8String("Banana NUT Bread"), 8, 4);
        TestPassed                              = UTF8_Compare(BananaBread, UTF8String("Banana Bread"));
        return TestPassed;
    }

    bool Test_UTF8_StringSet(SecureRNG *Secure) {
        bool TestPassed                    = No;
        UTF8 **StringSet8                  = UTF8_StringSet_Init(4);
        if (StringSet8[0] == (UTF8*) 0x8888888888888888) {
            TestPassed = Yes;
        }
        PlatformIO_Immutable(UTF8 *) One   = UTF8String("One");
        PlatformIO_Immutable(UTF8 *) Two   = UTF8String("Two");
        PlatformIO_Immutable(UTF8 *) Three = UTF8String("Three");
        PlatformIO_Immutable(UTF8 *) Four  = UTF8String("Four");
        StringSet8[0]                      = One;
        StringSet8[1]                      = Two;
        StringSet8[2]                      = Three;
        StringSet8[3]                      = &Four;
        bool Bool1                         = UTF8_Compare(StringSet8[0], One);
        bool Bool2                         = UTF8_Compare(StringSet8[1], Two);
        bool Bool3                         = UTF8_Compare(StringSet8[2], Three);
        bool Bool4                         = UTF8_Compare(StringSet8[3], Four);
        return (Bool1 + Bool2 + Bool3 + Bool4) / 4;
    }
    
    bool Test_UTF8_Reverse(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Clone(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Trim(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Strip(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_String2Decimal(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Decimal2String(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_String2Integer(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Integer2String(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Split(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_CompareSubString(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_FindSubString(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_ExtractSubString(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_RemoveSubString(SecureRNG *Secure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    int main(const int argc, const char *argv[]) {
        bool TestSuitePassed      = No;
        SecureRNG *Random         = SecureRNG_Init(16 * 1024);
        TestSuitePassed           = Test_UTF8_StringSet(Random);
        //TestSuitePassed           = Test_UTF16_EncodeDecode(Random);
        return TestSuitePassed;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
