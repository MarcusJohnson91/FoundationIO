#include "../../include/TestIO.h"                 /* Included for testing */
#include "../../include/PlatfomIO.h"              /* Included for our declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    int main(const int argc, const char *argv[]) {
        bool   TestSuitePassed      = true;
        size_t       total, Index;
        double       total_time, total_time1, total_time2;
        const size_t max_size = 1500000;
        Var(array1, Allocate(Test, max_size));
        Var(array2, Allocate(Test, max_size));
        Comparison compare = TestCompare;

        /* initialize the random-number generator */
        srand(time(NULL));
        /*srand(10141985);*/ /* in case you want the same random numbers */

        total = max_size;

#if !SLOW_COMPARISONS && VERIFY
        printf("running test cases... ");
        fflush(stdout);

        for (test_case = 0; test_case < sizeof(test_cases) / sizeof(test_cases[0]); test_case++) {
            for (Index = 0; Index < total; Index++) {
                Test item;

                item.Value = test_cases[test_case](Index, total);
                item.Index = Index;

                array1[Index] = array2[Index] = item;
            }

            WikiSort(array1, total, compare);

            MergeSort(array2, total, compare);

            WikiVerify(array1, Range_new(0, total), compare, "test case failed");
            for (Index = 0; Index < total; Index++)
                assert(!compare(array1[Index], array2[Index]) && !compare(array2[Index], array1[Index]));
        }
        printf("passed!\n");
#endif

        total_time  = Seconds();
        total_time1 = total_time2 = 0;

        for (total = 0; total < max_size; total += 2048 * 16) {
            double time1, time2;

            for (Index = 0; Index < total; Index++) {
                Test item;

                /* TestingRandom, TestingRandomFew, TestingMostlyDescending, TestingMostlyAscending, */
                /* TestingAscending, TestingDescending, TestingEqual, TestingJittered, TestingMostlyEqual, TestingAppend */
                item.Value = TestingRandom(Index, total);
#if VERIFY
                item.Index = Index;
#endif

                array1[Index] = array2[Index] = item;
            }

            time1 = Seconds();
            WikiSort(array1, total, compare);
            time1 = Seconds() - time1;
            total_time1 += time1;

            time2 = Seconds();
            MergeSort(array2, total, compare);
            time2 = Seconds() - time2;
            total_time2 += time2;

            printf("[%zu]\n", total);

            if (time1 >= time2)
                printf("WikiSort: %f seconds, MergeSort: %f seconds (%f%% as fast)\n", time1, time2, time2 / time1 * 100.0);
            else
                printf("WikiSort: %f seconds, MergeSort: %f seconds (%f%% faster)\n", time1, time2, time2 / time1 * 100.0 - 100.0);

#if VERIFY
            /* make sure the arrays are sorted correctly, and that the results were stable */
            printf("verifying... ");
            fflush(stdout);

            WikiVerify(array1, Range_new(0, total), compare, "testing the final array");
            for (Index = 0; Index < total; Index++)
                assert(!compare(array1[Index], array2[Index]) && !compare(array2[Index], array1[Index]));

            printf("correct!\n");
#endif
        }

        total_time = Seconds() - total_time;
        printf("tests completed in %f seconds\n", total_time);
        if (total_time1 >= total_time2)
            printf("WikiSort: %f seconds, MergeSort: %f seconds (%f%% as fast)\n", total_time1, total_time2, total_time2 / total_time1 * 100.0);
        else
            printf("WikiSort: %f seconds, MergeSort: %f seconds (%f%% faster)\n", total_time1, total_time2, total_time2 / total_time1 * 100.0 - 100.0);

        free(array1);
        free(array2);
        return 0;
        return TestSuitePassed;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
