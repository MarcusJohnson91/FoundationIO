#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/TestIO.h"         /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */

#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
#include <time.h>                      /* Included for timespec_get */
#elif (FoundationIOTargetOS == FoundationIOAppleOS)
#include <mach/mach_time.h>
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#include <winbase.h>                   /* Included for QueryPerformanceCounter, Windows.h MUST be included before winbase */
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
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
    
    /*
     I need a high quality random number generator that is configurable.
     
     Being able to specify ranges and possibly even multiple range at once to create random numbers would be VERY useful, especially for testing Unicode strings.
     */
    
#ifdef __cplusplus
}
#endif
