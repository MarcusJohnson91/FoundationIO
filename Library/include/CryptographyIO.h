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
    
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_CryptographyIO_H */
