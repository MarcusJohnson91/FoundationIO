/*!
 @header:                  SliceIO.h
 @author:                  Marcus Johnson
 @copyright:               2024+
 @version:                 1.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains code for Slicing strings, which should probably be in ArrayIO eventually.
 */

#pragma once

#ifndef FoundationIO_TextIO_SliceIO_H
#define FoundationIO_TextIO_SliceIO_H

#include "TextIOTypes.h"    /* Included for Text types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct Slice {
        size_t Header;
        size_t Footer;
    } Slice;
    
    typedef struct SliceSet {
        size_t NumSlices;
        Slice *Slices;
    } SliceSet;

    /*!
     @abstract                                  Creates an instance of a Slice.
     @param       Header                  Where should the string start?
     @param       Footer                   Where should the string end?
     @return                                    Returns the initailized Slice.
     */
    Slice Slice_Init(size_t Header, size_t Footer);
    
    SliceSet SliceSet_Init(size_t NumSlices);
    
    size_t SliceSet_Sum(SliceSet Set);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_SliceIO_H */
