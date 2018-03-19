#include <stdarg.h>                    /* Included for va_start, va_end */
#include <stdint.h>                    /* Included for u/intX_t */
#include <stdlib.h>                    /* Included for calloc, and free */
#include <float.h>                     /* Included for */
#include <limits.h>                    /* Included for */

#include "../include/Macros.h"         /* Included for NewLineWithNULLSize */
#include "../include/Math.h"           /* Included for Integer functions */
#include "../include/StringIOTables.h" /* Included for the tables, and StringIO.h */
#include "../include/Log.h"            /* Included for error logging */

#ifdef  __cplusplus
extern  "C" {
#endif
    
    /* UTF-8 I/O */
    static uint8_t UTF8_GetCodeUnitSize(UTF8 CodeUnit) {
        uint8_t CodePointSize      = 0;
        if (((CodeUnit & 0x80) >> 7) == 0) {
            CodePointSize          = 1;
        } else if (((CodeUnit & 0xE0) >> 5) == 6) {
            CodePointSize          = 2;
        } else if (((CodeUnit & 0xF0) >> 4) == 14) {
            CodePointSize          = 3;
        } else if (((CodeUnit & 0xF8) >> 3) == 30) {
            CodePointSize          = 4;
        }
        return CodePointSize;
    }
    
    uint64_t UTF8_GetSizeInCodeUnits(UTF8 *String2Count) {
        uint64_t NumCodeUnits                       = 0ULL;
        uint64_t CurrentCodeUnit                    = 0ULL;
        if (String2Count != NULL) {
            do {
                NumCodeUnits                       += UTF8_GetCodeUnitSize(String2Count[CurrentCodeUnit]);
            } while (String2Count[CurrentCodeUnit] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodeUnits;
    }
    
    uint64_t UTF8_GetSizeInCodePoints(UTF8 *String) {
        uint64_t NumCodePoints                = 0ULL;
        uint64_t CurrentCodeUnit              = 0ULL;
        if (String != NULL) {
            do {
                CurrentCodeUnit              += UTF8_GetCodeUnitSize(String[CurrentCodeUnit]);
                NumCodePoints                += 1;
            } while (String[CurrentCodeUnit] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    static uint8_t UTF16_GetCodeUnitSize(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit >= UTF16HighSurrogateStart && CodeUnit <= UTF16LowSurrogateEnd) {
            CodePointSize     = 2;
        } else {
            CodePointSize     = 1;
        }
        return CodePointSize;
    }
    
    uint64_t UTF16_GetSizeInCodeUnits(UTF16 *String) {
        uint64_t NumCodeUnits              = 0ULL;
        if (String != NULL) {
            do {
                NumCodeUnits              += UTF16_GetCodeUnitSize(String[NumCodeUnits]);
            } while (String[NumCodeUnits] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodeUnits;
    }
    
    uint64_t UTF16_GetSizeInCodePoints(UTF16 *String) {
        uint64_t NumCodeUnits               = 0ULL;
        uint64_t NumCodePoints              = 0ULL;
        if (String != NULL) {
            do {
                NumCodeUnits               += UTF16_GetCodeUnitSize(String[NumCodeUnits]);
                NumCodePoints              += 1;
            } while (String[NumCodePoints] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF32_GetSizeInUTF8CodeUnits(UTF32 *String) {
        uint64_t CodePoint              = 0ULL;
        uint64_t UTF8CodeUnits          = 0ULL;
        if (String != NULL) {
            do {
                if (String[CodePoint] > UnicodeMaxCodePoint || (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd)) {
                    String[CodePoint]   = InvalidCodePointReplacementCharacter;
                } else if (String[CodePoint] <= 0x7F) {
                    UTF8CodeUnits      += 1;
                } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                    UTF8CodeUnits      += 2;
                } else if (String[CodePoint] >= 0x800 && String[CodePoint] <= UTF16MaxCodePoint) {
                    UTF8CodeUnits      += 3;
                } else if (String[CodePoint] > UTF16MaxCodePoint && String[CodePoint] <= UnicodeMaxCodePoint) {
                    UTF8CodeUnits      += 4;
                }
                CodePoint              += 1;
            } while (String[CodePoint] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return UTF8CodeUnits;
    }
    
    uint64_t UTF32_GetSizeInUTF16CodeUnits(UTF32 *String) {
        uint64_t CodePoint              = 0ULL;
        uint64_t UTF16CodeUnits         = 0ULL;
        if (String != NULL) {
            do {
                if (String[CodePoint] > UTF16MaxCodePoint) {
                    UTF16CodeUnits     += 2;
                } else {
                    UTF16CodeUnits     += 1;
                }
                CodePoint              += 1;
            } while (String[CodePoint] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return UTF16CodeUnits;
    }
    
    uint64_t UTF32_GetSizeInCodePoints(UTF32 *String) {
        uint64_t NumCodePoints              = 0ULL;
        if (String != NULL) {
            do {
                NumCodePoints              += 1;
            } while (String[NumCodePoints] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    UTF32 *UTF8_Decode(UTF8 *String) {
        uint8_t  CodePointSize      = 0;
        uint64_t CodeUnitNum        = 0ULL;
        uint64_t CodePoint          = 0ULL;
        UTF32   *DecodedString      = NULL;
        if (String != NULL) {
            uint64_t NumCodePoints  = UTF1632BOMSizeInCodePoints + UTF8_GetSizeInCodePoints(String) + UnicodeNULLTerminatorSize;
            if (String[0] == 0xEF && String[1] == 0xBB && String[2] == 0xBF) { // We need to remove the BOM, and subtract it's size from the Num codepoints
                NumCodePoints      -= UTF8BOMSizeInCodePoints;
                CodeUnitNum        += UTF8BOMSizeInCodeUnits;
            }
            DecodedString           = calloc(NumCodePoints, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if (CodePoint == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            DecodedString[0] = UTF32LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            DecodedString[0] = UTF32BE;
                        }
                    }
                    switch (UTF8_GetCodeUnitSize(String[CodeUnitNum])) {
                        case 1:
                            DecodedString[CodePoint] =  String[CodeUnitNum - 1];
                            CodeUnitNum             += 1;
                            break;
                        case 2:
                            DecodedString[CodePoint] = (String[CodeUnitNum - 1] & 0x1F) << 6;
                            DecodedString[CodePoint] =  String[CodeUnitNum]     & 0x3F;
                            CodeUnitNum             += 2;
                            break;
                        case 3:
                            DecodedString[CodePoint] = (String[CodeUnitNum - 1] & 0x0F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x1F) << 6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x1F);
                            CodeUnitNum             += 3;
                            break;
                        case 4:
                            DecodedString[CodePoint] = (String[CodeUnitNum - 1] & 0x07) << 18;
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x3F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x3F) <<  6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 2] & 0x3F);
                            CodeUnitNum             += 4;
                            break;
                    }
                    if (CodePointSize > 1 && DecodedString[CodePoint] <= 0x7F) {
                        DecodedString[CodePoint]     = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("CodePoint U+%06X is overlong, replaced with U+FFFD"), CodePoint);
                    } else if (DecodedString[CodePoint] >= UTF16HighSurrogateStart && DecodedString[CodePoint] <= UTF16LowSurrogateEnd && DecodedString[CodePoint] != UTF32LE && DecodedString[CodePoint] != UTF32BE) {
                        DecodedString[CodePoint]     = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("Codepoint U+%06X is invalid, overlaps Surrogate Pair Block, replaced with U+FFFD"), CodePoint);
                    }
                    if (GlobalByteOrder == LSByteFirst) {
                        DecodedString[CodePoint]     = SwapEndian32(String[CodePoint]);
                    }
                } while (String[CodeUnitNum] != UnicodeNULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate DecodedString"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF8 *UTF8_Encode(UTF32 *String, const bool IncludeBOM) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        uint64_t CodePoint                             = 0ULL;
        uint64_t CodeUnitNum                           = 0ULL;
        UTF8    *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t UTF8CodeUnits                     = UnicodeNULLTerminatorSize + UTF32_GetSizeInUTF8CodeUnits(String) + (IncludeBOM == true ? UTF8BOMSizeInCodeUnits : 0);
            EncodedString                              = calloc(UTF8CodeUnits, sizeof(UTF8));
            if (EncodedString != NULL) {
                do {
                    if ((String[0] == UTF32LE && GlobalByteOrder == MSByteFirst) || (String[0] == UTF32BE && GlobalByteOrder == LSByteFirst)) {
                        String[CodePoint]               = SwapEndian32(String[CodePoint]);
                    }
                    if (CodeUnitNum == 0 && IncludeBOM == Yes) {
                        EncodedString[CodeUnitNum]      = 0xEF;
                        EncodedString[CodeUnitNum += 1] = 0xBB;
                        EncodedString[CodeUnitNum += 2] = 0xBF;
                        CodeUnitNum                    += UTF8BOMSizeInCodeUnits;
                    }
                    if (String[CodePoint] <= 0x7F) {
                        EncodedString[CodeUnitNum]      = String[CodePoint];
                        CodeUnitNum                    += 1;
                    } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                        EncodedString[CodeUnitNum]      = (0xC0 & ((String[CodePoint] & 0x7C0) >> 6));
                        EncodedString[CodeUnitNum + 1]  = (0x80 &  (String[CodePoint] & 0x03F));
                        CodeUnitNum                    += 2;
                    }  else if (String[CodePoint] >= 0x800 && String[CodePoint] <= UTF16MaxCodePoint && String[CodePoint] < UTF16HighSurrogateStart && String[CodePoint] > UTF16LowSurrogateEnd && String[CodePoint] != UTF16LE && String[CodePoint] != UTF16BE) {
                        EncodedString[CodeUnitNum]      = (0xE0 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 1]  = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 2]  = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                    += 3;
                    } else if (String[CodePoint] > UTF16MaxCodePoint && String[CodePoint] <= UnicodeMaxCodePoint) {
                        EncodedString[CodeUnitNum]      = (0xF0 & ((String[CodePoint] & 0x1C0000) >> 18));
                        EncodedString[CodeUnitNum + 1]  = (0x80 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 2]  = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 3]  = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                    += 4;
                    } else if (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd && String[CodePoint] != UTF32LE && String[CodePoint] != UTF32BE) {
                        String[CodePoint]               = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD"));
                    }
                } while (String[CodePoint] != UnicodeNULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("String Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF32 *UTF16_Decode(UTF16 *String) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        uint64_t CodePoint                       = 0ULL;
        uint64_t CodeUnitNum                     = 0ULL;
        UTF16    StringByteOrder                 = 0U;
        UTF32   *DecodedString                   = NULL;
        if (String != NULL) {
            StringByteOrder                      = String[0];
            uint64_t UTF16CodeUnits              = UTF16_GetSizeInCodePoints(String) + UnicodeNULLTerminatorSize;
            DecodedString                        = calloc(UTF16CodeUnits, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if ((StringByteOrder == UTF16LE && GlobalByteOrder == MSByteFirst) || (StringByteOrder == UTF16BE && GlobalByteOrder == LSByteFirst)) {
                        String[CodePoint]        = SwapEndian16(String[CodePoint]);
                        if (CodePoint == 0) {
                            if (GlobalByteOrder == LSByteFirst) {
                                DecodedString[0] = UTF16LE;
                            } else if (GlobalByteOrder == MSByteFirst) {
                                DecodedString[0] = UTF16BE;
                            }
                        }
                    }
                    if (String[CodeUnitNum] <= UTF16HighSurrogateStart - 1 || (String[CodeUnitNum] >= UTF16LowSurrogateEnd + 1 && String[CodeUnitNum] <= UTF16MaxCodePoint)) {
                        DecodedString[CodePoint] = String[CodeUnitNum];
                    } else {
                        uint16_t HighSurrogate   = (String[CodeUnitNum]     - UTF16HighSurrogateStart) * UTF16SurrogatePairModDividend;
                        uint16_t LowSurrogate    =  String[CodeUnitNum + 1] - UTF16LowSurrogateStart;
                        DecodedString[CodePoint] = HighSurrogate + LowSurrogate + UTF16SurrogatePairStart;
                    }
                } while (String[CodePoint] != UnicodeNULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("DecodedString Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF16 *UTF16_Encode(UTF32 *String, UnicodeByteOrder OutputByteOrder) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        UTF16   *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t CodeUnitNum                       = 0ULL;
            uint64_t UTF16NumCodeUnits                 = UTF1632BOMSizeInCodePoints + UTF32_GetSizeInUTF16CodeUnits(String) + UnicodeNULLTerminatorSize;
            EncodedString                              = calloc(UTF16NumCodeUnits, sizeof(UTF16));
            if (EncodedString != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < UTF16NumCodeUnits; CodeUnit++) {
                    if ((String[0] == UTF16LE && GlobalByteOrder == MSByteFirst) || (String[0] == UTF16BE && GlobalByteOrder == LSByteFirst)) {
                        String[CodeUnit]               = SwapEndian32(String[CodeUnit]);
                    }
                    if (CodeUnit == 0) {
                        if (OutputByteOrder == UseNativeByteOrder) {
                            if (GlobalByteOrder == LSByteFirst) {
                                EncodedString[0]       = UTF16LE;
                            } else if (GlobalByteOrder == MSByteFirst) {
                                EncodedString[0]       = UTF16BE;
                            }
                        } else if (OutputByteOrder == UseBEByteOrder) {
                            EncodedString[0]           = UTF16BE;
                        } else if (OutputByteOrder == UseLEByteOrder) {
                            EncodedString[0]           = UTF16LE;
                        }
                    }
                    if (String[CodeUnitNum] < UTF16HighSurrogateStart || (String[CodeUnitNum] > UTF16LowSurrogateEnd && String[CodeUnitNum] <= UTF16MaxCodePoint)) { // Single code point
                        EncodedString[CodeUnitNum]     = String[CodeUnit];
                    } else {
                        uint16_t HighSurrogate         = ((String[CodeUnitNum] - UTF16SurrogatePairStart) / UTF16SurrogatePairModDividend) + UTF16HighSurrogateStart;
                        uint16_t LowSurrogate          = ((String[CodeUnitNum] - UTF16SurrogatePairStart) % UTF16SurrogatePairModDividend) + UTF16LowSurrogateStart;
                        if ((OutputByteOrder == UseLEByteOrder && GlobalByteOrder == MSByteFirst) || (OutputByteOrder == UseBEByteOrder && GlobalByteOrder == LSByteFirst)) {
                            HighSurrogate              = SwapEndian16(HighSurrogate);
                            LowSurrogate               = SwapEndian16(LowSurrogate);
                        }
                        EncodedString[CodeUnitNum]     = HighSurrogate;
                        EncodedString[CodeUnitNum + 1] = LowSurrogate;
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("EncodedString Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    void UTF16_ConvertByteOrder(UTF16 *String2Convert, UnicodeByteOrder OutputByteOrder) {
        uint16_t UTF16ByteOrder                 = String2Convert[0];
        uint64_t CurrentCodeUnit                = 1LL;
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        if (String2Convert != NULL && OutputByteOrder != UseNativeByteOrder) {
            do {
                String2Convert[CurrentCodeUnit] = SwapEndian16(String2Convert[CurrentCodeUnit]);
            } while (String2Convert[CurrentCodeUnit] != UnicodeNULLTerminator && UTF16ByteOrder != OutputByteOrder);
        }
    }
    
    void UTF32_ConvertByteOrder(UTF32 *String2Convert, UnicodeByteOrder OutputByteOrder) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        if (String2Convert != NULL && OutputByteOrder != UseNativeByteOrder) {
            uint32_t         UTF32ByteOrder                = String2Convert[0];
            uint64_t         CurrentCodePoint              = 1ULL;
            do {
                String2Convert[CurrentCodePoint]           = SwapEndian32(String2Convert[CurrentCodePoint]);
            } while (String2Convert[CurrentCodePoint] != UnicodeNULLTerminator && UTF32ByteOrder != OutputByteOrder);
        }
    }
    
    void UTF32_NormalizeString(UTF32 *String2Normalize) {
        uint64_t CodePoint    = 1ULL; // Skip the BOM
        if (String2Normalize != NULL) {
            do {
            } while (String2Normalize[CodePoint] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String2Normalize Pointer is NULL"));
        }
    }
    
    void UTF32_CaseFoldString(UTF32 *String) {
        uint64_t CodePoint = 1ULL;
        if (String != NULL) {
            do {
            } while (String[CodePoint] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
    }
    
    bool UTF8_Compare(UTF8 *String1, UTF8 *String2, bool Normalize, bool CaseInsensitive) {
        bool StringsMatch       = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32 = UTF8_Decode(String1);
            UTF32 *String2UTF32 = UTF8_Decode(String2);
            if (Normalize == Yes) {
                UTF32_NormalizeString(String1UTF32);
                UTF32_NormalizeString(String2UTF32);
            }
            if (CaseInsensitive == Yes) {
                UTF32_CaseFoldString(String1UTF32);
                UTF32_CaseFoldString(String2UTF32);
            }
            StringsMatch        = UTF32_Compare(String1UTF32, String2UTF32);
            free(String1UTF32);
            free(String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF16_Compare(UTF16 *String1, UTF16 *String2, bool Normalize, bool CaseInsensitive) {
        bool StringsMatch       = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32 = UTF16_Decode(String1);
            UTF32 *String2UTF32 = UTF16_Decode(String2);
            if (Normalize == Yes) {
                UTF32_NormalizeString(String1UTF32);
                UTF32_NormalizeString(String2UTF32);
            }
            if (CaseInsensitive == Yes) {
                UTF32_CaseFoldString(String1UTF32);
                UTF32_CaseFoldString(String2UTF32);
            }
            StringsMatch        = UTF32_Compare(String1UTF32, String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF32_Compare(UTF32 *String1, UTF32 *String2) {
        uint64_t CodePoint           = 0ULL;
        bool StringsMatch            = Yes;
        if (String1 != NULL && String2 != NULL) {
            uint64_t String1Size     = UTF32_GetSizeInCodePoints(String1);
            uint64_t String2Size     = UTF32_GetSizeInCodePoints(String2);
            if (String1Size != String2Size) {
                StringsMatch         = No;
            } else {
                do {
                    if (String1[CodePoint] != String2[CodePoint]) {
                        StringsMatch = No;
                        break;
                    }
                } while (String1[CodePoint] != UnicodeNULLTerminator && String2[CodePoint] != UnicodeNULLTerminator);
            }
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    UTF32 *UTF32_StripWhiteSpace(UTF32 *String) {
        uint64_t CodePoint                                    = 0ULL;
        uint64_t StrippedCodePoint                            = 0ULL;
        uint64_t ValidCodePoints                              = 0ULL + UTF1632BOMSizeInCodePoints + UnicodeNULLTerminatorSize;
        UTF32   *StrippedString                               = NULL;
        if (String != NULL) {
            do {
                for (uint64_t Whitespace = 0ULL; Whitespace < WhiteSpaceTableSize; Whitespace++) {
                    if (String[CodePoint] != WhiteSpaceTable[Whitespace] && String[CodePoint] != InvalidCodePointReplacementCharacter) {
                        ValidCodePoints                      += 1;
                    }
                }
            } while (String[CodePoint] != UnicodeNULLTerminatorSize && String[CodePoint] != InvalidCodePointReplacementCharacter);
            StrippedString                                    = calloc(ValidCodePoints, sizeof(UTF32));
            if (StrippedString != NULL) {
                do {
                    for (uint64_t Whitespace = 0ULL; Whitespace < WhiteSpaceTableSize; Whitespace++) {
                        if (String[CodePoint] != WhiteSpaceTable[Whitespace]) {
                            StrippedString[StrippedCodePoint] = String[CodePoint];
                        }
                    }
                } while (String[CodePoint] != UnicodeNULLTerminatorSize && String[CodePoint] != InvalidCodePointReplacementCharacter);
            }
        }
        return StrippedString;
    }
    
    int64_t UTF32_String2Integer(UTF32 *String) {
        uint64_t CodePoint =  0ULL;
        int8_t   Sign      =  1;
        uint8_t  Base      = 10;
        int64_t  Value     =  0;
        if (String != NULL) {
            /*
             for (uint8_t WhiteSpace = 0; WhiteSpace < WhitespaceTableSize; WhiteSpace++) {
             if (String[CodePoint] == WhitespaceTable[WhiteSpace]) {
             CodePoint    += 1;
             }
             }
             */
            if (String[CodePoint] == '0') {
                if (String[CodePoint + 1] == 'b' || String[CodePoint + 1] == 'B') {
                    Base   = 2;
                } else if (String[CodePoint + 1] == 'o' || String[CodePoint] == 'O') {
                    Base   = 8;
                } else if (String[CodePoint + 1] == 'x' || String[CodePoint] == 'X') {
                    Base   = 16;
                }
            } else if (String[CodePoint] == '-') {
                Sign       = -1;
            }
            do {
                Value     *= Base;
                if (Base == 16 && (String[CodePoint] >= 0x41 && String[CodePoint] <= 0x46)) {
                    Value += (String[CodePoint] - 50);
                } else if (Base == 16 && (String[CodePoint] >= 0x61 && String[CodePoint] <= 0x66)) {
                    Value += (String[CodePoint] - 82);
                } else if ((Base == 10 || Base == 16) && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39)) {
                    Value += (String[CodePoint] - 48);
                } else if (Base == 8 && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x37)) {
                    Value += (String[CodePoint] - 48);
                } else if (Base == 2 && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x31)) {
                    Value += (String[CodePoint] - 48);
                }
            } while (String[CodePoint] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        
        return (Sign < 0 ? Value * Sign : Value);
    }
    
    UTF32 *UTF32_Integer2String(const Number2StringBases Base, const bool UpperCase, int64_t Integer2Convert) {
        int64_t  Sign            = 0LL;
        int64_t  Num             = Integer2Convert;
        uint8_t  NumDigits       = 0;
        while (Num > 0) {
            Num                 /= Base;
            NumDigits           += 1;
        }
        if (GlobalByteOrder == LSByteFirst) {
            Integer2Convert      = SwapEndian64(Integer2Convert);
        }
        if (Integer2Convert < 0 && Base == Decimal) {
            Sign                 = -1;
            NumDigits           +=  1;
        }
        UTF32 *NumberString      = calloc(NumDigits + UnicodeNULLTerminatorSize, sizeof(UTF32));
        UTF32  UpperNumerals[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        UTF32  LowerNumerals[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        if (NumberString != NULL) {
            for (uint64_t CodePoint = NumDigits - 1; CodePoint > 0; CodePoint--) {
                // Ok, so we've clamped the looping, now all we need to do is malloc a string with NumDigits as it's size.
                // Now we just AND the number with Bit to get it's value, if the value is a 1 write a 1, otherwise write a zero
                int64_t CurrentDigit    = (Base == Decimal ? Absolute(Integer2Convert %= Base) : (Integer2Convert %= Base));
                NumberString[CodePoint] = (UpperCase == Yes ? UpperNumerals[CurrentDigit] : LowerNumerals[CurrentDigit]);
            }
        }
        return NumberString;
    }
    
    double String2Decimal(UTF32 *String) { // Replaces strtod and atof
        /*
         Invalid strings result in 0.0 since Mac doesn't support NaN.
         */
        uint64_t CodePoint = 0;
        if (String != NULL) {
            // Extract the sign bit
            // Extract the Significand
            // Extract the Base
            // Extract the Exponent
            bool IsNegative        = 0;
            for (uint8_t Whitespace = 0; Whitespace < 99; Whitespace++) {
                do {
                    if (String[CodePoint] == '-') {
                        IsNegative = Yes;
                    }
                    CodePoint     += 1;
                } while (String[CodePoint] != UnicodeNULLTerminator);
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return 0.0;
    }
    
    UTF32 *Decimal2String(double Decimal) {
        /*
         Should we support Binary Coded Decimal BCD?
         Invalid doubles return a NULL string
         */
        UTF32   *OutputString = NULL;
        int8_t   Sign         = Decimal < 0.0 ? -1 : 1;
        int32_t *Exponent     = 0;
        int32_t  Exponent2    = Exponent;
        float    Mantissa     = frexp(Decimal, Exponent);
        float    Mantissa2    = Mantissa;
        
        // Ok now we just need to get the strings size
        uint64_t StringSize   = UTF1632BOMSizeInCodePoints; // 1 to account for the BOM
        if (Sign == -1) {
            StringSize       += 1;
        }
        while (Exponent2 > 10) {
            Exponent2        /= 10;
            StringSize       +=  1;
        }
        while (Mantissa2 > 0) {
            Mantissa2        /= 10;
            StringSize       +=  1;
        }
        OutputString          = calloc(StringSize, sizeof(UTF32));
        // Now we go ahead and create the string
        if (GlobalByteOrder == LSByteFirst) {
            OutputString[0]   = UTF32LE;
        } else if (GlobalByteOrder == MSByteFirst) {
            OutputString[0]   = UTF32BE;
        }
        if (Sign == -1) {
            OutputString[1]   = '-';
        }
        // Now we start popping in the other variables, first is the Exponent.
        while (*Exponent > 0) { // TODO: This assumes there's only 1 codepoint nessicary to express teh exponent
            OutputString[2]   = *Exponent /= 10;
        }
        OutputString[3]       = '.';
        // Now let's start popping in the Mantissa
        while (Mantissa > 0) { // TODO: This assumes there's only 1 codepoint nessicary to express teh exponent
            OutputString[4]   = Mantissa /= 10;
        }
        return OutputString;
    }
    
    uint64_t UTF32_GetStringSizeInGraphemes(UTF32 *String) {
        if (String != NULL) {
            // Basically loop over the string counting just codepoints that are not combining codepoints.
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return 0;
    }
    
    int64_t UTF32_FindSubString(UTF32 *String, UTF32 *SubString, uint64_t Offset, int64_t Length) {
        uint64_t StringSize            = UTF32_GetSizeInCodePoints(String);
        uint64_t SubStringSize         = UTF32_GetSizeInCodePoints(SubString);
        int64_t  MatchingOffset        = 0LL;
        if (String != NULL && SubString != NULL && StringSize < Offset + Length) {
            for (uint64_t SubCodePoint = 0; SubCodePoint < SubStringSize; SubCodePoint++) {
                for (uint64_t StringCodePoint = Offset; StringCodePoint < Offset + Length; StringCodePoint++) {
                    if (String[StringCodePoint] != SubString[SubCodePoint]) {
                        MatchingOffset = -1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_ERROR, __func__, U8("SubString Pointer is NULL"));
        } else if (Length >= 1 && StringSize < Offset + Length) {
            Log(Log_ERROR, __func__, U8("Offset %d + Length %d is larger than String %d"), Offset, Length, StringSize);
        }
        return MatchingOffset;
    }
    
    UTF32 *UTF32_ExtractSubString(UTF32 *String, uint64_t Offset, uint64_t Length) {
        uint64_t  StringSize                            = UTF32_GetSizeInCodePoints(String);
        uint64_t  ExtractedStringSize                   = Length + UTF1632BOMSizeInCodePoints + UnicodeNULLTerminatorSize;
        UTF32    *ExtractedString                       = NULL;
        if (String != NULL && StringSize >= Length + Offset) {
            ExtractedString                             = calloc(ExtractedStringSize, sizeof(UTF32));
            if (ExtractedString != NULL) {
                for (uint64_t CodePoint = Offset; CodePoint < Offset + Length; CodePoint++) {
                    ExtractedString[CodePoint - Offset] = String[CodePoint];
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate ExtractedString"));
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (StringSize < Length + Offset) {
            Log(Log_ERROR, __func__, U8("Length %d + Offset %d is larger than String %d"), Length, Offset, StringSize);
        }
        return ExtractedString;
    }
    
    UTF32 *UTF32_ReplaceSubString(UTF32 *String, UTF32 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF32 *ReplacedString                 = NULL;
        if (String != NULL && Replacement != NULL && Length >= 1) {
            uint64_t StringSize               = UTF32_GetSizeInCodePoints(String);
            uint64_t ReplacementStringSize    = UTF32_GetSizeInCodePoints(Replacement);
            uint64_t NewStringSize            = 0ULL;
            if (ReplacementStringSize < Length) {
                NewStringSize                 = StringSize - (ReplacementStringSize - Length);
            } else if (ReplacementStringSize >= Length) {
                NewStringSize                 = StringSize + (ReplacementStringSize - Length);
            }
            ReplacedString                    = calloc(NewStringSize, sizeof(UTF32));
            for (uint64_t CodePoint = 0ULL; CodePoint < NewStringSize; CodePoint++) {
                if (CodePoint < Offset) {
                    ReplacedString[CodePoint] = String[CodePoint];
                }
                for (uint64_t ReplacementCodePoint = 0ULL; ReplacementCodePoint < ReplacementStringSize; ReplacementCodePoint++) {
                    ReplacedString[CodePoint] = Replacement[ReplacementCodePoint];
                }
                if (CodePoint > ReplacementStringSize) {
                    ReplacedString[CodePoint] = String[CodePoint];
                }
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_ERROR, __func__, U8("Replacement Pointer is NULL"));
        } else if (Length == 0) {
            Log(Log_ERROR, __func__, U8("Length %d is too short"), Length);
        }
        return ReplacedString;
    }
    
    UTF32 *UTF32_FormatString(const uint64_t VarArgCount, UTF32 *Format, ...) {
        /* So, first we need to break the Format string into the various printf specifiers, which start with a percent symbol */
        
        /* Get the size of the format string in CodePoints, normalize the string, then start counting the format specifiers */
        
        uint64_t  CodePoint                   = 0ULL;
        uint64_t  CurrentSpecifier            = 0ULL;
        uint8_t   CurrentFormatSpecifierTable = 0ULL;
        uint64_t *SpecifierOffset             = calloc(VarArgCount, sizeof(uint64_t));
        uint64_t *SpecifierSize               = calloc(VarArgCount, sizeof(uint64_t));
        
        // %05d means the format specifier is decimal, with 5 digits, with leading zeros if nessicary.
        if (Format != NULL) {
            do {
                if (Format[CodePoint] == '%') {
                    CurrentSpecifier += 1;
                    /* Ok, so we need to get the size of this specifier, and it's offset */
                    /* Compare the number of format specifier arguments to the VarArgCount, if VarArgCount is less than the number of found specifiers, ignore the last ones.
                     
                     Or, just count up until we reach VarArgCount */
                    SpecifierOffset[CurrentSpecifier] = CodePoint;
                    do {
                        
                    } while (Format[CodePoint + 1] == FormatSpecifierCodePoints[CurrentFormatSpecifierTable]);
                }
                CodePoint += 1;
            } while (Format[CodePoint] != UnicodeNULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NULL;
    }
    
#ifdef  __cplusplus
}
#endif
