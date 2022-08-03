#include "../include/CryptographyIO.h"    /* Included for our declarations */

#include "../include/AssertIO.h"          /* Included for Assertions */
#include "../include/AsynchronousIO.h"    /* Included for AsynchronousIOStream_Init */
#include "../include/BufferIO.h"          /* Included for BitBuffer */
#include "../include/FileIO.h"            /* Included for File operations */
#include "../include/MathIO.h"            /* Included for Bits2Bytes, etc */
#include "../include/TextIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/TextIO/StringIO.h"   /* Included for StringIO's declarations */

#include <string.h>
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#include <BCrypt.h>
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsLinux)
#include <linux/random.h>
#include <sys/syscall.h>
#include <sys/random.h>
#endif /* TargetOS */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct InsecurePRNG {
        uint64_t State[4];
        uint16_t NumBitsRemaining;
    } InsecurePRNG;

    /*!
     @abstract Generated with ANU QRNG
     @date     Dec 31, 2021
     @remark   Feel free to change these default seeds as often as you like.
     */
    static const uint64_t InsecurePRNG_DefaultSeed[4] = {
        0x66506C56A7381C49, 0x56512E8D36C1F104,
        0x85DED0B33FDCBAC3, 0xB12A0DA1E2C23775,
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
        AssertIO(Insecure != NULL);

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
        AssertIO(Insecure != NULL && "Couldn't allocate InsecurePRNG");

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
        AssertIO(Insecure != NULL);
        AssertIO(NumBits <= 64);

        uint64_t Value = 0;
        if (Insecure->NumBitsRemaining < NumBits) {
            // Mix the state again, this time
            InsecurePRNG_MixState(Insecure, /*IsLongJump*/ true);
        }

        while (NumBits > 0) {
            uint8_t  Start                  = Insecure->NumBitsRemaining - 256;
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
        return Value;
    }

    uint64_t InsecurePRNG_CreateInteger(InsecurePRNG *Insecure, uint8_t IntegerSizeInBits) {
        AssertIO(Insecure != NULL);
        AssertIO(IntegerSizeInBits <= 64);

        return InsecurePRNG_ExtractBits(Insecure, IntegerSizeInBits); // SignExtend the Value before returning it
    }

    uint64_t InsecurePRNG_CreateIntegerInRange(InsecurePRNG *Insecure, int64_t Min, int64_t Max) {
        AssertIO(Insecure != NULL);

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
        AssertIO(Insecure != NULL);

        double Decimal        = 0.0;

        int8_t   Sign     = InsecurePRNG_CreateInteger(Insecure, 1);
        int16_t  Exponent = InsecurePRNG_CreateIntegerInRange(Insecure, -1023, 1023);
        uint64_t Mantissa = InsecurePRNG_CreateInteger(Insecure, 53);

        Decimal           = InsertSignD(Decimal, Sign);
        Decimal           = InsertExponentD(Decimal, Exponent);
        Decimal           = InsertMantissaD(Decimal, Mantissa);
        return Decimal;
    }

    bool InsecurePRNG_Erase(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);

        bool EraseWasSucessful = No;
        Insecure->State[0]         = 0;
        Insecure->State[1]         = 0;
        Insecure->State[2]         = 0;
        Insecure->State[3]         = 0;
        Insecure->NumBitsRemaining = 0;
        EraseWasSucessful          = Yes;
        return EraseWasSucessful;
    }

    void InsecurePRNG_Deinit(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        InsecurePRNG_Erase(Insecure);
        free(Insecure);
    }

    /* MD5, Based on WJCryptLib, Unlicense, Date = Dec_30_2021 */
    /* MD5.h */
    typedef struct MD5Context {
        uint32_t Low; // Lo
        uint32_t High; // Hi
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;
        uint8_t  Buffer[64];
        uint32_t Block[16];
    } MD5Context;

#define MD5_HASH_SIZE (128 / 8)

    typedef struct MD5Hash {
        uint8_t Bytes[MD5_HASH_SIZE];
    } MD5Hash;
    /* MD5.h */

    /* MD5.c */
#define MD5_F(X, Y, Z) ((Z) ^ ((X) & ((Y) ^ (Z))))
#define MD5_G(X, Y, Z) ((Y) ^ ((Z) & ((X) ^ (Y))))
#define MD5_H(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define MD5_I(X, Y, Z) ((Y) ^ ((X) | ~(Z)))

#define MD5_STEP(F, A, B, C, D, X, T, S)                  \
(A) += F((B), (C), (D)) + (X) + (T);                      \
(A) = (((A) << (S)) | (((A) &0xffffffff) >> (32 - (S)))); \
(A) += (B);

    static void *MD5Transform(MD5Context *Context, void const *data, uintmax_t size) {
        AssertIO(Context != NULL);

        uint8_t *ptr;
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;
        uint32_t A_Saved;
        uint32_t B_Saved;
        uint32_t C_Saved;
        uint32_t D_Saved;

#define GET(n) (Context->Block[(n)])
#define SET(n) (Context->Block[(n)] = ((uint32_t) ptr[(n) *4 + 0] << 0) | ((uint32_t) ptr[(n) *4 + 1] << 8) | ((uint32_t) ptr[(n) *4 + 2] << 16) | ((uint32_t) ptr[(n) *4 + 3] << 24))

        ptr = (uint8_t *) data;

        A = Context->A;
        B = Context->B;
        C = Context->C;
        D = Context->D;

        do {
            A_Saved = A;
            B_Saved = B;
            C_Saved = C;
            D_Saved = D;

            // Round 1
            MD5_STEP(MD5_F, A, B, C, D, SET(0), 0xD76AA478, 7)
            MD5_STEP(MD5_F, D, A, B, C, SET(1), 0xE8C7B756, 12)
            MD5_STEP(MD5_F, C, D, A, B, SET(2), 0x242070DB, 17)
            MD5_STEP(MD5_F, B, C, D, A, SET(3), 0xC1BDCEEE, 22)
            MD5_STEP(MD5_F, A, B, C, D, SET(4), 0xF57C0FAF, 7)
            MD5_STEP(MD5_F, D, A, B, C, SET(5), 0x4787C62A, 12)
            MD5_STEP(MD5_F, C, D, A, B, SET(6), 0xA8304613, 17)
            MD5_STEP(MD5_F, B, C, D, A, SET(7), 0xFD469501, 22)
            MD5_STEP(MD5_F, A, B, C, D, SET(8), 0x698098D8, 7)
            MD5_STEP(MD5_F, D, A, B, C, SET(9), 0x8B44F7AF, 12)
            MD5_STEP(MD5_F, C, D, A, B, SET(10), 0xFFFF5BB1, 17)
            MD5_STEP(MD5_F, B, C, D, A, SET(11), 0x895CD7BE, 22)
            MD5_STEP(MD5_F, A, B, C, D, SET(12), 0x6B901122, 7)
            MD5_STEP(MD5_F, D, A, B, C, SET(13), 0xFD987193, 12)
            MD5_STEP(MD5_F, C, D, A, B, SET(14), 0xA679438E, 17)
            MD5_STEP(MD5_F, B, C, D, A, SET(15), 0x49B40821, 22)

            // Round 2
            MD5_STEP(MD5_G, A, B, C, D, GET(1), 0xF61E2562, 5)
            MD5_STEP(MD5_G, D, A, B, C, GET(6), 0xC040B340, 9)
            MD5_STEP(MD5_G, C, D, A, B, GET(11), 0x265E5A51, 14)
            MD5_STEP(MD5_G, B, C, D, A, GET(0), 0xE9B6C7AA, 20)
            MD5_STEP(MD5_G, A, B, C, D, GET(5), 0xD62F105D, 5)
            MD5_STEP(MD5_G, D, A, B, C, GET(10), 0x2441453, 9)
            MD5_STEP(MD5_G, C, D, A, B, GET(15), 0xD8A1E681, 14)
            MD5_STEP(MD5_G, B, C, D, A, GET(4), 0xE7D3FBC8, 20)
            MD5_STEP(MD5_G, A, B, C, D, GET(9), 0x21E1CDE6, 5)
            MD5_STEP(MD5_G, D, A, B, C, GET(14), 0xC33707D6, 9)
            MD5_STEP(MD5_G, C, D, A, B, GET(3), 0xF4D50D87, 14)
            MD5_STEP(MD5_G, B, C, D, A, GET(8), 0x455A14ED, 20)
            MD5_STEP(MD5_G, A, B, C, D, GET(13), 0xA9E3E905, 5)
            MD5_STEP(MD5_G, D, A, B, C, GET(2), 0xFCEFA3F8, 9)
            MD5_STEP(MD5_G, C, D, A, B, GET(7), 0x676F02D9, 14)
            MD5_STEP(MD5_G, B, C, D, A, GET(12), 0x8D2A4C8A, 20)

            // Round 3
            MD5_STEP(MD5_H, A, B, C, D, GET(5), 0xFFFA3942, 4)
            MD5_STEP(MD5_H, D, A, B, C, GET(8), 0x8771F681, 11)
            MD5_STEP(MD5_H, C, D, A, B, GET(11), 0x6D9D6122, 16)
            MD5_STEP(MD5_H, B, C, D, A, GET(14), 0xFDE5380C, 23)
            MD5_STEP(MD5_H, A, B, C, D, GET(1), 0xA4BEEA44, 4)
            MD5_STEP(MD5_H, D, A, B, C, GET(4), 0x4BDECFA9, 11)
            MD5_STEP(MD5_H, C, D, A, B, GET(7), 0xF6BB4B60, 16)
            MD5_STEP(MD5_H, B, C, D, A, GET(10), 0xBEBFBC70, 23)
            MD5_STEP(MD5_H, A, B, C, D, GET(13), 0x289B7EC6, 4)
            MD5_STEP(MD5_H, D, A, B, C, GET(0), 0xEAA127FA, 11)
            MD5_STEP(MD5_H, C, D, A, B, GET(3), 0xD4EF3085, 16)
            MD5_STEP(MD5_H, B, C, D, A, GET(6), 0x4881D05, 23)
            MD5_STEP(MD5_H, A, B, C, D, GET(9), 0xD9D4D039, 4)
            MD5_STEP(MD5_H, D, A, B, C, GET(12), 0xE6DB99E5, 11)
            MD5_STEP(MD5_H, C, D, A, B, GET(15), 0x1FA27CF8, 16)
            MD5_STEP(MD5_H, B, C, D, A, GET(2), 0xC4AC5665, 23)

            // Round 4
            MD5_STEP(MD5_I, A, B, C, D, GET(0), 0xF4292244, 6)
            MD5_STEP(MD5_I, D, A, B, C, GET(7), 0x432AFF97, 10)
            MD5_STEP(MD5_I, C, D, A, B, GET(14), 0xAB9423A7, 15)
            MD5_STEP(MD5_I, B, C, D, A, GET(5), 0xFC93A039, 21)
            MD5_STEP(MD5_I, A, B, C, D, GET(12), 0x655B59C3, 6)
            MD5_STEP(MD5_I, D, A, B, C, GET(3), 0x8F0CCC92, 10)
            MD5_STEP(MD5_I, C, D, A, B, GET(10), 0xFFEFF47D, 15)
            MD5_STEP(MD5_I, B, C, D, A, GET(1), 0x85845DD1, 21)
            MD5_STEP(MD5_I, A, B, C, D, GET(8), 0x6FA87E4F, 6)
            MD5_STEP(MD5_I, D, A, B, C, GET(15), 0xFE2CE6E0, 10)
            MD5_STEP(MD5_I, C, D, A, B, GET(6), 0xA3014314, 15)
            MD5_STEP(MD5_I, B, C, D, A, GET(13), 0x4E0811A1, 21)
            MD5_STEP(MD5_I, A, B, C, D, GET(4), 0xF7537E82, 6)
            MD5_STEP(MD5_I, D, A, B, C, GET(11), 0xBD3AF235, 10)
            MD5_STEP(MD5_I, C, D, A, B, GET(2), 0x2AD7D2BB, 15)
            MD5_STEP(MD5_I, B, C, D, A, GET(9), 0xEB86D391, 21)

            A += A_Saved;
            B += B_Saved;
            C += C_Saved;
            D += D_Saved;

            ptr += 64;
        } while (size -= 64);

        Context->A = A;
        Context->B = B;
        Context->C = C;
        Context->D = D;
#undef GET
#undef SET
        return ptr;
    }

    MD5Context *MD5Context_Init(void) {
        MD5Context *Context = calloc(1, sizeof(MD5Context));
        AssertIO(Context != NULL);

        Context->A      = 0x67452301;
        Context->B      = 0xEFCDAB89;
        Context->C      = 0x98BADCFE;
        Context->D      = 0x10325476;

        Context->Low    = 0;
        Context->High   = 0;

        return Context;
    }

    void MD5Update(MD5Context *Context, void const *Buffer, size_t BufferSize) {
        AssertIO(Context != NULL);
        AssertIO(Buffer != NULL);

        uint32_t Low_Saved = Context->Low;
        uint32_t Used;
        uint32_t Free;

        if ((Context->Low = (Low_Saved + BufferSize) & 0x1FFFFFFF) < Low_Saved) {
            Context->High++;
        }
        Context->High += (uint32_t) (BufferSize >> 29);

        Used = Low_Saved & 0x3F;

        if (Used) {
            Free = 64 - Used;

            if (BufferSize < Free) {
                BufferIO_MemoryCopy8(&Context->Buffer[Used], Buffer, BufferSize);
                return;
            }

            BufferIO_MemoryCopy8(&Context->Buffer[Used], Buffer, Free);
            Buffer = (uint8_t *) Buffer + Free;
            BufferSize -= Free;
            MD5Transform(Context, Context->Buffer, 64);
        }

        if (BufferSize >= 64) {
            Buffer = MD5Transform(Context, Buffer, BufferSize & ~(uint32_t) 0x3F);
            BufferSize &= 0x3F;
        }

        BufferIO_MemoryCopy8(Context->Buffer, Buffer, BufferSize);
    }

    void MD5Finalize(MD5Context *Context, MD5Hash *Digest) {
        AssertIO(Context != NULL);
        AssertIO(Digest != NULL);

        uint32_t Used;
        uint32_t Free;

        Used = Context->Low & 0x3F;

        Context->Buffer[Used++] = 0x80;

        Free = 64 - Used;

        if (Free < 8) {
            BufferIO_MemorySet8(&Context->Buffer[Used], 0, Free);
            MD5Transform(Context, Context->Buffer, 64);
            Used = 0;
            Free = 64;
        }

        BufferIO_MemorySet8(&Context->Buffer[Used], 0, Free - 8);

        Context->Low <<= 3;
        Context->Buffer[56] = (uint8_t) (Context->Low);
        Context->Buffer[57] = (uint8_t) (Context->Low >> 8);
        Context->Buffer[58] = (uint8_t) (Context->Low >> 16);
        Context->Buffer[59] = (uint8_t) (Context->Low >> 24);
        Context->Buffer[60] = (uint8_t) (Context->High);
        Context->Buffer[61] = (uint8_t) (Context->High >> 8);
        Context->Buffer[62] = (uint8_t) (Context->High >> 16);
        Context->Buffer[63] = (uint8_t) (Context->High >> 24);

        MD5Transform(Context, Context->Buffer, 64);

        Digest->Bytes[0]  = (uint8_t) (Context->A);
        Digest->Bytes[1]  = (uint8_t) (Context->A >> 8);
        Digest->Bytes[2]  = (uint8_t) (Context->A >> 16);
        Digest->Bytes[3]  = (uint8_t) (Context->A >> 24);
        Digest->Bytes[4]  = (uint8_t) (Context->B);
        Digest->Bytes[5]  = (uint8_t) (Context->B >> 8);
        Digest->Bytes[6]  = (uint8_t) (Context->B >> 16);
        Digest->Bytes[7]  = (uint8_t) (Context->B >> 24);
        Digest->Bytes[8]  = (uint8_t) (Context->C);
        Digest->Bytes[9]  = (uint8_t) (Context->C >> 8);
        Digest->Bytes[10] = (uint8_t) (Context->C >> 16);
        Digest->Bytes[11] = (uint8_t) (Context->C >> 24);
        Digest->Bytes[12] = (uint8_t) (Context->D);
        Digest->Bytes[13] = (uint8_t) (Context->D >> 8);
        Digest->Bytes[14] = (uint8_t) (Context->D >> 16);
        Digest->Bytes[15] = (uint8_t) (Context->D >> 24);
    }
    
    char *MD5ToString(MD5Hash *Digest) {
        AssertIO(Digest != NULL);

        uint8_t  MD5StringSize = (MD5_HASH_SIZE * 2) + TextIO_NULLTerminatorSize;
        char    *String        = calloc(MD5StringSize, sizeof(char));
        uint8_t  Offset        = 0;
        for (uint8_t Character = 0; Character < MD5StringSize - 1; Character++) {
            Offset += snprintf(String + Offset, MD5StringSize - Offset, "%hhX", Digest->Bytes[Character]);
        }
        return String;
    }
    /* MD5.c */
    /* MD5, Based on WJCryptLib, Unlicense, Date = Dec_30_2021 */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
