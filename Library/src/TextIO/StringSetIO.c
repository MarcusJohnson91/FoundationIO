#include "../../include/TextIO/StringSetIO.h"          /* Included for our declarations */

#include "../../include/BufferIO.h"                    /* Included for BufferIO_MemorySet */
#include "../../include/FileIO.h"                      /* Included for File operations */
#include "../../include/MathIO.h"                      /* Included for endian swapping */
#include "../../include/TextIO/LogIO.h"                /* Included for error logging */
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

    UTF8 **UTF8_StringSet_Init(uint64_t NumStrings) {
        UTF8 **StringSet = NULL;
        if (NumStrings > 0) {
            StringSet    = calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(UTF8*));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
            BufferIO_MemorySet8(*StringSet, UTF8_Debug_Text_8, NumStrings + TextIO_NULLTerminatorSize);
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumStrings %llu is invalid"), NumStrings);
        }
        return StringSet;
    }

    UTF16 **UTF16_StringSet_Init(uint64_t NumStrings) {
        UTF16 **StringSet = NULL;
        if (NumStrings > 0) {
            StringSet     = (UTF16**) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(UTF16*));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_TargetByteOrder == PlatformIO_TargetByteOrderIsBE)
            BufferIO_MemorySet16(*StringSet, UTF16_Debug_Text_16BE, NumStrings + TextIO_NULLTerminatorSize);
#elif (PlatformIO_TargetByteOrder == PlatformIO_TargetByteOrderIsLE)
            BufferIO_MemorySet16(*StringSet, UTF16_Debug_Text_16LE, NumStrings + TextIO_NULLTerminatorSize);
#endif /* ByteOrder */
#endif /* BuildType */
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumStrings %llu is invalid"), NumStrings);
        }
        return StringSet;
    }

    UTF32 **UTF32_StringSet_Init(uint64_t NumStrings) {
        UTF32 **StringSet = NULL;
        if (NumStrings > 0) {
            StringSet     = (UTF32**) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(UTF32*));
#if   (PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug)
#if   (PlatformIO_TargetByteOrder == PlatformIO_TargetByteOrderIsBE)
            BufferIO_MemorySet16(*StringSet, UTF32_Debug_Text_32BE, NumStrings + TextIO_NULLTerminatorSize);
#elif (PlatformIO_TargetByteOrder == PlatformIO_TargetByteOrderIsLE)
            BufferIO_MemorySet16(*StringSet, UTF32_Debug_Text_32LE, NumStrings + TextIO_NULLTerminatorSize);
#endif /* ByteOrder */
#endif /* BuildType */
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumStrings %llu is invalid"), NumStrings);
        }
        return StringSet;
    }

    bool UTF8_StringSet_Attach(UTF8 **StringSet, UTF8 *String2Attach, uint64_t Index) {
        bool AttachedSucessfully = No;
        if (StringSet != NULL && String2Attach != NULL) {
            uint64_t NumStrings  = 0ULL;
            while (StringSet[NumStrings] != TextIO_NULLTerminator) {
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
            while (StringSet[NumStrings] != TextIO_NULLTerminator) {
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
            while (StringSet[NumStrings] != TextIO_NULLTerminator) {
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
            while (StringSet[NumStrings][0] != TextIO_NULLTerminator) {
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
            while (StringSet[NumStrings][0] != TextIO_NULLTerminator) {
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
            while (StringSet[NumStrings][0] != TextIO_NULLTerminator) {
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
            StringSetSizes             = (uint64_t*) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(uint64_t));
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
            StringSetSizes             = (uint64_t*) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(uint64_t));
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
            StringSetSizes             = (uint64_t*) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(uint64_t));
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
            StringSetSizes             = (uint64_t*) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(uint64_t));
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
            StringSetSizes             = (uint64_t*) calloc(NumStrings + TextIO_NULLTerminatorSize, sizeof(uint64_t));
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
            while (StringSet[String] != TextIO_NULLTerminator) {
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
            while (StringSet[String] != TextIO_NULLTerminator) {
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
            while (StringSet[String] != TextIO_NULLTerminator) {
                free(StringSet[String]);
                String += 1;
            }
            free(StringSet);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSet Pointer is NULL"));
        }
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
