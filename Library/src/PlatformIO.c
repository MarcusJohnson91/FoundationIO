#include "../include/Macros.h"

#if (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
#include <stddef.h>
#include <unistd.h>
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#include <sysinfoapi.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    uint64_t FoundationIO_GetNumCPUCores(void) {
        uint64_t NumCPUCores = 0ULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS) || (FoundationIOTargetOS == FoundationIOAppleOS)
        int SysInfo[2]  = {CTL_HW, HW_AVAILCPU};
        size_t Length   = sizeof(int);
        sysctl(SysInfo, 2, &NumCPUCores, &Length, NULL, 0);
        if (NumCPUCores < 1) {
            NumCPUCores = 1;
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        SYSTEM_INFO WinSysInfo;
        GetSystemInfo(&WinSysInfo); // Might need to call GetNativeSystemInfo?
        NumCPUCores = WinSysInfo.dwNumberOfProcessors;
#endif
        return NumCPUCores;
    }
    
    uint64_t FoundationIO_GetTotalMemoryInBytes(void) {
        uint64_t TotalMemory = 0ULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        uint64_t NumPages    = (uint64_t) sysconf(_SC_PHYS_PAGES);
        uint64_t PageSize    = (uint64_t) sysconf(_SC_PAGE_SIZE);
        TotalMemory          = NumPages * PageSize;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        MEMORYSTATUSEX MemoryStatus;
        MemoryStatus.dwLength(sizeof(MemoryStatus));
        GlobalMemoryStatusEx(&MemoryStatus);
        TotalMemory          = MemoryStatus.ullTotalPhys;
#endif
        return TotalMemory;
    }
    
#ifdef __cplusplus
}
#endif
