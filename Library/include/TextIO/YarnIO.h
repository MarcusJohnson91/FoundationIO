/*!
 @header:                  YarnIO.h
 @author:                  Marcus Johnson
 @copyright:               2024+
 @version:                 1.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains code for replacing sections of a string in place; Primarily used by FormatIO, ExtensibleIO, and ConfigurationIO.
 */

#pragma once

#ifndef FoundationIO_TextIO_YarnIO_H
#define FoundationIO_TextIO_YarnIO_H

#include "TextIOTypes.h"    /* Included for Text types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct Yarn8 Yarn8;
    
    typedef struct Yarn16 Yarn16;
    
    typedef struct Yarn32 Yarn32;
    
    /*!
     @abstract                                   Initailizes a Yarn, Not all fields have to be present at once.
          @param      Yarn                        The Yarn to compare.
          @param      String                   The String to compare.
          @return Returns true if the String matches the Yarn.
     */
    Yarn8 *Yarn8_Init(UTF8 *Original, size_t NumReplacements, TextIO_Slice *Slices, UTF8 **Replacements);
    
    /*!
     @abstract                                   Initailizes a Yarn, Not all fields have to be present at once.
          @param      Yarn                        The Yarn to compare.
          @param      String                   The String to compare.
          @return Returns true if the String matches the Yarn.
     */
    Yarn16 *Yarn16_Init(UTF16 *Original, size_t NumReplacements, TextIO_Slice *Slices, UTF16 **Replacements);
    
    /*!
     @abstract                                   Initailizes a Yarn, Not all fields have to be present at once.
          @param      Yarn                        The Yarn to compare.
          @param      String                   The String to compare.
          @return Returns true if the String matches the Yarn.
     */ 
    Yarn32 *Yarn32_Init(UTF32 *Original, size_t NumReplacements, TextIO_Slice *Slices, UTF32 **Replacements);
    
        /*!
     @abstract                                   Compares a Yarn to a string as if the replacements had been knitted together.
          @param      Yarn                        The Yarn to compare.
          @param      String                   The String to compare.
          @return Returns true if the String matches the Yarn.
     */
    bool Yarn8_Compare2String(Yarn8 *Yarn, UTF8 *String);
    
    /*!
     @abstract                                   Compares a Yarn to a string as if the replacements had been knitted together.
          @param      Yarn                        The Yarn to compare.
          @param      String                   The String to compare.
          @return Returns true if the String matches the Yarn.
     */
    bool Yarn16_Compare2String(Yarn16 *Yarn, UTF16 *String);
    
    /*!
     @abstract                                   Compares a Yarn to a string as if the replacements had been knitted together.
          @param      Yarn                        The Yarn to compare.
          @param      String                   The String to compare.
          @return Returns true if the String matches the Yarn. 
     */
    bool Yarn32_Compare2String(Yarn32 *Yarn, UTF32 *String);
    
    /*!
     @abstract        Creates a new string from the Yarn with the replacements applied.
          @param      Yarn                        The Yarn to knit into a new string.
          @return Returns a new string with all replacements applied.
     */
    UTF8 *Yarn8_Knit(Yarn8 *Yarn);
    
    /*!
     @abstract        Creates a new string from the Yarn with the replacements applied.
          @param      Yarn                        The Yarn to knit into a new string.
          @return Returns a new string with all replacements applied.
     */
    UTF16 *Yarn16_Knit(Yarn16 *Yarn);
    
    /*!
     @abstract        Creates a new string from the Yarn with the replacements applied.
          @param      Yarn                        The Yarn to knit into a new string.
          @return Returns a new string with all replacements applied.
     */
    UTF32 *Yarn32_Knit(Yarn32 *Yarn);
     
    /*!
     @abstract        Deallocates a Yarn.
     @param           Yarn to deallocate.
     */
    void   Yarn8_Deinit(Yarn8 *Yarn);
    
    /*!
     @abstract        Deallocates a Yarn.
     @param           Yarn to deallocate.
     */
    void   Yarn16_Deinit(Yarn16 *Yarn);
    
    /*!
     @abstract        Deallocates a Yarn.
     @param           Yarn to deallocate.
     */
    void   Yarn32_Deinit(Yarn32 *Yarn);
    

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_YarnIO_H */
