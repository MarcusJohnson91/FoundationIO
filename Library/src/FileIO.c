#include "../include/FileIO.h"            /* Included for our declarations */

#include "../include/BufferIO.h"          /* Included for BitBuffer */
#include "../include/CryptographyIO.h"    /* Included for SecureRNG_GenerateInteger */
#include "../include/MathIO.h"            /* Included for Bits2Bytes */
#include "../include/TextIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/TextIO/LogIO.h"      /* Included for Logging */
#include "../include/TextIO/StringIO.h"   /* Included for StringIO's declarations */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#include <dirent.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     File Operations:
     Open
     Close
     Read
     Write
     Create
     Delete
     GetType
     */
    
    UTF8 *FileIO_UTF8_GetFileName(ImmutableString_UTF8 Path8) {
        UTF8 *Base = NULL;
        uint64_t Path8SizeInCodeUnits  = UTF8_GetStringSizeInCodeUnits(Path8);
        uint64_t Start                 = Path8SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0 && Path8[Start] != '\\') {
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0 && (Path8[Start] != '\\' || Path8[Start] != '/')) {
            Start                     -= 1;
        }
#endif
        uint64_t BaseSize              = Path8SizeInCodeUnits - Start;
        Base                           = UTF8_Init(BaseSize);
        if (Base != NULL) {
            uint64_t BaseIndex         = 0;
            uint64_t Path8Index        = Start;
            while (BaseIndex < BaseSize && Path8Index < Path8SizeInCodeUnits) {
                Base[BaseIndex]        = Path8[Path8Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF16 *FileIO_UTF16_GetFileName(ImmutableString_UTF16 Path16) {
        UTF16 *Base = NULL;
        uint64_t Path16SizeInCodeUnits = UTF16_GetStringSizeInCodeUnits(Path16);
        uint64_t Start                 = Path16SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0 && Path16[Start] != '\\') {
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0 && (Path16[Start] != '\\' || Path16[Start] != '/')) {
            Start                     -= 1;
        }
#endif
        uint64_t BaseSize              = Path16SizeInCodeUnits - Start;
        Base                           = UTF16_Init(BaseSize);
        if (Base != NULL) {
            uint64_t BaseIndex         = 0;
            uint64_t Path16Index       = Start;
            while (BaseIndex < BaseSize && Path16Index < Path16SizeInCodeUnits) {
                Base[BaseIndex]        = Path16[Path16Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF32 *FileIO_UTF32_GetFileName(ImmutableString_UTF32 Path32) {
        UTF32 *Base = NULL;
        uint64_t Path32SizeInCodeUnits = UTF32_GetStringSizeInCodePoints(Path32);
        uint64_t Start                 = Path32SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0 && Path32[Start] != '\\') {
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0 && (Path32[Start] != '\\' || Path32[Start] != '/')) {
            Start                     -= 1;
        }
#endif
        uint64_t BaseSize              = Path32SizeInCodeUnits - Start;
        Base                           = UTF32_Init(BaseSize);
        if (Base != NULL) {
            uint64_t BaseIndex         = 0;
            uint64_t Path32Index       = Start;
            while (BaseIndex < BaseSize && Path32Index < Path32SizeInCodeUnits) {
                Base[BaseIndex]        = Path32[Path32Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF8 *FileIO_UTF8_GetFileExtension(ImmutableString_UTF8 Path8) {
        UTF8 *Base = NULL;
        /*
         if you find a '.' before a slash or after the start of the string, it has an extension.
         Start at the end of the string, loop until you find a dot or the start of the string or a slash
         */
        uint64_t Path8SizeInCodeUnits  = UTF8_GetStringSizeInCodeUnits(Path8);
        uint64_t Start                 = Path8SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0) {
            if (Path8[Start] == '\\') { // Stop the loop because we're now in a folder
                return Base;
            } else if (Path8[Start] == '.') {
                Start                 -= 1;
                break;
            }
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0) {
            if (Path8[Start] == '\\' || Path8[Start] == '/') { // Stop the loop because we're now in a folder
                return Base;
            } else if (Path8[Start] == '.') {
                Start                 -= 1;
                break;
            }
            Start                     -= 1;
        }
#endif
        uint64_t BaseSize              = Path8SizeInCodeUnits - Start;
        Base                           = UTF8_Init(BaseSize);
        if (Base != NULL) {
            uint64_t BaseIndex         = 0;
            uint64_t Path8Index        = Start;
            while (BaseIndex < BaseSize && Path8Index < Path8SizeInCodeUnits) {
                Base[BaseIndex]        = Path8[Path8Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF16 *FileIO_UTF16_GetFileExtension(ImmutableString_UTF16 Path16) {
        UTF16 *Base = NULL;
        uint64_t Path16SizeInCodeUnits = UTF16_GetStringSizeInCodeUnits(Path16);
        uint64_t Start                 = Path16SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0) {
            if (Path16[Start] == '\\') { // Stop the loop because we're now in a folder
                return Base;
            } else if (Path16[Start] == '.') {
                Start                 -= 1;
                break;
            }
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0) {
            if (Path16[Start] == '\\') { // Stop the loop because we're now in a folder
                return Base;
            } else if (Path16[Start] == '.') {
                Start                 -= 1;
                break;
            }
            Start                     -= 1;
        }
#endif
        uint64_t BaseSize              = Path16SizeInCodeUnits - Start;
        Base                           = UTF16_Init(BaseSize);
        if (Base != NULL) {
            uint64_t BaseIndex         = 0;
            uint64_t Path16Index       = Start;
            while (BaseIndex < BaseSize && Path16Index < Path16SizeInCodeUnits) {
                Base[BaseIndex]        = Path16[Path16Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF32 *FileIO_UTF32_GetFileExtension(ImmutableString_UTF32 Path32) {
        UTF32 *Base = NULL;
        uint64_t Path32SizeInCodeUnits = UTF32_GetStringSizeInCodePoints(Path32);
        uint64_t Start                 = Path32SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0) {
            if (Path32[Start] == '\\') { // Stop the loop because we're now in a folder
                return Base;
            } else if (Path32[Start] == '.') {
                Start                 -= 1;
                break;
            }
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0) {
            if (Path32[Start] == '\\') { // Stop the loop because we're now in a folder
                return Base;
            } else if (Path32[Start] == '.') {
                Start                 -= 1;
                break;
            }
            Start                     -= 1;
        }
#endif
        uint64_t BaseSize              = Path32SizeInCodeUnits - Start;
        Base                           = UTF32_Init(BaseSize);
        if (Base != NULL) {
            uint64_t BaseIndex         = 0;
            uint64_t Path32Index       = Start;
            while (BaseIndex < BaseSize && Path32Index < Path32SizeInCodeUnits) {
                Base[BaseIndex]        = Path32[Path32Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    
    TextIO_StringTypes FileIO_GetFileOrientation(PlatformIO_Immutable(FILE *) File2Orient) {
        TextIO_StringTypes StringType       = StringType_Unspecified;
        FILE *File2Orient2                  = PlatformIO_Mutable(FILE*, File2Orient);
        int Orientation                     = fwide(File2Orient2, 0);
        if (Orientation < 0) {
            StringType                      = StringType_UTF8;
        } else if (Orientation > 0) {
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            StringType                      = StringType_UTF32;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            StringType                      = StringType_UTF16;
#endif
        }
        return StringType;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    uint8_t FileIO_GetEncodingSize(AsyncIOStream *Stream) {
        uint8_t EncodingSize = 0;
        return EncodingSize;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    /* File Operations */
    static UTF8 *UTF8_CreateModeString(AsyncIO_FileModes Mode) {
        UTF8 *ModeString = NULL;
        if ((Mode & FileMode_Read) == FileMode_Read) {
            if ((Mode & FileMode_Binary) == FileMode_Binary) {
                ModeString = UTF8String("rb");
            } else if ((Mode & FileMode_Text) == FileMode_Text) {
                ModeString = UTF8String("r");
            }
        } else if ((Mode & FileMode_Write) == FileMode_Write) {
            if ((Mode & FileMode_Append) == FileMode_Append) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = UTF8String("ab");
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = UTF8String("a");
                }
            } else {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = UTF8String("wb");
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = UTF8String("w");
                }
            }
        }
        return ModeString;
    }
    
    static UTF16 *UTF16_CreateModeString(AsyncIO_FileModes Mode) {
        UTF16 *ModeString = NULL;
        if ((Mode & FileMode_Read) == FileMode_Read) {
            if ((Mode & FileMode_Binary) == FileMode_Binary) {
                ModeString = UTF16String("rb");
            } else if ((Mode & FileMode_Text) == FileMode_Text) {
                ModeString = UTF16String("r");
            }
        } else if ((Mode & FileMode_Write) == FileMode_Write) {
            if ((Mode & FileMode_Append) == FileMode_Append) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = UTF16String("ab");
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = UTF16String("a");
                }
            } else {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = UTF16String("wb");
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = UTF16String("w");
                }
            }
        }
        return ModeString;
    }
    
    typedef enum FileIO_Permissions {
        Permissions_Unknown        = 0,
        Permissions_Owner_Read     = 1,
        Permissions_Owner_Write    = 2,
        Permissions_Owner_Execute  = 4,
        Permissions_Group_Read     = 8,
        Permissions_Group_Write    = 16,
        Permissions_Group_Execute  = 32,
        Permissions_Global_Read    = 64,
        Permissions_Global_Write   = 128,
        Permissions_Global_Execute = 256,
    } FileIO_Permissions;
    
    /*
     Functions that would be useful to have:
     IsFolder/IsDirectory, for telling if an inode belongs to a file or directory
     
     Actually, a more flexible option would be GetInodeType or something, that would return the file associate with a path or inode number.
     
     Relative vs Absolute path
     
     So yeah, I think we can handle this with just 2 or 3 functions.
     
     UTF8_GetPathID which returns an INODE number.
     
     What about ZFS tho? it uses 128 bit INODE numbers i believe?
     
     Also, we should have function to get Permissions from an Inode as well
     
     UserID, GroupID,
     */
    
    UTF16 *GetCWD(void) {
        UTF16 *CWD = NULL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        struct stat Info;
        stat(".", &Info);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        struct __stat64 Info;
        _stat64(".", &Info);
#endif /* TargetOS */
        
        // Why can't we just implement CWD from scratch? how does stat/_stat64/_wstat64 work?
        
        // We need to find the current working directory, aka stat"."? Yes stat . is correct
        
        // So then all we need to do is loop like the other guy said  until we reach root, which we can do by
        // Looping until the path equals the device name in Windows Stat command
        // On MacOS "pwd" does not have a trailing slash, so I will not include one either.
        
        // POSIX OSes use `stat`; Windows needs _stat64/_wstat64
        return CWD;
    }
    
    /*
     Might want to support tilde expansion as well
     
     FileIO_UTF8_GetUsersName
     
     FileIO_UTF8_GetUsersDirectory
     */
    
    UTF8 *FileIO_UTF8_GetUsersPath(void) {
        UTF8 *UsersPath = NULL;
        // Users folder for example: /Users/Marcus
        // CSIDL_PROFILE on Windows, HOME on POSIX
        return UsersPath;
    }
    
    typedef struct FileID {
        dev_t DeviceID;
        ino_t iNodeID;
    } FileID;
    
    UTF8 *FileIO_UTF8_GetCurrentWorkingPath(void) {
        UTF8 *CurrentWorkingPath = NULL;
        /*
         I don't want to rely on an OS to do it for me, I want to do it from scratch.
         
         So, What I need is to find the number of directories until I hit root so I can create a StringSet.
         
         Then read The Directory Name for each
         
         You know what would be a SUPER useful function to have?
         
         StringSet_Flatten, which returns a string with each part of the StringSet in it's proper place.
         
         Well shit that was easy, what now?
         */
        
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        CurrentWorkingPath       = PlatformIO_Cast(UTF8*, getcwd(NULL, 0));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        UTF16 *CWP16[_MAX_PATH + TextIO_NULLTerminatorSize];
        _wgetcwd(PlatformIO_Cast(wchar_t*, &CWP16), _MAX_PATH);
        CurrentWorkingPath       = UTF16_Convert(CWP16);
#endif /* TargetOS */
        return CurrentWorkingPath;
    }
    
    UTF16 *FileIO_UTF16_GetCurrentWorkingPath(void) {
        UTF16 *CurrentWorkingPath = NULL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        CurrentWorkingPath        = UTF8_Convert(getcwd(NULL, 0));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        CurrentWorkingPath        = _wgetcwd(PlatformIO_Cast(wchar_t*, &CWP16), _MAX_PATH);
#endif /* TargetOS */
        return CurrentWorkingPath;
    }
    
    /*!
     @abstract Dirname replacement
     */
    UTF8 *FileIO_UTF8_GetDirectoryName(ImmutableString_UTF8 Path8) {
        UTF8 *DirectoryName = NULL;
        // Just lob off the trailing slash, filename, and extension
        return DirectoryName;
    }
    
    /*!
     @abstract Dirname replacement
     */
    UTF16 *FileIO_UTF16_GetDirectoryName(ImmutableString_UTF16 Path16) {
        UTF16 *DirectoryName = NULL;
        return DirectoryName;
    }
    
    uint64_t FileIO_Read(PlatformIO_Immutable(FILE *) File2Read, void *Buffer, uint8_t BufferElementSize, uint64_t Elements2Read) {
        uint64_t BytesRead = 0;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        BytesRead = fread((void*) Buffer, BufferElementSize, Elements2Read, PlatformIO_Mutable(FILE*, File2Read));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        ReadFile(File2Read, Buffer, Elements2Read, &BytesRead, NULL);
#endif
        return BytesRead;
    }
    
    bool FileIO_Seek(PlatformIO_Immutable(FILE *) File2Seek, int64_t SeekSizeInBytes, AsyncIO_SeekTypes SeekType) {
        bool SuccessIsZero      = 1;
        if ((SeekType & SeekType_Beginning) == SeekType_Beginning) {
            // TODO: Maybe we should make sure that SeekSize fits within the file
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            SuccessIsZero       = fseeko(PlatformIO_Cast(FILE*, File2Seek), PlatformIO_Cast(off_t, SeekSizeInBytes), SEEK_SET);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            SuccessIsZero       = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_SET);
#endif
        } else if ((SeekType & SeekType_Current) == SeekType_Current) {
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            SuccessIsZero       = fseeko(PlatformIO_Cast(FILE*, File2Seek), PlatformIO_Cast(off_t, SeekSizeInBytes), SEEK_CUR);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            SuccessIsZero       = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_CUR);
#endif
        } else if ((SeekType & SeekType_End) == SeekType_End) {
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            SuccessIsZero       = fseeko(PlatformIO_Cast(FILE*, File2Seek), PlatformIO_Cast(off_t, SeekSizeInBytes), SEEK_END);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            SuccessIsZero       = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_END);
#endif
        }
        return SuccessIsZero ^ 1;
    }
    
    uint64_t FileIO_Write(PlatformIO_Immutable(FILE *) File2Write, PlatformIO_Immutable(void *) Buffer, uint8_t BufferElementSize, uint64_t Elements2Write) {
        uint64_t BytesWritten = 0;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        BytesWritten          = fwrite(Buffer, BufferElementSize, Elements2Write, PlatformIO_Mutable(FILE*, File2Write));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        WriteFile(File2Write, Buffer, Elements2Write, &BytesWritten, NULL);
#endif
        if (BytesWritten != Elements2Write) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %llu Elements but %llu Elements were requested"), BytesWritten, Elements2Write);
        }
        return BytesWritten;
    }
    
    bool FileIO_Close(FILE *File) {
        bool FileClosedSucessfully = No;
        if (File != NULL) {
            fflush(File);
            FileClosedSucessfully  = fclose(File);
        }
        return !FileClosedSucessfully;
    }
    /* File Operations */
    
    // fchdir changes the current working directory
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
