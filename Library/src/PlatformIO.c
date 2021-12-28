#include "../include/PlatformIO.h"      /* Included for our declarations */

//#include "../include/TextIO/StringIO.h" /* Included for Path functions */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
#include <sys/sysctl.h>
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#include <stddef.h>
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
#include <sysinfoapi.h>
#endif /* PlatformIO_TargetOS */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    size_t PlatformIO_GetTotalMemoryInBytes(void) {
        size_t TotalMemory = 0ULL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsApple)
        size_t PageSize      = sysconf(_SC_PAGE_SIZE);
        size_t NumPages;
        size_t len           = sizeof(NumPages);
        sysctlbyname("hw.memsize", &NumPages, &len, NULL, 0);
        TotalMemory          = NumPages * PageSize;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        size_t PageSize      = sysconf(_SC_PAGE_SIZE);
        size_t NumPages      = sysconf(_SC_PHYS_PAGES);
        TotalMemory          = NumPages * PageSize;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        MEMORYSTATUSEX MemoryStatus;
        GlobalMemoryStatusEx(&MemoryStatus);
        TotalMemory          = MemoryStatus.ullTotalPhys;
#endif
        return TotalMemory;
    }

    /*
     Kinda wonder if I could make Assert handle format args?
     */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
