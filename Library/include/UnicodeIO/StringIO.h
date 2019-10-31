/*!
 @header              StringIO.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for Unicode support, including UTF-8, UTF-16, and our internal format, UTF-32.
 */

#include "../../include/Macros.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_StringIO_H
#define FoundationIO_UnicodeIO_StringIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef   char8_t        UTF8;
#else
    typedef   unsigned char  UTF8;
#endif /* __CHAR8_TYPE__ */
#endif /* FoundationIO_StringType8 */
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
#if (defined __STDC_UTF_16__ && defined __CHAR16_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef                     char16_t                        UTF16;
#else
    typedef                     uint_least16_t                  UTF16;
#endif /* __CHAR16_TYPE__ */
#endif /* FoundationIO_StringType16 */
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char32_t                             UTF32;
#else
    typedef               uint_least32_t                       UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
#ifndef                   FoundationIO_Unicodize8
#define                   FoundationIO_Unicodize8              (1)
#define                   UTF8String(Literal)                  (const UTF8*) u8##Literal
#define                   UTF8Character(Literal)               (const UTF8)  u8##Literal
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                   FoundationIO_Unicodize16
#define                   FoundationIO_Unicodize16            (2)
#define                   UTF16String(Literal)                (UTF16*) u##Literal
#define                   UTF16Character(Literal)             (UTF16)  u##Literal
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32            (4)
#define                   UTF32String(Literal)                (UTF32*) U##Literal
#define                   UTF32Character(Literal)             (UTF32) U##Literal
#endif /* FoundationIO_Unicodize32 */
    
    /*!
     @enum                StringIOCommon
     @constant            UTF8BOMSizeInCodeUnits                "The number of code units (8 bits) the UTF8 BOM takes".
     @constant            UTF16BOMSizeInCodeUnits               "The number of code units (16 bits) the UTF16 BOM takes".
     @constant            UnicodeBOMSizeInCodePoints            "The number of CodePoints in a BOM".
     @constant            UTF16BOM_LE                           "UTF16BOM_LE byte order mark".
     @constant            UTF16BOM_BE                           "UTF16BOM_BE byte order mark".
     @constant            UTF32BOM_LE                           "UTF32BOM_LE byte order mark".
     @constant            UTF32BOM_BE                           "UTF32BOM_LE byte order mark".
     @constant            UnicodeUNCPathPrefixSize              "Size of "//?/" in CodePoints and CodeUnits"
     @constant            UTF16HighSurrogateStart               "The value that marks the start of the High Surrogate range".
     @constant            UTF16HighSurrogateEnd                 "The value that marks the end   of the High Surrogate range".
     @constant            UTF16LowSurrogateStart                "The value that marks the start of the Low  Surrogate range".
     @constant            UTF16LowSurrogateEnd                  "The value that marks the end   of the Low  Surrogate range".
     @constant            UTF16MaxCodeUnitValue                 "The highest value that can be stored in a single UTF16 CodeUnit".
     @constant            UTF16SurrogatePairModDividend         "The value to modulo the surrogate pair by to decode a High Surrogate".
     @constant            UTF16SurrogatePairStart               "The first UTF-32 CodePoint to require Surrogate Pairs in UTF-16".
     @constant            InvalidReplacementCodePoint           "The CodePoint to replace invalid codeunits".
     @constant            UnicodeMaxCodePoint                   "The highest CodePoint possible in Unicode, 1,114,111".
     @constant            UTF8MaxCodeUnits                      "The maximum number of codeunits per codepoint".
     @constant            UTF16MaxCodeUnits                     "The maximum number of codeunits per codepoint".
     */
    typedef enum StringIOCommon {
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
                          UnicodeUNCPathPrefixSize              = 4, // "//?/" or "\\?\"
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
                          UTF8MaxCodeUnits                      = 4,
                          UTF16MaxCodeUnits                     = 2,
    } StringIOCommon;
    
    /*!
     @enum                StringIONormalizationForms
     @constant            NormalizationForm_Unknown             "Unknown Normalization Form".
     @constant            NormalizationForm_CanonicalDecompose  "Decompose the String".
     @constant            NormalizationForm_CanonicalCompose    "Decompose, then Compose the String".
     @constant            NormalizationForm_KompatibleDecompose "NormalizationForm_CanonicalDecompose, plus Kompatibility decompositions".
     @constant            NormalizationForm_KompatibleCompose   "NormalizationForm_CanonicalDecompose, plus Kompatibility compositions".
     */
    typedef enum StringIONormalizationForms {
                          NormalizationForm_Unknown             = 0,
                          NormalizationForm_CanonicalDecompose  = 1,
                          NormalizationForm_CanonicalCompose    = 2,
                          NormalizationForm_KompatibleDecompose = 3,
                          NormalizationForm_KompatibleCompose   = 4,
    } StringIONormalizationForms;
    
    /*!
     @enum                StringIOBases
     @constant            Base_Unknown                          "Unknown base".
     @constant            Base_Integer_Radix2                   "Integer, Binary".
     @constant            Base_Integer_Radix8                   "Integer, Octal".
     @constant            Base_Integer_Radix10                  "Integer, Base10".
     @constant            Base_Integer_Radix16_Uppercase        "Integer, Base16, Uppercase".
     @constant            Base_Integer_Radix16_Lowercase        "Integer, Base16, Lowercase".
     @constant            Base_Decimal_Radix10                  "Decimal, Base10, example: 3.14".
     @constant            Base_Decimal_Scientific_Uppercase     "Decimal, Decimal+Exponent, Uppercase".
     @constant            Base_Decimal_Scientific_Lowercase     "Decimal, Decimal+Exponent, Lowercase".
     @constant            Base_Decimal_Shortest_Uppercase       "Decimal, Base10 or Scientific, Uppercase".
     @constant            Base_Decimal_Shortest_Lowercase       "Decimal, Base10 or Scientific, Lowercase".
     @constant            Base_Decimal_Hex_Uppercase            "Decimal, 1.3DEp42, Uppercase".
     @constant            Base_Decimal_Hex_Lowercase            "Decimal, 1.3DEp42, Lowercase".
     */
    typedef enum StringIOBases {
                          Base_Unknown                          = 0,
                          Base_Integer_Radix2                   = 1,
                          Base_Integer_Radix8                   = 2,
                          Base_Integer_Radix10                  = 3,
                          Base_Integer_Radix16_Uppercase        = 4,
                          Base_Integer_Radix16_Lowercase        = 5,
                          Base_Decimal_Radix10                  = 6,
                          Base_Decimal_Scientific_Uppercase     = 7,
                          Base_Decimal_Scientific_Lowercase     = 8,
                          Base_Decimal_Shortest_Uppercase       = 9,
                          Base_Decimal_Shortest_Lowercase       = 10,
                          Base_Decimal_Hex_Uppercase            = 11,
                          Base_Decimal_Hex_Lowercase            = 12,
    } StringIOBases;
    
    /*!
     @enum                StringIOByteOrders
     @constant            ByteOrder_Unknown                     "Byte order is unknown".
     @constant            ByteOrder_Native                      "Use the byte order of the host".
     @constant            ByteOrder_Little                      "Use the little endian, Least-Significant-Byte first order".
     @constant            ByteOrder_Big                         "Use the big endian, Most-Significant-Byte first order".
     */
    typedef enum StringIOByteOrders {
                          ByteOrder_Unknown                     = 0,
                          ByteOrder_Native                      = 1,
                          ByteOrder_Little                      = 2,
                          ByteOrder_Big                         = 3,
    } StringIOByteOrders;
    
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
    
    /*!
     @enum                TrimStringTypes
     @abstract                                                  "Is this BitInput or BitOutput connected to a File or Socket?".
     @constant            TrimString_Unknown                    "Unknown TrimString command".
     @constant            TrimString_StartEndRemoveAll          "Trim at the beginning and end, removing all occurrences found there".
     @constant            TrimString_BetweenValidKeep1          "Trim between start and end, removing all but 1 occurrence found between non-removable CodePoints".
     @constant            TrimString_All                        "Remove all occurrences regardless of context".
     */
    typedef enum TrimStringTypes {
                          TrimString_Unknown                    = 0,
                          TrimString_StartEndRemoveAll          = 1,
                          TrimString_BetweenValidKeep1          = 2,
                          TrimString_RemoveAll                  = 3,
    } TrimStringTypes;
    
    /*!
     @abstract                             "Allocates a UTF8 string plus a NULL terminator".
     @param               NumCodeUnits     "The size of the string not counting the NULL terminator".
     */
    UTF8                 *UTF8_Init(uint64_t NumCodeUnits);
    
    /*!
     @abstract                             "Allocates a UTF8 string plus a NULL terminator".
     @param               NumCodeUnits     "The size of the string not counting the NULL terminator".
     */
    UTF16                *UTF16_Init(uint64_t NumCodeUnits);
    
    /*!
     @abstract                             "Allocates a UTF8 string plus a NULL terminator".
     @param               NumCodePoints    "The size of the string not counting the NULL terminator".
     */
    UTF32                *UTF32_Init(uint64_t NumCodePoints);
    
    /* Basic String Property Functions */
    /*!
     @abstract                             "Returns the number of CodeUnits in the CodePoint".
     @param               CodeUnit         "The leading CodeUnit for a CodePoint".
     */
    uint8_t               UTF8_GetCodePointSizeInCodeUnits(UTF8 CodeUnit);
    
    /*!
     @abstract                             "Returns the number of CodeUnits in the CodePoint".
     @param               CodeUnit         "A CodeUnit for a CodePoint".
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
     @param               String           "The string to get the number of CodePoints in".
     */
    uint64_t              UTF16_GetStringSizeInCodeUnits(UTF16 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode CodePoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of CodePoints in".
     */
    uint64_t              UTF8_GetStringSizeInCodePoints(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode CodePoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of CodePoints in".
     */
    uint64_t              UTF16_GetStringSizeInCodePoints(UTF16 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode CodePoints in the string".
     @param               String           "The string to get the number of CodePoints in".
     */
    uint64_t              UTF32_GetStringSizeInCodePoints(UTF32 *String);
    
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
     @abstract                             "Tells if the string pointed to by String has "//?/" right after the BOM, if it exists".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string contains "//?/", otherwise it returns No".
     */
    bool                  UTF8_IsUNCPath(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String has "//?/" right after the BOM, if it exists".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string contains "//?/", otherwise it returns No".
     */
    bool                  UTF16_IsUNCPath(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String has "//?/" right after the BOM, if it exists".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string contains "//?/", otherwise it returns No".
     */
    bool                  UTF32_IsUNCPath(UTF32 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String starts with / on POSIX, or the second character is : on Windows".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string starts with "/" or the second character is ":" on Windows, otherwise it returns No".
     */
    bool                  UTF8_IsAbsolutePath(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String starts with / on POSIX, or the second character is : on Windows".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string starts with "/" or the second character is ":" on Windows, otherwise it returns No".
     */
    bool                  UTF16_IsAbsolutePath(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String starts with / on POSIX, or the second character is : on Windows".
     @param               String           "The string to check".
     @return                               "Returns Yes if the string starts with "/" or the second character is ":" on Windows, otherwise it returns No".
     */
    bool                  UTF32_IsAbsolutePath(UTF32 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String contains a Windows or UNIX style line ending".
     @param               String           "The string to check".
     @return                               "Returns Yes if the String contains a Windows or UNIX style line ending, otherwise no".
     */
    bool                  UTF8_HasNewLine(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String contains a Windows or UNIX style line ending".
     @param               String           "The string to check".
     @return                               "Returns Yes if the String contains a Windows or UNIX style line ending, otherwise no".
     */
    bool                  UTF16_HasNewLine(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the string pointed to by String contains a Windows or UNIX style line ending".
     @param               String           "The string to check".
     @return                               "Returns Yes if the String contains a Windows or UNIX style line ending, otherwise no".
     */
    bool                  UTF32_HasNewLine(UTF32 *String);
    
    /*!
     @abstract                             "Tells if the UTF-8 string pointed to by String is a valid UTF-8 encoded string".
     @param               String           "The string to get the validity status from".
     @return                               "Returns Yes if the string is valid, otherwise it returns No".
     */
    bool                  UTF8_IsValid(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the UTF-16 string pointed to by String is a valid UTF-16 encoded string".
     @param               String           "The string to get the validity status from".
     @return                               "Returns Yes if the string is valid, otherwise it returns No".
     */
    bool                  UTF16_IsValid(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the UTF-32 string pointed to by String is a valid UTF-32 encoded string".
     @param               String           "The string to get the validity status from".
     @return                               "Returns Yes if the string is valid, otherwise it returns No".
     */
    bool                  UTF32_IsValid(UTF32 *String);
    
    /*!
     @abstract                             "Tells if the UTF-8 string pointed to by String has a Byte Order Mark at the beginning".
     @param               String           "The string to get the BOM status from".
     @return                               "Returns Yes if the string contains a BOM, otherwise it returns No".
     */
    bool                  UTF8_HasBOM(UTF8 *String);
    
    /*!
     @abstract                             "Tells if the UTF-16 string pointed to by String has a Byte Order Mark at the beginning".
     @param               String           "The string to get the BOM status from".
     @return                               "Returns Yes if the string contains a BOM, otherwise it returns No".
     */
    bool                  UTF16_HasBOM(UTF16 *String);
    
    /*!
     @abstract                             "Tells if the UTF-32 string pointed to by String has a Byte Order Mark at the beginning".
     @param               String           "The string to get the BOM status from".
     @return                               "Returns Yes if the string contains a BOM, otherwise it returns No".
     */
    bool                  UTF32_HasBOM(UTF32 *String);
    
    /*!
     @abstract                             "Adds the BOM to the UTF-8 string, UTF-8's only valid BOM is BE".
     @param               String           "The string to add the BOM to".
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
     @abstract                             "Converts a UTF8 string to a UTF16 string".
     @param               String           "The string to convert".
     */
    UTF16                *UTF8_Convert(UTF8 *String);
    
    /*!
     @abstract                             "Converts a UTF16 string to a UTF8 string".
     @param               String           "The string to convert".
     */
    UTF8                 *UTF16_Convert(UTF16 *String);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String           "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF8                 *UTF8_CaseFold(UTF8 *String);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String           "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF16                *UTF16_CaseFold(UTF16 *String);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String           "The string to be casefolded".
     @return                               "Returns the case folded string".
     */
    UTF32                *UTF32_CaseFold(UTF32 *String);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining CodePoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @remark                               "This function simply decodes the string, sends it to the UTF32 version, then reencodes it".
     @param               String           "The string to be normalized".
     @param               NormalizedForm   "The type of normalization to use on the String".
     */
    UTF8                 *UTF8_Normalize(UTF8 *String, StringIONormalizationForms NormalizedForm);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining CodePoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @remark                               "This function simply decodes the string, sends it to the UTF32 version, then reencodes it".
     @param               String           "The string to be normalized".
     @param               NormalizedForm   "The type of normalization to use on the String".
     */
    UTF16                *UTF16_Normalize(UTF16 *String, StringIONormalizationForms NormalizedForm);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining CodePoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @param               String           "The string to be normalized".
     @param               NormalizedForm   "The type of normalization to use on the String".
     */
    UTF32                *UTF32_Normalize(UTF32 *String, StringIONormalizationForms NormalizedForm);
    
    /*!
     @abstract                             "Extracts a Grapheme from String".
     @param               String           "The string to extract from".
     @param               Grapheme         "The Grapheme to start extracting from".
     */
    UTF8                 *UTF8_ExtractGrapheme(UTF8 *String, uint64_t Grapheme);
    
    /*!
     @abstract                             "Extracts a Grapheme from String".
     @param               String           "The string to extract from".
     @param               Grapheme         "The Grapheme to start extracting from".
     */
    UTF16                *UTF16_ExtractGrapheme(UTF16 *String, uint64_t Grapheme);
    
    /*!
     @abstract                             "Extracts a Grapheme from String".
     @param               String           "The string to extract from".
     @param               Grapheme         "The Grapheme to start extracting from".
     */
    UTF32                *UTF32_ExtractGrapheme(UTF32 *String, uint64_t Grapheme);
    
    /*!
     @abstract                             "Compares String1 to String2".
     @param               String1          "String1 for comparison".
     @param               String2          "String2 for comparison".
     @return                               "Returns true if the strings match exactly, otherwise false".
     */
    bool                  UTF8_Compare(UTF8 *String1, UTF8 *String2);
    
    /*!
     @abstract                             "Compares String1 to String2".
     @param               String1          "String1 for comparison".
     @param               String2          "String2 for comparison".
     @return                               "Returns true if the strings match exactly, otherwise false".
     */
    bool                  UTF16_Compare(UTF16 *String1, UTF16 *String2);
    
    /*!
     @abstract                             "Compares String1 to String2".
     @param               String1          "String1 for comparison".
     @param               String2          "String2 for comparison".
     @return                               "Returns true if the strings match exactly, otherwise false".
     */
    bool                  UTF32_Compare(UTF32 *String1, UTF32 *String2);
    
    /*!
     @abstract                             "Finds a substring within string, starting at CodePoint Offset, and ending at Offset + Length".
     @remark                               "We do NOT casefold, or normalize the String or SubString, that's your job".
     @param               String           "The string to search for SubString in".
     @param               SubString        "The SubString to find in String".
     @param               Offset           "Where in the string should we start looking for the substring"?
     @param               Length           "How many CodePoints should we search for the substring? -1 means all CodePoints".
     @return                               "Returns the offset of the start of the substring in String, or -1 if a match wasn't found.".
     */
    int64_t               UTF8_FindSubString(UTF8 *String, UTF8 *SubString, uint64_t Offset, int64_t Length);
    
    /*!
     @abstract                             "Finds a substring within string, starting at CodePoint Offset, and ending at Offset + Length".
     @remark                               "We do NOT casefold, or normalize the String or SubString, that's your job".
     @param               String           "The string to search for SubString in".
     @param               SubString        "The SubString to find in String".
     @param               Offset           "Where in the string should we start looking for the substring"?
     @param               Length           "How many CodePoints should we search for the substring? -1 means all CodePoints".
     @return                               "Returns the offset of the start of the substring in String, or -1 if a match wasn't found.".
     */
    int64_t               UTF16_FindSubString(UTF16 *String, UTF16 *SubString, uint64_t Offset, int64_t Length);
    
    /*!
     @abstract                             "Finds a substring within string, starting at CodePoint Offset, and ending at Offset + Length".
     @remark                               "We do NOT casefold, or normalize the String or SubString, that's your job".
     @param               String           "The string to search for SubString in".
     @param               SubString        "The SubString to find in String".
     @param               Offset           "Where in the string should we start looking for the substring"?
     @param               Length           "How many CodePoints should we search for the substring? -1 means all CodePoints".
     @return                               "Returns the offset of the start of the substring in String, or -1 if a match wasn't found.".
     */
    int64_t               UTF32_FindSubString(UTF32 *String, UTF32 *SubString, uint64_t Offset, int64_t Length);
    
    /*!
     @abstract                             "Extracts a SubString from String".
     @param               String           "The string to extract from".
     @param               Offset           "The CodePoint to start extracting from".
     @param               Length           "The number of CodePoints to extract".
     */
    UTF8                 *UTF8_ExtractSubString(UTF8 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Extracts a SubString from String".
     @param               String           "The string to extract from".
     @param               Offset           "The CodePoint to start extracting from".
     @param               Length           "The number of CodePoints to extract".
     */
    UTF16                *UTF16_ExtractSubString(UTF16 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Extracts a SubString from String".
     @param               String           "The string to extract from".
     @param               Offset           "The CodePoint to start extracting from".
     @param               Length           "The number of CodePoints to extract".
     */
    UTF32                *UTF32_ExtractSubString(UTF32 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Replaces a section in String starting at Offset and ending at Offset + Length with Replacement".
     @param               String           "The string to edit".
     @param               Replacement      "The string to splice in".
     @param               Offset           "Where to start replacing String with Replacement".
     @param               Length           "The number of CodePoints to replace, can be more or less than Replacement".
     */
    UTF8                 *UTF8_ReplaceSubString(UTF8 *String, UTF8 *Replacement, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Replaces a section in String starting at Offset and ending at Offset + Length with Replacement".
     @param               String           "The string to edit".
     @param               Replacement      "The string to splice in".
     @param               Offset           "Where to start replacing String with Replacement".
     @param               Length           "The number of CodePoints to replace, can be more or less than Replacement".
     */
    UTF16                *UTF16_ReplaceSubString(UTF16 *String, UTF16 *Replacement, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Replaces a section in String starting at Offset and ending at Offset + Length with Replacement".
     @param               String           "The string to edit".
     @param               Replacement      "The string to splice in".
     @param               Offset           "Where to start replacing String with Replacement".
     @param               Length           "The number of CodePoints to replace, can be more or less than Replacement".
     */
    UTF32                *UTF32_ReplaceSubString(UTF32 *String, UTF32 *Replacement, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Reallocates String and copies it except for the CodePoints between Offset and Length (inclusive)".
     @param               String           "The string to operate on".
     @param               Offset           "The first CodePoint to remove".
     @param               Length           "The last CodePoint to remove minus Offset".
     */
    UTF8                 *UTF8_StitchSubString(UTF8 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Reallocates String and copies it except for the CodePoints between Offset and Length (inclusive)".
     @param               String           "The string to operate on".
     @param               Offset           "The first CodePoint to remove".
     @param               Length           "The last CodePoint to remove minus Offset".
     */
    UTF16                *UTF16_StitchSubString(UTF16 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Reallocates String and copies it except for the CodePoints between Offset and Length (inclusive)".
     @param               String           "The string to operate on".
     @param               Offset           "The first CodePoint to remove".
     @param               Length           "The last CodePoint to remove minus Offset".
     */
    UTF32                *UTF32_StitchSubString(UTF32 *String, uint64_t Offset, uint64_t Length);
    
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
     @abstract                             "Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "The string to check".
     @param               Substring        "The substring to check".
     @return                               "Returns whether Substring matches at the given offsets".
     */
    bool                  UTF8_CompareSubString(UTF8 *String, UTF8 *Substring, uint64_t StringOffset, uint64_t SubstringOffset);
    
    /*!
     @abstract                             "Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "The string to check".
     @param               Substring        "The substring to check".
     @return                               "Returns whether Substring matches at the given offsets".
     */
    bool                  UTF16_CompareSubString(UTF16 *String, UTF16 *Substring, uint64_t StringOffset, uint64_t SubstringOffset);
    
    /*!
     @abstract                             "Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence".
     @remark                               "The caller needs to handle casefolding and normalization".
     @param               String           "The string to check".
     @param               Substring        "The substring to check".
     @return                               "Returns whether Substring matches at the given offsets".
     */
    bool                  UTF32_CompareSubString(UTF32 *String, UTF32 *Substring, uint64_t StringOffset, uint64_t SubstringOffset);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String           "The string you want to be split".
     @param               Delimiters       "An StringArray containing the delimiters, one delimiter per string".
     */
    UTF8                **UTF8_Split(UTF8 *String, UTF8 **Delimiters);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String           "The string you want to be split".
     @param               Delimiters       "An StringArray containing the delimiters, one delimiter per string".
     */
    UTF16               **UTF16_Split(UTF16 *String, UTF16 **Delimiters);
    
    /*!
     @abstract                             "Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process".
     @remark                               "Replaces strtok from the standard library".
     @param               String           "The string you want to be split".
     @param               Delimiters       "An StringArray containing the delimiters, one delimiter per string".
     */
    UTF32               **UTF32_Split(UTF32 *String, UTF32 **Delimiters);
    
    /*!
     @abstract                             "Converts a string to an integer; replaces atoi, atol, strtol, strtoul".
     @remark                               "If the number can be negatiev, check for a minus before extracting the digits as a substring".
     @param               Base             "The base to output the integer in".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF8_String2Integer(StringIOBases Base, UTF8 *String);
    
    /*!
     @abstract                             "Converts a string to an integer; replaces atoi, atol, strtol, strtoul".
     @remark                               "If the number can be negatiev, check for a minus before extracting the digits as a substring".
     @param               Base             "The base to output the integer in".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF16_String2Integer(StringIOBases Base, UTF16 *String);
    
    /*!
     @abstract                             "Converts a string to an integer; replaces atoi, atol, strtol, strtoul".
     @remark                               "If the number can be negatiev, check for a minus before extracting the digits as a substring".
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
    double                UTF8_String2Decimal(StringIOBases Base, UTF8 *String);
    
    /*!
     @abstract                             "Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                UTF16_String2Decimal(StringIOBases Base, UTF16 *String);
    
    /*!
     @abstract                             "Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                UTF32_String2Decimal(StringIOBases Base, UTF32 *String);
    
    /*!
     @abstract                             "Converts a double to a string; replaces dtostr".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF8                 *UTF8_Decimal2String(StringIOBases Base, double Decimal);
    
    /*!
     @abstract                             "Converts a double to a string; replaces ftoa".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF16                *UTF16_Decimal2String(StringIOBases Base, double Decimal);
    
    /*!
     @abstract                             "Converts a double to a string; replaces ftoa".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF32                *UTF32_Decimal2String(StringIOBases Base, double Decimal);
    
    /*!
     @abstract                             "Removes substrings (including single CodePoints) from a string".
     @param               String           "The string to perform the trimming operations on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF8                 *UTF8_Trim(UTF8 *String, TrimStringTypes Type, UTF8 **Strings2Remove);
    
    /*!
     @abstract                             "Removes substrings (including single CodePoints) from a string".
     @param               String           "The string to perform the trimming operations on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF16                *UTF16_Trim(UTF16 *String, TrimStringTypes Type, UTF16 **Strings2Remove);
    
    /*!
     @abstract                             "Removes substrings (including single CodePoints) from a string".
     @param               String           "The string to perform the trimming operations on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF32                *UTF32_Trim(UTF32 *String, TrimStringTypes Type, UTF32 **Strings2Remove);
    
    /*!
     @abstract                             "Strips all instances of Strings2Remove from String".
     @param               String           "The string to perform the strip operation on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF8                 *UTF8_Strip(UTF8 *String, UTF8 **Strings2Remove);
    
    /*!
     @abstract                             "Strips all instances of Strings2Remove from String".
     @param               String           "The string to perform the strip operation on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF16                *UTF16_Strip(UTF16 *String, UTF16 **Strings2Remove);
    
    /*!
     @abstract                             "Strips all instances of Strings2Remove from String".
     @param               String           "The string to perform the strip operation on".
     @param               Strings2Remove   "An StringArray to remove from the String".
     */
    UTF32                *UTF32_Strip(UTF32 *String, UTF32 **Strings2Remove);
    
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
     @abstract                             "Securely erases a string".
     @param               String           "Pointer to the String to be erased".
     */
    void                  UTF8_Erase(UTF8 *String);
    
    /*!
     @abstract                             "Securely erases a string".
     @param               String           "Pointer to the String to be erased".
     */
    void                  UTF16_Erase(UTF16 *String);
    
    /*!
     @abstract                             "Securely erases a string".
     @param               String           "Pointer to the String to be erased".
     */
    void                  UTF32_Erase(UTF32 *String);
    
    /*!
     @abstract                             "Cuts a string down to MaxCodeUnits CodeUnits".
     @remark                               "MaxCodeUnits DOES NOT include the NULL Terminator".
     @param               String           "The string to truncate".
     @param               NumGraphemes     "The maxiumum amount of CodeUnits in the string, not counting the null terminator".
     @return                               "Returns the truncated, null terminated copy of String".
     */
    UTF8                 *UTF8_Truncate(UTF8 *String, uint64_t NumGraphemes);
    
    /*!
     @abstract                             "Cuts a string down to MaxCodeUnits CodeUnits".
     @remark                               "MaxCodeUnits DOES NOT include the NULL Terminator".
     @param               String           "The string to truncate".
     @param               NumGraphemes     "The maxiumum amount of CodeUnits in the string, not counting the null terminator".
     @return                               "Returns the truncated, null terminated copy of String".
     */
    UTF16                *UTF16_Truncate(UTF16 *String, uint64_t NumGraphemes);
    
    /*!
     @abstract                             "Cuts a string down to MaxCodePoints CodePoints".
     @remark                               "MaxCodePoints DOES NOT include the NULL Terminator".
     @param               String           "The string to truncate".
     @param               NumGraphemes     "The maxiumum amount of CodePoints in the string, not counting the null terminator".
     @return                               "Returns the truncated, null terminated copy of String".
     */
    UTF32                *UTF32_Truncate(UTF32 *String, uint64_t NumGraphemes);
    
    /*!
     @abstract                             "Creates a copy of String, with String2Insert starting at Offset".
     @remark                               "An offset of 0xFFFFFFFFFFFFFFFF means the end of the string".
     @param               String           "The string to manipulate".
     @param               String2Insert    "The string to be inserted into the String".
     @param               Offset           "In CodePoints, not code units".
     @return                               "Returns a pointer to a new string containing the original, and String2Insert at Offset".
     */
    UTF8                 *UTF8_Insert(UTF8 *String, UTF8 *String2Insert, uint64_t Offset);
    
    /*!
     @abstract                             "Creates a copy of String, with String2Insert starting at Offset".
     @remark                               "An offset of 0xFFFFFFFFFFFFFFFF means the end of the string".
     @param               String           "The string to manipulate".
     @param               String2Insert    "The string to be inserted into the String".
     @param               Offset           "In CodePoints, not code units".
     @return                               "Returns a pointer to a new string containing the original, and String2Insert at Offset".
     */
    UTF16                *UTF16_Insert(UTF16 *String, UTF16 *String2Insert, uint64_t Offset);
    
    /*!
     @abstract                             "Creates a copy of String, with String2Insert starting at Offset".
     @remark                               "An offset of 0xFFFFFFFFFFFFFFFF means the end of the string".
     @param               String           "The string to manipulate".
     @param               String2Insert    "The string to be inserted into the String".
     @param               Offset           "In CodePoints, not code units".
     @return                               "Returns a pointer to a new string containing the original, and String2Insert at Offset".
     */
    UTF32                *UTF32_Insert(UTF32 *String, UTF32 *String2Insert, uint64_t Offset);
    
    /*!
     @abstract                             "Reverses a string Grapheme by Grapheme".
     @remark                               "It is your responsibility to free the returned string when you're done using it".
     @param               String           "A string to reverse".
     */
    UTF8                 *UTF8_Reverse(UTF8 *String);
    
    /*!
     @abstract                             "Reverses a string Grapheme by Grapheme".
     @remark                               "It is your responsibility to free the returned string when you're done using it".
     @param               String           "A string to reverse".
     */
    UTF16                *UTF16_Reverse(UTF16 *String);
    
    /*!
     @abstract                             "Reverses a string Grapheme by Grapheme".
     @remark                               "It is your responsibility to free the returned string when you're done using it".
     @param               String           "A string to reverse".
     */
    UTF32                *UTF32_Reverse(UTF32 *String);
    
    /*!
     @abstract                             "Reads a Grapheme from Source".
     @remark                               "Replaces Fgetc and getc".
     @param               Source           "The file to read from".
     @return                               "Returns the UTF-8 encoded CodePoint which will be between 1-4 CodeUnits".
     */
    UTF8                 *UTF8_ReadGraphemeFromFile(FILE *Source);
    
    /*!
     @abstract                             "Reads a Grapheme from Source".
     @remark                               "Replaces Fgetwc and getwc".
     @param               Source           "The file to read from".
     @return                               "Returns the UTF-16 encoded CodePoint which will be between 1-2 CodeUnits".
     */
    UTF16                *UTF16_ReadGraphemeFromFile(FILE *Source);
    
    /*!
     @abstract                             "Writes a CodePoint to Source".
     @remark                               "Replaces Fputc and putc".
     @param               Source           "The file to write to".
     @param               CodePoint        "An array of CodeUnits containing one CodePoint".
     */
    void                  UTF8_WriteGrapheme(FILE *Source, UTF8 *CodePoint);
    
    /*!
     @abstract                             "Writes a CodePoint to Source".
     @remark                               "Replaces Fputwc and putwc".
     @param               Source           "The file to write to".
     @param               CodePoint        "An array of CodeUnits containing one CodePoint".
     */
    void                  UTF16_WriteGrapheme(FILE *Source, UTF16 *CodePoint);
    
    /*!
     @abstract                             "Reads a Line (Including mewline) from Source".
     @remark                               "Replaces Fgets and gets".
     @param               Source           "The file to read from".
     @return                               "Returns the UTF-8 encoded line".
     */
    UTF8                 *UTF8_ReadLine(FILE *Source);
    
    /*!
     @abstract                             "Reads a Line (Including mewline) from Source".
     @remark                               "Replaces Fgetws and getws".
     @param               Source           "The file to read from".
     @return                               "Returns the UTF-16 encoded line".
     */
    UTF16                *UTF16_ReadLine(FILE *Source);
    
    /*!
     @abstract                             "Writes a Line (Including mewline) to Source".
     @remark                               "Replaces Fputs and puts".
     @param               OutputFile       "The file to write the string to".
     @param               String           "The String to write".
     */
    void                  UTF8_WriteLine(FILE *OutputFile, UTF8 *String);
    
    /*!
     @abstract                             "Writes a Line (Including mewline) to Source".
     @remark                               "Replaces Fputws and putws".
     @param               OutputFile       "The file to write the string to".
     @param               String           "The String to write".
     */
    void                  UTF16_WriteLine(FILE *OutputFile, UTF16 *String);
    
    /*!
     @abstract                             "Counts the number of Format Specifiers in String".
     @param               String           "The string to check".
     @return                               "Returns the number of format specifiers found".
     */
    uint64_t              UTF8_GetNumFormatSpecifiers(UTF8 *String);
    
    /*!
     @abstract                             "Counts the number of Format Specifiers in String".
     @param               String           "The string to check".
     @return                               "Returns the number of format specifiers found".
     */
    uint64_t              UTF16_GetNumFormatSpecifiers(UTF16 *String);
    
    /*!
     @abstract                             "Counts the number of Format Specifiers in String".
     @param               String           "The string to check".
     @return                               "Returns the number of format specifiers found".
     */
    uint64_t              UTF32_GetNumFormatSpecifiers(UTF32 *String);
    
    /*!
     @abstract                             "Counts the number of Digits in String starting at Offset (inclusive)".
     @param               Base             "The base the string is in".
     @param               String           "The string to check".
     @param               Offset           "Where to start looking for digits".
     @return                               "Returns the number of format specifiers found".
     */
    uint64_t              UTF32_GetNumDigits(StringIOBases Base, UTF32 *String, uint64_t Offset); // Format/Deformat
    
    /*!
     @abstract                             "Gets a substring from Offset to where Format and Formatted start matching".
     @param               Format           "The base the string is in".
     @param               Formatted        "The string to check".
     @param               Offset           "Where to start looking for digits".
     @return                               "Returns the SubString".
     */
    uint64_t              UTF32_GetSubStringLength(UTF32 *Format, UTF32 *Formatted, uint64_t Offset);
    
    /*!
     @abstract                             "Deinitializes String".
     @param               String           "The string to deinitialize".
     */
    void                  UTF8_Deinit(UTF8 *String);
    
    /*!
     @abstract                             "Deinitializes String".
     @param               String           "The string to deinitialize".
     */
    void                  UTF16_Deinit(UTF16 *String);
    
    /*!
     @abstract                             "Deinitializes String".
     @param               String           "The string to deinitialize".
     */
    void                  UTF32_Deinit(UTF32 *String);
    
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
    
    void                  UTF8_StringArray_Print(UTF8 **StringArray);
    
    void                  UTF16_StringArray_Print(UTF16 **StringArray);
    
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
    
#ifdef __cplusplus
}
#endif

#endif  /* FoundationIO_UnicodeIO_StringIO_H */
