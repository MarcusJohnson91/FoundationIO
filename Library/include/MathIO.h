#include <stdbool.h>
#include <stdint.h>

#if   (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L)
#include <tgmath.h>
#elif (defined __STDC_NO_COMPLEX__)
#include <math.h>
#endif /* __STDC_VERSION__ */

#pragma  once

#ifndef  FoundationIO_Math_H
#define  FoundationIO_Math_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header                    Math.h
     @author                    Marcus Johnson
     @copyright                 2017+
     @version                   1.0.0
     @brief                     This header contains code for specific mathematical functions used in FoundationIO and it's consumers.
     */
    
    /*!
     @abstract                                                  "Tells if the number is negative".
     @param                     Integer                         "The value to find the signedness of".
     */
    bool                        IsNegative(int64_t Integer);
    
    /*!
     @abstract                                                  "Tells whether Integer is even or odd".
     @param                     Integer                         "The number to see if it's odd or even".
     @return                                                    "True for odd, false for even".
     */
    bool                        IsOdd(int64_t Integer);
    
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
     @abstract                                                  "Returns the absolute value of an integer (removes the sign)".
     @param                     Integer                         "The value to find the absolute value of".
     */
    uint64_t                    AbsoluteI(int64_t Integer);
    
    /*!
     @abstract                                                  "Returns the absolute value of an integer (removes the sign)".
     @param                     Decimal                         "The value to find the absolute value of".
     */
    uint8_t                     AbsoluteF(float Decimal);
    
    /*!
     @abstract                                                  "Returns the absolute value of an integer (removes the sign)".
     @param                     Decimal                         "The value to find the absolute value of".
     */
    uint16_t                    AbsoluteD(double Decimal);
    
    /*!
     @abstract                                                  "Returns the Floor value of a decimal".
     @param                     Decimal                         "The value to find the floor value of".
     */
    int64_t                     FloorF(float Decimal);
    
    /*!
     @abstract                                                  "Returns the Floor value of a decimal".
     @param                     Decimal                         "The value to find the floor value of".
     */
    int64_t                     FloorD(double Decimal);
    
    /*!
     @abstract                                                  "Returns the Ceiling value of a decimal".
     @param                     Decimal                         "The value to find the ceil value of".
     */
    int64_t                     CeilF(float Decimal);
    
    /*!
     @abstract                                                  "Returns the Ceiling value of a decimal".
     @param                     Decimal                         "The value to find the ceil value of".
     */
    int64_t                     CeilD(double Decimal);
    
    /*!
     @abstract                                                  "Returns the rounded value of a decimal".
     @param                     Decimal                         "The value to find the round value of".
     */
    int64_t                     RoundF(float Decimal);
    
    /*!
     @abstract                                                  "Returns the rounded value of a decimal".
     @param                     Decimal                         "The value to find the round value of".
     */
    int64_t                     RoundD(double Decimal);
    
#ifdef Min
#undef Min
#endif
    
    /*!
     @abstract                                                  "Branchless and shiftless Min function".
     @return                                                    "Returns the smaller value".
     */
    int64_t                     Min(int64_t Integer1, int64_t Integer2);
    
#ifdef Max
#undef Max
#endif
    
    /*!
     @abstract                                                  "Branchless and shiftless Max function".
     @return                                                    "Returns the larger value".
     */
    int64_t                     Max(int64_t Integer1, int64_t Integer2);
    
    /*!
     @abstract                                                  "Is the decimal normal"?
     @param                     Decimal                         "The decimal to test for normalcy".
     @return                                                    "Returns true if the decimal is normal, otherwise false".
     */
    bool                        DecimalIsNormalF(float Decimal);
    
    /*!
     @abstract                                                  "Is the decimal normal"?
     @param                     Decimal                         "The decimal to test for normalcy".
     @return                                                    "Returns true if the decimal is normal, otherwise false".
     */
    bool                        DecimalIsNormalD(double Decimal);
    
    /*!
     @abstract                                                  "Is the decimal +/- infinity"?
     @param                     Decimal                         "The decimal to test for infinity".
     @return                                                    "Returns true if the decimal is infinite, otherwise false".
     */
    bool                        DecimalIsInfinityF(float Decimal);
    
    /*!
     @abstract                                                  "Is the decimal +/- infinity"?
     @param                     Decimal                         "The decimal to test for infinity".
     @return                                                    "Returns true if the decimal is infinite, otherwise false".
     */
    bool                        DecimalIsInfinityD(double Decimal);
    
    /*!
     @abstract                                                  "Is the decimal not a number"?
     @param                     Decimal                         "The integer number you want to tell if it's a number or not".
     @return                                                    "Returns true if the decimal is not a number, otherwise false".
     */
    bool                        DecimalIsNotANumberF(float Decimal);
    
    /*!
     @abstract                                                  "Is the decimal not a number"?
     @param                     Decimal                         "The integer number you want to tell if it's a number or not".
     @return                                                    "Returns true if the decimal is not a number, otherwise false".
     */
    bool                        DecimalIsNotANumberD(double Decimal);
    
    /*!
     @abstract                                                  "Does the decimal contain a decimal point"?
     @param                     Decimal                         "The decimal you want to know if it contains all zeros for the fraction".
     @return                                                    "Returns true if the number's fraction bits are set, otherwise false".
     */
    bool                        NumberHasDecimalPointF(float Decimal);
    
    /*!
     @abstract                                                  "Does the decimal contain a decimal point"?
     @param                     Decimal                         "The decimal you want to know if it contains all zeros for the fraction".
     @return                                                    "Returns true if the number's fraction bits are set, otherwise false".
     */
    bool                        NumberHasDecimalPointD(double Decimal);
    
    /*!
     @abstract                                                  "Extracts the sign from the integer given as Decimal".
     @param                     Integer                         "The integer number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
    int8_t                      ExtractSignI(int64_t Integer);
    
    /*!
     @abstract                                                  "Extracts the sign from the float given as Decimal".
     @param                     Decimal                         "The float number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
    int8_t                      ExtractSignF(float Decimal);
    
    /*!
     @abstract                                                  "Extracts the sign from the double given as Decimal".
     @param                     Decimal                         "The double number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
    int8_t                      ExtractSignD(double Decimal);
    
    /*!
     @abstract                                                  "Extracts the exponent from the float given as Decimal".
     @param                     Decimal                         "The decimal number you want to get the exponent from".
     */
    int8_t                      ExtractExponentF(float Decimal);
    
    /*!
     @abstract                                                  "Extracts the exponent from the double given as Decimal".
     @param                     Decimal                         "The decimal number you want to get the exponent from".
     */
    int16_t                     ExtractExponentD(double Decimal);
    
    /*!
     @abstract                                                  "Extracts the mantissa from the float given as Decimal".
     @param                     Decimal                         "The mantissa number you want to get the exponent from".
     */
    int32_t                     ExtractMantissaF(float Decimal);
    
    /*!
     @abstract                                                  "Extracts the mantissa from the double given as Decimal".
     @param                     Decimal                         "The mantissa number you want to get the exponent from".
     */
    int64_t                     ExtractMantissaD(double Decimal);
    
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
     @abstract                                                  "Byte swaps a 16 bit integer".
     @param                     Value2Swap                      "Data to swap endian".
     @return                                                    "Returns swapped uint16_t".
     */
    uint16_t                    SwapEndian16(uint16_t Value2Swap);
    
    /*!
     @abstract                                                  "Byte swaps a 32 bit integer".
     @param                     Value2Swap                      "Data to swap endian".
     @return                                                    "Returns swapped uint32_t".
     */
    uint32_t                    SwapEndian32(uint32_t Value2Swap);
    
    /*!
     @abstract                                                  "Byte swaps a 64 bit integer".
     @param                     Value2Swap                      "Data to swap endian".
     @return                                                    "Returns swapped uint64_t".
     */
    uint64_t                    SwapEndian64(uint64_t Value2Swap);
    
    /*!
     @abstract                                                  "Computes the number of bits from the number of bytes".
     @param                     Bytes                           "The number of bytes you want to interpret as bits".
     @return                                                    "Returns the number of bits".
     */
    int64_t                     Bytes2Bits(int64_t Bytes);
    
    /*!
     @abstract                                                  "Computes the number of bytes from the number of bits".
     @param                     Bits                            "The bits to convert to bytes".
     @param                     RoundUp                         "Should the resulting bytes be rounded up or down?".
     @return                                                    "Returns the number of bytes".
     */
    int64_t                     Bits2Bytes(int64_t Bits, bool RoundUp);
    
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
    uint8_t                     CreateBitMaskLSBit(const uint8_t NumBits2Select);
    
    /*!
     @abstract                                                  "Creates a bitmask".
     @param                     NumBits2Select                  "The number of bits to select".
     @return                                                    "Returns the mask".
     */
    uint8_t                     CreateBitMaskMSBit(const uint8_t NumBits2Select);
    
    /*!
     @abstract                                                  "Value Mod Base, if the remainder is 0 Value is a power of the base".
     @param                     Base                            "The base to check".
     @param                     Value                           "The value to check".
     @return                                                    "Returns true if there's no remainder, otherwise returns false".
     */
    bool                        IsPowerOfBase(uint8_t Base, uint64_t Value);
    
    /*!
     @abstract                                                  "Gets the number of digits nessicary to store Value in Base".
     @param                     Base                            "The base to check".
     @param                     Value                           "The value to check".
     @return                                                    "Returns the number of digits".
     */
    uint8_t                     GetNumDigitsInBase(uint8_t Base, int64_t Value);
    
#define Absolute(Value)                     _Generic((Value), uint8_t:AbsoluteI, int8_t:AbsoluteI, uint16_t:AbsoluteI, int16_t:AbsoluteI, uint32_t:AbsoluteI, int32_t:AbsoluteI, uint64_t:AbsoluteI, int64_t:AbsoluteI, float:AbsoluteF, double:AbsoluteD)(Value)
#define Floor(Value)                        _Generic((Value),   float:FloorF, double:FloorD)(Value)
#define Ceil(Value)                         _Generic((Value),   float:CeilF, double:CeilD)(Value)
#define Round(Value)                        _Generic((Value),   float:RoundF, double:RoundD)(Value)
#define DecimalIsNormal(Decimal)            _Generic((Decimal), float:DecimalIsNormalF, double:DecimalIsNormalD)(Decimal)
#define DecimalIsInfinity(Decimal)          _Generic((Decimal), float:DecimalIsInfinityF, double:DecimalIsInfinityD)(Decimal)
#define DecimalIsNotANumber(Decimal)        _Generic((Decimal), float:DecimalIsNotANumberF, double:DecimalIsNotANumberD)(Decimal)
#define NumberHasDecimalPoint(Decimal)      _Generic((Decimal), float:NumberHasDecimalPointF, double:NumberHasDecimalPointD)(Decimal)
#define ExtractSign(Decimal)                _Generic((Decimal), int8_t:ExtractSignI, int16_t:ExtractSignI, int32_t:ExtractSignI, int64_t:ExtractSignI, float:ExtractSignF,     double:ExtractSignD)(Decimal)
#define ExtractExponent(Decimal)            _Generic((Decimal), float:ExtractExponentF, double:ExtractExponentD)(Decimal)
#define ExtractMantissa(Decimal)            _Generic((Decimal), float:ExtractMantissaF, double:ExtractMantissaD)(Decimal)
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_Math_H */
