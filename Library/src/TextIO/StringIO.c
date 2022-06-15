#include "../../include/TextIO/StringIO.h"             /* Included for our declarations */

#include "../../include/AssertIO.h"                    /* Included for Assertions */
#include "../../include/BufferIO.h"                    /* Included for BufferIO_MemorySet */
#include "../../include/FileIO.h"                      /* Included for File operations */
#include "../../include/MathIO.h"                      /* Included for endian swapping */
#include "../../include/TextIO/Private/TextIOTables.h" /* Included for the Text tables */
#include "../../include/TextIO/StringSetIO.h"          /* Included for StringSet support */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Sanitize strings: Remove U+202E from all strings in all cases, it is only used maliciously.
     */

    /*
     I really want to rebase everything on Graphemes though

     read CodeUnits from a string into a CodePoint, read CodePoints into Graphemes, then handle it all correctly.
     */

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
        AssertIO(String != NULL);
        UTF8 *CodeUnits                                       = NULL;
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
        return CodeUnits;
    }

    static UTF16 *UTF16_ExtractCodePointAsCodeUnits(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        UTF16 *CodeUnits                                   = NULL;
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
        return CodeUnits;
    }

    static UTF32 UTF32_ExtractCodePoint(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);

        return String[0];
    }
    
    UTF8 *UTF8_Init(size_t NumCodeUnits) {
        AssertIO(NumCodeUnits > 0);
        UTF8 *String        = NULL;
        size_t StringSize   = sizeof(size_t) + NumCodeUnits + TextIO_NULLTerminatorSize;
        String              = (UTF8*) calloc(StringSize, sizeof(UTF8));
        AssertIO(String != NULL);
        static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8, "size_t is not 4 or 8 bytes!");
        if (sizeof(size_t) == 4) {
            uint8_t Unpacked[4];
            UnpackInteger32To8(NumCodeUnits, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            // Let's always write the value little endian first
            String[0] = Unpacked[3];
            String[1] = Unpacked[2];
            String[2] = Unpacked[1];
            String[3] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            String[0] = Unpacked[0];
            String[1] = Unpacked[1];
            String[2] = Unpacked[2];
            String[3] = Unpacked[3];
#endif
        } else if (sizeof(size_t) == 8) {
            uint8_t Unpacked[8];
            UnpackInteger64To8(NumCodeUnits, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            String[0] = Unpacked[7];
            String[1] = Unpacked[6];
            String[2] = Unpacked[5];
            String[3] = Unpacked[4];
            String[4] = Unpacked[3];
            String[5] = Unpacked[2];
            String[6] = Unpacked[1];
            String[7] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            String[0] = Unpacked[0];
            String[1] = Unpacked[1];
            String[2] = Unpacked[2];
            String[3] = Unpacked[3];
            String[4] = Unpacked[4];
            String[5] = Unpacked[5];
            String[6] = Unpacked[6];
            String[7] = Unpacked[7];
#endif
        }
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
        BufferIO_MemorySet8(&String[sizeof(size_t)], UTF8_Debug_Text_8, NumCodeUnits);
#endif
        return &String[sizeof(size_t)];
    }
    
    UTF16 *UTF16_Init(size_t NumCodeUnits) {
        AssertIO(NumCodeUnits >= 1);
        UTF16 *String       = NULL;
        size_t StringSize   = (sizeof(size_t) / sizeof(UTF16)) + NumCodeUnits + TextIO_NULLTerminatorSize;
        String              = (UTF16*) calloc(StringSize, sizeof(UTF16));

        static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8, "size_t is not 4 or 8 bytes!");
        if (sizeof(size_t) == 4) {
            uint16_t Unpacked[2];
            UnpackInteger32To16(NumCodeUnits, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            String[0] = Unpacked[1];
            String[1] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            String[0] = Unpacked[0];
            String[1] = Unpacked[1];
#endif
        } else if (sizeof(size_t) == 8) {
            uint16_t Unpacked[4];
            UnpackInteger64To16(NumCodeUnits, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            String[0] = Unpacked[3];
            String[1] = Unpacked[2];
            String[2] = Unpacked[1];
            String[3] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            String[0] = Unpacked[0];
            String[1] = Unpacked[1];
            String[2] = Unpacked[2];
            String[3] = Unpacked[3];
#endif
        }

#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
        BufferIO_MemorySet16(String, UTF16_Debug_Text_16BE, StringSize);
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
        BufferIO_MemorySet16(String, UTF16_Debug_Text_16LE, StringSize);
#endif /* ByteOrder */
#endif /* Debug */
        AssertIO(String != NULL);
        return &String[(sizeof(size_t) / sizeof(UTF16))];
    }
    
    UTF32 *UTF32_Init(size_t NumCodePoints) {
        AssertIO(NumCodePoints >= 1);
        UTF32 *String       = NULL;
        size_t StringSize   = (sizeof(size_t) / sizeof(UTF16)) + NumCodePoints + TextIO_NULLTerminatorSize;
        String              = (UTF32*) calloc(StringSize, sizeof(UTF32));
        AssertIO(String != NULL);

        static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8, "size_t is not 4 or 8 bytes!");
        if (sizeof(size_t) == 4) {
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            String[0] = NumCodePoints;
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            String[0] = NumCodePoints;
#endif
        } else if (sizeof(size_t) == 8) {
            uint32_t Unpacked[2];
            UnpackInteger64To32(NumCodePoints, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            String[0] = Unpacked[1];
            String[1] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            String[0] = Unpacked[0];
            String[1] = Unpacked[1];
#endif
        }

#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
        BufferIO_MemorySet32(String, UTF32_Debug_Text_32BE, StringSize);
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
        BufferIO_MemorySet32(String, UTF32_Debug_Text_32LE, StringSize);
#endif /* ByteOrder */
#endif /* Debug */
        return &String[(sizeof(size_t) / sizeof(UTF32))];
    }

    uint64_t UTF8_Hash(UTF8 *String) {
        AssertIO(String != NULL);
        uint64_t Hash     = 0;
        size_t   CodeUnit = 0;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            Hash         += (UTF8_ExtractCodePoint(&String[CodeUnit]) * 0x935d50f3) % 0x25D56F;
            CodeUnit     += UTF8_GetCodePointSizeInCodeUnits(String[CodeUnit]);
        }
        return Hash;
    }

    uint64_t UTF16_Hash(UTF16 *String) {
        AssertIO(String != NULL);
        uint64_t Hash     = 0;
        size_t   CodeUnit = 0;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            Hash         += (UTF16_ExtractCodePoint(&String[CodeUnit]) * 0x935d50f3) % 0x25D56F;
            CodeUnit     += UTF16_GetCodePointSizeInCodeUnits(String[CodeUnit]);
        }
        return Hash;
    }

    uint64_t UTF32_Hash(UTF32 *String) {
        AssertIO(String != NULL);
        uint64_t Hash      = 0;
        size_t   CodePoint = 0;
        while (String[CodePoint] != TextIO_NULLTerminator) {
            Hash          += (UTF32_ExtractCodePoint(&String[CodePoint]) * 0x935d50f3) % 0x25D56F;
            CodePoint     += 1;
        }
        return Hash;
    }
    
    void UTF8_Set(UTF8 *String, UTF8 Value, size_t NumCodeUnits) {
        AssertIO(String != NULL);
        BufferIO_MemorySet8((uint8_t*) String, (uint8_t) Value, NumCodeUnits);
    }
    
    void UTF16_Set(UTF16 *String, UTF16 Value, size_t NumCodeUnits) {
        AssertIO(String != NULL);
        BufferIO_MemorySet16((uint16_t*) String, (uint16_t) Value, NumCodeUnits);
    }
    
    void UTF32_Set(UTF32 *String, UTF32 Value, size_t NumCodePoints) {
        AssertIO(String != NULL);
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
    
    UTF32 UTF8_ExtractCodePoint(ImmutableString_UTF8 StartCodeUnit) { // The CodeUnits have already been checked
        AssertIO(StartCodeUnit != NULL);
        UTF32 CodePoint                       = 0;
        UTF8 *Extracted                   = UTF8_ExtractCodePointAsCodeUnits(StartCodeUnit);
        switch (UTF8_GetCodePointSizeInCodeUnits(StartCodeUnit[0])) {
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
        AssertIO(CodeUnits != NULL);
        UTF32 CodePoint                       = 0;
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
        return CodePoint;
    }

    UTF8 *UTF8_ExtractGrapheme(ImmutableString_UTF8 String, size_t GraphemeIndex) { // We could just return the size of the Grapheme in UTF8 CodeUnits
        AssertIO(String != NULL);
        UTF8 *Grapheme        = NULL;
        /*
         Loop over chunks of CodeUnits extracting CodePoints, looking up each Codepoint in the Grapheme extender table; keeping track of the number of codeunits in this Grapheme.
         */
        UTF32 CodePoint   = UTF8_ExtractCodePoint(String);
        if (UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint)) {
            // If this CodePoint is a grapheme extender, we're kinda in the middle of a Grapheme, and we need to discard until we get to a codepoint that is not a Grapheme extender
        }

        UTF32 *String32   = UTF8_Decode(String);
        UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, GraphemeIndex);
        UTF32_Deinit(String32);
        Grapheme          = UTF8_Encode(Grapheme32);
        UTF32_Deinit(Grapheme32);
        return Grapheme;
    }

    UTF16 *UTF16_ExtractGrapheme(ImmutableString_UTF16 String, size_t GraphemeIndex) {
        AssertIO(String != NULL);
        UTF16 *Grapheme        = NULL;
        UTF32 *String32   = UTF16_Decode(String);
        UTF32 *Grapheme32 = UTF32_ExtractGrapheme(String32, GraphemeIndex);
        UTF32_Deinit(String32);
        Grapheme          = UTF16_Encode(Grapheme32);
        UTF32_Deinit(Grapheme32);
        return Grapheme;
    }

    UTF32 *UTF32_ExtractGrapheme(ImmutableString_UTF32 String, size_t GraphemeIndex) {
        AssertIO(String != NULL);
        UTF32 *Grapheme         = NULL;
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
        return Grapheme;
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
        AssertIO(String != NULL);
        size_t GraphemeSize         = 1ULL;
        size_t CodeUnit         = OffsetInCodeUnits;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            UTF32 CodePoint1    = UTF8_ExtractCodePoint(&String[CodeUnit]);
            bool IsGraphemeExt  = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint1);
            if (IsGraphemeExt == true) {
                GraphemeSize   += 1;
                CodeUnit       += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint1);
            }
        }
        return GraphemeSize;
    }
    
    size_t UTF16_GetGraphemeSizeInCodeUnits(UTF16 *String, size_t OffsetInCodeUnits) {
        AssertIO(String != NULL);
        size_t GraphemeSize           = 1ULL;
        size_t   CodeUnit         = OffsetInCodeUnits;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            UTF32 CodePoint1      = UTF16_ExtractCodePoint(&String[CodeUnit]);
            bool IsGraphemeExt    = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, CodePoint1);
            if (IsGraphemeExt == true) {
                GraphemeSize     += 1;
                CodeUnit         += UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint1);
            }
        }
        return GraphemeSize;
    }
    
    size_t UTF32_GetGraphemeSizeInCodePoints(UTF32 *String, size_t OffsetInCodePoints) {
        AssertIO(String != NULL);
        size_t GraphemeSize          = 1ULL;
        size_t CodePoint         = OffsetInCodePoints;
        while (String[CodePoint] != TextIO_NULLTerminator) {
            bool IsGraphemeExt   = UTF32_IsCodePointInTable(GraphemeExtensionTable, GraphemeExtensionTableSize, String[CodePoint]);
            if (IsGraphemeExt == true) {
                GraphemeSize    += 1;
                CodePoint       += 1;
            }
        }
        return GraphemeSize;
    }
    
    size_t UTF8_GetStringSizeInCodeUnits(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        size_t StringSizeInCodeUnits = String[-sizeof(size_t)];
        /*
        while (String[StringSizeInCodeUnits] != TextIO_NULLTerminator) {
            StringSizeInCodeUnits += 1;
        }
         */
        return StringSizeInCodeUnits;
    }
    
    size_t UTF16_GetStringSizeInCodeUnits(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        size_t StringSizeInCodeUnits = String[-(sizeof(size_t) / sizeof(UTF16))];
        while (String[StringSizeInCodeUnits] != TextIO_NULLTerminator) {
            StringSizeInCodeUnits += 1;
        }
        return StringSizeInCodeUnits;
    }
    
    static size_t UTF32_GetStringSizeInUTF8CodeUnits(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        size_t UTF8CodeUnits      = 0;
        size_t CodePoint          = 0;
        while (String[CodePoint] != TextIO_NULLTerminator) {
            UTF8CodeUnits        += UTF32_GetCodePointSizeInUTF8CodeUnits(String[CodePoint]);
            CodePoint            += 1;
        }
        return UTF8CodeUnits;
    }
    
    static size_t UTF32_GetStringSizeInUTF16CodeUnits(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        size_t UTF16CodeUnits         = 0;
        size_t CodePoint          = 0;
        while (String[CodePoint] != TextIO_NULLTerminator) {
            UTF16CodeUnits       += UTF32_GetCodePointSizeInUTF16CodeUnits(String[CodePoint]);
            CodePoint            += 1;
        }
        return UTF16CodeUnits;
    }
    
    size_t UTF8_GetStringSizeInCodePoints(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        size_t StringSizeInCodePoints = 0;
        size_t CodeUnit               = 0;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            StringSizeInCodePoints   += 1;
            CodeUnit                 += UTF8_GetCodePointSizeInCodeUnits(String[CodeUnit]);
        }
        return StringSizeInCodePoints;
    }
    
    size_t UTF16_GetStringSizeInCodePoints(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        size_t NumCodePoints         = 0;
        size_t CodeUnit              = 0;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            CodeUnit                += UTF16_GetCodePointSizeInCodeUnits(String[CodeUnit]);
            NumCodePoints           += 1;
        }
        return NumCodePoints;
    }
    
    size_t UTF32_GetStringSizeInCodePoints(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        size_t NumCodePoints = String[-(sizeof(size_t) / sizeof(UTF32))];
        while (String[NumCodePoints] != TextIO_NULLTerminator) {
            NumCodePoints += 1;
        }
        return NumCodePoints;
    }

    /*!
     FormatIO needs a similar function for replacing format specifiers
     The general concept is: Take two strings, compare their sizes, saying the difference of the second string from the first.

     All I need now is a good name.

     ssize_t UTF32_CompareStringSizesInCodePoints(UTF32 *String1, UTF32 *String2);

     e.g: String1 = 1, String2 = 3, then +2 should be returned.
     */

    ssize_t UTF32_CompareStringSizesInCodePoints(UTF32 *Old, UTF32 *New) {
        AssertIO(Old != NULL);
        AssertIO(New != NULL);
        ssize_t SizeDifference = 0;
        size_t OldSize     = UTF32_GetStringSizeInCodePoints(Old);
        size_t NewSize     = UTF32_GetStringSizeInCodePoints(New);
        SizeDifference     = NewSize - OldSize;
        return SizeDifference;
    }
    
    size_t UTF8_GetStringSizeInGraphemes(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        size_t NumGraphemes    = 0;
        UTF32 *Decoded     = UTF8_Decode(String);
        NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
        UTF32_Deinit(Decoded);
        return NumGraphemes;
    }
    
    size_t UTF16_GetStringSizeInGraphemes(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        size_t NumGraphemes    = 0;
        UTF32 *Decoded     = UTF16_Decode(String);
        NumGraphemes       = UTF32_GetStringSizeInGraphemes(Decoded);
        UTF32_Deinit(Decoded);
        return NumGraphemes;
    }
    
    size_t UTF32_GetStringSizeInGraphemes(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        size_t NumGraphemes            = 0;
        size_t CodePoint               = 1;
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
        return NumGraphemes;
    }

    size_t UTF8_GetWordSizeInCodePoints(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        size_t WordSize      = 0;
        size_t CodeUnit  = 0;
        UTF32 CodePoint  = UTF8_ExtractCodePoint(&String[CodeUnit]);
        while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == false) {
            WordSize    += 1;
            CodeUnit    += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint);
            CodePoint    = UTF8_ExtractCodePoint(&String[CodeUnit]);
        }
        return WordSize;
    }
    
    size_t UTF16_GetWordSizeInCodePoints(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        size_t WordSize      = 0;
        size_t CodeUnit  = 0;
        UTF32 CodePoint  = UTF16_ExtractCodePoint(&String[CodeUnit]);
        while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == false) {
            WordSize    += 1;
            CodeUnit    += UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint);
            CodePoint    = UTF16_ExtractCodePoint(&String[CodeUnit]);
        }
        return WordSize;
    }

    size_t UTF8_GetWordBreakSizeInCodePoints(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        size_t WordBreakSize   = 0;
        /* Count the number of word breaks, for example /cr/lf before the start of a new word */
        size_t CodeUnit    = 0;
        UTF32  CodePoint   = UTF8_ExtractCodePoint(&String[CodeUnit]);
        while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == true) {
            WordBreakSize += 1;
            CodeUnit      += UTF32_GetCodePointSizeInUTF8CodeUnits(CodePoint);
            CodePoint      = UTF8_ExtractCodePoint(&String[CodeUnit]);
        }
        return WordBreakSize;
    }

    size_t UTF16_GetWordBreakSizeInCodePoints(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        size_t WordBreakSize   = 0;
        size_t CodeUnit    = 0;
        UTF32 CodePoint    = UTF16_ExtractCodePoint(&String[CodeUnit]);
        while (UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint) == true) {
            WordBreakSize += 1;
            CodeUnit      += UTF32_GetCodePointSizeInUTF16CodeUnits(CodePoint);
            CodePoint      = UTF16_ExtractCodePoint(&String[CodeUnit]);
        }
        return WordBreakSize;
    }

    /* Low Level functions, be careful */
    bool UTF32_ShiftCodePoints(UTF32 *String, size_t StringSize, size_t StringCapacity, size_t Start, size_t NumCodePointsToShift) {
        AssertIO(String != NULL);
        AssertIO(StringSize + NumCodePointsToShift <= StringCapacity);
        for (size_t New = StringSize + NumCodePointsToShift; New > Start; New--) {
            String[New] = String[New - NumCodePointsToShift];
        }
        return Yes;
    }

    bool UTF32_ReplaceInPlace(UTF32 *String, const UTF32 *const Replacement, size_t Start) {
        AssertIO(String != NULL);
        AssertIO(Replacement != NULL);
        bool SucessfullyReplaced = No;
        size_t StringSize      = UTF32_GetStringSizeInCodePoints(String);
        size_t ReplacementSize = UTF32_GetStringSizeInCodePoints(Replacement);
        AssertIO(ReplacementSize + Start <= StringSize);
        size_t CodePoint = 0;
        while (String[CodePoint + Start] != TextIO_NULLTerminator && Replacement[CodePoint] != TextIO_NULLTerminator) {
            String[Start + CodePoint] = Replacement[CodePoint];
            CodePoint += 1;
        }
        return SucessfullyReplaced;
    }
    /* Low Level functions, be careful */
    
    bool UTF8_HasBOM(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        bool StringHasABOM        = No;
        size_t StringSize     = UTF8_GetStringSizeInCodeUnits(String);
        if (StringSize >= 3) {
            if (String[0] == UTF8BOM_1 && String[1] == UTF8BOM_2 && String[2] == UTF8BOM_3) {
                StringHasABOM = Yes;
            }
        }
        return StringHasABOM;
    }
    
    bool UTF16_HasBOM(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        bool StringHasABOM    = No;
        if (String[0] == UTF16BOM_LE || String[0] == UTF16BOM_BE) {
            StringHasABOM = Yes;
        }
        return StringHasABOM;
    }
    
    bool UTF32_HasBOM(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        bool StringHasABOM    = No;
        if (String[0] == UTF32BOM_LE || String[0] == UTF32BOM_BE) {
            StringHasABOM = Yes;
        }
        return StringHasABOM;
    }
    
    bool UTF8_IsWordBreak(const UTF8 *CodePoint) {
        AssertIO(CodePoint != NULL);
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
        AssertIO(String != NULL);
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
        AssertIO(String != NULL);
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
        AssertIO(String != NULL);
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

    /*!
     @abstract The point of this function is to make a strign safe, to make sure that like Trojan Source isn't possible, etc.
     @abstract2 This function IS NOT meant to fix look-alike codepoints masquerading as others, for that see UTF32_
     */
    void UTF32_SanitizeBIDI() {
        // So, we need to know when and where BIDI directionality codepoints should and shouldn't be present.

        /*
         the goal here is to balance BIDI characters; in ordr to do that we need to know where right to left characters are.

         Which means we need a new table in TextIOTables to list either the ranges of character types, or better yet, just a list of all characters that can be read right-to-left or down-to-up.

         Until an example is found that actually needs them, this function will just count the number of BIDI characters in a string and if nessessary will replace them with nothing.
         */
    }

    void UTF32_ReplaceLookalikes() {

    }
    
    bool UTF8_IsUNCPath(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        bool StringIsUNCPath = No;
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
        return StringIsUNCPath;
    }
    
    bool UTF16_IsUNCPath(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        bool StringIsUNCPath = No;
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
        return StringIsUNCPath;
    }
    
    bool UTF32_IsUNCPath(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        bool StringIsUNCPath        = No;
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
        return StringIsUNCPath;
    }
    
    bool UTF8_IsAbsolutePath(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        bool PathIsAbsolute = No;
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
        return PathIsAbsolute;
    }
    
    bool UTF16_IsAbsolutePath(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        bool PathIsAbsolute        = No;
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
        return PathIsAbsolute;
    }
    
    bool UTF32_IsAbsolutePath(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        bool PathIsAbsolute        = No;
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
        return PathIsAbsolute;
    }

    bool UTF32_IsWhitespace(UTF32 CodePoint) {
        bool IsWhitespace = false;
        bool IsLineBreak  = UTF32_IsCodePointInTable(LineBreakTable, LineBreakTableSize, CodePoint);
        bool IsWordBreak  = UTF32_IsCodePointInTable(WordBreakTable, WordBreakTableSize, CodePoint);
        if (IsLineBreak || IsWordBreak) {
            IsWhitespace  = true;
        }
        return IsWhitespace;
    }
    
    bool UTF8_HasNewLine(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        bool StringHasNewLine = No;
        UTF32 *String32   = UTF8_Decode(String);
        StringHasNewLine  = UTF32_HasNewLine(String32);
        UTF32_Deinit(String32);
        return StringHasNewLine;
    }
    
    bool UTF16_HasNewLine(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        bool StringHasNewLine = No;
        UTF32 *String32   = UTF16_Decode(String);
        StringHasNewLine  = UTF32_HasNewLine(String32);
        UTF32_Deinit(String32);
        return StringHasNewLine;
    }
    
    bool UTF32_HasNewLine(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        bool HasNewLine                  = No;
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
        return HasNewLine;
    }
    
    bool UTF32_IsUpperCase(UTF32 CodePoint) { // TODO: Honestly, there should be two functions, one for ASCII and one for Unicode exended characters for not only uppercase, but also numbers
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
        AssertIO(String != NULL);
        size_t   CodeUnit    = 0ULL;
        bool     IsValidUTF8 = Yes;
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
        return IsValidUTF8;
    }
    
    bool UTF16_IsValid(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        size_t   CodeUnit             = 1ULL;
        bool     IsValidUTF16         = Yes;
        while (String[CodeUnit] != TextIO_NULLTerminator) {
            if ((String[CodeUnit - 1] >= UTF16HighSurrogateStart && String[CodeUnit - 1] <= UTF16HighSurrogateEnd) && (String[CodeUnit] <= UTF16LowSurrogateStart && String[CodeUnit] >= UTF16LowSurrogateEnd)) {
                IsValidUTF16 = No;
                break;
            }
            CodeUnit += 1;
        }
        return IsValidUTF16;
    }
    
    bool UTF32_IsValid(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        size_t   CodePoint       = 0ULL;
        bool     IsValidUTF32    = Yes;
        while (String[CodePoint] != TextIO_NULLTerminator) {
            if (String[CodePoint] >= UnicodeMaxCodePoint || (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd)) {
                IsValidUTF32 = No;
                break;
            }
            CodePoint       += 1;
        }
        return IsValidUTF32;
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
    
    UTF8 *UTF8_AddBOM(ImmutableString_UTF8 String, StringIO_BOMs BOM2Add) {
        AssertIO(String != NULL);
        AssertIO(BOM2Add != StringIO_BOM_Unspecified);
        UTF32 *String32  = UTF8_Decode(String);
        UTF32 *BOMAdded  = UTF32_AddBOM(String32, StringIO_BOM_Big);
        UTF32_Deinit(String32);
        UTF8 *StringWithBOM = UTF8_Encode(BOMAdded);
        return StringWithBOM;
    }
    
    UTF16 *UTF16_AddBOM(ImmutableString_UTF16 String, StringIO_BOMs BOM2Add) {
        AssertIO(String != NULL);
        AssertIO(BOM2Add != StringIO_BOM_Unspecified);
        UTF32 *String32  = UTF16_Decode(String);
        UTF32 *BOMAdded  = UTF32_AddBOM(String32, BOM2Add);
        UTF16 *StringWithBOM = UTF16_Encode(BOMAdded);
        UTF32_Deinit(String32);
        return StringWithBOM;
    }
    
    UTF32 *UTF32_AddBOM(ImmutableString_UTF32 String, StringIO_BOMs BOM2Add) {
        AssertIO(String != NULL);
        AssertIO(BOM2Add != StringIO_BOM_Unspecified);
        UTF32   *StringWithBOM        = NULL;
        UTF32    ByteOrder            = 0;
        if (String[0] != UTF32BOM_LE && String[0] != UTF32BOM_BE) {
            size_t StringSize     = UTF32_GetStringSizeInCodePoints(String);
            StringWithBOM         = UTF32_Init(StringSize + UnicodeBOMSizeInCodePoints);
            AssertIO(StringWithBOM != NULL);
            if (BOM2Add == StringIO_BOM_Native) {
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
                ByteOrder     = UTF32BOM_LE;
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
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
        }
        return StringWithBOM;
    }
    
    UTF8 *UTF8_RemoveBOM(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        UTF8    *BOMLessString                      = NULL;
        size_t   StringSize                         = 0ULL;
        StringSize                              = UTF8_GetStringSizeInCodeUnits(String);
        bool StringHasBOM                       = UTF8_HasBOM(String);
        if (StringHasBOM) {
            BOMLessString                       = UTF8_Init(StringSize - UTF8BOMSizeInCodeUnits);
            AssertIO(BOMLessString != NULL);
            for (size_t CodeUnit = 2ULL; CodeUnit < StringSize; CodeUnit++) {
                BOMLessString[CodeUnit - 3] = String[CodeUnit];
            }
        }
        return BOMLessString;
    }
    
    UTF16 *UTF16_RemoveBOM(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        UTF16   *BOMLessString                      = NULL;
        size_t   StringSize                         = 0ULL;
        StringSize                              = UTF16_GetStringSizeInCodeUnits(String);
        bool StringHasBOM                       = UTF16_HasBOM(String);
        if (StringHasBOM) {
            BOMLessString                       = UTF16_Init(StringSize - UTF16BOMSizeInCodeUnits);
            AssertIO(BOMLessString != NULL);
            for (size_t CodeUnit = 1ULL; CodeUnit < StringSize; CodeUnit++) {
                BOMLessString[CodeUnit - 1] = String[CodeUnit];
            }
        }
        return BOMLessString;
    }
    
    UTF32 *UTF32_RemoveBOM(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        UTF32   *BOMLessString                   = NULL;
        size_t   StringSize                      = 0ULL;
        StringSize                           = UTF32_GetStringSizeInCodePoints(String);
        bool StringHasBOM                    = UTF32_HasBOM(String);
        if (StringHasBOM) {
            BOMLessString                    = UTF32_Init(StringSize - UnicodeBOMSizeInCodePoints);
            AssertIO(BOMLessString != NULL);
            for (size_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                BOMLessString[CodePoint] = String[CodePoint + 1];
            }
        }
        return BOMLessString;
    }
    
    UTF32 *UTF8_Decode(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        size_t   CodeUnit                        = 0ULL;
        size_t   StringSize                      = UTF8_GetStringSizeInCodePoints(String);
        UTF32   *DecodedString                   = UTF32_Init(StringSize);
        AssertIO(DecodedString != NULL);
        for (size_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
            DecodedString[CodePoint]         = UTF8_ExtractCodePoint(&String[CodeUnit]);
            CodeUnit                        += UTF8_GetCodePointSizeInCodeUnits(String[CodePoint]);
        }
        return DecodedString;
    }
    
    UTF32 *UTF16_Decode(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        size_t   CodeUnit                        = 0ULL;
        size_t   StringSize                      = UTF16_GetStringSizeInCodePoints(String);
        UTF32   *DecodedString                   = UTF32_Init(StringSize);
        AssertIO(DecodedString != NULL);
        for (size_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
            DecodedString[CodePoint]         = UTF16_ExtractCodePoint(&String[CodeUnit]);
            CodeUnit                        += UTF16_GetCodePointSizeInCodeUnits(String[CodeUnit]);
        }
        return DecodedString;
    }
    
    UTF8 *UTF8_Encode(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        size_t   UTF8CodeUnits                     = UTF32_GetStringSizeInUTF8CodeUnits(String);
        UTF8    *EncodedString                     = UTF8_Init(UTF8CodeUnits);
        size_t   CodePoint                         = 0ULL;
        size_t   CodeUnit                          = 0ULL;
        uint8_t  Index                             = 0;
        AssertIO(EncodedString != NULL);
        while (String[CodePoint] != TextIO_NULLTerminator) {
            UTF8_EncodeCodePoint(String[CodePoint]);
            for (uint8_t CodeUnit2 = 0; CodeUnit2 < UTF32_GetCodePointSizeInUTF8CodeUnits(String[CodePoint]); CodeUnit2++) {
                EncodedString[CodeUnit]        = StringIO_PreallocateCodePoint_UTF8[Index];
            }
            CodePoint += 1;
        }
        return EncodedString;
    }
    
    UTF16 *UTF16_Encode(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        UTF16   *EncodedString                       = NULL;
        size_t   CodePoint                           = 0ULL;
        size_t   CodeUnit                            = 0ULL;
        size_t   NumCodeUnits                        = UTF32_GetStringSizeInUTF16CodeUnits(String);
        EncodedString                                = UTF16_Init(NumCodeUnits);
        AssertIO(EncodedString != NULL);
        // This is kinda dumb... just call UTF16_EncodeCodePoint for each UTF32 codepoint
        while (String[CodePoint] != TextIO_NULLTerminator) {
            UTF16_EncodeCodePoint(String[CodePoint]);
            for (uint8_t CodeUnit2 = 0; CodeUnit2 < UTF32_GetCodePointSizeInUTF16CodeUnits(String[CodePoint]); CodeUnit2++) {
                EncodedString[CodeUnit] = StringIO_PreallocateCodePoint_UTF16[CodeUnit2];
            }
            CodePoint += 1;
        }
        return EncodedString;
    }
    
    UTF8 *UTF16_Convert(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        UTF8 *String8       = NULL;
        UTF32 *String32 = UTF16_Decode(String);
        String8         = UTF8_Encode(String32);
        UTF32_Deinit(String32);
        return String8;
    }
    
    UTF16 *UTF8_Convert(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        UTF16 *String16     = NULL;
        UTF32 *String32 = UTF8_Decode(String);
        String16        = UTF16_Encode(String32);
        UTF32_Deinit(String32);
        return String16;
    }
    
    UTF8 *UTF8_Clone(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        UTF8 *Copy = NULL;
        size_t StringSizeInCodeUnits   = UTF8_GetStringSizeInCodeUnits(String);
        Copy                           = UTF8_Init(StringSizeInCodeUnits);
        AssertIO(Copy != NULL);
        for (size_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
            Copy[CodeUnit] = String[CodeUnit];
        }
        return Copy;
    }
    
    UTF16 *UTF16_Clone(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        UTF16 *Copy = NULL;
        size_t StringSizeInCodeUnits   = UTF16_GetStringSizeInCodeUnits(String);
        Copy                           = UTF16_Init(StringSizeInCodeUnits);
        AssertIO(Copy != NULL);
        for (size_t CodeUnit = 0ULL; CodeUnit < StringSizeInCodeUnits; CodeUnit++) {
            Copy[CodeUnit] = String[CodeUnit];
        }
        return Copy;
    }
    
    UTF32 *UTF32_Clone(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        UTF32 *Copy = NULL;
        size_t StringSizeInCodePoints   = UTF32_GetStringSizeInCodePoints(String);
        Copy                            = UTF32_Init(StringSizeInCodePoints);
        AssertIO(Copy != NULL);
        for (size_t CodePoint = 0ULL; CodePoint < StringSizeInCodePoints; CodePoint++) {
            Copy[CodePoint] = String[CodePoint];
        }
        return Copy;
    }
    
    UTF8 UTF8_Erase(UTF8 *String, UTF8 NewValue) {
        AssertIO(String != NULL);
        UTF8 Verification = 0xFE;
        size_t StringSize  = UTF8_GetStringSizeInCodePoints(String);
        for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
            String[CodeUnit] = NewValue;
        }
        Verification         = String[0];
        return Verification;
    }
    
    UTF16 UTF16_Erase(UTF16 *String, UTF16 NewValue) {
        AssertIO(String != NULL);
        UTF16 Verification = 0xFE;
        size_t StringSize  = UTF16_GetStringSizeInCodePoints(String);
        for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
            String[CodeUnit] = NewValue;
        }
        Verification = String[0];
        return Verification;
    }
    
    UTF32 UTF32_Erase(UTF32 *String, UTF32 NewValue) {
        AssertIO(String != NULL);
        UTF32 Verification = 0xFE;
        size_t StringSize  = UTF32_GetStringSizeInCodePoints(String);
        for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
            String[CodeUnit] = NewValue;
        }
        Verification = String[0];
        return Verification;
    }
    
    UTF8 *UTF8_Truncate(ImmutableString_UTF8 String, size_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        AssertIO(String != NULL);
        AssertIO(NumGraphemes >= 1);
        UTF8 *Truncated = NULL;
        UTF32 *String32    = UTF8_Decode(String);
        UTF32 *Truncated32 = UTF32_Truncate(String32, NumGraphemes);
        UTF32_Deinit(String32);
        Truncated          = UTF8_Encode(Truncated32);
        UTF32_Deinit(Truncated32);
        return Truncated;
    }
    
    UTF16 *UTF16_Truncate(ImmutableString_UTF16 String, size_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        AssertIO(String != NULL);
        AssertIO(NumGraphemes >= 1);
        UTF16 *Truncated = NULL;
        UTF32 *String32    = UTF16_Decode(String);
        UTF32 *Truncated32 = UTF32_Truncate(String32, NumGraphemes);
        UTF32_Deinit(String32);
        Truncated          = UTF16_Encode(Truncated32);
        UTF32_Deinit(Truncated32);
        return Truncated;
    }
    
    UTF32 *UTF32_Truncate(ImmutableString_UTF32 String, size_t NumGraphemes) { // FIXME: Rewrite this so it works on Graphemes not CodeUnits
        AssertIO(String != NULL);
        AssertIO(NumGraphemes >= 1);
        UTF32 *Truncated = NULL;
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
        return Truncated;
    }
    
    void UTF8_WriteGrapheme(FILE *Source, ImmutableString_UTF8 Grapheme) {
        AssertIO(Source != NULL);
        AssertIO(Grapheme != NULL);
        size_t   StringSize       = UTF8_GetStringSizeInCodeUnits(Grapheme);
        size_t   CodeUnitsWritten = FileIO_Write(Source, Grapheme, sizeof(UTF8), StringSize);
        AssertIO(CodeUnitsWritten == StringSize);
    }
    
    void UTF16_WriteGrapheme(FILE *Source, ImmutableString_UTF16 Grapheme) {
        AssertIO(Source != NULL);
        AssertIO(Grapheme != NULL);
        size_t   StringSize       = UTF16_GetStringSizeInCodeUnits(Grapheme);
        size_t   CodeUnitsWritten = FileIO_Write(Source, Grapheme, sizeof(UTF8), StringSize);
        AssertIO(CodeUnitsWritten == StringSize);
    }
    
    UTF8 UTF8_ReadCodeUnit(FILE *Source) { // Replaces fgetc, getc, getchar; depending on your definition of "Character"
        AssertIO(Source != NULL);
        UTF8 CodeUnit = 0;
        FileIO_Read(Source, &CodeUnit, sizeof(UTF8), 1);
        return CodeUnit;
    }
    
    UTF16 UTF16_ReadCodeUnit(FILE *Source) {
        AssertIO(Source != NULL);
        UTF16 CodeUnit = 0;
        FileIO_Read(Source, &CodeUnit, sizeof(UTF16), 1);
        return CodeUnit;
    }
    
    UTF32 UTF8_ReadCodePoint(FILE *Source) {
        AssertIO(Source != NULL);
        UTF32 CodePoint         = 0;
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
        CodePoint           = UTF8_Decode(&StringIO_PreallocateCodePoint_UTF8[0]);
        return CodePoint;
    }
    
    UTF32 UTF16_ReadCodePoint(FILE *Source) {
        AssertIO(Source != NULL);
        UTF32 CodePoint          = 0;
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
        CodePoint         = UTF16_Decode(&StringIO_PreallocateCodePoint_UTF16[0]);
        return CodePoint;
    }
    
    UTF32 UTF32_ReadCodePoint(FILE *Source) {
        AssertIO(Source != NULL);
        UTF32 CodePoint          = 0;
        size_t CodeUnitsRead = FileIO_Read(Source, &CodePoint, sizeof(UTF32), 1);
        return CodePoint;
    }
    
    UTF8 *UTF8_ReadGrapheme(FILE *Source) {
        AssertIO(Source != NULL);
        UTF8 *Grapheme       = NULL;
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
            CodePointSizeInCodeUnits      += UTF8_GetCodePointSizeInCodeUnits(CodeUnit);
            FileIO_Seek(Source, CodePointSizeInCodeUnits, SeekType_Current);
            Grapheme                       = UTF8_Init(CodePointSizeInCodeUnits);
            CodeUnitsRead                  = FileIO_Read(Source, &Grapheme, sizeof(UTF8), CodePointSizeInCodeUnits);
            AssertIO(CodeUnitsRead == CodePointSizeInCodeUnits);
            FileIO_Read(Source, &Grapheme, sizeof(UTF8), CodePointSizeInCodeUnits);
            UTF32 *CodePoint           = UTF8_Decode(Grapheme);
            for (size_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                if (CodePoint[0] == GraphemeExtensionTable[GraphemeExtension]) {
                } else if (CodePoint[0] != GraphemeExtensionTable[GraphemeExtension] && GraphemeExtension == GraphemeExtensionTableSize - 1) {
                    GraphemeFound      = Yes;
                }
            }
        }
        return Grapheme;
    }
    
    UTF16 *UTF16_ReadGrapheme(FILE *Source) {
        AssertIO(Source != NULL);
        UTF16 *Grapheme                            = NULL;
        bool     GraphemeFound                 = No;
        size_t   CodePointSizeInCodeUnits      = 0;
        size_t   CodeUnitsRead                 = 0;
        UTF16    CodeUnit                      = 0;
        while (GraphemeFound == No) {
            CodeUnitsRead                      = FileIO_Read(Source, &CodeUnit, sizeof(UTF16), 1);
            CodePointSizeInCodeUnits      += UTF16_GetCodePointSizeInCodeUnits(CodeUnit);
            FileIO_Seek(Source, CodePointSizeInCodeUnits, SeekType_Current);
            Grapheme                       = UTF16_Init(CodePointSizeInCodeUnits);
            CodeUnitsRead                  = FileIO_Read(Source, &Grapheme, sizeof(UTF16), CodePointSizeInCodeUnits);
            AssertIO(CodeUnitsRead == CodePointSizeInCodeUnits);
            FileIO_Read(Source, &Grapheme, sizeof(UTF16), CodePointSizeInCodeUnits);
            UTF32 *CodePoint           = UTF16_Decode(Grapheme);
            for (size_t GraphemeExtension = 0ULL; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                if (CodePoint[0] == GraphemeExtensionTable[GraphemeExtension]) {
                } else if (CodePoint[0] != GraphemeExtensionTable[GraphemeExtension] && GraphemeExtension == GraphemeExtensionTableSize - 1) {
                    GraphemeFound      = Yes;
                }
            }
        }
        return Grapheme;
    }
    
    size_t UTF32_LineBreakSize(UTF32 *String) {
        AssertIO(String != NULL);
        size_t CodePoint   = 0;
        while (UTF32_IsCodePointInTable(LineBreakTable, LineBreakTableSize, String[CodePoint])) {
            CodePoint += 1;
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
        AssertIO(Source != NULL);
        UTF8 *Sentence                                     = NULL;
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
        return Sentence;
    }
    
    UTF16 *UTF16_ReadSentence(FILE *Source) {
        AssertIO(Source != NULL);
        UTF16 *Sentence                                    = NULL;

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
        return Sentence;
    }
    
    void UTF8_File_WriteString(FILE *OutputFile, ImmutableString_UTF8 String) {
        AssertIO(OutputFile != NULL);
        AssertIO(String != NULL);

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

        AssertIO(CodeUnitsWritten == StringSize);
    }
    
    void UTF16_File_WriteString(FILE *OutputFile, ImmutableString_UTF16 String) {
        AssertIO(OutputFile != NULL);
        AssertIO(String != NULL);

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

        AssertIO(CodeUnitsWritten == StringSize);
    }
    
    void UTF32_File_WriteString(FILE *OutputFile, ImmutableString_UTF32 String) {
        AssertIO(OutputFile != NULL);
        AssertIO(String != NULL);

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
        AssertIO(CodeUnitsWritten == StringSize);
    }
    
    UTF8 *UTF8_Reverse(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        UTF8 *Reversed        = NULL;
        UTF32 *String32   = UTF8_Decode(String);
        UTF32 *Reversed32 = UTF32_Reverse(String32);
        UTF32_Deinit(String32);
        Reversed          = UTF8_Encode(Reversed32);
        UTF32_Deinit(Reversed32);
        return Reversed;
    }
    
    UTF16 *UTF16_Reverse(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        UTF16 *Reversed = NULL;
        UTF32 *String32   = UTF16_Decode(String);
        UTF32 *Reversed32 = UTF32_Reverse(String32);
        UTF32_Deinit(String32);
        Reversed          = UTF16_Encode(Reversed32);
        UTF32_Deinit(Reversed32);
        return Reversed;
    }
    
    UTF32 *UTF32_Reverse(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        UTF32 *Reverse = NULL;
        size_t StringSize   = UTF32_GetStringSizeInCodePoints(String);
        Reverse             = UTF32_Init(StringSize);
        AssertIO(Reverse != NULL);
        for (size_t CodePoint = StringSize; CodePoint > 0; CodePoint--) {
            Reverse[CodePoint] = String[CodePoint - StringSize];
        }
        return Reverse;
    }
    
    size_t UTF8_Find(UTF8 *String, size_t StringLength, UTF8 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        AssertIO(String != NULL);
        AssertIO(String2Find != NULL);
        AssertIO(WhitespaceType != WhitespaceType_Unspecified);
        size_t Offset = 0;
        size_t String2FindSize = UTF8_GetStringSizeInCodeUnits(String2Find);
        AssertIO(StringLength >= String2FindSize);
        if (WhitespaceType == WhitespaceType_Insignificant) {
            while (UTF8_IsWordBreak(&String[Offset])) {
                Offset += 1;
            }
        }

        for (size_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
            for (size_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                    Offset = SubCodePoint;
                }
            }
        }
        return Offset;
    }
    
    size_t UTF16_Find(UTF16 *String, size_t StringLength, UTF16 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        AssertIO(String != NULL);
        AssertIO(String2Find != NULL);
        AssertIO(WhitespaceType != WhitespaceType_Unspecified);
        size_t Offset          = 0;
        size_t String2FindSize = UTF16_GetStringSizeInCodeUnits(String2Find);
        AssertIO(StringLength >= String2FindSize);
        if (WhitespaceType == WhitespaceType_Insignificant) {
            while (UTF16_IsWordBreak(String[Offset])) {
                Offset += 1;
            }
        }

        for (size_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
            for (size_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                    Offset = SubCodePoint;
                }
            }
        }
        return Offset;
    }
    
    size_t UTF32_Find(UTF32 *String, size_t StringLength, UTF32 *String2Find, StringIO_WhitespaceTypes WhitespaceType) {
        AssertIO(String != NULL);
        AssertIO(String2Find != NULL);
        AssertIO(WhitespaceType != WhitespaceType_Unspecified);
        size_t Offset = 0;
        size_t String2FindSize = UTF32_GetStringSizeInCodePoints(String2Find);
        AssertIO(StringLength >= String2FindSize);
        if (WhitespaceType == WhitespaceType_Insignificant) {
            while (UTF32_IsWordBreak(String[Offset])) {
                Offset += 1;
            }
        }

        for (size_t SubCodePoint = 0ULL; SubCodePoint < String2FindSize; SubCodePoint++) {
            for (size_t StringCodePoint = Offset; StringCodePoint < Offset + StringLength; StringCodePoint++) {
                if (String[StringCodePoint] != String2Find[SubCodePoint]) {
                    Offset = SubCodePoint;
                }
            }
        }
        return Offset;
    }
    
    size_t UTF8_FindSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        AssertIO(SubString != NULL);
        ssize_t FoundOffset    = 0LL;
        UTF32 *String32    = UTF8_Decode(String);
        UTF32 *SubString32 = UTF8_Decode(SubString);
        FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
        UTF32_Deinit(String32);
        UTF32_Deinit(SubString32);
        return FoundOffset;
    }
    
    size_t UTF16_FindSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        AssertIO(SubString != NULL);
        ssize_t FoundOffset    = 0LL;
        UTF32 *String32    = UTF16_Decode(String);
        UTF32 *SubString32 = UTF16_Decode(SubString);
        FoundOffset        = UTF32_FindSubString(String32, SubString32, Offset, Length);
        UTF32_Deinit(String32);
        UTF32_Deinit(SubString32);
        return FoundOffset;
    }
    
    size_t UTF32_FindSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        AssertIO(SubString != NULL);
        size_t  StringSize            = UTF32_GetStringSizeInCodePoints(String);
        size_t  SubStringSize         = UTF32_GetStringSizeInCodePoints(SubString);
        ssize_t MatchingOffset        = 0LL;
        AssertIO(StringSize < Offset + Length);
        for (size_t SubCodePoint = 0ULL; SubCodePoint < SubStringSize; SubCodePoint++) {
            for (size_t StringCodePoint = Offset; StringCodePoint < Offset + Length; StringCodePoint++) {
                if (String[StringCodePoint] != SubString[SubCodePoint]) {
                    MatchingOffset = -1;
                }
            }
        }
        return MatchingOffset;
    }
    
    UTF8 *UTF8_ExtractSubString(ImmutableString_UTF8 String, size_t Offset, size_t NumCodeUnits) {
        AssertIO(String != NULL);
        UTF8 *ExtractedSubString = NULL;
        UTF32 *String32    = UTF8_Decode(String);
        UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, NumCodeUnits);
        ExtractedSubString = UTF8_Encode(Extracted32);
        UTF32_Deinit(String32);
        UTF32_Deinit(Extracted32);
        return ExtractedSubString;
    }
    
    UTF16 *UTF16_ExtractSubString(ImmutableString_UTF16 String, size_t Offset, size_t NumCodeUnits) {
        AssertIO(String != NULL);
        UTF16 *ExtractedSubString = NULL;
        UTF32 *String32    = UTF16_Decode(String);
        UTF32 *Extracted32 = UTF32_ExtractSubString(String32, Offset, NumCodeUnits);
        ExtractedSubString = UTF16_Encode(Extracted32);
        UTF32_Deinit(String32);
        UTF32_Deinit(Extracted32);
        return ExtractedSubString;
    }
    
    UTF32 *UTF32_ExtractSubString(ImmutableString_UTF32 String, size_t Offset, size_t NumCodePoints) {
        AssertIO(String != NULL);
        size_t    StringSize                            = UTF32_GetStringSizeInCodePoints(String);
        AssertIO(StringSize >= Offset + NumCodePoints);
        UTF32    *ExtractedString                       = NULL;

        ExtractedString                             = UTF32_Init(NumCodePoints);
        AssertIO(ExtractedString != NULL);
        for (size_t CodePoint = Offset; CodePoint < Offset + NumCodePoints; CodePoint++) {
            ExtractedString[CodePoint - Offset] = String[CodePoint];
        }
        return ExtractedString;
    }
    
    UTF8 *UTF8_Create(ImmutableString_UTF8 Padding, size_t Times2Pad) {
        AssertIO(Padding != NULL);
        AssertIO(Times2Pad >= 1);
        UTF8 *Padded           = NULL;
        UTF32 *Padding32   = UTF8_Decode(Padding);
        UTF32 *Padded32    = UTF32_Create(Padding32, Times2Pad);
        UTF32_Deinit(Padding32);
        Padded             = UTF8_Encode(Padded32);
        return Padded;
    }
    
    UTF16 *UTF16_Create(ImmutableString_UTF16 Padding, size_t Times2Pad) {
        AssertIO(Padding != NULL);
        AssertIO(Times2Pad >= 1);
        UTF16 *Padded          = NULL;
        UTF32 *Padding32   = UTF16_Decode(Padding);
        UTF32 *Padded32    = UTF32_Create(Padding32, Times2Pad);
        UTF32_Deinit(Padding32);
        Padded             = UTF16_Encode(Padded32);
        return Padded;
    }
    
    UTF32 *UTF32_Create(ImmutableString_UTF32 Padding, size_t Times2Pad) {
        AssertIO(Padding != NULL);
        AssertIO(Times2Pad >= 1);
        UTF32 *Padded                     = NULL;

        size_t Size                   = UTF32_GetStringSizeInCodePoints(Padding);
        Padded                        = UTF32_Init(Size * Times2Pad);
        if (Padded != NULL) {
            for (size_t PadString = 0; PadString < Size * Times2Pad; PadString++) {
                for (size_t PaddingCodePoint = 0; PaddingCodePoint < Size; PaddingCodePoint++) {
                    Padded[PadString] = Padding[PaddingCodePoint];
                }
            }
        }
        return Padded;
    }
    
    UTF8 *UTF8_SubstituteSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substitution, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        AssertIO(Substitution != NULL);
        UTF8 *Replaced8           = NULL;
        UTF32 *String32       = UTF8_Decode(String);
        UTF32 *Substitution32 = UTF8_Decode(Substitution);
        UTF32 *Replaced32     = UTF32_SubstituteSubString(String32, Substitution32, Offset, Length);
        Replaced8             = UTF8_Encode(Replaced32);
        UTF32_Deinit(String32);
        UTF32_Deinit(Substitution32);
        UTF32_Deinit(Replaced32);
        return Replaced8;
    }
    
    UTF16 *UTF16_SubstituteSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substitution, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        AssertIO(Substitution != NULL);
        UTF16 *Replaced16         = NULL;
        UTF32 *String32       = UTF16_Decode(String);
        UTF32 *Substitution32 = UTF16_Decode(Substitution);
        UTF32 *Replaced32     = UTF32_SubstituteSubString(String32, Substitution32, Offset, Length);
        Replaced16            = UTF16_Encode(Replaced32);
        UTF32_Deinit(String32);
        UTF32_Deinit(Substitution32);
        UTF32_Deinit(Replaced32);
        return Replaced16;
    }
    
    UTF32 *UTF32_SubstituteSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substitution, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        AssertIO(Substitution != NULL);
        UTF32 *NewString                          = NULL;

        size_t StringSize                     = UTF32_GetStringSizeInCodePoints(String);
        size_t SubstitutionSize               = UTF32_GetStringSizeInCodePoints(Substitution);
        size_t NewStringSize                  = (Maximum(StringSize, Length) - Minimum(StringSize, Length)) + SubstitutionSize;

        NewString                             = UTF32_Init(NewStringSize);
        AssertIO(NewString != NULL);
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
        AssertIO(String != NULL);
        UTF8 *Stitched = NULL;

        UTF32 *Decoded    = UTF8_Decode(String);
        UTF32 *Stitched32 = UTF32_StitchSubString(Decoded, Offset, Length);
        UTF32_Deinit(Decoded);
        Stitched          = UTF8_Encode(Stitched32);
        return Stitched;
    }
    
    UTF16 *UTF16_StitchSubString(ImmutableString_UTF16 String, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        UTF16 *Stitched       = NULL;
        UTF32 *Decoded    = UTF16_Decode(String);
        UTF32 *Stitched32 = UTF32_StitchSubString(Decoded, Offset, Length);
        UTF32_Deinit(Decoded);
        Stitched          = UTF16_Encode(Stitched32);
        UTF32_Deinit(Stitched32);
        return Stitched;
    }
    
    UTF32 *UTF32_StitchSubString(ImmutableString_UTF32 String, size_t Offset, size_t Length) {
        AssertIO(String != NULL);
        UTF32 *Stitched               = NULL;
        size_t StringSize         = UTF32_GetStringSizeInCodePoints(String);
        AssertIO(Offset < StringSize);
        AssertIO(Length < StringSize);
        AssertIO(Offset + Length < StringSize);
        size_t StitchedSize = StringSize - Length;
        Stitched              = UTF32_Init(StitchedSize);
        AssertIO(Stitched != NULL);

        size_t CodePoint = 0ULL;
        while (CodePoint < StitchedSize && CodePoint + Length < StringSize) {
            if (CodePoint < Offset) {
                Stitched[CodePoint] = String[CodePoint];
            } else if (CodePoint > Offset + Length) {
                Stitched[CodePoint] = String[CodePoint + Length];
            }
            CodePoint += 1;
        }
        return NULL;
    }
    
    UTF8 *UTF8_RemoveSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString2Remove, size_t Instance2Remove) {
        AssertIO(String != NULL);
        AssertIO(SubString2Remove != NULL);
        UTF8 *TrimmedString         = NULL;
        UTF32 *DecodedString    = UTF8_Decode(String);
        UTF32 *DecodedSubString = UTF8_Decode(SubString2Remove);
        UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
        TrimmedString           = UTF8_Encode(Trimmed32);
        UTF32_Deinit(DecodedString);
        UTF32_Deinit(DecodedSubString);
        UTF32_Deinit(Trimmed32);
        return TrimmedString;
    }
    
    UTF16 *UTF16_RemoveSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString2Remove, size_t Instance2Remove) {
        AssertIO(String != NULL);
        AssertIO(SubString2Remove != NULL);
        UTF16 *TrimmedString        = NULL;
        UTF32 *DecodedString    = UTF16_Decode(String);
        UTF32 *DecodedSubString = UTF16_Decode(SubString2Remove);
        UTF32 *Trimmed32        = UTF32_RemoveSubString(DecodedString, DecodedSubString, Instance2Remove);
        TrimmedString           = UTF16_Encode(Trimmed32);
        UTF32_Deinit(DecodedString);
        UTF32_Deinit(DecodedSubString);
        UTF32_Deinit(Trimmed32);
        return TrimmedString;
    }
    
    UTF32 *UTF32_RemoveSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString2Remove, size_t Instance2Remove) {
        AssertIO(String != NULL);
        AssertIO(SubString2Remove != NULL);
        UTF32 *EditedString = NULL;
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
        AssertIO(Instances != NULL);
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
        AssertIO(EditedString != NULL);

        for (size_t EditedCodePoint = 0ULL; EditedCodePoint < EditedStringSize; EditedCodePoint++) {
            for (size_t Instance = 0ULL; Instance < NumInstances; Instance++) {
                if (EditedCodePoint < Instances[Instance] || EditedCodePoint > Instances[Instance] + SubStringSize) {
                    EditedString[EditedCodePoint] = String[EditedCodePoint - (Instances[Instance] + SubStringSize)];
                }
            }
        }
        return EditedString;
    }
    
    UTF8 *UTF8_Insert(ImmutableString_UTF8 String, ImmutableString_UTF8 String2Insert, size_t Offset) {
        AssertIO(String != NULL);
        AssertIO(String2Insert != NULL);
        UTF8 *Inserted = NULL;

        UTF32 *String32   = UTF8_Decode(String);
        UTF32 *Insert32   = UTF8_Decode(String2Insert);
        UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
        UTF32_Deinit(String32);
        UTF32_Deinit(Insert32);
        Inserted          = UTF8_Encode(Inserted32);
        return Inserted;
    }
    
    UTF16 *UTF16_Insert(ImmutableString_UTF16 String, ImmutableString_UTF16 String2Insert, size_t Offset) {
        AssertIO(String != NULL);
        AssertIO(String2Insert != NULL);
        UTF16 *Inserted = NULL;
        UTF32 *String32   = UTF16_Decode(String);
        UTF32 *Insert32   = UTF16_Decode(String2Insert);
        UTF32 *Inserted32 = UTF32_Insert(String32, Insert32, Offset);
        UTF32_Deinit(String32);
        UTF32_Deinit(Insert32);
        Inserted          = UTF16_Encode(Inserted32);
        return Inserted;
    }
    
    UTF32 *UTF32_Insert(ImmutableString_UTF32 String, ImmutableString_UTF32 String2Insert, size_t Offset) {
        AssertIO(String != NULL);
        AssertIO(String2Insert != NULL);
        UTF32 *Inserted = NULL;
        size_t   StringSize = UTF32_GetStringSizeInCodePoints(String);
        size_t   InsertSize = UTF32_GetStringSizeInCodePoints(String2Insert);
        AssertIO(Offset < StringSize);
        if (Offset == 0xFFFFFFFFFFFFFFFF) {
            Offset          = StringSize;
        }
        Inserted        = UTF32_Init(StringSize + InsertSize);
        AssertIO(Inserted != NULL);

        for (size_t CodePoint = 0ULL; CodePoint < StringSize + InsertSize; CodePoint++) {
            if (CodePoint < Offset || CodePoint > Offset + InsertSize) {
                Inserted[CodePoint] = String[CodePoint];
            } else {
                Inserted[CodePoint] = String2Insert[CodePoint - StringSize];
            }
        }
        return Inserted;
    }

    /* TextIOTables Operations */
    size_t UTF32_ShouldCodePointBeNormalized(StringIO_NormalizationForms NormalizationType, UTF32 CodePoint) {
        bool   NeedsNormalizaion = No;
        size_t Index             = 0;
        if (NormalizationType == NormalizationForm_KompatibleCompose) {
            while (NeedsNormalizaion == No && Index < KompatibleNormalizationTableSize) {
                if (KompatibleNormalizationTable[0][Index][0] == CodePoint) {
                    NeedsNormalizaion = Yes;
                }
            }
        } else if (NormalizationType == NormalizationForm_CanonicalCompose) {
            while (NeedsNormalizaion == No && Index < CanonicalNormalizationTableSize) {
                if (CanonicalNormalizationTable[0][Index][0] == CodePoint) {
                    NeedsNormalizaion = Yes;
                }
            }
        }
        return NeedsNormalizaion ? Index : -1;
    }

    bool UTF32_IsCodePointInTable(const UTF32 *const Table, size_t TableSize, UTF32 CodePoint) {
        AssertIO(Table != NULL);
        AssertIO(TableSize > 0);
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
        AssertIO(Table != NULL);
        AssertIO(String2Replace != NULL);
        AssertIO(TableSize > 0);
        AssertIO(String2ReplaceSize > 0);
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
        AssertIO(String != NULL);
        UTF8 *CaseFolded      = NULL;
        UTF32 *String32   = UTF8_Decode(String);
        UTF32 *CaseFold32 = UTF32_CaseFold(String32);
        UTF32_Deinit(String32);
        CaseFolded        = UTF8_Encode(CaseFold32);
        UTF32_Deinit(CaseFold32);
        return CaseFolded;
    }
    
    UTF16 *UTF16_CaseFold(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        UTF16 *CaseFolded     = NULL;
        UTF32 *String32   = UTF16_Decode(String);
        UTF32 *CaseFold32 = UTF32_CaseFold(String32);
        UTF32_Deinit(String32);
        CaseFolded        = UTF16_Encode(CaseFold32);
        UTF32_Deinit(CaseFold32);
        return CaseFolded;
    }
    
    static size_t UTF32_GetCaseFoldedSize(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
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
        AssertIO(String2Edit != NULL);
        AssertIO(Replacement != NULL);
        size_t CodePoint = 0ULL;
        while (String2Edit[CodePoint] != TextIO_NULLTerminator && Replacement[CodePoint] != TextIO_NULLTerminator) {
            String2Edit[CodePoint] = Replacement[CodePoint];
            CodePoint += 1;
        }
    }
    
    UTF32 *UTF32_CaseFold(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        UTF32   *CaseFoldedString                = NULL;

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
        return CaseFoldedString;
    }
    
    static UTF32 *UTF32_Reorder(UTF32 *String) { // Stable sort
        AssertIO(String != NULL);
        size_t   CodePoint  = 1ULL;
        uint32_t CodePointA = 0UL;
        uint32_t CodePointB = 0UL;

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
        return NULL;
    }

    static size_t UTF32_NeedsNormalization(UTF32 CodePoint, StringIO_NormalizationForms NormalizationForm) {
        AssertIO(NormalizationForm != NormalizationForm_Unspecified);
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
        AssertIO(String != NULL);
        AssertIO(NormalizationForm != NormalizationForm_Unspecified);
        size_t CumulativeReplacementSize       = 0;
        size_t CodePoint = 0;
        if (NormalizationForm == NormalizationForm_CanonicalCompose) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
                size_t TableIndex              = UTF32_NeedsNormalization(String[CodePoint], NormalizationForm_CanonicalCompose);
                if (TableIndex != -1) {
                    CumulativeReplacementSize += UTF32_GetStringSizeInCodePoints((const UTF32 *) &CanonicalNormalizationTable[TableIndex][1]);
                    CodePoint                 += 1;
                }
            }
        } else if (NormalizationForm == NormalizationForm_KompatibleCompose) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
                size_t TableIndex              = UTF32_NeedsNormalization(String[CodePoint], NormalizationForm_KompatibleCompose);
                if (TableIndex != -1) {
                    CumulativeReplacementSize += UTF32_GetStringSizeInCodePoints((const UTF32 *) &KompatibleNormalizationTable[TableIndex][1]);
                    CodePoint                 += 1;
                }
            }
        }
        return CumulativeReplacementSize;
    }
    
    static UTF32 *UTF32_Compose(UTF32 *String, StringIO_NormalizationForms CompositionType) { // TODO: Must use a stable sorting algorithm
        AssertIO(String != NULL);
        AssertIO(CompositionType == NormalizationForm_CanonicalCompose || CompositionType == NormalizationForm_KompatibleCompose);
        size_t   CodePoint            = 0ULL;
        UTF32   *ComposedString       = NULL;
        size_t ComposedStringSize = UTF32_GetComulativeReplacementSizeForNormalization(String, CompositionType);
        ComposedString            = UTF32_Init(ComposedStringSize);
        if (ComposedString != NULL) {
            while (String[CodePoint] != TextIO_NULLTerminator) {
                size_t TableID = UTF32_NeedsNormalization(String[CodePoint], CompositionType);
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
        return ComposedString;
    }
    
    static UTF32 *UTF32_Decompose(ImmutableString_UTF32 String, StringIO_NormalizationForms DecompositionType) { // TODO: Must use a stable sorting algorithm
        AssertIO(String != NULL);
        AssertIO(DecompositionType == NormalizationForm_CanonicalDecompose || DecompositionType == NormalizationForm_KompatibleDecompose);
        size_t   CodePoint      = 0ULL;
        UTF32   *DecomposedString = NULL;
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
        return DecomposedString;
    }
    
    UTF8 *UTF8_Normalize(ImmutableString_UTF8 String, StringIO_NormalizationForms NormalizedForm) {
        AssertIO(String != NULL);
        AssertIO(NormalizedForm != NormalizationForm_Unspecified);
        UTF8 *NormalizedString8       = NULL;
        UTF32 *String32           = UTF8_Decode(String);
        UTF32 *NormalizedString32 = UTF32_Normalize(String32, NormalizedForm);
        NormalizedString8         = UTF8_Encode(NormalizedString32);
        UTF32_Deinit(String32);
        UTF32_Deinit(NormalizedString32);
        return NormalizedString8;
    }
    
    UTF16 *UTF16_Normalize(ImmutableString_UTF16 String, StringIO_NormalizationForms NormalizedForm) {
        AssertIO(String != NULL);
        AssertIO(NormalizedForm != NormalizationForm_Unspecified);
        UTF16 *NormalizedString16     = NULL;
        UTF32 *String32           = UTF16_Decode(String);
        UTF32 *NormalizedString32 = UTF32_Normalize(String32, NormalizedForm);
        NormalizedString16        = UTF16_Encode(NormalizedString32);
        UTF32_Deinit(String32);
        UTF32_Deinit(NormalizedString32);
        return NormalizedString16;
    }
    
    UTF32 *UTF32_Normalize(ImmutableString_UTF32 String, StringIO_NormalizationForms NormalizedForm) {
        AssertIO(String != NULL);
        AssertIO(NormalizedForm != NormalizationForm_Unspecified);
        UTF32 *NormalizedString = NULL;
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
        return NormalizedString;
    }
    /* TextIOTables Operations */
    
    int64_t UTF8_String2Integer(TextIO_Bases Base, ImmutableString_UTF8 String) { // Replaces atoi, atol, strtol, strtoul,
        AssertIO(PlatformIO_Is(Base, Base_Integer));
        AssertIO(String != NULL);
        int64_t Value = 0LL;
        UTF32 *String32 = UTF8_Decode(String);
        Value           = UTF32_String2Integer(Base, String32);
        UTF32_Deinit(String32);
        return Value;
    }
    
    int64_t UTF16_String2Integer(TextIO_Bases Base, ImmutableString_UTF16 String) {
        AssertIO(PlatformIO_Is(Base, Base_Integer));
        AssertIO(String != NULL);
        int64_t Value = 0LL;
        UTF32 *String32 = UTF16_Decode(String);
        Value           = UTF32_String2Integer(Base, String32);
        UTF32_Deinit(String32);
        return Value;
    }
    
    // Integer2String should accept any integer base from the lookup table and shift the value until it can't anymore
    int64_t UTF32_String2Integer(TextIO_Bases Base, ImmutableString_UTF32 String) {
        AssertIO(PlatformIO_Is(Base, Base_Integer));
        AssertIO(String != NULL);
        size_t   CodePoint = 0;
        int8_t   Sign      = 1;
        int64_t  Value     = 0LL;
        if PlatformIO_Is(Base, Base_Radix2) {
            while (String[CodePoint] != TextIO_NULLTerminator && (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('1'))) {
                Value    <<= 1;
                Value     += String[CodePoint] - 0x30;
                CodePoint += 1;
            }
        } else if PlatformIO_Is(Base, Base_Radix8) {
            while (String[CodePoint] != TextIO_NULLTerminator && (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('7'))) {
                Value     *= 8;
                Value     += String[CodePoint] - 0x30;
                CodePoint += 1;
            }
        } else if PlatformIO_Is(Base, Base_Radix10) {
            while (String[CodePoint] != TextIO_NULLTerminator && ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (CodePoint == 0 && String[CodePoint] == UTF32Character('-')))) {
                if (CodePoint == 0 && String[CodePoint] == UTF32Character('-')) { // FIXME: Sign discovery needs work
                    Sign        = -1;
                } else if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) {
                    Value      *= 10;
                    Value      += String[CodePoint] - 0x30;
                }
                CodePoint          += 1;
            }
        } else if PlatformIO_Is(Base, Base_Radix16) {
            if PlatformIO_Is(Base, Base_Uppercase) {
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
            } else if PlatformIO_Is(Base, Base_Lowercase) {
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
        Value                      *= Sign;
        return Value;
    }
    
    UTF8 *UTF8_Integer2String(TextIO_Bases Base, int64_t Integer2Convert) {
        AssertIO(PlatformIO_Is(Base, Base_Integer));
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF8  *IntegerString8  = UTF8_Encode((ImmutableString_UTF32) IntegerString32);
        UTF32_Deinit(IntegerString32);
        return IntegerString8;
    }
    
    UTF16 *UTF16_Integer2String(TextIO_Bases Base, int64_t Integer2Convert) {
        AssertIO(PlatformIO_Is(Base, Base_Integer));
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, Integer2Convert);
        UTF16 *IntegerString16 = UTF16_Encode((ImmutableString_UTF32) IntegerString32);
        UTF32_Deinit(IntegerString32);
        return IntegerString16;
    }
    
    UTF32 *UTF32_Integer2String(TextIO_Bases Base, int64_t Integer2Convert) {
        AssertIO(PlatformIO_Is(Base, Base_Integer));
        UTF32   *String               = NULL;
        int64_t  Sign                 = 0LL;
        uint64_t Num                  = AbsoluteI(Integer2Convert);
        uint8_t  Radix                = 1;
        uint8_t  NumDigits            = 0;
        
        if (Integer2Convert < 0) { // Signed
            Sign                      = -1;
            NumDigits                +=  1;
        }

        if PlatformIO_Is(Base, Base_Radix2) {
            Radix                 = 2;
        } else if PlatformIO_Is(Base, Base_Radix8) {
            Radix                 = 8;
        } else if PlatformIO_Is(Base, Base_Radix10) {
            Radix                 = 10;
        } else if PlatformIO_Is(Base, Base_Radix16) {
            Radix                 = 16;
        }

        NumDigits                    += NumDigitsInInteger(Radix, Integer2Convert);

        String                        = UTF32_Init(NumDigits);
        AssertIO(String != NULL);

        for (size_t CodePoint = NumDigits; CodePoint > 0; CodePoint--) {
            uint8_t Digit                 = Num % Radix;
            Num                          /= Radix;
            if PlatformIO_Is(Base, Base_Radix2) {
                String[CodePoint - 1]     = IntegerTableBase2[Digit];
            } else if PlatformIO_Is(Base, Base_Radix8) {
                String[CodePoint - 1]     = IntegerTableBase8[Digit];
            } else if PlatformIO_Is(Base, Base_Radix10) {
                if (Sign == -1 && CodePoint == 1) {
                    String[CodePoint - 1] = UTF32Character('-');
                } else {
                    String[CodePoint - 1] = IntegerTableBase10[Digit];
                }
            } else if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    String[CodePoint - 1] = IntegerTableBase16Uppercase[Digit];
                } else if PlatformIO_Is(Base, Base_Lowercase) {
                    String[CodePoint - 1] = IntegerTableBase16Lowercase[Digit];
                }
                Radix                     = 16;
            }
        }
        return String;
    }

    double UTF8_String2Decimal(TextIO_Bases Base, ImmutableString_UTF8 String) {
        AssertIO(PlatformIO_Is(Base, Base_Decimal));
        double Decimal = 0.0;
        UTF32 *String32 = UTF8_Decode(String);
        Decimal         = UTF32_String2Decimal(Base, String32);
        UTF32_Deinit(String32);
        return Decimal;
    }

    double UTF16_String2Decimal(TextIO_Bases Base, ImmutableString_UTF16 String) {
        AssertIO(PlatformIO_Is(Base, Base_Decimal));
        double Decimal = 0.0;
        UTF32 *String32 = UTF16_Decode(String);
        Decimal         = UTF32_String2Decimal(Base, String32);
        UTF32_Deinit(String32);
        return Decimal;
    }

    double UTF32_String2Decimal(TextIO_Bases Base, ImmutableString_UTF32 String) { // Replaces strtod, strtof, strold, atof, and atof_l
        AssertIO(PlatformIO_Is(Base, Base_Decimal));
        double   Value         = 0.0;
        bool     IsNegative    = No;
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
        return Value;
    }

    UTF8 *UTF8_Decimal2String(TextIO_Bases Base, double Decimal) {
        AssertIO(PlatformIO_Is(Base, Base_Decimal));
        UTF32 *String32 = UTF32_Decimal2String(Decimal, Base);
        UTF8  *String8  = UTF8_Encode(String32);
        UTF32_Deinit(String32);
        return String8;
    }

    UTF16 *UTF16_Decimal2String(TextIO_Bases Base, double Decimal) {
        AssertIO(PlatformIO_Is(Base, Base_Decimal));
        UTF32 *String32 = UTF32_Decimal2String(Decimal, Base);
        UTF16 *String16 = UTF16_Encode(String32);
        UTF32_Deinit(String32);
        return String16;
    }

    static UTF32 *Decimal2String_UTF32(TextIO_Bases Base, double Decimal, size_t MinimumWidth, size_t Precision) {
        AssertIO(PlatformIO_Is(Base, Base_Decimal));
        UTF32 *String = NULL;
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

        if PlatformIO_Is(Base, Base_Shortest) {
            if (MinimumWidth == 0 && Precision == 0) {
                // Ryu
            } else {
                // Do something stranger
            }
        } else if PlatformIO_Is(Base, Base_Scientific) {
            if (MinimumWidth == 0 && Precision == 0) {
                // Ryu
            } else {
                // Do something stranger
            }
        } else if PlatformIO_Is(Base, Base_Radix10) {
            if (MinimumWidth == 0 && Precision == 0) {
                // Ryu
            } else {
                // Do something stranger
            }
        } else if PlatformIO_Is(Base, Base_Radix16) {
            StringSize += 2;
            AssertIO(PlatformIO_Is(Base, Base_Uppercase) || PlatformIO_Is(Base, Base_Lowercase));
            if PlatformIO_Is(Base, Base_Uppercase) {
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
            } else if PlatformIO_Is(Base, Base_Lowercase) {
                if (MinimumWidth == 0 && Precision == 0) {
                    // Ryu
                } else {
                    // Do something stranger
                }
            }
        }
        return String;
    }

    UTF32 *UTF32_Decimal2String(TextIO_Bases Base, double Number) {
        AssertIO(Base != Base_Unspecified);
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
        if PlatformIO_Is(Base, Base_Radix2) {
            Radix                 = 2;
            StringSize           += 1;
        } else if PlatformIO_Is(Base, Base_Radix8) {
            Radix                 = 8;
        } else if PlatformIO_Is(Base, Base_Radix10) {
            Radix                 = 10;
            if (Sign == -1) {
                StringSize       += 1;
            }
        } else if PlatformIO_Is(Base, Base_Radix16) {
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
            if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = UTF32Character('A');
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('P');
                } else if PlatformIO_Is(Base, Base_Lowercase) {
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 1] = UTF32Character('a');
                    OutputString[StringSize + NumDigitsExponent + NumDigitsMantissa + 2] = UTF32Character('p');
                }
            } else if PlatformIO_Is(Base, Base_Scientific) {
                if PlatformIO_Is(Base, Base_Uppercase) {
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
                } else if PlatformIO_Is(Base, Base_Lowercase) {
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
        }
        return OutputString;
    }
    /* Number Conversions */

    bool UTF8_Compare(ImmutableString_UTF8 String1, ImmutableString_UTF8 String2) {
        AssertIO(String1 != NULL);
        AssertIO(String2 != NULL);
        bool StringsMatch         = No;
        if (String1 != String2) {
            UTF32 *String1_32 = UTF8_Decode(String1);
            UTF32 *String2_32 = UTF8_Decode(String2);
            StringsMatch      = UTF32_Compare(String1_32, String2_32);
            UTF32_Deinit(String1_32);
            UTF32_Deinit(String2_32);
        } else {
            StringsMatch      = Yes;
        }
        return StringsMatch;
    }

    bool UTF16_Compare(ImmutableString_UTF16 String1, ImmutableString_UTF16 String2) {
        AssertIO(String1 != NULL);
        AssertIO(String2 != NULL);
        bool StringsMatch         = No;
        if (String1 != String2) {
            UTF32 *String1_32 = UTF16_Decode(String1);
            UTF32 *String2_32 = UTF16_Decode(String2);
            StringsMatch      = UTF32_Compare(String1_32, String2_32);
            UTF32_Deinit(String1_32);
            UTF32_Deinit(String2_32);
        } else {
            StringsMatch      = Yes;
        }
        return StringsMatch;
    }

    bool UTF32_Compare(ImmutableString_UTF32 String1, ImmutableString_UTF32 String2) {
        AssertIO(String1 != NULL);
        AssertIO(String2 != NULL);
        bool StringsMatch                    = No;
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
        return StringsMatch;
    }

    bool UTF8_CompareSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substring, size_t StringOffset, size_t SubstringOffset) {
        AssertIO(String != NULL);
        AssertIO(Substring != NULL);
        bool SubstringMatchesAtOffset = No;
        UTF32 *String32           = UTF8_Decode(String);
        UTF32 *Sub32              = UTF8_Decode(Substring);
        SubstringMatchesAtOffset  = UTF32_CompareSubString(String32, Sub32, StringOffset, SubstringOffset);
        UTF32_Deinit(String32);
        UTF32_Deinit(Sub32);
        return SubstringMatchesAtOffset;
    }

    bool UTF16_CompareSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substring, size_t StringOffset, size_t SubstringOffset) {
        AssertIO(String != NULL);
        AssertIO(Substring != NULL);
        bool SubstringMatchesAtOffset = No;
        UTF32 *String32           = UTF16_Decode(String);
        UTF32 *Sub32              = UTF16_Decode(Substring);
        SubstringMatchesAtOffset  = UTF32_CompareSubString(String32, Sub32, StringOffset, SubstringOffset);
        UTF32_Deinit(String32);
        UTF32_Deinit(Sub32);
        return SubstringMatchesAtOffset;
    }

    bool UTF32_CompareSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substring, size_t StringOffset, size_t SubstringOffset) {
        AssertIO(String != NULL);
        AssertIO(Substring != NULL);
        bool SubstringMatchesAtOffset            = No;
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
        return SubstringMatchesAtOffset;
    }

    UTF8 *UTF8_Trim(ImmutableString_UTF8 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF8 Strings2Remove) {
        AssertIO(String != NULL);
        AssertIO(Type != TruncationType_Unspecified);
        AssertIO(Strings2Remove != NULL);
        UTF8 *Trimmed = NULL;
        UTF32    *String32                  = UTF8_Decode(String);
        UTF32   **Strings2Remove32          = UTF8_StringSet_Decode(Strings2Remove);
        UTF32    *Trimmed32                 = UTF32_Trim(String32, Type, (const UTF32**) Strings2Remove32);
        UTF32_StringSet_Deinit(Strings2Remove32);
        Trimmed                             = UTF8_Encode(Trimmed32);
        UTF32_Deinit(String32);
        return Trimmed;
    }

    UTF16 *UTF16_Trim(ImmutableString_UTF16 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF16 Strings2Remove) {
        AssertIO(String != NULL);
        AssertIO(Type != TruncationType_Unspecified);
        AssertIO(Strings2Remove != NULL);
        UTF16 *Trimmed = NULL;
        UTF32    *String32                  = UTF16_Decode(String);
        UTF32   **Strings2Remove32          = UTF16_StringSet_Decode((const UTF16**) Strings2Remove);
        UTF32    *Trimmed32                 = UTF32_Trim(String32, Type, (const UTF32**) Strings2Remove32);
        UTF32_StringSet_Deinit(Strings2Remove32);
        Trimmed                             = UTF16_Encode(Trimmed32);
        UTF32_Deinit(String32);
        return Trimmed;
    }

    UTF32 *UTF32_Trim(ImmutableString_UTF32 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF32 Strings2Remove) {
        AssertIO(String != NULL);
        AssertIO(Type != TruncationType_Unspecified);
        AssertIO(Strings2Remove != NULL);
        UTF32 *Trimmed = NULL;
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
        AssertIO(Trimmed != NULL);
        for (size_t RemovalPoint = 0ULL; RemovalPoint < NumRemovalPoints; RemovalPoint++) {
            for (size_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                for (size_t TrimmedCodePoint = 0ULL; TrimmedCodePoint < TrimmedStringSize; TrimmedCodePoint++) {
                    if (StringCodePoint < RemovalPointsStart[RemovalPoint] || StringCodePoint > RemovalPointsEnd[RemovalPoint]) {
                        Trimmed[TrimmedCodePoint] = String[StringCodePoint];
                    }
                }
            }
        }
        return Trimmed;
    }

    UTF8 **UTF8_Split(ImmutableString_UTF8 String, ImmutableStringSet_UTF8 Delimiters) {
        AssertIO(String != NULL);
        AssertIO(Delimiters != NULL);
        UTF8 **SplitString                                 = NULL;
        ImmutableString_UTF32  String32        = UTF8_Decode(String);
        UTF32 **Delimiters32  = UTF8_StringSet_Decode(Delimiters);
        UTF32 **SplitString32 = UTF32_Split(String32, (const UTF32 *const *) Delimiters32);
        UTF32_Deinit((UTF32*) String32);
        UTF32_StringSet_Deinit((UTF32**) Delimiters32);
        SplitString           = UTF8_StringSet_Encode((const UTF32**) SplitString32);
        return SplitString;
    }

    UTF16 **UTF16_Split(ImmutableString_UTF16 String, ImmutableStringSet_UTF16 Delimiters) {
        AssertIO(String != NULL);
        AssertIO(Delimiters != NULL);
        UTF16 **SplitString       = NULL;
        UTF32  *String32      = UTF16_Decode(String);
        UTF32 **Delimiters32  = UTF16_StringSet_Decode((const UTF16**) Delimiters);
        UTF32 **SplitString32 = UTF32_Split(String32, (const UTF32 *const *) Delimiters32);
        UTF32_Deinit(String32);
        UTF32_StringSet_Deinit((UTF32**) Delimiters32);
        SplitString           = UTF16_StringSet_Encode((const UTF32**) SplitString32);
        return SplitString;
    }

    UTF32 **UTF32_Split(ImmutableString_UTF32 String, ImmutableStringSet_UTF32 Delimiters) {
        AssertIO(String != NULL);
        AssertIO(Delimiters != NULL);
        UTF32    **SplitStrings    = NULL; // What we return, it's a 0 indexed array of strings
        size_t     StringSize      = 0ULL; // The size of the first parameter
        size_t     NumDelimiters   = 0ULL; // The number of delimiters in the second parameter
        size_t     NumSplitStrings = 0ULL; // The number of strings to return
        size_t    *SplitSizes      = NULL; // The size of each split string
        size_t    *SplitOffsets    = NULL; // The starting position of each split
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
        free(SplitSizes);
        free(SplitOffsets);
        return SplitStrings;
    }

    size_t UTF8_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF8 String) {
        AssertIO(Base != Base_Unspecified);
        AssertIO(String != NULL);
        size_t NumDigits      = 0ULL;
        size_t CodePoint  = 0;

        if PlatformIO_Is(Base, Base_Integer) {
            if PlatformIO_Is(Base, Base_Radix2) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= '0' && String[CodePoint] <= '1') {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix8) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= '0' && String[CodePoint] <= '7') {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix10) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= '0' && String[CodePoint] <= '9') {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'A' && String[CodePoint] <= 'F')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if PlatformIO_Is(Base, Base_Lowercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'a' && String[CodePoint] <= 'f')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            }
        } else if PlatformIO_Is(Base, Base_Decimal) {
            if PlatformIO_Is(Base, Base_Radix10) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.')) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'A' && String[CodePoint] <= 'F') || String[CodePoint] <= 'P' || String[CodePoint] <= 'X') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if PlatformIO_Is(Base, Base_Lowercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] >= 'a' && String[CodePoint] <= 'f') || String[CodePoint] <= 'p' || String[CodePoint] <= 'x') {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            } else { // Scientific/Shortest
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.' || String[CodePoint] == 'E')) { // Todo: Make sure the Exponent is preceded by a digit
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if PlatformIO_Is(Base, Base_Lowercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= '0' && String[CodePoint] <= '9') || (String[CodePoint] == '.' || String[CodePoint] == 'e')) { // Todo: Make sure the Exponent is preceded by a digit
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            }
        }
        return NumDigits;
    }

    size_t UTF16_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF16 String) {
        AssertIO(Base != Base_Unspecified);
        AssertIO(String != NULL);
        size_t NumDigits      = 0ULL;
        size_t CodePoint  = 0;

        if PlatformIO_Is(Base, Base_Integer) {
            if PlatformIO_Is(Base, Base_Radix2) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('1')) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix8) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('7')) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix10) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('A') && String[CodePoint] <= UTF16Character('F'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if (PlatformIO_Is(Base, Base_Lowercase)) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('a') && String[CodePoint] <= UTF16Character('f'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            }
        } else if PlatformIO_Is(Base, Base_Decimal) {
            if PlatformIO_Is(Base, Base_Radix10) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.'))) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('A') && String[CodePoint] <= UTF16Character('F')) || String[CodePoint] <= UTF16Character('P') || String[CodePoint] <= UTF16Character('X')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if (PlatformIO_Is(Base, Base_Lowercase)) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] >= UTF16Character('a') && String[CodePoint] <= UTF16Character('f')) || String[CodePoint] <= UTF16Character('p') || String[CodePoint] <= UTF16Character('x')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            } else { // Scientific/Shortest
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.') || String[CodePoint] == UTF16Character('E'))) { // Todo: Make sure the Exponent is preceded by a digit
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if (PlatformIO_Is(Base, Base_Lowercase)) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF16Character('0') && String[CodePoint] <= UTF16Character('9')) || (String[CodePoint] == UTF16Character('.') || String[CodePoint] == UTF16Character('e'))) { // Todo: Make sure the Exponent is preceded by a digit
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            }
        }
        return NumDigits;
    }

    size_t UTF32_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF32 String) {
        AssertIO(Base != Base_Unspecified);
        AssertIO(String != NULL);
        size_t NumDigits      = 0ULL;
        size_t CodePoint  = 0;

        if PlatformIO_Is(Base, Base_Integer) {
            if PlatformIO_Is(Base, Base_Radix2) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('1')) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix8) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('7')) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix10) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if (String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if (PlatformIO_Is(Base, Base_Lowercase)) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f'))) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            }
        } else if PlatformIO_Is(Base, Base_Decimal) {
            if PlatformIO_Is(Base, Base_Radix10) {
                while (String[CodePoint] != TextIO_NULLTerminator) {
                    if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.'))) {
                        NumDigits     += 1;
                    }
                    CodePoint         += 1;
                }
            } else if PlatformIO_Is(Base, Base_Radix16) {
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('A') && String[CodePoint] <= UTF32Character('F')) || String[CodePoint] <= UTF32Character('P') || String[CodePoint] <= UTF32Character('X')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if (PlatformIO_Is(Base, Base_Lowercase)) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] >= UTF32Character('a') && String[CodePoint] <= UTF32Character('f')) || String[CodePoint] <= UTF32Character('p') || String[CodePoint] <= UTF32Character('x')) {
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            } else { // Scientific/Shortest
                if PlatformIO_Is(Base, Base_Uppercase) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.') || String[CodePoint] == UTF32Character('E'))) { // Todo: Make sure the Exponent is preceded by a digit
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                } else if (PlatformIO_Is(Base, Base_Lowercase)) {
                    while (String[CodePoint] != TextIO_NULLTerminator) {
                        if ((String[CodePoint] >= UTF32Character('0') && String[CodePoint] <= UTF32Character('9')) || (String[CodePoint] == UTF32Character('.') || String[CodePoint] == UTF32Character('e'))) { // Todo: Make sure the Exponent is preceded by a digit
                            NumDigits     += 1;
                        }
                        CodePoint         += 1;
                    }
                }
            }
        }
        return NumDigits;
    }

    size_t UTF32_GetSubStringLength(ImmutableString_UTF32 Format, ImmutableString_UTF32 Formatted, size_t Offset) {
        AssertIO(Format != NULL);
        AssertIO(Formatted != NULL);
        size_t Length        = 0ULL;
        size_t CodePoint = Offset;

        while ((Format[CodePoint] != Formatted[CodePoint]) && (Format[CodePoint] != TextIO_NULLTerminator && Formatted[CodePoint] != TextIO_NULLTerminator)) {
            CodePoint += 1;
            Length    += 1;
        }
        return Length;
    }

    /* Unicode Conversion */
    CharSet8 *UTF8_ConvertUnicode2CharSet(ImmutableString_UTF8 String, StringIO_CodePages CodePage) {
        AssertIO(String != NULL);
        AssertIO(CodePage != CodePage_Unspecified);
        CharSet8 *Encoded = NULL;

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
        return Encoded;
    }

    CharSet16 *UTF16_ConvertUnicode2CharSet(ImmutableString_UTF16 String, StringIO_CodePages CodePage) {
        AssertIO(String != NULL);
        AssertIO(CodePage != CodePage_Unspecified);
        CharSet16 *Encoded = NULL;

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

        return Encoded;
    }

    CharSet32 *UTF32_ConvertUnicode2CharSet(ImmutableString_UTF32 String, StringIO_CodePages CodePage) {
        AssertIO(String != NULL);
        AssertIO(CodePage != CodePage_Unspecified);
        CharSet32 *Encoded = NULL;
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
        return Encoded;
    }

    UTF8 *UTF8_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet8 *) String, StringIO_CodePages CodePage) {
        AssertIO(String != NULL);
        AssertIO(CodePage != CodePage_Unspecified);
        UTF8 *Unicode = NULL;

        UTF32   *Decoded          = UTF8_Decode(String);
        size_t   Character        = 0ULL;
        size_t   CodePoint        = 0ULL;
        size_t   NumCodeUnits     = 0ULL;
        if (CodePage == CodePage_ISO_8859_1) {
            while (Decoded[Character] != TextIO_NULLTerminator) {
                size_t NormOffset = UTF32_ShouldCodePointBeNormalized(NormalizationForm_KompatibleCompose, Decoded[Character]);
                if (NormOffset > -1) {
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
        return Unicode;
    }

    UTF16 *UTF16_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet16 *) String, StringIO_CodePages CodePage) {
        AssertIO(String != NULL);
        AssertIO(CodePage != CodePage_Unspecified);
        UTF16 *Unicode = NULL;

        UTF32   *Decoded          = UTF16_Decode(String);
        size_t   Character        = 0ULL;
        size_t   CodePoint        = 0ULL;
        size_t   NumCodeUnits     = 0ULL;
        if (CodePage == CodePage_ISO_8859_1) {
            while (Decoded[Character] != TextIO_NULLTerminator) {
                size_t NormOffset = UTF32_ShouldCodePointBeNormalized(NormalizationForm_KompatibleCompose, Decoded[Character]);
                if (NormOffset > -1) {
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

        return Unicode;
    }

    UTF32 *UTF32_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet32 *) String, StringIO_CodePages CodePage) {
        UTF32 *Unicode = NULL;
        AssertIO(String != NULL);
        AssertIO(CodePage != CodePage_Unspecified);

        size_t   Character        = 0ULL;
        size_t   CodePoint        = 0ULL;
        size_t   NumCodeUnits     = 0ULL;
        if (CodePage == CodePage_ISO_8859_1) {
            while (String[Character] != TextIO_NULLTerminator) {
                size_t NormOffset = UTF32_ShouldCodePointBeNormalized(NormalizationForm_KompatibleCompose, String[Character]);
                if (NormOffset > -1) {
                    NumCodeUnits += UTF32_GetCodePointSizeInUTF8CodeUnits(String[Character]);
                }
                Character        += 1;
            }
            Unicode               = UTF32_Init(NumCodeUnits);
            while (String[Character] != TextIO_NULLTerminator && Unicode[CodePoint] != TextIO_NULLTerminator) {
                Unicode[CodePoint] = String[Character];
            }
        }
        return Unicode;
    }
    /* Unicode Conversion */

    void UTF8_Deinit(UTF8 *String) {
        AssertIO(String != NULL);
        free(String[-sizeof(size_t)]);
    }

    void UTF16_Deinit(UTF16 *String) {
        AssertIO(String != NULL);
        free(String[-(sizeof(size_t) / sizeof(UTF16))]);
    }

    void UTF32_Deinit(UTF32 *String) {
        AssertIO(String != NULL);
        free(String[-(sizeof(size_t) / sizeof(UTF32))]);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
