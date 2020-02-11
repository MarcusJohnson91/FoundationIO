#include "../Library/include/CryptographyIO.h"
#include "../Library/include/TestIO.h"
#include "../Library/include/UnicodeIO/FormatIO.h"
#include "../Library/include/UnicodeIO/LogIO.h"
#include "../Library/include/UnicodeIO/StringIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Colorize Text API:
     
     Colorize(Color, String)
     
     Where Color is an enum contining a list of possible colors.
     
     Let's put it in CommandLineIO
     */
    
    void Test_UTF8_EncodeDecode(Entropy *Random) {
        if (Random != NULL) {
            uint64_t  NumCodePoints    = Entropy_GenerateInteger(Random, 16);
            UTF32    *GeneratedString  = UTF32_GenerateString(Random, NumCodePoints);
            UTF8     *Generated8       = UTF8_Encode(GeneratedString);
            UTF32    *Decoded8         = UTF8_Decode(Generated8);
            bool      StringsMatch     = UTF32_Compare(GeneratedString, Decoded8);
            if (StringsMatch == No) {
                Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Strings DO NOT match!"));
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        }
    }
    
    void Test_UTF16_EncodeDecode(Entropy *Random) {
        if (Random != NULL) {
            uint64_t  NumCodePoints   = Entropy_GenerateInteger(Random, 16);
            UTF32    *GeneratedString = UTF32_GenerateString(Random, NumCodePoints);
            UTF16    *Generated16     = UTF16_Encode(GeneratedString);
            UTF32    *Decoded16       = UTF16_Decode(Generated16);
            bool      StringsMatch    = UTF32_CompareSubString(GeneratedString, Decoded16, 0, 0);
            if (StringsMatch == No) {
                Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Strings DO NOT match!"));
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        }
    }
    
    void Test_UTF8_Insert(void) {
        UTF8 *Original = UTF8String("Original");
        UTF8 *Insertee = UTF8String("Insert");
        UTF8 *Inserted = UTF8_Insert(Original, Insertee, 0);
        UTF8 *Correct  = UTF8String("InsertOriginal");
        bool Matches   = UTF8_Compare(Inserted, Correct);
        if (Matches == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Strings do not match"));
        }
    }
    
    bool Test_UTF8_Properties(void) {
        bool      TestPassed     = true;
        UTF8     *TestString8    = UTF8String("Size: 7");
        UTF32    *TestString32   = UTF8_Decode(TestString8);
        uint64_t  TestStringSize = UTF32_GetStringSizeInCodePoints(TestString32);
        if (TestStringSize != 7) {
            TestPassed           = false;
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String \"%s\" is supposed to be 7 CodePoints long, but is actually %llu"), TestString8, TestStringSize);
        }
        return TestPassed;
    }
    
    bool Test_SubstituteSubString() {
        bool TestPassed = No;
        
        UTF32 *Replacement = UTF32String("987654321");
        UTF32 *String      = UTF32String("123456789");
        UTF32 *Replaced    = UTF32_SubstituteSubString(String, Replacement, 0, 0);
        
        return TestPassed;
    }
    
    bool Test_UTF8_StitchSubString(void) {
        bool TestPassed = No;
        UTF8 *BananaBread = UTF8_StitchSubString(UTF8String("Banana WAT Bread"), 8, 4);
        TestPassed        = UTF8_Compare(BananaBread, UTF8String("Banana Bread"));
        return TestPassed;
    }
    
    bool Test_Deformat(void) {
        bool TestPassed      = No;
        UTF8 **DeformatTest1 = UTF8_Deformat(UTF8String("NumArgs: %1$llu, Equal: %llu, Type: %3$s"), UTF8String("NumArgs: 3, Equal: 1234, Type: Positional"));
        UTF8_StringArray_Print(DeformatTest1);
        bool  SubString1     = UTF8_Compare(DeformatTest1[0], UTF8String("3"));
        bool  SubString2     = UTF8_Compare(DeformatTest1[1], UTF8String("1234"));
        bool  SubString3     = UTF8_Compare(DeformatTest1[2], UTF8String("Positional"));
        if (SubString1 == No || SubString2 == No || SubString3 == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("SubString doesn't match"));
        } else {
            TestPassed       = Yes;
        }
        return TestPassed;
    }
    
    bool Test_UTF8_Format(void) {
        /*
         TestIO Notes:
         
         It'd be useful if there was a way to store the number of test successes vs test failures
         
         
         Another interesting thing to note:
         
         We're gonna have to have a test function for each function in StringIO.
         
         So, all the testing functions will start with Test_XXXXXXXXXXXXXXXX
         
         TestIO should also contain all of the Generate functions that, for example CodePoints, Strings, etc.
         */
        
        bool TestPassed                        = false;
        
        uint8_t OctalValue                     = 014; // 12, 0xC
        UTF8   *Octal                          = UTF8_Format(UTF8String("%03o"), OctalValue);
        bool    OctalTest                      = UTF8_Compare(Octal, UTF8String("014"));
        if (OctalTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("OctalTest Failed"));
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("OctalTest Passed"));
        }
        
        UTF8 *Positional1                      = UTF8_Format(UTF8String("NumArgs: %1$u, Equal: %2$u, Type: %3$s"), 3, 1234, UTF8String("Positional"));
        bool  Positional1Test                  = UTF8_Compare(Positional1, UTF8String("NumArgs: 3, Equal: 1234, Type: Positional"));
        // "NumArgs: 3, Equal: 1234, Type: Positional"
        if (Positional1Test == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Positional1Test Failed"));
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Positional1Test Passed"));
        }
        /*
         "NumArgs: %1$llu, Equal: %llu, Type: %3$s"  StringSize: 40
         "NumArgs: 3, Equal: 1234, Type: Positional" StringSize: 41
         "NumArgs: 3, Equal: 1234, Type: "
         "NumArgs: 3, Equal: 1234, Type: %3Position" Offset is 2 later than it should be (negative instead of positive?)
         "NumArgs: 3, Equal: 1234, Type: Positional"
         
         Let's operate under the theory that the last character being 1 is from 1234 for the second argument which is just way off from the offset.
         
         and with that theory, "1" actually equals "1234" is just stopped before it was done.
         
         it's 11 or 12 CodePoint farther than it should be.
         
         The offset for the first argument is correct.
         
         Firt off, just add Length, not (Length + 1) cuz it's aready moved to the following codepoint so no need to bump it up
         */
        
        /*
         UTF8 *Positional2                      = UTF8_Format(UTF8String("%2$s: NumArgs: %1$llu, Type: %2$s"), 2, UTF8String("Positional"));
         bool  Positional2Test                  = UTF8_Compare(Positional2, UTF8String("Positional: NumArgs: 2, Type: Positional"));
         if (Positional2Test == No) {
         Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Positional2Test Failed"));
         }
         */
        
        UTF8 *Positional3                      = UTF8_Format(UTF8String("NumArgs: %2$d, %1$s EXTEND THE STRING"), UTF8String("Positional"), 2); // Remapping isn't working.
        bool  Positional3Test                  = UTF8_Compare(Positional3, UTF8String("NumArgs: 2, Positional EXTEND THE STRING"));
        // "NumArgs: -$llu, Positi"
        // "NumArgs: Positional, %12XTEND THE STRING"
        if (Positional3Test == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Positional3Test Failed"));
        }
        
        UTF8 *PositionPrecision                = UTF8_Format(UTF8String("%.*f"), 2, 0.33333333);
        bool PositionPrecisionTest             = UTF8_Compare(PositionPrecision, UTF8String("0.33"));
        if (PositionPrecisionTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("PositionPrecisionTest Failed"));
        }
        
        UTF8 *StringPrecision                  = UTF8_Format(UTF8String("%.3s"), UTF8String("FooBar"));
        bool StringPrecisionTest               = UTF8_Compare(StringPrecision, UTF8String("Foo"));
        if (StringPrecisionTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("StringPreicisonTest Failed"));
        }
        
        UTF8 *Padding                          = UTF8_Format(UTF8String("%10.5i"), 9);
        bool  PaddingTest                      = UTF8_Compare(Padding, UTF8String("     00009"));
        if (PaddingTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("PaddingTest Failed"));
        }
        
        UTF8 *SpaceAlign                       = UTF8_Format(UTF8String("% d"), 4);
        bool SpaceAlignTest                    = UTF8_Compare(SpaceAlign, UTF8String(" 4"));
        if (SpaceAlignTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("SpaceAlignTest Failed"));
        }
        
        UTF8 *ZeroLeftAlign                    = UTF8_Format(UTF8String("%05d"), 4);
        bool ZeroLeftAlignTest                 = UTF8_Compare(ZeroLeftAlign, UTF8String("00004"));
        if (ZeroLeftAlignTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("ZeroLeftAlignTest Failed"));
        }
        
        UTF8 *Percent                          = UTF8_Format(UTF8String("%%"));
        bool PercentTest                       = UTF8_Compare(Percent, UTF8String("%"));
        if (PercentTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("PercentTest Failed"));
        }
        
        UTF8 *Smaller                          = UTF8_Format(UTF8String("%d"), 123);
        bool SmallerTest                       = UTF8_Compare(Smaller, UTF8String("123"));
        if (SmallerTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("SmallerTest Failed"));
        }
        
        UTF8 *Equal                            = UTF8_Format(UTF8String("%d"), 1094);
        bool EqualTest                         = UTF8_Compare(Equal, UTF8String("1094"));
        if (EqualTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("EqualTest Failed"));
        }
        
        // Equal and Smaller fail, but Longer works, and Smaller works sometimes?
        UTF8 *Longer                           = UTF8_Format(UTF8String("%d"), 99999);
        bool LongerTest                        = UTF8_Compare(Longer, UTF8String("99999"));
        if (LongerTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("LongerTest Failed"));
        }
        
        UTF8 *DoubleSpecifier                  = UTF8_Format(UTF8String("%d:%d"), 1, 2);
        bool  DoubleSpecifierTest              = UTF8_Compare(DoubleSpecifier, UTF8String("1:2"));
        if (DoubleSpecifierTest == No) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("DoubleSpecifierTest Failed"));
        }
        return TestPassed;
    }
    
    bool Test_UTF8_Deformat(void) {
        bool TestSuitePassed = false;
        return TestSuitePassed;
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
        //Entropy *Random           = Entropy_Init(8000000);
        TestSuitePassed           = Test_UTF8_Format();
        //Test_UTF8_EncodeDecode(Random);
        //TestSuitePassed           = Test_UTF8_Format();
        //TestSuitePassed           = Test_UTF8_StitchSubString();
        return TestSuitePassed;
    }
    
#ifdef __cplusplus
}
#endif
