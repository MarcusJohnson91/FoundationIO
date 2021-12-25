/*!
 @header          StringSetIO.h
 @author          Marcus Johnson
 @copyright       2021+
 @version         1.0.0
 @brief           This header contains types, functions, and tables for interacting with StringSets.
 */

#include "TextIOTypes.h"    /* Included for Text types */

#pragma once

#ifndef FoundationIO_TextIO_StringSetIO_H
#define FoundationIO_TextIO_StringSetIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    /*!
     @abstract                                           Creates a UTF-8 encoded StringSet.
     @param            NumStrings                        How many strings will this StringSet contain?
     @return                                             Returns an empty StringSet with room for NumStrings pointers plus a null terminator.
     */
    UTF8             **UTF8_StringSet_Init(uint64_t NumStrings);

    /*!
     @abstract                                           Creates a UTF-16 encoded StringSet.
     @param            NumStrings                        How many strings will this StringSet contain?
     @return                                             Returns an empty StringSet with room for NumStrings pointers plus a null terminator.
     */
    UTF16            **UTF16_StringSet_Init(uint64_t NumStrings);

    /*!
     @abstract                                           Creates a UTF-32 encoded StringSet.
     @param            NumStrings                        How many strings will this StringSet contain?
     @return                                             Returns an empty StringSet with room for NumStrings pointers plus a null terminator.
     */
    UTF32            **UTF32_StringSet_Init(uint64_t NumStrings);

    /*!
     @abstract                                           Attaches a string to a StringSet at the specified position.
     @param            StringSet                         The StringSet to attach the String to.
     @param            String2Attach                     The String to attach.
     @param            Index                             Which position should String2Attach be in?
     @return                                             Returns true when attaching was sucessful.
     */
    bool               UTF8_StringSet_Attach(UTF8 **StringSet, UTF8 *String2Attach, uint64_t Index);

    /*!
     @abstract                                           Attaches a string to a StringSet at the specified position.
     @param            StringSet                         The StringSet to attach the String to.
     @param            String2Attach                     The String to attach.
     @param            Index                             Which position should String2Attach be in?
     @return                                             Returns true when attaching was sucessful.
     */
    bool               UTF16_StringSet_Attach(UTF16 **StringSet, UTF16 *String2Attach, uint64_t Index);

    /*!
     @abstract                                           Attaches a string to a StringSet at the specified position.
     @param            StringSet                         The StringSet to attach the String to.
     @param            String2Attach                     The String to attach.
     @param            Index                             Which position should String2Attach be in?
     @return                                             Returns true when attaching was sucessful.
     */
    bool               UTF32_StringSet_Attach(UTF32 **StringSet, UTF32 *String2Attach, uint64_t Index);

    /*!
     @abstract                                           Gets the number of strings in a StringSet.
     @param            StringSet                         The StringSet to get the number of strings in.
     @return                                             Returns the number of strings in StringSet.
     */
    uint64_t           UTF8_StringSet_GetNumStrings(ImmutableStringSet_UTF8 StringSet);

    /*!
     @abstract                                           Gets the number of strings in a StringSet.
     @param            StringSet                         The StringSet to get the number of strings in.
     @return                                             Returns the number of strings in StringSet.
     */
    uint64_t           UTF16_StringSet_GetNumStrings(ImmutableStringSet_UTF16 StringSet);

    /*!
     @abstract                                           Gets the number of strings in a StringSet.
     @param            StringSet                         The StringSet to get the number of strings in.
     @return                                             Returns the number of strings in StringSet.
     */
    uint64_t           UTF32_StringSet_GetNumStrings(ImmutableStringSet_UTF32 StringSet);

    /*!
     @abstract                                           Gets the size of each string in the StringSet in code units.
     @param            StringSet                         The StringSet to get the size of each string.
     @return                                             Returns an array containing NumStrings elements, where each element contains the size.
     */
    uint64_t          *UTF8_StringSet_GetStringSizesInCodeUnits(ImmutableStringSet_UTF8 StringSet);

    /*!
     @abstract                                           Gets the size of each string in the StringSet in code units.
     @param            StringSet                         The StringSet to get the size of each string.
     @return                                             Returns an array containing NumStrings elements, where each element contains the size.
     */
    uint64_t          *UTF16_StringSet_GetStringSizesInCodeUnits(ImmutableStringSet_UTF16 StringSet);

    /*!
     @abstract                                           Gets the size of each string in the StringSet in code points.
     @param            StringSet                         The StringSet to get the size of each string.
     @return                                             Returns an array containing NumStrings elements, where each element contains the size.
     */
    uint64_t          *UTF8_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF8 StringSet);

    /*!
     @abstract                                           Gets the size of each string in the StringSet in code points.
     @param            StringSet                         The StringSet to get the size of each string.
     @return                                             Returns an array containing NumStrings elements, where each element contains the size.
     */
    uint64_t          *UTF16_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF16 StringSet);

    /*!
     @abstract                                           Gets the size of each string in the StringSet in code points.
     @param            StringSet                         The StringSet to get the size of each string.
     @return                                             Returns an array containing NumStrings elements, where each element contains the size.
     */
    uint64_t          *UTF32_StringSet_GetStringSizesInCodePoints(ImmutableStringSet_UTF32 StringSet);

    /*!
     @abstract                                           Decodes a StringSet to a UTF32_StringSet.
     @param            StringSet                         The StringSet to decode.
     @return                                             Returns the decoded StringSet.
     */
    UTF32            **UTF8_StringSet_Decode(ImmutableStringSet_UTF8 StringSet);

    /*!
     @abstract                                           Decodes a StringSet to a UTF32_StringSet.
     @param            StringSet                         The StringSet to decode.
     @return                                             Returns the decoded StringSet.
     */
    UTF32            **UTF16_StringSet_Decode(ImmutableStringSet_UTF16 StringSet);

    /*!
     @abstract                                           Encodes a StringSet to a UTF8_StringSet.
     @param            StringSet                         The StringSet to encode.
     @return                                             Returns the encoded StringSet.
     */
    UTF8             **UTF8_StringSet_Encode(MutableStringSet_UTF32 StringSet);

    /*!
     @abstract                                           Encodes a StringSet to a UTF16_StringSet.
     @param            StringSet                         The StringSet to encode.
     @return                                             Returns the encoded StringSet.
     */
    UTF16            **UTF16_StringSet_Encode(MutableStringSet_UTF32 StringSet);

    /*!
     @abstract                                           Creates a String from a StringSet.
     @param            StringSet                         The StringSet to convert to a string.
     @return                                             Returns the encoded String.
     */
    UTF8              *UTF8_StringSet_Flatten(ImmutableStringSet_UTF8 StringSet);

    /*!
     @abstract                                           Creates a String from a StringSet.
     @param            StringSet                         The StringSet to convert to a string.
     @return                                             Returns the encoded String.
     */
    UTF16             *UTF16_StringSet_Flatten(ImmutableStringSet_UTF16 StringSet);

    /*!
     @abstract                                           Creates a String from a StringSet.
     @param            StringSet                         The StringSet to convert to a string.
     @return                                             Returns the encoded String.
     */
    UTF32             *UTF32_StringSet_Flatten(ImmutableStringSet_UTF32 StringSet);

    /*!
     @abstract                                           Deletes a UTF-8 encoded StringSet (like is returned by SplitString).
     @param            StringSet                         An StringSet to deinitialize, all strings will be freed.
     */
    void               UTF8_StringSet_Deinit(UTF8 **StringSet);

    /*!
     @abstract                                           Deletes a UTF-16 encoded StringSet (like is returned by SplitString).
     @param            StringSet                         An StringSet to deinitialize, all strings will be freed.
     */
    void               UTF16_StringSet_Deinit(UTF16 **StringSet);

    /*!
     @abstract                                           Deletes a UTF-16 encoded StringSet (like is returned by SplitString).
     @param            StringSet                         An StringSet to deinitialize, all strings will be freed.
     */
    void               UTF32_StringSet_Deinit(UTF32 **StringSet);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_StringSetIO_H */
