/*!
 @header              CryptographyIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for cryptography and hashing.
 */

#include <stdbool.h>
#include <stdint.h>

#include "Macros.h"

#pragma once

#ifndef FoundationIO_CryptographyIO_H
#define FoundationIO_CryptographyIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef   char8_t        UTF8;
#else
    typedef   unsigned char  UTF8;
#endif /* __CHAR8_TYPE__ */
#endif /* FoundationIO_StringType8 */
    
    typedef struct    MD5 MD5;
    
    MD5              *MD5_Init(void);
    
    void              MD5_Process(uint32_t *State, uint32_t *Block);
    
    UTF8             *MD5_Finalize(MD5 *MD5);
    
    bool              MD5_Compare(uint8_t *Hash1, uint8_t *Hash2);
    
    void              MD5_Deinit(MD5 *MD5);
    
    typedef struct    Entropy Entropy;
    
    /*!
     @abstract                             "Creates an Entropy container for generating random numbers".
     @param           EntropyPoolSize      "The amount of random data to contain in bytes".
     @return                               "Returns the newly created entropy".
     */
    Entropy          *Entropy_Init(uint64_t EntropyPoolSize);
    
    /*!
     @abstract                             "The number of bits of entropy available".
     @param           Random               "The Entropy pointer".
     @return                               "Returns the the amount of entropy in bits".
     */
    uint64_t          Entropy_GetRemainingEntropy(Entropy *Random);
    
    /*!
     @abstract                             "Generates an integer occupying NumBits".
     @param           Random               "The Entropy pointer".
     @param           NumBits              "The number of bits for the integer to take up, between 1 and 64".
     @return                               "Returns an integer matching those characteristics".
     */
    int64_t           Entropy_GenerateInteger(Entropy *Random, uint8_t NumBits);
    
    /*!
     @abstract                             "Generates an integer between MinValue and MaxValue, inclusive".
     @param           Random               "The Entropy pointer".
     @param           MinValue             "The minimum valid value in the range, inclusive".
     @param           MaxValue             "The maximum valud value in the range, inclusive".
     @return                               "Returns an integer matching those characteristics".
     */
    int64_t           Entropy_GenerateIntegerInRange(Entropy *Random, int64_t MinValue, int64_t MaxValue);
    
    /*!
     @abstract                             "Generates a Decimal".
     @param           Random               "The Entropy pointer".
     @return                               "Returns the generated decimal".
     */
    double            Entropy_GenerateDecimal(Entropy *Random);
    
    /*!
     @abstract                             "Erases the entropy pool for sexurity reasons".
     @param           Random               "The Entropy pointer".
     */
    void              Entropy_Erase(Entropy *Random);
    
    /*!
     @abstract                             "The number of bits of entropy available".
     @param           Random               "The Entropy pointer".
     */
    void              Entropy_Deinit(Entropy *Random);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_CryptographyIO_H */
