/*!
 @header              CryptographyIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for cryptography and hashing.
 */

#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifndef FoundationIO_CryptographyIO_H
#define FoundationIO_CryptographyIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    // PLEASE! USE YOUR OWN SEEDS AND ROTATES with: od -vAx -N8 -tx8 < /dev/urandom
    static const uint64_t Entropy_Seed1   = 0x996B01419E29C332;
    static const uint64_t Entropy_Seed2   = 0x1F8FC6D7F904D5C2;
    static const uint64_t Entropy_Seed3   = 0x2CB2699B18EBB96E;
    static const uint64_t Entropy_Seed4   = 0xBD64B91757E88DB3;
    static const uint8_t  Entropy_Rotate1 = 38;
    static const uint8_t  Entropy_Rotate2 = 59;
    static const uint8_t  Entropy_Rotate3 = 44;
    static const uint8_t  Entropy_Rotate4 = 60;
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef   char8_t        UTF8;
#else
    typedef   unsigned char  UTF8;
#endif /* __CHAR8_TYPE__ */
#endif /* FoundationIO_StringType8 */
    
    typedef struct MD5 MD5;
    
    MD5  *MD5_Init(void);
    
    void  MD5_Process(uint32_t *State, uint32_t *Block);
    
    UTF8 *MD5_Finalize(MD5 *MD5);
    
    bool  MD5_Compare(uint8_t *Hash1, uint8_t *Hash2);
    
    void  MD5_Deinit(MD5 *MD5);
    
    typedef struct    Entropy Entropy;
    
    /*!
     @abstract                             "Creates an Entropy container for generating random numbers".
     @param           EntropyPoolSize      "The amount of random data to contain in bytes".
     @return                               "Returns the newly created entropy".
     */
    Entropy          *Entropy_Init(uint64_t EntropyPoolSize);
    
    /*!
     @abstract                             "The number of bits of entropy available".
     @param           Entropy              "The Entropy pointer".
     @return                               "Returns the the amount of entropy in bits".
     */
    uint64_t          Entropy_GetRemainingEntropy(Entropy *Entropy);
    
    /*!
     @abstract                             "The number of bits of entropy available".
     @param           Entropy              "The Entropy pointer".
     @return                               "Returns the the amount of entropy in bits".
     */
    int64_t           Entropy_GenerateIntegerInRange(Entropy *Entropy, int64_t MinValue, int64_t MaxValue);
    
    /*!
     @abstract                             "Erases the entropy pool for sexurity reasons".
     @param           Entropy              "The Entropy pointer".
     */
    void              Entropy_Erase(Entropy *Entropy);
    
    /*!
     @abstract                             "The number of bits of entropy available".
     @param           Entropy              "The Entropy pointer".
     */
    void              Entropy_Deinit(Entropy *Entropy);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_CryptographyIO_H */
