#include <stdio.h>                      /* Included for FILE, SEEK SET/END/CUR macros */
#include "../include/PlatformIO.h"      /* Included for Platform Independence macros */
#include "../include/UnicodeIO/LogIO.h" /* Included for logging errors */

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

    void PlatformIO_Seek(FILE *File2Seek, int64_t SeekSizeInBytes, PlatformIO_SeekTypes SeekType) {
        // Always open Streams in Binary Mode, not Text Mode; we'll handle Text conversion ourselves
        bool SuccessIsZero = 1;
        if (SeekType == SeekType_Beginning) {
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SuccessIsZero = fseeko(File2Seek, SeekSizeInBytes, SEEK_SET);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SuccessIsZero = fseeko(File2Seek, reintrepret_cast<offt>(SeekSizeInBytes), SEEK_SET);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SuccessIsZero = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_SET);
#endif
        } else if (SeekType == SeekType_Current) {
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SuccessIsZero = fseeko(File2Seek, SeekSizeInBytes, SEEK_CUR);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SuccessIsZero = fseeko(File2Seek, reintrepret_cast<offt>(SeekSizeInBytes), SEEK_CUR);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SuccessIsZero = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_CUR);
#endif
        } else if (SeekType == SeekType_End) {
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SuccessIsZero = fseeko(File2Seek, SeekSizeInBytes, SEEK_END);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SuccessIsZero = fseeko(File2Seek, reintrepret_cast<offt>(SeekSizeInBytes), SEEK_END);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SuccessIsZero = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_END);
#endif
        }
        if (SuccessIsZero != 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Seeking failed"));
        }
    }

    uint64_t PlatformIO_Write(FILE *File2Write, uint8_t BufferElementSize, PlatformIO_Immutable(void *) Buffer, uint64_t Elements2Write) {
        uint64_t BytesWritten = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
        BytesWritten = fwrite(Buffer, BufferElementSize, Elements2Write, File2Write);
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
        WriteFile(File2Write, Buffer, Elements2Write, &BytesWritten, NULL);
#endif
        if (BytesWritten != Elements2Write) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Wrote %llu Elements but %llu Elements were requested"), BytesWritten, Elements2Write);
        }
        return BytesWritten;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
