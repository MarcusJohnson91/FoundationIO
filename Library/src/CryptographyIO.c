#include "../include/Macros.h"         /* Included for FoundationIO's macros */
#include "../include/CryptographyIO.h" /* Included for our Declarations */

#include "../include/Log.h"            /* Included for error logging */
#include "../include/Math.h"           /* Included for Bits2Bytes, etc */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum CryptographyIOConstants {
        MD5HashSize = 16,
    } CryptographyIOConstants;
    
    static uint32_t MD5_1(uint32_t X, uint32_t Y, uint32_t Z) { // F
        return Z ^ (X & (Y ^ Z));
    }
    
    static uint32_t MD5_2(uint32_t X, uint32_t Y, uint32_t Z) { // G
        return Y ^ (Z & (X ^ Y));
    }
    
    static uint32_t MD5_3(uint32_t X, uint32_t Y, uint32_t Z) { // H
        return X ^ Y ^ Z;
    }
    
    static uint32_t MD5_4(uint32_t X, uint32_t Y, uint32_t Z) { // I
        return Y ^ (X | ~Z);
    }
    
    static uint32_t MD5_Step(uint32_t F, uint32_t W, uint32_t X, uint32_t Y, uint32_t Z, uint32_t Data, uint8_t Size) {
        return ((W += MD5_1(X, Y, Z) + Data), W = W << Size | W >> (32 - Size), W += X);
    }
    
    static void MD5_Transform(uint32_t Buf[4], uint32_t const In[16]) {
        uint32_t A = Buf[0];
        uint32_t B = Buf[1];
        uint32_t C = Buf[2];
        uint32_t D = Buf[3];
        
        MD5_Step(MD5_1, A, B, C, D, In[0] + 0xd76aa478, 7);
        MD5_Step(MD5_1, D, A, B, C, In[1] + 0xe8c7b756, 12);
        MD5_Step(MD5_1, C, D, A, B, In[2] + 0x242070dB, 17);
        MD5_Step(MD5_1, B, C, D, A, In[3] + 0xc1bdceee, 22);
        MD5_Step(MD5_1, A, B, C, D, In[4] + 0xf57c0faf, 7);
        MD5_Step(MD5_1, D, A, B, C, In[5] + 0x4787c62A, 12);
        MD5_Step(MD5_1, C, D, A, B, In[6] + 0xa8304613, 17);
        MD5_Step(MD5_1, B, C, D, A, In[7] + 0xfd469501, 22);
        MD5_Step(MD5_1, A, B, C, D, In[8] + 0x698098d8, 7);
        MD5_Step(MD5_1, D, A, B, C, In[9] + 0x8b44f7af, 12);
        MD5_Step(MD5_1, C, D, A, B, In[10] + 0xffff5bb1, 17);
        MD5_Step(MD5_1, B, C, D, A, In[11] + 0x895cd7be, 22);
        MD5_Step(MD5_1, A, B, C, D, In[12] + 0x6b901122, 7);
        MD5_Step(MD5_1, D, A, B, C, In[13] + 0xfd987193, 12);
        MD5_Step(MD5_1, C, D, A, B, In[14] + 0xa679438e, 17);
        MD5_Step(MD5_1, B, C, D, A, In[15] + 0x49b40821, 22);
        
        MD5_Step(MD5_2, A, B, C, D, In[1] + 0xf61e2562, 5);
        MD5_Step(MD5_2, D, A, B, C, In[6] + 0xc040b340, 9);
        MD5_Step(MD5_2, C, D, A, B, In[11] + 0x265e5a51, 14);
        MD5_Step(MD5_2, B, C, D, A, In[0] + 0xe9b6c7aA, 20);
        MD5_Step(MD5_2, A, B, C, D, In[5] + 0xd62f105d, 5);
        MD5_Step(MD5_2, D, A, B, C, In[10] + 0x02441453, 9);
        MD5_Step(MD5_2, C, D, A, B, In[15] + 0xd8a1e681, 14);
        MD5_Step(MD5_2, B, C, D, A, In[4] + 0xe7d3fbc8, 20);
        MD5_Step(MD5_2, A, B, C, D, In[9] + 0x21e1cde6, 5);
        MD5_Step(MD5_2, D, A, B, C, In[14] + 0xc33707d6, 9);
        MD5_Step(MD5_2, C, D, A, B, In[3] + 0xf4d50d87, 14);
        MD5_Step(MD5_2, B, C, D, A, In[8] + 0x455a14eD, 20);
        MD5_Step(MD5_2, A, B, C, D, In[13] + 0xa9e3e905, 5);
        MD5_Step(MD5_2, D, A, B, C, In[2] + 0xfcefa3f8, 9);
        MD5_Step(MD5_2, C, D, A, B, In[7] + 0x676f02d9, 14);
        MD5_Step(MD5_2, B, C, D, A, In[12] + 0x8d2a4c8A, 20);
        
        MD5_Step(MD5_3, A, B, C, D, In[5] + 0xfffa3942, 4);
        MD5_Step(MD5_3, D, A, B, C, In[8] + 0x8771f681, 11);
        MD5_Step(MD5_3, C, D, A, B, In[11] + 0x6d9d6122, 16);
        MD5_Step(MD5_3, B, C, D, A, In[14] + 0xfde5380C, 23);
        MD5_Step(MD5_3, A, B, C, D, In[1] + 0xa4beea44, 4);
        MD5_Step(MD5_3, D, A, B, C, In[4] + 0x4bdecfa9, 11);
        MD5_Step(MD5_3, C, D, A, B, In[7] + 0xf6bb4b60, 16);
        MD5_Step(MD5_3, B, C, D, A, In[10] + 0xbebfbc70, 23);
        MD5_Step(MD5_3, A, B, C, D, In[13] + 0x289b7ec6, 4);
        MD5_Step(MD5_3, D, A, B, C, In[0] + 0xeaa127fA, 11);
        MD5_Step(MD5_3, C, D, A, B, In[3] + 0xd4ef3085, 16);
        MD5_Step(MD5_3, B, C, D, A, In[6] + 0x04881d05, 23);
        MD5_Step(MD5_3, A, B, C, D, In[9] + 0xd9d4d039, 4);
        MD5_Step(MD5_3, D, A, B, C, In[12] + 0xe6db99e5, 11);
        MD5_Step(MD5_3, C, D, A, B, In[15] + 0x1fa27cf8, 16);
        MD5_Step(MD5_3, B, C, D, A, In[2] + 0xc4ac5665, 23);
        
        MD5_Step(MD5_4, A, B, C, D, In[0] + 0xf4292244, 6);
        MD5_Step(MD5_4, D, A, B, C, In[7] + 0x432aff97, 10);
        MD5_Step(MD5_4, C, D, A, B, In[14] + 0xab9423a7, 15);
        MD5_Step(MD5_4, B, C, D, A, In[5] + 0xfc93a039, 21);
        MD5_Step(MD5_4, A, B, C, D, In[12] + 0x655b59c3, 6);
        MD5_Step(MD5_4, D, A, B, C, In[3] + 0x8f0ccc92, 10);
        MD5_Step(MD5_4, C, D, A, B, In[10] + 0xffeff47d, 15);
        MD5_Step(MD5_4, B, C, D, A, In[1] + 0x85845dd1, 21);
        MD5_Step(MD5_4, A, B, C, D, In[8] + 0x6fa87e4f, 6);
        MD5_Step(MD5_4, D, A, B, C, In[15] + 0xfe2ce6e0, 10);
        MD5_Step(MD5_4, C, D, A, B, In[6] + 0xa3014314, 15);
        MD5_Step(MD5_4, B, C, D, A, In[13] + 0x4e0811a1, 21);
        MD5_Step(MD5_4, A, B, C, D, In[4] + 0xf7537e82, 6);
        MD5_Step(MD5_4, D, A, B, C, In[11] + 0xbd3af235, 10);
        MD5_Step(MD5_4, C, D, A, B, In[2] + 0x2ad7d2bB, 15);
        MD5_Step(MD5_4, B, C, D, A, In[9] + 0xeb86d391, 21);
        
        Buf[0] += A;
        Buf[1] += B;
        Buf[2] += C;
        Buf[3] += D;
    }
    
    typedef struct MD5 {
        uint8_t  Input[64];
        uint32_t Buffer[4];
        uint32_t Bits[2];
    } MD5;
    
    MD5 *MD5_Init(void) {
        MD5 *MD5 = calloc(1, sizeof(MD5));
        if (MD5 != NULL) {
            MD5->Buffer[0] = 0x67452301;
            MD5->Buffer[1] = 0xEFCDAB89;
            MD5->Buffer[2] = 0x98BADCFE;
            MD5->Buffer[3] = 0x10325476;
        } else {
            Log(Log_ERROR, __func__, U8("Couldn't allocate MD5"));
        }
        return MD5;
    }
    
    uint8_t *MD5_Update(MD5 *MD5, uint8_t *Data2Hash, uint64_t Data2HashSize) {
        /* Start MD5Update */
        
        /* Update bitcount */
        uint32_t NumBits = MD5->Bits[0];
        if ((MD5->Bits[0] = NumBits + Bytes2Bits(Data2HashSize)) < NumBits) { // if Bits + Data2HashSizeInBits <  Bits
            MD5->Bits[1] += 1;     /* Carry from low to high */
        }
        MD5->Bits[1] += Data2HashSize >> 29;
        
        NumBits = (NumBits >> 3) & 0x3f;    /* Bytes already in shsInfo->data */
        
        /* Handle any leading odd-sized chunks */
        
        if (NumBits) {
            uint8_t *p = &MD5->Input[Bits2Bytes(NumBits, No)];
            
            NumBits -= 64;
            if (Data2HashSize < NumBits) {
                memcpy(p, Data2Hash, Data2HashSize);
            }
            memcpy(p, Data2Hash, NumBits);
            SwapEndian32(MD5->Input); // 16 bytes
            MD5_Transform(MD5->Buffer, MD5->Input);
            Data2Hash += NumBits;
            Data2HashSize -= NumBits;
        }
        /* Process data in 64-byte chunks */
        
        while (Data2HashSize >= 64) {
            memcpy(MD5->Input, Data2Hash, 64);
            SwapEndian32(MD5->Input); // 16 bytes
            MD5_Transform(MD5->Buffer, MD5->Input);
            Data2Hash += 64;
            Data2HashSize -= 64;
        }
        
        /* Handle any remaining bytes of data. */
        
        memcpy(MD5->Input, Data2Hash, Data2HashSize);
        /* End MD5Update */
        
        
        
        
        
        
        
        
        
        
        
        uint8_t *Hash = calloc(MD5HashSize, sizeof(uint8_t));
        if (Hash != NULL) {
            for (uint64_t Byte = 0ULL; Byte < Data2HashSize - 1; Byte++) {
                // Loop over the Data2Hash array, doing what exactly?
            }
        } else {
            Log(Log_ERROR, __func__, U8("Couldn't Allocate Hash"));
        }
        return Hash;
    }
    
    bool MD5_Compare(uint8_t *Hash1, uint8_t *Hash2) {
        bool HashesMatch        = Yes;
        if (Hash1 != NULL && Hash2 != NULL) {
            for (uint8_t Byte = 0; Byte < MD5HashSize; Byte++) {
                if (Hash1[Byte] != Hash2[Byte]) {
                    HashesMatch = No;
                }
            }
        } else if (Hash1 == NULL) {
            Log(Log_ERROR, __func__, U8("Hash1 Pointer is NULL"));
        } else if (Hash2 == NULL) {
            Log(Log_ERROR, __func__, U8("Hash2 Pointer is NULL"));
        } else {
            HashesMatch         = No;
        }
        return HashesMatch;
    }
    
    static const uint8_t AESSubstitutionArray[256] = { // aka S-Box
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
    
    void SHA3() {
        
    }
    
#ifdef __cplusplus
}
#endif
