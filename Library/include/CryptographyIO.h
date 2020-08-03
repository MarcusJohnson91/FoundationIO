/*!
 @header          CryptographyIO.h
 @author          Marcus Johnson
 @copyright       2019+
 @version         1.0.0
 @brief           This header contains types, functions, and tables for cryptography and hashing.
 */

#include "PlatformIO.h"         /* Included for Platform Independence macros */
#include "TextIO/TextIOTypes.h" /* Included for Text types */

#pragma once

#ifndef FoundationIO_CryptographyIO_H
#define FoundationIO_CryptographyIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @abstract                              Algorithm inspired by public domain Shishua; Nothing was copied.
     */
    typedef struct    InsecurePRNG          InsecurePRNG;

    /*!
     @abstract                              Own algorithm; Uses InsecurePRNG and AES tables as a starting point to generate the (hopefully, UNTESTED) secure random numbers.
     */
    typedef struct    SecureRNG             SecureRNG;

    typedef struct    MD5                   MD5;

    /*!
     @abstract                              Creates an InsecurePRNG structure with random-ish data.
     @remark                                InsecurePRNG is for fast random-ish numbers that are NOT to be used for cryptographic purposes.
     @param           Seed                  Seed needs to be an array of 4 64-bit variables.
     @return                                Returns an initialized instance of InsecurePRNG.
     */
    InsecurePRNG     *InsecurePRNG_Init(uint64_t Seed[4]);

    void              InsecurePRNG_Generate(InsecurePRNG *Insecure, uint8_t *Buffer, uint64_t BufferSize);

    /*!
     @abstract                              Zeros and deinitializes InsecurePRNG instance.
     */
    void              InsecurePRNG_Deinit(InsecurePRNG *Insecure);
    
    /*!
     @abstract                              Creates an SecureRNG container for generating random numbers.
     @param           EntropyPoolSize       The amount of random data to contain in bytes.
     @return                                Returns the newly created SecureRNG.
     */
    SecureRNG        *SecureRNG_Init(uint64_t EntropyPoolSize);
    
    /*!
     @abstract                              The number of bits of SecureRNG available.
     @param           Random                The SecureRNG pointer.
     @return                                Returns the the amount of SecureRNG in bits.
     */
    uint64_t          SecureRNG_GetRemainingEntropy(SecureRNG *Random);
    
    /*!
     @abstract                              Generates an integer occupying NumBits.
     @param           Random                The SecureRNG pointer.
     @param           NumBits               The number of bits for the integer to take up, between 1 and 64.
     @return                                Returns an integer matching those characteristics.
     */
    int64_t           SecureRNG_GenerateInteger(SecureRNG *Random, uint8_t NumBits);
    
    /*!
     @abstract                              Generates an integer between MinValue and MaxValue, inclusive.
     @param           Random                The SecureRNG pointer.
     @param           MinValue              The minimum valid value in the range, inclusive.
     @param           MaxValue              The maximum valud value in the range, inclusive.
     @return                                Returns an integer matching those characteristics.
     */
    int64_t           SecureRNG_GenerateIntegerInRange(SecureRNG *Random, int64_t MinValue, int64_t MaxValue);
    
    /*!
     @abstract                              Generates a Decimal.
     @param           Random                The SecureRNG pointer.
     @return                                Returns the generated decimal.
     */
    double            SecureRNG_GenerateDecimal(SecureRNG *Random);
    
    /*!
     @abstract                              Erases the SecureRNG pool for sexurity reasons.
     @param           Random                The SecureRNG pointer.
     @param           NewValue              The value to set each codeunit to while erasing.
     @return                                Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    uint8_t           SecureRNG_Erase(SecureRNG *Random, uint8_t NewValue);
    
    /*!
     @abstract                              The number of bits of SecureRNG available.
     @param           Random                The SecureRNG pointer.
     */
    void              SecureRNG_Deinit(SecureRNG *Random);

    MD5              *MD5_Init(void);

    void              MD5_Process(uint32_t *State, uint32_t *Block);

    UTF8             *MD5_Finalize(MD5 *MD5);

    bool              MD5_Compare(uint8_t *Hash1, uint8_t *Hash2);

    void              MD5_Deinit(MD5 *MD5);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_CryptographyIO_H */
