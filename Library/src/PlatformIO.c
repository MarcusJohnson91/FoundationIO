#include "../include/PlatformIO.h" /* Included for Platform Independence macros */

#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#include <stddef.h>
#include <unistd.h>
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
#include <sysinfoapi.h>
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    uint64_t PlatformOS_GetNumCPUCores(void) {
        uint64_t NumCPUCores = 0ULL;
#if   (((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS) && ((PlatformIO_TargetOS & PlatformIO_AppleOS) != PlatformIO_AppleOS))
        NumCPUCores          = sysconf(_SC_NPROCESSORS_ONLN);
#elif (((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS) || ((PlatformIO_TargetOS & PlatformIO_AppleOS) == PlatformIO_AppleOS))
        int SysInfo[2]  = {CTL_HW, HW_AVAILCPU};
        size_t Length   = sizeof(int);
        sysctl(SysInfo, 2, &NumCPUCores, &Length, NULL, 0);
        if (NumCPUCores < 1) {
            NumCPUCores = 1;
        }
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
        SYSTEM_INFO WinSysInfo;
        GetNativeSystemInfo(&WinSysInfo);
        NumCPUCores = WinSysInfo.dwNumberOfProcessors;
#endif
        return NumCPUCores;
    }
    
    uint64_t PlatformOS_GetTotalMemoryInBytes(void) {
        uint64_t TotalMemory = 0ULL;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
        uint64_t NumPages    = (uint64_t) sysconf(_SC_PHYS_PAGES);
        uint64_t PageSize    = (uint64_t) sysconf(_SC_PAGE_SIZE);
        TotalMemory          = NumPages * PageSize;
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
		MEMORYSTATUSEX MemoryStatus;
		GlobalMemoryStatusEx(&MemoryStatus);
        TotalMemory          = MemoryStatus.ullTotalPhys;
#endif
        return TotalMemory;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
