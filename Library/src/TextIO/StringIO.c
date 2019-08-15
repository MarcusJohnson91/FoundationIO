#include "../include/StringIO.h"       /* Included for our declarations */
#include "../include/UnicodeTables.h"  /* Included for the Unicode tables */

#include "../include/Log.h"            /* Included for error logging */
#include "../include/Math.h"           /* Included for endian swapping */

#include <stdarg.h>                    /* Included for va_list, va_copy, va_start, va_end */
#include <wchar.h>                     /* Included for Fwide */

#ifdef __cplusplus
extern "C" {
#endif
    
    uint8_t UTF8_GetCodePointSizeInCodeUnits(UTF8 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (((CodeUnit & 0x80) >> 7) == 0) {
            CodePointSize     = 1;
        } else if (((CodeUnit & 0xF8) >> 3) == 0x1E) {
            CodePointSize     = 4;
        } else if (((CodeUnit & 0xF0) >> 4) == 0x0E) {
            CodePointSize     = 3;
        } else if (((CodeUnit & 0xE0) >> 5) == 0x06) {
            CodePointSize     = 2;
        } else if (((CodeUnit & 0xC0) >> 6) == 0x02) {
            CodePointSize     = 1;
        }
        return CodePointSize;
    }
    
    uint8_t UTF16_GetCodePointSizeInCodeUnits(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit < UTF16HighSurrogateStart || (CodeUnit > UTF16LowSurrogateEnd && CodeUnit <= UTF16MaxCodeUnit)) {
            CodePointSize     = 1;
        } else if (CodeUnit >= UTF16HighSurrogateStart && CodeUnit <= UTF16LowSurrogateEnd) {
            CodePointSize     = 2;
        } else {
            CodePointSize     = 0;
        }
        return CodePointSize;
    }
    
    uint64_t UTF8_GetStringSizeInCodeUnits(UTF8 *String) {
        uint64_t StringSizeInCodeUnits = 0ULL;
        if (String != NULL) {
            while (String[StringSizeInCodeUnits] != FoundationIONULLTerminator) {
                StringSizeInCodeUnits += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    uint64_t UTF16_GetStringSizeInCodeUnits(UTF16 *String) {
        uint64_t StringSizeInCodeUnits = 0ULL;
        if (String != NULL) {
            while (String[StringSizeInCodeUnits] != FoundationIONULLTerminator) {
                StringSizeInCodeUnits += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    uint64_t UTF8_GetStringSizeInCodePoints(UTF8 *String) {
        uint64_t StringSizeInCodePoints = 0ULL;
        uint64_t CodeUnit               = 0ULL;
        if (String != NULL) {
            while (String[CodeUnit] != FoundationIONULLTerminator) {
                StringSizeInCodePoints += 1;
                CodeUnit               += UTF8_GetCodePointSizeInCodeUnits(String[CodeUnit]);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringSizeInCodePoints;
    }
    
    uint64_t UTF16_GetStringSizeInCodePoints(UTF16 *String) {
        uint64_t NumCodePoints             = 0ULL;
        if (String != NULL) {
            uint64_t CodeUnit              = 0ULL;
            while (String[CodeUnit] != FoundationIONULLTerminator) {
                if (String[CodeUnit] <= UTF16MaxCodeUnit && (String[CodeUnit] < UTF16HighSurrogateStart || String[CodeUnit] > UTF16LowSurrogateEnd)) {
                    NumCodePoints         += 1;
                } else if (String[CodeUnit] >= UTF16HighSurrogateStart && String[CodeUnit] <= UTF16HighSurrogateEnd) {
                    NumCodePoints         += 1; // Only count the high surrogates, not the low surrogates so that the codepoint count is accurate
                }
                CodeUnit                  += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF32_GetStringSizeInCodePoints(UTF32 *String) {
        uint64_t NumCodePoints = 0ULL;
        if (String != NULL) {
            while (String[NumCodePoints] != FoundationIONULLTerminator) {
                NumCodePoints += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF32_GetStringSizeInUTF8CodeUnits(UTF32 *String) {
        uint64_t CodePoint            = 0ULL;
        uint64_t UTF8CodeUnits        = 0ULL;
        if (String != NULL) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (String[CodePoint] <= 0x7F) {
                    UTF8CodeUnits    += 1;
                } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                    UTF8CodeUnits    += 2;
                } else if (String[CodePoint] >= 0x800 && String[CodePoint] <= 0xFFFF) {
                    UTF8CodeUnits    += 3;
                } else if (String[CodePoint] >= 0x10000 && String[CodePoint] <= UnicodeMaxCodePoint) {
                    UTF8CodeUnits    += 4;
                }
                CodePoint            += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return UTF8CodeUnits;
    }
    
    uint64_t UTF32_GetStringSizeInUTF16CodeUnits(UTF32 *String) {
        uint64_t CodePoint          = 0ULL;
        uint64_t UTF16CodeUnits     = 0ULL;
        if (String != NULL) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (String[CodePoint] <= UTF16MaxCodeUnit) {
                    UTF16CodeUnits += 1;
                } else if (String[CodePoint] > UTF16MaxCodeUnit && String[CodePoint] <= UnicodeMaxCodePoint) {
                    UTF16CodeUnits += 2;
                }
                CodePoint          += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return UTF16CodeUnits;
    }
    
    uint64_t UTF8_GetStringSizeInGraphemes(UTF8 *String) {
        uint64_t NumGraphemes  = 0ULL;
        if (String != NULL) {
            UTF32 *Decoded     = UTF8_Decode(String);
            NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
            free(Decoded);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    uint64_t UTF16_GetStringSizeInGraphemes(UTF16 *String) {
        uint64_t NumGraphemes  = 0ULL;
        if (String != NULL) {
            UTF32 *Decoded     = UTF16_Decode(String);
            NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
            free(Decoded);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    uint64_t UTF32_GetStringSizeInGraphemes(UTF32 *String) {
        uint64_t NumGraphemes         = 0ULL;
        uint64_t CodePoint            = 0ULL;
        if (String != NULL) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                for (uint64_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                    if (String[CodePoint] != GraphemeExtensionTable[GraphemeExtension]) {
                        NumGraphemes += 1;
                    }
                }
                CodePoint            += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    uint64_t UTF8_GetNumFormatSpecifiers(UTF8 *String) {
        uint64_t StringSizeInCodeUnits = UTF8_GetStringSizeInCodeUnits(String);
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodeUnit              = 0ULL;
        if (String != NULL) {
            while (CodeUnit < StringSizeInCodeUnits) {
                if (CodeUnit + 1 < StringSizeInCodeUnits) {
                    if (String[CodeUnit] == '%' && String[CodeUnit + 1] == '%') {
                        NumSpecifiers += 1;
                        CodeUnit      += 2;
                    }
                }
                if (String[CodeUnit] == '%' || String[CodeUnit] == '\\') {
                    NumSpecifiers     += 1;
                    CodeUnit          += 1;
                }
                CodeUnit              += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF16_GetNumFormatSpecifiers(UTF16 *String) {
        uint64_t StringSizeInCodeUnits = UTF16_GetStringSizeInCodeUnits(String);
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodeUnit              = 0ULL;
        if (String != NULL) {
            while (CodeUnit < StringSizeInCodeUnits) {
                if (CodeUnit + 1 < StringSizeInCodeUnits) {
                    if (String[CodeUnit] == U16('%') && String[CodeUnit + 1] == U16('%')) {
                        NumSpecifiers += 1;
                        CodeUnit      += 2;
                    }
                }
                if (String[CodeUnit] == U16('%') || String[CodeUnit] == U16('\\')) {
                    NumSpecifiers     += 1;
                    CodeUnit          += 1;
                }
                CodeUnit              += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF32_GetNumFormatSpecifiers(UTF32 *String) {
        uint64_t StringSizeInCodeUnits = UTF32_GetStringSizeInCodePoints(String);
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodeUnit              = 0ULL;
        if (String != NULL) {
            while (CodeUnit < StringSizeInCodeUnits) {
                if (CodeUnit + 1 < StringSizeInCodeUnits) {
                    if (String[CodeUnit] == U32('%') && String[CodeUnit + 1] == U32('%')) {
                        NumSpecifiers += 1;
                        CodeUnit      += 2;
                    }
                }
                if (String[CodeUnit] == U32('%') || String[CodeUnit] == U32('\\')) {
                    NumSpecifiers     += 1;
                    CodeUnit          += 1;
                }
                CodeUnit              += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    UTF8 *UTF8_ExtractGrapheme(UTF8 *String, uint64_t Grapheme2Extract) {
        UTF8 *Grapheme        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, Grapheme2Extract);
            free(String32);
            Grapheme          = UTF8_Encode(Grapheme32);
            free(Grapheme32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF16 *UTF16_ExtractGrapheme(UTF16 *String, uint64_t Grapheme2Extract) {
        UTF16 *Grapheme        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, Grapheme2Extract);
            free(String32);
            Grapheme          = UTF16_Encode(Grapheme32);
            free(Grapheme32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF32 *UTF32_ExtractGrapheme(UTF32 *String, uint64_t Grapheme2Extract) {
        UTF32 *Grapheme                    = NULL;
        if (String != NULL) {
            uint64_t NumCodePointsInString = UTF32_GetStringSizeInCodePoints(String);
            uint64_t NumGraphemesInString  = UTF32_GetStringSizeInGraphemes(String);
            uint64_t CurrentGrapheme       = 0ULL;
            uint64_t CodePointStart        = 0ULL;
            if (Grapheme2Extract < NumGraphemesInString) {
                for (uint64_t CodePoint = 0ULL; CodePoint < NumCodePointsInString; CodePoint++) {
                    for (uint64_t GraphemeCodePoint = 0ULL; GraphemeCodePoint < GraphemeExtensionTableSize; GraphemeCodePoint++) {
                        if (String[CodePoint] == GraphemeExtensionTable[GraphemeCodePoint]) {
                            CurrentGrapheme                         += 1;
                        }
                        if (CurrentGrapheme == Grapheme2Extract) {
                            CodePointStart                           = CodePoint;
                            break;
                        }
                    }
                }
                uint64_t GraphemeSizeInCodePoints                    = 0ULL;
                for (uint64_t CodePoint = CodePointStart; CodePoint < NumCodePointsInString; CodePoint++) {
                    for (uint64_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                        if (String[CodePoint] == GraphemeExtensionTable[GraphemeExtension]) {
                            GraphemeSizeInCodePoints                += 1;
                        } else {
                            break;
                        }
                    }
                }
                Grapheme                                             = calloc(GraphemeSizeInCodePoints + FoundationIONULLTerminatorSize, sizeof(UTF32));
                if (Grapheme != NULL) {
                    for (uint64_t GraphemeCodePoint = CodePointStart; GraphemeCodePoint < CodePointStart + GraphemeSizeInCodePoints; GraphemeCodePoint++) {
                        Grapheme[GraphemeCodePoint - CodePointStart] = String[GraphemeCodePoint];
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("Couldn't allocate %llu CodePoints for the Grapheme"), GraphemeSizeInCodePoints);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Grapheme %llu is greater than there are Graphemes %llu"), Grapheme2Extract, NumGraphemesInString);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    bool UTF8_HasBOM(UTF8 *String) {
        bool StringHasABOM        = No;
        if (String != NULL) {
            uint64_t StringSize   = UTF8_GetStringSizeInCodeUnits(String);
            if (StringSize >= 3) {
                if (String[0] == UTF8BOM_1 && String[1] == UTF8BOM_2 && String[2] == UTF8BOM_3) {
                    StringHasABOM = Yes;
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool UTF16_HasBOM(UTF16 *String) {
        bool StringHasABOM    = No;
        if (String != NULL) {
            if (String[0] == UTF16BOM_LE || String[0] == UTF16BOM_BE) {
                StringHasABOM = Yes;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool UTF32_HasBOM(UTF32 *String) {
        bool StringHasABOM    = No;
        if (String != NULL) {
            if (String[0] == UTF32BOM_LE || String[0] == UTF32BOM_BE) {
                StringHasABOM = Yes;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool  UTF8_HasUNCPathPrefix(UTF8 *String) {
        bool StringHasUNCPathPrefix = No;
        if (String != NULL) {
            uint64_t StringSize     = UTF8_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF8_HasBOM(String);
                if (StringHasBOM && StringSize >= UTF8BOMSizeInCodeUnits + UnicodeUNCPathPrefixSize) {
                    // "//?/" or "\\?\"
                    if ((String[1] == '/' || String[1] == '\\') && (String[2] == '/' || String[2] == '\\') && String[3] == '?' && (String[4] == '/' || String[4] == '\\')) {
                        StringHasUNCPathPrefix = Yes;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    if ((String[0] == '/' || String[0] == '\\') && (String[1] == '/' || String[1] == '\\') && String[2] == '?' && (String[3] == '/' || String[3] == '\\')) {
                        StringHasUNCPathPrefix = Yes;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasUNCPathPrefix;
    }
    
    bool  UTF16_HasUNCPathPrefix(UTF16 *String) {
        bool StringHasUNCPathPrefix = No;
        if (String != NULL) {
            uint64_t StringSize     = UTF16_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF16_HasBOM(String);
                if (StringHasBOM && StringSize >= UTF16BOMSizeInCodeUnits + UnicodeUNCPathPrefixSize) {
                    if ((String[1] == U16('/') || String[1] == U16('\\')) && (String[2] == U16('/') || String[2] == U16('\\')) && String[3] == U16('?') && (String[4] == U16('/') || String[4] == U16('\\'))) {
                        StringHasUNCPathPrefix = Yes;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    if ((String[0] == U16('/') || String[0] == U16('\\')) && (String[1] == U16('/') || String[1] == U16('\\')) && String[2] == U16('?') && (String[3] == U16('/') || String[3] == U16('\\'))) {
                        StringHasUNCPathPrefix = Yes;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasUNCPathPrefix;
    }
    
    bool  UTF32_HasUNCPathPrefix(UTF32 *String) {
        bool StringHasUNCPathPrefix = No;
        if (String != NULL) {
            uint64_t StringSize     = UTF32_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF32_HasBOM(String);
                if (StringHasBOM && StringSize >= UnicodeBOMSizeInCodePoints + UnicodeUNCPathPrefixSize) {
                    if ((String[1] == U32('/') || String[1] == U32('\\')) && (String[2] == U32('/') || String[2] == U32('\\')) && String[3] == U32('?') && (String[4] == U32('/') || String[4] == U32('\\'))) {
                        StringHasUNCPathPrefix = Yes;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    if ((String[0] == U32('/') || String[0] == U32('\\')) && (String[1] == U32('/') || String[1] == U32('\\')) && String[2] == U32('?') && (String[3] == U32('/') || String[3] == U32('\\'))) {
                        StringHasUNCPathPrefix = Yes;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasUNCPathPrefix;
    }
    
    bool UTF8_PathIsAbsolute(UTF8 *String) {
        bool PathIsAbsolute = No;
        if (String != NULL) {
#if  (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
            if (String[0] == '/') {
                PathIsAbsolute     = Yes;
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            uint64_t StringSize    = UTF8_GetStringSizeInCodeUnits(String);
            if (StringSize > 2) {
                if (String[1] == ':') {
                    PathIsAbsolute = Yes;
                }
            }
#endif
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return PathIsAbsolute;
    }
    
    bool UTF16_PathIsAbsolute(UTF16 *String) {
        bool PathIsAbsolute        = No;
        if (String != NULL) {
#if  (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
            if (String[0] == U16('/')) {
                PathIsAbsolute     = Yes;
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(String);
            if (StringSize > 2) {
                if (String[1] == U16(':')) {
                    PathIsAbsolute = Yes;
                }
            }
#endif
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return PathIsAbsolute;
    }
    
    bool UTF32_PathIsAbsolute(UTF32 *String) {
        bool PathIsAbsolute = No;
        if (String != NULL) {
#if  (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
            if (String[0] == U32('/')) {
                PathIsAbsolute = Yes;
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            if (StringSize > 2) {
                if (String[1] == U32(':')) {
                    PathIsAbsolute = Yes;
                }
            }
#endif
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return PathIsAbsolute;
    }
    
    bool UTF8_HasNewLine(UTF8 *String) {
        bool StringHasNewLine = No;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            StringHasNewLine  = UTF32_HasNewLine(String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasNewLine;
    }
    
    bool UTF16_HasNewLine(UTF16 *String) {
        bool StringHasNewLine = No;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            StringHasNewLine  = UTF32_HasNewLine(String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasNewLine;
    }
    
    bool UTF32_HasNewLine(UTF32 *String) {
        bool StringHasNewLine            = No;
        if (String != NULL) {
            uint64_t CodePoint           = 1ULL;
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (String[CodePoint] == 0x0A) { // MacOSX, Unix
                    StringHasNewLine     = Yes;
                }
                if (String[CodePoint - 1] == 0x0D && String[CodePoint] == 0x0A) { // Windows
                    StringHasNewLine     = Yes;
                }
                if (String[CodePoint] == 0x0D) { // Classic Mac
                    StringHasNewLine     = Yes;
                }
                CodePoint               += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringHasNewLine;
    }
    
    bool UTF8_IsValid(UTF8 *String) {
        uint64_t CodeUnit    = 0ULL;
        bool     IsValidUTF8 = Yes;
        if (String != NULL) {
            while (String[CodeUnit] != FoundationIONULLTerminator) {
                // Make sure no byte equals 0xC0 or 0xC1, or 0xF5 - 0xFF
                // Test for Overlong encodings and shit.
                uint8_t Byte = String[CodeUnit];
                if (Byte == 0xC0 || Byte == 0xC1 || Byte == 0xF5 || Byte == 0xF6 || Byte == 0xF7 || Byte == 0xF8 || Byte == 0xF9 || Byte == 0xFA || Byte == 0xFB || Byte == 0xFC || Byte == 0xFD || Byte == 0xFE || Byte == 0xFF) {
                    IsValidUTF8 = No;
                    break;
                } else {
                    // If a codepoint is a header byte, verify that there are the required number of continuation code units afterwards.
                }
                CodeUnit += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return IsValidUTF8;
    }
    
    bool UTF16_IsValid(UTF16 *String) {
        uint64_t CodeUnit             = 1ULL;
        bool     IsValidUTF16         = Yes;
        if (String != NULL) {
            while (String[CodeUnit] != FoundationIONULLTerminator) {
                if ((String[CodeUnit - 1] >= UTF16HighSurrogateStart && String[CodeUnit - 1] <= UTF16HighSurrogateEnd) && (String[CodeUnit] <= UTF16LowSurrogateStart && String[CodeUnit] >= UTF16LowSurrogateEnd)) {
                    IsValidUTF16 = No;
                    break;
                }
                CodeUnit += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return IsValidUTF16;
    }
    
    bool UTF32_IsValid(UTF32 *String) {
        uint64_t CodePoint       = 0ULL;
        bool     IsValidUTF32    = Yes;
        if (String != NULL) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (String[CodePoint] >= UnicodeMaxCodePoint || (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd)) {
                    IsValidUTF32 = No;
                    break;
                }
                CodePoint       += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return IsValidUTF32;
    }
    
    UTF8 *UTF8_AddBOM(UTF8 *String) {
        UTF8 *StringWithBOM = NULL;
        if (String != NULL) {
            UTF32 *String32  = UTF8_Decode(String);
            UTF32 *BOMAdded  = UTF32_AddBOM(String32, ByteOrder_Big);
            free(String32);
            StringWithBOM    = UTF8_Encode(BOMAdded);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF16 *UTF16_AddBOM(UTF16 *String, StringIOByteOrders BOM2Add) {
        UTF16 *StringWithBOM = NULL;
        if (String != NULL) {
            UTF32 *String32  = UTF16_Decode(String);
            UTF32 *BOMAdded  = UTF32_AddBOM(String32, BOM2Add);
            StringWithBOM    = UTF16_Encode(BOMAdded);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF32 *UTF32_AddBOM(UTF32 *String, StringIOByteOrders BOM2Add) {
        UTF32   *StringWithBOM        = NULL;
        UTF32    ByteOrder            = 0;
        if (String != NULL) {
            if (String[0] != UTF32BOM_LE && String[0] != UTF32BOM_BE) {
                uint64_t StringSize   = UTF32_GetStringSizeInCodePoints(String) + UnicodeBOMSizeInCodePoints + FoundationIONULLTerminatorSize;
                StringWithBOM         = calloc(StringSize + UnicodeBOMSizeInCodePoints + FoundationIONULLTerminatorSize, sizeof(UTF32));
                if (StringWithBOM != NULL) {
                    if (BOM2Add == ByteOrder_Native) {
#if   (FoundationIOTargetByteOrder == FoundationIOByteOrderLE)
                        ByteOrder     = UTF32BOM_LE;
#elif (FoundationIOTargetByteOrder == FoundationIOByteOrderBE)
                        ByteOrder     = UTF32BOM_BE;
#endif
                    } else if (BOM2Add == ByteOrder_Little) {
                        ByteOrder     = UTF32BOM_LE;
                    } else if (BOM2Add == ByteOrder_Big) {
                        ByteOrder     = UTF32BOM_BE;
                    }
                    StringWithBOM[0] = ByteOrder;
                    for (uint64_t CodePoint = 1ULL; CodePoint < StringSize; CodePoint++) {
                        StringWithBOM[CodePoint] = String[CodePoint + 1];
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("StringWithBOM couldn't be allocated"));
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF8 *UTF8_RemoveBOM(UTF8 *String) {
        UTF8    *BOMLessString                      = NULL;
        uint64_t StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF8_GetStringSizeInCodeUnits(String);
            bool StringHasBOM                       = UTF8_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                       = calloc(StringSize - UTF8BOMSizeInCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF8));
                if (BOMLessString != NULL) {
                    for (uint64_t CodeUnit = 2ULL; CodeUnit < StringSize; CodeUnit++) {
                        BOMLessString[CodeUnit - 3] = String[CodeUnit];
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("Couldn't allocate BOMLessString"));
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    
    UTF16 *UTF16_RemoveBOM(UTF16 *String) {
        UTF16   *BOMLessString                      = NULL;
        uint64_t StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF16_GetStringSizeInCodeUnits(String);
            bool StringHasBOM                       = UTF16_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                       = calloc(StringSize - UTF16BOMSizeInCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF16));
                if (BOMLessString != NULL) {
                    for (uint64_t CodeUnit = 1ULL; CodeUnit < StringSize; CodeUnit++) {
                        BOMLessString[CodeUnit - 1] = String[CodeUnit];
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("Couldn't allocate BOMLessString"));
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    
    UTF32 *UTF32_RemoveBOM(UTF32 *String) {
        UTF32   *BOMLessString                   = NULL;
        uint64_t StringSize                      = 0ULL;
        if (String != NULL) {
            StringSize                           = UTF32_GetStringSizeInCodePoints(String);
            bool StringHasBOM                    = UTF32_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                    = calloc(StringSize - UnicodeBOMSizeInCodePoints + FoundationIONULLTerminatorSize, sizeof(UTF32));
                if (BOMLessString != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                        BOMLessString[CodePoint] = String[CodePoint + 1];
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("Couldn't allocate BOMLessString"));
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    
    UTF32 *UTF8_Decode(UTF8 *String) {
        uint64_t StringSize                          = 0ULL;
        uint64_t CodePoint                           = 0ULL;
        uint64_t CodeUnit                            = 0ULL;
        UTF32   *DecodedString                       = NULL;
        
        if (String != NULL) {
            StringSize                               = UTF8_GetStringSizeInCodePoints(String);
            DecodedString                            = calloc(StringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (DecodedString != NULL) {
                while (String[CodeUnit] != FoundationIONULLTerminator) {
                    uint8_t CodePointSize            = UTF8_GetCodePointSizeInCodeUnits(String[CodeUnit]);
                    switch (CodePointSize) {
                            case 1:
                            DecodedString[CodePoint] =  String[CodeUnit] & 0x7F;
                            CodeUnit                += 1;
                            CodePoint               += 1;
                            break;
                            case 2:
                            DecodedString[CodePoint] |= (String[CodeUnit]     & 0x1F) << 6;
                            DecodedString[CodePoint] |= (String[CodeUnit + 1] & 0x3F) << 0;
                            CodeUnit                 += 2;
                            CodePoint                += 1;
                            break;
                            case 3:
                            DecodedString[CodePoint] |= (String[CodeUnit]     & 0x0F) << 12;
                            DecodedString[CodePoint] |= (String[CodeUnit + 1] & 0x1F) << 6;
                            DecodedString[CodePoint] |= (String[CodeUnit + 2] & 0x1F) << 0;
                            CodeUnit                 += 3;
                            CodePoint                += 1;
                            break;
                            case 4:
                            DecodedString[CodePoint] |= (String[CodeUnit]     & 0x07) << 18;
                            DecodedString[CodePoint] |= (String[CodeUnit + 1] & 0x3F) << 12;
                            DecodedString[CodePoint] |= (String[CodeUnit + 2] & 0x3F) <<  6;
                            DecodedString[CodePoint] |= (String[CodeUnit + 3] & 0x3F) <<  0;
                            CodeUnit                 += 4;
                            CodePoint                += 1;
                            break;
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate DecodedString"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF32 *UTF16_Decode(UTF16 *String) {
        UTF32   *DecodedString                   = NULL;
        if (String != NULL) {
            uint64_t NumCodePoints               = UTF16_GetStringSizeInCodePoints(String) + FoundationIONULLTerminatorSize;
            bool     StringHasBOM                = UTF16_HasBOM(String);
            if (StringHasBOM == No) {
                NumCodePoints                   += 1;
            }
            uint64_t CodePoint                   = 0ULL;
            uint64_t CodeUnit                    = 0ULL;
            UTF16    StringsByteOrder            = 0;
            
            if (String[0] == UTF16BOM_LE || String[0] == UTF16BOM_BE) {
                StringsByteOrder                 = String[0];
            } else {
#if   (FoundationIOTargetByteOrder == FoundationIOByteOrderLE)
                StringsByteOrder                 = UTF16BOM_LE;
#elif (FoundationIOTargetByteOrder == FoundationIOByteOrderBE)
                StringsByteOrder                 = UTF16BOM_BE;
#endif
            }
            DecodedString                        = calloc(NumCodePoints + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (DecodedString != NULL) {
                while (String[CodeUnit] != FoundationIONULLTerminator) {
                    if (String[CodeUnit] <= UTF16MaxCodeUnit && (String[CodeUnit < UTF16HighSurrogateStart] || String[CodeUnit] > UTF16LowSurrogateEnd)) {
                        DecodedString[CodePoint] = String[CodeUnit];
                        CodeUnit                += 1;
                    } else if (String[CodeUnit] >= UTF16HighSurrogateStart || String[CodeUnit] <= UTF16LowSurrogateEnd) {
                        UTF16 HighSurrogate      = String[CodeUnit];
                        UTF16 LowSurrogate       = String[CodeUnit + 1];
                        DecodedString[CodePoint] = (UTF16MaxCodeUnit + 1) + ((HighSurrogate & UTF16SurrogateMask) << UTF16SurrogateShift) | (LowSurrogate & UTF16SurrogateMask);
                        CodeUnit                += 2;
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("DecodedString Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF8 *UTF8_Encode(UTF32 *String) { // 0x000bfdff
        uint64_t CodeUnitNum                           = 0ULL;
        UTF8    *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodePoints            = UTF32_GetStringSizeInCodePoints(String);
            uint64_t UTF8CodeUnits                     = UTF32_GetStringSizeInUTF8CodeUnits(String);
            EncodedString                              = calloc(UTF8CodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF8));
            if (EncodedString != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < StringSizeInCodePoints; CodePoint++) {
                    if (String[CodePoint] <= 0x7F) {
                        EncodedString[CodeUnitNum]     = String[CodePoint] & 0x7F;
                        CodeUnitNum                   += 1;
                    } else if (String[CodePoint] <= 0x7FF) {
                        EncodedString[CodeUnitNum]     = 0xC0 | (String[CodePoint] & ((0x1F << 6) >> 6));
                        EncodedString[CodeUnitNum + 1] = 0x80 | (String[CodePoint] & 0x3F);
                        CodeUnitNum                   += 2;
                    }  else if (String[CodePoint] <= UTF16MaxCodeUnit) {
                        EncodedString[CodeUnitNum]     = 0xE0 | (String[CodePoint] & ((0x0F << 12) >> 12));
                        EncodedString[CodeUnitNum + 1] = 0x80 | (String[CodePoint] & ((0x3F << 6) >> 6));
                        EncodedString[CodeUnitNum + 2] = 0x80 | (String[CodePoint] &   0x3F);
                        CodeUnitNum                   += 3;
                    } else if (String[CodePoint] <= UnicodeMaxCodePoint) { // 0x000bfdff
                        uint8_t CodeUnit1              = (String[CodePoint] & 0x1C0000) >> 18; // 2
                        EncodedString[CodeUnitNum]     = 0xF0 | CodeUnit1; // F2
                        uint8_t CodeUnit2              = (String[CodePoint] & 0x3F000) >> 12; // 0x3F
                        EncodedString[CodeUnitNum + 1] = 0x80 | CodeUnit2; // BF
                        uint8_t CodeUnit3              = (String[CodePoint] & 0xFC0) >> 6; // 0x37
                        EncodedString[CodeUnitNum + 2] = 0x80 | CodeUnit3; // B7
                        uint8_t CodeUnit4              = (String[CodePoint] & 0x3F); // 0x3F
                        EncodedString[CodeUnitNum + 3] = 0x80 | CodeUnit4; // BF
                        CodeUnitNum                   += 4;
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Encoded Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF16 *UTF16_Encode(UTF32 *String) {
        UTF16   *EncodedString                           = NULL;
        if (String != NULL) {
            uint64_t CodePoint                           = 0ULL;
            uint64_t CodeUnit                            = 0ULL;
            uint64_t NumCodeUnits                        = UTF32_GetStringSizeInUTF16CodeUnits(String);
            EncodedString                                = calloc(NumCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF16));
            if (EncodedString != NULL) {
                while (CodeUnit < NumCodeUnits) {
                    if (String[CodePoint] > UTF16MaxCodeUnit) {
                        uint32_t Ranged              = String[CodePoint] - UTF16SurrogatePairStart; // 0xF0731
                        UTF16    HighCodeUnit        = UTF16HighSurrogateStart + (Ranged & (UTF16SurrogateMask << UTF16SurrogateShift) >> UTF16SurrogateShift);
                        UTF16    LowCodeUnit         = UTF16LowSurrogateStart  + (Ranged &  UTF16SurrogateMask);
                        
                        EncodedString[CodeUnit]      = HighCodeUnit;
                        EncodedString[CodeUnit + 1]  = LowCodeUnit;
                        CodeUnit                    += 2;
                    } else {
                        EncodedString[CodeUnit]      = String[CodePoint] & UTF16MaxCodeUnit;
                        CodeUnit                    += 1;
                    }
                    CodePoint                       += 1;
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Encoded Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF8 *UTF16_Convert(UTF16 *String) {
        UTF8 *String8       = NULL;
        if (String != NULL) {
            UTF32 *String32 = UTF16_Decode(String);
            String8         = UTF8_Encode(String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return String8;
    }
    
    UTF16 *UTF8_Convert(UTF8 *String) {
        UTF16 *String16     = NULL;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            String16        = UTF16_Encode(String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return String16;
    }
    
    UTF8 *UTF8_Clone(UTF8 *String) {
        UTF8 *Copy = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodeUnits = UTF8_GetStringSizeInCodeUnits(String);
            Copy    = calloc(StringSizeInCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF8));
            if (Copy != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
                    Copy[CodeUnit] = String[CodeUnit];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Copy Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF16 *UTF16_Clone(UTF16 *String) {
        UTF16 *Copy = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodeUnits = UTF16_GetStringSizeInCodeUnits(String);
            Copy    = calloc(StringSizeInCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF16));
            if (Copy != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
                    Copy[CodeUnit] = String[CodeUnit];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Copy Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF32 *UTF32_Clone(UTF32 *String) {
        UTF32 *Copy = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodePoints = UTF32_GetStringSizeInCodePoints(String);
            Copy    = calloc(StringSizeInCodePoints + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (Copy != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < StringSizeInCodePoints; CodePoint++) {
                    Copy[CodePoint] = String[CodePoint];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Copy Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Copy;
    }
    
    void UTF8_Erase(UTF8 *String) {
        if (String != NULL) {
            uint64_t StringSize  = UTF8_GetStringSizeInCodePoints(String);
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                String[CodeUnit] = 0;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void UTF16_Erase(UTF16 *String) {
        if (String != NULL) {
            uint64_t StringSize  = UTF16_GetStringSizeInCodePoints(String);
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                String[CodeUnit] = 0;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void UTF32_Erase(UTF32 *String) {
        if (String != NULL) {
            uint64_t StringSize  = UTF32_GetStringSizeInCodePoints(String);
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                String[CodeUnit] = 0;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
    }
    
    UTF8 *UTF8_Truncate(UTF8 *String, uint64_t MaxCodeUnits) {
        UTF8 *Truncated = NULL;
        if (String != NULL) {
            uint64_t StringSize     = UTF8_GetStringSizeInCodeUnits(String);
            uint64_t CodeUnitOffset = MaxCodeUnits;
            uint64_t CodeUnit       = 0ULL;
            if (MaxCodeUnits <= StringSize) {
                CodeUnitOffset     -= UTF8_GetCodePointSizeInCodeUnits(String[MaxCodeUnits]);
                Truncated           = calloc(CodeUnitOffset + FoundationIONULLTerminatorSize, sizeof(UTF8));
                if (Truncated != NULL) {
                    while (CodeUnit < MaxCodeUnits) {
                        Truncated[CodeUnit]  = String[CodeUnit];
                        CodeUnit            += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    UTF16 *UTF16_Truncate(UTF16 *String, uint64_t MaxCodeUnits) {
        UTF16 *Truncated = NULL;
        if (String != NULL) {
            uint64_t StringSize     = UTF16_GetStringSizeInCodeUnits(String);
            uint64_t CodeUnitOffset = MaxCodeUnits;
            uint64_t CodeUnit       = 0ULL;
            if (MaxCodeUnits <= StringSize) {
                CodeUnitOffset     -= UTF16_GetCodePointSizeInCodeUnits(String[MaxCodeUnits]);
                Truncated           = calloc(CodeUnitOffset + FoundationIONULLTerminatorSize, sizeof(UTF16));
                if (Truncated != NULL) {
                    while (CodeUnit < MaxCodeUnits) {
                        Truncated[CodeUnit]  = String[CodeUnit];
                        CodeUnit            += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    UTF32 *UTF32_Truncate(UTF32 *String, uint64_t MaxCodePoints) {
        UTF32 *Truncated = NULL;
        if (String != NULL) {
            uint64_t StringSize      = UTF32_GetStringSizeInCodePoints(String);
            uint64_t CodePointOffset = MaxCodePoints;
            uint64_t CodePoint       = 0ULL;
            if (MaxCodePoints <= StringSize) {
                Truncated            = calloc(CodePointOffset + FoundationIONULLTerminatorSize, sizeof(UTF16));
                if (Truncated != NULL) {
                    while (CodePoint < MaxCodePoints) {
                        Truncated[CodePoint]  = String[CodePoint];
                        CodePoint            += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    UTF8 *UTF8_ReadGraphemeFromFile(FILE *Source) {
        UTF8 *Grapheme                         = NULL;
        if (Source != NULL) {
            bool     GraphemeFound             = No;
            uint64_t CodePointSizeInCodeUnits  = 0ULL;
            uint64_t GraphemeSizeInCodePoints  = 0ULL;
            UTF8     CodeUnit                  = 0;
            /*
             While loop reading bytes until wehave a grapheme
             */
            while (GraphemeFound == No) {
                fread(&CodeUnit, sizeof(UTF8), 1, Source);
                CodePointSizeInCodeUnits     += UTF8_GetCodePointSizeInCodeUnits(CodeUnit);
                // Now back up, read the CodePoint, decode it, check to see if it's a Grapheme Extension codepoint
                fseek(Source, CodePointSizeInCodeUnits, SEEK_CUR);
                Grapheme          = calloc(CodePointSizeInCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF8));
                // now use fread to read that many bytes
                fread(&Grapheme, sizeof(UTF8), CodePointSizeInCodeUnits, Source);
                // Now decode
                UTF32 *CodePoint  = UTF8_Decode(Grapheme);
                for (uint64_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                    if (CodePoint[0] == GraphemeExtensionTable[GraphemeExtension]) {
                        // Read another CodePoint
                    } else if (CodePoint[0] != GraphemeExtensionTable[GraphemeExtension] && GraphemeExtension == GraphemeExtensionTableSize - 1) {
                        GraphemeFound = Yes;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF16 *UTF16_ReadGraphemeFromFile(FILE *Source) {
        UTF16 *CodePoint          = NULL;
        if (Source != NULL) {
            UTF16 CodeUnit        = 0;
            fread(&CodeUnit, sizeof(UTF16), 1, Source);
            uint8_t CodePointSize = UTF16_GetCodePointSizeInCodeUnits(CodeUnit);
            CodePoint             = calloc(CodePointSize + FoundationIONULLTerminatorSize, sizeof(UTF16));
            fread(CodePoint, CodePointSize, 1, Source);
        } else {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
        return CodePoint;
    }
    
    void UTF8_WriteGrapheme(FILE *Source, UTF8 *CodePoint) { // Replaces fputc and putc
        if (Source != NULL) {
            uint64_t StringSize = UTF8_GetStringSizeInCodeUnits(CodePoint);
            fwrite(CodePoint, StringSize, 1, Source);
        } else {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_WriteGrapheme(FILE *Source, UTF16 *CodePoint) { // replaces fputwc and putwc
        if (Source != NULL) {
            uint64_t StringSize = UTF16_GetStringSizeInCodeUnits(CodePoint);
            fwrite(CodePoint, StringSize, 1, Source);
        } else {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    UTF8 *UTF8_ReadLine(FILE *Source) { // Replaces Fgets
        UTF8 *Line = NULL;
        if (Source != NULL) {
            // So we need to know the size of the string to get, so count the number of codeunits that don't match any line ending.
            uint64_t StringSizeInCodeUnits  = 0ULL;
            uint64_t StringSizeInCodePoints = 0ULL;
            UTF32   *CurrentCodePoint       = 1UL;
            while (CurrentCodePoint[0] != U32('\n') || CurrentCodePoint[0] != FoundationIONULLTerminator) {
                /*
                 Loop reading a codepoint each time until we find one that is a new line character.
                 */
                StringSizeInCodePoints     += 1;
                UTF8 *CodePoint             = UTF8_ReadGraphemeFromFile(Source);
                CurrentCodePoint            = UTF8_Decode(CodePoint);
            } // FIXME: Use our newline array
            // Now we need to allocate memory for that string
            Line                            = calloc(StringSizeInCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF8));
        } else {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
        return Line;
    }
    
    UTF16 *UTF16_ReadLine(FILE *Source) { // Replaces Fgetws
        UTF16 *Line = NULL;
        if (Source != NULL) {
            // So we need to know the size of the string to get, so count the number of codeunits that don't match any line ending.
            uint64_t StringSizeInCodeUnits  = 0ULL;
            uint64_t StringSizeInCodePoints = 0ULL;
            UTF32   *CurrentCodePoint       = 1UL;
            while (CurrentCodePoint[0] != U32('\n') || CurrentCodePoint[0] != FoundationIONULLTerminator) {
                /*
                 Loop reading a codepoint each time until we find one that is a new line character.
                 */
                StringSizeInCodePoints     += 1;
                UTF16 *CodePoint            = UTF16_ReadGraphemeFromFile(Source);
                CurrentCodePoint            = UTF16_Decode(CodePoint);
            }
            // Now we need to allocate memory for that string
            Line                            = calloc(StringSizeInCodeUnits + FoundationIONULLTerminatorSize, sizeof(UTF16));
        } else {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
        return Line;
    }
    
    void UTF8_WriteLine(FILE *OutputFile, UTF8 *String) { // Replaces Fputs and puts
        if (String != NULL && OutputFile != NULL) {
            int8_t   StreamMode        = (int8_t) fwide(OutputFile, 0);
            uint64_t StringSize        = UTF8_GetStringSizeInCodeUnits(String);
            uint64_t CodeUnitsWritten  = 0ULL;
            bool     StringHasNewLine  = UTF8_HasNewLine(String);
            if (StreamMode < 0) { // UTF-8
                CodeUnitsWritten       = fwrite(String, StringSize, sizeof(UTF8), OutputFile);
                if (StringHasNewLine == No) {
                    fwrite(FoundationIONewLine8, FoundationIONewLine8Size, 1, OutputFile);
                }
                if (CodeUnitsWritten != StringSize) {
                    Log(Log_DEBUG, __func__, U8("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (StreamMode > 0) { // UTF-16
                UTF32 *String32        = UTF8_Decode(String);
                UTF16 *String16        = UTF16_Encode(String32);
                free(String32);
                fwrite(String16, StringSize, sizeof(UTF16), OutputFile);
                free(String16);
                if (StringHasNewLine == No) {
                    fwrite(FoundationIONewLine16, FoundationIONewLine16Size, 1, OutputFile);
                }
                if (CodeUnitsWritten != StringSize) {
                    Log(Log_DEBUG, __func__, U8("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_WriteLine(FILE *OutputFile, UTF16 *String) { // Replaces Fputws and putws
        if (String != NULL && OutputFile != NULL) {
            int8_t   StreamMode        = (int8_t) fwide(OutputFile, 0);
            uint64_t StringSize        = UTF16_GetStringSizeInCodeUnits(String);
            uint64_t CodeUnitsWritten  = 0ULL;
            bool     StringHasNewLine  = UTF16_HasNewLine(String);
            if (StreamMode > 0) { // UTF-16
                CodeUnitsWritten       = fwrite(String, StringSize, sizeof(UTF16), OutputFile);
                if (StringHasNewLine == No) {
                    fwrite(FoundationIONewLine16, FoundationIONewLine16Size, 1, OutputFile);
                }
                if (CodeUnitsWritten != StringSize) {
                    Log(Log_DEBUG, __func__, U8("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (StreamMode < 0) { // UTF-8
                UTF32 *String32        = UTF16_Decode(String);
                UTF8  *String8         = UTF8_Encode(String32);
                free(String32);
                CodeUnitsWritten       = fwrite(String8, StringSize, sizeof(UTF8), OutputFile);
                free(String8);
                if (StringHasNewLine == No) {
                    fwrite(FoundationIONewLine16, FoundationIONewLine16Size, 1, OutputFile);
                }
                if (CodeUnitsWritten != StringSize) {
                    Log(Log_DEBUG, __func__, U8("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_DEBUG, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    UTF8 *UTF8_Reverse(UTF8 *String) {
        UTF8 *Reversed        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Reversed32 = UTF32_Reverse(String32);
            free(String32);
            Reversed          = UTF8_Encode(Reversed32);
            free(Reversed32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Reversed;
    }
    
    UTF16 *UTF16_Reverse(UTF16 *String) {
        UTF16 *Reversed = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Reversed32 = UTF32_Reverse(String32);
            free(String32);
            Reversed          = UTF16_Encode(Reversed32);
            free(Reversed32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Reversed;
    }
    
    UTF32 *UTF32_Reverse(UTF32 *String) {
        UTF32 *Reverse = NULL;
        if (String != NULL) {
            // Get the size of the string
            // Copy from back to front
            uint64_t StringSize = UTF32_GetStringSizeInGraphemes(String); // We need a function that will allow us to copy each Grapheme
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Reverse;
    }
    
    int64_t UTF8_FindSubString(UTF8 *String, UTF8 *SubString, uint64_t Offset, int64_t Length) {
        int64_t FoundOffset    = 0LL;
        if (String != NULL && SubString != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *SubString32 = UTF8_Decode(SubString);
            FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
            free(String32);
            free(SubString32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_DEBUG, __func__, U8("SubString Pointer is NULL"));
        }
        return FoundOffset;
    }
    
    int64_t UTF16_FindSubString(UTF16 *String, UTF16 *SubString, uint64_t Offset, int64_t Length) {
        int64_t FoundOffset = 0LL;
        if (String != NULL && SubString != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *SubString32 = UTF16_Decode(SubString);
            FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
            free(String32);
            free(SubString32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_DEBUG, __func__, U8("SubString Pointer is NULL"));
        }
        return FoundOffset;
    }
    
    int64_t UTF32_FindSubString(UTF32 *String, UTF32 *SubString, uint64_t Offset, int64_t Length) {
        uint64_t StringSize            = UTF32_GetStringSizeInCodePoints(String);
        uint64_t SubStringSize         = UTF32_GetStringSizeInCodePoints(SubString);
        int64_t  MatchingOffset        = 0LL;
        if (String != NULL && SubString != NULL && StringSize < Offset + Length) {
            for (uint64_t SubCodePoint = 0ULL; SubCodePoint < SubStringSize; SubCodePoint++) {
                for (uint64_t StringCodePoint = Offset; StringCodePoint < Offset + Length; StringCodePoint++) {
                    if (String[StringCodePoint] != SubString[SubCodePoint]) {
                        MatchingOffset = -1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_DEBUG, __func__, U8("SubString Pointer is NULL"));
        } else if (Length >= 1 && StringSize < Offset + Length) {
            Log(Log_DEBUG, __func__, U8("Offset %llu + Length %lld is larger than String %llu"), Offset, Length, StringSize);
        }
        return MatchingOffset;
    }
    
    UTF8 *UTF8_ExtractSubString(UTF8 *String, uint64_t Offset, uint64_t Length) {
        UTF8 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, Length);
            ExtractedSubString = UTF8_Encode(Extracted32);
            free(String32);
            free(Extracted32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF16 *UTF16_ExtractSubString(UTF16 *String, uint64_t Offset, uint64_t Length) {
        UTF16 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, Length);
            ExtractedSubString = UTF16_Encode(Extracted32);
            free(String32);
            free(Extracted32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF32 *UTF32_ExtractSubString(UTF32 *String, uint64_t Offset, uint64_t Length) {
        uint64_t  StringSize                            = UTF32_GetStringSizeInCodePoints(String);
        uint64_t  ExtractedStringSize                   = Length + FoundationIONULLTerminatorSize;
        UTF32    *ExtractedString                       = NULL;
        if (String != NULL && StringSize >= Length + Offset) {
            ExtractedString                             = calloc(ExtractedStringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (ExtractedString != NULL) {
                for (uint64_t CodePoint = Offset; CodePoint < Offset + Length; CodePoint++) {
                    ExtractedString[CodePoint - Offset] = String[CodePoint];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate ExtractedString"));
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (StringSize < Length + Offset) {
            Log(Log_DEBUG, __func__, U8("Offset %llu + Length %lld is larger than String %llu"), Length, Offset, StringSize);
        }
        return ExtractedString;
    }
    
    UTF8  *UTF8_ReplaceSubString(UTF8 *String, UTF8 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF8 *Replaced8          = NULL;
        if (String != NULL && Replacement != NULL) {
            UTF32 *String32      = UTF8_Decode(String);
            UTF32 *Replacement32 = UTF8_Decode(Replacement);
            UTF32 *Replaced32    = UTF32_ReplaceSubString(String32, Replacement32, Offset, Length);
            Replaced8            = UTF8_Encode(Replaced32);
            free(String32);
            free(Replacement32);
            free(Replaced32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_DEBUG, __func__, U8("Replacement Pointer is NULL"));
        }
        return Replaced8;
    }
    
    UTF16 *UTF16_ReplaceSubString(UTF16 *String, UTF16 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF16 *Replaced16        = NULL;
        if (String != NULL && Replacement != NULL) {
            UTF32 *String32      = UTF16_Decode(String);
            UTF32 *Replacement32 = UTF16_Decode(Replacement);
            UTF32 *Replaced32    = UTF32_ReplaceSubString(String32, Replacement32, Offset, Length);
            Replaced16           = UTF16_Encode(Replaced32);
            free(String32);
            free(Replacement32);
            free(Replaced32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_DEBUG, __func__, U8("Replacement Pointer is NULL"));
        }
        return Replaced16;
    }
    
    UTF32 *UTF32_ReplaceSubString(UTF32 *String, UTF32 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF32 *NewString                          = NULL;
        if (String != NULL && Replacement != NULL) {
            uint64_t StringSize                   = UTF32_GetStringSizeInCodePoints(String);
            uint64_t ReplacementSize              = UTF32_GetStringSizeInCodePoints(Replacement);
            uint64_t NewStringSize                = (StringSize + ReplacementSize) - Length;
            NewString                             = calloc(NewStringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (NewString != NULL) {
                uint64_t CodePoint                = 0ULL;
                uint64_t ReplacementCodePoint     = 0ULL;
                while (CodePoint < NewStringSize) {
                    
                    /*
                     Shorter:
                     String1      = "NumArgs = %llu, Invalid"
                     Replacement1 = "1"
                     Offset       = 11
                     Output       = ""
                     
                     Equal:
                     String2      = "NumArgs = %llu, Invalid"
                     Replacement2 = "1234"
                     Offset       = 11
                     Output       = ""
                     
                     Longer:
                     String3      = "NumArgs = %llu, Invalid"
                     Replacement3 = "12345"
                     Offset       = 11
                     Output       = "NumArgs = 12345, Invalid"
                     */
                    
                    if (CodePoint < Offset) {
                        NewString[CodePoint]      = String[CodePoint];
                        CodePoint                += 1;
                    } else if (CodePoint >= Offset && CodePoint < Offset + Minimum(ReplacementSize, Length)) {
                        while(Replacement[ReplacementCodePoint] != FoundationIONULLTerminator) {
                            NewString[CodePoint]  = Replacement[ReplacementCodePoint];
                            CodePoint            += 1;
                            ReplacementCodePoint += 1;
                        }
                    } else {
                        NewString[CodePoint]      = String[CodePoint + Minimum(ReplacementSize, Length)];
                        CodePoint                += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_DEBUG, __func__, U8("Replacement Pointer is NULL"));
        }
        return NewString;
    }
    
    UTF8 *UTF8_RemoveSubString(UTF8 *String, UTF8 *SubString2Remove, uint64_t Instance2Remove) {
        UTF8 *TrimmedString         = NULL;
        if (String != NULL && SubString2Remove != NULL) {
            UTF32 *DecodedString    = UTF8_Decode(String);
            UTF32 *DecodedSubString = UTF8_Decode(SubString2Remove);
            UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
            TrimmedString           = UTF8_Encode(Trimmed32);
            free(DecodedString);
            free(DecodedSubString);
            free(Trimmed32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_DEBUG, __func__, U8("SubString2Remove Pointer is NULL"));
        }
        return TrimmedString;
    }
    
    UTF16 *UTF16_RemoveSubString(UTF16 *String, UTF16 *SubString2Remove, uint64_t Instance2Remove) {
        UTF16 *TrimmedString        = NULL;
        if (String != NULL && SubString2Remove != NULL) {
            UTF32 *DecodedString    = UTF16_Decode(String);
            UTF32 *DecodedSubString = UTF16_Decode(SubString2Remove);
            UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
            TrimmedString           = UTF16_Encode(Trimmed32);
            free(DecodedString);
            free(DecodedSubString);
            free(Trimmed32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_DEBUG, __func__, U8("SubString2Remove Pointer is NULL"));
        }
        return TrimmedString;
    }
    
    UTF32 *UTF32_RemoveSubString(UTF32 *String, UTF32 *SubString2Remove, uint64_t Instance2Remove) {
        UTF32 *EditedString = NULL;
        if (String != NULL && SubString2Remove != NULL && Instance2Remove > 0) {
            uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(String);
            uint64_t SubStringSize    = UTF32_GetStringSizeInCodePoints(SubString2Remove);
            uint64_t EditedStringSize = 0ULL;
            
            uint64_t   NumInstances   = 0ULL;
            uint64_t  *Instances      = NULL;
            
            for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                for (uint64_t SubStringCodePoint = 0ULL; SubStringCodePoint < SubStringSize; SubStringCodePoint++) {
                    if (String[StringCodePoint] != SubString2Remove[SubStringCodePoint]) {
                        break;
                    } else {
                        NumInstances += 1;
                    }
                }
            }
            
            Instances = calloc(NumInstances, sizeof(uint64_t));
            if (Instances != NULL) {
                for (uint64_t Instance = 0ULL; Instance < NumInstances; Instance++) {
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        for (uint64_t RemoveCodePoint = 0ULL; RemoveCodePoint < SubStringSize; RemoveCodePoint++) {
                            if (String[StringCodePoint] != SubString2Remove[RemoveCodePoint]) {
                                break;
                            } else {
                                Instances[Instance] = StringCodePoint;
                            }
                        }
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate the instances"));
            }
            
            if (Instance2Remove >= 1) {
                EditedStringSize = StringSize - SubStringSize;
            } else if (Instance2Remove == 0xFFFFFFFFFFFFFFFF) {
                EditedStringSize = StringSize - (NumInstances * SubStringSize);
            }
            
            EditedString         = calloc(EditedStringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (EditedString != NULL) {
                for (uint64_t EditedCodePoint = 0ULL; EditedCodePoint < EditedStringSize; EditedCodePoint++) {
                    for (uint64_t Instance = 0ULL; Instance < NumInstances; Instance++) {
                        if (EditedCodePoint < Instances[Instance] || EditedCodePoint > Instances[Instance] + SubStringSize) {
                            EditedString[EditedCodePoint] = String[EditedCodePoint - (Instances[Instance] + SubStringSize)];
                        }
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate the edited string"));
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_DEBUG, __func__, U8("SubString2Remove Pointer is NULL"));
        } else if (Instance2Remove == 0) {
            Log(Log_DEBUG, __func__, U8("Removing 0 instances of the SubString does not make sense"));
        }
        return EditedString;
    }
    
    UTF8 *UTF8_Insert(UTF8 *String, UTF8 *String2Insert, uint64_t Offset) {
        UTF8 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Insert32   = UTF8_Decode(String2Insert);
            UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
            free(String32);
            free(Insert32);
            Inserted          = UTF8_Encode(Inserted32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF16 *UTF16_Insert(UTF16 *String, UTF16 *String2Insert, uint64_t Offset) {
        UTF16 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Insert32   = UTF16_Decode(String2Insert);
            UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
            free(String32);
            free(Insert32);
            Inserted          = UTF16_Encode(Inserted32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF32 *UTF32_Insert(UTF32 *String, UTF32 *String2Insert, uint64_t Offset) {
        UTF32 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            uint64_t InsertSize = UTF32_GetStringSizeInCodePoints(String2Insert);
            if (Offset == 0xFFFFFFFFFFFFFFFF) {
                Offset          = StringSize;
            }
            if (Offset <= StringSize) {
                Inserted        = calloc(StringSize + InsertSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
                if (Inserted != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize + InsertSize; CodePoint++) {
                        if (CodePoint < Offset || CodePoint > Offset + InsertSize) {
                            Inserted[CodePoint] = String[CodePoint];
                        } else {
                            Inserted[CodePoint] = String2Insert[CodePoint - StringSize];
                        }
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("Couldn't allocate InsertedString"));
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Offset %llu is greater than the string's size %llu"), Offset, StringSize);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF8 *UTF8_CaseFold(UTF8 *String) {
        UTF8 *CaseFolded      = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *CaseFold32 = UTF32_CaseFold(String32);
            free(String32);
            CaseFolded        = UTF8_Encode(CaseFold32);
            free(CaseFold32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return CaseFolded;
    }
    
    UTF16 *UTF16_CaseFold(UTF16 *String) {
        UTF16 *CaseFolded     = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *CaseFold32 = UTF32_CaseFold(String32);
            free(String32);
            CaseFolded        = UTF16_Encode(CaseFold32);
            free(CaseFold32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return CaseFolded;
    }
    
    UTF32 *UTF32_CaseFold(UTF32 *String) {
        uint64_t CodePoint        = 0ULL;
        UTF32   *CaseFoldedString = NULL;
        if (String != NULL) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                for (uint64_t Index = 0ULL; Index < CaseFoldTableSize; Index++) {
                    if (String[CodePoint] == CaseFoldCodePoints[Index]) {
                        uint64_t ReplacementSize = UTF32_GetStringSizeInCodePoints(&CaseFoldedString[Index]);
                        CaseFoldedString         = UTF32_ReplaceSubString(String, CaseFoldStrings[Index], CodePoint, ReplacementSize);
                    }
                }
                CodePoint += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return CaseFoldedString;
    }
    
    typedef enum CombiningCharacterClasses {
        CCCOverlay                           = 1,
        CCCNukta                             = 7,
        CCCKanaVoicing                       = 8,
        CCCVirama                            = 9,
        CCC10                                = 10,
        CCC11                                = 11,
        CCC12                                = 12,
        CCC13                                = 13,
        CCC14                                = 14,
        CCC15                                = 15,
        CCC16                                = 16,
        CCC17                                = 17,
        CCC18                                = 18,
        CCC19                                = 19,
        CCC20                                = 20,
        CCC21                                = 21,
        CCC22                                = 22,
        CCC23                                = 23,
        CCC24                                = 24,
        CCC25                                = 25,
        CCC26                                = 26,
        CCC27                                = 27,
        CCC28                                = 28,
        CCC29                                = 29,
        CCC30                                = 30,
        CCC31                                = 31,
        CCC32                                = 32,
        CCC33                                = 33,
        CCC34                                = 34,
        CCC35                                = 35,
        CCC36                                = 36,
        CCC84                                = 84,
        CCC91                                = 91,
        CCC103                               = 103,
        CCC107                               = 107,
        CCC118                               = 118,
        CCC122                               = 122,
        CCC129                               = 129,
        CCC130                               = 130,
        CCC132                               = 132,
        CCCAttachBelowLeft                   = 200,
        CCCAttachBelow                       = 202,
        CCCAttachAbove                       = 214,
        CCCAttachAboveRight                  = 216,
        CCCAttachBelowLeft2                  = 218,
        CCCAttachBelow2                      = 220,
        CCCAttachBelowRight2                 = 222,
        CCCAttachLeft                        = 224,
        CCCAttachRight                       = 226,
        CCCAttachAboveLeft2                  = 228,
        CCCAttachAbove2                      = 230,
        CCCAttachAboveRight2                 = 232,
        CCCAttachDoubleBelow                 = 233,
        CCCAttachDoubleAbove                 = 234,
        CCCAttachIOTASubscript               = 240,
    } CombiningCharacterClasses;
    
    static UTF32 *UTF32_Reorder(UTF32 *String) { // Stable sort
        uint64_t CodePoint  = 1ULL;
        uint32_t CodePointA = 0UL;
        uint32_t CodePointB = 0UL;
        if (String != NULL) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                CodePointA = String[CodePoint - 1];
                CodePointB = String[CodePoint];
                for (uint64_t IndexA = 0ULL; IndexA < CombiningCharacterClassTableSize; IndexA++) {
                    for (uint64_t IndexB = 0ULL; IndexB < CombiningCharacterClassTableSize; IndexB++) {
                        if (CodePointA == CombiningCharacterClassTable[IndexA][0] && CodePointB == CombiningCharacterClassTable[IndexB][0]) {
                            if (CombiningCharacterClassTable[IndexA][1] > CombiningCharacterClassTable[IndexB][1]) {
                                String[CodePoint - 1] = CodePointB;
                                String[CodePoint]     = CodePointA;
                            }
                        }
                    }
                }
                CodePoint += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NULL;
    }
    
    static UTF32 *UTF32_Compose(UTF32 *String, bool Kompatibility) { // FIXME: Must use a stable sorting algorithm
        uint64_t CodePoint      = 0ULL;
        UTF32   *ComposedString = NULL;
        if (String != NULL && (Kompatibility == No || Kompatibility == Yes)) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (Kompatibility == Yes) {
                    for (uint64_t Index = 0ULL; Index < KompatibleNormalizationTableSize; Index++) {
                        if (String[CodePoint] == KompatibleNormalizationCodePoints[Index]) {
                            ComposedString = UTF32_ReplaceSubString(String, KompatibleNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                } else {
                    for (uint64_t DecomposeCodePoint = 0ULL; DecomposeCodePoint < CanonicalNormalizationTableSize; DecomposeCodePoint++) {
                        if (String[CodePoint] == CanonicalNormalizationCodePoints[DecomposeCodePoint]) {
                            ComposedString = UTF32_ReplaceSubString(String, CanonicalNormalizationStrings[DecomposeCodePoint], CodePoint, 1);
                        }
                    }
                }
                CodePoint += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return ComposedString;
    }
    
    static UTF32 *UTF32_Decompose(UTF32 *String, bool Kompatibility) { // FIXME: Must use a stable sorting algorithm
        uint64_t CodePoint             = 0ULL;
        UTF32   *DecomposedString      = NULL;
        if (String != NULL && (Kompatibility == No || Kompatibility == Yes)) {
            UTF32 *Decomposed          = NULL;
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (Kompatibility == Yes) {
                    for (uint64_t Index = 0ULL; Index < KompatibleNormalizationTableSize; Index++) {
                        if (String[CodePoint] == KompatibleNormalizationCodePoints[Index]) {
                            Decomposed = UTF32_ReplaceSubString(String, KompatibleNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                } else {
                    for (uint64_t Index = 0ULL; Index < CanonicalNormalizationTableSize; Index++) {
                        if (String[CodePoint] == CanonicalNormalizationCodePoints[Index]) {
                            Decomposed = UTF32_ReplaceSubString(String, CanonicalNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                }
                CodePoint             += 1;
            }
            DecomposedString           = UTF32_Reorder(Decomposed);
            free(Decomposed);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return DecomposedString;
    }
    
    UTF8 *UTF8_Normalize(UTF8 *String, StringIONormalizationForms NormalizedForm) {
        UTF8 *NormalizedString8       = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unknown) {
            UTF32 *String32           = UTF8_Decode(String);
            UTF32 *NormalizedString32 = UTF32_Normalize(String32, NormalizedForm);
            NormalizedString8         = UTF8_Encode(NormalizedString32);
            free(String32);
            free(NormalizedString32);
        }
        return NormalizedString8;
    }
    
    UTF16 *UTF16_Normalize(UTF16 *String, StringIONormalizationForms NormalizedForm) {
        UTF16 *NormalizedString16     = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unknown) {
            UTF32 *String32           = UTF16_Decode(String);
            UTF32 *NormalizedString32 = UTF32_Normalize(String32, NormalizedForm);
            NormalizedString16        = UTF16_Encode(NormalizedString32);
            free(String32);
            free(NormalizedString32);
        }
        return NormalizedString16;
    }
    
    UTF32 *UTF32_Normalize(UTF32 *String, StringIONormalizationForms NormalizedForm) {
        UTF32 *NormalizedString = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unknown) {
            if (NormalizedForm == NormalizationForm_CanonicalCompose) {
                UTF32 *Decomposed = UTF32_Decompose(String, No);
                NormalizedString  = UTF32_Compose(Decomposed, No);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationForm_KompatibleCompose) {
                UTF32 *Decomposed = UTF32_Decompose(String, Yes);
                NormalizedString  = UTF32_Compose(Decomposed, Yes);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationForm_CanonicalDecompose) {
                NormalizedString  = UTF32_Decompose(String, No);
            } else if (NormalizedForm == NormalizationForm_KompatibleDecompose) {
                NormalizedString  = UTF32_Decompose(String, Yes);
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (NormalizedForm == NormalizationForm_Unknown) {
            Log(Log_DEBUG, __func__, U8("Unknown Normalization form"));
        }
        return NormalizedString;
    }
    
#define StringIOIntegerTableBase2Size        2
#define StringIOIntegerTableBase8Size        8
#define StringIOIntegerTableBase10Size      10
#define StringIOIntegerTableBase16Size      16
#define StringIODecimalTableSize            11
#define StringIODecimalTableScientificSize  14
#define StringIODecimalTableHexadecimalSize 21
    
    
    static const UTF32 StringIOIntegerTableBase2[StringIOIntegerTableBase2Size] = {
        U32('0'), U32('1')
    };
    
    static const UTF32 StringIOIntegerTableBase8[StringIOIntegerTableBase8Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7')
    };
    
    static const UTF32 StringIOIntegerTableBase10[StringIOIntegerTableBase10Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9')
    };
    
    static const UTF32 StringIOIntegerTableUppercaseBase16[StringIOIntegerTableBase16Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'), U32('E'), U32('F')
    };
    
    static const UTF32 StringIOIntegerTableLowercaseBase16[StringIOIntegerTableBase16Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'), U32('e'), U32('f')
    };
    
    static const UTF32 StringIODecimalTable[StringIODecimalTableSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('.')
    };
    
    static const UTF32 StringIODecimalScientificUppercase[StringIODecimalTableScientificSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('E'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 StringIODecimalScientificLowercase[StringIODecimalTableScientificSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('e'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 StringIODecimalHexUppercase[StringIODecimalTableHexadecimalSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'),
        U32('E'), U32('F'), U32('P'), U32('X'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 StringIODecimalHexLowercase[StringIODecimalTableHexadecimalSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'),
        U32('e'), U32('f'), U32('p'), U32('x'), U32('.'), U32('+'), U32('-')
    };
    
    int64_t UTF8_String2Integer(StringIOBases Base, UTF8 *String) { // Replaces atoi, atol, strtol, strtoul,
        int64_t Value = 0LL;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            Value           = UTF32_String2Integer(Base, String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Value;
    }
    
    int64_t UTF16_String2Integer(StringIOBases Base, UTF16 *String) {
        int64_t Value = 0LL;
        if (String != NULL) {
            UTF32 *String32 = UTF16_Decode(String);
            Value           = UTF32_String2Integer(Base, String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Value;
    }
    
    int64_t UTF32_String2Integer(StringIOBases Base, UTF32 *String) {
        uint64_t CodePoint = 0ULL;
        int8_t   Sign      = 1;
        int64_t  Value     = 0LL;
        if (String != NULL && (Base & Decimal) != Decimal) {
            if ((Base & Integer) == Integer) {
                if ((Base & Base2) == Base2) {
                    while (String[CodePoint] != FoundationIONULLTerminator) {
                        for (uint8_t Digit = 0; Digit < StringIOIntegerTableBase2Size; Digit++) {
                            if (String[CodePoint] == StringIOIntegerTableBase2[Digit]) {
                                if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x31) {
                                    Value <<= 1;
                                    Value  += String[CodePoint] - 0x30;
                                }
                            }
                        }
                        CodePoint          += 1;
                    }
                } else if ((Base & Base8) == Base8) {
                    while (String[CodePoint] != FoundationIONULLTerminator) {
                        for (uint8_t Digit = 0; Digit < StringIOIntegerTableBase8Size; Digit++) {
                            if (String[CodePoint] == StringIOIntegerTableBase8[Digit]) {
                                if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x37) {
                                    Value  *= 8;
                                    Value  += String[CodePoint] - 0x30;
                                }
                            }
                        }
                        CodePoint          += 1;
                    }
                } else if ((Base & Base10) == Base10) {
                    while (String[CodePoint] != FoundationIONULLTerminator) {
                        for (uint8_t Digit = 0; Digit < StringIOIntegerTableBase10Size; Digit++) {
                            if (CodePoint == 0 && String[CodePoint] == U32('-')) {
                                Sign    = -1;
                            } else if (String[CodePoint] == StringIOIntegerTableBase10[Digit]) {
                                Value  *= 10;
                                Value  += String[CodePoint] - 0x30;
                            }
                        }
                        CodePoint      += 1;
                    }
                } else if ((Base & Base16) == Base16) {
                    if ((Base & Uppercase) == Uppercase) {
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t Digit = 0; Digit < StringIOIntegerTableBase16Size; Digit++) {
                                if (String[CodePoint] == StringIOIntegerTableUppercaseBase16[Digit]) {
                                    if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39) {
                                        Value <<= 4;
                                        Value  += String[CodePoint] - 0x30;
                                    } else if (String[CodePoint] >= 0x41 && String[CodePoint] <= 0x46) {
                                        Value <<= 4;
                                        Value  += String[CodePoint] - 0x37;
                                    }
                                }
                            }
                            CodePoint          += 1;
                        }
                    } else if ((Base & Lowercase) == Lowercase) {
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t Digit = 0; Digit < StringIOIntegerTableBase16Size; Digit++) {
                                if (String[CodePoint] == StringIOIntegerTableLowercaseBase16[Digit]) {
                                    if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39) {
                                        Value <<= 4;
                                        Value  += String[CodePoint] - 0x30;
                                    } else if (String[CodePoint] >= 0x61 && String[CodePoint] <= 0x66) {
                                        Value <<= 4;
                                        Value  += String[CodePoint] - 0x51;
                                    }
                                }
                            }
                            CodePoint          += 1;
                        }
                    }
                }
            }
            Value                              *= Sign;
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Base == Decimal) {
            Log(Log_DEBUG, __func__, U8("Decimals are invalid input for this function"));
        }
        return Value;
    }
    
    UTF8 *UTF8_Integer2String(StringIOBases Base, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF8  *IntegerString8  = UTF8_Encode(IntegerString32);
        free(IntegerString32);
        return IntegerString8;
    }
    
    UTF16 *UTF16_Integer2String(StringIOBases Base, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF16 *IntegerString16 = UTF16_Encode(IntegerString32);
        free(IntegerString32);
        return IntegerString16;
    }
    
    UTF32 *UTF32_Integer2String(StringIOBases Base, int64_t Integer2Convert) {
        UTF32   *String               = NULL;
        int64_t  Sign                 = 0LL;
        uint64_t Num                  = AbsoluteI(Integer2Convert);
        uint8_t  Radix                = 0;
        uint8_t  NumDigits            = 0;
        
        if ((Base & Integer) == Integer) {
            if ((Integer2Convert & 0x8000000000000000) == 0x8000000000000000) { // Signed
                Sign                      = -1;
                NumDigits                +=  1;
            }
            
            if ((Base & Base2) == Base2) {
                Radix                     = 2;
            } else if ((Base & Base8) == Base8) {
                Radix                     = 8;
            } else if ((Base & Base10) == Base10) {
                Radix                     = 10;
            } else if ((Base & Base16) == Base16) {
                Radix                     = 16;
            }
            NumDigits                    += NumDigitsInInteger(Radix, Integer2Convert);
            
            String                        = calloc(NumDigits + FoundationIONULLTerminatorSize, sizeof(UTF32));
            
            if (String != NULL) {
                for (uint64_t CodePoint = NumDigits; CodePoint > 0; CodePoint--) {
                    uint8_t Digit             = Num % Radix;
                    Num                      /= Radix;
                    if ((Base & Base2) == Base2) {
                        String[CodePoint - 1]     = StringIOIntegerTableBase2[Digit];
                    } else if ((Base & Base8) == Base8) {
                        String[CodePoint - 1]     = StringIOIntegerTableBase8[Digit];
                    } else if ((Base & Base10) == Base10) {
                        if (Sign != -1 && CodePoint != 1) {
                            String[CodePoint - 1] = StringIOIntegerTableBase10[Digit];
                        } else {
                            String[CodePoint - 1] = U32('-');
                        }
                    } else if ((Base & Base16) == Base16) {
                        if ((Base & Uppercase) == Uppercase) {
                            String[CodePoint - 1] = StringIOIntegerTableUppercaseBase16[Digit];
                        } else if ((Base & Lowercase) == Lowercase) {
                            String[CodePoint - 1] = StringIOIntegerTableLowercaseBase16[Digit];
                        }
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Base is not an integer, exiting"));
        }
        return String;
    }
    
    double UTF8_String2Decimal(UTF8 *String) {
        double Decimal = 0.0;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            Decimal         = UTF32_String2Decimal(String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF16_String2Decimal(UTF16 *String) {
        double Decimal = 0.0;
        if (String != NULL) {
            UTF32 *String32 = UTF16_Decode(String);
            Decimal         = UTF32_String2Decimal(String32);
            free(String32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF32_String2Decimal(UTF32 *String) { // Replaces strtod, strtof, strold, atof, and atof_l
        double   Value         = 0.0;
        bool     IsNegative    = No;
        if (String != NULL) {
            uint64_t CodePoint = 0ULL;
            for (uint8_t Whitespace = 0; Whitespace < WhiteSpaceTableSize; Whitespace++) {
                if (String[CodePoint] == WhiteSpaceTable[Whitespace]) {
                    CodePoint += 1;
                }
            }
            
            if (String[CodePoint] == U32('-')) {
                IsNegative = Yes;
            }
            
            while (String[CodePoint] != '.') {
                CodePoint     += 1;
            } // Before the decimal
            
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (String[0] == U32('-')) {
                    IsNegative = Yes;
                }
                CodePoint     += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Value;
    }
    
    UTF8 *UTF8_Decimal2String(StringIOBases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Base, Decimal);
        UTF8  *String8  = UTF8_Encode(String32);
        free(String32);
        return String8;
    }
    
    UTF16 *UTF16_Decimal2String(StringIOBases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Base, Decimal);
        UTF16 *String16 = UTF16_Encode(String32);
        free(String32);
        return String16;
    }
    
    UTF32 *UTF32_Decimal2String(StringIOBases Base, double Number) {
        UTF32   *OutputString     = NULL;
        uint64_t StringSize       = 0ULL;
        int8_t   Sign             = ExtractSignD(Number);
        int16_t  Exponent         = ExtractExponentD(Number);
        int16_t  Exponent2        = AbsoluteD(Exponent);
        int64_t  Mantissa         = ExtractMantissaD(Number);
        int64_t  Mantissa2        = AbsoluteD(Mantissa);
        bool     IsDenormal       = DecimalIsNormalD(Number);
        bool     IsNotANumber     = DecimalIsNotANumberD(Number);
        bool     IsInfinite       = DecimalIsInfinityD(Number);
        
        if (IsNotANumber) {
            OutputString          = UTF32_Clone(U32("Not A Number"));
        } else if (IsInfinite) {
            OutputString          = UTF32_Clone(U32("Infinity"));
        }
        
        
        
        
        /* OLD CODE BELOW */
        uint8_t NumDigitsExponent = 0;
        uint8_t NumDigitsMantissa = 0;
        
        if ((Base & Base2) == Base2) {
            NumDigitsExponent = Exponentiate(2, Exponent2);
            NumDigitsMantissa = Exponentiate(2, Mantissa2);
        } else if ((Base & Base8) == Base8) {
            NumDigitsExponent = Exponentiate(8, Exponent2);
            NumDigitsMantissa = Exponentiate(8, Mantissa2);
        } else if ((Base & Base10) == Base10) {
            NumDigitsExponent = Exponentiate(10, Exponent2);
            NumDigitsMantissa = Exponentiate(10, Mantissa2);
        } else if ((Base & Base16) == Base16) {
            NumDigitsExponent = Exponentiate(16, Exponent2);
            NumDigitsMantissa = Exponentiate(16, Mantissa2);
        }
        
        StringSize           += NumDigitsExponent + NumDigitsMantissa;
        
        if ((Base & Base2) == Base2) {
            StringSize += 1;
        }
        
        if (((Base & Base2) == Base2) && Sign == -1) {
            StringSize       += 1;
        }
        
        if (((Base & Base10) == Base10)) {
            StringSize       += 1;
        } else if ((Base & Hex) == Hex) {
            StringSize       += 5;
        } else if ((Base & Scientific) == Scientific) {
            StringSize       += 2;
        }
        
        OutputString          = calloc(StringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
        if (OutputString != NULL) {
            // Now we go ahead and create the string
            if (Sign == -1) {
                OutputString[1] = U32('-');
            }
            // Now we start popping in the other variables, first is the Exponent.
            while (Exponent > 0) { // TODO: This assumes there's only 1 codepoint necessary to express the exponent
                for (uint64_t ExponentCodePoint = 0ULL; ExponentCodePoint < NumDigitsExponent; ExponentCodePoint++) {
                    OutputString[ExponentCodePoint + StringSize]                 = Exponent /= 10;
                }
            }
            OutputString[StringSize + NumDigitsExponent + UnicodeBOMSizeInCodePoints] = U32('.');
            // Now let's start popping in the Mantissa
            while (Mantissa > 0) { // TODO: This assumes there's only 1 codepoint necessary to express the exponent
                for (uint64_t MantissaCodePoint = 0ULL; MantissaCodePoint < NumDigitsExponent; MantissaCodePoint++) {
                    OutputString[StringSize + NumDigitsExponent + MantissaCodePoint]  = Mantissa /= 10;
                }
            }
            if (Base == (Hex | Uppercase)) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = U32('A');
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = U32('P');
            } else if (Base == (Hex | Lowercase)) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = U32('a');
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = U32('p');
            } else if (Base == (Scientific | Uppercase)) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = U32('E');
                // Write the sign, if the number is positive, write a +, otherwise write a -
                if (Sign == -1) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = U32('-');
                } else {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = U32('+');
                }
                // Write the Exponent
                uint16_t NumDigitsExponentInDigits = (uint16_t) Logarithm(2, Exponent);
                for (uint64_t ExponentDigit = 0ULL; ExponentDigit < NumDigitsExponentInDigits; ExponentDigit++) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2 + ExponentDigit] = U32("Exponent"); // FIXME: "Exponent" is NOT right
                }
            } else if (Base == (Scientific | Lowercase)) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = U32('e');
                if (Sign == -1) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = U32('-');
                } else {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = U32('+');
                }
                // Write the Exponent
                uint64_t NumDigitsExponentInDigits = (uint16_t) Logarithm(2, Exponent);
                for (uint64_t ExponentDigit = 0ULL; ExponentDigit < NumDigitsExponentInDigits; ExponentDigit++) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2 + ExponentDigit] = U32("Exponent"); // FIXME: "Exponent" is NOT right
                }
            }
        }
        return OutputString;
    }
    /* Number Conversions */
    
    bool UTF8_Compare(UTF8 *String1, UTF8 *String2) {
        bool StringsMatch = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1_32 = UTF8_Decode(String1);
            UTF32 *String2_32 = UTF8_Decode(String2);
            StringsMatch      = UTF32_Compare(String1_32, String2_32);
            free(String1_32);
            free(String2_32);
        } else if (String1 == NULL) {
            Log(Log_DEBUG, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_DEBUG, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF16_Compare(UTF16 *String1, UTF16 *String2) {
        bool StringsMatch = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1_32 = UTF16_Decode(String1);
            UTF32 *String2_32 = UTF16_Decode(String2);
            StringsMatch      = UTF32_Compare(String1_32, String2_32);
            free(String1_32);
            free(String2_32);
        } else if (String1 == NULL) {
            Log(Log_DEBUG, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_DEBUG, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF32_Compare(UTF32 *String1, UTF32 *String2) {
        bool StringsMatch                    = No;
        if (String1 != NULL && String2 != NULL) {
            uint64_t String1SizeInCodePoints = UTF32_GetStringSizeInCodePoints(String1);
            uint64_t String2SizeInCodePoints = UTF32_GetStringSizeInCodePoints(String2);
            if (String1SizeInCodePoints == String2SizeInCodePoints) {
                for (uint64_t CodePoint = 0ULL; CodePoint < (String1SizeInCodePoints + String2SizeInCodePoints) / 2; CodePoint++) {
                    UTF32 CodePoint1         = String1[CodePoint];
                    UTF32 CodePoint2         = String2[CodePoint];
                    if (CodePoint1 != CodePoint2) {
                        StringsMatch         = No;
                        break;
                    } else {
                        StringsMatch         = Yes;
                    }
                }
            }
        } else if (String1 == NULL) {
            Log(Log_DEBUG, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_DEBUG, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF8_CompareSubString(UTF8 *String, UTF8 *Substring, uint64_t StringOffset, uint64_t SubstringOffset) {
        bool SubstringMatchesAtOffset = No;
        if (String != NULL && Substring != NULL) {
            UTF32 *String32           = UTF8_Decode(String);
            UTF32 *Sub32              = UTF8_Decode(Substring);
            SubstringMatchesAtOffset  = UTF32_CompareSubString(String32, Sub32, StringOffset, SubstringOffset);
            free(String32);
            free(Sub32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Substring == NULL) {
            Log(Log_DEBUG, __func__, U8("Substring Pointer is NULL"));
        }
        return SubstringMatchesAtOffset;
    }
    
    bool UTF16_CompareSubString(UTF16 *String, UTF16 *Substring, uint64_t StringOffset, uint64_t SubstringOffset) {
        bool SubstringMatchesAtOffset = No;
        if (String != NULL && Substring != NULL) {
            UTF32 *String32           = UTF16_Decode(String);
            UTF32 *Sub32              = UTF16_Decode(Substring);
            SubstringMatchesAtOffset  = UTF32_CompareSubString(String32, Sub32, StringOffset, SubstringOffset);
            free(String32);
            free(Sub32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Substring == NULL) {
            Log(Log_DEBUG, __func__, U8("Substring Pointer is NULL"));
        }
        return SubstringMatchesAtOffset;
    }
    
    bool UTF32_CompareSubString(UTF32 *String, UTF32 *Substring, uint64_t StringOffset, uint64_t SubstringOffset) {
        bool SubstringMatchesAtOffset            = No;
        if (String != NULL && Substring != NULL) {
            uint64_t StringSize                  = UTF32_GetStringSizeInCodePoints(String);
            uint64_t SubstringSize               = UTF32_GetStringSizeInCodePoints(Substring);
            for (uint64_t StringCodePoint = StringOffset; StringCodePoint < StringSize; StringCodePoint++) {
                for (uint64_t SubstringCodePoint = SubstringOffset; SubstringCodePoint < SubstringSize; SubstringCodePoint++) {
                    if (String[StringCodePoint] != Substring[SubstringCodePoint]) {
                        break;
                    } else {
                        SubstringMatchesAtOffset = Yes;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Substring == NULL) {
            Log(Log_DEBUG, __func__, U8("Substring Pointer is NULL"));
        } else {
            SubstringMatchesAtOffset             = No;
        }
        return SubstringMatchesAtOffset;
    }
    
    UTF8  *UTF8_Trim(UTF8 *String, TrimStringTypes Type, UTF8 **Strings2Remove) {
        UTF8 *Trimmed = NULL;
        if (String != NULL && Type != TrimString_Unknown && Strings2Remove != NULL) {
            UTF32    *String32                  = UTF8_Decode(String);
            UTF32   **Strings2Remove32          = UTF8_StringArray_Decode(Strings2Remove);
            UTF32    *Trimmed32                 = UTF32_Trim(String32, Type, Strings2Remove32);
            UTF32_StringArray_Deinit(Strings2Remove32);
            Trimmed                             = UTF8_Encode(Trimmed32);
            free(String32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Type == TrimString_Unknown) {
            Log(Log_DEBUG, __func__, U8("TrimString_Unknown is invalid"));
        } else if (Strings2Remove == NULL) {
            Log(Log_DEBUG, __func__, U8("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF16 *UTF16_Trim(UTF16 *String, TrimStringTypes Type, UTF16 **Strings2Remove) {
        UTF16 *Trimmed = NULL;
        if (String != NULL && Type != TrimString_Unknown && Strings2Remove != NULL) {
            UTF32    *String32                  = UTF16_Decode(String);
            UTF32   **Strings2Remove32          = UTF16_StringArray_Decode(Strings2Remove);
            UTF32    *Trimmed32                 = UTF32_Trim(String32, Type, Strings2Remove32);
            UTF32_StringArray_Deinit(Strings2Remove32);
            Trimmed                             = UTF16_Encode(Trimmed32);
            free(String32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Type == TrimString_Unknown) {
            Log(Log_DEBUG, __func__, U8("TrimString_Unknown is invalid"));
        } else if (Strings2Remove == NULL) {
            Log(Log_DEBUG, __func__, U8("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF32 *UTF32_Trim(UTF32 *String, TrimStringTypes Type, UTF32 **Strings2Remove) {
        UTF32 *Trimmed = NULL;
        if (String != NULL && Type != TrimString_Unknown && Strings2Remove != NULL) {
            uint64_t   StringSize          = UTF32_GetStringSizeInCodePoints(String);
            uint64_t   NumRemovalStrings   = UTF32_StringArray_GetNumStrings(Strings2Remove);
            uint64_t  *RemovalStringSizes  = UTF32_StringArray_GetStringSizesInCodePoints(Strings2Remove);
            uint64_t   NumRemovalPoints    = 0ULL;
            uint64_t   CurrentRemovalPoint = 0ULL;
            uint64_t  *RemovalPointsStart  = NULL; // RemovalPoint[0] = {0, 6}; start and stop points
            uint64_t  *RemovalPointsEnd    = NULL; // RemovalPoint[0] = {0, 6}; start and stop points
            uint64_t   TrimmedStringSize   = 0ULL;
            if (Type == TrimString_RemoveAll) {
                for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                        bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                        if (SubstringFound) {
                            NumRemovalPoints += 1;
                        }
                    }
                }
                
                RemovalPointsStart                                  = calloc(NumRemovalPoints, sizeof(uint64_t));
                RemovalPointsEnd                                    = calloc(NumRemovalPoints, sizeof(uint64_t));
                
                for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                        bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                        if (SubstringFound) {
                            RemovalPointsStart[CurrentRemovalPoint] = StringCodePoint;
                            RemovalPointsEnd[CurrentRemovalPoint]   = RemovalStringSizes[RemovalString];
                            CurrentRemovalPoint                    += 1;
                        }
                    }
                }
            } else {
                if (Type == TrimString_StartEndRemoveAll) {
                    // Loop over all the codepoints until you find one that is not on the list, then remove it; BUT JUST FOR THE BEGINNING AND END
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                            bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                            if (SubstringFound) {
                                NumRemovalPoints += 1;
                            }
                        }
                    }
                    
                    RemovalPointsStart                                  = calloc(NumRemovalPoints, sizeof(uint64_t));
                    RemovalPointsEnd                                    = calloc(NumRemovalPoints, sizeof(uint64_t));
                    
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                            bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                            if (SubstringFound) {
                                RemovalPointsStart[CurrentRemovalPoint] = StringCodePoint;
                                RemovalPointsEnd[CurrentRemovalPoint]   = RemovalStringSizes[RemovalString];
                                CurrentRemovalPoint                    += 1;
                            }
                        }
                    }
                }
                if (Type == TrimString_BetweenValidKeep1) {
                    // Loop over all the codepoints until you find one that is not on the list, then remove it.
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                            bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                            if (SubstringFound) {
                                NumRemovalPoints += 1;
                            }
                        }
                    }
                    
                    RemovalPointsStart                                  = calloc(NumRemovalPoints, sizeof(uint64_t));
                    RemovalPointsEnd                                    = calloc(NumRemovalPoints, sizeof(uint64_t));
                    
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                            bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                            if (SubstringFound) {
                                RemovalPointsStart[CurrentRemovalPoint] = StringCodePoint;
                                RemovalPointsEnd[CurrentRemovalPoint]   = RemovalStringSizes[RemovalString];
                                CurrentRemovalPoint                    += 1;
                            }
                        }
                    }
                }
            }
            // Actually build the string, regardless of mode.
            TrimmedStringSize = StringSize;
            for (uint64_t RemovalPoint = 0ULL; RemovalPoint < NumRemovalPoints; RemovalPoint++) {
                TrimmedStringSize     -= RemovalPointsEnd[RemovalPoint] - RemovalPointsStart[RemovalPoint];
            }
            
            Trimmed                    = calloc(TrimmedStringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (Trimmed != NULL) {
                for (uint64_t RemovalPoint = 0ULL; RemovalPoint < NumRemovalPoints; RemovalPoint++) {
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        for (uint64_t TrimmedCodePoint = 0ULL; TrimmedCodePoint < TrimmedStringSize; TrimmedCodePoint++) {
                            if (StringCodePoint < RemovalPointsStart[RemovalPoint] || StringCodePoint > RemovalPointsEnd[RemovalPoint]) {
                                Trimmed[TrimmedCodePoint] = String[StringCodePoint];
                            }
                        }
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate Trimmed string"));
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Type == TrimString_Unknown) {
            Log(Log_DEBUG, __func__, U8("TrimString_Unknown is invalid"));
        } else if (Strings2Remove == NULL) {
            Log(Log_DEBUG, __func__, U8("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF8 **UTF8_Split(UTF8 *String, UTF8 **Delimiters) {
        UTF8 **SplitString        = NULL;
        if (String != NULL && Delimiters != NULL) {
            UTF32  *String32      = UTF8_Decode(String);
            UTF32 **Delimiters32  = UTF8_StringArray_Decode(Delimiters);
            UTF32 **SplitString32 = UTF32_Split(String32, Delimiters32);
            free(String32);
            free(Delimiters32);
            SplitString           = UTF8_StringArray_Encode(SplitString32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Log_DEBUG, __func__, U8("Delimiters Pointer is NULL"));
        }
        return SplitString;
    }
    
    UTF16 **UTF16_Split(UTF16 *String, UTF16 **Delimiters) {
        UTF16 **SplitString       = NULL;
        if (String != NULL && Delimiters != NULL) {
            UTF32  *String32      = UTF16_Decode(String);
            UTF32 **Delimiters32  = UTF16_StringArray_Decode(Delimiters);
            UTF32 **SplitString32 = UTF32_Split(String32, Delimiters32);
            free(String32);
            free(Delimiters32);
            SplitString           = UTF16_StringArray_Encode(SplitString32);
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Log_DEBUG, __func__, U8("Delimiters Pointer is NULL"));
        }
        return SplitString;
    }
    
    UTF32 **UTF32_Split(UTF32 *String, UTF32 **Delimiters) {
        UTF32    **SplitStrings    = NULL; // What we return, it's a 0 indexed array of strings
        uint64_t   StringSize      = 0ULL; // The size of the first parameter
        uint64_t   NumDelimiters   = 0ULL; // The number of delimiters in the second parameter
        uint64_t  *DelimitersSize  = NULL; // an array containing the size of each delimiter
        uint64_t   NumSplitStrings = 0ULL; // The number of strings to return
        uint64_t  *SplitSizes      = NULL; // The size of each split string
        uint64_t  *SplitOffsets    = NULL; // The starting position of each split
        if (String != NULL && Delimiters != NULL) {
            StringSize             = UTF32_GetStringSizeInCodePoints(String);
            NumDelimiters          = UTF32_StringArray_GetNumStrings(Delimiters);
            DelimitersSize         = calloc(NumDelimiters + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            if (DelimitersSize != NULL) {
                for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                    DelimitersSize[Delimiter] = UTF32_GetStringSizeInCodePoints(Delimiters[Delimiter]);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate space for the delimiter sizes"));
            }
            // Check if the current delimiter is larger than the string, if so, it can't match.
            // Well we need to loop over the string NumDelimiters times, so Delimiters, String, DelimiterString
            for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                for (uint64_t DelimiterCodePoint = 0ULL; DelimiterCodePoint < DelimitersSize[Delimiter]; DelimiterCodePoint++) {
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        // Ok, so now we need to count the number of split strings we're gonna have, and their sizes in codepoints.
                        // Example: "/Users/Marcus/Desktop/Elephants Dream/%05d.png"
                        // Delimiters: 1, "/"
                        // Splits: 5; Users, Marcus, Desktop, Elephants Dream, %05d.png
                        if (String[StringCodePoint] == Delimiters[Delimiter][DelimiterCodePoint] && DelimiterCodePoint == DelimitersSize[Delimiter]) {
                            NumSplitStrings += 1;
                        }
                    }
                }
            }
            
            SplitSizes   = calloc(NumSplitStrings + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            SplitOffsets = calloc(NumSplitStrings + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            
            for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                for (uint64_t DelimiterCodePoint = 0ULL; DelimiterCodePoint < DelimitersSize[Delimiter]; DelimiterCodePoint++) {
                    for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        if (String[StringCodePoint] == Delimiters[Delimiter][DelimiterCodePoint]) {
                            if (DelimitersSize[Delimiter] > 1) {
                                // We need to make sure the delimiter matches entirely
                            } else {
                                // You've found the delimiter, time to start or stop the current delimiter and move to the next one
                                // So set DelimiterSize the SplitSize - SplitOffset?
                            }
                        }
                        // Now how do we get the size of the actual string sections?
                        // To determine the end of  split, we need to look for either the end of a string, or the start of a new delimiter, but maybe that's not the best way? maybe we should end when we're at the end of a delimiter?
                        
                        // Example: Kanye West, T.I., Jay-Z & Beyoncé
                        // Delimiters: (,|&| )
                        // Splits: "Kanye West", "T.I.", "Jay-Z", "Beyoncé"
                        // Offsets: 0, 12, 18, 26
                        // Sizes:   10, 4, 5, 7
                        
                        // Ok so basically we need to make sure that each codepoint does not match any coepoint in any delimiter, if it does, we need to further check?
                        
                        /* What we need to do is compare the stream a codepoint at a time to the delimiters, make sure the whole thing matches, and if it does increment the number of splits. */
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Log_DEBUG, __func__, U8("Delimiters Pointer is NULL"));
        }
        return SplitStrings;
    }
    
    typedef enum FormatSpecifier_EscapeSequences {
        EscapeSequence_Unknown         = 0,
        EscapeSequence_Beep            = 1,
        EscapeSequence_Backspace       = 2,
        EscapeSequence_FormFeed        = 3,
        EscapeSequence_NewLine         = 4,
        EscapeSequence_CarriageReturn  = 5,
        EscapeSequence_Tab             = 6,
        EscapeSequence_VerticalTab     = 7,
        EscapeSequence_Backslash       = 8, // Replace with a simple "\"
        EscapeSequence_Apostrophe      = 9,
        EscapeSequence_Quote           = 10,
        EscapeSequence_QuestionMark    = 11,
        EscapeSequence_Octal           = 12, // \nnn
        EscapeSequence_Hexadecimal     = 13, // \xYY where Y is 0-9a-fA-F
        EscapeSequence_Unicode4        = 14, // \uYYYY where y is 0-9a-fA-F
        EscapeSequence_Unicode8        = 15, // \UYYYYYYYY where y is 0-9a-fA-F
    } FormatSpecifier_EscapeSequences;
    
    typedef enum FormatSpecifier_Flags {
        Flag_Unknown                   = 0,
        Flag_Minus_LeftJustify         = 1,
        Flag_Plus_AddSign              = 2,
        Flag_Space_Pad                 = 4,
        Flag_Pound1_PrefixBase         = 8,
        Flag_Pound2_SuffixDecimal      = 16,
        Flag_Zero_Pad                  = 32,
    } FormatSpecifier_Flags;
    
    typedef enum FormatSpecifier_MinWidths {
        MinWidth_Unknown               = 0,
        MinWidth_Asterisk_NextArg      = 1,
        MinWidth_Digits                = 2,
    } FormatSpecifier_MinWidths;
    
    typedef enum FormatSpecifier_Precisions {
        Precision_Unknown              = 0,
        Precision_Dot_Number           = 1,
        Precision_Dot_Asterisk_NextArg = 2,
    } FormatSpecifier_Precisions;
    
    typedef enum FormatSpecifier_BaseTypes {
        BaseType_Unknown               = 0,
        BaseType_Integer               = 1,
        BaseType_Decimal               = 2,
        BaseType_Character             = 4,
        BaseType_String                = 8,
        BaseType_Literal               = 16,
        BaseType_EscapeSequence        = 32,
        BaseType_Positional            = 64,
    } FormatSpecifier_BaseTypes;
    
    typedef enum FormatSpecifier_TypeModifiers { // MSVC supports capital C and S for "wide" aka UTF-16 characters/strings
        Modifier_Unknown               = 0,
        Modifier_Percent               = 1,
        Modifier_UTF8                  = 2,
        Modifier_UTF16                 = 4,
        Modifier_UTF32                 = 8,
        Modifier_Base2                 = 16,
        Modifier_Base8                 = 32,
        Modifier_Base10                = 64,
        Modifier_Base16                = 128,
        Modifier_Decimal               = 256,  // XX.YYY
        Modifier_Scientific            = 512, // XX.YYYE(+|-)Z
        Modifier_Shortest              = 1024, // Scientific, or Decimal, whichever is  shorter
        Modifier_Hex                   = 2048, // 0XF.FFFFFFFFFFFFFFFP+60
        Modifier_Uppercase             = 4096,
        Modifier_Lowercase             = 8192,
        Modifier_Long                  = 16384,
        Modifier_Signed                = 131072,
        Modifier_Unsigned              = 262144,
    } FormatSpecifier_TypeModifiers;
    
    typedef enum FormatSpecifier_LengthModifiers {
        Length_Unknown                 = 0,
        Length_8Bit                    = 1,
        Length_16Bit                   = 2,
        Length_32Bit                   = 4,
        Length_64Bit                   = 8,
        Length_SizeType                = 16, // Modifier_Size
        Length_PointerDiff             = 32, // Modifier_PointerDiff
        Length_IntMax                  = 64,
    } FormatSpecifier_LengthModifiers;
    
    typedef enum FormatSpecifier_StringTypes {
        StringType_Unknown             = 0,
        UTF8Format                     = 1,
        UTF16Format                    = 2,
        UTF32Format                    = 3,
    } FormatSpecifier_StringTypes;
    
    typedef struct FormatSpecifier {
        UTF32                          *Argument;        // The actual argument contained in the va_list
        uint64_t                        SpecifierOffset; // Start location in the format string
        uint64_t                        SpecifierLength; // Start - Size
        uint64_t                        MinWidth;        // Actual Width
        uint64_t                        Precision;       // Actual Precision
        uint64_t                        PositionalArg;   // Argument number to substitute
        FormatSpecifier_TypeModifiers   TypeModifier;
        FormatSpecifier_LengthModifiers LengthModifier;
        FormatSpecifier_Flags           Flag;
        FormatSpecifier_MinWidths       MinWidthFlag;
        FormatSpecifier_Precisions      PrecisionFlag;
        FormatSpecifier_BaseTypes       BaseType;
        FormatSpecifier_EscapeSequences EscapeType;
        uint8_t                         OctalSeqSize;    // Octal Sequence Size, Max 3
    } FormatSpecifier;
    
    typedef struct FormatSpecifiers {
        FormatSpecifier             *Specifiers;
        uint64_t                     NumSpecifiers;
        FormatSpecifier_StringTypes  StringType;
    } FormatSpecifiers;
    
    static void FormatSpecifiers_Deinit(FormatSpecifiers *Details) {
        if (Details != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                free(Details->Specifiers[Specifier].Argument);
            }
            free(Details->Specifiers);
            free(Details);
        }
    }
    
    static FormatSpecifiers *FormatSpecifiers_Init(uint64_t NumSpecifiers) {
        FormatSpecifiers *NewFormatSpecifiers      = calloc(1, sizeof(FormatSpecifiers));
        if (NewFormatSpecifiers != NULL) {
            NewFormatSpecifiers->Specifiers        = calloc(NumSpecifiers, sizeof(FormatSpecifier));
            if (NewFormatSpecifiers->Specifiers != NULL) {
                NewFormatSpecifiers->NumSpecifiers = NumSpecifiers;
            } else {
                FormatSpecifiers_Deinit(NewFormatSpecifiers);
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld Specifiers"), NumSpecifiers);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate FormatSpecifiers"));
        }
        return NewFormatSpecifiers;
    }
    
    uint64_t UTF32_GetNumDigits(StringIOBases Base, UTF32 *String, uint64_t Offset) {
        uint64_t NumDigits      = 0ULL;
        if (String != NULL) {
            uint64_t CodePoint  = Offset;
            bool     ValidDigit = Yes;
            if ((Base & Integer) == Integer) {
                if ((Base & Base2) == Base2) {
                    while (String[CodePoint] != FoundationIONULLTerminator) {
                        for (uint8_t Base2CodePoint = 0; Base2CodePoint < StringIOIntegerTableBase2Size; Base2CodePoint++) {
                            if (String[CodePoint] == StringIOIntegerTableBase2[Base2CodePoint]) {
                                NumDigits += 1;
                            }
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base8) == Base8) {
                    while (String[CodePoint] != FoundationIONULLTerminator) {
                        for (uint8_t Base8CodePoint = 0; Base8CodePoint < StringIOIntegerTableBase8Size; Base8CodePoint++) {
                            if (String[CodePoint] == StringIOIntegerTableBase8[Base8CodePoint]) {
                                NumDigits += 1;
                            }
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base10) == Base10) {
                    while (String[CodePoint] != FoundationIONULLTerminator && ValidDigit == Yes) {
                        for (uint8_t Base10CodePoint = 0; Base10CodePoint < StringIOIntegerTableBase10Size; Base10CodePoint++) {
                            if (String[CodePoint] == StringIOIntegerTableBase10[Base10CodePoint]) {
                                NumDigits += 1;
                                break;
                            } else if (Base10CodePoint == StringIOIntegerTableBase10Size - 1) {
                                ValidDigit = No;
                                break;
                            }
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base16) == Base16) {
                    if ((Base & Uppercase) == Uppercase) {
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t Base16UCodePoint = 0; Base16UCodePoint < StringIOIntegerTableBase16Size; Base16UCodePoint++) {
                                if (String[CodePoint] == StringIOIntegerTableUppercaseBase16[Base16UCodePoint]) {
                                    NumDigits += 1;
                                }
                            }
                            CodePoint         += 1;
                        }
                    } else if ((Base & Lowercase) == Lowercase) {
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t Base16LCodePoint = 0; Base16LCodePoint < StringIOIntegerTableBase16Size; Base16LCodePoint++) {
                                if (String[CodePoint] == StringIOIntegerTableLowercaseBase16[Base16LCodePoint]) {
                                    NumDigits += 1;
                                }
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            } else if ((Base & Decimal) == Decimal) {
                if ((Base & Uppercase) == Uppercase) {
                    if (((Base & Scientific) == Scientific) || ((Base & Shortest) == Shortest)) { // 1.844674E+19, or regular decimal
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t ScientificCodePoint = 0; ScientificCodePoint < StringIODecimalTableScientificSize; ScientificCodePoint++) {
                                if (String[CodePoint] == StringIODecimalScientificUppercase[ScientificCodePoint]) {
                                    NumDigits += 1;
                                }
                            }
                            CodePoint         += 1;
                        }
                    } else if ((Base & Hex) == Hex) {
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t HexCodePoint = 0; HexCodePoint < StringIODecimalTableHexadecimalSize; HexCodePoint++) {
                                if (String[CodePoint] == StringIODecimalHexUppercase[HexCodePoint]) {
                                    NumDigits += 1;
                                }
                            }
                            CodePoint         += 1;
                        }
                    }
                } else if ((Base & Lowercase) == Lowercase) {
                    if (((Base & Scientific) == Scientific) || ((Base & Shortest) == Shortest)) { // 1.844674E+19, or regular decimal
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t ScientificCodePoint = 0; ScientificCodePoint < StringIODecimalTableScientificSize; ScientificCodePoint++) {
                                if (String[CodePoint] == StringIODecimalScientificLowercase[ScientificCodePoint]) {
                                    NumDigits += 1;
                                }
                            }
                            CodePoint         += 1;
                        }
                    } else if ((Base & Hex) == Hex) {
                        while (String[CodePoint] != FoundationIONULLTerminator) {
                            for (uint8_t HexCodePoint = 0; HexCodePoint < StringIODecimalTableHexadecimalSize; HexCodePoint++) {
                                if (String[CodePoint] == StringIODecimalHexLowercase[HexCodePoint]) {
                                    NumDigits += 1;
                                }
                            }
                            CodePoint         += 1;
                        }
                    }
                } else {
                    while (String[CodePoint] != FoundationIONULLTerminator) {
                        for (uint8_t DecimalCodePoint = 0; DecimalCodePoint < StringIODecimalTableSize; DecimalCodePoint++) {
                            if (String[CodePoint] == StringIODecimalTable[DecimalCodePoint]) {
                                NumDigits += 1;
                            }
                        }
                        CodePoint         += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return NumDigits;
    }
    
    static StringIOBases ConvertTypeModifier2Base(FormatSpecifier_TypeModifiers TypeModifier) {
        StringIOBases Base = UnknownBase;
        if ((TypeModifier & Modifier_Decimal) == Modifier_Decimal) {
            Base           = Decimal;
        } else if ((TypeModifier & Modifier_Scientific) == Modifier_Scientific) {
            Base          |= Decimal | Scientific;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        } else if ((TypeModifier & Modifier_Shortest) == Modifier_Shortest) {
            Base          |= Decimal | Shortest;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        } else if ((TypeModifier & Modifier_Hex) == Modifier_Hex) {
            Base          |= Decimal | Hex;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        } else if ((TypeModifier & Modifier_Base2) == Modifier_Base2) {
            Base          |= Integer | Base2;
        } else if ((TypeModifier & Modifier_Base8) == Modifier_Base8) {
            Base          |= Integer | Base8;
        } else if ((TypeModifier & Modifier_Base10) == Modifier_Base10) {
            Base          |= Integer | Base10;
        } else if ((TypeModifier & Modifier_Base16) == Modifier_Base16) {
            Base          |= Integer | Base16;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        }
        return Base;
    }
    
    static FormatSpecifiers *UTF32_ParseFormatSpecifiers(UTF32 *Format, uint64_t NumSpecifiers, FormatSpecifier_StringTypes StringType) {
        FormatSpecifiers *Details         = NULL;
        if (Format != NULL && StringType != StringType_Unknown) {
            Details                       = FormatSpecifiers_Init(NumSpecifiers);
            if (Details != NULL) {
                Details->StringType       = StringType;
                uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(Format);
                uint64_t Specifier        = 0ULL;
                uint64_t CodePoint        = 0ULL;
                uint64_t CodePoint2       = 0ULL;
                while (Specifier < NumSpecifiers) {
                    while (CodePoint < StringSize) {
                        if (Format[CodePoint] == U32('%')) { // Found a Format Specifier
                            Details->Specifiers[Specifier].SpecifierOffset = CodePoint;
                            Details->Specifiers[Specifier].SpecifierLength = 1; // 1 to account for the percent
                            CodePoint2                                     = CodePoint + 1;
                            /*
                             We have to disambiguate between Flag 0, and Octal, or even Hex input.
                             
                             Specifier is the only Mandatory field, everything else is Optional.
                             */
                            if (CodePoint2 < StringSize) {
                                /* Flags, may be multiple */
                                if (Format[CodePoint2] == U32('+')) {
                                    Details->Specifiers[Specifier].Flag     |= Flag_Plus_AddSign;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32('-')) {
                                    Details->Specifiers[Specifier].Flag     |= Flag_Minus_LeftJustify;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32(' ')) { // If Space and + are both used, Ignore the Space
                                    Details->Specifiers[Specifier].Flag     |= Flag_Space_Pad;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32('0')) {
                                    Details->Specifiers[Specifier].Flag     |= Flag_Zero_Pad;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32('#')) {
                                    if ((Details->Specifiers[Specifier].TypeModifier & Modifier_Base8) == Modifier_Base8 || (Details->Specifiers[Specifier].TypeModifier & Modifier_Base16) == Modifier_Base16) {
                                        Details->Specifiers[Specifier].Flag |= Flag_Pound1_PrefixBase;
                                    } else if ((Details->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                                        Details->Specifiers[Specifier].Flag |= Flag_Pound2_SuffixDecimal;
                                    }
                                    CodePoint2                              += 1;
                                }
                                /* Flags */
                                
                                /* Positional, MinWidth */
                                uint64_t NumDigits   = 0ULL;
                                int64_t  Digits      = 0LL;
                                UTF32   *DigitString = NULL;
                                switch (Format[CodePoint2]) {
                                        case U32('0'):
                                        case U32('1'):
                                        case U32('2'):
                                        case U32('3'):
                                        case U32('4'):
                                        case U32('5'):
                                        case U32('6'):
                                        case U32('7'):
                                        case U32('8'):
                                        case U32('9'):
                                        NumDigits   = UTF32_GetNumDigits(Integer | Base10, Format, CodePoint2);
                                        DigitString = UTF32_ExtractSubString(Format, CodePoint2, NumDigits);
                                        Digits      = UTF32_String2Integer(Integer | Base10, DigitString);
                                        Details->Specifiers[Specifier].SpecifierLength += NumDigits;
                                        CodePoint2 += NumDigits;
                                        break;
                                }
                                
                                if (CodePoint2 < StringSize && NumDigits > 0) {
                                    if (Format[CodePoint2] == U32('$')) { // Positional
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_Positional;
                                        Details->Specifiers[Specifier].PositionalArg    = Digits - 1;
                                        CodePoint2                                     += 1;
                                        Details->Specifiers[Specifier].SpecifierLength += 1;
                                    } else {
                                        Details->Specifiers[Specifier].MinWidthFlag     = MinWidth_Digits;
                                        Details->Specifiers[Specifier].MinWidth         = Digits;
                                    }
                                } else {
                                    if (Format[CodePoint2] == U32('*')) { // MinWidth
                                        Details->Specifiers[Specifier].MinWidthFlag     = MinWidth_Asterisk_NextArg;
                                        CodePoint2                                     += 1;
                                        Details->Specifiers[Specifier].SpecifierLength += 1;
                                    }
                                }
                                /* Positional,MinWidth */
                                
                                /* Precision */
                                if (Format[CodePoint2] == U32('.')) {
                                    Details->Specifiers[Specifier].SpecifierLength             += 1;
                                    CodePoint2                                                 += 1;
                                    if (Format[CodePoint2] < StringSize) {
                                        uint64_t NumDigits                                      = 0ULL;
                                        UTF32   *DigitString                                    = NULL;
                                        int64_t  Digits                                         = 0LL;
                                        switch (Format[CodePoint2]) {
                                                case U32('*'):
                                                Details->Specifiers[Specifier].PrecisionFlag   |= Precision_Dot_Asterisk_NextArg;
                                                Details->Specifiers[Specifier].SpecifierLength += 1;
                                                CodePoint2                                     += 1;
                                                break;
                                                case U32('0'):
                                                case U32('1'):
                                                case U32('2'):
                                                case U32('3'):
                                                case U32('4'):
                                                case U32('5'):
                                                case U32('6'):
                                                case U32('7'):
                                                case U32('8'):
                                                case U32('9'):
                                                Details->Specifiers[Specifier].PrecisionFlag   |= Precision_Dot_Number;
                                                NumDigits                                       = UTF32_GetNumDigits(Integer | Base10, Format, CodePoint2);
                                                DigitString                                     = UTF32_ExtractSubString(Format, CodePoint2, NumDigits);
                                                Digits                                          = UTF32_String2Integer(Integer | Base10, DigitString);
                                                
                                                Details->Specifiers[Specifier].MinWidth         = Digits;
                                                CodePoint2                                     += NumDigits;
                                                Details->Specifiers[Specifier].SpecifierLength += NumDigits;
                                                break;
                                        }
                                    }
                                }
                                /* Precision */
                                
                                /* Length Modifiers */
                                if (CodePoint2 + 1 < StringSize) {
                                    if (Format[CodePoint2] == U32('l') && Format[CodePoint2 + 1] == U32('l')) {
                                        Details->Specifiers[Specifier].LengthModifier  |= Length_64Bit;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        CodePoint2                                     += 2;
                                    } else if (Format[CodePoint2] == U32('h') && Format[CodePoint2 + 1] == U32('h')) {
                                        Details->Specifiers[Specifier].LengthModifier  |= Length_8Bit;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        CodePoint2                                     += 2;
                                    }
                                }
                                
                                if (Format[CodePoint2] == U32('l') && (Details->Specifiers[Specifier].LengthModifier & Length_64Bit) != Length_64Bit) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_32Bit;
                                    Details->Specifiers[Specifier].SpecifierLength  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('h') && (Details->Specifiers[Specifier].LengthModifier & Length_8Bit) != Length_8Bit) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_16Bit;
                                    Details->Specifiers[Specifier].SpecifierLength  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('j')) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_IntMax;
                                    Details->Specifiers[Specifier].SpecifierLength  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('z')) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_SizeType;
                                    Details->Specifiers[Specifier].SpecifierLength  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('t')) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_PointerDiff;
                                    Details->Specifiers[Specifier].SpecifierLength  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('L')) {
                                    Details->Specifiers[Specifier].TypeModifier     |= Modifier_UTF16;
                                    Details->Specifiers[Specifier].SpecifierLength  += 1;
                                    CodePoint2                                      += 1;
                                }
                                /* Length Modifiers */
                                
                                /* Type */
                                switch (Format[CodePoint2]) {
                                        case U32('d'): // Fallthrough, bitches!
                                        case U32('i'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Signed | Modifier_Base10);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('u'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base10);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('o'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base8);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('x'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('X'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('f'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Decimal | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('F'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Decimal | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('e'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Scientific | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('E'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Scientific | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('g'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Shortest | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('G'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Shortest | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('a'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Hex | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('A'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Hex | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('c'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Character;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('C'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Character;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('s'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_String;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('S'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_String;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                        case U32('%'):
                                        Details->Specifiers[Specifier].SpecifierLength  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Literal;
                                        Details->Specifiers[Specifier].TypeModifier     |= Modifier_Percent;
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    default:
                                        Log(Log_DEBUG, __func__, U8("Unknown Format Specifier %s"), Format[CodePoint]);
                                        break;
                                }
                                Specifier  += 1;
                            }
                        } else if (Format[CodePoint] == U32('\\')) { // Found an Escape Sequence
                            Details->Specifiers[Specifier].SpecifierOffset              = CodePoint;
                            Details->Specifiers[Specifier].SpecifierLength              = 1;
                            CodePoint2                                                  = CodePoint + 1;
                            while (CodePoint2 < StringSize) {
                                switch (Format[CodePoint2]) {
                                        case U32('a'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Beep;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('b'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Backslash;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('f'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_FormFeed;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('n'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_NewLine;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('r'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_CarriageReturn;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('t'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Tab;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('v'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_VerticalTab;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('\\'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Backslash;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('\''):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Apostrophe;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('\"'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Quote;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('\?'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_QuestionMark;
                                        Details->Specifiers[Specifier].SpecifierLength += 2;
                                        break;
                                        case U32('x'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Hexadecimal;
                                        Details->Specifiers[Specifier].SpecifierLength += 2 + UTF32_GetNumDigits(Integer | Base16, Format, CodePoint2);
                                        break;
                                        case U32('u'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Unicode4;
                                        Details->Specifiers[Specifier].SpecifierLength += 6;
                                        break;
                                        case U32('U'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Unicode8;
                                        Details->Specifiers[Specifier].SpecifierLength += 10;
                                        break;
                                        case U32('0'):
                                        case U32('1'):
                                        case U32('2'):
                                        case U32('3'):
                                        case U32('4'):
                                        case U32('5'):
                                        case U32('6'):
                                        case U32('7'):
                                        Details->Specifiers[Specifier].BaseType        |= BaseType_EscapeSequence;
                                        Details->Specifiers[Specifier].EscapeType       = EscapeSequence_Octal;
                                        Details->Specifiers[Specifier].OctalSeqSize     = UTF32_GetNumDigits(Integer | Base8, Format, CodePoint2);
                                        if (Details->Specifiers[Specifier].OctalSeqSize > 3) {
                                            Details->Specifiers[Specifier].OctalSeqSize = 3;
                                            
                                        }
                                        break;
                                }
                                CodePoint2 += 1;
                                Specifier  += 1;
                            }
                        }
                        CodePoint          += 1;
                    }
                }
                /*
                 Lets reorganize these CodePoint and Specifier loops that way we don't do unnessicary work.
                 
                 Specifier should just be a vriable that gets incremented each time a specifier is found, we can use a while loop for it.
                 
                 while(Specifier < NumSpecifiers)
                 
                 Now, should the CodePoint lop be inside or outside of the Dowhile
                 */
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate FormatSpecifiers"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Details;
    }
    
    static UTF8 *UTF8_CodeUnit2String(UTF8 CodeUnit) {
        UTF8 *String  = calloc(1 + FoundationIONULLTerminatorSize, sizeof(UTF8));
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    static UTF16 *UTF16_CodeUnit2String(UTF16 CodeUnit) {
        UTF16 *String  = calloc(1 + FoundationIONULLTerminatorSize, sizeof(UTF16));
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    static UTF32 *UTF32_CodeUnit2String(UTF32 CodeUnit) {
        UTF32 *String  = calloc(1 + FoundationIONULLTerminatorSize, sizeof(UTF32));
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    static void FormatSpecifiers_FixOffsets(FormatSpecifiers *Details) {
        uint64_t Offset          = 0ULL;
        uint64_t Length          = 0ULL;
        uint64_t ArgumentSize    = 0ULL;
        
        /*
         Input string: "NumArgs: %2$llu, %1$s", U8("Positional"), 2
         Input length: 22
         Spec1 Length: 10
         
         Output string: "NumArgs: 2, Positional"
         Output length: 22
         */
        
        if (Details != NULL) {
            Offset                        = Details->Specifiers[0].SpecifierOffset;
            
            for (uint64_t Specifier = 1ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                if ((Details->Specifiers[Specifier].BaseType & BaseType_Positional) == BaseType_Positional) {
                    if (Details->Specifiers[Specifier].PositionalArg < Details->NumSpecifiers) {
                        uint64_t Position = Details->Specifiers[Specifier].PositionalArg;
                        ArgumentSize      = UTF32_GetStringSizeInCodePoints(Details->Specifiers[Position].Argument);
                        Length            = Details->Specifiers[Position].SpecifierLength;
                    }
                } else {
                    ArgumentSize          = UTF32_GetStringSizeInCodePoints(Details->Specifiers[Specifier].Argument);
                    Length                = Details->Specifiers[Specifier].SpecifierLength;
                }
                
                if (Length >= ArgumentSize) {
                    Offset               -= ArgumentSize;
                } else {
                    Offset               += ArgumentSize;
                }
                
                Details->Specifiers[Specifier].SpecifierOffset += Offset;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    static UTF32 *FormatString_UTF32(UTF32 *Format, FormatSpecifiers *Details, va_list VariadicArguments) {
        UTF32 *Formatted                                 = NULL;
        UTF32 *FormatTemp                                = Format;
        if (Format != NULL && Details != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                FormatSpecifier_BaseTypes       Base     = BaseType_Unknown;
                FormatSpecifier_TypeModifiers   Type     = Modifier_Unknown;
                FormatSpecifier_LengthModifiers Length   = Length_Unknown;
                uint64_t                        Position = 0ULL;
                
                if ((Details->Specifiers[Specifier].BaseType & BaseType_Positional) != BaseType_Positional) { // Regular argument, ordered for branch prediciton
                    Position                             = Specifier;
                    Base                                 = Details->Specifiers[Specifier].BaseType;
                    Type                                 = Details->Specifiers[Specifier].TypeModifier;
                    Length                               = Details->Specifiers[Specifier].LengthModifier;
                } else if ((Details->Specifiers[Specifier].BaseType & BaseType_Positional) == BaseType_Positional && Details->Specifiers[Specifier].PositionalArg < Details->NumSpecifiers) { // Positional Argument
                    Position                             = Details->Specifiers[Specifier].PositionalArg;
                    Base                                 = Details->Specifiers[Position].BaseType;
                    Type                                 = Details->Specifiers[Position].TypeModifier;
                    Length                               = Details->Specifiers[Position].LengthModifier;
                }
                
                if ((Base & BaseType_Literal) == BaseType_Literal && Type == Modifier_Percent) {
                    Details->Specifiers[Position].Argument         = UTF32_Clone(U32("%"));
                } else if ((Base & BaseType_EscapeSequence) == BaseType_EscapeSequence) {
                    switch (Details->Specifiers[Position].EscapeType & BaseType_Positional - 1) {
                            case EscapeSequence_Beep:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\x07"));
                            break;
                            case EscapeSequence_Backspace:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\x08"));
                            break;
                            case EscapeSequence_Tab:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\x09"));
                            break;
                            case EscapeSequence_VerticalTab:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\x0B"));
                            break;
                            case EscapeSequence_FormFeed:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\x0C"));
                            break;
                            case EscapeSequence_CarriageReturn:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\x0D"));
                            break;
                            case EscapeSequence_NewLine:
                            Details->Specifiers[Position].Argument = UTF32_Clone(FoundationIONewLine32);
                            break;
                            case EscapeSequence_Backslash:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\\"));
                            break;
                            case EscapeSequence_Apostrophe:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\'"));
                            break;
                            case EscapeSequence_Quote:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\""));
                            break;
                            case EscapeSequence_QuestionMark:
                            Details->Specifiers[Position].Argument = UTF32_Clone(U32("\?"));
                            break;
                            case EscapeSequence_Octal:
                            Details->Specifiers[Position].OctalSeqSize = UTF32_GetNumDigits(Integer | Base8, Format, Details->Specifiers[Position].SpecifierOffset);
                            if (Details->Specifiers[Position].OctalSeqSize > 3) {
                                Details->Specifiers[Position].OctalSeqSize = 3;
                            }
                            Details->Specifiers[Position].Argument = UTF32_ExtractSubString(Format, Details->Specifiers[Position].SpecifierOffset, Details->Specifiers[Position].OctalSeqSize);
                            break;
                            case EscapeSequence_Hexadecimal:
                            Details->Specifiers[Position].Argument = UTF32_ExtractSubString(Format, Details->Specifiers[Position].SpecifierOffset, Details->Specifiers[Position].SpecifierLength - 2); // minus 2 to account for \x
                            break;
                            case EscapeSequence_Unicode4:
                            Details->Specifiers[Position].Argument = UTF32_ExtractSubString(Format, Details->Specifiers[Position].SpecifierOffset, 4);
                            break;
                            case EscapeSequence_Unicode8:
                            Details->Specifiers[Position].Argument = UTF32_ExtractSubString(Format, Details->Specifiers[Position].SpecifierOffset, 8);
                            break;
                            case EscapeSequence_Unknown:
                            Log(Log_DEBUG, __func__, U8("Unknown Escape Sequence: %d"), Details->Specifiers[Position].EscapeType);
                            break;
                    }
                } else if ((Base & BaseType_Character) == BaseType_Character) {
                        if ((Type & Modifier_UTF8) == Modifier_UTF8) {
                            UTF8  VariadicArgument                  = va_arg(VariadicArguments, UTF8);
                            UTF8 *String                            = UTF8_CodeUnit2String(VariadicArgument);
                            Details->Specifiers[Position].Argument = UTF8_Decode(String);
                            free(String);
                        } else if ((Type & Modifier_UTF16) == Modifier_UTF16) {
                            UTF16  VariadicArgument                 = va_arg(VariadicArguments, UTF16);
                            UTF16 *String                           = UTF16_CodeUnit2String(VariadicArgument);
                            Details->Specifiers[Position].Argument = UTF16_Decode(String);
                            free(String);
                        } else if ((Type & Modifier_UTF32) == Modifier_UTF32) {
                            UTF32  VariadicArgument                 = va_arg(VariadicArguments, UTF32);
                            UTF32 *String                           = UTF32_CodeUnit2String(VariadicArgument);
                            Details->Specifiers[Position].Argument  = String;
                            free(String);
                        }
                } else if ((Base & BaseType_String) == BaseType_String) {
                    if ((Type & Modifier_UTF8) == Modifier_UTF8) {
                        UTF8  *VariadicArgument                 = va_arg(VariadicArguments, UTF8*);
                        UTF32 *VariadicArgument32               = UTF8_Decode(VariadicArgument);
                        Details->Specifiers[Position].Argument = VariadicArgument32;
                    } else if ((Type & Modifier_UTF16) == Modifier_UTF16) {
                        UTF16 *VariadicArgument                 = va_arg(VariadicArguments, UTF16*);
                        UTF32 *VariadicArgument32               = UTF16_Decode(VariadicArgument);
                        Details->Specifiers[Position].Argument = VariadicArgument32;
                    } else if ((Type & Modifier_UTF32) == Modifier_UTF32) {
                        Details->Specifiers[Position].Argument = va_arg(VariadicArguments, UTF32*);
                    }
                } else if ((Base & BaseType_Integer) == BaseType_Integer) {
                    if ((Type & Modifier_Unsigned) == Modifier_Unsigned) {
                        if ((Details->Specifiers[Position].LengthModifier & Length_8Bit) == Length_8Bit) {
                            uint8_t  Arg                            = va_arg(VariadicArguments, uint8_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        } else if ((Details->Specifiers[Position].LengthModifier & Length_16Bit) == Length_16Bit) {
                            uint16_t  Arg                           = va_arg(VariadicArguments, uint16_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        } else if ((Details->Specifiers[Position].LengthModifier & Length_32Bit) == Length_32Bit) {
                            uint32_t  Arg                           = va_arg(VariadicArguments, uint32_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        } else if ((Details->Specifiers[Position].LengthModifier & Length_64Bit) == Length_64Bit) {
                            uint64_t  Arg                           = va_arg(VariadicArguments, uint64_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        }
                    } else if ((Type & Modifier_Signed) == Modifier_Signed) {
                        if ((Details->Specifiers[Position].LengthModifier & Length_8Bit) == Length_8Bit) {
                            int8_t  Arg                             = va_arg(VariadicArguments, int8_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        } else if ((Details->Specifiers[Position].LengthModifier & Length_16Bit) == Length_16Bit) {
                            int16_t  Arg                            = va_arg(VariadicArguments, int16_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        } else if ((Details->Specifiers[Position].LengthModifier & Length_32Bit) == Length_32Bit) {
                            int32_t  Arg                            = va_arg(VariadicArguments, int32_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        } else if ((Details->Specifiers[Position].LengthModifier & Length_64Bit) == Length_64Bit) {
                            int64_t  Arg                            = va_arg(VariadicArguments, int64_t);
                            Details->Specifiers[Position].Argument = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                        }
                    }
                } else if ((Base & BaseType_Decimal) == BaseType_Decimal) {
                    if ((Details->Specifiers[Position].LengthModifier & Length_32Bit) == Length_32Bit) {
                        float    Arg                            = va_arg(VariadicArguments, float);
                        Details->Specifiers[Position].Argument = UTF32_Decimal2String(ConvertTypeModifier2Base(Type), (double) Arg);
                    } else if ((Details->Specifiers[Position].LengthModifier & Length_64Bit) == Length_64Bit) {
                        double   Arg                            = va_arg(VariadicArguments, double);
                        Details->Specifiers[Position].Argument = UTF32_Decimal2String(ConvertTypeModifier2Base(Type), Arg);
                    }
                }
            }
            
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                UTF32    *OriginalTemp = FormatTemp;
                UTF32    *Argument     = NULL;
                uint64_t  Position     = 0ULL;
                if ((Details->Specifiers[Specifier].BaseType & BaseType_Positional) != BaseType_Positional) {
                    Position           = Specifier;
                } else {
                    Position           = Details->Specifiers[Specifier].PositionalArg;
                }
                uint64_t  Offset       = Details->Specifiers[Position].SpecifierOffset;
                uint64_t  Length       = Details->Specifiers[Position].SpecifierLength;
                Argument               = Details->Specifiers[Position].Argument;
                
                FormatTemp             = UTF32_ReplaceSubString(FormatTemp, Argument, Offset, Length);
                
                if (Specifier + 1 < Details->NumSpecifiers) {
                    if ((Details->Specifiers[Specifier + 1].BaseType & BaseType_Positional) == BaseType_Positional) {
                        uint64_t Position     = Details->Specifiers[Specifier + 1].PositionalArg;
                        uint64_t ArgumentSize = UTF32_GetStringSizeInCodePoints(Details->Specifiers[Position].Argument);
                        uint64_t Length       = Details->Specifiers[Position].SpecifierLength;
                        Details->Specifiers[Specifier + 1].SpecifierOffset += Minimum(Length, Details->Specifiers[Specifier + 1].SpecifierOffset);
                    }
                }
                
                if (OriginalTemp != Format) {
                    free(OriginalTemp);
                }
            }
            Formatted                  = FormatTemp;
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("Format Pointer is NULL"));
        } else if (Details == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
        return Formatted;
    }
    
    UTF8 *UTF8_Format(UTF8 *Format, ...) {
        UTF8 *Format8                     = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers        = UTF8_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32           = UTF8_Decode(Format);
                FormatSpecifiers *Details = UTF32_ParseFormatSpecifiers(Format32, NumSpecifiers, UTF8Format);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                UTF32 *FormattedString    = FormatString_UTF32(Format32, Details, VariadicArguments);
                va_end(VariadicArguments);
                free(Format32);
                FormatSpecifiers_Deinit(Details);
                Format8                   = UTF8_Encode(FormattedString);
                free(FormattedString);
            } else {
                Format8                   = Format;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Format8;
    }
    
    UTF16 *UTF16_Format(UTF16 *Format, ...) {
        UTF16 *Format16                   = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers        = UTF16_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32           = UTF16_Decode(Format);
                FormatSpecifiers *Details = UTF32_ParseFormatSpecifiers(Format32, NumSpecifiers, UTF16Format);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                UTF32 *FormattedString    = FormatString_UTF32(Format32, Details, VariadicArguments);
                va_end(VariadicArguments);
                free(Format32);
                FormatSpecifiers_Deinit(Details);
                Format16                  = UTF16_Encode(FormattedString);
                free(FormattedString);
            } else {
                Format16                  = Format;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Format16;
    }
    
    UTF32 *UTF32_Format(UTF32 *Format, ...) {
        UTF32 *FormattedString            = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers        = UTF32_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Details = UTF32_ParseFormatSpecifiers(Format, NumSpecifiers, UTF32Format);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                FormattedString           = FormatString_UTF32(Format, Details, VariadicArguments);
                va_end(VariadicArguments);
                FormatSpecifiers_Deinit(Details);
            } else {
                FormattedString           = Format;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return FormattedString;
    }
    
    /* StringArray Functions */
    UTF8 **UTF8_StringArray_Init(uint64_t NumStrings) {
        UTF8 **StringArray = NULL;
        if (NumStrings > 0) {
            StringArray    = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(UTF8*));
        } else {
            Log(Log_DEBUG, __func__, U8("NumStrings %llu is invalid"), NumStrings);
        }
        return StringArray;
    }
    
    UTF16 **UTF16_StringArray_Init(uint64_t NumStrings) {
        UTF16 **StringArray = NULL;
        if (NumStrings > 0) {
            StringArray    = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(UTF16*));
        } else {
            Log(Log_DEBUG, __func__, U8("NumStrings %llu is invalid"), NumStrings);
        }
        return StringArray;
    }
    
    UTF32 **UTF32_StringArray_Init(uint64_t NumStrings) {
        UTF32 **StringArray = NULL;
        if (NumStrings > 0) {
            StringArray    = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(UTF32*));
        } else {
            Log(Log_DEBUG, __func__, U8("NumStrings %llu is invalid"), NumStrings);
        }
        return StringArray;
    }
    
    void UTF8_StringArray_Attach(UTF8 **StringArray, UTF8 *String2Attach, uint64_t Index) {
        if (StringArray != NULL && String2Attach != NULL) { // We can't actually see if there's enough room to attach it because it's all null
            StringArray[Index] = String2Attach;
        } else if (StringArray == NULL) {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Log_DEBUG, __func__, U8("String2Attach Pointer is NULL"));
        }
    }
    
    void UTF16_StringArray_Attach(UTF16 **StringArray, UTF16 *String2Attach, uint64_t Index) {
        if (StringArray != NULL && String2Attach != NULL) { // We can't actually see if there's enough room to attach it because it's all null
            StringArray[Index] = String2Attach;
        } else if (StringArray == NULL) {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Log_DEBUG, __func__, U8("String2Attach Pointer is NULL"));
        }
    }
    
    void UTF32_StringArray_Attach(UTF32 **StringArray, UTF32 *String2Attach, uint64_t Index) {
        if (StringArray != NULL && String2Attach != NULL) { // We can't actually see if there's enough room to attach it because it's all null
            StringArray[Index] = String2Attach;
        } else if (StringArray == NULL) {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Log_DEBUG, __func__, U8("String2Attach Pointer is NULL"));
        }
    }
    
    uint64_t UTF8_StringArray_GetNumStrings(UTF8 **StringArray) {
        uint64_t NumStrings = 0ULL;
        if (StringArray != NULL) {
            while (StringArray[NumStrings] != FoundationIONULLTerminator) {
                NumStrings += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t UTF16_StringArray_GetNumStrings(UTF16 **StringArray) {
        uint64_t NumStrings = 0ULL;
        if (StringArray != NULL) {
            while (StringArray[NumStrings] != FoundationIONULLTerminator) {
                NumStrings += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t UTF32_StringArray_GetNumStrings(UTF32 **StringArray) {
        uint64_t NumStrings = 0ULL;
        if (StringArray != NULL) {
            while (StringArray[NumStrings] != FoundationIONULLTerminator) {
                NumStrings += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t **UTF8_StringArray_GetStringSizesInCodeUnits(UTF8 **StringArray) {
        uint64_t **StringArraySizes       = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings          = UTF8_StringArray_GetNumStrings(StringArray);
            StringArraySizes             = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringArraySizes[String] = UTF8_GetStringSizeInCodeUnits(StringArray[String]);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return StringArraySizes;
    }
    
    uint64_t **UTF16_StringArray_GetStringSizesInCodeUnits(UTF16 **StringArray) {
        uint64_t **StringArraySizes       = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings          = UTF16_StringArray_GetNumStrings(StringArray);
            StringArraySizes             = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringArraySizes[String] = UTF16_GetStringSizeInCodeUnits(StringArray[String]);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return StringArraySizes;
    }
    
    uint64_t **UTF8_StringArray_GetStringSizesInCodePoints(UTF8 **StringArray) {
        uint64_t **StringArraySizes       = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings          = UTF8_StringArray_GetNumStrings(StringArray);
            StringArraySizes             = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringArraySizes[String] = UTF8_GetStringSizeInCodePoints(StringArray[String]);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return StringArraySizes;
    }
    
    uint64_t **UTF16_StringArray_GetStringSizesInCodePoints(UTF16 **StringArray) {
        uint64_t **StringArraySizes       = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings          = UTF16_StringArray_GetNumStrings(StringArray);
            StringArraySizes             = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringArraySizes[String] = UTF16_GetStringSizeInCodePoints(StringArray[String]);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return StringArraySizes;
    }
    
    uint64_t **UTF32_StringArray_GetStringSizesInCodePoints(UTF32 **StringArray) {
        uint64_t **StringArraySizes       = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings          = UTF32_StringArray_GetNumStrings(StringArray);
            StringArraySizes             = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringArraySizes[String] = UTF32_GetStringSizeInCodePoints(StringArray[String]);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return StringArraySizes;
    }
    
    UTF32 **UTF8_StringArray_Decode(UTF8 **StringArray) {
        UTF32 **Decoded             = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings     = UTF8_StringArray_GetNumStrings(StringArray);
            Decoded                 = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(UTF32*));
            if (Decoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Decoded[String] = UTF8_Decode(StringArray[String]);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate decoded StringArray"));
            }
            
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return Decoded;
    }
    
    UTF32 **UTF16_StringArray_Decode(UTF16 **StringArray) {
        UTF32 **Decoded             = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings     = UTF16_StringArray_GetNumStrings(StringArray);
            Decoded                 = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(UTF32*));
            if (Decoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Decoded[String] = UTF16_Decode(StringArray[String]);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate decoded StringArray"));
            }
            
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return Decoded;
    }
    
    UTF8 **UTF8_StringArray_Encode(UTF32 **StringArray) {
        UTF8 **Encoded              = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings     = UTF32_StringArray_GetNumStrings(StringArray);
            Encoded                 = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(UTF8*));
            if (Encoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Encoded[String] = UTF8_Encode(StringArray[String]);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate decoded StringArray"));
            }
            
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return Encoded;
    }
    
    UTF16 **UTF16_StringArray_Encode(UTF32 **StringArray) {
        UTF16 **Encoded             = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings     = UTF32_StringArray_GetNumStrings(StringArray);
            Encoded                 = calloc(NumStrings + FoundationIONULLTerminatorSize, sizeof(UTF16*));
            if (Encoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Encoded[String] = UTF16_Encode(StringArray[String]);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate decoded StringArray"));
            }
            
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
        return Encoded;
    }
    
    void UTF8_StringArray_Deinit(UTF8 **StringArray) {
        if (StringArray != NULL) {
            uint64_t NumStrings = UTF8_StringArray_GetNumStrings(StringArray);
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                free(StringArray[String]);
            }
            free(StringArray);
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
    }
    
    void UTF16_StringArray_Deinit(UTF16 **StringArray) {
        if (StringArray != NULL) {
            uint64_t NumStrings = UTF16_StringArray_GetNumStrings(StringArray);
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                free(StringArray[String]);
            }
            free(StringArray);
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
    }
    
    void UTF32_StringArray_Deinit(UTF32 **StringArray) {
        if (StringArray != NULL) {
            uint64_t NumStrings = UTF32_StringArray_GetNumStrings(StringArray);
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                free(StringArray[String]);
            }
            free(StringArray);
        } else {
            Log(Log_DEBUG, __func__, U8("StringArray Pointer is NULL"));
        }
    }
    
    static UTF32 **DeformatString_UTF32(UTF32 *Format, UTF32 *Result, FormatSpecifiers *Details) {
        UTF32 **Deformatted                            = NULL;
        uint64_t NumStrings                            = 0ULL;
        for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
            if (Details->Specifiers[Specifier].BaseType != BaseType_Literal && Details->Specifiers[Specifier].TypeModifier != Modifier_Percent) {
                NumStrings                            += 1;
            }
        }
        Deformatted                                    = UTF32_StringArray_Init(NumStrings);
        if (Deformatted != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) { // Stringify each specifier
                FormatSpecifier_BaseTypes     BaseType = Details->Specifiers[Specifier].BaseType;
                FormatSpecifier_TypeModifiers Modifier = Details->Specifiers[Specifier].TypeModifier;
                uint64_t                      Offset   = Details->Specifiers[Specifier].SpecifierOffset;
                uint64_t                      Length   = 0ULL;
                if (BaseType == BaseType_Integer || BaseType == BaseType_Decimal) {
                    StringIOBases             Base     = ConvertTypeModifier2Base(Modifier);
                    Length                             = UTF32_GetNumDigits(Base, Result, Offset);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Result, Offset, Length);
                    Length                             = 0ULL;
                } else if (BaseType == BaseType_Character) { // Character = Grapheme
                    Deformatted[Specifier]             = UTF32_ExtractGrapheme(Result, Offset);
                } else if (BaseType == BaseType_String) {
                    uint64_t SubstringStart            = Details->Specifiers[Specifier].SpecifierOffset + Details->Specifiers[Specifier].SpecifierLength;
                    uint64_t SubstringEnd              = Details->Specifiers[Specifier].SpecifierOffset;
                    
                    UTF32 *SubString                   = UTF32_ExtractSubString(Format, SubstringStart, SubstringEnd);
                    uint64_t EndOffset                 = UTF32_FindSubString(Result, SubString, Details->Specifiers[Specifier].SpecifierOffset, -1);
                    free(SubString);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Result, SubstringStart, EndOffset);
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
        return Deformatted;
    }
    
    UTF8 **UTF8_Deformat(UTF8 *Format, UTF8 *Result) {
        UTF8 **StringArray                = NULL;
        if (Format != NULL && Result != NULL) {
            uint64_t NumSpecifiers        = UTF8_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32           = UTF8_Decode(Format);
                FormatSpecifiers *Details = UTF32_ParseFormatSpecifiers(Format32, NumSpecifiers, UTF8Format);
                UTF32 *Result32           = UTF8_Decode(Result);
                UTF32 **Strings32         = DeformatString_UTF32(Format32, Result32, Details);
                free(Format32);
                free(Result32);
                FormatSpecifiers_Deinit(Details);
                StringArray               = UTF8_StringArray_Encode(Strings32);
                free(Strings32);
            } else {
                UTF8  *SourceClone        = UTF8_Clone(Result);
                StringArray               = UTF8_StringArray_Init(1);
                StringArray[0]            = SourceClone;
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("Format Pointer is NULL"));
        } else if (Result == NULL) {
            Log(Log_DEBUG, __func__, U8("Result String is NULL"));
        }
        return StringArray;
    }
    
    UTF16 **UTF16_Deformat(UTF16 *Format, UTF16 *Result) {
        UTF16 **StringArray               = NULL;
        if (Format != NULL && Result != NULL) {
            uint64_t NumSpecifiers        = UTF16_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32           = UTF16_Decode(Format);
                FormatSpecifiers *Details = UTF32_ParseFormatSpecifiers(Format32, NumSpecifiers, UTF16Format);
                UTF32 *Result32           = UTF16_Decode(Result);
                UTF32 **Strings32         = DeformatString_UTF32(Format32, Result32, Details);
                free(Format32);
                free(Result32);
                FormatSpecifiers_Deinit(Details);
                StringArray               = UTF16_StringArray_Encode(Strings32);
                free(Strings32);
            } else {
                UTF16 *SourceClone        = UTF16_Clone(Result);
                StringArray               = UTF16_StringArray_Init(1);
                StringArray[0]            = SourceClone;
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("Format Pointer is NULL"));
        } else if (Result == NULL) {
            Log(Log_DEBUG, __func__, U8("Result String is NULL"));
        }
        return StringArray;
    }
    
    UTF32 **UTF32_Deformat(UTF32 *Format, UTF32 *Result) {
        UTF32 **StringArray               = NULL;
        if (Format != NULL && Result != NULL) {
            uint64_t NumSpecifiers        = UTF32_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Details = UTF32_ParseFormatSpecifiers(Format, NumSpecifiers, UTF32Format);
                StringArray               = DeformatString_UTF32(Format, Result, Details);
                FormatSpecifiers_Deinit(Details);
            } else {
                StringArray               = UTF32_StringArray_Init(1);
                StringArray[0]            = Result;
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("Format Pointer is NULL"));
        } else if (Result == NULL) {
            Log(Log_DEBUG, __func__, U8("Result String is NULL"));
        }
        return StringArray;
    }
    /* StringArray Functions */
    
#ifdef __cplusplus
}
#endif
