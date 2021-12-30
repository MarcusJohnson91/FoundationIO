#include "../include/CryptographyIO.h"    /* Included for our declarations */

#include "../include/AsynchronousIO.h"    /* Included for AsyncIOStream_Init */
#include "../include/BufferIO.h"          /* Included for BitBuffer for CRC32 and Adler32 */
#include "../include/FileIO.h"            /* Included for File operations */
#include "../include/MathIO.h"            /* Included for Bits2Bytes, etc */
#include "../include/TextIO/LogIO.h"      /* Included for error logging */
#include "../include/TextIO/StringIO.h"   /* Included for UTF8_GetStringSizeInCodeUnits */

#if defined(__has_include)
#if __has_include(<assert.h>)
#include <assert.h>
#endif /* __has_include */
#else
#define assert(X) _Assert(X)
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct InsecurePRNG {
        uint64_t State[4];
        uint16_t NumBitsRemaining;
    } InsecurePRNG;

    /*!
     @abstract Generated with WolframAlpha query "RandomInteger[{0, 18446744073709551615}]"
     @date     Dec 29, 2021
     @remark   Feel free to change these default seeds as often as you like.
     */
    static const uint64_t InsecurePRNG_DefaultSeed[4] = {
        0x1EDC87AE5FDA334D, 0xCBD633180FD9371B,
        0xA7D0130AD80AB611, 0x3A4CB28F2DBEF158,
    };

    /*
     Jump/long_jump and next require the state to be set to something that isn't zero before being called
     */
    static const uint64_t Xoshiro_Constants[2][4] = {
        [0] = {0x180EC6D33CFD0ABA, 0xD5A61266F0C9392C, 0xA9582618E03FC9AA, 0x39ABDC4529B1661C}, // JUMP
        [1] = {0x76E15D3EFEFDCBBF, 0xC5004E441C522FB3, 0x77710069854EE241, 0x39109BB02ACBE635}, // LONG_JUMP
    };

    static uint64_t InsecurePRNG_Seed(uint64_t Seed) { // srand() replacement, based on CC0 SplitMix64
        uint64_t Z = Seed + 0x9E3779B97F4A7C15;
        Z          = (Z ^ (Z >> 30)) * 0xBF58476D1CE4E5B9;
        Z          = (Z ^ (Z >> 27)) * 0x94D049BB133111EB;
        return Z ^ (Z >> 31);
    }

    static void InsecurePRNG_MixState(InsecurePRNG *Insecure, bool IsLongJump) { // Xoshiro256** jump/long_jump + next
        uint64_t S0 = 0, S1 = 0, S2 = 0, S3 = 0;
        if (IsLongJump != false) {
            if (Insecure->State[0] != 0) {
                S0 = Insecure->State[0];
            } else {
                S0 = Xoshiro_Constants[1][0];
            }
            S1 = Xoshiro_Constants[1][1];
            S2 = Xoshiro_Constants[1][2];
            S3 = Xoshiro_Constants[1][3];
        } else {
            if (Insecure->State[0] != 0) {
                S0 = Insecure->State[0];
            } else {
                S0 = Xoshiro_Constants[0][0];
            }
            S1 = Xoshiro_Constants[0][1];
            S2 = Xoshiro_Constants[0][2];
            S3 = Xoshiro_Constants[0][3];
        }
        for (uint8_t LeftShift = 0; LeftShift < 64; LeftShift++) {
            S0 ^= Rotate(RotationType_Left, LeftShift, Insecure->State[0]);
            S1 ^= Rotate(RotationType_Left, LeftShift, Insecure->State[1]);
            S2 ^= Rotate(RotationType_Left, LeftShift, Insecure->State[2]);
            S3 ^= Rotate(RotationType_Left, LeftShift, Insecure->State[3]);
        }
        const uint64_t Temp = S0 << 17;
        Insecure->State[2] ^= S0;
        Insecure->State[3] ^= S1;
        Insecure->State[1] ^= S2;
        Insecure->State[0] ^= S3;
        Insecure->State[2] ^= Temp;
        Insecure->State[3]  = Rotate(RotationType_Left, 45, Insecure->State[3]);
        Insecure->NumBitsRemaining = 256;
    }

    InsecurePRNG *InsecurePRNG_Init(uint64_t Seed) {
        InsecurePRNG *Insecure = calloc(1, sizeof(InsecurePRNG));
        assert(Insecure != NULL && "Couldn't allocate InsecurePRNG!");
        if (Seed == 0) {
            Insecure->State[0] = InsecurePRNG_Seed(InsecurePRNG_DefaultSeed[0]);
        } else {
            Insecure->State[0] = InsecurePRNG_Seed(Seed);
        }
        Insecure->State[1]     = InsecurePRNG_Seed(InsecurePRNG_DefaultSeed[1]);
        Insecure->State[2]     = InsecurePRNG_Seed(InsecurePRNG_DefaultSeed[2]);
        Insecure->State[3]     = InsecurePRNG_Seed(InsecurePRNG_DefaultSeed[3]);
        InsecurePRNG_MixState(Insecure, /*IsLongJump*/ true);
        return Insecure;
    }

    /* RAND_MAX is just 31 bits on MacOS? weird */
    static uint64_t InsecurePRNG_ExtractBits(InsecurePRNG *Insecure, uint8_t NumBits) {
        uint64_t Value = 0;
        if (Insecure != NULL && (NumBits > 0 && NumBits < 64)) {
            if (Insecure->NumBitsRemaining < NumBits) {
                // Mix the state again, this time
                InsecurePRNG_MixState(Insecure, /*IsLongJump*/ true);
            }

            while (NumBits > 0) {
                uint8_t  Start                  = 256 - Insecure->NumBitsRemaining;
                uint8_t  End                    = (Start + NumBits) % 64;
                uint8_t  NumBitsAvailableInWord = 64 - ((End - Start) % 64);
                uint8_t  Shift                  = (64 - (Start + NumBitsAvailableInWord));
                uint64_t Mask                   = (Exponentiate(2, NumBitsAvailableInWord) - 1) << Shift;
                uint64_t Extracted              = (Insecure->State[Start / 64] & Mask) >> Shift;
                Value                         <<= Shift;
                Value                          |= Extracted;
                NumBits                        -= NumBitsAvailableInWord;
                Insecure->NumBitsRemaining     -= NumBitsAvailableInWord;
            }
        }
        return Value;
    }

    int64_t InsecurePRNG_CreateInteger(InsecurePRNG *Insecure, uint8_t IntegerSizeInBits) {
        int64_t Value = InsecurePRNG_ExtractBits(Insecure, IntegerSizeInBits);
        return Value; // SignExtend the Value before returning it
    }

    int64_t InsecurePRNG_CreateIntegerInRange(InsecurePRNG *Insecure, int64_t Min, int64_t Max) {
        int64_t Value = 0;
        uint8_t  BitsToRead = Logarithm(2, Max % Min);
        uint64_t Extracted  = InsecurePRNG_ExtractBits(Insecure, BitsToRead);
        if (Min <= 0 && Max <= 0) {
            Value = (0xFFFFFFFFFFFFFFFF - Extracted) + 1;
        } else if (Min >= 0 && Max >= 0) {
            Value = (Extracted - 0x8000000000000001);
        } else {
            Value = Extracted;
        }
        return Value;
    }

    double InsecurePRNG_CreateDecimal(InsecurePRNG *Insecure) {
        double Decimal        = 0.0;
        if (Insecure != NULL) {
            int8_t   Sign     = InsecurePRNG_CreateInteger(Insecure, 1);
            int16_t  Exponent = InsecurePRNG_CreateIntegerInRange(Insecure, -1023, 1023);
            uint64_t Mantissa = InsecurePRNG_CreateInteger(Insecure, 53);

            Decimal           = InsertSignD(Decimal, Sign);
            Decimal           = InsertExponentD(Decimal, Exponent);
            Decimal           = InsertMantissaD(Decimal, Mantissa);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("InsecurePRNG Pointer is NULL"));
        }
        return Decimal;
    }

    bool InsecurePRNG_Erase(InsecurePRNG *Insecure) {
        bool EraseWasSucessful = No;
        if (Insecure != NULL) {
            Insecure->State[0]         = 0;
            Insecure->State[1]         = 0;
            Insecure->State[2]         = 0;
            Insecure->State[3]         = 0;
            Insecure->NumBitsRemaining = 0;
            EraseWasSucessful          = Yes;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("InsecurePRNG Pointer is NULL"));
        }
        return EraseWasSucessful;
    }

    void InsecurePRNG_Deinit(InsecurePRNG *Insecure) {
        if (Insecure != NULL) {
            InsecurePRNG_Erase(Insecure);
            free(Insecure);
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
