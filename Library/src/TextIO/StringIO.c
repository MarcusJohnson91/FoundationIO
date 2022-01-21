#include "../../include/TextIO/StringIO.h"             /* Included for our declarations */

#include "../../include/BufferIO.h"                    /* Included for BufferIO_MemorySet */
#include "../../include/FileIO.h"                      /* Included for File operations */
#include "../../include/MathIO.h"                      /* Included for endian swapping */
#include "../../include/TextIO/LogIO.h"                /* Included for error logging */
#include "../../include/TextIO/Private/TextIOTables.h" /* Included for the Text tables */
#include "../../include/TextIO/StringSetIO.h"          /* Included for StringSet support */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum StringIOConstants {
        UTF8Header_NumCodeUnits = 0xF8,
        UTF8Header_4CodeUnits   = 0xF0,
        UTF8Header_3CodeUnits   = 0xE0,
        UTF8Header_2CodeUnits   = 0xC0,
        UTF8Header_Contine      = 0x80,
        UTF8Max_ASCII           = 0x7F,
        UTF8Mask6Bit            = 0x3F,
        UTF8Mask5Bit            = 0x1F,
        UTF8Mask4Bit            = 0xF,
        UTF8Mask3Bit            = 0x7,
        UTF8_Debug_Text_8       = 0x38,
        UTF16_Debug_Text_16BE   = 0x3136,
        UTF16_Debug_Text_16LE   = 0x3631,
        UTF32_Debug_Text_32BE   = 0x3332,
        UTF32_Debug_Text_32LE   = 0x3233,
    } StringIOConstants;
    
    static UTF8  StringIO_PreallocateCodePoint_UTF8[UTF8MaxCodeUnitsInCodePoint   + TextIO_NULLTerminatorSize] = {0, 0, 0, 0, 0};
    static UTF16 StringIO_PreallocateCodePoint_UTF16[UTF16MaxCodeUnitsInCodePoint + TextIO_NULLTerminatorSize] = {0, 0, 0};

    static void UTF8_Clear_Preallocated(void) {
        for (uint8_t CodeUnit = 0; CodeUnit < UTF8MaxCodeUnitsInCodePoint + TextIO_NULLTerminatorSize; CodeUnit++) {
            StringIO_PreallocateCodePoint_UTF8[CodeUnit] = 0;
        }
    }

    static void UTF16_Clear_Preallocated(void) {
        for (uint8_t CodeUnit = 0; CodeUnit < UTF16MaxCodeUnitsInCodePoint + TextIO_NULLTerminatorSize; CodeUnit++) {
            StringIO_PreallocateCodePoint_UTF16[CodeUnit] = 0;
        }
    }

    static UTF8 *UTF8_ExtractCodePointAsCodeUnits(ImmutableString_UTF8 String) {
        UTF8 *CodeUnits                                       = NULL;
        if (String != NULL && String[0] != TextIO_NULLTerminator) {
            UTF8_Clear_Preallocated();
            switch (UTF8_GetCodePointSizeInCodeUnits(String[0])) {
                case 1:
                    StringIO_PreallocateCodePoint_UTF8[0]     = String[0];
                    CodeUnits                                 = StringIO_PreallocateCodePoint_UTF8;
                    break;
                case 2:
                    if (String[1] != TextIO_NULLTerminator) {
                        StringIO_PreallocateCodePoint_UTF8[0] = String[0];
                        StringIO_PreallocateCodePoint_UTF8[1] = String[1];
                        CodeUnits                             = StringIO_PreallocateCodePoint_UTF8;
                    }
                    break;
                case 3:
                    if (String[1] != TextIO_NULLTerminator && String[2] != TextIO_NULLTerminator) {
                        StringIO_PreallocateCodePoint_UTF8[0] = String[0];
                        StringIO_PreallocateCodePoint_UTF8[1] = String[1];
                        StringIO_PreallocateCodePoint_UTF8[2] = String[2];
                        CodeUnits                             = StringIO_PreallocateCodePoint_UTF8;
                    }
                    break;
                case 4:
                    if (String[1] != TextIO_NULLTerminator && String[2] != TextIO_NULLTerminator && String[3] != TextIO_NULLTerminator) {
                        StringIO_PreallocateCodePoint_UTF8[0] = String[0];
                        StringIO_PreallocateCodePoint_UTF8[1] = String[1];
                        StringIO_PreallocateCodePoint_UTF8[2] = String[2];
                        StringIO_PreallocateCodePoint_UTF8[3] = String[3];
                        CodeUnits                             = StringIO_PreallocateCodePoint_UTF8;
                    }
                    break;
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CodeUnits;
    }

    static UTF16 *UTF16_ExtractCodePointAsCodeUnits(ImmutableString_UTF16 String) {
        UTF16 *CodeUnits                                   = NULL;
        if (String != NULL && String[0] != TextIO_NULLTerminator) {
            UTF16_Clear_Preallocated();
            if (String[0] < UTF16HighSurrogateStart || String[0] > UTF16LowSurrogateEnd) {
                StringIO_PreallocateCodePoint_UTF16[0]     = String[0];
                CodeUnits                                  = StringIO_PreallocateCodePoint_UTF16;
            } else {
                if (String[1] != TextIO_NULLTerminator) {
                    StringIO_PreallocateCodePoint_UTF16[0] = String[0];
                    StringIO_PreallocateCodePoint_UTF16[1] = String[1];
                    CodeUnits                              = StringIO_PreallocateCodePoint_UTF16;
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CodeUnits;
    }

    static UTF32 UTF32_ExtractCodePoint(ImmutableString_UTF32 String) {
        UTF32 CodePoint         = 0;
        if (String != NULL && String[0] != TextIO_NULLTerminator) {
            CodePoint           = String[0];
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF8 *UTF8_Init(size_t NumCodeUnits) {
        UTF8 *String            = NULL;
        if (NumCodeUnits >= 1) {
            size_t StringSize   = NumCodeUnits + TextIO_NULLTerminatorSize;
            String              = (UTF8*) calloc(StringSize, sizeof(UTF8));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            BufferIO_MemorySet8(String, UTF8_Debug_Text_8, NumCodeUnits);
#endif
            if (String == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Allocation failure: Couldn't allocate %zu bytes"), StringSize * sizeof(UTF8));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid number of CodeUnits to allocate: %zu"), NumCodeUnits);
        }
        return String;
    }
    
    UTF16 *UTF16_Init(size_t NumCodeUnits) {
        UTF16 *String           = NULL;
        if (NumCodeUnits >= 1) {
            size_t StringSize   = NumCodeUnits + TextIO_NULLTerminatorSize;
            String              = (UTF16*) calloc(StringSize, sizeof(UTF16));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_ByteOrder == PlatformIO_TargetByteOrderIsBE)
            BufferIO_MemorySet16(String, UTF16_Debug_Text_16BE, StringSize);
#elif (PlatformIO_ByteOrder == PlatformIO_TargetByteOrderIsLE)
            BufferIO_MemorySet16(String, UTF16_Debug_Text_16LE, StringSize);
#endif /* ByteOrder */
#endif /* Debug */
            if (String == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Allocation failure: Couldn't allocate %zu bytes"), StringSize * sizeof(UTF16));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid number of CodeUnits to allocate: %zu"), NumCodeUnits);
        }
        return String;
    }
    
    UTF32 *UTF32_Init(size_t NumCodePoints) {
        UTF32 *String           = NULL;
        if (NumCodePoints >= 1) {
            size_t StringSize   = NumCodePoints + TextIO_NULLTerminatorSize;
            String              = (UTF32*) calloc(StringSize, sizeof(UTF32));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_ByteOrder == PlatformIO_TargetByteOrderIsBE)
            BufferIO_MemorySet32(String, UTF32_Debug_Text_32BE, StringSize);
#elif (PlatformIO_ByteOrder == PlatformIO_TargetByteOrderIsLE)
            BufferIO_MemorySet32(String, UTF32_Debug_Text_32LE, StringSize);
#endif /* ByteOrder */
#endif /* Debug */
            if (String == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Allocation failure: Couldn't allocate %zu bytes"), StringSize * sizeof(UTF32));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid number of CodePoints to allocate: %zu"), NumCodePoints);
        }
        return String;
    }

    uint64_t UTF8_Hash(UTF8 *String) {
        uint64_t Hash     = 0;
        size_t   CodeUnit = 0;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            Hash         += (UTF8_ExtractCodePoint(UTF8_ExtractCodePointAsCodeUnits(&String[CodeUnit])) * 0x935d50f3) % 0x25D56F;
            CodeUnit     += UTF8_GetCodePointSizeInCodeUnits(String[CodeUnit]);
        }
        return Hash;
    }

    uint64_t UTF16_Hash(UTF16 *String) {
        uint64_t Hash     = 0;
        size_t   CodeUnit = 0;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            Hash         += (UTF16_ExtractCodePoint(UTF16_ExtractCodePointAsCodeUnits(&String[CodeUnit])) * 0x935d50f3) % 0x25D56F;
            CodeUnit     += UTF16_GetCodePointSizeInCodeUnits(String[CodeUnit]);
        }
        return Hash;
    }

    uint64_t UTF32_Hash(UTF32 *String) {
        uint64_t Hash      = 0;
        size_t   CodePoint = 0;
        while (String[CodePoint] != TextIO_NULLTerminator) {
            Hash          += (UTF32_ExtractCodePoint(&String[CodePoint]) * 0x935d50f3) % 0x25D56F;
            CodePoint     += 1;
        }
        return Hash;
    }
    
    void UTF8_Set(UTF8 *String, UTF8 Value, size_t NumCodeUnits) {
        BufferIO_MemorySet8((uint8_t*) String, (uint8_t) Value, NumCodeUnits);
    }
    
    void UTF16_Set(UTF16 *String, UTF16 Value, size_t NumCodeUnits) {
        BufferIO_MemorySet16((uint16_t*) String, (uint16_t) Value, NumCodeUnits);
    }
    
    void UTF32_Set(UTF32 *String, UTF32 Value, size_t NumCodePoints) {
        BufferIO_MemorySet32((uint32_t*) String, (uint32_t) Value, NumCodePoints);
    }
    
    uint8_t UTF8_GetCodePointSizeInCodeUnits(UTF8 CodeUnit) {
        uint8_t CodePointSize = 0;
        switch (CodeUnit & UTF8Header_NumCodeUnits) {
            case UTF8Header_4CodeUnits:
                CodePointSize = 4;
                break;
            case UTF8Header_3CodeUnits:
                CodePointSize = 3;
                break;
            case UTF8Header_2CodeUnits:
                CodePointSize = 2;
                break;
            default:
                CodePointSize = 1;
                break;
        }
        return CodePointSize;
    }
    
    uint8_t UTF16_GetCodePointSizeInCodeUnits(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit < UTF16HighSurrogateStart || (CodeUnit > UTF16LowSurrogateEnd && CodeUnit <= UTF16MaxCodeUnitValue)) {
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
        if (CodePoint <= UTF16MaxCodeUnitValue) {
            UTF16CodeUnits     = 1;
        } else if (CodePoint >= 0x10000 && CodePoint <= UnicodeMaxCodePoint) {
            UTF16CodeUnits     = 2;
        }
        return UTF16CodeUnits;
    }
    
    UTF32 UTF8_ExtractCodePoint(ImmutableString_UTF8 CodeUnits) { // The CodeUnits have already been checked
        UTF32 CodePoint                       = 0;
        if (CodeUnits != NULL) {
            UTF8_Clear_Preallocated();
            UTF8 *Extracted                   = UTF8_ExtractCodePointAsCodeUnits(CodeUnits);
            switch (UTF8_GetCodePointSizeInCodeUnits(CodeUnits[0])) {
                case 1:
                    CodePoint                 = (Extracted[0] & UTF8Max_ASCII);
                    break;
                case 2:
                    CodePoint                 = (Extracted[0] & UTF8Mask5Bit) << 6;
                    CodePoint                |= (Extracted[1] & UTF8Mask6Bit) << 0;
                    break;
                case 3:
                    CodePoint                 = (Extracted[0] & UTF8Mask4Bit) << 12;
                    CodePoint                |= (Extracted[1] & UTF8Mask5Bit) << 6;
                    CodePoint                |= (Extracted[2] & UTF8Mask5Bit) << 0;
                    break;
                case 4:
                    CodePoint                 = (Extracted[0] & 0x07)         << 18;
                    CodePoint                |= (Extracted[1] & UTF8Mask6Bit) << 12;
                    CodePoint                |= (Extracted[2] & UTF8Mask6Bit) <<  6;
                    CodePoint                |= (Extracted[3] & UTF8Mask6Bit) <<  0;
                    break;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodeUnits Pointer is NULL"));
        }
        return CodePoint;
    }

    static void UTF8_EncodeCodePoint(UTF32 CodePoint) {
        UTF8_Clear_Preallocated();
        switch (UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint)) {
            case 1:
                StringIO_PreallocateCodePoint_UTF8[0] = (CodePoint & UTF8Max_ASCII);
                break;
            case 2:
                StringIO_PreallocateCodePoint_UTF8[0] = UTF8Header_2CodeUnits | (CodePoint & (UTF8Mask5Bit << 6)) >> 6;
                StringIO_PreallocateCodePoint_UTF8[1] = UTF8Header_Contine    | (CodePoint &  UTF8Mask6Bit);
                break;
            case 3:
                StringIO_PreallocateCodePoint_UTF8[0] = UTF8Header_3CodeUnits | (CodePoint & (UTF8Mask4Bit << 12)) >> 12;
                StringIO_PreallocateCodePoint_UTF8[1] = UTF8Header_Contine    | (CodePoint & (UTF8Mask6Bit << 6))  >> 6;
                StringIO_PreallocateCodePoint_UTF8[2] = UTF8Header_Contine    | (CodePoint & UTF8Mask6Bit);
                break;
            case 4:
                StringIO_PreallocateCodePoint_UTF8[0] = UTF8Header_4CodeUnits | (CodePoint & (UTF8Mask3Bit << 18)) >> 18;
                StringIO_PreallocateCodePoint_UTF8[1] = UTF8Header_Contine    | (CodePoint & (UTF8Mask6Bit << 12)) >> 12;
                StringIO_PreallocateCodePoint_UTF8[2] = UTF8Header_Contine    | (CodePoint & (UTF8Mask6Bit << 6))  >> 6;
                StringIO_PreallocateCodePoint_UTF8[3] = UTF8Header_Contine    | (CodePoint & UTF8Mask6Bit);
                break;
        }
    }
    
    UTF32 UTF16_ExtractCodePoint(ImmutableString_UTF16 CodeUnits) {
        UTF32 CodePoint                       = 0;
        if (CodeUnits != NULL) {
            UTF16 *Extracted                  = UTF16_ExtractCodePointAsCodeUnits(CodeUnits);
            switch (UTF16_GetCodePointSizeInCodeUnits(CodeUnits[0])) {
                case 1:
                    CodePoint                 = (Extracted[0] & UTF16MaxCodeUnitValue);
                    break;
                case 2:
                    CodePoint                 = UTF16MaxCodeUnitValue + 1;
                    CodePoint                |= (Extracted[0] & UTF16SurrogateMask) << UTF16SurrogateShift;
                    if (CodeUnits[1] != TextIO_NULLTerminator) {
                        CodePoint            |= (Extracted[1] & UTF16SurrogateMask);
                    }
                    break;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodeUnits Pointer is NULL"));
        }
        return CodePoint;
    }
    
    static void UTF16_EncodeCodePoint(UTF32 CodePoint) {
        UTF32   Ranged                                 = CodePoint + UTF16SurrogatePairStart;
        switch (UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint)) {
            case 1:
                StringIO_PreallocateCodePoint_UTF16[0] = (CodePoint & UTF16MaxCodeUnitValue);
                break;
            case 2:
                StringIO_PreallocateCodePoint_UTF16[0] = UTF16HighSurrogateStart + (Ranged & (UTF16SurrogateMask << UTF16SurrogateShift) >> UTF16SurrogateShift);
                StringIO_PreallocateCodePoint_UTF16[1] = UTF16LowSurrogateStart  + (Ranged & UTF16SurrogateMask);
                break;
        }
    }
    
    size_t UTF8_GetGraphemeSizeInCodeUnits(UTF8 *String, size_t OffsetInCodeUnits) {
        size_t GraphemeSize         = 1ULL;
        if (String != NULL) {
            size_t CodeUnit         = OffsetInCodeUnits;
            while (String[CodeUnit] != TextIO_NULLTerminator) {
                UTF32 CodePoint1    = UTF8_ExtractCodePoint(UTF8_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
                bool IsGraphemeExt  = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint1);
                if (IsGraphemeExt == true) {
                    GraphemeSize   += 1;
                    CodeUnit       += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return GraphemeSize;
    }
    
    size_t UTF16_GetGraphemeSizeInCodeUnits(UTF16 *String, size_t OffsetInCodeUnits) {
        size_t GraphemeSize           = 1ULL;
        if (String != NULL) {
            size_t   CodeUnit         = OffsetInCodeUnits;
            while (String[CodeUnit] != TextIO_NULLTerminator) {
                UTF32 CodePoint1      = UTF16_ExtractCodePoint(UTF16_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
                bool IsGraphemeExt    = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint1);
                if (IsGraphemeExt == true) {
                    GraphemeSize     += 1;
                    CodeUnit         += UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint1);
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return GraphemeSize;
    }
    
    size_t UTF32_GetGraphemeSizeInCodePoints(UTF32 *String, size_t OffsetInCodePoints) {
        size_t GraphemeSize          = 1ULL;
        if (String != NULL) {
            size_t CodePoint         = OffsetInCodePoints;
            while (String[CodePoint] != TextIO_NULLTerminator) {
                bool IsGraphemeExt   = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint);
                if (IsGraphemeExt == true) {
                    GraphemeSize    += 1;
                    CodePoint       += 1;
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return GraphemeSize;
    }
    
    size_t UTF8_GetStringSizeInCodeUnits(ImmutableString_UTF8 String) {
        size_t StringSizeInCodeUnits   = 0;
        if (String != NULL) {
            while (String[StringSizeInCodeUnits] != TextIO_NULLTerminator) {
                StringSizeInCodeUnits += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    size_t UTF16_GetStringSizeInCodeUnits(ImmutableString_UTF16 String) {
        size_t StringSizeInCodeUnits   = 0;
        if (String != NULL) {
            while (String[StringSizeInCodeUnits] != TextIO_NULLTerminator) {
                StringSizeInCodeUnits += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringSizeInCodeUnits;
    }
    
    static size_t UTF32_GetStringSizeInUTF8CodeUnits(ImmutableString_UTF32 String) {
        size_t UTF8CodeUnits          = 0;
        if (String != NULL) {
            size_t CodePoint          = 0;
            while (String[CodePoint] != TextIO_NULLTerminator) {
                UTF8CodeUnits        += UTF32_GetCodePointSizeInUTF8CodeUnits(String[CodePoint]);
                CodePoint            += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return UTF8CodeUnits;
    }
    
    static size_t UTF32_GetStringSizeInUTF16CodeUnits(ImmutableString_UTF32 String) {
        size_t UTF16CodeUnits         = 0;
        if (String != NULL) {
            size_t CodePoint          = 0;
            while (String[CodePoint] != TextIO_NULLTerminator) {
                UTF16CodeUnits       += UTF32_GetCodePointSizeInUTF16CodeUnits(String[CodePoint]);
                CodePoint            += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return UTF16CodeUnits;
    }
    
    size_t UTF8_GetStringSizeInCodePoints(ImmutableString_UTF8 String) {
        size_t StringSizeInCodePoints   = 0;
        if (String != NULL) {
            size_t CodeUnit             = 0;
            while (String[CodeUnit] != TextIO_NULLTerminator) {
                StringSizeInCodePoints += 1;
                CodeUnit               += UTF8_GetCodePointSizeInCodeUnits(String[CodeUnit]);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringSizeInCodePoints;
    }
    
    size_t UTF16_GetStringSizeInCodePoints(ImmutableString_UTF16 String) {
        size_t NumCodePoints             = 0;
        if (String != NULL) {
            size_t CodeUnit              = 0;
            while (String[CodeUnit] != TextIO_NULLTerminator) {
                CodeUnit                += UTF16_GetCodePointSizeInCodeUnits(String[CodeUnit]);
                NumCodePoints           += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    size_t UTF32_GetStringSizeInCodePoints(ImmutableString_UTF32 String) {
        size_t NumCodePoints   = 0;
        if (String != NULL) {
            while (String[NumCodePoints] != TextIO_NULLTerminator) {
                NumCodePoints += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    size_t UTF8_GetStringSizeInGraphemes(ImmutableString_UTF8 String) {
        size_t NumGraphemes    = 0;
        if (String != NULL) {
            UTF32 *Decoded     = UTF8_Decode(String);
            NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
            UTF32_Deinit(Decoded);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    size_t UTF16_GetStringSizeInGraphemes(ImmutableString_UTF16 String) {
        size_t NumGraphemes    = 0;
        if (String != NULL) {
            UTF32 *Decoded     = UTF16_Decode(String);
            NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
            UTF32_Deinit(Decoded);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    size_t UTF32_GetStringSizeInGraphemes(ImmutableString_UTF32 String) {
        size_t NumGraphemes            = 0;
        size_t CodePoint               = 1;
        if (String != NULL) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
                bool CurrentIsExtender = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, String[CodePoint - 1]);
                bool NextIsExtender    = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, String[CodePoint]);
                // Loop over the string, only increase the grapheme count when a change happens
                if (CurrentIsExtender == No && NextIsExtender == No) {
                    NumGraphemes      += 1;
                } else if (CurrentIsExtender == Yes && NextIsExtender == No) {
                    NumGraphemes      += 1;
                } else if (CurrentIsExtender == No && NextIsExtender == Yes) {
                    // Loop until we find a not extender, keep track of the CodePoints
                    NumGraphemes      += 1;
                } else if (CurrentIsExtender == Yes && NextIsExtender == Yes) {
                    NumGraphemes      += 1;
                }
                CodePoint             += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumGraphemes;
    }

    size_t UTF8_GetWordSizeInCodePoints(ImmutableString_UTF8 String) {
        size_t WordSize      = 0;
        if (String != NULL) {
            size_t CodeUnit  = 0;
            UTF32 CodePoint  = UTF8_ExtractCodePoint(UTF8_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == false) {
                WordSize    += 1;
                CodeUnit    += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint);
                CodePoint    = UTF8_ExtractCodePoint(UTF8_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return WordSize;
    }
    
    size_t UTF16_GetWordSizeInCodePoints(ImmutableString_UTF16 String) {
        size_t WordSize      = 0;
        if (String != NULL) {
            size_t CodeUnit  = 0;
            UTF32 CodePoint  = UTF16_ExtractCodePoint(UTF16_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == false) {
                WordSize    += 1;
                CodeUnit    += UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint);
                CodePoint    = UTF16_ExtractCodePoint(UTF16_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return WordSize;
    }

    size_t UTF8_GetWordBreakSizeInCodePoints(ImmutableString_UTF8 String) {
        size_t WordBreakSize   = 0;
        if (String != NULL) {
            /* Count the number of word breaks, for example /cr/lf before the start of a new word */
            size_t CodeUnit    = 0;
            UTF32  CodePoint   = UTF8_ExtractCodePoint(UTF8_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == true) {
                WordBreakSize += 1;
                CodeUnit      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint);
                CodePoint      = UTF8_ExtractCodePoint(UTF8_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return WordBreakSize;
    }

    size_t UTF16_GetWordBreakSizeInCodePoints(ImmutableString_UTF16 String) {
        size_t WordBreakSize   = 0;
        if (String != NULL) {
            size_t CodeUnit    = 0;
            UTF32 CodePoint    = UTF16_ExtractCodePoint(UTF16_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == true) {
                WordBreakSize += 1;
                CodeUnit      += UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint);
                CodePoint      = UTF16_ExtractCodePoint(UTF16_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return WordBreakSize;
    }
    
    UTF8 *UTF8_ExtractGrapheme(ImmutableString_UTF8 String, size_t Grapheme2Extract) {
        UTF8 *Grapheme        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, Grapheme2Extract);
            UTF32_Deinit(String32);
            Grapheme          = UTF8_Encode(Grapheme32);
            UTF32_Deinit(Grapheme32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF16 *UTF16_ExtractGrapheme(ImmutableString_UTF16 String, size_t Grapheme2Extract) {
        UTF16 *Grapheme        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, Grapheme2Extract);
            UTF32_Deinit(String32);
            Grapheme          = UTF16_Encode(Grapheme32);
            UTF32_Deinit(Grapheme32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Grapheme;
    }
    
    UTF32 *UTF32_ExtractGrapheme(ImmutableString_UTF32 String, size_t GraphemeIndex) {
        UTF32 *Grapheme         = NULL;
        if (String != NULL) {
            UTF32  CodePoint1   = 0;
            UTF32  CodePoint2   = 0;
            size_t CodePoint    = 0;
            while (String[CodePoint] != TextIO_NULLTerminator && String[CodePoint + 1] != TextIO_NULLTerminator) {
                CodePoint1       = UTF32_ExtractCodePoint(&String[CodePoint]);
                CodePoint2       = UTF32_ExtractCodePoint(&String[CodePoint + 1]);
                // Now look up the codepoints to see if they're grapheme extenders
                bool IsExtender1 = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint1);
                bool IsExtender2 = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint2);
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

    /* Low Level functions, be careful */
    bool UTF32_ShiftCodePoints(UTF32 *String, size_t StringSize, size_t StringCapacity, size_t Start, size_t NumCodePointsToShift) {
        bool ShiftWasSucessful = No;
        if (StringSize + NumCodePointsToShift <= StringCapacity) {
            for (size_t New = StringSize + NumCodePointsToShift; New > Start; New--) {
                String[New] = String[New - NumCodePointsToShift];
            }
            ShiftWasSucessful = Yes;
        }
        return ShiftWasSucessful;
    }

    bool UTF32_ReplaceInPlace(UTF32 *String, const UTF32 *const Replacement, size_t Start) {
        bool SucessfullyReplaced = No;
        if (String != NULL && Replacement != NULL) {
            size_t StringSize      = UTF32_GetStringSizeInCodePoints(String);
            size_t ReplacementSize = UTF32_GetStringSizeInCodePoints(Replacement);
            if (ReplacementSize + Start <= StringSize) {
                size_t CodePoint = 0;
                while (String[CodePoint + Start] != TextIO_NULLTerminator && Replacement[CodePoint] != TextIO_NULLTerminator) {
                    String[Start + CodePoint] = Replacement[CodePoint];
                    CodePoint += 1;
                }
            }
        }
        return SucessfullyReplaced;
    }
    /* Low Level functions, be careful */
    
    bool UTF8_HasBOM(ImmutableString_UTF8 String) {
        bool StringHasABOM        = No;
        if (String != NULL) {
            size_t StringSize     = UTF8_GetStringSizeInCodeUnits(String);
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
        while (String[CodeUnit] != TextIO_NULLTerminator) {
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
        while (String[CodeUnit] != TextIO_NULLTerminator) {
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
        while (String[CodeUnit] != TextIO_NULLTerminator) {
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
            size_t StringSize       = UTF8_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF8_HasBOM(String);
                if (StringHasBOM == Yes && StringSize >= UTF8BOMSizeInCodeUnits + UnicodeUNCPathPrefixSize) {
                    uint8_t CodeUnit   = UTF8BOMSizeInCodeUnits + 1;
                    uint8_t PrefixByte = 0;
                    if (String[CodeUnit] == TextIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == UTF8BOMSizeInCodeUnits + 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    uint8_t CodeUnit   = 0;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == TextIO_UNCPathPrefix8[PrefixByte]) {
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
            size_t StringSize       = UTF16_GetStringSizeInCodePoints(String);
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF16_HasBOM(String);
                if (StringHasBOM == Yes && StringSize >= UTF16BOMSizeInCodeUnits + UnicodeUNCPathPrefixSize) {
                    uint8_t CodeUnit   = UTF16BOMSizeInCodeUnits + 1;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == TextIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == UTF16BOMSizeInCodeUnits + 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    uint8_t CodeUnit   = 0;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == TextIO_UNCPathPrefix8[PrefixByte]) {
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
        bool StringIsUNCPath        = No;
        if (String != NULL) {
            size_t StringSize       = UTF32_GetStringSizeInCodePoints(String);
            
            if (StringSize > UnicodeBOMSizeInCodePoints) {
                bool StringHasBOM   = UTF32_HasBOM(String);
                if (StringHasBOM == Yes && StringSize >= UnicodeBOMSizeInCodePoints + UnicodeUNCPathPrefixSize) {
                    uint8_t CodeUnit   = UnicodeBOMSizeInCodePoints + 1;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == TextIO_UNCPathPrefix8[PrefixByte]) {
                        if (PrefixByte == UnicodeBOMSizeInCodePoints + 4) {
                            StringIsUNCPath = Yes;
                        }
                        PrefixByte    += 1;
                        CodeUnit      += 1;
                    }
                } else if (StringHasBOM == No && StringSize >= 4) {
                    uint8_t CodeUnit   = 0;
                    uint8_t PrefixByte = 0;
                    while (String[CodeUnit] == TextIO_UNCPathPrefix8[PrefixByte]) {
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
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            if (StringHasBOM && String[UTF8BOMSizeInCodeUnits] != TextIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            } else if (String[UTF8BOMSizeInCodeUnits] != TextIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
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
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            if (StringHasBOM && String[UTF8BOMSizeInCodeUnits] != TextIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            } else if (String[UTF8BOMSizeInCodeUnits] != TextIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
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
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            if (StringHasBOM && String[UTF8BOMSizeInCodeUnits] != TextIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            } else if (String[UTF8BOMSizeInCodeUnits] != TextIO_NULLTerminator && String[UTF8BOMSizeInCodeUnits] == '/') {
                PathIsAbsolute     = Yes;
            }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
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
            UTF32_Deinit(String32);
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
            UTF32_Deinit(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return StringHasNewLine;
    }
    
    bool UTF32_HasNewLine(ImmutableString_UTF32 String) {
        bool HasNewLine                  = No;
        if (String != NULL) {
            size_t CodePoint             = 0ULL;
            /*
             Loop over the LineBreak table, if a character matches skip it, loop until the end of the string
             */
            while (String[CodePoint] != TextIO_NULLTerminator) {
                uint8_t LBIndex = 0;
                if (String[CodePoint] == LineBreakTable[LBIndex]) {
                    // LineBreak found, set HasNewLine to true and break
                    HasNewLine = true;
                    return HasNewLine;
                }
                CodePoint += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return HasNewLine;
    }
    
    bool UTF32_IsUpperCase(UTF32 CodePoint) {
        bool CodePointIsUppercase            = No;
        if (CodePoint != TextIO_NULLTerminator) {
            if ((CodePoint >= 0x41 && CodePoint <= 0x5A) || (CodePoint >= 0xC0 && CodePoint <= 0xD6) || (CodePoint >= 0xD8 && CodePoint <= 0xDE)) {
                CodePointIsUppercase         = Yes;
            } else {
                size_t UppercaseIndex        = 0xDE + 1;
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
        size_t   CodeUnit    = 0ULL;
        bool     IsValidUTF8 = Yes;
        if (String != NULL) {
            while (String[CodeUnit] != TextIO_NULLTerminator) {
                // Make sure no byte equals 0xC0 or 0xC1, or 0xF5 - 0xFF
                // Test for Overlong encodings and shit.
                uint8_t Byte = String[CodeUnit];
                switch (Byte) {
                    case 0xC0:
                    case 0xC1:
                    case 0xF5:
                    case 0xF6:
                    case 0xF7:
                    case 0xF8:
                    case 0xF9:
                    case 0xFA:
                    case 0xFB:
                    case 0xFC:
                    case 0xFD:
                    case 0xFE:
                    case 0xFF:
                        IsValidUTF8 = false;
                        break;
                }
                CodeUnit += 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return IsValidUTF8;
    }
    
    bool UTF16_IsValid(ImmutableString_UTF16 String) {
        size_t   CodeUnit             = 1ULL;
        bool     IsValidUTF16         = Yes;
        if (String != NULL) {
            while (String[CodeUnit] != TextIO_NULLTerminator) {
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
        size_t   CodePoint       = 0ULL;
        bool     IsValidUTF32    = Yes;
        if (String != NULL) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
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
            UTF32_Deinit(String32);
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
            UTF32_Deinit(String32);
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
                size_t StringSize     = UTF32_GetStringSizeInCodePoints(String);
                StringWithBOM         = UTF32_Init(StringSize + UnicodeBOMSizeInCodePoints);
                if (StringWithBOM != NULL) {
                    if (BOM2Add == StringIO_BOM_Native) {
#if   (PlatformIO_TargetByteOrder == PlatformIO_TargetByteOrderIsLE)
                        ByteOrder     = UTF32BOM_LE;
#elif (PlatformIO_TargetByteOrder == PlatformIO_TargetByteOrderIsBE)
                        ByteOrder     = UTF32BOM_BE;
#endif
                    } else if (BOM2Add == StringIO_BOM_Little) {
                        ByteOrder     = UTF32BOM_LE;
                    } else if (BOM2Add == StringIO_BOM_Big) {
                        ByteOrder     = UTF32BOM_BE;
                    }
                    StringWithBOM[0] = ByteOrder;
                    for (size_t CodePoint = 1ULL; CodePoint < StringSize; CodePoint++) {
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
        size_t   StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF8_GetStringSizeInCodeUnits(String);
            bool StringHasBOM                       = UTF8_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                       = UTF8_Init(StringSize - UTF8BOMSizeInCodeUnits);
                if (BOMLessString != NULL) {
                    for (size_t CodeUnit = 2ULL; CodeUnit < StringSize; CodeUnit++) {
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
        size_t   StringSize                         = 0ULL;
        if (String != NULL) {
            StringSize                              = UTF16_GetStringSizeInCodeUnits(String);
            bool StringHasBOM                       = UTF16_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                       = UTF16_Init(StringSize - UTF16BOMSizeInCodeUnits);
                if (BOMLessString != NULL) {
                    for (size_t CodeUnit = 1ULL; CodeUnit < StringSize; CodeUnit++) {
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
        size_t   StringSize                      = 0ULL;
        if (String != NULL) {
            StringSize                           = UTF32_GetStringSizeInCodePoints(String);
            bool StringHasBOM                    = UTF32_HasBOM(String);
            if (StringHasBOM) {
                BOMLessString                    = UTF32_Init(StringSize - UnicodeBOMSizeInCodePoints);
                if (BOMLessString != NULL) {
                    for (size_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
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
            size_t   CodeUnit                        = 0ULL;
            size_t   StringSize                      = UTF8_GetStringSizeInCodePoints(String);
            DecodedString                            = UTF32_Init(StringSize);
            if (DecodedString != NULL) {
                for (size_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                    DecodedString[CodePoint]         = UTF8_ExtractCodePoint(UTF8_ExtractCodePointAsCodeUnits(&String[CodeUnit]));
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
            size_t   CodeUnit                        = 0ULL;
            size_t   StringSize                      = UTF16_GetStringSizeInCodePoints(String);
            DecodedString                            = UTF32_Init(StringSize);
            if (DecodedString != NULL) {
                for (size_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                    DecodedString[CodePoint]         = UTF16_ExtractCodePoint(&String[CodeUnit]);
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
            size_t   UTF8CodeUnits                     = UTF32_GetStringSizeInUTF8CodeUnits(String);
            EncodedString                              = UTF8_Init(UTF8CodeUnits);
            size_t   CodePoint                         = 0ULL;
            size_t   CodeUnit                          = 0ULL;
            uint8_t  Index                             = 0;
            if (EncodedString != NULL) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    UTF8_EncodeCodePoint(String[CodePoint]);
                    for (uint8_t CodeUnit2 = 0; CodeUnit2 < UTF32_GetCodePointSizeInUTF8CodeUnits(String[CodePoint]); CodeUnit2++) {
                        EncodedString[CodeUnit]        = StringIO_PreallocateCodePoint_UTF8[Index];
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
            size_t   CodePoint                           = 0ULL;
            size_t   CodeUnit                            = 0ULL;
            size_t   NumCodeUnits                        = UTF32_GetStringSizeInUTF16CodeUnits(String);
            EncodedString                                = UTF16_Init(NumCodeUnits);
            if (EncodedString != NULL) {
                // This is kinda dumb... just call UTF16_EncodeCodePoint for each UTF32 codepoint
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    UTF16_EncodeCodePoint(String[CodePoint]);
                    for (uint8_t CodeUnit2 = 0; CodeUnit2 < UTF32_GetCodePointSizeInUTF16CodeUnits(String[CodePoint]); CodeUnit2++) {
                        EncodedString[CodeUnit] = StringIO_PreallocateCodePoint_UTF16[CodeUnit2];
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
    
    UTF8 *UTF16_Convert(ImmutableString_UTF16 String) {
        UTF8 *String8       = NULL;
        if (String != NULL) {
            UTF32 *String32 = UTF16_Decode(String);
            String8         = UTF8_Encode(String32);
            UTF32_Deinit(String32);
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
            UTF32_Deinit(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return String16;
    }
    
    UTF8 *UTF8_Clone(ImmutableString_UTF8 String) {
        UTF8 *Copy = NULL;
        if (String != NULL) {
            size_t StringSizeInCodeUnits   = UTF8_GetStringSizeInCodeUnits(String);
            Copy                           = UTF8_Init(StringSizeInCodeUnits);
            if (Copy != NULL) {
                for (size_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
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
            size_t StringSizeInCodeUnits   = UTF16_GetStringSizeInCodeUnits(String);
            Copy                           = UTF16_Init(StringSizeInCodeUnits);
            if (Copy != NULL) {
                for (size_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
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
            size_t StringSizeInCodePoints   = UTF32_GetStringSizeInCodePoints(String);
            Copy                            = UTF32_Init(StringSizeInCodePoints);
            if (Copy != NULL) {
                for (size_t CodePoint = 0ULL; CodePoint < StringSizeInCodePoints; CodePoint++) {
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
            size_t StringSize  = UTF8_GetStringSizeInCodePoints(String);
            for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
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
            size_t StringSize  = UTF16_GetStringSizeInCodePoints(String);
            for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
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
            size_t StringSize  = UTF32_GetStringSizeInCodePoints(String);
            for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                String[CodeUnit] = NewValue;
            }
            Verification = String[0];
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Verification;
    }
    
    UTF8 *UTF8_Truncate(ImmutableString_UTF8 String, size_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        UTF8 *Truncated = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *Truncated32 = UTF32_Truncate(String32, NumGraphemes);
            UTF32_Deinit(String32);
            Truncated          = UTF8_Encode(Truncated32);
            UTF32_Deinit(Truncated32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    UTF16 *UTF16_Truncate(ImmutableString_UTF16 String, size_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        UTF16 *Truncated = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *Truncated32 = UTF32_Truncate(String32, NumGraphemes);
            UTF32_Deinit(String32);
            Truncated          = UTF16_Encode(Truncated32);
            UTF32_Deinit(Truncated32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Truncated;
    }
    
    UTF32 *UTF32_Truncate(ImmutableString_UTF32 String, size_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        UTF32 *Truncated = NULL;
        if (String != NULL) {
            size_t   StringSize      = UTF32_GetStringSizeInCodePoints(String);
            size_t   CodePoint       = 0ULL;
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
            size_t   StringSize       = UTF8_GetStringSizeInCodeUnits(Grapheme);
            size_t   CodeUnitsWritten = FileIO_Write(Source, Grapheme, sizeof(UTF8), StringSize);
            if (CodeUnitsWritten != StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %zu CodeUnits but %zu was requested"), CodeUnitsWritten, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_WriteGrapheme(FILE *Source, ImmutableString_UTF16 Grapheme) {
        if (Source != NULL) {
            size_t   StringSize       = UTF16_GetStringSizeInCodeUnits(Grapheme);
            size_t   CodeUnitsWritten = FileIO_Write(Source, Grapheme, sizeof(UTF8), StringSize);
            if (CodeUnitsWritten != StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %zu CodeUnits but %zu was requested"), CodeUnitsWritten, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }
    
    UTF8 UTF8_ReadCodeUnit(FILE *Source) { // Replaces fgetc, getc, getchar; depending on your definition of "Character"
        UTF8 CodeUnit = 0;
        if (Source != NULL) {
            size_t CodeUnitsRead = FileIO_Read(Source, &CodeUnit, sizeof(UTF8), 1);
            if (CodeUnitsRead != sizeof(UTF8)) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unable to read UTF8 CodeUnit"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodeUnit;
    }
    
    UTF16 UTF16_ReadCodeUnit(FILE *Source) {
        UTF16 CodeUnit = 0;
        if (Source != NULL) {
            size_t CodeUnitsRead = FileIO_Read(Source, &CodeUnit, sizeof(UTF16), 1);
            if (CodeUnitsRead != sizeof(UTF16)) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unable to read UTF16 CodeUnit"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodeUnit;
    }
    
    UTF32 UTF8_ReadCodePoint(FILE *Source) {
        UTF32 CodePoint         = 0;
        if (Source != NULL) {
            UTF8_Clear_Preallocated();
            UTF8 HeaderCodeUnit = UTF8_ReadCodeUnit(Source);
            StringIO_PreallocateCodePoint_UTF8[0] = HeaderCodeUnit;
            switch (UTF8_GetCodePointSizeInCodeUnits(HeaderCodeUnit)) {
                case 4:
                    // Read the following 3 CodeUnits and decode them all
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[1], sizeof(UTF8), 1);
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[2], sizeof(UTF8), 1);
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[3], sizeof(UTF8), 1);
                    break;
                case 3:
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[1], sizeof(UTF8), 1);
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[2], sizeof(UTF8), 1);
                    break;
                case 2:
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF8[1], sizeof(UTF8), 1);
                    break;
            }
            CodePoint           = UTF8_Decode(StringIO_PreallocateCodePoint_UTF8);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF32 UTF16_ReadCodePoint(FILE *Source) {
        UTF32 CodePoint          = 0;
        if (Source != NULL) {
            UTF16_Clear_Preallocated();
            UTF16 HeaderCodeUnit = UTF16_ReadCodeUnit(Source);
            StringIO_PreallocateCodePoint_UTF16[0] = HeaderCodeUnit;
            switch (UTF16_GetCodePointSizeInCodeUnits(HeaderCodeUnit)) {
                case 4:
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[1], sizeof(UTF16), 1);
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[2], sizeof(UTF16), 1);
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[3], sizeof(UTF16), 1);
                    break;
                case 3:
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[1], sizeof(UTF16), 1);
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[2], sizeof(UTF16), 1);
                    break;
                case 2:
                    FileIO_Read(Source, &StringIO_PreallocateCodePoint_UTF16[1], sizeof(UTF16), 1);
                    break;
            }
            CodePoint           = UTF16_Decode(StringIO_PreallocateCodePoint_UTF16);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF32 UTF32_ReadCodePoint(FILE *Source) {
        UTF32 CodePoint          = 0;
        if (Source != NULL) {
            size_t CodeUnitsRead = FileIO_Read(Source, &CodePoint, sizeof(UTF32), 1);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF8 *UTF8_ReadGrapheme(FILE *Source) {
        UTF8 *Grapheme       = NULL;
        if (Source != NULL) {
            /*
             Read a CodePoint, look it up in the GrapheneTable, if it, or the following CodePoint are GraphemeExtenders, keep going.
             */
            UTF32 CodePoint1 = UTF8_ReadCodePoint(Source);
            UTF32 CodePoint2 = UTF8_ReadCodePoint(Source);
            
            bool     GraphemeFound                 = No;
            size_t   CodePointSizeInCodeUnits      = 0ULL;
            size_t   CodeUnitsRead                 = 0ULL;
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
                        for (size_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                            if (CodePoint[0] == GraphemeExtensionTable[GraphemeExtension]) {
                            } else if (CodePoint[0] != GraphemeExtensionTable[GraphemeExtension] && GraphemeExtension == GraphemeExtensionTableSize - 1) {
                                GraphemeFound      = Yes;
                            }
                        }
                    } else {
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %zu bytes but %zu was requested"), CodeUnitsRead, CodePointSizeInCodeUnits);
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %zu bytes but 1 was requested"), CodeUnitsRead);
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
            size_t   CodePointSizeInCodeUnits      = 0;
            size_t   CodeUnitsRead                 = 0;
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
                        for (size_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                            if (CodePoint[0] == GraphemeExtensionTable[GraphemeExtension]) {
                            } else if (CodePoint[0] != GraphemeExtensionTable[GraphemeExtension] && GraphemeExtension == GraphemeExtensionTableSize - 1) {
                                GraphemeFound      = Yes;
                            }
                        }
                    } else {
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %zu bytes but %zu was requested"), CodeUnitsRead, CodePointSizeInCodeUnits);
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Read %zu bytes but 1 was requested"), CodeUnitsRead);
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
        return Grapheme;
    }
    
    size_t UTF32_LineBreakSize(UTF32 *String) {
        size_t CodePoint   = 0;
        if (String != NULL && String[CodePoint] != TextIO_NULLTerminator && String[CodePoint + 1] != TextIO_NULLTerminator) {
            while (UTF32_IsCodePointInTable(LineBreakTable, LineBreakTableSize, String[CodePoint])) {
                CodePoint += 1;
            }
        }
        return CodePoint;
    }
    
    StringIO_LineBreakTypes UTF32_GetLineBreakType(UTF32 CodePoint1, UTF32 CodePoint2) {
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
            size_t    SentenceSize                         = 0LL;
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
            size_t    SentenceSize                         = 0ULL;
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
            size_t   StringSize           = UTF8_GetStringSizeInCodeUnits(String);
            size_t   CodeUnitsWritten     = 0ULL;
            if (Type == StringType_UTF8) {
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String, sizeof(UTF8), StringSize);
            } else if (Type == StringType_UTF16) {
                UTF32 *String32        = UTF8_Decode(String);
                UTF16 *String16        = UTF16_Encode(String32);
                UTF32_Deinit(String32);
                CodeUnitsWritten       = FileIO_Write(OutputFile, String16, sizeof(UTF16), TextIO_NewLine16Size);
                UTF16_Deinit(String16);
            } else if (Type == StringType_UTF32) {
                UTF32 *String32        = UTF8_Decode(String);
                CodeUnitsWritten       = FileIO_Write(OutputFile, String32, sizeof(UTF32), TextIO_NewLine32Size);
                UTF32_Deinit(String32);
            }

            if (CodeUnitsWritten != StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %zu CodeUnits of %zu"), CodeUnitsWritten, StringSize);
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
            size_t   StringSize           = UTF16_GetStringSizeInCodeUnits(String);
            size_t   CodeUnitsWritten     = 0;
            if (Type == StringType_UTF16) {
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String, sizeof(UTF16), StringSize);
            } else if (Type == StringType_UTF8) {
                UTF32 *String32           = UTF16_Decode(String);
                UTF8  *String8            = UTF8_Encode(String32);
                size_t StringSize         = UTF8_GetStringSizeInCodeUnits(String8);
                UTF32_Deinit(String32);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String8, sizeof(UTF8), StringSize);
                UTF8_Deinit(String8);
            } else if (Type == StringType_UTF32) {
                UTF32 *String32           = UTF16_Decode(String);
                size_t StringSize         = UTF32_GetStringSizeInCodePoints(String32);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String32, sizeof(UTF32), StringSize);
                UTF32_Deinit(String32);
            }

            if (CodeUnitsWritten != StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %zu CodeUnits of %zu"), CodeUnitsWritten, StringSize);
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
            size_t   StringSize           = UTF32_GetStringSizeInCodePoints(String);
            size_t   CodeUnitsWritten     = 0;
            if (Type == StringType_UTF8) {
                UTF8  *String8            = UTF8_Encode(String);
                size_t StringSize8        = UTF8_GetStringSizeInCodeUnits(String8);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String8, sizeof(UTF8), StringSize8);
                UTF8_Deinit(String8);
            } else if (Type == StringType_UTF16) {
                UTF16 *String16           = UTF16_Encode(String);
                size_t StringSize         = UTF16_GetStringSizeInCodeUnits(String16);
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String16, sizeof(UTF16), StringSize);
                UTF16_Deinit(String16);
            } else if (Type == StringType_UTF32) {
                CodeUnitsWritten          = FileIO_Write(OutputFile, &String, sizeof(UTF32), StringSize);
            }

            if (CodeUnitsWritten != StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %zu CodeUnits of %zu"), CodeUnitsWritten, StringSize);
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
            UTF32_Deinit(String32);
            Reversed          = UTF8_Encode(Reversed32);
            UTF32_Deinit(Reversed32);
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
            UTF32_Deinit(String32);
            Reversed          = UTF16_Encode(Reversed32);
            UTF32_Deinit(Reversed32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Reversed;
    }
    
    UTF32 *UTF32_Reverse(ImmutableString_UTF32 String) {
        UTF32 *Reverse = NULL;
        if (String != NULL) {
            size_t StringSize   = UTF32_GetStringSizeInCodePoints(String);
            Reverse             = UTF32_Init(StringSize);
            if (Reverse != NULL) {
                for (size_t CodePoint = StringSize; CodePoint > 0; CodePoint--) {
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
        size_t String2FindSize = UTF8_GetStringSizeInCodeUnits(String2Find);
        if (StringLength >= String2FindSize) {
            if (WhitespaceType == WhitespaceType_Insignificant) {
                while (UTF8_IsWordBreak(&String[Offset])) {
                    Offset += 1;
                }
            } else if (WhitespaceType == WhitespaceType_Unspecified) {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("WhitespaceType_Unspecified is invalid"));
            }
            
            for (size_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
                for (size_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                    if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                        Offset = SubCodePoint;
                    }
                }
            }
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("StringLength %zu is smaller than String2Find %zu"), StringLength, String2FindSize);
        }
        return Offset;
    }
    
    size_t UTF16_Find(UTF16 *String, size_t StringLength, UTF16 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        size_t Offset          = 0;
        size_t String2FindSize = UTF16_GetStringSizeInCodeUnits(String2Find);
        if (StringLength >= String2FindSize) {
            if (WhitespaceType == WhitespaceType_Insignificant) {
                while (UTF16_IsWordBreak(String[Offset])) {
                    Offset += 1;
                }
            } else if (WhitespaceType == WhitespaceType_Unspecified) {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("WhitespaceType_Unspecified is invalid"));
            }
            
            for (size_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
                for (size_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                    if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                        Offset = SubCodePoint;
                    }
                }
            }
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("StringLength %zu is smaller than String2Find %zu"), StringLength, String2FindSize);
        }
        return Offset;
    }
    
    size_t UTF32_Find(UTF32 *String, size_t StringLength, UTF32 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        size_t Offset = 0;
        size_t String2FindSize = UTF32_GetStringSizeInCodePoints(String2Find);
        if (StringLength >= String2FindSize) {
            if (WhitespaceType == WhitespaceType_Insignificant) {
                while (UTF32_IsWordBreak(String[Offset])) {
                    Offset += 1;
                }
            } else if (WhitespaceType == WhitespaceType_Unspecified) {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("WhitespaceType_Unspecified is invalid"));
            }
            
            for (size_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
                for (size_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                    if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                        Offset = SubCodePoint;
                    }
                }
            }
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("StringLength %zu is smaller than String2Find %zu"), StringLength, String2FindSize);
        }
        return Offset;
    }
    
    size_t UTF8_FindSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString, size_t Offset, size_t Length) {
        ssize_t FoundOffset    = 0LL;
        if (String != NULL && SubString != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *SubString32 = UTF8_Decode(SubString);
            FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
            UTF32_Deinit(String32);
            UTF32_Deinit(SubString32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString Pointer is NULL"));
        }
        return FoundOffset;
    }
    
    size_t UTF16_FindSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString, size_t Offset, size_t Length) {
        ssize_t FoundOffset    = 0LL;
        if (String != NULL && SubString != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *SubString32 = UTF16_Decode(SubString);
            FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
            UTF32_Deinit(String32);
            UTF32_Deinit(SubString32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString Pointer is NULL"));
        }
        return FoundOffset;
    }
    
    size_t UTF32_FindSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString, size_t Offset, size_t Length) {
        size_t  StringSize            = UTF32_GetStringSizeInCodePoints(String);
        size_t  SubStringSize         = UTF32_GetStringSizeInCodePoints(SubString);
        ssize_t MatchingOffset        = 0LL;
        if (String != NULL && SubString != NULL && StringSize < Offset + Length) {
            for (size_t SubCodePoint = 0ULL; SubCodePoint < SubStringSize; SubCodePoint++) {
                for (size_t StringCodePoint = Offset; StringCodePoint < Offset + Length; StringCodePoint++) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %zu + Length %zu is larger than String %zu"), Offset, Length, StringSize);
        }
        return MatchingOffset;
    }
    
    UTF8 *UTF8_ExtractSubString(ImmutableString_UTF8 String, size_t Offset, size_t NumCodeUnits) {
        UTF8 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF8_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, NumCodeUnits);
            ExtractedSubString = UTF8_Encode(Extracted32);
            UTF32_Deinit(String32);
            UTF32_Deinit(Extracted32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF16 *UTF16_ExtractSubString(ImmutableString_UTF16 String, size_t Offset, size_t NumCodeUnits) {
        UTF16 *ExtractedSubString = NULL;
        if (String != NULL) {
            UTF32 *String32    = UTF16_Decode(String);
            UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, NumCodeUnits);
            ExtractedSubString = UTF16_Encode(Extracted32);
            UTF32_Deinit(String32);
            UTF32_Deinit(Extracted32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return ExtractedSubString;
    }
    
    UTF32 *UTF32_ExtractSubString(ImmutableString_UTF32 String, size_t Offset, size_t NumCodePoints) {
        size_t    StringSize                            = UTF32_GetStringSizeInCodePoints(String);
        UTF32    *ExtractedString                       = NULL;
        if (String != NULL && StringSize >= Offset + NumCodePoints) {
            ExtractedString                             = UTF32_Init(NumCodePoints);
            if (ExtractedString != NULL) {
                for (size_t CodePoint = Offset; CodePoint < Offset + NumCodePoints; CodePoint++) {
                    ExtractedString[CodePoint - Offset] = String[CodePoint];
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate ExtractedString"));
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (StringSize < Offset + NumCodePoints) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %zu + NumCodePoints %zu is larger than String %zu"), Offset, NumCodePoints, StringSize);
        }
        return ExtractedString;
    }
    
    UTF8 *UTF8_Create(ImmutableString_UTF8 Padding, size_t Times2Pad) {
        UTF8 *Padded           = NULL;
        if (Padding != NULL) {
            UTF32 *Padding32   = UTF8_Decode(Padding);
            UTF32 *Padded32    = UTF32_Create(Padding32, Times2Pad);
            UTF32_Deinit(Padding32);
            Padded             = UTF8_Encode(Padded32);
        } else if (Padding == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Padding Pointer is NULL"));
        }
        return Padded;
    }
    
    UTF16 *UTF16_Create(ImmutableString_UTF16 Padding, size_t Times2Pad) {
        UTF16 *Padded          = NULL;
        if (Padding != NULL) {
            UTF32 *Padding32   = UTF16_Decode(Padding);
            UTF32 *Padded32    = UTF32_Create(Padding32, Times2Pad);
            UTF32_Deinit(Padding32);
            Padded             = UTF16_Encode(Padded32);
        } else if (Padding == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Padding Pointer is NULL"));
        }
        return Padded;
    }
    
    UTF32 *UTF32_Create(ImmutableString_UTF32 Padding, size_t Times2Pad) {
        UTF32 *Padded                     = NULL;
        if (Padding != NULL) {
            size_t Size                   = UTF32_GetStringSizeInCodePoints(Padding);
            Padded                        = UTF32_Init(Size * Times2Pad);
            if (Padded != NULL) {
                for (size_t PadString = 0; PadString < Size * Times2Pad; PadString++) {
                    for (size_t PaddingCodePoint = 0; PaddingCodePoint < Size; PaddingCodePoint++) {
                        Padded[PadString] = Padding[PaddingCodePoint];
                    }
                }
            }
        } else if (Padding == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Padding Pointer is NULL"));
        }
        return Padded;
    }
    
    UTF8 *UTF8_SubstituteSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substitution, size_t Offset, size_t Length) {
        UTF8 *Replaced8           = NULL;
        if (String != NULL && Substitution != NULL) {
            UTF32 *String32       = UTF8_Decode(String);
            UTF32 *Substitution32 = UTF8_Decode(Substitution);
            UTF32 *Replaced32     = UTF32_SubstituteSubString(String32, Substitution32, Offset, Length);
            Replaced8             = UTF8_Encode(Replaced32);
            UTF32_Deinit(String32);
            UTF32_Deinit(Substitution32);
            UTF32_Deinit(Replaced32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substitution == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Replacement Pointer is NULL"));
        }
        return Replaced8;
    }
    
    UTF16 *UTF16_SubstituteSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substitution, size_t Offset, size_t Length) {
        UTF16 *Replaced16         = NULL;
        if (String != NULL && Substitution != NULL) {
            UTF32 *String32       = UTF16_Decode(String);
            UTF32 *Substitution32 = UTF16_Decode(Substitution);
            UTF32 *Replaced32     = UTF32_SubstituteSubString(String32, Substitution32, Offset, Length);
            Replaced16            = UTF16_Encode(Replaced32);
            UTF32_Deinit(String32);
            UTF32_Deinit(Substitution32);
            UTF32_Deinit(Replaced32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Substitution == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Substitution Pointer is NULL"));
        }
        return Replaced16;
    }
    
    UTF32 *UTF32_SubstituteSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substitution, size_t Offset, size_t Length) {
        UTF32 *NewString                          = NULL;
        if (String != NULL && Substitution != NULL) {
            size_t StringSize                     = UTF32_GetStringSizeInCodePoints(String);
            size_t SubstitutionSize               = UTF32_GetStringSizeInCodePoints(Substitution);
            size_t NewStringSize                  = (Maximum(StringSize, Length) - Minimum(StringSize, Length)) + SubstitutionSize;
            
            NewString                             = UTF32_Init(NewStringSize);
            if (NewString != NULL) {
                size_t NewCodePoint               = 0ULL;
                size_t StringCodePoint            = 0ULL;
                size_t SubstitutionCodePoint      = 0ULL;
                
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

    static void UTF32_SubstituteRangeInPlace(UTF32 *StringToModify, size_t StringToModifySize, size_t ModifyStart, size_t ModifyEnd, UTF32 *Replacement) {
        /*
         it is an internal-only function, so don't worry about users feeding it bad data.

         Limit this function to just doing the insertion, the updating later on just needs to be copied over later?

         "StringToModify" = 14
         ModifyStart = 3
         ModifyEnd = 5
         Replacement = "ING"
         Modified = "StrINGToModify"

         "StringToModify" = 14
         ModifyStart = 3
         ModifyEnd = 5
         Replacement = "IN"
         Modified = "StrINgToModify"

         The math is kinda complicated.

         If I want to take a string and substitute a longer string in place of a single character, there's a way to do it in place.

         "StringToModify"
         "StringModification"

         "AsToMount\0\0"
         "AssToMouth\0"

         Two ways to think of this Ass replacement:
         1: Inserting just the second S, and copying the data over to the correct place

         So, basically just shifting characters from the end of the string X positions towards the right, then doing the insertion; that's how you do it in the end.
         */
    }
    
    UTF8 *UTF8_StitchSubString(ImmutableString_UTF8 String, size_t Offset, size_t Length) {
        UTF8 *Stitched = NULL;
        if (String != NULL) {
            UTF32 *Decoded    = UTF8_Decode(String);
            UTF32 *Stitched32 = UTF32_StitchSubString(Decoded, Offset, Length);
            UTF32_Deinit(Decoded);
            Stitched          = UTF8_Encode(Stitched32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Stitched;
    }
    
    UTF16 *UTF16_StitchSubString(ImmutableString_UTF16 String, size_t Offset, size_t Length) {
        UTF16 *Stitched       = NULL;
        if (String != NULL) {
            UTF32 *Decoded    = UTF16_Decode(String);
            UTF32 *Stitched32 = UTF32_StitchSubString(Decoded, Offset, Length);
            UTF32_Deinit(Decoded);
            Stitched          = UTF16_Encode(Stitched32);
            UTF32_Deinit(Stitched32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Stitched;
    }
    
    UTF32 *UTF32_StitchSubString(ImmutableString_UTF32 String, size_t Offset, size_t Length) {
        UTF32 *Stitched               = NULL;
        if (String != NULL) {
            size_t StringSize         = UTF32_GetStringSizeInCodePoints(String);
            if (Offset <= StringSize && Length <= StringSize && Offset + Length <= StringSize) {
                size_t StitchedSize = StringSize - Length;
                Stitched              = UTF32_Init(StitchedSize);
                if (Stitched != NULL) {
                    size_t CodePoint = 0ULL;
                    while (CodePoint < StitchedSize && CodePoint + Length < StringSize) {
                        if (CodePoint < Offset) {
                            Stitched[CodePoint] = String[CodePoint];
                        } else if (CodePoint > Offset + Length) {
                            Stitched[CodePoint] = String[CodePoint + Length];
                        }
                        CodePoint += 1;
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Could not allocate Stitched string, size: %zu"), StitchedSize);
                }
            } else if (Offset > StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %zu is greater than the String's size %zu"), Offset, StringSize);
            } else if (Length > StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Length %zu is greater than the String's size %zu"), Length, StringSize);
            } else if (Offset + Length > StringSize) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset + Length %zu is greater than the String's size %zu"), Offset + Length, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF8 *UTF8_RemoveSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString2Remove, size_t Instance2Remove) {
        UTF8 *TrimmedString         = NULL;
        if (String != NULL && SubString2Remove != NULL) {
            UTF32 *DecodedString    = UTF8_Decode(String);
            UTF32 *DecodedSubString = UTF8_Decode(SubString2Remove);
            UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
            TrimmedString           = UTF8_Encode(Trimmed32);
            UTF32_Deinit(DecodedString);
            UTF32_Deinit(DecodedSubString);
            UTF32_Deinit(Trimmed32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString2Remove Pointer is NULL"));
        }
        return TrimmedString;
    }
    
    UTF16 *UTF16_RemoveSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString2Remove, size_t Instance2Remove) {
        UTF16 *TrimmedString        = NULL;
        if (String != NULL && SubString2Remove != NULL) {
            UTF32 *DecodedString    = UTF16_Decode(String);
            UTF32 *DecodedSubString = UTF16_Decode(SubString2Remove);
            UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
            TrimmedString           = UTF16_Encode(Trimmed32);
            UTF32_Deinit(DecodedString);
            UTF32_Deinit(DecodedSubString);
            UTF32_Deinit(Trimmed32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SubString2Remove Pointer is NULL"));
        }
        return TrimmedString;
    }
    
    UTF32 *UTF32_RemoveSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString2Remove, size_t Instance2Remove) {
        UTF32 *EditedString = NULL;
        if (String != NULL && SubString2Remove != NULL && Instance2Remove > 0) {
            size_t   StringSize       = UTF32_GetStringSizeInCodePoints(String);
            size_t   SubStringSize    = UTF32_GetStringSizeInCodePoints(SubString2Remove);
            size_t   EditedStringSize = 0ULL;
            
            size_t     NumInstances   = 0ULL;
            size_t    *Instances      = NULL;
            
            for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                for (size_t SubStringCodePoint = 0ULL; SubStringCodePoint < SubStringSize; SubStringCodePoint++) {
                    if (String[StringCodePoint] != SubString2Remove[SubStringCodePoint]) {
                        break;
                    } else {
                        NumInstances += 1;
                    }
                }
            }
            
            Instances = calloc(NumInstances, sizeof(size_t));
            for (size_t Instance = 0ULL; Instance < NumInstances; Instance++) {
                for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (size_t RemoveCodePoint = 0ULL; RemoveCodePoint < SubStringSize; RemoveCodePoint++) {
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
                for (size_t EditedCodePoint = 0ULL; EditedCodePoint < EditedStringSize; EditedCodePoint++) {
                    for (size_t Instance = 0ULL; Instance < NumInstances; Instance++) {
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
    
    UTF8 *UTF8_Insert(ImmutableString_UTF8 String, ImmutableString_UTF8 String2Insert, size_t Offset) {
        UTF8 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Insert32   = UTF8_Decode(String2Insert);
            UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
            UTF32_Deinit(String32);
            UTF32_Deinit(Insert32);
            Inserted          = UTF8_Encode(Inserted32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF16 *UTF16_Insert(ImmutableString_UTF16 String, ImmutableString_UTF16 String2Insert, size_t Offset) {
        UTF16 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Insert32   = UTF16_Decode(String2Insert);
            UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
            UTF32_Deinit(String32);
            UTF32_Deinit(Insert32);
            Inserted          = UTF16_Encode(Inserted32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Inserted;
    }
    
    UTF32 *UTF32_Insert(ImmutableString_UTF32 String, ImmutableString_UTF32 String2Insert, size_t Offset) {
        UTF32 *Inserted = NULL;
        if (String != NULL && String2Insert != NULL) {
            size_t   StringSize = UTF32_GetStringSizeInCodePoints(String);
            size_t   InsertSize = UTF32_GetStringSizeInCodePoints(String2Insert);
            if (Offset == 0xFFFFFFFFFFFFFFFF) {
                Offset          = StringSize;
            }
            if (Offset <= StringSize) {
                Inserted        = UTF32_Init(StringSize + InsertSize);
                if (Inserted != NULL) {
                    for (size_t CodePoint = 0ULL; CodePoint < StringSize + InsertSize; CodePoint++) {
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
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Offset %zu is greater than the string's size %zu"), Offset, StringSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Inserted;
    }

    /* TextIOTables Operations */
    bool UTF32_IsCodePointInTable(const UTF32 *Table, size_t TableSize, UTF32 CodePoint) {
        bool Match = No;
        for (size_t Index = 0; Index < TableSize; Index++) {
            if (CodePoint == Table[Index]) {
                Match = Yes;
                break;
            }
        }
        return Match;
    }

    CodePointClass UTF32_GetCharacterClassOfCodePoint(UTF32 CodePoint) {
        CodePointClass CharacterClass = CodePointClass_Unspecified;
        for (size_t Index = 0; Index < CombiningCharacterClassTableSize; Index++) {
            if (CodePoint == CombiningCharacterClassTable[Index][0]) {
                CharacterClass = CombiningCharacterClassTable[Index][1];
                break;
            }
        }
        return CharacterClass;
    }

    int64_t UTF32_GetIntegerValueOfCodePoint(UTF32 CodePoint) {
        int64_t Integer = 0;
        for (size_t Index = 0; Index < IntegerValueTableSize; Index++) {
            if (CodePoint == IntegerValueTable[Index][0]) {
                Integer = IntegerValueTable[Index][1];
                break;
            }
        }
        return Integer;
    }

    float UTF32_GetDecimalValueOfCodePoint32(UTF32 CodePoint) {
        int64_t Integer = 0;
        for (size_t Index = 0; Index < IntegerValueTableSize; Index++) {
            if (CodePoint == IntegerValueTable[Index][0]) {
                Integer = IntegerValueTable[Index][1];
                break;
            }
        }
        return Integer;
    }

    double UTF32_GetDecimalValueOfCodePoint64(UTF32 CodePoint) {
        double Decimal = 0.0;
        for (size_t Index = 0; Index < DecimalValueTableSize; Index++) {
            if (CodePoint == DecimalValueTable[Index][0]) {
                Decimal = DecimalValueTable[Index][1] / DecimalValueTable[Index][2];
                break;
            }
        }
        return Decimal;
    }

    UTF32 *UTF32_GetReplacementStringFromTable(UTF32 *Table, size_t TableSize, UTF32 *String2Replace, size_t String2ReplaceSize) {
        UTF32 *Replacement = NULL;
        for (size_t Index = 0; Index < TableSize; Index++) {
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
            UTF32_Deinit(String32);
            CaseFolded        = UTF8_Encode(CaseFold32);
            UTF32_Deinit(CaseFold32);
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
            UTF32_Deinit(String32);
            CaseFolded        = UTF16_Encode(CaseFold32);
            UTF32_Deinit(CaseFold32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return CaseFolded;
    }
    
    static size_t UTF32_GetCaseFoldedSize(ImmutableString_UTF32 String) {
        size_t NumCodePoints = 0ULL;
        size_t CodePoint = 0ULL;
        while (String[CodePoint] != TextIO_NULLTerminator) {
            size_t TableIndex = 0ULL;
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
        size_t CodePoint = 0ULL;
        while (String2Edit[CodePoint] != TextIO_NULLTerminator && Replacement[CodePoint] != TextIO_NULLTerminator) {
            String2Edit[CodePoint] = Replacement[CodePoint];
            CodePoint += 1;
        }
    }
    
    UTF32 *UTF32_CaseFold(ImmutableString_UTF32 String) {
        UTF32   *CaseFoldedString                = NULL;
        if (String != NULL) {
            size_t CaseFoldedSize                = UTF32_GetCaseFoldedSize(String);
            CaseFoldedString                     = UTF32_Init(CaseFoldedSize);
            size_t CodePoint                     = 0ULL;
            while (String[CodePoint] != TextIO_NULLTerminator) {
                if (UTF32_IsUpperCase(String[CodePoint])) {
                    for (size_t Index = 0ULL; Index < CaseFoldTableSize; Index++) {
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
        size_t   CodePoint  = 1ULL;
        uint32_t CodePointA = 0UL;
        uint32_t CodePointB = 0UL;
        if (String != NULL) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
                CodePointA = String[CodePoint - 1];
                CodePointB = String[CodePoint];
                for (size_t IndexA = 0ULL; IndexA < CombiningCharacterClassTableSize; IndexA++) {
                    for (size_t IndexB = 0ULL; IndexB < CombiningCharacterClassTableSize; IndexB++) {
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

    /*
     There should be a helper function so that we can build up these multi-codepoints one at a time.

     It'd be useful for Grapheme replacements too.

     So, here we just need to know th size of the original string in codepoints, as well as the total number of codepoints to be replaced, as well as the total size of the replacements

     So here I guess we just need to loop over looking for matching codepoints, and getting the size of the replacement strings
     */

    static size_t UTF32_ShouldBeNormalized(UTF32 CodePoint, StringIO_NormalizationForms NormalizationForm) {
        size_t TableIndex         = 0;
        if (NormalizationForm == NormalizationForm_CanonicalCompose) {
            while (TableIndex < CanonicalNormalizationTableSize) {
                if (CodePoint == *CanonicalNormalizationTable[TableIndex][0]) {
                    return TableIndex;
                }
                TableIndex += 1;
            }
        } else if (NormalizationForm == NormalizationForm_KompatibleCompose) {
            while (TableIndex < KompatibleNormalizationTableSize) {
                if (CodePoint == *KompatibleNormalizationTable[TableIndex][0]) {
                    return TableIndex;
                }
                TableIndex += 1;
            }
        }
        return -1;
    }

    static size_t UTF32_GetComulativeReplacementSizeForNormalization(ImmutableString_UTF32 String, StringIO_NormalizationForms NormalizationForm) {
        size_t CumulativeReplacementSize       = 0;
        size_t CodePoint = 0;
        if (NormalizationForm == NormalizationForm_CanonicalCompose) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
                size_t TableIndex              = UTF32_ShouldBeNormalized(String[CodePoint], NormalizationForm_CanonicalCompose);
                if (TableIndex != -1) {
                    CumulativeReplacementSize += UTF32_GetStringSizeInCodePoints((const UTF32 *) &CanonicalNormalizationTable[TableIndex][1]);
                    CodePoint                 += 1;
                }
            }
        } else if (NormalizationForm == NormalizationForm_KompatibleCompose) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
                size_t TableIndex              = UTF32_ShouldBeNormalized(String[CodePoint], NormalizationForm_KompatibleCompose);
                if (TableIndex != -1) {
                    CumulativeReplacementSize += UTF32_GetStringSizeInCodePoints((const UTF32 *) &KompatibleNormalizationTable[TableIndex][1]);
                    CodePoint                 += 1;
                }
            }
        }
        return CumulativeReplacementSize;
    }
    
    static UTF32 *UTF32_Compose(UTF32 *String, StringIO_NormalizationForms CompositionType) { // FIXME: Must use a stable sorting algorithm
        size_t   CodePoint            = 0ULL;
        UTF32   *ComposedString       = NULL;
        if (String != NULL && (CompositionType == NormalizationForm_CanonicalCompose || CompositionType == NormalizationForm_KompatibleCompose)) {
            size_t ComposedStringSize = UTF32_GetComulativeReplacementSizeForNormalization(String, CompositionType);
            ComposedString            = UTF32_Init(ComposedStringSize);
            if (ComposedString != NULL) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    size_t TableID = UTF32_ShouldBeNormalized(String[CodePoint], CompositionType);
                    if (TableID != -1) {
                        // Find the length of the replacement string
                        bool Shifted  = UTF32_ShiftCodePoints(ComposedString, UTF32_GetStringSizeInCodePoints(String), ComposedStringSize, CodePoint, UTF32_GetStringSizeInCodePoints((const UTF32 *) &CanonicalNormalizationTable[TableID][1]));
                        if (Shifted) {
                            // Insert in place the replacement string
                            UTF32_ReplaceInPlace(String, (const UTF32 *const ) &CanonicalNormalizationTable[TableID][1], CodePoint);
                        }
                    }
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
        size_t   CodePoint      = 0ULL;
        UTF32   *DecomposedString = NULL;
        if (String != NULL && (DecompositionType == NormalizationForm_CanonicalDecompose || DecompositionType == NormalizationForm_KompatibleDecompose)) {
            if (DecompositionType == NormalizationForm_CanonicalDecompose) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    for (size_t DecomposeCodePoint = 0ULL; DecomposeCodePoint < CanonicalNormalizationTableSize; DecomposeCodePoint++) {
                        if (String[CodePoint] == CanonicalNormalizationTable[DecomposeCodePoint][0][0]) {
                            DecomposedString = UTF32_SubstituteSubString(String, CanonicalNormalizationTable[DecomposeCodePoint][1], CodePoint, 1);
                        }
                    }
                    CodePoint += 1;
                }
            } else if (DecompositionType == NormalizationForm_KompatibleDecompose) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    for (size_t DecomposeCodePoint = 0ULL; DecomposeCodePoint < KompatibleNormalizationTableSize; DecomposeCodePoint++) {
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
            UTF32_Deinit(String32);
            UTF32_Deinit(NormalizedString32);
        }
        return NormalizedString8;
    }
    
    UTF16 *UTF16_Normalize(ImmutableString_UTF16 String, StringIO_NormalizationForms NormalizedForm) {
        UTF16 *NormalizedString16     = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unspecified) {
            UTF32 *String32           = UTF16_Decode(String);
            UTF32 *NormalizedString32 = UTF32_Normalize(String32, NormalizedForm);
            NormalizedString16        = UTF16_Encode(NormalizedString32);
            UTF32_Deinit(String32);
            UTF32_Deinit(NormalizedString32);
        }
        return NormalizedString16;
    }
    
    UTF32 *UTF32_Normalize(ImmutableString_UTF32 String, StringIO_NormalizationForms NormalizedForm) {
        UTF32 *NormalizedString = NULL;
        if (String != NULL && NormalizedForm != NormalizationForm_Unspecified) {
            if (NormalizedForm == NormalizationForm_CanonicalCompose) {
                UTF32 *Decomposed = UTF32_Decompose(String, NormalizationForm_CanonicalDecompose);
                NormalizedString  = UTF32_Compose(Decomposed, NormalizationForm_CanonicalCompose);
                UTF32_Deinit(Decomposed);
            } else if (NormalizedForm == NormalizationForm_KompatibleCompose) {
                UTF32 *Decomposed = UTF32_Decompose(String, NormalizationForm_KompatibleDecompose);
                NormalizedString  = UTF32_Compose(Decomposed, NormalizationForm_KompatibleCompose);
                UTF32_Deinit(Decomposed);
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
            UTF32_Deinit(String32);
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
            UTF32_Deinit(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Value;
    }
    
    // Integer2String should accept any integer base from the lookup table and shift the value until it can't anymore
    int64_t UTF32_String2Integer(TextIO_Bases Base, ImmutableString_UTF32 String) {
        size_t   CodePoint = 0;
        int8_t   Sign      = 1;
        int64_t  Value     = 0LL;
        if (String != NULL) {
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != TextIO_NULLTerminator && (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('1'))) {
                        Value    <<= 1;
                        Value     += String[CodePoint] - 0x30;
                        CodePoint += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != TextIO_NULLTerminator && (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('7'))) {
                        Value     *= 8;
                        Value     += String[CodePoint] - 0x30;
                        CodePoint += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != TextIO_NULLTerminator && ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (CodePoint == 0 && String[CodePoint] == UTF32Character('-')))) {
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
                        while (String[CodePoint] != TextIO_NULLTerminator && ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F')))) {
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
                        while (String[CodePoint] != TextIO_NULLTerminator && ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f')))) {
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
                for (size_t CodePoint = NumDigits; CodePoint > 0; CodePoint--) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Base: %u is not an integer"), Base);
        }
        return String;
    }
    
    double UTF8_String2Decimal(TextIO_Bases Base, ImmutableString_UTF8 String) {
        double Decimal = 0.0;
        if (String != NULL) {
            UTF32 *String32 = UTF8_Decode(String);
            Decimal         = UTF32_String2Decimal(Base, String32);
            UTF32_Deinit(String32);
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
            UTF32_Deinit(String32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF32_String2Decimal(TextIO_Bases Base, ImmutableString_UTF32 String) { // Replaces strtod, strtof, strold, atof, and atof_l
        double   Value         = 0.0;
        bool     IsNegative    = No;
        if (String != NULL) {
            size_t CodePoint   = 0ULL;
            
            if (String[CodePoint] == UTF32Character('-')) {
                IsNegative = Yes;
            }
            
            while (String[CodePoint] != UTF32Character('.')) {
                CodePoint     += 1;
            } // Before the decimal
            
            while (String[CodePoint] != TextIO_NULLTerminator) {
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
        UTF32_Deinit(String32);
        return String8;
    }
    
    UTF16 *UTF16_Decimal2String(TextIO_Bases Base, double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Decimal, Base);
        UTF16 *String16 = UTF16_Encode(String32);
        UTF32_Deinit(String32);
        return String16;
    }
    
    static UTF32 *Decimal2String_UTF32(TextIO_Bases Base, double Decimal, size_t MinimumWidth, size_t Precision) {
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
                StringSize += 2;
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
        size_t   StringSize       = 0ULL;
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
                for (size_t ExponentCodePoint = 0ULL; ExponentCodePoint < NumDigitsExponent; ExponentCodePoint++) {
                    OutputString[ExponentCodePoint + StringSize]                 = Exponent /= 10;
                }
            }
            OutputString[StringSize + NumDigitsExponent + UnicodeBOMSizeInCodePoints] = UTF32Character('.');
            // Now let's start popping in the Mantissa
            while (Mantissa > 0) { // TODO: This assumes there's only 1 codepoint necessary to express the exponent
                for (size_t MantissaCodePoint = 0ULL; MantissaCodePoint < NumDigitsExponent; MantissaCodePoint++) {
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
                for (size_t ExponentDigit = 0ULL; ExponentDigit < NumDigitsExponentInDigits; ExponentDigit++) {
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
                for (size_t ExponentDigit = 0ULL; ExponentDigit < NumDigitsExponentInDigits; ExponentDigit++) {
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
                UTF32_Deinit(String1_32);
                UTF32_Deinit(String2_32);
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
                UTF32_Deinit(String1_32);
                UTF32_Deinit(String2_32);
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
            size_t   String1SizeInCodePoints = UTF32_GetStringSizeInCodePoints(String1);
            size_t   String2SizeInCodePoints = UTF32_GetStringSizeInCodePoints(String2);
            if (String1SizeInCodePoints == String2SizeInCodePoints) {
                for (size_t CodePoint = 0ULL; CodePoint < (String1SizeInCodePoints + String2SizeInCodePoints) / 2; CodePoint++) {
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
    
    bool UTF8_CompareSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substring, size_t StringOffset, size_t SubstringOffset) {
        bool SubstringMatchesAtOffset = No;
        if (String != NULL && Substring != NULL) {
            UTF32 *String32           = UTF8_Decode(String);
            UTF32 *Sub32              = UTF8_Decode(Substring);
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
    
    bool UTF16_CompareSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substring, size_t StringOffset, size_t SubstringOffset) {
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
    
    bool UTF32_CompareSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substring, size_t StringOffset, size_t SubstringOffset) {
        bool SubstringMatchesAtOffset            = No;
        if (String != NULL && Substring != NULL) {
            size_t   StringSize                  = UTF32_GetStringSizeInCodePoints(String);
            size_t   SubstringSize               = UTF32_GetStringSizeInCodePoints(Substring);
            for (size_t StringCodePoint = StringOffset; StringCodePoint < StringSize; StringCodePoint++) {
                for (size_t SubstringCodePoint = SubstringOffset; SubstringCodePoint < SubstringSize; SubstringCodePoint++) {
                    if (String[StringCodePoint] != Substring[SubstringCodePoint]) {
                        return SubstringMatchesAtOffset;
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
            UTF32    *Trimmed32                 = UTF32_Trim(String32, Type, (const UTF32**) Strings2Remove32);
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
            UTF32   **Strings2Remove32          = UTF16_StringSet_Decode((const UTF16**) Strings2Remove);
            UTF32    *Trimmed32                 = UTF32_Trim(String32, Type, (const UTF32**) Strings2Remove32);
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
            size_t     StringSize          = UTF32_GetStringSizeInCodePoints(String);
            size_t     NumRemovalStrings   = UTF32_StringSet_GetNumStrings(Strings2Remove);
            size_t    *RemovalStringSizes  = UTF32_StringSet_GetStringSizesInCodePoints(Strings2Remove);
            size_t     NumRemovalPoints    = 0ULL;
            size_t     CurrentRemovalPoint = 0ULL;
            size_t    *RemovalPointsStart  = NULL; // RemovalPoint[0] = {0, 6}; start and stop points
            size_t    *RemovalPointsEnd    = NULL; // RemovalPoint[0] = {0, 6}; start and stop points
            size_t     TrimmedStringSize   = 0ULL;
            if (Type == TruncationType_All) {
                // Loop over all the codepoints until you find one that is not on the list, then remove it; BUT JUST FOR THE BEGINNING AND END
                for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (size_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                        bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                        if (SubstringFound) {
                            NumRemovalPoints += 1;
                        }
                    }
                }
                
                RemovalPointsStart                                  = calloc(NumRemovalPoints, sizeof(size_t));
                RemovalPointsEnd                                    = calloc(NumRemovalPoints, sizeof(size_t));
                
                for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (size_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
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
                for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (size_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
                        bool SubstringFound                         = UTF32_CompareSubString(String, Strings2Remove[RemovalString], StringCodePoint, 0);
                        if (SubstringFound) {
                            NumRemovalPoints += 1;
                        }
                    }
                }
                
                RemovalPointsStart                                  = calloc(NumRemovalPoints, sizeof(size_t));
                RemovalPointsEnd                                    = calloc(NumRemovalPoints, sizeof(size_t));
                
                for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    for (size_t RemovalString = 0ULL; RemovalString < NumRemovalStrings; RemovalString++) {
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
            for (size_t RemovalPoint = 0ULL; RemovalPoint < NumRemovalPoints; RemovalPoint++) {
                TrimmedStringSize     -= RemovalPointsEnd[RemovalPoint] - RemovalPointsStart[RemovalPoint];
            }
            
            Trimmed                    = UTF32_Init(TrimmedStringSize);
            if (Trimmed != NULL) {
                for (size_t RemovalPoint = 0ULL; RemovalPoint < NumRemovalPoints; RemovalPoint++) {
                    for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                        for (size_t TrimmedCodePoint = 0ULL; TrimmedCodePoint < TrimmedStringSize; TrimmedCodePoint++) {
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
            UTF32 **SplitString32 = UTF32_Split(String32, (const UTF32 *const *) Delimiters32);
            UTF32_Deinit((UTF32*) String32);
            UTF32_StringSet_Deinit((UTF32**) Delimiters32);
            SplitString           = UTF8_StringSet_Encode((const UTF32**) SplitString32);
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
            UTF32 **Delimiters32  = UTF16_StringSet_Decode((const UTF16**) Delimiters);
            UTF32 **SplitString32 = UTF32_Split(String32, (const UTF32 *const *) Delimiters32);
            UTF32_Deinit(String32);
            UTF32_StringSet_Deinit((UTF32**) Delimiters32);
            SplitString           = UTF16_StringSet_Encode((const UTF32**) SplitString32);
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (Delimiters == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Delimiters Pointer is NULL"));
        }
        return SplitString;
    }
    
    UTF32 **UTF32_Split(ImmutableString_UTF32 String, ImmutableStringSet_UTF32 Delimiters) {
        UTF32    **SplitStrings    = NULL; // What we return, it's a 0 indexed array of strings
        size_t     StringSize      = 0ULL; // The size of the first parameter
        size_t     NumDelimiters   = 0ULL; // The number of delimiters in the second parameter
        size_t     NumSplitStrings = 0ULL; // The number of strings to return
        size_t    *SplitSizes      = NULL; // The size of each split string
        size_t    *SplitOffsets    = NULL; // The starting position of each split
        if (String != NULL && Delimiters != NULL) {
            StringSize             = UTF32_GetStringSizeInCodePoints(String);
            NumDelimiters          = UTF32_StringSet_GetNumStrings(Delimiters);
            size_t *DelimitersSize = calloc(NumDelimiters, sizeof(size_t));
            for (size_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                DelimitersSize[Delimiter] = UTF32_GetStringSizeInCodePoints(Delimiters[Delimiter]);
            }
            // Check if the current delimiter is larger than the string, if so, it can't match.
            // Well we need to loop over the string NumDelimiters times, so Delimiters, String, DelimiterString
            for (size_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                for (size_t DelimiterCodePoint = 0ULL; DelimiterCodePoint < DelimitersSize[Delimiter]; DelimiterCodePoint++) {
                    for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
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
            
            SplitSizes   = calloc(NumSplitStrings, sizeof(size_t));
            SplitOffsets = calloc(NumSplitStrings, sizeof(size_t));
            
            for (size_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                for (size_t DelimiterCodePoint = 0ULL; DelimiterCodePoint < DelimitersSize[Delimiter]; DelimiterCodePoint++) {
                    for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
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
    
    size_t UTF8_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF8 String) {
        size_t NumDigits      = 0ULL;
        if (String != NULL) {
            size_t CodePoint  = 0;
            
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= '0' && String[CodePoint] <= '1') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= '0' && String[CodePoint] <= '7') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= '0' && String[CodePoint] <= '9') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'A' && String[CodePoint] <= 'F')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'a' && String[CodePoint] <= 'f')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            } else if ((Base & Base_Decimal) == Base_Decimal) {
                if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Scientific) == Base_Scientific || (Base & Base_Shortest) == Base_Shortest) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.' || String[CodePoint] == 'E')) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.' || String[CodePoint] == 'e')) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'A' && String[CodePoint] <= 'F') || String[CodePoint] <= 'P' || String[CodePoint] <= 'X') {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
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
    
    size_t UTF16_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF16 String) {
        size_t NumDigits      = 0ULL;
        if (String != NULL) {
            size_t CodePoint  = 0;
            
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('1')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('7')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('A') && String[CodePoint] <= UTF16Character('F'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('a') && String[CodePoint] <= UTF16Character('f'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            } else if ((Base & Base_Decimal) == Base_Decimal) {
                if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Scientific) == Base_Scientific || (Base & Base_Shortest) == Base_Shortest) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.') || String[CodePoint] == UTF16Character('E'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.') || String[CodePoint] == UTF16Character('e'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('A') && String[CodePoint] <= UTF16Character('F')) || String[CodePoint] <= UTF16Character('P') || String[CodePoint] <= UTF16Character('X')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
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
    
    size_t UTF32_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF32 String) {
        size_t NumDigits      = 0ULL;
        if (String != NULL) {
            size_t CodePoint  = 0;
            
            if ((Base & Base_Integer) == Base_Integer) {
                if ((Base & Base_Radix2) == Base_Radix2) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('1')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix8) == Base_Radix8) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('7')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f'))) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                }
            } else if ((Base & Base_Decimal) == Base_Decimal) {
                if ((Base & Base_Radix10) == Base_Radix10) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if ((Base & Base_Scientific) == Base_Scientific || (Base & Base_Shortest) == Base_Shortest) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.') || String[CodePoint] == UTF32Character('E'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.') || String[CodePoint] == UTF32Character('e'))) { // Todo: Make sure the Exponent is preceded by a digit
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    }
                } else if ((Base & Base_Radix16) == Base_Radix16) {
                    if ((Base & Base_Uppercase) == Base_Uppercase) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
                            if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F')) || String[CodePoint] <= UTF32Character('P') || String[CodePoint] <= UTF32Character('X')) {
                                NumDigits     += 1;
                            }
                            CodePoint         += 1;
                        }
                    } else if (((Base & Base_Lowercase) == Base_Lowercase)) {
                        while (String[CodePoint] != TextIO_NULLTerminator) {
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
    
    size_t UTF32_GetSubStringLength(ImmutableString_UTF32 Format, ImmutableString_UTF32 Formatted, size_t Offset) {
        size_t Length        = 0ULL;
        if (Format != NULL && Formatted != NULL) {
            size_t CodePoint = Offset;
            
            while ((Format[CodePoint] != Formatted[CodePoint]) && (Format[CodePoint] != TextIO_NULLTerminator && Formatted[CodePoint] != TextIO_NULLTerminator)) {
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
    CharSet8 *UTF8_ConvertUnicode2CharSet(ImmutableString_UTF8 String, StringIO_CodePages CodePage) {
        CharSet8 *Encoded = NULL;
        if (String != NULL && CodePage != CodePage_Unspecified) {
            UTF32 *Decoded      = UTF8_Decode(String);
            UTF32 *Composed     = UTF32_Normalize(Decoded, NormalizationForm_KompatibleCompose);
            size_t   Characters = 0ULL;
            size_t   Character  = 0ULL;
            if (CodePage == CodePage_ISO_8859_1) {
                while (Composed[Characters] != TextIO_NULLTerminator) {
                    if (Composed[Characters] <= 0xFF) {
                        Characters += 1;
                    }
                }
                // Ok so now we have our number of characters, init a new string with that many and copy em over
                Encoded = UTF8_Init(Characters);
                while (Composed[Character] != TextIO_NULLTerminator && Character < Characters) {
                    if (Composed[Characters] <= 0xFF) {
                        Encoded[Character] = Composed[Characters] & 0xFF;
                        Character  += 1;
                        Characters += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CodePage_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Encoded;
    }

    CharSet16 *UTF16_ConvertUnicode2CharSet(ImmutableString_UTF16 String, StringIO_CodePages CodePage) {
        CharSet16 *Encoded = NULL;
        if (String != NULL && CodePage != CodePage_Unspecified) {
            UTF32 *Decoded      = UTF16_Decode(String);
            UTF32 *Composed     = UTF32_Normalize(Decoded, NormalizationForm_KompatibleCompose);
            size_t   Characters = 0ULL;
            size_t   Character  = 0ULL;
            if (CodePage == CodePage_ISO_8859_1) {
                while (Composed[Characters] != TextIO_NULLTerminator) {
                    if (Composed[Characters] <= 0xFF) {
                        Characters += 1;
                    }
                }
                // Ok so now we have our number of characters, init a new string with that many and copy em over
                Encoded = UTF16_Init(Characters);
                while (Composed[Character] != TextIO_NULLTerminator && Character < Characters) {
                    if (Composed[Characters] <= 0xFF) {
                        Encoded[Character] = Composed[Characters] & 0xFF;
                        Character  += 1;
                        Characters += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CodePage_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Encoded;
    }

    CharSet32 *UTF32_ConvertUnicode2CharSet(ImmutableString_UTF32 String, StringIO_CodePages CodePage) {
        CharSet32 *Encoded = NULL;
        if (String != NULL && CodePage != CodePage_Unspecified) {
            UTF32 *Composed     = UTF32_Normalize(String, NormalizationForm_KompatibleCompose);
            size_t   Characters = 0ULL;
            size_t   Character  = 0ULL;
            if (CodePage == CodePage_ISO_8859_1) {
                while (Composed[Characters] != TextIO_NULLTerminator) {
                    if (Composed[Characters] <= 0xFF) {
                        Characters += 1;
                    }
                }
                // Ok so now we have our number of characters, init a new string with that many and copy em over
                Encoded = UTF32_Init(Characters);
                while (Composed[Character] != TextIO_NULLTerminator && Character < Characters) {
                    if (Composed[Characters] <= 0xFF) {
                        Encoded[Character] = Composed[Characters] & 0xFF;
                        Character  += 1;
                        Characters += 1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CodePage_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Encoded;
    }

    UTF8 *UTF8_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet8 *) String, StringIO_CodePages CodePage) {
        UTF8 *Unicode = NULL;
        if (String != NULL && CodePage != CodePage_Unspecified) {
            UTF32   *Decoded          = UTF8_Decode(String);
            size_t   Character        = 0ULL;
            size_t   CodePoint        = 0ULL;
            size_t   NumCodeUnits     = 0ULL;
            if (CodePage == CodePage_ISO_8859_1) {
                while (Decoded[Character] != TextIO_NULLTerminator) {
                    if (UTF32_IsCodePointInTable(KompatibleNormalizationTable, KompatibleNormalizationTableSize, Decoded[Character])) { // Kompatible is just a standin here for the actual table
                        NumCodeUnits += UTF32_GetCodePointSizeInUTF8CodeUnits(Decoded[Character]);
                    }
                    Character        += 1;
                }
                Unicode               = UTF8_Init(NumCodeUnits);
                while (Decoded[Character] != TextIO_NULLTerminator && Unicode[CodePoint] != TextIO_NULLTerminator) {
                    for (uint8_t Byte = 0; Byte < UTF32_GetCodePointSizeInUTF8CodeUnits(Decoded[Character]); Byte++) {
                        Unicode[CodePoint + Byte] = StringIO_PreallocateCodePoint_UTF8[Byte];
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CodePage_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Unicode;
    }

    UTF16 *UTF16_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet16 *) String, StringIO_CodePages CodePage) {
        UTF16 *Unicode = NULL;
        if (String != NULL && CodePage != CodePage_Unspecified) {
            UTF32   *Decoded          = UTF16_Decode(String);
            size_t   Character        = 0ULL;
            size_t   CodePoint        = 0ULL;
            size_t   NumCodeUnits     = 0ULL;
            if (CodePage == CodePage_ISO_8859_1) {
                while (Decoded[Character] != TextIO_NULLTerminator) {
                    if (UTF32_IsCodePointInTable(KompatibleNormalizationTable, KompatibleNormalizationTableSize, Decoded[Character])) { // Kompatible is just a standin here for the actual table
                        NumCodeUnits += UTF32_GetCodePointSizeInUTF8CodeUnits(Decoded[Character]);
                    }
                    Character        += 1;
                }
                Unicode               = UTF16_Init(NumCodeUnits);
                while (Decoded[Character] != TextIO_NULLTerminator && Unicode[CodePoint] != TextIO_NULLTerminator) {
                    for (uint8_t Byte = 0; Byte < UTF32_GetCodePointSizeInUTF16CodeUnits(Decoded[Character]); Byte++) {
                        Unicode[CodePoint + Byte] = StringIO_PreallocateCodePoint_UTF16[Byte];
                    }
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CodePage_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CodePage is Invalid"));
        }
        return Unicode;
    }

    UTF32 *UTF32_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet32 *) String, StringIO_CodePages CodePage) {
        UTF32 *Unicode = NULL;
        if (String != NULL && CodePage != CodePage_Unspecified) {
            size_t   Character        = 0ULL;
            size_t   CodePoint        = 0ULL;
            size_t   NumCodeUnits     = 0ULL;
            if (CodePage == CodePage_ISO_8859_1) {
                while (String[Character] != TextIO_NULLTerminator) {
                    if (UTF32_IsCodePointInTable(KompatibleNormalizationTable, KompatibleNormalizationTableSize, String[Character])) { // Kompatible is just a standin here for the actual table
                        NumCodeUnits += 1;
                    }
                    Character        += 1;
                }
                Unicode               = UTF32_Init(NumCodeUnits);
                while (String[Character] != TextIO_NULLTerminator && Unicode[CodePoint] != TextIO_NULLTerminator) {
                    Unicode[CodePoint] = String[Character];
                }
            }
        } else if (String == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        } else if (CodePage == CodePage_Unspecified) {
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
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
