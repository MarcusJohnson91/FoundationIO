#include "../../../include/TextIO/FormatIO.h"   /* Included for our declarations */

#include "../../../include/AssertIO.h"          /* Included for Assertions */
#include "../../../include/TextIO/StringIO.h"   /* Included for StringIO */
#include "../../../include/TestIO.h"            /* Included for testing */
#include "../../../include/CryptographyIO.h"    /* Included for CryptographyIO */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    TestSuite FormatTests;

    bool Test_UTF8_Deformat(InsecurePRNG *Insecure) {
        bool TestPassed      = No;
        UTF8 **DeformatTest1 = UTF8_Deformat(UTF8String("NumArgs: %1$zu, Equal: %zu, Type: %3$s"), UTF8String("NumArgs: 3, Equal: 1234, Type: Positional"));
        bool  SubString1     = UTF8_Compare(DeformatTest1[0], UTF8String("3"));
        bool  SubString2     = UTF8_Compare(DeformatTest1[1], UTF8String("1234"));
        bool  SubString3     = UTF8_Compare(DeformatTest1[2], UTF8String("Positional"));

        AssertIO(SubString1 == true);
        AssertIO(SubString2 == true);
        AssertIO(SubString3 == true);
        return TestPassed;

        TestCase TestCase_Deformat8 = {
            .Function       = Test_UTF8_Deformat,
            .State      = TestState_Enabled,
            .Expectation = Outcome_Passed,
        };
    }



    bool Test_UTF8_Format(InsecurePRNG *Insecure) {
        /*
         TestIO Notes:

         It'd be useful if there was a way to store the number of test successes vs test failures


         Another interesting thing to note:

         We're gonna have to have a test function for each function in StringIO.

         So, all the testing functions will start with Test_XXXXXXXXXXXXXXXX

         TestIO should also contain all of the Generate functions that, for example CodePoints, Strings, etc.
         */

        bool TestPassed  = false;
        ImmutableString_UTF32 String32_1   = UTF32String("%U32s");
        ImmutableString_UTF32 String32_2   = UTF32String("wat");
        ImmutableString_UTF32 TestU32      = UTF32_Format(String32_1, String32_2);
        if (UTF32_Compare(TestU32, String32_2)) {
            printf("%s\n", UTF8String("Test1 Passed"));
        } else {
            printf("%s\n", UTF8String("Test1 Failed"));
        }
      
        ImmutableString_UTF16 String16_1   = UTF16String("%U16s");
        ImmutableString_UTF16 String16_2   = UTF16String("wat");
        ImmutableString_UTF16 TestU16      = UTF16_Format(String16_1, String16_2);
        if (UTF16_Compare(TestU16, String16_2)) {
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

        ImmutableString_UTF32 Test3 = UTF32_SubstituteSubString(UTF32String("surr %zu ound"), UTF32String("1,234,567"), 5, 4);
        printf("%ls\n", (wchar_t*) Test3);
        bool  Test3Match = UTF32_Compare(Test3, UTF32String("surr 1,234,567 ound"));
        if (Test3Match == No) {
            printf("%s\n", UTF8String("Test3 Failed"));
        } else {
            printf("%s\n", UTF8String("Test3 Passed"));
        }

        UTF8 *DoubleSpecifier                  = UTF8_Format(UTF8String("%d:%d"), 1, 2);
        bool  DoubleSpecifierTest              = UTF8_Compare(DoubleSpecifier, UTF8String("1:2"));
        AssertIO(DoubleSpecifierTest == true);

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
        AssertIO(DuplicatePosition2Test1 == true);

        ImmutableString_UTF8 DuplicatePosition2Mom            = UTF8_Format(UTF8String("Hush little baby don't say a word, %1$s's gonna buy you a Mockingbird; And if that Mockingbird don't s%2$s, %1$s's gonna buy you a Diamond r%2$s; and if that Diamond r%2$s turns br%3$s, %1$s's gonna buy you a looking gl%3$s"), HLBParent2, HLBRhyme1Ending, HLBRhyme2Ending);
        bool  DuplicatePosition2Test2          = UTF8_Compare(DuplicatePosition2Mom, HushLittleBabyResult2);
        AssertIO(DuplicatePosition2Test2 == true);

        UTF8 *DuplicatePosition                = UTF8_Format(UTF8String("%2$s %2$s in the %1$s %1$s!!"), UTF8String("butt"), UTF8String("wat"));
        bool  DuplicatePositionTest            = UTF8_Compare(DuplicatePosition, UTF8String("wat wat in the butt butt"));
        AssertIO(DuplicatePositionTest == true);


        uint8_t OctalValue                     = 014; // 12, 0xC
        UTF8   *Octal                          = UTF8_Format(UTF8String("%03o"), OctalValue);
        bool    OctalTest                      = UTF8_Compare(Octal, UTF8String("014"));
        AssertIO(OctalTest == true);

        UTF8 *Positional1                      = UTF8_Format(UTF8String("NumArgs: %1$u, Equal: %2$u, Type: %3$s"), 3, 1234, UTF8String("Positional"));
        bool  Positional1Test                  = UTF8_Compare(Positional1, UTF8String("NumArgs: 3, Equal: 1234, Type: Positional"));
        // "NumArgs: 3, Equal: 1234, Type: Positional"
        AssertIO(Positional1Test == true);

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
         AssertIO(Positional2Test == true);
         */

        UTF8 *Positional3                      = UTF8_Format(UTF8String("NumArgs: %2$d, %1$s EXTEND THE STRING"), UTF8String("Positional"), 2);
        bool  Positional3Test                  = UTF8_Compare(Positional3, UTF8String("NumArgs: 2, Positional EXTEND THE STRING"));
        AssertIO(Positional3Test == true);

        UTF8 *PositionPrecision                = UTF8_Format(UTF8String("%.*f"), 2, 0.33333333);
        bool PositionPrecisionTest             = UTF8_Compare(PositionPrecision, UTF8String("0.33"));
        AssertIO(PositionPrecisionTest == true);

        UTF8 *StringPrecision                  = UTF8_Format(UTF8String("%.3s"), UTF8String("FooBar"));
        bool StringPrecisionTest               = UTF8_Compare(StringPrecision, UTF8String("Foo"));
        AssertIO(StringPrecisionTest == true);

        UTF8 *Padding                          = UTF8_Format(UTF8String("%10.5i"), 9);
        bool  PaddingTest                      = UTF8_Compare(Padding, UTF8String("     00009"));
        AssertIO(PaddingTest == true);

        UTF8 *SpaceAlign                       = UTF8_Format(UTF8String("% d"), 4);
        bool SpaceAlignTest                    = UTF8_Compare(SpaceAlign, UTF8String(" 4"));
        AssertIO(SpaceAlignTest == true);

        UTF8 *ZeroLeftAlign                    = UTF8_Format(UTF8String("%05d"), 4);
        bool ZeroLeftAlignTest                 = UTF8_Compare(ZeroLeftAlign, UTF8String("00004"));
        AssertIO(ZeroLeftAlignTest == true);

        UTF8 *Percent                          = UTF8_Format(UTF8String("%%"));
        bool PercentTest                       = UTF8_Compare(Percent, UTF8String("%"));
        AssertIO(PercentTest == true);

        UTF8 *Smaller                          = UTF8_Format(UTF8String("%d"), 123);
        bool SmallerTest                       = UTF8_Compare(Smaller, UTF8String("123"));
        AssertIO(SmallerTest == true);

        UTF8 *Equal                            = UTF8_Format(UTF8String("%d"), 1094);
        bool EqualTest                         = UTF8_Compare(Equal, UTF8String("1094"));
        AssertIO(EqualTest == true);

        // Equal and Smaller fail, but Longer works, and Smaller works sometimes?
        UTF8 *Longer                           = UTF8_Format(UTF8String("%d"), 99999);
        bool LongerTest                        = UTF8_Compare(Longer, UTF8String("99999"));
        AssertIO(LongerTest == true);
        return TestPassed;

        TestCase TestCase_Format8 = {
            .Function    = Test_UTF8_Format,
            .State       = TestState_Enabled,
            .Expectation = Outcome_Passed, // TestOutcome?
        };
    }

    int main(const int argc, const char *argv[]) {
        bool TestSuitePassed      = false;
        InsecurePRNG *Insecure    = InsecurePRNG_Init(0);
        TestSuitePassed           = Test_UTF8_Format(Insecure);
        return TestSuitePassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
