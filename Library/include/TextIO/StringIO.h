#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for FILE */

#include "Macros.h"                   /* Included for bool and Yes/No macros */

#pragma once

#ifndef FoundationIO_StringIO_H
#define FoundationIO_StringIO_H

#ifdef  __cplusplus
extern  "C" {
#endif
    
    /*!
     @header    StringIO.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2017+
     @version   1.0.0
     @brief     This header contains types, functions, and tables for Unicode support, including UTF-8, UTF-16, and our internal format, UTF-32.
     */
    
#ifndef                   U8
#define                   U8(QuotedLiteral)                    u8##QuotedLiteral
#endif
    
#ifndef                   U16
#define                   U16(QuotedLiteral)                   u##QuotedLiteral
#endif
    
#ifndef                   U32
#define                   U32(QuotedLiteral)                   U##QuotedLiteral
#endif
    
    typedef               uint_least8_t                        UTF8;
    typedef               uint_least16_t                       UTF16;
    typedef               uint_least32_t                       UTF32;
    
    /*!
     @enum                StringIOCommon
     */
    typedef enum StringIOCommon {
                          UTF8BOMSizeInCodeUnits               = 3,
                          UTF8BOMSizeInCodePoints              = 1,
                          UTF16LE                              = 0xFFFE,
                          UTF16BE                              = 0xFEFF,
                          UTF16HighSurrogateStart              = 0xD800,
                          UTF16HighSurrogateEnd                = 0xDBFF,
                          UTF16LowSurrogateStart               = 0xDC00,
                          UTF16LowSurrogateEnd                 = 0xDFFF,
                          UTF16MaxCodePoint                    = 0xFFFF,
                          UTF16SurrogatePairModDividend        = 0x400,
                          UTF16SurrogatePairStart              = 0x10000,
                          InvalidCodePointReplacementCharacter = 0xFFFD,
                          UTF1632BOMSizeInCodePoints           = 1,
                          UTF32LE                              = 0xFFFE,
                          UTF32BE                              = 0xFEFF,
                          UnicodeMaxCodePoint                  = 0x10FFFF,
    } StringIOCommon;
    
    /*!
     @enum                StringIONormalizationForms
     @constant            UnknownNormalizationForm             "Unknown Normalization Form".
     @constant            NormalizationFormD                   "Decompose the String".
     @constant            NormalizationFormC                   "Decompose, then Compose the String".
     @constant            NormalizationFormKD                  "The same as NormalizationFormD, plus Kompatibility decompositions".
     @constant            NormalizationFormKC                  "The same as NormalizationFormD, plus Kompatibility compositions".
     */
    typedef enum StringIONormalizationForms {
                          UnknownNormalizationForm             = 0,
                          NormalizationFormD                   = 1,
                          NormalizationFormC                   = 2,
                          NormalizationFormKD                  = 3,
                          NormalizationFormKC                  = 4,
    } StringIONormalizationForms;
    
    /*!
     @enum                StringIOBases
     @constant            Binary                               "Base 2".
     @constant            Octal                                "Base 8".
     @constant            Decimal                              "Base 10".
     @constant            Hex                                  "Base 16".
     */
    typedef enum StringIOBases {
                          Binary                               = 2,
                          Octal                                = 8,
                          Decimal                              = 10,
                          Hex                                  = 16,
    } StringIOBases;
    
    /*!
     @enum                StringIOBOMStates
     @constant            UnknownBOMState                      "It's unknown if the string contains a BOM".
     @constant            AddBOM                               "Add a BOM even if there already is one".
     @constant            RemoveBOM                            "Remove the BOM".
     @constant            KeepBOM                              "Keep the BOM".
     */
    typedef enum StringIOBOMStates {
                          UnknownBOMState                      = 0,
                          AddBOM                               = 1,
                          RemoveBOM                            = 2,
                          KeepBOM                              = 3,
    } StringIOBOMStates;
    
    /*!
     @enum                StringIOByteOrders
     @constant            UseNativeByteOrder                   "Use the byte order of the host".
     @constant            UseLEByteOrder                       "Use the little endian, Least-Significant-Byte First order".
     @constant            UseBEByteOrder                       "Use the big endian, Most-Significant-Byte First order".
     */
    typedef enum StringIOByteOrders {
                          UseNativeByteOrder                   = 0,
                          UseLEByteOrder                       = 1,
                          UseBEByteOrder                       = 2,
    } StringIOByteOrders;
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8 string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String2Count     "The string to get the number of codeunits in".
     */
    uint64_t              UTF8_GetSizeInCodeUnits(UTF8 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetSizeInCodeUnits(UTF16 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF8_GetSizeInCodePoints(UTF8 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetSizeInCodePoints(UTF16 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF32_GetSizeInCodePoints(UTF32 *String2Count);
    
    /*!
     @abstract                             "Gets the number of user visible characters in a UTF-8 string".
     @param               String           "The string to get the number of graphemes in".
     */
    uint64_t              UTF8_GetSizeInGraphemes(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of user visible characters in a UTF-16 string".
     @param               String           "The string to get the number of graphemes in".
     */
    uint64_t              UTF16_GetSizeInGraphemes(UTF16 *String);
    
    /*!
     @abstract                             "Decodes a UTF8 string to a UTF32 string".
     @param               String           "The string to decode".
     */
    UTF32                *UTF8_Decode(UTF8 *String);
    
    /*!
     @abstract                             "Decodes a UTF16 string to a UTF32 string".
     @param               String           "The string to decode".
     */
    UTF32                *UTF16_Decode(UTF16 *String);
    
    /*!
     @abstract                             "Encodes a UTF32 string to a UTF8 string".
     @param               String           "The UTF32 string to encode to a UTF8 string".
     */
    UTF8                 *UTF8_Encode(UTF32 *String, const bool IncludeBOM);
    
    /*!
     @abstract                             "Encodes a UTF32 *to a UTF16".
     @param               String           "The string to decode".
     */
    UTF16                *UTF16_Encode(UTF32 *String, StringIOByteOrders OutputByteOrder);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining codepoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @param               String2Normalize "The string to be normalized".
     @param               NormalizedForm   "The type of normalization of use on String2Normalize"
     */
    UTF8                 *UTF8_NormalizeString(UTF8 *String2Normalize, StringIONormalizationForms NormalizedForm);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining codepoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @param               String2Normalize "The string to be normalized".
     @param               NormalizedForm   "The type of normalization of use on String2Normalize"
     */
    UTF16                *UTF16_NormalizeString(UTF16 *String2Normalize, StringIONormalizationForms NormalizedForm);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String2CaseFold  "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF8                 *UTF8_CaseFoldString(UTF8 *String2CaseFold);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String2CaseFold  "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF16                *UTF16_CaseFoldString(UTF16 *String2CaseFold);
    
    /*!
     @abstract                             "Finds a substring within string, starting at codepoint Offset, and ending at Offset + Length".
     @remark                               "We do NOT casefold, or normalize the String or SubString, that's your job".
     @param               String           "The string to search for SubString in".
     @param               SubString        "The SubString to find in String".
     @param               Offset           "Where in the string should we start looking for the substring"?
     @param               Length           "How many codepoints should we search for the substring? -1 means all codepoints".
     @return                               "Returns the offset of the start of the substring in String, or -1 if a match wasn't found.".
     */
    int64_t               UTF8_FindSubString(UTF8 *String, UTF8 *SubString, uint64_t Offset, int64_t Length);
    
    /*!
     @abstract                             "Finds a substring within string, starting at codepoint Offset, and ending at Offset + Length".
     @remark                               "We do NOT casefold, or normalize the String or SubString, that's your job".
     @param               String           "The string to search for SubString in".
     @param               SubString        "The SubString to find in String".
     @param               Offset           "Where in the string should we start looking for the substring"?
     @param               Length           "How many codepoints should we search for the substring? -1 means all codepoints".
     @return                               "Returns the offset of the start of the substring in String, or -1 if a match wasn't found.".
     */
    int64_t               UTF16_FindSubString(UTF16 *String, UTF16 *SubString, uint64_t Offset, int64_t Length);
    
    /*!
     @abstract                             "Extracts a SubString from String".
     @param               String           "The string to extract from".
     @param               Offset           "The CodePoint to start extracting from".
     @param               Length           "The number of codepoints to extract".
     */
    UTF8                 *UTF8_ExtractSubString(UTF8 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Extracts a SubString from String".
     @param               String           "The string to extract from".
     @param               Offset           "The CodePoint to start extracting from".
     @param               Length           "The number of codepoints to extract".
     */
    UTF16                *UTF16_ExtractSubString(UTF16 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String2Split     "The string you want to be split".
     @param               NumDelimiters    "The number of delimiter strings in Delimiters".
     @param               Delimiters       "An array of strings containing the delimiters, one delimiter per string".
     */
    UTF8                **UTF8_SplitString(UTF8 *String2Split, uint64_t NumDelimiters, UTF8 **Delimiters);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String2Split     "The string you want to be split".
     @param               NumDelimiters    "The number of delimiter strings in Delimiters".
     @param               Delimiters       "An array of strings containing the delimiters, one delimiter per string".
     */
    UTF16               **UTF16_SplitString(UTF16 *String2Split, uint64_t NumDelimiters, UTF16 **Delimiters);
    
    /*!
     @abstract                             "Replaces a section in String starting at Offset and ending at Offset + Length with Replacement".
     @param               String           "The string to edit".
     @param               Replacement      "The string to splice in".
     @param               Offset           "Where to start replacing String with Replacement".
     @param               Length           "The number of codepoints to replace, can be more or less than Replacement".
     */
    UTF8                 *UTF8_ReplaceSubString(UTF8 *String, UTF8 *Replacement, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Replaces a section in String starting at Offset and ending at Offset + Length with Replacement".
     @param               String           "The string to edit".
     @param               Replacement      "The string to splice in".
     @param               Offset           "Where to start replacing String with Replacement".
     @param               Length           "The number of codepoints to replace, can be more or less than Replacement".
     */
    UTF16                *UTF16_ReplaceSubString(UTF16 *String, UTF16 *Replacement, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Extracts a number from a string".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF8_String2Integer(UTF8 *String);
    
    /*!
     @abstract                             "Extracts a number from a string".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF16_String2Integer(UTF16 *String);
    
    /*!
     @abstract                             "Converts an integer to a string".
     @param               UpperCase        "If the Base is Hex, should the output string be upper case"?
     @param               Base             "The base to output the integer in".
     @param               Integer2Convert  "The number to convert into a string".
     */
    UTF8                 *UTF8_Integer2String(const StringIOBases Base, const bool UpperCase, int64_t Integer2Convert);
    
    /*!
     @abstract                             "Converts an integer to a string".
     @param               UpperCase        "If the Base is Hex, should the output string be upper case"?
     @param               Base             "The base to output the integer in".
     @param               Integer2Convert  "The number to convert into a string".
     */
    UTF16                *UTF16_Integer2String(const StringIOBases Base, const bool UpperCase, int64_t Integer2Convert);
    
    /*!
     @abstract                             "Converts a string to a double, replaces strtod and atof".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                UTF8_String2Decimal(UTF8 *String);
    
    /*!
     @abstract                             "Converts a string to a double, replaces strtod and atof".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                UTF16_String2Decimal(UTF16 *String);
    
    /*!
     @abstract                             "Converts a double to a string, replaces strtod and atof".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF8                 *UTF8_Decimal2String(double Decimal);
    
    /*!
     @abstract                             "Converts a double to a string, replaces strtod and atof".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF16                *UTF16_Decimal2String(double Decimal);
    
    /*!
     @abstract                             "Compares String1 and String2 for equilivence".
     @remark                               "String1 and String2 MUST use the same Unicode encoding".
     @param               String1          "String1 Pointer".
     @param               String2          "String2 Pointer".
     @param               NormalizedForm   "The normalization to use before comparing the strings".
     @param               CaseInsensitive  "Should the strings be casefolded first"?
     @return                               "Returns wether the strings match or not".
     */
    bool                  UTF8_Compare(UTF8 *String1, UTF8 *String2, StringIONormalizationForms NormalizedForm, bool CaseInsensitive);
    
    /*!
     @abstract                             "Compares String1 and String2 for equilivence".
     @remark                               "String1 and String2 MUST use the same Unicode encoding".
     @param               String1          "String1 Pointer".
     @param               String2          "String2 Pointer".
     @param               NormalizedForm   "The normalization to use before comparing the strings".
     @param               CaseInsensitive  "Should the strings be casefolded first"?
     @return                               "Returns wether the strings match or not".
     */
    bool                  UTF16_Compare(UTF16 *String1, UTF16 *String2, StringIONormalizationForms NormalizedForm, bool CaseInsensitive);
    
    /*!
     @abstract                             "Formats a string according to the Format string, with all of it's options".
     @remark                               "We've extended FormatString from printf by adding the B specifier, for binary".
     @remark                               "We do not support the n specifier for security purposes".
     @param               Format           "A string with optional format specifiers".
     @return                               "Returns the formatted string encoded using the UTF-8 format".
     */
    UTF8                 *UTF8_FormatString(UTF8 *Format, ...) __attribute__((__format__ (__printf__, 1, 2)));
    
    /*!
     @abstract                             "Formats a string according to the Format string, with all of it's options".
     @remark                               "We've extended FormatString from printf by adding the B specifier, for binary".
     @remark                               "We do not support the n specifier for security purposes".
     @param               Format           "A string with optional format specifiers".
     @return                               "Returns the formatted string encoded using the UTF-16 format".
     */
    UTF16                *UTF16_FormatString(UTF16 *Format, ...) __attribute__((__format__ (__wprintf__, 1, 2)));
    
    /*!
     @abstract                             "Writes a UTF-8 encoded string to the OutputFile using the platform's default Unicode encoding".
     @remark                               "On Windows, Strings are converted to UTF-16; on POSIX platforms (including Mac/iOS) strings are written as UTF-8".
     @param               String2Write     "The string to write to OutputFile (including any newlines, etc)".
     @param               OutputFile       "a valid FILE pointer, or STDIN/STDOUT/STDERR".
     */
    void                  UTF8_WriteString2File(UTF8 *String2Write, FILE *OutputFile);
    
    /*!
     @abstract                             "Writes a UTF-16 encoded string to the OutputFile using the platform's default Unicode encoding".
     @remark                               "On Windows, Strings are output as UTF-16; on POSIX platforms (including Mac/iOS) strings are written as UTF-8".
     @param               String2Write     "The string to write to OutputFile (including any newlines, etc)".
     @param               OutputFile       "a valid FILE pointer, or STDIN/STDOUT/STDERR".
     */
    void                  UTF16_WriteString2File(UTF16 *String2Write, FILE *OutputFile);
    
#define CompareStrings(String1, String2, NormalizedForm, CaseInsensitive) _Generic((String1, String2), UTF8*:UTF8_Compare, UTF16*:UTF16_Compare)(String1, String2, NormalizedForm, CaseInsensitive)
    
#ifdef  __cplusplus
}
#endif

#endif  /* FoundationIO_StringIO_H */
