#include "../include/Macros.h"

#if (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
#include <stddef.h>
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
    
#ifdef __cplusplus
}
#endif
