#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/Math.h"           /* Included for our declarations */

#ifdef __cplusplus
extern "C" {
#endif
    
    bool IsNegative(const int64_t Integer) {
        return (Integer & 0x8000000000000000) >> 63;
    }
    
    bool IsOdd(const int64_t Integer) {
        return Integer & 1;
    }
    
    uint64_t AbsoluteI(const int64_t Integer) {
        bool Sign = Integer >> 63;
        return (Integer ^ -Sign) + Sign;
    }
    
    typedef union Float2Integer {
        float    Float;
        uint32_t Integer;
    } Float2Integer;
    
    typedef union Double2Integer {
        double    Float;
        uint64_t  Integer;
    } Double2Integer;
    
    uint32_t ConvertFloat2Integer(const float Decimal) {
        Float2Integer Integer = {.Float = Decimal};
        return Integer.Integer;
    }
    
    uint64_t ConvertDouble2Integer(const double Decimal) {
        Double2Integer Integer = {.Float = Decimal};
        return Integer.Integer;
    }
    
    float ConvertInteger2Float(const uint32_t Integer) {
        Float2Integer Float = {.Integer = Integer};
        return Float.Float;
    }
    
    double ConvertInteger2Double(const uint64_t Integer) {
        Double2Integer Float = {.Integer = Integer};
        return Float.Float;
    }
    
    uint8_t AbsoluteF(const float Decimal) {
        return (uint8_t) ExtractExponentF(Decimal);
    }
    
    uint16_t AbsoluteD(const double Decimal) {
        return (uint16_t) ExtractExponentD(Decimal);
    }
    
    int64_t  FloorF(const float Decimal) {
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
    
    int64_t  FloorD(const double Decimal) {
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
    
    int64_t  CeilF(const float Decimal) {
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
    
    int64_t  CeilD(const double Decimal) {
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
    
    int64_t  RoundF(const float Decimal) {
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
    
    int64_t  RoundD(const double Decimal) {
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
    
    int64_t  Min(const int64_t Integer1, const int64_t Integer2) {
        return Integer2 ^ ((Integer1 ^ Integer2) & -(Integer1 < Integer2));
    }
    
    int64_t  Max(const int64_t Integer1, const int64_t Integer2) {
        return Integer1 ^ ((Integer1 ^ Integer2) & -(Integer1 < Integer2));
    }
    
    bool     DecimalIsNormalF(const float Decimal) {
        bool   IsNormal = No;
        int8_t Exponent = ExtractExponentF(Decimal);
        if (Exponent >= 1 && Exponent <= 0x7E) {
            IsNormal    = Yes;
        }
        return IsNormal;
    }
    
    bool     DecimalIsNormalD(const double Decimal) {
        bool IsNormal = No;
        int16_t Exponent = ExtractExponentD(Decimal);
        if (Exponent >= 1 && Exponent <= 0x7FE) {
            IsNormal  = Yes;
        }
        return IsNormal;
    }
    
    bool     DecimalIsInfinityF(const float Decimal) {
        bool    IsInfinity = No;
        uint8_t Exponent   = (((uint32_t)Decimal) & 0x7F800000) >> 23;
        int32_t Mantissa   = ExtractMantissaF(Decimal);
        if (Exponent == 255 && Mantissa == 0) {
            IsInfinity     = Yes;
        }
        return IsInfinity;
    }
    
    bool     DecimalIsInfinityD(const double Decimal) {
        bool    IsInfinity = No;
        int16_t Exponent   = (((uint64_t)Decimal) & 0x7FF0000000000000) >> 52;
        int32_t Mantissa   = ExtractMantissaD(Decimal);
        if (Exponent == 2047 && Mantissa == 0) {
            IsInfinity     = Yes;
        }
        return IsInfinity;
    }
    
    bool     DecimalIsNotANumberF(const float Decimal) {
        bool   IsNotANumber = No;
        int8_t  Sign       = ExtractSignF(Decimal);
        uint8_t Exponent   = (((uint32_t)Decimal) & 0x7F800000) >> 23;
        int32_t Mantissa   = ExtractMantissaF(Decimal);
        if (Sign == 1 && Exponent == 0xFF && Mantissa > 0) {
            IsNotANumber   = Yes;
        }
        return IsNotANumber;
    }
    
    bool     DecimalIsNotANumberD(const double Decimal) {
        bool   IsNotANumber = No;
        int8_t  Sign       = ExtractSignD(Decimal);
        int16_t Exponent   = (((uint64_t)Decimal) & 0x7FF0000000000000) >> 52;
        int64_t Mantissa   = ExtractMantissaD(Decimal);
        if (Sign == 1 && Exponent == 0x7FF && Mantissa > 0) {
            IsNotANumber   = Yes;
        }
        return IsNotANumber;
    }
    
    bool     NumberHasDecimalPointF(const float Decimal) {
        bool HasDecimalPoint = No;
        int32_t Mantissa     = ExtractMantissaF(Decimal);
        if ((Mantissa & 0x1FFF) > 0) {
            HasDecimalPoint  = Yes;
        }
        return HasDecimalPoint;
    }
    
    bool     NumberHasDecimalPointD(const double Decimal) {
        bool HasDecimalPoint = No;
        int64_t Mantissa     = ExtractMantissaD(Decimal);
        if ((Mantissa & 0x7FFFFFFFFFF) > 0) {
            HasDecimalPoint  = Yes;
        }
        return HasDecimalPoint;
    }
    
    int8_t   ExtractSignI(const int64_t Integer) {
        return (Integer & 0x8000000000000000) >> 63 == 1 ? -1 : 1;
    }
    
    int8_t   ExtractSignF(const float Decimal) {
        uint32_t Integer       = ConvertFloat2Integer(Decimal);
        int8_t   Sign          = (Integer & 0x80000000) >> 31;
        return   Sign == 0 ? 1 : -1;
    }
    
    int8_t   ExtractSignD(const double Decimal) {
        uint64_t Integer        = ConvertDouble2Integer(Decimal);
        int8_t Sign             = (Integer & 0x8000000000000000) >> 63;
        return Sign == 0 ? 1 : -1;
    }
    
    int8_t  ExtractExponentF(const float Decimal) {
        uint32_t Integer       = ConvertFloat2Integer(Decimal);
        int8_t   Sign          = ExtractSignF(Decimal);
        int8_t   Exponent      = (Integer & 0x7F800000) >> 23;
        return (Exponent - 127) * Sign;
    }
    
    int16_t  ExtractExponentD(const double Decimal) {
        uint64_t Integer        = ConvertDouble2Integer(Decimal);
        int8_t   Sign           = ExtractSignD(Decimal);
        int16_t  Exponent       = (Integer & 0x7FF0000000000000) >> 52;
        return (Exponent - 1023) * Sign;
    }
    
    int32_t  ExtractMantissaF(const float Decimal) {
        uint32_t Integer       = ConvertFloat2Integer(Decimal);
        uint32_t Mantissa      = Integer & 0x7FFFFFUL;
        return DecimalIsNormalF(Decimal) ? Mantissa |= 0x800000 : Mantissa;
    }
    
    int64_t  ExtractMantissaD(const double Decimal) {
        uint64_t Integer        = ConvertDouble2Integer(Decimal);
        uint64_t Mantissa       = Integer & 0xFFFFFFFFFFFFFULL;
        return DecimalIsNormalF(Decimal) ? Mantissa |= 0x10000000000000 : Mantissa;
    }
    
    float InsertSignF(const float Insertee, const int8_t Sign) {
        Float2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer      &= Sign == -1 ? 0x80000000 : 0;
        return Insertee2.Float;
    }
    
    double InsertSignD(const double Insertee, const int8_t Sign) {
        Double2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer       &= Sign == -1 ? 0x8000000000000000 : 0;
        return Insertee2.Float;
    }
    
    float InsertExponentF(const float Insertee, const int8_t Value) {
        uint32_t Value2         = Value;
        Float2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer      &= ((Value2 & 0xFF) << 23); // Value is 8 bits, and needs to be normalized to +/-
        return Insertee2.Float;
    }
    
    double InsertExponentD(const double Insertee, const int16_t Value) {
        uint64_t Value2          = Value;
        Double2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer       &= ((Value2 & 0x7FF) << 52);
        return Insertee2.Float;
    }
    
    float InsertMantissaF(const float Insertee, const uint32_t Value) {
        Float2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer      &= (Value & 0x7FFFFFUL); // Value is 8 bits, and needs to be normalized to +/-
        return Insertee2.Float;
    }
    
    double InsertMantissaD(const double Insertee, const uint64_t Value) {
        Double2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer       &= (Value & 0xFFFFFFFFFFFFFULL);
        return Insertee2.Float;
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
    
    int64_t  Logarithm(const int64_t Base, const int64_t Exponent) {
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
    
    uint16_t SwapEndian16(const uint16_t Integer) {
        return ((Integer & 0xFF00) >> 8) | ((Integer & 0x00FF) << 8);
    }
    
    uint32_t SwapEndian32(const uint32_t Integer) {
        return ((Integer & 0xFF000000) >> 24) | ((Integer & 0x00FF0000) >> 8) | ((Integer & 0x0000FF00) << 8) | ((Integer & 0x000000FF) << 24);
    }
    
    uint64_t SwapEndian64(const uint64_t Integer) {
        return (((Integer & 0xFF00000000000000) >> 56) | ((Integer & 0x00FF000000000000) >> 40) | \
                ((Integer & 0x0000FF0000000000) >> 24) | ((Integer & 0x000000FF00000000) >>  8) | \
                ((Integer & 0x00000000FF000000) <<  8) | ((Integer & 0x0000000000FF0000) << 24) | \
                ((Integer & 0x000000000000FF00) << 40) | ((Integer & 0x00000000000000FF) << 56)
                );
    }
    
    int64_t  Bytes2Bits(const int64_t Bytes) {
        return Bytes * 8;
    }
    
    int64_t  Bits2Bytes(const int64_t Bits, const bool RoundUp) {
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
    
    uint8_t Bits2ExtractFromByte(const uint64_t Offset) {
        return 8 - (Offset % 8);
    }
    
    uint8_t CreateBitsMaskLSBit(const uint8_t Bits2Read) {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
        uint8_t Mask = Exponentiate(2, Bits2Read);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
        uint8_t Mask = Exponentiate(2, Bits2Read) >> (8 - Bits2Read);
#endif
        return Mask;
    }
    
    uint8_t CreateBitsMaskMSBit(const uint8_t Bits2Read) {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
        uint8_t Mask = Exponentiate(2, Bits2Read) >> (8 - Bits2Read);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
        uint8_t Mask = Exponentiate(2, Bits2Read);
#endif
        return Mask;
    }
    
    /* Ryū specific math functions */
    bool IsPowerOfBase(const uint8_t Base, const uint64_t Value) {
        return Value % Base == 0 ? Yes : No;
    }
    /* Ryū specific math functions */
    
    uint8_t GetNumDigitsInBase(const uint8_t Base, const int64_t Value) {
        uint64_t Value2    = AbsoluteI(Value);
        uint8_t  NumDigits = 0;
        do {
            Value2        /= Base;
            NumDigits     += 1;
        } while (Value2 > 0);
        return NumDigits;
    }
    
#ifdef __cplusplus
}
#endif
