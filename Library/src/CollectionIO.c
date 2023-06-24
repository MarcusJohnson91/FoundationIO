#include "../include/CollectionIO.h"   /* Included for our declarations */

#include "../include/AssertIO.h"       /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     if PlatformIO_Is(Type, PlatformIOType_Signed) {
     if PlatformIO_Is(Type, PlatformIOType_Integer8) {

     } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {

     } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {

     } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {

     }
     } else if PlatformIO_Is(Type, PlatformIOType_Unsigned) {
     if PlatformIO_Is(Type, PlatformIOType_Integer8) {

     } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {

     } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {

     } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {

     }
     }
     */

    int64_t ArrayIO_GetMin(void *Array, PlatformIOTypes Type, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(Type != PlatformIOType_Unspecified);
        AssertIO(NumElements > 0);
        int64_t Min = INT64_MAX;
        if PlatformIO_Is(Type, PlatformIOType_Signed) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                int8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                int16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                int32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                int64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            }
        } else if PlatformIO_Is(Type, PlatformIOType_Unsigned) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                uint8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                uint16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                uint32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                uint64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Min) {
                        Min = TypedArray[Element];
                    }
                }
            }
        }
        return Min;
    }

    int64_t ArrayIO_GetMax(void *Array, PlatformIOTypes Type, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(Type != PlatformIOType_Unspecified);
        AssertIO(NumElements > 0);
        int64_t Max = INT64_MIN;
        if PlatformIO_Is(Type, PlatformIOType_Signed) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                int8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                int16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                int32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                int64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            }
        } else if PlatformIO_Is(Type, PlatformIOType_Unsigned) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                uint8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                uint16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                uint32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                uint64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    if (TypedArray[Element] > Max) {
                        Max = TypedArray[Element];
                    }
                }
            }
        }
        return Max;
    }

    int64_t ArrayIO_GetSum(void *Array, PlatformIOTypes Type, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(Type != PlatformIOType_Unspecified);
        AssertIO(NumElements > 0);
        int64_t Sum = 0;
        if PlatformIO_Is(Type, PlatformIOType_Signed) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                int8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                int16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                int32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                int64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            }
        } else if PlatformIO_Is(Type, PlatformIOType_Unsigned) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                uint8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                uint16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                uint32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                uint64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    Sum += TypedArray[Element];
                }
            }
        }
        return Sum;
    }

    void ArrayIO_Reverse(void *Array, PlatformIOTypes Type, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        if PlatformIO_Is(Type, PlatformIOType_Signed) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                int8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                int16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                int32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                int64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            }
        } else if PlatformIO_Is(Type, PlatformIOType_Unsigned) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                int8_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {
                int16_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {
                int32_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {
                int64_t *TypedArray = Array;
                for (size_t Element = 0; Element < NumElements; Element++) {
                    TypedArray[Element]               = TypedArray[NumElements - Element];
                    TypedArray[NumElements - Element] = TypedArray[Element];
                }
            }
        }
    }

    /*
     Make a function to check that multiple chunks of an OR-able enum are all represented.

     like PlatformIOTypes, that all variables of that type have either a Signed or Unsigned bit set, and that all of them contain a valid type too.
     */

    void ArrayIO_Rotate(void *Array, size_t NumElementsInArray, PlatformIOTypes Type, size_t Amount2Rotate) {
        AssertIO(Array != NULL);
        AssertIO(Type != PlatformIOType_Unspecified);

        //Rotating an array is a circular shift, so move Amount2Rotate elements from the starting position to the ending position.

        /*
         {1, 2, 3, 4} Amount2Rotate = 2 = {3, 4, 1, 2}
         */

        if PlatformIO_Is(Type, PlatformIOType_Signed) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {
                int8_t *TypedArray = Array;
                for (size_t Element = Amount2Rotate; Element < NumElementsInArray; Element++) {
                    TypedArray[Element] = TypedArray[Element + Amount2Rotate];
                }
            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {

            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {

            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {

            }
        } else if PlatformIO_Is(Type, PlatformIOType_Unsigned) {
            if PlatformIO_Is(Type, PlatformIOType_Integer8) {

            } else if PlatformIO_Is(Type, PlatformIOType_Integer16) {

            } else if PlatformIO_Is(Type, PlatformIOType_Integer32) {

            } else if PlatformIO_Is(Type, PlatformIOType_Integer64) {

            }
        }
    }

    void ArrayIO_Reverse_Ranged(void *Array, PlatformIOTypes Type, PlatformIO_Range Range) {
        AssertIO(Array != NULL);
        AssertIO(Type != PlatformIOType_Unspecified);
        AssertIO(Range_GetLength(Range) != 0);

        size_t           Split  = Range.Start;
        PlatformIO_Range Range1 = Range_Init(Range.Start, Split);
        PlatformIO_Range Range2 = Range_Init(Split, Range.End);

        ArrayIO_Reverse_Ranged(Array, Type, Range1);
        ArrayIO_Reverse_Ranged(Array, Type, Range2);
        ArrayIO_Reverse_Ranged(Array, Type, Range);
    }

    void ArrayIO_CircleShift(void *Array, size_t NumElementsInArray, size_t NumElements2Shift) { // Inspired by UTF32_ShiftCodePoints
        AssertIO(Array != NULL);
        AssertIO(NumElementsInArray > 0);
        AssertIO(NumElements2Shift > 0);

        /*
         "0123456789" -> "2345678901"
         aka read the first two elements and put them at the end

         */
    }

    ArrayIO_Frequencies *ArrayIOFrequency_Init(size_t NumElements, PlatformIOTypes Type) {
        AssertIO(NumElements > 0);
        AssertIO(Type != PlatformIOType_Unspecified);

        ArrayIO_Frequencies *Frequencies = calloc(1, sizeof(ArrayIO_Frequencies));
        AssertIO(Frequencies != NULL);

        Frequencies->Array = calloc(NumElements, (Type & 0x3C) / 4);
        AssertIO(Frequencies->Array != NULL);
        Frequencies->NumEntries = NumElements;
        Frequencies->Type = Type;
        return Frequencies;
    }

    /*
     Now we need a function to actually measure the frequencies and a function to sort them

     like, if we're handed an array that we need to measure, what do we do?

     The values can be audio of image samples, it could be a string, it can be almos everything
     */

    void ArrayIOFequncies_Measure(ArrayIO_Frequencies *Frequencies, void *Array2Measure, size_t ArrayNumElements, PlatformIOTypes ArrayType) {
        AssertIO(Frequencies != NULL);
        AssertIO(Array2Measure != NULL);
        AssertIO(ArrayNumElements > 0);
        AssertIO(ArrayType != PlatformIOType_Unspecified);

        if PlatformIO_Is(ArrayType, PlatformIOType_Unsigned) {
            if PlatformIO_Is(ArrayType, PlatformIOType_Integer8) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    uint8_t *OldArray = Frequencies->Array;
                    uint8_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            } else if PlatformIO_Is(ArrayType, PlatformIOType_Integer16) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    uint16_t *OldArray = Frequencies->Array;
                    uint16_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            } else if PlatformIO_Is(ArrayType, PlatformIOType_Integer32) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    uint32_t *OldArray = Frequencies->Array;
                    uint32_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            } else if PlatformIO_Is(ArrayType, PlatformIOType_Integer64) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    uint64_t *OldArray = Frequencies->Array;
                    uint64_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            }
        } else if PlatformIO_Is(ArrayType, PlatformIOType_Signed) {
            if PlatformIO_Is(ArrayType, PlatformIOType_Integer8) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    int8_t *OldArray = Frequencies->Array;
                    int8_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            } else if PlatformIO_Is(ArrayType, PlatformIOType_Integer16) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    int16_t *OldArray = Frequencies->Array;
                    int16_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            } else if PlatformIO_Is(ArrayType, PlatformIOType_Integer32) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    int32_t *OldArray = Frequencies->Array;
                    int32_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            } else if PlatformIO_Is(ArrayType, PlatformIOType_Integer64) {
                for (size_t Element = 0; Element < ArrayNumElements; Element++) {
                    int64_t *OldArray = Frequencies->Array;
                    int64_t *NewArray = Array2Measure;
                    OldArray[NewArray[Element]] += 1;
                }
            }
        }
    }

    static int Sort_Compare_AscendingU8(const uint8_t *A, const uint8_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingS8(const int8_t *A, const int8_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingU16(const uint16_t *A, const uint16_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingS16(const int16_t *A, const int16_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingU32(const uint32_t *A, const uint32_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingS32(const int32_t *A, const int32_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingU64(const uint64_t *A, const uint64_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingS64(const int64_t *A, const int64_t *B) {
        return A - B;
    }

    static int Sort_Compare_DescendingU8(const uint8_t *A, const uint8_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingS8(const int8_t *A, const int8_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingU16(const uint16_t *A, const uint16_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingS16(const int16_t *A, const int16_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingU32(const uint32_t *A, const uint32_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingS32(const int32_t *A, const int32_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingU64(const uint64_t *A, const uint64_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingS64(const int64_t *A, const int64_t *B) {
        return B - A;
    }



    void ArrayIOFrequencies_Sort(ArrayIO_Frequencies *Frequencies, ArrayIO_SortTypes SortType) {
        AssertIO(Frequencies != NULL);
        AssertIO(SortType != SortType_Unspecified);

        if (SortType == SortType_Ascending) {
            if PlatformIO_Is(Frequencies->Type, PlatformIOType_Unsigned) {
                if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer8) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint8_t), Sort_Compare_AscendingU8);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer16) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint16_t), Sort_Compare_AscendingU16);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer32) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint32_t), Sort_Compare_AscendingU32);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer64) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint64_t), Sort_Compare_AscendingU64);
                }
            } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Signed) {
                if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer8) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint8_t), Sort_Compare_AscendingS8);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer16) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint16_t), Sort_Compare_AscendingS16);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer32) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint32_t), Sort_Compare_AscendingS32);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer64) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint64_t), Sort_Compare_AscendingS64);
                }
            }
        } else if (SortType == SortType_Descending) {
            if PlatformIO_Is(Frequencies->Type, PlatformIOType_Unsigned) {
                if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer8) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint8_t), Sort_Compare_DescendingS8);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer16) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint16_t), Sort_Compare_DescendingS16);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer32) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint32_t), Sort_Compare_DescendingS32);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer64) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint64_t), Sort_Compare_DescendingS64);
                }
            } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Signed) {
                if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer8) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint8_t), Sort_Compare_DescendingS8);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer16) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint16_t), Sort_Compare_DescendingS16);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer32) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint32_t), Sort_Compare_DescendingS32);
                } else if PlatformIO_Is(Frequencies->Type, PlatformIOType_Integer64) {
                    qsort(Frequencies->Array, Frequencies->NumEntries, sizeof(uint64_t), Sort_Compare_DescendingS64);
                }
            }
        }
    }

    void ArrayIOFequencies_Deinit(ArrayIO_Frequencies *Frequencies) {
        free(Frequencies->Array);
        free(Frequencies);
    }
    
    int64_t CollectionIO_SumS8(int8_t *Array2Sum, size_t NumElements2Sum) {
        size_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }
    
    uint64_t CollectionIO_SumU8(uint8_t *Array2Sum, size_t NumElements2Sum) {
        size_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }

    int64_t CollectionIO_SumS16(int16_t *Array2Sum, size_t NumElements2Sum) {
        size_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }
    
    uint64_t CollectionIO_SumU16(uint16_t *Array2Sum, size_t NumElements2Sum) {
        size_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }

    int64_t CollectionIO_SumS32(int32_t *Array2Sum, size_t NumElements2Sum) {
        int64_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }
    
    uint64_t CollectionIO_SumU32(uint32_t *Array2Sum, size_t NumElements2Sum) {
        uint64_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }

    int64_t CollectionIO_SumS64(int64_t *Array2Sum, size_t NumElements2Sum) {
        int64_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }
    
    uint64_t CollectionIO_SumU64(uint64_t *Array2Sum, size_t NumElements2Sum) {
        uint64_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }

    /* Slice stuff */
    CollectionIO_Slice Slice_Init(size_t StartElement, size_t StopElement) {
        AssertIO(StartElement < StopElement);
        CollectionIO_Slice Slice = {0};
        Slice.StartElement  = StartElement;
        Slice.StopElement   = StopElement;
        return Slice;
    }

    size_t Slice_GetStartElement(CollectionIO_Slice Slice) {
        return Slice.StartElement;
    }

    size_t Slice_GetStopElement(CollectionIO_Slice Slice) {
        return Slice.StopElement;
    }
    /* Slice stuff */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
