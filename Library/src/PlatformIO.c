#include "../include/Macros.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    uint64_t FoundationIO_GetNumCPUCores(void) {
        uint64_t NumCPUCores = 0ULL;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
        int SysInfo[2]  = {CTL_HW, HW_AVAILCPU};
        size_t Length   = sizeof(int);
        sysctl(SysInfo, 2, &NumCPUCores, &Length, NULL, 0);
        if (NumCPUCores < 1) {
            NumCPUCores = 1;
        }
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
        SYSTEM_INFO WinSysInfo;
        GetSystemInfo(&WinSysInfo);
        NumCPUCores = WinSysInfo.dwNumberOfProcessors;
#endif
        return NumCPUCores;
    }
    
#ifdef __cplusplus
}
#endif
