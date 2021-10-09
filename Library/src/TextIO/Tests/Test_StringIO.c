#include "../Library/include/TestIO.h"
#include "../Library/include/TextIO/StringIO.h"
#include "../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    bool Test_StringSet(SecureRNG *Secure) {
        bool TestPassed = Yes;
        ImmutableStringSet_UTF8 StringSet = UTF8StringSet(UTF8String("String1"), UTF8String("String2"), UTF8String("String3"));
        uint64_t NumStrings = UTF8_StringSet_GetNumStrings(StringSet); // 14?!
        if (NumStrings != 3) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumStrings is %llu but should be 3"), NumStrings);
            TestPassed = No;
        }
        uint64_t *StringSizes = UTF8_StringSet_GetStringSizesInCodeUnits(StringSet);
        if (StringSizes[0] != 7 || StringSizes[1] != 7 || StringSizes[2] != 7) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Size of StringSet's Strings doesn't = 7"));
        }
        /* So far so good, now lets test Allocation, Assignment, and Deinitalization */
        ImmutableStringSet_UTF8 StringSet2 = UTF8StringSet(UTF8String("String1"), UTF8String("String2"), UTF8String("String3"));
        
        if (StringSet[0] != StringSet2[0]) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Addresses don't match"));
        }
        
        return TestPassed;
    }

    bool Test_UTF8_Graphemes(SecureRNG *Secure) {
        bool TestPassed = No;
        UTF8 *Grapheme1 = UTF8String("🇺🇸");
        uint8_t Grapheme1Size = UTF8_GetStringSizeInGraphemes(Grapheme1);
        if (Grapheme1Size == 1) {
            TestPassed = Yes;
        }
        return TestPassed;
    }
    
    bool Test_UTF8_EncodeDecode(SecureRNG *Secure) {
        bool TestPassed                = No;
        if (Secure != NULL) {
            uint64_t  NumCodePoints    = SecureRNG_GenerateInteger(Secure, 16);
            ImmutableString_UTF32 GeneratedString  = UTF32_GenerateString(Secure, NumCodePoints);
            UTF8     *Generated8       = UTF8_Encode(GeneratedString);
            ImmutableString_UTF32 Decoded8         = UTF8_Decode(Generated8);
            TestPassed                 = UTF32_Compare(GeneratedString, Decoded8);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return TestPassed;
    }

    TestCase UTF8_EncodeDecode_Test = {
        .State       = TestState_Enabled,
        .Function    = Test_UTF8_EncodeDecode,
        .Expectation = Outcome_Passed,
    };
    
    bool Test_UTF16_EncodeDecode(SecureRNG *Secure) {
        bool TestPassed = Yes;
        if (Secure != NULL) {
            uint64_t  NumCodePoints                      = SecureRNG_GenerateInteger(Secure, 16);
            ImmutableString_UTF32 GeneratedString = UTF32_GenerateString(Secure, NumCodePoints);
            ImmutableString_UTF16 Generated16     = UTF16_Encode(GeneratedString);
            ImmutableString_UTF32 Decoded16       = UTF16_Decode(Generated16);
            TestPassed                                   = UTF32_CompareSubString(GeneratedString, Decoded16, 0, 0);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return TestPassed;
    }
    
    bool Test_UTF8_Insert(SecureRNG *Secure) {
        bool TestPassed = No;
        ImmutableString_UTF8 Original = UTF8String("Original");
        ImmutableString_UTF8 Insertee = UTF8String("Insert");
        ImmutableString_UTF8 Inserted = UTF8_Insert(Original, Insertee, 0);
        ImmutableString_UTF8 Correct  = UTF8String("InsertOriginal");
        TestPassed                            = UTF8_Compare(Inserted, Correct);
        return TestPassed;
    }
    
    bool Test_UTF8_Properties(SecureRNG *Secure) {
        bool      TestPassed     = Yes;
        ImmutableString_UTF8 TestString8    = UTF8String("Size: 7");
        ImmutableString_UTF32 TestString32   = UTF8_Decode(TestString8);
        uint64_t  TestStringSize = UTF32_GetStringSizeInCodePoints(TestString32);
        if (TestStringSize != 7) {
            TestPassed           = No;
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String \"%s\" is supposed to be 7 CodePoints long, but is actually %llu"), TestString8, TestStringSize);
        }
        return TestPassed;
    }
    
    bool Test_SubstituteSubString(SecureRNG *Secure) {
        bool TestPassed = No;
        
        ImmutableString_UTF32 Replacement = UTF32String("987654321");
        ImmutableString_UTF32 String      = UTF32String("123456789");
        UTF32                       *Replaced    = UTF32_SubstituteSubString(String, Replacement, 0, 0);
        
        return TestPassed;
    }
    
    bool Test_UTF8_StitchSubString(SecureRNG *Secure) {
        bool TestPassed                         = No;
        ImmutableString_UTF8 BananaBread = UTF8_StitchSubString(UTF8String("Banana NUT Bread"), 8, 4);
        TestPassed                              = UTF8_Compare(BananaBread, UTF8String("Banana Bread"));
        return TestPassed;
    }

    bool Test_UTF8_StringSet(SecureRNG *Secure) {
        bool TestPassed                    = No;
        UTF8 **StringSet8                  = UTF8_StringSet_Init(4);
        if (StringSet8[0] == (UTF8*) 0x8888888888888888) {
            TestPassed = Yes;
        }
        ImmutableString_UTF8 One   = UTF8String("One");
        ImmutableString_UTF8 Two   = UTF8String("Two");
        ImmutableString_UTF8 Three = UTF8String("Three");
        ImmutableString_UTF8 Four  = UTF8String("Four");
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
        TestSuitePassed           = Test_StringSet(Random);
        TestSuitePassed           = Test_UTF8_Graphemes(Random);
        //TestSuitePassed           = Test_UTF8_StringSet(Random);
        //TestSuitePassed           = Test_UTF16_EncodeDecode(Random);
        return TestSuitePassed;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
