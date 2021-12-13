/*!
 @header          MathIO.h
 @author          Marcus Johnson
 @copyright       2017+
 @version         1.0.0
 @brief           This header contains code for specific mathematical functions used in FoundationIO and it's consumers.
 */

#include "PlatformIO.h" /* Included for Platform Independence macros */

#pragma once

#ifndef  FoundationIO_MathIO_H
#define  FoundationIO_MathIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#include <math.h>
    
#if defined(__has_include) && __has_include(<tgmath.h>)
#include <tgmath.h>
#endif
    
    /*!
     @enum         MathIO_RoundingTypes
     @abstract                                    Defines the type of rounding.
     @constant     RoundingType_Unspecified       Invalid ImageType, exists solely to tell when it hasn't been set.
     @constant     RoundingType_Down              Flip the imge vertically, up and down.
     @constant     RoundingType_Up                Flip the image horizontally, side to side.
     */
    typedef enum MathIO_RoundingTypes {
                   RoundingType_Unspecified       = 0,
                   RoundingType_Down              = 1,
                   RoundingType_Up                = 2,
    } MathIO_RoundingTypes;
    
    typedef enum MathIO_RotationTypes {
                   RotationType_Unspecified       = 0,
                   RotationType_Left              = 1,
                   RotationType_Right             = 2,
    } MathIO_RotationTypes;
    
    /*!
     @abstract                                    Tells if the number is negative.
     @param      Integer                          The value to find the signedness of.
     @return                                      Returns if @b Integer is negative or not
     */
    bool         IsNegative8(const int8_t Integer);
    
    /*!
     @abstract                                    Tells if the number is negative.
     @param      Integer                          The value to find the signedness of.
     @return                                      Returns if @b Integer is negative or not
     */
    bool         IsNegative16(const int16_t Integer);
    
    /*!
     @abstract                                    Tells if the number is negative.
     @param      Integer                          The value to find the signedness of.
     @return                                      Returns if @b Integer is negative or not
     */
    bool         IsNegative32(const int32_t Integer);
    
    /*!
     @abstract                                    Tells if the number is negative.
     @param      Integer                          The value to find the signedness of.
     @return                                      Returns if @b Integer is negative or not
     */
    bool         IsNegative64(const int64_t Integer);
#ifdef           IsNegative
#undef           IsNegative
#define          IsNegative(Value)                _Generic((Value), int8_t:IsNegative8, uint8_t:IsNegative8, int16_t:IsNegative16, uint16_t:IsNegative16, int32_t:IsNegative32, uint32_t:IsNegative32, int64_t:IsNegative64, uint64_t:IsNegative64)(Value)
#endif
    
    /*!
     @abstract                                    Tells whether Integer is even or odd.
     @param      Integer                          The number to see if it's odd or even.
     @return                                      True for odd, false for even.
     */
    bool         IsOdd(const int64_t Integer);
    
    /*!
     @abstract                                    Converts an integer to an array of bytes.
     @param      Integer                          The integer to convert.
     @param      Bytes                            Allocated array of bytes to contain the output bytes.
     */
    void         GetBytesFromInteger(const uint64_t Integer, uint8_t *Bytes);
    
    /*!
     @abstract                                    Converts an array of bytes to an integer.
     @param      Bytes                            The array of bytes to convert.
     @return                                      Integer.
     */
    uint64_t     GetIntegerFromBytes(uint64_t *Bytes);
    
    /*!
     @abstract                                    Converts a Float to an integer.
     @param      Decimal                          The decimal to convert.
     @return                                      Integer representation of the decimal.
     */
    uint32_t     ConvertFloat2Integer(float Decimal);
    
    /*!
     @abstract                                    Converts a Double to an integer.
     @param      Decimal                          The decimal to convert.
     @return                                      Integer representation of the decimal.
     */
    uint64_t     ConvertDouble2Integer(double Decimal);
    
    /*!
     @abstract                                    Converts a Integer to a Float.
     @param      Integer                          The integer to convert.
     @return                                      Float representation of the integer.
     */
    float        ConvertInteger2Float(uint32_t Integer);
    
    /*!
     @abstract                                    Converts a Integer to a Double.
     @param      Integer                          The integer to convert.
     @return                                      Double representation of the integer.
     */
    double       ConvertInteger2Double(uint64_t Integer);
    
    /*!
     @abstract                                    Gets the absolute value of a number.
     @param      Integer                          The value to find the absolute value of.
     @return                                      Returns the absolute value of a number (removes the sign).
     */
    uint64_t     AbsoluteI(const int64_t Integer);
    
    /*!
     @abstract                                    Gets the absolute value of a number.
     @param      Decimal                          The value to find the absolute value of.
     @return                                      Returns the absolute value of a number (removes the sign).
     */
    uint8_t      AbsoluteF(const float Decimal);
    
    /*!
     @abstract                                    Gets the absolute value of a number.
     @param      Decimal                          The value to find the absolute value of.
     @return                                      Returns the absolute value of a number (removes the sign).
     */
    uint16_t     AbsoluteD(const double Decimal);
#ifdef           Absolute
#undef           Absolute
#endif
#define          Absolute(Value)                  _Generic((Value), int8_t:AbsoluteI, uint8_t:AbsoluteI, int16_t:AbsoluteI, uint16_t:AbsoluteI, int32_t:AbsoluteI, uint32_t:AbsoluteI, int64_t:AbsoluteI, uint64_t:AbsoluteI, float:AbsoluteF, double:AbsoluteD)(Value)
    
    /*!
     @abstract                                    Gets the Floor of Decimal.
     @param      Decimal                          The value to find the floor value of.
     @return                                      Returns the integer value of the float.
     */
    int16_t      FloorF(const float Decimal);
    
    /*!
     @abstract                                    Gets the Floor of Decimal.
     @param      Decimal                          The value to find the floor value of.
     @return                                      Returns the integer value of the float.
     */
    int32_t      FloorD(const double Decimal);
#ifdef           Floor
#undef           Floor
#endif
#define          Floor(Value)                     _Generic((Value), float:FloorF, double:FloorD)(Value)
    
    /*!
     @abstract                                    Gets the Ceil of Decimal.
     @param      Decimal                          The value to find the ceil value of.
     @return                                      Returns the Ceiling value of a decimal.
     */
    int16_t      CeilF(const float Decimal);
    
    /*!
     @abstract                                    Gets the Ceil of Decimal.
     @param      Decimal                          The value to find the ceil value of.
     @return                                      Returns the Ceiling value of a decimal.
     */
    int32_t      CeilD(const double Decimal);
    
#ifdef           Ceil
#undef           Ceil
#endif
#define          Ceil(Value)                      _Generic((Value), float:CeilF, double:CeilD)(Value)
    
    /*!
     @abstract                                    Rounds the Decimal
     @param      Decimal                          The value to find the round value of.
     @return                                      Returns the rounded value of a decimal.
     */
    int16_t      RoundF(const float Decimal);
    
    /*!
     @abstract                                    Rounds the Decimal
     @param      Decimal                          The value to find the round value of.
     @return                                      Returns the rounded value of a decimal.
     */
    int32_t      RoundD(const double Decimal);
#ifdef           Round
#undef           Round
#endif
#define          Round(Value)                     _Generic((Value), float:RoundF, double:RoundD)(Value)
    
    /*!
     @abstract                                    Branchless and shiftless Min function.
     @remark                                      Inspired by public domain Bit Twiddling Hacks.
     @param      Integer1                         The first integer to compare.
     @param      Integer2                         The second integer to compare.
     @return                                      Returns the smaller value.
     */
    int64_t      Minimum(const int64_t Integer1, const int64_t Integer2);
    
    /*!
     @abstract                                    Branchless and shiftless Max function.
     @remark                                      Inspired by public domain Bit Twiddling Hacks.
     @param      Integer1                         The first integer to compare.
     @param      Integer2                         The second integer to compare.
     @return                                      Returns the larger value.
     */
    int64_t      Maximum(const int64_t Integer1, const int64_t Integer2);

    /*!
     @abstract                                    Subtracts the minimum value from the maximum value.
     @remark                                      To help prevent under/over flows
     @param      Integer1                         The first integer to compare.
     @param      Integer2                         The second integer to compare.
     @return                                      Returns the larger value.
     */
    int64_t      Subtract(const int64_t Integer1, const int64_t Integer2);

    /*!
     @abstract                                    Takes the top bit and copies it to all higher bits.
     @param      HighestSetBit                    The highest set bit's index, starting at 1.
     @param      Integer                          The Integer to extend.
     @return                                      Returns the extended integger
     */
    int64_t       SignExtend(const uint8_t HighestSetBit, const int64_t Integer);
    
    /*!
     @abstract                                    Is the decimal normal?
     @param      Decimal                          The decimal to test for normalcy.
     @return                                      Returns true if the decimal is normal, otherwise false.
     */
    bool         DecimalIsNormalF(const float Decimal);
    
    /*!
     @abstract                                    Is the decimal normal?
     @param      Decimal                          The decimal to test for normalcy.
     @return                                      Returns true if the decimal is normal, otherwise false.
     */
    bool         DecimalIsNormalD(const double Decimal);
#ifdef           DecimalIsNormal
#undef           DecimalIsNormal
#endif
#define          DecimalIsNormal(Decimal)         _Generic((Decimal), float:DecimalIsNormalF, double:DecimalIsNormalD)(Decimal)
    
    /*!
     @abstract                                    Is the decimal +/- infinity?
     @param      Decimal                          The decimal to test for infinity.
     @return                                      Returns true if the decimal is infinite, otherwise false.
     */
    bool         DecimalIsInfinityF(const float Decimal);
    
    /*!
     @abstract                                    Is the decimal +/- infinity?
     @param      Decimal                          The decimal to test for infinity.
     @return                                      Returns true if the decimal is infinite, otherwise false.
     */
    bool         DecimalIsInfinityD(const double Decimal);
#ifdef           DecimalIsInfinity
#undef           DecimalIsInfinity
#endif
#define          DecimalIsInfinity(Decimal)       _Generic((Decimal), float:DecimalIsInfinityF, double:DecimalIsInfinityD)(Decimal)
    
    /*!
     @abstract                                    Is the decimal not a number?
     @param      Decimal                          The integer number you want to tell if it's a number or not.
     @return                                      Returns true if the decimal is not a number, otherwise false.
     */
    bool         DecimalIsANumberF(const float Decimal);
    
    /*!
     @abstract                                    Is the decimal not a number?
     @param      Decimal                          The integer number you want to tell if it's a number or not.
     @return                                      Returns true if the decimal is not a number, otherwise false.
     */
    bool         DecimalIsANumberD(const double Decimal);
#ifdef           DecimalIsANumber
#undef           DecimalIsANumber
#endif
#define          DecimalIsANumber(Decimal)     _Generic((Decimal), float:DecimalIsANumberF, double:DecimalIsANumberD)(Decimal)
    
    /*!
     @abstract                                    Does the decimal contain a decimal point?
     @param      Decimal                          The decimal you want to know if it contains all zeros for the fraction.
     @return                                      Returns true if the number's fraction bits are set, otherwise false.
     */
    bool         NumberHasDecimalPointF(const float Decimal);
    
    /*!
     @abstract                                    Does the decimal contain a decimal point?
     @param      Decimal                          The decimal you want to know if it contains all zeros for the fraction.
     @return                                      Returns true if the number's fraction bits are set, otherwise false.
     */
    bool         NumberHasDecimalPointD(const double Decimal);
#ifdef           NumberHasDecimalPoint
#undef           NumberHasDecimalPoint
#endif
#define          NumberHasDecimalPoint(Decimal)   _Generic((Decimal), float:NumberHasDecimalPointF, double:NumberHasDecimalPointD)(Decimal)
    
    /*!
     @abstract                                    Extracts the sign from the Number.
     @param       Number                          The Number you want to get the sign from.
     @return                                      Returns -1 if the sign is negative, otherwise 1.
     */
    int8_t       ExtractSignI(const int64_t Number);
    
    /*!
     @abstract                                    Extracts the sign from the Number.
     @param       Number                          The Number you want to get the sign from.
     @return                                      Returns -1 if the sign is negative, otherwise 1.
     */
    int8_t       ExtractSignF(const float Number);
    
    /*!
     @abstract                                    Extracts the sign from the Number.
     @param       Number                          The Number you want to get the sign from.
     @return                                      Returns -1 if the sign is negative, otherwise 1.
     */
    int8_t       ExtractSignD(const double Number);
#ifdef           ExtractSign
#undef           ExtractSign
#endif
#define          ExtractSign(Number)              _Generic((Number), int8_t:ExtractSignI, int16_t:ExtractSignI, int32_t:ExtractSignI, int64_t:ExtractSignI, float:ExtractSignF, double:ExtractSignD)(Number)
    
    /*!
     @abstract                                    Extracts the exponent from the given Decimal.
     @param      Decimal                          The decimal number you want to get the exponent from.
     */
    int16_t      ExtractExponentF(const float Decimal);
    
    /*!
     @abstract                                    Extracts the exponent from the given Decimal.
     @param      Decimal                          The decimal number you want to get the exponent from.
     */
    int32_t      ExtractExponentD(const double Decimal);
#ifdef           ExtractExponent
#undef           ExtractExponent
#endif
#define          ExtractExponent(Decimal)         _Generic((Decimal), float:ExtractExponentF, double:ExtractExponentD)(Decimal)
    
    /*!
     @abstract                                    Extracts the mantissa from the float given as Decimal.
     @param      Decimal                          The mantissa number you want to get the exponent from.
     */
    int32_t      ExtractMantissaF(const float Decimal);
    
    /*!
     @abstract                                    Extracts the mantissa from the float given as Decimal.
     @param      Decimal                          The mantissa number you want to get the exponent from.
     */
    int64_t      ExtractMantissaD(const double Decimal);
#ifdef           ExtractMantissa
#undef           ExtractMantissa
#endif
#define          ExtractMantissa(Decimal)         _Generic((Decimal), float:ExtractMantissaF, double:ExtractMantissaD)(Decimal)
    
    /*!
     @abstract                                    Inserts the sign to the float given as Decimal.
     @param      Decimal                          The Decimal you want to edit the sign of.
     @param      Sign                             The sign to insert.
     @return                                      Returns the edited decimal.
     */
    float        InsertSignF(const float Decimal, const int8_t Sign);
    
    /*!
     @abstract                                    Inserts the sign to the float given as Decimal.
     @param      Decimal                          The Decimal you want to edit the sign of.
     @param      Sign                             The sign to insert.
     @return                                      Returns the edited decimal.
     */
    double       InsertSignD(const double Decimal, const int8_t Sign);
#ifdef           InsertSign
#undef           InsertSign
#endif
#define          InsertSign(Decimal)              _Generic((Decimal), float:ExtractMantissaF, double:ExtractMantissaD)(Decimal)
    
    /*!
     @abstract                                    Inserts the exponent to the float given as Decimal.
     @param      Decimal                          The Decimal you want to edit the exponent of.
     @param      Exponent                         The exponent to insert.
     @return                                      Returns the edited decimal.
     */
    float        InsertExponentF(const float Decimal, const int8_t Exponent);
    
    /*!
     @abstract                                    Inserts the exponent to the float given as Decimal.
     @param      Decimal                          The Decimal you want to edit the exponent of.
     @param      Exponent                         The exponent to insert.
     @return                                      Returns the edited decimal.
     */
    double       InsertExponentD(const double Decimal, const int16_t Exponent);
#ifdef           InsertExponent
#undef           InsertExponent
#endif
#define          InsertExponent(Decimal, Exponent) _Generic((Decimal), float:InsertExponentF, double:InsertExponentD)(Decimal, Exponent)
    
    /*!
     @abstract                                    Inserts the mantissa to the float given as Decimal.
     @param      Decimal                          The Decimal you want to edit the mantissa of.
     @param      Mantissa                         The mantissa to insert.
     @return                                      Returns the edited decimal.
     */
    float        InsertMantissaF(const float Decimal, const uint32_t Mantissa);
    
    /*!
     @abstract                                    Inserts the mantissa to the float given as Decimal.
     @param      Decimal                          The Decimal you want to edit the mantissa of.
     @param      Mantissa                         The mantissa to insert.
     @return                                      Returns the edited decimal.
     */
    double       InsertMantissaD(const double Decimal, const uint64_t Mantissa);
#ifdef           InsertMantissa
#undef           InsertMantissa
#endif
#define          InsertMantissa(Decimal, Mantissa) _Generic((Decimal), float:InsertMantissaF, double:InsertMantissaD)(Decimal, Mantissa)
    
    /*!
     @abstract                                    Calculates the value of Base raised to Exponent's power (an integer version of the pow function).
     @param      Base                             What base should the power be calculated in?
     @param      Exponent                         How many times should the Base be raised?
     @return                                      Returns the result of 1 *= Base, Exponent times.
     */
    int64_t      Exponentiate(const uint64_t Base, const int64_t Exponent);
    
    /*!
     @abstract                                    Computes the number of times Exponent fits into Base.
     @remark                                      Rounds up to the next integer number of symbols.
     @param      Base                             The base, or radix to contain the symbol.
     @param      Exponent                         The value to calculate.
     @return                                      Returns the number of symbols required to store an integer.
     */
    int64_t      Logarithm(const int64_t Base, const int64_t Exponent);
    
    /*!
     @abstract                                    Byte swaps an integer.
     @param      Value2Swap                       Data to swap endian.
     @return                                      Returns swapped data.
     */
    uint16_t     SwapEndian16(const uint16_t Value2Swap);
    
    /*!
     @abstract                                    Byte swaps an integer.
     @param      Value2Swap                       Data to swap endian.
     @return                                      Returns swapped data.
     */
    uint32_t     SwapEndian32(const uint32_t Value2Swap);
    
    /*!
     @abstract                                    Byte swaps an integer.
     @param      Value2Swap                       Data to swap endian.
     @return                                      Returns swapped data.
     */
    uint64_t     SwapEndian64(const uint64_t Value2Swap);
#ifdef           SwapEndian
#undef           SwapEndian
#endif
#define          SwapEndiamn(Value2Swap)          _Generic((Value2Swap), int16_t:SwapEndian16, uint16_t:SwapEndian16, int32_t:SwapEndian32, uint32_t:SwapEndian32, int64_t:SwapEndian64, uint64_t:SwapEndian64)(Value2Swap)
    
    /*!
     @abstract                                    Computes the number of bits from the number of bytes.
     @param      Bytes                            The number of bytes you want to interpret as bits.
     @return                                      Returns the number of bits.
     */
    int64_t      Bytes2Bits(const int64_t Bytes);
    
    /*!
     @abstract                                    Computes the number of bytes from the number of bits.
     @param      RoundingType                     The type of rounding to do.
     @param      Bits                             The bits to convert to bytes.
     @return                                      Returns the number of bytes.
     */
    uint64_t     Bits2Bytes(const MathIO_RoundingTypes RoundingType, const uint64_t Bits);
    
    /*!
     @abstract                                    Calculates the number of bits of slack mod 8.
     @param      Offset                           The offset to calculate the number of bits unaccounted for in the byte.
     @return                                      Returns the number of unaccounted for bits.
     */
    uint8_t      BitsAvailableInByte(const uint64_t Offset);
    
    /*!
     @abstract                                    Creates a bitmask.
     @param      NumBits2Select                   The number of bits to select.
     @return                                      Returns the mask.
     */
    uint8_t      CreateBitMaskLSBit(const uint8_t NumBits2Select);
    
    /*!
     @abstract                                    Creates a bitmask.
     @param      NumBits2Select                   The number of bits to select.
     @return                                      Returns the mask.
     */
    uint8_t      CreateBitMaskMSBit(const uint8_t NumBits2Select);
    
    /*!
     @abstract                                    Counts the number of bits set to 1 in Value.
     @param      Value                            The number of bits to select.
     @return                                      Returns the number of set bits.
     */
    uint8_t      CountBitsSet(const uint64_t Value);
    
    /*!
     @abstract                                    Value Mod Base, if the remainder is 0 Value is a power of the base.
     @param      Base                             The base to check.
     @param      Value                            The value to check.
     @return                                      Returns true if there's no remainder, otherwise returns false.
     */
    bool         IsPowerOfBase(const uint8_t Base, const uint64_t Value);
    
    /*!
     @abstract                                    Value Mod Base, if the remainder is 0 Value is a power of the base.
     @param      Base                             The base to check.
     @param      Integer                          The value to check.
     @return                                      Returns true if there's no remainder, otherwise returns false.
     */
    uint8_t      NumDigitsInInteger(const uint8_t Base, const int64_t Integer);
    
    /*!
     @abstract                                    Rotates Value by Bits2Rotate.
     @param      RotationType                     The direction to rotate.
     @param      NumBits2Rotate                   The number of bits to rotate.
     @param      Value                            The value to rotate.
     @return                                      Returns the rotated value.
     */
    uint64_t      Rotate(const MathIO_RotationTypes RotationType, const uint8_t NumBits2Rotate, const uint64_t Value);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_MathIO_H */
