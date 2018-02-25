#include <stdarg.h>                   /* Included for va_start, va_end */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdlib.h>                   /* Included for calloc, and free */

#include "../include/Macros.h"        /* Included for NewLineWithNULLSize, FoundationIOTargetOS */
#include "../include/Math.h"          /* Included for Integer functions */
#include "../include/StringIO.h"      /* Included for UTF8 */
#include "../include/Log.h"

#define UnicodeNULLTerminatorSize       1
#define UTF1632BOMSize                  1
#define UTF8BOMSize                     3

/* 0x00 is valid in UTF-16, NULL is 0x0000, be sure to catch this mistake. */
/* U+D800 - U+DFFF are invalid UTF32 code points. when converting to/from UTF32 make sure that that is not a code point becuse they're reserved as UTF-16 surrogate pairs */
/* UTF-16 High/Low Surrogate mask = 0xFC00 */

#ifdef  __cplusplus
extern  "C" {
#endif
    
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
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
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
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        }
        return NumCodePoints;
    }
    
    static uint8_t      UTF16_GetCodePointSize(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit >= 0xD800 && CodeUnit <= 0xDFFF) { // Surrogate
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
    
    static uint64_t     UTF32_GetSizeInCodeUnits4UTF8(UTF32 *String) {
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
    
    uint64_t            UTF32_GetSizeInCodePoints(UTF32 *String) {
        uint64_t NumCodePoints          = 0ULL;
        do {
            NumCodePoints              += 1;
        } while (String[NumCodePoints] != 0);
        return NumCodePoints;
    }
    
    /* Decode UTF-X to UTF-32 */
    UTF32         *UTF8_Decode(UTF8 *String) {
        // We should make sure we don't decode anything in the surrogate block, between 0xDBFF to 0xD800 or DFFF to DC00
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
                            DecodedString[0] = 0xFFFE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            DecodedString[0] = 0xFEFF;
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
                        Log(Log_ERROR, __func__, u8"CodePoint %d is overlong, replaced with U+FFFD", CodePoint);
                    } else if (DecodedString[CodePoint] >= 0xD800 && DecodedString[CodePoint] <= 0xDFFF && DecodedString[CodePoint] != 0xFFFE && DecodedString[CodePoint] != 0xFEFF) {
                        DecodedString[CodePoint]     = 0xFFFD;
                        Log(Log_ERROR, __func__, u8"Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD", CodePoint);
                    }
                    if (GlobalByteOrder == LSByteFirst) { // The string byte order does not match the main machines, we need to swap
                        // Swap the endian as we encode
                        DecodedString[CodePoint] = SwapEndian32(String[CodePoint]);
                    }
                } while (String[CodeUnitNum] != 0);
            } else {
                Log(Log_ERROR, __func__, u8"Not enough memory to allocate string");
            }
        } else {
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        }
        return DecodedString;
    }
    
    UTF8          *UTF8_Encode(UTF32 *String, const bool IncludeBOM) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeBitByteOrder();
        }
        uint64_t CodePoint                             = 0ULL;
        uint64_t CodeUnitNum                           = 0ULL;
        UTF8    *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t UTF8CodeUnits                     = UnicodeNULLTerminatorSize + UTF32_GetSizeInCodeUnits4UTF8(String) + (IncludeBOM == true ? UTF8BOMSize : 0);
            
            EncodedString                              = calloc(UTF8CodeUnits, sizeof(UTF8));
            if (EncodedString != NULL) {
                do {
                    if ((String[0] == 0xFFFE && GlobalByteOrder == MSByteFirst) || (String[0] == 0xFEFF && GlobalByteOrder == LSByteFirst)) { // The string byte order does not match the main machines, we need to swap
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
                    }  else if (String[CodePoint] >= 0x800 && String[CodePoint] <= 0xFFFF && String[CodePoint] <= 0xD7FF && String[CodePoint] >= 0xE000 && String[CodePoint] != 0xFFFE && String[CodePoint] != 0xFEFF) {
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
                    } else if (String[CodePoint] >= 0xD800 && String[CodePoint] <= 0xDFFF && String[CodePoint] != 0xFFFE && String[CodePoint] != 0xFEFF) {
                        String[CodePoint]              = 0xFFFD;
                        Log(Log_ERROR, __func__, u8"Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD");
                    }
                } while (String[CodePoint] != 0);
            } else {
                Log(Log_ERROR, __func__, u8"String Pointer is NULL, not enough memory");
            }
        } else {
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        }
        return EncodedString;
    }
    
    UTF32         *UTF16_Decode(UTF16 *String) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeBitByteOrder();
        }
        /*
         High Surrogate range: 0xD800 - 0xDBFF
         Low  Surrogate range: 0xDC00 - 0xDFFF
         
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
                    if ((String[0] == 0xFFFE && GlobalByteOrder == MSByteFirst) || (String[0] == 0xFEFF && GlobalByteOrder == LSByteFirst)) { // The string byte order does not match the main machines, we need to swap
                        // Swap the endian as we encode
                        String[CodePoint] = SwapEndian16(String[CodePoint]);
                    }
                    if (CodePoint == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            DecodedString[0] = 0xFFFE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            DecodedString[0] = 0xFEFF;
                        }
                    }
                    if (String[CodeUnitNum] <= 0xD7FF || (String[CodeUnitNum] >= 0xE000 && String[CodeUnitNum] <= 0xFFFF)) {
                        DecodedString[CodePoint] = String[CodeUnitNum - 1];
                    } else {
                        DecodedString[CodePoint] = (((String[CodeUnitNum - 1] - 0xD800) * 0x400) + (String[CodeUnitNum] - 0xDC00) + 0x10000);
                    }
                } while (String[CodePoint] != 0);
            } else {
                Log(Log_ERROR, __func__, u8"DecodedString Pointer is NULL, not enough memory");
            }
        } else {
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        }
        return DecodedString;
    }
    
    UTF16         *UTF16_Encode(UTF32 *String) {
        UTF16   *EncodedString         = NULL;
        if (String != NULL) {
            if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
                GetRuntimeBitByteOrder();
            }
            uint8_t  CodePointSize         = 0;
            uint64_t CodeUnitNum           = 0ULL;
            uint64_t UTF16NumCodeUnits     = UTF32_GetSizeInCodeUnits4UTF16(String) + UTF1632BOMSize + UnicodeNULLTerminatorSize;
            EncodedString                  = calloc(UTF16NumCodeUnits, sizeof(UTF16));
            if (EncodedString != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < UTF16NumCodeUnits; CodeUnit++) {
                    // High = ((String[CodePoint] - 0x10000) / 0x400) + 0xD800
                    // Low  = (String[CodePoint]  - 0x10000) % 0x400) + 0xDC00
                    // High = 0x1F984 - 0x10000 = 0xF984 / 0x400 = 0x3E  + 0xD800 = 0xD83E
                    // Low  = 0x1F984 - 0x10000 = 0xF984 % 0x400 = 0x184 + 0xDC00 = 0xDD84
                    if ((String[0] == 0xFFFE && GlobalByteOrder == MSByteFirst) || (String[0] == 0xFEFF && GlobalByteOrder == LSByteFirst)) { // The string byte order does not match the main machines, we need to swap
                        // Swap the endian as we encode
                        String[CodeUnit] = SwapEndian32(String[CodeUnit]);
                    }
                    if (CodeUnit == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            EncodedString[0] = 0xFFFE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            EncodedString[0] = 0xFEFF;
                        }
                    }
                    if (String[CodeUnitNum] <= 0xD7FF || (String[CodeUnitNum] >= 0xE000 && String[CodeUnitNum] <= 0xFFFF)) { // Single code point
                        EncodedString[CodeUnitNum]         = String[CodeUnit];
                    } else {
                        if (GlobalByteOrder == LSByteFirst) {
                            EncodedString[CodeUnitNum]     = ((String[CodeUnitNum - 1] - 0x10000) % 0x400) + 0xDC00;
                            EncodedString[CodeUnitNum + 1] = ((String[CodeUnitNum] - 0x10000) / 0x400) + 0xD800;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            EncodedString[CodeUnitNum]     = ((String[CodeUnitNum - 1] - 0x10000) / 0x400) + 0xD800;
                            EncodedString[CodeUnitNum + 1] = ((String[CodeUnitNum] - 0x10000) % 0x400) + 0xDC00;
                        }
                    }
                }
            } else {
                Log(Log_ERROR, __func__, u8"EncodedString Pointer is NULL, not enough memory");
            }
        } else {
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        }
        return EncodedString;
    }
    
    UTF16 *UTF16_ConvertByteOrder(UnicodeTypes Type, UTF16 *String2Convert) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeBitByteOrder();
        }
        uint16_t         UTF16ByteOrder                = String2Convert[0];
        UnicodeTypes     CurrentByteOrder              = UnicodeUnknownSizeByteOrder;
        if (UTF16ByteOrder == 0xFFFE) {
            CurrentByteOrder = UTF16LE;
        } else if (UTF16ByteOrder == 0xFEFF) {
            CurrentByteOrder = UTF16BE;
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
    
    UTF32 *UTF32_ConvertByteOrder(UnicodeTypes Type, UTF32 *String2Convert) {
        if (Type != UnicodeUnknownSizeByteOrder && String2Convert != NULL) {
            if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
                GetRuntimeBitByteOrder();
            }
            uint32_t         UTF32ByteOrder                = String2Convert[0];
            UnicodeTypes     CurrentByteOrder              = UnicodeUnknownSizeByteOrder;
            if (UTF32ByteOrder == 0xFFFE) {
                CurrentByteOrder = UTF32LE;
            } else if (UTF32ByteOrder == 0xFEFF) {
                CurrentByteOrder = UTF32BE;
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
    
    void UTF32_Denormalize(UTF32 *String2Denormalize) {
        
    }
    
    void UTF32_Normalize(UTF32 *String2Normalize) {
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
            Log(Log_ERROR, __func__, u8"String2Normalize Pointer is NULL");
        }
    }
    
    void            UTF32_CaseFold(UTF32 *String) {
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
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        }
    }
    
    bool            UTF32Strings_Compare(UTF32 *String1, UTF32 *String2) {
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
            Log(Log_ERROR, __func__, u8"String1 Pointer is NULL");
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, u8"String2 Pointer is NULL");
        }
        return StringsMatch;
    }
    
    uint64_t        UTF32_FindSubstring(UTF32 *String, UTF32 *SubString) {
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
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        } else if (SubString == NULL) {
            Log(Log_ERROR, __func__, u8"SubString Pointer is NULL");
        }
        return Offset;
    }
    
    UTF32         *UTF32_Extract(UTF32 *String, uint64_t Start, uint64_t End) {
        uint64_t    ExtractedStringSize = (Start - End) + 1 + UnicodeNULLTerminatorSize;
        UTF32 *ExtractedString          = NULL;
        if (String != NULL && End > Start) {
            ExtractedString             = calloc(ExtractedStringSize, sizeof(uint32_t));
            if (ExtractedString != NULL) {
                for (uint64_t CodePoint = Start; CodePoint < End; CodePoint++) {
                    ExtractedString[CodePoint - Start] = String[CodePoint];
                }
            } else {
                Log(Log_ERROR, __func__, u8"Not enough memory to allocate ExtractedString");
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, u8"String Pointer is NULL");
        } else if (End < Start) {
            Log(Log_ERROR, __func__, u8"End is before Start");
        }
        return ExtractedString;
    }
    
    int64_t             UTF32_ToNumber(UTF32 *String) {
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
    
    bool                UTF8_Compare(UTF8 *String1, UTF8 *String2, bool Normalize, bool CaseInsensitive) {
        bool StringsMatch = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32 = UTF8_Decode(String1);
            UTF32 *String2UTF32 = UTF8_Decode(String2);
            if (Normalize == Yes) {
                UTF32_Normalize(String1UTF32);
                UTF32_Normalize(String2UTF32);
            }
            if (CaseInsensitive == Yes) {
                UTF32_CaseFold(String1UTF32);
                UTF32_CaseFold(String2UTF32);
            }
            StringsMatch = UTF32Strings_Compare(String1UTF32, String2UTF32);
            free(String1UTF32);
            free(String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, u8"String1 Pointer is NULL");
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, u8"String2 Pointer is NULL");
        }
        return StringsMatch;
    }
    
    bool               UTF16_Compare(UTF16 *String1, UTF16 *String2, bool Normalize, bool CaseInsensitive) {
        bool StringsMatch = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32 = UTF16_Decode(String1);
            UTF32 *String2UTF32 = UTF16_Decode(String2);
            if (Normalize == Yes) {
                UTF32_Normalize(String1UTF32);
                UTF32_Normalize(String2UTF32);
            }
            if (CaseInsensitive == Yes) {
                UTF32_CaseFold(String1UTF32);
                UTF32_CaseFold(String2UTF32);
            }
            StringsMatch = UTF32Strings_Compare(String1UTF32, String2UTF32);
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, u8"String1 Pointer is NULL");
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, u8"String2 Pointer is NULL");
        }
        return StringsMatch;
    }
    
    uint64_t UTF32_CountGraphemes(UTF32 *String2Count) {
        
        return 0;
    }
    
    const static UTF32 FormatSpecifierCodePoints[37] = { // extension: %b %B - Binary, MSB format.
        0x2A, 0x2B, 0x2D, 0x2E, 0x6E, 0x6F, 0x23, 0x25,
        0x25, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x41, 0x42, 0x45, 0x46, 0x47,
        0x50, 0x58, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x67, 0x69, 0x73, 0x75, 0x78
    };
    
    typedef enum UnicodeBOMState {
        UnknownUTF8BOMState      = 0,
        UnconditionallyAddBOM    = 1,
        UnconditionallyRemoveBOM = 2,
        KeepTheBOMStateTheSame   = 3,
    } UTF8BOMState;
    
    /* Function to replace a subsection of a string at offset X and length Y. */
    UTF32 *UTF32_ReplaceSubsection(UTF32 *String, uint64_t Offset, uint64_t Length, UTF32 *Replacement) {
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
            Log(Log_ERROR, __func__, "String Pointer is NULL");
        } else if (Replacement == NULL) {
            Log(Log_ERROR, __func__, "Replacement Pointer is NULL");
        } else if (Length == 0) {
            Log(Log_ERROR, __func__, "Length %d is too short", Length);
        }
        return NewString;
    }
    
    UTF32 *UTF32_Number2String(const bool UpperCase, const Number2StringBases Base, int64_t Number2Stringify) {
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
    
    uint8_t UTF32_GetFormatSpecifierSize(UTF32 *String, uint64_t FormatSpecifierNum) { // How do I want to do this, do I want to just say here's the original string, find the size of Formatspecifier X?
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
        return NULL;
    }
    
    UTF32 *UTF32_FormatString(const uint64_t VarArgCount, const UTF32 *Format, va_list *VariadicArguments) {
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
