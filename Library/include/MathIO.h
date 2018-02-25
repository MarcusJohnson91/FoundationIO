#include <stdbool.h>

#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L)
#include <tgmath.h>
#elif (!defined __STDC_NO_COMPLEX__)
#include <math.h>
#include <complex.h>
#else
#include <math.h>
#endif

#pragma  once

#ifndef  FoundationIO_Math_H
#define  FoundationIO_Math_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*!
     @header    Math.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2017+
     @version   1.0.0
     @brief     This header contains code for specific mathematical functions used in FoundationIO and it's consumers.
     */
    
    /*!
     @abstract                                                  "Tells if the number is negative".
     @param                     Integer                         "The value to find the signedness of".
     */
    bool                        IsNegative(const int64_t Integer);
    
    /*!
     @abstract                                                  "Unsigned integer absolute value function".
     @remark                                                    "Returning a signed absolute value integer is dumb. REQUIRES two's complement signed representation".
     @param                     Value                           "The value to find the absolute value of".
     */
    uint64_t                    Absolute(const int64_t Value);
    
    /*!
     @abstract                                                  "Integer Power function".
     @param                     Base                            "What base should the power be calculated in"?
     @param                     Exponent                        "How many times should the Base be raised"?
     @return                                                    "Returns the result of 1 *= Base, Exponent times".
     */
    uint64_t                    Exponentiate(const uint64_t Base, const uint64_t Exponent);
    
    /*!
     @abstract                                                  "Computes the number of bits required to hold a certain symbol".
     @remark                                                    "Rounds up to the next integer number of bits to ensure all symbols can be contained in a single integer".
     @param                     Base                            "The base, or radix to divide the exponent by".
     @param                     Exponent                        "The exponent".
     @return                                                    "Returns the number of bits required to store a symbol".
     */
    int64_t                     Logarithm(int64_t Base, int64_t Exponent);
    
    /*!
     @abstract                                                  "Swap endian of 16 bit integers".
     @param                     Data2Swap                       "Data to swap endian".
     @return                                                    "Returns swapped uint16_t".
     */
    uint16_t                    SwapEndian16(const uint16_t Data2Swap);
    
    /*!
     @abstract                                                  "Swap endian of 32 bit integers".
     @param                     Data2Swap                       "Data to swap endian".
     @return                                                    "Returns swapped uint32_t".
     */
    uint32_t                    SwapEndian32(const uint32_t Data2Swap);
    
    /*!
     @abstract                                                  "Swap endian of 64 bit integers".
     @param                     Data2Swap                       "Data to swap endian".
     @return                                                    "Returns swapped uint64_t".
     */
    uint64_t                    SwapEndian64(const uint64_t Data2Swap);
    
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
     @abstract                                                  "Tells whether Input is even or odd".
     @param                     Number2Check                    "The number to see if it's odd or even".
     @return                                                    "True for odd, false for even".
     */
    bool                        IsOdd(const int64_t Number2Check);
    
#ifdef   __cplusplus
}
#endif

#endif   /* FoundationIO_Math_H */
