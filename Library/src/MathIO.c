#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/Math.h"           /* Included for our declarations */

#ifdef __cplusplus
extern "C" {
#endif
    
    bool IsNegative(int64_t Integer) {
        return (Integer & 0x8000000000000000) >> 63;
    }
    
    bool IsOdd(int64_t Integer) {
        return Integer & 1;
    }
    
    uint64_t AbsoluteI(int64_t Integer) {
        bool Sign = Integer >> 63;
        return (Integer ^ -Sign) + Sign;
    }
    
    uint8_t AbsoluteF(float Decimal) {
        return (uint8_t) ExtractExponentF(Decimal);
    }
    
    uint16_t AbsoluteD(double Decimal) {
        return (uint16_t) ExtractExponentD(Decimal);
    }
    
    int64_t  FloorF(float Decimal) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSignF(Decimal);
        int8_t   Exponent = ExtractExponentF(Decimal);
        int32_t  Mantissa = ExtractMantissaF(Decimal);
        
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
    
    int64_t  FloorD(double Decimal) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSignD(Decimal);
        int16_t  Exponent = ExtractExponentD(Decimal);
        int64_t  Mantissa = ExtractMantissaD(Decimal);
        
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
    
    int64_t  CeilF(float Decimal) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSignF(Decimal);
        int8_t   Exponent = ExtractExponentF(Decimal);
        int32_t  Mantissa = ExtractMantissaF(Decimal);
        
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
    
    int64_t  CeilD(double Decimal) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSignD(Decimal);
        int16_t  Exponent = ExtractExponentD(Decimal);
        int64_t  Mantissa = ExtractMantissaD(Decimal);
        
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
    
    int64_t  RoundF(float Decimal) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSignF(Decimal);
        int8_t   Exponent = ExtractExponentF(Decimal);
        int64_t  Mantissa = ExtractMantissaF(Decimal);
        if (Mantissa >= 4096) { // 0.5
            Result        = Exponent + 1;
        } else {
            Result        = Exponent;
        }
        return Result * Sign;
    }
    
    int64_t  RoundD(double Decimal) {
        int64_t  Result   = 0;
        int8_t   Sign     = ExtractSignD(Decimal);
        int16_t  Exponent = ExtractExponentD(Decimal);
        int64_t  Mantissa = ExtractMantissaD(Decimal);
        if (Mantissa >= 4096) { // 0.5
            Result        = Exponent + 1;
        } else {
            Result        = Exponent;
        }
        return Result * Sign;
    }
    
    int64_t  Min(int64_t Integer1, int64_t Integer2) {
        return Integer2 ^ ((Integer1 ^ Integer2) & -(Integer1 < Integer2));
    }
    
    int64_t  Max(int64_t Integer1, int64_t Integer2) {
        return Integer1 ^ ((Integer1 ^ Integer2) & -(Integer1 < Integer2));
    }
    
    bool     DecimalIsNormalF(float Decimal) {
        bool IsNormal = No;
        int8_t Exponent = ExtractExponentF(Decimal);
        if (Exponent >= 1 && Exponent <= 0xFE) {
            IsNormal  = Yes;
        }
        return IsNormal;
    }
    
    bool     DecimalIsNormalD(double Decimal) {
        bool IsNormal = No;
        int16_t Exponent = ExtractExponentD(Decimal);
        if (Exponent >= 1 && Exponent <= 0x7FE) {
            IsNormal  = Yes;
        }
        return IsNormal;
    }
    
    bool     DecimalIsInfinityF(float Decimal) {
        bool    IsInfinity = No;
        uint8_t Exponent   = (((uint32_t)Decimal) & 0x7F800000) >> 23;
        int32_t Mantissa   = ExtractMantissaF(Decimal);
        if (Exponent == 255 && Mantissa == 0) {
            IsInfinity     = Yes;
        }
        return IsInfinity;
    }
    
    bool     DecimalIsInfinityD(double Decimal) {
        bool    IsInfinity = No;
        int16_t Exponent   = (((uint64_t)Decimal) & 0x7FF0000000000000) >> 52;
        int32_t Mantissa   = ExtractMantissaD(Decimal);
        if (Exponent == 2047 && Mantissa == 0) {
            IsInfinity     = Yes;
        }
        return IsInfinity;
    }
    
    bool     DecimalIsNotANumberF(float Decimal) {
        bool   IsNotANumber = No;
        int8_t  Sign       = ExtractSignF(Decimal);
        uint8_t Exponent   = (((uint32_t)Decimal) & 0x7F800000) >> 23;
        int32_t Mantissa   = ExtractMantissaF(Decimal);
        if (Sign == 1 && Exponent == 0xFF && Mantissa > 0) {
            IsNotANumber   = Yes;
        }
        return IsNotANumber;
    }
    
    bool     DecimalIsNotANumberD(double Decimal) {
        bool   IsNotANumber = No;
        int8_t  Sign       = ExtractSignD(Decimal);
        int16_t Exponent   = (((uint64_t)Decimal) & 0x7FF0000000000000) >> 52;
        int64_t Mantissa   = ExtractMantissaD(Decimal);
        if (Sign == 1 && Exponent == 0x7FF && Mantissa > 0) {
            IsNotANumber   = Yes;
        }
        return IsNotANumber;
    }
    
    bool     NumberHasDecimalPointF(float Decimal) {
        bool HasDecimalPoint = No;
        int32_t Mantissa     = ExtractMantissaF(Decimal);
        if ((Mantissa & 0x1FFF) > 0) {
            HasDecimalPoint  = Yes;
        }
        return HasDecimalPoint;
    }
    
    bool     NumberHasDecimalPointD(double Decimal) {
        bool HasDecimalPoint = No;
        int64_t Mantissa     = ExtractMantissaD(Decimal);
        if ((Mantissa & 0x7FFFFFFFFFF) > 0) {
            HasDecimalPoint  = Yes;
        }
        return HasDecimalPoint;
    }
    
    int8_t   ExtractSignI(int64_t Integer) {
        return (Integer & 0x8000000000000000) >> 63 == 1 ? -1 : 1;
    }
    
    int8_t   ExtractSignF(float Decimal) {
        uint32_t *Sign1 = (uint32_t *) &Decimal;
        uint32_t  Sign2 = (*Sign1 & 0x80000000) >> 31;
        return Sign2 == 0 ? 1 : -1;
    }
    
    int8_t   ExtractSignD(double Decimal) {
        uint64_t *Sign1 = (uint64_t *) &Decimal;
        uint64_t  Sign2 = (*Sign1 & 0x8000000000000000) >> 63;
        return Sign2 == 0 ? 1 : -1;
    }
    
    int8_t  ExtractExponentF(float Decimal) {
        int8_t    Sign      = ExtractSignF(Decimal);
        uint32_t *Exponent1 = (uint32_t *) &Decimal;
        int8_t    Exponent2 = (*Exponent1 & 0x7F800000) >> 23;
        int8_t    Exponent3 = (Exponent2 - 127) * Sign;
        return Exponent3;
    }
    
    int16_t  ExtractExponentD(double Decimal) {
        int8_t    Sign      = ExtractSignD(Decimal);
        uint64_t *Exponent1 = (uint64_t *) &Decimal;
        int16_t   Exponent2 = (*Exponent1 & 0x7FF0000000000000) >> 52;
        int16_t   Exponent3 = (Exponent2 - 1023) * Sign;
        return Exponent3;
    }
    
    int32_t  ExtractMantissaF(float Decimal) {
        uint32_t *Mantissa1  = (uint32_t *) &Decimal;
        int32_t   Mantissa2  = *Mantissa1 & 0x7FFFFFUL;
        int32_t   Mantissa3  = 0UL;
        if (DecimalIsNormalF(Decimal)) {
            Mantissa3        = Mantissa2 | 0x800000;
        } else {
            Mantissa3        = Mantissa2;
        }
        return Mantissa3;
    }
    
    int64_t  ExtractMantissaD(double Decimal) {
        uint64_t *Mantissa1  = (uint64_t *) &Decimal;
        uint64_t  Mantissa2  = *Mantissa1 & 0xFFFFFFFFFFFFFULL;
        int32_t   Mantissa3  = 0UL;
        if (DecimalIsNormalD(Decimal)) {
            Mantissa3        = Mantissa2 | 0x10000000000000;
        } else {
            Mantissa3        = Mantissa2;
        }
        return Mantissa3;
    }
    
    uint64_t Exponentiate(uint64_t Base, uint64_t Exponent) {
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
    
    int64_t  Logarithm(int64_t Base, int64_t Exponent) {
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
    
    uint16_t SwapEndian16(uint16_t Integer) {
        return ((Integer & 0xFF00) >> 8) | ((Integer & 0x00FF) << 8);
    }
    
    uint32_t SwapEndian32(uint32_t Integer) {
        return ((Integer & 0xFF000000) >> 24) | ((Integer & 0x00FF0000) >> 8) | ((Integer & 0x0000FF00) << 8) | ((Integer & 0x000000FF) << 24);
    }
    
    uint64_t SwapEndian64(uint64_t Integer) {
        return (((Integer & 0xFF00000000000000) >> 56) | ((Integer & 0x00FF000000000000) >> 40) | \
                ((Integer & 0x0000FF0000000000) >> 24) | ((Integer & 0x000000FF00000000) >>  8) | \
                ((Integer & 0x00000000FF000000) <<  8) | ((Integer & 0x0000000000FF0000) << 24) | \
                ((Integer & 0x000000000000FF00) << 40) | ((Integer & 0x00000000000000FF) << 56)
                );
    }
    
    int64_t  Bytes2Bits(int64_t Bytes) {
        return Bytes * 8;
    }
    
    int64_t  Bits2Bytes(int64_t Bits, bool RoundUp) {
        uint64_t AbsoluteBits = AbsoluteI(Bits);
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
    
    uint8_t  SwapBits(uint8_t Byte) {
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
    
    uint8_t CreateBitMask(uint8_t NumBits2Select) {
        uint8_t Mask = 0;
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
        Mask = Exponentiate(2, NumBits2Select) >> (8 - NumBits2Select);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
        Mask = Exponentiate(2, NumBits2Select) << (8 - NumBits2Select);
#endif
        return Mask;
    }
    
    /* Ryū specific math functions */
    bool IsPowerOf2(uint64_t Value, uint8_t Shift) { // multipleOfPowerOf2
        return (Value & ((1 << Shift) - 1) >> (Shift - 1)); // (16 & 31) >> 4
    }
    /* Ryū specific math functions */
    
    uint8_t GetNumDigitsInBase(uint8_t Base, int64_t Value) {
        uint64_t Value2    = Absolute(Value);
        uint8_t  NumDigits = 0;
        do {
            Value2        /= Base;
            NumDigits     += 1;
        } while (Value2 > 0);
        return NumDigits;
    }
    
    uint8_t GetNumBitsToRepresentValue(uint64_t Value) { // This name is so bad
        return Value;
        // If the value is a power of 2, shift by 1
        // Otherwise shift by 1...
    }
    
#ifdef __cplusplus
}
#endif
