#include <stdint.h>
#include <stdbool.h>

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
    bool                        IsNegative(const int64_t Integer);
    
    /*!
     @abstract                                                  "Tells whether Input is even or odd".
     @param                     Number2Check                    "The number to see if it's odd or even".
     @return                                                    "True for odd, false for even".
     */
    bool                        IsOdd(const int64_t Number2Check);
    
    /*!
     @abstract                                                  "Returns the absolute value of an integer (removes the sign)".
     @param                     Value                           "The value to find the absolute value of".
     */
    uint64_t                    Absolute(const int64_t Value);
    
    /*!
     @abstract                                                  "Calculates the value of Base raised to Exponent's power (an integer version of the pow function)".
     @param                     Base                            "What base should the power be calculated in"?
     @param                     Exponent                        "How many times should the Base be raised"?
     @return                                                    "Returns the result of 1 *= Base, Exponent times".
     */
    uint64_t                    Exponentiate(const uint64_t Base, const uint64_t Exponent);
    
    /*!
     @abstract                                                  "Computes the number of times Exponent fits into Base".
     @remark                                                    "Rounds up to the next integer number of symbols".
     @param                     Base                            "The base, or radix to contain the symbol".
     @param                     Exponent                        "The value to calculate".
     @return                                                    "Returns the number of symbols required to store an integer".
     */
    int64_t                     Logarithm(const uint64_t Base, const int64_t Exponent);
    
    /*!
     @abstract                                                  "Byte swaps a 16 bit integer".
     @param                     Value2Swap                      "Data to swap endian".
     @return                                                    "Returns swapped uint16_t".
     */
    uint16_t                    SwapEndian16(const uint16_t Value2Swap);
    
    /*!
     @abstract                                                  "Byte swaps a 32 bit integer".
     @param                     Value2Swap                      "Data to swap endian".
     @return                                                    "Returns swapped uint32_t".
     */
    uint32_t                    SwapEndian32(const uint32_t Value2Swap);
    
    /*!
     @abstract                                                  "Byte swaps a 64 bit integer".
     @param                     Value2Swap                      "Data to swap endian".
     @return                                                    "Returns swapped uint64_t".
     */
    uint64_t                    SwapEndian64(const uint64_t Value2Swap);
    
    /*!
     @abstract                                                  "Computes the number of bits from the number of bytes".
     @param                     Bytes                           "The number of bytes you want to interpret as bits".
     @return                                                    "Returns the number of bits".
     */
    int64_t                     Bytes2Bits(const int64_t Bytes);
    
    /*!
     @abstract                                                  "Computes the number of bytes from the number of bits".
     @param                     Bits                            "The bits to convert to bytes".
     @param                     RoundUp                         "Should the resulting bytes be rounded up or down?".
     @return                                                    "Returns the number of bytes".
     */
    int64_t                     Bits2Bytes(const int64_t Bits, const bool RoundUp);
    
    /*!
     @abstract                                                  "Swaps bits so the the MSB becoems the LSB, and vice versa".
     @param                     Byte                            "The byte to swap the bits in".
     @return                                                    "Returns the swapped byte".
     */
    uint8_t                     SwapBits(const uint8_t Byte);
    
    /*!
     @abstract                                                  "Creates a bitmask favoring the right side".
     @param                     Bits2Extract                    "The number of bits to mask".
     @return                                                    "Returns the mask".
     */
    uint8_t                     CreateBitMaskLSBit(const uint8_t Bits2Extract);
    
    /*!
     @abstract                                                  "Creates a bitmask favoring the left side".
     @param                     Bits2Extract                    "The number of bits to mask".
     @return                                                    "Returns the mask".
     */
    uint8_t                     CreateBitMaskMSBit(const uint8_t Bits2Extract);
    
    /*!
     @abstract                                                  "Extracts the sign from the decimal given as Number2Extract".
     @param                     Number2Extract                  "The decimal number you want to get the sign from".
     @return                                                    "Returns -1 if the sign is negative, otherwise 1".
     */
    int8_t                      ExtractSignFromDecimal(double Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the exponent from the decimal given as Number2Extract".
     @param                     Number2Extract                  "The decimal number you want to get the exponent from".
     */
    int16_t                     ExtractExponentFromDecimal(double Number2Extract);
    
    /*!
     @abstract                                                  "Extracts the mantissa from the decimal given as Number2Extract".
     @param                     Number2Extract                  "The mantissa number you want to get the exponent from".
     */
    int64_t                     ExtractMantissaFromDecimal(double Number2Extract);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_Math_H */
