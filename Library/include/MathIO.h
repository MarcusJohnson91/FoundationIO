#include "Macros.h"                   /* Included for u/intX_t, bool, Yes/No macros, FoundationIOTargetOS, GetRuntimeByteBitOrder */

#if   (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L)
#include      <tgmath.h>
#elif (!defined __STDC_NO_COMPLEX__)
#if   (FoundationIOTargetOS == WindowsOS)
#include      <math.h>
#elif (FoundationIOTargetOS == POSIXOS)
#include_next <math.h>
#endif  /* FoundationIOTargetOS */
#include      <complex.h>
#endif

#pragma  once

#ifndef  FoundationIO_Math_H
#define  FoundationIO_Math_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*!
     @header                    Math.h
     @author                    Marcus Johnson aka BumbleBritches57
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
     @abstract                                                  "Computes the number of symbols required to hold a certain value in Base X".
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
    
#ifdef   __cplusplus
}
#endif

#endif   /* FoundationIO_Math_H */
