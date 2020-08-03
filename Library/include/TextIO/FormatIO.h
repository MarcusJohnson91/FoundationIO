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

#if   (PlatformIO_TargetOS == PlatformIO_WindowsOS)
#include <sal.h>
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @enum         FormatIO_BaseTypes
     */
    typedef enum FormatIO_BaseTypes {
                   BaseType_Unspecified           = 0,
                   BaseType_Integer               = 1,
                   BaseType_Decimal               = 2,
                   BaseType_CodeUnit              = 4,
                   BaseType_String                = 8,
                   BaseType_Literal               = 16,
                   BaseType_Pointer               = 32,
                   BaseType_Remove                = 64,
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
            return static_cast<FormatIO_BaseTypes>(A1 |= B1);
        }
        
        constexpr inline FormatIO_BaseTypes operator &= (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_BaseTypes>(A1 &= B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_ModifierTypes
     */
    typedef enum FormatIO_ModifierTypes {
                   ModifierType_Unspecified       = 0,
                   ModifierType_Percent           = 1,
                   ModifierType_UTF8              = 2,
                   ModifierType_UTF16             = 4,
                   ModifierType_UTF32             = 8,
                   ModifierType_Integer           = 16,
                   ModifierType_Radix2            = 32,
                   ModifierType_Radix8            = 64,
                   ModifierType_Decimal           = 128,
                   ModifierType_Radix10           = 256,
                   ModifierType_Radix16           = 512,
                   ModifierType_Uppercase         = 1024,
                   ModifierType_Lowercase         = 2048,
                   ModifierType_Scientific        = 4096,
                   ModifierType_Shortest          = 8192,
                   ModifierType_Long              = 16384,
                   ModifierType_Signed            = 32768,
                   ModifierType_Unsigned          = 65536,
                   ModifierType_N                 = 131072,
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
            return static_cast<FormatIO_ModifierTypes>(A1 |= B1);
        }
        
        constexpr inline FormatIO_ModifierTypes operator &= (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            uint32_t A1 = static_cast<uint32_t>(A);
            uint32_t B1 = static_cast<uint32_t>(B);
            return static_cast<FormatIO_ModifierTypes>(A1 &= B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_ModifierLengths
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
            return static_cast<FormatIO_ModifierLengths>(A1 |= B1);
        }
        
        constexpr inline FormatIO_ModifierLengths operator &= (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_ModifierLengths>(A1 &= B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_Flags
     */
    typedef enum FormatIO_Flags {
                   Flag_Unspecified               = 0,
                   Flag_Zero_Pad                  = 1,
                   Flag_Space_Pad                 = 2,
                   Flag_Plus_AddSign              = 4,
                   Flag_Pound_Present             = 8,
                   Flag_Minus_LeftJustify         = 16,
                   Flag_Pound_PrefixOctal         = 32,
                   Flag_Pound_PrefixBase          = 64,
                   Flag_Pound_DecimalSuffix       = 128,
                   Flag_Apostrophe_Present        = 256,
                   Flag_Apostrophe_ExponentGroup  = 512,
    } FormatIO_Flags;
    
    /*!
     @enum         FormatIO_MinWidths
     */
    typedef enum FormatIO_MinWidths {
                   MinWidth_Unspecified           = 0,
                   MinWidth_Asterisk_NextArg      = 1,
                   MinWidth_Inline_Digits         = 2,
    } FormatIO_MinWidths;
    
    /*!
     @enum         FormatIO_Positions
     */
    typedef enum FormatIO_Positions {
                   Position_Unspecified           = 0,
                   Position_Asterisk_NextArg      = 1,
                   Position_Inline_Digits         = 2,
    } FormatIO_Positions;
    
    /*!
     @enum         FormatIO_Precisions
     */
    typedef enum FormatIO_Precisions {
                   Precision_Unspecified          = 0,
                   Precision_Asterisk_NextArg     = 1,
                   Precision_Inline_Digits        = 2,
    } FormatIO_Precisions;
    
    typedef struct FormatSpecifiers               FormatSpecifiers;
    
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
    void              UTF32_ParseFormatSpecifiers(PlatformIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, UnicodeIO_StringTypes StringType);
    
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
    @abstract                                     Formats a string according to the Format string.
    @param               Format                   A string with optional format specifiers.
    @return                                       Returns the formatted string encoded using the UTF-8 format.
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
    @abstract                                     Formats a string according to the Format string.
    @param               Format                   A string with optional format specifiers.
    @return                                       Returns the formatted string encoded using the UTF-8 format.
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
    @abstract                                     Splits a string based on the specifiers in Format.
    @param               Format                   A string with format specifiers.
    @param               Source                   The string to extract the information from.
    @return                                       Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
    */
    UTF16               **UTF16_Deformat(PlatformIO_Immutable(UTF16 *) Format, PlatformIO_Immutable(UTF16 *) Source);
    
    /*!
    @abstract                                     Splits a string based on the specifiers in Format.
    @param               Format                   A string with format specifiers.
    @param               Source                   The string to extract the information from.
    @return                                       Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
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
