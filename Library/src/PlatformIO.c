#include "../include/PlatformIO.h"         /* Included for Platform Independence macros */
#include "../include/UnicodeIO/LogIO.h"    /* Included for logging errors */
#include "../include/UnicodeIO/StringIO.h" /* Included for Path functions */

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

    FILE *PlatformIO_OpenUTF8(PlatformIO_Immutable(void *) Path, PlatformIO_FileModes Mode) {
        PlatformIO_Immutable(UTF8 *) Path8 = (PlatformIO_Immutable(UTF8 *)) Path;
        FILE *File = NULL;
        if (Path8 != NULL && Mode != FileMode_Unspecified) {
            errno_t ErrorCode = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
            UTF8 *ModeString = NULL;
            if ((Mode & FileMode_Read) == FileMode_Read) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"rb");
                } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"r");
                }
            } else if ((Mode & FileMode_Write) == FileMode_Write) {
                if ((Mode & FileMode_Append) == FileMode_Append) {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"ab");
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"a");
                    }
                } else {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"wb");
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"w");
                    }
                }
            }
#if __STDC_LIB_EXT1__
            if (UTF8_HasBOM(Path)) {
                ErrorCode = fopen_s(&File, &Path8[UTF8BOMSizeInCodeUnits], ModeString);
            } else {
                ErrorCode = fopen_s(&File, Path8, ModeString);
            }
#else
            if (UTF8_HasBOM(Path8)) {
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
                File = fopen(&Path8[UTF8BOMSizeInCodeUnits], ModeString);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
                File = fopen(reinterpret_cast<const char *>(&Path8[UTF8BOMSizeInCodeUnits]), reinterpret_cast<const char *>(ModeString));
#endif
            } else {
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
                File = fopen(Path8, ModeString);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
                File = fopen(reinterpret_cast<const char *>(Path8), reinterpret_cast<const char *>(ModeString));
#endif
            }
#endif /* __STDC_LIB_EXT1__ */
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
            PlatformIO_Immutable(UTF16 *) Path16 = UTF8_Convert(Path);
            static UTF16 *ModeString = NULL;
            if ((Mode & FileMode_Read) == FileMode_Read) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = u"rb";
                } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                    ModeString = u"r";
                }
            } else if ((Mode & FileMode_Write) == FileMode_Write) {
                if ((Mode & FileMode_Append) == FileMode_Append) {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = u"ab";
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = u"a";
                    }
                } else {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = u"wb";
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = u"w";
                    }
                }
            }
            if (UTF16_HasBOM(Path16)) {
                ErrorCode = _wfopen_s(&File, (const wchar_t*) &Path16[UTF16BOMSizeInCodeUnits], (PlatformIO_Immutable(wchar_t*)) ModeString);
            } else {
                ErrorCode = _wfopen_s(&File, (const wchar_t*) &Path16[UTF16BOMSizeInCodeUnits], (PlatformIO_Immutable(wchar_t*)) ModeString);
            }
#endif /* PlatformIO_TargetOS */
            if (File != NULL) {
                setvbuf(File, NULL, _IONBF, 0);
            }

            if (ErrorCode != 0) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't open %s, got Error: %u"), Path, ErrorCode);
            }
        }
        return File;
    }

    FILE *PlatformIO_OpenUTF16(PlatformIO_Immutable(void *) Path, PlatformIO_FileModes Mode) {
        PlatformIO_Immutable(UTF16 *) Path16 = (PlatformIO_Immutable(UTF16 *)) Path;
        FILE *File = NULL;
        if (Path16 != NULL && Mode != FileMode_Unspecified) {
            errno_t ErrorCode = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
            PlatformIO_Immutable(UTF8 *) Path8 = UTF16_Convert(Path16);
            const UTF8 *ModeString = NULL;
            if ((Mode & FileMode_Read) == FileMode_Read) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"rb");
                } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"r");
                }
            } else if ((Mode & FileMode_Write) == FileMode_Write) {
                if ((Mode & FileMode_Append) == FileMode_Append) {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"ab");
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"a");
                    }
                } else {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"wb");
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = PlatformIO_Literal(UTF8*, char*, u8"w");
                    }
                }
            }
#if __STDC_LIB_EXT1__
            if (UTF8_HasBOM(Path)) {
                ErrorCode = fopen_s(&File, &Path8[UTF8BOMSizeInCodeUnits], ModeString);
            } else {
                ErrorCode = fopen_s(&File, Path8, ModeString);
            }
#else
            if (UTF8_HasBOM(Path8)) {
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
                File = fopen(&Path8[UTF8BOMSizeInCodeUnits], ModeString);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
                File = fopen(reinterpret_cast<const char *>(&Path8[UTF8BOMSizeInCodeUnits]), reinterpret_cast<const char *>(ModeString));
#endif
            } else {
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
                File = fopen(Path8, ModeString);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
                File = fopen(reinterpret_cast<const char *>(Path8), reinterpret_cast<const char*>(ModeString));
#endif
            }

            if (ErrorCode != 0) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't open %s, got Error: %u"), Path8, ErrorCode);
            }
#endif /* __STDC_LIB_EXT1__ */
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
            static UTF16 *ModeString = NULL;
            if ((Mode & FileMode_Read) == FileMode_Read) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = u"rb";
                } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                    ModeString = u"r";
                }
            } else if ((Mode & FileMode_Write) == FileMode_Write) {
                if ((Mode & FileMode_Append) == FileMode_Append) {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = u"ab";
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = u"a";
                    }
                } else {
                    if ((Mode & FileMode_Binary) == FileMode_Binary) {
                        ModeString = u"wb";
                    } else if ((Mode & FileMode_Binary) == FileMode_Text) {
                        ModeString = u"w";
                    }
                }
            }

            if (UTF16_HasBOM(Path16)) {
                ErrorCode = _wfopen_s(&File, (const wchar_t*) &Path16[UTF16BOMSizeInCodeUnits], (PlatformIO_Immutable(wchar_t*)) ModeString);
            } else {
                ErrorCode = _wfopen_s(&File, (const wchar_t*) &Path16, (PlatformIO_Immutable(wchar_t*)) ModeString);
            }
            if (ErrorCode != 0) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't open %ls, got Error: %u"), (wchar_t*) Path, ErrorCode);
            }
#endif /* PlatformIO_TargetOS */
            if (File != NULL) {
                setvbuf(File, NULL, _IONBF, 0);
            }
        }
        return File;
    }

    uint64_t PlatformIO_GetSize(FILE *File) { // Hmm, things that return pointers may be very fancy
        /*
         in both AMD64 and ARM64 pointer addresses can only be 48 bits, the high 16 bits must be set to all 1's
         */
        uint64_t FileSize = 0ULL;
        if (File != NULL) {
#if   (PlatformIO_TargetOS == PlatformIO_TargetOSIsPOSIX)
            FileSize      = (uint64_t) ftello(File);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            FileSize      = (uint64_t) _ftelli64(File);
#endif /* PlatformIO_TargetOS */
        }
        return FileSize;
    }

    uint64_t PlatformIO_Read(FILE *File2Read, void *Buffer, uint8_t BufferElementSize, uint64_t Elements2Read) {
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

    bool PlatformIO_Seek(FILE *File2Seek, int64_t SeekSizeInBytes, PlatformIO_SeekTypes SeekType) {
        // Always open Streams in Binary Mode, not Text Mode; we'll handle Text conversion ourselves
        bool SeekingWasSucessful = No;
        bool SuccessIsZero = 1;
        if (SeekType == SeekType_Beginning) {
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SeekingWasSucessful = fseeko(File2Seek, SeekSizeInBytes, SEEK_SET);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SeekingWasSucessful = fseeko(File2Seek, reinterpret_cast<off_t>(SeekSizeInBytes), SEEK_SET);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SeekingWasSucessful = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_SET);
#endif
        } else if (SeekType == SeekType_Current) {
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SeekingWasSucessful = fseeko(File2Seek, SeekSizeInBytes, SEEK_CUR);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SeekingWasSucessful = fseeko(File2Seek, reinterpret_cast<off_t>(SeekSizeInBytes), SEEK_CUR);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SeekingWasSucessful = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_CUR);
#endif
        } else if (SeekType == SeekType_End) {
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SeekingWasSucessful = fseeko(File2Seek, SeekSizeInBytes, SEEK_END);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SeekingWasSucessful = fseeko(File2Seek, reinterpret_cast<off_t>(SeekSizeInBytes), SEEK_END);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SeekingWasSucessful = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_END);
#endif
        }
        if (SuccessIsZero != 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Seeking failed"));
        }
        return ~SeekingWasSucessful;
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

    bool PlatformIO_Close(FILE *File) {
        bool FileClosedSucessfully = No;
        if (File != NULL) {
            fflush(File);
            FileClosedSucessfully  = fclose(File);

        }
        return ~FileClosedSucessfully;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
