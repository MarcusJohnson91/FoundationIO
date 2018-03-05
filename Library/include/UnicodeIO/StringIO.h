#include <stdint.h>                   /* Included for u/intX_t */

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
     @version   0.8.0
     @brief     This header contains types, functions, and tables for Unicode support, including UTF-8, UTF-16, and our internal format, UTF-32.
     */
    
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
    
    typedef enum UnicodeTypes {
                                UnicodeUnknownSizeByteOrder        = 0x0000,
                                UTF16LE                            = 0xFFFE,
                                UTF16BE                            = 0xFEFF,
                                UTF32LE                            = 0xFFFE,
                                UTF32BE                            = 0xFEFF,
                                UTF16HighSurrogateStart            = 0xD800,
                                UTF16HighSurrogateEnd              = 0xDBFF,
                                UTF16LowSurrogateStart             = 0xDC00,
                                UTF16LowSurrogateEnd               = 0xDFFF,
    } UnicodeTypes;
    
    typedef enum UnicodeCodePointTypes {
                                UnknownCodePointType               = 0,
                                CaselessCodePoint                  = 1,
                                UpperCaseCodePoint                 = 2,
                                LowerCaseCodePoint                 = 3,
                                NumericIntegerCodePoint            = 4,
                                NumericDecimalCodePoint            = 5,
                                CombiningCodePoint                 = 6,
    } UnicodeCodePointTypes;
    
    typedef enum Number2StringBases {
                                Binary                             = 2,
                                Octal                              = 8,
                                Decimal                            = 10,
                                Hex                                = 16,
    } Number2StringBases;
    
    typedef enum FormatStringTypes {
                                UnknownFormatStringType            = 0,
                                SignedInteger8                     = 1,
                                SignedInteger16                    = 2,
                                SignedInteger32                    = 3,
                                SignedInteger64                    = 4,
                                UnsignedInteger8                   = 5,
                                UnsignedInteger16                  = 6,
                                UnsignedInteger32                  = 7,
                                UnsignedInteger64                  = 8,
                                Float16                            = 9,
                                Float32                            = 10,
                                Float64                            = 11,
                                Float128                           = 12,
                                String8                            = 13,
                                String16                           = 14,
                                String32                           = 15,
    } FormatStringTypes;
    
    typedef enum UnicodeBOMState {
                                UnknownBOMState                    = 0,
                                UnconditionallyAddBOM              = 1,
                                UnconditionallyRemoveBOM           = 2,
                                KeepTheBOMStateTheSame             = 3,
    } UTF8BOMState;
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8 string".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF8_GetSizeInCodeUnits(UTF8 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF8_GetSizeInCodePoints(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetSizeInCodeUnits(UTF16 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetSizeInCodePoints(UTF16 *String);
    
    uint64_t              UTF32_GetSizeInCodePoints(UTF32 *String);
    
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
    UTF16                *UTF16_Encode(UTF32 *String);
    
    /*!
     @abstract                             "Byteswaps a UTF16".
     @param               String2Convert   "The string to byteswap".
     @param               Type             "The byte order you want the string in".
     */
    UTF16                *UTF16_ConvertByteOrder(UnicodeTypes Type, UTF16 *String2Convert);
    
    /*!
     @abstract                             "Byteswaps a UTF32".
     @param               String2Convert   "The string to byteswap".
     @param               Type             "The byte order you want the string in".
     */
    UTF32                *UTF32_ConvertByteOrder(UnicodeTypes Type, UTF32 *String2Convert);
    
    /*!
     @abstract                             "Converts string to use precomposed forms when possible, otherwise it orders the combining codepoints in lexiographic order".
     @remark                               "The string is reallocated at the end to remove unused space".
     @param               String2Normalize "The sring to be normalized".
     */
    void                  UTF32_NormalizeString(UTF32 *String2Normalize);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String2CaseFold  "The string to be casefolded".
     */
    void                  UTF32_CaseFoldString(UTF32 *String2CaseFold);
    
    /*!
     @abstract                             "Extracts a substring from String starting at position Start and ending at position End".
     */
    UTF32                *UTF32_ExtractSubstring(UTF32 *String, uint64_t Start, uint64_t End);
    
    /*!
     @abstract                             "Returns the position of the first codepoint in the substring".
     */
    uint64_t              UTF32_FindSubstring(UTF32 *String, UTF32 *SubString);
    
    /*!
     @abstract                             "Extracts a number from a string".
     @param               String           "The sring to extract a number from".
     */
    int64_t               UTF32_String2Integer(UTF32 *String);
    
    /* High level fucntions */
    bool                  UTF8_Compare(UTF8 *String1, UTF8 *String2, bool Normalize, bool CaseInsensitive);
    
    bool                  UTF16_Compare(UTF16 *String1, UTF16 *String2, bool Normalize, bool CaseInsensitive);
    
    /*!
     @abstract                             "Compares String1 and String2 for equilivence".
     */
    bool                  CompareStrings32(UTF32 *String1, UTF32 *String2);
    
    UTF32                *FormatString32(const uint64_t VarArgCount, UTF32 *Format, ...);
    
#define                   FormatString(Format, ...) FormatString32(CountVariadicArguments(__VA_ARGS__), Format, __VA_ARGS__)
    
#ifdef  __cplusplus
}
#endif

#endif  /* FoundationIO_StringIO_H */
