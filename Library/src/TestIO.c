#include "../include/TestIO.h"          /* Included for our declarations */
#include "../include/CryptographyIO.h"  /* Included for SecureRNG */
#include "../include/TextIO/LogIO.h"    /* Included for error reporting */
#include "../include/TextIO/StringIO.h" /* Included for UTFX_Init functions */
#include "../include/TextIO/FormatIO.h" /* Included for UTF8_Format */

#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) == PlatformIO_TargetOSIsApple)
#include <time.h>                       /* Included for timespec_get */
#include <mach/mach_time.h>             /* Included for mach_continuous_time */
#elif ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#include <time.h>                       /* Included for timespec_get */
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#include <WinBase.h>                    /* Included for QueryPerformanceCounter */
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












/*
    static void RunTests_ReallocateHelper(TestSuite *Suite) {
        if (Suite != NULL) {
            realloc(Suite->UnexpectedFailues, sizeof(uint64_t) * Suite->UnexpectedFailureSize * 2);
            Suite->UnexpectedFailureSize *= 2;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("TestSuite Pointer is NULL"));
        }
    }
 */

    void TestIO_RunTests(TestSuite *Suite) {
        if (Suite != NULL) {
            uint64_t NumEnabledTests               = 0ULL;
            for (uint64_t Test = 0; Test < Suite->NumTests; Test++) {
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
            UTF8 *EnabledDisabledTests         = UTF8_Format(UTF8String("Enabled/Disabled Tests: %llu/%llu"), NumEnabledTests, Suite->NumTests - NumEnabledTests);
            UTF8 *TestResults                  = UTF8_Format(UTF8String("Correct/Incorrect Outcome: %llu/%llu"), Suite->NumCorrectOutcomes, Suite->NumTests - Suite->NumCompleted);
            UTF8_File_WriteString(stdout, EnabledDisabledTests);
            UTF8_File_WriteString(stdout, TestResults);
            UTF8_File_WriteString(stdout, UTF8String("Incorrect Tests:\n"));
            for (uint64_t Test = 0; Test < Suite->NumTests; Test++) {
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
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) == PlatformIO_TargetOSIsApple)
        struct timespec Resolution;
        clock_getres(CLOCK_MONOTONIC, &Resolution);
        TimerFrequency = Resolution.tv_nsec;
#elif ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        struct timespec Resolution;
        clock_getres(CLOCK_MONOTONIC, &Resolution);
        TimerFrequency = Resolution.tv_nsec;
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
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
