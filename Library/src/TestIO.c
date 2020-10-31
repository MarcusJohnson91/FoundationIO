#include "../include/TestIO.h"          /* Included for our declarations */
#include "../include/CryptographyIO.h"  /* Included for SecureRNG */
#include "../include/TextIO/LogIO.h"    /* Included for error reporting */
#include "../include/TextIO/StringIO.h" /* Included for UTFX_Init functions */
#include "../include/TextIO/FormatIO.h" /* Included for UTF8_Format */

#if   (((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX) && ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) != PlatformIO_TargetOSIsApple))
#include <time.h>                       /* Included for timespec_get */
#elif (((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX) && ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) == PlatformIO_TargetOSIsApple))
#include <mach/mach_time.h>             /* Included for mach_continuous_time */
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#include <Windows.h>
#include <WinBase.h>                    /* Included for QueryPerformanceCounter, Windows.h MUST be included first */
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Automatic Test Generation:

     Instrument the functions to see when and where branching occurs and to refine the randomly generated data

     */

    /*
     Automatic test registration:

     Have the preprocessor somehow loop over all the test functions in the file and find the ones with TestState_Enabled set?

     Our logger will include the name of the function when an error or test is message is sent, so if a test passes or fails or whatever it'll tell us the name of the function so we don't need Name.

     really all we need to know is the number of enabled tests, the number of disabled tests, and the number of tests that passed.

     Test Enabled/Disabled of Total passed

     Design.

     So, we need a way to have there be various comparison functions, and a way to set the expected result, and it needs to be runtime checking not compile time so no macros except make a _Generic comparison function.

     We need a very high resolution timer so we can compare performance.

     we might also want to have a good randomnes generator (maybe even Unicode)
     */

    static void RunTests_ReallocateHelper(TestSuite *Suite) {
        if (Suite != NULL) {
            uint64_t *OLD_Failures = Suite->UnexpectedFailues;
            uint64_t *New_Failures = realloc(Suite->UnexpectedFailues, sizeof(uint64_t) * Suite->UnexpectedFailureSize * 2);
            if (New_Failures != NULL) {
                Suite->UnexpectedFailues      = New_Failures;
                Suite->UnexpectedFailureSize *= 2;
                free(OLD_Failures);
            } else {
                free(New_Failures);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("TestSuite Pointer is NULL"));
        }
    }

    void TestIO_RunTests(TestSuite *Suite) {
        if (Suite != NULL) {
            for (uint64_t Test = 0; Test < Suite->NumTests; Test++) {
                if (Suite->Tests[Test].TestState == TestState_Enabled) {
                    bool TestPassed                                             = Suite->Tests[Test].Function;
                    if (TestPassed == Suite->Tests[Test].TestOutcome) {
                        Suite->NumWorkedAsExpected                             += 1;
                    } else {
                        if (Suite->NumUnexpectedFailures == Suite->UnexpectedFailureSize) {
                            RunTests_ReallocateHelper(Suite);
                        }

                        Suite->NumUnexpectedFailures                           += 1;
                        Suite->UnexpectedFailues[Suite->NumUnexpectedFailures]  = Test;
                    }
                }
            }
            UTF8 *TestsThatPerformedAsExpected = UTF8_Format(UTF8String("Tests that matched their expectation: %llu"), Suite->NumWorkedAsExpected);
            UTF8 *UnexpectedFailures           = UTF8_Format(UTF8String("Tests that DID NOT perform as expected: %llu"), Suite->NumUnexpectedFailures);
            UTF8_WriteSentence(stdout, UTF8String("Test Results:\n"));
            UTF8_WriteSentence(stdout, TestsThatPerformedAsExpected);
            UTF8_WriteSentence(stdout, UnexpectedFailures);
            for (uint64_t FailedTest = 0ULL; FailedTest < Suite->NumUnexpectedFailures; FailedTest++) {
                //UTF8_WriteSentence(stdout, Suite->Tests[Suite->UnexpectedFailues[FailedTest]].FunctionName);
            }
        }
    }

    uint64_t GetTimerFrequency(void) {
        uint64_t TimerFrequency = 0LL;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        struct timespec Resolution;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsLinux) == PlatformIO_TargetOSIsLinux)
        clock_getres(CLOCK_MONOTONIC, &Resolution);
#else
        clock_getres(CLOCK_REALTIME, &Resolution);
#endif /* PlatformIO_TargetOSIsLinux */
        TimerFrequency = Resolution.tv_nsec;
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        LARGE_INTEGER WinFrequency;
        bool Success = QueryPerformanceFrequency(&WinFrequency);
        if (Success) {
            TimerFrequency = WinFrequency.QuadPart;
        }
#endif /* PlatformIO_TargetOS */
        return TimerFrequency;
    }

    uint64_t GetTime_Elapsed(void) {
        uint64_t Time        = 0ULL;
        uint64_t CurrentTime = 0ULL;
        for (uint8_t Loop = 1; Loop <= 3; Loop++) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) == PlatformIO_TargetOSIsApple)
            CurrentTime      = mach_continuous_time();
#elif (((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX) && ((PlatformIO_TargetOS & PlatformIO_TargetOSIsLinux) == PlatformIO_TargetOSIsLinux))
            struct timespec *TimeSpec = NULL;
            clock_gettime(CLOCK_MONOTONIC, TimeSpec);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            LARGE_INTEGER WinCounter;
            bool Success    = QueryPerformanceCounter(&WinCounter);
            if (Success) {
                CurrentTime = WinCounter.QuadPart;
            }
#endif
            Time            += CurrentTime;
            Time            /= Loop;
        }
        return Time;
    }

    static UTF32 UTF32_GenerateCodePoint(SecureRNG *Random) {
        UTF32 CodePoint          = 0UL;
        if (Random != NULL) {
            UTF32  CodePointHigh = (UTF32) SecureRNG_GenerateIntegerInRange(Random, 1, 0xD7FF);
            UTF32  CodePointLow  = (UTF32) SecureRNG_GenerateIntegerInRange(Random, 0xE000, 0x10FFFF);
            CodePoint            = CodePointLow | CodePointHigh;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return CodePoint;
    }

    UTF32 *UTF32_GenerateString(SecureRNG *Random, uint64_t NumCodePoints) {
        UTF32 *String                 = 0UL;
        if (Random != NULL) {
            String                    = UTF32_Init(NumCodePoints);
            if (String != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < NumCodePoints; CodePoint++) {
                    String[CodePoint] = UTF32_GenerateCodePoint(Random);
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate string with %llu CodePoints"), NumCodePoints);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        }
        return String;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
