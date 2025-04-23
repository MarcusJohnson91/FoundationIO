#include "../../../include/TextIO/Private/NormalizationIO.h"          /* Included for our declarations */

#include "../../../include/AssertIO.h"                    /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum NormalizationIOConstants {
        InternalSizeFieldMask = 0xE00000,
        InternalSizeFieldShift = 21,
    } NormalizationIOConstants;
    
    typedef struct NormalizationMap {
    static_assert(sizeof(size_t) == 8, "NormalizationMap REQUIRES 64 bit pointers"); 
         const uint64_t Replacee;
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
    } NormalizationMap;
    
    void ReplacementMap_SetSize(ReplacementMap Map, uint8_t NumReplacements) {
        AssertIO(NumReplacements >= 1);
    if (NumReplacements <= 8) {
        // Internalize
        Map.Replacee |= ((Size - 1) & 0x7) << 21;
    } else {
        // Externalize
Map.External.ReplacementSizeMinus1 = NumReplacements - 1;
    }   
    }
    
    NormalizationMap NormalizationMap_Init(UTF32 Replacee, uint8_t NumReplacements, UTF32 *Replacements) {
        AssertIO(NumReplacements >= 1);
        NormalizationMap Map = {};
        NormalizationMap_SetSize(Map, NumReplacements);
        if (NumReplacements <= 8) {
            // Internal
            switch (NumReplacements) {
                case 1:
                Map.Replacee |= (Replacee & UnicodeCodePointMask)
Map.Replacee |= ((Replacements[0] & UnicodeCodePointMask) << 24);
                case 2:
                Map.Replacee |= ((Replacements[2] & 0x7FFFF) << 45);
Map.Internal.Payload1 |= ((Replacements[1] & 0x180000) >> 19);
                case 3:
                Map.Internal.Payload1 |= ((Replacements[2] & UnicodeCodePointMask) << 2); 
                case 4:
                Map.Internal.Payload1 |= ((Replacements[3] & UnicodeCodePointMask) << 23);
                case 5:
                Map.Internal.Payload1  |= ((Replacements[4] & 0xFFFFF) << 44); \
Map.Internal.Payload0 |= ((Replacements[4] & 0x100000) >> 20);
                case 6:
                Map.Internal.Payload0 |= ((Replacements[5] & UnicodeCodePointMask) << 1); 
                case 7:
                Map.Internal.Payload0 |= ((Replacements[6] & UnicodeCodePointMask) << 22);
                case 8:
                Map.Internal.Payload0 |= ((Replacements[7] & UnicodeCodePointMask) << 43);
                break; // Only break at the end, fallthrough is intentional 
            }
        } else {
            // External
            Map.Replacee = (Replacee & UnicodeMaxCodePoint);
    Map.External.Replacements = Replacements;
        }
        return Map;
    }
    
    bool NormalizationMap_IsInternalized(NormalizationMap Map) {
        return ((Map.Replacee & InternalSizeFieldMask) >> InternalSizeFieldShift) > 0 ? true : false;
    }
    
    size_t NormalizationMap_GetNumReplacements(NormalizationMap Map) {
        size_t NumReplacements = 0;
        if (TextIO_StringMap_IsInternalized(Map) {
            NumReplacements = ((Map.Replacee & InternalSizeFieldMask) >> 21) + 1;
        } else {
            NumReplacements = Map.External.ReplacementSizeMinus1 + 1;
        }
        return NumReplacements;
    }
    
    UTF32 NormalizationMap_GetReplacee(NormalizationMap Map) { 
        return Map.Replacee & UnicodeMaxCodePoint;
    }
    
    void NormalizationMap_SetReplacee(NormalizationMap Map, UTF32 Replacee) {
        Map.Replacee |= Replacee & UnicodeMaxCodePoint;
    }
    
    UTF32 NormalizationMap_GetReplacement(NormalizationMap Map, size_t ReplacementIndex) {
        UTF32 Replacement = 0;
        if (ReplacementIndex + 1 > NormalizationMap_GetNumReplacements(Map)) {
        return 0;
        } else {
            if (NormalizationMap_IsInternalized(Map) && ReplacementIndex <= 7) {
            switch (ReplacementIndex) {
                case 0:
                Replacement = Map.Replacee & (UnicodeCodePointMask << 24) >> 24;
                break;
                case 1:
                Replacement = Map.Replacee & (UnicodeCodePointMask << 45) >> 45;
                Replacement |= (Map.Internal.Payload1 & 0x3) << 19;
                case 2:
                Replacement = Map.Internal.Payload1 & (UnicodeCodePointMask << 2) >> 2;
                break;
                case 3:
                Replacement = Map.Internal.Payload1 & (UnicodeCodePointMask << 23) >> 23;
                break;
                case 4:
                Replacement = (Map.Internal.Payload1 & UnicodeCodePointMask) >> 44;
                Replacement |= (Map.Internal.Payload0 & 0x1) << 20;
                break;
                case 5:
                Replacement = Map.Internal.Payload0 & (UnicodeCodePointMask << 1) >> 1;
                break;
                case 6:
                Replacement = Map.Internal.Payload0 & (UnicodeCodePointMask << 22) >> 22;
                break;
                case 7:
                Replacement = (
                Map.Internal.Payload0 & (UnicodeCodePointMask << 43) >> 43;
                break;
            }
          } else {
              Replacement = Map.External.Replacement[ReplacementIndex]
          }
        } 
        return Replacement;
    }
    
    size_t NormalizationMap_UTF8_GetNumCodeUnitsInReplacements(NormalizationMap Map) {
        size_t NumCodeUnits = 0;
            for (size_t Replacement = 0; Replacement < NormalizationMap_GetNumReplacements(Map); Replacement++) {
                NumCodeUnits += UTF32_GetCodePointSizeInUTF8CodeUnits(TextIO_StringMap_GetReplacement(Map, Replacement));
            }
        return NumCodeUnits;
    }
    
    size_t NormalizationMap_UTF16_GetNumCodeUnitsInReplacements(NormalizationMap Map) {
        size_t NumCodeUnits = 0;
            for (size_t Replacement = 0; Replacement < NormalizationMap_GetNumReplacements(Map); Replacement++) {
                NumCodeUnits += UTF32_GetCodePointSizeInUTF16CodeUnits(NormalizationMap_GetReplacement(Map, Replacement));
            }
        return NumCodeUnits;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
