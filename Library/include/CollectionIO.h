/*!
 @unsorted
 @header          CollectionIO.h
 @author          Marcus Johnson
 @copyright       2022+
 @version         1.0.0
 @brief           This header contains code for array operations.
 */

#include "../include/PlatformIO.h" /* Included fo platfom independence macros */

#pragma once

#ifndef FoundationIO_CollectionIO_H
#define FoundationIO_CollectionIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define CollectionIO_Typename_Uint8  1
#define CollectionIO_Typename_Uint16 2
#define CollectionIO_Typename_Uint32 3
#define CollectionIO_Typename_Uint64 4
#define CollectionIO_Typename_Int8   5
#define CollectionIO_Typename_Int16  6
#define CollectionIO_Typename_Int32  7
#define CollectionIO_Typename_Int64  8

#ifndef CollectionIO_Typename_AddSuffix
#define CollectionIO_Typename_AddSuffix(Prefix, CollectionIO_Typename)
#if (CollectionIO_Typename == CollectionIO_Typename_Uint8)
    PlatformIO_Concat(Prefix, U8)
#elif (CollectionIO_Typename == CollectionIO_Typename_Uint16)
    PlatformIO_Concat(Prefix, U16)
#elif (CollectionIO_Typename == CollectionIO_Typename_Uint32)
    PlatformIO_Concat(Prefix, U32)
#elif (CollectionIO_Typename == CollectionIO_Typename_Uint64)
    PlatformIO_Concat(Prefix, U64)
#elif (CollectionIO_Typename == CollectionIO_Typename_Int8)
    PlatformIO_Concat(Prefix, S8)
#elif (CollectionIO_Typename == CollectionIO_Typename_Int16)
    PlatformIO_Concat(Prefix, S16)
#elif (CollectionIO_Typename == CollectionIO_Typename_Int32)
    PlatformIO_Concat(Prefix, S32)
#elif (CollectionIO_Typename == CollectionIO_Typename_Int64)
    PlatformIO_Concat(Prefix, S64)
#endif

    #define CollectionIO_Distribution(TYPE)                \
         typedef struct CollectionIO_Distribution_##TYPE { \
            TYPE  NumElements;                             \
            TYPE *Frequencies;                             \
        } CollectionIO_Distribution_##TYPE##;

#define CollectionIO_Distribution_Init(CollectionIO_TypeName, NumberOfElements)                                   \
    typedef CollectionIO_Typename_AddSuffix(CollectionIO_Distribution, CollectionIO_TypeName) CollectionIO_Typename_AddSuffix(CollectionIO_Distribution, CollectionIO_TypeName);
        CollectionIO_Typename_AddSuffix(CollectionIO_Distribution_Init_, CollectionIO_TypeName)##(size_t NumElements) {
        AssertIO(NumElements > 0);
        ArrayIO_Frequencies *Frequencies = calloc(1, sizeof(ArrayIO_Frequencies));
        AssertIO(Frequencies != NULL);
        Frequencies->Array = calloc(NumElements, sizeof(TYPE));
        AssertIO(Frequencies->Array != NULL);
        Frequencies->NumEntries = NumElements;
        return Frequencies;
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

    typedef struct ArrayIO_Frequencies {
        void           *Array;
        size_t          NumEntries;
        PlatformIOTypes Type;
    } ArrayIO_Frequencies;

    CollectionIO_Distribution_uint8_t What = CollectionIO_Distribution_Init(uint8_t, 67);

    /*!
     @abstract               Measure the frequency of each symbol in the array
     @param Array2Measure    The array to measure
     @param ArrayNumElements The number of elements in the array
     @param ArrayType        The actual type of the array
     */
    void ArrayIOFequncies_Measure(ArrayIO_Frequencies *Frequencies, void *Array2Measure, size_t ArrayNumElements, PlatformIOTypes ArrayType);

    void ArrayIOFrequencies_Sort(ArrayIO_Frequencies *Frequencies, ArrayIO_SortTypes SortType);

    void ArrayIOFequencies_Deinit(ArrayIO_Frequencies *Frequencies);

    void ArrayIO_Rotate(void *Array, PlatformIOTypes Type, size_t Amount2Rotate);

    void ArrayIO_Rotate_Ranged(void *Array, size_t Amount2Rotate, PlatformIO_Range Range);

    int64_t ArrayIO_GetMin(void *Array, PlatformIOTypes Type, size_t NumElements);

    int64_t ArrayIO_GetMax(void *Array, PlatformIOTypes Type, size_t NumElements);

    int64_t ArrayIO_GetSum(void *Array, PlatformIOTypes Type, size_t NumElements);

    void    ArrayIO_Reverse(void *Array, PlatformIOTypes Type, size_t NumElements);

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
