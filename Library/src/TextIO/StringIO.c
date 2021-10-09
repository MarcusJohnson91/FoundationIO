#include "../../include/TextIO/StringIO.h"             /* Included for our declarations */
#include "../../include/FileIO.h"                      /* Included for File operations */
#include "../../include/MathIO.h"                      /* Included for endian swapping */
#include "../../include/TextIO/LogIO.h"                /* Included for error logging */
#include "../../include/TextIO/Private/TextIOTables.h" /* Included for the Unicode tables */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static UTF8  StringIO_PreallocateCodePoint_UTF8[UTF8MaxCodeUnitsInCodePoint   + PlatformIO_NULLTerminatorSize] = {0, 0, 0, 0, 0};
    static UTF16 StringIO_PreallocateCodePoint_UTF16[UTF16MaxCodeUnitsInCodePoint + PlatformIO_NULLTerminatorSize] = {0, 0, 0};
    
    UTF8 *UTF8_Init(uint64_t NumCodeUnits) {
        UTF8 *String            = NULL;
        if (NumCodeUnits >= 1) {
            uint64_t StringSize = NumCodeUnits + PlatformIO_NULLTerminatorSize;
            String              = (UTF8*) calloc(StringSize, sizeof(UTF8));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            for (uint64_t Index = 0ULL; Index < NumCodeUnits; Index++) {
                String[Index]   = 0x38;
            }
#endif
            if (String == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Allocation failure: Couldn't allocate %llu bytes"), StringSize * sizeof(UTF8));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid number of CodeUnits to allocate: %llu"), NumCodeUnits);
        }
        return String;
    }
    
    UTF16 *UTF16_Init(uint64_t NumCodeUnits) {
        UTF16 *String           = NULL;
        if (NumCodeUnits >= 1) {
            uint64_t StringSize = NumCodeUnits + PlatformIO_NULLTerminatorSize;
            String              = (UTF16*) calloc(StringSize, sizeof(UTF16));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            for (uint64_t Index = 0ULL; Index < NumCodeUnits; Index++) {
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrder_BE)
                String[Index]   = 0x3136;
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrder_LE)
                String[Index]   = 0x3631;
#endif /* Byte Order */
            }
#endif
            if (String == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Allocation failure: Couldn't allocate %llu bytes"), StringSize * sizeof(UTF16));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid number of CodeUnits to allocate: %llu"), NumCodeUnits);
        }
        return String;
    }
    
    UTF32 *UTF32_Init(uint64_t NumCodePoints) {
        UTF32 *String           = NULL;
        if (NumCodePoints >= 1) {
            uint64_t StringSize = NumCodePoints + PlatformIO_NULLTerminatorSize;
            String              = (UTF32*) calloc(StringSize, sizeof(UTF32));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            for (uint64_t Index = 0ULL; Index < NumCodePoints; Index++) {
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrder_BE)
                String[Index]   = 0x3332;
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrder_LE)
                String[Index]   = 0x3233;
#endif /* Byte Order */
            }
#endif
            if (String == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Allocation failure: Couldn't allocate %llu bytes"), StringSize * sizeof(UTF32));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid number of CodePoints to allocate: %llu"), NumCodePoints);
        }
        return String;
    }
    
    void UTF8_Set(UTF8 *String, UTF8 Value, uint64_t NumCodeUnits) {
        if (String != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < NumCodeUnits; CodeUnit++) {
                String[CodeUnit] = Value;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void UTF16_Set(UTF16 *String, UTF16 Value, uint64_t NumCodeUnits) {
        if (String != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < NumCodeUnits; CodeUnit++) {
                String[CodeUnit] = Value;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void UTF32_Set(UTF32 *String, UTF32 Value, uint64_t NumCodePoints) {
        if (String != NULL) {
            for (uint64_t CodePoint = 0ULL; CodePoint < NumCodePoints; CodePoint++) {
                String[CodePoint] = Value;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
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
        if (CodeUnit < UTF16HighSurrogateStart || CodeUnit > UTF16LowSurrogateEnd) {
            CodePointSize     = 1;
        } else if (CodeUnit >= UTF16HighSurrogateStart && CodeUnit <= UTF16LowSurrogateEnd) {
            CodePointSize     = 2;
        } else {
            CodePointSize     = 0;
        }
        return CodePointSize;
    }
    
    uint8_t UTF32_GetCodePointSizeInUTF8CodeUnits(UTF32 CodePoint) {
        uint8_t UTF8CodeUnits = 0;
        if (CodePoint <= 0x7F) {
            UTF8CodeUnits     = 1;
        } else if (CodePoint >= 0x80 && CodePoint <= 0x7FF) {
            UTF8CodeUnits     = 2;
        } else if (CodePoint >= 0x800 && CodePoint <= 0xFFFF) {
            UTF8CodeUnits     = 3;
        } else if (CodePoint >= 0x10000 && CodePoint <= UnicodeMaxCodePoint) {
            UTF8CodeUnits     = 4;
        }
        return UTF8CodeUnits;
    }
    
    uint8_t UTF32_GetCodePointSizeInUTF16CodeUnits(UTF32 CodePoint) {
        uint8_t UTF16CodeUnits = 0;
        if (CodePoint <= 0xFFFF) {
            UTF16CodeUnits     = 1;
        } else if (CodePoint >= 0x10000 && CodePoint <= UnicodeMaxCodePoint) {
            UTF16CodeUnits     = 2;
        }
        return UTF16CodeUnits;
    }
    
    UTF32 UTF8_DecodeCodePoint(ImmutableString_UTF8 CodeUnits) {
        UTF32 CodePoint                       = 0;
        if (CodeUnits != NULL) {
            uint8_t CodePointSize             = UTF8_GetCodePointSizeInCodeUnits(CodeUnits[0]);
            switch (CodePointSize) {
                case 1:
                    CodePoint                 =  CodeUnits[0] & 0x7F;
                    break;
                case 2:
                    CodePoint                |= (CodeUnits[0] & 0x1F) << 6;
                    if (CodeUnits[1] != PlatformIO_NULLTerminator) {
                        CodePoint            |= (CodeUnits[1] & 0x3F) << 0;
                    }
                    break;
                case 3:
                    CodePoint                |= (CodeUnits[0] & 0x0F) << 12;
                    if (CodeUnits[1] != PlatformIO_NULLTerminator) {
                        CodePoint            |= (CodeUnits[1] & 0x1F) << 6;
                    }
                    if (CodeUnits[2] != PlatformIO_NULLTerminator) {
                        CodePoint            |= (CodeUnits[2] & 0x1F) << 0;
                    }
                    break;
                case 4:
                    CodePoint                |= (CodeUnits[0] & 0x07) << 18;
                    if (CodeUnits[1] != PlatformIO_NULLTerminator) {
                        CodePoint            |= (CodeUnits[1] & 0x3F) << 12;
                    }
                    if (CodeUnits[2] != PlatformIO_NULLTerminator) {
                        CodePoint            |= (CodeUnits[2] & 0x3F) <<  6;
                    }
                    if (CodeUnits[3] != PlatformIO_NULLTerminator) {
                        CodePoint            |= (CodeUnits[3] & 0x3F) <<  0;
                    }
                    break;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodeUnits Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF32 UTF16_DecodeCodePoint(ImmutableString_UTF16 CodeUnits) {
        UTF32 CodePoint                       = 0;
        if (CodeUnits != NULL) {
            uint8_t CodePointSize             = UTF16_GetCodePointSizeInCodeUnits(CodeUnits[0]);
            switch (CodePointSize) {
                case 1:
                    CodePoint                 =  CodeUnits[0];
                    break;
                case 2:
                    CodePoint                |= UTF16MaxCodeUnitValue + 1;
                    CodePoint                |= (CodeUnits[0] & UTF16SurrogateMask) << UTF16SurrogateShift;
                    if (CodeUnits[1] != PlatformIO_NULLTerminator) {
                        CodePoint            |= (CodeUnits[1] & UTF16SurrogateMask);
                    }
                    break;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodeUnits Pointer is NULL"));
        }
        return CodePoint;
    }
    
    static UTF16 *UTF16_EncodeCodePoint(UTF32 CodePoint) {
        uint8_t CodeUnitSize                           = UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint);
        UTF32   Ranged                                 = CodePoint + UTF16SurrogatePairStart;
        switch (CodeUnitSize) {
            case 1:
                StringIO_PreallocateCodePoint_UTF16[0] = (CodePoint & 0xFFFF);
                break;
            case 2:
                StringIO_PreallocateCodePoint_UTF16[0] = UTF16HighSurrogateStart + (Ranged & (UTF16SurrogateMask << UTF16SurrogateShift) >> UTF16SurrogateShift);
                StringIO_PreallocateCodePoint_UTF16[1] = UTF16LowSurrogateStart  + (Ranged & UTF16SurrogateMask);
                break;
        }
        return StringIO_PreallocateCodePoint_UTF16;
    }
    
    static UTF8 *UTF8_EncodeCodePoint(UTF32 CodePoint) {
        uint8_t CodeUnitSize                          = UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint);
        switch (CodeUnitSize) {
            case 1:
                StringIO_PreallocateCodePoint_UTF8[0] = (CodePoint & 0x7F);
                break;
            case 2:
                StringIO_PreallocateCodePoint_UTF8[0] = 0xC0 | (CodePoint & 0x7C0) >> 6;
                StringIO_PreallocateCodePoint_UTF8[1] = 0x80 | (CodePoint & 0x3F);
                break;
            case 3:
                StringIO_PreallocateCodePoint_UTF8[0] = 0xE0 | (CodePoint & 0xF000) >> 12;
                StringIO_PreallocateCodePoint_UTF8[1] = 0x80 | (CodePoint & 0xFC0)  >> 6;
                StringIO_PreallocateCodePoint_UTF8[2] = 0x80 | (CodePoint & 0x3F);
                break;
            case 4:
                StringIO_PreallocateCodePoint_UTF8[0] = 0xF0 | (CodePoint & 0x1C0000) >> 18;
                StringIO_PreallocateCodePoint_UTF8[1] = 0x80 | (CodePoint & 0x3F000)  >> 12;
                StringIO_PreallocateCodePoint_UTF8[2] = 0x80 | (CodePoint & 0xFC0)    >> 6;
                StringIO_PreallocateCodePoint_UTF8[3] = 0x80 | (CodePoint & 0x3F);
                break;
        }
        return StringIO_PreallocateCodePoint_UTF8;
    }
    
    uint64_t UTF8_GetGraphemeSizeInCodeUnits(UTF8 *String, uint64_t OffsetInCodeUnits) {
        uint64_t GraphemeSize         = 1ULL;
        if (String != NULL) {
            uint64_t CodeUnit         = OffsetInCodeUnits;
            while (String[CodeUnit] != PlatformIO_NULLTerminator) {
                UTF32 CodePoint1      = UTF8_DecodeCodePoint(&String[CodeUnit]);
                for (uint64_t GraphemeExt = 0ULL; GraphemeExt < GraphemeExtensionTableSize; GraphemeExt++) {
                    if (CodePoint1 == GraphemeExtensionTable[GraphemeExt]) {
                        GraphemeSize += 1;
                        CodeUnit     += 1;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return GraphemeSize;
    }
    
    uint64_t UTF16_GetGraphemeSizeInCodeUnits(UTF16 *String, uint64_t OffsetInCodeUnits) {
        uint64_t GraphemeSize         = 1ULL;
        if (String != NULL) {
            uint64_t CodeUnit         = OffsetInCodeUnits;
            UTF16   *CodeUnits        = UTF16_Init(UTF16MaxCodeUnitsInCodePoint);
            while (String[CodeUnit] != PlatformIO_NULLTerminator) {
                UTF16 Byte            = String[CodeUnit];
                uint8_t CodePointSize = UTF16_GetCodePointSizeInCodeUnits(Byte);
                for (uint8_t Index = 0; Index < CodePointSize; Index++) {
                    CodeUnits[Index]  = String[CodeUnit + Index];
                }
                UTF32 CodePoint1      = UTF16_DecodeCodePoint(CodeUnits);
                for (uint64_t GraphemeExt = 0ULL; GraphemeExt < GraphemeExtensionTableSize; GraphemeExt++) {
                    if (CodePoint1 == GraphemeExtensionTable[GraphemeExt]) {
                        GraphemeSize += 1;
                        CodeUnit     += 1;
                    }
                }
            }
            free(CodeUnits);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return GraphemeSize;
    }
    
    uint64_t UTF32_GetGraphemeSizeInCodePoints(UTF32 *String, uint64_t OffsetInCodePoints) {
        uint64_t NumCodePoints         = 1ULL;
        if (String != NULL) {
            uint64_t CodePoint         = OffsetInCodePoints;
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                for (uint64_t GraphemeExt = 0ULL; GraphemeExt < GraphemeExtensionTableSize; GraphemeExt++) {
                    while (String[CodePoint] == GraphemeExtensionTable[GraphemeExt]) {
                        NumCodePoints += 1;
                        CodePoint     += 1;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF8_GetStringSizeInCodeUnits(ImmutableString_UTF8 String) {
        uint64_t StringSizeInCodeUnits = 0ULL;
        if (String != NULL) {
            while (String[StringSizeInCodeUnits] != PlatformIO_NULLTerminator) {
                StringSizeInCodeUnits += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    uint64_t UTF16_GetStringSizeInCodeUnits(ImmutableString_UTF16 String) {
        uint64_t StringSizeInCodeUnits = 0ULL;
        if (String != NULL) {
            while (String[StringSizeInCodeUnits] != PlatformIO_NULLTerminator) {
                StringSizeInCodeUnits += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    static uint64_t UTF32_GetStringSizeInUTF8CodeUnits(ImmutableString_UTF32 String) {
        uint64_t CodePoint            = 0ULL;
        uint64_t UTF8CodeUnits        = 0ULL;
        if (String != NULL) {
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                UTF8CodeUnits        += UTF32_GetCodePointSizeInUTF8CodeUnits(String[CodePoint]);
                CodePoint            += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return UTF8CodeUnits;
    }
    
    static uint64_t UTF32_GetStringSizeInUTF16CodeUnits(ImmutableString_UTF32 String) {
        uint64_t CodePoint          = 0ULL;
        uint64_t UTF16CodeUnits     = 0ULL;
        if (String != NULL) {
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                UTF16CodeUnits       += UTF32_GetCodePointSizeInUTF16CodeUnits(String[CodePoint]);
                CodePoint            += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return UTF16CodeUnits;
    }
    
    uint64_t UTF8_GetStringSizeInCodePoints(ImmutableString_UTF8 String) {
        uint64_t StringSizeInCodePoints = 0ULL;
        uint64_t CodeUnit               = 0ULL;
        if (String != NULL) {
            while (String[CodeUnit] != PlatformIO_NULLTerminator) {
                StringSizeInCodePoints += 1;
                CodeUnit               += UTF8_GetCodePointSizeInCodeUnits(String[CodeUnit]);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringSizeInCodePoints;
    }
    
    uint64_t UTF16_GetStringSizeInCodePoints(ImmutableString_UTF16 String) {
        uint64_t NumCodePoints             = 0ULL;
        if (String != NULL) {
            uint64_t CodeUnit              = 0ULL;
            while (String[CodeUnit] != PlatformIO_NULLTerminator) {
                CodeUnit                  += UTF16_GetCodePointSizeInCodeUnits(String[CodeUnit]);
                NumCodePoints             += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF32_GetStringSizeInCodePoints(ImmutableString_UTF32 String) {
        uint64_t NumCodePoints = 0ULL;
        if (String != NULL) {
            while (String[NumCodePoints] != PlatformIO_NULLTerminator) {
                NumCodePoints += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF8_GetStringSizeInGraphemes(ImmutableString_UTF8 String) {
        uint64_t NumGraphemes  = 0ULL;
        if (String != NULL) {
            UTF32 *Decoded     = UTF8_Decode(String);
            NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
            UTF32_Deinit(Decoded);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    uint64_t UTF16_GetStringSizeInGraphemes(ImmutableString_UTF16 String) {
        uint64_t NumGraphemes  = 0ULL;
        if (String != NULL) {
            UTF32 *Decoded     = UTF16_Decode(String);
            NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
            UTF32_Deinit(Decoded);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    /*
     Grapheme Handling:
     IsGraphemeExtender
     if CodePoint 0 or CodePoint 1 is an extender, we need to loop again
     
     "e + ́" = 1 Grapheme, 2 CodePoints, 3 CodeUnits
     "1 e + ́ 2" = 3 Graphemes, 4 CodePoints, 4 CodeUnits
     
     So Grapheme Boundaries.
     if the first codepoint isn't a grapheme extender, check the next codepoint, if it is also not a grapheme extender, the first codepoint is it's own grapheme,
     then check the 3rd codepoint, if it is a grapheme extender, codepoint 2 and 3 are part of the same grapheme and all following codepoints remain part of the grapheme until you find a CodePoint that isn't a grapheme extender.
     */
    
    static bool UTF32_CodePointIsGraphemeExtender(UTF32 CodePoint) {
        bool IsGraphemeExtender    = No;
        if (CodePoint > 0x7F) {
            for (uint64_t GraphemeExtender = 0ULL; GraphemeExtender < GraphemeExtensionTableSize; GraphemeExtender++) {
                if (CodePoint == GraphemeExtensionTable[GraphemeExtender]) {
                    IsGraphemeExtender = Yes;
                    break;
                }
            }
        }
        return IsGraphemeExtender;
    }
    
    uint64_t UTF32_GetStringSizeInGraphemes(ImmutableString_UTF32 String) {
        uint64_t NumGraphemes          = 0ULL;
        uint64_t CodePoint             = 1ULL;
        if (String != NULL) {
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                bool CurrentIsExtender = UTF32_CodePointIsGraphemeExtender(String[CodePoint - 1]);
                bool NextIsExtender    = UTF32_CodePointIsGraphemeExtender(String[CodePoint]);
                if (CurrentIsExtender == No && NextIsExtender == No) {
                    // Both are individual Graphemes, add two
                    NumGraphemes      += 2;
                } else if (CurrentIsExtender == Yes && NextIsExtender == No) {
                    
                }
                
                else if (CurrentIsExtender == No && NextIsExtender == Yes) {
                    // Loop until we find a not extender, keep track of the CodePoints
                }
                CodePoint             += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    uint64_t UTF8_GetWordSizeInCodePoints(ImmutableString_UTF8 String, uint64_t OffsetInCodeUnits) {
        uint64_t WordSize = 0ULL;
        if (String != NULL) {
            
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return WordSize;
    }
    
    uint64_t UTF16_GetWordSizeInCodePoints(ImmutableString_UTF16 String, uint64_t OffsetInCodeUnits) {
        uint64_t WordSize = 0ULL;
        if (String != NULL) {
            
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return WordSize;
    }
    
    
    
    
    /*
     
     Break API:
     
     We have a string: "Hi, I'm a string!"
     
     What properties do we want to get from this string?
     
     The number of CodeUnits, CodePoints, Graphemes, Words, Lines/Sentences, Paragraphs?, and the number of CodeUnits, CodePoints, and Graphemes in each Word, Line/Sentence, Paragraph
     
     We can get the size of Words, Sentences, Paragraphs in CodeUnits, CodePoints, Graphemes with the normal functions, all we have to do is find the Size of the Word, Sentence, Paragraph first so it can be split or at least indexed.
     
     Should Words include punctuation, or should punctuation be it's own word?
     
     */
    
    uint64_t UTF32_GetWordSizeInCodePoints(ImmutableString_UTF32 String, uint64_t OffsetInCodeUnits) {
        uint64_t WordSize = 0ULL;
        if (String != NULL) {
            
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return WordSize;
    }
    
    static void UTF8_Clear_Preallocated(void) {
        StringIO_PreallocateCodePoint_UTF8[0] = 0;
        StringIO_PreallocateCodePoint_UTF8[1] = 0;
        StringIO_PreallocateCodePoint_UTF8[2] = 0;
        StringIO_PreallocateCodePoint_UTF8[3] = 0;
        StringIO_PreallocateCodePoint_UTF8[4] = 0;
    }
    
    static void UTF16_Clear_Preallocated(void) {
        StringIO_PreallocateCodePoint_UTF16[0] = 0;
        StringIO_PreallocateCodePoint_UTF16[1] = 0;
        StringIO_PreallocateCodePoint_UTF16[2] = 0;
    }
    
    UTF8 *UTF8_ExtractCodePoint(ImmutableString_UTF8 String) {
        UTF8 *CodeUnits                                   = NULL;
        if (String != NULL && String[0] != PlatformIO_NULLTerminator) {
            UTF8_Clear_Preallocated();
            uint8_t CodePointSize                         = UTF8_GetCodePointSizeInCodeUnits(String[0]);
            switch (CodePointSize) {
                case 1:
                    StringIO_PreallocateCodePoint_UTF8[0] = String[0];
                    CodeUnits                             = StringIO_PreallocateCodePoint_UTF8;
                    break;
                case 2:
                    StringIO_PreallocateCodePoint_UTF8[0] = String[0];
                    StringIO_PreallocateCodePoint_UTF8[1] = String[1];
                    CodeUnits                             = StringIO_PreallocateCodePoint_UTF8;
                    break;
                case 3:
                    StringIO_PreallocateCodePoint_UTF8[0] = String[0];
                    StringIO_PreallocateCodePoint_UTF8[1] = String[1];
                    StringIO_PreallocateCodePoint_UTF8[2] = String[2];
                    CodeUnits                             = StringIO_PreallocateCodePoint_UTF8;
                    break;
                case 4:
                    StringIO_PreallocateCodePoint_UTF8[0] = String[0];
                    StringIO_PreallocateCodePoint_UTF8[1] = String[1];
                    StringIO_PreallocateCodePoint_UTF8[2] = String[2];
                    StringIO_PreallocateCodePoint_UTF8[3] = String[3];
                    CodeUnits                             = StringIO_PreallocateCodePoint_UTF8;
                    break;
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CodeUnits;
    }
    
    UTF16 *UTF16_ExtractCodePoint(ImmutableString_UTF16 String) {
        UTF16 *CodeUnits                               = NULL;
        if (String != NULL && String[0] != PlatformIO_NULLTerminator) {
            UTF16_Clear_Preallocated();
            if (String[0] < UTF16HighSurrogateStart || String[0] > UTF16LowSurrogateEnd) {
                StringIO_PreallocateCodePoint_UTF16[0] = String[0];
                CodeUnits                              = StringIO_PreallocateCodePoint_UTF16;
            } else {
                StringIO_PreallocateCodePoint_UTF16[0] = String[0];
                StringIO_PreallocateCodePoint_UTF16[1] = String[1];
                CodeUnits                              = StringIO_PreallocateCodePoint_UTF16;
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CodeUnits;
    }
    
    UTF32 UTF32_ExtractCodePoint(ImmutableString_UTF32 String) {
        UTF32 CodePoint         = 0;
        if (String != NULL && String[0] != PlatformIO_NULLTerminator) {
            CodePoint           = String[0];
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF8 *UTF8_ExtractGrapheme(ImmutableString_UTF8 String, uint64_t Grapheme2Extract) {
        UTF8 *Grapheme        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, Grapheme2Extract);
            free(String32);
            Grapheme          = UTF8_Encode(Grapheme32);
            free(Grapheme32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF16 *UTF16_ExtractGrapheme(ImmutableString_UTF16 String, uint64_t Grapheme2Extract) {
        UTF16 *Grapheme        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, Grapheme2Extract);
            free(String32);
            Grapheme          = UTF16_Encode(Grapheme32);
            free(Grapheme32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF32 *UTF32_ExtractGrapheme(ImmutableString_UTF32 String, uint64_t GraphemeIndex) {
        UTF32 *Grapheme                    = NULL;
        if (String != NULL) {
            /*
             Loop over the string with a while loop
             extracting each pair of codepoints and finding grapheme boundaries
             */
            UTF32    CodePoint1   = 0;
            UTF32    CodePoint2   = 0;
            uint64_t CodePoint    = 0;
            uint64_t GraphemeSize = 0;
            while (String[CodePoint] != PlatformIO_NULLTerminator && String[CodePoint + 1] != PlatformIO_NULLTerminator) {
                CodePoint1       = UTF32_ExtractCodePoint(&String[CodePoint]);
                CodePoint2       = UTF32_ExtractCodePoint(&String[CodePoint + 1]);
                // Now look up the codepoints to see if they're grapheme extenders
                bool IsExtender1 = UTF32_CodePointIsGraphemeExtender(CodePoint1);
                bool IsExtender2 = UTF32_CodePointIsGraphemeExtender(CodePoint2);
                if (IsExtender1 == Yes && IsExtender2 == No) {
                    // Grapheme Boundary; CodePoint1 is part of the previous boundary; CodePoint2 is part of the next Grapheme
                } else if (IsExtender1 == No && IsExtender2 == Yes) {
                    // Grapheme Boundary, CodePoint1 and CodePoint2 are part of a new Grapheme
                } else if (IsExtender1 == Yes && IsExtender2 == Yes) {
                    // Both codepoints are part of a previous grapheme
                } else if (IsExtender1 == No && IsExtender2 == No) {
                    // 2 graphemes here
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    bool UTF8_HasBOM(ImmutableString_UTF8 String) {
        bool StringHasABOM        = No;
        if (String != NULL) {
            uint64_t StringSize   = UTF8_GetStringSizeInCodeUnits(String);
            if (StringSize >= 3) {
                if (String[0] == UTF8BOM_1 && String[1] == UTF8BOM_2 && String[2] == UTF8BOM_3) {
                    StringHasABOM = Yes;
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool UTF16_HasBOM(ImmutableString_UTF16 String) {
        bool StringHasABOM    = No;
        if (String != NULL) {
            if (String[0] == UTF16BOM_LE || String[0] == UTF16BOM_BE) {
                StringHasABOM = Yes;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool UTF32_HasBOM(ImmutableString_UTF32 String) {
        bool StringHasABOM    = No;
        if (String != NULL) {
            if (String[0] == UTF32BOM_LE || String[0] == UTF32BOM_BE) {
                StringHasABOM = Yes;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringHasABOM;
    }
    
    bool UTF8_IsWordBreak(const UTF8 *CodePoint) {
        bool IsWordBreak = false;
        UTF32 *Decoded = UTF8_Decode(CodePoint);
        for (uint8_t TableCodePoint = 0; TableCodePoint < WordBreakTableSize; TableCodePoint++) {
            if (Decoded[0] == WordBreakTable[TableCodePoint]) {
                IsWordBreak = true;
            }
        }
        return IsWordBreak;
    }
    
    bool UTF16_IsWordBreak(const UTF16 CodePoint) {
        bool IsWordBreak = false;
        for (uint8_t TableCodePoint = 0; TableCodePoint < WordBreakTableSize; TableCodePoint++) {
            if (CodePoint == WordBreakTable[TableCodePoint]) {
                IsWordBreak = true;
            }
        }
        return IsWordBreak;
    }
    
    bool UTF32_IsWordBreak(const UTF32 CodePoint) {
        bool IsWordBreak = false;
        for (uint8_t TableCodePoint = 0; TableCodePoint < WordBreakTableSize; TableCodePoint++) {
            if (CodePoint == WordBreakTable[TableCodePoint]) {
                IsWordBreak = true;
            }
        }
        return IsWordBreak;
    }
  
    bool UTF8_String2Bool(UTF8 *String) {
        bool   Boolean  = false;
        size_t CodeUnit = 0;
        while (String[CodeUnit] != PlatformIO_NULLTerminator) {
            if (String[CodeUnit] == '1') {
                Boolean = true;
                break;
            } else if (String[CodeUnit] == '0') {
                Boolean = false;
                break;
            } else if (String[CodeUnit] == 'N' || String[CodeUnit] == 'n') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                    Boolean = true;
                    break;
                }
            } else if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'N' || String[CodeUnit] == 'n') {
                    Boolean = true;
                    break;
                }
            } else if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                        Boolean = true;
                        break;
                    }
                }
            } else if (String[CodeUnit] == 'Y' || String[CodeUnit] == 'y') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'S' || String[CodeUnit] == 's') {
                        Boolean = true;
                        break;
                    }
                }
            } else if (String[CodeUnit] == 'T' || String[CodeUnit] == 't') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'R' || String[CodeUnit] == 'r') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'U' || String[CodeUnit] == 'u') {
                        CodeUnit += 1;
                        if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                            Boolean = true;
                            break;
                        }
                    }
                }
            } else if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'A' || String[CodeUnit] == 'a') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'L' || String[CodeUnit] == 'l') {
                        CodeUnit += 1;
                        if (String[CodeUnit] == 'S' || String[CodeUnit] == 's') {
                            CodeUnit += 1;
                            if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                                CodeUnit += 1;
                            }
                        }
                    }
                }
            }
        }
        return Boolean;
    }
    
    bool UTF16_String2Bool(UTF16 *String) {
        bool   Boolean  = false;
        size_t CodeUnit = 0;
        while (String[CodeUnit] != PlatformIO_NULLTerminator) {
            if (String[CodeUnit] == '1') {
                Boolean = true;
                break;
            } else if (String[CodeUnit] == '0') {
                Boolean = false;
                break;
            } else if (String[CodeUnit] == 'N' || String[CodeUnit] == 'n') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                    Boolean = true;
                    break;
                }
            } else if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'N' || String[CodeUnit] == 'n') {
                    Boolean = true;
                    break;
                }
            } else if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                        Boolean = true;
                        break;
                    }
                }
            } else if (String[CodeUnit] == 'Y' || String[CodeUnit] == 'y') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'S' || String[CodeUnit] == 's') {
                        Boolean = true;
                        break;
                    }
                }
            } else if (String[CodeUnit] == 'T' || String[CodeUnit] == 't') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'R' || String[CodeUnit] == 'r') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'U' || String[CodeUnit] == 'u') {
                        CodeUnit += 1;
                        if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                            Boolean = true;
                            break;
                        }
                    }
                }
            } else if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'A' || String[CodeUnit] == 'a') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'L' || String[CodeUnit] == 'l') {
                        CodeUnit += 1;
                        if (String[CodeUnit] == 'S' || String[CodeUnit] == 's') {
                            CodeUnit += 1;
                            if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                                CodeUnit += 1;
                            }
                        }
                    }
                }
            }
        }
        return Boolean;
    }
    
    bool UTF32_String2Bool(UTF32 *String) {
        bool   Boolean  = false;
        size_t CodeUnit = 0;
        while (String[CodeUnit] != PlatformIO_NULLTerminator) {
            if (String[CodeUnit] == '1') {
                Boolean = true;
                break;
            } else if (String[CodeUnit] == '0') {
                Boolean = false;
                break;
            } else if (String[CodeUnit] == 'N' || String[CodeUnit] == 'n') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                    Boolean = true;
                    break;
                }
            } else if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'N' || String[CodeUnit] == 'n') {
                    Boolean = true;
                    break;
                }
            } else if (String[CodeUnit] == 'O' || String[CodeUnit] == 'o') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                        Boolean = true;
                        break;
                    }
                }
            } else if (String[CodeUnit] == 'Y' || String[CodeUnit] == 'y') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'S' || String[CodeUnit] == 's') {
                        Boolean = true;
                        break;
                    }
                }
            } else if (String[CodeUnit] == 'T' || String[CodeUnit] == 't') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'R' || String[CodeUnit] == 'r') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'U' || String[CodeUnit] == 'u') {
                        CodeUnit += 1;
                        if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                            Boolean = true;
                            break;
                        }
                    }
                }
            } else if (String[CodeUnit] == 'F' || String[CodeUnit] == 'f') {
                CodeUnit += 1;
                if (String[CodeUnit] == 'A' || String[CodeUnit] == 'a') {
                    CodeUnit += 1;
                    if (String[CodeUnit] == 'L' || String[CodeUnit] == 'l') {
                        CodeUnit += 1;
                        if (String[CodeUnit] == 'S' || String[CodeUnit] == 's') {
                            CodeUnit += 1;
                            if (String[CodeUnit] == 'E' || String[CodeUnit] == 'e') {
                                CodeUnit += 1;
                            }
                        }
                    }
                }
            }
        }
        return Boolean;
    }
    
    bool UTF8_IsUNCPath(ImmutableString_UTF8 String) {
        bool StringIsUNCPath = No;
        if (String != NULL) {
            uint64_t StringSize     = UTF8_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF8_HasBOM(String);
                if (StringHasBOM == Yes && StringSize >= UTF8BOMSizeInCodeUnits + UnicodeUNCPathPrefixSize) {
                    uint8_t CodeUnit   = UTF8BOMSizeInCodeUnits + 1;
                    uint8_t PrefixByte = 0;
                    if (String[CodeUnit] == PlatformIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == UTF8BOMSizeInCodeUnits + 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    uint8_t CodeUnit   = 0;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == PlatformIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringIsUNCPath;
    }
    
    bool UTF16_IsUNCPath(ImmutableString_UTF16 String) {
        bool StringIsUNCPath = No;
        if (String != NULL) {
            uint64_t StringSize     = UTF16_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF16_HasBOM(String);
                if (StringHasBOM == Yes && StringSize >= UTF16BOMSizeInCodeUnits + UnicodeUNCPathPrefixSize) {
                    uint8_t CodeUnit   = UTF16BOMSizeInCodeUnits + 1;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == PlatformIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == UTF16BOMSizeInCodeUnits + 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    uint8_t CodeUnit   = 0;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == PlatformIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringIsUNCPath;
    }
    
    bool UTF32_IsUNCPath(ImmutableString_UTF32 String) {
        bool StringIsUNCPath = No;
        if (String != NULL) {
            uint64_t StringSize     = UTF32_GetStringSizeInCodePoints(String);
            
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF32_HasBOM(String);
                if (StringHasBOM == Yes && StringSize >= UnicodeBOMSizeInCodePoints + UnicodeUNCPathPrefixSize) {
                    uint8_t CodeUnit   = UnicodeBOMSizeInCodePoints + 1;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == PlatformIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == UnicodeBOMSizeInCodePoints + 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    uint8_t CodeUnit   = 0;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == PlatformIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringIsUNCPath;
    }
    
    bool UTF8_IsAbsolutePath(ImmutableString_UTF8 String) {
        bool PathIsAbsolute = No;
        if (String != NULL) {
            bool StringHasBOM      = UTF8_HasBOM(String);
#if  ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            if (StringHasBOM && String[UTF8BOMSizeInCodeUnits] != PlatformIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            } else if (String[UTF8BOMSizeInCodeUnits] != PlatformIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            }
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            if (StringHasBOM && String[0] != 0 && String[1] != 0 && String[2] != 0 && String[3] != 0 && String[4] != 0) {
                if (((String[3] >= 'A' && String[3] <= 'Z') || (String[3] >= 'a' && String[3] <= 'z')) && String[4] == ':') {
                    PathIsAbsolute = Yes;
                }
            } else if (String[0] != 0 && String[1] != 0 && ((String[0] >= 'A' && String[0] <= 'Z') || (String[0] >= 'a' && String[0] <= 'z')) && String[1] == ':') {
                PathIsAbsolute     = Yes;
            }
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return PathIsAbsolute;
    }
    
    bool UTF16_IsAbsolutePath(ImmutableString_UTF16 String) {
        bool PathIsAbsolute        = No;
        if (String != NULL) {
            bool StringHasBOM      = UTF16_HasBOM(String);
#if  ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            if (StringHasBOM && String[UTF8BOMSizeInCodeUnits] != PlatformIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            } else if (String[UTF8BOMSizeInCodeUnits] != PlatformIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            }
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            if (StringHasBOM && String[0] != 0 && String[1] != 0 && String[2] != 0 && String[3] != 0 && String[4] != 0) {
                if (((String[3] >= 'A' && String[3] <= 'Z') || (String[3] >= 'a' && String[3] <= 'z')) && String[4] == ':') {
                    PathIsAbsolute = Yes;
                }
            } else if (String[0] != 0 && String[1] != 0 && ((String[0] >= 'A' && String[0] <= 'Z') || (String[0] >= 'a' && String[0] <= 'z')) && String[1] == ':') {
                PathIsAbsolute     = Yes;
            }
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return PathIsAbsolute;
    }
    
    bool UTF32_IsAbsolutePath(ImmutableString_UTF32 String) {
        bool PathIsAbsolute        = No;
        if (String != NULL) {
            bool StringHasBOM      = UTF32_HasBOM(String);
#if  ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            if (StringHasBOM && String[UTF8BOMSizeInCodeUnits] != PlatformIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            } else if (String[UTF8BOMSizeInCodeUnits] != PlatformIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            }
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            if (StringHasBOM && String[0] != 0 && String[1] != 0 && String[2] != 0 && String[3] != 0 && String[4] != 0) {
                if (((String[3] >= 'A' && String[3] <= 'Z') || (String[3] >= 'a' && String[3] <= 'z')) && String[4] == ':') {
                    PathIsAbsolute = Yes;
                }
            } else if (String[0] != 0 && String[1] != 0 && ((String[0] >= 'A' && String[0] <= 'Z') || (String[0] >= 'a' && String[0] <= 'z')) && String[1] == ':') {
                PathIsAbsolute     = Yes;
            }
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return PathIsAbsolute;
    }
    
    bool UTF8_HasNewLine(ImmutableString_UTF8 String) {
        bool StringHasNewLine = No;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            StringHasNewLine  = UTF32_HasNewLine(String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringHasNewLine;
    }
    
    bool UTF16_HasNewLine(ImmutableString_UTF16 String) {
        bool StringHasNewLine = No;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            StringHasNewLine  = UTF32_HasNewLine(String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringHasNewLine;
    }
    
    bool UTF32_HasNewLine(ImmutableString_UTF32 String) {
        bool StringHasNewLine            = No;
        if (String != NULL) {
            uint64_t CodePoint           = 1ULL;
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                if (String[CodePoint] == 0x0A) {
                    StringHasNewLine     = Yes;
                }
                if (String[CodePoint - 1] == 0x0D && String[CodePoint] == 0x0A) {
                    StringHasNewLine     = Yes;
                }
                if (String[CodePoint] == 0x0D) {
                    StringHasNewLine     = Yes;
                }
                CodePoint               += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringHasNewLine;
    }
    
    bool UTF32_IsUpperCase(UTF32 CodePoint) {
        bool CodePointIsUppercase            = No;
        if (CodePoint != PlatformIO_NULLTerminator) {
            if ((CodePoint >= 0x41 && CodePoint <= 0x5A) || (CodePoint >= 0xC0 && CodePoint <= 0xD6) || (CodePoint >= 0xD8 && CodePoint <= 0xDE)) {
                CodePointIsUppercase         = Yes;
            } else {
                uint64_t UppercaseIndex      = 0xDE + 1;
                while (UppercaseIndex < CaseFoldTableSize) {
                    if (CodePoint == CaseFoldTable[UppercaseIndex][0][0]) {
                        CodePointIsUppercase = Yes;
                        break;
                    }
                    UppercaseIndex          += 1;
                }
            }
        }
        return CodePointIsUppercase;
    }
    
    bool UTF8_IsValid(ImmutableString_UTF8 String) {
        uint64_t CodeUnit    = 0ULL;
        bool     IsValidUTF8 = Yes;
        if (String != NULL) {
            while (String[CodeUnit] != PlatformIO_NULLTerminator) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return IsValidUTF8;
    }
    
    bool UTF16_IsValid(ImmutableString_UTF16 String) {
        uint64_t CodeUnit             = 1ULL;
        bool     IsValidUTF16         = Yes;
        if (String != NULL) {
            while (String[CodeUnit] != PlatformIO_NULLTerminator) {
                if ((String[CodeUnit - 1] >= UTF16HighSurrogateStart && String[CodeUnit - 1] <= UTF16HighSurrogateEnd) && (String[CodeUnit] <= UTF16LowSurrogateStart && String[CodeUnit] >= UTF16LowSurrogateEnd)) {
                    IsValidUTF16 = No;
                    break;
                }
                CodeUnit += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return IsValidUTF16;
    }
    
    bool UTF32_IsValid(ImmutableString_UTF32 String) {
        uint64_t CodePoint       = 0ULL;
        bool     IsValidUTF32    = Yes;
        if (String != NULL) {
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                if (String[CodePoint] >= UnicodeMaxCodePoint || (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd)) {
                    IsValidUTF32 = No;
                    break;
                }
                CodePoint       += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return IsValidUTF32;
    }
    
    UTF8 *UTF8_AddBOM(ImmutableString_UTF8 String) {
        UTF8 *StringWithBOM = NULL;
        if (String != NULL) {
            UTF32 *String32  = UTF8_Decode(String);
            UTF32 *BOMAdded  = UTF32_AddBOM(String32, StringIO_BOM_Big);
            free(String32);
            StringWithBOM    = UTF8_Encode(BOMAdded);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF16 *UTF16_AddBOM(ImmutableString_UTF16 String, StringIO_BOMs BOM2Add) {
        UTF16 *StringWithBOM = NULL;
        if (String != NULL) {
            UTF32 *String32  = UTF16_Decode(String);
            UTF32 *BOMAdded  = UTF32_AddBOM(String32, BOM2Add);
            StringWithBOM    = UTF16_Encode(BOMAdded);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF32 *UTF32_AddBOM(ImmutableString_UTF32 String, StringIO_BOMs BOM2Add) {
        UTF32   *StringWithBOM        = NULL;
        UTF32    ByteOrder            = 0;
        if (String != NULL) {
            if (String[0] != UTF32BOM_LE && String[0] != UTF32BOM_BE) {
                uint64_t StringSize   = UTF32_GetStringSizeInCodePoints(String);
                StringWithBOM         = UTF32_Init(StringSize + UnicodeBOMSizeInCodePoints);
                if (StringWithBOM != NULL) {
                    if (BOM2Add == StringIO_BOM_Native) {
#if   (PlatformIO_TargetByteOrder == PlatformIO_ByteOrder_LE)
                        ByteOrder     = UTF32BOM_LE;
#elif (PlatformIO_TargetByteOrder == PlatformIO_ByteOrder_BE)
                        ByteOrder     = UTF32BOM_BE;
#endif
                    } else if (BOM2Add == StringIO_BOM_Little) {
                        ByteOrder     = UTF32BOM_LE;
                    } else if (BOM2Add == StringIO_BOM_Big) {
                        ByteOrder     = UTF32BOM_BE;
                    }
                    StringWithBOM[0] = ByteOrder;
                    for (uint64_t CodePoint = 1ULL; CodePoint < StringSize; CodePoint++) {
                        StringWithBOM[CodePoint] = String[CodePoint + 1];
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringWithBOM couldn't be allocated"));
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringWithBOM;
    }
    
    UTF8 *UTF8_RemoveBOM(ImmutableString_UTF8 String) {
        UTF8    *BOMLessString                      = NULL;
        uint64_t StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF8_GetStringSizeInCodeUnits(String);
            bool StringHasBOM                       = UTF8_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                       = UTF8_Init(StringSize - UTF8BOMSizeInCodeUnits);
                if (BOMLessString != NULL) {
                    for (uint64_t CodeUnit = 2ULL; CodeUnit < StringSize; CodeUnit++) {
                        BOMLessString[CodeUnit - 3] = String[CodeUnit];
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate BOMLessString"));
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    
    UTF16 *UTF16_RemoveBOM(ImmutableString_UTF16 String) {
        UTF16   *BOMLessString                      = NULL;
        uint64_t StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF16_GetStringSizeInCodeUnits(String);
            bool StringHasBOM                       = UTF16_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                       = UTF16_Init(StringSize - UTF16BOMSizeInCodeUnits);
                if (BOMLessString != NULL) {
                    for (uint64_t CodeUnit = 1ULL; CodeUnit < StringSize; CodeUnit++) {
                        BOMLessString[CodeUnit - 1] = String[CodeUnit];
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate BOMLessString"));
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    
    UTF32 *UTF32_RemoveBOM(ImmutableString_UTF32 String) {
        UTF32   *BOMLessString                   = NULL;
        uint64_t StringSize                      = 0ULL;
        if (String != NULL) {
            StringSize                           = UTF32_GetStringSizeInCodePoints(String);
            bool StringHasBOM                    = UTF32_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                    = UTF32_Init(StringSize - UnicodeBOMSizeInCodePoints);
                if (BOMLessString != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                        BOMLessString[CodePoint] = String[CodePoint + 1];
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate BOMLessString"));
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return BOMLessString;
    }
    
    UTF32 *UTF8_Decode(ImmutableString_UTF8 String) {
        UTF32   *DecodedString                       = NULL;
        if (String != NULL) {
            uint64_t CodeUnit                        = 0ULL;
            uint64_t StringSize                      = UTF8_GetStringSizeInCodePoints(String);
            DecodedString                            = UTF32_Init(StringSize);
            if (DecodedString != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                    DecodedString[CodePoint]         = UTF8_DecodeCodePoint(&String[CodeUnit]);
                    CodeUnit                        += UTF8_GetCodePointSizeInCodeUnits(String[CodePoint]);
                }
            } else if (DecodedString == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate DecodedString"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    StringIO_BOMs UTF16_GetByteOrder(UTF16 CodeUnit) {
        StringIO_BOMs ByteOrder = StringIO_BOM_Unspecified;
        if (CodeUnit == UTF16BOM_LE) {
            ByteOrder                = StringIO_BOM_Little;
        } else if (CodeUnit == UTF16BOM_BE) {
            ByteOrder                = StringIO_BOM_Big;
        } else { // Heuristic (aka guess) mode
            uint8_t NumBitsInByte0   = CountBitsSet(CodeUnit & 0xFF);
            uint8_t NumBitsInByte1   = CountBitsSet((CodeUnit & 0xFF00) >> 8);
            if (NumBitsInByte0 >= NumBitsInByte1) {
                ByteOrder            = StringIO_BOM_Little;
            } else {
                ByteOrder            = StringIO_BOM_Big;
            }
        }
        return ByteOrder;
    }
    
    StringIO_BOMs UTF32_GetByteOrder(UTF32 CodeUnit) {
        StringIO_BOMs ByteOrder = StringIO_BOM_Unspecified;
        if (CodeUnit == UTF32BOM_LE) {
            ByteOrder                = StringIO_BOM_Little;
        } else if (CodeUnit == UTF32BOM_BE) {
            ByteOrder                = StringIO_BOM_Big;
        } else { // Heuristic (aka guess) mode
            uint8_t NumBitsInByte0   = CountBitsSet(CodeUnit & 0x000000FF);
            uint8_t NumBitsInByte1   = CountBitsSet((CodeUnit & 0x0000FF00) >> 8);
            uint8_t NumBitsInByte2   = CountBitsSet((CodeUnit & 0x00FF0000) >> 16);
            if (NumBitsInByte0 >= NumBitsInByte1 && NumBitsInByte2 >= NumBitsInByte0) {
                ByteOrder            = StringIO_BOM_Big;
            } else {
                ByteOrder            = StringIO_BOM_Little;
            }
        }
        return ByteOrder;
    }
    
    UTF32 *UTF16_Decode(ImmutableString_UTF16 String) {
        UTF32   *DecodedString                       = NULL;
        if (String != NULL) {
            uint64_t CodeUnit                        = 0ULL;
            uint64_t StringSize                      = UTF16_GetStringSizeInCodePoints(String);
            DecodedString                            = UTF32_Init(StringSize);
            if (DecodedString != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                    DecodedString[CodePoint]         = UTF16_DecodeCodePoint(&String[CodeUnit]);
                    CodeUnit                        += UTF16_GetCodePointSizeInCodeUnits(String[CodeUnit]);
                }
            } else if (DecodedString == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate DecodedString"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF8 *UTF8_Encode(ImmutableString_UTF32 String) {
        UTF8    *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t UTF8CodeUnits                     = UTF32_GetStringSizeInUTF8CodeUnits(String);
            EncodedString                              = UTF8_Init(UTF8CodeUnits);
            uint64_t CodePoint                         = 0ULL;
            uint64_t CodeUnit                          = 0ULL;
            uint8_t  Index                             = 0;
            if (EncodedString != NULL) {
                while (String[CodePoint] != PlatformIO_NULLTerminator) {
                    UTF8 *CodeUnits                    = UTF8_EncodeCodePoint(String[CodePoint]);
                    while (CodeUnits[Index] != PlatformIO_NULLTerminator) {
                        EncodedString[CodeUnit]        = CodeUnits[Index];
                        CodeUnit                      += 1;
                        Index                         += 1;
                    }
                    CodePoint += 1;
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Encoded Pointer is NULL"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF16 *UTF16_Encode(ImmutableString_UTF32 String) {
        UTF16   *EncodedString                           = NULL;
        if (String != NULL) {
            uint64_t CodePoint                           = 0ULL;
            uint64_t CodeUnit                            = 0ULL;
            uint64_t NumCodeUnits                        = UTF32_GetStringSizeInUTF16CodeUnits(String);
            EncodedString                                = UTF16_Init(NumCodeUnits);
            if (EncodedString != NULL) {
                while (CodeUnit < NumCodeUnits) {
                    if (String[CodePoint] > UTF16MaxCodeUnitValue && String[CodePoint] <= UnicodeMaxCodePoint) {
                        // CodePoint = 0x10FFFF
                        uint32_t Ranged              = String[CodePoint] - UTF16SurrogatePairStart; // 0xFFFFF
                        UTF16    HighCodeUnit        = UTF16HighSurrogateStart + (Ranged & (UTF16SurrogateMask << UTF16SurrogateShift) >> UTF16SurrogateShift);
                        // 0xD800 + (0xFFFFF & (0x3FF << 10) aka 0xFFC00) >> 10 = 0x3FF aka 0xDBFF
                        //0x10FFFF = 0xDBFF 0xDFFF
                        UTF16    LowCodeUnit         = UTF16LowSurrogateStart  + (Ranged &  UTF16SurrogateMask); // 0xDC00 + (0xFFFFF & 0x3FF) = 0xDFFF
                        
                        EncodedString[CodeUnit]      = HighCodeUnit;
                        EncodedString[CodeUnit + 1]  = LowCodeUnit;
                        CodeUnit                    += 2;
                    } else {
                        EncodedString[CodeUnit]      = String[CodePoint] & UTF16MaxCodeUnitValue;
                        CodeUnit                    += 1;
                    }
                    CodePoint                       += 1;
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Encoded Pointer is NULL"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF8 *UTF16_Convert(ImmutableString_UTF16 String) {
        UTF8 *String8       = NULL;
        if (String != NULL) {
            // 0x3FF = UTF32_GetStringSizeInUTF8CodeUnits
            UTF32 *String32 = UTF16_Decode(String);
            String8         = UTF8_Encode(String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return String8;
    }
    
    UTF16 *UTF8_Convert(ImmutableString_UTF8 String) {
        UTF16 *String16     = NULL;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            String16        = UTF16_Encode(String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return String16;
    }
    
    UTF8 *UTF8_Clone(ImmutableString_UTF8 String) {
        UTF8 *Copy = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodeUnits = UTF8_GetStringSizeInCodeUnits(String);
            Copy                           = UTF8_Init(StringSizeInCodeUnits);
            if (Copy != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
                    Copy[CodeUnit] = String[CodeUnit];
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Copy Pointer is NULL"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF16 *UTF16_Clone(ImmutableString_UTF16 String) {
        UTF16 *Copy = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodeUnits = UTF16_GetStringSizeInCodeUnits(String);
            Copy                           = UTF16_Init(StringSizeInCodeUnits);
            if (Copy != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
                    Copy[CodeUnit] = String[CodeUnit];
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Copy Pointer is NULL"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF32 *UTF32_Clone(ImmutableString_UTF32 String) {
        UTF32 *Copy = NULL;
        if (String != NULL) {
            uint64_t StringSizeInCodePoints = UTF32_GetStringSizeInCodePoints(String);
            Copy                            = UTF32_Init(StringSizeInCodePoints);
            if (Copy != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < StringSizeInCodePoints; CodePoint++) {
                    Copy[CodePoint] = String[CodePoint];
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Copy Pointer is NULL"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Copy;
    }
    
    UTF8 UTF8_Erase(UTF8 *String, UTF8 NewValue) {
        UTF8 Verification = 0xFE;
        if (String != NULL) {
            uint64_t StringSize  = UTF8_GetStringSizeInCodePoints(String);
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                String[CodeUnit] = NewValue;
            }
            Verification         = String[0];
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Verification;
    }
    
    UTF16 UTF16_Erase(UTF16 *String, UTF16 NewValue) {
        UTF16 Verification = 0xFE;
        if (String != NULL) {
            uint64_t StringSize  = UTF16_GetStringSizeInCodePoints(String);
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                String[CodeUnit] = NewValue;
            }
            Verification = String[0];
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Verification;
    }
    
    UTF32 UTF32_Erase(UTF32 *String, UTF32 NewValue) {
        UTF32 Verification = 0xFE;
        if (String != NULL) {
            uint64_t StringSize  = UTF32_GetStringSizeInCodePoints(String);
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                String[CodeUnit] = NewValue;
            }
            Verification = String[0];
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Verification;
    }
    
    UTF8 *UTF8_Truncate(ImmutableString_UTF8 String, uint64_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        UTF8 *Truncated = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *Truncated32 = UTF32_Truncate(String32, NumGraphemes);
            free(String32);
            Truncated          = UTF8_Encode(Truncated32);
            free(Truncated32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    UTF16 *UTF16_Truncate(ImmutableString_UTF16 String, uint64_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        UTF16 *Truncated = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *Truncated32 = UTF32_Truncate(String32, NumGraphemes);
            free(String32);
            Truncated          = UTF16_Encode(Truncated32);
            free(Truncated32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    UTF32 *UTF32_Truncate(ImmutableString_UTF32 String, uint64_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        UTF32 *Truncated = NULL;
        if (String != NULL) {
            uint64_t StringSize      = UTF32_GetStringSizeInCodePoints(String);
            uint64_t CodePoint       = 0ULL;
            if (NumGraphemes <= StringSize) {
                Truncated            = UTF32_Init(NumGraphemes);
                if (Truncated != NULL) {
                    while (CodePoint < NumGraphemes) {
                        Truncated[CodePoint]  = String[CodePoint];
                        CodePoint            += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    void UTF8_WriteGrapheme(FILE *Source, ImmutableString_UTF8 Grapheme) {
        if (Source != NULL) {
            uint64_t StringSize       = UTF8_GetStringSizeInCodeUnits(Grapheme);
            uint64_t CodeUnitsWritten = FileIO_Write(Source, Grapheme, sizeof(UTF8), StringSize);
            if (CodeUnitsWritten != StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits but %llu was requested"), CodeUnitsWritten, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_WriteGrapheme(FILE *Source, ImmutableString_UTF16 Grapheme) {
        if (Source != NULL) {
            uint64_t StringSize       = UTF16_GetStringSizeInCodeUnits(Grapheme);
            uint64_t CodeUnitsWritten = FileIO_Write(Source, Grapheme, sizeof(UTF8), StringSize);
            if (CodeUnitsWritten != StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits but %llu was requested"), CodeUnitsWritten, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }
    
    static UTF8 UTF8_ReadCodeUnit(FILE *Source) { // Replaces fgetc, getc, getchar; depending on your definition of "Character"
        UTF8 CodeUnit = 0;
        if (Source != NULL) {
            uint64_t CodeUnitsRead = FileIO_Read(Source, &CodeUnit, sizeof(UTF8), 1);
            if (CodeUnitsRead != sizeof(UTF8)) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unable to read UTF8 CodeUnit"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodeUnit;
    }
    
    static UTF16 UTF16_ReadCodeUnit(FILE *Source) {
        UTF16 CodeUnit = 0;
        if (Source != NULL) {
            uint64_t CodeUnitsRead = FileIO_Read(Source, &CodeUnit, sizeof(UTF16), 1);
            if (CodeUnitsRead != sizeof(UTF16)) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unable to read UTF16 CodeUnit"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodeUnit;
    }
    
    static UTF32 UTF8_ReadCodePoint(FILE *Source) {
        UTF32   CodePoint     = 0;
        uint8_t CodeUnitsRead = 0;
        if (Source != NULL) {
            CodeUnitsRead     = (uint8_t) FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[0], UTF8MaxCodeUnitsInCodePoint, 1);
            if (CodeUnitsRead == 1) {
                uint8_t CodePointSize = UTF8_GetCodePointSizeInCodeUnits(StringIO_PreallocateCodePoint_UTF8[0]) - 1;
                CodeUnitsRead = (uint8_t) FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[1], UTF8MaxCodeUnitsInCodePoint, CodePointSize);
                if (CodeUnitsRead == CodePointSize) {
                    CodePoint = UTF8_DecodeCodePoint(StringIO_PreallocateCodePoint_UTF8);
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %hhu CodeUnits but %hhu was requested"), CodeUnitsRead, CodePointSize);
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %hhu CodeUnits but 1 was requested"), CodeUnitsRead);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodePoint;
    }
    
    static UTF32 UTF16_ReadCodePoint(FILE *Source) {
        UTF32   CodePoint     = 0;
        uint8_t CodeUnitsRead = 0;
        if (Source != NULL) {
            CodeUnitsRead     = (uint8_t) FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[0], UTF16MaxCodeUnitsInCodePoint, 1);
            if (CodeUnitsRead == 1) {
                uint8_t CodePointSize = UTF16_GetCodePointSizeInCodeUnits(StringIO_PreallocateCodePoint_UTF16[0]) - 1;
                CodeUnitsRead = (uint8_t) FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[1], UTF16MaxCodeUnitsInCodePoint, CodePointSize);
                if (CodeUnitsRead == CodePointSize) {
                    CodePoint = UTF16_DecodeCodePoint(StringIO_PreallocateCodePoint_UTF16);
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %hhu CodeUnits but %hhu was requested"), CodeUnitsRead, CodePointSize);
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %hhu CodeUnits but 1 was requested"), CodeUnitsRead);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodePoint;
    }
    
    static UTF32 UTF32_ReadCodePoint(FILE *Source) {
        UTF32 CodePoint = 0;
        if (Source != NULL) {
            uint64_t CodeUnitsRead = FileIO_Read(Source, &CodePoint, sizeof(UTF32), 1);
            if (CodeUnitsRead != sizeof(UTF32)) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unable to read UTF32 CodePoint"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF8 *UTF8_ReadGrapheme(FILE *Source) {
        UTF8 *Grapheme                             = NULL;
        if (Source != NULL) {
            /*
             Read a CodePoint, look it up in the GrapheneTable, if it, or the following CodePoint are GraphemeExtenders, keep going.
             */
            UTF32 CodePoint1 = UTF8_ReadCodePoint(Source);
            UTF32 CodePoint2 = UTF8_ReadCodePoint(Source);
            
            /*
             CodePoint1 not GREXT = check cp2
             */
            
            
            
            
            bool     GraphemeFound                 = No;
            uint64_t CodePointSizeInCodeUnits      = 0ULL;
            uint64_t CodeUnitsRead                 = 0ULL;
            UTF8     CodeUnit                      = 0;
            while (GraphemeFound == No) {
                CodeUnitsRead                      = FileIO_Read(Source, &CodeUnit, sizeof(UTF8), 1);
                if (CodeUnitsRead == 1) {
                    CodePointSizeInCodeUnits      += UTF8_GetCodePointSizeInCodeUnits(CodeUnit);
                    FileIO_Seek(Source, CodePointSizeInCodeUnits, SeekType_Current);
                    Grapheme                       = UTF8_Init(CodePointSizeInCodeUnits);
                    CodeUnitsRead                  = FileIO_Read(Source, &Grapheme, sizeof(UTF8), CodePointSizeInCodeUnits);
                    if (CodeUnitsRead == CodePointSizeInCodeUnits) {
                        FileIO_Read(Source, &Grapheme, sizeof(UTF8), CodePointSizeInCodeUnits);
                        UTF32 *CodePoint           = UTF8_Decode(Grapheme);
                        for (uint64_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                            if (CodePoint[0] == GraphemeExtensionTable[GraphemeExtension]) {
                            } else if (CodePoint[0] != GraphemeExtensionTable[GraphemeExtension] && GraphemeExtension == GraphemeExtensionTableSize - 1) {
                                GraphemeFound      = Yes;
                            }
                        }
                    } else {
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %llu bytes but %llu was requested"), CodeUnitsRead, CodePointSizeInCodeUnits);
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %llu bytes but 1 was requested"), CodeUnitsRead);
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF16 *UTF16_ReadGrapheme(FILE *Source) {
        UTF16 *Grapheme                            = NULL;
        if (Source != NULL) {
            bool     GraphemeFound                 = No;
            uint64_t CodePointSizeInCodeUnits      = 0ULL;
            uint64_t CodeUnitsRead                 = 0ULL;
            UTF16    CodeUnit                      = 0;
            while (GraphemeFound == No) {
                CodeUnitsRead                      = FileIO_Read(Source, &CodeUnit, sizeof(UTF16), 1);
                if (CodeUnitsRead == 1) {
                    CodePointSizeInCodeUnits      += UTF16_GetCodePointSizeInCodeUnits(CodeUnit);
                    FileIO_Seek(Source, CodePointSizeInCodeUnits, SeekType_Current);
                    Grapheme                       = UTF16_Init(CodePointSizeInCodeUnits);
                    CodeUnitsRead                  = FileIO_Read(Source, &Grapheme, sizeof(UTF16), CodePointSizeInCodeUnits);
                    if (CodeUnitsRead == CodePointSizeInCodeUnits) {
                        FileIO_Read(Source, &Grapheme, sizeof(UTF16), CodePointSizeInCodeUnits);
                        UTF32 *CodePoint           = UTF16_Decode(Grapheme);
                        for (uint64_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                            if (CodePoint[0] == GraphemeExtensionTable[GraphemeExtension]) {
                            } else if (CodePoint[0] != GraphemeExtensionTable[GraphemeExtension] && GraphemeExtension == GraphemeExtensionTableSize - 1) {
                                GraphemeFound      = Yes;
                            }
                        }
                    } else {
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %llu bytes but %llu was requested"), CodeUnitsRead, CodePointSizeInCodeUnits);
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %llu bytes but 1 was requested"), CodeUnitsRead);
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return Grapheme;
    }
    
    static uint64_t UTF32_LineBreakPosition(UTF32 *String) {
        uint64_t CodePoint              = 0ULL;
        if (String != NULL && String[CodePoint] != PlatformIO_NULLTerminator) {
            if (String[CodePoint] != PlatformIO_NULLTerminator && String[CodePoint + 1] != PlatformIO_NULLTerminator) {
                if ((String[CodePoint] == '\r' && String[CodePoint + 1] == '\n') || (String[CodePoint] == '\n' && String[CodePoint + 1] == '\r')) { // CR+LF or LF+CR
                    CodePoint          += 2;
                } else {
                    for (uint64_t LineBreak = 0ULL; LineBreak < LineBreakTableSize; LineBreak++) {
                        if (CodePoint == LineBreakTable[LineBreak]) {
                            CodePoint  += 1;
                            break;
                        }
                    }
                }
            } else if (String[CodePoint] != PlatformIO_NULLTerminator) {
                for (uint64_t LineBreak = 0ULL; LineBreak < LineBreakTableSize; LineBreak++) {
                    if (CodePoint == LineBreakTable[LineBreak]) {
                        CodePoint      += 1;
                        break;
                    }
                }
            }
        }
        return CodePoint;
    }
    
    typedef enum StringIO_LineBreakTypes {
        LineBreakType_Unspecified        = 0, // Not a newline
        LineBreakType_LineFeed           = 1,
        LineBreakType_VerticalTab        = 2,
        LineBreakType_FormFeed           = 3,
        LineBreakType_CarriageReturn     = 4,
        LineBreakType_CRLF               = 5,
        LineBreakType_LFCR               = 6,
        LineBreakType_NextLine           = 7,
        LineBreakType_LineSeparator      = 8,
        LineBreakType_ParagraphSeparator = 9,
    } StringIO_LineBreakTypes;
    
    static StringIO_LineBreakTypes UTF32_GetLineBreakType(UTF32 CodePoint1, UTF32 CodePoint2) {
        StringIO_LineBreakTypes LineBreakType = LineBreakType_Unspecified;
        if (CodePoint1 == U'\r' && CodePoint2 == U'\n') {
            LineBreakType = LineBreakType_CRLF;
        } else if (CodePoint1 == U'\n' && CodePoint2 == U'\r') {
            LineBreakType = LineBreakType_LFCR;
        } else {
            if (CodePoint1 == U'\n' || CodePoint2 == U'\n') {
                LineBreakType = LineBreakType_LineFeed;
            } else if (CodePoint1 == U'\v' || CodePoint2 == U'\v') {
                LineBreakType = LineBreakType_VerticalTab;
            } else if (CodePoint1 == U'\f' || CodePoint2 == U'\f') {
                LineBreakType = LineBreakType_FormFeed;
            } else if (CodePoint1 == U'\r' || CodePoint2 == U'\r') {
                LineBreakType = LineBreakType_CarriageReturn;
            } else if (CodePoint1 == U'\x85' || CodePoint2 == U'\x85') {
                LineBreakType = LineBreakType_NextLine;
            } else if (CodePoint1 == U'\u2028' || CodePoint2 == U'\u2028') {
                LineBreakType = LineBreakType_LineSeparator;
            } else if (CodePoint1 == U'\u2029' || CodePoint2 == U'\u2029') {
                LineBreakType = LineBreakType_ParagraphSeparator;
            }
        }
        return LineBreakType;
    }
    
    UTF8 *UTF8_ReadSentence(FILE *Source) {
        UTF8 *Sentence                                     = NULL;
        if (Source != NULL) {
            uint64_t  SentenceSize                         = 0LL;
            StringIO_LineBreakTypes LineBreakType          = LineBreakType_Unspecified;
            while (LineBreakType == LineBreakType_Unspecified) {
                UTF32 CodePoint1                           = UTF8_ReadCodePoint(Source);
                UTF32 CodePoint2                           = UTF8_ReadCodePoint(Source);
                LineBreakType                              = UTF32_GetLineBreakType(CodePoint1, CodePoint2);
                if (LineBreakType == LineBreakType_Unspecified) {
                    SentenceSize                          += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
                    SentenceSize                          += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint2);
                } else if (LineBreakType != LineBreakType_CRLF && LineBreakType != LineBreakType_LFCR) {
                    StringIO_LineBreakTypes LineBreakType1 = UTF32_GetLineBreakType(CodePoint1, InvalidReplacementCodePoint);
                    StringIO_LineBreakTypes LineBreakType2 = UTF32_GetLineBreakType(CodePoint2, InvalidReplacementCodePoint);
                    if (LineBreakType1 != LineBreakType_Unspecified) {
                        SentenceSize                      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
                    } else if (LineBreakType2 != LineBreakType_Unspecified) {
                        SentenceSize                      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
                        SentenceSize                      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint2);
                    }
                }
            }
            Sentence                                       = UTF8_Init(SentenceSize);
            if (Sentence != NULL) {
                FileIO_Seek(Source, SentenceSize * -1, SeekType_Current); // Seek back to the beginning, allocate
                FileIO_Read(Source, &Sentence, 1, SentenceSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return Sentence;
    }
    
    UTF16 *UTF16_ReadSentence(FILE *Source) {
        UTF16 *Sentence                                    = NULL;
        if (Source != NULL) {
            uint64_t  SentenceSize                         = 0ULL;
            StringIO_LineBreakTypes LineBreakType          = LineBreakType_Unspecified;
            while (LineBreakType == LineBreakType_Unspecified) {
                UTF32 CodePoint1                           = UTF16_ReadCodePoint(Source);
                UTF32 CodePoint2                           = UTF16_ReadCodePoint(Source);
                LineBreakType                              = UTF32_GetLineBreakType(CodePoint1, CodePoint2);
                if (LineBreakType == LineBreakType_Unspecified) {
                    SentenceSize                          += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
                    SentenceSize                          += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint2);
                } else if (LineBreakType != LineBreakType_CRLF && LineBreakType != LineBreakType_LFCR) {
                    StringIO_LineBreakTypes LineBreakType1 = UTF32_GetLineBreakType(CodePoint1, InvalidReplacementCodePoint);
                    StringIO_LineBreakTypes LineBreakType2 = UTF32_GetLineBreakType(CodePoint2, InvalidReplacementCodePoint);
                    if (LineBreakType1 != LineBreakType_Unspecified) {
                        SentenceSize                      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
                    } else if (LineBreakType2 != LineBreakType_Unspecified) {
                        SentenceSize                      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
                        SentenceSize                      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint2);
                    }
                }
            }
            Sentence                                       = UTF16_Init(SentenceSize);
            if (Sentence != NULL) {
                FileIO_Seek(Source, SentenceSize * -1, SeekType_Current); // Seek back to the beginning, allocate
                FileIO_Read(Source, &Sentence, SentenceSize, SentenceSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return Sentence;
    }
    
    void UTF8_File_WriteString(FILE *OutputFile, ImmutableString_UTF8 String) {
        if (String != NULL && OutputFile != NULL) {
            TextIO_StringTypes Type       = FileIO_GetFileOrientation(OutputFile);
            uint64_t StringSize           = UTF8_GetStringSizeInCodeUnits(String);
            uint64_t CodeUnitsWritten     = 0ULL;
            if (Type == StringType_UTF8) {
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String, sizeof(UTF8), StringSize);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (Type == StringType_UTF16) {
                UTF32 *String32        = UTF8_Decode(String);
                UTF16 *String16        = UTF16_Encode(String32);
                UTF32_Deinit(String32);
                FileIO_Write(OutputFile, String16, sizeof(UTF16), PlatformIO_NewLine16Size);
                UTF16_Deinit(String16);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (Type == StringType_UTF32) {
                UTF32 *String32        = UTF8_Decode(String);
                FileIO_Write(OutputFile, String32, sizeof(UTF32), PlatformIO_NewLine32Size);
                UTF32_Deinit(String32);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_File_WriteString(FILE *OutputFile, ImmutableString_UTF16 String) {
        if (String != NULL && OutputFile != NULL) {
            TextIO_StringTypes Type       = FileIO_GetFileOrientation(OutputFile);
            uint64_t StringSize           = UTF16_GetStringSizeInCodeUnits(String);
            uint64_t CodeUnitsWritten     = 0ULL;
            if (Type == StringType_UTF16) {
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String, sizeof(UTF16), StringSize);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (Type == StringType_UTF8) {
                UTF32 *String32           = UTF16_Decode(String);
                UTF8  *String8            = UTF8_Encode(String32);
                uint64_t StringSize       = UTF8_GetStringSizeInCodeUnits(String8);
                UTF32_Deinit(String32);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String8, sizeof(UTF8), StringSize);
                UTF8_Deinit(String8);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (Type == StringType_UTF32) {
                UTF32 *String32           = UTF16_Decode(String);
                uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(String32);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String32, sizeof(UTF32), StringSize);
                UTF32_Deinit(String32);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }
    
    void UTF32_File_WriteString(FILE *OutputFile, ImmutableString_UTF32 String) {
        if (String != NULL && OutputFile != NULL) {
            TextIO_StringTypes Type       = FileIO_GetFileOrientation(OutputFile);
            uint64_t StringSize           = UTF32_GetStringSizeInCodePoints(String);
            uint64_t CodeUnitsWritten     = 0ULL;
            if (Type == StringType_UTF8) {
                UTF8  *String8            = UTF8_Encode(String);
                uint64_t StringSize8      = UTF8_GetStringSizeInCodeUnits(String8);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String8, sizeof(UTF8), StringSize8);
                UTF8_Deinit(String8);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (Type == StringType_UTF16) {
                UTF16 *String16           = UTF16_Encode(String);
                uint64_t StringSize       = UTF16_GetStringSizeInCodeUnits(String16);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String16, sizeof(UTF16), StringSize);
                UTF16_Deinit(String16);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            } else if (Type == StringType_UTF32) {
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String, sizeof(UTF32), StringSize);
                if (CodeUnitsWritten != StringSize) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu CodeUnits of %llu"), CodeUnitsWritten, StringSize);
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }
    
    UTF8 *UTF8_Reverse(ImmutableString_UTF8 String) {
        UTF8 *Reversed        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Reversed32 = UTF32_Reverse(String32);
            free(String32);
            Reversed          = UTF8_Encode(Reversed32);
            free(Reversed32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Reversed;
    }
    
    UTF16 *UTF16_Reverse(ImmutableString_UTF16 String) {
        UTF16 *Reversed = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Reversed32 = UTF32_Reverse(String32);
            free(String32);
            Reversed          = UTF16_Encode(Reversed32);
            free(Reversed32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Reversed;
    }
    
    UTF32 *UTF32_Reverse(ImmutableString_UTF32 String) {
        UTF32 *Reverse = NULL;
        if (String != NULL) {
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            Reverse             = UTF32_Init(StringSize);
            if (Reverse != NULL) {
                for (uint64_t CodePoint = StringSize; CodePoint > 0; CodePoint--) {
                    Reverse[CodePoint] = String[CodePoint - StringSize];
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Reverse;
    }
    
    size_t UTF8_Find(UTF8 *String, size_t StringLength, UTF8 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        size_t Offset = 0;
        uint64_t String2FindSize = UTF8_GetStringSizeInCodeUnits(String2Find);
        if (StringLength >= String2FindSize) {
            if (WhitespaceType == WhitespaceType_Insignificant) {
                while (UTF8_IsWordBreak(&String[Offset])) {
                    Offset += 1;
                }
            } else if (WhitespaceType == WhitespaceType_Unspecified) {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("WhitespaceType_Unspecified is invalid"));
            }
            
            for (uint64_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
                for (uint64_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                    if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                        Offset = SubCodePoint;
                    }
                }
            }
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("StringLength %zu is smaller than String2Find %llu"), StringLength, String2FindSize);
        }
        return Offset;
    }
    
    size_t UTF16_Find(UTF16 *String, size_t StringLength, UTF16 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        size_t Offset = 0;
        uint64_t String2FindSize = UTF16_GetStringSizeInCodeUnits(String2Find);
        if (StringLength >= String2FindSize) {
            if (WhitespaceType == WhitespaceType_Insignificant) {
                while (UTF16_IsWordBreak(String[Offset])) {
                    Offset += 1;
                }
            } else if (WhitespaceType == WhitespaceType_Unspecified) {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("WhitespaceType_Unspecified is invalid"));
            }
            
            for (uint64_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
                for (uint64_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                    if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                        Offset = SubCodePoint;
                    }
                }
            }
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("StringLength %zu is smaller than String2Find %llu"), StringLength, String2FindSize);
        }
        return Offset;
    }
    
    size_t UTF32_Find(UTF32 *String, size_t StringLength, UTF32 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        size_t Offset = 0;
        uint64_t String2FindSize = UTF32_GetStringSizeInCodePoints(String2Find);
        if (StringLength >= String2FindSize) {
            if (WhitespaceType == WhitespaceType_Insignificant) {
                while (UTF32_IsWordBreak(String[Offset])) {
                    Offset += 1;
                }
            } else if (WhitespaceType == WhitespaceType_Unspecified) {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("WhitespaceType_Unspecified is invalid"));
            }
            
            for (uint64_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
                for (uint64_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                    if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                        Offset = SubCodePoint;
                    }
                }
            }
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("StringLength %zu is smaller than String2Find %llu"), StringLength, String2FindSize);
        }
        return Offset;
    }
    
    int64_t UTF8_FindSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString, uint64_t Offset, int64_t Length) {
        int64_t FoundOffset    = 0LL;
        if (String != NULL && SubString != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *SubString32 = UTF8_Decode(SubString);
            FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
            free(String32);
            free(SubString32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString Pointer is NULL"));
        }
        return FoundOffset;
    }
    
    int64_t UTF16_FindSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString, uint64_t Offset, int64_t Length) {
        int64_t FoundOffset = 0LL;
        if (String != NULL && SubString != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *SubString32 = UTF16_Decode(SubString);
            FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
            free(String32);
            free(SubString32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString Pointer is NULL"));
        }
        return FoundOffset;
    }
    
    int64_t UTF32_FindSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString, uint64_t Offset, int64_t Length) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString Pointer is NULL"));
        } else if (Length >= 1 && StringSize < Offset + Length) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %llu + Length %lld is larger than String %llu"), Offset, Length, StringSize);
        }
        return MatchingOffset;
    }
    
    UTF8 *UTF8_ExtractSubString(ImmutableString_UTF8 String, uint64_t Offset, uint64_t NumCodeUnits) {
        UTF8 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, NumCodeUnits);
            ExtractedSubString = UTF8_Encode(Extracted32);
            free(String32);
            free(Extracted32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF16 *UTF16_ExtractSubString(ImmutableString_UTF16 String, uint64_t Offset, uint64_t NumCodeUnits) {
        UTF16 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, NumCodeUnits);
            ExtractedSubString = UTF16_Encode(Extracted32);
            free(String32);
            free(Extracted32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF32 *UTF32_ExtractSubString(ImmutableString_UTF32 String, uint64_t Offset, uint64_t NumCodePoints) {
        uint64_t  StringSize                            = UTF32_GetStringSizeInCodePoints(String);
        UTF32    *ExtractedString                       = NULL;
        if (String != NULL && StringSize >= Offset + NumCodePoints) {
            ExtractedString                             = UTF32_Init(NumCodePoints);
            if (ExtractedString != NULL) {
                for (uint64_t CodePoint = Offset; CodePoint < Offset + NumCodePoints; CodePoint++) {
                    ExtractedString[CodePoint - Offset] = String[CodePoint];
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate ExtractedString"));
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (StringSize < Offset + NumCodePoints) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %llu + NumCodePoints %lld is larger than String %llu"), Offset, NumCodePoints, StringSize);
        }
        return ExtractedString;
    }
    
    UTF8 *UTF8_Create(ImmutableString_UTF8 Padding, uint64_t Times2Pad) {
        UTF8 *Padded           = NULL;
        if (Padding != NULL) {
            UTF32 *Padding32   = UTF8_Decode(Padding);
            UTF32 *Padded32    = UTF32_Create(Padding32, Times2Pad);
            free(Padding32);
            Padded             = UTF8_Encode(Padded32);
        } else if (Padding == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Padding Pointer is NULL"));
        }
        return Padded;
    }
    
    UTF16 *UTF16_Create(ImmutableString_UTF16 Padding, uint64_t Times2Pad) {
        UTF16 *Padded          = NULL;
        if (Padding != NULL) {
            UTF32 *Padding32   = UTF16_Decode(Padding);
            UTF32 *Padded32    = UTF32_Create(Padding32, Times2Pad);
            free(Padding32);
            Padded             = UTF16_Encode(Padded32);
        } else if (Padding == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Padding Pointer is NULL"));
        }
        return Padded;
    }
    
    UTF32 *UTF32_Create(ImmutableString_UTF32 Padding, uint64_t Times2Pad) {
        UTF32 *Padded                     = NULL;
        if (Padding != NULL) {
            uint64_t Size                 = UTF32_GetStringSizeInCodePoints(Padding);
            Padded                        = UTF32_Init(Size * Times2Pad);
            if (Padded != NULL) {
                for (uint64_t PadString = 0; PadString < Size * Times2Pad; PadString++) {
                    for (uint64_t PaddingCodePoint = 0; PaddingCodePoint < Size; PaddingCodePoint++) {
                        Padded[PadString] = Padding[PaddingCodePoint];
                    }
                }
            }
        } else if (Padding == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Padding Pointer is NULL"));
        }
        return Padded;
    }
    
    UTF8 *UTF8_SubstituteSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substitution, uint64_t Offset, uint64_t Length) {
        UTF8 *Replaced8           = NULL;
        if (String != NULL && Substitution != NULL) {
            UTF32 *String32       = UTF8_Decode(String);
            UTF32 *Substitution32 = UTF8_Decode(Substitution);
            UTF32 *Replaced32     = UTF32_SubstituteSubString(String32, Substitution32, Offset, Length);
            Replaced8             = UTF8_Encode(Replaced32);
            free(String32);
            free(Substitution32);
            free(Replaced32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substitution == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Replacement Pointer is NULL"));
        }
        return Replaced8;
    }
    
    UTF16 *UTF16_SubstituteSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substitution, uint64_t Offset, uint64_t Length) {
        UTF16 *Replaced16         = NULL;
        if (String != NULL && Substitution != NULL) {
            UTF32 *String32       = UTF16_Decode(String);
            UTF32 *Substitution32 = UTF16_Decode(Substitution);
            UTF32 *Replaced32     = UTF32_SubstituteSubString(String32, Substitution32, Offset, Length);
            Replaced16            = UTF16_Encode(Replaced32);
            free(String32);
            free(Substitution32);
            free(Replaced32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substitution == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Substitution Pointer is NULL"));
        }
        return Replaced16;
    }
    
    UTF32 *UTF32_SubstituteSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substitution, uint64_t Offset, uint64_t Length) {
        UTF32 *NewString                            = NULL;
        if (String != NULL && Substitution != NULL) {
            uint64_t StringSize                     = UTF32_GetStringSizeInCodePoints(String);
            uint64_t SubstitutionSize               = UTF32_GetStringSizeInCodePoints(Substitution);
            uint64_t NewStringSize                  = (Maximum(StringSize, Length) - Minimum(StringSize, Length)) + SubstitutionSize;
            
            NewString                               = UTF32_Init(NewStringSize);
            if (NewString != NULL) {
                uint64_t NewCodePoint               = 0ULL;
                uint64_t StringCodePoint            = 0ULL;
                uint64_t SubstitutionCodePoint      = 0ULL;
                
                while (NewCodePoint < NewStringSize) {
                    if (NewCodePoint == Offset) {
                        while (SubstitutionCodePoint < SubstitutionSize) {
                            NewString[NewCodePoint] = Substitution[SubstitutionCodePoint];
                            NewCodePoint           += 1;
                            SubstitutionCodePoint  += 1;
                        }
                        StringCodePoint            += Length;
                    } else {
                        NewString[NewCodePoint]     = String[StringCodePoint];
                        NewCodePoint               += 1;
                        StringCodePoint            += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substitution == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Substitution Pointer is NULL"));
        }
        return NewString;
    }
    
    UTF8 *UTF8_StitchSubString(ImmutableString_UTF8 String, uint64_t Offset, uint64_t Length) {
        UTF8 *Stitched = NULL;
        if (String != NULL) {
            UTF32 *Decoded    = UTF8_Decode(String);
            UTF32 *Stitched32 = UTF32_StitchSubString(Decoded, Offset, Length);
            free(Decoded);
            Stitched          = UTF8_Encode(Stitched32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Stitched;
    }
    
    UTF16 *UTF16_StitchSubString(ImmutableString_UTF16 String, uint64_t Offset, uint64_t Length) {
        UTF16 *Stitched       = NULL;
        if (String != NULL) {
            UTF32 *Decoded    = UTF16_Decode(String);
            UTF32 *Stitched32 = UTF32_StitchSubString(Decoded, Offset, Length);
            free(Decoded);
            Stitched          = UTF16_Encode(Stitched32);
            free(Stitched32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Stitched;
    }
    
    UTF32 *UTF32_StitchSubString(ImmutableString_UTF32 String, uint64_t Offset, uint64_t Length) {
        UTF32 *Stitched = NULL;
        if (String != NULL) {
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            if (Offset <= StringSize && Length <= StringSize && Offset + Length <= StringSize) {
                uint64_t StitchedSize = StringSize - Length;
                Stitched              = UTF32_Init(StitchedSize);
                if (Stitched != NULL) {
                    uint64_t CodePoint = 0ULL;
                    while (CodePoint < StitchedSize && CodePoint + Length < StringSize) {
                        if (CodePoint < Offset) {
                            Stitched[CodePoint] = String[CodePoint];
                        } else if (CodePoint > Offset + Length) {
                            Stitched[CodePoint] = String[CodePoint + Length];
                        }
                        CodePoint += 1;
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Could not allocate Stitched string, size: %llu"), StitchedSize);
                }
            } else if (Offset > StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %llu is greater than the String's size %llu"), Offset, StringSize);
            } else if (Length > StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Length %llu is greater than the String's size %llu"), Length, StringSize);
            } else if (Offset + Length > StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset + Length %llu is greater than the String's size %llu"), Offset + Length, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF8 *UTF8_RemoveSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString2Remove, uint64_t Instance2Remove) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString2Remove Pointer is NULL"));
        }
        return TrimmedString;
    }
    
    UTF16 *UTF16_RemoveSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString2Remove, uint64_t Instance2Remove) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString2Remove Pointer is NULL"));
        }
        return TrimmedString;
    }
    
    UTF32 *UTF32_RemoveSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString2Remove, uint64_t Instance2Remove) {
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
            
            if (Instance2Remove >= 1) {
                EditedStringSize = StringSize - SubStringSize;
            } else if (Instance2Remove == 0xFFFFFFFFFFFFFFFF) {
                EditedStringSize = StringSize - (NumInstances * SubStringSize);
            }
            
            EditedString         = UTF32_Init(EditedStringSize);
            if (EditedString != NULL) {
                for (uint64_t EditedCodePoint = 0ULL; EditedCodePoint < EditedStringSize; EditedCodePoint++) {
                    for (uint64_t Instance = 0ULL; Instance < NumInstances; Instance++) {
                        if (EditedCodePoint < Instances[Instance] || EditedCodePoint > Instances[Instance] + SubStringSize) {
                            EditedString[EditedCodePoint] = String[EditedCodePoint - (Instances[Instance] + SubStringSize)];
                        }
                    }
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate the edited string"));
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString2Remove Pointer is NULL"));
        } else if (Instance2Remove == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Removing 0 instances of the SubString does not make sense"));
        }
        return EditedString;
    }
    
    UTF8 *UTF8_Insert(ImmutableString_UTF8 String, ImmutableString_UTF8 String2Insert, uint64_t Offset) {
        UTF8 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Insert32   = UTF8_Decode(String2Insert);
            UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
            free(String32);
            free(Insert32);
            Inserted          = UTF8_Encode(Inserted32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF16 *UTF16_Insert(ImmutableString_UTF16 String, ImmutableString_UTF16 String2Insert, uint64_t Offset) {
        UTF16 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Insert32   = UTF16_Decode(String2Insert);
            UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
            free(String32);
            free(Insert32);
            Inserted          = UTF16_Encode(Inserted32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF32 *UTF32_Insert(ImmutableString_UTF32 String, ImmutableString_UTF32 String2Insert, uint64_t Offset) {
        UTF32 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            uint64_t InsertSize = UTF32_GetStringSizeInCodePoints(String2Insert);
            if (Offset == 0xFFFFFFFFFFFFFFFF) {
                Offset          = StringSize;
            }
            if (Offset <= StringSize) {
                Inserted        = UTF32_Init(StringSize + InsertSize);
                if (Inserted != NULL) {
                    for (uint64_t CodePoint = 0ULL; CodePoint < StringSize + InsertSize; CodePoint++) {
                        if (CodePoint < Offset || CodePoint > Offset + InsertSize) {
                            Inserted[CodePoint] = String[CodePoint];
                        } else {
                            Inserted[CodePoint] = String2Insert[CodePoint - StringSize];
                        }
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate InsertedString"));
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %llu is greater than the string's size %llu"), Offset, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Inserted;
    }

    /* TextIOTables Operations */
    bool UTF32_IsCodePointInTable(const UTF32 *Table, uint64_t TableSize, UTF32 CodePoint) {
        bool Match = No;
        for (uint64_t Index = 0; Index < TableSize; Index++) {
            if (CodePoint == Table[Index]) {
                Match = Yes;
                break;
            }
        }
        return Match;
    }

    CodePointClass UTF32_GetCharacterClassOfCodePoint(UTF32 CodePoint) {
        CodePointClass CharacterClass = CodePointClass_Unspecified;
        for (uint64_t Index = 0; Index < CombiningCharacterClassTableSize; Index++) {
            if (CodePoint == CombiningCharacterClassTable[Index][0]) {
                CharacterClass = CombiningCharacterClassTable[Index][1];
                break;
            }
        }
        return CharacterClass;
    }

    int64_t UTF32_GetIntegerValueOfCodePoint(UTF32 CodePoint) {
        int64_t Integer = 0;
        for (uint64_t Index = 0; Index < IntegerValueTableSize; Index++) {
            if (CodePoint == IntegerValueTable[Index][0]) {
                Integer = IntegerValueTable[Index][1];
                break;
            }
        }
        return Integer;
    }

    float UTF32_GetDecimalValueOfCodePoint32(UTF32 CodePoint) {
        int64_t Integer = 0;
        for (uint64_t Index = 0; Index < IntegerValueTableSize; Index++) {
            if (CodePoint == IntegerValueTable[Index][0]) {
                Integer = IntegerValueTable[Index][1];
                break;
            }
        }
        return Integer;
    }

    double UTF32_GetDecimalValueOfCodePoint64(UTF32 CodePoint) {
        double Decimal = 0.0;
        for (uint64_t Index = 0; Index < DecimalValueTableSize; Index++) {
            if (CodePoint == DecimalValueTable[Index][0]) {
                Decimal = DecimalValueTable[Index][1] / DecimalValueTable[Index][2];
                break;
            }
        }
        return Decimal;
    }

    UTF32 *UTF32_GetReplacementStringFromTable(UTF32 *Table, uint64_t TableSize, UTF32 *String2Replace, uint64_t String2ReplaceSize) {
        UTF32 *Replacement = NULL;
        for (uint64_t Index = 0; Index < TableSize; Index++) {
            // Shit, gotta do string comparisons
            /*
             Loop over the table of replacement strings, checking the length of each one, we really should have the size of each replacement string in the tables
             */
        }
        return Replacement;
    }

    UTF8 *UTF8_CaseFold(ImmutableString_UTF8 String) {
        UTF8 *CaseFolded      = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *CaseFold32 = UTF32_CaseFold(String32);
            free(String32);
            CaseFolded        = UTF8_Encode(CaseFold32);
            free(CaseFold32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CaseFolded;
    }
    
    UTF16 *UTF16_CaseFold(ImmutableString_UTF16 String) {
        UTF16 *CaseFolded     = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *CaseFold32 = UTF32_CaseFold(String32);
            free(String32);
            CaseFolded        = UTF16_Encode(CaseFold32);
            free(CaseFold32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CaseFolded;
    }
    
    static uint64_t UTF32_GetCaseFoldedSize(ImmutableString_UTF32 String) {
        uint64_t NumCodePoints = 0ULL;
        uint64_t CodePoint = 0ULL;
        while (String[CodePoint] != PlatformIO_NULLTerminator) {
            uint64_t TableIndex = 0ULL;
            while (TableIndex < CaseFoldTableSize) {
                if (String[CodePoint] == CaseFoldTable[TableIndex][0][0]) {
                    NumCodePoints += UTF32_GetStringSizeInCodePoints(CaseFoldTable[TableIndex][1]);
                    CodePoint     += 1;
                    break;
                }
                TableIndex        += 1;
            }
        }
        return NumCodePoints;
    }
    
    static void UTF32_SubstitutePreallocated(UTF32 *String2Edit, ImmutableString_UTF32 Replacement) {
        uint64_t CodePoint = 0ULL;
        while (String2Edit[CodePoint] != PlatformIO_NULLTerminator && Replacement[CodePoint] != PlatformIO_NULLTerminator) {
            String2Edit[CodePoint] = Replacement[CodePoint];
            CodePoint += 1;
        }
    }
    
    UTF32 *UTF32_CaseFold(ImmutableString_UTF32 String) {
        UTF32   *CaseFoldedString                = NULL;
        if (String != NULL) {
            uint64_t CaseFoldedSize              = UTF32_GetCaseFoldedSize(String);
            CaseFoldedString                     = UTF32_Init(CaseFoldedSize);
            uint64_t CodePoint                   = 0ULL;
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                if (UTF32_IsUpperCase(String[CodePoint])) {
                    for (uint64_t Index = 0ULL; Index < CaseFoldTableSize; Index++) {
                        if (String[CodePoint] == CaseFoldTable[Index][0][0]) {
                            UTF32_SubstitutePreallocated(CaseFoldedString, CaseFoldTable[Index][1]);
                        }
                    }
                }
                CodePoint += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CaseFoldedString;
    }
    
    static UTF32 *UTF32_Reorder(UTF32 *String) { // Stable sort
        uint64_t CodePoint  = 1ULL;
        uint32_t CodePointA = 0UL;
        uint32_t CodePointB = 0UL;
        if (String != NULL) {
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NULL;
    }
    
    static UTF32 *UTF32_Compose(ImmutableString_UTF32 String, StringIO_NormalizationForms CompositionType) { // FIXME: Must use a stable sorting algorithm
        uint64_t CodePoint      = 0ULL;
        UTF32   *ComposedString = NULL;
        if (String != NULL && (CompositionType == NormalizationForm_CanonicalCompose || CompositionType == NormalizationForm_KompatibleCompose)) {
            if (CompositionType == NormalizationForm_CanonicalCompose) {
                while (String[CodePoint] != PlatformIO_NULLTerminator) {
                    for (uint64_t ComposeCodePoint = 0ULL; ComposeCodePoint < CanonicalNormalizationTableSize; ComposeCodePoint++) {
                        if (String[CodePoint] == CanonicalNormalizationTable[ComposeCodePoint][0][0]) {
                            ComposedString = UTF32_SubstituteSubString(String, CanonicalNormalizationTable[ComposeCodePoint][1], CodePoint, 1);
                        }
                    }
                    CodePoint += 1;
                }
            } else if (CompositionType == NormalizationForm_KompatibleCompose) {
                while (String[CodePoint] != PlatformIO_NULLTerminator) {
                    for (uint64_t ComposeCodePoint = 0ULL; ComposeCodePoint < KompatibleNormalizationTableSize; ComposeCodePoint++) {
                        if (String[CodePoint] == KompatibleNormalizationTable[ComposeCodePoint][0][0]) { // codepoint stored as a string
                            ComposedString = UTF32_SubstituteSubString(String, KompatibleNormalizationTable[ComposeCodePoint][1], CodePoint, 1);
                        }
                    }
                    CodePoint += 1;
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CompositionType != NormalizationForm_CanonicalCompose && CompositionType != NormalizationForm_KompatibleCompose) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid CompositionType"));
        }
        return ComposedString;
    }
    
    static UTF32 *UTF32_Decompose(ImmutableString_UTF32 String, StringIO_NormalizationForms DecompositionType) { // FIXME: Must use a stable sorting algorithm
        uint64_t CodePoint      = 0ULL;
        UTF32   *DecomposedString = NULL;
        if (String != NULL && (DecompositionType == NormalizationForm_CanonicalDecompose || DecompositionType == NormalizationForm_KompatibleDecompose)) {
            if (DecompositionType == NormalizationForm_CanonicalDecompose) {
                while (String[CodePoint] != PlatformIO_NULLTerminator) {
                    for (uint64_t DecomposeCodePoint = 0ULL; DecomposeCodePoint < CanonicalNormalizationTableSize; DecomposeCodePoint++) {
                        if (String[CodePoint] == CanonicalNormalizationTable[DecomposeCodePoint][0][0]) {
                            DecomposedString = UTF32_SubstituteSubString(String, CanonicalNormalizationTable[DecomposeCodePoint][1], CodePoint, 1);
                        }
                    }
                    CodePoint += 1;
                }
            } else if (DecompositionType == NormalizationForm_KompatibleDecompose) {
                while (String[CodePoint] != PlatformIO_NULLTerminator) {
                    for (uint64_t DecomposeCodePoint = 0ULL; DecomposeCodePoint < KompatibleNormalizationTableSize; DecomposeCodePoint++) {
                        if (String[CodePoint] == KompatibleNormalizationTable[DecomposeCodePoint][0][0]) { // codepoint stored as a string
                            DecomposedString = UTF32_SubstituteSubString(String, KompatibleNormalizationTable[DecomposeCodePoint][1], CodePoint, 1);
                        }
                    }
                    CodePoint += 1;
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (DecompositionType != NormalizationForm_CanonicalDecompose && DecompositionType != NormalizationForm_KompatibleDecompose) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid CompositionType"));
        }
        return DecomposedString;
    }
    
    UTF8 *UTF8_Normalize(ImmutableString_UTF8 String, StringIO_NormalizationForms NormalizedForm) {
        UTF8 *NormalizedString8       = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unspecified) {
            UTF32 *String32           = UTF8_Decode(String);
            UTF32 *NormalizedString32 = UTF32_Normalize(String32, NormalizedForm);
            NormalizedString8         = UTF8_Encode(NormalizedString32);
            free(String32);
            free(NormalizedString32);
        }
        return NormalizedString8;
    }
    
    UTF16 *UTF16_Normalize(ImmutableString_UTF16 String, StringIO_NormalizationForms NormalizedForm) {
        UTF16 *NormalizedString16     = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unspecified) {
            UTF32 *String32           = UTF16_Decode(String);
            UTF32 *NormalizedString32 = UTF32_Normalize(String32, NormalizedForm);
            NormalizedString16        = UTF16_Encode(NormalizedString32);
            free(String32);
            free(NormalizedString32);
        }
        return NormalizedString16;
    }
    
    UTF32 *UTF32_Normalize(ImmutableString_UTF32 String, StringIO_NormalizationForms NormalizedForm) {
        UTF32 *NormalizedString = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unspecified) {
            if (NormalizedForm == NormalizationForm_CanonicalCompose) {
                UTF32 *Decomposed = UTF32_Decompose(String, NormalizationForm_CanonicalDecompose);
                NormalizedString  = UTF32_Compose(Decomposed, NormalizationForm_CanonicalCompose);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationForm_KompatibleCompose) {
                UTF32 *Decomposed = UTF32_Decompose(String, NormalizationForm_KompatibleDecompose);
                NormalizedString  = UTF32_Compose(Decomposed, NormalizationForm_KompatibleCompose);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationForm_CanonicalDecompose) {
                NormalizedString  = UTF32_Decompose(String, NormalizationForm_CanonicalDecompose);
            } else if (NormalizedForm == NormalizationForm_KompatibleDecompose) {
                NormalizedString  = UTF32_Decompose(String, NormalizationForm_CanonicalDecompose);
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (NormalizedForm == NormalizationForm_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unspecified Normalization form"));
        }
        return NormalizedString;
    }
    /* TextIOTables Operations */
    
    int64_t UTF8_String2Integer(TextIO_Bases Base, ImmutableString_UTF8 String) { // Replaces atoi, atol, strtol, strtoul,
        int64_t Value = 0LL;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            Value           = UTF32_String2Integer(Base, String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Value;
    }
    
    int64_t UTF16_String2Integer(TextIO_Bases Base, ImmutableString_UTF16 String) {
        int64_t Value = 0LL;
        if (String != NULL) {
            UTF32 *String32 = UTF16_Decode(String);
            Value           = UTF32_String2Integer(Base, String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Value;
    }
    
    // Integer2String should accept any integer base from the lookup table and shift the value until it can't anymore
    int64_t UTF32_String2Integer(TextIO_Bases Base, ImmutableString_UTF32 String) {
        uint64_t CodePoint = 0ULL;
        int8_t   Sign      = 1;
        int64_t  Value     = 0LL;
        if (String != NULL) {
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator && (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('1'))) {
                        Value    <<= 1;
                        Value     += String[CodePoint] - 0x30;
                        CodePoint += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator && (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('7'))) {
                        Value     *= 8;
                        Value     += String[CodePoint] - 0x30;
                        CodePoint += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator && ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (CodePoint == 0 && String[CodePoint] == UTF32Character('-')))) {
                        if (CodePoint == 0 && String[CodePoint] == UTF32Character('-')) { // FIXME: Sign discovery needs work
                            Sign        = -1;
                        } else if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) {
                            Value      *= 10;
                            Value      += String[CodePoint] - 0x30;
                        }
                        CodePoint          += 1;
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator && ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F')))) {
                            if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) {
                                Value <<= 4;
                                Value  += String[CodePoint] - 0x30;
                            } else if (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f')) {
                                Value <<= 4;
                                Value  += String[CodePoint] - 0x41;
                            }
                            CodePoint  += 1;
                        }
                    } else if ((Base & Base_Lowercase) == Base_Lowercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator && ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f')))) {
                            if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) {
                                Value <<= 4;
                                Value  += String[CodePoint] - 0x30;
                            } else if (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f')) {
                                Value <<= 4;
                                Value  += String[CodePoint] - 0x61;
                            }
                            CodePoint  += 1;
                        }
                    }
                }
            } else if ((Base & Base_Decimal) == Base_Decimal || Base == Base_Unspecified) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid Base %u"), Base);
            }
            Value                      *= Sign;
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Value;
    }
    
    UTF8 *UTF8_Integer2String(TextIO_Bases Base, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF8  *IntegerString8  = UTF8_Encode((ImmutableString_UTF32) IntegerString32);
        UTF32_Deinit(IntegerString32);
        return IntegerString8;
    }
    
    UTF16 *UTF16_Integer2String(TextIO_Bases Base, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF16 *IntegerString16 = UTF16_Encode((ImmutableString_UTF32) IntegerString32);
        UTF32_Deinit(IntegerString32);
        return IntegerString16;
    }
    
    UTF32 *UTF32_Integer2String(TextIO_Bases Base, int64_t Integer2Convert) {
        UTF32   *String               = NULL;
        int64_t  Sign                 = 0LL;
        uint64_t Num                  = AbsoluteI(Integer2Convert);
        uint8_t  Radix                = 1;
        uint8_t  NumDigits            = 0;
        
        if (Integer2Convert < 0) { // Signed
            Sign                      = -1;
            NumDigits                +=  1;
        }
        
        if ((Base & Base_Integer) == Base_Integer) {
            if ((Base & Base_Radix2) == Base_Radix2) {
                Radix                 = 2;
            } else if ((Base & Base_Radix8) == Base_Radix8) {
                Radix                 = 8;
            } else if ((Base & Base_Radix10) == Base_Radix10) {
                Radix                 = 10;
            } else if ((Base & Base_Radix16) == Base_Radix16) {
                Radix                 = 16;
            }
            
            NumDigits                    += NumDigitsInInteger(Radix, Integer2Convert);
            
            String                        = UTF32_Init(NumDigits);
            
            if (String != NULL) {
                for (uint64_t CodePoint = NumDigits; CodePoint > 0; CodePoint--) {
                    uint8_t Digit                 = Num % Radix;
                    Num                          /= Radix;
                    if ((Base & Base_Integer) == Base_Integer && (Base & Base_Radix2) == Base_Radix2) {
                        String[CodePoint - 1]     = IntegerTableBase2[Digit];
                    } else if ((Base & Base_Integer) == Base_Integer && (Base & Base_Radix8) == Base_Radix8) {
                        String[CodePoint - 1]     = IntegerTableBase8[Digit];
                    } else if ((Base & Base_Integer) == Base_Integer && (Base & Base_Radix10) == Base_Radix10) {
                        if (Sign == -1 && CodePoint == 1) {
                            String[CodePoint - 1] = UTF32Character('-');
                        } else {
                            String[CodePoint - 1] = IntegerTableBase10[Digit];
                        }
                    } else if ((Base & Base_Integer) == Base_Integer && (Base & Base_Radix16) == Base_Radix16) {
                        if ((Base & Base_Uppercase) == Base_Uppercase) {
                            String[CodePoint - 1] = IntegerTableBase16Uppercase[Digit];
                        } else if ((Base & Base_Lowercase) == Base_Lowercase) {
                            String[CodePoint - 1] = IntegerTableBase16Lowercase[Digit];
                        }
                        Radix                     = 16;
                    }
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Base is not an integer, exiting"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Base: %hu is not an integer"), Base);
        }
        return String;
    }
    
    double UTF8_String2Decimal(TextIO_Bases Base, ImmutableString_UTF8 String) {
        double Decimal = 0.0;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            Decimal         = UTF32_String2Decimal(Base, String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF16_String2Decimal(TextIO_Bases Base, ImmutableString_UTF16 String) {
        double Decimal = 0.0;
        if (String != NULL) {
            UTF32 *String32 = UTF16_Decode(String);
            Decimal         = UTF32_String2Decimal(Base, String32);
            free(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF32_String2Decimal(TextIO_Bases Base, ImmutableString_UTF32 String) { // Replaces strtod, strtof, strold, atof, and atof_l
        double   Value         = 0.0;
        bool     IsNegative    = No;
        if (String != NULL) {
            uint64_t CodePoint = 0ULL;
            
            if (String[CodePoint] == UTF32Character('-')) {
                IsNegative = Yes;
            }
            
            while (String[CodePoint] != UTF32Character('.')) {
                CodePoint     += 1;
            } // Before the decimal
            
            while (String[CodePoint] != PlatformIO_NULLTerminator) {
                if (String[0] == UTF32Character('-')) {
                    IsNegative = Yes;
                }
                CodePoint     += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Value;
    }
    
    UTF8 *UTF8_Decimal2String(TextIO_Bases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Decimal, Base);
        UTF8  *String8  = UTF8_Encode(String32);
        free(String32);
        return String8;
    }
    
    UTF16 *UTF16_Decimal2String(TextIO_Bases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Decimal, Base);
        UTF16 *String16 = UTF16_Encode(String32);
        free(String32);
        return String16;
    }
    
    static UTF32 *Decimal2String_UTF32(TextIO_Bases Base, double Decimal, uint64_t MinimumWidth, uint64_t Precision) {
        UTF32 *String = NULL;
        if ((Base & Base_Decimal) == Base_Decimal) {
            uint8_t StringSize        = 0;
            int8_t  Sign              = ExtractSign(Decimal);
            int32_t Exponent          = ExtractExponent(Decimal);
            int64_t Mantissa          = ExtractMantissa(Decimal);
            uint8_t NumExponentDigits = NumDigitsInInteger(10, Exponent);
            uint8_t NumMantissaDigits = NumDigitsInInteger(10, Mantissa);
            
            /*
             Is there a way to calculate the number of digits needed for correct round tripping?
             
             if we could find that, we could bypass all of the other nonsense and just create a correct number right off the bat.
             */
            
            if ((Base & Base_Shortest) == Base_Shortest) {
                if (MinimumWidth == 0 && Precision == 0) {
                    // Ryu
                } else {
                    // Do something stranger
                }
            } else if ((Base & Base_Scientific) == Base_Scientific) {
                if (MinimumWidth == 0 && Precision == 0) {
                    // Ryu
                } else {
                    // Do something stranger
                }
            } else if ((Base & Base_Radix10) == Base_Radix10) {
                if (MinimumWidth == 0 && Precision == 0) {
                    // Ryu
                } else {
                    // Do something stranger
                }
            } else if ((Base & Base_Radix16) == Base_Radix16) {
                StringSize += 2; // + 2 for 0x/0X
                if ((Base & Base_Uppercase) == Base_Uppercase) {
                    if (MinimumWidth == 0 && Precision == 0) {
                        // Ryu
                        if (Sign == -1) {
                            // Prefix a hyphen
                            StringSize += 1;
                        }
                        // Prefix 0X
                        // WriteExponent
                        // Write period
                        // Write Hexadecimal Mantissa
                        // Write P for Power
                        // Write
                    } else {
                        // Do something stranger
                    }
                } else if ((Base & Base_Lowercase) == Base_Lowercase) {
                    if (MinimumWidth == 0 && Precision == 0) {
                        // Ryu
                    } else {
                        // Do something stranger
                    }
                } else {
                    Log(Severity_USER, PlatformIO_FunctionName, UTF8String("Hexadecimal base must be Uppercase or Lowercase"));
                }
            }
        } else if ((Base & Base_Decimal) != Base_Decimal) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Base must be Base_Decimal"));
        } else if (Base == Base_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Base_Unspecified is invalid"));
        }
        return String;
    }
    
    UTF32 *UTF32_Decimal2String(TextIO_Bases Base, double Number) {
        UTF32   *OutputString     = NULL;
        uint64_t StringSize       = 0ULL;
        int8_t   Sign             = ExtractSign(Number);
        int16_t  Exponent         = ExtractExponent(Number);
        int16_t  Exponent2        = Absolute(Exponent);
        int64_t  Mantissa         = ExtractMantissa(Number);
        int64_t  Mantissa2        = Absolute(Mantissa);
        bool     IsDenormal       = DecimalIsNormal(Number);
        bool     IsANumber        = DecimalIsANumber(Number);
        bool     IsInfinite       = DecimalIsInfinity(Number);
        
        /*
         
         Let's start looking at how to minimize the number of digits in a string, and also how to round
         
         Ok, so we always do shortest conversion.
         
         then we further fit it to minwidth and precision.
         
         Now we just need to support the various modes, Scientific, Hex, Decimal, Shortest
         
         Decimal:    3.000003
         Scientific: 3.0E-5? 10^NumZeros if positive, 10^(NumZeros+1) if negative
         
         Maybe there should be a static function that takes the requested format (Scientific, Decimal, Shortest, Hexadecimal), MinWidth, and Precision
         
         Decimal2String_UTF32(double Decimal2Convert, uint64_t MinWidth, uint64_t Precision, TextIO_Bases Base)
         
         */
        
        
        
        
        
        
        
        if (IsANumber) {
            OutputString          = UTF32_Clone(UTF32String("Not A Number"));
        } else if (IsInfinite) {
            OutputString          = UTF32_Clone(UTF32String("Infinity"));
        }
        
        uint8_t NumDigitsExponent = 0;
        uint8_t NumDigitsMantissa = 0;
        uint8_t Radix             = 0;
        if ((Base & Base_Decimal) == Base_Decimal && (Base & Base_Radix2) == Base_Radix2) {
            Radix                 = 2;
            StringSize           += 1;
        } else if ((Base & Base_Decimal) == Base_Decimal && (Base & Base_Radix8) == Base_Radix8) {
            Radix                 = 8;
        } else if ((Base & Base_Decimal) == Base_Decimal && (Base & Base_Radix10) == Base_Radix10) {
            Radix                 = 10;
            if (Sign == -1) {
                StringSize       += 1;
            }
        } else if ((Base & Base_Decimal) == Base_Decimal && (Base & Base_Radix16) == Base_Radix16) {
            Radix                 = 16;
            StringSize           += 7; // FIXME: ???
        }
        
        NumDigitsExponent         = (uint8_t) Exponentiate(Radix, Exponent2);
        NumDigitsMantissa         = (uint8_t) Exponentiate(Radix, Mantissa2);
        
        StringSize       += NumDigitsExponent + NumDigitsMantissa;
        
        OutputString          = UTF32_Init(StringSize);
        if (OutputString != NULL) {
            // Now we go ahead and create the string
            if (Sign == -1) {
                OutputString[1] = UTF32Character('-');
            }
            // Now we start popping in the other variables, first is the Exponent.
            while (Exponent > 0) { // TODO: This assumes there's only 1 codepoint necessary to express the exponent
                for (uint64_t ExponentCodePoint = 0ULL; ExponentCodePoint < NumDigitsExponent; ExponentCodePoint++) {
                    OutputString[ExponentCodePoint + StringSize]                 = Exponent /= 10;
                }
            }
            OutputString[StringSize + NumDigitsExponent + UnicodeBOMSizeInCodePoints] = UTF32Character('.');
            // Now let's start popping in the Mantissa
            while (Mantissa > 0) { // TODO: This assumes there's only 1 codepoint necessary to express the exponent
                for (uint64_t MantissaCodePoint = 0ULL; MantissaCodePoint < NumDigitsExponent; MantissaCodePoint++) {
                    OutputString[StringSize + NumDigitsExponent + MantissaCodePoint]  = Mantissa / 10;
                }
            }
            if ((Base & Base_Radix16) == Base_Radix16 && (Base & Base_Uppercase) == Base_Uppercase) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = UTF32Character('A');
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('P');
            } else if ((Base & Base_Radix16) == Base_Radix16 && (Base & Base_Lowercase) == Base_Lowercase) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = UTF32Character('a');
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('p');
            } else if ((Base & Base_Scientific) == Base_Scientific && (Base & Base_Uppercase) == Base_Uppercase) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = UTF32Character('E');
                // Write the sign, if the number is positive, write a +, otherwise write a -
                if (Sign == -1) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('-');
                } else {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('+');
                }
                // Write the Exponent
                uint16_t NumDigitsExponentInDigits = (uint16_t) Logarithm(2, Exponent);
                UTF32 *ExponentString              = UTF32_Integer2String(Exponent, Base_Integer | Base_Radix10);
                for (uint64_t ExponentDigit = 0ULL; ExponentDigit < NumDigitsExponentInDigits; ExponentDigit++) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2 + ExponentDigit] = ExponentString[ExponentDigit]; // FIXME: "Exponent" is NOT right
                }
            } else if ((Base & Base_Scientific) == Base_Scientific && (Base & Base_Lowercase) == Base_Lowercase) {
                OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('e');
                if (Sign == -1) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('-');
                } else {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('+');
                }
                // Write the Exponent
                uint16_t NumDigitsExponentInDigits = (uint16_t) Logarithm(2, Exponent);
                UTF32 *ExponentString              = UTF32_Integer2String(Exponent, Base_Integer | Base_Radix10);
                for (uint64_t ExponentDigit = 0ULL; ExponentDigit < NumDigitsExponentInDigits; ExponentDigit++) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2 + ExponentDigit] = ExponentString[ExponentDigit]; // FIXME: "Exponent" is NOT right
                }
            }
        }
        return OutputString;
    }
    /* Number Conversions */
    
    bool UTF8_Compare(ImmutableString_UTF8 String1, ImmutableString_UTF8 String2) {
        bool StringsMatch         = No;
        if (String1 != NULL && String2 != NULL) {
            if (String1 != String2) {
                UTF32 *String1_32 = UTF8_Decode(String1);
                UTF32 *String2_32 = UTF8_Decode(String2);
                StringsMatch      = UTF32_Compare(String1_32, String2_32);
                free(String1_32);
                free(String2_32);
            } else {
                StringsMatch      = Yes;
            }
        } else if (String1 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF16_Compare(ImmutableString_UTF16 String1, ImmutableString_UTF16 String2) {
        bool StringsMatch         = No;
        if (String1 != NULL && String2 != NULL) {
            if (String1 != String2) {
                UTF32 *String1_32 = UTF16_Decode(String1);
                UTF32 *String2_32 = UTF16_Decode(String2);
                StringsMatch      = UTF32_Compare(String1_32, String2_32);
                free(String1_32);
                free(String2_32);
            } else {
                StringsMatch      = Yes;
            }
        } else if (String1 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF32_Compare(ImmutableString_UTF32 String1, ImmutableString_UTF32 String2) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF8_CompareSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substring, uint64_t StringOffset, uint64_t SubstringOffset) {
        bool SubstringMatchesAtOffset = No;
        if (String != NULL && Substring != NULL) {
            UTF32 *String32           = UTF8_Decode(String);
            UTF32 *Sub32              = UTF8_Decode(Substring);
            SubstringMatchesAtOffset  = UTF32_CompareSubString(String32, Sub32, StringOffset, SubstringOffset);
            free(String32);
            free(Sub32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substring == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Substring Pointer is NULL"));
        }
        return SubstringMatchesAtOffset;
    }
    
    bool UTF16_CompareSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substring, uint64_t StringOffset, uint64_t SubstringOffset) {
        bool SubstringMatchesAtOffset = No;
        if (String != NULL && Substring != NULL) {
            UTF32 *String32           = UTF16_Decode(String);
            UTF32 *Sub32              = UTF16_Decode(Substring);
            SubstringMatchesAtOffset  = UTF32_CompareSubString(String32, Sub32, StringOffset, SubstringOffset);
            UTF32_Deinit(String32);
            UTF32_Deinit(Sub32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substring == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Substring Pointer is NULL"));
        }
        return SubstringMatchesAtOffset;
    }
    
    bool UTF32_CompareSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substring, uint64_t StringOffset, uint64_t SubstringOffset) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substring == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Substring Pointer is NULL"));
        } else {
            SubstringMatchesAtOffset             = No;
        }
        return SubstringMatchesAtOffset;
    }
    
    UTF8 *UTF8_Trim(ImmutableString_UTF8 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF8 Strings2Remove) {
        UTF8 *Trimmed = NULL;
        if (String != NULL && Type != TruncationType_Unspecified && Strings2Remove != NULL) {
            UTF32    *String32                  = UTF8_Decode(String);
            UTF32   **Strings2Remove32          = UTF8_StringSet_Decode(Strings2Remove);
            UTF32    *Trimmed32                 = UTF32_Trim((ImmutableString_UTF32) String32, Type, Strings2Remove32);
            UTF32_StringSet_Deinit(Strings2Remove32);
            Trimmed                             = UTF8_Encode(Trimmed32);
            UTF32_Deinit(String32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Type == TruncationType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("TruncationType_Unspecified is invalid"));
        } else if (Strings2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF16 *UTF16_Trim(ImmutableString_UTF16 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF16 Strings2Remove) {
        UTF16 *Trimmed = NULL;
        if (String != NULL && Type != TruncationType_Unspecified && Strings2Remove != NULL) {
            UTF32    *String32                  = UTF16_Decode(String);
            UTF32   **Strings2Remove32          = UTF16_StringSet_Decode(Strings2Remove);
            UTF32    *Trimmed32                 = UTF32_Trim(String32, Type, Strings2Remove32);
            UTF32_StringSet_Deinit(Strings2Remove32);
            Trimmed                             = UTF16_Encode(Trimmed32);
            UTF32_Deinit(String32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Type == TruncationType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("TruncationType_Unspecified is invalid"));
        } else if (Strings2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF32 *UTF32_Trim(ImmutableString_UTF32 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF32 Strings2Remove) {
        UTF32 *Trimmed = NULL;
        if (String != NULL && Type != TruncationType_Unspecified && Strings2Remove != NULL) {
            uint64_t   StringSize          = UTF32_GetStringSizeInCodePoints(String);
            uint64_t   NumRemovalStrings   = UTF32_StringSet_GetNumStrings(Strings2Remove);
            uint64_t  *RemovalStringSizes  = UTF32_StringSet_GetStringSizesInCodePoints(Strings2Remove);
            uint64_t   NumRemovalPoints    = 0ULL;
            uint64_t   CurrentRemovalPoint = 0ULL;
            uint64_t  *RemovalPointsStart  = NULL; // RemovalPoint[0] = {0, 6}; start and stop points
            uint64_t  *RemovalPointsEnd    = NULL; // RemovalPoint[0] = {0, 6}; start and stop points
            uint64_t   TrimmedStringSize   = 0ULL;
            if (Type == TruncationType_All) {
                // Loop over all the codepoints until you find one that is not on the list, then remove it; BUT JUST FOR THE BEGINNING AND END
                for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                        bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                        if (SubstringFound) {
                            NumRemovalPoints += 1;
                        }
                    }
                }
                
                RemovalPointsStart                                  = (uint64_t*) calloc(NumRemovalPoints, sizeof(uint64_t));
                RemovalPointsEnd                                    = (uint64_t*) calloc(NumRemovalPoints, sizeof(uint64_t));
                
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
            } else if (Type == TruncationType_Most1) {
                // Loop over all the codepoints until you find one that is not on the list, then remove it.
                for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (uint64_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                        bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                        if (SubstringFound) {
                            NumRemovalPoints += 1;
                        }
                    }
                }
                
                RemovalPointsStart                                  = (uint64_t*) calloc(NumRemovalPoints, sizeof(uint64_t));
                RemovalPointsEnd                                    = (uint64_t*) calloc(NumRemovalPoints, sizeof(uint64_t));
                
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
            // Actually build the string, regardless of mode.
            TrimmedStringSize = StringSize;
            for (uint64_t RemovalPoint = 0ULL; RemovalPoint < NumRemovalPoints; RemovalPoint++) {
                TrimmedStringSize     -= RemovalPointsEnd[RemovalPoint] - RemovalPointsStart[RemovalPoint];
            }
            
            Trimmed                    = UTF32_Init(TrimmedStringSize);
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
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate Trimmed string"));
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Type == TruncationType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("TruncationType_Unspecified is invalid"));
        } else if (Strings2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Strings2Remove Pointer is NULL"));
        }
        return Trimmed;
    }
    
    UTF8 **UTF8_Split(ImmutableString_UTF8 String, ImmutableStringSet_UTF8 Delimiters) {
        UTF8 **SplitString                                 = NULL;
        if (String != NULL && Delimiters != NULL) {
            ImmutableString_UTF32  String32        = UTF8_Decode(String);
            UTF32 **Delimiters32  = UTF8_StringSet_Decode(Delimiters);
            UTF32 **SplitString32 = UTF32_Split(String32, Delimiters32);
            UTF32_Deinit((UTF32*) String32);
            UTF32_StringSet_Deinit((UTF32**) Delimiters32);
            SplitString           = UTF8_StringSet_Encode(SplitString32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Delimiters Pointer is NULL"));
        }
        return SplitString;
    }
    
    UTF16 **UTF16_Split(ImmutableString_UTF16 String, ImmutableStringSet_UTF16 Delimiters) {
        UTF16 **SplitString       = NULL;
        if (String != NULL && Delimiters != NULL) {
            UTF32  *String32      = UTF16_Decode(String);
            UTF32 **Delimiters32  = UTF16_StringSet_Decode(Delimiters);
            UTF32 **SplitString32 = UTF32_Split(String32, Delimiters32);
            UTF32_Deinit(String32);
            UTF32_StringSet_Deinit((UTF32**) Delimiters32);
            SplitString           = UTF16_StringSet_Encode(SplitString32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Delimiters Pointer is NULL"));
        }
        return SplitString;
    }
    
    UTF32 **UTF32_Split(ImmutableString_UTF32 String, ImmutableStringSet_UTF32 Delimiters) {
        UTF32    **SplitStrings    = NULL; // What we return, it's a 0 indexed array of strings
        uint64_t   StringSize      = 0ULL; // The size of the first parameter
        uint64_t   NumDelimiters   = 0ULL; // The number of delimiters in the second parameter
        uint64_t   NumSplitStrings = 0ULL; // The number of strings to return
        uint64_t  *SplitSizes      = NULL; // The size of each split string
        uint64_t  *SplitOffsets    = NULL; // The starting position of each split
        if (String != NULL && Delimiters != NULL) {
            StringSize             = UTF32_GetStringSizeInCodePoints(String);
            NumDelimiters          = UTF32_StringSet_GetNumStrings(Delimiters);
            uint64_t *DelimitersSize = calloc(NumDelimiters, sizeof(uint64_t));
            for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                DelimitersSize[Delimiter] = UTF32_GetStringSizeInCodePoints(Delimiters[Delimiter]);
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Delimiters Pointer is NULL"));
        }
        free(SplitSizes);
        free(SplitOffsets);
        return SplitStrings;
    }
    
    uint64_t UTF8_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF8 String) {
        uint64_t NumDigits      = 0ULL;
        if (String != NULL) {
            uint64_t CodePoint  = 0;
            
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= '0' && String[CodePoint] <= '1') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= '0' && String[CodePoint] <= '7') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= '0' && String[CodePoint] <= '9') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'A' && String[CodePoint] <= 'F')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'a' && String[CodePoint] <= 'f')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            } else if ((Base & Base_Decimal) == Base_Decimal) {
                if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Scientific) == Base_Scientific || (Base & Base_Shortest) == Base_Shortest) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.' || String[CodePoint] == 'E')) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.' || String[CodePoint] == 'e')) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'A' && String[CodePoint] <= 'F') || String[CodePoint] <= 'P' || String[CodePoint] <= 'X') {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'a' && String[CodePoint] <= 'f') || String[CodePoint] <= 'p' || String[CodePoint] <= 'x') {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumDigits;
    }
    
    uint64_t UTF16_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF16 String) {
        uint64_t NumDigits      = 0ULL;
        if (String != NULL) {
            uint64_t CodePoint  = 0;
            
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('1')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('7')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('A') && String[CodePoint] <= UTF16Character('F'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('a') && String[CodePoint] <= UTF16Character('f'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            } else if ((Base & Base_Decimal) == Base_Decimal) {
                if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Scientific) == Base_Scientific || (Base & Base_Shortest) == Base_Shortest) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.') || String[CodePoint] == UTF16Character('E'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.') || String[CodePoint] == UTF16Character('e'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('A') && String[CodePoint] <= UTF16Character('F')) || String[CodePoint] <= UTF16Character('P') || String[CodePoint] <= UTF16Character('X')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('a') && String[CodePoint] <= UTF16Character('f')) || String[CodePoint] <= UTF16Character('p') || String[CodePoint] <= UTF16Character('x')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumDigits;
    }
    
    uint64_t UTF32_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF32 String) {
        uint64_t NumDigits      = 0ULL;
        if (String != NULL) {
            uint64_t CodePoint  = 0;
            
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('1')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('7')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            } else if ((Base & Base_Decimal) == Base_Decimal) {
                if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != PlatformIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Scientific) == Base_Scientific || (Base & Base_Shortest) == Base_Shortest) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.') || String[CodePoint] == UTF32Character('E'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.') || String[CodePoint] == UTF32Character('e'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F')) || String[CodePoint] <= UTF32Character('P') || String[CodePoint] <= UTF32Character('X')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != PlatformIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f')) || String[CodePoint] <= UTF32Character('p') || String[CodePoint] <= UTF32Character('x')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumDigits;
    }
    
    uint64_t UTF32_GetSubStringLength(ImmutableString_UTF32 Format, ImmutableString_UTF32 Formatted, uint64_t Offset) {
        uint64_t Length        = 0ULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t CodePoint = Offset;
            
            while ((Format[CodePoint] != Formatted[CodePoint]) && (Format[CodePoint] != PlatformIO_NULLTerminator && Formatted[CodePoint] != PlatformIO_NULLTerminator)) {
                CodePoint += 1;
                Length    += 1;
            }
        } else if (Format == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Format String Pointer is NUlL"));
        } else if (Formatted == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Formatted String Pointer is NULL"));
        }
        return Length;
    }

    /* Unicode Conversion */
    CharSet8 *UTF8_ConvertUnicode2CharSet(ImmutableString_UTF8 String, StringIO_CharSets CodePage) {
        CharSet8 *Encoded = NULL;
        if (String != NULL && CodePage != CharSet_Unspecified) {
            UTF32 *Decoded      = UTF8_Decode(String);
            UTF32 *Composed     = UTF32_Normalize(Decoded, NormalizationForm_KompatibleCompose);
            uint64_t Characters = 0ULL;
            uint64_t Character  = 0ULL;
            if (CodePage == CharSet_ISO_8859_1) {
                while (Composed[Characters] != PlatformIO_NULLTerminator) {
                    if (Composed[Characters] <= 0xFF) {
                        Characters += 1;
                    }
                }
                // Ok so now we have our number of characters, init a new string with that many and copy em over
                Encoded = UTF8_Init(Characters);
                while (Composed[Character] != PlatformIO_NULLTerminator && Character < Characters) {
                    if (Composed[Characters] <= 0xFF) {
                        Encoded[Character] = Composed[Characters] & 0xFF;
                        Character  += 1;
                        Characters += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CharSet_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Encoded;
    }

    CharSet16 *UTF16_ConvertUnicode2CharSet(ImmutableString_UTF16 String, StringIO_CharSets CodePage) {
        CharSet16 *Encoded = NULL;
        if (String != NULL && CodePage != CharSet_Unspecified) {
            UTF32 *Decoded      = UTF16_Decode(String);
            UTF32 *Composed     = UTF32_Normalize(Decoded, NormalizationForm_KompatibleCompose);
            uint64_t Characters = 0ULL;
            uint64_t Character  = 0ULL;
            if (CodePage == CharSet_ISO_8859_1) {
                while (Composed[Characters] != PlatformIO_NULLTerminator) {
                    if (Composed[Characters] <= 0xFF) {
                        Characters += 1;
                    }
                }
                // Ok so now we have our number of characters, init a new string with that many and copy em over
                Encoded = UTF16_Init(Characters);
                while (Composed[Character] != PlatformIO_NULLTerminator && Character < Characters) {
                    if (Composed[Characters] <= 0xFF) {
                        Encoded[Character] = Composed[Characters] & 0xFF;
                        Character  += 1;
                        Characters += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CharSet_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Encoded;
    }

    CharSet32 *UTF32_ConvertUnicode2CharSet(ImmutableString_UTF32 String, StringIO_CharSets CodePage) {
        CharSet32 *Encoded = NULL;
        if (String != NULL && CodePage != CharSet_Unspecified) {
            UTF32 *Composed     = UTF32_Normalize(String, NormalizationForm_KompatibleCompose);
            uint64_t Characters = 0ULL;
            uint64_t Character  = 0ULL;
            if (CodePage == CharSet_ISO_8859_1) {
                while (Composed[Characters] != PlatformIO_NULLTerminator) {
                    if (Composed[Characters] <= 0xFF) {
                        Characters += 1;
                    }
                }
                // Ok so now we have our number of characters, init a new string with that many and copy em over
                Encoded = UTF32_Init(Characters);
                while (Composed[Character] != PlatformIO_NULLTerminator && Character < Characters) {
                    if (Composed[Characters] <= 0xFF) {
                        Encoded[Character] = Composed[Characters] & 0xFF;
                        Character  += 1;
                        Characters += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CharSet_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Encoded;
    }

    UTF8 *UTF8_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet8 *) String, StringIO_CharSets CodePage) {
        UTF8 *Unicode = NULL;
        if (String != NULL && CodePage != CharSet_Unspecified) {
            UTF32   *Decoded          = UTF8_Decode(String);
            uint64_t Character        = 0ULL;
            uint64_t CodePoint        = 0ULL;
            uint64_t NumCodeUnits     = 0ULL;
            if (CodePage == CharSet_ISO_8859_1) {
                while (Decoded[Character] != PlatformIO_NULLTerminator) {
                    if (Decoded[Character] == KompatibleNormalizationTable[Character][0][0]) { // Kompatible is just a standin here for the actual table
                        NumCodeUnits += UTF32_GetCodePointSizeInUTF8CodeUnits(Decoded[Character]);
                    }
                    Character        += 1;
                }
                Unicode               = UTF8_Init(NumCodeUnits);
                while (Decoded[Character] != PlatformIO_NULLTerminator && Unicode[CodePoint] != PlatformIO_NULLTerminator) {
                    for (uint8_t Byte = 0; Byte < UTF32_GetCodePointSizeInUTF8CodeUnits(Decoded[Character]); Byte++) {
                        Unicode[CodePoint + Byte] = StringIO_PreallocateCodePoint_UTF8[Byte];
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CharSet_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Unicode;
    }

    UTF16 *UTF16_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet16 *) String, StringIO_CharSets CodePage) {
        UTF16 *Unicode = NULL;
        if (String != NULL && CodePage != CharSet_Unspecified) {
            UTF32   *Decoded          = UTF16_Decode(String);
            uint64_t Character        = 0ULL;
            uint64_t CodePoint        = 0ULL;
            uint64_t NumCodeUnits     = 0ULL;
            if (CodePage == CharSet_ISO_8859_1) {
                while (Decoded[Character] != PlatformIO_NULLTerminator) {
                    if (Decoded[Character] == KompatibleNormalizationTable[Character][0][0]) { // Kompatible is just a standin here for the actual table
                        NumCodeUnits += UTF32_GetCodePointSizeInUTF8CodeUnits(Decoded[Character]);
                    }
                    Character        += 1;
                }
                Unicode               = UTF16_Init(NumCodeUnits);
                while (Decoded[Character] != PlatformIO_NULLTerminator && Unicode[CodePoint] != PlatformIO_NULLTerminator) {
                    for (uint8_t Byte = 0; Byte < UTF32_GetCodePointSizeInUTF16CodeUnits(Decoded[Character]); Byte++) {
                        Unicode[CodePoint + Byte] = StringIO_PreallocateCodePoint_UTF16[Byte];
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CharSet_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Unicode;
    }

    UTF32 *UTF32_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet32 *) String, StringIO_CharSets CodePage) {
        UTF32 *Unicode = NULL;
        if (String != NULL && CodePage != CharSet_Unspecified) {
            uint64_t Character        = 0ULL;
            uint64_t CodePoint        = 0ULL;
            uint64_t NumCodeUnits     = 0ULL;
            if (CodePage == CharSet_ISO_8859_1) {
                while (String[Character] != PlatformIO_NULLTerminator) {
                    if (String[Character] == KompatibleNormalizationTable[Character][0][0]) { // Kompatible is just a standin here for the actual table
                        NumCodeUnits += 1;
                    }
                    Character        += 1;
                }
                Unicode               = UTF32_Init(NumCodeUnits);
                while (String[Character] != PlatformIO_NULLTerminator && Unicode[CodePoint] != PlatformIO_NULLTerminator) {
                    Unicode[CodePoint] = String[Character];
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CharSet_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Unicode;
    }
    /* Unicode Conversion */
    
    void UTF8_Deinit(UTF8 *String) {
        if (String != NULL) {
            free(String);
        }
    }
    
    void UTF16_Deinit(UTF16 *String) {
        if (String != NULL) {
            free(String);
        }
    }
    
    void UTF32_Deinit(UTF32 *String) {
        if (String != NULL) {
            free(String);
        }
    }
    
    /* StringSet Functions */
    UTF8 **UTF8_StringSet_Init(uint64_t NumStrings) {
        UTF8 **StringSet = NULL;
        if (NumStrings > 0) {
            StringSet    = calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(UTF8*));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSet[String] = (UTF8*) 0x8888888888888888;
            }
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumStrings %llu is invalid"), NumStrings);
        }
        return StringSet;
    }
    
    UTF16 **UTF16_StringSet_Init(uint64_t NumStrings) {
        UTF16 **StringSet = NULL;
        if (NumStrings > 0) {
            StringSet     = (UTF16**) calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(UTF16*));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSet[String] = (UTF16*) 0x1616161616161616;
            }
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumStrings %llu is invalid"), NumStrings);
        }
        return StringSet;
    }
    
    UTF32 **UTF32_StringSet_Init(uint64_t NumStrings) {
        UTF32 **StringSet = NULL;
        if (NumStrings > 0) {
            StringSet     = (UTF32**) calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(UTF32*));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSet[String] = (UTF32*) 0x3232323232323232;
            }
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumStrings %llu is invalid"), NumStrings);
        }
        return StringSet;
    }
    
    bool UTF8_StringSet_Attach(UTF8 **StringSet, UTF8 *String2Attach, uint64_t Index) {
        bool AttachedSucessfully = No;
        if (StringSet != NULL && String2Attach != NULL) {
            uint64_t NumStrings  = 0ULL;
            while (StringSet[NumStrings] != PlatformIO_NULLTerminator) {
                NumStrings      += 1;
            }
            if (NumStrings >= Index) {
                StringSet[Index] =  String2Attach;
                AttachedSucessfully = Yes;
            }
        } else if (StringSet == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String2Attach Pointer is NULL"));
        }
        return AttachedSucessfully;
    }
    
    bool UTF16_StringSet_Attach(UTF16 **StringSet, UTF16 *String2Attach, uint64_t Index) {
        bool AttachedSucessfully = No;
        if (StringSet != NULL && String2Attach != NULL) {
            uint64_t NumStrings  = 0ULL;
            while (StringSet[NumStrings] != PlatformIO_NULLTerminator) {
                NumStrings      += 1;
            }
            if (NumStrings >= Index) {
                StringSet[Index] = String2Attach;
                AttachedSucessfully = Yes;
            }
        } else if (StringSet == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String2Attach Pointer is NULL"));
        }
        return AttachedSucessfully;
    }
    
    bool UTF32_StringSet_Attach(UTF32 **StringSet, UTF32 *String2Attach, uint64_t Index) {
        bool AttachedSucessfully = No;
        if (StringSet != NULL && String2Attach != NULL) {
            uint64_t NumStrings  = 0ULL;
            while (StringSet[NumStrings] != PlatformIO_NULLTerminator) {
                NumStrings      += 1;
            }
            if (NumStrings >= Index) {
                StringSet[Index] = (UTF32*) String2Attach;
                AttachedSucessfully = Yes;
            }
        } else if (StringSet == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        } else if (String2Attach == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String2Attach Pointer is NULL"));
        }
        return AttachedSucessfully;
    }
    
    uint64_t UTF8_StringSet_GetNumStrings(ImmutableStringSet_UTF8 StringSet) {
        uint64_t NumStrings = 0ULL;
        if (StringSet != NULL) {
            while (StringSet[NumStrings][0] != PlatformIO_NULLTerminator) {
                NumStrings += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t UTF16_StringSet_GetNumStrings(ImmutableStringSet_UTF16 StringSet) {
        uint64_t NumStrings = 0ULL;
        if (StringSet != NULL) {
            while (StringSet[NumStrings][0] != PlatformIO_NULLTerminator) {
                NumStrings += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t UTF32_StringSet_GetNumStrings(ImmutableStringSet_UTF32 StringSet) {
        uint64_t NumStrings = 0ULL;
        if (StringSet != NULL) {
            while (StringSet[NumStrings][0] != PlatformIO_NULLTerminator) {
                NumStrings += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return NumStrings;
    }
    
    uint64_t *UTF8_StringSet_GetStringSizesInCodeUnits(ImmutableStringSet_UTF8 StringSet) {
        uint64_t *StringSetSizes       = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings        = UTF8_StringSet_GetNumStrings(StringSet);
            StringSetSizes             = (uint64_t*) calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSetSizes[String] = UTF8_GetStringSizeInCodeUnits(StringSet[String]);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return StringSetSizes;
    }
    
    uint64_t *UTF16_StringSet_GetStringSizesInCodeUnits(ImmutableStringSet_UTF16 StringSet) {
        uint64_t *StringSetSizes       = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings        = UTF16_StringSet_GetNumStrings(StringSet);
            StringSetSizes             = (uint64_t*) calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSetSizes[String] = UTF16_GetStringSizeInCodeUnits(StringSet[String]);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return StringSetSizes;
    }
    
    uint64_t *UTF8_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF8 StringSet) {
        uint64_t *StringSetSizes       = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings        = UTF8_StringSet_GetNumStrings(StringSet);
            StringSetSizes             = (uint64_t*) calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSetSizes[String] = UTF8_GetStringSizeInCodePoints(StringSet[String]);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return StringSetSizes;
    }
    
    uint64_t *UTF16_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF16 StringSet) {
        uint64_t *StringSetSizes       = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings        = UTF16_StringSet_GetNumStrings(StringSet);
            StringSetSizes             = (uint64_t*) calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSetSizes[String] = UTF16_GetStringSizeInCodePoints(StringSet[String]);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return StringSetSizes;
    }
    
    uint64_t *UTF32_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF32 StringSet) {
        uint64_t *StringSetSizes       = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings        = UTF32_StringSet_GetNumStrings(StringSet);
            StringSetSizes             = (uint64_t*) calloc(NumStrings + PlatformIO_NULLTerminatorSize, sizeof(uint64_t));
            for (uint64_t String = 0ULL; String < NumStrings; String++) {
                StringSetSizes[String] = UTF32_GetStringSizeInCodePoints(StringSet[String]);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return StringSetSizes;
    }
    
    UTF32 **UTF8_StringSet_Decode(ImmutableStringSet_UTF8 StringSet) {
        UTF32 **Decoded             = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings     = UTF8_StringSet_GetNumStrings(StringSet);
            Decoded                 = UTF32_StringSet_Init(NumStrings);
            if (Decoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Decoded[String] = UTF8_Decode(StringSet[String]);
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate decoded StringSet"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return Decoded;
    }
    
    UTF32 **UTF16_StringSet_Decode(ImmutableStringSet_UTF16 StringSet) {
        UTF32 **Decoded             = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings     = UTF16_StringSet_GetNumStrings(StringSet);
            Decoded                 = UTF32_StringSet_Init(NumStrings);
            if (Decoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Decoded[String] = UTF16_Decode(StringSet[String]);
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate decoded StringSet"));
            }
            
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return Decoded;
    }
    
    UTF8 **UTF8_StringSet_Encode(MutableStringSet_UTF32 StringSet) {
        UTF8 **Encoded              = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings     = UTF32_StringSet_GetNumStrings(StringSet);
            Encoded                 = UTF8_StringSet_Init(NumStrings);
            if (Encoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Encoded[String] = UTF8_Encode(StringSet[String]);
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate decoded StringSet"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return Encoded;
    }
    
    UTF16 **UTF16_StringSet_Encode(MutableStringSet_UTF32 StringSet) {
        UTF16 **Encoded             = NULL;
        if (StringSet != NULL) {
            uint64_t NumStrings     = UTF32_StringSet_GetNumStrings(StringSet);
            Encoded                 = UTF16_StringSet_Init(NumStrings);
            if (Encoded != NULL) {
                for (uint64_t String = 0ULL; String < NumStrings; String++) {
                    Encoded[String] = UTF16_Encode(StringSet[String]);
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate decoded StringSet"));
            }
            
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return Encoded;
    }
    
    UTF8 *UTF8_StringSet_Flatten(ImmutableStringSet_UTF8 StringSet) {
        UTF8 *Flattened = NULL;
        if (StringSet != NULL) {
            UTF32 **StringSet32 = UTF8_StringSet_Decode(StringSet);
            UTF32  *Flattened32 = UTF32_StringSet_Flatten(StringSet32);
            UTF32_StringSet_Deinit(StringSet32);
            Flattened           = UTF8_Encode(Flattened32);
            UTF32_Deinit(Flattened32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return Flattened;
    }
    
    UTF16 *UTF16_StringSet_Flatten(ImmutableStringSet_UTF16 StringSet) {
        UTF16 *Flattened        = NULL;
        if (StringSet != NULL) {
            UTF32 **StringSet32 = UTF16_StringSet_Decode(StringSet);
            UTF32  *Flattened32 = UTF32_StringSet_Flatten(StringSet32);
            UTF32_StringSet_Deinit(StringSet32);
            Flattened           = UTF16_Encode(Flattened32);
            UTF32_Deinit(Flattened32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return Flattened;
    }
    
    UTF32 *UTF32_StringSet_Flatten(ImmutableStringSet_UTF32 StringSet) {
        UTF32 *Flattened = NULL;
        if (StringSet != NULL) {
            uint64_t  NumStrings         = UTF32_StringSet_GetNumStrings(StringSet);
            uint64_t  FlattenedSize      = 0;
            uint64_t *PieceSizes         = calloc(NumStrings, sizeof(uint64_t));
            for (uint64_t String = 0; String < NumStrings; String++) {
                PieceSizes[String]       = UTF32_GetStringSizeInCodePoints(StringSet[String]);
                FlattenedSize           += PieceSizes[String];
            }
            Flattened                    = UTF32_Init(FlattenedSize);
            uint64_t FlattenedOffset     = 0;
            uint64_t CurrentPiece        = 0;
            uint64_t CurrentPieceOffset  = 0;
            if (Flattened != NULL) {
                while (FlattenedOffset < FlattenedSize && CurrentPieceOffset < PieceSizes[CurrentPiece]) {
                    Flattened[FlattenedOffset] = StringSet[CurrentPiece][CurrentPieceOffset];
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate Flattened string of size: %llu"), FlattenedSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
        return Flattened;
    }
    
    void UTF8_StringSet_Deinit(UTF8 **StringSet) {
        if (StringSet != NULL) {
            uint64_t String = 0ULL;
            while (StringSet[String] != PlatformIO_NULLTerminator) {
                free(StringSet[String]);
                String += 1;
            }
            free(StringSet);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
    }
    
    void UTF16_StringSet_Deinit(UTF16 **StringSet) {
        if (StringSet != NULL) {
            uint64_t String = 0ULL;
            while (StringSet[String] != PlatformIO_NULLTerminator) {
                free(StringSet[String]);
                String += 1;
            }
            free(StringSet);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
    }
    
    void UTF32_StringSet_Deinit(UTF32 **StringSet) {
        if (StringSet != NULL) {
            uint64_t String = 0ULL;
            while (StringSet[String] != PlatformIO_NULLTerminator) {
                free(StringSet[String]);
                String += 1;
            }
            free(StringSet);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
    }
    /* StringSet Functions */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
