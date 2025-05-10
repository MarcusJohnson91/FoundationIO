#include "../../../include/TestIO.h"                  /* Included for testing */
#include "../../../include/TextIO/LocalizationIO.h"   /* Included for our declarations */

#include "../../../include/AssertIO.h"                /* Included for Assertions */
#include "../../../include/TextIO/StringIO.h"         /* Included for StringIO */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    bool Test_DelocalizeInteger(void) {
        bool TestPassed                         = Yes;
        PlatformIO_Immutable(UTF8 *)  Unlocalized = UTF8String("1000000000");
        PlatformIO_Immutable(UTF8 *) Localized   = UTF8String("1,000,000,000");
        UTF8                     *Delocalized   = UTF8_DelocalizeInteger(Base_Integer | Base_Radix10, Localized);
        AssertIO(UTF8_Compare(Delocalized, Unlocalized) == true);
        return TestPassed;
    }

    bool Test_DelocalizeDecimal(void) {
        bool TestPassed                         = Yes;
        PlatformIO_Immutable(UTF8 *) Unlocalized = UTF8String("1000000000.0");
        PlatformIO_Immutable(UTF8 *) Localized   = UTF8String("1,000,000,000.0");
        UTF8                     *Delocalized   = UTF8_DelocalizeInteger(Base_Integer | Base_Radix10, Localized);
        AssertIO(UTF8_Compare(Delocalized, Unlocalized) == true);
        return TestPassed;
    }

    int main(const int argc, const char *argv[]) {
        bool TestsPassed = No;
        TestsPassed      = Test_DelocalizeInteger();
        return TestsPassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
