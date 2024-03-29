#include "../include/MathIO.h"       /* Included for our declarations */

#include "../include/AssertIO.h"     /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @enum         MathIO_Constants
     @abstract                                    Mathematical constants
     */
    typedef enum MathIO_Constants : uint16_t {
                   Decimal64Bias                  = 1023,
                   Decimal32Bias                  = 127,
                   Decimal64MantissaSize          = 52,
                   Decimal32MantissaSize          = 23,
                   Decimal64ExponentSize          = 11,
                   Decimal32ExponentSize          = 8,
    } MathIO_Constants;
    
    bool IsNegative8(const int8_t Integer) {
        return (Integer & 0x80) >> 7;
    }
    
    bool IsNegative16(const int16_t Integer) {
        return Integer >> 15;
    }
    
    bool IsNegative32(const int32_t Integer) {
        return Integer >> 31;
    }
    
    bool IsNegative64(const int64_t Integer) {
        return Integer >> 63;
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
        int32_t  Exponent = ExtractExponentD(Decimal);
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
        int32_t  Exponent = ExtractExponentD(Decimal);
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
        int32_t  Exponent = ExtractExponentD(Decimal);
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

    /*
     Value to extend = 10 aka 0b1010 aka 0x0A 5 bits
     1 << (NumBits - 1) = Sign Bit
     */
    int64_t SignExtend(const int64_t Integer, const uint8_t IntegerSizeInBits) {
        AssertIO(IntegerSizeInBits >= 1 && "IntegerSizeInBits must be at least one");
        int64_t Extended = 0;
        // (1 << IntegerSizeInBits) - 1 = all 1's for IntegerSizeInBits
        if (Integer < 1) {
            Extended     = (0xFFFFFFFFFFFFFFFF << IntegerSizeInBits) | Integer;
        } else {
            Extended     = Integer;
        }
        return Extended;
        // 0xFFFFFFFFFFFFFFFF XOR Integer = 0xFFFFFFFFFFFFFFF5
        // ((0xFFFFFFFFFFFFFFFF << IntegerSizeInBits) | Integer) = 0xFFFFFFFFFFFFFFEA
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
        int32_t Exponent = ExtractExponentD(Decimal);
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
    
    int64_t Logarithm(const uint8_t Base, const int64_t Exponent) {
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
    
    size_t Bytes2Bits(const size_t NumBytes) {
        return NumBytes * 8;
    }
    
    size_t Bits2Bytes(const MathIO_RoundingTypes RoundingType, const size_t NumBits) {
        size_t Bytes        = NumBits / 8;
        if (RoundingType == RoundingType_Up && NumBits % 8 != 0) {
            Bytes          += 1;
        }
        return Bytes;
    }
    
    uint8_t BitsAvailableInByte(const size_t NumBits) {
        return 8 - (NumBits % 8);
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

    uint8_t GetHighestSetBit(uint64_t Value) {
        uint8_t HighestBit = 0;
        while (Value >>= 1) {
            HighestBit    += 1;
        }
        return HighestBit;
    }

    uint64_t RoundDownToPowerOf2(uint64_t Value) {
        return 1ULL << GetHighestSetBit(Value | 1);
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
/*
#if (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
__attribute__((no_sanitize("undefined")))
#endif
 */
    
    uint64_t RotateLeft(uint8_t NumBits2Rotate, uint64_t Value) {
        return (Value << NumBits2Rotate) | (Value >> (64 - NumBits2Rotate));
    }
    
    uint64_t RotateRight(uint8_t NumBits2Rotate, uint64_t Value) {
        return (Value >> NumBits2Rotate) | (Value << (64 - NumBits2Rotate));
    }

    uint16_t PackIntegers8To16(uint8_t Values[2]) {
        return Values[0] | ((uint16_t) Values[1] << 8);
    }

    uint32_t PackIntegers8To32(uint8_t Values[4]) {
        return Values[0] | ((uint32_t) Values[1] << 8) | ((uint32_t) Values[2] << 16) | ((uint32_t) Values[3] << 24);
    }

    uint64_t PackIntegers8To64(uint8_t Values[8]) {
        return Values[0] | ((uint64_t) Values[1] << 8) | ((uint64_t) Values[2] << 16) | ((uint64_t) Values[3] << 24) | \
        ((uint64_t) Values[4] << 32) | ((uint64_t) Values[5] << 40) | ((uint64_t) Values[6] << 48) | ((uint64_t) Values[7] << 56);
    }

    uint32_t PackIntegers16To32(uint16_t Values[2]) {
        return Values[0] | ((uint32_t) Values[1] << 16);
    }

    uint64_t PackIntegers16To64(uint16_t Values[4]) {
        return Values[0] | ((uint64_t) Values[1] << 16) | ((uint64_t) Values[2] << 32) | ((uint64_t) Values[3] << 48);
    }

    uint64_t PackIntegers32To64(uint32_t Values[2]) {
        return Values[0] | ((uint64_t) Values[1] << 32);
    }

    void UnpackInteger16To8(uint16_t Value, uint8_t Returned[2]) {
        Returned[0] = (uint8_t) Value >> 0;
        Returned[1] = (uint8_t) Value >> 8;
    }

    void UnpackInteger32To8(uint32_t Value, uint8_t Returned[4]) {
        Returned[0] = (uint8_t) Value >> 0;
        Returned[1] = (uint8_t) Value >> 8;
        Returned[2] = (uint8_t) Value >> 16;
        Returned[3] = (uint8_t) Value >> 24;
    }

    void UnpackInteger32To16(uint32_t Value, uint16_t Returned[2]) {
        Returned[0] = (uint16_t) Value >> 0;
        Returned[1] = (uint16_t) Value >> 16;
    }

    void UnpackInteger64To8(uint64_t Value, uint8_t Returned[8]) {
        Returned[0] = (uint8_t) (Value >> 0);
        Returned[1] = (uint8_t) (Value >> 8);
        Returned[2] = (uint8_t) (Value >> 16);
        Returned[3] = (uint8_t) (Value >> 24);
        Returned[4] = (uint8_t) (Value >> 32);
        Returned[5] = (uint8_t) (Value >> 40);
        Returned[6] = (uint8_t) (Value >> 48);
        Returned[7] = (uint8_t) (Value >> 56);
    }

    void UnpackInteger64To16(uint64_t Value, uint16_t Returned[4]) {
        Returned[0] = (uint16_t) (Value >> 0);
        Returned[1] = (uint16_t) (Value >> 16);
        Returned[2] = (uint16_t) (Value >> 32);
        Returned[3] = (uint16_t) (Value >> 48);
    }

    void UnpackInteger64To32(uint64_t Value, uint32_t Returned[2]) {
        Returned[0] = (uint32_t) (Value >> 0);
        Returned[1] = (uint32_t) (Value >> 32);
    }

    size_t FloorPowerOfTwo(const size_t Value) {
        size_t x = Value;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
#if __LP64__
        x = x | (x >> 32);
#endif
        return x - (x >> 1);
    }
    
    int8_t SignExtend8(uint8_t Value2Extend, uint8_t FieldSizeInBits) {
        AssertIO(FieldSizeInBits <= 7);
        int8_t Extended = 0;
        bool Sign = (Value2Extend >> (FieldSizeInBits - 1));
        if (Sign == 1) {
           Extended = Value2Extend ^ 0;
        } else {
            Extended = Value2Extend ^ 1;
        }
        return Extended;
    }
    
    int16_t SignExtend16(uint16_t Value2Extend, uint8_t FieldSizeInBits) {
        AssertIO(FieldSizeInBits <= 15);
        int16_t Extended = 0;
        bool Sign = (Value2Extend >> (FieldSizeInBits - 1));
        if (Sign == 1) {
           Extended = Value2Extend ^ 0;
        } else {
            Extended = Value2Extend ^ 1;
        }
        return Extended;
    }
    
    int32_t SignExtend32(uint32_t Value2Extend, uint8_t FieldSizeInBits) {
        AssertIO(FieldSizeInBits <= 31);
        int32_t Extended = 0;
        bool Sign = (Value2Extend >> (FieldSizeInBits - 1));
        if (Sign == 1) {
           Extended = Value2Extend ^ 0;
        } else {
            Extended = Value2Extend ^ 1;
        }
        return Extended;
    }
    
    int64_t SignExtend64(uint64_t Value2Extend, uint8_t FieldSizeInBits) {
        AssertIO(FieldSizeInBits <= 63);
        int64_t Extended = 0;
        bool Sign = (Value2Extend >> (FieldSizeInBits - 1));
        if (Sign == 1) {
           Extended = Value2Extend ^ 0;
        } else {
            Extended = Value2Extend ^ 1;
        }
        return Extended;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
