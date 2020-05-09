/*!
 @header              UnicodeTypes.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.1.0
 @brief               This header contains types and macros used across FoundationIO.
 */

#include "../PlatformIO.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_UnicodeTypes_H
#define FoundationIO_UnicodeIO_UnicodeTypes_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifdef __has_include
#if    __has_include(<uchar.h>)
#include <uchar.h>
#endif // <uchar.h> exists
#endif // __has_include
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#ifdef __has_include
#if    __has_include(<cuchar>)
#include <cuchar>
#endif // <cuchar> exists
#endif // __has_include exists
#endif // PlatformIO_Language
    
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifndef __CHAR8_TYPE__
#define __CHAR8_TYPE__
typedef unsigned char char8_t;
#endif // __CHAR8_TYPE__
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#ifndef __cpp_char8_t
#define __cpp_char8_t
typedef unsigned char char8_t;
#endif // __cpp_char8_t
#endif // PlatformIO_Language
    
    
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifdef __CHAR16_TYPE__
typedef __CHAR16_TYPE__ char16_t;
#elif !defined(__CHAR16_TYPE__)
#if    defined(uint_least16_t)
typedef uint_least16_t char16_t;
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
typedef _Char16_t char16_t;
#elif !defined(uint_least16_t)
#error "Get a modern compiler that supports uint_least16_t"
#endif // Defined? uint_least16_t
#endif // Defined? __CHAR16_TYPE__

#ifdef __CHAR32_TYPE__
typedef __CHAR32_TYPE__ char32_t;
#elif !defined(__CHAR32_TYPE__)
#if    defined(uint_least32_t)
typedef uint_least32_t  char32_t;
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
typedef _Char32_t char32_t;
#elif !defined(uint_least32_t)
#error "Get a modern compiler that supports uint_least32_t"
#endif // Defined? uint_least32_t
#endif // Defined? __CHAR32_TYPE__
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#ifndef __cpp_unicode_characters
#define __cpp_unicode_characters
typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;
#endif // __cpp_unicode_characters
#endif // PlatformIO_Language
  
  
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
typedef                   char8_t                   UTF8;
#endif /* FoundationIO_StringType8 */
  
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
typedef                   char16_t                  UTF16;
#endif /* FoundationIO_StringType16 */
  
  
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (2)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
typedef                   char32_t                  UTF32;
#endif /* FoundationIO_StringType32 */
    
#ifndef                   FoundationIOUnicodePropertyConversion8
#define                   FoundationIOUnicodePropertyConversion8  (1)
    
#ifndef                   UTF8_MakeCharacterMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeCharacterMutable(String)  const_cast<UTF8>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeCharacterMutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8_MakeCharacterMutable(String) (String)
#endif /* PlatformIO_Language */
#endif /* UTF8_MakeCharacterMutable */
    
#ifndef                   UTF8_MakeStringMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringMutable(String)  const_cast<UTF8*>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringMutable(String)  (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8_MakeStringMutable(String)  (String)
#endif /* PlatformIO_Language */
#endif /* UTF8_MakeStringMutable */
    
#ifndef                   UTF8_MakeCharacterImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeCharacterImmutable(String)  const_cast<const UTF8>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeCharacterImmutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8_MakeCharacterImmutable(String) (const String)
#endif /* PlatformIO_Language */
#endif /* UTF8_MakeCharacterImmutable */
    
#ifndef                   UTF8_MakeStringImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringImmutable(String)  const_cast<const UTF8*>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringImmutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8_MakeStringImmutable(String) (const String)
#endif /* PlatformIO_Language */
#endif /* UTF8_MakeStringImmutable */
#endif /* FoundationIOUnicodePropertyConversion8 */
    
    
#ifndef                   FoundationIOUnicodePropertyConversion16
#define                   FoundationIOUnicodePropertyConversion16 (2)
    
#ifndef                   UTF16_MakeCharacterMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeCharacterMutable(String)  const_cast<UTF16>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeCharacterMutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16_MakeCharacterMutable(String) (String)
#endif /* PlatformIO_Language */
#endif /* UTF16_MakeCharacterMutable */
    
#ifndef                   UTF16_MakeStringMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringMutable(String)  const_cast<UTF16*>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringMutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16_MakeStringMutable(String) (String)
#endif /* PlatformIO_Language */
#endif /* UTF16_MakeStringMutable */
    
#ifndef                   UTF16_MakeCharacterImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeCharacterImmutable(String)  const_cast<const UTF16>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeCharacterImmutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16_MakeCharacterImmutable(String) (const String)
#endif /* PlatformIO_Language */
#endif /* UTF16_MakeCharacterImmutable */
    
#ifndef                   UTF16_MakeStringImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringImmutable(String)  const_cast<const UTF16*>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringImmutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16_MakeStringImmutable(String) (const String)
#endif /* PlatformIO_Language */
#endif /* UTF16_MakeStringImmutable */
    
#endif /* FoundationIOUnicodePropertyConversion16 */
    
    
#ifndef                   FoundationIOUnicodePropertyConversion32
#define                   FoundationIOUnicodePropertyConversion32 (4)
    
#ifndef                   UTF32_MakeCharacterMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeCharacterMutable(String)  const_cast<UTF32>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeCharacterMutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32_MakeCharacterMutable(String) (String)
#endif /* PlatformIO_Language */
#endif /* UTF32_MakeCharacterMutable */
    
#ifndef                   UTF32_MakeStringMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringMutable(String)  const_cast<UTF32*>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringMutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32_MakeStringMutable(String) (String)
#endif /* PlatformIO_Language */
#endif /* UTF32_MakeStringMutable */
    
#ifndef                   UTF8_MakeStringSetMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringSetMutable(StringSet)  const_cast<UTF8**>(StringSet)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringSetMutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8_MakeStringSetMutable(StringSet) (StringSet)
#endif /* PlatformIO_Language */
#endif /* UTF8_MakeStringSetMutable */
    
#ifndef                   UTF8_MakeStringSetImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringSetImmutable(StringSet)  const_cast<const UTF8**>(StringSet)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF8_MakeStringSetImmutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16_MakeStringSetImmutable(StringSet) (const StringSet)
#endif /* PlatformIO_Language */
#endif /* UTF8_MakeStringSetImmutable */
    
#ifndef                   UTF16_MakeStringSetMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringSetMutable(StringSet)  const_cast<UTF16**>(StringSet)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringSetMutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16_MakeStringSetMutable(StringSet) (StringSet)
#endif /* PlatformIO_Language */
#endif /* UTF16_MakeStringSetMutable */
    
#ifndef                   UTF16_MakeStringSetImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringSetImmutable(StringSet)  const_cast<const UTF16**>(StringSet)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16_MakeStringSetImmutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16_MakeStringSetImmutable(StringSet) (const StringSet)
#endif /* PlatformIO_Language */
#endif /* UTF16_MakeStringSetImmutable */
    
#ifndef                   UTF32_MakeStringSetMutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringSetMutable(StringSet)  const_cast<UTF32**>(StringSet)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringSetMutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32_MakeStringSetMutable(StringSet) (StringSet)
#endif /* PlatformIO_Language */
#endif /* UTF32_MakeStringSetMutable */
    
#ifndef                   UTF32_MakeStringSetImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringSetImmutable(StringSet)  const_cast<const UTF32**>(StringSet)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringSetImmutable(StringSet) (StringSet)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32_MakeStringSetImmutable(StringSet) (const StringSet)
#endif /* PlatformIO_Language */
#endif /* UTF32_MakeStringSetImmutable */
    
    
#ifndef                   UTF32_MakeCharacterImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeCharacterImmutable(String) const_cast<const UTF32>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeCharacterImmutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32_MakeCharacterImmutable(String) (const String)
#endif /* PlatformIO_Language */
#endif /* UTF32_MakeCharacterImmutable */
    
#ifndef                   UTF32_MakeStringImmutable
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringImmutable(String) const_cast<const UTF32*>(String)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32_MakeStringImmutable(String) (String)
#endif /* CXX11 */
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32_MakeStringImmutable(String) (const String)
#endif /* PlatformIO_Language */
#endif /* UTF32_MakeStringImmutable */
    
#endif /* FoundationIOUnicodePropertyConversion32 */
    
#ifndef                   FoundationIO_Unicodize8
#define                   FoundationIO_Unicodize8               (1)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX20)
#define                   UTF8String(Literal)                   reinterpret_cast<UTF8*>(const_cast<UTF8*>(u8##Literal))
#define                   UTF8Character(Literal)                reinterpret_cast<UTF8>(u8##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX20)
#define                   UTF8String(Literal)                   reinterpret_cast<PlatformIO_Immutable(UTF8 *)>(u8##Literal)
#define                   UTF8Character(Literal)                reinterpret_cast<UTF8>(u8##Literal)
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8String(Literal)                   (UTF8*) u8##Literal
#define                   UTF8Character(Literal)                (UTF8) u8##Literal
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                   FoundationIO_Unicodize16
#define                   FoundationIO_Unicodize16              (2)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16String(Literal)                  reinterpret_cast<UTF16*>(const_cast<UTF16*>(u##Literal))
#define                   UTF16Character(Literal)               reinterpret_cast<UTF16>(u##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16String(Literal)                  reinterpret_cast<UTF16*>(const_cast<UTF16*>(u##Literal))
#define                   UTF16Character(Literal)               reinterpret_cast<UTF16>(u##Literal)
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16String(Literal)                  u##Literal
#define                   UTF16Character(Literal)               u##Literal
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32              (4)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<UTF32*>(const_cast<UTF32*>(U##Literal))
#define                   UTF32Character(Literal)               reinterpret_cast<UTF32>(U##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<UTF32*>(const_cast<UTF32*>(U##Literal))
#define                   UTF32Character(Literal)               reinterpret_cast<UTF32>(U##Literal)
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32String(Literal)                  U##Literal
#define                   UTF32Character(Literal)               U##Literal
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize32 */
    
#ifndef                   FoundationIOFunctionName
#define                   FoundationIOFunctionName              (const UTF8*) __func__
#endif /* FoundationIOFunctionName */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* FoundationIO_UnicodeIO_UnicodeTypes_H */
