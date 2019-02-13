#include <stdbool.h>
#include <stdint.h>

#include "Macros.h"

#pragma once

#ifndef FoundationIO_StringIO_H
#define FoundationIO_StringIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header              StringIO.h
     @author              Marcus Johnson
     @copyright           2017+
     @version             1.0.0
     @brief               This header contains types, functions, and tables for Unicode support, including UTF-8, UTF-16, and our internal format, UTF-32.
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
    
#ifndef UTF8
#ifdef  char8_t
    typedef               char8_t                              UTF8;
#else
    typedef               unsigned char                        UTF8;
#endif /* char8_t */
#endif /* UTF8 */
    
#ifndef UTF16
#ifdef  char16_t
    typedef               char16_t                             UTF16;
#else
    typedef               uint_least16_t                       UTF16;
#endif /* char16_t */
#endif /* UTF16 */
    
#ifndef UTF32
#ifdef  char32_t
    typedef               char32_t                             UTF32;
#else
    typedef               uint_least32_t                       UTF32;
#endif /* char32_t */
#endif /* UTF32 */
    
    /*!
     @enum                StringIOCommon
     @constant            UTF8BOMSizeInCodeUnits               "The number of code units (8 bits) the UTF8 BOM takes".
     @constant            UTF16BOMSizeInCodeUnits              "The number of code units (16 bits) the UTF16 BOM takes".
     @constant            UnicodeBOMSizeInCodePoints           "The number of codepoints in a BOM".
     @constant            UTF16BOM_LE                          "UTF16BOM_LE byte order mark".
     @constant            UTF16BOM_BE                          "UTF16BOM_BE byte order mark".
     @constant            UTF32BOM_LE                          "UTF32BOM_LE byte order mark".
     @constant            UTF32BOM_BE                          "UTF32BOM_LE byte order mark".
     @constant            UnicodeWinPathPrefixSize             "Size of "//?/" in CodePoints and CodeUnits"
     @constant            UTF16HighSurrogateStart              "The value that marks the start of the High Surrogate range".
     @constant            UTF16HighSurrogateEnd                "The value that marks the end   of the High Surrogate range".
     @constant            UTF16LowSurrogateStart               "The value that marks the start of the Low  Surrogate range".
     @constant            UTF16LowSurrogateEnd                 "The value that marks the end   of the Low  Surrogate range".
     @constant            UTF16MaxCodePoint                    "The highest value that can be stored in a single UTF16 codeunit".
     @constant            UTF16SurrogatePairModDividend        "The value to modulo the surrogate pair by to decode a High Surrogate".
     @constant            UTF16SurrogatePairStart              "The first UTF-32 codepoint to require Surrogate Pairs in UTF-16".
     @constant            InvalidReplacementCodePoint          "The codepoint to replace invalid codeunits".
     @constant            UnicodeMaxCodePoint                  "The highest codepoint possible in Unicode, 1,114,111".
     */
    typedef enum StringIOCommon {
                          UTF8BOMSizeInCodeUnits               = 3,
                          UTF16BOMSizeInCodeUnits              = 1,
                          UnicodeBOMSizeInCodePoints           = 1,
                          UTF8BOM_1                            = 0xEF,
                          UTF8BOM_2                            = 0xBB,
                          UTF8BOM_3                            = 0xBF,
                          UTF16BOM_LE                          = 0xFFFE,
                          UTF16BOM_BE                          = 0xFEFF,
                          UTF32BOM_LE                          = 0xFFFE,
                          UTF32BOM_BE                          = 0xFEFF,
                          UnicodeWinPathPrefixSize             = 4, // "//?/" or "\\?\"
                          UTF16HighSurrogateStart              = 0xD800,
                          UTF16HighSurrogateEnd                = 0xDBFF,
                          UTF16LowSurrogateStart               = 0xDC00,
                          UTF16LowSurrogateEnd                 = 0xDFFF,
                          UTF16MaxCodePoint                    = 0xFFFF,
                          UTF16SurrogatePairModDividend        = 0x400,
                          UTF16SurrogatePairStart              = 0x10000,
                          InvalidReplacementCodePoint          = 0xFFFD,
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
     @constant            Integer                              "Integer".
     @constant            Decimal                              "Decimal".
     @constant            Base2                                "Base 2".
     @constant            Base8                                "Base 8".
     @constant            Base10                               "Base 10".
     @constant            Base16                               "Base 16".
     @constant            Uppercase                            "Digits 0-9A-F".
     @constant            Lowercase                            "Digits 0-9a-f".
     @constant            Scientific                           "392.96e+2 Scientific decimal representation, ".
     @constant            Shortest                             "Use either Decimal or Scientific representation".
     @constant            Hex                                  "1.3DEp42 where E is the exponent aka 10^X, and P is the power aka 2^Y"
     */
    typedef enum StringIOBases {
                          Integer                              = 1,
                          Decimal                              = 2,
                          Base2                                = 4,
                          Base8                                = 8,
                          Base10                               = 16,
                          Base16                               = 32,
                          Uppercase                            = 64,
                          Lowercase                            = 128,
                          Scientific                           = 256,
                          Shortest                             = 512,
                          Hex                                  = 1024,
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
     @enum                StringTypes
     @constant            UnknownStringType                    "Could be anything".
     @constant            UTF8Format                           "The string is UTF-8 encoded".
     @constant            UTF16Format                          "The string is UTF-15 encoded".
     @constant            UTF32Format                          "The string is UTF-32 encoded".
     */
    typedef enum StringTypes {
                          UnknownStringType                    = 0,
                          UTF8Format                           = 1,
                          UTF16Format                          = 2,
                          UTF32Format                          = 3,
    } StringTypes;
    
    /* Basic String Property Functions */
    /*!
     @abstract                             "Returns the number of CodeUnits in the CodePoint".
     @param               CodeUnit         "The leading codeunit for a codepoint".
     */
    uint8_t               UTF8_GetCodePointSizeInCodeUnits(UTF8 CodeUnit);
    
    /*!
     @abstract                             "Returns the number of CodeUnits in the CodePoint".
     @param               CodeUnit         "A codeunit for a codepoint".
     */
    uint8_t               UTF16_GetCodePointSizeInCodeUnits(UTF16 CodeUnit);
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8 string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of codeunits in".
     */
    uint64_t              UTF8_GetStringSizeInCodeUnits(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetStringSizeInCodeUnits(UTF16 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF8_GetStringSizeInCodePoints(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetStringSizeInCodePoints(UTF16 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF32_GetStringSizeInCodePoints(UTF32 *String);
    
    /*!
     @abstract                             "Gets the number of UTF-8 code units needed to store a UTF-32 string".
     @param               String           "The string to get the number of codeunits for".
     */
    uint64_t              UTF32_GetStringSizeInUTF8CodeUnits(UTF32 *String);
    
    /*!
     @abstract                             "Gets the number of UTF-16 code units needed to store a UTF-32 string".
     @param               String           "The string to get the number of codeunits for".
     */
    uint64_t              UTF32_GetStringSizeInUTF16CodeUnits(UTF32 *String);
    
    /*!
     @abstract                             "Gets the number of user visible characters in a UTF-8 string".
     @remark                               "This function just decodes the string and sends it off to the UTF32 version".
     @param               String           "The string to get the number of graphemes in".
     */
    uint64_t              UTF8_GetStringSizeInGraphemes(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of user visible characters in a UTF-16 string".
     @remark                               "This function just decodes the string and sends it off to the UTF32 version".
     @param               String           "The string to get the number of graphemes in".
     */
    uint64_t              UTF16_GetStringSizeInGraphemes(UTF16 *String);
    
    /*!
     @abstract                             "Gets the number of user visible characters in a UTF-32 string".
     @param               String           "The string to get the number of graphemes in".
     */
    uint64_t              UTF32_GetStringSizeInGraphemes(UTF32 *String);
    
    /*!
     @abstract                             "Tells if the UTF-8 string pointed to by String has a Byte Order Mark at the beginning".
     @param               String           "The string to get the BOM status from".
     @return                               "Returns Yes if the string contains a BOM, otherwise it returns No".
     */
    bool                  UTF8_StringHasBOM(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the UTF-16 string pointed to by String has a Byte Order Mark at the beginning".
     @param               String           "The string to get the BOM status from".
     @return                               "Returns Yes if the string contains a BOM, otherwise it returns No".
     */
    bool                  UTF16_StringHasBOM(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the UTF-32 string pointed to by String has a Byte Order Mark at the beginning".
     @param               String           "The string to get the BOM status from".
     @return                               "Returns Yes if the string contains a BOM, otherwise it returns No".
     */
    bool                  UTF32_StringHasBOM(UTF32 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String has "//?/" right after the BOM, if it exists".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string contains "//?/", otherwise it returns No".
     */
    bool                  UTF8_StringHasWinPathPrefix(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String has "//?/" right after the BOM, if it exists".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string contains "//?/", otherwise it returns No".
     */
    bool                  UTF16_StringHasWinPathPrefix(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String has "//?/" right after the BOM, if it exists".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string contains "//?/", otherwise it returns No".
     */
    bool                  UTF32_StringHasWinPathPrefix(UTF32 *String);
    
    /*!
     @abstract                             "Counts the number of Format Specifiers in String".
     @param               String           "The string to check".
     @return                               "Returns the number of format specifiers found".
     */
    uint64_t              UTF8_NumFormatSpecifiers(UTF8 *String);
    
    /*!
     @abstract                             "Counts the number of Format Specifiers in String".
     @param               String           "The string to check".
     @return                               "Returns the number of format specifiers found".
     */
    uint64_t              UTF16_NumFormatSpecifiers(UTF16 *String);
    
    /*!
     @abstract                             "Counts the number of Format Specifiers in String".
     @param               String           "The string to check".
     @return                               "Returns the number of format specifiers found".
     */
    uint64_t              UTF32_NumFormatSpecifiers(UTF32 *String);
    
    /*!
     @abstract                             "Tells if the UTF-8 string pointed to by String is a valid UTF-8 encoded string".
     @param               String           "The string to get the validity status from".
     @return                               "Returns Yes if the string is valid, otherwise it returns No".
     */
    bool                  UTF8_IsStringValid(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the UTF-16 string pointed to by String is a valid UTF-16 encoded string".
     @param               String           "The string to get the validity status from".
     @return                               "Returns Yes if the string is valid, otherwise it returns No".
     */
    bool                  UTF16_IsStringValid(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the UTF-32 string pointed to by String is a valid UTF-32 encoded string".
     @param               String           "The string to get the validity status from".
     @return                               "Returns Yes if the string is valid, otherwise it returns No".
     */
    bool                  UTF32_IsStringValid(UTF32 *String);
    
    /*!
     @abstract                             "Adds the BOM to the UTF-8 string, UTF-8's only valid BOM is BE".
     */
    UTF8                 *UTF8_AddBOM(UTF8 *String);
    
    /*!
     @abstract                             "Adds the specified BOM to the string".
     @param               String           "The string to add the BOM to".
     */
    UTF16                *UTF16_AddBOM(UTF16 *String, StringIOByteOrders BOM2Add);
    
    /*!
     @abstract                             "Adds the specified BOM to the string".
     @param               String           "The string to add the BOM to".
     */
    UTF32                *UTF32_AddBOM(UTF32 *String, StringIOByteOrders BOM2Add);
    
    /*!
     @abstract                             "Removes the BOM from the string".
     @param               String           "The string to remove the BOM from".
     */
    UTF8                 *UTF8_RemoveBOM(UTF8 *String);
    
    /*!
     @abstract                             "Removes the BOM from the string".
     @param               String           "The string to remove the BOM from".
     */
    UTF16                *UTF16_RemoveBOM(UTF16 *String);
    
    /*!
     @abstract                             "Removes the BOM from the string".
     @param               String           "The string to remove the BOM from".
     */
    UTF32                *UTF32_RemoveBOM(UTF32 *String);
    /* Basic String Property Functions */
    
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
    UTF8                 *UTF8_Encode(UTF32 *String);
    
    /*!
     @abstract                             "Encodes a UTF32 string to a UTF16 string".
     @param               String           "The string to encode".
     */
    UTF16                *UTF16_Encode(UTF32 *String);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String           "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF8                 *UTF8_CaseFoldString(UTF8 *String);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String           "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF16                *UTF16_CaseFoldString(UTF16 *String);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String           "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF32                *UTF32_CaseFoldString(UTF32 *String);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining codepoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @remark                               "This function simply decodes the string, sends it to the UTF32 version, then reencodes it".
     @param               String           "The string to be normalized".
     @param               NormalizedForm   "The type of normalization to use on the String".
     */
    UTF8                 *UTF8_NormalizeString(UTF8 *String, StringIONormalizationForms NormalizedForm);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining codepoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @remark                               "This function simply decodes the string, sends it to the UTF32 version, then reencodes it".
     @param               String           "The string to be normalized".
     @param               NormalizedForm   "The type of normalization to use on the String".
     */
    UTF16                *UTF16_NormalizeString(UTF16 *String, StringIONormalizationForms NormalizedForm);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining codepoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @param               String           "The string to be normalized".
     @param               NormalizedForm   "The type of normalization to use on the String".
     */
    UTF32                *UTF32_NormalizeString(UTF32 *String, StringIONormalizationForms NormalizedForm);
    
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
     @abstract                             "Finds a substring within string, starting at codepoint Offset, and ending at Offset + Length".
     @remark                               "We do NOT casefold, or normalize the String or SubString, that's your job".
     @param               String           "The string to search for SubString in".
     @param               SubString        "The SubString to find in String".
     @param               Offset           "Where in the string should we start looking for the substring"?
     @param               Length           "How many codepoints should we search for the substring? -1 means all codepoints".
     @return                               "Returns the offset of the start of the substring in String, or -1 if a match wasn't found.".
     */
    int64_t               UTF32_FindSubString(UTF32 *String, UTF32 *SubString, uint64_t Offset, int64_t Length);
    
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
     @abstract                             "Extracts a SubString from String".
     @param               String           "The string to extract from".
     @param               Offset           "The CodePoint to start extracting from".
     @param               Length           "The number of codepoints to extract".
     */
    UTF32                *UTF32_ExtractSubString(UTF32 *String, uint64_t Offset, uint64_t Length);
    
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
     @abstract                             "Replaces a section in String starting at Offset and ending at Offset + Length with Replacement".
     @param               String           "The string to edit".
     @param               Replacement      "The string to splice in".
     @param               Offset           "Where to start replacing String with Replacement".
     @param               Length           "The number of codepoints to replace, can be more or less than Replacement".
     */
    UTF32                *UTF32_ReplaceSubString(UTF32 *String, UTF32 *Replacement, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Reallocates String and copies it except for the instance (-1 for all instances) of the substring".
     @param               String           "The string to operate on".
     @param               SubString2Remove "The substring to remove from the string".
     @param               Instance2Remove  "The instance (0 for all) of the substring in the string to remove".
     */
    UTF8                 *UTF8_RemoveSubString(UTF8 *String, UTF8 *SubString2Remove, uint64_t Instance2Remove);
    
    /*!
     @abstract                             "Reallocates String and copies it except for the instance (-1 for all instances) of the substring".
     @param               String           "The string to operate on".
     @param               SubString2Remove "The substring to remove from the string".
     @param               Instance2Remove  "The instance (0 for all) of the substring in the string to remove".
     */
    UTF16                *UTF16_RemoveSubString(UTF16 *String, UTF16 *SubString2Remove, uint64_t Instance2Remove);
    
    /*!
     @abstract                             "Reallocates String and copies it except for the instance (-1 for all instances) of the substring".
     @param               String           "The string to operate on".
     @param               SubString2Remove "The substring to remove from the string".
     @param               Instance2Remove  "The instance (0 for all) of the substring in the string to remove".
     */
    UTF32                *UTF32_RemoveSubString(UTF32 *String, UTF32 *SubString2Remove, uint64_t Instance2Remove);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String           "The string you want to be split".
     @param               Delimiters       "An StringArray containing the delimiters, one delimiter per string".
     */
    UTF8                **UTF8_SplitString(UTF8 *String, UTF8 **Delimiters);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String           "The string you want to be split".
     @param               Delimiters       "An StringArray containing the delimiters, one delimiter per string".
     */
    UTF16               **UTF16_SplitString(UTF16 *String, UTF16 **Delimiters);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String           "The string you want to be split".
     @param               Delimiters       "An StringArray containing the delimiters, one delimiter per string".
     */
    UTF32               **UTF32_SplitString(UTF32 *String, UTF32 **Delimiters);
    
    /*!
     @abstract                             "Converts a string to an integer; replaces atoi, atol, strtol, strtoul".
     @param               Base             "The base to output the integer in".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF8_String2Integer(StringIOBases Base, UTF8 *String);
    
    /*!
     @abstract                             "Converts a string to an integer; replaces atoi, atol, strtol, strtoul".
     @param               Base             "The base to output the integer in".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF16_String2Integer(StringIOBases Base, UTF16 *String);
    
    /*!
     @abstract                             "Converts a string to an integer; replaces atoi, atol, strtol, strtoul".
     @param               Base             "The base to output the integer in".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF32_String2Integer(StringIOBases Base, UTF32 *String);
    
    /*!
     @abstract                             "Converts an integer to a string; replaces itoa".
     @param               Base             "The base to output the integer in".
     @param               Integer2Convert  "The number to convert into a string".
     */
    UTF8                 *UTF8_Integer2String(StringIOBases Base, int64_t Integer2Convert);
    
    /*!
     @abstract                             "Converts an integer to a string; replaces itoa".
     @param               Base             "The base to output the integer in".
     @param               Integer2Convert  "The number to convert into a string".
     */
    UTF16                *UTF16_Integer2String(StringIOBases Base, int64_t Integer2Convert);
    
    /*!
     @abstract                             "Converts an integer to a string; replaces itoa".
     @param               Base             "The base to output the integer in".
     @param               Integer2Convert  "The number to convert into a string".
     */
    UTF32                *UTF32_Integer2String(StringIOBases Base, int64_t Integer2Convert);
    
    /*!
     @abstract                             "Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                UTF8_String2Decimal(UTF8 *String);
    
    /*!
     @abstract                             "Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                UTF16_String2Decimal(UTF16 *String);
    
    /*!
     @abstract                             "Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                UTF32_String2Decimal(UTF32 *String);
    
    /*!
     @abstract                             "Converts a double to a string; replaces dtostr".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF8                 *UTF8_Decimal2String(const StringIOBases Base, double Decimal);
    
    /*!
     @abstract                             "Converts a double to a string; replaces ftoa".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF16                *UTF16_Decimal2String(const StringIOBases Base, double Decimal);
    
    /*!
     @abstract                             "Converts a double to a string; replaces ftoa".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF32                *UTF32_Decimal2String(const StringIOBases Base, double Decimal);
    
    /*!
     @enum                      TrimStringTypes
     @abstract                                                  "Is this BitInput or BitOutput connected to a File or Socket?".
     @constant                  TrimString_Unknown              "Unknown TrimString command".
     @constant                  TrimString_StartEndRemoveAll    "Trim at the beginning and end, removing all occurrences found there".
     @constant                  TrimString_BetweenValidKeep1    "Trim between start and end, removing all but 1 occurrence found between non-removable codepoints".
     @constant                  TrimString_All                  "Remove all occurrences regardless of context".
     */
    typedef enum TrimStringTypes {
                                TrimString_Unknown              = 0,
                                TrimString_StartEndRemoveAll    = 1,
                                TrimString_BetweenValidKeep1    = 2,
                                TrimString_RemoveAll            = 3,
    } TrimStringTypes;
    
    /*!
     @abstract                             "Removes substrings (including single codepoints) from a string".
     @param               String           "The string to perform the trimming operations on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF8                 *UTF8_TrimString(UTF8 *String, TrimStringTypes Type, UTF8 **Strings2Remove);
    
    /*!
     @abstract                             "Removes substrings (including single codepoints) from a string".
     @param               String           "The string to perform the trimming operations on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF16                *UTF16_TrimString(UTF16 *String, TrimStringTypes Type, UTF16 **Strings2Remove);
    
    /*!
     @abstract                             "Removes substrings (including single codepoints) from a string".
     @param               String           "The string to perform the trimming operations on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF32                *UTF32_TrimString(UTF32 *String, TrimStringTypes Type, UTF32 **Strings2Remove);
    
    /*!
     @abstract                             "Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "The string to check".
     @param               Substring        "The substring to check".
     @return                               "Returns whether Substring matches at the given offsets".
     */
    bool                  UTF8_CompareSubstring(UTF8 *String, UTF8 *Substring, uint64_t StringOffset, uint64_t SubstringOffset);
    
    /*!
     @abstract                             "Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "The string to check".
     @param               Substring        "The substring to check".
     @return                               "Returns whether Substring matches at the given offsets".
     */
    bool                  UTF16_CompareSubstring(UTF16 *String, UTF16 *Substring, uint64_t StringOffset, uint64_t SubstringOffset);
    
    /*!
     @abstract                             "Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "The string to check".
     @param               Substring        "The substring to check".
     @return                               "Returns whether Substring matches at the given offsets".
     */
    bool                  UTF32_CompareSubstring(UTF32 *String, UTF32 *Substring, uint64_t StringOffset, uint64_t SubstringOffset);
    
    /*!
     @abstract                             "Copies the String".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "Pointer to the String to be copied".
     @return                               "Returns A pointer to a copy of the String".
     */
    UTF8                 *UTF8_Clone(UTF8 *String);
    
    /*!
     @abstract                             "Copies the String".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "Pointer to the String to be copied".
     @return                               "Returns A pointer to a copy of the String".
     */
    UTF16                *UTF16_Clone(UTF16 *String);
    
    /*!
     @abstract                             "Copies the String".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "Pointer to the String to be copied".
     @return                               "Returns A pointer to a copy of the String".
     */
    UTF32                *UTF32_Clone(UTF32 *String);
    
    /*!
     @abstract                             "Creates a copy of String, with String2Insert starting at Offset".
     @remark                               "An offset of 0xFFFFFFFFFFFFFFFF means the end of the string".
     @param               String           "The string to manipulate".
     @param               String2Insert    "The string to be inserted into the String".
     @param               Offset           "In codepoints, not code units".
     @return                               "Returns a pointer to a new string containing the original, and String2Insert at Offset".
     */
    UTF8                 *UTF8_Insert(UTF8 *String, UTF8 *String2Insert, uint64_t Offset);
    
    /*!
     @abstract                             "Creates a copy of String, with String2Insert starting at Offset".
     @remark                               "An offset of 0xFFFFFFFFFFFFFFFF means the end of the string".
     @param               String           "The string to manipulate".
     @param               String2Insert    "The string to be inserted into the String".
     @param               Offset           "In codepoints, not code units".
     @return                               "Returns a pointer to a new string containing the original, and String2Insert at Offset".
     */
    UTF16                *UTF16_Insert(UTF16 *String, UTF16 *String2Insert, uint64_t Offset);
    
    /*!
     @abstract                             "Creates a copy of String, with String2Insert starting at Offset".
     @remark                               "An offset of 0xFFFFFFFFFFFFFFFF means the end of the string".
     @param               String           "The string to manipulate".
     @param               String2Insert    "The string to be inserted into the String".
     @param               Offset           "In codepoints, not code units".
     @return                               "Returns a pointer to a new string containing the original, and String2Insert at Offset".
     */
    UTF32                *UTF32_Insert(UTF32 *String, UTF32 *String2Insert, uint64_t Offset);
    
    /*!
     @abstract                             "Formats a string according to the Format string, with all of it's options".
     @remark                               "Extensions: B = Binary, the n specifier is unsupported, but it is removed from the output".
     @param               Format           "A string with optional format specifiers, all variadic strings need to match Format's type".
     @return                               "Returns the formatted string encoded using the UTF-8 format".
     */
#if   defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    UTF8                 *UTF8_FormatString(UTF8 *Format, ...) __attribute__((__format__(__wprintf__, 1, 2)));
#elif defined(_MSC_VER)
#include <sal.h>
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF8                 *UTF8_FormatString(__format_string UTF8 *Format, ...);
#elif    (_MSC_VER >= 1500)
    UTF8                 *UTF8_FormatString(_Printf_format_string_ UTF8 *Format, ...);
#endif /* MSVC Version */
#endif /* Compiler */
    
    /*!
     @abstract                             "Formats a string according to the Format string, with all of it's options".
     @remark                               "Extensions: B = Binary, the n specifier is unsupported, but it is removed from the output".
     @param               Format           "A string with optional format specifiers, all variadic strings need to match Format's type".
     @return                               "Returns the formatted string encoded using the UTF-16 format".
     */
#if   defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    UTF16                *UTF16_FormatString(UTF16 *Format, ...) __attribute__((__format__(__wprintf__, 1, 2)));
#elif defined(_MSC_VER)
#include <sal.h>
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF16                *UTF16_FormatString(__format_string UTF16 *Format, ...);
#elif    (_MSC_VER >= 1500)
    UTF16                *UTF16_FormatString(_Printf_format_string_ UTF16 *Format, ...);
#endif /* MSVC Version */
#endif /* Compiler */
    
    /*!
     @abstract                             "Formats a string according to the Format string, with all of it's options".
     @remark                               "Extensions: B = Binary, the n specifier is unsupported, but it is removed from the output".
     @param               Format           "A string with optional format specifiers, all variadic strings need to match Format's type".
     @return                               "Returns the formatted string encoded using the UTF-32 format".
     */
#if   defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    UTF32                *UTF32_FormatString(UTF32 *Format, ...) __attribute__((__format__(__wprintf__, 1, 2)));
#elif defined(_MSC_VER)
#include <sal.h>
#if      (_MSC_VER >= 1400 && _MSC_VER < 1500)
    UTF32                *UTF32_FormatString(__format_string UTF32 *Format, ...);
#elif    (_MSC_VER >= 1500)
    UTF32                *UTF32_FormatString(_Printf_format_string_ UTF32 *Format, ...);
#endif /* MSVC Version */
#endif /* Compiler */
    
    /*!
     @abstract                             "Counts the number of format specifiers (percent prefixed tokens) in String".
     @param               String           "The string to count the number of format specifiers contained within".
     */
    uint64_t              UTF8_GetNumFormatSpecifiers(UTF8 *String);
    
    /*!
     @abstract                             "Counts the number of format specifiers (percent prefixed tokens) in String".
     @param               String           "The string to count the number of format specifiers contained within".
     */
    uint64_t              UTF16_GetNumFormatSpecifiers(UTF16 *String);
    
    /*!
     @abstract                             "Counts the number of format specifiers (percent prefixed tokens) in String".
     @param               String           "The string to count the number of format specifiers contained within".
     */
    uint64_t              UTF32_GetNumFormatSpecifiers(UTF32 *String);
    
    /*!
     @abstract                             "Writes a UTF-8 encoded string to the OutputFile using the platform's default Unicode encoding".
     @remark                               "On Windows, Strings are converted to UTF-16; on POSIX platforms (including Mac/iOS) strings are written as UTF-8".
     @param               String           "The string to write to OutputFile (including any newlines, etc)".
     @param               OutputFile       "a valid FILE pointer, or STDIN/STDOUT/STDERR".
     */
    void                  UTF8_WriteString(UTF8 *String, FILE *OutputFile);
    
    /*!
     @abstract                             "Writes a UTF-16 encoded string to the OutputFile using the platform's default Unicode encoding".
     @remark                               "On Windows, Strings are output as UTF-16; on POSIX platforms (including Mac/iOS) strings are written as UTF-8".
     @param               String           "The string to write to OutputFile (including any newlines, etc)".
     @param               OutputFile       "a valid FILE pointer, or STDIN/STDOUT/STDERR".
     */
    void                  UTF16_WriteString(UTF16 *String, FILE *OutputFile);
    
    /*!
     @abstract                             "Writes a UTF-16 encoded string to the OutputFile using the platform's default Unicode encoding".
     @remark                               "On Windows, Strings are output as UTF-16; on POSIX platforms (including Mac/iOS) strings are written as UTF-8".
     @param               String           "The string to write to OutputFile (including any newlines, etc)".
     @param               OutputFile       "a valid FILE pointer, or STDIN/STDOUT/STDERR".
     */
    void                  UTF32_WriteString(UTF32 *String, FILE *OutputFile);
    
    /* StringArrays */
    
    /*!
     @abstract                             "Gets the size of each string in the StringArray in code units".
     @param               StringArray      "The StringArray to get the size of each string".
     @return                               "Returns an array containing NumStrings elements, where each element contains the size".
     */
    uint64_t             *UTF8_StringArray_GetStringSizesInCodeUnits(UTF8 **StringArray);
    
    /*!
     @abstract                             "Gets the size of each string in the StringArray in code units".
     @param               StringArray      "The StringArray to get the size of each string".
     @return                               "Returns an array containing NumStrings elements, where each element contains the size".
     */
    uint64_t             *UTF16_StringArray_GetStringSizesInCodeUnits(UTF16 **StringArray);
    
    /*!
     @abstract                             "Gets the size of each string in the StringArray in code points".
     @param               StringArray      "The StringArray to get the size of each string".
     @return                               "Returns an array containing NumStrings elements, where each element contains the size".
     */
    uint64_t             *UTF8_StringArray_GetStringSizesInCodePoints(UTF8 **StringArray);
    
    /*!
     @abstract                             "Gets the size of each string in the StringArray in code points".
     @param               StringArray      "The StringArray to get the size of each string".
     @return                               "Returns an array containing NumStrings elements, where each element contains the size".
     */
    uint64_t             *UTF16_StringArray_GetStringSizesInCodePoints(UTF16 **StringArray);
    
    /*!
     @abstract                             "Gets the size of each string in the StringArray in code points".
     @param               StringArray      "The StringArray to get the size of each string".
     @return                               "Returns an array containing NumStrings elements, where each element contains the size".
     */
    uint64_t             *UTF32_StringArray_GetStringSizesInCodePoints(UTF32 **StringArray);
    
    /*!
     @abstract                             "Gets the number of strings in a StringArray".
     @param               StringArray      "The StringArray to get the number of strings in".
     @return                               "Returns the number of strings in StringArray".
     */
    uint64_t              UTF8_StringArray_GetNumStrings(UTF8 **StringArray);
    
    /*!
     @abstract                             "Gets the number of strings in a StringArray".
     @param               StringArray      "The StringArray to get the number of strings in".
     @return                               "Returns the number of strings in StringArray".
     */
    uint64_t              UTF16_StringArray_GetNumStrings(UTF16 **StringArray);
    
    /*!
     @abstract                             "Gets the number of strings in a StringArray".
     @param               StringArray      "The StringArray to get the number of strings in".
     @return                               "Returns the number of strings in StringArray".
     */
    uint64_t              UTF32_StringArray_GetNumStrings(UTF32 **StringArray);
    
    /*!
     @abstract                             "Initalizes a UTF-8 encoded StringArray".
     @param               NumStrings       "How many strings will this StringArray contain"?
     @return                               "Returns an empty StringArray with room for NumStrings pointers plus a null terminator".
     */
    UTF8                **UTF8_StringArray_Init(uint64_t NumStrings);
    
    /*!
     @abstract                             "Initalizes a UTF-16 encoded StringArray".
     @param               NumStrings       "How many strings will this StringArray contain"?
     @return                               "Returns an empty StringArray with room for NumStrings pointers plus a null terminator".
     */
    UTF16               **UTF16_StringArray_Init(uint64_t NumStrings);
    
    /*!
     @abstract                             "Initalizes a UTF-32 encoded StringArray".
     @param               NumStrings       "How many strings will this StringArray contain"?
     @return                               "Returns an empty StringArray with room for NumStrings pointers plus a null terminator".
     */
    UTF32               **UTF32_StringArray_Init(uint64_t NumStrings);
    
    /*!
     @abstract                             "Attaches a string to a StringArray at the specified position".
     @param               StringArray      "The StringArray to attach the String to".
     @param               String2Attach    "The String to attach".
     @param               Index            "Which position should String2Attach be in"?
     */
    void                  UTF8_StringArray_Attach(UTF8 **StringArray, UTF8 *String2Attach, uint64_t Index);
    
    /*!
     @abstract                             "Attaches a string to a StringArray at the specified position".
     @param               StringArray      "The StringArray to attach the String to".
     @param               String2Attach    "The String to attach".
     @param               Index            "Which position should String2Attach be in"?
     */
    void                  UTF16_StringArray_Attach(UTF16 **StringArray, UTF16 *String2Attach, uint64_t Index);
    
    /*!
     @abstract                             "Attaches a string to a StringArray at the specified position".
     @param               StringArray      "The StringArray to attach the String to".
     @param               String2Attach    "The String to attach".
     @param               Index            "Which position should String2Attach be in"?
     */
    void                  UTF32_StringArray_Attach(UTF32 **StringArray, UTF32 *String2Attach, uint64_t Index);
    
    /*!
     @abstract                             "Decodes a StringArray to a UTF32_StringArray".
     @param               StringArray      "The StringArray to decode".
     @return                               "Returns the decoded StringArray".
     */
    UTF32               **UTF8_StringArray_Decode(UTF8 **StringArray);
    
    /*!
     @abstract                             "Decodes a StringArray to a UTF32_StringArray".
     @param               StringArray      "The StringArray to decode".
     @return                               "Returns the decoded StringArray".
     */
    UTF32               **UTF16_StringArray_Decode(UTF16 **StringArray);
    
    /*!
     @abstract                             "Encodes a StringArray to a UTF8_StringArray".
     @param               StringArray      "The StringArray to encode".
     @return                               "Returns the encoded StringArray".
     */
    UTF8                **UTF8_StringArray_Encode(UTF32 **StringArray);
    
    /*!
     @abstract                             "Encodes a StringArray to a UTF16_StringArray".
     @param               StringArray      "The StringArray to encode".
     @return                               "Returns the encoded StringArray".
     */
    UTF16               **UTF16_StringArray_Encode(UTF32 **StringArray);
    
    /*!
     @abstract                             "Deinitializes a UTF-8 encoded StringArray (like is returned by SplitString)".
     @param               StringArray      "An StringArray to deinitalize, all strings will be freed".
     */
    void                  UTF8_StringArray_Deinit(UTF8 **StringArray);
    
    /*!
     @abstract                             "Deinitializes a UTF-16 encoded StringArray (like is returned by SplitString)".
     @param               StringArray      "An StringArray to deinitalize, all strings will be freed".
     */
    void                  UTF16_StringArray_Deinit(UTF16 **StringArray);
    
    /*!
     @abstract                             "Deinitializes a UTF-16 encoded StringArray (like is returned by SplitString)".
     @param               StringArray      "An StringArray to deinitalize, all strings will be freed".
     */
    void                  UTF32_StringArray_Deinit(UTF32 **StringArray);
    
#define GetStringSizeInCodeUnits(String)  _Generic((String), UTF8:UTF8_GetStringSizeInCodeUnits, UTF8*:UTF8_GetStringSizeInCodeUnits, UTF16:UTF16_GetStringSizeInCodeUnits, UTF16*:UTF16_GetStringSizeInCodeUnits)(String)
    
#define GetStringSizeInCodePoints(String) _Generic((String), UTF8:UTF8_GetStringSizeInCodePoints, UTF8*:UTF8_GetStringSizeInCodePoints, UTF16:UTF16_GetStringSizeInCodePoints, UTF16*:UTF16_GetStringSizeInCodePoints, UTF32:UTF32_GetStringSizeInCodePoints, UTF32*:UTF32_GetStringSizeInCodePoints)(String)
    
#define GetStringSizeInGraphemes(String)  _Generic((String), UTF8:UTF8_GetStringSizeInGraphemes, UTF8*:UTF8_GetStringSizeInGraphemes, UTF16:UTF16_GetStringSizeInGraphemes, UTF16*:UTF16_GetStringSizeInGraphemes, UTF32:UTF32_GetStringSizeInGraphemes, UTF32*:UTF32_GetStringSizeInGraphemes)(String)
    
#define StringHasBOM(String)              _Generic((String), UTF8:UTF8_StringHasBOM, UTF8*:UTF8_StringHasBOM, UTF16:UTF16_StringHasBOM, UTF16*:UTF16_StringHasBOM, UTF32:UTF32_StringHasBOM, UTF32*:UTF32_StringHasBOM)(String)
    
#define FormatString(String, ...)         _Generic((String), UTF8:UTF8_FormatString, UTF8*:UTF8_FormatString, UTF16:UTF16_FormatString, UTF16*:UTF16_FormatString, UTF32:UTF32_FormatString, UTF32*:UTF32_FormatString)(String, ...)
    
#define GetNumFormatSpecifiers(String)    _Generic((String), UTF8:UTF8_GetNumFormatSpecifiers, UTF8*:UTF8_GetNumFormatSpecifiers, UTF16:UTF16_GetNumFormatSpecifiers, UTF16*:UTF16_GetNumFormatSpecifiers, UTF32:UTF32_GetNumFormatSpecifiers, UTF32*:UTF32_GetNumFormatSpecifiers)(String)
    
#define WriteString(String)               _Generic((String), UTF8:UTF8_WriteString, UTF8*:UTF8_WriteString, UTF16:UTF16_WriteString, UTF16*:UTF16_WriteString, UTF32:UTF32_WriteString, UTF32*:UTF32_WriteString)(String)
    
#define Clone(String)                     _Generic((String), UTF8:UTF8_Clone, UTF8*:UTF8_Clone, UTF16:UTF16_Clone, UTF16*:UTF16_Clone, UTF32:UTF32_Clone, UTF32*:UTF32_Clone)(String)
    
#define Insert(String, Substring2Insert, Offset)                    _Generic((String), UTF8:UTF8_Insert, UTF8*:UTF8_Insert, UTF16:UTF16_Insert, UTF16*:UTF16_Insert, UTF32:UTF32_Insert, UTF32*:UTF32_Insert)(String, Substring2Insert, Offset)
    
#define Compare(String1, String2)         _Generic((String1), UTF8:UTF8_Compare, UTF8*:UTF8_Compare, UTF16:UTF16_Compare, UTF16*:UTF16_Compare, UTF32:UTF32_Compare, UTF32*:UTF32_Compare)(String1, String2)
    
#define Trim(String, Strings2Remove)      _Generic((String), UTF8:UTF8_TrimString, UTF8*:UTF8_TrimString, UTF16:UTF16_TrimString, UTF16*:UTF16_TrimString, UTF32:UTF32_TrimString, UTF32*:UTF32_TrimString)(String, Strings2Remove)
    
#define String2Decimal(String)            _Generic((String), UTF8:UTF8_String2Decimal, UTF8*:UTF8_String2Decimal, UTF16:UTF16_String2Decimal, UTF16*:UTF16_String2Decimal, UTF32:UTF32_String2Decimal, UTF32*:UTF32_String2Decimal)(String)
    
#define String2Integer(String)            _Generic((String), UTF8:UTF8_String2Integer, UTF8*:UTF8_String2Integer, UTF16:UTF16_String2Integer, UTF16*:UTF16_String2Integer, UTF32:UTF32_String2Integer, UTF32*:UTF32_String2Integer)(String)
    
#define SplitString(String, Delimiters)   _Generic((String), UTF8:UTF8_SplitString, UTF8*:UTF8_SplitString, UTF16:UTF16_SplitString, UTF16*:UTF16_SplitString, UTF32:UTF32_SplitString, UTF32*:UTF32_SplitString)(String, Delimiters)
    
#define Normalize(String)                 _Generic((String), UTF8:UTF8_NormalizeString, UTF8*:UTF8_NormalizeString, UTF16:UTF16_NormalizeString, UTF16*:UTF16_NormalizeString, UTF32:UTF32_NormalizeString, UTF32*:UTF32_NormalizeString)(String)
    
#define CaseFold(String)                  _Generic((String), UTF8:UTF8_CaseFold, UTF8*:UTF8_CaseFold, UTF16:UTF16_CaseFold, UTF16*:UTF16_CaseFold, UTF32:UTF32_CaseFold, UTF32*:UTF32_CaseFold)(String)
    
#ifdef __cplusplus
}
#endif

#endif  /* FoundationIO_StringIO_H */
