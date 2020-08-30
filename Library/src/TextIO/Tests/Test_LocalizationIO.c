#include "../../Library/include/TestIO.h"
#include "../../Library/include/TextIO/LocalizationIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    bool Test_DelocalizeInteger(void) {
        bool TestPassed   = Yes;
        UTF8 *Unlocalized = UTF8String("1000000000");
        UTF8 *Localized   = UTF8String("1,000,000,000");
        UTF8 *Delocalized = UTF8_DelocalizeInteger(Localized, Base_Integer | Base_Radix10);
        if (UTF8_Compare(Delocalized, Unlocalized) == false) {
            TestPassed    = No;
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Delocalize failed: %s does not match %s"), Delocalized, Unlocalized);
        }
        return TestPassed;
    }
    
    int main(int argc, const char *argv[]) {
        bool TestsPassed = No;
        TestsPassed      = Test_DelocalizeInteger();
        return TestsPassed;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
