#include <stdarg.h>                    /* Included for va_start, va_end */
#include <stdint.h>                    /* Included for u/intX_t */
#include <stdlib.h>                    /* Included for calloc, free */

#include "../include/StringIOTables.h" /* Included for the tables, and StringIO declarations */
#include "../include/Math.h"           /* Included for endian swapping */
#include "../include/Log.h"            /* Included for error logging */

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
            Log(Log_ERROR, __func__, U8("String2Count Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("String2Count Pointer is NULL"));
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
    
    UTF8 *UTF8_AddBOM(UTF8 *String, StringIOByteOrders BOM2Add) {
        UTF8 *StringWithBOM = NULL;
        if (String != NULL) {
            if (String[0] != 0xEF && String[1] != 0xBB && String[2] != 0xBF) {
                // Reallocate the string and add 3 more codeunits
            }
            if (String[0] != UTF16LE && String[0] != UTF16BE) {
                uint64_t StringSize = UTF16_GetStringSizeInCodeUnits(String);
                StringWithBOM       = calloc(StringSize + UTF1632BOMSizeInCodeUnits + NULLTerminatorSize, sizeof(UTF16));
                if (StringWithBOM != NULL) {
                    if (BOM2Add == UseNativeByteOrder) {
                        GetRuntimeByteBitOrder();
                        
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("StringWithBOM couldn't be allocated"));
                }
            } else {
                // We need to verify the String's byte order, how do we do that? well if a codeunit contains a high surrogate or low surrogate you can tell by it's value, otherwise look for code units who's value is less than 256, or greater than 256, that'll tell you which side of the string contains the low bits.
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF16 *UTF16_AddBOM(UTF16 *String, StringIOByteOrders BOM2Add) {
        UTF16   *StringWithBOM = NULL;
        uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(String);
        uint16_t ByteOrder     = 0;
        if (String != NULL) {
            if (String[0] != UTF16LE && String[0] != UTF16BE) {
                uint64_t StringSize = UTF16_GetStringSizeInCodeUnits(String);
                StringWithBOM       = calloc(StringSize + UTF1632BOMSizeInCodeUnits + NULLTerminatorSize, sizeof(UTF16));
                if (StringWithBOM != NULL) {
                    if (BOM2Add == UseNativeByteOrder) {
                        GetRuntimeByteBitOrder();
                        if (GlobalByteOrder == LSByteFirst) {
                            // Lil endian
                            ByteOrder = UTF16LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            // Big endian
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
        return NULL;
    }
    
    UTF8 *UTF8_RemoveBOM(UTF8 *String) {
        UTF8    *BOMLessString = NULL;
        uint64_t StringSize    = 0ULL;
        if (String != NULL) {
            StringSize         = UTF8_GetStringSizeInCodeUnits(String);
            // Check to see if there is a BOM in the first place, then remove it
            if (String[0] == 0xEF && String[1] == 0xBB && String[2] == 0xBF) {
                BOMLessString  = calloc(StringSize - UTF8BOMSizeInCodeUnits + NULLTerminatorSize, sizeof(UTF8));
                if (BOMLessString != NULL) {
                    // Ok, so now we just start copying the string
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
        UTF16   *BOMLessString = NULL;
        uint64_t StringSize    = 0ULL;
        if (String != NULL) {
            StringSize         = UTF16_GetStringSizeInCodeUnits(String);
            // Check to see if there is a BOM in the first place, then remove it
            if (String[0] == UTF16BE || String[0] == UTF16LE) {
                BOMLessString  = calloc(StringSize - UTF1632BOMSizeInCodeUnits + NULLTerminatorSize, sizeof(UTF16));
                if (BOMLessString != NULL) {
                    // Ok, so now we just start copying the string
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
        uint8_t  CodePointSize                       = 0;
        uint64_t CodeUnitNum                         = 0ULL;
        uint64_t CodePoint                           = 0ULL;
        UTF32   *DecodedString                       = NULL;
        if (String != NULL) {
            uint64_t NumCodePoints                   = UTF8_GetStringSizeInCodePoints(String) + UTF1632BOMSizeInCodeUnits + NULLTerminatorSize;
            if (String[0] == 0xEF && String[1] == 0xBB && String[2] == 0xBF) { // We need to remove the BOM, and subtract it's size from the Num codepoints
                NumCodePoints                       -= UTF8BOMSizeInCodePoints;
                CodeUnitNum                         += UTF8BOMSizeInCodeUnits;
            }
            DecodedString                            = calloc(NumCodePoints, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if (CodePoint == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            DecodedString[0]         = UTF32LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            DecodedString[0]         = UTF32BE;
                        }
                    }
                    switch (UTF8_GetCodePointSize(String[CodeUnitNum])) { // UTF-8 is MSB first, if the platform is LSB first, we need to swap as we read
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
                    if (GlobalByteOrder == LSByteFirst) {
                        DecodedString[CodePoint]     = SwapEndian32(String[CodePoint]);
                    }
                    
                    if (CodePointSize > 1 && DecodedString[CodePoint] <= 0x7F) {
                        DecodedString[CodePoint]     = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("CodePoint %d is overlong, replaced with U+FFFD"), DecodedString[CodePoint]);
                    } else if (DecodedString[CodePoint] >= UTF16HighSurrogateStart && DecodedString[CodePoint] <= UTF16LowSurrogateEnd && DecodedString[CodePoint] != UTF32LE && DecodedString[CodePoint] != UTF32BE) {
                        DecodedString[CodePoint]     = InvalidCodePointReplacementCharacter;
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
    
    UTF32 *UTF16_Decode(UTF16 *String) { // We need to decode the string from UTF-16BE/LE to UTF-32Native
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder(); // so now we know the byte order of the running machine, all we need to do is get the UTF-16 string's byte order (hint, we should have a function for that), and then decode it properly.
        }
        uint64_t CodePoint                       = 0ULL;
        uint64_t CodeUnitNum                     = 0ULL;
        UTF16    StringByteOrder                 = 0U;
        UTF32   *DecodedString                   = NULL;
        if (String != NULL) {
            StringByteOrder                      = String[0];
            uint64_t UTF16CodeUnits              = UTF16_GetStringSizeInCodePoints(String) + NULLTerminatorSize;
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
                } while (String[CodePoint] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("DecodedString Pointer is NULL, not enough memory"));
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
                        String[CodePoint]              = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD"), String[CodePoint]);
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("String Pointer is NULL, not enough memory"));
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
            uint64_t UTF16NumCodeUnits                 = UTF1632BOMSizeInCodeUnits + UTF32_GetStringSizeInUTF16CodeUnits(String) + NULLTerminatorSize;
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
                Log(Log_ERROR, __func__, U8("EncodedString Pointer is NULL, not enough memory"));
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
        uint64_t CodePoint        = UTF1632BOMSizeInCodeUnits;
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
    
    static UTF32 *UTF32_Compose(UTF32 *String, bool Kompatibility) { // FIXME: Must use a stable sorting algorithm
        uint64_t CodePoint      = UTF1632BOMSizeInCodeUnits;
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
        uint64_t CodePoint        = 0ULL;
        UTF32   *DecomposedString = NULL;
        if (String != NULL && (Kompatibility == No || Kompatibility == Yes)) {
            do {
                if (Kompatibility == Yes) {
                    for (uint64_t Index = 0; Index < KompatibleNormalizationTableSize; Index++) {
                        if (String[CodePoint] == KompatibleNormalizationCodePoints[Index]) {
                            DecomposedString    = UTF32_ReplaceSubString(String, KompatibleNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                } else {
                    for (uint64_t Index = 0; Index < CanonicalNormalizationTableSize; Index++) {
                        if (String[CodePoint] == CanonicalNormalizationCodePoints[Index]) {
                            DecomposedString = UTF32_ReplaceSubString(String, CanonicalNormalizationStrings[Index], CodePoint, 1);
                        }
                    }
                }
                CodePoint += 1;
            } while (String[CodePoint] != NULLTerminator);
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
        uint64_t  ExtractedStringSize                   = Length + UTF1632BOMSizeInCodeUnits + NULLTerminatorSize;
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
        uint64_t CodePoint =  0ULL;
        int8_t   Sign      =  1;
        uint8_t  Base      = 10;
        int64_t  Value     =  0;
        if (String != NULL) {
            for (uint64_t WhiteSpace = 0; WhiteSpace < WhiteSpaceTableSize; WhiteSpace++) {
                if (String[CodePoint] == WhiteSpaceTable[WhiteSpace]) {
                    CodePoint    += 1;
                }
            }
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
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return (Sign < 0 ? Value * Sign : Value);
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
        if (GlobalByteOrder == LSByteFirst) {
            Integer2Convert      = SwapEndian64(Integer2Convert);
        }
        if (Integer2Convert < 0 && Base == Decimal) {
            Sign                 = -1;
            NumDigits           +=  1;
        }
        UTF32 *NumberString      = calloc(NumDigits + NULLTerminatorSize, sizeof(UTF32));
        UTF32  UpperNumerals[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        UTF32  LowerNumerals[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
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
                    if (String[CodePoint] == '-') {
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
    
    UTF8 *UTF8_Decimal2String(double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Decimal);
        UTF8  *String8  = UTF8_Encode(String32, No);
        free(String32);
        return String8;
    }
    
    UTF16 *UTF16_Decimal2String(double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Decimal);
        UTF16 *String16 = UTF16_Encode(String32, UseLEByteOrder);
        free(String32);
        return String16;
    }
    
    UTF32 *UTF32_Decimal2String(double Decimal) {
        /*
         Should we support Binary Coded Decimal BCD?
         Invalid doubles return a NULL string
         */
        UTF32   *OutputString = NULL;
        int8_t   Sign         = Decimal < 0.0 ? -1 : 1;
        int32_t *Exponent     = calloc(1, sizeof(int32_t));
        int32_t  Exponent2    = Exponent;
        float    Mantissa     = frexp(Decimal, Exponent);
        float    Mantissa2    = Mantissa;
        
        // Ok now we just need to get the strings size
        uint64_t StringSize   = UTF1632BOMSizeInCodeUnits; // 1 to account for the BOM
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
            if (GlobalByteOrder == LSByteFirst) {
                OutputString[0]   = UTF32LE;
            } else if (GlobalByteOrder == MSByteFirst) {
                OutputString[0]   = UTF32BE;
            }
            if (Sign == -1) {
                OutputString[1]   = '-';
            }
            // Now we start popping in the other variables, first is the Exponent.
            while (Exponent2 > 0) { // TODO: This assumes there's only 1 codepoint nessicary to express teh exponent
                OutputString[2]   = Exponent2 /= 10;
            }
            OutputString[3]       = '.';
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
    
    UTF8 *UTF8_FormatString(UTF8 *Format, ...) {
        UTF8 *Format8 = NULL;
        if (Format != NULL) {
            UTF32 *Format32           = UTF8_Decode(Format);
            va_list VariadicArguments;
            va_start(VariadicArguments, Format);
            UTF32 *FormattedString    = UTF32_FormatString(Format32, VariadicArguments);
            va_end(VariadicArguments);
            Format8                   = UTF8_Encode(FormattedString, No);
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
            UTF32 *Format32           = UTF16_Decode(Format);
            va_list VariadicArguments;
            va_start(VariadicArguments, Format);
            UTF32 *FormattedString    = UTF32_FormatString(Format32, VariadicArguments);
            va_end(VariadicArguments);
            Format16                  = UTF16_Encode(FormattedString, UseLEByteOrder);
            free(Format32);
            free(FormattedString);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Format16;
    }
    
    UTF32 *UTF32_FormatString(UTF32 *Format, va_list Arguments) {
        /*
         Make an array to store the offset, size, and type of the format specifiers, then start looping to get those results.
         */
        
        typedef enum FormatSpecifierTypes { // U = uppercase, L = lowercase
            UnknownSpecifierType           = 0,
            FormatCodePoint8               = 1,
            FormatCodePoint16              = 2,
            FormatCodePoint32              = 3,
            FormatCodePointHex4            = 4,
            FormatCodePointHex8            = 5,
            FormatCodePointHexVariable     = 6,
            FormatStringUTF8               = 7,
            FormatStringUTF16              = 8,
            FormatAlarmBell                = 9,
            FormatNULLTerminatingChar      = 10,
            FormatBackspace                = 11,
            FormatFormFeed                 = 12,
            FormatNewLine                  = 13,
            FormatCarriageReturn           = 14,
            FormatTab                      = 15,
            FormatVerticalTab              = 16,
            FormatApostrophe               = 17,
            FormatQuote                    = 18,
            FormatBackslash                = 19,
            FormatPointer                  = 20,
            FormatBase2Integer             = 21,
            FormatUnsignedBase8Integer     = 22,
            FormatSignedBase10Integer      = 23,
            FormatUnsignedBase16IntegerU   = 24,
            FormatUnsignedBase16IntegerL   = 25,
            FormatUnsignedBase10Integer    = 26,
            FormatBase10DecimalU           = 27,
            FormatBase10DecimalL           = 28,
            FormatScientificDecimalU       = 29,
            FormatScientificDecimalL       = 30,
            FormatShortestDecimalU         = 31,
            FormatShortestDecimalL         = 32,
            FormatBase16DecimalU           = 34,
            FormatBase16DecimalL           = 35,
        } FormatSpecifierTypes;
        
        uint64_t NumSpecifiers             = 0ULL;
        uint64_t CurrentSpecifier          = 0ULL;
        uint64_t FormatStringSize          = 0ULL;
        
        if (Format != NULL) {
            FormatStringSize               = UTF32_GetStringSizeInCodePoints(Format);
            
            for (uint64_t CodePoint = 0ULL; CodePoint < FormatStringSize; CodePoint++) {
                if (Format[CodePoint] == U'%' || Format[CodePoint] == U'\\') {
                    NumSpecifiers         += 1;
                }
            }
            
            uint64_t                     *SpecifierOffset    = calloc(NumSpecifiers, sizeof(uint64_t));
            uint64_t                     *SpecifierSize      = calloc(NumSpecifiers, sizeof(uint64_t));
            uint64_t                     *SpecifierParameter = calloc(NumSpecifiers, sizeof(uint64_t));
            uint64_t                     *SpecifierFlags     = calloc(NumSpecifiers, sizeof(uint64_t));
            uint64_t                     *SpecifierWidth     = calloc(NumSpecifiers, sizeof(uint64_t));
            uint64_t                     *SpecifierPrecision = calloc(NumSpecifiers, sizeof(uint64_t));
            uint64_t                     *SpecifierLength    = calloc(NumSpecifiers, sizeof(uint64_t));
            FormatSpecifierTypes         *SpecifierType      = calloc(NumSpecifiers, sizeof(FormatSpecifierTypes));
            
            UTF32 TypeSpecifiers[21] = {U'a', U'A', U'b', U'c', U'C', U'd', U'e', U'E', U'f', U'F', U'g', U'G', U'i', U'o', U'p', U's', U'S', U'u', U'x', U'X', U'%'};
            
            // Ok, so we know how many specifiers there are, now we need to know their offsets and sizes.
            // So, that means we need to loop again.
            
            for (uint64_t CodePoint = 0ULL; CodePoint < FormatStringSize; CodePoint++) {
                for (uint64_t Type = 0ULL; Type < 21; Type++) {
                    for (uint64_t Specifier = 0ULL; Specifier < NumSpecifiers; Specifier++) {
                        if (Format[CodePoint] == U'%' || Format[CodePoint] == U'\\') {
                            // Found the start of a format specifier, so set the SpecifierOffset
                            SpecifierOffset[Specifier] = CodePoint;
                        }
                        if (Format[CodePoint] == TypeSpecifiers[Type]) {
                            SpecifierSize[Specifier] = CodePoint - SpecifierSize[Specifier];
                        }
                    }
                }
            }
            
            // 3rd loop, the logic needs to be while we're greater than or equal to SpecifierOffset but less than or equal to SpecifierSize, look for flags
            
            for (uint64_t CodePoint = 0ULL; CodePoint < FormatStringSize; CodePoint++) {
                for (uint64_t Specifier = 0ULL; Specifier < NumSpecifiers; Specifier++) {
                    while (CodePoint >= SpecifierOffset[Specifier] && CodePoint <= SpecifierOffset[Specifier] + SpecifierSize[Specifier]) {
                        // we're inside of a specifier, start reading this specifiers type, and with that info start creatng strings
                    }
                }
            }
            
            
            for (uint64_t CodePoint = 0ULL; CodePoint < FormatStringSize; CodePoint++) { // We know how many specifiers there are, now we need to figure out each ones properties.
                if (Format[CodePoint] == '%') {
                    // Ok so now we check for all of the various specifiers, flags, widhts, precisions, lengths, and types.
                    
                    switch (Format[CodePoint + 1]) { // Flags, + 1 needs to be replaced by a second loop
                        case U'-':
                            // Left align
                            break;
                        case U'+':
                            // Prepend a sign symbol (+ or -) to a number
                            break;
                        case U' ':
                            // Prepend a space for positive numbers or a - for negative ones
                            break;
                        case U'0':
                            // When the width field exists, prepend 0's for numeric types, the default is to prepend spaces
                            break;
                            
                        case U'#':
                            // for g and G types, trailing 0's aren't removed, for f F e E g G types, the output always contains a decimal; for o, x X types o,ox, oX is prepended to non-zero numbers.
                            break;
                    }
                    
                    switch (Format[CodePoint + 1]) { // Width, + 1 needs to be replaced by a second loop
                        case U'*': // Get the value from the field
                            
                            break;
                            
                        default:
                            break;
                    }
                    
                    /*
                     So, we've found a percent symbol, lets start checking the next symbol against all the shit.
                     */
                    
                    /*
                     Control logic:
                     
                     Loop until we find a codepoint matching a type, which has to be last.
                     
                     
                     */
                    
                    for (uint64_t Specifier = 1ULL; Specifier < FormatStringSize - CodePoint; Specifier++) {
                        for (uint8_t Type = 0; Type < 21; Type++) {
                            if (Format[CodePoint + Specifier] == TypeSpecifiers[Type]) {
                                SpecifierSize[CurrentSpecifier] = Specifier;
                            }
                        }
                    }
                    
                    switch (Format[CodePoint + 1]) { // Types
                        case U'a':
                            break;
                        case U'A':
                            break;
                        case U'b': // Binary, extension
                            break;
                        case U'c':
                            break;
                        case U'C': // Windows extension for "wide" characters
                            break;
                        case U'd':
                            break;
                        case U'e':
                            break;
                        case U'E':
                            break;
                        case U'f':
                            break;
                        case U'F':
                            break;
                        case U'g':
                            break;
                        case U'G':
                            break;
                        case U'i':
                            break;
                        case U'o':
                            break;
                        case U'p':
                            break;
                        case U's':
                            break;
                        case U'S': // Windows extension for "wide" strings
                            break;
                        case U'u':
                            break;
                        case U'x':
                            break;
                        case U'X':
                            break;
                        case U'%':
                            break;
                    }
                } else if (Format[CodePoint] == '\\') {
                    // This is a lot easier, just look for the previously listed ones, and set the offsets and shit.
                    SpecifierOffset[CurrentSpecifier] = CodePoint;
                    SpecifierSize[CurrentSpecifier]   = 2; // 1 for the / and another for whatever character this matches
                    switch (Format[CodePoint + 1]) {
                        case U'0':  // NULL terminator
                            SpecifierType[CurrentSpecifier] = FormatNULLTerminatingChar;
                            break;
                        case U'a':  // Alarm aka beep
                            SpecifierType[CurrentSpecifier] = FormatAlarmBell;
                            break;
                        case U'b':  // Backspace
                            SpecifierType[CurrentSpecifier] = FormatBackspace;
                            break;
                        case U'f':  // Form feed
                            SpecifierType[CurrentSpecifier] = FormatFormFeed;
                            break;
                        case U'n':  // Newline
                            SpecifierType[CurrentSpecifier] = FormatNewLine;
                            break;
                        case U'r':  // Carriage return
                            SpecifierType[CurrentSpecifier] = FormatCarriageReturn;
                            break;
                        case U't':  // Horizontal tab
                            SpecifierType[CurrentSpecifier] = FormatTab;
                            break;
                        case U'u':  // Unicode 4 digit codepoint
                            SpecifierType[CurrentSpecifier] = FormatCodePointHex4;
                            break;
                        case U'U':  // Unicode 8 digit codepoint
                            SpecifierType[CurrentSpecifier] = FormatCodePointHex8;
                            break;
                        case U'v':  // Vertical tab
                            SpecifierType[CurrentSpecifier] = FormatVerticalTab;
                            break;
                        case U'x':  // Hex specifier
                            SpecifierType[CurrentSpecifier] = FormatCodePointHexVariable;
                            break;
                        case U'\'': // Apostrophe
                            SpecifierType[CurrentSpecifier] = FormatApostrophe;
                            break;
                        case U'"':  // Quote
                            SpecifierType[CurrentSpecifier] = FormatQuote;
                            break;
                        case U'\\': // Backslash
                            SpecifierType[CurrentSpecifier] = FormatBackslash;
                            break;
                    }
                }
                free(SpecifierOffset);
                free(SpecifierSize);
                free(SpecifierParameter);
                free(SpecifierFlags);
                free(SpecifierWidth);
                free(SpecifierPrecision);
                free(SpecifierLength);
                free(SpecifierType);
            }
        } else {
            Log(Log_ERROR, __func__, U8("Format Pointer is NULL"));
        }
        return NULL;
    }
    
    uint8_t UTF16_DetectByteOrder(UTF16 *String) { // UTF-16BE = U+AB30, UTF-16LE = U+30AB
        // UTF-32BE = U+1F984, UTF-32LE = U+84F901, UTF-16BE = 0xD83E 0xDD84
        
        // So, for codepoints greater than 0x10000 we need to identify the high surrogate, which is between 0xD800 and 0xDBFF, and the low surrogate which is between 0xDC00 and 0xDFFF.
        uint64_t CodeUnit = 0ULL;
        if (String != NULL) {
            // Should we trust the byte order mark if it exists? I'm thinking no. if someone calls this function, it's because they want to know the real byte order.
            do {
                if (String[CodeUnit] < 256) {
                    
                } else if (String[CodeUnit] >= 256 && (String[CodeUnit] < UTF16LowSurrogateStart)) {
                    
                } else if (String[CodeUnit] >= UTF16HighSurrogateStart && String[CodeUnit] <= UTF16HighSurrogateEnd) { // High surrogate
                    
                }
            } while (String[CodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return 0;
    }
    
    void UTF8_WriteString2File(UTF8 *String, FILE *OutputFile) {
        if (String != NULL && OutputFile != NULL) {
#if   (FoundationIOTargetOS == POSIXOS)
            fputs(String, OutputFile);
#elif (FoundationIOTargetOS == WindowsOS)
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
#if   (FoundationIOTargetOS == POSIXOS)
            UTF32 *StringUTF32 = UTF16_Decode(String);
            UTF8  *StringUTF8  = UTF8_Encode(StringUTF32, No);
            fputs(StringUTF8, OutputFile);
            free(StringUTF32);
            free(StringUTF8);
#elif (FoundationIOTargetOS == WindowsOS)
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

