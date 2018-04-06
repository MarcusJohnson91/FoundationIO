#include <stdint.h>                    /* Included for u/intX_t */

#include "../include/Math.h"           /* Included for our declarations */
#include "../include/Macros.h"         /* Included for the Yes/No macros */

#ifdef   __cplusplus
extern   "C" {
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
        for (uint64_t i = 0; i < Exponent; i++) {
            Result *= Base;
        }
        return Result;
    }
    
    int64_t Logarithm(int64_t Base, int64_t Exponent) {
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
    
    inline uint16_t SwapEndian16(const uint16_t Value2Swap) {
        return ((Value2Swap & 0xFF00) >> 8) | ((Value2Swap & 0x00FF) << 8);
    }
    
    inline uint32_t SwapEndian32(const uint32_t Value2Swap) {
        return ((Value2Swap & 0xFF000000) >> 24) | ((Value2Swap & 0x00FF0000) >> 8) | ((Value2Swap & 0x0000FF00) << 8) | ((Value2Swap & 0x000000FF) << 24);
    }
    
    inline uint64_t SwapEndian64(const uint64_t Value2Swap) {
        return (((Value2Swap & 0xFF00000000000000) >> 56) | ((Value2Swap & 0x00FF000000000000) >> 40) | \
                ((Value2Swap & 0x0000FF0000000000) >> 24) | ((Value2Swap & 0x000000FF00000000) >>  8) | \
                ((Value2Swap & 0x00000000FF000000) <<  8) | ((Value2Swap & 0x0000000000FF0000) << 24) | \
                ((Value2Swap & 0x000000000000FF00) << 40) | ((Value2Swap & 0x00000000000000FF) << 56));
    }
    
    int64_t Bytes2Bits(const int64_t Bytes) {
        return Bytes * 8;
    }
    
    int64_t Bits2Bytes(const int64_t Bits, const bool RoundUp) {
        int64_t Bytes = 0ULL;
        if (RoundUp == No) {
            Bytes = Bits / 8;
        } else {
            Bytes = (Bits / 8) + (8 - (Bits % 8));
        }
        return Bytes;
    }
    
#ifdef   __cplusplus
}
#endif
