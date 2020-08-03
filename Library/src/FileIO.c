#include "../include/FileIO.h"             /* Included for our declarations */
#include "../include/BufferIO.h"           /* Included for BitBuffer */
#include "../include/CryptographyIO.h"     /* Included for SecureRNG_GenerateInteger for GUUID_Generate */
#include "../include/MathIO.h"             /* Included for Bits2Bytes */
#include "../include/Private/Constants.h"  /* Included for BitMaskTables */
#include "../include/TextIO/FormatIO.h" /* Included for UTF32_Format */
#include "../include/TextIO/LogIO.h"    /* Included for Logging */
#include "../include/TextIO/StringIO.h" /* Included for StringIO's declarations */


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
            PlatformIO_Seek(Input->File, 0, SeekType_End);
            Input->FileSize     = (uint64_t) PlatformIO_GetSize(Input->File);
            PlatformIO_Seek(Input->File, 0, SeekType_Beginning);
            Input->FilePosition = (uint64_t) PlatformIO_GetSize(Input->File);
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
            PlatformIO_Close(Input->File);
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
            PlatformIO_Close(Output->File);
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
                uint8_t Saved        = Array[Bytes2Read + 1] & BitMaskTable[Bits2Save - 1];
                Array[0]             = Saved;
                BitBuffer_SetPosition(BitB, Bits2Save);
                for (uint64_t Byte   = (uint64_t) Bits2Bytes(ArrayOffset, RoundingType_Down); Byte < (uint64_t) Bits2Bytes(ArraySizeInBits - ArrayOffset, RoundingType_Down); Byte++) {
                    Array[Byte]      = 0;
                }
            }
            uint64_t BytesRead       = 0ULL;
            BytesRead                = PlatformIO_Read(Input->File, Array, sizeof(Array[0]), Bytes2Read);
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
            BytesRead                = PlatformIO_Read(Input->File, Array, sizeof(Array[0]), Bytes2Read);
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
            uint64_t BytesWritten    = PlatformIO_Write(Output->File, sizeof(Array[0]), Array, Bytes2Write);
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

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
