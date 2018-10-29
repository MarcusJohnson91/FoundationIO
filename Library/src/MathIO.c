#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/Math.h"           /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */

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
    
    uint64_t Absolutei(const int64_t Value) {
       return (Value >= 0 ? (uint64_t) Value : (uint64_t) ~Value + 1);
    }
    
    uint64_t Absolutef(const float Value) {
        return (uint64_t) ExtractExponent(Value);
    }
    
    uint64_t Absoluted(const double Value) {
        return (uint64_t) ExtractExponent(Value);
    }
    
    int64_t  Floorf(const float Value) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSign(Value);
        int32_t  Mantissa = ExtractMantissa(Value);
        int16_t  Exponent = ExtractExponent(Value);
        
        if (Mantissa == 0) {
            Result        =  Exponent * Sign;
        } else {
            if (Sign == -1) {
                Result    = (Exponent * Sign) - 1;
            } else {
                Result    =  Exponent * Sign;
            }
        }
        return Result;
    }
    
    int64_t  Floord(const double Value) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSign(Value);
        int64_t  Mantissa = ExtractMantissa(Value);
        int16_t  Exponent = ExtractExponent(Value);
        
        if (Mantissa == 0) {
            Result        =  Exponent * Sign;
        } else {
            if (Sign == -1) {
                Result    = (Exponent * Sign) - 1;
            } else {
                Result    =  Exponent * Sign;
            }
        }
        return Result;
    }
    
    int64_t  Ceilf(const float Value) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSign(Value);
        int32_t  Mantissa = ExtractMantissa(Value);
        int16_t  Exponent = ExtractExponent(Value);
        
        if (Mantissa == 0) {
            Result        =  Exponent * Sign;
        } else {
            if (Sign == -1) {
                Result    = (Exponent * Sign) - 1;
            } else {
                Result    =  Exponent * Sign;
            }
        }
        return Result;
    }
    
    int64_t  Ceild(const double Value) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSign(Value);
        int64_t  Mantissa = ExtractMantissa(Value);
        int16_t  Exponent = ExtractExponent(Value);
        
        if (Mantissa == 0) {
            Result        =  Exponent * Sign;
        } else {
            if (Sign == -1) {
                Result    = (Exponent * Sign) - 1;
            } else {
                Result    =  Exponent * Sign;
            }
        }
        return Result;
    }
    
    uint64_t Exponentiate(const uint64_t Base, const uint64_t Exponent) {
        int64_t Value      = 0;
        int64_t Exponent2  = Exponent;
        if (Base > 0 && Exponent > 0) {
            while (Exponent2 > 0) {
                Value     *= Base;
                Exponent2 /= Base;
            }
        }
        return Value;
    }
    
    int64_t  Logarithm(const uint64_t Base, const int64_t Exponent) {
        uint64_t Result    = 0ULL;
        int64_t  Exponent2 = Exponent;
        if (Base > 1 && Exponent > 0) {
            do {
                Result    += 1;
                Exponent2 -= Base;
            } while (Exponent2 > Base);
        } else if (Base > 1 && Exponent < 0) {
            do {
                Result    += 1;
                Exponent2 += Base;
            } while (Exponent2 < Base);
        }
        return Result;
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
                ((Value2Swap & 0x000000000000FF00) << 40) | ((Value2Swap & 0x00000000000000FF) << 56)
                );
    }
    
    int64_t  Bytes2Bits(const int64_t Bytes) {
        return Bytes * 8;
    }
    
    int64_t  Bits2Bytes(const int64_t Bits, const bool RoundUp) {
        uint64_t AbsoluteBits = Absolutei(Bits);
        int64_t  Bytes        = 0ULL;
        if (RoundUp == Yes) {
            Bytes             = (AbsoluteBits >> 3) + 1;
        } else if (RoundUp == No) {
            Bytes             = (AbsoluteBits >> 3);
        }
        if (Bits < 0) {
            Bytes *= -1;
        }
        return Bytes;
    }
    
    uint8_t  SwapBits(const uint8_t Byte) {
        return (
                (Byte & 0x80 >> 7) |
                (Byte & 0x40 >> 5) |
                (Byte & 0x20 >> 3) |
                (Byte & 0x10 >> 1) |
                (Byte & 0x08 << 1) |
                (Byte & 0x04 << 3) |
                (Byte & 0x02 << 5) |
                (Byte & 0x01 << 7)
                );
    }
    
    uint8_t Bits2ExtractFromByte(uint64_t Offset) {
        return 8 - (Offset % 8);
    }
    
    uint8_t CreateBitMask(const uint8_t NumBits2Select) {
        uint8_t Mask = 0;
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
        Mask = Exponentiate(2, NumBits2Select) >> (8 - NumBits2Select);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
        Mask = Exponentiate(2, NumBits2Select) << (8 - NumBits2Select);
#endif
        return Mask;
    }
    
    int8_t   ExtractSignI(int64_t Number2Extract) {
        return (Number2Extract & 0x8000000000000000) >> 63 == 1 ? -1 : 1;
    }
    
    int8_t   ExtractSignF(float Number2Extract) {
        uint32_t *Sign1 = (uint32_t *) &Number2Extract;
        uint32_t  Sign2 = (*Sign1 & 0x80000000) >> 31;
        return Sign2 == 0 ? 1 : -1;
    }
    
    int8_t   ExtractSignD(double Number2Extract) {
        uint64_t *Sign1 = (uint64_t *) &Number2Extract;
        uint64_t  Sign2 = (*Sign1 & 0x8000000000000000) >> 63;
        return Sign2 == 0 ? 1 : -1;
    }
    
    int16_t  ExtractExponentF(float Number2Extract) {
        int8_t    Sign      = ExtractSign(Number2Extract);
        uint32_t *Exponent1 = (uint32_t *) &Number2Extract;
        int16_t   Exponent2 = (*Exponent1 & 0x7F800000) >> 23;
        int16_t   Exponent3 = 0;
        if (Sign == 1) {
            Exponent3       = Exponent2 - 1022;
        } else if (Sign == -1) {
            Exponent3       = Exponent2 - 3070;
        }
        return Exponent3;
    }
    
    int16_t  ExtractExponentD(double Number2Extract) {
        int8_t    Sign      = ExtractSign(Number2Extract);
        uint64_t *Exponent1 = (uint64_t *) &Number2Extract;
        int16_t   Exponent2 = (*Exponent1 & 0x7FF0000000000000) >> 52;
        int16_t   Exponent3 = 0;
        if (Sign == 1) {
            Exponent3       = Exponent2 - 1022;
        } else if (Sign == -1) {
            Exponent3       = Exponent2 - 3070;
        }
        return Exponent3;
    }
    
    int32_t  ExtractMantissaF(float Number2Extract) {
        uint32_t *Mantissa1  = (uint32_t *) &Number2Extract;
        int32_t   Mantissa2  = *Mantissa1 & 0x7FFFFFULL;
        return (Mantissa2 | 0x800000) - 127;
    }
    
    int64_t  ExtractMantissaD(double Number2Extract) {
        uint64_t *Mantissa1  = (uint64_t *) &Number2Extract;
        uint64_t  Mantissa2  = *Mantissa1 & 0xFFFFFFFFFFFFFULL;
        return (Mantissa2 | 0x10000000000000) - 1023;
    }
    
#ifdef __cplusplus
}
#endif
