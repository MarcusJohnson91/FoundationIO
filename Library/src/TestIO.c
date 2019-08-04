#include "../include/TestIO.h"         /* Included for our declarations */
#include "../include/CryptographyIO.h" /* Included for Entropy */
#include "../include/Log.h"            /* Included for error reporting */

#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
#include <time.h>                      /* Included for timespec_get */
#elif (FoundationIOTargetOS == FoundationIOAppleOS)
#include <mach/mach_time.h>
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#ifndef   WIN32_LEAN_AND_MEAN
#define   WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */
#ifndef   VC_EXTRALEAN
#define   VC_EXTRALEAN
#endif /* VC_EXTRALEAN */
#include <Windows.h>                   /* Included for WinBase.h */
#include <WinBase.h>                   /* Included for QueryPerformanceCounter, Windows.h MUST be included before winbase */
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Automatic test registration:
     
     Have the preprocessor somehow loop over all the test functions in the file and find the ones with TestState_Enabled set?
     
     Our logger will include the name of the function when an error or test is message is sent, so if a test passes or fails or whatever it'll tell us the name of the function so we don't need Name.
     
     really all we need to know is the number of enabled tests, the number of disabled tests, and the number of tests that passed.
     
     Test Enabled/Disabled of Total passed
     */
    
    /*
     Design.
     
     So, we need a way to have there be various comparison functions, and a way to set the expected result, and it needs to be runtime checking not compile time so no macros except make a _Generic comparison function.
     */
    
    // We need a very high resolution timer so we can compare performance.
    
    // we might also want to have a good randomnes generator (maybe even Unicode)
    
    uint64_t GetTimerFrequency(void) {
        uint64_t TimerFrequency = 0LL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
        clock_getres(TimerFrequency);
#elif (FoundationIOTargetOS == FoundationIOAppleOS)
        
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        QueryPerformanceFrequency(TimerFrequency);
#endif
        return TimerFrequency;
    }
    
    uint64_t GetTime(void) {
        uint64_t Time        = 0ULL;
        uint64_t CurrentTime = 0ULL;
        for (uint8_t Loop = 1; Loop <= 3; Loop++) {
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
            clock_getres(CurrentTime);
#elif (FoundationIOTargetOS == FoundationIOAppleOS)
            CurrentTime      = mach_absolute_time(); // or mach_continuous_time?
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            QueryPerformanceFrequency(CurrentTime);
#endif
            Time            += CurrentTime;
            Time            /= Loop;
        }
        return Time;
    }
    
    static UTF32 UTF32_GenerateCodePoint(Entropy *Random) {
        UTF32 CodePoint          = 0UL;
        if (Random != NULL) {
            //UTF32  CodePointHigh = (UTF32) Entropy_GenerateIntegerInRange(Random, 1, 0xD7FF); // 1..D7FF
           // UTF32  CodePointLow  = (UTF32) Entropy_GenerateIntegerInRange(Random, 0xE000, 0x10FFFF); //  E000..10FFFF
            //CodePoint            = CodePointLow | CodePointHigh; // D7FE..101FFF = 0xF4801
        } else {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        }
        return CodePoint;
    }
    
    UTF32 *UTF32_GenerateString(Entropy *Random, uint64_t NumCodePoints) {
        UTF32 *String                 = 0UL;
        if (Random != NULL) {
            String                    = calloc(NumCodePoints + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (String != NULL) {
                for (uint64_t CodePoint = 0ULL; CodePoint < NumCodePoints; CodePoint++) {
                    String[CodePoint] = UTF32_GenerateCodePoint(Random);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate string with %llu CodePoints"), NumCodePoints);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        }
        return String;
    }
    
    /*
     Algorithm to generate random numbers:
     
     Read X bytes for the IV for a random number generator seed.
     
     seed a random number generator (good luck finding the deets on how the hell to make a random number generator)
     
     Mix the seeded data with AES.
     
     boom, you've got a array of random data.
     
     */
    
    /* API:
     GenerateRandomInteger_Insecure();
     GenerateRandomInteger_Secure();
     GenerateRandomCodePoint();
     GenerateRandomDecimal_Insecure();
     GenerateRandomDecimal_Secure();
     */
    
#ifdef __cplusplus
}
#endif
