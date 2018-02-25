#include <stdint.h>                    /* Included for u/intX_t */

#include "../include/Macros.h"
#include "../include/Math.h"

#ifdef   __cplusplus
extern   "C" {
#endif
    
    bool     IsNegative(const int64_t Integer) {
        if (Integer >= 0 && ~Integer >= 0) {
            return No;
        } else {
            return Yes;
        }
    }
    
    bool     IsOdd(const int64_t Number2Check) {
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
    
#ifdef   __cplusplus
}
#endif
