/*!
 @header              FormatIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for string formatting/deformatting
 */

#include "../Macros.h"
#include "UnicodeTypes.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_FormatIO_H
#define FoundationIO_UnicodeIO_FormatIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum                StringIOStringTypes
     @constant            StringType_Unknown                    "Byte order is unknown".
     @constant            StringType_UTF8                       "Use the byte order of the host".
     @constant            StringType_UTF16                      "Use the little endian, Least-Significant-Byte first order".
     @constant            StringType_UTF32                      "Use the big endian, Most-Significant-Byte first order".
     */
    typedef enum StringIOStringTypes {
                          StringType_Unknown                    = 0,
                          StringType_UTF8                       = 1,
                          StringType_UTF16                      = 2,
                          StringType_UTF32                      = 4,
    } StringIOStringTypes;
    
    typedef struct        FormatSpecifiers                      FormatSpecifiers;
    
    /*!
     @abstract                                                  "Creates a instance of FormatSpecifiers".
     @param               NumSpecifiers                         "The number of Specifiers to create".
     @return                                                    "Returns the newly created FormatSpecifiers".
     */
    FormatSpecifiers     *FormatSpecifiers_Init(uint64_t NumSpecifiers);
    
    /*!
     @abstract                                                  "Parses the format specifiers".
     @param               Format                                "The string to substitute the specifiers with".
     @param               Specifiers                            "The Format Specifiers".
     @param               StringType                            "The original encoding of the Format string".
     */
    void                  UTF32_ParseFormatSpecifiers(UTF32 *Format, FormatSpecifiers *Specifiers, StringIOStringTypes StringType);
    
    /*!
     @abstract                                                  "Formats a UTF-32 encoded string".
     @param               Specifiers                            "The Format Specifiers".
     @param               Format                                "The string to substitute the specifiers with".
     @return                                                    "Returns the formatted string".
     */
    UTF32                *FormatString_UTF32(FormatSpecifiers *Specifiers, UTF32 *Format);
    
    /*!
     @abstract                                                  "Deletes an instance of FormatSpecifiers".
     @param               Specifiers                            "The Specifiers to delete".
     */
    void                  FormatSpecifiers_Deinit(FormatSpecifiers *Specifiers);
    
    /*!
     @abstract                                                  "Formats a string according to the Format string".
     @param               Format                                "A string with optional format specifiers, all variadic strings need to match Format's type".
     @return                                                    "Returns the formatted string encoded using the UTF-8 format".
     */
#if   (FoundationIOCompiler == FoundationIOCompilerIsClang)
    UTF8                 *UTF8_Format(UTF8 *Format, ...) __attribute__((__format__(__printf__, 1, 2)));
#elif (FoundationIOCompiler == FoundationIOCompilerIsGCC)
    UTF8                 *UTF8_Format(UTF8 *Format, ...);
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#include <sal.h>
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF8                 *UTF8_Format(__format_string UTF8 *Format, ...);
#elif (_MSC_VER >= 1500)
    UTF8                 *UTF8_Format(_Printf_format_string_ UTF8 *Format, ...);
#endif /* MSVC Version */
#endif /* FoundationIOCompiler */
    
    /*!
     @abstract                                                  "Formats a string according to the Format string".
     @param               Format                                "A string with optional format specifiers, all variadic strings need to match Format's type".
     @return                                                    "Returns the formatted string encoded using the UTF-8 format".
     */
#if   (FoundationIOCompiler == FoundationIOCompilerIsClang)
    UTF16                *UTF16_Format(UTF16 *Format, ...) __attribute__((__format__(__wprintf__, 1, 2)));
#elif (FoundationIOCompiler == FoundationIOCompilerIsGCC)
    UTF16                *UTF16_Format(UTF16 *Format, ...);
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#include <sal.h>
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF16                *UTF16_Format(__format_string UTF16 *Format, ...);
#elif (_MSC_VER >= 1500)
    UTF16                *UTF16_Format(_Printf_format_string_ UTF16 *Format, ...);
#endif /* MSVC Version */
#endif /* FoundationIOCompiler */
    
    /*!
     @abstract                                                  "Formats a string according to the Format string".
     @param               Format                                "A string with optional format specifiers, all variadic strings need to match Format's type".
     @return                                                    "Returns the formatted string encoded using the UTF-8 format".
     */
#if   (FoundationIOCompiler == FoundationIOCompilerIsClang)
    UTF32                *UTF32_Format(UTF32 *Format, ...) __attribute__((__format__(__wprintf__, 1, 2)));
#elif (FoundationIOCompiler == FoundationIOCompilerIsGCC)
    UTF32                *UTF32_Format(UTF32 *Format, ...);
#elif (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#include <sal.h>
#if   (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF32                *UTF32_Format(__format_string UTF32 *Format, ...);
#elif (_MSC_VER >= 1500)
    UTF32                *UTF32_Format(_Printf_format_string_ UTF32 *Format, ...);
#endif /* MSVC Version */
#endif /* FoundationIOCompiler */
    
    /*!
     @abstract                                                  "Gets the Variadic Arguments from a Va_list, and adds them to FormatSpecifiers".
     @param               Specifiers                            "Type to hold the Specifiers' data".
     @param               Arguments                             "The variadic arguments".
     */
    void                  Format_Specifiers_RetrieveArguments(FormatSpecifiers *Specifiers, va_list Arguments);
    
    /*!
     @abstract                                                  "Splits a string based on the specifiers in Format".
     @param               Format                                "A string with format specifiers".
     @param               Source                                "The string to extract the information from".
     @return                                                    "Returns a StringArray containing NumFormatSpecifiers strings, one string for each specifier".
     */
    UTF8                **UTF8_Deformat(UTF8 *Format, UTF8 *Source);
    
    /*!
     @abstract                                                  "Splits a string based on the specifiers in Format".
     @param               Format                                "A string with format specifiers".
     @param               Source                                "The string to extract the information from".
     @return                                                    "Returns a StringArray containing NumFormatSpecifiers strings, one string for each specifier".
     */
    UTF16               **UTF16_Deformat(UTF16 *Format, UTF16 *Source);
    
    /*!
     @abstract                                                  "Splits a string based on the specifiers in Format".
     @param               Format                                "A string with format specifiers".
     @param               Source                                "The string to extract the information from".
     @return                                                    "Returns a StringArray containing NumFormatSpecifiers strings, one string for each specifier".
     */
    UTF32               **UTF32_Deformat(UTF32 *Format, UTF32 *Source);
    
    /*!
     @abstract                                                 "Deformats a UTF-32 encoded string, essentially scanf replacement".
     @param               Specifiers                           "The Format Specifiers".
     @param               Format                               "The string with specifiers to parse".
     @param               Result                               "The formatted string to extract the data for the StringArray into".
     @return                                                   "Returns a StringArray for each specifier".
     */
    UTF32               **DeformatString_UTF32(FormatSpecifiers *Specifiers, UTF32 *Format, UTF32 *Result);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_UnicodeIO_FormatIO_H */
