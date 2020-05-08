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

#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
extern "C" {
#endif
    
#if   (FoundationIOLanguage == FoundationIOLanguageIsC)
#ifdef __has_include
#if    __has_include(<uchar.h>)
#include <uchar.h>
#endif // <uchar.h> exists
#endif // __has_include
#elif (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#ifdef __has_include
#if    __has_include(<cuchar>)
#include <cuchar>
#endif // <cuchar> exists
#endif // __has_include exists
#endif // FoundationIOLanguage
    
#if   (FoundationIOLanguage == FoundationIOLanguageIsC)
#ifndef __CHAR8_TYPE__
#define __CHAR8_TYPE__
typedef unsigned char char8_t;
#endif // __CHAR8_TYPE__
#elif (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#ifndef __cpp_char8_t
#define __cpp_char8_t
typedef unsigned char char8_t;
#endif // __cpp_char8_t
#endif // FoundationIOLanguage
    
    
#if   (FoundationIOLanguage == FoundationIOLanguageIsC)
#ifdef __CHAR16_TYPE__
typedef __CHAR16_TYPE__ char16_t;
#elif !defined(__CHAR16_TYPE__)
#if    defined(uint_least16_t)
typedef uint_least16_t char16_t;
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
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
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
typedef _Char32_t char32_t;
#elif !defined(uint_least32_t)
#error "Get a modern compiler that supports uint_least32_t"
#endif // Defined? uint_least32_t
#endif // Defined? __CHAR32_TYPE__
#elif (FoundationIOLanguage == FoundationIOLanguageIsCXX)
#ifndef __cpp_unicode_characters
#define __cpp_unicode_characters
typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;
#endif // __cpp_unicode_characters
#endif // FoundationIOLanguage
  
  
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
#define                   UTF8_MakeStringMutable(String)  (String)
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
#define                   UTF32_MakeCharacterImmutable(String) const_cast<const UTF32>(String)
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
#define                   UTF32_MakeStringImmutable(String) const_cast<const UTF32*>(String)
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
#define                   UTF8String(Literal)                   reinterpret_cast<FoundationIO_Immutable(UTF8 *)>(u8##Literal)
#define                   UTF8Character(Literal)                reinterpret_cast<UTF8>(u8##Literal)
#endif
#elif (FoundationIOLanguage == FoundationIOLanguageIsC)
#define                   UTF8String(Literal)                   (UTF8*) u8##Literal
#define                   UTF8Character(Literal)                (UTF8) u8##Literal
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
