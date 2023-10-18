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

    typedef struct TextIO_CaseMap {
        const uint64_t Map;
        /*
    Bits 0-20 = Start of the range.
    Bits 21-41 = End of the range.
    Bits 42-63 = Start of Replacement, so we can calculate the real value of the casefolded codepoint
    */
    } TextIO_CaseMap;
    
    typedef struct TextIO_Normalization {
         const union {
             const struct Internal {
                 const uint64_t Pauload0;
                 const uint64_t Payload1;
            } Internal;
            const struct External {
                const char32_t *Replacement;
                const size_t ReplacementSizeMinus1;
                } External;
            };
        const uint64_t Replacee;
    } TextIO_Normalization;
    
    bool TextIO_Normalization_IsInternalized(TextIO_Normalization Norm) {
        return ((Norm.Replacee & 0x10FFFF000000) >> 24) > 0 ? true : false;
    }
    
    size_t TextIO_Normalization_GetNumReplacements(TextIO_Normalization Norm) {
        size_t NumReplacements = 0;
        if (TextIO_Normalization_IsInternalized(Norm) {
            NumReplacements = ((Norm.Replacee & 0xE00000) >> 21) + 1;
        } else {
            NumReplacements = Norm.External.ReplacementSizeMinus1 + 1;
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
            Replacement = (Norm.Replacee & 0x1FFFFF000000) >> 24;
            } else if (ReplacementIndex == 1) {
            Replacement = (Norm.Replacee & 0xFFFFE00000000000) >> 45;
            Replacement |= ((Norm.Internal.Payload1 & 0x3) << 19);
            } else if (ReplacementIndex == 2) {
                Replacement = ((Norm.Internal.Payload1 & 0x7FFFFC) >> 2);
            } else if (ReplacementIndex == 3) {
                Replacement = ((Norm.Internal.Payload1 & 0xFFFFF800000) >> 23;
            } else if (ReplacementIndex == 4) {
            // 20 bits here, gotta OR it with the last bit of Payload0 at the top
                Replacement = ((Norm.Internal.Payload1 & 0xFFFFF00000000000) >> 44);
                Replacement |= ((Norm.Internal.Payload0 & 0x1) << 20);
            } else if (ReplacementIndex == 5) {
                Replacement = ((Norm.Internal.Payload0 & 0x3FFFFE) >> 1);
            } else if (ReplacementIndex == 6) {
                Replacement = ((Norm.Internal.Payload0 & 0x7FFFFC00000) >> 22;
            } else if (ReplacementIndex == 7) {
                Replacement = ((Norm.Internal.Payload0 & 0xFFFFF80000000000) >> 43);
            }
            } else { // Externalized
                Replacement = Norm.External.Replacement[ReplacementIndex];
            }
        } 
        return Replacement;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
