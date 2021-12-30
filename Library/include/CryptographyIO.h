/*!
 @header          CryptographyIO.h
 @author          Marcus Johnson
 @copyright       2019+
 @version         1.0.0
 @brief           This header contains types, functions, and tables for cryptography and hashing.
 @TODO:           Look into replacing Xoshiro256** with Shishua
 */

#include "PlatformIO.h"

#pragma once

#ifndef FoundationIO_CryptographyIO_H
#define FoundationIO_CryptographyIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @abstract                              Forward declare BitBuffer
     */
    typedef struct    BitBuffer             BitBuffer;

    /*!
     @abstract                              Algorithm inspired by public domain Xoshiro256**
     */
    typedef struct    InsecurePRNG          InsecurePRNG;

    /*!
     @abstract                              Creates an InsecurePRNG structure with random-ish data.
     @remark                                InsecurePRNG is for fast random-ish numbers that are NOT to be used for cryptographic purposes.
     @param           Seed                  Seed is a 64 bit signed integer, there is a default seed.
     @return                                Returns an initialized instance of InsecurePRNG.
     */
    InsecurePRNG     *InsecurePRNG_Init(uint64_t Seed);

    /*!
     @abstract                              Generates an integer occupying NumBits.
     @param           Insecure              The InsecurePRNG pointer.
     @param           IntegerSizeInBits     The number of bits for the integer to take up, between 1 and 64.
     @return                                Returns an integer matching those characteristics.
     */
    int64_t           InsecurePRNG_CreateInteger(InsecurePRNG *Insecure, uint8_t IntegerSizeInBits);

    /*!
     @abstract                              Generates an integer between MinValue and MaxValue, inclusive.
     @param           Insecure              The InsecurePRNG pointer.
     @param           MinValue              The minimum valid value in the range, inclusive.
     @param           MaxValue              The maximum valud value in the range, inclusive.
     @return                                Returns an integer matching those characteristics.
     */
    int64_t           InsecurePRNG_CreateIntegerInRange(InsecurePRNG *Insecure, int64_t MinValue, int64_t MaxValue);

    /*!
     @abstract                              Generates a Decimal.
     @param           Insecure              The InsecurePRNG pointer.
     @return                                Returns the generated decimal.
     */
    double            InsecurePRNG_CreateDecimal(InsecurePRNG *Insecure);

    /*!
     @abstract                              Erases InsecurePRNG for sexurity reasons.
     @param           Insecure              The InsecurePRNG pointer.
     @return                                Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    bool              InsecurePRNG_Erase(InsecurePRNG *Insecure);

    /*!
     @abstract                              Zeros and deinitializes InsecurePRNG instance.
     */
    void              InsecurePRNG_Deinit(InsecurePRNG *Insecure);

    /*!
     @remark                                MD5 is EXTREMELY INSECURE, IT'S ONLY HERE FOR OVIA
     */
    typedef struct    MD5Context            MD5Context;
    typedef struct    MD5Hash               MD5Hash;

    /*!
     @abstract                              Creates an MD5 hasher instance.
     */
    MD5Context       *MD5Context_Init(void);

    /*!
     @abstract                              Updates an MD5 instance.
     */
    void              MD5Update(MD5Context *Context, void const *Buffer, size_t BufferSize);

    /*!
     @abstract                              Finalizes an MD5 instance.
     */
    void              MD5Finalize(MD5Context *Context, MD5Hash *Digest);

    /*!
     @abstract                              Converts a binary MD5 hash to a string.
     */
    char             *MD5ToString(MD5Hash *Digest);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_CryptographyIO_H */
