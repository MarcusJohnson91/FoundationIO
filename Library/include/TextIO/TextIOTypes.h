/*!
 @header              UnicodeTypes.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains types and macros used across FoundationIO.
 */

#include "../PlatformIO.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_UnicodeTypes_H
#define FoundationIO_UnicodeIO_UnicodeTypes_H

#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
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
    typedef               char                                  UTF8;
#elif ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
    typedef               char8_t                               UTF8;
#elif  (FoundationIOTargetOS == FoundationIOWindowsOS)
    typedef               char                                  UTF8;
#endif
#else /* FoundationIOStandardVersionC < C11 */
    typedef               char                                  UTF8;
#endif /* FoundationIOStandardVersionC */
#elif (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX20)
    typedef               char8_t                               UTF8;
#else
    typedef               char                                  UTF8;
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
    
    
    
    
#ifndef                   FoundationIOUnicodePropertyConversion8
#define                   FoundationIOUnicodePropertyConversion8  (1)
    
#ifndef                   UTF8_MakeCharacterMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeCharacterMutable(String)  const_cast<UTF8>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeCharacterMutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF8_MakeCharacterMutable(String) (String)
#endif /* FoundationIOLanguage */
#endif /* UTF8_MakeCharacterMutable */
    
#ifndef                   UTF8_MakeStringMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringMutable(String)  const_cast<UTF8*>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringMutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF8_MakeStringMutable(String)  (String)
#endif /* FoundationIOLanguage */
#endif /* UTF8_MakeStringMutable */
    
#ifndef                   UTF8_MakeCharacterImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeCharacterImmutable(String)  const_cast<const UTF8>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeCharacterImmutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF8_MakeCharacterImmutable(String) (const String)
#endif /* FoundationIOLanguage */
#endif /* UTF8_MakeCharacterImmutable */
    
#ifndef                   UTF8_MakeStringImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringImmutable(String)  const_cast<const UTF8*>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringImmutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF8_MakeStringImmutable(String) (const String)
#endif /* FoundationIOLanguage */
#endif /* UTF8_MakeStringImmutable */
#endif /* FoundationIOUnicodePropertyConversion8 */
    
    
#ifndef                   FoundationIOUnicodePropertyConversion16
#define                   FoundationIOUnicodePropertyConversion16 (2)
    
#ifndef                   UTF16_MakeCharacterMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeCharacterMutable(String)  const_cast<UTF16>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeCharacterMutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16_MakeCharacterMutable(String) (String)
#endif /* FoundationIOLanguage */
#endif /* UTF16_MakeCharacterMutable */
    
#ifndef                   UTF16_MakeStringMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringMutable(String)  const_cast<UTF16*>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringMutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16_MakeStringMutable(String) (String)
#endif /* FoundationIOLanguage */
#endif /* UTF16_MakeStringMutable */
    
#ifndef                   UTF16_MakeCharacterImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeCharacterImmutable(String)  const_cast<const UTF16>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeCharacterImmutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16_MakeCharacterImmutable(String) (const String)
#endif /* FoundationIOLanguage */
#endif /* UTF16_MakeCharacterImmutable */
    
#ifndef                   UTF16_MakeStringImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringImmutable(String)  const_cast<const UTF16*>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringImmutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16_MakeStringImmutable(String) (const String)
#endif /* FoundationIOLanguage */
#endif /* UTF16_MakeStringImmutable */
    
#endif /* FoundationIOUnicodePropertyConversion16 */
    
    
#ifndef                   FoundationIOUnicodePropertyConversion32
#define                   FoundationIOUnicodePropertyConversion32 (4)
    
#ifndef                   UTF32_MakeCharacterMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeCharacterMutable(String)  const_cast<UTF32>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeCharacterMutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF32_MakeCharacterMutable(String) (String)
#endif /* FoundationIOLanguage */
#endif /* UTF32_MakeCharacterMutable */
    
#ifndef                   UTF32_MakeStringMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringMutable(String)  const_cast<UTF32*>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringMutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF32_MakeStringMutable(String) (String)
#endif /* FoundationIOLanguage */
#endif /* UTF32_MakeStringMutable */
    
#ifndef                   UTF8_MakeStringSetMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringSetMutable(StringSet)  const_cast<UTF8**>(StringSet)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringSetMutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF8_MakeStringSetMutable(StringSet) (StringSet)
#endif /* FoundationIOLanguage */
#endif /* UTF8_MakeStringSetMutable */
    
#ifndef                   UTF8_MakeStringSetImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringSetImmutable(StringSet)  const_cast<const UTF8**>(StringSet)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF8_MakeStringSetImmutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16_MakeStringSetImmutable(StringSet) (const StringSet)
#endif /* FoundationIOLanguage */
#endif /* UTF8_MakeStringSetImmutable */
    
#ifndef                   UTF16_MakeStringSetMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringSetMutable(StringSet)  const_cast<UTF16**>(StringSet)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringSetMutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16_MakeStringSetMutable(StringSet) (StringSet)
#endif /* FoundationIOLanguage */
#endif /* UTF16_MakeStringSetMutable */
    
#ifndef                   UTF16_MakeStringSetImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringSetImmutable(StringSet)  const_cast<const UTF16**>(StringSet)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF16_MakeStringSetImmutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16_MakeStringSetImmutable(StringSet) (const StringSet)
#endif /* FoundationIOLanguage */
#endif /* UTF16_MakeStringSetImmutable */
    
#ifndef                   UTF32_MakeStringSetMutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringSetMutable(StringSet)  const_cast<UTF32**>(StringSet)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringSetMutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF32_MakeStringSetMutable(StringSet) (StringSet)
#endif /* FoundationIOLanguage */
#endif /* UTF32_MakeStringSetMutable */
    
#ifndef                   UTF32_MakeStringSetImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringSetImmutable(StringSet)  const_cast<const UTF32**>(StringSet)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringSetImmutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF32_MakeStringSetImmutable(StringSet) (const StringSet)
#endif /* FoundationIOLanguage */
#endif /* UTF32_MakeStringSetImmutable */
    
    
#ifndef                   UTF32_MakeCharacterImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeCharacterImmutable(String)  const_cast<const UTF32>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeCharacterImmutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF32_MakeCharacterImmutable(String) (const String)
#endif /* FoundationIOLanguage */
#endif /* UTF32_MakeCharacterImmutable */
    
#ifndef                   UTF32_MakeStringImmutable
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringImmutable(String)  const_cast<const UTF32*>(String)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF32_MakeStringImmutable(String) (String)
#endif /* CXX11 */
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF32_MakeStringImmutable(String) (const String)
#endif /* FoundationIOLanguage */
#endif /* UTF32_MakeStringImmutable */
    
#endif /* FoundationIOUnicodePropertyConversion32 */
    
#ifndef                   FoundationIO_Unicodize8
#define                   FoundationIO_Unicodize8               (1)
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX20)
#define                   UTF8String(Literal)                   reinterpret_cast<UTF8*>(const_cast<UTF8*>(u8##Literal))
#define                   UTF8Character(Literal)                reinterpret_cast<UTF8>(u8##Literal)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX20)
#define                   UTF8String(Literal)                   reinterpret_cast<UTF8*>(const_cast<UTF8*>(u8##Literal))
#define                   UTF8Character(Literal)                reinterpret_cast<UTF8>(u8##Literal)
#endif
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF8String(Literal)                   u8##Literal
#define                   UTF8Character(Literal)                u8##Literal
#endif /* FoundationIOLanguage */
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                   FoundationIO_Unicodize16
#define                   FoundationIO_Unicodize16              (2)
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF16String(Literal)                  reinterpret_cast<UTF16*>(const_cast<UTF16*>(u##Literal))
#define                   UTF16Character(Literal)               reinterpret_cast<UTF16>(u##Literal)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF16String(Literal)                  reinterpret_cast<UTF16*>(const_cast<UTF16*>(u##Literal))
#define                   UTF16Character(Literal)               reinterpret_cast<UTF16>(u##Literal)
#endif
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF16String(Literal)                  u##Literal
#define                   UTF16Character(Literal)               u##Literal
#endif /* FoundationIOLanguage */
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32              (4)
#if   (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#if   (FoundationIOStandardVersionCXX >= FoundationIOStandardVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<UTF32*>(const_cast<UTF32*>(U##Literal))
#define                   UTF32Character(Literal)               reinterpret_cast<UTF32>(U##Literal)
#elif (FoundationIOStandardVersionCXX < FoundationIOStandardVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<UTF32*>(const_cast<UTF32*>(U##Literal))
#define                   UTF32Character(Literal)               reinterpret_cast<UTF32>(U##Literal)
#endif
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF32String(Literal)                  U##Literal
#define                   UTF32Character(Literal)               U##Literal
#endif /* FoundationIOLanguage */
#endif /* FoundationIO_Unicodize32 */
    
#ifndef                   FoundationIOFunctionName
#define                   FoundationIOFunctionName              (const UTF8*) __func__
#endif /* FoundationIOFunctionName */
    
#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
}
#endif

#endif /* FoundationIO_UnicodeIO_UnicodeTypes_H */