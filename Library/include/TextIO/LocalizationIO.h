/*!
 @header              LocalizationIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for cryptography and hashing.
 */

#include <stdbool.h>
#include <stdint.h>

#include "Macros.h"

#pragma once

#ifndef FoundationIO_LocalizationIO_H
#define FoundationIO_LocalizationIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char8_t          UTF8;
#else
    typedef               unsigned char    UTF8;
#endif /* __CHAR8_TYPE__ */
#endif /* FoundationIO_StringType8 */
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
#if (defined __STDC_UTF_16__ && defined __CHAR16_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char16_t         UTF16;
#else
    typedef               uint_least16_t   UTF16;
#endif /* __CHAR16_TYPE__ */
#endif /* FoundationIO_StringType16 */
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char32_t         UTF32;
#else
    typedef               uint_least32_t   UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
    /*!
     @abstract                             "Creates a string containing the language code (en for englisn, etc)".
     @return                               "Returns the string".
     */
    UTF8                 *Localize_UTF8_GetLanguage(void);
    
    /*!
     @abstract                             "Creates a string containing the language code (en for englisn, etc)".
     @return                               "Returns the string".
     */
    UTF16                *Localize_UTF16_GetLanguage(void);
    
    /*!
     @abstract                             "Creates a string containing the region code (us for America, etc)".
     @return                               "Returns the string".
     */
    UTF8                 *Localize_UTF8_GetRegion(void);
    
    /*!
     @abstract                             "Creates a string containing the region code (us for America, etc)".
     @return                               "Returns the string".
     */
    UTF16                *Localize_UTF16_GetRegion(void);
    
    /*!
     @abstract                             "Creates a string containing the encoding (UTF-16 for UTF-16, etc)".
     @return                               "Returns the string".
     */
    UTF8                 *Localize_UTF8_GetEncoding(void);
    
    /*!
     @abstract                             "Creates a string containing the encoding (UTF-8 for UTF-8, etc)".
     @return                               "Returns the string".
     */
    UTF16                *Localize_UTF16_GetEncoding(void);
    
    /*!
     @abstract                             "Creates a string containing the symbol to seperate the main value from the decimal (XXX.YYY)".
     @return                               "Returns the string".
     */
    UTF8                 *Localize_UTF8_GetDecimalSeperator(void);
    
    /*!
     @abstract                             "Creates a string containing the symbol to seperate the main value from the decimal (XXX.YYY)".
     @return                               "Returns the string".
     */
    UTF16                *Localize_UTF16_GetDecimalSeperator(void);
    
    /*!
     @abstract                             "Creates a string containing the symbol to seperate groups (1,000,000)".
     @return                               "Returns the string".
     */
    UTF8                 *Localize_UTF8_GetGroupingSeperator(void);
    
    /*!
     @abstract                             "Creates a string containing the symbol to seperate groups (1,000,000)".
     @return                               "Returns the string".
     */
    UTF16                *Localize_UTF16_GetGroupingSeperator(void);
    
    /*!
     @abstract                             "Creates a string array containing the number of digits for each group (1,000,000)".
     @return                               "Returns the string".
     */
    UTF8                **Localize_UTF8_GetGroupingSize(void);
    
    /*!
     @abstract                             "Creates a string array containing the number of digits for each group (1,000,000)".
     @return                               "Returns the string".
     */
    UTF16               **Localize_UTF16_GetGroupingSize(void);
    
    /*!
     @abstract                             "Creates a string containing the currency symbol ($1)".
     @return                               "Returns the string".
     */
    UTF8                 *Localize_UTF8_GetCurrencySymbol(void);
    
    /*!
     @abstract                             "Creates a string containing the currency symbol ($1)".
     @return                               "Returns the string".
     */
    UTF16                *Localize_UTF16_GetCurrencySymbol(void);
    
    /*!
     @abstract                             "Removes currency and grouping symbols from a string".
     @param               String           "The string to strip".
     @return                               "Returns the stripped string".
     */
    UTF8                 *Delocalize_UTF8_Currency(UTF8 *String);
    
    /*!
     @abstract                             "Removes currency and grouping symbols from a string".
     @param               String           "The string to strip".
     @return                               "Returns the stripped string".
     */
    UTF16                *Delocalize_UTF16_Currency(UTF16 *String);
    
    /*!
     @abstract                             "Removes currency and grouping symbols from a string leaving only an integer value".
     @param               String           "The string to strip".
     @return                               "Returns the stripped string".
     */
    UTF32                *Delocalize_UTF32_Currency(UTF32 *String);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_LocalizationIO_H */

