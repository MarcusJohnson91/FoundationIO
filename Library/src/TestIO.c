#include "../include/TestIO.h"          /* Included for our declarations */
#include "../include/CryptographyIO.h"  /* Included for SecureRNG */
#include "../include/TextIO/LogIO.h"    /* Included for error reporting */
#include "../include/TextIO/StringIO.h" /* Included for UTFX_Init functions */

#if   (((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS) && ((PlatformIO_TargetOS & PlatformIO_AppleOS) != PlatformIO_AppleOS))
#include <time.h>                       /* Included for timespec_get */
#elif (((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS) && ((PlatformIO_TargetOS & PlatformIO_AppleOS) == PlatformIO_AppleOS))
#include <mach/mach_time.h>             /* Included for mach_continuous_time */
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
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
    
    /*
     
     Catch2 Layout:
     
     TEST_CASE -> INTERNAL_CATCH_TESTCASE -> INTERNAL_CATCH_TESTCASE -> INTERNAL_CATCH_TESTCASE2 ->
     
     #define INTERNAL_CATCH_TESTCASE2(TestName, ...)                                                                                                                                     \
     static void TestName();                                                                                                                                                         \
     CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                                                                                                                                       \
     CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS                                                                                                                                        \
     namespace {                                                                                                                                                                     \
     Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME(autoRegistrar)(Catch::makeTestInvoker(&TestName), CATCH_INTERNAL_LINEINFO, Catch::StringRef(), Catch::NameAndTags {__VA_ARGS__}); \
     }                                                                                                                                                                   \
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION                                                                                                                                        \
    static void TestName()
     
     TestIO Layout:
     
     ? -> ?? -> ??? -> TestIO_CreateFunction(FunctionBeingTested, TestNumber, ...)
     
     Should _Generic be involved?
     
     #define TestIO_String_CreateFunction(FunctionBeingTested, TestNumber, ...) \
     static bool FunctionBeingTested##TestNumber(void) { \
     
     #define TestIO_TestString(FunctionName, ReturnType, SolutionString, TestString, TestArguments ...) \
     Hmm we need to get the type name from the FunctionName and we need to know if it's a pointer or not, well really just copy the return type?
     ReturnType Result = FunctionName(TestString, TestArguments);
     
     
     
     */
    
    //typedef struct TestIORegistry {
        /*
         
         Well, we need to know the test function names, as well as if they're enabled or not
         
         */
        //void Function_RegisterTestCase(void);
    //} TestIORegistry;
    
    uint64_t GetTimerFrequency(void) {
        uint64_t TimerFrequency = 0LL;
#if ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
        struct timespec Resolution;
        clock_getres(CLOCK_MONOTONIC, &Resolution);
        TimerFrequency = Resolution.tv_nsec;
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
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
#if   ((PlatformIO_TargetOS & PlatformIO_AppleOS) == PlatformIO_AppleOS)
                CurrentTime      = mach_continuous_time();
#elif (((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS) && ((PlatformIO_TargetOS & PlatformIO_LinuxOS) == PlatformIO_LinuxOS))
            struct timespec *TimeSpec = NULL;
            clock_gettime(CLOCK_MONOTONIC, TimeSpec);
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
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
