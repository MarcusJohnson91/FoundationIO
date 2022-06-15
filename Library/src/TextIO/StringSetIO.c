#include "../../include/TextIO/StringSetIO.h"          /* Included for our declarations */

#include "../../include/AssertIO.h"                    /* Included for Assertions */
#include "../../include/BufferIO.h"                    /* Included for BufferIO_MemorySet */
#include "../../include/FileIO.h"                      /* Included for File operations */
#include "../../include/MathIO.h"                      /* Included for endian swapping */
#include "../../include/TextIO/StringIO.h"             /* Included for Encoding/Decoding functions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum StringSetConstants {
        UTF8_Debug_Text_8       = 0x38,
        UTF16_Debug_Text_16BE   = 0x3136,
        UTF16_Debug_Text_16LE   = 0x3631,
        UTF32_Debug_Text_32BE   = 0x3332,
        UTF32_Debug_Text_32LE   = 0x3233,
    } StringSetConstants;

    UTF8 **UTF8_StringSet_Init(size_t NumStrings) {
        AssertIO(NumStrings > 0);

        UTF8 **StringSet = NULL;
        StringSet        = calloc(sizeof(size_t) + NumStrings + TextIO_NULLTerminatorSize, sizeof(UTF8*));
        AssertIO(StringSet != NULL);

        static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8, "size_t is not 4 or 8 bytes!");
        if (sizeof(size_t) == 4) {
            uint8_t Unpacked[4];
            UnpackInteger32To8(NumStrings, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            StringSet[0][0] = Unpacked[3];
            StringSet[0][1] = Unpacked[2];
            StringSet[0][2] = Unpacked[1];
            StringSet[0][3] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            StringSet[0][0] = Unpacked[0];
            StringSet[0][1] = Unpacked[1];
            StringSet[0][2] = Unpacked[2];
            StringSet[0][3] = Unpacked[3];
#endif
        } else if (sizeof(size_t) == 8) {
            uint8_t Unpacked[8];
            UnpackInteger64To8(NumStrings, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            StringSet[0][0] = Unpacked[7];
            StringSet[0][1] = Unpacked[6];
            StringSet[0][2] = Unpacked[5];
            StringSet[0][3] = Unpacked[4];
            StringSet[0][4] = Unpacked[3];
            StringSet[0][5] = Unpacked[2];
            StringSet[0][6] = Unpacked[1];
            StringSet[0][7] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            StringSet[0][0] = Unpacked[0];
            StringSet[0][1] = Unpacked[1];
            StringSet[0][2] = Unpacked[2];
            StringSet[0][3] = Unpacked[3];
            StringSet[0][4] = Unpacked[4];
            StringSet[0][5] = Unpacked[5];
            StringSet[0][6] = Unpacked[6];
            StringSet[0][7] = Unpacked[7];
#endif
        }

#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
        BufferIO_MemorySet8(*StringSet, UTF8_Debug_Text_8, NumStrings + TextIO_NULLTerminatorSize);
#endif
        return &StringSet[sizeof(size_t)];
    }

    UTF16 **UTF16_StringSet_Init(size_t NumStrings) {
        AssertIO(NumStrings > 0);
        UTF16 **StringSet = NULL;
        StringSet         = (UTF16**) calloc((sizeof(size_t) / sizeof(UTF16)) + NumStrings + TextIO_NULLTerminatorSize, sizeof(UTF16*));
        AssertIO(StringSet != NULL);

        static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8, "size_t is not 4 or 8 bytes!");
        if (sizeof(size_t) == 4) {
            uint16_t Unpacked[2];
            UnpackInteger32To16(NumStrings, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            StringSet[0][0] = Unpacked[1];
            StringSet[0][1] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            StringSet[0][0] = Unpacked[0];
            StringSet[0][1] = Unpacked[1];
#endif
        } else if (sizeof(size_t) == 8) {
            uint16_t Unpacked[4];
            UnpackInteger64To16(NumStrings, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            StringSet[0][0] = Unpacked[3];
            StringSet[0][1] = Unpacked[2];
            StringSet[0][2] = Unpacked[1];
            StringSet[0][3] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            StringSet[0][0] = Unpacked[0];
            StringSet[0][1] = Unpacked[1];
            StringSet[0][2] = Unpacked[2];
            StringSet[0][3] = Unpacked[3];
#endif
        }

#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
        BufferIO_MemorySet16(*StringSet, UTF16_Debug_Text_16BE, NumStrings + TextIO_NULLTerminatorSize);
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
        BufferIO_MemorySet16(*StringSet, UTF16_Debug_Text_16LE, NumStrings + TextIO_NULLTerminatorSize);
#endif /* ByteOrder */
#endif /* BuildType */
        return &StringSet[(sizeof(size_t) / sizeof(UTF16))];
    }

    UTF32 **UTF32_StringSet_Init(size_t NumStrings) {
        AssertIO(NumStrings > 0);
        UTF32 **StringSet = NULL;
        StringSet     = (UTF32**) calloc((sizeof(size_t) / sizeof(UTF32)) + NumStrings + TextIO_NULLTerminatorSize, sizeof(UTF32*));
        AssertIO(StringSet != NULL);

        static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8, "size_t is not 4 or 8 bytes!");
        if (sizeof(size_t) == 4) {
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            StringSet[0][0] = NumStrings;
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            StringSet[0][0] = NumStrings;
#endif
        } else if (sizeof(size_t) == 8) {
            uint32_t Unpacked[2];
            UnpackInteger64To32(NumStrings, Unpacked);
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
            StringSet[0][0] = Unpacked[1];
            StringSet[0][1] = Unpacked[0];
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
            StringSet[0][0] = Unpacked[0];
            StringSet[0][1] = Unpacked[1];
#endif
        }

#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsBE)
        BufferIO_MemorySet32(*StringSet, UTF32_Debug_Text_32BE, NumStrings + TextIO_NULLTerminatorSize);
#elif (PlatformIO_ByteOrder == PlatformIO_ByteOrderIsLE)
        BufferIO_MemorySet32(*StringSet, UTF32_Debug_Text_32LE, NumStrings + TextIO_NULLTerminatorSize);
#endif /* ByteOrder */
#endif /* BuildType */
        return &StringSet[(sizeof(size_t) / sizeof(UTF32))];
    }

    bool UTF8_StringSet_Attach(UTF8 **StringSet, UTF8 *String2Attach, size_t Index) {
        AssertIO(StringSet != NULL);
        AssertIO(String2Attach != NULL);
        AssertIO(Index < UTF8_StringSet_GetNumStrings(StringSet));

        bool AttachedSucessfully = No;
        size_t NumStrings  = 0ULL;
        while (StringSet[NumStrings] != TextIO_NULLTerminator) {
            NumStrings      += 1;
        }
        if (NumStrings >= Index) {
            StringSet[Index] =  String2Attach;
            AttachedSucessfully = Yes;
        }

        return AttachedSucessfully;
    }

    bool UTF16_StringSet_Attach(UTF16 **StringSet, UTF16 *String2Attach, size_t Index) {
        AssertIO(StringSet != NULL);
        AssertIO(String2Attach != NULL);
        AssertIO(Index < UTF16_StringSet_GetNumStrings(StringSet));

        bool AttachedSucessfully = No;

        size_t NumStrings  = 0ULL;
        while (StringSet[NumStrings] != TextIO_NULLTerminator) {
            NumStrings      += 1;
        }
        if (NumStrings >= Index) {
            StringSet[Index] = String2Attach;
            AttachedSucessfully = Yes;
        }

        return AttachedSucessfully;
    }

    bool UTF32_StringSet_Attach(UTF32 **StringSet, UTF32 *String2Attach, size_t Index) {
        AssertIO(StringSet != NULL);
        AssertIO(String2Attach != NULL);
        AssertIO(Index < UTF32_StringSet_GetNumStrings(StringSet));

        bool AttachedSucessfully = No;

        size_t NumStrings  = 0ULL;
        while (StringSet[NumStrings] != TextIO_NULLTerminator) {
            NumStrings      += 1;
        }
        if (NumStrings >= Index) {
            StringSet[Index] = (UTF32*) String2Attach;
            AttachedSucessfully = Yes;
        }

        return AttachedSucessfully;
    }

    size_t UTF8_StringSet_GetNumStrings(ImmutableStringSet_UTF8 StringSet) {
        AssertIO(StringSet != NULL);

        size_t NumStrings = StringSet[-sizeof(size_t)];
        /*
        while (StringSet[NumStrings][0] != TextIO_NULLTerminator) {
            NumStrings += 1;
        }
         */
        return NumStrings;
    }

    size_t UTF16_StringSet_GetNumStrings(ImmutableStringSet_UTF16 StringSet) {
        AssertIO(StringSet != NULL);

        size_t NumStrings = StringSet[-(sizeof(size_t) / sizeof(UTF16))];
        /*
        while (StringSet[NumStrings][0] != TextIO_NULLTerminator) {
            NumStrings += 1;
        }
         */
        return NumStrings;
    }

    size_t UTF32_StringSet_GetNumStrings(ImmutableStringSet_UTF32 StringSet) {
        AssertIO(StringSet != NULL);

        size_t NumStrings = StringSet[-(sizeof(size_t) / sizeof(UTF32))];
        /*
        while (StringSet[NumStrings][0] != TextIO_NULLTerminator) {
            NumStrings += 1;
        }
         */
        return NumStrings;
    }

    size_t *UTF8_StringSet_GetStringSizesInCodeUnits(ImmutableStringSet_UTF8 StringSet) {
        AssertIO(StringSet != NULL);

        size_t *StringSetSizes     = NULL;
        size_t NumStrings          = UTF8_StringSet_GetNumStrings(StringSet);
        StringSetSizes             = calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(size_t));
        for (size_t String = 0ULL; String < NumStrings; String++) {
            StringSetSizes[String] = UTF8_GetStringSizeInCodeUnits(StringSet[String]);
        }

        return StringSetSizes;
    }

    size_t *UTF16_StringSet_GetStringSizesInCodeUnits(ImmutableStringSet_UTF16 StringSet) {
        AssertIO(StringSet != NULL);

        size_t *StringSetSizes     = NULL;

        size_t NumStrings          = UTF16_StringSet_GetNumStrings(StringSet);
        StringSetSizes             = (size_t*) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(size_t));
        for (size_t String = 0ULL; String < NumStrings; String++) {
            StringSetSizes[String] = UTF16_GetStringSizeInCodeUnits(StringSet[String]);
        }

        return StringSetSizes;
    }

    size_t *UTF8_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF8 StringSet) {
        AssertIO(StringSet != NULL);

        size_t *StringSetSizes   = NULL;

        size_t NumStrings        = UTF8_StringSet_GetNumStrings(StringSet);
        StringSetSizes           = calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(size_t));
        for (size_t String = 0ULL; String < NumStrings; String++) {
            StringSetSizes[String] = UTF8_GetStringSizeInCodePoints(StringSet[String]);
        }

        return StringSetSizes;
    }

    size_t *UTF16_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF16 StringSet) {
        AssertIO(StringSet != NULL);

        size_t *StringSetSizes     = NULL;

        size_t NumStrings          = UTF16_StringSet_GetNumStrings(StringSet);
        StringSetSizes             = calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(size_t));
        for (size_t String = 0ULL; String < NumStrings; String++) {
            StringSetSizes[String] = UTF16_GetStringSizeInCodePoints(StringSet[String]);
        }

        return StringSetSizes;
    }

    size_t *UTF32_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF32 StringSet) {
        AssertIO(StringSet != NULL);

        size_t *StringSetSizes     = NULL;

        size_t NumStrings          = UTF32_StringSet_GetNumStrings(StringSet);
        StringSetSizes             = calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(size_t));
        for (size_t String = 0ULL; String < NumStrings; String++) {
            StringSetSizes[String] = UTF32_GetStringSizeInCodePoints(StringSet[String]);
        }

        return StringSetSizes;
    }

    UTF32 **UTF8_StringSet_Decode(ImmutableStringSet_UTF8 StringSet) {
        AssertIO(StringSet != NULL);

        UTF32 **Decoded         = NULL;

        size_t NumStrings       = UTF8_StringSet_GetNumStrings(StringSet);
        Decoded                 = UTF32_StringSet_Init(NumStrings);
        AssertIO(Decoded != NULL);
        for (size_t String = 0ULL; String < NumStrings; String++) {
            Decoded[String] = UTF8_Decode(StringSet[String]);
        }
        return Decoded;
    }

    UTF32 **UTF16_StringSet_Decode(ImmutableStringSet_UTF16 StringSet) {
        AssertIO(StringSet != NULL);

        UTF32 **Decoded         = NULL;

        size_t NumStrings       = UTF16_StringSet_GetNumStrings(StringSet);
        Decoded                 = UTF32_StringSet_Init(NumStrings);
        AssertIO(Decoded != NULL);

        for (size_t String = 0ULL; String < NumStrings; String++) {
            Decoded[String] = UTF16_Decode(StringSet[String]);
        }
        return Decoded;
    }

    UTF8 **UTF8_StringSet_Encode(MutableStringSet_UTF32 StringSet) {
        AssertIO(StringSet != NULL);

        UTF8 **Encoded          = NULL;

        size_t NumStrings       = UTF32_StringSet_GetNumStrings(StringSet);
        Encoded                 = UTF8_StringSet_Init(NumStrings);

        AssertIO(Encoded != NULL);
        for (size_t String = 0ULL; String < NumStrings; String++) {
            Encoded[String] = UTF8_Encode(StringSet[String]);
        }
        return Encoded;
    }

    UTF16 **UTF16_StringSet_Encode(MutableStringSet_UTF32 StringSet) {
        AssertIO(StringSet != NULL);

        UTF16 **Encoded         = NULL;

        size_t NumStrings       = UTF32_StringSet_GetNumStrings(StringSet);
        Encoded                 = UTF16_StringSet_Init(NumStrings);

        for (size_t String = 0ULL; String < NumStrings; String++) {
            Encoded[String] = UTF16_Encode(StringSet[String]);
        }
        return Encoded;
    }

    UTF8 *UTF8_StringSet_Flatten(ImmutableStringSet_UTF8 StringSet) {
        AssertIO(StringSet != NULL);

        UTF8 *Flattened = NULL;

        UTF32 **StringSet32 = UTF8_StringSet_Decode(StringSet);
        UTF32  *Flattened32 = UTF32_StringSet_Flatten((const UTF32 *const *) StringSet32);
        UTF32_StringSet_Deinit(StringSet32);
        Flattened           = UTF8_Encode(Flattened32);
        UTF32_Deinit(Flattened32);

        return Flattened;
    }

    UTF16 *UTF16_StringSet_Flatten(ImmutableStringSet_UTF16 StringSet) {
        AssertIO(StringSet != NULL);

        UTF16 *Flattened    = NULL;

        UTF32 **StringSet32 = UTF16_StringSet_Decode((const UTF16**) StringSet);
        UTF32  *Flattened32 = UTF32_StringSet_Flatten((const UTF32**) StringSet32);
        UTF32_StringSet_Deinit(StringSet32);
        Flattened           = UTF16_Encode(Flattened32);
        UTF32_Deinit(Flattened32);

        return Flattened;
    }

    UTF32 *UTF32_StringSet_Flatten(ImmutableStringSet_UTF32 StringSet) {
        AssertIO(StringSet != NULL);

        UTF32 *Flattened           = NULL;

        size_t  NumStrings         = UTF32_StringSet_GetNumStrings(StringSet);
        size_t  FlattenedSize      = 0;
        size_t *PieceSizes         = calloc(NumStrings, sizeof(size_t));
        for (size_t String = 0; String < NumStrings; String++) {
            PieceSizes[String]     = UTF32_GetStringSizeInCodePoints(StringSet[String]);
            FlattenedSize         += PieceSizes[String];
        }
        Flattened                  = UTF32_Init(FlattenedSize);
        AssertIO(Flattened != NULL);
        size_t FlattenedOffset     = 0;
        size_t CurrentPiece        = 0;
        size_t CurrentPieceOffset  = 0;

        while (FlattenedOffset < FlattenedSize && CurrentPieceOffset < PieceSizes[CurrentPiece]) {
            Flattened[FlattenedOffset] = StringSet[CurrentPiece][CurrentPieceOffset];
        }

        return Flattened;
    }

    void UTF8_StringSet_Deinit(UTF8 **StringSet) {
        AssertIO(StringSet != NULL);

        size_t String = 0ULL;
        while (StringSet[String] != TextIO_NULLTerminator) {
            UTF8_Deinit(StringSet[String]);
            String += 1;
        }
        free(StringSet[-sizeof(size_t)]);
    }

    void UTF16_StringSet_Deinit(UTF16 **StringSet) {
        AssertIO(StringSet != NULL);

        size_t String = 0ULL;
        while (StringSet[String] != TextIO_NULLTerminator) {
            UTF16_Deinit(StringSet[String]);
            String += 1;
        }
        free(StringSet[-(sizeof(size_t) / sizeof(UTF16))]);
    }

    void UTF32_StringSet_Deinit(UTF32 **StringSet) {
        AssertIO(StringSet != NULL);

        size_t String = 0ULL;
        while (StringSet[String] != TextIO_NULLTerminator) {
            UTF32_Deinit(StringSet[String]);
            String += 1;
        }
        free(StringSet[-(sizeof(size_t) / sizeof(UTF32))]);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
