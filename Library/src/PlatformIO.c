#include "../include/PlatformIO.h"      /* Included for our declarations */
#include "../include/TextIO/LogIO.h"    /* Included for logging errors */
#include "../include/TextIO/StringIO.h" /* Included for Path functions */

#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) == PlatformIO_TargetOSIsApple)
#include <sys/sysctl.h>
#elif ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#include <stddef.h>
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#include <sysinfoapi.h>
#endif /* PlatformIO_TargetOS */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    uint64_t PlatformIO_GetTotalMemoryInBytes(void) {
        uint64_t TotalMemory = 0ULL;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsApple) == PlatformIO_TargetOSIsApple)
        size_t PageSize      = sysconf(_SC_PAGE_SIZE);
        size_t NumPages;
        size_t len           = sizeof(NumPages);
        sysctlbyname("hw.memsize", &NumPages, &len, NULL, 0);
        TotalMemory          = NumPages * PageSize;
#elif ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        size_t PageSize      = sysconf(_SC_PAGE_SIZE);
        size_t NumPages      = sysconf(_SC_PHYS_PAGES);
        TotalMemory          = NumPages * PageSize;
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        MEMORYSTATUSEX MemoryStatus;
        GlobalMemoryStatusEx(&MemoryStatus);
        TotalMemory          = MemoryStatus.ullTotalPhys;
#endif
        return TotalMemory;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
