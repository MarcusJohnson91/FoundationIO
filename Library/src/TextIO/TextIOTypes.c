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
    
#if (sizeof(size_t) != 8)
#error "TextIO_Normalization ONLY WORKS for 64 bit"
#endif
    
    typedef struct TextIO_Normalization {
         union {
             struct { // Optimized
                 char32_t Replacement[(sizeof(size_t) * 2) / sizeof(char32_t)];
                uint8_t ReplacementSizeMinus1:3;
            }; // Short/Optimized version
            struct { // Pointer
                char32_t *Replacement;
                size_t ReplacementSizeMinus1;
                }; // Pointer version
            };
        uint64_t Replacee;
    } TextIO_Normalization;
    
    bool TextIO_Normalization_IsInternalized(TextIO_Normalization Norm) {
        return ((Norm.Replacee & 0x10FFFF000000) >> 24) > 0 ? true : false;
    }
    
    size_t TextIO_Normalization_GetNumReplacements(TextIO_Normalization Norm) {
        AssertIO(sizeof(size_t) == 8);
        size_t NumReplacements = 0;
        if (TextIO_Normalization_IsInternalized(Norm) {
            NumReplacements = ((Norm.Replacee & 0xE00000) >> 21) + 1;
        } else {
            NumReplacements = Norm.ReplacementSizeMinus1 + 1;
        }
        return NumReplacements;
    }
    
    UTF32 TextIO_Normalization_GetReplacee(TextIO_Normalization Norm) { 
        return Norm.Replacee & UnicodeMaxCodePoint;
    }
    
    void TextIO_Normalization_SetReplacee(TextIO_Normalization Norm, UTF32 Replacee) {
        Norm.Replacee |= Replacee & UnicodeMaxCodePoint;
    }
    
    UTF32 TextIO_Normalization_GetReplacement(TextIO_Normalization Norm, size_t ReplacementIndex) {
        UTF32 Replacement = 0;
        if (ReplacementIndex + 1 > TextIO_Normalization_GetNumReplacements) {
        return 0;
        } else {
            if (TextIO_Normalization_IsInternalized(Norm) {
            if (ReplacementIndex == 0) {
            // the first Replacement is stored in Replacee so we can tell when Replacements are internalized, the remaining bits are in the rest of the members, layout not yet finalized
            Replacement = (Norm.Replacee & 0xFF000000) >> 24;
            // The bottom bits which are most likely to be set are in the top of Replacee, which means we have to shift Replacee's upper bits the most, and the other bits the least.
            } else { // Internalized, Replacement >= 1
                // 21 bits per codepoint
            }
            } else { // Externalized
                Replacement = Norm.Replacement[ReplacementIndex];
            }
        } 
        return Replacement;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
