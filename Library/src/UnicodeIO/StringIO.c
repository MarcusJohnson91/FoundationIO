#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/StringIOTables.h" /* Included for the tables, and StringIO declarations, NEEDS TO BE FIRST? */
#include "../include/Log.h"            /* Included for error logging */
#include "../include/Math.h"           /* Included for endian swapping */

#ifdef __cplusplus
extern "C" {
#endif
    
    /* API Design: All UTF-32 Strings will contain a BOM that we create anyway. */
    
    /* Basic String Property Functions */
    uint8_t UTF8_GetCodePointSize(UTF8 CodeUnit) {
        uint8_t CodePointSize      = 0;
        if (((CodeUnit & 0x80) >> 7) == 0) {
            CodePointSize          = 1;
        } else if (((CodeUnit & 0xF8) >> 3) == 0x1E) {
            CodePointSize          = 4;
        } else if (((CodeUnit & 0xF0) >> 4) == 0x0E) {
            CodePointSize          = 3;
        } else if (((CodeUnit & 0xE0) >> 5) == 0x06) {
            CodePointSize          = 2;
        } else if (((CodeUnit & 0xC0) >> 6) == 0x02) {
            CodePointSize          = 1;
        } else {
            CodePointSize          = 0;
        }
        return CodePointSize;
    }
    
    uint8_t UTF16_GetCodePointSize(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit < UTF16HighSurrogateStart || (CodeUnit > UTF16LowSurrogateEnd && CodeUnit <= UTF16MaxCodePoint)) {
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
                for (uint64_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
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
    
    bool UTF8_StringHasBOM(UTF8 *String) {
        bool StringHasABOM = No;
        if (String != NULL) {
            // Get String Size
            uint64_t StringSize = UTF8_GetStringSizeInCodeUnits(String);
            if (StringSize >= 3) {
                if (String[0] == 0xEF && String[1] == 0xBB && String[2] == 0xBF) {
                    StringHasABOM = Yes;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool UTF16_StringHasBOM(UTF16 *String) {
        bool StringHasABOM = No;
        if (String != NULL) {
            // Get String Size
            uint64_t StringSize = UTF16_GetStringSizeInCodeUnits(String);
            if (StringSize >= 1) {
                if (String[0] == UTF16LE || String[0] == UTF16BE) {
                    StringHasABOM = Yes;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool UTF32_StringHasBOM(UTF32 *String) {
        bool StringHasABOM = No;
        if (String != NULL) {
            // Get String Size
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            if (StringSize >= 1) {
                if (String[0] == UTF32LE || String[0] == UTF32BE) {
                    StringHasABOM = Yes;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool  UTF8_StringHasWinPathPrefix(UTF8 *String) {
        bool StringHasWinPathPrefix = No;
        if (String != NULL) {
            UTF32 *String32         = UTF8_Decode(String);
            StringHasWinPathPrefix  = UTF32_StringHasWinPathPrefix(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringHasWinPathPrefix;
    }
    
    bool  UTF16_StringHasWinPathPrefix(UTF16 *String) {
        bool StringHasWinPathPrefix = No;
        if (String != NULL) {
            UTF32 *String32         = UTF16_Decode(String);
            StringHasWinPathPrefix  = UTF32_StringHasWinPathPrefix(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringHasWinPathPrefix;
    }
    
    bool  UTF32_StringHasWinPathPrefix(UTF32 *String) {
        bool StringHasLongPathPrefix = No;
        if (String != NULL) {
            uint64_t StringSize      = UTF32_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM    = UTF32_StringHasBOM(String);
                if (StringHasBOM && StringSize >= UTF8BOMSizeInCodeUnits + 4) { // 4 for "\\?\"
                    if ((String[1] == U32('\\') || String[1] == U32('/')) && (String[2] == U32('\\') || String[2] == U32('/')) && (String[3] == U32('\?')) && (String[4] == U32('\\') || String[4] == U32('/'))) {
                        StringHasLongPathPrefix = Yes;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    if ((String[0] == U32('\\') || String[0] == U32('/')) && (String[1] == U32('\\') || String[1] == U32('/')) && (String[2] == U32('\?')) &&
                        (String[3] == U32('\\') || String[3] == U32('/'))) {
                        StringHasLongPathPrefix = Yes;
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringHasLongPathPrefix;
    }
    
    uint64_t UTF8_NumFormatSpecifiers(UTF8 *String) {
        uint64_t NumFormatSpecifiers = 0ULL;
        if (String != NULL) {
            UTF32 *String32          = UTF8_Decode(String);
            NumFormatSpecifiers      = UTF32_NumFormatSpecifiers(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumFormatSpecifiers;
    }
    
    uint64_t UTF16_NumFormatSpecifiers(UTF16 *String) {
        uint64_t NumFormatSpecifiers = 0ULL;
        if (String != NULL) {
            UTF32 *String32          = UTF16_Decode(String);
            NumFormatSpecifiers      = UTF32_NumFormatSpecifiers(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumFormatSpecifiers;
    }
    
    uint64_t UTF32_NumFormatSpecifiers(UTF32 *String) {
        uint64_t NumFormatSpecifiers = 0ULL;
        if (String != NULL) {
            uint64_t CodePoint = 1ULL;
            do {
                if (String[CodePoint - 1] == U32('%') && String[CodePoint] != U32('%')) {
                    NumFormatSpecifiers += 1;
                }
                CodePoint += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumFormatSpecifiers;
    }
    
    bool  UTF8_IsStringValid(UTF8 *String) {
        uint64_t CodeUnit    = 0ULL;
        bool     IsValidUTF8 = Yes;
        if (String != NULL) {
            do {
                // Get the CodePoint size
                // How about, instead we just get if it's a UTF-8 header?
                bool IsUTF8Header = String[CodeUnit] & 0x80 >> 7;
                if (IsUTF8Header == Yes) {
                    // Do magic
                } else {
                    // idek
                }
                
                
                
                if (String[CodeUnit] == 0xC0 || String[CodeUnit] == 0xC1 || (String[CodeUnit] >= 0xF5 && String[CodeUnit] <= 0xFF)) {
                    IsValidUTF8 = No;
                    break;
                } else {
                    if (((String[CodeUnit] & 0x80) >> 7) == 1) {
                        // Extract the top
                        UTF8 Wat = (String[CodeUnit] & 0xF8) >> 3;
                        if (Wat == 0x1F) {
                            IsValidUTF8 = No;
                            break;
                        }
                    }
                }
                CodeUnit += 1;
            } while (String[CodeUnit] != NULLTerminator);
            if (IsValidUTF8 == No) {
                Log(Log_ERROR, __func__, U8("String is invalid"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return IsValidUTF8;
    }
    
    bool UTF16_IsStringValid(UTF16 *String) {
        uint64_t CodeUnit             = 1ULL;
        bool     IsValidUTF16         = Yes;
        if (String != NULL) {
            do {
                if ((String[CodeUnit - 1] >= UTF16HighSurrogateStart && String[CodeUnit - 1] <= UTF16HighSurrogateEnd) && (String[CodeUnit] >= UTF16LowSurrogateStart && String[CodeUnit] <= UTF16LowSurrogateEnd)) {
                    IsValidUTF16 = No;
                    break;
                }
                CodeUnit += 1;
            } while (String[CodeUnit] != NULLTerminator);
            if (IsValidUTF16 == No) {
                Log(Log_ERROR, __func__, U8("String is invalid"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return IsValidUTF16;
    }
    
    bool UTF32_IsStringValid(UTF32 *String) {
        uint64_t CodePoint       = 0ULL;
        bool     IsValidUTF32    = Yes;
        if (String != NULL) {
            do {
                if (String[CodePoint] >= UnicodeMaxCodePoint || (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd)) {
                    IsValidUTF32 = No;
                    break;
                }
                CodePoint       += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return IsValidUTF32;
    }
    
    UTF8 *UTF8_AddBOM(UTF8 *String) {
        UTF8 *StringWithBOM = NULL;
        if (String != NULL) {
            UTF32 *String32  = UTF8_Decode(String);
            UTF32 *BOMAdded  = UTF32_AddBOM(String32, UseBEByteOrder);
            free(String32);
            StringWithBOM    = UTF8_Encode(BOMAdded);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF32 *UTF32_AddBOM(UTF32 *String, StringIOByteOrders BOM2Add) {
        UTF32   *StringWithBOM        = NULL;
        UTF32    ByteOrder            = 0;
        if (String != NULL) {
            if (String[0] != UTF32LE && String[0] != UTF32BE) {
                uint64_t StringSize   = UTF32_GetStringSizeInCodePoints(String) + UnicodeBOMSizeInCodePoints + NULLTerminatorSize;
                StringWithBOM         = calloc(StringSize, sizeof(UTF32));
                if (StringWithBOM != NULL) {
                    if (BOM2Add == UseNativeByteOrder) {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                        ByteOrder     = UTF32LE;
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                        ByteOrder     = UTF32BE;
#endif
                    } else if (BOM2Add == UseLEByteOrder) {
                        ByteOrder     = UTF32LE;
                    } else if (BOM2Add == UseBEByteOrder) {
                        ByteOrder     = UTF32BE;
                    }
                    StringWithBOM[0] = ByteOrder;
                    for (uint64_t CodePoint = 1ULL; CodePoint < StringSize; CodePoint++) {
                        StringWithBOM[CodePoint] = String[CodePoint + 1];
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
    
    UTF32 *UTF32_RemoveBOM(UTF32 *String) {
        UTF32   *BOMLessString                   = NULL;
        uint64_t StringSize                      = 0ULL;
        if (String != NULL) {
            StringSize                           = UTF32_GetStringSizeInCodePoints(String);
            if (String[0] == UTF32LE || String[0] == UTF32BE) {
                BOMLessString                    = calloc(StringSize - UnicodeBOMSizeInCodePoints + NULLTerminatorSize, sizeof(UTF32));
                if (BOMLessString != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                        BOMLessString[CodePoint] = String[CodePoint + 1];
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
        uint64_t StringSize                              = 0ULL;
        uint64_t CodePoint                               = 0ULL;
        uint64_t CodeUnit                                = 0ULL;
        UTF32   *DecodedString                           = NULL;
        
        if (String != NULL) {
            StringSize = UTF8_GetStringSizeInCodePoints(String);
            if (String[0] != 0xEF && String[1] != 0xBB && String[2] != 0xBF) {
                StringSize                              += UnicodeBOMSizeInCodePoints;
            } else {
                CodeUnit                                 = UTF8BOMSizeInCodeUnits;
            }
            DecodedString                                = calloc(StringSize, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if (CodePoint == 0) {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                        DecodedString[0]                    = UTF32LE;
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                        DecodedString[0]                    = UTF32BE;
#endif
                    } else {
                        uint8_t CodePointSize            = UTF8_GetCodePointSize(String[CodeUnit]);
                        switch (CodePointSize) {
                            case 1:
                                DecodedString[CodePoint] =  String[CodeUnit];
                                CodeUnit                += 1;
                                CodePoint               += 1;
                                break;
                            case 2:
                                DecodedString[CodePoint] = (String[CodeUnit]     & 0x1F) << 6;
                                DecodedString[CodePoint] =  String[CodeUnit + 1] & 0x3F;
                                CodeUnit                += 2;
                                CodePoint               += 1;
                                break;
                            case 3:
                                DecodedString[CodePoint] = (String[CodeUnit]     & 0x0F) << 12;
                                DecodedString[CodePoint] = (String[CodeUnit + 1] & 0x1F) << 6;
                                DecodedString[CodePoint] = (String[CodeUnit + 2] & 0x1F);
                                CodeUnit                += 3;
                                CodePoint               += 1;
                                break;
                            case 4:
                                DecodedString[CodePoint] = (String[CodeUnit]     & 0x07) << 18;
                                DecodedString[CodePoint] = (String[CodeUnit + 1] & 0x3F) << 12;
                                DecodedString[CodePoint] = (String[CodeUnit + 2] & 0x3F) <<  6;
                                DecodedString[CodePoint] = (String[CodeUnit + 3] & 0x3F);
                                CodeUnit                += 4;
                                CodePoint               += 1;
                                break;
                        }
                    }
                    if (DecodedString[CodePoint] >= UTF16HighSurrogateStart && DecodedString[CodePoint] <= UTF16LowSurrogateEnd) {
                        DecodedString[CodePoint]         = InvalidReplacementCodePoint;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, because it overlaps the Surrogate Pair Block, it was replaced with U+FFFD"), DecodedString[CodePoint]);
                    }
                } while (DecodedString[CodePoint] != NULLTerminator && String[CodeUnit] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate DecodedString"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF32 *UTF16_Decode(UTF16 *String) {  // CodeUnit = Input, CodePoint = Output
        uint64_t NumCodePoints                   = UTF16_GetStringSizeInCodePoints(String) + NULLTerminatorSize;
        uint64_t CodePoint                       = 0ULL;
        uint64_t CodeUnit                        = 0ULL;
        UTF32   *DecodedString                   = NULL;
        UTF16    StringsByteOrder                = 0;
        if (String != NULL) {
            if (String[0] == UTF16LE || String[0] == UTF16BE) {
                StringsByteOrder                 = String[0];
                NumCodePoints                   += 1;
            } else {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                StringsByteOrder = UTF16LE;
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                StringsByteOrder = UTF16BE;
#endif
                NumCodePoints   += 1;
            }
            DecodedString                        = calloc(NumCodePoints, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if (String[CodeUnit] < UTF16HighSurrogateStart || (String[CodeUnit] > UTF16LowSurrogateEnd && String[CodeUnit] <= UTF16MaxCodePoint)) {
                        UTF16 CodePoint2Swap     = String[CodeUnit];
                        DecodedString[CodePoint] = SwapEndian16(CodePoint2Swap);
                        CodeUnit += 1;
                    } else {
                        UTF16 HighSurrogate      = (SwapEndian16(String[CodeUnit])     - UTF16HighSurrogateStart) * UTF16SurrogatePairModDividend;
                        UTF16 LowSurrogate       =  SwapEndian16(String[CodeUnit + 1]) - UTF16LowSurrogateStart;
                        DecodedString[CodePoint] =  HighSurrogate + LowSurrogate + UTF16SurrogatePairStart;
                        CodeUnit += 2;
                    }
                } while (String[CodeUnit] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("DecodedString Pointer is NULL"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF8 *UTF8_Encode(UTF32 *String) {
        uint64_t CodePoint                             = 0ULL;
        uint64_t CodeUnitNum                           = 0ULL;
        UTF8    *EncodedString                         = NULL;
        bool     AddBOM2String                         = No;
        if (String != NULL) {
            uint64_t UTF8CodeUnits                     = NULLTerminatorSize + UTF32_GetStringSizeInUTF8CodeUnits(String) + (AddBOM2String == Yes ? UTF8BOMSizeInCodeUnits : 0);
            EncodedString                              = calloc(UTF8CodeUnits, sizeof(UTF8));
            if (EncodedString != NULL) {
                do {
                    if (CodeUnitNum == 0) {
                        EncodedString[CodeUnitNum]     = 0xEF;
                        EncodedString[CodeUnitNum + 1] = 0xBB;
                        EncodedString[CodeUnitNum + 2] = 0xBF;
                        CodeUnitNum                   += UTF8BOMSizeInCodeUnits;
                    }
                    if (String[CodePoint] <= 0x7F) {
                        EncodedString[CodeUnitNum]     = String[CodePoint];
                        CodeUnitNum                   += 1;
                    } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                        EncodedString[CodeUnitNum]     = (0xC0 + ((String[CodePoint] & 0x7C0) >> 6));
                        EncodedString[CodeUnitNum + 1] = (0x80 +  (String[CodePoint] & 0x03F));
                        CodeUnitNum                   += 2;
                    }  else if (String[CodePoint] >= 0x800 && (String[CodePoint] < UTF16HighSurrogateStart || String[CodePoint] > UTF16LowSurrogateEnd)) {
                        EncodedString[CodeUnitNum]     = (0xE0 + ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 1] = (0x80 + ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 2] = (0x80 +  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 3;
                    } else if (String[CodePoint] > UTF16MaxCodePoint && String[CodePoint] <= UnicodeMaxCodePoint) {
                        EncodedString[CodeUnitNum]     = (0xF0 + ((String[CodePoint] & 0x1C0000) >> 18));
                        EncodedString[CodeUnitNum + 1] = (0x80 + ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 2] = (0x80 + ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 3] = (0x80 +  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 4;
                    }
                    if ((String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd) || String[CodePoint] > UnicodeMaxCodePoint) {
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
    
    UTF16 *UTF16_Encode(UTF32 *String) {
        UTF16   *EncodedString                   = NULL;
        if (String != NULL) {
            uint64_t CodePoint                   = 0ULL;
            UTF32    ByteOrder                   = 0;
            uint64_t NumCodeUnits                = UTF32_GetStringSizeInUTF16CodeUnits(String) + NULLTerminatorSize;
            if (String[0] == UTF32LE || String[0] == UTF32BE) {
                ByteOrder                        = String[0];
            }
            EncodedString                        = calloc(NumCodeUnits, sizeof(UTF16));
            if (EncodedString != NULL) {
                UTF32 CurrentCodePoint           = 0;
                do {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                    if (ByteOrder == UTF32BE) {
                        CurrentCodePoint         = SwapEndian32(String[CodePoint]);
                    } else {
                        CurrentCodePoint         = String[CodePoint];
                    }
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                    if (ByteOrder == UTF32LE) {
                        CurrentCodePoint         = SwapEndian32(String[CodePoint]);
                    } else {
                        CurrentCodePoint         = String[CodePoint];
                    }
#endif
                    if (String[CodePoint] < UTF16HighSurrogateStart || (String[CodePoint] > UTF16LowSurrogateEnd && String[CodePoint] < UTF16MaxCodePoint)) {
                        EncodedString[CodePoint] = String[CodePoint];
                        CodePoint               += 1;
                    } else {
                        EncodedString[CodePoint] = ((String[CodePoint] - UTF16SurrogatePairStart) / UTF16SurrogatePairModDividend) + UTF16HighSurrogateStart;
                        CodePoint               += 1;
                        EncodedString[CodePoint] = ((String[CodePoint] - UTF16SurrogatePairStart) % UTF16SurrogatePairModDividend) + UTF16LowSurrogateStart;
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
    
    /* Medium Functions */
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
            for (uint64_t SubCodePoint = 0ULL; SubCodePoint < SubStringSize; SubCodePoint++) {
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
            ExtractedSubString = UTF8_Encode(Extracted32);
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
            ExtractedSubString = UTF16_Encode(Extracted32);
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
            Replaced8            = UTF8_Encode(Replaced32);
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
            Replaced16           = UTF16_Encode(Replaced32);
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
    
    /*
     Ok, we need to change the API for ReplaceSubString.
     If a SubString to be replaced, is smaller than what to replace it with, we need to go ahead and simply leave the string as it.
     */
    
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
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("SubString2Remove Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("SubString2Remove Pointer is NULL"));
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
                Log(Log_ERROR, __func__, U8("Couldn't allocate the instances"));
            }
            
            if (Instance2Remove >= 1) {
                EditedStringSize = StringSize - SubStringSize;
            } else if (Instance2Remove == 0xFFFFFFFFFFFFFFFF) {
                EditedStringSize = StringSize - (NumInstances * SubStringSize);
            }
            
            EditedString         = calloc(EditedStringSize, sizeof(UTF32));
            if (EditedString != NULL) {
                for (uint64_t EditedCodePoint = 0ULL; EditedCodePoint < EditedStringSize; EditedCodePoint++) {
                    for (uint64_t Instance = 0ULL; Instance < NumInstances; Instance++) {
                        if (EditedCodePoint < Instances[Instance] || EditedCodePoint > Instances[Instance] + SubStringSize) {
                            EditedString[EditedCodePoint] = String[EditedCodePoint - (Instances[Instance] + SubStringSize)];
                        }
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate the edited string"));
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("SubString2Remove Pointer is NULL"));
        } else if (Instance2Remove == 0) {
            Log(Log_ERROR, __func__, U8("Removing 0 instances of the SubString does not make sense"));
        }
        return EditedString;
    }
    
    bool UTF8_Compare(UTF8 *String1, UTF8 *String2) {
        bool StringsMatch          = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32    = UTF8_Decode(String1);
            UTF32 *String2UTF32    = UTF8_Decode(String2);
            StringsMatch           = UTF32_Compare(String1UTF32, String2UTF32);
            free(String1UTF32);
            free(String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF16_Compare(UTF16 *String1, UTF16 *String2) {
        bool StringsMatch          = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32    = UTF16_Decode(String1);
            UTF32 *String2UTF32    = UTF16_Decode(String2);
            StringsMatch           = UTF32_Compare(String1UTF32, String2UTF32);
            free(String1UTF32);
            free(String2UTF32);
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
            uint64_t String1Size     = UTF32_GetStringSizeInCodePoints(String1);
            uint64_t String2Size     = UTF32_GetStringSizeInCodePoints(String2);
            if (String1Size != String2Size) {
                StringsMatch         = No;
            } else {
                do {
                    if (String1[CodePoint] != String2[CodePoint]) {
                        StringsMatch = No;
                    }
                } while (String1[CodePoint] != NULLTerminator && String2[CodePoint] != NULLTerminator);
            }
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    UTF8 *UTF8_Clone(UTF8 *String) {
        UTF8 *Copy = NULL;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            UTF32 *Clone32  = UTF32_Clone(String32);
            free(String32);
            Copy            = UTF8_Encode(Clone32);
            free(Clone32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF16 *UTF16_Clone(UTF16 *String) {
        UTF16 *Copy = NULL;
        if (String != NULL) {
            UTF32 *String32 = UTF16_Decode(String);
            UTF32 *Clone32  = UTF32_Clone(String32);
            free(String32);
            Copy            = UTF16_Encode(Clone32);
            free(Clone32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF32 *UTF32_Clone(UTF32 *String) {
        UTF32 *Copy = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodeUnits = UTF32_GetStringSizeInCodePoints(String);
            Copy   = calloc(StringSizeInCodeUnits, sizeof(UTF32));
            if (Copy != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
                    Copy[CodeUnit] = String[CodeUnit];
                }
            } else {
                Log(Log_ERROR, __func__, U8("Copy Pointer is NULL"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF8 *UTF8_Insert(UTF8 *String, UTF8 *String2Insert, uint64_t Offset) {
        UTF8 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            uint64_t StringSize = UTF8_GetStringSizeInCodeUnits(String);
            uint64_t InsertSize = UTF8_GetStringSizeInCodeUnits(String2Insert);
            if (Offset == 0xFFFFFFFFFFFFFFFF) {
                Offset = StringSize;
            }
            if (String2Insert[0] == 0xEF && String2Insert[1] == 0xBB && String2Insert[2] == 0xBF) {
                InsertSize -= 3;
            }
            if (Offset <= StringSize) {
                Inserted = calloc(StringSize + InsertSize, sizeof(UTF8));
                if (Inserted != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize + InsertSize; CodePoint++) {
                        if (CodePoint < Offset || CodePoint > Offset + InsertSize) {
                            Inserted[CodePoint] = String[CodePoint];
                        } else {
                            Inserted[CodePoint] = String2Insert[CodePoint - StringSize];
                        }
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate InsertedString"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Offset %llu is greater than the string's size %llu"), Offset, StringSize);
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF16 *UTF16_Insert(UTF16 *String, UTF16 *String2Insert, uint64_t Offset) {
        UTF16 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            uint64_t StringSize = UTF16_GetStringSizeInCodeUnits(String);
            uint64_t InsertSize = UTF16_GetStringSizeInCodeUnits(String2Insert);
            if (Offset == 0xFFFFFFFFFFFFFFFF) {
                Offset = StringSize;
            }
            if (String2Insert[0] == 0xEF && String2Insert[1] == 0xBB && String2Insert[2] == 0xBF) {
                InsertSize -= 3;
            }
            if (Offset <= StringSize) {
                Inserted = calloc(StringSize + InsertSize, sizeof(UTF16));
                if (Inserted != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize + InsertSize; CodePoint++) {
                        if (CodePoint < Offset || CodePoint > Offset + InsertSize) {
                            Inserted[CodePoint] = String[CodePoint];
                        } else {
                            Inserted[CodePoint] = String2Insert[CodePoint - StringSize];
                        }
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate InsertedString"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Offset %llu is greater than the string's size %llu"), Offset, StringSize);
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF32 *UTF32_Insert(UTF32 *String, UTF32 *String2Insert, uint64_t Offset) {
        UTF32 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            uint64_t InsertSize = UTF32_GetStringSizeInCodePoints(String2Insert);
            if (Offset == 0xFFFFFFFFFFFFFFFF) {
                Offset = StringSize;
            }
            if (String2Insert[0] == 0xEF && String2Insert[1] == 0xBB && String2Insert[2] == 0xBF) {
                InsertSize -= 3;
            }
            if (Offset <= StringSize) {
                Inserted = calloc(StringSize + InsertSize, sizeof(UTF32));
                if (Inserted != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize + InsertSize; CodePoint++) {
                        if (CodePoint < Offset || CodePoint > Offset + InsertSize) {
                            Inserted[CodePoint] = String[CodePoint];
                        } else {
                            Inserted[CodePoint] = String2Insert[CodePoint - StringSize];
                        }
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate InsertedString"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Offset %llu is greater than the string's size %llu"), Offset, StringSize);
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Inserted;
    }
    /* Medium Functions */
    
    /* Fancy functions */
    UTF8 *UTF8_CaseFoldString(UTF8 *String) {
        UTF8 *CaseFolded      = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *CaseFold32 = UTF32_CaseFoldString(String32);
            free(String32);
            CaseFolded        = UTF8_Encode(CaseFold32);
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
            CaseFolded        = UTF16_Encode(CaseFold32);
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
                for (uint64_t Index = 0ULL; Index < CaseFoldTableSize; Index++) {
                    if (String[CodePoint] == CaseFoldCodePoints[Index]) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                        CaseFoldedString = UTF32_ReplaceSubString(String, CaseFoldStrings[Index], CodePoint, 1);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
                    }
                }
                CodePoint += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
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
                    for (uint64_t Index = 0ULL; Index < KompatibleNormalizationTableSize; Index++) {
                        if (String[CodePoint] == KompatibleNormalizationCodePoints[Index]) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                            ComposedString = UTF32_ReplaceSubString(String, KompatibleNormalizationStrings[Index], CodePoint, 1);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
                        }
                    }
                } else {
                    for (uint64_t DecomposeCodePoint = 0ULL; DecomposeCodePoint < CanonicalNormalizationTableSize; DecomposeCodePoint++) {
                        if (String[CodePoint] == CanonicalNormalizationCodePoints[DecomposeCodePoint]) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                            ComposedString = UTF32_ReplaceSubString(String, CanonicalNormalizationStrings[DecomposeCodePoint], CodePoint, 1);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
                        }
                    }
                }
                CodePoint += 1;
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
                    for (uint64_t Index = 0ULL; Index < KompatibleNormalizationTableSize; Index++) {
                        if (String[CodePoint] == KompatibleNormalizationCodePoints[Index]) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                            Decomposed = UTF32_ReplaceSubString(String, KompatibleNormalizationStrings[Index], CodePoint, 1);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
                        }
                    }
                } else {
                    for (uint64_t Index = 0ULL; Index < CanonicalNormalizationTableSize; Index++) {
                        if (String[CodePoint] == CanonicalNormalizationCodePoints[Index]) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                            Decomposed = UTF32_ReplaceSubString(String, CanonicalNormalizationStrings[Index], CodePoint, 1);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
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
            NormalizedString8         = UTF8_Encode(NormalizedString32);
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
            NormalizedString16        = UTF16_Encode(NormalizedString32);
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
    /* Unicode Functions */
    
    int64_t UTF8_String2Integer(StringIOBases Base, UTF8 *String) { // Replaces atoi, atol, strtol, strtoul,
        UTF32 *String32 = UTF8_Decode(String);
        int64_t Value   = UTF32_String2Integer(Base, String32);
        free(String32);
        return Value;
    }
    
    int64_t UTF16_String2Integer(StringIOBases Base, UTF16 *String) {
        UTF32 *String32 = UTF16_Decode(String);
        int64_t Value   = UTF32_String2Integer(Base, String32);
        free(String32);
        return Value;
    }
    
    int64_t UTF32_String2Integer(StringIOBases Base, UTF32 *String) {
        uint64_t CodePoint = 0ULL;
        int8_t   Sign      = 1;
        int64_t  Value     = 0LL;
        if (String != NULL) {
            if (Base == IntegerBase2) {
                do {
                    if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x31) {
                        Value <<= 1;
                        Value   = String[CodePoint] - 0x30;
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else if (Base == IntegerBase8) {
                do {
                    if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x37) {
                        Value  *= 8;
                        Value   = String[CodePoint] - 0x30;
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else if (Base == IntegerBase10) {
                do {
                    if (CodePoint == 1 && String[CodePoint] == U32('-')) {
                        Sign    = -1;
                    }
                    if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39) {
                        Value  *= 10;
                        Value   = String[CodePoint] - 0x30;
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else if (Base == IntegerBase16U) {
                do {
                    if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39) {
                        Value  *= 16;
                        Value   = String[CodePoint] - 0x30;
                    } else if (String[CodePoint] >= 0x41 && String[CodePoint] <= 0x46) {
                        Value  *= 16;
                        Value   = String[CodePoint] - 0x37;
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else if (Base == IntegerBase16L) {
                do {
                    if (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39) {
                        Value <<= 16;
                        Value   = String[CodePoint] - 0x30;
                    } else if (String[CodePoint] >= 0x61 && String[CodePoint] <= 0x66) {
                        Value <<= 16;
                        Value   = String[CodePoint] - 0x51;
                    }
                } while (String[CodePoint] != NULLTerminator);
            }
            Value *= Sign;
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
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
        int64_t  Sign            = 0LL;
        int64_t  Num             = Integer2Convert;
        uint8_t  NumDigits       = 0;
        while (Num > 0) {
            Num                 /= Base;
            NumDigits           += 1;
        }
        if (Integer2Convert < 0 && Base == IntegerBase10) {
            Sign                 = -1;
            NumDigits           +=  1;
        }
        UTF32 *NumberString      = calloc(NumDigits + NULLTerminatorSize, sizeof(UTF32));
        UTF32  UpperNumerals[16] = {U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'), U32('E'), U32('F')};
        UTF32  LowerNumerals[16] = {U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'), U32('e'), U32('f')};
        if (NumberString != NULL) {
            for (uint64_t CodePoint = NumDigits - 1; CodePoint > 0; CodePoint--) {
                int64_t CurrentDigit    = (Base == IntegerBase10 ? Absolute(Integer2Convert %= Base) : (Integer2Convert %= Base));
                NumberString[CodePoint] = (Base == IntegerBase16U ? UpperNumerals[CurrentDigit] : LowerNumerals[CurrentDigit]);
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
    
    double UTF32_String2Decimal(UTF32 *String) { // Replaces strtod, strtof, strold, atof, and atof_l
        uint64_t CodePoint  = 0ULL;
        bool     IsNegative = No;
        if (String != NULL) {
            // Extract the sign bit
            // Extract the Significand
            // Extract the Base
            // Extract the Exponent
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
        UTF8  *String8  = UTF8_Encode(String32);
        free(String32);
        return String8;
    }
    
    UTF16 *UTF16_Decimal2String(const StringIOBases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Base, Decimal);
        UTF16 *String16 = UTF16_Encode(String32);
        free(String32);
        return String16;
    }
    
    UTF32 *UTF32_Decimal2String(StringIOBases Base, double Number) {
        UTF32   *OutputString     = NULL;
        int8_t   Sign             = ExtractSignFromDecimal(Number);
        int16_t  Exponent         = ExtractExponentFromDecimal(Number);
        int16_t  Exponent2        = Absolute(Exponent);
        uint64_t Mantissa         = ExtractMantissaFromDecimal(Number);
        uint64_t Mantissa2        = Absolute(Mantissa);
        uint16_t ExponentSize     = 0ULL;
        uint64_t MantissaSize     = 0ULL;
        uint64_t StringSize       = 0ULL;
        bool     IsDenormal       = No;
        bool     IsNotANumber     = No;
        bool     IsInfinite       = No;
        
        if (Exponent == 0x7FF && Mantissa > 0) {
            IsNotANumber          = Yes;
        } else if (Exponent == 0x7FF && Mantissa == 0) {
            IsInfinite            = Yes;
        } else if (Exponent == 0 && Mantissa >= 0) {
            IsDenormal            = Yes;
        }
        
        // The Number to write is 1024.0
        // The Exponent's size is 4
        // The Mantissa's size is 1?
        
        /*
         Denorm = Exponent = 0,   Mantissa == Anything
         NaN    = Exponent = Max, Mantissa >  0
         Inf    = Exponent = Max, Mantissa == 0
         */
        
        if (Base == FloatShortestU) { // Get the size of the string for both Decimal and Scientific representations, and choose the shortest.
            
            /*
             Choose between Decimal and Scientific at will.
             
             Get the size of the string in both representations and go with the smallest
             So, we need a way to figure out the size of each representation without actually doing it.
             */
        } else if (Base == FloatShortestL) { // Get the size of the string for both Decimal and Scientific representations, and choose the shortest.
            /*
             Choose between Decimal and Scientific at will.
             
             Get the size of the string in both representations and go with the smallest
             So, we need a way to figure out the size of each representation without actually doing it.
             */
            
        }
        
        
        
        if (Sign == -1) {
            StringSize           += 1;
        }
        do {
            Exponent2            /= 10;
            ExponentSize         += 1;
        } while (ExponentSize > 0);
        do {
            Mantissa2            /= 10;
            MantissaSize         += 1;
        } while (MantissaSize > 0);
        
        if (Base == FloatDecimalU) { // Write the number as SXXX.MM // Exponent * Sign . Fraction, -1.1 = -1.100000
            StringSize += 1; // Add one for the decimal seperator
        } else if (Base == FloatDecimalL) { // Exponent * Sign . Fraction, -1.1 = -1.100000
            StringSize += 1; // Add one for the decimal seperator
        } else if (Base == FloatHexU) { // Sign 0(x|X) . Fraction EP Exponent, -1.1 = -0X1.199999999999AP+0
            StringSize += (5 + ExponentSize); // Add seven for the 0X, decimal seperator, AP, +, and 0.
        } else if (Base == FloatHexL) { // Sign 0(x|X) . Fraction EP Exponent, -1.1 = -0x1.199999999999ap+0
            StringSize += (5 + ExponentSize); // Add seven for the 0x, decimal seperator, ap, +, and 0.
        } else if (Base == FloatScientificU) { // Write the number as XXX.YYE(+|-)Z // -1.1 = -1.100000E+00
            StringSize += (2 + ExponentSize); // Add five for the decimal seperator, E, Sign, X.
        } else if (Base == FloatScientificL) { // Write the number as XXX.YYe(+|-)Z // -1.1 = -1.100000e+00
            StringSize += (2 + ExponentSize); // Add five for the decimal seperator, e, +, and X.
        }
        
        OutputString              = calloc(StringSize + ExponentSize + MantissaSize, sizeof(UTF32));
        if (OutputString != NULL) {
            // Now we go ahead and create the string
            if (Sign == -1) {
                OutputString[1]   = U32('-');
            }
            // Now we start popping in the other variables, first is the Exponent.
            while (Exponent > 0) { // TODO: This assumes there's only 1 codepoint necessary to express the exponent
                for (uint64_t ExponentCodePoint = 0ULL; ExponentCodePoint < ExponentSize; ExponentCodePoint++) {
                    OutputString[ExponentCodePoint + StringSize]                 = Exponent /= 10;
                }
            }
            OutputString[StringSize + ExponentSize + UnicodeBOMSizeInCodePoints] = U32('.');
            // Now let's start popping in the Mantissa
            while (Mantissa > 0) { // TODO: This assumes there's only 1 codepoint necessary to express the exponent
                for (uint64_t MantissaCodePoint = 0ULL; MantissaCodePoint < ExponentSize; MantissaCodePoint++) {
                    OutputString[StringSize + ExponentSize + MantissaCodePoint]  = Mantissa /= 10;
                }
            }
            if (Base == FloatHexU) {
                OutputString[StringSize + ExponentSize + MantissaSize + 1] = U32('A');
                OutputString[StringSize + ExponentSize + MantissaSize + 2] = U32('P');
            } else if (Base == FloatHexL) {
                OutputString[StringSize + ExponentSize + MantissaSize + 1] = U32('a');
                OutputString[StringSize + ExponentSize + MantissaSize + 2] = U32('p');
            } else if (Base == FloatScientificU) {
                OutputString[StringSize + ExponentSize + MantissaSize + 1] = U32('E');
                // Write the sign, if the number is positive, write a +, otherwise write a -
                if (Sign == -1) {
                    OutputString[StringSize + ExponentSize + MantissaSize + 2] = U32('-');
                } else {
                    OutputString[StringSize + ExponentSize + MantissaSize + 2] = U32('+');
                }
                // Write the Exponent
                uint64_t ExponentSizeInDigits = Logarithm(2, Exponent);
                for (uint64_t ExponentDigit = 0ULL; ExponentDigit < ExponentSizeInDigits; ExponentDigit++) {
                    OutputString[StringSize + ExponentSize + MantissaSize + 2 + ExponentDigit] = U32("Exponent"); // FIXME: "Exponent" is NOT right
                }
            } else if (Base == FloatScientificL) {
                OutputString[StringSize + ExponentSize + MantissaSize + 2] = U32('e');
                if (Sign == -1) {
                    OutputString[StringSize + ExponentSize + MantissaSize + 2] = U32('-');
                } else {
                    OutputString[StringSize + ExponentSize + MantissaSize + 2] = U32('+');
                }
                // Write the Exponent
                uint64_t ExponentSizeInDigits = Logarithm(2, Exponent);
                for (uint64_t ExponentDigit = 0ULL; ExponentDigit < ExponentSizeInDigits; ExponentDigit++) {
                    OutputString[StringSize + ExponentSize + MantissaSize + 2 + ExponentDigit] = U32("Exponent"); // FIXME: "Exponent" is NOT right
                }
            }
        }
        return OutputString;
    }
    
    /* StringArray Functions */
    UTF8 **UTF8_StringArray_Init(uint64_t NumStrings) {
        UTF8 **StringArray = NULL;
        if (NumStrings > 0) {
            StringArray    = calloc(NumStrings + 1, sizeof(UTF8*));
        } else {
            Log(Log_ERROR, __func__, U8("NumStrings %llu is invalid"), NumStrings);
        }
        return StringArray;
    }
    
    UTF16 **UTF16_StringArray_Init(uint64_t NumStrings) {
        UTF16 **StringArray = NULL;
        if (NumStrings > 0) {
            StringArray    = calloc(NumStrings + 1, sizeof(UTF16*));
        } else {
            Log(Log_ERROR, __func__, U8("NumStrings %llu is invalid"), NumStrings);
        }
        return StringArray;
    }
    
    UTF32 **UTF32_StringArray_Init(uint64_t NumStrings) {
        UTF32 **StringArray = NULL;
        if (NumStrings > 0) {
            StringArray    = calloc(NumStrings + 1, sizeof(UTF32*));
        } else {
            Log(Log_ERROR, __func__, U8("NumStrings %llu is invalid"), NumStrings);
        }
        return StringArray;
    }
    
    void UTF8_StringArray_Attach(UTF8 **StringArray, UTF8 *String2Attach, uint64_t Index) {
        if (StringArray != NULL && String2Attach != NULL) { // We can't actually see if there's enough room to attach it because it's all null
            StringArray[Index] = String2Attach;
        } else if (StringArray == NULL) {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Log_ERROR, __func__, U8("String2Attach Pointer is NULL"));
        }
    }
    
    void UTF16_StringArray_Attach(UTF16 **StringArray, UTF16 *String2Attach, uint64_t Index) {
        if (StringArray != NULL && String2Attach != NULL) { // We can't actually see if there's enough room to attach it because it's all null
            StringArray[Index] = String2Attach;
        } else if (StringArray == NULL) {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Log_ERROR, __func__, U8("String2Attach Pointer is NULL"));
        }
    }
    
    void UTF32_StringArray_Attach(UTF32 **StringArray, UTF32 *String2Attach, uint64_t Index) {
        if (StringArray != NULL && String2Attach != NULL) { // We can't actually see if there's enough room to attach it because it's all null
            StringArray[Index] = String2Attach;
        } else if (StringArray == NULL) {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Log_ERROR, __func__, U8("String2Attach Pointer is NULL"));
        }
    }
    
    uint64_t UTF8_StringArray_GetNumStrings(UTF8 **StringArray) {
        uint64_t NumStrings = 0ULL;
        if (StringArray != NULL) {
            do {
                NumStrings += 1;
            } while (StringArray[NumStrings][0] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t UTF16_StringArray_GetNumStrings(UTF16 **StringArray) {
        uint64_t NumStrings = 0ULL;
        if (StringArray != NULL) {
            do {
                NumStrings += 1;
            } while (StringArray[NumStrings][0] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t UTF32_StringArray_GetNumStrings(UTF32 **StringArray) {
        uint64_t NumStrings = 0ULL;
        if (StringArray != NULL) {
            do {
                NumStrings += 1;
            } while (StringArray[NumStrings][0] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        }
        return NumStrings;
    }
    
    UTF8 *UTF8_StringArray_GetString(UTF8 **StringArray, uint64_t Index) {
        UTF8 *String = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings = UTF8_StringArray_GetNumStrings(StringArray);
            if (Index <= NumStrings) {
                // get the string
                String = StringArray[Index];
            } else {
                Log(Log_ERROR, __func__, U8("Index %llu is greater than there are strings %llu"), Index, NumStrings);
            }
        } else {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        }
        return String;
    }
    
    UTF16 *UTF16_StringArray_GetString(UTF16 **StringArray, uint64_t Index) {
        UTF16 *String = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings = UTF16_StringArray_GetNumStrings(StringArray);
            if (Index <= NumStrings) {
                // get the string
                String = StringArray[Index];
            } else {
                Log(Log_ERROR, __func__, U8("Index %llu is greater than there are strings %llu"), Index, NumStrings);
            }
        } else {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        }
        return String;
    }
    
    UTF32 *UTF32_StringArray_GetString(UTF32 **StringArray, uint64_t Index) {
        UTF32 *String = NULL;
        if (StringArray != NULL) {
            uint64_t NumStrings = UTF32_StringArray_GetNumStrings(StringArray);
            if (Index <= NumStrings) {
                // get the string
                String = StringArray[Index];
            } else {
                Log(Log_ERROR, __func__, U8("Index %llu is greater than there are strings %llu"), Index, NumStrings);
            }
        } else {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        }
        return String;
    }
    
    void UTF8_StringArray_Deinit(UTF8 **StringArray) {
        if (StringArray != NULL) {
            uint64_t NumStrings = UTF8_StringArray_GetNumStrings(StringArray);
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                free(StringArray[String]);
            }
            free(StringArray);
        } else {
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("StringArray Pointer is NULL"));
        }
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
    
    UTF8 **UTF8_SplitString(UTF8 *String, UTF8 **Delimiters) {
        UTF8 **SplitString                      = NULL;
        if (String != NULL && Delimiters != NULL) {
            uint64_t   NumDelimiters            = UTF8_StringArray_GetNumStrings(Delimiters);
            UTF32     *String32                 = UTF8_Decode(String);
            UTF32    **Delimiters32             = calloc(NumDelimiters + 1, sizeof(UTF32*));
            if (Delimiters32 != NULL) {
                for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                    Delimiters32[Delimiter]     = UTF8_Decode(Delimiters[Delimiter]);
                }
                UTF32 **SplitString32           = UTF32_SplitString(String32, Delimiters32);
                free(String32);
                free(Delimiters32);
                uint64_t NumStringParts         = UTF32_StringArray_GetNumStrings(SplitString32);
                SplitString                     = calloc(NumStringParts + 1, sizeof(UTF8*));
                if (SplitString != NULL) {
                    for (uint64_t StringPart = 0ULL; StringPart < NumStringParts; StringPart++) {
                        SplitString[StringPart] = UTF8_Encode(SplitString32[StringPart]);
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate space for the encoded string pieces"));
                }
                free(SplitString32);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate space for the decoded delimiters"));
            }
            
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return SplitString;
    }
    
    UTF16 **UTF16_SplitString(UTF16 *String, UTF16 **Delimiters) {
        UTF16 **SplitString                     = NULL;
        if (String != NULL && Delimiters != NULL) {
            uint64_t   NumDelimiters            = UTF16_StringArray_GetNumStrings(Delimiters);
            UTF32     *String32                 = UTF16_Decode(String);
            UTF32    **Delimiters32             = calloc(NumDelimiters + 1, sizeof(UTF32*));
            if (Delimiters32 != NULL) {
                for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                    Delimiters32[Delimiter]     = UTF16_Decode(Delimiters[Delimiter]);
                }
                UTF32 **SplitString32           = UTF32_SplitString(String32, Delimiters32);
                free(String32);
                free(Delimiters32);
                uint64_t NumStringParts         = UTF32_StringArray_GetNumStrings(SplitString32);
                SplitString                     = calloc(NumStringParts + 1, sizeof(UTF16*));
                if (SplitString != NULL) {
                    for (uint64_t StringPart = 0ULL; StringPart < NumStringParts; StringPart++) {
                        SplitString[StringPart] = UTF16_Encode(SplitString32[StringPart]);
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate space for the encoded string pieces"));
                }
                free(SplitString32);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate space for the decoded delimiters"));
            }
            
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return SplitString;
    }
    
    UTF32 **UTF32_SplitString(UTF32 *String, UTF32 **Delimiters) {
        // NEW
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
            DelimitersSize         = calloc(NumDelimiters, sizeof(uint64_t));
            if (DelimitersSize != NULL) {
                for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                    DelimitersSize[Delimiter] = UTF32_GetStringSizeInCodePoints(Delimiters[Delimiter]);
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate space for the delimiter sizes"));
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
            
            SplitSizes   = calloc(NumSplitStrings, sizeof(uint64_t));
            SplitOffsets = calloc(NumSplitStrings, sizeof(uint64_t));
            
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
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return SplitStrings;
    }
    /* StringArray Functions */
    
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
        uint64_t                  StringOffset;
        uint64_t                  StringSize;
        uint64_t                  MinWidth;
        uint64_t                  Precision;
        UTF32                    *String;
        FormatSpecifierTypes      Type;
        FormatSpecifierLengths    Length;
        FormatSpecifierFlags      Flag;
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
    
    static FormatString *UTF32_ParseFormatSpecifiers(UTF32 *Format, StringTypes StringType) {
        FormatString *Details      = NULL;
        uint64_t NumSpecifiers     = 0ULL;
        uint64_t FormatStringSize  = 0ULL;
        if (Format != NULL && StringType != UnknownUnicodeFormat) {
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
                        Details->Specifiers[Details->NumSpecifiers - 1].StringOffset = CodePoint + 1;
                        for (uint64_t EndCodePoint = 1ULL; EndCodePoint + CodePoint < FormatStringSize; EndCodePoint++) {
                            UTF32 CurrentCodePoint = Format[CodePoint + EndCodePoint];
                            if (CurrentCodePoint == U'd' || CurrentCodePoint == U'i' || CurrentCodePoint == U'o' || CurrentCodePoint == U'u' ||
                                CurrentCodePoint == U'x' || CurrentCodePoint == U'X' || CurrentCodePoint == U'f' || CurrentCodePoint == U'F' ||
                                CurrentCodePoint == U'e' || CurrentCodePoint == U'E' || CurrentCodePoint == U'g' || CurrentCodePoint == U'G' ||
                                CurrentCodePoint == U'a' || CurrentCodePoint == U'A' || CurrentCodePoint == U'c' || CurrentCodePoint == U's' ||
                                CurrentCodePoint == U'%' || CurrentCodePoint == U'n' || CurrentCodePoint == U'r' || CurrentCodePoint == U't' ||
                                CurrentCodePoint == U'v') {
                                
                                Details->Specifiers[Details->NumSpecifiers - 1].StringSize = Details->Specifiers[Details->NumSpecifiers - 1].StringOffset - EndCodePoint;
                            }
                        }
                    }
                }
                for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                    uint64_t SpecifierStart = Details->Specifiers[Specifier].StringOffset + 1;
                    uint64_t SpecifierEnd   = Details->Specifiers[Specifier].StringOffset + Details->Specifiers[Specifier].StringSize;
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
                            Details->Specifiers[Specifier].MinWidth = UTF32_String2Integer(IntegerBase10, MinWidthString);
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
                                Details->Specifiers[Specifier].Precision = UTF32_String2Integer(IntegerBase10, PrecisionString);
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
    
    static void FormatString_Deinit(FormatString *String2Deinit) {
        if (String2Deinit != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < String2Deinit->NumSpecifiers; Specifier++) {
                free(String2Deinit->Specifiers[Specifier].String);
            }
            free(String2Deinit->Specifiers);
            free(String2Deinit);
        }
    }
    
    static UTF32 *FormatString_UTF32(UTF32 *Format, FormatString *Details, va_list VariadicArguments) {
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
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatDecimalL, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase10DecimalU) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatDecimalU, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase16DecimalL) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatHexL, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase16DecimalU) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatHexU, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase2Integer) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(IntegerBase2, va_arg(VariadicArguments, double));
                } else if (Type == FormatScientificDecimalU) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatScientificU, va_arg(VariadicArguments, double));
                } else if (Type == FormatScientificDecimalL) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatScientificL, va_arg(VariadicArguments, double));
                } else if (Type == FormatShortestDecimalU) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatShortestU, va_arg(VariadicArguments, double));
                } else if (Type == FormatShortestDecimalL) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatShortestL, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase16DecimalU) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatHexU, va_arg(VariadicArguments, double));
                } else if (Type == FormatBase16DecimalL) {
                    Details->Specifiers[Specifier].String = UTF32_Decimal2String(FloatHexL, va_arg(VariadicArguments, double));
                }
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
            UTF32 *FormattedString        = FormatString_UTF32(Format32, Details, VariadicArguments);
            va_end(VariadicArguments);
            FormatString_Deinit(Details);
            Format8                       = UTF8_Encode(FormattedString);
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
            UTF32 *FormattedString        = FormatString_UTF32(Format32, Details, VariadicArguments);
            va_end(VariadicArguments);
            FormatString_Deinit(Details);
            Format16                      = UTF16_Encode(FormattedString);
            free(Format32);
            free(FormattedString);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Format16;
    }
    
    UTF32 *UTF32_FormatString(UTF32 *Format, ...) {
        UTF32 *Format32 = NULL;
        if (Format != NULL) {
            FormatString *Details         = UTF32_ParseFormatSpecifiers(Format, UTF32Format);
            va_list VariadicArguments;
            Format32                      = FormatString_UTF32(Format, Details, VariadicArguments);
            va_end(VariadicArguments);
            FormatString_Deinit(Details);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Format32;
    }
    
    void UTF8_WriteString2File(UTF8 *String, FILE *OutputFile) {
        if (String != NULL && OutputFile != NULL) {
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            fputs(String, OutputFile);
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            UTF32 *String32 = UTF8_Decode(String);
            UTF16 *String16 = UTF16_Encode(String32);
            fputws(String16, OutputFile);
            free(String32);
            free(String16);
#endif
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_ERROR, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_WriteString2File(UTF16 *String, FILE *OutputFile) {
        if (String != NULL && OutputFile != NULL) {
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            UTF32 *String32 = UTF16_Decode(String);
            UTF8  *String8  = UTF8_Encode(String32);
            free(String32);
            fputs(String8, OutputFile);
            free(String8);
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            fputws(String, OutputFile);
#endif
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_ERROR, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
