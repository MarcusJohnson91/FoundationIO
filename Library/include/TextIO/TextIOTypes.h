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
#ifdef __has_include
#if    __has_include(<uchar.h>)
#include <uchar.h>
#endif /* <uchar.h> exists */
#endif /* __has_include exists */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#ifdef __has_include
#if    __has_include(<cuchar>)
#include <cuchar>
#endif /* <cuchar> exists */
#endif /* __has_include exists */
#endif /* PlatformIO_Language */

#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifndef                   __CHAR8_TYPE__
#define                   __CHAR8_TYPE__                       unsigned char
typedef                   __CHAR8_TYPE__                       char8_t;
#endif /* __CHAR8_TYPE__ */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#ifndef                   __cpp_char8_t
#define                   __cpp_char8_t
typedef                   unsigned char                        char8_t;
#endif /* __cpp_char8_t */
#endif /* PlatformIO_Language */

#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifndef                   __STDC_UTF_16__
#define                   __STDC_UTF_16__                     (1) /* char16_t values is encoded as UTF-16 */
#endif
#ifdef                    __CHAR16_TYPE__
typedef                   __CHAR16_TYPE__                      char16_t;
#elif !defined(__CHAR16_TYPE__)
#if    defined(uint_least16_t)
typedef                   uint_least16_t                       char16_t;
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
typedef                   _Char16_t                            char16_t;
#elif !defined(uint_least16_t)
#error "Get a modern compiler that supports uint_least16_t"
#endif /* Defined? uint_least16_t */
#endif /* Defined? __CHAR16_TYPE__ */

#ifndef                   __STDC_UTF_32__
#define                   __STDC_UTF_32__                     (1) /* char32_t values are encoded as UTF-32 */
#endif
#ifdef                    __CHAR32_TYPE__
typedef                   __CHAR32_TYPE__                      char32_t;
#elif !defined(__CHAR32_TYPE__)
#if    defined(uint_least32_t)
typedef                   uint_least32_t                       char32_t;
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
typedef                   _Char32_t                            char32_t;
#elif !defined(uint_least32_t)
#error "Get a modern compiler that supports uint_least32_t"
#endif /* Defined? uint_least32_t */
#endif /* Defined? __CHAR32_TYPE__ */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#ifndef                   __cpp_unicode_characters
#define                   __cpp_unicode_characters
typedef                   uint_least16_t                       char16_t;
typedef                   uint_least32_t                       char32_t;
#endif /* __cpp_unicode_characters */
#endif /* PlatformIO_Language */


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
@abstract                 UTF16                                is guaranteed to only contain valid Unicode, use CharSet8 otherwise.
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
@abstract                 UTF32                                is guaranteed to only contain valid Unicode, use CharSet8 otherwise.
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

#ifndef                   PlatformIO_MakeStringSet
#define                   PlatformIO_MakeStringSet(StringSetSize, ...) {__VA_ARGS__,};
#endif

#endif /* TextIOTypes_PropertyConversion32 */

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
#define                   UTF8String(Literal)                   (PlatformIO_Immutable(UTF8 *)) u8##Literal
#define                   UTF8Character(Literal)                (PlatformIO_Constant(UTF8))   u8##Literal
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
#define                   UTF16Character(Literal)               u##Literal
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16String(Literal)                  (PlatformIO_Immutable(UTF16 *)) u##Literal
#define                   UTF16Character(Literal)               (PlatformIO_Constant(UTF16))   u##Literal
#endif /* PlatformIO_Language */
#endif /* FoundationIO_Unicodize16 */

#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32              (4)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#if   (PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  reinterpret_cast<UTF32*>(const_cast<UTF32*>(U##Literal))
#define                   UTF32Character(Literal)               reinterpret_cast<UTF32>(U##Literal)
#elif (PlatformIO_LanguageVersionCXX < PlatformIO_LanguageVersionCXX11)
#define                   UTF32String(Literal)                  const_cast<const UTF32 *>(reinterpret_cast<UTF32*>(const_cast<char32_t*>(U##Literal)))
#define                   UTF32Character(Literal)               U##Literal
#endif
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32String(Literal)                  (PlatformIO_Immutable(UTF32 *)) U##Literal
#define                   UTF32Character(Literal)               (PlatformIO_Constant(UTF32)) U##Literal
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
            return static_cast<TextIO_Bases>(static_cast<uint16_t>(A) | static_cast<uint16_t>(B));
        }

        constexpr inline TextIO_Bases operator & (TextIO_Bases A, TextIO_Bases B) {
            return static_cast<TextIO_Bases>(static_cast<uint16_t>(A) & static_cast<uint16_t>(B));
        }

        constexpr inline TextIO_Bases operator |= (TextIO_Bases A, TextIO_Bases B) {
            uint16_t A1 = static_cast<uint16_t>(A);
            uint16_t B1 = static_cast<uint16_t>(B);
            return static_cast<TextIO_Bases>(A1 |= B1);
        }

        constexpr inline TextIO_Bases operator &= (TextIO_Bases A, TextIO_Bases B) {
            uint16_t A1 = static_cast<uint16_t>(A);
            uint16_t B1 = static_cast<uint16_t>(B);
            return static_cast<TextIO_Bases>(A1 &= B1);
        }
    }
#endif /* PlatformIO_Language */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_TextIOTypes_H */
