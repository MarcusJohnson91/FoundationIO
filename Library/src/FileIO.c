#include "../include/FileIO.h"            /* Included for our declarations */
#include "../include/BufferIO.h"          /* Included for BitBuffer */
#include "../include/CryptographyIO.h"    /* Included for SecureRNG_GenerateInteger for GUUID_Generate */
#include "../include/MathIO.h"            /* Included for Bits2Bytes */
#include "../include/TextIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/TextIO/LogIO.h"      /* Included for Logging */
#include "../include/TextIO/StringIO.h"   /* Included for StringIO's declarations */


#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct FileInput {
        FILE            *File;
        uint64_t         FilePosition;
        uint64_t         FileSize;
    } FileInput;

    typedef struct FileOutput {
        FILE            *File;
        uint64_t         FilePosition;
    } FileOutput;

    /* FileInput */
    FileInput *FileInput_Init(void) {
        FileInput *Input = (FileInput*) calloc(1, sizeof(FileInput));
        if (Input == NULL) {
            FileInput_Deinit(Input);
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate FileInput"));
        }
        return Input;
    }

    void FileInput_SetFile(FileInput *Input, FILE *File) {
        if (Input != NULL && File != NULL) {
            Input->File = File;
        } else if (Input == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileInput Pointer is NULL"));
        } else if (File == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }

    static void FileInput_FindFileSize(FileInput *Input) {
        if (Input != NULL) {
            FileIO_Seek(Input->File, 0, SeekType_End);
            Input->FileSize     = (uint64_t) FileIO_GetSize(Input->File);
            FileIO_Seek(Input->File, 0, SeekType_Beginning);
            Input->FilePosition = (uint64_t) FileIO_GetSize(Input->File);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileInput Pointer is NULL"));
        }
    }

    uint64_t FileInput_GetFileSize(FileInput *Input) {
        uint64_t InputSize = 0ULL;
        if (Input != NULL) {
            if (Input->FileSize == 0) {
                FileInput_FindFileSize(Input);
            }
            InputSize     = Input->FileSize;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileInput Pointer is NULL"));
        }
        return InputSize;
    }

    uint64_t FileInput_GetFilePosition(FileInput *Input) {
        uint64_t Position = 0ULL;
        if (Input != NULL) {
            if (Input->FilePosition == 0) {
                FileInput_FindFileSize(Input);
            }
            Position    = Input->FilePosition;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileInput Pointer is NULL"));
        }
        return Position;
    }

    uint64_t FileInput_BytesRemaining(FileInput *Input) {
        uint64_t BytesLeft = 0ULL;
        if (Input != NULL) {
            if (Input->FilePosition == 0) {
                FileInput_FindFileSize(Input);
            }
            BytesLeft    = Input->FileSize - Input->FilePosition;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileInput Pointer is NULL"));
        }
        return BytesLeft;
    }

    void FileInput_Deinit(FileInput *Input) {
        if (Input != NULL) {
            FileIO_Close(Input->File);
            free(Input);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileInput Pointer is NULL"));
        }
    }
    /* FileInput */

    /* FileOutput */
    FileOutput *FileOutput_Init(void) {
        FileOutput *Output = (FileOutput*) calloc(1, sizeof(FileOutput));
        if (Output == NULL) {
            FileOutput_Deinit(Output);
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate FileOutput"));
        }
        return Output;
    }

    void FileOutput_SetFile(FileOutput *Output, FILE *File) {
        if (Output != NULL && File != NULL) {
            Output->File = File;
        } else if (Output == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileOutput Pointer is NULL"));
        } else if (File == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FILE Pointer is NULL"));
        }
    }

    void FileOutput_Deinit(FileOutput *Output) {
        if (Output != NULL) {
            fflush(Output->File);
            FileIO_Close(Output->File);
            free(Output);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileOutput Pointer is NULL"));
        }
    }
    /* FileOutput */

    /* BitBuffer */
    void FileInput_ReadBitBuffer(FileInput *Input, BitBuffer *BitB) {
        if (Input != NULL && BitB != NULL) {
            uint64_t ArraySizeInBits = BitBuffer_GetSize(BitB);
            uint64_t ArrayOffset     = BitBuffer_GetPosition(BitB);
            uint8_t *Array           = BitBuffer_GetArray(BitB);
            uint64_t Bytes2Read      = Bits2Bytes(ArraySizeInBits - ArrayOffset, RoundingType_Down);
            uint8_t  Bits2Save       = ArrayOffset % 8;
            if (Bits2Save > 0) {
                Array[0]             = 0;
                uint8_t Saved        = Array[Bytes2Read + 1] & (Logarithm(2, Bits2Save) - 1); // Todo: Add shift
                Array[0]             = Saved;
                BitBuffer_SetPosition(BitB, Bits2Save);
                for (uint64_t Byte   = (uint64_t) Bits2Bytes(ArrayOffset, RoundingType_Down); Byte < (uint64_t) Bits2Bytes(ArraySizeInBits - ArrayOffset, RoundingType_Down); Byte++) {
                    Array[Byte]      = 0;
                }
            }
            uint64_t BytesRead       = 0ULL;
            BytesRead                = FileIO_Read(Input->File, Array, sizeof(Array[0]), Bytes2Read);
            if (BytesRead == Bytes2Read) {
                BitBuffer_SetSize(BitB, Bits2Bytes(BytesRead, RoundingType_Down) + ArrayOffset);
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Num bytes read %llu does not match num bytes requested %llu"), BytesRead, Bytes2Read);
            }
        } else if (Input == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitInput Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void BitBuffer_Refresh(FileInput *Input, BitBuffer *BitB) {
        if (Input != NULL && BitB != NULL) {
            uint64_t ArraySizeInBits = BitBuffer_GetSize(BitB);
            uint64_t ArrayOffset     = BitBuffer_GetPosition(BitB);
            uint8_t *Array           = BitBuffer_GetArray(BitB);
            uint64_t Bytes2Read      = Bits2Bytes(BitBuffer_GetSize(BitB), RoundingType_Down);
            uint64_t BytesRead       = 0ULL;
            BytesRead                = FileIO_Read(Input->File, Array, sizeof(Array[0]), Bytes2Read);
            if (BytesRead != Bytes2Read) {
                uint8_t *Buffer_Old  = Array;
                uint8_t *Buffer_New  = (uint8_t*) realloc(Array, BytesRead);
                if (Buffer_New != NULL) {
                    BitBuffer_SetPosition(BitB, 0);
                    BitBuffer_SetArray(BitB, Buffer_New, BytesRead);
                    free(Buffer_Old);
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Realloc failed"));
                }
            }
        } else if (Input == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileInput Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void BitBuffer_Write(FileOutput *Output, BitBuffer *BitB) {
        if (Output != NULL && BitB != NULL) {
            uint64_t ArraySizeInBits = BitBuffer_GetSize(BitB);
            uint64_t ArrayOffset     = BitBuffer_GetPosition(BitB);
            uint8_t *Array           = BitBuffer_GetArray(BitB);
            uint64_t Bytes2Write     = Bits2Bytes(BitBuffer_GetPosition(BitB), RoundingType_Down);
            uint64_t Bits2Keep       = ArrayOffset % 8;
            uint64_t BytesWritten    = FileIO_Write(Output->File, Array, sizeof(uint8_t), Bytes2Write);
            if (BytesWritten == Bytes2Write) {
                Array[0]             = 0;
                Array[0]             = Array[Bytes2Write + 1] & (Exponentiate(2, Bits2Keep) << (8 - Bits2Keep));
                BitBuffer_SetPosition(BitB, Bits2Keep + 1);
                for (uint64_t Byte = (uint64_t) Bits2Bytes(ArrayOffset, RoundingType_Up); Byte < (uint64_t) Bits2Bytes(ArraySizeInBits, RoundingType_Down); Byte++) {
                    Array[Byte]      = 0;
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %lld of %lld bits"), BytesWritten * 8, Bytes2Write * 8);
            }
        } else if (Output == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitOutput Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    /* BitBuffer */

    /* File Operations */
    static UTF8 *UTF8_CreateModeString(FileIO_FileModes Mode) {
        UTF8 *ModeString = NULL;
        if ((Mode & FileMode_Read) == FileMode_Read) {
            if ((Mode & FileMode_Binary) == FileMode_Binary) {
                ModeString = PlatformIO_Literal(UTF8*, char*, u8"rb");
            } else if ((Mode & FileMode_Text) == FileMode_Text) {
                ModeString = PlatformIO_Literal(UTF8*, char*, u8"r");
            }
        } else if ((Mode & FileMode_Write) == FileMode_Write) {
            if ((Mode & FileMode_Append) == FileMode_Append) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"ab");
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"a");
                }
            } else {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"wb");
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = PlatformIO_Literal(UTF8*, char*, u8"w");
                }
            }
        }
        return ModeString;
    }

    static UTF16 *UTF16_CreateModeString(FileIO_FileModes Mode) {
        UTF16 *ModeString = NULL;
        if ((Mode & FileMode_Read) == FileMode_Read) {
            if ((Mode & FileMode_Binary) == FileMode_Binary) {
                ModeString = PlatformIO_Literal(UTF16*, char*, UTF16String("rb"));
            } else if ((Mode & FileMode_Text) == FileMode_Text) {
                ModeString = PlatformIO_Literal(UTF16*, char*, UTF16String("r"));
            }
        } else if ((Mode & FileMode_Write) == FileMode_Write) {
            if ((Mode & FileMode_Append) == FileMode_Append) {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = PlatformIO_Literal(UTF16*, char*, UTF16String("ab"));
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = PlatformIO_Literal(UTF16*, char*, UTF16String("a"));
                }
            } else {
                if ((Mode & FileMode_Binary) == FileMode_Binary) {
                    ModeString = PlatformIO_Literal(UTF16*, char*, UTF16String("wb"));
                } else if ((Mode & FileMode_Text) == FileMode_Text) {
                    ModeString = PlatformIO_Literal(UTF16*, char*, UTF16String("w"));
                }
            }
        }
        return ModeString;
    }

    FILE *FileIO_OpenUTF8(PlatformIO_Immutable(UTF8 *) Path8, FileIO_FileModes Mode) {
        FILE *File = NULL;
        if (Path8 != NULL && Mode != FileMode_Unspecified) {
            bool Path8HasBOM = UTF8_HasBOM(Path8);
            int  ErrorCode   = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            UTF8 *Mode8      = UTF8_CreateModeString(Mode);
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifdef __STDC_LIB_EXT1__
            if (Path8HasBOM) {
                ErrorCode = fopen_s(&File, &Path8[UTF8BOMSizeInCodeUnits], Mode8);
            } else {
                ErrorCode = fopen_s(&File, Path8, Mode8);
            }
            if (ErrorCode != 0) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't open \'%s\' got error code: %u"), Path8, ErrorCode);
            }
#else
            if (Path8HasBOM) {
                File = fopen(&Path8[UTF8BOMSizeInCodeUnits], Mode8);
            } else {
                File = fopen(Path8, Mode8);
            }
#endif /* __STDC_LIB_EXT1__ */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            if (Path8HasBOM) {
                File = fopen(reinterpret_cast<const char *>(&Path8[UTF8BOMSizeInCodeUnits]), reinterpret_cast<const char *>(Mode8));
            } else {
                File = fopen(reinterpret_cast<const char *>(Path8), reinterpret_cast<const char *>(Mode8));
            }
#endif /* PlatformIO_Language */
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            PlatformIO_Immutable(UTF16 *) Path16 = UTF8_Convert(Path8);
            PlatformIO_Immutable(UTF16 *) Mode16 = UTF16_CreateModeString(Mode);
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifdef __STDC_LIB_EXT1__
            if (Path8HasBOM) {
                ErrorCode = _wfopen_s(&File, &Path16[UTF16BOMSizeInCodeUnits], Mode16);
            } else {
                ErrorCode = _wfopen_s(&File, Path16, Mode16);
            }
#else
            if (Path8HasBOM) {
                File = _wfopen(&Path16[UTF16BOMSizeInCodeUnits], Mode16);
            } else {
                File = _wfopen(Path16, Mode16);
            }
#endif /* __STDC_LIB_EXT1__ */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            if (UTF8_HasBOM(Path8)) {
                File = fopen(reinterpret_cast<const char *>(&Path16[UTF16BOMSizeInCodeUnits]), reinterpret_cast<const char *>(Mode16));
            } else {
                File = fopen(reinterpret_cast<const char *>(&Path16), reinterpret_cast<const char *>(Mode16));
            }
#endif /* PlatformIO_Language */
#endif /* TargetOS */
            if (File != NULL) {
                setvbuf(File, NULL, _IONBF, 0);
            }

            if (ErrorCode != 0) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't open %s, got Error: %u"), Path8, ErrorCode);
            }
        }
        return File;
    }

    FILE *FileIO_OpenUTF16(PlatformIO_Immutable(UTF16 *) Path16, FileIO_FileModes Mode) {
        FILE *File            = NULL;
        if (Path16 != NULL && Mode != FileMode_Unspecified) {
            bool Path16HasBOM = UTF16_HasBOM(Path16);
            int  ErrorCode    = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            PlatformIO_Immutable(UTF8 *) Path8 = UTF16_Convert(Path16);
            PlatformIO_Immutable(UTF8 *) Mode8 = UTF8_CreateModeString(Mode);
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifdef __STDC_LIB_EXT1__
            if (Path16HasBOM) {
                ErrorCode = fopen_s(&File, &Path8[UTF8BOMSizeInCodeUnits], Mode8);
            } else {
                ErrorCode = fopen_s(&File, Path8, Mode8);
            }
#else
            if (Path16HasBOM) {
                File = fopen(&Path8[UTF8BOMSizeInCodeUnits], Mode8);
            } else {
                File = fopen(Path8, Mode8);
            }
#endif /* __STDC_LIB_EXT1__ */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            if (Path16HasBOM) {
                File = fopen(reinterpret_cast<const char *>(&Path8[UTF8BOMSizeInCodeUnits]), reinterpret_cast<const char *>(Mode8));
            } else {
                File = fopen(reinterpret_cast<const char *>(Path8), reinterpret_cast<const char *>(Mode8));
            }
#endif /* PlatformIO_Language */
            free(Path8);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            PlatformIO_Immutable(UTF16 *) Mode16 = UTF16_CreateModeString(Mode);
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
#ifdef __STDC_LIB_EXT1__
            if (Path16HasBOM) {
                ErrorCode = _wfopen_s(&File, &Path16[UTF16BOMSizeInCodeUnits], Mode16);
            } else {
                ErrorCode = _wfopen_s(&File, Path16, Mode16);
            }
#else
            if (Path16HasBOM) {
                File = _wfopen(&Path16[UTF16BOMSizeInCodeUnits], Mode16);
            } else {
                File = _wfopen(Path16, Mode16);
            }
#endif /* __STDC_LIB_EXT1__ */
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            if (Path16HasBOM) {
                File = fopen(reinterpret_cast<const char *>(&Path16[UTF16BOMSizeInCodeUnits]), reinterpret_cast<const char *>(Mode16));
            } else {
                File = fopen(reinterpret_cast<const char *>(&Path16), reinterpret_cast<const char *>(Mode16));
            }
#endif /* PlatformIO_Language */
#endif /* TargetOS */
            if (File != NULL) {
                setvbuf(File, NULL, _IONBF, 0);
            }

            if (ErrorCode != 0) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't open %s, got Error: %u"), Path8, ErrorCode);
            }
        }
        return File;
    }

    TextIO_StringTypes FileIO_GetFileOrientation(PlatformIO_Immutable(FILE *) File) {
        TextIO_StringTypes StringType       = StringType_Unspecified;
        int Orientation                     = fwide(PlatformIO_Literal(FILE*, PlatformIO_Immutable(FILE*), File), 0);
        if (Orientation < 0) {
            StringType                      = StringType_UTF8;
        } else if (Orientation > 0) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            StringType                      = StringType_UTF32;
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            StringType                      = StringType_UTF16;
#endif
        }
        return StringType;
    }

    uint64_t FileIO_GetSize(const FILE *File) {
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

    uint64_t FileIO_Read(PlatformIO_Immutable(FILE *) File2Read, void *Buffer, uint8_t BufferElementSize, uint64_t Elements2Read) {
        uint64_t BytesRead = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
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

    bool FileIO_Seek(PlatformIO_Immutable(FILE *) File2Seek, int64_t SeekSizeInBytes, FileIO_SeekTypes SeekType) {
        // Always open Streams in Binary Mode, not Text Mode; we'll handle Text conversion ourselves
        bool SeekingWasSucessful = No;
        bool SuccessIsZero       = 1;
        if (SeekType == SeekType_Beginning) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SeekingWasSucessful = fseeko(File2Seek, SeekSizeInBytes, SEEK_SET);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SeekingWasSucessful = fseeko(File2Seek, reinterpret_cast<off_t>(SeekSizeInBytes), SEEK_SET);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SeekingWasSucessful = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_SET);
#endif
        } else if (SeekType == SeekType_Current) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
            SeekingWasSucessful = fseeko(File2Seek, SeekSizeInBytes, SEEK_CUR);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
            SeekingWasSucessful = fseeko(File2Seek, reinterpret_cast<off_t>(SeekSizeInBytes), SEEK_CUR);
#endif
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
            SeekingWasSucessful = _fseeki64(File2Seek, SeekSizeInBytes, SEEK_CUR);
#endif
        } else if (SeekType == SeekType_End) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Seeking failed"));
        }
        return !SeekingWasSucessful;
    }

    uint64_t FileIO_Write(PlatformIO_Immutable(FILE *) File2Write, PlatformIO_Immutable(void *) Buffer, uint8_t BufferElementSize, uint64_t Elements2Write) {
        uint64_t BytesWritten = 0;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        BytesWritten = fwrite(Buffer, BufferElementSize, Elements2Write, File2Write);
#elif (PlatformIO_TargetOS == PlatformIOWindowsOS)
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

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
