#include "../include/Macros.h"

#if   (FoundationIOStandardVersion >= FoundationIOStandardVersionC99)
#include <tgmath.h>
#if (defined __STDC_NO_COMPLEX__)
#include <math.h>
#elif (defined __STDC_COMPLEX__)
#include <math.h>
#include <complex.h>
#endif /* Complex */
#endif /* Version check */

/*!
 @header              MathIO.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for specific mathematical functions used in FoundationIO and it's consumers.
 */

#pragma  once

#ifndef  FoundationIO_MathIO_H
#define  FoundationIO_MathIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum                      RoundingTypes
     @abstract                                                  "Defines the type of flipping".
     @constant                  RoundingType_Unknown            "Invalid ImageType, exists solely to tell when it hasn't been set".
     @constant                  RoundingType_Down               "Flip the imge vertically, up and down".
     @constant                  RoundingType_Up                 "Flip the image horizontally, side to side".
     */
    typedef enum RoundingTypes {
                                RoundingType_Unknown            = 0,
                                RoundingType_Down               = 1,
							    RoundingType_Up                 = 2,
    } RoundingTypes;
    
    /*!
     @abstract                                                  "Tells if the number is negative".
     @param                     Integer                         "The value to find the signedness of".
     */
#ifdef                          IsNegative
#undef                          IsNegative
#endif
    bool                        IsNegative8(int8_t Integer);
    bool                        IsNegative16(int16_t Integer);
    bool                        IsNegative32(int32_t Integer);
    bool                        IsNegative64(int64_t Integer);
#define                         IsNegative(Integer) _Generic(Integer, int8_t:IsNegative8, uint8_t:IsNegative8, int16_t:IsNegative16, uint16_t:IsNegative16, int32_t:IsNegative32, uint32_t:IsNegative32, int64_t:IsNegative64, uint64_t:IsNegative64)(Integer)
    
    /*!
     @abstract                                                  "Tells whether Integer is even or odd".
     @param                     Integer                         "The number to see if it's odd or even".
     @return                                                    "True for odd, false for even".
     */
    bool                        IsOdd(int64_t Integer);
    
    /*!
     @abstract                                                  "Converts an integer to an array of bytes".
     @param                     Integer                         "The integer to convert".
     @param                     Bytes                           "Allocated array of bytes to contain the output bytes".
     */
    void                        GetBytesFromInteger(uint64_t Integer, uint8_t *Bytes);
    
    /*!
     @abstract                                                  "Converts an array of bytes to an integer".
     @param                     Bytes                           "The array of bytes to convert".
     @return                                                    "Integer".
     */
    uint64_t                    GetIntegerFromBytes(uint64_t *Bytes);
    
    /*!
     @abstract                                                  "Converts a Float to an integer".
     @param                     Decimal                         "The decimal to convert".
     @return                                                    "Integer representation of the decimal".
     */
    uint32_t                    ConvertFloat2Integer(float Decimal);
    
    /*!
     @abstract                                                  "Converts a Double to an integer".
     @param                     Decimal                         "The decimal to convert".
     @return                                                    "Integer representation of the decimal".
     */
    uint64_t                    ConvertDouble2Integer(double Decimal);
    
    /*!
     @abstract                                                  "Converts a Integer to a Float".
     @param                     Integer                         "The integer to convert".
     @return                                                    "float representation of the integer".
     */
    float                       ConvertInteger2Float(uint32_t Integer);
    
    /*!
     @abstract                                                  "Converts a Integer to a Double".
     @param                     Integer                         "The integer to convert".
     @return                                                    "double representation of the integer".
     */
    double                      ConvertInteger2Double(uint64_t Integer);
    
    /*!
     @abstract                                                  "Returns the absolute value of a number (removes the sign)".
     @param                     Integer                         "The value to find the absolute value of".
     */
#ifdef                          Absolute
#undef                          Absolute
#endif
    uint64_t                    AbsoluteI(int64_t Integer);
    uint8_t                     AbsoluteF(float Decimal);
    uint16_t                    AbsoluteD(double Decimal);
#define                         Absolute(Value) _Generic(Value, int8_t:AbsoluteI, uint8_t:AbsoluteI, int16_t:AbsoluteI, uint16_t:AbsoluteI, int32_t:AbsoluteI, uint32_t:AbsoluteI, int64_t:AbsoluteI, uint64_t:AbsoluteI, float:AbsoluteF, double:AbsoluteD)(Value)
    
    /*!
     @abstract                                                  "Returns the Floor value of a decimal".
     @param                     Decimal                         "The value to find the floor value of".
     */
#ifdef                          Floor
#undef                          Floor
#endif
    int64_t                     FloorF(float Decimal);
    int64_t                     FloorD(double Decimal);
#define                         Floor(Value)                   _Generic((Value), float:FloorF, double:FloorD)(Value)
    
    /*!
     @abstract                                                  "Returns the Ceiling value of a decimal".
     @param                     Decimal                         "The value to find the ceil value of".
     */
#ifdef                          Ceil
#undef                          Ceil
#endif
    int64_t                     CeilF(float Decimal);
    int64_t                     CeilD(double Decimal);
#define                         Ceil(Value)                    _Generic((Value), float:CeilF, double:CeilD)(Value)
    
    /*!
     @abstract                                                  "Returns the rounded value of a decimal".
     @param                     Decimal                         "The value to find the round value of".
     */
#ifdef                          Round
#undef                          Round
#endif
    int64_t                     RoundF(float Decimal);
    int64_t                     RoundD(double Decimal);
#define                         Round(Value)                   _Generic((Value), float:RoundF, double:RoundD)(Value)
    
    /*!
     @abstract                                                  "Branchless and shiftless Min function".
     @return                                                    "Returns the smaller value".
     */
#ifdef                          Minimum
#undef                          Minimum
#endif
    int64_t                     Minimum(int64_t Integer1, int64_t Integer2);
    
    /*!
     @abstract                                                  "Branchless and shiftless Max function".
     @return                                                    "Returns the larger value".
     */
#ifdef                          Maximum
#undef                          Maximum
#endif
    int64_t                     Maximum(int64_t Integer1, int64_t Integer2);
    
    /*!
     @abstract                                                  "Is the decimal normal"?
     @param                     Decimal                         "The decimal to test for normalcy".
     @return                                                    "Returns true if the decimal is normal, otherwise false".
     */
#ifdef                          DecimalIsNormal
#undef                          DecimalIsNormal
#endif
    bool                        DecimalIsNormalF(float Decimal);
    bool                        DecimalIsNormalD(double Decimal);
#define                         DecimalIsNormal(Decimal)       _Generic((Decimal), float:DecimalIsNormalF, double:DecimalIsNormalD)(Decimal)
    
    /*!
     @abstract                                                  "Is the decimal +/- infinity"?
     @param                     Decimal                         "The decimal to test for infinity".
     @return                                                    "Returns true if the decimal is infinite, otherwise false".
     */
#ifdef                          DecimalIsInfinity
#undef                          DecimalIsInfinity
#endif
    bool                        DecimalIsInfinityF(float Decimal);
    bool                        DecimalIsInfinityD(double Decimal);
#define                         DecimalIsInfinity(Decimal)     _Generic((Decimal), float:DecimalIsInfinityF, double:DecimalIsInfinityD)(Decimal)
    
    /*!
     @abstract                                                  "Is the decimal not a number"?
     @param                     Decimal                         "The integer number you want to tell if it's a number or not".
     @return                                                    "Returns true if the decimal is not a number, otherwise false".
     */
#ifdef                          DecimalIsNotANumber
#undef                          DecimalIsNotANumber
#endif
    bool                        DecimalIsNotANumberF(float Decimal);
    bool                        DecimalIsNotANumberD(double Decimal);
#define                         DecimalIsNotANumber(Decimal)   _Generic((Decimal), float:DecimalIsNotANumberF, double:DecimalIsNotANumberD)(Decimal)

    /*!
     @abstract                                                  "Does the decimal contain a decimal point"?
     @param                     Decimal                         "The decimal you want to know if it contains all zeros for the fraction".
     @return                                                    "Returns true if the number's fraction bits are set, otherwise false".
     */
#ifdef                          DecimalIsNotANumber
#undef                          DecimalIsNotANumber
#endif
    bool                        NumberHasDecimalPointF(float Decimal);
    bool                        NumberHasDecimalPointD(double Decimal);
#define                         NumberHasDecimalPoint(Decimal) _Generic((Decimal), float:NumberHasDecimalPointF, double:NumberHasDecimalPointD)(Decimal)
    
    /*!
     @abstract                                                  "Extracts the sign from the Number".
     @param                     Number                          "The Number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
#ifdef                          ExtractSign
#undef                          ExtractSign
#endif
    int8_t                      ExtractSignI(int64_t Number);
    int8_t                      ExtractSignF(float Number);
    int8_t                      ExtractSignD(double Number);
#define                         ExtractSign(Number)           _Generic(Number, int8_t:ExtractSignI, int16_t:ExtractSignI, int32_t:ExtractSignI, int64_t:ExtractSignI, float:ExtractSignF, double:ExtractSignD)(Number)
    
    /*!
     @abstract                                                  "Extracts the exponent from the given Decimal".
     @param                     Decimal                         "The decimal number you want to get the exponent from".
     */
#ifdef                          ExtractExponent
#undef                          ExtractExponent
#endif
    int8_t                      ExtractExponentF(float Decimal);
    int16_t                     ExtractExponentD(double Decimal);
#define                         ExtractExponent(Decimal)       _Generic(Decimal, float:ExtractExponentF, double:ExtractExponentD)(Decimal)
    
    /*!
     @abstract                                                  "Extracts the mantissa from the float given as Decimal".
     @param                     Decimal                         "The mantissa number you want to get the exponent from".
     */
#ifdef                          ExtractMantissa
#undef                          ExtractMantissa
#endif
    int32_t                     ExtractMantissaF(float Decimal);
    int64_t                     ExtractMantissaD(double Decimal);
#define                         ExtractMantissa(Decimal)       _Generic((Decimal), float:ExtractMantissaF, double:ExtractMantissaD)(Decimal)
    
    /*!
     @abstract                                                  "Inserts the sign to the float given as Decimal".
     @param                     Decimal                         "The Decimal you want to edit the sign of".
     @param                     Sign                            "The sign to insert".
     @return                                                    "Returns the edited decimal".
     */
#ifdef                          InsertSign
#undef                          InsertSign
#endif
    float                       InsertSignF(float Decimal, int8_t Sign);
    double                      InsertSignD(double Decimal, int8_t Sign);
#define                         InsertSign(Decimal)       _Generic((Decimal), float:ExtractMantissaF, double:ExtractMantissaD)(Decimal)

    /*!
     @abstract                                                  "Inserts the exponent to the float given as Decimal".
     @param                     Decimal                         "The Decimal you want to edit the exponent of".
     @param                     Exponent                        "The exponent to insert".
     @return                                                    "Returns the edited decimal".
     */
#ifdef                          InsertExponent
#undef                          InsertExponent
#endif
    float                       InsertExponentF(float Decimal, int8_t Exponent);
    double                      InsertExponentD(double Decimal, int16_t Exponent);
#define                         InsertExponent(Decimal, Exponent) _Generic(Decimal, float:InsertExponentF, double:InsertExponentD)(Decimal, Exponent)
    
    /*!
     @abstract                                                  "Inserts the mantissa to the float given as Decimal".
     @param                     Decimal                         "The Decimal you want to edit the mantissa of".
     @param                     Mantissa                        "The mantissa to insert".
     @return                                                    "Returns the edited decimal".
     */
#ifdef                          InsertMantissa
#undef                          InsertMantissa
#endif
    float                       InsertMantissaF(float Decimal, uint32_t Mantissa);
    double                      InsertMantissaD(double Decimal, uint64_t Mantissa);
#define                         InsertMantissa(Decimal, Mantissa) _Generic(Decimal, float:InsertMantissaF, double:InsertMantissaD)(Decimal, Mantissa)
    
    /*!
     @abstract                                                  "Calculates the value of Base raised to Exponent's power (an integer version of the pow function)".
     @param                     Base                            "What base should the power be calculated in"?
     @param                     Exponent                        "How many times should the Base be raised"?
     @return                                                    "Returns the result of 1 *= Base, Exponent times".
     */
    uint64_t                    Exponentiate(uint64_t Base, uint64_t Exponent);
    
    /*!
     @abstract                                                  "Computes the number of times Exponent fits into Base".
     @remark                                                    "Rounds up to the next integer number of symbols".
     @param                     Base                            "The base, or radix to contain the symbol".
     @param                     Exponent                        "The value to calculate".
     @return                                                    "Returns the number of symbols required to store an integer".
     */
    int64_t                     Logarithm(int64_t Base, int64_t Exponent);
    
    /*!
     @abstract                                                  "Byte swaps an integer".
     @param                     Value2Swap                      "Data to swap endian".
     @return                                                    "Returns swapped data".
     */
#ifdef                          SwapEndian
#undef                          SwapEndian
#endif
    uint16_t                    SwapEndian16(uint16_t Value2Swap);
    uint32_t                    SwapEndian32(uint32_t Value2Swap);
    uint64_t                    SwapEndian64(uint64_t Value2Swap);
#define                         SwapEndiamn(Value2Swap)        _Generic(Value2Swap, int16_t:SwapEndian16, uint16_t:SwapEndian16, int32_t:SwapEndian32, uint32_t:SwapEndian32, int64_t:SwapEndian64, uint64_t:SwapEndian64)(Value2Swap)
    
    /*!
     @abstract                                                  "Computes the number of bits from the number of bytes".
     @param                     Bytes                           "The number of bytes you want to interpret as bits".
     @return                                                    "Returns the number of bits".
     */
    int64_t                     Bytes2Bits(int64_t Bytes);
    
    /*!
     @abstract                                                  "Computes the number of bytes from the number of bits".
     @param                     Bits                            "The bits to convert to bytes".
     @param                     RoundingType                    "The type of rounding to do".
     @return                                                    "Returns the number of bytes".
     */
    uint64_t                    Bits2Bytes(uint64_t Bits, RoundingTypes RoundingType);
    
    /*!
     @abstract                                                  "Swaps bits so the the MSB becoems the LSB, and vice versa".
     @param                     Byte                            "The byte to swap the bits in".
     @return                                                    "Returns the swapped byte".
     */
    uint8_t                     SwapBits(uint8_t Byte);
    
    /*!
     @abstract                                                  "Calculates the number of bits of slack mod 8".
     @param                     Offset                          "The offset to calculate the number of bits unaccounted for in the byte".
     @return                                                    "Returns the number of unaccounted for bits".
     */
    uint8_t                     Bits2ExtractFromByte(uint64_t Offset);
    
    /*!
     @abstract                                                  "Creates a bitmask".
     @param                     NumBits2Select                  "The number of bits to select".
     @return                                                    "Returns the mask".
     */
    uint8_t                     CreateBitMaskLSBit(uint8_t NumBits2Select);
    
    /*!
     @abstract                                                  "Creates a bitmask".
     @param                     NumBits2Select                  "The number of bits to select".
     @return                                                    "Returns the mask".
     */
    uint8_t                     CreateBitMaskMSBit(uint8_t NumBits2Select);
    
    /*!
     @abstract                                                  "Counts the number of bits set to 1 in Value".
     @param                     Value                           "The number of bits to select".
     @return                                                    "Returns the number of set bits".
     */
    uint8_t                     CountBitsSet(uint64_t Value);
    
    /*!
     @abstract                                                  "Value Mod Base, if the remainder is 0 Value is a power of the base".
     @param                     Base                            "The base to check".
     @param                     Value                           "The value to check".
     @return                                                    "Returns true if there's no remainder, otherwise returns false".
     */
    bool                        IsPowerOfBase(uint8_t Base, uint64_t Value);
    
    /*!
     @abstract                                                  "Value Mod Base, if the remainder is 0 Value is a power of the base".
     @param                     Base                            "The base to check".
     @param                     Integer                         "The value to check".
     @return                                                    "Returns true if there's no remainder, otherwise returns false".
     */
    uint8_t                     NumDigitsInInteger(uint8_t Base, int64_t Integer);
    
    /*!
     @abstract                                                  "Rotates Value by Bits2Rotate".
     @param                     Value                           "The value to rotate".
     @param                     Bits2Rotate                     "The number of bits to rotate".
     @return                                                    "Returns the rotated value".
     */
    uint64_t                    RotateLeft(uint64_t Value, uint8_t Bits2Rotate);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_MathIO_H */
