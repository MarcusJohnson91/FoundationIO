#include "../include/StringIOTables.h" /* Included for the tables, and StringIO declarations, NEEDS TO BE FIRST? */
#include "../include/Log.h"            /* Included for error logging */
#include "../include/Math.h"           /* Included for endian swapping */

#ifdef  __cplusplus
extern  "C" {
#endif
    
    /* Basic String Property Functions */
    uint8_t UTF8_GetCodePointSize(UTF8 CodeUnit) {
        uint8_t CodePointSize      = 0;
        if (((CodeUnit & 0x80) >> 7) == 0) {
            CodePointSize          = 1;
        } else if (((CodeUnit & 0xE0) >> 5) == 6) {
            CodePointSize          = 2;
        } else if (((CodeUnit & 0xF0) >> 4) == 14) {
            CodePointSize          = 3;
        } else if (((CodeUnit & 0xF8) >> 3) == 30) {
            CodePointSize          = 4;
        } else {
            CodePointSize          = 1;
        }
        return CodePointSize;
    }
    
    uint8_t UTF16_GetCodePointSize(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit < UTF16HighSurrogateStart || (CodeUnit > UTF16LowSurrogateEnd && CodeUnit <= UTF16MaxCodePoint)) {
            CodePointSize     = 1;
        } else if (CodeUnit >= UTF16HighSurrogateStart && CodeUnit <= UTF16LowSurrogateEnd) {
            CodePointSize     = 2;
        }
        return CodePointSize;
    }
    
    uint64_t UTF8_GetStringSizeInCodeUnits(UTF8 *String) {
        uint64_t StringSizeInCodeUnits = 0ULL;
        uint64_t CodeUnit              = 0ULL;
        uint8_t  CodeUnitSize          = 0ULL;
        if (String != NULL) {
            do {
                CodeUnitSize           = UTF8_GetCodePointSize(String[CodeUnit]);
                StringSizeInCodeUnits += CodeUnitSize;
                CodeUnit              += CodeUnitSize;
            } while (String[CodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    uint64_t UTF16_GetStringSizeInCodeUnits(UTF16 *String) {
        uint64_t StringSizeInCodeUnits = 0ULL;
        uint64_t CodeUnit              = 0ULL;
        uint8_t  CodeUnitSize          = 0;
        if (String != NULL) {
            do {
                CodeUnitSize           = UTF16_GetCodePointSize(String[CodeUnit]);
                StringSizeInCodeUnits += CodeUnitSize;
                CodeUnit              += CodeUnitSize;
            } while (String[CodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    uint64_t UTF8_GetStringSizeInCodePoints(UTF8 *String) {
        uint64_t StringSizeInCodePoints = 0ULL;
        uint64_t CodeUnit               = 0ULL;
        if (String != NULL) {
            do {
                StringSizeInCodePoints += 1;
                CodeUnit               += UTF8_GetCodePointSize(String[CodeUnit]);
            } while (String[CodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringSizeInCodePoints;
    }
    
    uint64_t UTF16_GetStringSizeInCodePoints(UTF16 *String) {
        uint64_t StringSizeInCodePoints = 0ULL;
        uint64_t CodeUnit               = 0ULL;
        if (String != NULL) {
            do {
                CodeUnit               += UTF16_GetCodePointSize(String[CodeUnit]);
                StringSizeInCodePoints += 1;
            } while (String[CodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringSizeInCodePoints;
    }
    
    uint64_t UTF32_GetStringSizeInCodePoints(UTF32 *String) {
        uint64_t NumCodePoints = 0ULL;
        if (String != NULL) {
            do {
                NumCodePoints += 1;
            } while (String[NumCodePoints] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF32_GetStringSizeInUTF8CodeUnits(UTF32 *String) {
        uint64_t CodePoint            = 0ULL;
        uint64_t UTF8CodeUnits        = 0ULL;
        if (String != NULL) {
            do {
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
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return UTF8CodeUnits;
    }
    
    uint64_t UTF32_GetStringSizeInUTF16CodeUnits(UTF32 *String) {
        uint64_t CodePoint          = 0ULL;
        uint64_t UTF16CodeUnits     = 0ULL;
        if (String != NULL) {
            do {
                if (String[CodePoint] < UTF16HighSurrogateStart || (String[CodePoint] > UTF16LowSurrogateEnd && String[CodePoint] <= UTF16MaxCodePoint)) {
                    UTF16CodeUnits += 1;
                } else if (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd) {
                    UTF16CodeUnits += 2;
                }
                CodePoint          += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    uint64_t UTF32_GetStringSizeInGraphemes(UTF32 *String) {
        uint64_t NumGraphemes         = 0ULL;
        uint64_t CodePoint            = 0ULL;
        if (String != NULL) {
            do {
                for (uint64_t GraphemeExtension = 0; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                    if (String[CodePoint] == GraphemeExtensionTable[GraphemeExtension]) {
                        NumGraphemes += 1;
                    }
                }
                CodePoint            += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    bool  UTF8_StringIsValid(UTF8 *String) {
        uint64_t CodeUnit    = 0ULL;
        bool     IsValidUTF8 = No;
        if (String != NULL) {
            do {
                uint8_t CodePointSize = (String[CodeUnit] & 0xF0) >> 4;
                /*
                 if (String[CodeUnit] & 0x80 >> 7 == 1) {
                 
                 }
                 */
                CodeUnit += 1;
            } while (String[CodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return IsValidUTF8;
    }
    
    bool UTF16_StringIsValid(UTF16 *String) {
        if (String != NULL) {
            
        }
        return No;
    }
    
    UTF8 *UTF8_AddBOM(UTF8 *String) {
        UTF8 *StringWithBOM                         = NULL;
        if (String != NULL) {
            if (String[0] != 0xEF && String[1] != 0xBB && String[2] != 0xBF) {
                uint64_t StringSizeInCodeUnits      = UTF8_GetStringSizeInCodeUnits(String) + UTF8BOMSizeInCodeUnits + NULLTerminatorSize;
                StringWithBOM                       = calloc(StringSizeInCodeUnits, sizeof(UTF8));
                if (StringWithBOM != NULL) {
                    for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
                        if (CodeUnit != 0 && CodeUnit != 1 && CodeUnit != 2) {
                            StringWithBOM[CodeUnit] = String[CodeUnit - 2];
                        } else if (CodeUnit == 0) {
                            StringWithBOM[CodeUnit] = 0xEF;
                        } else if (CodeUnit == 1) {
                            StringWithBOM[CodeUnit] = 0xBB;
                        } else if (CodeUnit == 2) {
                            StringWithBOM[CodeUnit] = 0xBF;
                        }
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate StringWithBOM"));
                }
            } else {
                StringWithBOM = String;
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF16 *UTF16_AddBOM(UTF16 *String, StringIOByteOrders BOM2Add) {
        UTF16   *StringWithBOM        = NULL;
        uint16_t ByteOrder            = 0;
        if (String != NULL) {
            if (String[0] != UTF16LE && String[0] != UTF16BE) {
                uint64_t StringSize   = UTF16_GetStringSizeInCodeUnits(String) + UTF16BOMSizeInCodeUnits + NULLTerminatorSize;
                StringWithBOM         = calloc(StringSize, sizeof(UTF16));
                if (StringWithBOM != NULL) {
                    if (BOM2Add == UseNativeByteOrder) {
                        GetRuntimeByteBitOrder();
                        if (GlobalByteOrder == LSByteFirst) {
                            ByteOrder = UTF16LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            ByteOrder = UTF16BE;
                        }
                    } else {
                        if (BOM2Add == UseLEByteOrder) {
                            ByteOrder = UTF16LE;
                        } else if (BOM2Add == UseBEByteOrder) {
                            ByteOrder = UTF16BE;
                        }
                    }
                    StringWithBOM[0] = ByteOrder;
                    for (uint64_t CodeUnit = 1ULL; CodeUnit < StringSize; CodeUnit++) {
                        StringWithBOM[CodeUnit] = String[CodeUnit - 1];
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("StringWithBOM couldn't be allocated"));
                }
            } else {
                // We need to verify the String's byte order, how do we do that? well if a codeunit contains a high surrogate or low surrogate you can tell by it's value, otherwise look for code units who's value is less than 256, or greater than 256, that'll tell you which side of the string contains the low bits.
                StringWithBOM = String;
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF8 *UTF8_RemoveBOM(UTF8 *String) {
        UTF8    *BOMLessString                      = NULL;
        uint64_t StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF8_GetStringSizeInCodeUnits(String);
            if (String[0] == 0xEF && String[1] == 0xBB && String[2] == 0xBF) {
                BOMLessString                       = calloc(StringSize - UTF8BOMSizeInCodeUnits + NULLTerminatorSize, sizeof(UTF8));
                if (BOMLessString != NULL) {
                    for (uint64_t CodeUnit = 3ULL; CodeUnit < StringSize; CodeUnit++) {
                        BOMLessString[CodeUnit - 3] = String[CodeUnit];
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate BOMLessString"));
                }
            } else {
                BOMLessString = String;
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    
    UTF16 *UTF16_RemoveBOM(UTF16 *String) {
        UTF16   *BOMLessString                      = NULL;
        uint64_t StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF16_GetStringSizeInCodeUnits(String);
            if (String[0] == UTF16BE || String[0] == UTF16LE) {
                BOMLessString                       = calloc(StringSize - UTF16BOMSizeInCodeUnits + NULLTerminatorSize, sizeof(UTF16));
                if (BOMLessString != NULL) {
                    for (uint64_t CodeUnit = 1ULL; CodeUnit < StringSize; CodeUnit++) {
                        BOMLessString[CodeUnit - 1] = String[CodeUnit];
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate BOMLessString"));
                }
            } else {
                BOMLessString = String;
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    /* Basic String Property Functions */
    
    UTF32 *UTF8_Decode(UTF8 *String) {
        uint8_t  CodePointSizeInCodeUnits            = 0;
        uint64_t CodeUnitNum                         = 0ULL;
        uint64_t CodePoint                           = 0ULL;
        UTF32   *DecodedString                       = NULL;
        if (String != NULL) {
            uint64_t NumCodePoints                   = UTF8_GetStringSizeInCodePoints(String) + NULLTerminatorSize;
            if (String[0] == 0xEF && String[1] == 0xBB && String[2] == 0xBF) { // We need to remove the BOM, and subtract it's size from the Num codepoints
                NumCodePoints                       -= UnicodeBOMSizeInCodePoints;
                CodeUnitNum                         += UTF8BOMSizeInCodeUnits;
            }
            DecodedString                            = calloc(NumCodePoints, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    CodePointSizeInCodeUnits         = UTF8_GetCodePointSize(String[CodeUnitNum]);
                    switch (CodePointSizeInCodeUnits) { // UTF-8 is MSB first, if the platform is LSB first, we need to swap as we read
                        case 1:
                            DecodedString[CodePoint] =  String[CodeUnitNum];
                            CodeUnitNum             += 1;
                            CodePoint               += 1;
                            break;
                        case 2:
                            DecodedString[CodePoint] = (String[CodeUnitNum] & 0x1F) << 6;
                            DecodedString[CodePoint] =  String[CodeUnitNum] & 0x3F;
                            CodeUnitNum             += 2;
                            CodePoint               += 1;
                            break;
                        case 3:
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x0F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x1F) << 6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 2] & 0x1F);
                            CodeUnitNum             += 3;
                            CodePoint               += 1;
                            break;
                        case 4:
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x07) << 18;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x3F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 2] & 0x3F) <<  6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 3] & 0x3F);
                            CodeUnitNum             += 4;
                            CodePoint               += 1;
                            break;
                    }
                    if (CodePointSizeInCodeUnits > 1 && DecodedString[CodePoint] <= 0x7F) {
                        Log(Log_ERROR, __func__, U8("CodePoint %llu, U+%X is overlong"), CodePoint, DecodedString[CodePoint]);
                    } else if (DecodedString[CodePoint] >= UTF16HighSurrogateStart && DecodedString[CodePoint] <= UTF16LowSurrogateEnd) {
                        DecodedString[CodePoint]     = InvalidReplacementCodePoint;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, because it overlaps the Surrogate Pair Block, it was replaced with U+FFFD"), DecodedString[CodePoint]);
                    }
                } while (String[CodeUnitNum] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate DecodedString"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF32 *UTF16_Decode(UTF16 *String) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        uint64_t CodePoint                       = 0ULL;
        uint64_t CodeUnitNum                     = 0ULL;
        UTF32   *DecodedString                   = NULL;
        if (String != NULL) {
            uint64_t UTF16CodeUnits              = UTF16_GetStringSizeInCodePoints(String) + NULLTerminatorSize;
            if (String[0] != UTF16BE && String[0] != UTF16LE) {
                UTF16CodeUnits                  += 1;
            }
            DecodedString                        = calloc(UTF16CodeUnits, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if ((String[0] == UTF16LE && GlobalByteOrder == MSByteFirst) || (String[0] == UTF16BE && GlobalByteOrder == LSByteFirst)) {
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
                } while (String[CodePoint] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("DecodedString Pointer is NULL"));
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
            uint64_t UTF8CodeUnits                     = NULLTerminatorSize + UTF32_GetStringSizeInUTF8CodeUnits(String) + (IncludeBOM == true ? UTF8BOMSizeInCodeUnits : 0);
            EncodedString                              = calloc(UTF8CodeUnits, sizeof(UTF8));
            if (EncodedString != NULL) {
                do {
                    if (GlobalByteOrder == LSByteFirst) {
                        String[CodePoint]              = SwapEndian32(String[CodePoint]);
                    }
                    if (CodeUnitNum == 0 && IncludeBOM == Yes) {
                        EncodedString[CodeUnitNum]     = 0xEF;
                        EncodedString[CodeUnitNum + 1] = 0xBB;
                        EncodedString[CodeUnitNum + 2] = 0xBF;
                        CodeUnitNum                   += UTF8BOMSizeInCodeUnits;
                    }
                    if (String[CodePoint] <= 0x7F) {
                        EncodedString[CodeUnitNum]     = String[CodePoint];
                        CodeUnitNum                   += 1;
                    } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                        EncodedString[CodeUnitNum]     = (0xC0 & ((String[CodePoint] & 0x7C0) >> 6));
                        EncodedString[CodeUnitNum + 1] = (0x80 &  (String[CodePoint] & 0x03F));
                        CodeUnitNum                   += 2;
                    }  else if (String[CodePoint] >= 0x800 && String[CodePoint] <= UTF16MaxCodePoint && String[CodePoint] < UTF16HighSurrogateStart && String[CodePoint] > UTF16LowSurrogateEnd && String[CodePoint] != UTF16LE && String[CodePoint] != UTF16BE) {
                        EncodedString[CodeUnitNum]     = (0xE0 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 1] = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 2] = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 3;
                    } else if (String[CodePoint] > UTF16MaxCodePoint && String[CodePoint] <= UnicodeMaxCodePoint) {
                        EncodedString[CodeUnitNum]     = (0xF0 & ((String[CodePoint] & 0x1C0000) >> 18));
                        EncodedString[CodeUnitNum + 1] = (0x80 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 2] = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 3] = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 4;
                    } else if (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd && String[CodePoint] != UTF32LE && String[CodePoint] != UTF32BE) {
                        String[CodePoint]              = InvalidReplacementCodePoint;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD"), String[CodePoint]);
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("Encoded Pointer is NULL"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF16 *UTF16_Encode(UTF32 *String, StringIOByteOrders OutputByteOrder) {
        if (GlobalByteOrder == UnknownByteFirst) {
            GetRuntimeByteBitOrder();
        }
        UTF16   *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t CodeUnitNum                       = 0ULL;
            uint64_t UTF16NumCodeUnits                 = UTF16BOMSizeInCodeUnits + UTF32_GetStringSizeInUTF16CodeUnits(String) + NULLTerminatorSize;
            EncodedString                              = calloc(UTF16NumCodeUnits, sizeof(UTF16));
            if (EncodedString != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < UTF16NumCodeUnits; CodeUnit++) {
                    if ((OutputByteOrder == UseBEByteOrder && GlobalByteOrder == LSByteFirst) || (OutputByteOrder == UseLEByteOrder && GlobalByteOrder == MSByteFirst)) {
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
                Log(Log_ERROR, __func__, U8("Encoded Pointer is NULL"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF8 *UTF8_CaseFoldString(UTF8 *String) {
        UTF8 *CaseFolded      = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *CaseFold32 = UTF32_CaseFoldString(String32);
            free(String32);
            CaseFolded        = UTF8_Encode(CaseFold32, No);
            free(CaseFold32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return CaseFolded;
    }
    
    UTF16 *UTF16_CaseFoldString(UTF16 *String) {
        UTF16 *CaseFolded     = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *CaseFold32 = UTF32_CaseFoldString(String32);
            free(String32);
            CaseFolded        = UTF16_Encode(CaseFold32, UseLEByteOrder);
            free(CaseFold32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return CaseFolded;
    }
    
    UTF32 *UTF32_CaseFoldString(UTF32 *String) {
        uint64_t CodePoint        = 0ULL;
        UTF32   *CaseFoldedString = NULL;
        if (String != NULL) {
            do {
                for (uint64_t Index = 0; Index < CaseFoldTableSize; Index++) {
                    if (String[CodePoint] == CaseFoldCodePoints[Index]) {
                        CaseFoldedString = UTF32_ReplaceSubString(String, CaseFoldStrings[Index], CodePoint, 1);
                    }
                }
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return CaseFoldedString;
    }
    
    static UTF32 *UTF32_Reorder(UTF32 *String) { // Stable sort
        uint64_t CodePoint  = 1ULL;
        uint32_t CodePointA = 0UL;
        uint32_t CodePointB = 0UL;
        if (String != NULL) {
            do {
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
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NULL;
    }
    
    static UTF32 *UTF32_Compose(UTF32 *String, bool Kompatibility) { // FIXME: Must use a stable sorting algorithm
        uint64_t CodePoint      = 0ULL;
        UTF32   *ComposedString = NULL;
        if (String != NULL && (Kompatibility == No || Kompatibility == Yes)) {
            do {
                if (Kompatibility == Yes) {
                    for (uint64_t Index = 0; Index < KompatibleNormalizationTableSize; Index++) {
                        if (String[CodePoint] == KompatibleNormalizationCodePoints[Index]) {
                            ComposedString = UTF32_ReplaceSubString(String, KompatibleNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                } else {
                    for (uint64_t DecomposeCodePoint = 0; DecomposeCodePoint < CanonicalNormalizationTableSize; DecomposeCodePoint++) {
                        if (String[CodePoint] == CanonicalNormalizationCodePoints[DecomposeCodePoint]) {
                            ComposedString = UTF32_ReplaceSubString(String, CanonicalNormalizationStrings[DecomposeCodePoint], CodePoint, 1);
                        }
                    }
                }
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return ComposedString;
    }
    
    static UTF32 *UTF32_Decompose(UTF32 *String, const bool Kompatibility) { // FIXME: Must use a stable sorting algorithm
        uint64_t CodePoint             = 0ULL;
        UTF32   *DecomposedString      = NULL;
        if (String != NULL && (Kompatibility == No || Kompatibility == Yes)) {
            UTF32 *Decomposed          = NULL;
            do {
                if (Kompatibility == Yes) {
                    for (uint64_t Index = 0; Index < KompatibleNormalizationTableSize; Index++) {
                        if (String[CodePoint] == KompatibleNormalizationCodePoints[Index]) {
                            Decomposed = UTF32_ReplaceSubString(String, KompatibleNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                } else {
                    for (uint64_t Index = 0; Index < CanonicalNormalizationTableSize; Index++) {
                        if (String[CodePoint] == CanonicalNormalizationCodePoints[Index]) {
                            Decomposed = UTF32_ReplaceSubString(String, CanonicalNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                }
                CodePoint             += 1;
            } while (String[CodePoint] != NULLTerminator);
            DecomposedString           = UTF32_Reorder(Decomposed);
            free(Decomposed);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecomposedString;
    }
    
    UTF8 *UTF8_NormalizeString(UTF8 *String, StringIONormalizationForms NormalizedForm) {
        UTF8 *NormalizedString8       = NULL;
        if (String != NULL && NormalizedForm != UnknownNormalizationForm) {
            UTF32 *String32           = UTF8_Decode(String);
            UTF32 *NormalizedString32 = UTF32_NormalizeString(String32, NormalizedForm);
            NormalizedString8         = UTF8_Encode(NormalizedString32, No);
            free(String32);
            free(NormalizedString32);
        }
        return NormalizedString8;
    }
    
    UTF16 *UTF16_NormalizeString(UTF16 *String, StringIONormalizationForms NormalizedForm) {
        UTF16 *NormalizedString16     = NULL;
        if (String != NULL && NormalizedForm != UnknownNormalizationForm) {
            UTF32 *String32           = UTF16_Decode(String);
            UTF32 *NormalizedString32 = UTF32_NormalizeString(String32, NormalizedForm);
            NormalizedString16        = UTF16_Encode(NormalizedString32, UseLEByteOrder);
            free(String32);
            free(NormalizedString32);
        }
        return NormalizedString16;
    }
    
    UTF32 *UTF32_NormalizeString(UTF32 *String, StringIONormalizationForms NormalizedForm) {
        UTF32 *NormalizedString = NULL;
        if (String != NULL && NormalizedForm != UnknownNormalizationForm) {
            if (NormalizedForm == NormalizationFormC) {
                UTF32 *Decomposed = UTF32_Decompose(String, No);
                NormalizedString  = UTF32_Compose(Decomposed, No);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationFormKC) {
                UTF32 *Decomposed = UTF32_Decompose(String, Yes);
                NormalizedString  = UTF32_Compose(Decomposed, Yes);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationFormD) {
                NormalizedString  = UTF32_Decompose(String, No);
            } else if (NormalizedForm == NormalizationFormKD) {
                NormalizedString  = UTF32_Decompose(String, Yes);
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (NormalizedForm == UnknownNormalizationForm) {
            Log(Log_ERROR, __func__, U8("Unknown Normalization form"));
        }
        return NormalizedString;
    }
    
    int64_t UTF8_FindSubString(UTF8 *String, UTF8 *SubString, uint64_t Offset, int64_t Length) {
        int64_t FoundOffset = 0LL;
        if (String != NULL && SubString != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *SubString32 = UTF8_Decode(SubString);
            FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
            free(String32);
            free(SubString32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_ERROR, __func__, U8("SubString Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_ERROR, __func__, U8("SubString Pointer is NULL"));
        }
        return FoundOffset;
    }
    
    int64_t UTF32_FindSubString(UTF32 *String, UTF32 *SubString, uint64_t Offset, int64_t Length) {
        uint64_t StringSize            = UTF32_GetStringSizeInCodePoints(String);
        uint64_t SubStringSize         = UTF32_GetStringSizeInCodePoints(SubString);
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
            Log(Log_ERROR, __func__, U8("Offset %llu + Length %lld is larger than String %llu"), Offset, Length, StringSize);
        }
        return MatchingOffset;
    }
    
    UTF8 *UTF8_ExtractSubString(UTF8 *String, uint64_t Offset, uint64_t Length) {
        UTF8 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, Length);
            ExtractedSubString = UTF8_Encode(Extracted32, No);
            free(String32);
            free(Extracted32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF16 *UTF16_ExtractSubString(UTF16 *String, uint64_t Offset, uint64_t Length) {
        UTF16 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, Length);
            ExtractedSubString = UTF16_Encode(Extracted32, UseLEByteOrder);
            free(String32);
            free(Extracted32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF32 *UTF32_ExtractSubString(UTF32 *String, uint64_t Offset, uint64_t Length) {
        uint64_t  StringSize                            = UTF32_GetStringSizeInCodePoints(String);
        uint64_t  ExtractedStringSize                   = Length + NULLTerminatorSize;
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
            Log(Log_ERROR, __func__, U8("Offset %llu + Length %lld is larger than String %llu"), Length, Offset, StringSize);
        }
        return ExtractedString;
    }
    
    UTF8  *UTF8_ReplaceSubString(UTF8 *String, UTF8 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF8 *Replaced8          = NULL;
        if (String != NULL && Replacement != NULL && Length >= 1) {
            UTF32 *String32      = UTF8_Decode(String);
            UTF32 *Replacement32 = UTF8_Decode(Replacement);
            UTF32 *Replaced32    = UTF32_ReplaceSubString(String32, Replacement32, Offset, Length);
            Replaced8            = UTF8_Encode(Replaced32, No);
            free(String32);
            free(Replacement32);
            free(Replaced32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_ERROR, __func__, U8("Replacement Pointer is NULL"));
        } else if (Length == 0) {
            Log(Log_ERROR, __func__, U8("Length %llu is too short"), Length);
        }
        return Replaced8;
    }
    
    UTF16 *UTF16_ReplaceSubString(UTF16 *String, UTF16 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF16 *Replaced16        = NULL;
        if (String != NULL && Replacement != NULL && Length >= 1) {
            UTF32 *String32      = UTF16_Decode(String);
            UTF32 *Replacement32 = UTF16_Decode(Replacement);
            UTF32 *Replaced32    = UTF32_ReplaceSubString(String32, Replacement32, Offset, Length);
            Replaced16           = UTF16_Encode(Replaced32, UseLEByteOrder);
            free(String32);
            free(Replacement32);
            free(Replaced32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_ERROR, __func__, U8("Replacement Pointer is NULL"));
        } else if (Length == 0) {
            Log(Log_ERROR, __func__, U8("Length %llu is too short"), Length);
        }
        return Replaced16;
    }
    
    UTF32 *UTF32_ReplaceSubString(UTF32 *String, UTF32 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF32 *ReplacedString                 = NULL;
        if (String != NULL && Replacement != NULL && Length >= 1) {
            uint64_t StringSize               = UTF32_GetStringSizeInCodePoints(String);
            uint64_t ReplacementStringSize    = UTF32_GetStringSizeInCodePoints(Replacement);
            uint64_t NewStringSize            = 0ULL;
            if (ReplacementStringSize < Length) {
                NewStringSize                 = StringSize - (ReplacementStringSize - Length);
            } else if (ReplacementStringSize >= Length) {
                NewStringSize                 = StringSize + (ReplacementStringSize - Length);
            }
            ReplacedString                    = calloc(NewStringSize, sizeof(UTF32));
            if (ReplacedString != NULL) {
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
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_ERROR, __func__, U8("Replacement Pointer is NULL"));
        } else if (Length == 0) {
            Log(Log_ERROR, __func__, U8("Length %llu is too short"), Length);
        }
        return ReplacedString;
    }
    
    UTF8 *UTF8_RemoveSubString(UTF8 *String, UTF8 *SubString2Remove, int64_t Instance2Remove) {
        UTF8 *TrimmedString         = NULL;
        if (String != NULL && SubString2Remove != NULL && Instance2Remove >= -1) {
            UTF32 *DecodedString    = UTF8_Decode(String);
            UTF32 *DecodedSubString = UTF8_Decode(SubString2Remove);
            UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
            TrimmedString           = UTF8_Encode(Trimmed32, No);
            free(DecodedString);
            free(DecodedSubString);
            free(Trimmed32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("SubString2Remove Pointer is NULL"));
        } else if (Instance2Remove < -1) {
            Log(Log_ERROR, __func__, U8("Instance2Remove %lld is invalid"), Instance2Remove);
        }
        return TrimmedString;
    }
    
    UTF16 *UTF16_RemoveSubString(UTF16 *String, UTF16 *SubString2Remove, int64_t Instance2Remove) {
        UTF16 *TrimmedString        = NULL;
        if (String != NULL && SubString2Remove != NULL && Instance2Remove >= -1) {
            UTF32 *DecodedString    = UTF16_Decode(String);
            UTF32 *DecodedSubString = UTF16_Decode(SubString2Remove);
            UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
            TrimmedString           = UTF16_Encode(Trimmed32, UseLEByteOrder);
            free(DecodedString);
            free(DecodedSubString);
            free(Trimmed32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("SubString2Remove Pointer is NULL"));
        } else if (Instance2Remove < -1) {
            Log(Log_ERROR, __func__, U8("Instance2Remove %lld is invalid"), Instance2Remove);
        }
        return TrimmedString;
    }
    
    UTF32 *UTF32_RemoveSubString(UTF32 *String, UTF32 *SubString2Remove, int64_t Instance2Remove) {
        uint64_t SubStringSize     = UTF32_GetStringSizeInCodePoints(SubString2Remove);
        if (String != NULL && SubString2Remove != NULL) {
            // Ok well we need to start counting how many instances there are, if instance = -1, we need to remove all.
            uint64_t CodePoint     = 0ULL;
            int64_t  FoundInstance = 0ULL;
            /*
             We need to know the size of the substring that way we can loop over the string properly.
             
             Then we need 2 loops, one for the main string, and one for the substring.
             
             Each time a non-matching codepoint is found in the string, we need to reset the substring back to the beginning.
             */
            
            
            do { // We need to get the size of SubString2Remove, and loop over it comparing
                if (Instance2Remove == -1) {
                    // Remove each and every instance of the substring
                } else if (FoundInstance == Instance2Remove) {
                    // Count the instances
                }
                CodePoint += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("SubString2Remove Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF8 **UTF8_SplitString(UTF8 *String, uint64_t NumDelimiters, UTF8 **Delimiters) {
        if (String != NULL && NumDelimiters > 0 && Delimiters != NULL) {
            // Decode String2Split, and make an array of decoded strings for the Delimiters
            UTF32  *String2Split32      = UTF8_Decode(String);
            UTF32 **Delimiters32        = calloc(NumDelimiters, sizeof(UTF32*));
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                Delimiters32[Delimiter] = UTF8_Decode(Delimiters[Delimiter]);
            }
            UTF32_SplitString(String2Split32, NumDelimiters, Delimiters32);
            free(String2Split32);
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                free(Delimiters32[Delimiter]);
            }
            free(Delimiters32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (NumDelimiters == 0) {
            Log(Log_ERROR, __func__, U8("There are no delimiters"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF16 **UTF16_SplitString(UTF16 *String, uint64_t NumDelimiters, UTF16 **Delimiters) {
        if (String != NULL && NumDelimiters > 0 && Delimiters != NULL) {
            UTF32  *String2Split32      = UTF16_Decode(String);
            UTF32 **Delimiters32        = calloc(NumDelimiters, sizeof(UTF32*));
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                Delimiters32[Delimiter] = UTF16_Decode(Delimiters[Delimiter]);
            }
            UTF32_SplitString(String2Split32, NumDelimiters, Delimiters32);
            free(String2Split32);
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                free(Delimiters32[Delimiter]);
            }
            free(Delimiters32);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (NumDelimiters == 0) {
            Log(Log_ERROR, __func__, U8("There are no delimiters"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF32 **UTF32_SplitString(UTF32 *String, uint64_t NumDelimiters, UTF32 **Delimiters) {
        // We once again need to loop over the string getting the number of substrings and their sizes.
        uint64_t NumSubStrings = 1ULL; // 1 so we can put the count as the first variable
        uint64_t StringSize    = UTF32_GetStringSizeInCodePoints(String);
        uint64_t CodePoint     = 0ULL;
        if (String != NULL && NumDelimiters > 0 && Delimiters != NULL) {
            // The aasy way would be to just loop over the string NumDelimiters times, counting the number of occurances for each delimiter in the string
            
            
            /* ALL of the logic: The string and delimiter strings must not be NULL, the strings needs to be at least as long as the longest delimiter,  */
            
            
            for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    if (String[StringCodePoint] == Delimiters[Delimiter][0]) { // We've found the start of a delimiter, now we just ned to make sure it fits the rest
                        for (uint64_t DelimiterCodePoint = 0ULL; DelimiterCodePoint < UTF32_GetStringSizeInCodePoints(Delimiters[Delimiter]); DelimiterCodePoint++) {
                            // Start looping?
                        }
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (NumDelimiters == 0) {
            Log(Log_ERROR, __func__, U8("There are no delimiters"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return NULL;
    }
    
    int64_t UTF8_String2Integer(UTF8 *String) {
        UTF32 *String32 = UTF8_Decode(String);
        int64_t Value   = UTF32_String2Integer(String32);
        free(String32);
        return Value;
    }
    
    int64_t UTF16_String2Integer(UTF16 *String) {
        UTF32 *String32 = UTF16_Decode(String);
        int64_t Value   = UTF32_String2Integer(String32);
        free(String32);
        return Value;
    }
    
    int64_t UTF32_String2Integer(UTF32 *String) {
        uint64_t CodePoint         =  0ULL;
        int8_t   Sign              =  1;
        int64_t  Value             =  0;
        uint8_t  Base              = 10;
        if (String != NULL) {
            UTF32 *Decomposed      = UTF32_NormalizeString(String, NormalizationFormKD);
            do {
                for (uint64_t WhiteSpace = 0; WhiteSpace < WhiteSpaceTableSize; WhiteSpace++) {
                    if (Decomposed[CodePoint] == WhiteSpaceTable[WhiteSpace]) {
                        CodePoint += 1;
                    }
                }
                for (uint64_t IntegerIndex = 0ULL; IntegerIndex < IntegerTableSize; IntegerIndex++) {
                    if (Decomposed[CodePoint] == IntegerCodePoints[IntegerIndex]) {
                        Value     += IntegerValues[IntegerIndex];
                        CodePoint += 1;
                    }
                }
                if (Decomposed[CodePoint] == U32('0')) {
                    if (Decomposed[CodePoint + 1] == U32('b') || Decomposed[CodePoint + 1] == U32('B')) {
                        Base       = 2;
                    } else if (Decomposed[CodePoint + 1] == U32('o') || Decomposed[CodePoint] == U32('O')) {
                        Base       = 8;
                    } else if (Decomposed[CodePoint + 1] == U32('x') || Decomposed[CodePoint] == U32('X')) {
                        Base       = 16;
                    }
                } else if (String[CodePoint] == U32('-')) {
                    Sign           = -1;
                }
                Value             *= Base;
                if (Base == 16 && (String[CodePoint] >= 0x41 && String[CodePoint] <= 0x46)) {
                    Value         += (String[CodePoint] - 50);
                } else if (Base == 16 && (String[CodePoint] >= 0x61 && String[CodePoint] <= 0x66)) {
                    Value         += (String[CodePoint] - 82);
                } else if ((Base == 10 || Base == 16) && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39)) {
                    Value         += (String[CodePoint] - 48);
                } else if (Base == 8 && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x37)) {
                    Value         += (String[CodePoint] - 48);
                } else if (Base == 2 && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x31)) {
                    Value         += (String[CodePoint] - 48);
                }
                CodePoint         += 1;
                Value             *= Sign;
            } while (Decomposed[CodePoint] != NULLTerminator);
            free(Decomposed);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Value;
    }
    
    UTF8 *UTF8_Integer2String(const StringIOBases Base, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF8  *IntegerString8  = UTF8_Encode(IntegerString32, No);
        free(IntegerString32);
        return IntegerString8;
    }
    
    UTF16 *UTF16_Integer2String(const StringIOBases Base, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF16 *IntegerString16 = UTF16_Encode(IntegerString32, UseLEByteOrder);
        free(IntegerString32);
        return IntegerString16;
    }
    
    UTF32 *UTF32_Integer2String(const StringIOBases Base, int64_t Integer2Convert) {
        int64_t  Sign            = 0LL;
        int64_t  Num             = Integer2Convert;
        uint8_t  NumDigits       = 0;
        while (Num > 0) {
            Num                 /= Base;
            NumDigits           += 1;
        }
        if (Integer2Convert < 0 && Base == Decimal) {
            Sign                 = -1;
            NumDigits           +=  1;
        }
        UTF32 *NumberString      = calloc(NumDigits + NULLTerminatorSize, sizeof(UTF32));
        UTF32  UpperNumerals[16] = {U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'), U32('E'), U32('F')};
        UTF32  LowerNumerals[16] = {U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'), U32('e'), U32('f')};
        if (NumberString != NULL) {
            for (uint64_t CodePoint = NumDigits - 1; CodePoint > 0; CodePoint--) {
                // Ok, so we've clamped the looping, now all we need to do is malloc a string with NumDigits as it's size.
                // Now we just AND the number with Bit to get it's value, if the value is a 1 write a 1, otherwise write a zero
                int64_t CurrentDigit    = (Base == Decimal ? Absolute(Integer2Convert %= Base) : (Integer2Convert %= Base));
                NumberString[CodePoint] = (Base == HexUpper ? UpperNumerals[CurrentDigit] : LowerNumerals[CurrentDigit]);
            }
        }
        return NumberString;
    }
    
    double UTF8_String2Decimal(UTF8 *String) {
        double Decimal = 0.0;
        if (String != NULL) {
            // Decode the string, then send it on over to UTF32_String2Decimal, then free the utf32 string
            UTF32 *String32 = UTF8_Decode(String);
            Decimal         = UTF32_String2Decimal(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF16_String2Decimal(UTF16 *String) {
        double Decimal = 0.0;
        if (String != NULL) {
            // Decode the string, then send it on over to UTF32_String2Decimal, then free the utf32 string
            UTF32 *String32 = UTF16_Decode(String);
            Decimal         = UTF32_String2Decimal(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF32_String2Decimal(UTF32 *String) { // Replaces strtod and atof
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
                    if (String[CodePoint] == U32('-')) {
                        IsNegative = Yes;
                    }
                    CodePoint     += 1;
                } while (String[CodePoint] != NULLTerminator);
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return 0.0;
    }
    
    UTF8 *UTF8_Decimal2String(const StringIOBases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Base, Decimal);
        UTF8  *String8  = UTF8_Encode(String32, No);
        free(String32);
        return String8;
    }
    
    UTF16 *UTF16_Decimal2String(const StringIOBases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Base, Decimal);
        UTF16 *String16 = UTF16_Encode(String32, UseLEByteOrder);
        free(String32);
        return String16;
    }
    
    UTF32 *UTF32_Decimal2String(const StringIOBases Base, double Decimal) {
        UTF32   *OutputString = NULL;
        int8_t   Sign         = Decimal < 0.0 ? -1 : 1;
        int32_t *Exponent     = calloc(1, sizeof(int32_t));
        int32_t  Exponent2    = Exponent;
        float    Mantissa     = frexp(Decimal, Exponent);
        float    Mantissa2    = Mantissa;
        
        // Ok now we just need to get the strings size
        uint64_t StringSize   = 0ULL;
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
        if (OutputString != NULL) {
            // Now we go ahead and create the string
            if (Sign == -1) {
                OutputString[1]   = U32('-');
            }
            // Now we start popping in the other variables, first is the Exponent.
            while (Exponent2 > 0) { // TODO: This assumes there's only 1 codepoint nessicary to express teh exponent
                OutputString[2]   = Exponent2 /= 10;
            }
            OutputString[3]       = U32('.');
            // Now let's start popping in the Mantissa
            while (Mantissa > 0) { // TODO: This assumes there's only 1 codepoint nessicary to express teh exponent
                OutputString[4]   = Mantissa /= 10;
            }
        }
        return OutputString;
    }
    
    UTF8  *UTF8_TrimString(UTF8 *String, UTF8 **Strings2Remove) {
        UTF8 *Trimmed = NULL;
        if (String != NULL && Strings2Remove != NULL) {
            // Loop over Strng2Trim looking at each codepoint. if codepoint X matches codepoint 0 from Strings2Remove Y, start looping over Strings2Remove Y making sure the whole string matches, if it does, remove that.
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Strings2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF16 *UTF16_TrimString(UTF16 *String, UTF16 **Strings2Remove) {
        UTF16 *Trimmed = NULL;
        if (String != NULL && Strings2Remove != NULL) {
            // Loop over Strng2Trim looking at each codepoint. if codepoint X matches codepoint 0 from Strings2Remove Y, start looping over Strings2Remove Y making sure the whole string matches, if it does, remove that.
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Strings2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF32 *UTF32_TrimString(UTF32 *String, UTF32 **Strings2Remove) { // How should this API be done? should we take in a array of strings to remove? also should we have an option to remove these strings from not just the start and end of the string, but
        UTF32 *Trimmed = NULL;
        if (String != NULL && Strings2Remove != NULL) {
            // Loop over Strng2Trim looking at each codepoint. if codepoint X matches codepoint 0 from Strings2Remove Y, start looping over Strings2Remove Y making sure the whole string matches, if it does, remove that.
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Strings2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    bool UTF8_Compare(UTF8 *String1, UTF8 *String2, StringIONormalizationForms NormalizedForm, bool CaseInsensitive) {
        bool StringsMatch          = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32    = UTF8_Decode(String1);
            UTF32 *String2UTF32    = UTF8_Decode(String2);
            StringsMatch           = UTF32_Compare(String1UTF32, String2UTF32, NormalizedForm, CaseInsensitive);
            free(String1UTF32);
            free(String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF16_Compare(UTF16 *String1, UTF16 *String2, StringIONormalizationForms NormalizedForm, bool CaseInsensitive) {
        bool StringsMatch          = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32    = UTF16_Decode(String1);
            UTF32 *String2UTF32    = UTF16_Decode(String2);
            StringsMatch           = UTF32_Compare(String1UTF32, String2UTF32, NormalizedForm, CaseInsensitive);
            free(String1UTF32);
            free(String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF32_Compare(UTF32 *String1, UTF32 *String2, StringIONormalizationForms NormalizedForm, bool CaseInsensitive) {
        uint64_t CodePoint           = 0ULL;
        bool StringsMatch            = Yes;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *Normalized1       = UTF32_NormalizeString(String1, NormalizedForm);
            UTF32 *Normalized2       = UTF32_NormalizeString(String2, NormalizedForm);
            if (CaseInsensitive == Yes) {
                UTF32 *CaseFolded1   = UTF32_CaseFoldString(Normalized1);
                UTF32 *CaseFolded2   = UTF32_CaseFoldString(Normalized2);
                do {
                    if (CaseFolded1[CodePoint] != CaseFolded2[CodePoint]) {
                        StringsMatch = No;
                    }
                } while (CaseFolded1[CodePoint] != NULLTerminator && CaseFolded2[CodePoint] != NULLTerminator);
                free(CaseFolded1);
                free(CaseFolded2);
            } else {
                do {
                    if (Normalized1[CodePoint] != Normalized2[CodePoint]) {
                        StringsMatch = No;
                    }
                } while (Normalized1[CodePoint] != NULLTerminator && Normalized2[CodePoint] != NULLTerminator);
            }
            free(Normalized1);
            free(Normalized2);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    typedef enum StringTypes {
        UnknownFormat = 0,
        UTF8Format    = 1,
        UTF16Format   = 2,
    } StringTypes;
    
    typedef enum FormatSpecifierFlags {
        NoFormatFlag                   = 0,
        LeftAlignFormatFlag            = 1,
        RightAlignFormatFlag           = 2,
        PrependSignFormatFlag          = 3,
        AlignSignsFormatFlag           = 4, // Prepend a space in front of positive ints, a minus for negative
        TellTypeFormatFlag             = 5,
        PrependZerosFormatFlag         = 6, // Don't remove trailing zeros, or add a decimal point or prepend with non-zeros
    } FormatSpecifierFlags;
    
    typedef enum FormatSpecifierLengths { // Length field
        NoLengthFlag                   = 0,
        CharPromoted2Int               = 1,
        ShortPromoted2Int              = 2,
        LongLengthSpecifier            = 3,
        LongLongLengthSpecifier        = 4,
        LongDoubleSpecifier            = 5,
        Size_TSpecifiier               = 6,
        IntMaxSpecifier                = 7,
        PtrDiffSpecifier               = 8,
    } FormatSpecifierLengths;
    
    typedef enum FormatSpecifierTypes { // U = uppercase, L = lowercase
        UnknownSpecifierType           = 0,
        FormatCodePoint8               = 1,
        FormatCodePoint16              = 2,
        FormatStringUTF8               = 3,
        FormatStringUTF16              = 4,
        FormatPercent                  = 5,
        FormatBase2Integer             = 6,
        FormatUnsignedBase8Integer     = 7,
        FormatSignedBase10Integer      = 8,
        FormatUnsignedBase16IntegerU   = 9,
        FormatUnsignedBase16IntegerL   = 10,
        FormatUnsignedBase10Integer    = 11,
        FormatBase10DecimalU           = 12,
        FormatBase10DecimalL           = 13,
        FormatScientificDecimalU       = 14,
        FormatScientificDecimalL       = 15,
        FormatShortestDecimalU         = 16,
        FormatShortestDecimalL         = 17,
        FormatBase16DecimalU           = 18,
        FormatBase16DecimalL           = 19,
    } FormatSpecifierTypes;
    
    typedef struct FormatSpecifier {
        uint64_t                  Offset;
        uint64_t                  Size;
        
        uint64_t                  MinWidth;
        uint64_t                  Precision;
        UTF32                    *String;
        FormatSpecifierFlags      Flag;
        FormatSpecifierLengths    Length;
        FormatSpecifierTypes      Type;
    } FormatSpecifier;
    
    typedef struct FormatString {
        uint64_t         NumSpecifiers;
        FormatSpecifier *Specifiers;
        StringTypes      StringType;
    } FormatString;
    
    static FormatString *FormatString_Init(uint64_t NumSpecifiers) {
        FormatString *NewFormatString          = calloc(1, sizeof(FormatString));
        if (NewFormatString != NULL) {
            NewFormatString->Specifiers        = calloc(NumSpecifiers, sizeof(FormatSpecifier));
            if (NewFormatString->Specifiers != NULL) {
                NewFormatString->NumSpecifiers = NumSpecifiers;
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate %lld Specifiers"), NumSpecifiers);
            }
        } else {
            Log(Log_ERROR, __func__, U8("Couldn't allocate FormatString"));
        }
        return NewFormatString;
    }
    
    static void FormatString_Deinit(FormatString *String2Deinit) {
        if (String2Deinit != NULL) {
            free(String2Deinit->Specifiers);
            free(String2Deinit);
        }
    }
    
    static FormatString *UTF32_ParseFormatSpecifiers(UTF32 *Format, StringTypes StringType) {
        FormatString *Details      = NULL;
        uint64_t NumSpecifiers     = 0ULL;
        uint64_t FormatStringSize  = 0ULL;
        if (Format != NULL && StringType != UnknownFormat) {
            FormatStringSize       = UTF32_GetStringSizeInCodePoints(Format);
            for (uint64_t CodePoint = 0ULL; CodePoint < FormatStringSize; CodePoint++) { // Get the number of specifiers, and init.
                if (Format[CodePoint] == U32('%')) {
                    NumSpecifiers += 1;
                }
            }
            Details = FormatString_Init(NumSpecifiers);
            if (Details != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < FormatStringSize; CodePoint++) {
                    if (Format[CodePoint] == U32('%')) {
                        Details->NumSpecifiers += 1;
                        Details->Specifiers[Details->NumSpecifiers - 1].Offset = CodePoint + 1;
                        for (uint64_t EndCodePoint = 1ULL; EndCodePoint + CodePoint < FormatStringSize; EndCodePoint++) {
                            UTF32 CurrentCodePoint = Format[CodePoint + EndCodePoint];
                            if (CurrentCodePoint == U'd' || CurrentCodePoint == U'i' || CurrentCodePoint == U'o' || CurrentCodePoint == U'u' ||
                                CurrentCodePoint == U'x' || CurrentCodePoint == U'X' || CurrentCodePoint == U'f' || CurrentCodePoint == U'F' ||
                                CurrentCodePoint == U'e' || CurrentCodePoint == U'E' || CurrentCodePoint == U'g' || CurrentCodePoint == U'G' ||
                                CurrentCodePoint == U'a' || CurrentCodePoint == U'A' || CurrentCodePoint == U'c' || CurrentCodePoint == U's' ||
                                CurrentCodePoint == U'%' || CurrentCodePoint == U'n' || CurrentCodePoint == U'r' || CurrentCodePoint == U't' ||
                                CurrentCodePoint == U'v') {
                                
                                Details->Specifiers[Details->NumSpecifiers - 1].Size = Details->Specifiers[Details->NumSpecifiers - 1].Offset - EndCodePoint;
                            }
                        }
                    }
                }
                for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                    uint64_t SpecifierStart = Details->Specifiers[Specifier].Offset + 1;
                    uint64_t SpecifierEnd   = Details->Specifiers[Specifier].Offset + Details->Specifiers[Specifier].Size;
                    for (uint64_t CodePoint = SpecifierStart; CodePoint < SpecifierEnd; Specifier++) {
                        if (CodePoint == SpecifierStart) { // Flags
                            switch (Format[CodePoint]) {
                                case U'-':
                                    Details->Specifiers[Specifier].Flag = LeftAlignFormatFlag;
                                    break;
                                case U'+':
                                    Details->Specifiers[Specifier].Flag = PrependSignFormatFlag;
                                    break;
                                case U' ':
                                    Details->Specifiers[Specifier].Flag = AlignSignsFormatFlag;
                                    break;
                                case U'#':
                                    Details->Specifiers[Specifier].Flag = TellTypeFormatFlag;
                                    break;
                                case U'0':
                                    Details->Specifiers[Specifier].Flag = PrependZerosFormatFlag;
                                    break;
                                default:
                                    Details->Specifiers[Specifier].Flag = RightAlignFormatFlag;
                                    break;
                            }
                        }
                        
                        /* MinWidth */
                        uint64_t MinWidthStart = CodePoint;
                        uint64_t MinWidthEnd   = 0ULL;
                        
                        do {
                            // Well I guess we just need to find the start and end of this minwidth parameter then call String2Integer on it, by copying it out?
                            // How do we set the start and end points tho?
                            MinWidthEnd = CodePoint;
                        } while (Format[CodePoint] == U'0' || Format[CodePoint] == U'1' || Format[CodePoint] == U'2' || Format[CodePoint] == U'3' || Format[CodePoint] == U'4' || Format[CodePoint] == U'5' || Format[CodePoint] == U'6' || Format[CodePoint] == U'7' || Format[CodePoint] == U'8' || Format[CodePoint] == U'9');
                        
                        if (MinWidthEnd > 0) {
                            // We found a MinWidth parameter, we need to cut out the section into a new string, and call String2Integer
                            // First get the size of the string by taking End - Start, create a new string, copy intot he new string from the format string at thse offsets, then call String2Integer on that.
                            UTF32 *MinWidthString                   = UTF32_ExtractSubString(Format, MinWidthStart, MinWidthEnd);
                            Details->Specifiers[Specifier].MinWidth = UTF32_String2Integer(MinWidthString);
                            free(MinWidthString);
                        }
                        /* MinWidth */
                        
                        /* Precision */
                        if (Format[CodePoint] == U'.') {
                            /* Now we basically do the same thing as we did in MinWidth, but this time for the Precision */
                            // FIXME: we need to replace all this gobedy gook with ExtractSubString
                            uint64_t PrecisionStart = CodePoint + 1;
                            uint64_t PrecisionEnd   = 0ULL;
                            do {
                                PrecisionEnd        = CodePoint;
                            } while (Format[CodePoint] == U'0' || Format[CodePoint] == U'1' || Format[CodePoint] == U'2' || Format[CodePoint] == U'3' || Format[CodePoint] == U'4' || Format[CodePoint] == U'5' || Format[CodePoint] == U'6' || Format[CodePoint] == U'7' || Format[CodePoint] == U'8' || Format[CodePoint] == U'9');
                            
                            if (PrecisionEnd > 0) {
                                UTF32 *PrecisionString                   = UTF32_ExtractSubString(Format, PrecisionStart, PrecisionEnd);
                                Details->Specifiers[Specifier].Precision = UTF32_String2Integer(PrecisionString);
                                free(PrecisionString);
                            }
                        }
                        /* Precision */
                        
                        /* Length */
                        if (Format[CodePoint] == U32('h') && Format[CodePoint + 1] == U32('h')) { // Length specifiers
                            Details->Specifiers[Specifier].Length = CharPromoted2Int;
                        } else if (Format[CodePoint] == U32('l') && Format[CodePoint] == U32('l')) {
                            Details->Specifiers[Specifier].Length = LongLongLengthSpecifier;
                        } else if (Format[CodePoint] == U32('h')) {
                            Details->Specifiers[Specifier].Length = ShortPromoted2Int;
                        } else if (Format[CodePoint] == U32('l')) {
                            Details->Specifiers[Specifier].Length = LongLengthSpecifier;
                        } else if (Format[CodePoint] == U32('L')) {
                            Details->Specifiers[Specifier].Length = LongDoubleSpecifier;
                        } else if (Format[CodePoint] == U32('z')) {
                            Details->Specifiers[Specifier].Length = Size_TSpecifiier;
                        } else if (Format[CodePoint] == U32('j')) {
                            Details->Specifiers[Specifier].Length = IntMaxSpecifier;
                        } else if (Format[CodePoint] == U32('t')) {
                            Details->Specifiers[Specifier].Length = PtrDiffSpecifier;
                        }
                        /* Length */
                        
                        /* Type */
                        if (Format[CodePoint] == U32('d') || Format[CodePoint] == U32('i')) {
                            Details->Specifiers[Specifier].Type   = FormatSignedBase10Integer;
                        } else if (Format[CodePoint] == U32('u')) {
                            Details->Specifiers[Specifier].Type   = FormatUnsignedBase10Integer;
                        } else if (Format[CodePoint] == U32('x')) {
                            Details->Specifiers[Specifier].Type   = FormatBase16DecimalL;
                        } else if (Format[CodePoint] == U32('X')) {
                            Details->Specifiers[Specifier].Type   = FormatBase16DecimalU;
                        } else if (Format[CodePoint] == U32('o')) {
                            Details->Specifiers[Specifier].Type   = FormatUnsignedBase8Integer;
                        } else if (Format[CodePoint] == U32('s')) {
                            if (Details->StringType == UTF8Format) {
                                Details->Specifiers[Specifier].Type = FormatStringUTF8;
                            } else if (Details->StringType == UTF16Format) {
                                Details->Specifiers[Specifier].Type = FormatStringUTF16;
                            }
                        } else if (Format[CodePoint] == U32('c')) {
                            if (Details->StringType == UTF8Format) {
                                Details->Specifiers[Specifier].Type = FormatCodePoint8;
                            } else if (Details->StringType == UTF16Format) {
                                Details->Specifiers[Specifier].Type = FormatCodePoint16;
                            }
                        } else if (Format[CodePoint] == U32('%')) {
                            // Just write  percent codepoint
                            Details->Specifiers[Specifier].Type     = FormatPercent;
                        } else if (Format[CodePoint] == U32('f')) {
                            Details->Specifiers[Specifier].Type     = FormatBase10DecimalL;
                        } else if (Format[CodePoint] == U32('F')) {
                            Details->Specifiers[Specifier].Type     = FormatBase10DecimalU;
                        } else if (Format[CodePoint] == U32('e')) {
                            Details->Specifiers[Specifier].Type     = FormatScientificDecimalL;
                        } else if (Format[CodePoint] == U32('E')) {
                            Details->Specifiers[Specifier].Type     = FormatScientificDecimalU;
                        } else if (Format[CodePoint] == U32('g')) {
                            Details->Specifiers[Specifier].Type     = FormatShortestDecimalL;
                        } else if (Format[CodePoint] == U32('G')) {
                            Details->Specifiers[Specifier].Type     = FormatShortestDecimalU;
                        } else if (Format[CodePoint] == U32('a')) {
                            Details->Specifiers[Specifier].Type     = FormatBase16DecimalL;
                        } else if (Format[CodePoint] == U32('A')) {
                            Details->Specifiers[Specifier].Type     = FormatBase16DecimalU;
                        }
                        /* Type */
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate FormatString"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Details;
    }
    
    UTF32 *UTF32_FormatString(UTF32 *Format, FormatString *Details, va_list VariadicArguments) {
        UTF32 *Formatted = Format;
        if (Format != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) { // Stringify each specifier
                FormatSpecifierTypes Type = Details->Specifiers[Specifier].Type;
                if (Type == FormatStringUTF8 || Type == FormatCodePoint8) {
                    Details->Specifiers[Specifier].String = UTF8_Decode(va_arg(VariadicArguments, UTF8*));
                } else if (Type == FormatStringUTF16 || Type == FormatCodePoint16) {
                    Details->Specifiers[Specifier].String = UTF16_Decode(va_arg(VariadicArguments, UTF16*));
                } else if (Type == FormatPercent) {
                    Details->Specifiers[Specifier].String = U32("%");
                } else if (Type == FormatBase10DecimalL) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(Decimal, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase10DecimalU) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(Decimal, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase16DecimalL) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(HexLower, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase16DecimalU) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(HexUpper, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase2Integer) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(Binary, va_arg(VariadicArguments, double));
                } else if (Type == FormatScientificDecimalU)
            }
        } else {
            Log(Log_ERROR, __func__, U8("FormatString Pointer is NULL"));
        }
        return Formatted;
    }
    
    UTF8 *UTF8_FormatString(UTF8 *Format, ...) {
        UTF8 *Format8 = NULL;
        if (Format != NULL) {
            UTF32 *Format32               = UTF8_Decode(Format);
            FormatString *Details         = UTF32_ParseFormatSpecifiers(Format32, UTF8Format);
            va_list VariadicArguments;
            UTF32 *FormattedString        = UTF32_FormatString(Format32, Details, VariadicArguments);
            va_end(VariadicArguments);
            FormatString_Deinit(Details);
            Format8                       = UTF8_Encode(FormattedString, No);
            free(Format32);
            free(FormattedString);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Format8;
    }
    
    UTF16 *UTF16_FormatString(UTF16 *Format, ...) {
        UTF16 *Format16 = NULL;
        if (Format != NULL) {
            UTF32 *Format32               = UTF16_Decode(Format);
            FormatString *Details         = UTF32_ParseFormatSpecifiers(Format32, UTF16Format);
            va_list VariadicArguments;
            UTF32 *FormattedString        = UTF32_FormatString(Format32, Details, VariadicArguments);
            va_end(VariadicArguments);
            FormatString_Deinit(Details);
            Format16                      = UTF16_Encode(FormattedString, UseLEByteOrder);
            free(Format32);
            free(FormattedString);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Format16;
    }
    
    void UTF8_WriteString2File(UTF8 *String, FILE *OutputFile) {
        if (String != NULL && OutputFile != NULL) {
#if   (FoundationIOTargetOS == POSIX)
            fputs(String, OutputFile);
#elif (FoundationIOTargetOS == Windows)
            UTF32 *StringUTF32 = UTF8_Decode(String);
            UTF16 *StringUTF16 = UTF16_Encode(StringUTF32, UseLEByteOrder);
            fputws(StringUTF16, OutputFile);
            free(StringUTF32);
            free(StringUTF16);
#endif
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_ERROR, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_WriteString2File(UTF16 *String, FILE *OutputFile) {
        if (String != NULL && OutputFile != NULL) {
#if   (FoundationIOTargetOS == POSIX)
            UTF32 *StringUTF32 = UTF16_Decode(String);
            UTF8  *StringUTF8  = UTF8_Encode(StringUTF32, No);
            fputs(StringUTF8, OutputFile);
            free(StringUTF32);
            free(StringUTF8);
#elif (FoundationIOTargetOS == Windows)
            fputws(String, OutputFile);
#endif
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_ERROR, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
#ifdef  __cplusplus
}
#endif
