/*!
 @header          TextIOTypes.h
 @author          Marcus Johnson
 @copyright       2020+
 @version         1.2.0
 @brief           This header contains types and macros used across FoundationIO for Text handling.
 */

#include "../PlatformIO.h" /* Included for Platform Independence macros */

#pragma once

#ifndef FoundationIO_TextIO_TextIOTypes_H
#define FoundationIO_TextIO_TextIOTypes_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#if defined(__has_include) && __has_include(<uchar.h>)
#include <uchar.h>
#elif !defined(__has_include) || !__has_include(<uchar.h>)

#ifdef  __STDC_UTF_16__
#undef  __STDC_UTF_16__
#define __STDC_UTF_16__ (1)
#else
#define __STDC_UTF_16__ (1)
#endif /* __STDC_UTF_16__ */

#ifdef  __STDC_UTF_32__
#undef  __STDC_UTF_32__
#define __STDC_UTF_32__ (1)
#else
#define __STDC_UTF_32__ (1)
#endif /* __STDC_UTF_32__ */

#endif /* __has_include */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if defined(__has_include) && __has_include(<cuchar>)
#include <cuchar>
#elif !defined(__has_include) || !__has_include(<cuchar>)

#ifndef __cpp_char8_t
#define __cpp_char8_t
#endif

#ifndef __cpp_unicode_characters
#define __cpp_unicode_characters
#endif

#endif /* __has_include */
#endif /* Language */

#ifdef  __CHAR8_TYPE__
#undef  __CHAR8_TYPE__
#define __CHAR8_TYPE__ unsigned char
#else
#define __CHAR8_TYPE__ unsigned char
#endif /* __CHAR8_TYPE__ */

#ifdef  __CHAR16_TYPE__
#undef  __CHAR16_TYPE__
#if   (PlatformIO_Compiler != PlatformIO_CompilerIsMSVC)
#define __CHAR16_TYPE__ uint_least16_t
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define __CHAR16_TYPE__ _Char16_t
#endif /* PlatformIO_Compiler */
#elif !defined(__CHAR16_TYPE__)
#if (PlatformIO_Compiler != PlatformIO_CompilerIsMSVC)
#define __CHAR16_TYPE__ uint_least16_t
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define __CHAR16_TYPE__ _Char16_t
#endif /* PlatformIO_Compiler */
#endif /* __CHAR16_TYPE__ */

#ifdef  __CHAR32_TYPE__
#undef  __CHAR32_TYPE__
#if   (PlatformIO_Compiler != PlatformIO_CompilerIsMSVC)
#define __CHAR32_TYPE__ uint_least32_t
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define __CHAR32_TYPE__ _Char32_t
#endif /* PlatformIO_Compiler */
#elif !defined(__CHAR32_TYPE__)
#if (PlatformIO_Compiler != PlatformIO_CompilerIsMSVC)
#define __CHAR32_TYPE__ uint_least32_t
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define __CHAR32_TYPE__ _Char32_t
#endif /* PlatformIO_Compiler */
#endif /* __CHAR32_TYPE__ */
  
#if (PlatformIO_Language == PlatformIO_LanguageIsC && PlatformIO_LanguageVersion < PlatformIO_LanguageVersionC23)
  typedef __CHAR8_TYPE__  char8_t;
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX20)
  typedef __CHAR8_TYPE__  char8_t;
#endif
  
#if (PlatformIO_Language == PlatformIO_LanguageIsC && PlatformIO_LanguageVersion < PlatformIO_LanguageVersionC11)
  typedef __CHAR16_TYPE__  char16_t;
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
  typedef __CHAR16_TYPE__  char16_t;
#endif
  
#if (PlatformIO_Language == PlatformIO_LanguageIsC && PlatformIO_LanguageVersion < PlatformIO_LanguageVersionC11)
  typedef __CHAR32_TYPE__  char32_t;
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
  typedef __CHAR32_TYPE__  char32_t;
#endif

#ifndef                   FoundationIO_StringTypes8
#define                   FoundationIO_StringTypes8 (1)
#ifdef                    UTF8
#undef                    UTF8
#endif /* UTF8 */
#ifdef                    CharSet8
#undef                    CharSet8
#endif /* CharSet8 */
/*!
@abstract                 UTF8                                 is guaranteed to only contain valid Unicode, use CharSet8 otherwise.
*/
typedef                   char8_t                              UTF8;
/*!
@abstract                 CharSet8                             is for non-Unicode character sets.
*/
typedef                   char8_t                              CharSet8;
#endif /* FoundationIO_StringTypes8 */

#ifndef                   FoundationIO_StringTypes16
#define                   FoundationIO_StringTypes16 (2)
#ifdef                    UTF16
#undef                    UTF16
#endif /* UTF16 */
#ifdef                    CharSet16
#undef                    CharSet16
#endif /* CharSet16 */
/*!
@abstract                 UTF16                                is guaranteed to only contain valid Unicode, use CharSet16 otherwise.
*/
typedef                   char16_t                             UTF16;
/*!
@abstract                 CharSet16                            is for non-Unicode character sets.
*/
typedef                   char16_t                             CharSet16;
#endif /* FoundationIO_StringTypes16 */

#ifndef                   FoundationIO_StringTypes32
#define                   FoundationIO_StringTypes32 (4)
#ifdef                    UTF32
#undef                    UTF32
#endif /* UTF32 */
#ifdef                    CharSet32
#undef                    CharSet32
#endif /* CharSet32 */
/*!
@abstract                 UTF32                                is guaranteed to only contain valid Unicode, use CharSet32 otherwise.
 */
typedef                   char32_t                             UTF32;
/*!
@abstract                 CharSet32                            is for non-Unicode character sets.
 */
typedef                   char32_t                             CharSet32;
#endif /* FoundationIO_StringTypes32 */

#ifndef                   TextIOTypes_PropertyConversion8
#define                   TextIOTypes_PropertyConversion8  (1)

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
#endif /* TextIOTypes_PropertyConversion8 */


#ifndef                   TextIOTypes_PropertyConversion16
#define                   TextIOTypes_PropertyConversion16 (2)

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

#endif /* TextIOTypes_PropertyConversion16 */


#ifndef                   TextIOTypes_PropertyConversion32
#define                   TextIOTypes_PropertyConversion32 (4)

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

#endif /* TextIOTypes_PropertyConversion32 */
  
typedef const UTF8     ImmutableChar_UTF8;
typedef const UTF16    ImmutableChar_UTF16;
typedef const UTF32    ImmutableChar_UTF32;

typedef const UTF8     *const ImmutableString_UTF8;
typedef const UTF16    *const ImmutableString_UTF16;
typedef const UTF32    *const ImmutableString_UTF32;

typedef const UTF8     *const ImmutableStringSet_UTF8[];
typedef const UTF16    *const ImmutableStringSet_UTF16[];
typedef const UTF32    *const ImmutableStringSet_UTF32[];

typedef const UTF8     *MutableStringSet_UTF8[];
typedef const UTF16    *MutableStringSet_UTF16[];
typedef const UTF32    *MutableStringSet_UTF32[];

#ifndef                   FoundationIO_Unicodize8
#define                   FoundationIO_Unicodize8               (1)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX20)
#define                   UTF8String(Literal)                   reinterpret_cast<ImmutableString_UTF8>(const_cast<ImmutableString_UTF8>(u8##Literal))
#define                   UTF8StringSet(...)                    {__VA_ARGS__, UTF8String("\0")}
#define                   UTF8Character(Literal)                reinterpret_cast<ImmutableChar_UTF8>(u8##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX20)
#define                   UTF8String(Literal)                   reinterpret_cast<ImmutableString_UTF8>(u8##Literal)
#define                   UTF8StringSet(...)                    {__VA_ARGS__, UTF8String("\0")}
#define                   UTF8Character(Literal)                reinterpret_cast<ImmutableChar_UTF8>(u8##Literal)
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8String(Literal)                   _Generic(Literal, unsigned char:(UTF8*) u8##Literal, unsigned char*:(UTF8*) u8##Literal, signed char:(UTF8*) u8##Literal, signed char*:(UTF8*) u8##Literal, char:(UTF8*) u8##Literal, char*:(UTF8*) u8##Literal, const unsigned char: (const UTF8*) u8##Literal, const unsigned char*: (const UTF8*) u8##Literal, const signed char: (const UTF8*) u8##Literal, const signed char*: (const UTF8*) u8##Literal, const char: (const UTF8*) u8##Literal, const char*: (const UTF8*) u8##Literal)
    // Create the string literal, but at the end create a NULL; so I guess count the number of variadic arguments, cloud just add an empty string manually...
#define                   UTF8StringSet(...)                    {__VA_ARGS__, UTF8String("\0")}
#define UTF8String2(A) u8##A
#define IMPLEMENT_MODULE(name) UTF8String2(A)
#define                   UTF8Character(Literal)                _Generic((0,Literal), unsigned char:(UTF8) u8##Literal, signed char:(UTF8) u8##Literal, char:(UTF8) u8##Literal)
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize8 */

#ifndef                   FoundationIO_Unicodize16
#define                   FoundationIO_Unicodize16              (2)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF16String(Literal)                  reinterpret_cast<ImmutableString_UTF16>(const_cast<ImmutableString_UTF16>(u##Literal))
#define                   UTF16StringSet(...)                   {__VA_ARGS__, UTF16String("\0")}
#define                   UTF16Character(Literal)               reinterpret_cast<ImmutableChar_UTF16>(u##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF16String(Literal)                  reinterpret_cast<ImmutableString_UTF16>(const_cast<ImmutableString_UTF16>(u##Literal))
#define                   UTF16StringSet(...)                   {__VA_ARGS__, UTF16String("\0")}
#define                   UTF16Character(Literal)               reinterpret_cast<ImmutableChar_UTF16>(u##Literal)
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16String(Literal)                  _Generic((0,Literal), unsigned short:(UTF16*) u##Literal, unsigned short*:(UTF16*) u##Literal, signed short:(UTF16*) u##Literal, signed short*:(UTF16*) u##Literal, const unsigned short:(UTF16*) u##Literal, const unsigned short*:(UTF16*) u##Literal, const signed short:(UTF16*) u##Literal, const signed short*:(UTF16*) u##Literal, unsigned char:(UTF16*) u##Literal, unsigned char*:(UTF16*) u##Literal, signed char:(UTF16*) u##Literal, signed char*:(UTF16*) u##Literal, char:(UTF16*) u##Literal, char*:(UTF16*) u##Literal, const unsigned char:(UTF16*) u##Literal, const unsigned char*:(UTF16*) u##Literal, const signed char:(UTF16*) u##Literal, const signed char*:(UTF16*) u##Literal, const char:(UTF16*) u##Literal, const char*:(UTF16*) u##Literal)
#define                   UTF16StringSet(...)                   {__VA_ARGS__, UTF16String("\0")}
#define                   UTF16Character(Literal)               (ImmutableChar_UTF16)   u##Literal
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize16 */

#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32              (4)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<ImmutableString_UTF32>(const_cast<ImmutableString_UTF32>(U##Literal))
#define                   UTF32StringSet(...)                   {__VA_ARGS__, UTF32String("\0")}
#define                   UTF32Character(Literal)               reinterpret_cast<ImmutableChar_UTF32>(U##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<ImmutableString_UTF32>(const_cast<ImmutableString_UTF32>(U##Literal))
#define                   UTF32StringSet(...)                   {__VA_ARGS__, UTF32String("\0")}
#define                   UTF32Character(Literal)               reinterpret_cast<ImmutableChar_UTF32>(U##Literal)
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32String(Literal)                  _Generic((0,Literal), unsigned int:(UTF32*) U##Literal, unsigned int*:(UTF32*) U##Literal, signed int:(UTF32*) U##Literal, signed int*:(UTF32*) U##Literal, const unsigned int:(UTF32*) U##Literal, const unsigned int*:(UTF32*) U##Literal, const signed int:(UTF32*) U##Literal, const signed int*:(UTF32*) U##Literal, unsigned char:(UTF32*) U##Literal, unsigned char*:(UTF32*) U##Literal, signed char:(UTF32*) U##Literal, signed char*:(UTF32*) U##Literal, char:(UTF32*) U##Literal, char*:(UTF32*) U##Literal, const unsigned char:(UTF32*) U##Literal, const unsigned char*:(UTF32*) U##Literal, const signed char:(UTF32*) U##Literal, const signed char*:(UTF32*) U##Literal, const char:(UTF32*) U##Literal, const char*:(UTF32*) U##Literal)
#define                   UTF32StringSet(...)                   {__VA_ARGS__, UTF32String("\0")}
/* TODO: ^Not sure how if programming model (LP64 vs ILP64 etc) needs to be handled or not, so I'll skip it */
#define                   UTF32Character(Literal)               U##Literal
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize32 */

#ifndef                   FoundationIO_Stringize
#define                   FoundationIO_Stringize                (1)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<ImmutableString_UTF32>(const_cast<ImmutableString_UTF32>(U##Literal))
#define                   UTF32Character(Literal)               reinterpret_cast<ImmutableChar_UTF32>(U##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<ImmutableString_UTF32>(const_cast<ImmutableString_UTF32>(U##Literal))
#define                   UTF32Character(Literal)               reinterpret_cast<ImmutableChar_UTF32>(U##Literal)
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   FoundationIO_String(OutputVariable, Literal) _Generic((OutputVariable), const UTF8*:u8##Literal, UTF8*:u8##Literal, const UTF16*:u##Literal, UTF16*:u##Literal, const UTF32*:U##Literal, UTF32*:U##Literal)
/* TODO: ^Not sure how if programming model (LP64 vs ILP64 etc) needs to be handled or not, so I'll skip it */
#define                   UTF32Character(Literal)               U##Literal
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize32 */

    /*!
     @enum                TextIO_StringTypes
     @constant            StringType_Unspecified                 Invalid/Default value
     @constant            StringType_UTF8                        UTF-8
     @constant            StringType_UTF16                       UTF-16
     @constant            StringType_UTF32                       UTF-32
     */
    typedef enum TextIO_StringTypes {
                          StringType_Unspecified                 = 0,
                          StringType_UTF8                        = 1,
                          StringType_UTF16                       = 2,
                          StringType_UTF32                       = 4,
    } TextIO_StringTypes;

    /*!
     @enum                TextIO_Bases
     @abstract                                                   Defines the type of option.
     @constant            Base_Integer                           ORable mask for Integers.
     @constant            Base_Radix2                            Integer only, base-2/binary.
     @constant            Base_Radix8                            Integer only, base-8/octal.
     @constant            Base_Decimal                           ORable mask for floating point numbers.
     @constant            Base_Radix10                           Integer or Decimal, base-10.
     @constant            Base_Radix16                           Integer or Decimal, base-16/hexadecimal.
     @constant            Base_Uppercase                         Base-16 only.
     @constant            Base_Lowercase                         Base-16 only.
     @constant            Base_Scientific                        Decimal only.
     @constant            Base_Shortest                          Decimal only.
     */
    typedef enum TextIO_Bases {
                          Base_Unspecified                       = 0,
                          Base_Integer                           = 1,
                          Base_Radix2                            = 2,
                          Base_Radix8                            = 4,
                          Base_Decimal                           = 8,
                          Base_Radix10                           = 16,
                          Base_Radix16                           = 32,
                          Base_Uppercase                         = 64,
                          Base_Lowercase                         = 128,
                          Base_Scientific                        = 256,
                          Base_Shortest                          = 512,
    } TextIO_Bases;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline TextIO_Bases operator | (TextIO_Bases A, TextIO_Bases B) {
            return static_cast<TextIO_Bases>(static_cast<int>(A) | static_cast<int>(B));
        }

        constexpr inline TextIO_Bases operator & (TextIO_Bases A, TextIO_Bases B) {
            return static_cast<TextIO_Bases>(static_cast<int>(A) & static_cast<int>(B));
        }

        constexpr inline TextIO_Bases operator |= (TextIO_Bases A, TextIO_Bases B) {
            uint16_t A1 = static_cast<int>(A);
            uint16_t B1 = static_cast<int>(B);
            return static_cast<TextIO_Bases>(A1 | B1);
        }

        constexpr inline TextIO_Bases operator &= (TextIO_Bases A, TextIO_Bases B) {
            uint16_t A1 = static_cast<int>(A);
            uint16_t B1 = static_cast<int>(B);
            return static_cast<TextIO_Bases>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */

    typedef struct TextIO_Slice {
        size_t StartInCodeUnits;
        size_t EndInCodeUnits;
    } TextIO_Slice;

    /* TextIO_Slice Functions */
    /*!
     @abstract                                           Creates an instance of a TextIO_Slice.
     @param            StartInCodeUnits                  Where should the string start?
     @param            EndInCodeUnits                    Where should the string end?
     */
    TextIO_Slice       TextIO_Slice_Init(size_t StartInCodeUnits, size_t EndInCodeUnits);

    /*!
     @abstract                                           Where does this slice start?
     @param            Slice                             The instance of the TextIO_Slice.
     @return                                             The Start of the TextIO_Slice in CodeUnits.
     */
    size_t             TextIO_Slice_GetStartInCodeUnits(TextIO_Slice Slice);

    /*!
     @abstract                                           Where does this slice end?
     @param            Slice                             The instance of the TextIO_Slice.
     @return                                             The End of the TextIO_Slice in CodeUnits.
     */
    size_t             TextIO_Slice_GetEndInCodeUnits(TextIO_Slice Slice);
    /* TextIO_Slice Functions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_TextIOTypes_H */
