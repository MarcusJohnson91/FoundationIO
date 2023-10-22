/*!
 @header:                  TextIOTypes.h
 @author:                  Marcus Johnson
 @copyright:               2020+
 @version:                 1.2.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains types and macros used across FoundationIO for Text handling.
 */

#pragma once

#ifndef FoundationIO_TextIO_TextIOTypes_H
#define FoundationIO_TextIO_TextIOTypes_H

#include "../PlatformIO.h" /* Included for Platform Independence macros */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

        /*!
     @enum         TextIOCommon
     @constant     UTF8CodeUnitSizeInBits                The size of a single code unit in bits.
     @constant     UTF16CodeUnitSizeInBits               The size of a single code unit in bits.
     @constant     UTF32CodeUnitSizeInBits               The size of a single code unit in bits.
     @constant     UTF8BOMSizeInCodeUnits                The number of code units (8 bits) the UTF8 BOM takes.
     @constant     UTF16BOMSizeInCodeUnits               The number of code units (16 bits) the UTF16 BOM takes.
     @constant     UnicodeBOMSizeInCodePoints            The number of CodePoints in a BOM.
     @constant     UTF16BOM_LE                           UTF16BOM_LE byte order mark.
     @constant     UTF16BOM_BE                           UTF16BOM_BE byte order mark.
     @constant     UTF32BOM_LE                           UTF32BOM_LE byte order mark.
     @constant     UTF32BOM_BE                           UTF32BOM_BE byte order mark.
     @constant     UnicodeUNCPathPrefixSize              Size of "//?/" or "\\?\" in CodePoints and CodeUnits.
     @constant     UTF16HighSurrogateStart               The value that marks the start of the High Surrogate range.
     @constant     UTF16HighSurrogateEnd                 The value that marks the end   of the High Surrogate range.
     @constant     UTF16LowSurrogateStart                The value that marks the start of the Low  Surrogate range.
     @constant     UTF16LowSurrogateEnd                  The value that marks the end   of the Low  Surrogate range.
     @constant     UTF16MaxCodeUnitValue                 The highest value that can be stored in a single UTF16 CodeUnit.
     @constant     UTF16SurrogatePairModDividend         The value to modulo the surrogate pair by to decode a High Surrogate.
     @constant     UTF16SurrogatePairStart               The first UTF-32 CodePoint to require Surrogate Pairs in UTF-16.
     @constant     InvalidReplacementCodePoint           The CodePoint to replace invalid codeunits.
     @constant     UnicodeMaxCodePoint                   The highest CodePoint possible in Unicode, 1,114,111.
     @constant     UTF8MaxCodeUnitsInCodePoint           The maximum number of codeunits per codepoint.
     @constant     UTF16MaxCodeUnitsInCodePoint          The maximum number of codeunits per codepoint.
     */
    typedef enum TextIOCommon : uint32_t {
                   UTF8CodeUnitSizeInBits                = 8,
                   UTF16CodeUnitSizeInBits               = 16,
                   UTF32CodeUnitSizeInBits               = 32,
                   UTF8BOMSizeInCodeUnits                = 3,
                   UTF16BOMSizeInCodeUnits               = 1,
                   UnicodeBOMSizeInCodePoints            = 1,
                   UTF8BOM_1                             = 0xEF,
                   UTF8BOM_2                             = 0xBB,
                   UTF8BOM_3                             = 0xBF,
                   UTF16BOM_LE                           = 0xFFFE,
                   UTF16BOM_BE                           = 0xFEFF,
                   UTF32BOM_LE                           = 0xFFFE,
                   UTF32BOM_BE                           = 0xFEFF,
                   UnicodeUNCPathPrefixSize              = 4,
                   UTF16SurrogateMask                    = 0x3FF,
                   UTF16SurrogateShift                   = 10,
                   UTF16HighSurrogateStart               = 0xD800,
                   UTF16HighSurrogateEnd                 = 0xDBFF,
                   UTF16LowSurrogateStart                = 0xDC00,
                   UTF16LowSurrogateEnd                  = 0xDFFF,
                   UTF16MaxCodeUnitValue                 = 0xFFFF,
                   UTF16SurrogatePairModDividend         = 0x400,
                   UTF16SurrogatePairStart               = 0x10000,
                   InvalidReplacementCodePoint           = 0xFFFD,
                   UnicodeMaxCodePoint                   = 0x10FFFF,
                   UTF8MaxCodeUnitsInCodePoint           = 4,
                   UTF16MaxCodeUnitsInCodePoint          = 2,
                   UnicodeCodePointMask = 0x1FFFFF,
    } TextIOCommon;

  /*!
   @enum                TextIO_StringTypes
   @constant            StringType_Unspecified                 Invalid/Default value
   @constant            StringType_UTF8                        UTF-8
   @constant            StringType_UTF16                       UTF-16
   @constant            StringType_UTF32                       UTF-32
   */
  typedef enum TextIO_StringTypes : uint8_t {
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
  typedef enum TextIO_Bases : uint16_t {
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
    constexpr inline  TextIO_Bases operator | (TextIO_Bases A, TextIO_Bases B) {
      return static_cast<TextIO_Bases>(static_cast<int>(A) | static_cast<int>(B));
    }

    constexpr inline  TextIO_Bases operator & (TextIO_Bases A, TextIO_Bases B) {
      return static_cast<TextIO_Bases>(static_cast<int>(A) & static_cast<int>(B));
    }

    constexpr inline  TextIO_Bases operator |= (TextIO_Bases A, TextIO_Bases B) {
      uint16_t A1 = static_cast<int>(A);
      uint16_t B1 = static_cast<int>(B);
      return static_cast<TextIO_Bases>(A1 | B1);
    }

    constexpr inline  TextIO_Bases operator &= (TextIO_Bases A, TextIO_Bases B) {
      uint16_t A1 = static_cast<int>(A);
      uint16_t B1 = static_cast<int>(B);
      return static_cast<TextIO_Bases>(A1 & B1);
    }
  }
#endif /* PlatformIO_Language */

#ifndef TextIO_NULLTerminator
#define                   TextIO_NULLTerminator                                           (0)
#endif

#ifndef TextIO_NULLTerminatorSize
#define                   TextIO_NULLTerminatorSize                                       (1)
#endif

#ifndef TextIO_UNCPathPrefix8
#define                   TextIO_UNCPathPrefix8                                           UTF8String("//?/")
#endif

#ifndef TextIO_InvisibleString32
#define                   TextIO_InvisibleString32                                        UTF32String("\u00A0")
#endif

#ifndef TextIO_WCharType
#if   (WCHAR_MAX == 0x7FFFFFFF || WCHAR_MAX == 0xFFFFFFFF)
#define                   TextIO_WCharType                                                StringType_UTF32
#elif (WCHAR_MAX == 0x7FFF || WCGAR_MAX == 0xFFFF)
#define                   TextIO_WCharType                                                StringType_UTF16
#endif /* WCHAR_MAX */
#endif /* TextIO_WCharType */

#ifndef                   TextIO_NewLineTypes
#define                   TextIO_NewLineTypes                                             (1)
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#define                   TextIO_NewLine8                                                 UTF8String("\n")
#define                   TextIO_NewLine8Size                                             (1)
#define                   TextIO_NewLine16                                                UTF16String("\n")
#define                   TextIO_NewLine16Size                                            (1)
#define                   TextIO_NewLine32                                                UTF32String("\n")
#define                   TextIO_NewLine32Size                                            (1)
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#define                   TextIO_NewLine8                                                 UTF8String("\r\n")
#define                   TextIO_NewLine8Size                                             (2)
#define                   TextIO_NewLine16                                                UTF16String("\r\n")
#define                   TextIO_NewLine16Size                                            (2)
#define                   TextIO_NewLine32                                                UTF32String("\r\n")
#define                   TextIO_NewLine32Size                                            (2)
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsClassicMac)
#define                   TextIO_NewLine8                                                 UTF8String("\r")
#define                   TextIO_NewLine8Size                                             (1)
#define                   TextIO_NewLine16                                                UTF16String("\r")
#define                   TextIO_NewLine16Size                                            (1)
#define                   TextIO_NewLine32                                                UTF32String("\r")
#define                   TextIO_NewLine32Size                                            (1)
#endif /* PlatformIO_TargetOS */
#endif /* TextIO_NewLineTypes */

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
  
#if   (PlatformIO_Language == PlatformIO_LanguageIsC && PlatformIO_LanguageVersion < PlatformIO_LanguageVersionC23)
  typedef __CHAR8_TYPE__   char8_t;
#endif
  
#if   (PlatformIO_Language == PlatformIO_LanguageIsC && PlatformIO_LanguageVersion < PlatformIO_LanguageVersionC11)
  typedef __CHAR16_TYPE__  char16_t;
#endif
  
#if   (PlatformIO_Language == PlatformIO_LanguageIsC && PlatformIO_LanguageVersion < PlatformIO_LanguageVersionC11)
  typedef __CHAR32_TYPE__  char32_t;
#endif

#ifndef                   TextIO_StringTypes8
#define                   TextIO_StringTypes8 (1)
#ifdef                    UTF8
#undef                    UTF8
#endif /* UTF8 */
#ifdef                    CharSet8
#undef                    CharSet8
#endif /* CharSet8 */
typedef                   char8_t                              UTF8;
typedef                   char8_t                              CharSet8;
typedef                   const UTF8                           ImmutableChar_UTF8;
typedef                   const UTF8 *const                    ImmutableString_UTF8;
typedef                   const UTF8                          *StringSet_UTF8;
typedef                   const UTF8 *const *const             ImmutableStringSet_UTF8;
typedef                   const UTF8                          *MutableStringSet_UTF8[];
#endif /* TextIO_StringTypes8 */

#ifndef                   TextIO_StringTypes16
#define                   TextIO_StringTypes16 (2)
#ifdef                    UTF16
#undef                    UTF16
#endif /* UTF16 */
#ifdef                    CharSet16
#undef                    CharSet16
#endif /* CharSet16 */
typedef                   char16_t                             UTF16;
typedef                   char16_t                             CharSet16;
typedef                   const UTF16                          ImmutableChar_UTF16;
typedef                   const UTF16 *const                   ImmutableString_UTF16;
typedef                   const UTF16                         *StringSet_UTF16;
typedef                   const UTF16 *const *const            ImmutableStringSet_UTF16;
typedef                   const UTF16                         *MutableStringSet_UTF16[];
#endif /* TextIO_StringTypes16 */

#ifndef                   TextIO_StringTypes32
#define                   TextIO_StringTypes32 (4)
#ifdef                    UTF32
#undef                    UTF32
#endif /* UTF32 */
#ifdef                    CharSet32
#undef                    CharSet32
#endif /* CharSet32 */
typedef                   char32_t                             UTF32;
typedef                   char32_t                             CharSet32;
typedef                   const UTF32                          ImmutableChar_UTF32;
typedef                   const UTF32 *const                   ImmutableString_UTF32;
typedef                   const UTF32                         *StringSet_UTF32;
typedef                   const UTF32 *const *const            ImmutableStringSet_UTF32;
typedef                   const UTF32                         *MutableStringSet_UTF32[];
#endif /* TextIO_StringTypes32 */

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

#ifndef                   TextIO_Unicodize8
#define                   TextIO_Unicodize8               (1)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define                   UTF8String(Literal)                   PlatformIO_Cast(ImmutableString_UTF8, u8##Literal)
#define                   UTF8StringSet(...)                    {PlatformIO_Expand(__VA_ARGS__), UTF8String("\0")}
#define                   UTF8Character(Literal)                PlatformIO_Cast(ImmutableChar_UTF8, u8##Literal)
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF8String(Literal) _Generic(Literal, char[PlatformIO_GetStringSizeInCodeUnits(Literal)]: u8##Literal, char8_t[PlatformIO_GetStringSizeInCodeUnits(Literal)]: u8##Literal, signed char[PlatformIO_GetStringSizeInCodeUnits(Literal)]: u8##Literal, const char[PlatformIO_GetStringSizeInCodeUnits(Literal)]: u8##Literal, const signed char[PlatformIO_GetStringSizeInCodeUnits(Literal)]: u8##Literal, const char8_t[PlatformIO_GetStringSizeInCodeUnits(Literal)]: u8##Literal, char *: (UTF8*) u8##Literal, signed char *: (UTF8*) u8##Literal, char8_t *: (UTF8*) u8##Literal, const char *: (const UTF8*) u8##Literal, const signed char *: (const UTF8*) u8##Literal, const char8_t *: (const UTF8*) u8##Literal)
#define                   UTF8StringSet(...)                    {PlatformIO_Expand(__VA_ARGS__), UTF8String("\0")}
#define                   UTF8Character(Literal)                _Generic((0,Literal), unsigned char:(UTF8) u8##Literal, signed char:(UTF8) u8##Literal, char:(UTF8) u8##Literal)
#endif /* PlatformIO_Language */
#endif /* TextIO_Unicodize8 */

#ifndef                   TextIO_Unicodize16
#define                   TextIO_Unicodize16              (2)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define                   UTF16String(Literal)                  PlatformIO_Cast(ImmutableString_UTF16, u##Literal)
#define                   UTF16StringSet(...)                   {PlatformIO_Expand(__VA_ARGS__), UTF16String("\0")}
#define                   UTF16Character(Literal)               PlatformIO_Cast(ImmutableChar_UTF16, u##Literal)
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF16String(Literal)                  _Generic((0,Literal), unsigned short:(UTF16*) u##Literal, unsigned short*:(UTF16*) u##Literal, signed short:(UTF16*) u##Literal, signed short*:(UTF16*) u##Literal, const unsigned short:(UTF16*) u##Literal, const unsigned short*:(UTF16*) u##Literal, const signed short:(UTF16*) u##Literal, const signed short*:(UTF16*) u##Literal, unsigned char:(UTF16*) u##Literal, unsigned char*:(UTF16*) u##Literal, signed char:(UTF16*) u##Literal, signed char*:(UTF16*) u##Literal, char:(UTF16*) u##Literal, char*:(UTF16*) u##Literal, const unsigned char:(UTF16*) u##Literal, const unsigned char*:(UTF16*) u##Literal, const signed char:(UTF16*) u##Literal, const signed char*:(UTF16*) u##Literal, const char:(UTF16*) u##Literal, const char*:(UTF16*) u##Literal)
#define                   UTF16StringSet(...)                   {PlatformIO_Expand(__VA_ARGS__), UTF16String("\0")}
#define                   UTF16Character(Literal)               PlatformIO_Cast(ImmutableChar_UTF16, u##Literal)
#endif /* PlatformIO_Language */
#endif /* TextIO_Unicodize16 */

#ifndef                   TextIO_Unicodize32
#define                   TextIO_Unicodize32              (4)
#if   (PlatformIO_Language == PlatformIO_LanguageIsCXX)
#define                   UTF32String(Literal)                  PlatformIO_Cast(ImmutableString_UTF32, U##Literal)
#define                   UTF32StringSet(...)                   {PlatformIO_Expand(__VA_ARGS__), UTF32String("\0")}
#define                   UTF32Character(Literal)               PlatformIO_Cast(ImmutableChar_UTF32, U##Literal)
#elif (PlatformIO_Language == PlatformIO_LanguageIsC)
#define                   UTF32String(Literal)                  _Generic((0,Literal), unsigned int:(UTF32*) U##Literal, unsigned int*:(UTF32*) U##Literal, signed int:(UTF32*) U##Literal, signed int*:(UTF32*) U##Literal, const unsigned int:(UTF32*) U##Literal, const unsigned int*:(UTF32*) U##Literal, const signed int:(UTF32*) U##Literal, const signed int*:(UTF32*) U##Literal, unsigned char:(UTF32*) U##Literal, unsigned char*:(UTF32*) U##Literal, signed char:(UTF32*) U##Literal, signed char*:(UTF32*) U##Literal, char:(UTF32*) U##Literal, char*:(UTF32*) U##Literal, const unsigned char:(UTF32*) U##Literal, const unsigned char*:(UTF32*) U##Literal, const signed char:(UTF32*) U##Literal, const signed char*:(UTF32*) U##Literal, const char:(UTF32*) U##Literal, const char*:(UTF32*) U##Literal)
#define                   UTF32StringSet(...)                   {PlatformIO_Expand(__VA_ARGS__), UTF32String("\0")}
/* TODO: ^Not sure how if programming model (LP64 vs ILP64 etc) needs to be handled or not, so I'll skip it */
#define                   UTF32Character(Literal)               U##Literal
#endif /* PlatformIO_Language */
#endif /* TextIO_Unicodize32 */

    typedef struct TextIO_String8 {
        size_t   NumCodeUnits;
        char8_t *Data;
    } TextIO_String8;

    typedef struct TextIO_String16 {
        size_t   NumCodeUnits;
        char8_t *Data;
    } TextIO_String16;

    typedef struct TextIO_String32 {
        size_t   NumCodeUnits;
        char8_t *Data;
    } TextIO_String32;

    //_Generic((Literal), UTF8[]:PlatformIO_GetStringSizeInCodeUnits, UTF8*:UTF8_GetStringSizeInCodeUnits)

#define String8Constuct(Literal) {.NumCodeUnits = PlatformIO_GetStringSizeInCodeUnits(Literal), .Data = Literal}
#define String16Constuct(Literal) {.NumCodeUnits = PlatformIO_GetStringSizeInCodeUnits(Literal), .Data = Literal}
#define String32Constuct(Literal) {.NumCodeUnits = PlatformIO_GetStringSizeInCodeUnits(Literal), .Data = Literal}

    UTF32 TextIO_CaseMap_GetUppercase(TextIO_CaseMap CaseMap);
    
    UTF32 TextIO_CaseMap_GetLowercaseTextIO_CaseMap CaseMap);

    typedef struct TextIO_Normalization TextIO_Normalization;
    
    typedef struct TextIO_Normalizatoons TextIO_Normalizations; 
    

#ifndef TextIO_Normalization_SetSize
#define TextIO_Normalization_SetSize(Size) 
#if (Size <= 8)
.Replacee |= ((Size - 1) & 0x7) << 21;
#elif (Size > 8)
.ReplacementSizeMinus1 = Size - 1;
#endif /* Size Check */
#endif /* TextIO_Normalization_SetSize */

#ifndef TextIO_Normalization_Init1
#define TextIO_Normalization_Init1(Replacee, Replacement1) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(1) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
};
#endif /* TextIO_Normalization_Init1 */

#ifndef TextIO_Normalization_Init2
#define TextIO_Normalization_Init2(Replacee, Replacement1, Replacement2) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(2) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
};
#endif /* TextIO_Normalization_Init2 */

#ifndef TextIO_Normalization_Init3
#define TextIO_Normalization_Init3(Replacee, Replacement1, Replacement2, Replacement3) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(3) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
};
#endif /* TextIO_Normalization_Init3 */

#ifndef TextIO_Normalization_Init4
#define TextIO_Normalization_Init4(Replacee, Replacement1, Replacement2, Replacement3, Replacement4) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(4) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
};
#endif /* TextIO_Normalization_Init4 */

#ifndef TextIO_Normalization_Init5
#define TextIO_Normalization_Init5(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(5) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
};
#endif /* TextIO_Normalization_Init5 */

#ifndef TextIO_Normalization_Init6
#define TextIO_Normalization_Init6(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(6) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
.Payload1 |= ((Replacement6 & 0x1FFFFF) << 1); \
};
#endif /* TextIO_Normalization_Init6 */

#ifndef TextIO_Normalization_Init7
#define TextIO_Normalization_Init7(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6, Replacement7) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(7) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
.Payload1 |= ((Replacement6 & 0x1FFFFF) << 1); \
.Payload1 |= ((Replacement7 & 0x1FFFFF) << 22); \
};
#endif /* TextIO_Normalization_Init7 */

#ifndef TextIO_Normalization_Init8
#define TextIO_Normalization_Init8(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6, Replacement7, Replacement8) \
TextIO_Normalization = { \
 TextIO_Normalization_SetSize(8) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
.Payload1 |= ((Replacement6 & 0x1FFFFF) << 1); \
.Payload1 |= ((Replacement7 & 0x1FFFFF) << 22); \
.Payload1 |= ((Replacement8 & 0x1FFFFF) << 43); \
};
#endif /* TextIO_Normalization_Init8 */

#ifndef TextIO_Normalization_InitExternal
#define TextIO_Normalization_InitExternal(Replacee, NumReplacements, String) \
TextIO_Normalization = { \
    .Replacee = (Replacee & UnicodeMaxCodePoint), \
    .ReplacementSizeMinus1 = NumReplacements, \
    .Replacement = U"/"String"/", \
};
#endif /* TextIO_Normalization_InitExternal */

#ifndef TextIO_Normalization_Init
#define TextIO_Normalization_Init(Replacee, NumReplacements, Replacements) \
#if (NumReplacements == 1) \
TextIO_Normalization_Init1(Replacee, Replacements)
#elif (NumReplacements == 2) \
TextIO_Normalization_Init2(Replacee, Replacements)
#elif (NumReplacements == 3) \
TextIO_Normalization_Init3(Replacee, Replacements)
#elif (NumReplacements == 4) \
TextIO_Normalization_Init4(Replacee, Replacements)
#elif (NumReplacements == 5) \
TextIO_Normalization_Init5(Replacee, Replacements)
#elif (NumReplacements == 6) \
TextIO_Normalization_Init6(Replacee, Replacements)
#elif (NumReplacements == 7) \
TextIO_Normalization_Init7(Replacee, Replacements)
#elif (NumReplacements == 8) \
TextIO_Normalization_Init8(Replacee, Replacements)
#else
TectIO_Normalization_InitExternal(Replacee, NumReplacements, String)
#endif /* NumReplacements */
#endif /* TextIO_Normalization_Init */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_TextIOTypes_H */
