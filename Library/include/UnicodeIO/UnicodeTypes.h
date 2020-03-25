/*!
 @header              UnicodeTypes.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains types and macros used across FoundationIO.
 */

#include "../Macros.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_UnicodeTypes_H
#define FoundationIO_UnicodeIO_UnicodeTypes_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if   (FoundationIOLanguage == FoundationIOLanguageIsC)
#if   (FoundationIOStandardVersionC >= FoundationIOStandardVersionC2X)
#if   ((FoundationIOTargetOS & FoundationIOAppleOS) == FoundationIOAppleOS)
    typedef               uint_least8_t                         UTF8;
#elif ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
    typedef               char8_t                               UTF8;
#elif  (FoundationIOTargetOS == FoundationIOWindowsOS)
    typedef               uint_least8_t                         UTF8;
#endif
#else /* FoundationIOStandardVersionC < C11 */
    typedef               uint_least8_t                         UTF8;
#endif /* FoundationIOStandardVersionC */
#elif (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX20)
    typedef               char8_t                               UTF8;
#else
    typedef               uint_least8_t                         UTF8;
#endif
#endif /* FoundationIOStandardVersionCXX >= CXX11 */
#endif /* FoundationIOLanguage */
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
#if   (FoundationIOLanguage == FoundationIOLanguageIsC)
#if   (FoundationIOStandardVersionC >= FoundationIOStandardVersionC11)
#if   ((FoundationIOTargetOS & FoundationIOAppleOS) == FoundationIOAppleOS)
    typedef               uint_least16_t                        UTF16;
#elif ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
    typedef               char16_t                              UTF16;
#elif  (FoundationIOTargetOS == FoundationIOWindowsOS)
    typedef               uint_least16_t                        UTF16;
#endif
#else /* FoundationIOStandardVersionC < C11 */
    typedef               uint_least16_t                        UTF16;
#endif /* FoundationIOStandardVersionC */
#elif (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
    typedef               char16_t                              UTF16;
#else
    typedef               uint_least16_t                        UTF16;
#endif
#endif /* FoundationIOStandardVersionCXX >= CXX11 */
#endif /* FoundationIOLanguage */
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if   (FoundationIOLanguage == FoundationIOLanguageIsC)
#if   (FoundationIOStandardVersionC >= FoundationIOStandardVersionC11)
#if   ((FoundationIOTargetOS & FoundationIOAppleOS) == FoundationIOAppleOS)
    typedef               uint_least32_t                        UTF32;
#elif ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
    typedef               char32_t                              UTF32;
#elif  (FoundationIOTargetOS == FoundationIOWindowsOS)
    typedef               uint_least32_t                        UTF32;
#endif
#else /* FoundationIOStandardVersionC < C11 */
    typedef               uint_least32_t                        UTF32;
#endif /* FoundationIOStandardVersionC */
#elif (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
    typedef               char32_t                              UTF32;
#else
    typedef               uint_least32_t                        UTF32;
#endif
#endif /* FoundationIOStandardVersionCXX >= CXX11 */
#endif /* FoundationIOLanguage */
    
#ifndef                   FoundationIO_Unicodize8
#define                   FoundationIO_Unicodize8               (1)
#define                   UTF8String(Literal)                   u8##Literal
#define                   UTF8Character(Literal)                u8##Literal
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                   FoundationIO_Unicodize16
#define                   FoundationIO_Unicodize16              (2)
#define                   UTF16String(Literal)                  u##Literal
#define                   UTF16Character(Literal)               u##Literal
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32              (4)
#define                   UTF32String(Literal)                  U##Literal
#define                   UTF32Character(Literal)               U##Literal
#endif /* FoundationIO_Unicodize32 */
    
#ifndef                   FoundationIOFunctionName
#define                   FoundationIOFunctionName              (const UTF8*) __func__
#endif /* FoundationIOFunctionName */
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_UnicodeIO_UnicodeTypes_H */
