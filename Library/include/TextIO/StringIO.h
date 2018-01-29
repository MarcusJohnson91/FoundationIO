#include <stdbool.h> /* Included for the bool type */
#include <stdint.h>  /* Included for the u/intX_t types */

#pragma once

#ifndef LIBBITIO_StringIO_H
#define LIBBITIO_StringIO_H

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
    
    typedef unsigned char   UTF8;
    typedef unsigned short  UTF16;
    typedef unsigned int    UTF32;
    
    typedef enum UnicodeTypes {
        UnicodeUnknownSizeByteOrder        = 0,
        UTF16LE                            = 1,
        UTF16BE                            = 2,
        UTF32LE                            = 3,
        UTF32BE                            = 4,
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
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF8_GetSizeInCodeUnits(UTF8 *String2Count);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF8_GetSizeInCodePoints(UTF8 *String);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the string".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF16_GetSizeInCodePoints(UTF16 *String);
    
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
    void                  UTF32_Normalize(UTF32 *String2Normalize);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String2CaseFold  "The sring to be casefolded".
     */
    void                  UTF32_CaseFold(UTF32 *String2CaseFold);
    
    /*!
     @abstract                             "Compares String1 and String2 for equilivence".
     */
    bool                  UTF32Strings_Compare(UTF32 *String1, UTF32 *String2);
    
    /*!
     @abstract                             "Extracts a substring from String starting at position Start and ending at position End".
     */
    UTF32                *UTF32_Extract(UTF32 *String, uint64_t Start, uint64_t End);
    
    /*!
     @abstract                             "Returns the position of the first codepoint in the substring".
     */
    uint64_t              UTF32_FindSubstring(UTF32 *String, UTF32 *SubString);
    
    /*!
     @abstract                             "Extracts a number from a string".
     @param               String           "The sring to extract a number from".
     */
    int64_t               UTF32_ToNumber(UTF32 *String);
    
    /* High level fucntions */
    bool                  UTF8_Compare(UTF8 *String1, UTF8 *String2, bool Normalize, bool CaseFold);
    
    bool                  UTF16_Compare(UTF16 *String1, UTF16 *String2, bool Normalize, bool CaseFold);
    
#ifdef  __cplusplus
}
#endif

#endif  /* LIBBITIO_StringIO_H */

