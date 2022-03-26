#include "../include/TestIO.h"          /* Included for our declarations */

#include "../include/AssertIO.h"        /* Included for Assertions */
#include "../include/CryptographyIO.h"  /* Included for InsecurePRNG */
#include "../include/TextIO/FormatIO.h" /* Included for UTF8_Format */
#include "../include/TextIO/LogIO.h"    /* Included for error reporting */
#include "../include/TextIO/StringIO.h" /* Included for UTFX_Init functions */

#if PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#include <time.h>                       /* Included for timespec_get */
#endif

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
#include <mach/mach_time.h>             /* Included for mach_continuous_time */
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#include <WinBase.h>                    /* Included for QueryPerformanceCounter */
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    const TestIO_Case TestIO_NULLCase = {
        .Name        = NULL,
        .Function    = NULL,
        .State       = TestState_Unspecified,
        .Expectation = Outcome_Unspecified,
    };

    const TestIO_Suite TestIO_NULLSuite = {
        .Name     = NULL,
        .Init     = NULL,
        .Deinit   = NULL,
        .NumTests = 0,
        .Tests    = &TestIO_NULLCase,
    };

    /*
     Memory Layout of TestIO Section:

     --------------------------------
     TestIOSectionMarker
     ?NumSuites?
     Suite[0] = {
     .Name     = SuiteName,
     .Init     = <FunctionPointer>,
     .Deinit   = <FunctionPointer>,
     .NumTests = <size_t> 3,
     .Tests[]  = {
     [0] = {
     .Name       = TestName,
     .Function   = <FunctionPointer>,
     .State      = <TestState>,
     .Expetation = <TestOutcome>,
     .Result     = <TestOutcome>,
     },
     [1] = {
     .Name       = TestName,
     .Function   = <FunctionPointer>,
     .State      = <TestState>,
     .Expetation = <TestOutcome>,
     .Result     = <TestOutcome>,
     },
     [2] = {
     .Name       = TestName,
     .Function   = <FunctionPointer>,
     .State      = <TestState>,
     .Expetation = <TestOutcome>,
     .Result     = <TestOutcome>,
     },
     },
     }
     Suite[1] = {

     }
     Suite[...] = {

     }
     TestIOSectionMarker
     --------------------------------
     */

    extern const int *__start_foo;

    extern PlatformIO_ArraySet(TestIO_Suite) __TestIO_Suites_Start = NULLSuite;
    extern PlatformIO_ArraySet(TestIO_Suite) __TestIO_Suites_Stop  = NULLSuite;

    /*
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


    void TestIO_Run(void) {
        size_t NumSuites = 0;
        for (TestIO_Suite *Suite = __TestIO_Suites_Start; Suite < __TestIO_Suites_Stop; Suite++) {
            // Ok, so we have the actual test suite.
            /*
             Ok, so we're going to make an ArraySet of TestIO_Suites, aka a series of TestIO_Suite followed by a NULL
             */
        }
        //while (SuiteStart != Suites_End) {
        Section += sizeof(TestIO_Suite);
        size_t NumCases = 0;
        Section += NumCases * sizeof(TestIO_Case);
        NumSuites += 1;
        //}

        TestIO_Suite **Suites = calloc(NumSuites, sizeof(TestIO_Suite));
        InsecurePRNG *Insecure                         = InsecurePRNG_Init(0);
        size_t NumEnabledTests                         = 0;
        for (size_t Suite = 0; Suite < NumSuites; Suite++) {
            for (size_t Test = 0; Test < Suites[Suite]->NumTests; Test++) {
                if (Suites[Suite]->Tests[Test].State == TestState_Enabled) {
                    NumEnabledTests                   += 1;
                    Suites[Suite]->Tests[Test].Result  = Suites[Suite]->Tests[Test].Function(Insecure);
                }
            }
        }
    }
    */
    static ImmutableStringSet_UTF8 UTF8_TestIO_GetFunctionNamesAsStringSet(TestIO_Suite *Suite) {
        ImmutableStringSet_UTF8 StringSet = NULL;
        if (Suite != NULL) {
            size_t NumEnabledTests               = 0ULL;
            for (size_t Test = 0; Test < Suite->NumTests; Test++) {
                if (Suite->Tests[Test].State == TestState_Enabled) {
                    NumEnabledTests               += 1;
                    bool Outcome                   = Suite->Tests[Test].Function;
                    if (Outcome == Suite->Tests[Test].Expectation) {
                        Suite->NumCorrectOutcomes += 1; // Not good enough, we need to know which tests passed and which failed; gotta add a field to TestCase
                    }
                }
            }
            /*
             Pass/Fail is not the correct abstraction:

             If a test was supposed to fail and it does, the test actually passed...

             Sooo how do I word this?

             Correct/Incorrect outcome seems good
             */
            UTF8 *EnabledDisabledTests         = UTF8_Format(UTF8String("Enabled/Disabled Tests: %zu/%zu"), NumEnabledTests, Suite->NumTests - NumEnabledTests);
            UTF8 *TestResults                  = UTF8_Format(UTF8String("Correct/Incorrect Outcome: %zu/%zu"), Suite->NumCorrectOutcomes, Suite->NumTests - Suite->NumCompleted);
            UTF8_File_WriteString(stdout, EnabledDisabledTests);
            UTF8_File_WriteString(stdout, TestResults);
            UTF8_File_WriteString(stdout, UTF8String("Incorrect Tests:\n"));
            for (size_t Test = 0; Test < Suite->NumTests; Test++) {
                if (Suite->Tests[Test].State == TestState_Enabled && Suite->Tests[Test].Expectation != Suite->Tests[Test].Outcome) {
                    UTF8_File_WriteString(stdout, Suite->Tests[Test].Name);
                }
            }
        }
    }

#ifdef TestIO_RegisterCase
#undef TestIO_RegisterCase
#endif /* Undefine TestIO_RegisterCase as a macro so the real function can be defined, we do this so it can only be called through TestIO_Register */

    bool TestIO_RegisterCase(TestSuite *Suite, UTF8 *FunctionName, TestIO_TestFunction Function2Test, TestIO_TestStates State, TestIO_TestOutcomes Expectation) {
        TestCase Case = {
            .Name         = FunctionName,
            .Function     = Function2Test,
            .State        = State,
            .Expectation  = Expectation,
        };
        Suite->Tests[Suite->NumTests] = Case;
        Suite->NumTests += 1;
        return true;
    }

    uint64_t GetTimerFrequency(void) {
        uint64_t TimerFrequency = 0LL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
        struct timespec Resolution;
        clock_getres(CLOCK_MONOTONIC, &Resolution);
        TimerFrequency = Resolution.tv_nsec;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        struct timespec Resolution;
        clock_getres(CLOCK_MONOTONIC, &Resolution);
        TimerFrequency = Resolution.tv_nsec;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        LARGE_INTEGER WinFrequency;
        bool Success = QueryPerformanceFrequency(&WinFrequency);
        if (Success) {
            TimerFrequency = WinFrequency.QuadPart;
        }
#endif
        return TimerFrequency;
    }

    uint64_t GetTime_Elapsed(void) {
        uint64_t Time        = 0ULL;
        uint64_t CurrentTime = 0ULL;
        for (uint8_t Loop = 1; Loop <= 3; Loop++) {
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
            CurrentTime      = mach_continuous_time();
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsLinux)
            struct timespec *TimeSpec = NULL;
            clock_gettime(CLOCK_MONOTONIC, TimeSpec);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
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

    static UTF32 UTF32_GenerateCodePoint(InsecurePRNG *Insecure) {
        AssertIO(Insecure != NULL);
        UTF32 CodePoint          = 0UL;
        CodePoint            = (UTF32) InsecurePRNG_CreateInteger(Insecure, UTF16HighSurrogateStart - 1);
        /*
         UTF32  CodePointHigh = (UTF32) InsecurePRNG_CreateIntegerInRange(Insecure, 1, 0xD7FF);
         UTF32  CodePointLow  = (UTF32) InsecurePRNG_CreateIntegerInRange(Insecure, 0xE000, 0x10FFFF);
         CodePoint            = CodePointLow | CodePointHigh;
         */
        return CodePoint;
    }

    UTF32 *UTF32_GenerateString(InsecurePRNG *Insecure, size_t NumCodePoints) {
        AssertIO(Insecure != NULL);
        AssertIO(NumCodePoints > 0);
        UTF32 *String                 = 0UL;
        String                    = UTF32_Init(NumCodePoints);
        AssertIO(String != NULL);
        for (size_t CodePoint = 0ULL; CodePoint < NumCodePoints; CodePoint++) {
            String[CodePoint] = UTF32_GenerateCodePoint(Insecure);
        }
        return String;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
