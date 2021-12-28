#include "../include/CryptographyIO.h"    /* Included for our declarations */

#include "../include/AsynchronousIO.h"    /* Included for AsyncIOStream_Init */
#include "../include/BufferIO.h"          /* Included for BitBuffer for CRC32 and Adler32 */
#include "../include/FileIO.h"            /* Included for File operations */
#include "../include/MathIO.h"            /* Included for Bits2Bytes, etc */
#include "../include/TextIO/LogIO.h"      /* Included for error logging */
#include "../include/TextIO/StringIO.h"   /* Included for UTF8_GetStringSizeInCodeUnits */


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
        uint64_t State[16];
        uint64_t Output[16];
        size_t   Iteration;
    } InsecurePRNG;

    typedef struct SecureRNG {
        uint8_t  *EntropyPool;
        size_t    NumBits;
        size_t    BitOffset;
    } SecureRNG;

    InsecurePRNG *InsecurePRNG_Init(uint64_t Seed[4]) {
        InsecurePRNG *Insecure = (InsecurePRNG*) calloc(1, sizeof(InsecurePRNG));
        if (Insecure != NULL) {
            static const uint64_t Phi[16] = {
                0x9E3779B97F4A7C15, 0xF39CC0605CEDC834, 0x1082276BF3A27251, 0xF86C6A11D0C18E95,
                0x2767F0B153D27B7F, 0x0347045B5BF1827F, 0x01886F0928403002, 0xC1D64BA40F335E36,
                0xF06AD7AE9717877E, 0x85839D6EFFBD7DC6, 0x64D325D1C5371682, 0xCADD0CCCFDFFBBE1,
                0x626E33B8D04B4331, 0xBBF73C790D94F79D, 0x471C4AB3ED3D82A5, 0xFEC507705E4AE6E5,
            };
            Insecure->State[0x0]   = Phi[0x3];
            Insecure->State[0x1]   = Phi[0x2] ^ Seed[1];
            Insecure->State[0x2]   = Phi[0x1];
            Insecure->State[0x3]   = Phi[0x0] ^ Seed[0];
            Insecure->State[0x4]   = Phi[0x7];
            Insecure->State[0x5]   = Phi[0x6] ^ Seed[3];
            Insecure->State[0x6]   = Phi[0x5];
            Insecure->State[0x7]   = Phi[0x4] ^ Seed[2];
            Insecure->State[0x8]   = Phi[0xB];
            Insecure->State[0x9]   = Phi[0xA] ^ Seed[3];
            Insecure->State[0xA]   = Phi[0x9];
            Insecure->State[0xB]   = Phi[0x8] ^ Seed[2];
            Insecure->State[0xC]   = Phi[0xF];
            Insecure->State[0xD]   = Phi[0xE] ^ Seed[1];
            Insecure->State[0xE]   = Phi[0xD];
            Insecure->State[0xF]   = Phi[0xC] ^ Seed[0];

            for (uint8_t Index = 0; Index < 13; Index++) {
                Insecure->State[0] = Insecure->Output[3];
                Insecure->State[1] = Insecure->Output[2];
                Insecure->State[2] = Insecure->Output[1];
                Insecure->State[3] = Insecure->Output[0];
            }
        }
        return Insecure;
    }

    void InsecurePRNG_Generate(InsecurePRNG *Insecure, uint8_t *Buffer, size_t BufferSize) {
        if (Insecure != NULL) {
            uint64_t *Buffer64                         = (uint64_t*) Buffer;
            uint64_t State1                            = Insecure->State[1];
            uint64_t State3                            = Insecure->State[3];
            uint64_t Iteration                         = Insecure->Iteration;
            uint64_t Increment[4] = {1, 3, 5, 7};

            for (size_t OutputWord = 0; OutputWord < 4; OutputWord++) {
                for (size_t BufferWord = 0; BufferWord < BufferSize; BufferWord += 64) {
                    Buffer64[BufferWord + OutputWord]  = Insecure->Output[OutputWord];
                    State1                            += Iteration;
                    State3                            += Iteration;
                    for (uint8_t Index = 0; Index < 4; Index++) {
                        Iteration                     += Increment[Index];
                    }
                    uint64_t U0 = Insecure->State[0x0] >> 1;
                    uint64_t U1 = Insecure->State[0x1] >> 1;
                    uint64_t U2 = Insecure->State[0x2] >> 1;
                    uint64_t U3 = Insecure->State[0x3] >> 1;
                    uint64_t U4 = Insecure->State[0x4] >> 3;
                    uint64_t U5 = Insecure->State[0x5] >> 3;
                    uint64_t U6 = Insecure->State[0x6] >> 3;
                    uint64_t U7 = Insecure->State[0x7] >> 3;
                    uint64_t U8 = Insecure->State[0x8] >> 1;
                    uint64_t U9 = Insecure->State[0x9] >> 1;
                    uint64_t UA = Insecure->State[0xA] >> 1;
                    uint64_t UB = Insecure->State[0xB] >> 1;
                    uint64_t UC = Insecure->State[0xC] >> 3;
                    uint64_t UD = Insecure->State[0xD] >> 3;
                    uint64_t UE = Insecure->State[0xE] >> 3;
                    uint64_t UF = Insecure->State[0xF] >> 3;

                    uint64_t T0 = ((Insecure->State[0x2] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[1] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t T1 = ((Insecure->State[0x1] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t T2 = ((Insecure->State[0x0] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[3] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t T3 = ((Insecure->State[0x3] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[2] & 0xFFFFFFFF00000000) >> 32);

                    uint64_t T4 = ((Insecure->State[0x5] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[4] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t T5 = ((Insecure->State[0x4] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[7] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t T6 = ((Insecure->State[0x7] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[6] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t T7 = ((Insecure->State[0x6] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[5] & 0xFFFFFFFF00000000) >> 32);

                    uint64_t T8 = ((Insecure->State[0xA] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0x9] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t T9 = ((Insecure->State[0x9] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0x8] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t TA = ((Insecure->State[0x8] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0xB] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t TB = ((Insecure->State[0xB] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0xA] & 0xFFFFFFFF00000000) >> 32);

                    uint64_t TC = ((Insecure->State[0xD] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0xC] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t TD = ((Insecure->State[0xC] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0xF] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t TE = ((Insecure->State[0xF] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0xE] & 0xFFFFFFFF00000000) >> 32);
                    uint64_t TF = ((Insecure->State[0xE] & 0x00000000FFFFFFFF) << 32) | ((Insecure->State[0xD] & 0xFFFFFFFF00000000) >> 32);

                    Insecure->State[0x0]  = T0 + U0;
                    Insecure->State[0x1]  = T1 + U1;
                    Insecure->State[0x2]  = T2 + U2;
                    Insecure->State[0x3]  = T3 + U3;
                    Insecure->State[0x4]  = T4 + U4;
                    Insecure->State[0x5]  = T5 + U5;
                    Insecure->State[0x6]  = T6 + U6;
                    Insecure->State[0x7]  = T7 + U7;
                    Insecure->State[0x8]  = T8 + U8;
                    Insecure->State[0x9]  = T9 + U9;
                    Insecure->State[0xA]  = TA + UA;
                    Insecure->State[0xB]  = TB + UB;
                    Insecure->State[0xC]  = TC + UC;
                    Insecure->State[0xD]  = TD + UD;
                    Insecure->State[0xE]  = TE + UE;
                    Insecure->State[0xF]  = TF + UF;

                    Insecure->Output[0x0] = U0 ^ T4;
                    Insecure->Output[0x1] = U1 ^ T5;
                    Insecure->Output[0x2] = U2 ^ T6;
                    Insecure->Output[0x3] = U3 ^ T7;
                    Insecure->Output[0x4] = U8 ^ TC;
                    Insecure->Output[0x5] = U9 ^ TD;
                    Insecure->Output[0x6] = UA ^ TE;
                    Insecure->Output[0x7] = UB ^ TF;

                    Insecure->Output[0x8] = Insecure->State[0x0] ^ Insecure->State[0xC];
                    Insecure->Output[0x9] = Insecure->State[0x1] ^ Insecure->State[0xD];
                    Insecure->Output[0xA] = Insecure->State[0x2] ^ Insecure->State[0xE];
                    Insecure->Output[0xB] = Insecure->State[0x3] ^ Insecure->State[0xF];
                    Insecure->Output[0xC] = Insecure->State[0x8] ^ Insecure->State[0x4];
                    Insecure->Output[0xD] = Insecure->State[0x9] ^ Insecure->State[0x5];
                    Insecure->Output[0xE] = Insecure->State[0xA] ^ Insecure->State[0x6];
                    Insecure->Output[0xF] = Insecure->State[0xB] ^ Insecure->State[0x7];

                    Insecure->Iteration   = Iteration;
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("InsecurePRNG Pointer is NULL"));
        }
    }

    void InsecurePRNG_Deinit(InsecurePRNG *Insecure) {
        if (Insecure != NULL) {
            for (uint8_t Index = 0; Index < 16; Index++) {
                Insecure->Output[Index] = 0;
                Insecure->State[Index]  = 0;
            }
            Insecure->Iteration = 0;
            free(Insecure->Output);
            free(Insecure->State);
            free(Insecure);
        }
    }

    static int64_t SecureRNG_BaseSeed(uint8_t NumBytes) {
        int64_t RandomValue = 0LL;
        if (NumBytes <= 8) {
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
            AsyncIOStream *Random     = AsyncIOStream_Init();
            bool OpenedSuccessfully   = AsyncIOStream_OpenPathUTF8(Random, UTF8String("/dev/urandom"), FileMode_Read | FileMode_Binary);
            if (OpenedSuccessfully) {
                size_t BytesRead      = AsyncIOStream_Read(Random, &RandomValue, 8, 1);
                if (BytesRead != 8) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Could not read 8 bytes from /dev/urandom"));
                }
            }
            AsyncIOStream_Deinit(Random);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsLinux)
            //int BytesRead             = syscall(SYS_getrandom, &RandomValue, NumBytes, 1);
            ssize_t BytesRead           = getrandom(&RandomValue, NumBytes, 1);
            if (BytesRead != NumBytes) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Could not read 8 bytes from getrandom on Linux"));
            }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsBSD)
            arc4random_buf(&RandomValue, NumBytes);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            BCryptGenRandom(NULL, (PUCHAR) &RandomValue, NumBytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#endif
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Can't return more than 8 bytes"));
        }
        return RandomValue;
    }

    static void SecureRNG_Seed(SecureRNG *Random) {
        if (Random != NULL) {
            if (Random->EntropyPool != NULL) {

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX) && !PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsLinux)
                arc4random_buf(Random->EntropyPool, Bits2Bytes(RoundingType_Down, Random->NumBits));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsLinux)
                AsyncIOStream *Random     = AsyncIOStream_Init();
                bool OpenedSuccessfully   = AsyncIOStream_OpenPathUTF8(Random, UTF8String("/dev/random"), FileMode_Read | FileMode_Binary);
                if (OpenedSuccessfully) {
                    size_t BytesRead      = AsyncIOStream_Read(&Random->EntropyPool, &RandomValue, Bits2Bytes(RoundingType_Down, Random->NumBits), 1);
                    if (BytesRead != Bits2Bytes(RoundingType_Down, Random->NumBits)) {
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Could not read 8 bytes from /dev/urandom"));
                    }
                }
                AsyncIOStream_Deinit(Random);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
                NTSTATUS Status           = BCryptGenRandom(NULL, (PUCHAR) Random->EntropyPool, (ULONG) Bits2Bytes(RoundingType_Down, Random->NumBits), (ULONG) BCRYPT_USE_SYSTEM_PREFERRED_RNG);
                if (Status <= 0) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Failed to read random data, SecureRNG is extremely insecure, aborting"));
                    abort();
                }
#endif
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
    }

    static void SecureRNG_Mix(SecureRNG *Random) {
        if (Random != NULL) {
            for (uint64_t Word = 0ULL; Word < Bits2Bytes(RoundingType_Down, Random->NumBits) / 8; Word++) {
                uint64_t Integer  = (int64_t) Random->EntropyPool;

                uint64_t Seed1    = SecureRNG_BaseSeed(8);
                uint64_t Seed2    = SecureRNG_BaseSeed(8);
                uint64_t Seed3    = SecureRNG_BaseSeed(8);
                uint64_t Seed4    = SecureRNG_BaseSeed(8);

                uint8_t  Rotate1  = SecureRNG_BaseSeed(1) % 7;
                uint8_t  Rotate2  = SecureRNG_BaseSeed(1) % 7;
                uint8_t  Rotate3  = SecureRNG_BaseSeed(1) % 7;
                uint8_t  Rotate4  = SecureRNG_BaseSeed(1) % 7;


                uint64_t Mixed1   = Integer ^ Seed1;
                uint64_t Rotated1 = Rotate(RotationType_Left, Rotate1, Mixed1);

                uint64_t Mixed2   = Rotated1 & Seed2;
                uint64_t Rotated2 = Rotate(RotationType_Left, Rotate2, Mixed2);

                uint64_t Mixed3   = Rotated2 | Seed3;
                uint64_t Rotated3 = Rotate(RotationType_Left, Rotate3, Mixed3);

                uint64_t Mixed4   = Rotated3 | Seed4;
                uint64_t Rotated4 = Rotate(RotationType_Left, Rotate4, Mixed4);

                for (uint8_t Loop = 0; Loop < 8; Loop++) {
                    Random->EntropyPool[Word + Loop] = (Rotated4 & (0xFF << Loop)) >> Loop;
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
    }

    static uint64_t SecureRNG_ExtractBits(SecureRNG *Random, uint8_t NumBits) {
        uint64_t Bits                          = 0ULL;
        if (Random != NULL && NumBits > 0) {
            if (NumBits <= SecureRNG_GetRemainingEntropy(Random)) {
                uint8_t  Bits2Read             = NumBits;

                while (Bits2Read > 0) {
                    uint8_t  BitsInCurrentByte = BitsAvailableInByte(Random->BitOffset);
                    uint8_t  Bits2Get          = (uint8_t) Minimum(BitsInCurrentByte, Bits2Read);
                    uint8_t  BufferShift       = BitsInCurrentByte % 8;
                    uint64_t BufferMask        = (Exponentiate(2, Bits2Get) - 1) << BufferShift;
                    uint64_t BufferOffset      = Bits2Bytes(RoundingType_Down, Random->BitOffset);
                    uint8_t  ExtractedBits     = Random->EntropyPool[BufferOffset] & BufferMask;
                    uint8_t  ValueShift        = NumBits - Bits2Read;
                    Bits                     <<= ValueShift;
                    Bits                      |= ExtractedBits;

                    Bits2Read                 -= Bits2Get;
                    Random->BitOffset         += Bits2Get;
                }
            } else {
                SecureRNG_Erase(Random, 0xFF);
                SecureRNG_Seed(Random);
                //SecureRNG_Mix(Random);
                Bits                           = SecureRNG_ExtractBits(Random, NumBits);
            }
        } else if (Random == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return Bits;
    }

    SecureRNG *SecureRNG_Init(size_t EntropyPoolSize) {
        SecureRNG *Random             = (SecureRNG*) calloc(1, sizeof(SecureRNG));
        if (Random != NULL) {
            size_t PoolSize           = EntropyPoolSize;
            if (EntropyPoolSize % 16 != 0) {
                PoolSize              = EntropyPoolSize + (16 - (EntropyPoolSize % 16));
            }
            
            Random->EntropyPool       = (uint8_t*) calloc(EntropyPoolSize, sizeof(uint8_t));
            if (Random->EntropyPool != NULL) {
                Random->NumBits       = EntropyPoolSize * 8;
                Random->BitOffset     = 0ULL;
                SecureRNG_Seed(Random);
                //SecureRNG_Mix(Random);
            } else {
                SecureRNG_Deinit(Random);
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate EntropyPool"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate SecureRNG"));
        }
        return Random;
    }

    size_t SecureRNG_GetRemainingEntropy(SecureRNG *Random) {
        size_t RemainingBits   = 0;
        if (Random != NULL) {
            RemainingBits      = Random->NumBits - Random->BitOffset;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return RemainingBits;
    }

    uint64_t SecureRNG_GenerateInteger(SecureRNG *Random, uint8_t NumBits) {
        uint64_t GeneratedInteger = 0;
        if (Random != NULL && NumBits >= 1 && NumBits <= 64) {
            GeneratedInteger     = SecureRNG_ExtractBits(Random, NumBits);
        } else if (Random == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        } else if (NumBits > 64) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Generating an integer %u bits long is invalid"), NumBits);
        }
        return GeneratedInteger;
    }

    uint64_t SecureRNG_GenerateIntegerInRange(SecureRNG *Random, int64_t MinValue, int64_t MaxValue) {
        uint64_t RandomInteger                    = 0;
        if (Random != NULL && MinValue <= MaxValue) {
            uint8_t Bits2Read                     = (uint8_t) Exponentiate(2, Subtract(MinValue, MaxValue));
            RandomInteger                         = SecureRNG_ExtractBits(Random, Bits2Read);

            if (RandomInteger < MinValue || RandomInteger > MaxValue) {
                uint8_t NumFixBits                = (uint8_t) Exponentiate(2, Subtract(RandomInteger, MaxValue) + Bits2Read);
                NumFixBits                        = RoundD(NumFixBits / 2);
                uint64_t FixBits                  = SecureRNG_ExtractBits(Random, NumFixBits);
                if (RandomInteger < MinValue) {
                    RandomInteger                += FixBits;
                } else {
                    RandomInteger                -= FixBits;
                }
            }
        } else if (Random == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        } else if (MinValue > MaxValue) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("MinValue %lld is greater than MaxValue %lld"), MinValue, MaxValue);
        }
        return RandomInteger;
    }

    double SecureRNG_GenerateDecimal(SecureRNG *Random) {
        double Decimal        = 0.0;
        if (Random != NULL) {
            int8_t   Sign     = SecureRNG_GenerateInteger(Random, 1) == 1 ? 1 : -1;
            int16_t  Exponent = (int16_t) SecureRNG_GenerateIntegerInRange(Random, -1023, 1023);
            uint64_t Mantissa = SecureRNG_GenerateInteger(Random, 52);

            Decimal           = InsertSignD(Decimal, Sign);
            Decimal           = InsertExponentD(Decimal, Exponent);
            Decimal           = InsertMantissaD(Decimal, Mantissa);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return Decimal;
    }

    uint8_t SecureRNG_Erase(SecureRNG *Random, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Random != NULL) {
            for (size_t Byte = 0ULL; Byte < Bits2Bytes(RoundingType_Down, Random->NumBits); Byte++) {
                Random->EntropyPool[Byte] = NewValue;
            }
            Random->BitOffset             = NewValue;
            Verification = Random->EntropyPool[0] & 0xFF;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return Verification;
    }

    void SecureRNG_Deinit(SecureRNG *Random) {
        if (Random != NULL) {
            free(Random->EntropyPool);
            free(Random);
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
