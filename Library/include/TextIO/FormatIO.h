/*!
 @header          FormatIO.h
 @author          Marcus Johnson
 @copyright       2019+
 @version         1.0.0
 @brief           This header contains types, functions, and tables for string formatting/deformatting
 */

#include "../PlatformIO.h"  /* Included for Platform Independence macros */
#include "TextIOTypes.h"    /* Included for Text types */

#pragma once

#ifndef FoundationIO_TextIO_FormatIO_H
#define FoundationIO_TextIO_FormatIO_H

#if   (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#include <sal.h>
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @enum         FormatIO_BaseTypes
     @abstract                                    Defines the base type of each format specifier.
     @constant     BaseType_Unspecified           Invalid base type.
     @constant     BaseType_Integer               The specifier is an Integer.
     @constant     BaseType_Decimal               The specifier is a floating point number.
     @constant     BaseType_CodeUnit              The specifier is a single value encoded as UTF 8/16/32 bits max, otherwise use String.
     @constant     BaseType_String                The specifier is a null terminated array encoded as UTF 8/16/32.
     @constant     BaseType_LiteralPercent        The specifier is a double percent token to print a single percent symbol.
     @constant     BaseType_Pointer               The specifier is a Pointer address.
     @constant     BaseType_RemoveN               The specifier is N, we remove it for interoperability.
     */
    typedef enum FormatIO_BaseTypes {
                   BaseType_Unspecified           = 0,
                   BaseType_Integer               = 1,
                   BaseType_Decimal               = 2,
                   BaseType_CodeUnit              = 4,
                   BaseType_String                = 8,
                   BaseType_Pointer               = 16,
                   BaseType_RemoveN               = 32,
                   BaseType_LiteralPercent        = 64,
    } FormatIO_BaseTypes;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FormatIO_BaseTypes operator | (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            return static_cast<FormatIO_BaseTypes>(static_cast<uint8_t>(A) | static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_BaseTypes operator & (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            return static_cast<FormatIO_BaseTypes>(static_cast<uint8_t>(A) & static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_BaseTypes operator |= (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_BaseTypes>(A1 | B1);
        }
        
        constexpr inline FormatIO_BaseTypes operator &= (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_BaseTypes>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_ModifierTypes
     @abstract                                    Defines the modifiers of each format specifier.
     @constant     ModifierType_Unspecified       Invalid modifier type.
     @constant     ModifierType_UTF8              The modifier is UTF-8 encoded CodeUnit or String.
     @constant     ModifierType_UTF16             The modifier is UTF-16 encoded CodeUnit or String.
     @constant     ModifierType_UTF32             The modifier is UTF-32 encoded CodeUnit or String.
     @constant     ModifierType_Radix8            The modifier is base-8.
     @constant     ModifierType_Radix10           The modifier is base-10, either Integer or Decimal.
     @constant     ModifierType_Radix16           The modifier is base-16, either Integer or Decimal.
     @constant     ModifierType_Uppercase         The modifier is Uppercase.
     @constant     ModifierType_Lowercase         The modifier is Lowercase.
     @constant     ModifierType_Scientific        The modifier is Scientific format Decimal.
     @constant     ModifierType_Shortest          The modifier is Shortest Decimal, either base-10 format or Scientific.
     @constant     ModifierType_Long              The modifier is Long, depends on the Base type.
     @constant     ModifierType_Signed            The modifier is a Signed Integer.
     @constant     ModifierType_Unsigned          The modifier is a Unsigned Integer.
     */
    typedef enum FormatIO_ModifierTypes {
                   ModifierType_Unspecified       = 0,
                   ModifierType_UTF8              = 1,
                   ModifierType_UTF16             = 2,
                   ModifierType_UTF32             = 4,
                   ModifierType_Radix8            = 8,
                   ModifierType_Radix10           = 16,
                   ModifierType_Radix16           = 32,
                   ModifierType_Uppercase         = 64,
                   ModifierType_Lowercase         = 128,
                   ModifierType_Scientific        = 256,
                   ModifierType_Shortest          = 512,
                   ModifierType_Long              = 1024,
                   ModifierType_Signed            = 2048,
                   ModifierType_Unsigned          = 4096,
    } FormatIO_ModifierTypes;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FormatIO_ModifierTypes operator | (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            return static_cast<FormatIO_ModifierTypes>(static_cast<uint32_t>(A) | static_cast<uint32_t>(B));
        }
        
        constexpr inline FormatIO_ModifierTypes operator & (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            return static_cast<FormatIO_ModifierTypes>(static_cast<uint32_t>(A) & static_cast<uint32_t>(B));
        }
        
        constexpr inline FormatIO_ModifierTypes operator |= (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            uint32_t A1 = static_cast<uint32_t>(A);
            uint32_t B1 = static_cast<uint32_t>(B);
            return static_cast<FormatIO_ModifierTypes>(A1 | B1);
        }
        
        constexpr inline FormatIO_ModifierTypes operator &= (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            uint32_t A1 = static_cast<uint32_t>(A);
            uint32_t B1 = static_cast<uint32_t>(B);
            return static_cast<FormatIO_ModifierTypes>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_ModifierLengths
     @abstract                                    Defines the lengths of each format specifier, generally Integer or Decimal sizes.
     @constant     ModifierLength_Unspecified     Invalid modifier length.
     @constant     ModifierLength_8Bit            The length is 8 bit.
     @constant     ModifierLength_16Bit           The length is 16 bit.
     @constant     ModifierLength_32Bit           The length is 32 bit.
     @constant     ModifierLength_64Bit           The length is 64 bit.
     @constant     ModifierLength_SizeType        The length is equal to size_t, generally 32 or 64 bit depending on platform.
     @constant     ModifierLength_PointerDiff     The length is Pointer Diff as in ptrdiff_t/uptrdiff_t.
     @constant     ModifierLength_IntMax          The length is IntMax as in intmax_t.
     */
    typedef enum FormatIO_ModifierLengths {
                   ModifierLength_Unspecified     = 0,
                   ModifierLength_8Bit            = 1,
                   ModifierLength_16Bit           = 2,
                   ModifierLength_32Bit           = 4,
                   ModifierLength_64Bit           = 8,
                   ModifierLength_SizeType        = 16,
                   ModifierLength_PointerDiff     = 32,
                   ModifierLength_IntMax          = 64,
    } FormatIO_ModifierLengths;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FormatIO_ModifierLengths operator | (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            return static_cast<FormatIO_ModifierLengths>(static_cast<uint8_t>(A) | static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_ModifierLengths operator & (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            return static_cast<FormatIO_ModifierLengths>(static_cast<uint8_t>(A) & static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_ModifierLengths operator |= (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_ModifierLengths>(A1 | B1);
        }
        
        constexpr inline FormatIO_ModifierLengths operator &= (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_ModifierLengths>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_Flags
     @abstract                                    Defines the Flags of each format specifier.
     @constant     Flag_Unspecified               There is no Flag.
     @constant     Flag_Zero_Pad                  Pad the number with leading zeros.
     @constant     Flag_Space_Pad                 Pad the string with spaces or Integer with a prefixed space for positive integers.
     @constant     Flag_Plus_AddSign              Add the + sign to positive Integers.
     @constant     Flag_Minus_LeftJustify         Left justify a field, right justify is default.
     @constant     Flag_Pound_PrefixBase          Prefix '0' for radix8, 0x/0X for radix16 on non-zero Integers.
     @constant     Flag_Pound_DecimalSuffix       The length is IntMax as in intmax_t.
     @constant     Flag_Apostrophe_ExponentGroup  The Integer or Decimals Exponent has grouping seperators applied.
     */
    typedef enum FormatIO_Flags {
                   Flag_Unspecified               = 0,
                   Flag_Zero_Pad                  = 1,
                   Flag_Space_Pad                 = 2,
                   Flag_Plus_AddSign              = 4,
                   Flag_Minus_LeftJustify         = 8,
                   Flag_Pound_PrefixBase          = 16,
                   Flag_Pound_DecimalSuffix       = 32,
                   Flag_Apostrophe_ExponentGroup  = 64,
    } FormatIO_Flags;
    
    /*!
     @enum         FormatIO_MinWidths
     @abstract                                    The Minimum Widths of each format specifier.
     @constant     MinWidth_Unspecified           There is no Flag.
     @constant     MinWidth_Asterisk_NextArg      The minimum width is the next argument.
     @constant     MinWidth_Inline_Digits         The minimum width is in the format specifier.
     */
    typedef enum FormatIO_MinWidths {
                   MinWidth_Unspecified           = 0,
                   MinWidth_Asterisk_NextArg      = 1,
                   MinWidth_Inline_Digits         = 2,
    } FormatIO_MinWidths;
    
    /*!
     @enum         FormatIO_Positions
     @abstract                                    The Position of each format specifier.
     @constant     Position_Unspecified           The format specifier is not Positional.
     @constant     Position_Asterisk_NextArg      The Position index is the next argument.
     @constant     MinWidth_Inline_Digits         The Position index is in the format specifier.
     */
    typedef enum FormatIO_Positions {
                   Position_Unspecified           = 0,
                   Position_Asterisk_NextArg      = 1,
                   Position_Inline_Digits         = 2,
    } FormatIO_Positions;
    
    /*!
     @enum         FormatIO_Precisions
     @abstract                                    The Precision of each format specifier.
     @constant     Precision_Unspecified          The Precision is unspecified.
     @constant     Position_Asterisk_NextArg      The Precision is the next argument.
     @constant     MinWidth_Inline_Digits         The Precision is in the format specifier.
     */
    typedef enum FormatIO_Precisions {
                   Precision_Unspecified          = 0,
                   Precision_Asterisk_NextArg     = 1,
                   Precision_Inline_Digits        = 2,
    } FormatIO_Precisions;
    
    typedef struct    FormatSpecifiers            FormatSpecifiers;
    
    /*!
     @abstract                                    Creates a instance of FormatSpecifiers.
     @param           NumSpecifiers               The number of Specifiers to create.
     @return                                      Returns the newly created FormatSpecifiers.
     */
    FormatSpecifiers *FormatSpecifiers_Init(uint64_t NumSpecifiers);
    
    /*!
     @remark                                      MUST be called after Format_Specifiers_RetrieveArguments
     */
    uint64_t          UTF32_GetFormattedStringSize(PlatformIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers);
    
    /*!
     @abstract                                    Parses the format specifiers.
     @param           Format                      The string to substitute the specifiers with.
     @param           Specifiers                  The Format Specifiers.
     @param           StringType                  The original encoding of the Format string.
     */
    void              UTF32_ParseFormatSpecifiers(PlatformIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, TextIO_StringTypes StringType);
    
    /*!
     @abstract                                    Formats a UTF-32 encoded string.
     @param               Specifiers              The Format Specifiers.
     @param               Format                  The string to substitute the specifiers with.
     @return                                      Returns the formatted string.
     */
    UTF32            *FormatString_UTF32(PlatformIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, uint64_t FormattedStringSize);
    
    /*!
     @abstract                                    Gets the Variadic Arguments from a Va_list, and adds them to FormatSpecifiers.
     @param               Specifiers              Type to hold the Specifiers' data.
     @param               Arguments               The variadic arguments.
     */
    void              Format_Specifiers_RetrieveArguments(FormatSpecifiers *Specifiers, va_list Arguments);
    
    /*!
     @abstract                                    Deletes an instance of FormatSpecifiers.
     @param               Specifiers              The Specifiers to delete.
     */
    void              FormatSpecifiers_Deinit(FormatSpecifiers *Specifiers);
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param               Format                  A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-8 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    UTF8             *UTF8_Format(PlatformIO_Immutable(UTF8 *) Format, ...) __attribute__((format(printf, 1, 2)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    UTF8             *UTF8_Format(PlatformIO_Immutable(UTF8 *) Format, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF8             *UTF8_Format(__format_string PlatformIO_Immutable(UTF8 *) Format, ...);
#elif (_MSC_VER >= 1500)
    UTF8             *UTF8_Format(_Printf_format_string_ PlatformIO_Immutable(UTF8 *) Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param               Format                  A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-8 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    UTF16                *UTF16_Format(PlatformIO_Immutable(UTF16 *) Format, ...) __attribute__((format(wprintf, 1, 2)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    UTF16                *UTF16_Format(PlatformIO_Immutable(UTF16 *) Format, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF16                *UTF16_Format(__format_string PlatformIO_Immutable(UTF16 *) Format, ...);
#elif (_MSC_VER >= 1500)
    UTF16                *UTF16_Format(_Printf_format_string_ PlatformIO_Immutable(UTF16 *) Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param               Format                  A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-8 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    UTF32                *UTF32_Format(PlatformIO_Immutable(UTF32 *) Format, ...) __attribute__((format(wprintf, 1, 2)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    UTF32                *UTF32_Format(PlatformIO_Immutable(UTF32 *) Format, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF32                *UTF32_Format(__format_string PlatformIO_Immutable(UTF32 *) Format, ...);
#elif (_MSC_VER >= 1500)
    UTF32                *UTF32_Format(_Printf_format_string_ PlatformIO_Immutable(UTF32 *) Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param               Format                  A string with format specifiers.
     @param               Source                  The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF8                **UTF8_Deformat(PlatformIO_Immutable(UTF8 *) Format, PlatformIO_Immutable(UTF8 *) Source);
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param               Format                  A string with format specifiers.
     @param               Source                  The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF16               **UTF16_Deformat(PlatformIO_Immutable(UTF16 *) Format, PlatformIO_Immutable(UTF16 *) Source);
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param               Format                  A string with format specifiers.
     @param               Source                  The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF32               **UTF32_Deformat(PlatformIO_Immutable(UTF32 *) Format, PlatformIO_Immutable(UTF32 *) Source);
    
    /*!
     @abstract                                    Deformats a UTF-32 encoded string, essentially scanf replacement.
     @param               Format                  The string with specifiers to parse.
     @param               Specifiers              The Format Specifiers.
     @param               Result                  The formatted string to extract the data for the StringSet into.
     @return                                      Returns a StringSet for each specifier.
     */
    UTF32               **DeformatString_UTF32(PlatformIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, PlatformIO_Immutable(UTF32 *) Result);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_FormatIO_H */
