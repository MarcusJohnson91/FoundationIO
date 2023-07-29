#include "../../include/TextIO/TextIOTypes.h"          /* Included for our declarations */

#include "../../include/AssertIO.h"                    /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct Unicode8 {
        size_t  NumCodeUnits;
        char8_t Array[];
    } Unicode8;

#define UTF8Constant(Literal) {.NumCodeUnits = PlatformIO_GetStringSizeInCodeUnits(Literal), .Array = Literal}

    Unicode8 Blah2 = UTF8Constant("Blah?");

    bool Unicode8_Compare(Unicode8 String1, Unicode8 String2) {
        bool StringsMatch = true;
        if (String1.NumCodeUnits == String2.NumCodeUnits) {
            for (size_t CodeUnit = 0; CodeUnit < String1.NumCodeUnits; CodeUnit++) {
                if (String1.Array[CodeUnit] != String2.Array[CodeUnit]) {
                    StringsMatch = false;
                    break;
                }
            }
        } else {
            StringsMatch = false;
        }
        return StringsMatch;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
