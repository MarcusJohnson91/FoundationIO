#include "../include/PlatformIO.h"         /* Included for our declarations */

#include "../include/AssertIO.h"           /* Included for Assertions */
#include "../include/TextIO/FormatIO.h"    /* Included for UTF8_Format */
#include "../include/TextIO/StringIO.h"    /* Included for WriteLine8 */
#include "../include/TextIO/TextIOTypes.h" /* Included for NewLine8 */

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
        size_t PageSize      = (size_t) sysconf(_SC_PAGE_SIZE);
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

    PlatformIO_Range Range_Init(size_t Start, size_t End) {
        PlatformIO_Range Range;
        Range.Start = Start;
        Range.End = End;
        return Range;
    }

    size_t Range_GetLength(PlatformIO_Range Range) {
        return Range.End - Range.Start;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
