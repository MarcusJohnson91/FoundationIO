/*!
 @header    StringIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2017+
 @version   0.8.0
 @brief     This header contains types, functions, and tables for Unicode support, including UTF-8, UTF-16, and our internal format, UTF-32.
 */

#include <stdint.h>                   /* Included for u/intX_t */

#include "Macros.h"                   /* Included for bool and Yes/No macros */

#pragma once

#ifndef FoundationIO_StringIO_H
#define FoundationIO_StringIO_H

#ifdef  __cplusplus
extern  "C" {
#endif
    
    typedef uint_least8_t   UTF8;
    typedef uint_least16_t  UTF16;
    typedef uint_least32_t  UTF32;
    
#ifndef             U8
#define             U8(QuotedLiteral) u8##QuotedLiteral
#endif
    
#ifndef             U16
#define             U16(QuotedLiteral) u##QuotedLiteral
#endif
    
#ifndef             U32
#define             U32(QuotedLiteral) U##QuotedLiteral
#endif
    
    typedef enum StringIOCommon {
                                UTF8BOMSizeInCodeUnits               = 3,
                                UTF8BOMSizeInCodePoints              = 1,
                                UTF1632BOMSizeInCodePoints           = 1,
                                UnicodeNULLTerminatorSize            = 1,
                                UTF16LE                              = 0xFFFE,
                                UTF16BE                              = 0xFEFF,
                                UTF32LE                              = 0xFFFE,
                                UTF32BE                              = 0xFEFF,
                                UTF16HighSurrogateStart              = 0xD800,
                                UTF16HighSurrogateEnd                = 0xDBFF,
                                UTF16LowSurrogateStart               = 0xDC00,
                                UTF16LowSurrogateEnd                 = 0xDFFF,
                                InvalidCodePointReplacementCharacter = 0xFFFD,
                                UnicodeMaxCodePoint                  = 0x10FFFF,
                                UTF16MaxCodePoint                    = 0xFFFF,
                                UTF16SurrogatePairModDividend        = 0x400,
                                UTF16SurrogatePairStart              = 0x10000,
                                UnicodeNULLTerminator                = 0x0,
    } StringIOCommon;
    
    typedef enum UnicodeCodePointTypes {
                                UnknownCodePointType                 = 0,
                                CaselessCodePoint                    = 1,
                                UpperCaseCodePoint                   = 2,
                                LowerCaseCodePoint                   = 3,
                                NumericIntegerCodePoint              = 4,
                                NumericDecimalCodePoint              = 5,
                                CombiningCodePoint                   = 6,
    } UnicodeCodePointTypes;
    
    typedef enum Number2StringBases {
                                Binary                               = 2,
                                Octal                                = 8,
                                Decimal                              = 10,
                                Hex                                  = 16,
    } Number2StringBases;
    
    typedef enum FormatStringTypes {
                                UnknownFormatStringType              = 0,
                                SignedInteger8                       = 1,
                                SignedInteger16                      = 2,
                                SignedInteger32                      = 3,
                                SignedInteger64                      = 4,
                                UnsignedInteger8                     = 5,
                                UnsignedInteger16                    = 6,
                                UnsignedInteger32                    = 7,
                                UnsignedInteger64                    = 8,
                                Float16                              = 9,
                                Float32                              = 10,
                                Float64                              = 11,
                                Float128                             = 12,
                                String8                              = 13,
                                String16                             = 14,
                                String32                             = 15,
    } FormatStringTypes;
    
    typedef enum UnicodeBOMState {
                                UnknownBOMState                      = 0,
                                UnconditionallyAddBOM                = 1,
                                UnconditionallyRemoveBOM             = 2,
                                KeepTheBOMStateTheSame               = 3,
    } UTF8BOMState;
    
    typedef enum UnicodeByteOrder {
                                UseNativeByteOrder                   = 0,
                                UseLEByteOrder                       = 1,
                                UseBEByteOrder                       = 2,
    } UnicodeByteOrder;
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8 string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String2Count     "The string to get the number of codeunits in".
     */
    uint64_t              UTF8_GetSizeInCodeUnits(UTF8 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF8_GetSizeInCodePoints(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetSizeInCodeUnits(UTF16 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @remark                               "This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetSizeInCodePoints(UTF16 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF32_GetSizeInCodePoints(UTF32 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode code units in the string if it was encoded in UTF8".
     @param               String           "The string to get the number of code units from".
     */
    uint64_t              UTF32_GetSizeInUTF8CodeUnits(UTF32 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode code units in the string if it was encoded in UTF16".
     @param               String           "The string to get the number of code units from".
     */
    uint64_t              UTF32_GetSizeInUTF16CodeUnits(UTF32 *String);
    
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
    UTF16                *UTF16_Encode(UTF32 *String, UnicodeByteOrder OutputByteOrder);
    
    /*!
     @abstract                             "Byteswaps a UTF16".
     @param               String2Convert   "The string to byteswap".
     @param               OutputByteOrder  "The byte order you want the string in".
     */
    void                  UTF16_ConvertByteOrder(UTF16 *String2Convert, UnicodeByteOrder OutputByteOrder);
    
    /*!
     @abstract                             "Byteswaps a UTF32".
     @param               String2Convert   "The string to byteswap".
     @param               OutputByteOrder  "The byte order you want the string in".
     */
    void                  UTF32_ConvertByteOrder(UTF32 *String2Convert, UnicodeByteOrder OutputByteOrder);
    
    /*!
     @abstract                             "Converts string to use precomposed forms, otherwise it orders the combining codepoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @param               String2Normalize "The string to be normalized".
     */
    void                  UTF32_NormalizeString(UTF32 *String2Normalize);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String2CaseFold  "The string to be casefolded".
     */
    void                  UTF32_CaseFoldString(UTF32 *String2CaseFold);
    
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
    UTF32                *UTF32_ExtractSubString(UTF32 *String, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Replaces a section in String starting at Offset and ending at Offset + Length with Replacement".
     @param               String           "The string to edit".
     @param               Replacement      "The string to splice in".
     @param               Offset           "Where to start replacing String with Replacement".
     @param               Length           "The number of codepoints to replace, can be more or less than Replacement".
     */
    UTF32                *UTF32_ReplaceSubString(UTF32 *String, UTF32 *Replacement, uint64_t Offset, uint64_t Length);
    
    /*!
     @abstract                             "Extracts a number from a string".
     @param               String           "The string to extract a number from".
     */
    int64_t               UTF32_String2Integer(UTF32 *String);
    
    /*!
     @abstract                             "Converts an integer to a string".
     @param               UpperCase        "If the Base is Hex, should the output string be upper case"?
     @param               Base             "The base to output the integer in".
     @param               Integer2Convert  "The number to convert into a string".
     */
    UTF32                *UTF32_Integer2String(const Number2StringBases Base, const bool UpperCase, int64_t Integer2Convert);
    
    /*!
     @abstract                             "Converts a string to a double, replaces strtod and atof".
     @param               String           "The string composed of a decimal number to convert to a decimal".
     */
    double                String2Decimal(UTF32 *String);
    
    /*!
     @abstract                             "Converts a double to a string, replaces strtod and atof".
     @param               Decimal          "The decimal number to convert to a string".
     */
    UTF32                *Decimal2String(double Decimal);
    
    /* High level functions */
    bool                  UTF8_Compare(UTF8 *String1, UTF8 *String2, bool Normalize, bool CaseInsensitive);
    
    bool                  UTF16_Compare(UTF16 *String1, UTF16 *String2, bool Normalize, bool CaseInsensitive);
    
    /*!
     @abstract                             "Compares String1 and String2 for equilivence".
     */
    bool                  UTF32_Compare(UTF32 *String1, UTF32 *String2);
    
    UTF32                *UTF32_FormatString(const uint64_t VarArgCount, UTF32 *Format, ...);
    
#define                   FormatString(Format, ...) UTF32_FormatString(CountVariadicArguments(__VA_ARGS__), Format, __VA_ARGS__)
    
#ifdef  __cplusplus
}
#endif

#endif  /* FoundationIO_StringIO_H */
