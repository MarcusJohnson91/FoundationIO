#include "../../../include/TextIO/StringIO.h"   /* Included for our declarations */

#include "../../../include/AssertIO.h"          /* Included for Assertions */
#include "../../../include/TestIO.h"            /* Included for testing */
#include "../../../include/CryptographyIO.h"    /* Included for testing */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    bool Test_StringSet(InsecurePRNG *Insecure) {
        bool TestPassed = Yes;
        ImmutableStringSet_UTF8 StringSet = UTF8StringSet(UTF8String("String1"), UTF8String("String2"), UTF8String("String3"));
        uint64_t NumStrings = UTF8_StringSet_GetNumStrings(StringSet); // 14?!
        AssertIO(NumStrings == 3);
        uint64_t *StringSizes = UTF8_StringSet_GetStringSizesInCodeUnits(StringSet);
        AssertIO(StringSizes[0] == 7);
        AssertIO(StringSizes[1] == 7);
        AssertIO(StringSizes[2] == 7);
        /* So far so good, now lets test Allocation, Assignment, and Deinitalization */
        ImmutableStringSet_UTF8 StringSet2 = UTF8StringSet(UTF8String("String1"), UTF8String("String2"), UTF8String("String3"));

        AssertIO(StringSet[0] == StringSet2[0]);
        
        return TestPassed;
    }

    bool Test_UTF8_Graphemes(InsecurePRNG *Insecure) {
        bool TestPassed = No;
        UTF8 *Grapheme1 = UTF8String("🇺🇸");
        UTF8 *Grapheme2 = UTF8String("NumGraphemes = 16");
        uint8_t Grapheme1Size = UTF8_GetStringSizeInGraphemes(Grapheme1);
        uint8_t Grapheme2Size = UTF8_GetStringSizeInGraphemes(Grapheme2);
        AssertIO(Grapheme1Size == 1);
        AssertIO(Grapheme2Size == 16);
        return TestPassed;
    }
    
    bool Test_UTF8_EncodeDecode(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestPassed                = No;
        uint64_t  NumCodePoints    = InsecurePRNG_CreateInteger(Insecure, 16);
        ImmutableString_UTF32 GeneratedString  = UTF32_GenerateString(Insecure, NumCodePoints);
        UTF8     *Generated8       = UTF8_Encode(GeneratedString);
        ImmutableString_UTF32 Decoded8         = UTF8_Decode(Generated8);
        TestPassed                 = UTF32_Compare(GeneratedString, Decoded8);
        return TestPassed;
    }

    TestCase UTF8_EncodeDecode_Test = {
        .State       = TestState_Enabled,
        .Function    = Test_UTF8_EncodeDecode,
        .Expectation = Outcome_Passed,
    };
    
    bool Test_UTF16_EncodeDecode(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestPassed = Yes;
        uint64_t  NumCodePoints               = InsecurePRNG_CreateInteger(Insecure, 16);
        ImmutableString_UTF32 GeneratedString = UTF32_GenerateString(Insecure, NumCodePoints);
        ImmutableString_UTF16 Generated16     = UTF16_Encode(GeneratedString);
        ImmutableString_UTF32 Decoded16       = UTF16_Decode(Generated16);
        TestPassed                            = UTF32_Compare(GeneratedString, Decoded16);
        UTF32_Deinit(GeneratedString);
        UTF16_Deinit(Generated16);
        UTF32_Deinit(Decoded16);
        return TestPassed;
    }
    
    bool Test_UTF8_Insert(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestPassed = No;
        ImmutableString_UTF8 Original = UTF8String("Original");
        ImmutableString_UTF8 Insertee = UTF8String("Insert");
        ImmutableString_UTF8 Inserted = UTF8_Insert(Original, Insertee, 0);
        ImmutableString_UTF8 Correct  = UTF8String("InsertOriginal");
        TestPassed                            = UTF8_Compare(Inserted, Correct);
        return TestPassed;
    }
    
    bool Test_UTF8_Properties(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool      TestPassed     = Yes;
        ImmutableString_UTF8 TestString8    = UTF8String("Size: 7");
        ImmutableString_UTF32 TestString32   = UTF8_Decode(TestString8);
        uint64_t  TestStringSize = UTF32_GetStringSizeInCodePoints(TestString32);
        AssertIO(TestStringSize == 7);
        return TestPassed;
    }
    
    bool Test_SubstituteSubString(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestPassed = No;
        
        ImmutableString_UTF32 Replacement = UTF32String("987654321");
        ImmutableString_UTF32 String      = UTF32String("123456789");
        UTF32                       *Replaced    = UTF32_SubstituteSubString(String, Replacement, 0, 0);
        
        return TestPassed;
    }
    
    bool Test_UTF8_StitchSubString(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestPassed                         = No;
        ImmutableString_UTF8 BananaBread        = UTF8_StitchSubString(UTF8String("Banana NUT Bread"), 8, 4);
        TestPassed                              = UTF8_Compare(BananaBread, UTF8String("Banana Bread"));
        return TestPassed;
    }

    bool Test_UTF8_StringSet(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestPassed                    = No;
        ImmutableStringSet_UTF8 StringSet  = UTF8StringSet(UTF8String("One"), UTF8String("Two"), UTF8String("Three"), UTF8String("Four"));
        return TestPassed;
    }
    
    bool Test_UTF8_Reverse(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Clone(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Trim(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Strip(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_String2Decimal(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Decimal2String(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_String2Integer(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Integer2String(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_Split(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_CompareSubString(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_FindSubString(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_ExtractSubString(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    bool Test_UTF8_RemoveSubString(InsecurePRNG *Insecure) {
        bool TestSuitePassed = No;
        return TestSuitePassed;
    }
    
    int main(const int argc, const char *argv[]) {
        bool TestSuitePassed      = No;
        InsecurePRNG *Insecure    = InsecurePRNG_Init(0);
        //TestSuitePassed           = Test_StringSet(Insecure);
        TestSuitePassed           = Test_UTF8_Graphemes(Insecure);
        //TestSuitePassed           = Test_UTF8_StringSet(Insecure);
        //TestSuitePassed           = Test_UTF16_EncodeDecode(Insecure);
        return TestSuitePassed;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
