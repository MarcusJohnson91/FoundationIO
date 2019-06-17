#include "../include/Macros.h"         /* Included for FoundationIO's macros */
#include "../include/CryptographyIO.h" /* Included for our Declarations */

#include "../include/Log.h"            /* Included for error logging */
#include "../include/Math.h"           /* Included for Bits2Bytes, etc */

#if   (FoundationIOTargetOS == FoundationIOWindowsOS)
#include <Windows.h>
#include <BCrypt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum CryptographyIOConstants {
        MD5HashSize              = 128 / 8,
        AES256Alignment          = 256 / 8,
        AES192Alignment          = 192 / 8,
        AES128Alignment          = 128 / 8,
        AES256Rounds             = 14,
        AES192Rounds             = 12,
        AES128Rounds             = 10,
        AESSubstitutionArraySize = 256,
    } CryptographyIOConstants;
    
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
        uint8_t  Data2Hash[64];
        uint32_t Hash[4];       // Contains the current hash; aka Buffer
        uint32_t Bits[2];
    } MD5;
    
    MD5 *MD5_Init(void) {
        MD5 *MD5         = calloc(1, sizeof(MD5));
        if (MD5 != NULL) {
            MD5->Hash[0] = 0x67452301;
            MD5->Hash[1] = 0xEFCDAB89;
            MD5->Hash[2] = 0x98BADCFE;
            MD5->Hash[3] = 0x10325476;
        } else {
            Log(Log_ERROR, __func__, U8("Couldn't allocate MD5"));
        }
        return MD5;
    }
    
    static void MD5_A(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t Shift, uint32_t AC) { // 11/FF
        A += ((B & C) | (~B & D)) + X + AC;
        A  = (uint32_t) RotateLeft(A, Shift);
        A += B;
    }
    
    static void MD5_B(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t Shift, uint32_t AC) { // 22/GG
        A += ((B & D) | (C & ~D)) + X + AC;
        A  = (uint32_t) RotateLeft(A, Shift);
        A += B;
    }
    
    static void MD5_C(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t Shift, uint32_t AC) { // 33/HH
        A += (B ^ C ^ D) + X + AC;
        A  = (uint32_t) RotateLeft(A, Shift);
        A += B;
    }
    
    static void MD5_D(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X, uint32_t Shift, uint32_t AC) { // 44/II
        A += (C ^ (B | ~D)) + X + AC;
        A  = (uint32_t) RotateLeft(A, Shift);
        A += B;
    }
    
    uint32_t **MD5_Packetize(uint8_t *Data, uint64_t NumBytes) { // Returns a array of Packets.
        uint64_t NumBits       = Bytes2Bits(NumBytes);
        uint32_t NumPackets    = NumBits / 448; // NumBytes = 271, 271 * 8 = 2168, 2168 / 448 =
        uint32_t **PacketArray = calloc(NumBytes / 56, sizeof(uint32_t));
        if (Data != NULL && PacketArray != NULL) {
            // Perform Padding and Size appending here as well as breaking it into packets.
            
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
            // So we need to read the data and reorder the bytes.
            // if NumBytes is not a multiple of 56, we need to pad it out to the next 56 bytes.
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
            
#endif
        } else if (PacketArray == NULL) {
            Log(Log_ERROR, __func__, U8("PacketArray Pointer is NULL"));
        } else if (Data == NULL) {
            Log(Log_ERROR, __func__, U8("Data Pointer is NULL"));
        }
        return PacketArray;
    }
    
    void MD5_Process(uint32_t *State, uint32_t *Packet) {
        uint32_t Index1 = 0;
        uint32_t Index2 = 0;
        uint32_t Index3 = 0;
        uint32_t Index4 = 0;
        
        for (uint8_t Round = 0; Round < 64; Round++) {
            uint32_t Index1 = State[MD5ParameterTable[(Round / 4) + 0]];
            uint32_t Index2 = State[MD5ParameterTable[(Round / 4) + 1]];
            uint32_t Index3 = State[MD5ParameterTable[(Round / 4) + 2]];
            uint32_t Index4 = State[MD5ParameterTable[(Round / 4) + 3]];
            
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
        UTF8 *Hash = calloc(MD5HashSize * 2, sizeof(UTF8));
        if (MD5 != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("MD5 Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("Hash1 Pointer is NULL"));
        } else if (Hash2 == NULL) {
            Log(Log_ERROR, __func__, U8("Hash2 Pointer is NULL"));
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
    
    static void Entropy_Seed(Entropy *Random) {
        if (Random != NULL) {
            if (Random->EntropyPool != NULL) {
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
                arc4random_buf(Random->EntropyPool, Random->EntropySize);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                NTSTATUS Status           = BCryptGenRandom(NULL, Random->EntropyPool, Random->EntropySize, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
                if (Status != STATUS_SUCCESS) {
                    Log(Log_ERROR, __func__, U8("Failed to read random data, Enropy is extremely insecure, aborting"));
                    abort();
                }
#else
                FILE *RandomFile          = fopen("/dev/urandom", "rb");
                size_t BytesRead          = fread(Random->EntropyPool, Random->EntropySize, 1, RandomFile);
                if (BytesRead != Random->EntropySize) {
                    Log(Log_ERROR, __func__, U8("Failed to read random data, Enropy is extremely insecure, aborting"));
                    abort();
                }
                fclose(RandomFile);
#endif
            }
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    static void Entropy_Mix(Entropy *Random) {
        if (Random != NULL) {
            uint8_t *Bytes        = calloc(1, sizeof(uint64_t));
            
            for (uint64_t Byte1 = 0ULL; Byte1 < (Random->EntropySize - 1) / 8; Byte1 += 8) {
                uint64_t Integer  = GetIntegerFromBytes(&Random->EntropyPool[Byte1]);
                
                uint64_t Mixed1   = Integer ^ Entropy_Seed1;
                uint64_t Rotated1 = RotateLeft(Mixed1, Entropy_Rotate1);
                
                uint64_t Mixed2   = Rotated1 & Entropy_Seed2;
                uint64_t Rotated2 = RotateLeft(Mixed2, Entropy_Rotate2);
                
                uint64_t Mixed3   = Rotated2 | Entropy_Seed3;
                uint64_t Rotated3 = RotateLeft(Mixed3, Entropy_Rotate3);
                
                uint64_t Mixed4   = Rotated3 | Entropy_Seed4;
                uint64_t Rotated4 = RotateLeft(Mixed4, Entropy_Rotate4);
                
                GetBytesFromInteger(Rotated4, Bytes);
                for (uint8_t Byte2 = 0; Byte2 < 8; Byte2++) {
                    Random->EntropyPool[Byte1 + Byte2] = Bytes[Byte1 + Byte2];
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    static uint64_t Entropy_ExtractBits(Entropy *Random, uint8_t NumBits) {
        uint64_t Bits                                 = 0ULL;
        if (Random != NULL) {
            if (NumBits < Bytes2Bits(Random->EntropySize) - Random->BitOffset) {
                uint64_t Bits2Read                    = NumBits;
                do {
                    uint64_t EntropyByte              = Bits2Bytes(Random->BitOffset, No);
                    uint8_t  BitsInEntropyByte        = 8 - (Random->BitOffset % 8);
                    uint8_t  Bits2ReadFromEntropyByte = 8 - (NumBits % 8);
                    uint8_t  Bits2Get                 = Minimum(BitsInEntropyByte, Bits2ReadFromEntropyByte);
                    Bits                            <<= Bits2Get;
#if  (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                    Bits                              = Random->EntropyPool[EntropyByte] << CreateBitMaskLSBit(Bits2Get);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                    Bits                              = Random->EntropyPool[EntropyByte] >> CreateBitMaskMSBit(Bits2Get);
#endif
                    Bits2Read                        -= Bits2Get;
                } while (Bits2Read > 0);
                Random->BitOffset                    += Bits2Read;
            } else {
                Entropy_Erase(Random);
                Entropy_Seed(Random);
                Entropy_Mix(Random);
                Bits = Entropy_ExtractBits(Random, NumBits);
            }
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
        return Bits;
    }
    
    Entropy *Entropy_Init(uint64_t EntropyPoolSize) {
        Entropy *Random               = calloc(1, sizeof(Entropy));
        if (Random != NULL) {
            Random->EntropyPool       = calloc(EntropyPoolSize, sizeof(uint8_t));
            if (Random->EntropyPool != NULL) {
                Random->EntropySize   = EntropyPoolSize;
                Random->BitOffset     = 0ULL;
                Entropy_Seed(Random);
                Entropy_Mix(Random);
            } else {
                Entropy_Deinit(Random);
                Log(Log_ERROR, __func__, U8("Couldn't allocate EntropyPool"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("Couldn't allocate Entropy"));
        }
        return Random;
    }
    
    uint64_t Entropy_GetRemainingEntropy(Entropy *Random) {
        uint64_t RemainingBits = 0ULL;
        if (Random != NULL) {
            RemainingBits      = Random->EntropySize - Random->BitOffset;
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
        return RemainingBits;
    }
    
    void Entropy_Erase(Entropy *Random) {
        if (Random != NULL) {
            for (uint64_t Byte = 0ULL; Byte < Random->EntropySize - 1; Byte++) {
                Random->EntropyPool[Byte] = 0;
            }
            Random->BitOffset             = 0;
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    int64_t Entropy_GenerateIntegerInRange(Entropy *Random, int64_t MinValue, int64_t MaxValue) {
        int64_t RandomInteger                     = 0ULL;
        if (Random != NULL) {
            uint8_t Bits2Read                     = CeilD(Logarithm(2, Absolute(MaxValue) - Absolute(MinValue)));
            int64_t GeneratedValue                = (int64_t) Entropy_ExtractBits(Random, Bits2Read);
            
            if (GeneratedValue < MinValue || GeneratedValue > MaxValue) {
                uint8_t NumFixBits                = (CeilD(Logarithm(2, Maximum(GeneratedValue, MaxValue) - Minimum(GeneratedValue, MaxValue))) + Bits2Read);
                NumFixBits                        = RoundD(NumFixBits / 2);
                uint64_t FixBits                  = Entropy_ExtractBits(Random, NumFixBits);
                if (GeneratedValue < MinValue) {
                    GeneratedValue               += FixBits;
                } else {
                    GeneratedValue               -= FixBits;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
        return RandomInteger;
    }
    
    void Entropy_Deinit(Entropy *Random) {
        if (Random != NULL) {
            free(Random->EntropyPool);
            free(Random);
        }
    }
    
#ifdef __cplusplus
}
#endif
