#include "../include/CollectionIO.h"   /* Included for our declarations */

#include "../include/AssertIO.h"       /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
     
     size_t CollectionIO_GetMinS8(int8_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT8_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
         
     
     size_t CollectionIO_GetMinU8(uint8_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT8_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
     
     size_t CollectionIO_GetMinS16(int16_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT16_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
     
     size_t CollectionIO_GetMinU16(uint16_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT16_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
     
     size_t CollectionIO_GetMinS32(int32_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT32_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
     
     size_t CollectionIO_GetMinU32(uint32_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT32_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
     
     size_t CollectionIO_GetMinS64(int64_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT64_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
     
     size_t CollectionIO_GetMinU64(uint64_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MinIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT64_MIN) {
                 MinIndex = Element;
                 break; // Return
             } else if (Array[Element] < Array[MinIndex]) {
                 MinIndex = Element;
         }
         return MinIndex;
    }
     
     int8_t CollectionIO_GetMaxS8(int8_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT8_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     uint8_t CollectionIO_GetMaxU8(uint8_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT8_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     int16_t CollectionIO_GetMaxS16(int16_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT16_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     uint16_t CollectionIO_GetMAxU16(uint16_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT16_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     int32_t CollectionIO_GetMaxS32(int32_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT32_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     uint32_t CollectionIO_GetMaxU32(uint32_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT32_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     int64_t CollectionIO_GetMaxS64(int64_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == INT64_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     uint64_t CollectionIO_GetMaxU64(uint64_t *Array, size_t NumElements) {
         AssertIO(Array != NULL);
         AssertIO(NumElements > 0);
         size_t MaxIndex = 0;
         
         for (size_t Element = 0; Element < NumElements; Element++) {
             if (Array[Element] == UINT64_MAX) {
                 MaxIndex = Element;
                 break; // Return
             } else if (Array[Element] > Array[MaxIndex]) {
                 MaxIndex = Element;
         }
         return MaxIndex;
    }
     
     
     int64_t CollectionIO_SumS8(int8_t *Array2Sum, size_t NumElements2Sum) {
        int64_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }
    
    uint64_t CollectionIO_SumU8(uint8_t *Array2Sum, size_t NumElements2Sum) {
        uint64_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }

    int64_t CollectionIO_SumS16(int16_t *Array2Sum, size_t NumElements2Sum) {
        int64_t Sum = 0;
        for (size_t Element = 0; Element < NumElements2Sum; Element++) {
            Sum += Array2Sum[Element];
        }
        return Sum;
    }
    
    uint64_t CollectionIO_SumU16(uint16_t *Array2Sum, size_t NumElements2Sum) {
        uint64_t Sum = 0;
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

    void CollectionIO_ReverseS8(int8_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }


void CollectionIO_ReverseU8(uint8_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }

void CollectionIO_ReverseS16(int16_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }

void CollectionIO_ReverseU16(uint16_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }

    void CollectionIO_ReverseS32(int32_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }


void CollectionIO_ReverseU32(uint32_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }

void CollectionIO_ReverseS64(int64_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }

void CollectionIO_ReverseU64(uint64_t *Array, size_t NumElements) {
        AssertIO(Array != NULL);
        AssertIO(NumElements > 0);
        
        for (size_t Element = 0; Element < NumElements; Element++) {
            Array[Element]               = Array[NumElements - Element];
                    Array[NumElements - Element] = Array[Element];
        }
    }

   void CollectionIO_RotateS8(int8_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   void CollectionIO_RotateU8(uint8_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   void CollectionIO_RotateS16(int16_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   void CollectionIO_RotateU16(uint16_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   void CollectionIO_RotateS32(int32_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   void CollectionIO_RotateU32(uint32_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   void CollectionIO_RotateS64(int64_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   void CollectionIO_RotateU64(uint64_t *Array, size_t NumElements, size_t Amount2Rotate) {
       AssertIO(Array != NULL);
       AssertIO(NumElements > 0);
       AssertIO(Amount2Rotate > 0);
       AssertIO(Amount2Rotate <= NumElements);
       
       CollectionIO_Reverse(Array, Amount2Rotate);

          CollectionIO_Reverse(Array[Amount2Rotate], NumElements - Amount2Rotate);
       
       CollectionIO_Reverse(Array, NumElements);
   }
   
   typedef struct FrequencyS8 {
       size_t Count;
       int8_t Value;
   } FrequencyS8;
   
   typedef struct HistogramS8 {
       FrequencyS8 *Frequencies;
       size_t       NumFrequencies;
   } HistogramS8;
   
   typedef struct FrequencyU8 {
       size_t  Count;
       uint8_t Value;
   } FrequencyU8;
   
   typedef struct HistogramU8 {
       FrequencyU8 *Frequencies;
       size_t       NumFrequencies;
   } HistogramU8;
   
   typedef struct FrequencyS16 {
       size_t  Count;
       int16_t Value;
   } FrequencyS16;
   
   typedef struct HistogramS16 {
       FrequencyS16 *Frequencies;
       size_t        NumFrequencies;
   } HistogramS16;
   
   typedef struct FrequencyU16 {
       size_t   Count;
       uint16_t Value;
   } FrequencyU16;
   
   typedef struct HistogramU16 {
       FrequencyU16 *Frequencies;
       size_t       NumFrequencies;
   } HistogramU16;
   
   typedef struct FrequencyS32 {
       size_t  Count;
       int32_t Value;
   } FrequencyS32;
   
   typedef struct HistogramS32 {
       FrequencyS32 *Frequencies;
       size_t        NumFrequencies;
   } HistogramS32;
   
   typedef struct FrequencyU32 {
       size_t   Count;
       uint32_t Value;
   } FrequencyU32;
   
   typedef struct HistogramU32 {
       FrequencyU32 *Frequencies;
       size_t       NumFrequencies;
   } HistogramU32;
   
   typedef struct FrequencyS64 {
       size_t  Count;
       int64_t Value;
   } FrequencyS64;
   
   typedef struct HistogramS64 {
       FrequencyS64 *Frequencies;
       size_t        NumFrequencies;
   } HistogramS64;
   
   typedef struct FrequencyU64 {
       size_t   Count;
       uint64_t Value;
   } FrequencyU64;
   
   typedef struct HistogramU64 {
       FrequencyU64 *Frequencies;
       size_t       NumFrequencies;
   } HistogramU64;
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   HistogramS8 HistogramS8_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 256);
       
       HistogramS8 Histogram = calloc(1, sizeof(HistogramS8);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyS8), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   
   HistogramU8 HistogramU8_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 256);
       
       HistogramU8 Histogram = calloc(1, sizeof(HistogramU8);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyU8), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   HistogramS16 HistogramS16_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 65536);
       
       HistogramS16 Histogram = calloc(1, sizeof(HistogramS16);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyS16), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   HistogramU16 HistogramU16_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 65536);
       
       HistogramU16 Histogram = calloc(1, sizeof(HistogramU16);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyU16), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   HistogramS32 HistogramS32_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 4294967296);
       
       HistogramS32 Histogram = calloc(1, sizeof(HistogramS32);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyS32), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   HistogramU32 HistogramU32_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 4294967296);
       
       HistogramU32 Histogram = calloc(1, sizeof(HistogramU32);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyU32), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   HistogramS64 HistogramS64_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 18446744073709551616);
       
       HistogramS64 Histogram = calloc(1, sizeof(HistogramS64);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyS64), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   HistogramU64 HistogramU64_Init(size_t NumElements) {
       AssertIO(NumElements > 0);
       AssertIO(NumElements <= 18446744073709551616);
       
       HistogramU64 Histogram = calloc(1, sizeof(HistogramU64);
       
       Histogram.Frequencies = calloc(sizeof(FrequencyU64), NumElements);
       AssertIO(Histogram.Frequencies != NULL);
       
       Histogram.NumElements = NumElements;
       return Histogram;
   }
   
   
   void HistogramS8_Measure(HistogramS8 *Histogram, int8_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
    Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   void HistogramU8_Measure(HistogramU8 *Histogram, uint8_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
           Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   void HistogramS16_Measure(HistogramS16 *Histogram, int16_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
           Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   void HistogramU16_Measure(HistogramU16 *Histogram, uint16_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
           Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   void HistogramS32_Measure(HistogramS32 *Histogram, int32_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
           Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   void HistogramU32_Measure(HistogramU32 *Histogram, uint32_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
           Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   void HistogramS64_Measure(HistogramS64 *Histogram, int64_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
           Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   void HistogramU64_Measure(HistogramU64 *Histogram, uint64_t *Array, size_t NumElements) {
       AssertIO(Histogram != NULL);
       AssertIO(Array != NULL);
       AssertIO(ArraySize > 0);
       
       for (size_t Element = 0; Element < NumElements; Element++) {
           Histogram.Frequencies.Count += 1;
    Histogram.Frequencies.Value = Array[Element];
       }
   }
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   

    

    static int Sort_Compare_AscendingS8(const int8_t *A, const int8_t *B) {
        return A - B;
    }
    
    static int Sort_Compare_AscendingU8(const uint8_t *A, const uint8_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingS16(const int16_t *A, const int16_t *B) {
        return A - B;
    }
    
    static int Sort_Compare_AscendingU16(const uint16_t *A, const uint16_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingS32(const int32_t *A, const int32_t *B) {
        return A - B;
    }
    
    static int Sort_Compare_AscendingU32(const uint32_t *A, const uint32_t *B) {
        return A - B;
    }

    static int Sort_Compare_AscendingS64(const int64_t *A, const int64_t *B) {
        return A - B;
    }
    
    static int Sort_Compare_AscendingU64(const uint64_t *A, const uint64_t *B) {
        return A - B;
    }

    static int Sort_Compare_DescendingS8(const int8_t *A, const int8_t *B) {
        return B - A;
    }
    
    static int Sort_Compare_DescendingU8(const uint8_t *A, const uint8_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingS16(const int16_t *A, const int16_t *B) {
        return B - A;
    }
    
    static int Sort_Compare_DescendingU16(const uint16_t *A, const uint16_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingS32(const int32_t *A, const int32_t *B) {
        return B - A;
    }
    
    static int Sort_Compare_DescendingU32(const uint32_t *A, const uint32_t *B) {
        return B - A;
    }

    static int Sort_Compare_DescendingS64(const int64_t *A, const int64_t *B) {
        return B - A;
    }
    
    static int Sort_Compare_DescendingU64(const uint64_t *A, const uint64_t *B) {
        return B - A;
    }
    
    void HistogramS8_Sort(HistogramS8 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int8_t), Sort_Compare_AscendingS8);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int8_t), Sort_Compare_DescendingS8);
        }
    }

void HistogramU8_Sort(HistogramU8 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint8_t), Sort_Compare_AscendingU8);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint8_t), Sort_Compare_DescendingU8);
        }
    }
    
    void HistogramS16_Sort(HistogramS16 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int16_t), Sort_Compare_AscendingS16);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int16_t), Sort_Compare_DescendingS16);
        }
    }
    
    void HistogramU16_Sort(HistogramU16 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint16_t), Sort_Compare_AscendingU16);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint16_t), Sort_Compare_DescendingU16);
        }
    }
    
    void HistogramS32_Sort(HistogramS32 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int32_t), Sort_Compare_AscendingS32);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int32_t), Sort_Compare_DescendingS32);
        }
    }
    
    void HistogramU32_Sort(HistogramU32 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint32_t), Sort_Compare_AscendingU32);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint32_t), Sort_Compare_DescendingU32);
        }
    }
    
    void HistogramS64_Sort(HistogramS64 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int64_t), Sort_Compare_AscendingS64);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(int64_t), Sort_Compare_DescendingS64);
        }
    }
    
    void HistogramU64_Sort(HistogramU64 Histogram, CollectionIO_SortTypes SortType) {
        AssertIO(SortType != SortType_Unspecified);
        
        if (SortType == SortType_Ascending) {
            qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint64_t), Sort_Compare_AscendingU64);
        } else if (SortType == SortType_Descending) {
        qsort(Histogram.Frequencies, Histogram.NumEntries, sizeof(uint64_t), Sort_Compare_DescendingU64);
        }
    }
    
    void HistogramS8_Deinit(HistogramS8 Histogram) {
        free(Histogram.Frequencies); 
    }
    
    
    void HistogramU8_Deinit(HistogramU8 Histogram) {
        free(Histogram.Frequencies); 
    }
    
    void HistogramS16_Deinit(HistogramS16 Histogram) {
        free(Histogram.Frequencies); 
    }
    
    void HistogramU16_Deinit(HistogramU16 Histogram) {
        free(Histogram.Frequencies); 
    }
    
    void HistogramS32_Deinit(HistogramS32 Histogram) {
        free(Histogram.Frequencies); 
    }
    
    void HistogramU32_Deinit(HistogramU32 Histogram) {
        free(Histogram.Frequencies); 
    }
    
    void HistogramS64_Deinit(HistogramS64 Histogram) {
        free(Histogram.Frequencies); 
    }
    
    
    void HistogramU64_Deinit(HistogramU64 Histogram) {
        free(Histogram.Frequencies); 
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
