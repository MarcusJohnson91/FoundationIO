#include "../include/BitIO.h"                /* Included for BitBuffer for CRC32 and Adler32 */
#include "../include/Constants.h"            /* Included for BitMaskTables */
#include "../include/CryptographyIO.h"       /* Included for our declarations */
#include "../include/MathIO.h"               /* Included for Bits2Bytes, etc */
#include "../include/UnicodeIO/LogIO.h"      /* Included for error logging */

#if   (FoundationIOTargetOS == FoundationIOWindowsOS)
#include <BCrypt.h>
#endif

#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
extern "C" {
#endif
    
    typedef enum CryptographyIO_Constants {
        MD5HashSize              = 128 / 8,
        AESTableSize             = 256,
        AESBlockSizeInBytes      = 16,
        AES256Rounds             = 14,
        AES192Rounds             = 12,
        AES128Rounds             = 10,
        AESSubstitutionArraySize = 256,
    } CryptographyIO_Constants;
    
    static const uint8_t MD5BlockIndexTable[64] = {
        0,  1,  2,  3,  4,  5,  6,  7,
        8,  9, 10, 11, 12, 13, 14, 15,
        1,  6, 11,  0,  5, 10, 15,  4,
        9, 14,  3,  8, 13,  2,  7, 12,
        5,  8, 11, 14,  1,  4,  7, 10,
        13, 0,  3,  6,  9, 12, 15,  2,
        0,  7, 14,  5, 12,  3, 10,  1,
        8, 15,  6, 13,  4, 11,  2,  9
    };
    
    static const uint8_t MD5ShiftTable[64] = {
        7, 12, 17, 22, 7, 12, 17, 22,
        7, 12, 17, 22, 7, 12, 17, 22,
        5,  9, 14, 20, 5,  9, 14, 20,
        5,  9, 14, 20, 5,  9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23,
        4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21,
        6, 10, 15, 21, 6, 10, 15, 21
    };
    
    static const uint8_t MD5ParameterTable[64] = { // A, B, C, D
        0, 1, 2, 3, 3, 0, 1, 2,
        2, 3, 0, 1, 1, 2, 3, 0,
        0, 1, 2, 3, 3, 0, 1, 2,
        2, 3, 0, 1, 1, 2, 3, 0,
        0, 1, 2, 3, 3, 0, 1, 2,
        2, 3, 0, 1, 1, 2, 3, 0,
        0, 1, 2, 3, 3, 0, 1, 2,
        2, 3, 0, 1, 1, 2, 3, 0,
    };
    
    static const uint32_t MD5GeneratedConstants[64] = { // for (int i = 1; i < 65; i++) {printf("0x%08X,\n", (uint32_t) fabs(sin(Integer) * pow(2, 32)));}
        0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
        0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
        0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
        0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
        0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
        0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
        0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
        0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
        0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
        0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
        0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
        0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
        0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
        0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
        0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
        0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391,
    };
    
    typedef struct MD5 {
        uint64_t FileSize;
        uint32_t Hash[4];
        uint32_t Bits[2];
        uint8_t  Data2Hash[64];
    } MD5;
    
    MD5 *MD5_Init(void) {
        MD5 *Hash         = (MD5*) calloc(1, sizeof(MD5));
        if (Hash != NULL) {
            Hash->Hash[0] = 0x67452301;
            Hash->Hash[1] = 0xEFCDAB89;
            Hash->Hash[2] = 0x98BADCFE;
            Hash->Hash[3] = 0x10325476;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate MD5"));
        }
        return Hash;
    }
    
    static void MD5_A(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint8_t Shift, uint32_t AC) {
        A += ((B & C) | (~B & D)) + X + AC;
        A  = (uint32_t) Rotate(A, Shift, Rotate_Left);
        A += B;
    }
    
    static void MD5_B(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint8_t Shift, uint32_t AC) {
        A += ((B & D) | (C & ~D)) + X + AC;
        A  = (uint32_t) Rotate(A, Shift, Rotate_Left);
        A += B;
    }
    
    static void MD5_C(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint8_t Shift, uint32_t AC) {
        A += (B ^ C ^ D) + X + AC;
        A  = (uint32_t) Rotate(A, Shift, Rotate_Left);
        A += B;
    }
    
    static void MD5_D(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint8_t Shift, uint32_t AC) {
        A += (C ^ (B | ~D)) + X + AC;
        A  = (uint32_t) Rotate(A, Shift, Rotate_Left);
        A += B;
    }
    
    void MD5_Process(uint32_t *State, uint32_t *Packet) {
        uint32_t Index1 = 0;
        uint32_t Index2 = 0;
        uint32_t Index3 = 0;
        uint32_t Index4 = 0;
        
        for (uint8_t Round = 0; Round < 64; Round++) {
            Index1      = State[MD5ParameterTable[(Round / 4) + 0]];
            Index2      = State[MD5ParameterTable[(Round / 4) + 1]];
            Index3      = State[MD5ParameterTable[(Round / 4) + 2]];
            Index4      = State[MD5ParameterTable[(Round / 4) + 3]];
            
            if (Round < 16) {
                MD5_A(Index1, Index2, Index3, Index4, Packet[MD5BlockIndexTable[Round]], MD5ShiftTable[Round], MD5GeneratedConstants[Round]);
            } else if (Round < 32) {
                MD5_B(Index1, Index2, Index3, Index4, Packet[MD5BlockIndexTable[Round]], MD5ShiftTable[Round], MD5GeneratedConstants[Round]);
            } else if (Round < 48) {
                MD5_C(Index1, Index2, Index3, Index4, Packet[MD5BlockIndexTable[Round]], MD5ShiftTable[Round], MD5GeneratedConstants[Round]);
            } else if (Round < 64) {
                MD5_D(Index1, Index2, Index3, Index4, Packet[MD5BlockIndexTable[Round]], MD5ShiftTable[Round], MD5GeneratedConstants[Round]);
            }
        }
        
        State[0] += Index1;
        State[1] += Index2;
        State[2] += Index3;
        State[3] += Index4;
    }
    
    UTF8 *MD5_Finalize(MD5 *MD5) {
        UTF8 *Hash = (UTF8*) calloc(MD5HashSize * 2, sizeof(UTF8));
        if (MD5 != NULL) {
            
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("MD5 Pointer is NULL"));
        }
        return Hash;
    }
    
    bool MD5_Compare(uint8_t *Hash1, uint8_t *Hash2) {
        bool HashesMatch            = No;
        if (Hash1 != NULL && Hash2 != NULL) {
            for (uint8_t Byte = 0; Byte < MD5HashSize; Byte++) {
                while (Hash1[Byte] == Hash2[Byte]) {
                    if (Byte == MD5HashSize - 1) {
                        HashesMatch = Yes;
                    }
                }
            }
        } else if (Hash1 == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Hash1 Pointer is NULL"));
        } else if (Hash2 == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Hash2 Pointer is NULL"));
        }
        return HashesMatch;
    }
    
    void MD5_Deinit(MD5 *MD5) {
        free(MD5);
    }
    
    typedef struct Entropy {
        uint8_t  *EntropyPool;
        uint64_t  EntropySize;
        uint64_t  BitOffset;
    } Entropy;
    
    static int64_t Entropy_BaseSeed(uint8_t NumBytes) {
        int64_t RandomValue = 0LL;
        if (NumBytes <= 8) {
#if   (((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS) && (((FoundationIOTargetOS & FoundationIOBSDOS) == FoundationIOBSDOS) || ((FoundationIOTargetOS & FoundationIOAppleOS) == FoundationIOAppleOS)))
            arc4random_buf(&RandomValue, NumBytes);
#elif ((FoundationIOTargetOS & FoundationIOLinuxOS) == FoundationIOLinuxOS)
            #include <linux/random.h>
            getrandom(&RandomValue, NumBytes, GRND_BLOCK);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            BCryptGenRandom(NULL, &RandomValue, NumBytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#else
            FILE *RandomFile          = FoundationIO_File_Open(UTF8String("/dev/urandom"), UTF8String("rb"));
            size_t BytesRead          = fread(&RandomValue, NumBytes, 1, RandomFile);
            FoundationIO_File_Close(RandomFile);
#endif
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Can't return more than 8 bytes"));
        }
        return RandomValue;
    }
    
    static void Entropy_Seed(Entropy *Random) {
        if (Random != NULL) {
            if (Random->EntropyPool != NULL) {
#if   (((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS) && ((FoundationIOTargetOS & FoundationIOLinuxOS) != FoundationIOLinuxOS))
                arc4random_buf(Random->EntropyPool, Random->EntropySize);
#elif (((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS) && ((FoundationIOTargetOS & FoundationIOLinuxOS) == FoundationIOLinuxOS))
                FILE *RandomFile          = FoundationIO_File_Open(UTF8String("/dev/urandom"), UTF8String("rb"));
                size_t BytesRead          = fread(Random->EntropyPool, Random->EntropySize, 1, RandomFile);
                if (BytesRead != Random->EntropySize) {
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Failed to read random data, Entropy is extremely insecure, aborting"));
                }
                FoundationIO_File_Close(RandomFile);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                NTSTATUS Status           = BCryptGenRandom(NULL, Random->EntropyPool, Random->EntropySize, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
                if (Status <= 0) {
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Failed to read random data, Entropy is extremely insecure, aborting"));
                    abort();
                }
#endif
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        }
    }
    
    static void Entropy_Mix(Entropy *Random) {
        if (Random != NULL) {
            for (uint64_t Word = 0ULL; Word < Random->EntropySize / 8; Word++) {
                uint64_t Integer  = (int64_t) Random->EntropyPool;
                
                uint64_t Seed1    = Entropy_BaseSeed(8);
                uint64_t Seed2    = Entropy_BaseSeed(8);
                uint64_t Seed3    = Entropy_BaseSeed(8);
                uint64_t Seed4    = Entropy_BaseSeed(8);
                
                uint8_t  Rotate1  = Entropy_BaseSeed(1) % 64;
                uint8_t  Rotate2  = Entropy_BaseSeed(1) % 64;
                uint8_t  Rotate3  = Entropy_BaseSeed(1) % 64;
                uint8_t  Rotate4  = Entropy_BaseSeed(1) % 64;
                
                
                uint64_t Mixed1   = Integer ^ Seed1;
                uint64_t Rotated1 = Rotate(Mixed1, Rotate1, Rotate_Left);
                
                uint64_t Mixed2   = Rotated1 & Seed2;
                uint64_t Rotated2 = Rotate(Mixed2, Rotate2, Rotate_Left);
                
                uint64_t Mixed3   = Rotated2 | Seed3;
                uint64_t Rotated3 = Rotate(Mixed3, Rotate3, Rotate_Left);
                
                uint64_t Mixed4   = Rotated3 | Seed4;
                uint64_t Rotated4 = Rotate(Mixed4, Rotate4, Rotate_Left);
                
                Random->EntropyPool[Word + 0] = (Rotated4 & 0xFF00000000000000) >> 56;
                Random->EntropyPool[Word + 1] = (Rotated4 & 0x00FF000000000000) >> 48;
                Random->EntropyPool[Word + 2] = (Rotated4 & 0x0000FF0000000000) >> 40;
                Random->EntropyPool[Word + 3] = (Rotated4 & 0x000000FF00000000) >> 32;
                Random->EntropyPool[Word + 4] = (Rotated4 & 0x00000000FF000000) >> 24;
                Random->EntropyPool[Word + 5] = (Rotated4 & 0x0000000000FF0000) >> 16;
                Random->EntropyPool[Word + 6] = (Rotated4 & 0x000000000000FF00) >> 8;
                Random->EntropyPool[Word + 7] = (Rotated4 & 0x00000000000000FF) >> 0;
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        }
    }
    
    static uint64_t Entropy_ExtractBits(Entropy *Random, uint8_t NumBits) {
        uint64_t Bits                                 = 0ULL;
        if (Random != NULL && NumBits > 0) {
            if (NumBits <= Entropy_GetRemainingEntropy(Random)) {
                uint64_t Bits2Read                    = NumBits;
                
                while (Bits2Read > 0) {
                    uint8_t  BitsInCurrentByte        = Bits2ExtractFromByte(Random->BitOffset);
                    uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInCurrentByte, Bits2Read);
                    uint8_t  Shift                    = BitsInCurrentByte - Bits2Get;
                    uint8_t  BitMask                  = BitMaskTable[Bits2Get - 1] << Shift;
                    uint64_t Byte                     = Bits2Bytes(Random->BitOffset, RoundingType_Down);
                    uint8_t  ExtractedBits            = Random->EntropyPool[Byte] & BitMask;
                    ExtractedBits                   >>= Shift;
                    
                    Bits                            <<= Bits2Get;
                    Bits                             |= ExtractedBits;
                    
                    Bits2Read                        -= Bits2Get;
                    Random->BitOffset                += Bits2Get;
                }
            } else {
                Entropy_Erase(Random, 0xFF);
                Entropy_Seed(Random);
                Entropy_Mix(Random);
                Bits                                  = Entropy_ExtractBits(Random, NumBits);
            }
        } else if (Random == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        } else if (NumBits == 0) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Reading zero bits does not make sense"));
        }
        return Bits;
    }
    
    Entropy *Entropy_Init(uint64_t EntropyPoolSize) {
        Entropy *Random               = (Entropy*) calloc(1, sizeof(Entropy));
        if (Random != NULL) {
            Random->EntropyPool       = (uint8_t*) calloc(EntropyPoolSize, sizeof(uint8_t));
            if (Random->EntropyPool != NULL) {
                Random->EntropySize   = EntropyPoolSize;
                Random->BitOffset     = 0ULL;
                Entropy_Seed(Random);
                Entropy_Mix(Random);
            } else {
                Entropy_Deinit(Random);
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate EntropyPool"));
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate Entropy"));
        }
        return Random;
    }
    
    uint64_t Entropy_GetRemainingEntropy(Entropy *Random) {
        uint64_t RemainingBits = 0ULL;
        if (Random != NULL) {
            RemainingBits      = Random->EntropySize - Random->BitOffset;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        }
        return RemainingBits;
    }
    
    int64_t Entropy_GenerateInteger(Entropy *Random, uint8_t NumBits) {
        int64_t GeneratedInteger = 0LL;
        if (Random != NULL && NumBits >= 1 && NumBits <= 64) {
            GeneratedInteger     = Entropy_ExtractBits(Random, NumBits);
        } else if (Random == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        } else if (NumBits == 0) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Generating an integer 0 bits long is invalid"));
        } else if (NumBits > 64) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Generating an integer %u bits long is invalid"), NumBits);
        }
        return GeneratedInteger;
    }
    
    int64_t Entropy_GenerateIntegerInRange(Entropy *Random, int64_t MinValue, int64_t MaxValue) {
        int64_t RandomInteger                     = 0ULL;
        if (Random != NULL && MinValue <= MaxValue) {
            uint8_t Bits2Read                     = (uint8_t) Logarithm(2, Minimum(MinValue, MaxValue) - Maximum(MinValue, MaxValue));
            RandomInteger                         = Entropy_ExtractBits(Random, Bits2Read);
            
            if (RandomInteger < MinValue || RandomInteger > MaxValue) {
                uint8_t NumFixBits                = (CeilD(Logarithm(2, Maximum(RandomInteger, MaxValue) - Minimum(RandomInteger, MaxValue))) + Bits2Read);
                NumFixBits                        = RoundD(NumFixBits / 2);
                uint64_t FixBits                  = Entropy_ExtractBits(Random, NumFixBits);
                if (RandomInteger < MinValue) {
                    RandomInteger                += FixBits;
                } else {
                    RandomInteger                -= FixBits;
                }
            }
        } else if (Random == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        } else if (MinValue > MaxValue) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("MinValud %lld is greater than MaxValue %lld"), MinValue, MaxValue);
        }
        return RandomInteger;
    }
    
    double Entropy_GenerateDecimal(Entropy *Random) {
        double Decimal        = 0.0;
        if (Random != NULL) {
            int8_t   Sign     = Entropy_GenerateInteger(Random, 1) == 1 ? 1 : -1;
            int16_t  Exponent = Entropy_GenerateIntegerInRange(Random, -1023, 1023);
            uint64_t Mantissa = Entropy_GenerateInteger(Random, 52);
            
            Decimal           = InsertSignD(Decimal, Sign);
            Decimal           = InsertExponentD(Decimal, Exponent);
            Decimal           = InsertMantissaD(Decimal, Mantissa);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        }
        return Decimal;
    }
    
    uint8_t Entropy_Erase(Entropy *Random, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Random != NULL) {
            for (uint64_t Byte = 0ULL; Byte < Random->EntropySize; Byte++) {
                Random->EntropyPool[Byte] = NewValue;
            }
            Random->BitOffset             = NewValue;
            Verification = Random->EntropyPool[0] & 0xFF;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        }
        return Verification;
    }
    
    void Entropy_Deinit(Entropy *Random) {
        if (Random != NULL) {
            free(Random->EntropyPool);
            free(Random);
        }
    }
    
    uint32_t Adler32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t Output = 0;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            uint16_t A = 1;
            uint16_t B = 0;
            
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint8_t Value = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                A = (A + Value) % 65521;
                B = (B + A)     % 65521;
            }
            
            Output = (B << 16) | A;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return Output;
    }
    
    uint32_t CRC32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t Output = -1;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint32_t Polynomial = 0x82608EDB;
                uint8_t  Data       = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                
                Output               ^= Data;
                for (uint8_t Bit = 0; Bit < 8; Bit++) {
                    if (Output & 1) {
                        Output = (Output >> 1) ^ Polynomial;
                    } else {
                        Output >>= 1;
                    }
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return ~Output;
    }
    
    static const uint8_t EncodeAESTable[AESTableSize] = {
        0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
        0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
        0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
        0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
        0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
        0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
        0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
        0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
        0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
        0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
        0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
        0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
        0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
        0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
        0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
        0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
    };
    
    static const uint8_t DecodeAESTable[AESTableSize] = {
        0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
        0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
        0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
        0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
        0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
        0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
        0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
        0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
        0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
        0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
        0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
        0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
        0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
        0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
        0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
    };
    
#ifdef __cplusplus
}
#endif
