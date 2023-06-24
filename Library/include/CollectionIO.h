/*!
 @header:                  CollectionIO.h
 @author:                  Marcus Johnson
 @copyright:               2022+
 @version:                 1.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains code for array operations.
 */

#include "../include/PlatformIO.h" /* Included fo platfom independence macros */

#pragma once

#ifndef FoundationIO_CollectionIO_H
#define FoundationIO_CollectionIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    #ifndef CollectionIO_CreateTypedef
    #define CollectionIO_CreateTypedef(PlatformIO_TypeName)
        typedef PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName) PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName);
    #endif /* CollectionIO_CreateTypedef */

    #define CollectionIO_Distribution(PlatformIO_TypeName)                                                \
         typedef struct PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName) { \
            PlatformIO_TypeName2Type(PlatformIO_TypeName) NumElements;                                  \
            PlatformIO_TypeName2Type(PlatformIO_TypeName) *Frequencies;                                 \
        } PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName);

        /* Macro to make the function declaration:
        
        */

#define CollectionIO_Distribution_Init(PlatformIO_TypeName, NumberOfElements) \
    typedef PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName) PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName); \ // Typedef the newly created type created by this macro.
            PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName) PlatformIO_Concat(PlatformIO_Typename2Suffix(CollectionIO_Distribution_Init, PlatformIO_TypeName), (size_t NumElements)) { \
        AssertIO(NumElements > 0); \
        PlatformIO_Typename2Suffix(CollectionIO_Distribution, PlatformIO_TypeName) Distribution = {0}; \
        Distribution.Array = calloc(NumElements, sizeof(PlatformIO_TypeName2Type(PlatformIO_TypeName))); \
        AssertIO(Distribution.Frequencies != NULL); \
        Distribution.NumEntries = NumElements; \
        return Frequencies; \
    }

    /*!
     @enum         ArrayIO_SortTypes
     @abstract                                      Defines the type of sorting.
     @constant     SortType_Unspecified             Invalid SortType, exists solely to tell when it hasn't been set.
     @constant     SortType_Ascending               Index 0 contains the most common value.
     @constant     SortType_Descending              Index 0 contains the least common value.
     */
    typedef enum ArrayIO_SortTypes : uint8_t {
        SortType_Unspecified             = 0,
        SortType_Ascending               = 1,
        SortType_Descending              = 2,
    } ArrayIO_SortTypes;

    /*!
     @abstract               Measure the frequency of each symbol in the array
     @param Array2Measure    The array to measure
     @param ArrayNumElements The number of elements in the array
     @param ArrayType        The actual type of the array
     */
    void ArrayIOFequncies_Measure(auto Frequencies, auto *Array2Measure, size_t ArrayNumElements);

    void ArrayIOFrequencies_Sort(auto Frequencies, ArrayIO_SortTypes SortType);

    void ArrayIOFequencies_Deinit(auto Frequencies);

    void ArrayIO_Rotate(auto *Array, size_t Amount2Rotate);

    void ArrayIO_Rotate_Ranged(auto *Array, size_t Amount2Rotate, PlatformIO_Range Range);

    int64_t ArrayIO_GetMin(auto *Array, size_t NumElements);

    int64_t ArrayIO_GetMax(auto *Array, size_t NumElements);

    int64_t ArrayIO_GetSum(auto *Array, PlatformIOTypes Type, size_t NumElements);
    
    auto CollectionIO_SumS8(int8_t *Array2Sum, size_t NumElements);
    
    uint64_t CollectionIO_SumU8(uint8_t *Array2Sum, size_t NumElements);
    
    int64_t CollectionIO_SumS16(int16_t *Array2Sum, size_t NumElements);
    
    uint64_t CollectionIO_SumU16(uint16_t *Array2Sum, size_t NumElements);
    
    int64_t CollectionIO_SumS32(int32_t *Array2Sum, size_t NumElements);
    
    uint64_t CollectionIO_SumU32(uint32_t *Array2Sum, size_t NumElements);
    
    int64_t CollectionIO_SumS64(int64_t *Array2Sum, size_t NumElements);
    
    uint64_t CollectionIO_SumU64(uint64_t *Array2Sum, size_t NumElements);
    
#ifndef CollectionIO_Sun
#define CollectionIO_Sum(Array2Sum, NumElements) \
_Generic(Array2Sum, int8_t:CollectionIO_SumS8, uint8_t:CollectionIO_SumU8, int16_t:CollectionIO_SumS16, uint16_t:CollectionIO_SunU16, int32_t:CollectionIO_SumS32, uint32_t:CollectionIO_SumU32, int64_t:CollectionIO_SumS64, uint64_t:CollectionIO_SumU64)(Array2Sum, NumElements)
#endif /* CollectionIO_Sum */

    void ArrayIO_Reverse(auto *Array, PlatformIOTypes Type, size_t NumElements);

    /*!
     @abstract                                    Sums an array from whatever position the start pointer is to that + NumElements2Sum.
     @param       Array2Sum                       Where to start the sumation.
     @param       NumElements2Sum                 How many elements to sum.
     @return                                      Returns the sum of the array.
     @function    ArrayIO_Sum8
     @function    ArrayIO_Sum16
     @function    ArrayIO_Sum32
     @function    ArrayIO_Sum64
     */
    size_t        ArrayIO_Sum8(uint8_t *Array2Sum, size_t NumElements2Sum);
    size_t        ArrayIO_Sum16(uint16_t *Array2Sum, size_t NumElements2Sum);
    size_t        ArrayIO_Sum32(uint32_t *Array2Sum, size_t NumElements2Sum);
    size_t        ArrayIO_Sum64(uint64_t *Array2Sum, size_t NumElements2Sum);

    /* Slice stuff */
    typedef struct CollectionIO_Slice {
        size_t StartElement;
        size_t StopElement;
    } CollectionIO_Slice;

    /*!
     @abstract                                  Creates an instance of a ArrayIO_Slice.
     @param       StartElement                  Where should the string start?
     @param       StopElement                   Where should the string end?
     @return                                    Returns the initailized Slice.
     */
    CollectionIO_Slice Slice_Init(size_t StartElement, size_t StopElement);

    /*!
     @abstract                                  Where does this slice start?
     @param       Slice                         The instance of the ArrayIO_Slice.
     @return                                    The Start of the ArrayIO_Slice in Elements.
     */
    size_t Slice_GetStartElement(CollectionIO_Slice Slice);

    /*!
     @abstract                                      Where does this slice end?
     @param       Slice                             The instance of the ArrayIO_Slice.
     @return                                        The End of the ArrayIO_Slice in Elements.
     */
    size_t Slice_GetStopElement(CollectionIO_Slice Slice);
    /* Slice stuff */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_CollectionIO_H */
