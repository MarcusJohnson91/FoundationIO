#pragma warning(push, 0)
#include <assert.h>  /* Included for static_assert */
#include <stdlib.h>  /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, and free */
#include <string.h>  /* Included for the atoll */
#pragma warning(pop)

#include "../include/BitIOMacros.h"
#include "../include/StringIO.h"
#include "../include/BitIOMath.h"
#include "../include/BitIOLog.h"

#include "StringIOTables.h"

#define UnicodeNULLStringSize  1
#define UTF16BOMSize           1

/* 0x00 is valid in UTF-16, NULL is 0x0000, be sure to catch this mistake. */
/* U+D800 - U+DFFF are invalid UTF32 code points. when converting to/from UTF32 make sure that that is not a code point becuse they're reserved as UTF-16 surrogate pairs */
/* UTF-16 High/Low Surrogate mask = 0xFC00 */

#ifdef  __cplusplus
extern  "C" {
#endif
    
    static uint8_t UTF8String_GetCodePointSize(uint8_t CodeUnit) {
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
    
    uint64_t    UTF8String_GetNumCodePoints(UTF8String String) {
        uint64_t CurrentByteNum          = 0ULL;
        uint64_t NumCodePoints           = 0ULL;
        
        if (String != NULL) {
            do {
                CurrentByteNum              += UTF8String_GetCodePointSize(String[CurrentByteNum]);
                NumCodePoints               += 1;
            } while (String[CurrentByteNum] != 0x0);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        }
        
        
        return NumCodePoints;
    }
    
    uint64_t    UTF8String_GetNumCodeUnits(UTF8String String2Count) { // Read a codeunit, get it's size, skip that many bytes, check that it's not 0x0, then repeat
        uint64_t NumCodeUnits                = 0ULL;
        uint64_t CurrentCodeUnit             = 0ULL;
        if (String2Count != NULL) {
            do {
                NumCodeUnits                    += UTF8String_GetCodePointSize(String2Count[CurrentCodeUnit]);
            } while (String2Count[CurrentCodeUnit] != 0x0);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        }
        
        return NumCodeUnits;
    }
    
    /* Decode UTF-X to UTF-32 */
    UTF32String UTF8String_Decode(UTF8String String, uint64_t NumCodePoints) {
        uint8_t  CodePointSize = 0;
        uint64_t CodeUnitNum   = 0ULL;
        uint8_t  Byte1 = 0, Byte2 = 0, Byte3 = 0, Byte4 = 0;
        UTF32String DecodedString = NULL;
        if (String != NULL) {
            DecodedString = calloc(NumCodePoints, sizeof(UTF32String));
            if (DecodedString != NULL) {
                do {
                    // Loop over the bytes in the bytes
                    for (uint64_t CodePoint = 0ULL; CodePoint < NumCodePoints; CodePoint++) {
                        CodePointSize = UTF8String_GetCodePointSize(String[CodeUnitNum]);
                        switch (CodePointSize) {
                            case 1: // 7 bits
                                DecodedString[CodePoint] = String[CodeUnitNum] & 0x7F;
                                break;
                            case 2: // 11 bits, U+E4, 0xC3A4
                                DecodedString[CodePoint] = ((String[CodeUnitNum] & 0x1F) << 6);
                                DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x3F);
                                break;
                            case 3: // 16 bits, U+16A0, 0xE19AA0
                                DecodedString[CodePoint] = ((String[CodeUnitNum] & 0x0F) << 12);
                                DecodedString[CodePoint] = ((String[CodeUnitNum + 1] & 0x3F) << 6);
                                DecodedString[CodePoint] = (String[CodeUnitNum + 2] & 0x3F);
                                break;
                            case 4: // 21 bits, U+1F984, 0xF09FA684
                                DecodedString[CodePoint] = ((String[CodeUnitNum] & 0x07) << 18);
                                DecodedString[CodePoint] = ((String[CodeUnitNum + 1] & 0x3F) << 12);
                                DecodedString[CodePoint] = ((String[CodeUnitNum + 2] & 0x3F) << 6);
                                DecodedString[CodePoint] = (String[CodeUnitNum + 3] & 0x3F);
                                break;
                        }
                        if (DecodedString[CodePoint] <= 0x7F && CodePointSize > 1) {
                            // Invalid, overlong sequence detected, replace it with 0xFFFD
                            DecodedString[CodePoint] = 0xFFFD;
                            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "CodePoint %d is overlong", CodeUnitNum);
                        }
                    }
                    CodeUnitNum += CodePointSize;
                } while (String[CodeUnitNum] != 0x00);
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL, not enough memory");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        }
        
        return DecodedString;
    }
    
    UTF8String  UTF8String_Encode(UTF32String String, uint64_t StringSize) {
        uint8_t  CodePointSize   = 0;
        uint64_t CodeUnitNum     = 0ULL;
        UTF8String EncodedString = NULL;
        if (String != NULL) {
            EncodedString        = calloc(StringSize + UnicodeNULLStringSize, sizeof(UTF8String));
            if (EncodedString != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                    if (String <= 0x7F) {
                        // 1 byte ASCII codepoint
                        EncodedString[CodePoint]     = String[CodePoint] & 0x7F;
                    } else if (String >= 0x80 && String <= 0x7FF) {
                        // 2 byte UTF8
                        EncodedString[CodePoint]     = (0xC0 & (String[CodePoint] & 0x7C0) >> 6);
                        EncodedString[CodePoint + 1] = (0x80 & (String[CodePoint] & 0x3F));
                    } else if (String >= 0x800 && String <= 0xFFFF) {
                        // 3 byte UTF8
                        EncodedString[CodePoint]     = (0xE0 & (String[CodePoint] & 0xF800) >> 11);
                        EncodedString[CodePoint + 1] = (0xC0 & (String[CodePoint] & 0xFC0) >> 6);
                        EncodedString[CodePoint + 2] = (0x80 & (String[CodePoint] & 0x3F));
                    } else if (String >= 0x10000 && String <= 0x10FFFF) {
                        // 4 byte UTF8
                        EncodedString[CodePoint]     = (0xE0 & (String[CodePoint] & 0xF800) >> 11);
                        EncodedString[CodePoint + 1] = (0xC0 & (String[CodePoint] & 0xFC0) >> 6);
                        EncodedString[CodePoint + 2] = (0x80 & (String[CodePoint] & 0x3F));
                    }
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL, not enough memory");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        }
        
        return EncodedString;
    }
    
    UTF32String UTF16String_Decode(UTF16String String, uint64_t NumCodePoints) {
        uint8_t  CodePointSize = 0;
        uint64_t CodeUnitNum   = 0ULL;
        UTF32String DecodedString = NULL;
        if (String != NULL) {
            DecodedString = calloc(NumCodePoints, sizeof(UTF32String));
            if (DecodedString != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < NumCodePoints; CodePoint++) {
                    if ((String[CodeUnitNum] <= 0xD7FF) && (String[CodeUnitNum] >= 0xE000 && String[CodeUnitNum] <= 0xFFFF)) { // single code unit per code point
                        DecodedString[CodePoint] = String[CodeUnitNum];
                    } else {
                        DecodedString[CodePoint] = ((String[CodePoint] - 0xD800) * 0x400) + (String[CodePoint + 1] - 0xDC00) + 0x10000;
                    }
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL, not enough memory");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        }
        
        return DecodedString;
    }
    
    UTF16String UTF16String_Encode(UTF32String String, uint64_t NumCodePoints) {
        uint8_t  CodePointSize    = 0;
        uint64_t CodeUnitNum      = 0ULL;
        UTF16String EncodedString = NULL;
        if (String != NULL) {
            uint64_t UTF16NumCodePoints = NumCodePoints + UTF16BOMSize + UnicodeNULLStringSize;
            EncodedString = calloc(UTF16NumCodePoints, sizeof(UTF16String));
            if (EncodedString != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < UTF16NumCodePoints; CodePoint++) {
                    // High = ((String[CodePoint] - 0x10000) / 0x400) + 0xD800
                    // Low  = (String[CodePoint]  - 0x10000) % 0x400) + 0xDC00
                    // High = 0x1F984 - 0x10000 = 0xF984 / 0x400 = 0x3E  + 0xD800 = 0xD83E
                    // Low  = 0x1F984 - 0x10000 = 0xF984 % 0x400 = 0x184 + 0xDC00 = 0xDD84
                    if (CodePoint == 0) {
                        // Insert the BOM
                        uint16_t ByteOrderMark = 0xFEFF;
                        EncodedString[CodePoint] = ByteOrderMark;
                    }
                    uint32_t ModCodePoint = String[CodePoint] - 0x10000;
                    uint16_t High         = (ModCodePoint / 0x400) + 0xD800;
                    uint16_t Low          = (ModCodePoint % 0x400) + 0xDC00;
                    EncodedString[CodePoint]     = High;
                    EncodedString[CodePoint + 1] = Low;
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "EncodedString Pointer is NULL, not enough memory");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        }
        
        return EncodedString;
    }
    
    UTF32String UTF32String_Normalize(UTF32String String2Normalize, uint64_t StringSize) {
        /*
         So we should try to convert codepoints to a precomposed codepoint, but if we can't we need to order them by their lexiographic value.
         */
        if (String2Normalize != NULL && StringSize > 0) {
            for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                // For each codepoint, compare it to the base number, if it matches one, then compare the following codepoint(s) to the diacritic(s), if they all match, replace.
                for (uint8_t TableColumn = 0; TableColumn < NormalizationTableSize; TableColumn++) {
                    for (uint8_t Diacritic = 2; Diacritic < NormalizationTable[TableColumn][0]; Diacritic++) {
                        
                    }
                }
            }
        } else if (String2Normalize == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String2Normalize Pointer is NULL");
        }
        
        return NULL;
    }

    UTF32String UTF32String_CaseFold(UTF32String String, uint64_t StringSize) {
        UTF32String FoldedString = NULL;
        if (String != NULL) {
            FoldedString = calloc(StringSize, sizeof(UTF32String));
            if (FoldedString != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                    // Loop over the table collumn 0 comparing it to the codepoints in the table, and replacing it with the same in #1.
                    for (uint16_t Table = 0; Table < SimpleCaseFoldTableSize; Table++) {
                        if (String[CodePoint] == SimpleCaseFoldTable[Table][0]) {
                            FoldedString[CodePoint] = SimpleCaseFoldTable[Table][1];
                        }
                    }
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "FoldedString Pointer is NULL");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        }
        return FoldedString;
    }
    
    bool        UTF32Strings_Compare(UTF32String String1, uint64_t String1Size, UTF32String String2, uint64_t String2Size) {
        bool StringsMatch = No;
        if (String1 != NULL && String2 != NULL && String1Size == String2Size) {
            if (String1Size == String2Size) {
                for (uint64_t CodePoint = 0ULL; CodePoint < String1Size; CodePoint++) {
                    if (String1[CodePoint] != String2[CodePoint]) {
                        StringsMatch = No;
                        break;
                    }
                }
            } else {
                StringsMatch = No;
            }
        } else if (String1 == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String1 Pointer is NULL");
        } else if (String2 == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String2 Pointer is NULL");
        } else if (String1Size != String2Size) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String1Size and String2Size don't match");
        }
        
        return StringsMatch;
    }
    
    uint64_t    UTF32String_FindSubstring(UTF32String String, uint64_t StringSize, UTF32String SubString, uint64_t SubStringSize) {
        uint64_t   Offset             = 0ULL;
        uint64_t   CodePoint          = 0ULL;
        uint64_t   SubStringCodePoint = 0ULL;
        
        if (String != NULL && SubString != NULL && SubStringSize < StringSize) {
            while (String[CodePoint] == SubString[SubStringCodePoint] && CodePoint <= StringSize && SubStringCodePoint <= SubStringSize) {
                CodePoint                += 1;
                SubStringCodePoint       += 1;
            }
            
            if (CodePoint == StringSize) {
                Offset = CodePoint;
            }
        } else if (String == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        } else if (SubString == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "SubString Pointer is NULL");
        } else if (SubStringSize >= StringSize) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "SubString is bigger than or equal to String");
        }
        
        return Offset;
    }
    
    UTF32String UTF32String_Extract(UTF32String String, uint64_t StringSize, uint64_t Start, uint64_t End) {// Example: ~/Desktop/ElephantsDream_%05d.png, we're extracting %05d, Start = 26, End = 29, Size = 4
        uint64_t    ExtractedStringSize = (Start - End) + 1 + UnicodeNULLStringSize;
        UTF32String ExtractedString     = NULL;
        if (String != NULL && Start <= StringSize && End <= StringSize && End > Start) {
            // the extracted strings size is End - Start
            ExtractedString             = calloc(ExtractedStringSize, sizeof(uint32_t));
            if (ExtractedString != NULL) {
                // Start copying the codepoints
                for (uint64_t CodePoint = Start; CodePoint < End; CodePoint++) {
                    ExtractedString[CodePoint - Start] = String[CodePoint];
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate ExtractedString");
            }
        } else if (String == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "String Pointer is NULL");
        } else if (Start > StringSize) {
            // You can't copy past the end of the string
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Start is after the end of the string");
        } else if (End > StringSize) {
            // You can't copy past the end of the string
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "End is after the end of the string");
        } else if (End < Start) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "End is before Start");
        }
        return ExtractedString;
    }
    
    int64_t      UTF32String_ToNumber(UTF32String String, uint64_t StringSize) {
        uint64_t CodePoint  = 0ULL;
        int8_t   Sign       =  1;
        uint8_t  Base       = 10;
        int64_t  Value      =  0;
        
        for (uint8_t WhiteSpace = 0; WhiteSpace < WhitespaceTableSize; WhiteSpace++) {
            if (String[CodePoint] == WhitespaceTable[WhiteSpace]) {
                CodePoint += 1;
            }
        }
        
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
        
        while (CodePoint < StringSize) {
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
        }
        return Sign < 0 ? Value * Sign : Value;
    }
    
