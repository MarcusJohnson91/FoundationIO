#include "../include/PlatformIO.h"         /* Included for our declarations */
#include "../include/TextIO/TextIOTypes.h" /* Included for NewLine8 */
#include "../include/TextIO/FormatIO.h"    /* Included for UTF8_Format */
#include "../include/TextIO/StringIO.h"    /* Included for WriteLine8 */

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

    void PlatformIO_AssertFail(const char *FileName, const char *FunctionName, const unsigned char *Expression, ...) {
        // Check if there's any specifiers in Expression
        uint64_t NumSpecifiers = UTF8_GetNumFormatSpecifiers(Expression);
        UTF8    *ExpressionVA  = (UTF8*) Expression;
        if (NumSpecifiers > 0) {
            va_list VariadicArguments;
            va_start(VariadicArguments, Expression);
            ExpressionVA = UTF8_Format(ExpressionVA, VariadicArguments);
            va_end(VariadicArguments);
        }
        UTF8 *Formatted = UTF8_Format(UTF8String("Assertion '%s' in %s::%s Failed%s"), ExpressionVA, FileName, FunctionName, TextIO_NewLine8);
        UTF8_File_WriteString(stderr, Formatted);
        UTF8_Deinit(Formatted);
        exit(EXIT_FAILURE);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
