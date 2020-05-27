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
    
    uint64_t PlatformIO_GetTotalMemoryInBytes(void) {
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

    uint64_t PlatformIO_Read(FILE *File2Read, uint8_t BufferElementSize, void *Buffer, uint64_t Elements2Read) {
        uint64_t BytesRead = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
        BytesRead = fread((void*) Buffer, BufferElementSize, Elements2Read, File2Read);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        BytesRead = fread(reinterpret_cast<void*>(Buffer), BufferElementSize, Elements2Read, File2Read);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
        ReadFile(File2Read, Buffer, Elements2Read, &BytesRead, NULL);
#endif
        return BytesRead;
    }

    uint64_t PlatformIO_Write(FILE *File2Write, uint8_t BufferElementSize, PlatformIO_Immutable(void *) Buffer, uint64_t Bytes2Write) {
        uint64_t BytesWritten = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
        BytesWritten = fwrite(Buffer, BufferElementSize, Bytes2Write, File2Write);
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
        WriteFile(File2Write, Buffer, Bytes2Write, &BytesWritten, NULL);
#endif
        return BytesWritten;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
