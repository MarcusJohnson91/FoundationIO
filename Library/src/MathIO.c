#include "../include/MathIO.h"       /* Included for our declarations */
#include "../include/TextIO/LogIO.h" /* Included for Logging */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    bool IsNegative8(const int8_t Integer) {
        return (Integer & 0x80) >> 7;
    }
    
    bool IsNegative16(const int16_t Integer) {
        return (Integer & 0x8000) >> 15;
    }
    
    bool IsNegative32(const int32_t Integer) {
        return (Integer & 0x80000000) >> 31;
    }
    
    bool IsNegative64(const int64_t Integer) {
        return (Integer & 0x8000000000000000) >> 63;
    }
    
    bool IsOdd(const int64_t Integer) {
        return Integer & 1;
    }
    
    uint64_t AbsoluteI(const int64_t Integer) {
        uint64_t AbsoluteValue = 0ULL;
        bool     Sign          = (Integer & 0x8000000000000000) >> 63;
        AbsoluteValue          = (Integer ^ (Sign * -1)) + Sign;
        return AbsoluteValue;
    }
    
    typedef union Float2Integer {
        float    Float;
        uint32_t Integer;
    } Float2Integer;
    
    typedef union Double2Integer {
        double    Float;
        uint64_t  Integer;
    } Double2Integer;
    
    typedef union Integer2Bytes {
        uint64_t Integer;
        uint8_t  Bytes[8];
    } Integer2Bytes;
    
    void GetBytesFromInteger(const uint64_t Integer, uint8_t *Bytes) {
        Integer2Bytes Data = {.Integer = Integer};
        Bytes              = Data.Bytes;
    }
    
    uint64_t GetIntegerFromBytes(uint64_t *Bytes) {
        Integer2Bytes Data = {.Integer = *Bytes};
        return Data.Integer;
    }
    
    uint32_t ConvertFloat2Integer(float Decimal) {
        Float2Integer Integer = {.Float = Decimal};
        return Integer.Integer;
    }
    
    uint64_t ConvertDouble2Integer(double Decimal) {
        Double2Integer Integer = {.Float = Decimal};
        return Integer.Integer;
    }
    
    float ConvertInteger2Float(uint32_t Integer) {
        Float2Integer Float = {.Integer = Integer};
        return Float.Float;
    }
    
    double ConvertInteger2Double(uint64_t Integer) {
        Double2Integer Float = {.Integer = Integer};
        return Float.Float;
    }
    
    uint8_t AbsoluteF(const float Decimal) {
        return (uint8_t) ExtractExponentF(Decimal);
    }
    
    uint16_t AbsoluteD(const double Decimal) {
        return (uint16_t) ExtractExponentD(Decimal);
    }
    
    int16_t FloorF(const float Decimal) {
        int16_t  Result   = 0;
        int8_t   Sign     = ExtractSignF(Decimal);
        int16_t  Exponent = ExtractExponentF(Decimal);
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
    
    int32_t FloorD(const double Decimal) {
        int32_t  Result   = 0;
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
    
    int16_t CeilF(const float Decimal) {
        int16_t  Result   = 0;
        int8_t   Sign     = ExtractSignF(Decimal);
        int16_t  Exponent = ExtractExponentF(Decimal);
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
    
    int32_t CeilD(const double Decimal) {
        int32_t  Result   = 0;
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
    
    int16_t RoundF(const float Decimal) {
        int16_t  Result   = 0;
        int8_t   Sign     = ExtractSignF(Decimal);
        int16_t  Exponent = ExtractExponentF(Decimal);
        int64_t  Mantissa = ExtractMantissaF(Decimal);
        if (Mantissa >= 4096) {
            Result        = Exponent + 1;
        } else {
            Result        = Exponent;
        }
        return Result * Sign;
    }
    
    int32_t RoundD(const double Decimal) {
        int32_t  Result   = 0;
        int8_t   Sign     = ExtractSignD(Decimal);
        int16_t  Exponent = ExtractExponentD(Decimal);
        int64_t  Mantissa = ExtractMantissaD(Decimal);
        if (Mantissa >= 4096) {
            Result        = Exponent + 1;
        } else {
            Result        = Exponent;
        }
        return Result * Sign;
    }
    
    int64_t Minimum(const int64_t Integer1, const int64_t Integer2) {
        int64_t Min1    = Integer1 ^ Integer2;
        int64_t Min2    = -(Integer1 < Integer2);
        int64_t Min3    = Integer2 ^ (Min1 & Min2);
        return  Min3;
    }
    
    int64_t Maximum(const int64_t Integer1, const int64_t Integer2) {
        int64_t Max1    = Integer1 ^ Integer2;
        int64_t Max2    = -(Integer1 < Integer2);
        int64_t Max3    = Integer1 ^ (Max1 & Max2);
        return Max3;
    }

    int64_t Subtract(const int64_t Integer1, const int64_t Integer2) {
        return Maximum(Integer1, Integer2) - Minimum(Integer1, Integer2);
    }
    
    bool DecimalIsNormalF(const float Decimal) {
        bool    IsNormal = No;
        int16_t Exponent = ExtractExponentF(Decimal);
        if (Exponent >= 1 && Exponent <= 0x7E) {
            IsNormal    = Yes;
        }
        return IsNormal;
    }
    
    bool DecimalIsNormalD(const double Decimal) {
        bool IsNormal = No;
        int16_t Exponent = ExtractExponentD(Decimal);
        if (Exponent >= 1 && Exponent <= 0x7FE) {
            IsNormal  = Yes;
        }
        return IsNormal;
    }
    
    bool DecimalIsInfinityF(const float Decimal) {
        bool    IsInfinity = No;
        uint8_t Exponent   = (((uint32_t)Decimal) & 0x7F800000) >> 23;
        int32_t Mantissa   = ExtractMantissaF(Decimal);
        if (Exponent == 255 && Mantissa == 0) {
            IsInfinity     = Yes;
        }
        return IsInfinity;
    }
    
    bool DecimalIsInfinityD(const double Decimal) {
        bool    IsInfinity = No;
        int16_t Exponent   = (((uint64_t)Decimal) & 0x7FF0000000000000) >> 52;
        int64_t Mantissa   = ExtractMantissaD(Decimal);
        if (Exponent == 2047 && Mantissa == 0) {
            IsInfinity     = Yes;
        }
        return IsInfinity;
    }
    
    bool DecimalIsANumberF(const float Decimal) {
        bool   IsNotANumber = No;
        int8_t  Sign       = ExtractSignF(Decimal);
        uint8_t Exponent   = (((uint32_t)Decimal) & 0x7F800000) >> 23;
        int32_t Mantissa   = ExtractMantissaF(Decimal);
        if (Sign == 1 && Exponent == 0xFF && Mantissa > 0) {
            IsNotANumber   = Yes;
        }
        return IsNotANumber;
    }
    
    bool DecimalIsANumberD(const double Decimal) {
        bool   IsNotANumber = No;
        int8_t  Sign       = ExtractSignD(Decimal);
        int16_t Exponent   = (((uint64_t)Decimal) & 0x7FF0000000000000) >> 52;
        int64_t Mantissa   = ExtractMantissaD(Decimal);
        if (Sign == 1 && Exponent == 0x7FF && Mantissa > 0) {
            IsNotANumber   = Yes;
        }
        return IsNotANumber;
    }
    
    bool NumberHasDecimalPointF(const float Decimal) {
        bool HasDecimalPoint = No;
        int32_t Mantissa     = ExtractMantissaF(Decimal);
        if ((Mantissa & 0x1FFF) > 0) {
            HasDecimalPoint  = Yes;
        }
        return HasDecimalPoint;
    }
    
    bool NumberHasDecimalPointD(const double Decimal) {
        bool HasDecimalPoint = No;
        int64_t Mantissa     = ExtractMantissaD(Decimal);
        if ((Mantissa & 0x7FFFFFFFFFF) > 0) {
            HasDecimalPoint  = Yes;
        }
        return HasDecimalPoint;
    }
    
    int8_t ExtractSignI(const int64_t Integer) {
        return (Integer & 0x8000000000000000) >> 63 == 1 ? -1 : 1;
    }
    
    int8_t ExtractSignF(const float Decimal) {
        uint32_t Integer       = ConvertFloat2Integer(Decimal);
        int8_t   Sign          = (Integer & 0x80000000) >> 31;
        return   Sign == 0 ? 1 : -1;
    }
    
    int8_t ExtractSignD(const double Decimal) {
        uint64_t Integer        = ConvertDouble2Integer(Decimal);
        int8_t Sign             = (Integer & 0x8000000000000000) >> 63;
        return Sign == 0 ? 1 : -1;
    }
    
    int16_t ExtractExponentF(const float Decimal) {
        uint32_t Integer       = ConvertFloat2Integer(Decimal);
        int8_t   Sign          = ExtractSignF(Decimal);
        int8_t   Exponent      = (Integer & 0x7F800000) >> 23;
        return (Exponent - 127) * Sign;
    }
    
    int32_t ExtractExponentD(const double Decimal) {
        uint64_t Integer        = ConvertDouble2Integer(Decimal);
        int8_t   Sign           = ExtractSignD(Decimal);
        int16_t  Exponent       = (Integer & 0x7FF0000000000000) >> 52;
        return (Exponent - 1023) * Sign;
    }
    
    int32_t ExtractMantissaF(const float Decimal) {
        uint32_t Integer       = ConvertFloat2Integer(Decimal);
        uint32_t Mantissa      = Integer & 0x7FFFFFUL;
        bool     IsNormal      = DecimalIsNormal(Decimal);
        if (IsNormal) {
            Mantissa          |= 0x800000;
        }
        return Mantissa;
    }
    
    int64_t ExtractMantissaD(const double Decimal) {
        uint64_t Integer        = ConvertDouble2Integer(Decimal);
        uint64_t Mantissa       = Integer & 0xFFFFFFFFFFFFFULL;
        bool     IsNormal       = DecimalIsNormal(Decimal);
        if (IsNormal) {
            Mantissa           |= 0x10000000000000;
        }
        return Mantissa;
    }
    
    float InsertSignF(float Insertee, const int8_t Sign) {
        Float2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer      &= Sign == -1 ? 0x80000000 : 0;
        return Insertee2.Float;
    }
    
    double InsertSignD(double Insertee, const int8_t Sign) {
        Double2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer       &= Sign == -1 ? 0x8000000000000000 : 0;
        return Insertee2.Float;
    }
    
    float InsertExponentF(float Insertee, const int8_t Value) {
        uint32_t Value2         = Value;
        Float2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer      &= ((Value2 & 0xFF) << 23);
        return Insertee2.Float;
    }
    
    double InsertExponentD(double Insertee, const int16_t Value) {
        uint64_t Value2          = Value;
        Double2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer       &= ((Value2 & 0x7FF) << 52);
        return Insertee2.Float;
    }
    
    float InsertMantissaF(float Insertee, const uint32_t Value) {
        Float2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer      &= (Value & 0x7FFFFFUL);
        return Insertee2.Float;
    }
    
    double InsertMantissaD(double Insertee, const uint64_t Value) {
        Double2Integer Insertee2 = {.Float = Insertee};
        Insertee2.Integer       &= (Value & 0xFFFFFFFFFFFFFULL);
        return Insertee2.Float;
    }
    
    int64_t Exponentiate(const uint64_t Base, const int64_t Exponent) {
        int64_t Value     = Base;
        int64_t Exponent2 = Exponent;
        while (Exponent2 > 1) {
            Value        *= Base;
            Exponent2    -= 1;
        }
        return Value;
    }
    
    int64_t Logarithm(const int64_t Base, const int64_t Exponent) {
        int64_t  Result    = 0ULL;
        int64_t  Exponent2 = Exponent;
        if (Exponent2 != 0) {
            while (Exponent2 != 0) {
                Result    += 1;
                Exponent2 /= Base;
            }
        } else {
            Result         = 1;
        }
        return Result;
    }
    
    uint16_t SwapEndian16(const uint16_t Integer) {
        uint16_t Swapped = (uint16_t) ((Integer & 0xFF00) >> 8) | ((Integer & 0x00FF) << 8);
        return Swapped;
    }
    
    uint32_t SwapEndian32(const uint32_t Integer) {
        uint32_t Swapped = (uint32_t) ((Integer & 0xFF000000) >> 24) | ((Integer & 0x00FF0000) >> 8) | ((Integer & 0x0000FF00) << 8) | ((Integer & 0x000000FF) << 24);
        return Swapped;
    }
    
    uint64_t SwapEndian64(const uint64_t Integer) {
        uint64_t Swapped = (uint64_t) (((Integer & 0xFF00000000000000) >> 56) | ((Integer & 0x00FF000000000000) >> 40) | \
                                       ((Integer & 0x0000FF0000000000) >> 24) | ((Integer & 0x000000FF00000000) >>  8) | \
                                       ((Integer & 0x00000000FF000000) <<  8) | ((Integer & 0x0000000000FF0000) << 24) | \
                                       ((Integer & 0x000000000000FF00) << 40) | ((Integer & 0x00000000000000FF) << 56)
                                       );
        return Swapped;
    }
    
    int64_t  Bytes2Bits(const int64_t Bytes) {
        return Bytes * 8;
    }
    
    uint64_t Bits2Bytes(const MathIO_RoundingTypes RoundingType, const uint64_t Bits) {
        uint64_t Bytes        = Bits / 8;
        if (RoundingType == RoundingType_Up && Bits % 8 != 0) {
            Bytes            += 1;
        }
        return Bytes;
    }
    
    uint8_t BitsAvailableInByte(const uint64_t Offset) {
        return 8 - (Offset % 8);
    }
    
    uint8_t CountBitsSet(const uint64_t Value) {
        uint8_t NumBitsSet = 0;
        uint64_t Value2    = Value;
        while (Value2 != 0) {
            Value2        &= Value2 - 1;
            NumBitsSet    += 1;
        }
        return NumBitsSet;
    }
    
    bool IsPowerOfBase(const uint8_t Base, const uint64_t Value) {
        return Value % Base == 0 ? Yes : No;
    }
    
    uint8_t NumDigitsInInteger(const uint8_t Base, const int64_t Integer) {
        uint8_t NumDigits = 1;
        int64_t Integer2  = Integer / Base;
        while (Integer2 != 0) {
            Integer2     /= Base;
            NumDigits    += 1;
        }
        return NumDigits;
    }

#if (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
__attribute__((no_sanitize("undefined")))
#endif
    int64_t Rotate(const MathIO_RotationType Rotate, const uint8_t NumBits2Rotate, const int64_t Value) {
        int64_t Rotated = 0ULL;
        if (Rotate == Rotate_Left) {
            Rotated     = (Value << NumBits2Rotate) | (Value >> (64 - NumBits2Rotate));
            Rotated     = (Value << NumBits2Rotate) | (Value >> (-NumBits2Rotate) & 63);
        } else if (Rotate == Rotate_Right) {
            Rotated     = (Value >> NumBits2Rotate) | (Value << (64 - NumBits2Rotate));
        }
        return Rotated;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
