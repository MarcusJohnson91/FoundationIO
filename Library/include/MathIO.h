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
     @abstract                                                  "Tells whether Input is even or odd".
     @param                     Number2Check                    "The number to see if it's odd or even".
     @return                                                    "True for odd, false for even".
     */
    bool                        IsOdd(int64_t Number2Check);
    
    /*!
     @abstract                                                  "Returns the absolute value of an integer (removes the sign)".
     @param                     Value                           "The value to find the absolute value of".
     */
    uint64_t                    AbsoluteI(int64_t Value);
    
    /*!
     @abstract                                                  "Returns the absolute value of an integer (removes the sign)".
     @param                     Value                           "The value to find the absolute value of".
     */
    uint64_t                    AbsoluteF(float Value);
    
    /*!
     @abstract                                                  "Returns the absolute value of an integer (removes the sign)".
     @param                     Value                           "The value to find the absolute value of".
     */
    uint64_t                    AbsoluteD(double Value);
    
    /*!
     @abstract                                                  "Returns the Floor value of a decimal".
     @param                     Value                           "The value to find the floor value of".
     */
    int64_t                     FloorF(float Value);
    
    /*!
     @abstract                                                  "Returns the Floor value of a decimal".
     @param                     Value                           "The value to find the floor value of".
     */
    int64_t                     FloorD(double Value);
    
    /*!
     @abstract                                                  "Returns the Ceiling value of a decimal".
     @param                     Value                           "The value to find the ceil value of".
     */
    int64_t                     CeilF(float Value);
    
    /*!
     @abstract                                                  "Returns the Ceiling value of a decimal".
     @param                     Value                           "The value to find the ceil value of".
     */
    int64_t                     CeilD(double Value);
    
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
     @abstract                                                  "Extracts the sign from the integer given as Number2Extract".
     @param                     Number2Extract                  "The integer number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
    int8_t                      ExtractSignI(int64_t Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the sign from the float given as Number2Extract".
     @param                     Number2Extract                  "The float number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
    int8_t                      ExtractSignF(float Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the sign from the double given as Number2Extract".
     @param                     Number2Extract                  "The double number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
    int8_t                      ExtractSignD(double Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the exponent from the float given as Number2Extract".
     @param                     Number2Extract                  "The decimal number you want to get the exponent from".
     */
    int8_t                      ExtractExponentF(float Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the exponent from the double given as Number2Extract".
     @param                     Number2Extract                  "The decimal number you want to get the exponent from".
     */
    int16_t                     ExtractExponentD(double Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the mantissa from the float given as Number2Extract".
     @param                     Number2Extract                  "The mantissa number you want to get the exponent from".
     */
    int32_t                     ExtractMantissaF(float Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the mantissa from the double given as Number2Extract".
     @param                     Number2Extract                  "The mantissa number you want to get the exponent from".
     */
    int64_t                     ExtractMantissaD(double Number2Extract);
    
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
    uint8_t                     CreateBitMask(uint8_t NumBits2Select);
    
#define Absolute(Value)                     _Generic((Value), uint8_t:AbsoluteI, int8_t:AbsoluteI, uint16_t:AbsoluteI, int16_t:AbsoluteI, uint32_t:AbsoluteI, int32_t:AbsoluteI, uint64_t:AbsoluteI, int64_t:AbsoluteI, float:AbsoluteF, double:AbsoluteD)(Value)
#define Floor(Value)                        _Generic((Value),   float:FloorF, double:FloorD)(Value)
#define Ceil(Value)                         _Generic((Value),   float:CeilF, double:CeilD)(Value)
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
