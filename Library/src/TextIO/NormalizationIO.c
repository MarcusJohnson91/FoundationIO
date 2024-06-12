#include "../../include/TextIO/TextIOTypes.h"          /* Included for our declarations */

#include "../../include/AssertIO.h"                    /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#if (sizeof(size_t) != 8)
#error "TextIO_StringMap ONLY WORKS on 64 bit architectures"
#endif
    
    typedef struct TextIO_StringMap {
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
    } TextIO_StringMap;
    
    bool TextIO_StringMap_IsInternalized(TextIO_StringMap Map) {
        return ((Map.Replacee & 0x10FFFF000000) >> 24) > 0 ? true : false;
    }
    
    size_t TextIO_StringMap_GetNumReplacements(TextIO_StringMap Map) {
        size_t NumReplacements = 0;
        if (TextIO_StringMap_IsInternalized(Map) {
            NumReplacements = ((Map.Replacee & 0xE00000) >> 21) + 1;
        } else {
            NumReplacements = Map.External.ReplacementSizeMinus1 + 1;
        }
        return NumReplacements;
    }
    
    UTF32 TextIO_StringMap_GetReplacee(TextIO_StringMap Map) { 
        return Map.Replacee & UnicodeMaxCodePoint;
    }
    
    void TextIO_StringMap_SetReplacee(TextIO_StringMap Map, UTF32 Replacee) {
        Map.Replacee |= Replacee & UnicodeMaxCodePoint;
    }
    
    UTF32 TextIO_StringMap_GetReplacement(TextIO_StringMap Map, size_t ReplacementIndex) {
        UTF32 Replacement = 0;
        if (ReplacementIndex + 1 > TextIO_StringMap_GetNumReplacements(Map)) {
        return 0;
        } else {
            if (TextIO_StringMap_IsInternalized(Map) {
            if (ReplacementIndex == 0) {
            Replacement = (Map.Replacee & 0x1FFFFF000000) >> 24;
            } else if (ReplacementIndex == 1) {
            Replacement = (Map.Replacee & 0xFFFFE00000000000) >> 45;
            Replacement |= ((Map.Internal.Payload1 & 0x3) << 19);
            } else if (ReplacementIndex == 2) {
                Replacement = ((Map.Internal.Payload1 & 0x7FFFFC) >> 2);
            } else if (ReplacementIndex == 3) {
                Replacement = ((Map.Internal.Payload1 & 0xFFFFF800000) >> 23;
            } else if (ReplacementIndex == 4) {
            // 20 bits here, gotta OR it with the last bit of Payload0 at the top
                Replacement = ((Map.Internal.Payload1 & 0xFFFFF00000000000) >> 44);
                Replacement |= ((Map.Internal.Payload0 & 0x1) << 20);
            } else if (ReplacementIndex == 5) {
                Replacement = ((Map.Internal.Payload0 & 0x3FFFFE) >> 1);
            } else if (ReplacementIndex == 6) {
                Replacement = ((Map.Internal.Payload0 & 0x7FFFFC00000) >> 22;
            } else if (ReplacementIndex == 7) {
                Replacement = ((Map.Internal.Payload0 & 0xFFFFF80000000000) >> 43);
            } else { // Externalized
                Replacement = Map.External.Replacement[ReplacementIndex];
            }
        } 
        return Replacement;
    }
    
    size_t TextIO_StringMap_UTF8_GetNumCodeUnitsInReplacements(TextIO_StringMap Map) {
        size_t NumCodeUnits = 0;
            for (size_t Replacement = 0; Replacement < TextIO_StringMap_GetNumReplacements(Map); Replacement++) {
                NumCodeUnits += UTF32_GetCodePointSizeInUTF8CodeUnits(TextIO_StringMap_GetReplacement(Map, Replacement));
            }
        return NumCodeUnits;
    }
    
    size_t TextIO_StringMap_UTF16_GetNumCodeUnitsInReplacements(TextIO_StringMap Map) {
        size_t NumCodeUnits = 0;
            for (size_t Replacement = 0; Replacement < TextIO_StringMap_GetNumReplacements(Map); Replacement++) {
                NumCodeUnits += UTF32_GetCodePointSizeInUTF16CodeUnits(TextIO_StringMap_GetReplacement(Map, Replacement));
            }
        return NumCodeUnits;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
