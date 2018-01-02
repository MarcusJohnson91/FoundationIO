#include "../include/BitIOMacros.h"

#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(push, 0)
#endif
#include <stdbool.h> /* Included for the bool type */
#include <stdint.h>  /* Included for the u/intX_t types */
#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(pop)
#endif

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
    
    typedef       unsigned char*  UTF8String;     // MUST be NULL terminated, and just tweak the code until you get it to while loop while not 0
    typedef const unsigned char   UTF8Constant[]; // MUST be NULL terminated
    typedef       unsigned short* UTF16String;    // MUST be NULL terminated
    typedef       unsigned long*  UTF32String;    // MUST be NULL terminated
    typedef       unsigned long   UTF32CodePoint;
    
    typedef enum UnicodeByteOrder {
                  UnicodeUnknownOrder = 0,
                  UnicodeLittleEndian = 1,
                  UnicodeBigEndian    = 2,
    } UnicodeByteOrder;
    
    /*!
     @abstract                             "Gets the number of Unicode codeunits in the UTF8String".
     @param               String2Count     "The string to get the number of codepoints in".
     */
    uint64_t              UTF8String_GetNumCodeUnits(UTF8String String2Count);
    
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
     */
    UTF32String           UTF8String_Decode(UTF8String String);
    
    /*!
     @abstract                             "Decodes a UTF16String to a UTF32String".
     @param               String           "The string to decode".
     */
    UTF32String           UTF16String_Decode(UTF16String String);
    
    /*!
     @abstract                             "Encodes a UTF32String to a UTF8String".
     @param               String           "The UTF32String to encode to a UTF8String".
     */
    UTF8String            UTF8String_Encode(UTF32String String);
    
    /*!
     @abstract                             "Encodes a UTF32String to a UTF16String".
     @param               String           "The string to decode".
     */
    UTF16String           UTF16String_Encode(UTF32String String);
    
    /*!
     @abstract                             "Byteswaps a UTF16String".
     @param               String2Convert   "The string to byteswap".
     @param               TargetByteOrder  "The byte order you want the string in".
     */
    UTF16String           UTF16String_ConvertByteOrder(UTF16String String2Convert, UnicodeByteOrder TargetByteOrder);
    
    /*!
     @abstract                             "Byteswaps a UTF32String".
     @param               String2Convert   "The string to byteswap".
     @param               TargetByteOrder  "The byte order you want the string in".
     */
    UTF32String           UTF32String_ConvertByteOrder(UTF32String String2Convert, UnicodeByteOrder TargetByteOrder);
    
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
    bool                  UTF32Strings_Compare(UTF32String String1, UTF32String String2);
    
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
    
    bool                  UTF16String_Compare(UTF16String String1, uint64_t String1Size, UTF16String String2, uint64_t String2Size, bool Normalize, bool CaseFold);
    
#ifdef  __cplusplus
}
#endif

#endif  /* LIBBITIO_StringIO_H */
