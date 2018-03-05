#include <stdarg.h>                    /* Included for va_start, va_end */
#include <stdint.h>                    /* Included for u/intX_t */
#include <stdlib.h>                    /* Included for calloc, and free */
#include <float.h>                     /* Included for */
#include <limits.h>                    /* Included for */

#include "../include/Macros.h"         /* Included for NewLineWithNULLSize */
#include "../include/Math.h"           /* Included for Integer functions */
#include "../include/StringIO.h"       /* Included for UTF8 */
#include "../include/StringIOTables.h" /* Included for the tables */
#include "../include/Log.h"

#define UnicodeNULLTerminatorSize       1
#define UTF1632BOMSize                  1
#define UTF8BOMSize                     3

/* 0x00 is valid in UTF-16, NULL is 0x0000, be sure to catch this mistake. */
/* U+D800 - U+DFFF are invalid UTF32 code points. when converting to/from UTF32 make sure that that is not a code point becuse they're reserved as UTF-16 surrogate pairs */
/* UTF-16 High/Low Surrogate mask = 0xFC00 */

/* StringIO organization structure:
    UTF-8  I/O
    UTF-16 I/O
    UTF-32 I/O
 
    UTF-32 Fancy shit.
 
 */

#ifdef  __cplusplus
extern  "C" {
#endif
    
    /* UTF-8 I/O */
    static uint8_t      UTF8_GetCodePointSize(UTF8 CodeUnit) {
        uint8_t CodePointSize      = 0;
        if (((CodeUnit & 0x80) >> 7) == 0) {
            CodePointSize         += 1;
        } else if (((CodeUnit & 0xE0) >> 5) == 6) {
            CodePointSize         += 2;
        } else if (((CodeUnit & 0xF0) >> 4) == 14) {
            CodePointSize         += 3;
        } else if (((CodeUnit & 0xF8) >> 3) == 30) {
            CodePointSize         += 4;
        }
        return CodePointSize;
    }
    
    uint64_t            UTF8_GetSizeInCodeUnits(UTF8 *String2Count) { // Read a codeunit, get it's size, skip that many bytes, check that it's not 0x0, then repeat
        uint64_t NumCodeUnits                       = 0ULL;
        uint64_t CurrentCodeUnit                    = 0ULL;
        if (String2Count != NULL) {
            do {
                NumCodeUnits                       += UTF8_GetCodePointSize(String2Count[CurrentCodeUnit]);
            } while (String2Count[CurrentCodeUnit] != 0);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodeUnits;
    }
    
    uint64_t            UTF8_GetSizeInCodePoints(UTF8 *String) {
        uint64_t CurrentCodeUnit              = 0ULL;
        uint64_t NumCodePoints                = 0ULL;
        if (String != NULL) {
            do {
                CurrentCodeUnit              += UTF8_GetCodePointSize(String[CurrentCodeUnit]);
                NumCodePoints                += 1;
            } while (String[CurrentCodeUnit] != 0);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    static uint64_t UTF32_GetSizeInCodeUnits4UTF8(UTF32 *String) {
        uint64_t CodePoint          = 0ULL;
        uint64_t UTF8CodeUnits      = 0ULL;
        do {
            if (String[CodePoint] <= 0x7F) {
                UTF8CodeUnits      += 1;
            } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                UTF8CodeUnits      += 2;
            } else if (String[CodePoint] >= 0x800 && String[CodePoint] <= 0xFFFF) {
                UTF8CodeUnits      += 3;
            } else if (String[CodePoint] >= 0x10000 && String[CodePoint] <= 0x1FFFFF) {
                UTF8CodeUnits      += 4;
            }
            CodePoint              += 1;
        } while (String[CodePoint] != 0);
        return UTF8CodeUnits;
    }
    
    static uint64_t     UTF32_GetSizeInCodeUnits4UTF16(UTF32 *String) {
        uint64_t CodePoint          = 0ULL;
        uint64_t UTF16CodeUnits     = 0ULL;
        do {
            if (String[CodePoint] >= 0x10000) {
                UTF16CodeUnits     += 2;
            } else {
                UTF16CodeUnits     += 1;
            }
            CodePoint              += 1;
        } while (String[CodePoint] != 0);
        return UTF16CodeUnits;
    }
    
    UTF32         *UTF8_Decode(UTF8 *String) {
        // We should make sure we don't decode anything in the surrogate block, between UTF16HighSurrogateEnd to UTF16HighSurrogateStart or DFFF to DC00
        uint8_t  CodePointSize      = 0;
        uint64_t CodeUnitNum        = 0ULL;
        uint64_t CodePoint          = 0ULL;
        UTF32   *DecodedString      = NULL;
        if (String != NULL) {
            uint64_t NumCodePoints  = UTF1632BOMSize + UTF8_GetSizeInCodePoints(String) + UnicodeNULLTerminatorSize;
            DecodedString           = calloc(NumCodePoints, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    // Get the number of codepoints contained in the UTF8, then allocate that much memory, then decode.
                    if (CodePoint == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            DecodedString[0] = UTF32LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            DecodedString[0] = UTF32BE;
                        }
                    }
                    switch (CodePointSize) {
                        case 1: // 7 bits
                            DecodedString[CodePoint] =  String[CodeUnitNum - 1];
                            CodeUnitNum             += 1;
                            break;
                        case 2: // 11 bits, U+80 - U+7FF
                            DecodedString[CodePoint] = (String[CodeUnitNum - 1] & 0x1F) << 6;
                            DecodedString[CodePoint] =  String[CodeUnitNum]     & 0x3F;
                            CodeUnitNum             += 2;
                            break;
                        case 3: // 16 bits, U+800 - U+FFFF (except U+D800 - U+DFFF)
                            DecodedString[CodePoint] = (String[CodeUnitNum - 1] & 0x0F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x1F) << 6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x1F);
                            CodeUnitNum             += 3;
                            break;
                        case 4: // 21 bits, U+10000 - U+10FFFF
                            DecodedString[CodePoint] = (String[CodeUnitNum - 1] & 0x07) << 18;
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x3F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x3F) <<  6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 2] & 0x3F);
                            CodeUnitNum             += 4;
                            break;
                    }
                    if (CodePointSize > 1 && DecodedString[CodePoint] <= 0x7F) { // Invalid, overlong sequence detected, replace it with 0xFFFD
                        DecodedString[CodePoint]     = 0xFFFD;
                        Log(Log_ERROR, __func__, U8("CodePoint %d is overlong, replaced with U+FFFD"), CodePoint);
                    } else if (DecodedString[CodePoint] >= UTF16HighSurrogateStart && DecodedString[CodePoint] <= UTF16LowSurrogateEnd && DecodedString[CodePoint] != UTF32LE && DecodedString[CodePoint] != UTF32BE) {
                        DecodedString[CodePoint]     = 0xFFFD;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD"), CodePoint);
                    }
                    if (GlobalByteOrder == LSByteFirst) { // The string byte order does not match the main machines, we need to swap
                        // Swap the endian as we encode
                        DecodedString[CodePoint] = SwapEndian32(String[CodePoint]);
                    }
                } while (String[CodeUnitNum] != 0);
            } else {
                Log(Log_ERROR, __func__, U8("Not enough memory to allocate string"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF8          *UTF8_Encode(UTF32 *String, const bool IncludeBOM) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        uint64_t CodePoint                             = 0ULL;
        uint64_t CodeUnitNum                           = 0ULL;
        UTF8    *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t UTF8CodeUnits                     = UnicodeNULLTerminatorSize + UTF32_GetSizeInCodeUnits4UTF8(String) + (IncludeBOM == true ? UTF8BOMSize : 0);
            
            EncodedString                              = calloc(UTF8CodeUnits, sizeof(UTF8));
            if (EncodedString != NULL) {
                do {
                    if ((String[0] == UTF32LE && GlobalByteOrder == MSByteFirst) || (String[0] == UTF32BE && GlobalByteOrder == LSByteFirst)) { // The string byte order does not match the main machines, we need to swap
                        // Swap the endian as we encode
                        String[CodePoint] = SwapEndian32(String[CodePoint]);
                    }
                    if (CodeUnitNum == 0 && IncludeBOM == Yes) {
                        EncodedString[CodeUnitNum]      = 0xEF;
                        EncodedString[CodeUnitNum += 1] = 0xBB;
                        EncodedString[CodeUnitNum += 1] = 0xBF;
                    }
                    if (String[CodePoint] <= 0x7F) {
                        EncodedString[CodeUnitNum]     = String[CodePoint];
                        CodeUnitNum                   += 1;
                    } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                        EncodedString[CodeUnitNum]     = (0xC0 & ((String[CodePoint] & 0x7C0) >> 6));
                        EncodedString[CodeUnitNum + 1] = (0x80 &  (String[CodePoint] & 0x03F));
                        CodeUnitNum                   += 2;
                    }  else if (String[CodePoint] >= 0x800 && String[CodePoint] <= 0xFFFF && String[CodePoint] <= 0xD7FF && String[CodePoint] >= 0xE000 && String[CodePoint] != UTF32LE && String[CodePoint] != UTF32BE) {
                        EncodedString[CodeUnitNum]     = (0xE0 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 1] = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 2] = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 3;
                    } else if (String[CodePoint] >= 0x10000 && String[CodePoint] <= 0x10FFFF) {
                        EncodedString[CodeUnitNum]     = (0xF0 & ((String[CodePoint] & 0x1C0000) >> 18));
                        EncodedString[CodeUnitNum + 1] = (0x80 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 2] = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 3] = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 4;
                    } else if (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd && String[CodePoint] != UTF32LE && String[CodePoint] != UTF32BE) {
                        String[CodePoint]              = 0xFFFD;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD"));
                    }
                } while (String[CodePoint] != 0);
            } else {
                Log(Log_ERROR, __func__, U8("String Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    
    /* UTF-16 I/O */
    static uint8_t      UTF16_GetCodePointSize(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit >= UTF16HighSurrogateStart && CodeUnit <= UTF16LowSurrogateEnd) { // Surrogate
            CodePointSize     = 2;
        } else {
            CodePointSize     = 1;
        }
        return CodePointSize;
    }
    
    uint64_t            UTF16_GetSizeInCodeUnits(UTF16 *String) {
        uint64_t NumCodeUnits          = 0ULL;
        do {
            NumCodeUnits              += UTF16_GetCodePointSize(String[NumCodeUnits]);
        } while (String[NumCodeUnits] != 0);
        return NumCodeUnits;
    }
    
    uint64_t            UTF16_GetSizeInCodePoints(UTF16 *String) {
        uint64_t NumCodeUnits           = 0ULL;
        uint64_t NumCodePoints          = 0ULL;
        do {
            NumCodeUnits               += UTF16_GetCodePointSize(String[NumCodeUnits]);
            NumCodePoints              += 1;
        } while (String[NumCodePoints] != 0);
        return NumCodePoints;
    }
    
    UTF32         *UTF16_Decode(UTF16 *String) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        /*
         High Surrogate range: UTF16HighSurrogateStart - UTF16HighSurrogateEnd
         Low  Surrogate range: UTF16LowSurrogateStart - UTF16LowSurrogateEnd
         
         No matter the byte order, the High Surrogate is always at a lower address in the UTF16 stream than the Low Surrogate, aka the High Surrogate is always first, followed by a Low Surrogate.
         */
        uint64_t CodePoint          = 0ULL;
        uint64_t CodeUnitNum        = 0ULL;
        UTF32   *DecodedString      = NULL;
        if (String != NULL) {
            uint64_t UTF16CodeUnits = UTF1632BOMSize + UTF16_GetSizeInCodePoints(String) + UnicodeNULLTerminatorSize;
            DecodedString           = calloc(UTF16CodeUnits, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if ((String[0] == UTF16LE && GlobalByteOrder == MSByteFirst) || (String[0] == UTF16BE && GlobalByteOrder == LSByteFirst)) { // The string byte order does not match the main machines, we need to swap
                        // Swap the endian as we encode
                        String[CodePoint] = SwapEndian16(String[CodePoint]);
                    }
                    if (CodePoint == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            DecodedString[0] = UTF16LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            DecodedString[0] = UTF16BE;
                        }
                    }
                    if (String[CodeUnitNum] <= 0xD7FF || (String[CodeUnitNum] >= 0xE000 && String[CodeUnitNum] <= 0xFFFF)) {
                        DecodedString[CodePoint] = String[CodeUnitNum - 1];
                    } else {
                        DecodedString[CodePoint] = (((String[CodeUnitNum - 1] - UTF16HighSurrogateStart) * 0x400) + (String[CodeUnitNum] - UTF16LowSurrogateStart) + 0x10000);
                    }
                } while (String[CodePoint] != 0);
            } else {
                Log(Log_ERROR, __func__, U8("DecodedString Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF16         *UTF16_Encode(UTF32 *String) {
        UTF16   *EncodedString         = NULL;
        if (String != NULL) {
            if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
                GetRuntimeByteBitOrder();
            }
            uint8_t  CodePointSize         = 0;
            uint64_t CodeUnitNum           = 0ULL;
            uint64_t UTF16NumCodeUnits     = UTF32_GetSizeInCodeUnits4UTF16(String) + UTF1632BOMSize + UnicodeNULLTerminatorSize;
            EncodedString                  = calloc(UTF16NumCodeUnits, sizeof(UTF16));
            if (EncodedString != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < UTF16NumCodeUnits; CodeUnit++) {
                    // High = ((String[CodePoint] - 0x10000) / 0x400) + UTF16HighSurrogateStart
                    // Low  = (String[CodePoint]  - 0x10000) % 0x400) + UTF16LowSurrogateStart
                    // High = 0x1F984 - 0x10000 = 0xF984 / 0x400 = 0x3E  + UTF16HighSurrogateStart = 0xD83E
                    // Low  = 0x1F984 - 0x10000 = 0xF984 % 0x400 = 0x184 + UTF16LowSurrogateStart = 0xDD84
                    if ((String[0] == UTF16LE && GlobalByteOrder == MSByteFirst) || (String[0] == UTF16BE && GlobalByteOrder == LSByteFirst)) { // The string byte order does not match the main machines, we need to swap
                        // Swap the endian as we encode
                        String[CodeUnit] = SwapEndian32(String[CodeUnit]);
                    }
                    if (CodeUnit == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            EncodedString[0] = UTF16LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            EncodedString[0] = UTF16BE;
                        }
                    }
                    if (String[CodeUnitNum] <= 0xD7FF || (String[CodeUnitNum] >= 0xE000 && String[CodeUnitNum] <= 0xFFFF)) { // Single code point
                        EncodedString[CodeUnitNum]         = String[CodeUnit];
                    } else {
                        if (GlobalByteOrder == LSByteFirst) {
                            EncodedString[CodeUnitNum]     = ((String[CodeUnitNum - 1] - 0x10000) % 0x400) + UTF16LowSurrogateStart;
                            EncodedString[CodeUnitNum + 1] = ((String[CodeUnitNum] - 0x10000) / 0x400) + UTF16HighSurrogateStart;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            EncodedString[CodeUnitNum]     = ((String[CodeUnitNum - 1] - 0x10000) / 0x400) + UTF16HighSurrogateStart;
                            EncodedString[CodeUnitNum + 1] = ((String[CodeUnitNum] - 0x10000) % 0x400) + UTF16LowSurrogateStart;
                        }
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
    
    UTF16 *UTF16_ConvertByteOrder(UnicodeTypes Type, UTF16 *String2Convert) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        uint16_t         UTF16ByteOrder                = String2Convert[0];
        UnicodeTypes     CurrentByteOrder              = UnicodeUnknownSizeByteOrder;
        if (UTF16ByteOrder == UTF16LE) {
            CurrentByteOrder = UTF16BE;
        } else if (UTF16ByteOrder == UTF16BE) {
            CurrentByteOrder = UTF16LE;
        }
        uint64_t         CurrentCodePoint              = 1ULL;
        uint8_t          HighByte                      = 0;
        uint8_t          LowByte                       = 0;
        if (CurrentByteOrder != Type) {
            do {
                HighByte                               = (String2Convert[CurrentCodePoint] & 0xFF00) >> 8;
                LowByte                                = (String2Convert[CurrentCodePoint] & 0x00FF);
                String2Convert[CurrentCodePoint]       = (LowByte & HighByte);
            } while (String2Convert[CurrentCodePoint] != 0);
        }
        return String2Convert;
    }
    
    /* UTF-32 I/O */
    uint64_t            UTF32_GetSizeInCodePoints(UTF32 *String) {
        uint64_t NumCodePoints          = 0ULL;
        do {
            NumCodePoints              += 1;
        } while (String[NumCodePoints] != 0);
        return NumCodePoints;
    }
    
    UTF32 *UTF32_ConvertByteOrder(UnicodeTypes Type, UTF32 *String2Convert) {
        if (Type != UnicodeUnknownSizeByteOrder && String2Convert != NULL) {
            if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
                GetRuntimeByteBitOrder();
            }
            uint32_t         UTF32ByteOrder                = String2Convert[0];
            UnicodeTypes     CurrentByteOrder              = UnicodeUnknownSizeByteOrder;
            if (UTF32ByteOrder == UTF32LE) {
                CurrentByteOrder = UTF32BE;
            } else if (UTF32ByteOrder == UTF32BE) {
                CurrentByteOrder = UTF32LE;
            }
            uint64_t         CurrentCodePoint              = 1ULL;
            uint8_t          Byte0                         = 0;
            uint8_t          Byte1                         = 0;
            uint8_t          Byte2                         = 0;
            uint8_t          Byte3                         = 0;
            if (CurrentByteOrder != Type) {
                do {
                    Byte0                                  = (String2Convert[CurrentCodePoint] & 0xFF000000) >> 24;
                    Byte1                                  = (String2Convert[CurrentCodePoint] & 0x00FF0000) >> 16;
                    Byte2                                  = (String2Convert[CurrentCodePoint] & 0x0000FF00) >>  8;
                    Byte3                                  = (String2Convert[CurrentCodePoint] & 0x000000FF);
                    String2Convert[CurrentCodePoint]       = (Byte0 & (Byte1 << 8) & (Byte2 << 16) & (Byte3 << 24));
                } while (String2Convert[CurrentCodePoint] != 0);
            }
        }
        return String2Convert;
    }
    
    bool UTF8_Compare(UTF8 *String1, UTF8 *String2, bool Normalize, bool CaseInsensitive) {
        bool StringsMatch = No;
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
            StringsMatch = CompareStrings32(String1UTF32, String2UTF32);
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
        bool StringsMatch = No;
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
            StringsMatch = CompareStrings32(String1UTF32, String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    
    /* StringIO High Level Functions */
    void UTF32_NormalizeString(UTF32 *String2Normalize) {
        /*
         So we should try to convert codepoints to a precomposed codepoint, but if we can't we need to order them by their lexiographic value.
         */
        uint64_t CodePoint    = 1ULL; // Skip the BOM
        if (String2Normalize != NULL) {
            do {
                // For each codepoint, compare it to the base number, if it matches one, then compare the following codepoint(s) to the diacritic(s), if they all match, replace.
                /*
                 for (uint8_t TableColumn = 0; TableColumn < NormalizationTableIndex; TableColumn++) {
                 for (uint8_t Diacritic = NormalizationTable[]; Diacritic < NormalizationTable[TableColumn][0]; Diacritic++) {
                 
                 }
                 }
                 */
            } while (String2Normalize[CodePoint] != 0);
        } else {
            Log(Log_ERROR, __func__, U8("String2Normalize Pointer is NULL"));
        }
    }
    
    void UTF32_CaseFoldString(UTF32 *String) {
        uint64_t CodePoint = 1ULL;
        if (String != NULL) {
            do {
                /*
                 for (uint16_t Table = 0; Table < SimpleCaseFoldTableSize; Table++) {
                 /*
                 if (String[CodePoint] == SimpleCaseFoldTable[Table][0]) {
                 String[CodePoint]  = SimpleCaseFoldTable[Table][1];
                 }
                 }
                 */
            } while (String[CodePoint] != 0);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
    }
    
    bool CompareStrings32(UTF32 *String1, UTF32 *String2) {
        uint64_t CodePoint = 0ULL;
        bool StringsMatch  = Yes;
        if (String1 != NULL && String2 != NULL) {
            uint64_t String1Size = UTF32_GetSizeInCodePoints(String1);
            uint64_t String2Size = UTF32_GetSizeInCodePoints(String2);
            if (String1Size != String2Size) {
                StringsMatch = No;
            } else {
                do {
                    if (String1[CodePoint] != String2[CodePoint]) {
                        StringsMatch = No;
                        break;
                    }
                } while (String1[CodePoint] != 0 && String2[CodePoint] != 0);
            }
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    uint64_t UTF32_FindSubstring(UTF32 *String, UTF32 *SubString) {
        uint64_t   Offset             = 0ULL;
        uint64_t   CodePoint          = 1ULL;
        uint64_t   SubStringCodePoint = 1ULL;
        if (String != NULL && SubString != NULL) {
            do {
                if (String[CodePoint] == SubString[SubStringCodePoint] && String[CodePoint] == SubString[SubStringCodePoint]) {
                    // Ok well lets go ahead and implement finding substring
                }
            } while (String[CodePoint] != 0);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_ERROR, __func__, U8("SubString Pointer is NULL"));
        }
        return Offset;
    }
    
    UTF32 *UTF32_ExtractSubstring(UTF32 *String, uint64_t Start, uint64_t End) {
        uint64_t    ExtractedStringSize = (Start - End) + 1 + UnicodeNULLTerminatorSize;
        UTF32 *ExtractedString          = NULL;
        if (String != NULL && End > Start) {
            ExtractedString             = calloc(ExtractedStringSize, sizeof(uint32_t));
            if (ExtractedString != NULL) {
                for (uint64_t CodePoint = Start; CodePoint < End; CodePoint++) {
                    ExtractedString[CodePoint - Start] = String[CodePoint];
                }
            } else {
                Log(Log_ERROR, __func__, U8("Not enough memory to allocate ExtractedString"));
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (End < Start) {
            Log(Log_ERROR, __func__, U8("End is before Start"));
        }
        return ExtractedString;
    }
    
    int64_t UTF32_String2Integer(UTF32 *String) {
        uint64_t CodePoint  = 0ULL;
        int8_t   Sign       =  1;
        uint8_t  Base       = 10;
        int64_t  Value      =  0;
        /*
         for (uint8_t WhiteSpace = 0; WhiteSpace < WhitespaceTableSize; WhiteSpace++) {
         if (String[CodePoint] == WhitespaceTable[WhiteSpace]) {
         CodePoint += 1;
         }
         }
         */
        if (String[CodePoint] == '0') {
            if (String[CodePoint + 1] == 'b' || String[CodePoint + 1] == 'B') {
                Base = 2;
            } else if (String[CodePoint + 1] == 'o' || String[CodePoint] == 'O') {
                Base = 8;
            } else if (String[CodePoint + 1] == 'x' || String[CodePoint] == 'X') {
                Base = 16;
            }
        } else if (String[CodePoint] == '-') {
            Sign     = -1;
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
        } while (String[CodePoint] != 0);
        return (Sign < 0 ? Value * Sign : Value);
    }
    
    UTF32 *Integer2String(const bool UpperCase, const Number2StringBases Base, int64_t Number2Stringify) {
        int64_t  Sign            = 0LL;
        int64_t  Num             = Number2Stringify;
        uint8_t  NumDigits       = 0;
        while (Num > 0) {
            Num                 /= Base;
            NumDigits           += 1;
        }
        if (GlobalByteOrder == LSByteFirst) {
            Number2Stringify     = SwapEndian64(Number2Stringify);
        }
        if (Number2Stringify < 0 && Base == Decimal) {
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
                int64_t CurrentDigit    = (Base == Decimal ? Absolute(Number2Stringify %= Base) : (Number2Stringify %= Base));
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
            // Extract the Exponent
            // Extract the Mantissa
            
            for (uint8_t Whitespace = 0; Whitespace < 99; Whitespace++) {
                do {
                    
                    CodePoint += 1;
                } while (String[CodePoint] != 0 && String[CodePoint] != Whitespace);
            }
            
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return 0.0;
    }
    
    UTF32 *Decimal2String(double Value) {
        /*
         Should we support Binary Coded Decimal BCD?
         Invalid doubles return a NULL string
         */
        UTF32   *OutputString = NULL;
        int8_t   Sign         = Value < 0.0 ? -1 : 1;
        int32_t *Exponent     = 0;
        int32_t  Exponent2    = Exponent;
        float    Mantissa     = frexp(Value, Exponent);
        float    Mantissa2    = Mantissa;
        
        // Ok now we just need to get the strings size
        uint64_t StringSize   = UTF1632BOMSize; // 1 to account for the BOM
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
    
    uint64_t CountGraphemes(UTF32 *String2Count) {
        
        return 0;
    }
    
    /* Function to replace a subsection of a string at offset X and length Y. */
    UTF32 *ReplaceSubstring(UTF32 *String, uint64_t Offset, uint64_t Length, UTF32 *Replacement) {
        UTF32   *NewString                 = NULL;
        if (String != NULL && Replacement != NULL && Length >= 1) {
            uint64_t StringSize            = UTF32_GetSizeInCodePoints(String);
            uint64_t ReplacementStringSize = UTF32_GetSizeInCodePoints(Replacement);
            uint64_t NewStringSize         = 0ULL;
            if (ReplacementStringSize < Length) {
                NewStringSize              = StringSize - (ReplacementStringSize - Length);
            } else if (ReplacementStringSize >= Length) {
                NewStringSize              = StringSize + (ReplacementStringSize - Length);
            }
            NewString                      = calloc(NewStringSize, sizeof(UTF32));
            for (uint64_t CodePoint = 0ULL; CodePoint < NewStringSize; CodePoint++) {
                if (CodePoint < Offset) {
                    NewString[CodePoint]   = String[CodePoint];
                }
                for (uint64_t ReplacementCodePoint = 0ULL; ReplacementCodePoint < ReplacementStringSize; ReplacementCodePoint++) {
                    NewString[CodePoint]   = Replacement[ReplacementCodePoint];
                }
                if (CodePoint > ReplacementStringSize) {
                    NewString[CodePoint]   = String[CodePoint];
                }
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_ERROR, __func__, U8("Replacement Pointer is NULL"));
        } else if (Length == 0) {
            Log(Log_ERROR, __func__, U8("Length %d is too short"), Length);
        }
        return NewString;
    }
    
    uint64_t GetNumValidFormatSpecifiersInString(UTF32 *String) {
        uint64_t NumValidFormatSpecifiers = 0ULL;
        uint64_t StringSize               = UTF32_GetSizeInCodePoints(String);
        /*
         So, loop over each codepoint looking for a percent symbol, when a percent symbol is found, check the following codepoints to make sure they're valid, if they all are then increment the NumValidFormatSpecifiers variable
         
         Invalid codepoint sequences:
         Just a percent sign.
         Multiple percent signs in a row (2 percent signs is ok tho)
         1 percent sign followed by anything but 0-9, Aa, Bb (binary), c, d, Ee, Ff, Gg, i, o (octal), p (pointer), s (string), u (unsigned), Xx (hex).
         
         Printf Flags:     (-|+|(space)|#|0)
         Printf Width:     ([0-9]+|*) * = the width is the next argument
         Printf Precision: ([0-9]+|*) * = the width is the next argument
         Printf Length:    ([0-9]+|)
         
         So, the format of a printf format string, is a % sign followed by a bunch of other shit and ending with ^^^
         */
        
        for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
            if (String[CodePoint] == '%' && (CodePoint < StringSize && String[CodePoint + 1] != '%')) { // While this codepoint = '%' and the next one does not = '%' AND we're not at the end of the string so there is a next codepoint to check
                
                // Start looking for flags.
                if (String[CodePoint] == '-' || String[CodePoint] == '+' || String[CodePoint] == '0' || String[CodePoint] == ' ' || String[CodePoint] == '#') {
                    // Ok, we have a flag.
                }
                
                if (String[CodePoint] == '*' || String[CodePoint] == '0' || String[CodePoint] == '1' || String[CodePoint] == '2' || String[CodePoint] == '3' || String[CodePoint] == '4' || String[CodePoint] == '5' || String[CodePoint] == '6' || String[CodePoint] == '7' || String[CodePoint] == '8' || String[CodePoint] == '9') { // Width field, may contain any decimal digit character, or an asterisk, if there is a digit, keep reading until you find a non-decimal digit.
                }
                
                // Start looking for the Precision field
                if (String[CodePoint] == '*' || String[CodePoint] == '0' || String[CodePoint] == '1' || String[CodePoint] == '2' || String[CodePoint] == '3' || String[CodePoint] == '4' || String[CodePoint] == '5' || String[CodePoint] == '6' || String[CodePoint] == '7' || String[CodePoint] == '8' || String[CodePoint] == '9') {
                }
                
                // Start looking for the size field, String[CodePoint] == ''
                if ((String[CodePoint] == 'h' && String[CodePoint + 1] == 'h') || String[CodePoint] == 'h' || (String[CodePoint] == 'l' && String[CodePoint] == 'l') || String[CodePoint] == 'l' || String[CodePoint] == 'L' || String[CodePoint] == 'z' || String[CodePoint] == 'j' || String[CodePoint] == 't') {
                    
                }
                
                // Start looking for the type field, String[CodePoint] == '' ||
                if (String[CodePoint] == 'd' || String[CodePoint] == 'i' || String[CodePoint] == 'u' || String[CodePoint] == 'f' || String[CodePoint] == 'F' || String[CodePoint] == 'e' || String[CodePoint] == 'E' || String[CodePoint] == 'g' || String[CodePoint] == 'G' || String[CodePoint] == 'x' || String[CodePoint] == 'X' || String[CodePoint] == 'o' || String[CodePoint] == 's' || String[CodePoint] == 'c' || String[CodePoint] == 'p' || String[CodePoint] == 'a' || String[CodePoint] == 'A' || String[CodePoint] == 'n' || String[CodePoint] == 'b' || String[CodePoint] == 'B') { // (b|B) is my extension to support writing binary
                }
                
                
            }
        }
        
        
        /*
        uint64_t CodePoint = 0ULL;
        do {
            if (String[CodePoint] == '%' && String[CodePoint + 1] != '%') {
                // Now look for Flags, Width, Precision, Length
                // Flag format: (-|+|0| |#)
                
                
                
                
                switch (String[CodePoint + 1]) {
                    /* Start Flags
                    case '-':
                        break;
                    case '+':
                        break;
                    case ' ': // Space
                        break;
                    case '#':
                        break;
                    case '0':
                        break;
                    /* End Flags
                    /* Start Width
                    case '1':
                        break;
                    case '2':
                        break;
                    case '3':
                        break;
                    case '4':
                        break;
                    case '5':
                        break;
                    case '6':
                        break;
                    case '7':
                        break;
                    case '8':
                        break;
                    case '9':
                        break;
                    case '*':
                        break;
                    /* End Width
                        
                    default:
                        break;
                }
            }
            CodePoint     += 1;
        } while (String[CodePoint] != 0);
         */
        return NumValidFormatSpecifiers;
    }
    
    uint8_t GetFormatSpecifierSize(UTF32 *String, uint64_t FormatSpecifierNum) { // How do I want to do this, do I want to just say here's the original string, find the size of Formatspecifier X?
        // in order to do that, we need to know the number of format specifiers in the string to begin with, which we can do with VarArgCount.
        // Ok, so basically we scan the string for a percent character, verify that it's a legit format specifier and not just a random percent character, find the end of it, and keep going until we hit formatSpecifierX.
        uint64_t CodePoint          = 0ULL;
        uint64_t FormatSpecifier    = 0ULL;
        do {
            if (String[CodePoint] == '%') {
                // and the rest of the specifier is valid
                FormatSpecifier    += 1;
            }
        } while (String[CodePoint] != 0);
        return 0;
    }
    
    FormatStringTypes GetFormatStringType(UTF32 *String, uint64_t FormatSpecifierNum) {
        uint64_t CodePoint       = 0ULL;
        uint64_t FormatSpecifier = 0ULL;
        do {
            if (String[CodePoint] == '%' || String[CodePoint] == '\\') {
                // Ok, check to be sure that the percent character is followed by valid characters, after the percent there can be a 0 for padding, then the number of digits, then an actual format specifier.
                
                // List of escape characters: (%|\)
            }
            CodePoint           += 1;
        } while (String[CodePoint] != 0);
        return UnknownFormatStringType;
    }
    
    UTF32 *FormatString32(const uint64_t VarArgCount, UTF32 *Format, ...) {
        /* So, first we need to break the Format string into the various printf specifiers, which start with a percent symbol */
        
        /* Get the size of the format string in CodePoints, normalize the string, then start counting the format specifiers */
        
        uint64_t  CodePoint                   = 0ULL;
        uint64_t  CurrentSpecifier            = 0ULL;
        uint8_t   CurrentFormatSpecifierTable = 0ULL;
        uint64_t *SpecifierOffset             = calloc(VarArgCount, sizeof(uint64_t));
        uint64_t *SpecifierSize               = calloc(VarArgCount, sizeof(uint64_t));
        
        // %05d means the format specifier is decimal, with 5 digits, with leading zeros if nessicary.
        
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
        } while (Format[CodePoint] != 0);
        
        return NULL;
    }
    
#ifdef  __cplusplus
}
#endif
