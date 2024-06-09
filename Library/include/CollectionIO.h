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
     @enum         CollectionIO_SortTypes
     @abstract                                      Defines the type of sorting.
     @constant     SortType_Unspecified             Invalid SortType, exists solely to tell when it hasn't been set.
     @constant     SortType_Ascending               Index 0 contains the most common value.
     @constant     SortType_Descending              Index 0 contains the least common value.
     */
    typedef enum CollectionIO_SortTypes : uint8_t {
        SortType_Unspecified             = 0,
        SortType_Ascending               = 1,
        SortType_Descending              = 2,
    } CollectionIO_SortTypes;

    /*!
     @abstract               Measure the frequency of each symbol in the array
     @param Array2Measure    The array to measure
     @param ArrayNumElements The number of elements in the array
     @param ArrayType        The actual type of the array
     */
    void CollectionIO_Histogram_Measure(auto Frequencies, auto *Array2Measure, size_t ArrayNumElements);

    void CollectionIO_Histogram_Sort(auto Frequencies, CollectionIO_SortTypes SortType);

    void CollectionIO_Histogram_Deinit(auto Frequencies);
    
    /*!
     @abstract                                    Gets the Minimum value from whatever position the start pointer is to that + NumElements.
     @param       Array                       Where to start.
     @param       NumElements                 How many elements to search.
     @return                                      Returns the Index of the minimum value, if the value bottoms out it returns the first element of that dip.
     @function    CollectionIO_GetMinS8
     @function    CollectionIO_GetMinU8
     @function    CollectionIO_GetMinS16 
     @function    CollectionIO_GetMinU15
          @function    CollectionIO_GetMinS32
     @function    CollectionIO_GetMinU32
     @function    CollectionIO_GetMinS64
     @function    CollectionIO_GetMinU64
     */
    size_t CollectionIO_GetMinS8(int8_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMinU8(uint8_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMinS16(int16_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMinU16(uint16_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMinS32(int32_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMinU32(uint32_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMinS64(int64_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMinU64(uint64_t *Array, size_t NumElements);
    
#ifndef CollectionIO_GetMin
#define CollectionIO_GetMin(Array, NumElements) \
_Generic(Array, int8_t*:CollectionIO_GetMinS8, uint8_t*:CollectionIO_GetMinU8, int16_t*:CollectionIO_GetMinS16, uint16_t*:CollectionIO_GetMinU16, int32_t*:CollectionIO_GetMinS32, uint32_t*:CollectionIO_GetMinU32, int64_t*:CollectionIO_GetMinS64, uint64_t*:CollectionIO_GetMinU64)(Array, NumElements)
#endif /* CollectionIO_GetMin */
    
    /*!
     @abstract                                    Gets the Maximum value from whatever position the start pointer is to that + NumElements.
     @param       Array                       Where to start.
     @param       NumElements                 How many elements to search.
     @return                                      Returns the Index of the maximum value, if the value tops out it returns the first element of that peak.
     @function    CollectionIO_GetMaxS8
     @function    CollectionIO_GetMaxU8
     @function    CollectionIO_GetMaxS16 
     @function    CollectionIO_GetMaxU16
          @function    CollectionIO_GetMaxS32
     @function    CollectionIO_GetMaxU32
     @function    CollectionIO_GetMaxS64
     @function    CollectionIO_GetMaxU64
     */
    size_t CollectionIO_GetMaxS8(int8_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMaxU8(uint8_t *Array, size_t NumElements);
    
    size_t CollectionIO_GetMaxS16(int16_t *Array, size_t NumElements);
    
    uint64_t CollectionIO_GetMaxU16(uint16_t *Array, size_t NumElements);
    
    int64_t CollectionIO_GetMaxS32(int32_t *Array, size_t NumElements);
    
    uint64_t CollectionIO_GetMaxU32(uint32_t *Array, size_t NumElements);
    
    int64_t CollectionIO_GetMaxS64(int64_t *Array, size_t NumElements);
    
    uint64_t CollectionIO_GetMaxU64(uint64_t *Array, size_t NumElements);
    
#ifndef CollectionIO_GetMax
#define CollectionIO_GetMax(Array, NumElements) \
_Generic(Array, int8_t*:CollectionIO_GetMaxS8, uint8_t*:CollectionIO_GetMaxU8, int16_t*:CollectionIO_GetMaxS16, uint16_t*:CollectionIO_GetMaxU16, int32_t*:CollectionIO_GetMaxS32, uint32_t*:CollectionIO_GetMaxU32, int64_t*:CollectionIO_GetMaxS64, uint64_t*:CollectionIO_GetMaxU64)(Array, NumElements)
#endif /* CollectionIO_GetMax */
    
    /*!
     @abstract                                    Sums an array from whatever position the start pointer is to that + NumElements2Sum.
     @param       Array2Sum                       Where to start the sumation.
     @param       NumElements2Sum                 How many elements to sum.
     @return                                      Returns the sum of the array.
     @function    CollectionIO_SumS8
     @function    CollectionIO_SumU8
     @function    CollectionIO_SumS16 
     @function    CollectionIO_SumU16
          @function    CollectionIO_SumS32
     @function    CollectionIO_SumU32
     @function    CollectionIO_SumS64
     @function    CollectionIO_SumU64
     */
    int64_t CollectionIO_SumS8(int8_t *Array2Sum, size_t NumElements2Sun);
    
    uint64_t CollectionIO_SumU8(uint8_t *Array2Sum, size_t NumElements2Sun);
    
    int64_t CollectionIO_SumS16(int16_t *Array2Sum, size_t NumElements2Sun);
    
    uint64_t CollectionIO_SumU16(uint16_t *Array2Sum, size_t NumElements2Sun);
    
    int64_t CollectionIO_SumS32(int32_t *Array2Sum, size_t NumElements2Sun);
    
    uint64_t CollectionIO_SumU32(uint32_t *Array2Sum, size_t NumElements2Sun);
    
    int64_t CollectionIO_SumS64(int64_t *Array2Sum, size_t NumElements2Sun);
    
    uint64_t CollectionIO_SumU64(uint64_t *Array2Sum, size_t NumElements2Sun);
    
#ifndef CollectionIO_Sum
#define CollectionIO_Sum(Array2Sum, NumElements) \
_Generic(Array2Sum, int8_t*:CollectionIO_SumS8, uint8_t*:CollectionIO_SumU8, int16_t*:CollectionIO_SumS16, uint16_t*:CollectionIO_SunU16, int32_t*:CollectionIO_SumS32, uint32_t*:CollectionIO_SumU32, int64_t*:CollectionIO_SumS64, uint64_t*:CollectionIO_SumU64)(Array2Sum, NumElements)
#endif /* CollectionIO_Sum */
    
    /*!
     @abstract                                    Reverses an array in place from whatever position the start pointer is to that + NumElements.
     @param       Array                       Where to start the Reversal.
     @param       NumElements                 How many elements to reverse.
     @return                                      Returns the sum of the array.
     @function    CollectionIO_ReverseS8
     @function    CollectionIO_ReverseU8
     @function    CollectionIO_ReverseS16 
     @function    CollectionIO_ReverseU16
          @function    CollectionIO_ReverseS32
     @function    CollectionIO_ReverseU32
     @function    CollectionIO_ReverseS64
     @function    CollectionIO_ReverseU64
     */
    int64_t CollectionIO_ReverseS8(int8_t *Array, size_t NumElements);
    
    void CollectionIO_ReverseU8(uint8_t *Array, size_t NumElements);
    
    void CollectionIO_ReverseS16(int16_t *Array, size_t NumElements);
    
    void CollectionIO_ReverseU16(uint16_t *Array, size_t NumElements);
    
    void CollectionIO_ReverseS32(int32_t *Array, size_t NumElements);
    
    void CollectionIO_ReverseU32(uint32_t *Array, size_t NumElements);
    
    void CollectionIO_ReverseS64(int64_t *Array, size_t NumElements);
    
    void CollectionIO_ReverseU64(uint64_t *Array, size_t NumElements);
    
#ifndef CollectionIO_Reverse
#define CollectionIO_Reverse(Array, NumElements) \
_Generic(Array, int8_t*:CollectionIO_SumS8, uint8_t*:CollectionIO_SumU8, int16_t*:CollectionIO_SumS16, uint16_t*:CollectionIO_SunU16, int32_t*:CollectionIO_SumS32, uint32_t*:CollectionIO_SumU32, int64_t*:CollectionIO_SumS64, uint64_t*:CollectionIO_SumU64)(Array, NumElements)
#endif /* CollectionIO_Reverse */

    /*!
     @abstract                                    Rotates an array in place from whatever position the start pointer is to that + Amount2Rotate.
     @param       Array                       Where to start the Rotation.
     @param       NumElements                 How many elements to reverse.
     @param        Amount2Rotate The number of elements to rotate.
     @function    CollectionIO_RotateS8
     @function    CollectionIO_RotateU8
     @function    CollectionIO_RotateS16 
     @function    CollectionIO_RotateU16
          @function    CollectionIO_RotateS32
     @function    CollectionIO_RotateU32
     @function    CollectionIO_RotateS64
     @function    CollectionIO_RotateU64
     */
     void CollectionIO_RotateS8(int8_t *Array, size_t NumElements, size_t Amount2Rotate);
     
     void CollectionIO_RotateU8(uint8_t *Array, size_t NumElements, size_t Amount2Rotate);
     
     void CollectionIO_RotateS16(int16_t *Array, size_t NumElements, size_t Amount2Rotate);
     
     void CollectionIO_RotateU16(uint16_t *Array, size_t NumElements, size_t Amount2Rotate);
     
     void CollectionIO_RotateS32(int32_t *Array, size_t NumElements, size_t Amount2Rotate);
     
     void CollectionIO_RotateU32(uint32_t *Array, size_t NumElements, size_t Amount2Rotate);
     
     void CollectionIO_RotateS64(int64_t *Array, size_t NumElements, size_t Amount2Rotate);
     
     void CollectionIO_RotateU64(uint64_t *Array, size_t NumElements, size_t Amount2Rotate);
    
#ifndef CollectionIO_Rotate
#define CollectionIO_Rotate(Array, NumElements) \
_Generic(Array, int8_t*:CollectionIO_RotateS8, uint8_*t*:CollectionIO_RotateU8, int16_t*:CollectionIO_RotateS16, uint16_t*:CollectionIO_RotateU16, int32_t*:CollectionIO_RotateS32, uint32_t*:CollectionIO_RotateU32, int64_t*:CollectionIO_RotateS64, uint64_t*:CollectionIO_RotateU64)(Array, NumElements)
#endif /* CollectionIO_Rotate */

    /*!
     @abstract                                    Measures the histogram of the Array from whatever position the start pointer is to that + NumElements.
     @param       Histogram                Where to store the measured results.
     @param       Array                       Where to start measuring.
     @param       NumElements                 How many elements to measure.
     @function    HistogramS8_Measure
     @function    HistogramS16_Measure
     @function    HistogramU16_Measure 
     @function    HistogramS32_Measure
          @function    HistogramU32_Measure
     @function    HistogramS64_Measure
     @function    HistogramU64_Measure
     */
void HistogramS8_Measure(HistogramS8 *Histogram, int8_t *Array, size_t NumElements);

void HistogramU8_Measure(HistogramU8 *Histogram, uint8_t *Array, size_t NumElements);

void HistogramS16_Measure(HistogramS16 *Histogram, int16_t *Array, size_t NumElements);

void HistogramU16_Measure(HistogramU16 *Histogram, uint16_t *Array, size_t NumElements);

void HistogramS32_Measure(HistogramS32 *Histogram, int32_t *Array, size_t NumElements);

void HistogramU32_Measure(HistogramU32 *Histogram, uint32_t *Array, size_t NumElements);

void HistogramS64_Measure(HistogramS64 *Histogram, int64_t *Array, size_t NumElements)

void HistogramU64_Measure(HistogramU64 *Histogram, int64_t *Array, size_t NumElements);
#ifndef Histogram_Measure
#define Histogram_Measure(Histogram, Array, NumElements) \
_Generic(Array, int8_t*:HistogramS8_Measure, uint8_*t*:HistogramU8_Measure, int16_t*:HistogramS16_Measure, uint16_t*:HistogramU16_Measure, int32_t*:HistogramS32_Measure, uint32_t*:HistogramU32_Measure, int64_t*:HistogramS64_Measure, uint64_t*:HistogramU64_Measure)(Histogram, Array, NumElements)
#endif /* Histogram_Measure */

    /*!
     @abstract                                    Sorts the histogram.
     @param       Histogram                The Histogram to sort.
     @param       SortType                       Should the Histogram be sorted in Ascending or Descending order?
     @function    HistogramS8_Sort
     @function    HistogramS16_Sort
     @function    HistogramU16_Sort 
     @function    HistogramS32_Sort
          @function    HistogramU32_Sort
     @function    HistogramS64_Sort
     @function    HistogramU64_Sort
     */
void HistogramS8_Sort(HistogramS8 Histogram, CollectionIO_SortTypes SortType);

void HistogramU8_Sort(HistogramU8 Histogram, CollectionIO_SortTypes SortType);

void HistogramS16_Sort(HistogramS16 Histogram, CollectionIO_SortTypes SortType);

void HistogramU16_Sort(HistogramU16 Histogram, CollectionIO_SortTypes SortType);

void HistogramS32_Sort(HistogramS32 Histogram, CollectionIO_SortTypes SortType);

void HistogramU32_Sort(HistogramU32 Histogram, CollectionIO_SortTypes SortType);

void HistogramS64_Sort(HistogramS64 Histogram, CollectionIO_SortTypes SortType);

void HistogramU64_Sort(HistogramU64 Histogram, CollectionIO_SortTypes SortType);
#ifndef Histogram_Sort
#define Histogram_Sort(Histogram, SortType) \
_Generic(Histogram, HistogramS8:HistogramS8_Sort, HistogramU8:HistogramU8_Sort, HistogramS16:HistogramS16_Sort, HistogramU16:HistogramU16_Sort, HistogramS32:HistogramS32_Sort, HistogramU32:HistogramU32_Sort, HistogramS64:HistogramS64_Sort, HistogramU64:HistogramU64_Sort)(Histogram, SortType)
#endif /* Histogram_Sort */














    /*!
     @abstract                                    Deinitializes the histogram.
     @param       Histogram                The Histogram to deinitialize.
     @function    HistogramS8_Deinit
     @function HistogramU8_Deinit
     @function    HistogramS16_Deinit
     @function    HistogramU16_Deinit 
     @function    HistogramS32_Deinit
          @function    HistogramU32_Deinit
     @function    HistogramS64_Deinit
     @function    HistogramU64_Deinit
     */
void HistogramS8_Deinit(HistogramS8 Histogram);

void HistogramU8_Deinit(HistogramU8 Histogram);

void HistogramS16_Deinit(HistogramS16 Histogram);

void HistogramU16_Deinit(HistogramU16 Histogram);

void HistogramS32_Deinit(HistogramS32 Histogram);

void HistogramU32_Deinit(HistogramU32 Histogram);

void HistogramS64_Deinit(HistogramS64 Histogram);

void HistogramU64_Deinit(HistogramU64 Histogram);
#ifndef Histogram_Deinit
#define Histogram_Deinit(Histogram) \
_Generic(Histogram, HistogramS8:HistogramS8_Deinit, HistogramU8:HistogramU8_Deinit, HistogramS16:HistogramS16_Deinit, HistogramU16:HistogramU16_Deinit, HistogramS32:HistogramS32_Deinit, HistogramU32:HistogramU32_Deinit, HistogramS64:HistogramS64_Deinit, HistogramU64:HistogramU64_Deinit)(Histogram)
#endif /* Histogram_Deinit */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_CollectionIO_H */
