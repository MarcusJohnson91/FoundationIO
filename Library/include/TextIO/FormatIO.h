/*!
 @header              FormatIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for string formatting/deformatting
 */

#include <stdbool.h>
#include <stdint.h>

#include "Macros.h"

#pragma once

#ifndef FoundationIO_FormatIO_H
#define FoundationIO_FormatIO_H

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
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
#if (defined __STDC_UTF_16__ && defined __CHAR16_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef                     char16_t                        UTF16;
#else
    typedef                     uint_least16_t                  UTF16;
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
    
#ifndef                         FoundationIO_Unicodize8
#define                         FoundationIO_Unicodize8         (1)
#define                         U8(QuotedLiteral)               u8##QuotedLiteral
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                         FoundationIO_Unicodize16
#define                         FoundationIO_Unicodize16        (2)
#define                         U16(QuotedLiteral)              u##QuotedLiteral
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                         FoundationIO_Unicodize32
#define                         FoundationIO_Unicodize32        (4)
#define                         U32(QuotedLiteral)              U##QuotedLiteral
#endif /* FoundationIO_Unicodize32 */
    
    typedef struct FormatSpecifiers FormatSpecifiers;
    
    typedef enum FormatSpecifier_StringTypes {
        StringType_Unknown             = 0,
        UTF8Format                     = 1,
        UTF16Format                    = 2,
        UTF32Format                    = 3,
    } FormatSpecifier_StringTypes;
    
    FormatSpecifiers *FormatSpecifiers_Init(uint64_t NumSpecifiers);
    
    UTF32            *FormatString_UTF32(UTF32 *Format, FormatSpecifiers *Details, va_list VariadicArguments);
    
    FormatSpecifiers *UTF32_ParseFormatString(UTF32 *Format, uint64_t NumSpecifiers, FormatSpecifier_StringTypes StringType);
    
    FormatSpecifiers *UTF32_ParseFormatSpecifiers(UTF32 *Format, uint64_t NumSpecifiers, FormatSpecifier_StringTypes StringType);
    
    UTF32           **DeformatString_UTF32(UTF32 *Format, UTF32 *Result, FormatSpecifiers *Details);
    
    void              FormatSpecifiers_Deinit(FormatSpecifiers *Details);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_FormatIO_H */
