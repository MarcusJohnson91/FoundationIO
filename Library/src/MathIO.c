#include "../include/Macros.h"
#include "../include/Math.h"           /* Included for our declarations */

#ifdef __cplusplus
extern "C" {
#endif
    
    bool IsNegative(const int64_t Integer) {
        if (Integer >= 0 && ~Integer >= 0) {
            return No;
        } else {
            return Yes;
        }
    }
    
    bool IsOdd(const int64_t Number2Check) {
        bool X = No;
        if (Number2Check % 2 == 0) {
            X = Yes;
        }
        return X;
    }
    
    uint64_t Absolute(const int64_t Value) {
        return (Value >= 0 ? (uint64_t) Value : (uint64_t) ~Value + 1);
    }
    
    uint64_t Exponentiate(const uint64_t Base, const uint64_t Exponent) {
        uint64_t Result = 1ULL;
        for (uint64_t i = 0ULL; i < Exponent; i++) {
            Result *= Base;
        }
        return Result;
    }
    
    int64_t  Logarithm(int64_t Base, int64_t Exponent) {
        int8_t Bits       = 0;
        if (Exponent == 0) {
            Bits          = 1;
        } else {
            while (Exponent > 0) {
                Bits     += 1;
                Exponent /= Base;
            }
        }
        return Bits;
    }
    
    uint16_t SwapEndian16(const uint16_t Value2Swap) {
        return ((Value2Swap & 0xFF00) >> 8) | ((Value2Swap & 0x00FF) << 8);
    }
    
    uint32_t SwapEndian32(const uint32_t Value2Swap) {
        return ((Value2Swap & 0xFF000000) >> 24) | ((Value2Swap & 0x00FF0000) >> 8) | ((Value2Swap & 0x0000FF00) << 8) | ((Value2Swap & 0x000000FF) << 24);
    }
    
    uint64_t SwapEndian64(const uint64_t Value2Swap) {
        return (((Value2Swap & 0xFF00000000000000) >> 56) | ((Value2Swap & 0x00FF000000000000) >> 40) | \
                ((Value2Swap & 0x0000FF0000000000) >> 24) | ((Value2Swap & 0x000000FF00000000) >>  8) | \
                ((Value2Swap & 0x00000000FF000000) <<  8) | ((Value2Swap & 0x0000000000FF0000) << 24) | \
                ((Value2Swap & 0x000000000000FF00) << 40) | ((Value2Swap & 0x00000000000000FF) << 56));
    }
    
    int64_t  Bytes2Bits(const int64_t Bytes) {
        return Bytes * 8;
    }
    
    int64_t  Bits2Bytes(const int64_t Bits, const bool RoundUp) {
        int64_t Bytes = 0ULL;
        if (RoundUp == No) {
            Bytes = Bits / 8;
        } else {
            Bytes = (Bits / 8) + (8 - (Bits % 8));
        }
        return Bytes;
    }
    
    uint8_t  SwapBits(const uint8_t Byte) {
        return ((Byte & 0x80 >> 7)|(Byte & 0x40 >> 5)|(Byte & 0x20 >> 3)|(Byte & 0x10 >> 1)|(Byte & 0x8 << 1)|(Byte & 0x4 << 3)|(Byte & 0x2 << 5)|(Byte & 0x1 << 7));
    }
    
    uint8_t  CreateBitMaskLSBit(const uint8_t Bits2Extract) {
        return (uint8_t) Exponentiate(2, Bits2Extract) << (8 - Bits2Extract);
    }
    
    uint8_t  CreateBitMaskMSBit(const uint8_t Bits2Extract) {
        return (uint8_t) Exponentiate(2, Bits2Extract) >> (8 - Bits2Extract);
    }
    
    int8_t   ExtractSignFromDecimal(double Number2Extract) {
        uint64_t *Sign1 = (uint64_t *) &Number2Extract;
        uint64_t  Sign2 = *Sign1 >> 63;
        return Sign2 == 1 ? -1 : 1;
    }
    
    int16_t  ExtractExponentFromDecimal(double Number2Extract) {
        int8_t    Sign      = ExtractSignFromDecimal(Number2Extract);
        uint64_t *Exponent1 = (uint64_t *) &Number2Extract;
        int16_t   Exponent2 = *Exponent1 >> 52;
        int16_t   Exponent3 = 0;
        if (Sign == 1) {
            Exponent3       = Exponent2 - 1022;
        } else if (Sign == -1) {
            Exponent3       = Exponent2 - 3070;
        }
        return Exponent3;
    }
    
    int64_t  ExtractMantissaFromDecimal(double Number2Extract) {
        uint64_t *Mantissa1  = (uint64_t *) &Number2Extract;
        uint64_t  Mantissa2  = *Mantissa1 & 0xFFFFFFFFFFFFFULL;
        return Mantissa2;
    }
    
#ifdef __cplusplus
}
#endif
