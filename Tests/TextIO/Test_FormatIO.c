#include "../../Library/include/TestIO.h"
#include "../../Library/include/TextIO/FormatIO.h"
#include "../../Library/include/CryptographyIO.h"
#include "../../Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    TestSuite FormatTests;

    bool Test_UTF8_Deformat(void) {
        bool TestPassed      = No;
        ImmutableStringSet_UTF8 DeformatTest1 = (ImmutableStringSet_UTF8) UTF8_Deformat(UTF8String("NumArgs: %1$llu, Equal: %llu, Type: %3$s"), UTF8String("NumArgs: 3, Equal: 1234, Type: Positional"));
        bool  SubString1     = UTF8_Compare(DeformatTest1[0], UTF8String("3"));
        bool  SubString2     = UTF8_Compare(DeformatTest1[1], UTF8String("1234"));
        bool  SubString3     = UTF8_Compare(DeformatTest1[2], UTF8String("Positional"));
        if (SubString1 == No || SubString2 == No || SubString3 == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString doesn't match"));
        } else {
            TestPassed       = Yes;
        }
        return TestPassed;

        TestCase TestCase_Deformat8 = {
            .Function     = Test_UTF8_Deformat,
            .TestState    = TestState_Enabled,
            .TestOutcome  = Outcome_Passed,
        };
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

        bool TestPassed  = false;
        ImmutableString_UTF32 String1   = UTF32String("%U32s");
        ImmutableString_UTF32 String2   = UTF32String("wat");
        ImmutableString_UTF32 TestU32   = UTF32_Format(String1, String2);
        if (UTF32_Compare(TestU32, String2)) {
            printf("%s\n", UTF8String("Test1 Passed"));
        } else {
            printf("%s\n", UTF8String("Test1 Failed"));
        }

        ImmutableString_UTF32 Test1     = UTF32_SubstituteSubString(UTF32String("surr %1$s ound"), UTF32String("3"), 5, 4);
        printf("%ls\n", (wchar_t*) Test1);
        bool  Test1Match = UTF32_Compare(Test1, UTF32String("surr 3 ound"));
        if (Test1Match == No) {
            printf("%s\n", UTF8String("Test1 Failed"));
        } else {
            printf("%s\n", UTF8String("Test1 Passed"));
        }

        ImmutableString_UTF32 Test2 = UTF32_SubstituteSubString(UTF32String("surr %s ound"), UTF32String("12"), 5, 2);
        printf("%ls\n", (wchar_t*) Test2);
        bool  Test2Match = UTF32_Compare(Test2, UTF32String("surr 12 ound"));
        if (Test2Match == No) {
            printf("%s\n", UTF8String("Test2 Failed"));
        } else {
            printf("%s\n", UTF8String("Test2 Passed"));
        }

        ImmutableString_UTF32 Test3 = UTF32_SubstituteSubString(UTF32String("surr %llu ound"), UTF32String("1,234,567"), 5, 4);
        printf("%ls\n", (wchar_t*) Test3);
        bool  Test3Match = UTF32_Compare(Test3, UTF32String("surr 1,234,567 ound"));
        if (Test3Match == No) {
            printf("%s\n", UTF8String("Test3 Failed"));
        } else {
            printf("%s\n", UTF8String("Test3 Passed"));
        }

        UTF8 *DoubleSpecifier                  = UTF8_Format(UTF8String("%d:%d"), 1, 2);
        bool  DoubleSpecifierTest              = UTF8_Compare(DoubleSpecifier, UTF8String("1:2"));
        if (DoubleSpecifierTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("DoubleSpecifierTest Failed"));
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("DoubleSpecifierTest Passed"));
        }

        // Test UTF32_SubstituteSubString


        // Duplicate Specifiers, something where repition would be helpful

        ImmutableString_UTF8 HLBParent1           = UTF8String("Popa");
        ImmutableString_UTF8 HLBParent2           = UTF8String("Momma");
        ImmutableString_UTF8 HLBRhyme1Ending      = UTF8String("ing");
        ImmutableString_UTF8 HLBRhyme2Ending      = UTF8String("ass");

        ImmutableString_UTF8 HushLittleBabyResult1 = UTF8String("Hush little baby don't say a word, Popa's gonna buy you a Mockingbird; And if that Mockingbird don't sing, Popa's gonna buy you a Diamond ring; and if that Diamond ring turns brass, Popa's gonna buy you a looking glass");
        ImmutableString_UTF8 HushLittleBabyResult2 = UTF8String("Hush little baby don't say a word, Momma's gonna buy you a Mockingbird; And if that Mockingbird don't sing, Momma's gonna buy you a Diamond ring; and if that Diamond ring turns brass, Momma's gonna buy you a looking glass");

       ImmutableString_UTF8 DuplicatePosition2Dad            = UTF8_Format(UTF8String("Hush little baby don't say a word, %1$s's gonna buy you a Mockingbird; And if that Mockingbird don't s%2$s, %1$s's gonna buy you a Diamond r%2$s; and if that Diamond r%2$s turns br%3$s, %1$s's gonna buy you a looking gl%3$s"), HLBParent1, HLBRhyme1Ending, HLBRhyme2Ending); // %1$ = 3, %2$ = 3, %3$ = 2, Dupes = 5
        bool  DuplicatePosition2Test1          = UTF8_Compare(DuplicatePosition2Dad, HushLittleBabyResult1);
        if (DuplicatePosition2Test1 == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Rhyme Positional Test Failed"));
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Rhyme Positional Test Passed"));
        }

        ImmutableString_UTF8 DuplicatePosition2Mom            = UTF8_Format(UTF8String("Hush little baby don't say a word, %1$s's gonna buy you a Mockingbird; And if that Mockingbird don't s%2$s, %1$s's gonna buy you a Diamond r%2$s; and if that Diamond r%2$s turns br%3$s, %1$s's gonna buy you a looking gl%3$s"), HLBParent2, HLBRhyme1Ending, HLBRhyme2Ending);
        bool  DuplicatePosition2Test2          = UTF8_Compare(DuplicatePosition2Mom, HushLittleBabyResult2);
        if (DuplicatePosition2Test2 == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Rhyme Positional Test Failed"));
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Rhyme Positional Test Passed"));
        }


        UTF8 *DuplicatePosition                = UTF8_Format(UTF8String("%2$s %2$s in the %1$s %1$s!!"), UTF8String("butt"), UTF8String("wat"));
        bool  DuplicatePositionTest            = UTF8_Compare(DuplicatePosition, UTF8String("wat wat in the butt butt"));
        if (DuplicatePositionTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OctalTest Failed"));
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OctalTest Passed"));
        }

        uint8_t OctalValue                     = 014; // 12, 0xC
        UTF8   *Octal                          = UTF8_Format(UTF8String("%03o"), OctalValue);
        bool    OctalTest                      = UTF8_Compare(Octal, UTF8String("014"));
        if (OctalTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OctalTest Failed"));
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OctalTest Passed"));
        }

        UTF8 *Positional1                      = UTF8_Format(UTF8String("NumArgs: %1$u, Equal: %2$u, Type: %3$s"), 3, 1234, UTF8String("Positional"));
        bool  Positional1Test                  = UTF8_Compare(Positional1, UTF8String("NumArgs: 3, Equal: 1234, Type: Positional"));
        // "NumArgs: 3, Equal: 1234, Type: Positional"
        if (Positional1Test == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Positional1Test Failed"));
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Positional1Test Passed"));
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
         Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Positional2Test Failed"));
         }
         */

        UTF8 *Positional3                      = UTF8_Format(UTF8String("NumArgs: %2$d, %1$s EXTEND THE STRING"), UTF8String("Positional"), 2); // Remapping isn't working.
        bool  Positional3Test                  = UTF8_Compare(Positional3, UTF8String("NumArgs: 2, Positional EXTEND THE STRING"));
        // "NumArgs: -$llu, Positi"
        // "NumArgs: Positional, %12XTEND THE STRING"
        if (Positional3Test == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Positional3Test Failed"));
        }

        UTF8 *PositionPrecision                = UTF8_Format(UTF8String("%.*f"), 2, 0.33333333);
        bool PositionPrecisionTest             = UTF8_Compare(PositionPrecision, UTF8String("0.33"));
        if (PositionPrecisionTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PositionPrecisionTest Failed"));
        }

        UTF8 *StringPrecision                  = UTF8_Format(UTF8String("%.3s"), UTF8String("FooBar"));
        bool StringPrecisionTest               = UTF8_Compare(StringPrecision, UTF8String("Foo"));
        if (StringPrecisionTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringPreicisonTest Failed"));
        }

        UTF8 *Padding                          = UTF8_Format(UTF8String("%10.5i"), 9);
        bool  PaddingTest                      = UTF8_Compare(Padding, UTF8String("     00009"));
        if (PaddingTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PaddingTest Failed"));
        }

        UTF8 *SpaceAlign                       = UTF8_Format(UTF8String("% d"), 4);
        bool SpaceAlignTest                    = UTF8_Compare(SpaceAlign, UTF8String(" 4"));
        if (SpaceAlignTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SpaceAlignTest Failed"));
        }

        UTF8 *ZeroLeftAlign                    = UTF8_Format(UTF8String("%05d"), 4);
        bool ZeroLeftAlignTest                 = UTF8_Compare(ZeroLeftAlign, UTF8String("00004"));
        if (ZeroLeftAlignTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ZeroLeftAlignTest Failed"));
        }

        UTF8 *Percent                          = UTF8_Format(UTF8String("%%"));
        bool PercentTest                       = UTF8_Compare(Percent, UTF8String("%"));
        if (PercentTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PercentTest Failed"));
        }

        UTF8 *Smaller                          = UTF8_Format(UTF8String("%d"), 123);
        bool SmallerTest                       = UTF8_Compare(Smaller, UTF8String("123"));
        if (SmallerTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SmallerTest Failed"));
        }

        UTF8 *Equal                            = UTF8_Format(UTF8String("%d"), 1094);
        bool EqualTest                         = UTF8_Compare(Equal, UTF8String("1094"));
        if (EqualTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("EqualTest Failed"));
        }

        // Equal and Smaller fail, but Longer works, and Smaller works sometimes?
        UTF8 *Longer                           = UTF8_Format(UTF8String("%d"), 99999);
        bool LongerTest                        = UTF8_Compare(Longer, UTF8String("99999"));
        if (LongerTest == No) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("LongerTest Failed"));
        }
        return TestPassed;

        TestCase TestCase_Format8 = {
            .Function     = Test_UTF8_Format,
            .TestState    = TestState_Enabled,
            .TestOutcome  = Outcome_Passed, // TestOutcome?
        };
    }

    int main(const int argc, const char *argv[]) {
        bool TestSuitePassed      = false;
        TestSuitePassed           = Test_UTF8_Format();
        return TestSuitePassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
