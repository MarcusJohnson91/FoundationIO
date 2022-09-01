#include "../include/FileIO.h"            /* Included for our declarations */

#include "../include/AssertIO.h"          /* Included for Assertions */
#include "../include/AsynchronousIO.h"    /* FileIO is a higher level wrapper around AsyncIO */
#include "../include/BufferIO.h"          /* Included for BitBuffer */
#include "../include/CryptographyIO.h"    /* Included for InsecurePRNG_CreateInteger */
#include "../include/MathIO.h"            /* Included for Bits2Bytes */
#include "../include/TextIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/TextIO/StringIO.h"   /* Included for StringIO's declarations */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#include <dirent.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /* Create our own FILE implementation */
    typedef struct FileIO_FILE {
        void               *Buffer; // if there is a buffer, it's contents will be here
        uint64_t            BufferSize;   // Size of the FILE
        uint64_t            BufferOffset; // Offset into the Buffer
        AsynchronousIO_Descriptor  FileNum; // File Descriptor
        TextIO_StringTypes  Type; // Orientation
        AsynchronousIO_FileModes   Mode; // like was the file opened for reading/writing, etc
    } FileIO_FILE;
    
    UTF8 *FileIO_UTF8_GetFileName(ImmutableString_UTF8 Path8) {
        AssertIO(Path8 != NULL);

        UTF8  *Base                    = NULL;
        size_t Path8SizeInCodeUnits    = UTF8_GetStringSizeInCodeUnits(Path8);
        size_t Start                   = Path8SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0 && Path8[Start] != '\\') {
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0 && (Path8[Start] != '\\' || Path8[Start] != '/')) {
            Start                     -= 1;
        }
#endif
        size_t BaseSize                = Path8SizeInCodeUnits - Start;
        Base                           = UTF8_Init(BaseSize);
        if (Base != NULL) {
            size_t BaseIndex           = 0;
            size_t Path8Index          = Start;
            while (BaseIndex < BaseSize && Path8Index < Path8SizeInCodeUnits) {
                Base[BaseIndex]        = Path8[Path8Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF16 *FileIO_UTF16_GetFileName(ImmutableString_UTF16 Path16) {
        AssertIO(Path16 != NULL);

        UTF16 *Base                    = NULL;
        size_t Path16SizeInCodeUnits   = UTF16_GetStringSizeInCodeUnits(Path16);
        size_t Start                   = Path16SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0 && Path16[Start] != '\\') {
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0 && (Path16[Start] != '\\' || Path16[Start] != '/')) {
            Start                     -= 1;
        }
#endif
        size_t BaseSize                = Path16SizeInCodeUnits - Start;
        Base                           = UTF16_Init(BaseSize);
        if (Base != NULL) {
            size_t BaseIndex           = 0;
            size_t Path16Index         = Start;
            while (BaseIndex < BaseSize && Path16Index < Path16SizeInCodeUnits) {
                Base[BaseIndex]        = Path16[Path16Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF32 *FileIO_UTF32_GetFileName(ImmutableString_UTF32 Path32) {
        AssertIO(Path32 != NULL);
        UTF32 *Base                  = NULL;
        size_t Path32SizeInCodeUnits = UTF32_GetStringSizeInCodePoints(Path32);
        size_t Start                 = Path32SizeInCodeUnits;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        while (Start > 0 && Path32[Start] != '\\') {
            Start                     -= 1;
        }
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        while (Start > 0 && (Path32[Start] != '\\' || Path32[Start] != '/')) {
            Start                     -= 1;
        }
#endif
        size_t BaseSize                = Path32SizeInCodeUnits - Start;
        Base                           = UTF32_Init(BaseSize);
        if (Base != NULL) {
            size_t BaseIndex           = 0;
            size_t Path32Index         = Start;
            while (BaseIndex < BaseSize && Path32Index < Path32SizeInCodeUnits) {
                Base[BaseIndex]        = Path32[Path32Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF8 *FileIO_UTF8_GetFileExtension(ImmutableString_UTF8 Path8) {
        AssertIO(Path8 != NULL);

        UTF8    *Base                  = NULL;
        size_t   Path8SizeInCodeUnits  = UTF8_GetStringSizeInCodeUnits(Path8);
        size_t   Start                 = Path8SizeInCodeUnits;
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
        size_t BaseSize                = Path8SizeInCodeUnits - Start;
        Base                           = UTF8_Init(BaseSize);
        if (Base != NULL) {
            size_t BaseIndex           = 0;
            size_t Path8Index          = Start;
            while (BaseIndex < BaseSize && Path8Index < Path8SizeInCodeUnits) {
                Base[BaseIndex]        = Path8[Path8Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF16 *FileIO_UTF16_GetFileExtension(ImmutableString_UTF16 Path16) {
        AssertIO(Path16 != NULL);

        UTF16 *Base                    = NULL;
        size_t Path16SizeInCodeUnits   = UTF16_GetStringSizeInCodeUnits(Path16);
        size_t Start                   = Path16SizeInCodeUnits;
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
        size_t BaseSize                = Path16SizeInCodeUnits - Start;
        Base                           = UTF16_Init(BaseSize);
        if (Base != NULL) {
            size_t BaseIndex           = 0;
            size_t Path16Index         = Start;
            while (BaseIndex < BaseSize && Path16Index < Path16SizeInCodeUnits) {
                Base[BaseIndex]        = Path16[Path16Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }
    
    UTF32 *FileIO_UTF32_GetFileExtension(ImmutableString_UTF32 Path32) {
        AssertIO(Path32 != NULL);

        UTF32 *Base                  = NULL;
        size_t Path32SizeInCodeUnits = UTF32_GetStringSizeInCodePoints(Path32);
        size_t Start                 = Path32SizeInCodeUnits;
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
        size_t BaseSize                = Path32SizeInCodeUnits - Start;
        Base                           = UTF32_Init(BaseSize);
        if (Base != NULL) {
            size_t BaseIndex           = 0;
            size_t Path32Index         = Start;
            while (BaseIndex < BaseSize && Path32Index < Path32SizeInCodeUnits) {
                Base[BaseIndex]        = Path32[Path32Index];
                BaseIndex             += 1;
            }
        }
        return Base;
    }

    TextIO_StringTypes FileIO_File_GetType(FileIO_FILE *File) { // fwide
        AssertIO(File != NULL);

        return File->Type;
    }

    bool FileIO_File_SetType(FileIO_FILE *File, TextIO_StringTypes NewType) {
        AssertIO(File != NULL);

        bool DidItWork = false;
        if (File->Type != NewType) {
            // We gotta change the type
        } else {
            // It's already that type,just flush the buffer and say yes
            AsynchronousIOStream_Write(File->FileNum, File->Buffer, File->Type, File->BufferSize - File->BufferOffset);
        }
        return DidItWork;
    }
    
    
    TextIO_StringTypes FileIO_GetFileOrientation(PlatformIO_Immutable(FILE *) File2Orient) {
        AssertIO(File2Orient != NULL);

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
    
    /* File Operations */
    static UTF8 *UTF8_CreateModeString(AsynchronousIO_FileModes Mode) {
        UTF8 *ModeString = NULL;
        if PlatformIO_Is(Mode, FileMode_Read) {
            if PlatformIO_Is(Mode, FileMode_Binary) {
                ModeString = UTF8String("rb");
            } else if PlatformIO_Is(Mode, FileMode_Text) {
                ModeString = UTF8String("r");
            }
        } else if PlatformIO_Is(Mode, FileMode_Write) {
            if PlatformIO_Is(Mode, FileMode_Append) {
                if PlatformIO_Is(Mode, FileMode_Binary) {
                    ModeString = UTF8String("ab");
                } else if PlatformIO_Is(Mode, FileMode_Text) {
                    ModeString = UTF8String("a");
                }
            } else {
                if PlatformIO_Is(Mode, FileMode_Binary) {
                    ModeString = UTF8String("wb");
                } else if PlatformIO_Is(Mode, FileMode_Text) {
                    ModeString = UTF8String("w");
                }
            }
        }
        return ModeString;
    }
    
    static UTF16 *UTF16_CreateModeString(AsynchronousIO_FileModes Mode) {
        UTF16 *ModeString = NULL;
        if PlatformIO_Is(Mode, FileMode_Read) {
            if PlatformIO_Is(Mode, FileMode_Binary) {
                ModeString = UTF16String("rb");
            } else if PlatformIO_Is(Mode, FileMode_Text) {
                ModeString = UTF16String("r");
            }
        } else if PlatformIO_Is(Mode, FileMode_Write) {
            if PlatformIO_Is(Mode, FileMode_Append) {
                if PlatformIO_Is(Mode, FileMode_Binary) {
                    ModeString = UTF16String("ab");
                } else if PlatformIO_Is(Mode, FileMode_Text) {
                    ModeString = UTF16String("a");
                }
            } else {
                if PlatformIO_Is(Mode, FileMode_Binary) {
                    ModeString = UTF16String("wb");
                } else if PlatformIO_Is(Mode, FileMode_Text) {
                    ModeString = UTF16String("w");
                }
            }
        }
        return ModeString;
    }
    
    typedef enum FileIO_Permissions : uint16_t {
        Permissions_Unspecified    = 0,
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
        CurrentWorkingPath        = UTF8_Convert((UTF8 const*) getcwd(NULL, 0));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        CurrentWorkingPath        = _wgetcwd(PlatformIO_Cast(wchar_t*, &CWP16), _MAX_PATH);
#endif /* TargetOS */
        return CurrentWorkingPath;
    }
    
    /*!
     @abstract Dirname replacement
     */
    UTF8 *FileIO_UTF8_GetDirectoryName(ImmutableString_UTF8 Path8) {
        AssertIO(Path8 != NULL);

        UTF8 *DirectoryName = NULL;
        // Just lob off the trailing slash, filename, and extension
        return DirectoryName;
    }
    
    /*!
     @abstract Dirname replacement
     */
    UTF16 *FileIO_UTF16_GetDirectoryName(ImmutableString_UTF16 Path16) {
        AssertIO(Path16 != NULL);

        UTF16 *DirectoryName = NULL;
        return DirectoryName;
    }

    int64_t FileIO_GetPosition(FILE *Source) {
        AssertIO(Source != NULL);

        return ftello(Source);
    }

    bool FileIO_SetPosition(FILE *Source, int64_t Position) {
        AssertIO(Source != NULL);

        bool Worked = Yes;
        fseeko(Source, Position, SeekType_Current);
        return Worked;
    }

    size_t UTF8_GetStringSize(FILE *Source) {
        AssertIO(Source != NULL);

        size_t StringSizeInCodeUnits = 0;
        off_t  OriginalOffset        = FileIO_GetPosition(Source);
        UTF8   CodeUnit              = FileIO_Read(Source, &CodeUnit, sizeof(UTF8), 1);
        while (CodeUnit != TextIO_NULLTerminator) {
            CodeUnit                 = FileIO_Read(Source, &CodeUnit, sizeof(UTF8), 1);
            StringSizeInCodeUnits   += 1;
        }
        fseeko(Source, -OriginalOffset, SEEK_CUR); // Reset the stream to it's original position
        return StringSizeInCodeUnits;
    }
    
    size_t FileIO_Read(const FILE *const File2Read, void *Buffer, uint8_t BufferElementSize, size_t Elements2Read) {
        AssertIO(File2Read != NULL);
        AssertIO(Buffer != NULL);
        AssertIO(BufferElementSize >= 1 && BufferElementSize <= 8);
        AssertIO(Elements2Read >= 1);

        size_t ElementsRead = 0;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        ElementsRead = fread(Buffer, BufferElementSize, Elements2Read, PlatformIO_Mutable(FILE*, File2Read));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        ReadFile(File2Read, Buffer, Elements2Read, &ElementsRead, NULL);
#endif
        return ElementsRead;
    }
    
    bool FileIO_Seek(FILE *File2Seek, ssize_t SeekSizeInBytes, AsynchronousIO_SeekTypes SeekType) {
        AssertIO(File2Seek != NULL);

        bool SuccessIsZero      = 1;
        if PlatformIO_Is(SeekType, SeekType_Beginning) {
            // TODO: Maybe we should make sure that SeekSize fits within the file
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            SuccessIsZero       = fseeko(PlatformIO_Cast(FILE*, File2Seek), PlatformIO_Cast(off_t, SeekSizeInBytes), SEEK_SET);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            SuccessIsZero       = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_SET);
#endif
        } else if PlatformIO_Is(SeekType, SeekType_Current) {
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            SuccessIsZero       = fseeko(PlatformIO_Cast(FILE*, File2Seek), PlatformIO_Cast(off_t, SeekSizeInBytes), SEEK_CUR);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            SuccessIsZero       = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_CUR);
#endif
        } else if PlatformIO_Is(SeekType, SeekType_End) {
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            SuccessIsZero       = fseeko(PlatformIO_Cast(FILE*, File2Seek), PlatformIO_Cast(off_t, SeekSizeInBytes), SEEK_END);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            SuccessIsZero       = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_END);
#endif
        }
        return SuccessIsZero ^ 1;
    }

    size_t FileIO_Write(FILE *File2Write, PlatformIO_Immutable(void *) Buffer, uint8_t BufferElementSize, size_t Elements2Write) {
        AssertIO(File2Write != NULL);
        AssertIO(Buffer != NULL);
        AssertIO(BufferElementSize >= 1 && BufferElementSize <= 8);
        AssertIO(Elements2Write >= 1);

        size_t ElementsWritten = 0;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        ElementsWritten        = fwrite(Buffer, BufferElementSize, Elements2Write, PlatformIO_Mutable(FILE*, File2Write));
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        WriteFile(File2Write, Buffer, Elements2Write, &ElementsWritten, NULL);
#endif
        AssertIO(ElementsWritten == Elements2Write);
        return ElementsWritten;
    }
    
    bool FileIO_Close(FILE *File) {
        AssertIO(File != NULL);

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
