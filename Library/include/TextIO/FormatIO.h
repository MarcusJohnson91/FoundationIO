/*!
 @header          FormatIO.h
 @author          Marcus Johnson
 @copyright       2019+
 @version         1.0.0
 @brief           This header contains types, functions, and tables for string formatting/deformatting
 */

#include "TextIOTypes.h"    /* Included for Text types */

#pragma once

#ifndef FoundationIO_TextIO_FormatIO_H
#define FoundationIO_TextIO_FormatIO_H

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#include <sal.h>
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param           Format                      A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-8 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    UTF8             *UTF8_Format(ImmutableString_UTF8 Format, ...) __attribute__((format(printf, 1, 2)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    UTF8             *UTF8_Format(ImmutableString_UTF8 Format, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF8             *UTF8_Format(__format_string ImmutableString_UTF8 Format, ...);
#elif (_MSC_VER >= 1500)
    UTF8             *UTF8_Format(_Printf_format_string_ ImmutableString_UTF8 Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param           Format                      A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-16 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
    UTF16            *UTF16_Format(ImmutableString_UTF16 Format, ...) __attribute__((format(wprintf, 1, 2)));
#pragma clang diagnostic pop
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
    UTF16            *UTF16_Format(ImmutableString_UTF16 Format, ...) __attribute__((format(wprintf, 1, 2)));
#pragma GCC diagnostic pop
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF16            *UTF16_Format(__format_string ImmutableString_UTF16 Format, ...);
#elif (_MSC_VER >= 1500)
    UTF16            *UTF16_Format(_Printf_format_string_ ImmutableString_UTF16 Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param           Format                      A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-32 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
    UTF32            *UTF32_Format(ImmutableString_UTF32 Format, ...) __attribute__((__format__(__wprintf__, 1, 2)));
#pragma clang diagnostic pop
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
    UTF32            *UTF32_Format(ImmutableString_UTF32 Format, ...) __attribute__((format(wprintf, 1, 2)));
#pragma GCC diagnostic pop
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF32            *UTF32_Format(__format_string ImmutableString_UTF32 Format, ...);
#elif (_MSC_VER >= 1500)
    UTF32            *UTF32_Format(_Printf_format_string_ ImmutableString_UTF32 Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param           Format                      A string with format specifiers.
     @param           Source                      The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF8            **UTF8_Deformat(ImmutableString_UTF8 Format, ImmutableString_UTF8 Source);
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param           Format                      A string with format specifiers.
     @param           Source                      The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF16           **UTF16_Deformat(ImmutableString_UTF16 Format, ImmutableString_UTF16 Source);
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param           Format                      A string with format specifiers.
     @param           Source                      The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF32           **UTF32_Deformat(ImmutableString_UTF32 Format, ImmutableString_UTF32 Source);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_FormatIO_H */
