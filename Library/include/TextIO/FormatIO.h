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
     @param           Specifiers                  The Format Specifiers.
     @param           Format                      The string to substitute the specifiers with.
     @return                                      Returns the formatted string.
     */
    UTF32            *FormatString_UTF32(PlatformIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, uint64_t FormattedStringSize);
    
    /*!
     @abstract                                    Gets the Variadic Arguments from a Va_list, and adds them to FormatSpecifiers.
     @param           Specifiers                  Type to hold the Specifiers' data.
     @param           Arguments                   The variadic arguments.
     */
    void              Format_Specifiers_RetrieveArguments(FormatSpecifiers *Specifiers, va_list Arguments);
    
    /*!
     @abstract                                    Deletes an instance of FormatSpecifiers.
     @param           Specifiers                  The Specifiers to delete.
     */
    void              FormatSpecifiers_Deinit(FormatSpecifiers *Specifiers);
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param           Format                      A string with optional format specifiers.
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
     @param           Format                      A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-8 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    UTF16            *UTF16_Format(PlatformIO_Immutable(UTF16 *) Format, ...) __attribute__((format(wprintf, 1, 2)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    UTF16            *UTF16_Format(PlatformIO_Immutable(UTF16 *) Format, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF16            *UTF16_Format(__format_string PlatformIO_Immutable(UTF16 *) Format, ...);
#elif (_MSC_VER >= 1500)
    UTF16            *UTF16_Format(_Printf_format_string_ PlatformIO_Immutable(UTF16 *) Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Formats a string according to the Format string.
     @param           Format                      A string with optional format specifiers.
     @return                                      Returns the formatted string encoded using the UTF-8 format.
     */
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang)
    UTF32            *UTF32_Format(PlatformIO_Immutable(UTF32 *) Format, ...) __attribute__((format(wprintf, 1, 2)));
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
    UTF32            *UTF32_Format(PlatformIO_Immutable(UTF32 *) Format, ...);
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF32            *UTF32_Format(__format_string PlatformIO_Immutable(UTF32 *) Format, ...);
#elif (_MSC_VER >= 1500)
    UTF32            *UTF32_Format(_Printf_format_string_ PlatformIO_Immutable(UTF32 *) Format, ...);
#endif /* MSVC Version */
#endif /* PlatformIO_Compiler */
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param           Format                      A string with format specifiers.
     @param           Source                      The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF8            **UTF8_Deformat(PlatformIO_Immutable(UTF8 *) Format, PlatformIO_Immutable(UTF8 *) Source);
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param           Format                      A string with format specifiers.
     @param           Source                      The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF16           **UTF16_Deformat(PlatformIO_Immutable(UTF16 *) Format, PlatformIO_Immutable(UTF16 *) Source);
    
    /*!
     @abstract                                    Splits a string based on the specifiers in Format.
     @param           Format                      A string with format specifiers.
     @param           Source                      The string to extract the information from.
     @return                                      Returns a StringSet containing NumFormatSpecifiers strings, one string for each specifier.
     */
    UTF32           **UTF32_Deformat(PlatformIO_Immutable(UTF32 *) Format, PlatformIO_Immutable(UTF32 *) Source);
    
    /*!
     @abstract                                    Deformats a UTF-32 encoded string, essentially scanf replacement.
     @param           Format                      The string with specifiers to parse.
     @param           Specifiers                  The Format Specifiers.
     @param           Result                      The formatted string to extract the data for the StringSet into.
     @return                                      Returns a StringSet for each specifier.
     */
    UTF32           **DeformatString_UTF32(PlatformIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, PlatformIO_Immutable(UTF32 *) Result);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_FormatIO_H */
