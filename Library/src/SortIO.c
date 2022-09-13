#include "../include/SortIO.h"         /* Included for our declarations */

#include "../include/AssertIO.h"       /* Included for Assertions */
#include "../include/MathIO.h"         /* Included for Minimum and Maximum */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct Pair {
        size_t Value;
        size_t Index;
    } Pair;

    typedef bool (*CompareFunction)(Pair, Pair);

    typedef struct WikiIterator {
        size_t Size;
        size_t PowerOfTwo;
        size_t Numerator;
        size_t Decimal;
        size_t Denominator;
        size_t DecimalStep;
        size_t NumeratorStep;
    } WikiIterator;

    void WikiIterator_begin(WikiIterator *Wiki) {
        Wiki->Numerator = 0;
        Wiki->Decimal = 0;
    }

    PlatformIO_Range WikiIterator_nextRange(WikiIterator *Wiki) {
        size_t Start = Wiki->Decimal;

        Wiki->Decimal += Wiki->DecimalStep;
        Wiki->Numerator += Wiki->NumeratorStep;
        if (Wiki->Numerator >= Wiki->Denominator) {
            Wiki->Numerator -= Wiki->Denominator;
            Wiki->Decimal++;
        }

        return Range_Init(Start, Wiki->Decimal);
    }

    bool WikiIterator_finished(WikiIterator *Wiki) {
        return (Wiki->Decimal >= Wiki->Size);
    }

    bool WikiIterator_nextLevel(WikiIterator *Wiki) {
        Wiki->DecimalStep += Wiki->DecimalStep;
        Wiki->NumeratorStep += Wiki->NumeratorStep;
        if (Wiki->NumeratorStep >= Wiki->Denominator) {
            Wiki->NumeratorStep -= Wiki->Denominator;
            Wiki->DecimalStep++;
        }

        return (Wiki->DecimalStep < Wiki->Size);
    }

    size_t WikiIterator_length(WikiIterator *Wiki) {
        return Wiki->DecimalStep;
    }

    WikiIterator WikiIterator_Init(size_t size2, size_t min_level) {
        WikiIterator Wiki;
        Wiki.Size          = size2;
        Wiki.PowerOfTwo    = FloorPowerOfTwo(Wiki.Size);
        Wiki.Denominator   = Wiki.PowerOfTwo / min_level;
        Wiki.NumeratorStep = Wiki.Size % Wiki.Denominator;
        Wiki.DecimalStep   = Wiki.Size / Wiki.Denominator;
        WikiIterator_begin(&Wiki);
        return Wiki;
    }

    static void SortIO_Swap(int64_t Value1, int64_t Value2) {
        int64_t Temp = Value1;
        Value2 = Temp;
        
        PlatformIO_Generic(A, &Value1);
        PlatformIO_Generic(B, &Value2);
        PlatformIO_Generic(C, *A);
        *A = *B;
        *B = C;
    }

    static void SortIO_Reverse(Pair Array[], const PlatformIO_Range Range) {
        for (size_t Index = Range_GetLength(Range) / 2; Index > 0; Index--) {
            SortIO_Swap(Array[Range.Start + Index - 1], Array[Range.End - Index]);
        }
    }

    static void SortIO_Rotate(Pair Array[], size_t Amount2Rotate, PlatformIO_Range Range) {
        AssertIO(Range_GetLength(Range) != 0);

        size_t           Split  = Range.Start + Amount2Rotate;
        PlatformIO_Range Range1 = Range_Init(Range.Start, Split);
        PlatformIO_Range Range2 = Range_Init(Split, Range.End);

        SortIO_Reverse(Array, Range1);
        SortIO_Reverse(Array, Range2);
        SortIO_Reverse(Array, Range);
    }

    void WikiSort(Pair array[], const size_t Size, const CompareFunction compare) {
        /* use a small cache to speed up some of the operations */
#if DYNAMIC_CACHE
        size_t cache_size;
        Pair  *cache = NULL;
#else
        /* since the cache Size is fixed, it's still O(1) memory! */
        /* just keep in mind that making it too small ruins the point (nothing will fit into it), */
        /* and making it too large also ruins the point (so much for "low memory"!) */
        /* removing the cache entirely still gives 70% of the performance of a standard merge */
#define CACHE_SIZE 512
        const size_t cache_size = CACHE_SIZE;
        Pair         cache[CACHE_SIZE];
#endif

        WikiIterator iterator;

        /* if the array is of Size 0, 1, 2, or 3, just sort them like so: */
        if (Size < 4) {
            if (Size == 3) {
                /* hard-coded insertion sort */
                if (compare(array[1], array[0]))
                    SortIO_Swap(array[0], array[1]);
                if (compare(array[2], array[1])) {
                    SortIO_Swap(array[1], array[2]);
                    if (compare(array[1], array[0]))
                        SortIO_Swap(array[0], array[1]);
                }
            } else if (Size == 2) {
                /* SortIO_Swap the items if they're out of order */
                if (compare(array[1], array[0]))
                    SortIO_Swap(array[0], array[1]);
            }

            return;
        }

        /* sort groups of 4-8 items at a time using an unstable sorting network, */
        /* but keep track of the original item orders to force it to be stable */
        /* http://pages.ripco.net/~jgamble/nw.html */
        iterator = WikiIterator_Init(Size, 4);
        WikiIterator_begin(&iterator);
        while (!WikiIterator_finished(&iterator)) {
            uint8_t          order[] = {0, 1, 2, 3, 4, 5, 6, 7};
            PlatformIO_Range range   = WikiIterator_nextRange(&iterator);

#define SortIO_Swap(x, y)                                                                                                                                   \
if (compare(array[range.Start + y], array[range.Start + x]) || (order[x] > order[y] && !compare(array[range.Start + x], array[range.Start + y]))) { \
SortIO_Swap(array[range.Start + x], array[range.Start + y]);                                                                                    \
SortIO_Swap(order[x], order[y]);                                                                                                                \
}

            if (Range_GetLength(range) == 8) {
                SortIO_Swap(0, 1);
                SortIO_Swap(2, 3);
                SortIO_Swap(4, 5);
                SortIO_Swap(6, 7);
                SortIO_Swap(0, 2);
                SortIO_Swap(1, 3);
                SortIO_Swap(4, 6);
                SortIO_Swap(5, 7);
                SortIO_Swap(1, 2);
                SortIO_Swap(5, 6);
                SortIO_Swap(0, 4);
                SortIO_Swap(3, 7);
                SortIO_Swap(1, 5);
                SortIO_Swap(2, 6);
                SortIO_Swap(1, 4);
                SortIO_Swap(3, 6);
                SortIO_Swap(2, 4);
                SortIO_Swap(3, 5);
                SortIO_Swap(3, 4);

            } else if (Range_GetLength(range) == 7) {
                SortIO_Swap(1, 2);
                SortIO_Swap(3, 4);
                SortIO_Swap(5, 6);
                SortIO_Swap(0, 2);
                SortIO_Swap(3, 5);
                SortIO_Swap(4, 6);
                SortIO_Swap(0, 1);
                SortIO_Swap(4, 5);
                SortIO_Swap(2, 6);
                SortIO_Swap(0, 4);
                SortIO_Swap(1, 5);
                SortIO_Swap(0, 3);
                SortIO_Swap(2, 5);
                SortIO_Swap(1, 3);
                SortIO_Swap(2, 4);
                SortIO_Swap(2, 3);

            } else if (Range_GetLength(range) == 6) {
                SortIO_Swap(1, 2);
                SortIO_Swap(4, 5);
                SortIO_Swap(0, 2);
                SortIO_Swap(3, 5);
                SortIO_Swap(0, 1);
                SortIO_Swap(3, 4);
                SortIO_Swap(2, 5);
                SortIO_Swap(0, 3);
                SortIO_Swap(1, 4);
                SortIO_Swap(2, 4);
                SortIO_Swap(1, 3);
                SortIO_Swap(2, 3);

            } else if (Range_GetLength(range) == 5) {
                SortIO_Swap(0, 1);
                SortIO_Swap(3, 4);
                SortIO_Swap(2, 4);
                SortIO_Swap(2, 3);
                SortIO_Swap(1, 4);
                SortIO_Swap(0, 3);
                SortIO_Swap(0, 2);
                SortIO_Swap(1, 3);
                SortIO_Swap(1, 2);

            } else if (Range_GetLength(range) == 4) {
                SortIO_Swap(0, 1);
                SortIO_Swap(2, 3);
                SortIO_Swap(0, 2);
                SortIO_Swap(1, 3);
                SortIO_Swap(1, 2);
            }
        }
        if (Size < 8)
            return;

#if DYNAMIC_CACHE
        /* good choices for the cache Size are: */
        /* (Size + 1)/2 – turns into a full-speed standard merge sort since everything fits into the cache */
        cache_size = (Size + 1) / 2;
        cache      = (Pair *) malloc(cache_size * sizeof(array[0]));

        if (!cache) {
            /* sqrt((Size + 1)/2) + 1 – this will be the Size of the A blocks at the largest level of merges, */
            /* so a buffer of this Size would allow it to skip using internal or in-place merges for anything */
            cache_size = sqrt(cache_size) + 1;
            cache      = (Pair *) malloc(cache_size * sizeof(array[0]));

            if (!cache) {
                /* 512 – chosen from careful testing as a good balance between fixed-Size memory use and run time */
                if (cache_size > 512) {
                    cache_size = 512;
                    cache      = (Pair *) malloc(cache_size * sizeof(array[0]));
                }

                /* 0 – if the system simply cannot allocate any extra memory whatsoever, no memory works just fine */
                if (!cache)
                    cache_size = 0;
            }
        }
#endif

        /* then merge sort the higher levels, which can be 8-15, 16-31, 32-63, 64-127, etc. */
        while (true) {
            /* if every A and B block will fit into the cache, use a special branch specifically for merging with the cache */
            /* (we use < rather than <= since the block Size might be one more than iterator.length()) */
            if (WikiIterator_length(&iterator) < cache_size) {
                /* if four subarrays fit into the cache, it's faster to merge both pairs of subarrays into the cache, */
                /* then merge the two merged subarrays from the cache back into the original array */
                if ((WikiIterator_length(&iterator) + 1) * 4 <= cache_size && WikiIterator_length(&iterator) * 4 <= Size) {
                    WikiIterator_begin(&iterator);
                    while (!WikiIterator_finished(&iterator)) {
                        /* merge A1 and B1 into the cache */
                        PlatformIO_Range A1, B1, A2, B2, A3, B3;
                        A1 = WikiIterator_nextRange(&iterator);
                        B1 = WikiIterator_nextRange(&iterator);
                        A2 = WikiIterator_nextRange(&iterator);
                        B2 = WikiIterator_nextRange(&iterator);

                        if (compare(array[B1.End - 1], array[A1.Start])) {
                            /* the two ranges are in reverse order, so copy them in reverse order into the cache */
                            memcpy(&cache[Range_GetLength(B1)], &array[A1.Start], Range_GetLength(A1) * sizeof(array[0]));
                            memcpy(&cache[0], &array[B1.Start], Range_GetLength(B1) * sizeof(array[0]));
                        } else if (compare(array[B1.Start], array[A1.End - 1])) {
                            /* these two ranges weren't already in order, so merge them into the cache */
                            MergeInto(array, A1, B1, compare, &cache[0]);
                        } else {
                            /* if A1, B1, A2, and B2 are all in order, skip doing anything else */
                            if (!compare(array[B2.Start], array[A2.End - 1]) && !compare(array[A2.Start], array[B1.End - 1]))
                                continue;

                            /* copy A1 and B1 into the cache in the same order */
                            memcpy(&cache[0], &array[A1.Start], Range_GetLength(A1) * sizeof(array[0]));
                            memcpy(&cache[Range_GetLength(A1)], &array[B1.Start], Range_GetLength(B1) * sizeof(array[0]));
                        }
                        A1 = Range_Init(A1.Start, B1.End);

                        /* merge A2 and B2 into the cache */
                        if (compare(array[B2.End - 1], array[A2.Start])) {
                            /* the two ranges are in reverse order, so copy them in reverse order into the cache */
                            memcpy(&cache[Range_GetLength(A1) + Range_GetLength(B2)], &array[A2.Start], Range_GetLength(A2) * sizeof(array[0]));
                            memcpy(&cache[Range_GetLength(A1)], &array[B2.Start], Range_GetLength(B2) * sizeof(array[0]));
                        } else if (compare(array[B2.Start], array[A2.End - 1])) {
                            /* these two ranges weren't already in order, so merge them into the cache */
                            MergeInto(array, A2, B2, compare, &cache[Range_GetLength(A1)]);
                        } else {
                            /* copy A2 and B2 into the cache in the same order */
                            memcpy(&cache[Range_GetLength(A1)], &array[A2.Start], Range_GetLength(A2) * sizeof(array[0]));
                            memcpy(&cache[Range_GetLength(A1) + Range_GetLength(A2)], &array[B2.Start], Range_GetLength(B2) * sizeof(array[0]));
                        }
                        A2 = Range_Init(A2.Start, B2.End);

                        /* merge A1 and A2 from the cache into the array */
                        A3 = Range_Init(0, Range_GetLength(A1));
                        B3 = Range_Init(Range_GetLength(A1), Range_GetLength(A1) + Range_GetLength(A2));

                        if (compare(cache[B3.End - 1], cache[A3.Start])) {
                            /* the two ranges are in reverse order, so copy them in reverse order into the array */
                            memcpy(&array[A1.Start + Range_GetLength(A2)], &cache[A3.Start], Range_GetLength(A3) * sizeof(array[0]));
                            memcpy(&array[A1.Start], &cache[B3.Start], Range_GetLength(B3) * sizeof(array[0]));
                        } else if (compare(cache[B3.Start], cache[A3.End - 1])) {
                            /* these two ranges weren't already in order, so merge them back into the array */
                            MergeInto(cache, A3, B3, compare, &array[A1.Start]);
                        } else {
                            /* copy A3 and B3 into the array in the same order */
                            memcpy(&array[A1.Start], &cache[A3.Start], Range_GetLength(A3) * sizeof(array[0]));
                            memcpy(&array[A1.Start + Range_GetLength(A1)], &cache[B3.Start], Range_GetLength(B3) * sizeof(array[0]));
                        }
                    }

                    /* we merged two levels at the same time, so we're done with this level already */
                    /* (iterator.nextLevel() is called again at the bottom of this outer merge loop) */
                    WikiIterator_nextLevel(&iterator);

                } else {
                    WikiIterator_begin(&iterator);
                    while (!WikiIterator_finished(&iterator)) {
                        PlatformIO_Range A = WikiIterator_nextRange(&iterator);
                        PlatformIO_Range B = WikiIterator_nextRange(&iterator);

                        if (compare(array[B.End - 1], array[A.Start])) {
                            /* the two ranges are in reverse order, so a simple rotation should fix it */
                            Rotate(array, Range_GetLength(A), Range_Init(A.Start, B.End), cache, cache_size);
                        } else if (compare(array[B.Start], array[A.End - 1])) {
                            /* these two ranges weren't already in order, so we'll need to merge them! */
                            memcpy(&cache[0], &array[A.Start], Range_GetLength(A) * sizeof(array[0]));
                            MergeExternal(array, A, B, compare, cache);
                        }
                    }
                }
            } else {
                /* this is where the in-place merge logic starts!
                 1. pull out two internal buffers each containing √A unique values
                 1a. adjust block_size and buffer_size if we couldn't find enough unique values
                 2. loop over the A and B subarrays within this level of the merge sort
                 3. break A and B into blocks of Size 'block_size'
                 4. "tag" each of the A blocks with values from the first internal buffer
                 5. roll the A blocks through the B blocks and drop/rotate them where they belong
                 6. merge each A block with any B values that follow, using the cache or the second internal buffer
                 7. sort the second internal buffer if it exists
                 8. redistribute the two internal buffers back into the array */

                size_t block_size  = sqrt(WikiIterator_length(&iterator));
                size_t buffer_size = WikiIterator_length(&iterator) / block_size + 1;

                /* as an optimization, we really only need to pull out the internal buffers once for each level of merges */
                /* after that we can reuse the same buffers over and over, then redistribute it when we're finished with this level */
                PlatformIO_Range buffer1, buffer2, A, B;
                bool             find_separately;
                size_t           Index, last, count, find, Start, pull_index = 0;
                struct {
                    size_t           from, to, count;
                    PlatformIO_Range range;
                } pull[2];
                pull[0].from = pull[0].to = pull[0].count = 0;
                pull[0].range                             = Range_Init(0, 0);
                pull[1].from = pull[1].to = pull[1].count = 0;
                pull[1].range                             = Range_Init(0, 0);

                buffer1 = Range_Init(0, 0);
                buffer2 = Range_Init(0, 0);

                /* find two internal buffers of Size 'buffer_size' each */
                find            = buffer_size + buffer_size;
                find_separately = false;

                if (block_size <= cache_size) {
                    /* if every A block fits into the cache then we won't need the second internal buffer, */
                    /* so we really only need to find 'buffer_size' unique values */
                    find = buffer_size;
                } else if (find > WikiIterator_length(&iterator)) {
                    /* we can't fit both buffers into the same A or B subarray, so find two buffers separately */
                    find            = buffer_size;
                    find_separately = true;
                }

                /* we need to find either a single contiguous space containing 2√A unique values (which will be split up into two buffers of Size √A each), */
                /* or we need to find one buffer of < 2√A unique values, and a second buffer of √A unique values, */
                /* OR if we couldn't find that many unique values, we need the largest possible buffer we can get */

                /* in the case where it couldn't find a single buffer of at least √A unique values, */
                /* all of the Merge steps must be replaced by a different merge algorithm (MergeInPlace) */
                WikiIterator_begin(&iterator);
                while (!WikiIterator_finished(&iterator)) {
                    A = WikiIterator_nextRange(&iterator);
                    B = WikiIterator_nextRange(&iterator);

                    /* just store information about where the values will be pulled from and to, */
                    /* as well as how many values there are, to create the two internal buffers */
#define PULL(_to)                                        \
pull[pull_index].range = Range_Init(A.Start, B.End); \
pull[pull_index].count = count;                      \
pull[pull_index].from  = Index;                      \
pull[pull_index].to    = _to

                    /* check A for the number of unique values we need to fill an internal buffer */
                    /* these values will be pulled out to the Start of A */
                    for (last = A.Start, count = 1; count < find; last = Index, count++) {
                        Index = FindLastForward(array, array[last], Range_Init(last + 1, A.End), compare, find - count);
                        if (Index == A.End)
                            break;
                    }
                    Index = last;

                    if (count >= buffer_size) {
                        /* keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer */
                        PULL(A.Start);
                        pull_index = 1;

                        if (count == buffer_size + buffer_size) {
                            /* we were able to find a single contiguous section containing 2√A unique values, */
                            /* so this section can be used to contain both of the internal buffers we'll need */
                            buffer1 = Range_Init(A.Start, A.Start + buffer_size);
                            buffer2 = Range_Init(A.Start + buffer_size, A.Start + count);
                            break;
                        } else if (find == buffer_size + buffer_size) {
                            /* we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values, */
                            /* so we still need to find a second separate buffer of at least √A unique values */
                            buffer1 = Range_Init(A.Start, A.Start + count);
                            find    = buffer_size;
                        } else if (block_size <= cache_size) {
                            /* we found the first and only internal buffer that we need, so we're done! */
                            buffer1 = Range_Init(A.Start, A.Start + count);
                            break;
                        } else if (find_separately) {
                            /* found one buffer, but now find the other one */
                            buffer1         = Range_Init(A.Start, A.Start + count);
                            find_separately = false;
                        } else {
                            /* we found a second buffer in an 'A' subarray containing √A unique values, so we're done! */
                            buffer2 = Range_Init(A.Start, A.Start + count);
                            break;
                        }
                    } else if (pull_index == 0 && count > Range_GetLength(buffer1)) {
                        /* keep track of the largest buffer we were able to find */
                        buffer1 = Range_Init(A.Start, A.Start + count);
                        PULL(A.Start);
                    }

                    /* check B for the number of unique values we need to fill an internal buffer */
                    /* these values will be pulled out to the End of B */
                    for (last = B.End - 1, count = 1; count < find; last = Index - 1, count++) {
                        Index = FindFirstBackward(array, array[last], Range_Init(B.Start, last), compare, find - count);
                        if (Index == B.Start)
                            break;
                    }
                    Index = last;

                    if (count >= buffer_size) {
                        /* keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer */
                        PULL(B.End);
                        pull_index = 1;

                        if (count == buffer_size + buffer_size) {
                            /* we were able to find a single contiguous section containing 2√A unique values, */
                            /* so this section can be used to contain both of the internal buffers we'll need */
                            buffer1 = Range_Init(B.End - count, B.End - buffer_size);
                            buffer2 = Range_Init(B.End - buffer_size, B.End);
                            break;
                        } else if (find == buffer_size + buffer_size) {
                            /* we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values, */
                            /* so we still need to find a second separate buffer of at least √A unique values */
                            buffer1 = Range_Init(B.End - count, B.End);
                            find    = buffer_size;
                        } else if (block_size <= cache_size) {
                            /* we found the first and only internal buffer that we need, so we're done! */
                            buffer1 = Range_Init(B.End - count, B.End);
                            break;
                        } else if (find_separately) {
                            /* found one buffer, but now find the other one */
                            buffer1         = Range_Init(B.End - count, B.End);
                            find_separately = false;
                        } else {
                            /* buffer2 will be pulled out from a 'B' subarray, so if the first buffer was pulled out from the corresponding 'A' subarray, */
                            /* we need to adjust the End point for that A subarray so it knows to stop redistributing its values before reaching buffer2 */
                            if (pull[0].range.Start == A.Start)
                                pull[0].range.End -= pull[1].count;

                            /* we found a second buffer in an 'B' subarray containing √A unique values, so we're done! */
                            buffer2 = Range_Init(B.End - count, B.End);
                            break;
                        }
                    } else if (pull_index == 0 && count > Range_GetLength(buffer1)) {
                        /* keep track of the largest buffer we were able to find */
                        buffer1 = Range_Init(B.End - count, B.End);
                        PULL(B.End);
                    }
                }

                /* pull out the two ranges so we can use them as internal buffers */
                for (pull_index = 0; pull_index < 2; pull_index++) {
                    PlatformIO_Range range;
                    size_t           length = pull[pull_index].count;

                    if (pull[pull_index].to < pull[pull_index].from) {
                        /* we're pulling the values out to the left, which means the Start of an A subarray */
                        Index = pull[pull_index].from;
                        for (count = 1; count < length; count++) {
                            Index = FindFirstBackward(array, array[Index - 1], Range_Init(pull[pull_index].to, pull[pull_index].from - (count - 1)), compare, length - count);
                            range = Range_Init(Index + 1, pull[pull_index].from + 1);
                            Rotate(array, Range_GetLength(range) - count, range, cache, cache_size);
                            pull[pull_index].from = Index + count;
                        }
                    } else if (pull[pull_index].to > pull[pull_index].from) {
                        /* we're pulling values out to the right, which means the End of a B subarray */
                        Index = pull[pull_index].from + 1;
                        for (count = 1; count < length; count++) {
                            Index = FindLastForward(array, array[Index], Range_Init(Index, pull[pull_index].to), compare, length - count);
                            range = Range_Init(pull[pull_index].from, Index - 1);
                            Rotate(array, count, range, cache, cache_size);
                            pull[pull_index].from = Index - 1 - count;
                        }
                    }
                }

                /* adjust block_size and buffer_size based on the values we were able to pull out */
                buffer_size = Range_GetLength(buffer1);
                block_size  = WikiIterator_length(&iterator) / buffer_size + 1;

                /* the first buffer NEEDS to be large enough to tag each of the evenly sized A blocks, */
                /* so this was originally here to Pair the math for adjusting block_size above */
                /* assert((WikiIterator_length(&iterator) + 1)/block_size <= buffer_size); */

                /* now that the two internal buffers have been created, it's time to merge each A+B combination at this level of the merge sort! */
                WikiIterator_begin(&iterator);
                while (!WikiIterator_finished(&iterator)) {
                    A = WikiIterator_nextRange(&iterator);
                    B = WikiIterator_nextRange(&iterator);

                    /* remove any parts of A or B that are being used by the internal buffers */
                    Start = A.Start;
                    if (Start == pull[0].range.Start) {
                        if (pull[0].from > pull[0].to) {
                            A.Start += pull[0].count;

                            /* if the internal buffer takes up the entire A or B subarray, then there's nothing to merge */
                            /* this only happens for very small subarrays, like √4 = 2, 2 * (2 internal buffers) = 4, */
                            /* which also only happens when cache_size is small or 0 since it'd otherwise use MergeExternal */
                            if (Range_GetLength(A) == 0)
                                continue;
                        } else if (pull[0].from < pull[0].to) {
                            B.End -= pull[0].count;
                            if (Range_GetLength(B) == 0)
                                continue;
                        }
                    }
                    if (Start == pull[1].range.Start) {
                        if (pull[1].from > pull[1].to) {
                            A.Start += pull[1].count;
                            if (Range_GetLength(A) == 0)
                                continue;
                        } else if (pull[1].from < pull[1].to) {
                            B.End -= pull[1].count;
                            if (Range_GetLength(B) == 0)
                                continue;
                        }
                    }

                    if (compare(array[B.End - 1], array[A.Start])) {
                        /* the two ranges are in reverse order, so a simple rotation should fix it */
                        Rotate(array, Range_GetLength(A), Range_Init(A.Start, B.End), cache, cache_size);
                    } else if (compare(array[A.End], array[A.End - 1])) {
                        /* these two ranges weren't already in order, so we'll need to merge them! */
                        PlatformIO_Range blockA, firstA, lastA, lastB, blockB;
                        size_t           indexA, findA;

                        /* break the remainder of A into blocks. firstA is the uneven-sized first A block */
                        blockA = Range_Init(A.Start, A.End);
                        firstA = Range_Init(A.Start, A.Start + Range_GetLength(blockA) % block_size);

                        /* SortIO_Swap the first Value of each A block with the Value in buffer1 */
                        for (indexA = buffer1.Start, Index = firstA.End; Index < blockA.End; indexA++, Index += block_size)
                            SortIO_Swap(array[indexA], array[Index]);

                        /* Start rolling the A blocks through the B blocks! */
                        /* whenever we leave an A block behind, we'll need to merge the previous A block with any B blocks that follow it, so track that information as well */
                        lastA  = firstA;
                        lastB  = Range_Init(0, 0);
                        blockB = Range_Init(B.Start, B.Start + Minimum(block_size, Range_GetLength(B)));
                        blockA.Start += Range_GetLength(firstA);
                        indexA = buffer1.Start;

                        /* if the first unevenly sized A block fits into the cache, copy it there for when we go to Merge it */
                        /* otherwise, if the second buffer is available, block SortIO_Swap the contents into that */
                        if (Range_GetLength(lastA) <= cache_size)
                            memcpy(&cache[0], &array[lastA.Start], Range_GetLength(lastA) * sizeof(array[0]));
                        else if (Range_GetLength(buffer2) > 0)
                            BlockSwap(array, lastA.Start, buffer2.Start, Range_GetLength(lastA));

                        if (Range_GetLength(blockA) > 0) {
                            while (true) {
                                /* if there's a previous B block and the first Value of the minimum A block is <= the last Value of the previous B block, */
                                /* then drop that minimum A block behind. or if there are no B blocks left then keep dropping the remaining A blocks. */
                                if ((Range_GetLength(lastB) > 0 && !compare(array[lastB.End - 1], array[indexA])) || Range_GetLength(blockB) == 0) {
                                    /* figure out where to split the previous B block, and rotate it at the split */
                                    size_t B_split     = BinaryFirst(array, array[indexA], lastB, compare);
                                    size_t B_remaining = lastB.End - B_split;

                                    /* SortIO_Swap the minimum A block to the beginning of the rolling A blocks */
                                    size_t minA = blockA.Start;
                                    for (findA = minA + block_size; findA < blockA.End; findA += block_size)
                                        if (compare(array[findA], array[minA]))
                                            minA = findA;
                                    BlockSwap(array, blockA.Start, minA, block_size);

                                    /* SortIO_Swap the first item of the previous A block back with its original Value, which is stored in buffer1 */
                                    SortIO_Swap(array[blockA.Start], array[indexA]);
                                    indexA++;

                                    /*
                                     locally merge the previous A block with the B values that follow it
                                     if lastA fits into the external cache we'll use that (with MergeExternal),
                                     or if the second internal buffer exists we'll use that (with MergeInternal),
                                     or failing that we'll use a strictly in-place merge algorithm (MergeInPlace)
                                     */
                                    if (Range_GetLength(lastA) <= cache_size)
                                        MergeExternal(array, lastA, Range_Init(lastA.End, B_split), compare, cache);
                                    else if (Range_GetLength(buffer2) > 0)
                                        MergeInternal(array, lastA, Range_Init(lastA.End, B_split), compare, buffer2);
                                    else
                                        MergeInPlace(array, lastA, Range_Init(lastA.End, B_split), compare, cache, cache_size);

                                    if (Range_GetLength(buffer2) > 0 || block_size <= cache_size) {
                                        /* copy the previous A block into the cache or buffer2, since that's where we need it to be when we go to merge it anyway */
                                        if (block_size <= cache_size)
                                            memcpy(&cache[0], &array[blockA.Start], block_size * sizeof(array[0]));
                                        else
                                            BlockSwap(array, blockA.Start, buffer2.Start, block_size);

                                        /* this is equivalent to rotating, but faster */
                                        /* the area normally taken up by the A block is either the contents of buffer2, or data we don't need anymore since we memcopied it */
                                        /* either way, we don't need to retain the order of those items, so instead of rotating we can just block SortIO_Swap B to where it belongs */
                                        BlockSwap(array, B_split, blockA.Start + block_size - B_remaining, B_remaining);
                                    } else {
                                        /* we are unable to use the 'buffer2' trick to speed up the rotation operation since buffer2 doesn't exist, so perform a normal rotation */
                                        Rotate(array, blockA.Start - B_split, Range_Init(B_split, blockA.Start + block_size), cache, cache_size);
                                    }

                                    /* update the range for the remaining A blocks, and the range remaining from the B block after it was split */
                                    lastA = Range_Init(blockA.Start - B_remaining, blockA.Start - B_remaining + block_size);
                                    lastB = Range_Init(lastA.End, lastA.End + B_remaining);

                                    /* if there are no more A blocks remaining, this step is finished! */
                                    blockA.Start += block_size;
                                    if (Range_GetLength(blockA) == 0)
                                        break;

                                } else if (Range_GetLength(blockB) < block_size) {
                                    /* move the last B block, which is unevenly sized, to before the remaining A blocks, by using a rotation */
                                    /* the cache is disabled here since it might contain the contents of the previous A block */
                                    Rotate(array, blockB.Start - blockA.Start, Range_Init(blockA.Start, blockB.End), cache, 0);

                                    lastB = Range_Init(blockA.Start, blockA.Start + Range_GetLength(blockB));
                                    blockA.Start += Range_GetLength(blockB);
                                    blockA.End += Range_GetLength(blockB);
                                    blockB.End = blockB.Start;
                                } else {
                                    /* roll the leftmost A block to the End by swapping it with the next B block */
                                    BlockSwap(array, blockA.Start, blockB.Start, block_size);
                                    lastB = Range_Init(blockA.Start, blockA.Start + block_size);

                                    blockA.Start += block_size;
                                    blockA.End += block_size;
                                    blockB.Start += block_size;

                                    if (blockB.End > B.End - block_size)
                                        blockB.End = B.End;
                                    else
                                        blockB.End += block_size;
                                }
                            }
                        }

                        /* merge the last A block with the remaining B values */
                        if (Range_GetLength(lastA) <= cache_size)
                            MergeExternal(array, lastA, Range_Init(lastA.End, B.End), compare, cache);
                        else if (Range_GetLength(buffer2) > 0)
                            MergeInternal(array, lastA, Range_Init(lastA.End, B.End), compare, buffer2);
                        else
                            MergeInPlace(array, lastA, Range_Init(lastA.End, B.End), compare, cache, cache_size);
                    }
                }

                /* when we're finished with this merge step we should have the one or two internal buffers left over, where the second buffer is all jumbled up */
                /* insertion sort the second buffer, then redistribute the buffers back into the array using the opposite process used for creating the buffer */

                /* while an unstable sort like quicksort could be applied here, in benchmarks it was consistently slightly slower than a simple insertion sort, */
                /* even for tens of millions of items. this may be because insertion sort is quite fast when the data is already somewhat sorted, like it is here */
                InsertionSort(array, buffer2, compare);

                for (pull_index = 0; pull_index < 2; pull_index++) {
                    size_t amount, unique = pull[pull_index].count * 2;
                    if (pull[pull_index].from > pull[pull_index].to) {
                        /* the values were pulled out to the left, so redistribute them back to the right */
                        PlatformIO_Range buffer = Range_Init(pull[pull_index].range.Start, pull[pull_index].range.Start + pull[pull_index].count);
                        while (Range_GetLength(buffer) > 0) {
                            Index = FindFirstForward(array, array[buffer.Start], Range_Init(buffer.End, pull[pull_index].range.End), compare, unique);
                            amount = Index - buffer.End;
                            Rotate(array, Range_GetLength(buffer), Range_Init(buffer.Start, Index), cache, cache_size);
                            buffer.Start += (amount + 1);
                            buffer.End += amount;
                            unique -= 2;
                        }
                    } else if (pull[pull_index].from < pull[pull_index].to) {
                        /* the values were pulled out to the right, so redistribute them back to the left */
                        PlatformIO_Range buffer = Range_Init(pull[pull_index].range.End - pull[pull_index].count, pull[pull_index].range.End);
                        while (Range_GetLength(buffer) > 0) {
                            Index = FindLastBackward(array, array[buffer.End - 1], Range_Init(pull[pull_index].range.Start, buffer.Start), compare, unique);
                            amount = buffer.Start - Index;
                            Rotate(array, amount, Range_Init(Index, buffer.End), cache, cache_size);
                            buffer.Start -= amount;
                            buffer.End -= (amount + 1);
                            unique -= 2;
                        }
                    }
                }
            }

            /* double the Size of each A and B subarray that will be merged in the next level */
            if (!WikiIterator_nextLevel(&iterator))
                break;
        }

#if DYNAMIC_CACHE
        if (cache)
            free(cache);
#endif

#undef CACHE_SIZE
    }











































    /* Unorganized code below */

    /* verify that WikiSort is actually correct */
    /* (this also reduces performance slightly) */
#define VERIFY true

    /* simulate comparisons that have a bit more overhead than just an inlined (int < int) */
    /* (so we can tell whether reducing the number of comparisons was worth the added complexity) */
#define SLOW_COMPARISONS false

    /* whether to give WikiSort a full-Size cache, to see how it performs when given more memory */
#define DYNAMIC_CACHE true

#define Allocate(type, count) (type *) malloc((count) * sizeof(type))

#if SLOW_COMPARISONS
#define NOOP_SIZE 50
    size_t noop1[NOOP_SIZE], noop2[NOOP_SIZE];
#endif

    /* find the Index of the first Value within the range that is equal to array[Index] */
    static size_t BinaryFirst(const Pair array[], const Pair Value, const PlatformIO_Range range, const CompareFunction compare) {
        size_t Start = range.Start, End = range.End - 1;
        if (range.Start >= range.End) {
            return range.Start;
        }
        while (Start < End) {
            size_t mid = Start + (End - Start) / 2;
            if (compare(array[mid], Value)) {
                Start = mid + 1;
            } else {
                End = mid;
            }
        }
        if (Start == range.End - 1 && compare(array[Start], Value)) {
            Start++;
        }
        return Start;
    }

    /* find the Index of the last Value within the range that is equal to array[Index], plus 1 */
    static size_t BinaryLast(const Pair array[], const Pair Value, const PlatformIO_Range range, const CompareFunction compare) {
        size_t Start = range.Start, End = range.End - 1;
        if (range.Start >= range.End) {
            return range.End;
        }
        while (Start < End) {
            size_t mid = Start + (End - Start) / 2;
            if (!compare(Value, array[mid])) {
                Start = mid + 1;
            } else {
                End = mid;
            }
        }
        if (Start == range.End - 1 && !compare(Value, array[Start])) {
            Start++;
        }
        return Start;
    }

    /* combine a linear search with a binary search to reduce the number of comparisons in situations */
    /* where have some idea as to how many unique values there are and where the next Value might be */
    static size_t FindFirstForward(const Pair array[], const Pair Value, const PlatformIO_Range range, const CompareFunction compare, const size_t unique) {
        size_t skip, Index;
        if (Range_GetLength(range) == 0) {
            return range.Start;
        }
        skip = Maximum(Range_GetLength(range) / unique, 1);

        for (Index = range.Start + skip; compare(array[Index - 1], Value); Index += skip)
            if (Index >= range.End - skip)
                return BinaryFirst(array, Value, Range_Init(Index, range.End), compare);

        return BinaryFirst(array, Value, Range_Init(Index - skip, Index), compare);
    }

    static size_t FindLastForward(const Pair array[], const Pair Value, const PlatformIO_Range range, const CompareFunction compare, const size_t unique) {
        size_t skip, Index;
        if (Range_GetLength(range) == 0)
            return range.Start;
        skip = Maximum(Range_GetLength(range) / unique, 1);

        for (Index = range.Start + skip; !compare(Value, array[Index - 1]); Index += skip)
            if (Index >= range.End - skip)
                return BinaryLast(array, Value, Range_Init(Index, range.End), compare);

        return BinaryLast(array, Value, Range_Init(Index - skip, Index), compare);
    }

    static size_t FindFirstBackward(const Pair array[], const Pair Value, const PlatformIO_Range range, const CompareFunction compare, const size_t unique) {
        size_t skip, Index;
        if (Range_GetLength(range) == 0)
            return range.Start;
        skip = Maximum(Range_GetLength(range) / unique, 1);

        for (Index = range.End - skip; Index > range.Start && !compare(array[Index - 1], Value); Index -= skip)
            if (Index < range.Start + skip)
                return BinaryFirst(array, Value, Range_Init(range.Start, Index), compare);

        return BinaryFirst(array, Value, Range_Init(Index, Index + skip), compare);
    }

    static size_t FindLastBackward(const Pair array[], const Pair Value, const PlatformIO_Range range, const CompareFunction compare, const size_t unique) {
        size_t skip, Index;
        if (Range_GetLength(range) == 0)
            return range.Start;
        skip = Maximum(Range_GetLength(range) / unique, 1);

        for (Index = range.End - skip; Index > range.Start && compare(Value, array[Index - 1]); Index -= skip)
            if (Index < range.Start + skip)
                return BinaryLast(array, Value, Range_Init(range.Start, Index), compare);

        return BinaryLast(array, Value, Range_Init(Index, Index + skip), compare);
    }

    /* n^2 sorting algorithm used to sort tiny chunks of the full array */
    static void InsertionSort(Pair array[], const PlatformIO_Range range, const CompareFunction compare) {
        size_t i, j;
        for (i = range.Start + 1; i < range.End; i++) {
            const Pair temp = array[i];
            for (j = i; j > range.Start && compare(temp, array[j - 1]); j--)
                array[j] = array[j - 1];
            array[j] = temp;
        }
    }

    /* reverse a range of values within the array */
    static void Reverse(Pair array[], const PlatformIO_Range range) {
        for (size_t Index = Range_GetLength(range) / 2; Index > 0; Index--)
            SortIO_Swap(array[range.Start + Index - 1], array[range.End - Index]);
    }

    /* SortIO_Swap a series of values in the array */
    static void BlockSwap(Pair array[], const size_t start1, const size_t start2, const size_t block_size) {
        size_t Index;
        for (Index = 0; Index < block_size; Index++)
            SortIO_Swap(array[start1 + Index], array[start2 + Index]);
    }

    /* rotate the values in an array ([0 1 2 3] becomes [1 2 3 0] if we rotate by 1) */
    /* this assumes that 0 <= amount <= range.length() */
    void Rotate(Pair array[], const size_t amount, const PlatformIO_Range range, Pair cache[], const size_t cache_size) {
        size_t split;
        PlatformIO_Range range1, range2;
        if (Range_GetLength(range) == 0)
            return;

        split  = range.Start + amount;
        range1 = Range_Init(range.Start, split);
        range2 = Range_Init(split, range.End);

        /* if the smaller of the two ranges fits into the cache, it's *slightly* faster copying it there and shifting the elements over */
        if (Range_GetLength(range1) <= Range_GetLength(range2)) {
            if (Range_GetLength(range1) <= cache_size) {
                memcpy(&cache[0], &array[range1.Start], Range_GetLength(range1) * sizeof(array[0]));
                memmove(&array[range1.Start], &array[range2.Start], Range_GetLength(range2) * sizeof(array[0]));
                memcpy(&array[range1.Start + Range_GetLength(range2)], &cache[0], Range_GetLength(range1) * sizeof(array[0]));
                return;
            }
        } else {
            if (Range_GetLength(range2) <= cache_size) {
                memcpy(&cache[0], &array[range2.Start], Range_GetLength(range2) * sizeof(array[0]));
                memmove(&array[range2.End - Range_GetLength(range1)], &array[range1.Start], Range_GetLength(range1) * sizeof(array[0]));
                memcpy(&array[range1.Start], &cache[0], Range_GetLength(range2) * sizeof(array[0]));
                return;
            }
        }

        Reverse(array, range1);
        Reverse(array, range2);
        Reverse(array, range);
    }

    /* calculate how to scale the Index Value to the range within the array */
    /* the bottom-up merge sort only operates on values that are powers of two, */
    /* so scale down to that power of two, then use a fraction to scale back again */

    /* merge two ranges from one array and save the results into a different array */
    void MergeInto(Pair from[], const PlatformIO_Range A, const PlatformIO_Range B, const CompareFunction compare, Pair into[]) {
        Pair *A_index = &from[A.Start], *B_index = &from[B.Start];
        Pair *A_last = &from[A.End], *B_last = &from[B.End];
        Pair *insert_index = &into[0];

        while (true) {
            if (!compare(*B_index, *A_index)) {
                *insert_index = *A_index;
                A_index++;
                insert_index++;
                if (A_index == A_last) {
                    /* copy the remainder of B into the final array */
                    memcpy(insert_index, B_index, (B_last - B_index) * sizeof(from[0]));
                    break;
                }
            } else {
                *insert_index = *B_index;
                B_index++;
                insert_index++;
                if (B_index == B_last) {
                    /* copy the remainder of A into the final array */
                    memcpy(insert_index, A_index, (A_last - A_index) * sizeof(from[0]));
                    break;
                }
            }
        }
    }

    /* merge operation using an external buffer, */
    void MergeExternal(Pair array[], const PlatformIO_Range A, const PlatformIO_Range B, const CompareFunction compare, Pair cache[]) {
        /* A fits into the cache, so use that instead of the internal buffer */
        Pair *A_index      = &cache[0];
        Pair *B_index      = &array[B.Start];
        Pair *insert_index = &array[A.Start];
        Pair *A_last       = &cache[Range_GetLength(A)];
        Pair *B_last       = &array[B.End];

        if (Range_GetLength(B) > 0 && Range_GetLength(A) > 0) {
            while (true) {
                if (!compare(*B_index, *A_index)) {
                    *insert_index = *A_index;
                    A_index++;
                    insert_index++;
                    if (A_index == A_last)
                        break;
                } else {
                    *insert_index = *B_index;
                    B_index++;
                    insert_index++;
                    if (B_index == B_last)
                        break;
                }
            }
        }

        /* copy the remainder of A into the final array */
        memcpy(insert_index, A_index, (A_last - A_index) * sizeof(array[0]));
    }

    /* merge operation using an internal buffer */
    void MergeInternal(Pair array[], const PlatformIO_Range A, const PlatformIO_Range B, const CompareFunction compare, const PlatformIO_Range buffer) {
        /* whenever we find a Value to add to the final array, SortIO_Swap it with the Value that's already in that spot */
        /* when this algorithm is finished, 'buffer' will contain its original contents, but in a different order */
        size_t A_count = 0, B_count = 0, insert = 0;

        if (Range_GetLength(B) > 0 && Range_GetLength(A) > 0) {
            while (true) {
                if (!compare(array[B.Start + B_count], array[buffer.Start + A_count])) {
                    SortIO_Swap(array[A.Start + insert], array[buffer.Start + A_count]);
                    A_count++;
                    insert++;
                    if (A_count >= Range_GetLength(A))
                        break;
                } else {
                    SortIO_Swap(array[A.Start + insert], array[B.Start + B_count]);
                    B_count++;
                    insert++;
                    if (B_count >= Range_GetLength(B))
                        break;
                }
            }
        }

        /* SortIO_Swap the remainder of A into the final array */
        BlockSwap(array, buffer.Start + A_count, A.Start + insert, Range_GetLength(A) - A_count);
    }

    /* merge operation without a buffer */
    void MergeInPlace(Pair array[], PlatformIO_Range A, PlatformIO_Range B, const CompareFunction compare, Pair cache[], const size_t cache_size) {
        if (Range_GetLength(A) == 0 || Range_GetLength(B) == 0)
            return;

        /*
         this just repeatedly binary searches into B and rotates A into position.
         the paper suggests using the 'rotation-based Hwang and Lin algorithm' here,
         but I decided to stick with this because it had better situational performance

         (Hwang and Lin is designed for merging subarrays of very different sizes,
         but WikiSort almost always uses subarrays that are roughly the same Size)

         normally this is incredibly suboptimal, but this function is only called
         when none of the A or B blocks in any subarray contained 2√A unique values,
         which places a hard limit on the number of times this will ACTUALLY need
         to binary search and rotate.

         according to my analysis the worst case is √A rotations performed on √A items
         once the constant factors are removed, which ends up being O(n)

         again, this is NOT a general-purpose solution – it only works well in this case!
         kind of like how the O(n^2) insertion sort is used in some places
         */

        while (true) {
            /* find the first place in B where the first item in A needs to be inserted */
            size_t mid = BinaryFirst(array, array[A.Start], B, compare);

            /* rotate A into place */
            size_t amount = mid - A.End;
            Rotate(array, Range_GetLength(A), Range_Init(A.Start, mid), cache, cache_size);
            if (B.End == mid)
                break;

            /* calculate the new A and B ranges */
            B.Start = mid;
            A       = Range_Init(A.Start + amount, B.Start);
            A.Start = BinaryLast(array, array[A.Start], A, compare);
            if (Range_GetLength(A) == 0)
                break;
        }
    }

    /* bottom-up merge sort combined with an in-place merge algorithm for O(1) memory use */
    void WikiSort(Pair array[], const size_t Size, const CompareFunction compare) {
        /* use a small cache to speed up some of the operations */
#if DYNAMIC_CACHE
        size_t cache_size;
        Pair  *cache = NULL;
#else
        /* since the cache Size is fixed, it's still O(1) memory! */
        /* just keep in mind that making it too small ruins the point (nothing will fit into it), */
        /* and making it too large also ruins the point (so much for "low memory"!) */
        /* removing the cache entirely still gives 70% of the performance of a standard merge */
#define CACHE_SIZE 512
        const size_t cache_size = CACHE_SIZE;
        Pair         cache[CACHE_SIZE];
#endif

        WikiIterator iterator;

        /* if the array is of Size 0, 1, 2, or 3, just sort them like so: */
        if (Size < 4) {
            if (Size == 3) {
                /* hard-coded insertion sort */
                if (compare(array[1], array[0]))
                    SortIO_Swap(array[0], array[1]);
                if (compare(array[2], array[1])) {
                    SortIO_Swap(array[1], array[2]);
                    if (compare(array[1], array[0]))
                        SortIO_Swap(array[0], array[1]);
                }
            } else if (Size == 2) {
                /* SortIO_Swap the items if they're out of order */
                if (compare(array[1], array[0]))
                    SortIO_Swap(array[0], array[1]);
            }

            return;
        }

        /* sort groups of 4-8 items at a time using an unstable sorting network, */
        /* but keep track of the original item orders to force it to be stable */
        /* http://pages.ripco.net/~jgamble/nw.html */
        iterator = WikiIterator_Init(Size, 4);
        WikiIterator_begin(&iterator);
        while (!WikiIterator_finished(&iterator)) {
            uint8_t order[] = {0, 1, 2, 3, 4, 5, 6, 7};
            PlatformIO_Range   range   = WikiIterator_nextRange(&iterator);

#define SortIO_Swap(x, y)                                                                                                                                          \
if (compare(array[range.Start + y], array[range.Start + x]) || (order[x] > order[y] && !compare(array[range.Start + x], array[range.Start + y]))) { \
SortIO_Swap(array[range.Start + x], array[range.Start + y]);                                                                                           \
SortIO_Swap(order[x], order[y]);                                                                                                                       \
}

            if (Range_GetLength(range) == 8) {
                SortIO_Swap(0, 1);
                SortIO_Swap(2, 3);
                SortIO_Swap(4, 5);
                SortIO_Swap(6, 7);
                SortIO_Swap(0, 2);
                SortIO_Swap(1, 3);
                SortIO_Swap(4, 6);
                SortIO_Swap(5, 7);
                SortIO_Swap(1, 2);
                SortIO_Swap(5, 6);
                SortIO_Swap(0, 4);
                SortIO_Swap(3, 7);
                SortIO_Swap(1, 5);
                SortIO_Swap(2, 6);
                SortIO_Swap(1, 4);
                SortIO_Swap(3, 6);
                SortIO_Swap(2, 4);
                SortIO_Swap(3, 5);
                SortIO_Swap(3, 4);

            } else if (Range_GetLength(range) == 7) {
                SortIO_Swap(1, 2);
                SortIO_Swap(3, 4);
                SortIO_Swap(5, 6);
                SortIO_Swap(0, 2);
                SortIO_Swap(3, 5);
                SortIO_Swap(4, 6);
                SortIO_Swap(0, 1);
                SortIO_Swap(4, 5);
                SortIO_Swap(2, 6);
                SortIO_Swap(0, 4);
                SortIO_Swap(1, 5);
                SortIO_Swap(0, 3);
                SortIO_Swap(2, 5);
                SortIO_Swap(1, 3);
                SortIO_Swap(2, 4);
                SortIO_Swap(2, 3);

            } else if (Range_GetLength(range) == 6) {
                SortIO_Swap(1, 2);
                SortIO_Swap(4, 5);
                SortIO_Swap(0, 2);
                SortIO_Swap(3, 5);
                SortIO_Swap(0, 1);
                SortIO_Swap(3, 4);
                SortIO_Swap(2, 5);
                SortIO_Swap(0, 3);
                SortIO_Swap(1, 4);
                SortIO_Swap(2, 4);
                SortIO_Swap(1, 3);
                SortIO_Swap(2, 3);

            } else if (Range_GetLength(range) == 5) {
                SortIO_Swap(0, 1);
                SortIO_Swap(3, 4);
                SortIO_Swap(2, 4);
                SortIO_Swap(2, 3);
                SortIO_Swap(1, 4);
                SortIO_Swap(0, 3);
                SortIO_Swap(0, 2);
                SortIO_Swap(1, 3);
                SortIO_Swap(1, 2);

            } else if (Range_GetLength(range) == 4) {
                SortIO_Swap(0, 1);
                SortIO_Swap(2, 3);
                SortIO_Swap(0, 2);
                SortIO_Swap(1, 3);
                SortIO_Swap(1, 2);
            }
        }
        if (Size < 8)
            return;

#if DYNAMIC_CACHE
        /* good choices for the cache Size are: */
        /* (Size + 1)/2 – turns into a full-speed standard merge sort since everything fits into the cache */
        cache_size = (Size + 1) / 2;
        cache      = (Pair *) malloc(cache_size * sizeof(array[0]));

        if (!cache) {
            /* sqrt((Size + 1)/2) + 1 – this will be the Size of the A blocks at the largest level of merges, */
            /* so a buffer of this Size would allow it to skip using internal or in-place merges for anything */
            cache_size = sqrt(cache_size) + 1;
            cache      = (Pair *) malloc(cache_size * sizeof(array[0]));

            if (!cache) {
                /* 512 – chosen from careful testing as a good balance between fixed-Size memory use and run time */
                if (cache_size > 512) {
                    cache_size = 512;
                    cache      = (Pair *) malloc(cache_size * sizeof(array[0]));
                }

                /* 0 – if the system simply cannot allocate any extra memory whatsoever, no memory works just fine */
                if (!cache)
                    cache_size = 0;
            }
        }
#endif

        /* then merge sort the higher levels, which can be 8-15, 16-31, 32-63, 64-127, etc. */
        while (true) {
            /* if every A and B block will fit into the cache, use a special branch specifically for merging with the cache */
            /* (we use < rather than <= since the block Size might be one more than iterator.length()) */
            if (WikiIterator_length(&iterator) < cache_size) {
                /* if four subarrays fit into the cache, it's faster to merge both pairs of subarrays into the cache, */
                /* then merge the two merged subarrays from the cache back into the original array */
                if ((WikiIterator_length(&iterator) + 1) * 4 <= cache_size && WikiIterator_length(&iterator) * 4 <= Size) {
                    WikiIterator_begin(&iterator);
                    while (!WikiIterator_finished(&iterator)) {
                        /* merge A1 and B1 into the cache */
                        PlatformIO_Range A1, B1, A2, B2, A3, B3;
                        A1 = WikiIterator_nextRange(&iterator);
                        B1 = WikiIterator_nextRange(&iterator);
                        A2 = WikiIterator_nextRange(&iterator);
                        B2 = WikiIterator_nextRange(&iterator);

                        if (compare(array[B1.End - 1], array[A1.Start])) {
                            /* the two ranges are in reverse order, so copy them in reverse order into the cache */
                            memcpy(&cache[Range_GetLength(B1)], &array[A1.Start], Range_GetLength(A1) * sizeof(array[0]));
                            memcpy(&cache[0], &array[B1.Start], Range_GetLength(B1) * sizeof(array[0]));
                        } else if (compare(array[B1.Start], array[A1.End - 1])) {
                            /* these two ranges weren't already in order, so merge them into the cache */
                            MergeInto(array, A1, B1, compare, &cache[0]);
                        } else {
                            /* if A1, B1, A2, and B2 are all in order, skip doing anything else */
                            if (!compare(array[B2.Start], array[A2.End - 1]) && !compare(array[A2.Start], array[B1.End - 1]))
                                continue;

                            /* copy A1 and B1 into the cache in the same order */
                            memcpy(&cache[0], &array[A1.Start], Range_GetLength(A1) * sizeof(array[0]));
                            memcpy(&cache[Range_GetLength(A1)], &array[B1.Start], Range_GetLength(B1) * sizeof(array[0]));
                        }
                        A1 = Range_Init(A1.Start, B1.End);

                        /* merge A2 and B2 into the cache */
                        if (compare(array[B2.End - 1], array[A2.Start])) {
                            /* the two ranges are in reverse order, so copy them in reverse order into the cache */
                            memcpy(&cache[Range_GetLength(A1) + Range_GetLength(B2)], &array[A2.Start], Range_GetLength(A2) * sizeof(array[0]));
                            memcpy(&cache[Range_GetLength(A1)], &array[B2.Start], Range_GetLength(B2) * sizeof(array[0]));
                        } else if (compare(array[B2.Start], array[A2.End - 1])) {
                            /* these two ranges weren't already in order, so merge them into the cache */
                            MergeInto(array, A2, B2, compare, &cache[Range_GetLength(A1)]);
                        } else {
                            /* copy A2 and B2 into the cache in the same order */
                            memcpy(&cache[Range_GetLength(A1)], &array[A2.Start], Range_GetLength(A2) * sizeof(array[0]));
                            memcpy(&cache[Range_GetLength(A1) + Range_GetLength(A2)], &array[B2.Start], Range_GetLength(B2) * sizeof(array[0]));
                        }
                        A2 = Range_Init(A2.Start, B2.End);

                        /* merge A1 and A2 from the cache into the array */
                        A3 = Range_Init(0, Range_GetLength(A1));
                        B3 = Range_Init(Range_GetLength(A1), Range_GetLength(A1) + Range_GetLength(A2));

                        if (compare(cache[B3.End - 1], cache[A3.Start])) {
                            /* the two ranges are in reverse order, so copy them in reverse order into the array */
                            memcpy(&array[A1.Start + Range_GetLength(A2)], &cache[A3.Start], Range_GetLength(A3) * sizeof(array[0]));
                            memcpy(&array[A1.Start], &cache[B3.Start], Range_GetLength(B3) * sizeof(array[0]));
                        } else if (compare(cache[B3.Start], cache[A3.End - 1])) {
                            /* these two ranges weren't already in order, so merge them back into the array */
                            MergeInto(cache, A3, B3, compare, &array[A1.Start]);
                        } else {
                            /* copy A3 and B3 into the array in the same order */
                            memcpy(&array[A1.Start], &cache[A3.Start], Range_GetLength(A3) * sizeof(array[0]));
                            memcpy(&array[A1.Start + Range_GetLength(A1)], &cache[B3.Start], Range_GetLength(B3) * sizeof(array[0]));
                        }
                    }

                    /* we merged two levels at the same time, so we're done with this level already */
                    /* (iterator.nextLevel() is called again at the bottom of this outer merge loop) */
                    WikiIterator_nextLevel(&iterator);

                } else {
                    WikiIterator_begin(&iterator);
                    while (!WikiIterator_finished(&iterator)) {
                        PlatformIO_Range A = WikiIterator_nextRange(&iterator);
                        PlatformIO_Range B = WikiIterator_nextRange(&iterator);

                        if (compare(array[B.End - 1], array[A.Start])) {
                            /* the two ranges are in reverse order, so a simple rotation should fix it */
                            Rotate(array, Range_GetLength(A), Range_Init(A.Start, B.End), cache, cache_size);
                        } else if (compare(array[B.Start], array[A.End - 1])) {
                            /* these two ranges weren't already in order, so we'll need to merge them! */
                            memcpy(&cache[0], &array[A.Start], Range_GetLength(A) * sizeof(array[0]));
                            MergeExternal(array, A, B, compare, cache);
                        }
                    }
                }
            } else {
                /* this is where the in-place merge logic starts!
                 1. pull out two internal buffers each containing √A unique values
                 1a. adjust block_size and buffer_size if we couldn't find enough unique values
                 2. loop over the A and B subarrays within this level of the merge sort
                 3. break A and B into blocks of Size 'block_size'
                 4. "tag" each of the A blocks with values from the first internal buffer
                 5. roll the A blocks through the B blocks and drop/rotate them where they belong
                 6. merge each A block with any B values that follow, using the cache or the second internal buffer
                 7. sort the second internal buffer if it exists
                 8. redistribute the two internal buffers back into the array */

                size_t block_size  = sqrt(WikiIterator_length(&iterator));
                size_t buffer_size = WikiIterator_length(&iterator) / block_size + 1;

                /* as an optimization, we really only need to pull out the internal buffers once for each level of merges */
                /* after that we can reuse the same buffers over and over, then redistribute it when we're finished with this level */
                PlatformIO_Range  buffer1, buffer2, A, B;
                bool   find_separately;
                size_t Index, last, count, find, Start, pull_index = 0;
                struct {
                    size_t from, to, count;
                    PlatformIO_Range  range;
                } pull[2];
                pull[0].from = pull[0].to = pull[0].count = 0;
                pull[0].range                             = Range_Init(0, 0);
                pull[1].from = pull[1].to = pull[1].count = 0;
                pull[1].range                             = Range_Init(0, 0);

                buffer1 = Range_Init(0, 0);
                buffer2 = Range_Init(0, 0);

                /* find two internal buffers of Size 'buffer_size' each */
                find            = buffer_size + buffer_size;
                find_separately = false;

                if (block_size <= cache_size) {
                    /* if every A block fits into the cache then we won't need the second internal buffer, */
                    /* so we really only need to find 'buffer_size' unique values */
                    find = buffer_size;
                } else if (find > WikiIterator_length(&iterator)) {
                    /* we can't fit both buffers into the same A or B subarray, so find two buffers separately */
                    find            = buffer_size;
                    find_separately = true;
                }

                /* we need to find either a single contiguous space containing 2√A unique values (which will be split up into two buffers of Size √A each), */
                /* or we need to find one buffer of < 2√A unique values, and a second buffer of √A unique values, */
                /* OR if we couldn't find that many unique values, we need the largest possible buffer we can get */

                /* in the case where it couldn't find a single buffer of at least √A unique values, */
                /* all of the Merge steps must be replaced by a different merge algorithm (MergeInPlace) */
                WikiIterator_begin(&iterator);
                while (!WikiIterator_finished(&iterator)) {
                    A = WikiIterator_nextRange(&iterator);
                    B = WikiIterator_nextRange(&iterator);

                    /* just store information about where the values will be pulled from and to, */
                    /* as well as how many values there are, to create the two internal buffers */
#define PULL(_to)                                       \
pull[pull_index].range = Range_Init(A.Start, B.End); \
pull[pull_index].count = count;                     \
pull[pull_index].from  = Index;                     \
pull[pull_index].to    = _to

                    /* check A for the number of unique values we need to fill an internal buffer */
                    /* these values will be pulled out to the Start of A */
                    for (last = A.Start, count = 1; count < find; last = Index, count++) {
                        Index = FindLastForward(array, array[last], Range_Init(last + 1, A.End), compare, find - count);
                        if (Index == A.End)
                            break;
                    }
                    Index = last;

                    if (count >= buffer_size) {
                        /* keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer */
                        PULL(A.Start);
                        pull_index = 1;

                        if (count == buffer_size + buffer_size) {
                            /* we were able to find a single contiguous section containing 2√A unique values, */
                            /* so this section can be used to contain both of the internal buffers we'll need */
                            buffer1 = Range_Init(A.Start, A.Start + buffer_size);
                            buffer2 = Range_Init(A.Start + buffer_size, A.Start + count);
                            break;
                        } else if (find == buffer_size + buffer_size) {
                            /* we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values, */
                            /* so we still need to find a second separate buffer of at least √A unique values */
                            buffer1 = Range_Init(A.Start, A.Start + count);
                            find    = buffer_size;
                        } else if (block_size <= cache_size) {
                            /* we found the first and only internal buffer that we need, so we're done! */
                            buffer1 = Range_Init(A.Start, A.Start + count);
                            break;
                        } else if (find_separately) {
                            /* found one buffer, but now find the other one */
                            buffer1         = Range_Init(A.Start, A.Start + count);
                            find_separately = false;
                        } else {
                            /* we found a second buffer in an 'A' subarray containing √A unique values, so we're done! */
                            buffer2 = Range_Init(A.Start, A.Start + count);
                            break;
                        }
                    } else if (pull_index == 0 && count > Range_GetLength(buffer1)) {
                        /* keep track of the largest buffer we were able to find */
                        buffer1 = Range_Init(A.Start, A.Start + count);
                        PULL(A.Start);
                    }

                    /* check B for the number of unique values we need to fill an internal buffer */
                    /* these values will be pulled out to the End of B */
                    for (last = B.End - 1, count = 1; count < find; last = Index - 1, count++) {
                        Index = FindFirstBackward(array, array[last], Range_Init(B.Start, last), compare, find - count);
                        if (Index == B.Start)
                            break;
                    }
                    Index = last;

                    if (count >= buffer_size) {
                        /* keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer */
                        PULL(B.End);
                        pull_index = 1;

                        if (count == buffer_size + buffer_size) {
                            /* we were able to find a single contiguous section containing 2√A unique values, */
                            /* so this section can be used to contain both of the internal buffers we'll need */
                            buffer1 = Range_Init(B.End - count, B.End - buffer_size);
                            buffer2 = Range_Init(B.End - buffer_size, B.End);
                            break;
                        } else if (find == buffer_size + buffer_size) {
                            /* we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values, */
                            /* so we still need to find a second separate buffer of at least √A unique values */
                            buffer1 = Range_Init(B.End - count, B.End);
                            find    = buffer_size;
                        } else if (block_size <= cache_size) {
                            /* we found the first and only internal buffer that we need, so we're done! */
                            buffer1 = Range_Init(B.End - count, B.End);
                            break;
                        } else if (find_separately) {
                            /* found one buffer, but now find the other one */
                            buffer1         = Range_Init(B.End - count, B.End);
                            find_separately = false;
                        } else {
                            /* buffer2 will be pulled out from a 'B' subarray, so if the first buffer was pulled out from the corresponding 'A' subarray, */
                            /* we need to adjust the End point for that A subarray so it knows to stop redistributing its values before reaching buffer2 */
                            if (pull[0].range.Start == A.Start)
                                pull[0].range.End -= pull[1].count;

                            /* we found a second buffer in an 'B' subarray containing √A unique values, so we're done! */
                            buffer2 = Range_Init(B.End - count, B.End);
                            break;
                        }
                    } else if (pull_index == 0 && count > Range_GetLength(buffer1)) {
                        /* keep track of the largest buffer we were able to find */
                        buffer1 = Range_Init(B.End - count, B.End);
                        PULL(B.End);
                    }
                }

                /* pull out the two ranges so we can use them as internal buffers */
                for (pull_index = 0; pull_index < 2; pull_index++) {
                    PlatformIO_Range  range;
                    size_t length = pull[pull_index].count;

                    if (pull[pull_index].to < pull[pull_index].from) {
                        /* we're pulling the values out to the left, which means the Start of an A subarray */
                        Index = pull[pull_index].from;
                        for (count = 1; count < length; count++) {
                            Index = FindFirstBackward(array, array[Index - 1], Range_Init(pull[pull_index].to, pull[pull_index].from - (count - 1)), compare, length - count);
                            range = Range_Init(Index + 1, pull[pull_index].from + 1);
                            Rotate(array, Range_GetLength(range) - count, range, cache, cache_size);
                            pull[pull_index].from = Index + count;
                        }
                    } else if (pull[pull_index].to > pull[pull_index].from) {
                        /* we're pulling values out to the right, which means the End of a B subarray */
                        Index = pull[pull_index].from + 1;
                        for (count = 1; count < length; count++) {
                            Index = FindLastForward(array, array[Index], Range_Init(Index, pull[pull_index].to), compare, length - count);
                            range = Range_Init(pull[pull_index].from, Index - 1);
                            Rotate(array, count, range, cache, cache_size);
                            pull[pull_index].from = Index - 1 - count;
                        }
                    }
                }

                /* adjust block_size and buffer_size based on the values we were able to pull out */
                buffer_size = Range_GetLength(buffer1);
                block_size  = WikiIterator_length(&iterator) / buffer_size + 1;

                /* the first buffer NEEDS to be large enough to tag each of the evenly sized A blocks, */
                /* so this was originally here to Pair the math for adjusting block_size above */
                /* assert((WikiIterator_length(&iterator) + 1)/block_size <= buffer_size); */

                /* now that the two internal buffers have been created, it's time to merge each A+B combination at this level of the merge sort! */
                WikiIterator_begin(&iterator);
                while (!WikiIterator_finished(&iterator)) {
                    A = WikiIterator_nextRange(&iterator);
                    B = WikiIterator_nextRange(&iterator);

                    /* remove any parts of A or B that are being used by the internal buffers */
                    Start = A.Start;
                    if (Start == pull[0].range.Start) {
                        if (pull[0].from > pull[0].to) {
                            A.Start += pull[0].count;

                            /* if the internal buffer takes up the entire A or B subarray, then there's nothing to merge */
                            /* this only happens for very small subarrays, like √4 = 2, 2 * (2 internal buffers) = 4, */
                            /* which also only happens when cache_size is small or 0 since it'd otherwise use MergeExternal */
                            if (Range_GetLength(A) == 0)
                                continue;
                        } else if (pull[0].from < pull[0].to) {
                            B.End -= pull[0].count;
                            if (Range_GetLength(B) == 0)
                                continue;
                        }
                    }
                    if (Start == pull[1].range.Start) {
                        if (pull[1].from > pull[1].to) {
                            A.Start += pull[1].count;
                            if (Range_GetLength(A) == 0)
                                continue;
                        } else if (pull[1].from < pull[1].to) {
                            B.End -= pull[1].count;
                            if (Range_GetLength(B) == 0)
                                continue;
                        }
                    }

                    if (compare(array[B.End - 1], array[A.Start])) {
                        /* the two ranges are in reverse order, so a simple rotation should fix it */
                        Rotate(array, Range_GetLength(A), Range_Init(A.Start, B.End), cache, cache_size);
                    } else if (compare(array[A.End], array[A.End - 1])) {
                        /* these two ranges weren't already in order, so we'll need to merge them! */
                        PlatformIO_Range  blockA, firstA, lastA, lastB, blockB;
                        size_t indexA, findA;

                        /* break the remainder of A into blocks. firstA is the uneven-sized first A block */
                        blockA = Range_Init(A.Start, A.End);
                        firstA = Range_Init(A.Start, A.Start + Range_GetLength(blockA) % block_size);

                        /* SortIO_Swap the first Value of each A block with the Value in buffer1 */
                        for (indexA = buffer1.Start, Index = firstA.End; Index < blockA.End; indexA++, Index += block_size)
                            SortIO_Swap(array[indexA], array[Index]);

                        /* Start rolling the A blocks through the B blocks! */
                        /* whenever we leave an A block behind, we'll need to merge the previous A block with any B blocks that follow it, so track that information as well */
                        lastA  = firstA;
                        lastB  = Range_Init(0, 0);
                        blockB = Range_Init(B.Start, B.Start + Minimum(block_size, Range_GetLength(B)));
                        blockA.Start += Range_GetLength(firstA);
                        indexA = buffer1.Start;

                        /* if the first unevenly sized A block fits into the cache, copy it there for when we go to Merge it */
                        /* otherwise, if the second buffer is available, block SortIO_Swap the contents into that */
                        if (Range_GetLength(lastA) <= cache_size)
                            memcpy(&cache[0], &array[lastA.Start], Range_GetLength(lastA) * sizeof(array[0]));
                        else if (Range_GetLength(buffer2) > 0)
                            BlockSwap(array, lastA.Start, buffer2.Start, Range_GetLength(lastA));

                        if (Range_GetLength(blockA) > 0) {
                            while (true) {
                                /* if there's a previous B block and the first Value of the minimum A block is <= the last Value of the previous B block, */
                                /* then drop that minimum A block behind. or if there are no B blocks left then keep dropping the remaining A blocks. */
                                if ((Range_GetLength(lastB) > 0 && !compare(array[lastB.End - 1], array[indexA])) || Range_GetLength(blockB) == 0) {
                                    /* figure out where to split the previous B block, and rotate it at the split */
                                    size_t B_split     = BinaryFirst(array, array[indexA], lastB, compare);
                                    size_t B_remaining = lastB.End - B_split;

                                    /* SortIO_Swap the minimum A block to the beginning of the rolling A blocks */
                                    size_t minA = blockA.Start;
                                    for (findA = minA + block_size; findA < blockA.End; findA += block_size)
                                        if (compare(array[findA], array[minA]))
                                            minA = findA;
                                    BlockSwap(array, blockA.Start, minA, block_size);

                                    /* SortIO_Swap the first item of the previous A block back with its original Value, which is stored in buffer1 */
                                    SortIO_Swap(array[blockA.Start], array[indexA]);
                                    indexA++;

                                    /*
                                     locally merge the previous A block with the B values that follow it
                                     if lastA fits into the external cache we'll use that (with MergeExternal),
                                     or if the second internal buffer exists we'll use that (with MergeInternal),
                                     or failing that we'll use a strictly in-place merge algorithm (MergeInPlace)
                                     */
                                    if (Range_GetLength(lastA) <= cache_size)
                                        MergeExternal(array, lastA, Range_Init(lastA.End, B_split), compare, cache);
                                    else if (Range_GetLength(buffer2) > 0)
                                        MergeInternal(array, lastA, Range_Init(lastA.End, B_split), compare, buffer2);
                                    else
                                        MergeInPlace(array, lastA, Range_Init(lastA.End, B_split), compare, cache, cache_size);

                                    if (Range_GetLength(buffer2) > 0 || block_size <= cache_size) {
                                        /* copy the previous A block into the cache or buffer2, since that's where we need it to be when we go to merge it anyway */
                                        if (block_size <= cache_size)
                                            memcpy(&cache[0], &array[blockA.Start], block_size * sizeof(array[0]));
                                        else
                                            BlockSwap(array, blockA.Start, buffer2.Start, block_size);

                                        /* this is equivalent to rotating, but faster */
                                        /* the area normally taken up by the A block is either the contents of buffer2, or data we don't need anymore since we memcopied it */
                                        /* either way, we don't need to retain the order of those items, so instead of rotating we can just block SortIO_Swap B to where it belongs */
                                        BlockSwap(array, B_split, blockA.Start + block_size - B_remaining, B_remaining);
                                    } else {
                                        /* we are unable to use the 'buffer2' trick to speed up the rotation operation since buffer2 doesn't exist, so perform a normal rotation */
                                        Rotate(array, blockA.Start - B_split, Range_Init(B_split, blockA.Start + block_size), cache, cache_size);
                                    }

                                    /* update the range for the remaining A blocks, and the range remaining from the B block after it was split */
                                    lastA = Range_Init(blockA.Start - B_remaining, blockA.Start - B_remaining + block_size);
                                    lastB = Range_Init(lastA.End, lastA.End + B_remaining);

                                    /* if there are no more A blocks remaining, this step is finished! */
                                    blockA.Start += block_size;
                                    if (Range_GetLength(blockA) == 0)
                                        break;

                                } else if (Range_GetLength(blockB) < block_size) {
                                    /* move the last B block, which is unevenly sized, to before the remaining A blocks, by using a rotation */
                                    /* the cache is disabled here since it might contain the contents of the previous A block */
                                    Rotate(array, blockB.Start - blockA.Start, Range_Init(blockA.Start, blockB.End), cache, 0);

                                    lastB = Range_Init(blockA.Start, blockA.Start + Range_GetLength(blockB));
                                    blockA.Start += Range_GetLength(blockB);
                                    blockA.End += Range_GetLength(blockB);
                                    blockB.End = blockB.Start;
                                } else {
                                    /* roll the leftmost A block to the End by swapping it with the next B block */
                                    BlockSwap(array, blockA.Start, blockB.Start, block_size);
                                    lastB = Range_Init(blockA.Start, blockA.Start + block_size);

                                    blockA.Start += block_size;
                                    blockA.End += block_size;
                                    blockB.Start += block_size;

                                    if (blockB.End > B.End - block_size)
                                        blockB.End = B.End;
                                    else
                                        blockB.End += block_size;
                                }
                            }
                        }

                        /* merge the last A block with the remaining B values */
                        if (Range_GetLength(lastA) <= cache_size)
                            MergeExternal(array, lastA, Range_Init(lastA.End, B.End), compare, cache);
                        else if (Range_GetLength(buffer2) > 0)
                            MergeInternal(array, lastA, Range_Init(lastA.End, B.End), compare, buffer2);
                        else
                            MergeInPlace(array, lastA, Range_Init(lastA.End, B.End), compare, cache, cache_size);
                    }
                }

                /* when we're finished with this merge step we should have the one or two internal buffers left over, where the second buffer is all jumbled up */
                /* insertion sort the second buffer, then redistribute the buffers back into the array using the opposite process used for creating the buffer */

                /* while an unstable sort like quicksort could be applied here, in benchmarks it was consistently slightly slower than a simple insertion sort, */
                /* even for tens of millions of items. this may be because insertion sort is quite fast when the data is already somewhat sorted, like it is here */
                InsertionSort(array, buffer2, compare);

                for (pull_index = 0; pull_index < 2; pull_index++) {
                    size_t amount, unique = pull[pull_index].count * 2;
                    if (pull[pull_index].from > pull[pull_index].to) {
                        /* the values were pulled out to the left, so redistribute them back to the right */
                        PlatformIO_Range buffer = Range_Init(pull[pull_index].range.Start, pull[pull_index].range.Start + pull[pull_index].count);
                        while (Range_GetLength(buffer) > 0) {
                            Index = FindFirstForward(array, array[buffer.Start], Range_Init(buffer.End, pull[pull_index].range.End), compare, unique);
                            amount = Index - buffer.End;
                            Rotate(array, Range_GetLength(buffer), Range_Init(buffer.Start, Index), cache, cache_size);
                            buffer.Start += (amount + 1);
                            buffer.End += amount;
                            unique -= 2;
                        }
                    } else if (pull[pull_index].from < pull[pull_index].to) {
                        /* the values were pulled out to the right, so redistribute them back to the left */
                        PlatformIO_Range buffer = Range_Init(pull[pull_index].range.End - pull[pull_index].count, pull[pull_index].range.End);
                        while (Range_GetLength(buffer) > 0) {
                            Index = FindLastBackward(array, array[buffer.End - 1], Range_Init(pull[pull_index].range.Start, buffer.Start), compare, unique);
                            amount = buffer.Start - Index;
                            Rotate(array, amount, Range_Init(Index, buffer.End), cache, cache_size);
                            buffer.Start -= amount;
                            buffer.End -= (amount + 1);
                            unique -= 2;
                        }
                    }
                }
            }

            /* double the Size of each A and B subarray that will be merged in the next level */
            if (!WikiIterator_nextLevel(&iterator))
                break;
        }

#if DYNAMIC_CACHE
        if (cache)
            free(cache);
#endif

#undef CACHE_SIZE
    }

    /* standard merge sort, so we have a baseline for how well WikiSort works */
    void MergeSortR(Pair array[], const PlatformIO_Range range, const CompareFunction compare, Pair buffer[]) {
        size_t mid, A_count = 0, B_count = 0, insert = 0;
        PlatformIO_Range  A, B;

        if (Range_GetLength(range) < 32) {
            InsertionSort(array, range, compare);
            return;
        }

        mid = range.Start + (range.End - range.Start) / 2;
        A   = Range_Init(range.Start, mid);
        B   = Range_Init(mid, range.End);

        MergeSortR(array, A, compare, buffer);
        MergeSortR(array, B, compare, buffer);

        /* standard merge operation here (only A is copied to the buffer, and only the parts that weren't already where they should be) */
        A = Range_Init(BinaryLast(array, array[B.Start], A, compare), A.End);
        memcpy(&buffer[0], &array[A.Start], Range_GetLength(A) * sizeof(array[0]));
        while (A_count < Range_GetLength(A) && B_count < Range_GetLength(B)) {
            if (!compare(array[A.End + B_count], buffer[A_count])) {
                array[A.Start + insert] = buffer[A_count];
                A_count++;
            } else {
                array[A.Start + insert] = array[A.End + B_count];
                B_count++;
            }
            insert++;
        }

        memcpy(&array[A.Start + insert], &buffer[A_count], (Range_GetLength(A) - A_count) * sizeof(array[0]));
    }

    void MergeSort(Pair array[], const size_t array_count, const CompareFunction compare) {
        PlatformIO_Generic(buffer, Allocate(Pair, (array_count + 1) / 2));
        MergeSortR(array, Range_Init(0, array_count), compare, buffer);
        free(buffer);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
