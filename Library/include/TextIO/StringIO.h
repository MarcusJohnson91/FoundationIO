/*!
 @header    StringIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2017 - 2017, Marcus Johnson
 @version   0.2.0
 @brief     This header contains types, functions, and tables for Unicode support, including UTF-8, UTF-16, and our internal format, UTF-32.
 */

#pragma warning(push, 0)
#include <stdbool.h> /* Included for the bool type */
#include <stdint.h>  /* Included for the u/intX_t types */
#pragma warning(pop)

#pragma once

#ifndef LIBBITIO_StringIO_H
#define LIBBITIO_StringIO_H

#ifdef  __cplusplus
extern  "C" {
#endif
    
    typedef       unsigned char*  UTF8String;     // MUST be NULL terminated, and just tweak the code until you get it to while loop while not 0
    typedef const unsigned char   UTF8Constant[]; // MUST be NULL terminated
    typedef       unsigned short* UTF16String;    // MUST be NULL terminated
    typedef       unsigned long*  UTF32String;    // MUST be NULL terminated
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the UTF8String".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF8String_GetNumCodePoints(UTF8String String);
    
    /*!
     @abstract                             "Gets the number of Unicode codepoints in the UTF16String".
     @param               String           "The string to get the number of codepoints in".
     */
    uint64_t              UTF16String_GetNumCodePoints(UTF16String String);
    
    /*!
     @abstract                             "Decodes a UTF8String to a UTF32String".
     @param               String           "The string to decode".
     @param               StringSize       "The size of the string in codepoints".
     */
    UTF32String           UTF8String_Decode(UTF8String String, uint64_t StringSize);
    
    /*!
     @abstract                             "Decodes a UTF16String to a UTF32String".
     @param               String           "The string to decode".
     @param               StringSize       "The size of the string in codepoints".
     */
    UTF32String           UTF16String_Decode(UTF16String String, uint64_t StringSize);
    
    /*!
     @abstract                             "Encodes a UTF32String to a UTF8String".
     @param               String           "The UTF32String to encode".
     @param               StringSize       "The size of the UTF8String in codepoints".
     */
    UTF8String            UTF8String_Encode(UTF32String String, uint64_t StringSize);
    
    /*!
     @abstract                             "Encodes a UTF32String to a UTF16String".
     @param               String           "The string to decode".
     @param               StringSize       "The size of the string in codepoints".
     */
    UTF16String           UTF16String_Encode(UTF32String String, uint64_t StringSize);
    
    /*!
     @abstract                             "Converts string to use precomposed forms when possible, otherwise it orders the combining codepoints in lexiographic order".
     @param               String2Normalize "The sring to be normalized".
     @param               StringSize       "The size of String2Normalize in CodePoints".
     */
    UTF32String           UTF32StringNormalize(UTF32String String2Normalize, uint64_t StringSize);
    
    /*!
     @abstract                             "Casefolds string for case insensitive comparison".
     @param               String2CaseFold  "The sring to be casefolded".
     @param               StringSize       "The size of String2CaseFold in CodePoints".
     */
    UTF32String           UTF32String_CaseFold(UTF32String String2CaseFold, uint64_t StringSize);
    
    /*!
     @abstract                             "Compares String1 and String2 for equilivence".
     */
    bool                  UTF32Strings_Compare(UTF32String String1, uint64_t String1Size, UTF32String String2, uint64_t String2Size);
    
    /*!
     @abstract                             "Extracts a substring from String starting at position Start and ending at position End".
     */
    UTF32String           UTF32String_Extract(UTF32String String, uint64_t StringSize, uint64_t Start, uint64_t End);
    
    /*!
     @abstract                             "Returns the position of the first non-substring codepoint".
     */
    uint64_t              UTF32String_FindSubstring(UTF32String String, uint64_t StringSize, UTF32String SubString, uint64_t SubStringSize);
    
    /*!
     @abstract                             "Extracts a number from a string".
     @param               String           "The sring to extract a number from".
     @param               StringSize       "The size of String in CodePoints".
     */
    int64_t               UTF32String_ToNumber(UTF32String String, uint64_t StringSize);
    
    /* High level fucntions */
    bool                  UTF8String_Compare(UTF8String String1, uint64_t String1Size, UTF8String String2, uint64_t String2Size, bool Normalize, bool CaseFold);
    
#ifdef  __cplusplus
}
#endif

#endif  /* LIBBITIO_StringIO_H */
