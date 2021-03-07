#include "../include/BufferIO.h"          /* Included for our declarations */
#include "../include/AsynchronousIO.h"    /* Included for AsyncIOStream */
#include "../include/CryptographyIO.h"    /* Included for SecureRNG_GenerateInteger for GUUID_Generate */
#include "../include/MathIO.h"            /* Included for Integer functions */
#include "../include/TextIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/TextIO/LogIO.h"      /* Included for Logging */
#include "../include/TextIO/StringIO.h"   /* Included for StringIO's declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum BufferIO_Constants {
        GUUIDString_Size                = 20,
        BinaryGUUID_Size                = 16,
    } BufferIO_Constants;
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        AsyncIOStream *Input;
        AsyncIOStream *Output;
        uint8_t      *Buffer;
        uint64_t      BitOffset;
        uint64_t      NumBits;
    } BitBuffer;
    
    BitBuffer *BitBuffer_Init(uint64_t BitBufferSize) {
        BitBuffer *BitB                  = (BitBuffer*) calloc(1, sizeof(BitBuffer));
        if (BitB != NULL && BitBufferSize > 0) {
            BitB->Buffer                 = (uint8_t*) calloc(BitBufferSize, sizeof(uint8_t));
            if (BitB->Buffer != NULL) {
                BitB->NumBits            = Bytes2Bits(BitBufferSize);
            } else {
                BitBuffer_Deinit(BitB);
                BitB                     = NULL;
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate %lld bits for BitBuffer's buffer"), BitBufferSize);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate BitBuffer"));
        }
        return BitB;
    }

    void BitBuffer_SetInputStream(BitBuffer *BitB, AsyncIOStream *Input) {
        if (BitB != NULL && Input != NULL) {
            BitB->Input = Input;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Input == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("AsyncIOStream Pointer is NULL"));
        }
    }

    void BitBuffer_SetOutputStream(BitBuffer *BitB, AsyncIOStream *Output) {
        if (BitB != NULL && Output != NULL) {
            BitB->Input = Output;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Output == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("AsyncIOStream Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetSize(BitBuffer *BitB) {
        uint64_t BitBufferSize = 0ULL;
        if (BitB != NULL) {
            BitBufferSize      = BitB->NumBits;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return BitBufferSize;
    }
    
    void BitBuffer_SetSize(BitBuffer *BitB, uint64_t Bits) {
        if (BitB != NULL) {
            BitB->NumBits = Bits;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB != NULL) {
            Position = BitB->BitOffset;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Position;
    }
    
    void BitBuffer_SetPosition(BitBuffer *BitB, uint64_t Offset) {
        if (BitB != NULL) {
            BitB->BitOffset = Offset;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetBitsFree(BitBuffer *BitB) {
        uint64_t BitsFree = 0ULL;
        if (BitB != NULL) {
            BitsFree      = BitB->NumBits - BitB->BitOffset;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return BitsFree;
    }

    uint8_t *BitBuffer_GetArray(BitBuffer *BitB) {
        uint8_t *Buffer = NULL;
        if (BitB != NULL) {
            Buffer      = BitB->Buffer;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Buffer;
    }

    void BitBuffer_SetArray(BitBuffer *BitB, uint8_t *Buffer, uint64_t BufferSizeInBytes) {
        if (BitB != NULL && Buffer != NULL && BufferSizeInBytes > 0) {
            BitB->Buffer  = Buffer;
            BitB->NumBits = (BufferSizeInBytes * 8);
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Buffer == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Buffer Pointer is NULL"));
        } else if (BufferSizeInBytes == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BufferSizeInBytes is 0"));
        }
    }
    
    bool BitBuffer_IsAligned(BitBuffer *BitB, uint8_t AlignmentSize) {
        bool AlignmentStatus    = No;
        if (BitB != NULL && AlignmentSize >= 1 && AlignmentSize % 2 == 0) {
            if (BitB->BitOffset % ((uint64_t) AlignmentSize * 8) == 0) {
                AlignmentStatus = Yes;
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize != 1 || AlignmentSize % 2 != 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("AlignmentSize: %d isn't 1 or an integer power of 2"), AlignmentSize);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, uint8_t AlignmentSize) {
        if (BitB != NULL && (AlignmentSize == 1 || AlignmentSize % 2 == 0)) {
            int64_t  AlignmentSizeInBits = Bytes2Bits(AlignmentSize);
            int64_t  Bits2Align          = AlignmentSizeInBits - (BitB->BitOffset % AlignmentSizeInBits);
            if (BitB->BitOffset + Bits2Align > BitB->NumBits) {
                uint8_t *Buffer_Old      = BitB->Buffer;
                BitB->Buffer             = (uint8_t*) realloc(BitB->Buffer, Bits2Bytes(RoundingType_Up, BitB->NumBits + Bits2Align));
                if (BitB->Buffer != NULL) {
                    free(Buffer_Old);
                } else {
                    BitB->Buffer         = Buffer_Old;
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Realloc failed"));
                }
                BitB->NumBits           += Bits2Align;
            }
            BitB->BitOffset             += Bits2Align;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize == 1 || AlignmentSize % 2 == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("AlignmentSize: %d isn't a multiple of 2"), AlignmentSize);
        }
    }
    
    void BitBuffer_Seek(BitBuffer *BitB, int64_t Bits2Seek) {
        if (BitB != NULL) {
            if (Bits2Seek > 0) {
                BitB->BitOffset += Bits2Seek;
            } else {
                BitB->BitOffset  -= AbsoluteI(Bits2Seek);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void BitBuffer_Read(BitBuffer *BitB) {
        if (BitB != NULL) {
            // save up to 1 byte
            if (BitB->BitOffset != 0) {
                uint64_t BitOffset  = BitB->BitOffset; // Saved for restoration
                uint64_t NumBits    = BitB->NumBits;
                uint64_t Bytes2Save = Bits2Bytes(RoundingType_Up, BitOffset);
                uint64_t BufferSize = Bits2Bytes(RoundingType_Up, NumBits);

                for (uint64_t Byte2Keep = BufferSize - Bytes2Save; Byte2Keep < BufferSize; Byte2Keep++) {
                    BitB->Buffer[BufferSize - (Bytes2Save + Byte2Keep)] = BitB->Buffer[Byte2Keep];
                }

                // Now we need to read in BufferSize - Bytes2Save, starting the array at Bytes2Save
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
                struct sigevent SignalEvent = {
                    .sigev_notify            = 0,
                    .sigev_signo             = 0,
                    .sigev_value             = 0,
                    .sigev_notify_function   = 0,
                    .sigev_notify_attributes = 0,
                };
                /*
                 struct aiocb Async = {
                 .aio_fildes     = AsyncIOStream_GetDescriptor(BitB->Input),
                 .aio_offset     = AsyncIOStream_GetPosition(BitB->Input),
                 .aio_buf        = &BitB->Buffer[Bytes2Save],
                 .aio_nbytes     = BufferSize - Bytes2Save,
                 // aio_reqprio  = Request Priority
                 .aio_sigevent   = SignalEvent,
                 .aio_lio_opcode = 0, // What operation are we doing?
                 };
                 aio_read(&Async);
                 */
                AsyncIOStream_Read(BitB->Input, BitB->Buffer, 1, Bits2Bytes(RoundingType_Down, BitB->NumBits));
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
                // IO completion ports
#endif

                /*
                 Async is Preferred, I've got an idea to optimize this while keeping the data correct.

                 Just return after the read or write has been cue'd.

                 we only need to worry about the buffer being correct if a call is made to Read or Write to the BitBuffer, so handle that check or loop or whatever it may be in that function.

                 We may need to have our own signal handler here to support different platforms...

                 Well, those are for the future
                 */




                /*
                 Ok, so, round up the bits to full bytes, copy that many bytes from the end of the buffer to the beginning.
                 then read in the new data, but what if there were some bits left over?
                 We have to shift everything?
                 Well, don't worry about that too much because the Write function only allows us to write at a byte resolution.

                 Also, we should consider having a function specifically for moving data in an aray because this is really common

                 Just, where should it go?
                 */
            }
        }
    }
    
    uint8_t BitBuffer_Erase(BitBuffer *BitB, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (BitB != NULL) {
            uint64_t BufferSize = Bits2Bytes(RoundingType_Up, BitB->NumBits);
            for (uint64_t Byte = 0ULL; Byte < BufferSize; Byte++) {
                BitB->Buffer[Byte] = NewValue;
            }
            Verification = BitB->Buffer[0];
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Verification;
    }
    
    void BitBuffer_Resize(BitBuffer *BitB, uint64_t NewSize) {
        if (BitB != NULL && NewSize * 8 >= BitB->BitOffset) {
            uint8_t *Buffer_Old = BitB->Buffer;
            BitB->Buffer        = (uint8_t*) realloc(BitB->Buffer, NewSize);
            if (BitB->Buffer != NULL) {
                BitB->BitOffset = 0;
                BitB->NumBits   = NewSize * 8;
                free(Buffer_Old);
            } else {
                BitB->Buffer    = Buffer_Old;
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Realloc failed"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Copy(BitBuffer *Source, BitBuffer *Destination, uint64_t BitStart, uint64_t BitEnd) {
        if (Source != NULL && Destination != NULL && BitStart < BitEnd && BitStart <= Source->NumBits && BitEnd <= Source->NumBits) {
            uint64_t NumBits2Copy = BitEnd - BitStart;
            if (BitStart % 8 == 0 && BitEnd % 8 == 0 && NumBits2Copy % 8 == 0) {
                Destination->NumBits = NumBits2Copy;
                for (uint64_t Byte = BitStart / 8; Byte < BitEnd / 8; Byte++) {
                    Destination->Buffer[Byte - (BitStart / 8)] = Source->Buffer[Byte];
                }
            } else {
                for (uint64_t Bit = BitStart; Bit < BitEnd / 8; Bit++) {
                    Destination->Buffer[Bit / 8] = Source->Buffer[Bit / 8];
                }
            }
        } else if (Source == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Source Pointer is NULL"));
        } else if (Destination == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Destination Pointer is NULL"));
        } else if (BitStart >= BitEnd) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitStart %lld is greater than or equal to BitEnd %lld"), BitStart, BitEnd);
        } else if (BitStart >= Source->NumBits || BitEnd >= Source->NumBits) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitStart %lld or BitEnd %lld is greater than there are bits in Source %lld"), BitStart, BitEnd, Source->NumBits);
        }
    }

    void BitBuffer_ReadStream(BitBuffer *BitB) {
        if (BitB != NULL) {
            uint64_t ArraySizeInBits = BitBuffer_GetSize(BitB);
            uint64_t ArrayOffset     = BitBuffer_GetPosition(BitB);
            uint8_t *Array           = BitBuffer_GetArray(BitB);
            uint64_t Bytes2Read      = Bits2Bytes(RoundingType_Down, ArraySizeInBits - ArrayOffset);
            uint8_t  Bits2Save       = ArrayOffset % 8;
            if (Bits2Save > 0) {
                Array[0]             = 0;
                uint8_t Saved        = Array[Bytes2Read + 1] & (Exponentiate(2, Bits2Save) - 1); // Todo: Add shift
                Array[0]             = Saved;
                BitBuffer_SetPosition(BitB, Bits2Save);
                for (uint64_t Byte   = (uint64_t) Bits2Bytes(RoundingType_Down, ArrayOffset); Byte < (uint64_t) Bits2Bytes(RoundingType_Down, ArraySizeInBits - ArrayOffset); Byte++) {
                    Array[Byte]      = 0;
                }
            }
            uint64_t BytesRead       = Bytes2Read;
            //BytesRead                = FileIO_Read(Input->FileID, Array, sizeof(Array[0]), Bytes2Read);
            if (BytesRead == Bytes2Read) {
                BitBuffer_SetSize(BitB, BytesRead + ArrayOffset);
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Num bytes read %llu does not match num bytes requested %llu"), BytesRead, Bytes2Read);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void BitBuffer_WriteStream(BitBuffer *BitB) {
        if (BitB != NULL) {
            uint64_t ArraySizeInBits = BitBuffer_GetSize(BitB);
            uint64_t ArrayOffset     = BitBuffer_GetPosition(BitB);
            uint8_t *Array           = BitBuffer_GetArray(BitB);
            uint64_t Bytes2Write     = Bits2Bytes(RoundingType_Down, BitBuffer_GetPosition(BitB));
            uint64_t Bits2Keep       = ArrayOffset % 8;
            uint64_t   BytesWritten    = Bytes2Write; // Just to get it to compile to test core dumps
                                                      //uint64_t BytesWritten    = FileIO_Write(Output->FileID, Array, sizeof(uint8_t), Bytes2Write);
            if (BytesWritten == Bytes2Write) {
                Array[0]             = 0;
                Array[0]             = Array[Bytes2Write + 1] & (Exponentiate(2, Bits2Keep) << (8 - Bits2Keep));
                BitBuffer_SetPosition(BitB, Bits2Keep + 1);
                for (uint64_t Byte = (uint64_t) Bits2Bytes(RoundingType_Up, ArrayOffset); Byte < (uint64_t) Bits2Bytes(RoundingType_Down, ArraySizeInBits); Byte++) {
                    Array[Byte]      = 0;
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %lld of %lld bits"), BytesWritten * 8, Bytes2Write * 8);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static uint64_t BitBuffer_Extract_FarByte_FarBit(BitBuffer *BitB, uint8_t NumBits) {
        uint64_t Extracted = 0ULL;
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain = BitsAvailableInByte(BitB->BitOffset + NumBits);
            uint8_t  Bits2Get           = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  Shift              = 8 - Bits2Get;
            uint8_t  BitMask            = (uint8_t) (Exponentiate(2, Bits2Get) - 1) << Shift;
            uint64_t Byte               = Bits2Bytes(RoundingType_Down, BitB->BitOffset + NumBits);
            uint8_t  ExtractedBits      = BitB->Buffer[Byte] & BitMask;
            uint8_t  ApplyBits          = ExtractedBits >> Shift;

            Extracted                 <<= Bits2Get;
            Extracted                  |= ApplyBits;

            NumBits                    -= Bits2Get;
            BitB->BitOffset            += Bits2Get;
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_FarByte_NearBit(BitBuffer *BitB, uint8_t NumBits) {
        uint64_t Extracted = 0ULL;
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain = BitsAvailableInByte(BitB->BitOffset + NumBits);
            uint8_t  Bits2Get           = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  Shift              = 8 - Bits2Get;
            uint8_t  BitMask            = (uint8_t) (Exponentiate(2, Bits2Get) - 1) << Shift;
            uint64_t Byte               = Bits2Bytes(RoundingType_Down, BitB->BitOffset + NumBits);
            uint8_t  ExtractedBits      = BitB->Buffer[Byte] & BitMask;
            uint8_t  ApplyBits          = (uint8_t) (ExtractedBits << Shift);

            Extracted                 <<= Bits2Get;
            Extracted                  |= ApplyBits;

            NumBits                    -= Bits2Get;
            BitB->BitOffset            += Bits2Get;
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_NearByte_FarBit(BitBuffer *BitB, uint8_t NumBits) {
        uint64_t Extracted = 0ULL;
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain = BitsAvailableInByte(BitB->BitOffset);
            uint8_t  Bits2Get           = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  BufferShift        = 8 - Bits2Get;
            uint8_t  BufferMask         = (Exponentiate(2, Bits2Get) - 1) << BufferShift;
            uint8_t  ApplyBits          = BitB->Buffer[Bits2Bytes(RoundingType_Down, BitB->BitOffset)] & BufferMask;
            uint8_t  ValueShift         = NumBits - NumBits;

            Extracted                 <<= Bits2Get;
            Extracted                  |= ApplyBits;

            NumBits                    -= Bits2Get;
            BitB->BitOffset            += Bits2Get;
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_NearByte_NearBit(BitBuffer *BitB, uint8_t NumBits) {
        uint64_t Extracted = 0ULL;
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain = BitsAvailableInByte(BitB->BitOffset);
            uint8_t  Bits2Get           = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  Shift              = 8 - Bits2Get;
            uint8_t  BitMask            = (uint8_t) (Exponentiate(2, Bits2Get) - 1) << Shift;
            uint64_t Byte               = Bits2Bytes(RoundingType_Down, BitB->BitOffset);
            uint8_t  ApplyBits          = BitB->Buffer[Byte] & BitMask;
            ApplyBits                 >>= Shift;

            Extracted                 <<= Bits2Get;
            Extracted                  |= ApplyBits;

            NumBits                    -= Bits2Get;
            BitB->BitOffset            += Bits2Get;
        }
        return Extracted;
    }
    
    static void BitBuffer_Append_FarByte_FarBit(BitBuffer *BitB, uint8_t NumBits, uint64_t Data2Append) {
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain = BitsAvailableInByte(BitB->BitOffset);
            uint8_t  NumBits2Append     = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  ExtractBitMask     = (uint8_t) (Exponentiate(2, NumBits2Append) - 1);
            uint8_t  ExtractedBits      = (Data2Append & ExtractBitMask);
            uint64_t Byte               = Bits2Bytes(RoundingType_Down, BitB->BitOffset + NumBits);
            BitB->Buffer[Byte]         |= ExtractedBits;
            NumBits                    -= NumBits2Append;
            BitB->BitOffset            += NumBits2Append;
        }
    }
    
    static void BitBuffer_Append_FarByte_NearBit(BitBuffer *BitB, uint8_t NumBits, uint64_t Data2Append) {
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain = BitsAvailableInByte(BitB->BitOffset);
            uint8_t  NumBits2Append     = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  Shift              = BitsByteCanContain - NumBits2Append;
            uint8_t  ExtractBitMask     = (uint8_t) (Exponentiate(2, NumBits2Append) - 1) << Shift;
            uint8_t  ExtractedBits      = (Data2Append & ExtractBitMask) >> (NumBits - NumBits2Append);
            uint64_t Byte               = Bits2Bytes(RoundingType_Down, BitB->BitOffset + NumBits);
            BitB->Buffer[Byte]         |= ExtractedBits;
            NumBits                    -= NumBits2Append;
            BitB->BitOffset            += NumBits2Append;
        }
    }
    
    static void BitBuffer_Append_NearByte_FarBit(BitBuffer *BitB, uint8_t NumBits, uint64_t Data2Append) {
        /*
         Buffer = 0[SSSSSSSS] 1[TTTTTTTT] 2[UUUUUUUU] 3[VVVVVVVV] 4[WWWWWWWW] 5[XXXXXXXX] 6[YYYYYYYY] 7[ZZZZZZZZ]

         Append = 0[SSSSSSSS] 1[TTTTTTTT] 2[UUUUUUUU] 3[VVVVVVVV] 4[WWWWWWWW] 5[XXXXXXXX] 6[YYYYYYYY] 7[ZZZZZZZZ]

         The goal here is to extract data from Append in such a way that it can be written in the direction the function name specifies

         for example 0xFFFE for FLAC is in NearByte_FarBit order

         therefore the top byte should be extracted first and the remaining bits last, start at the top.

         So, we need to generate the mask, shift, extract the bits, then apply them.

         Another thing to realize it's a LOT more efficent to just jump the buffer to the end and write backwards than to do all of this shifting.

         So yeah.
         */
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain  = BitsAvailableInByte(BitB->BitOffset);
            uint8_t  NumBits2Append      = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  Mask                = Exponentiate(2, NumBits2Append) - 1;
            uint8_t  Shift               = BitsByteCanContain - NumBits2Append;
            uint8_t  ExtractedBits       = (Data2Append & Mask) >> Shift;
            uint64_t Byte                = Bits2Bytes(RoundingType_Down, BitB->BitOffset);
            BitB->Buffer[Byte]          |= ExtractedBits;
            NumBits                     -= NumBits2Append;
            BitB->BitOffset             += NumBits2Append;
        }
    }
    
    static void BitBuffer_Append_NearByte_NearBit(BitBuffer *BitB, uint8_t NumBits, uint64_t Data2Append) { // Less Specific to More Specific
        while (NumBits > 0) {
            uint8_t  BitsByteCanContain  = BitsAvailableInByte(BitB->BitOffset);
            uint8_t  NumBits2Append      = (uint8_t) Minimum(BitsByteCanContain, NumBits);
            uint8_t  MaskShift           = NumBits2Append - NumBits2Append;
            uint64_t Mask                = Exponentiate(2, NumBits2Append) - 1; // Mask is 1 bit short for 0x33 aka 6 bits
            uint8_t  ExtractedBits       = (Data2Append & Mask) >> MaskShift;
            uint64_t Byte                = Bits2Bytes(RoundingType_Down, BitB->BitOffset);
            BitB->Buffer[Byte]          |= ExtractedBits;
            NumBits                     -= NumBits2Append;
            BitB->BitOffset             += NumBits2Append;
        }
    }
    
    static UTF32 *Format_BitBuffer(BitBuffer *BitB, uint8_t Length) {
        UTF32 *BitBufferString = NULL;
        if (BitB != NULL && Length >= 1 && Length <= 64) {
            BitBuffer_Seek(BitB, BitB->BitOffset - Length);
            uint64_t Data    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Length);
            BitBufferString  = UTF32_Format(UTF32String("BitBuffer: %P, NumBits: %llu, BitOffset: %llu, Buffer: %llX"), &BitB, BitB->NumBits, BitB->BitOffset, Data);
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Length == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Length is zero, less than the minimum of 1"));
        } else if (Length > 64) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Length: %u is greater than 64 bits, the maximum"), Length);
        }
        return BitBufferString;
    }
    
    uint64_t BitBuffer_PeekBits(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, uint8_t NumBits) {
        uint64_t Extracted = 0;
        if (BitB != NULL && NumBits <= 64 && (NumBits <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == ByteOrder_LSByteIsNearest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    Extracted           = BitBuffer_Extract_NearByte_NearBit(BitB, NumBits);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    Extracted           = BitBuffer_Extract_NearByte_FarBit(BitB, NumBits);
                }
            } else if (ByteOrder == ByteOrder_LSByteIsFarthest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    Extracted           = BitBuffer_Extract_FarByte_NearBit(BitB, NumBits);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    Extracted           = BitBuffer_Extract_FarByte_FarBit(BitB, NumBits);
                }
            }
            BitB->BitOffset   -= NumBits;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (NumBits > 64 || (NumBits > (BitB->BitOffset - BitB->NumBits))) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Bits2Peek %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_PeekBits can satisfy 0-64"), NumBits, BitB->BitOffset);
        }
        return Extracted;
    }
    
    uint64_t BitBuffer_ReadBits(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, uint8_t NumBits) {
        uint64_t Extracted = 0ULL;
        if (BitB != NULL && NumBits <= 64 && NumBits <= (BitB->NumBits - BitB->BitOffset)) {
            if (ByteOrder == ByteOrder_LSByteIsNearest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    Extracted = BitBuffer_Extract_NearByte_NearBit(BitB, NumBits);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    Extracted = BitBuffer_Extract_NearByte_FarBit(BitB, NumBits);
                }
            } else if (ByteOrder == ByteOrder_LSByteIsFarthest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    Extracted = BitBuffer_Extract_FarByte_NearBit(BitB, NumBits);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    Extracted = BitBuffer_Extract_FarByte_FarBit(BitB, NumBits);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (NumBits > 64 || NumBits <= (BitB->NumBits - BitB->BitOffset)) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits %d is greater than BitBuffer can provide %lld, or greater than can be satisfied 0-64"), NumBits, BitB->BitOffset);
        }
        return Extracted;
    }
    
    uint64_t BitBuffer_ReadUnary(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, BufferIO_UnaryTypes UnaryType, BufferIO_UnaryTerminators StopBit) {
        uint64_t Extracted = 0ULL;
        if (UnaryType == UnaryType_Count) {
            Extracted   += 1;
        }
        if (BitB != NULL) {
            uint8_t CurrentBit = StopBit ^ 1;
            while (CurrentBit != StopBit) {
                if (ByteOrder == ByteOrder_LSByteIsNearest) {
                    if (BitOrder == BitOrder_LSBitIsNearest) {
                        Extracted = BitBuffer_Extract_NearByte_NearBit(BitB, 1);
                    } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                        Extracted = BitBuffer_Extract_NearByte_FarBit(BitB, 1);
                    }
                } else if (ByteOrder == ByteOrder_LSByteIsFarthest) {
                    if (BitOrder == BitOrder_LSBitIsNearest) {
                        Extracted = BitBuffer_Extract_FarByte_NearBit(BitB, 1);
                    } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                        Extracted = BitBuffer_Extract_FarByte_FarBit(BitB, 1);
                    }
                }
                Extracted += 1;
            };
            BitBuffer_Seek(BitB, 1);
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Extracted;
    }
    
    uint64_t BitBuffer_GetUTF8StringSize(BitBuffer *BitB) {
        uint64_t StringSize               = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset       = BitBuffer_GetPosition(BitB);
            uint64_t Extracted            = 1;
            while (Extracted != PlatformIO_NULLTerminator) {
                Extracted                 = BitBuffer_Extract_FarByte_FarBit(BitB, 8);
                uint8_t  CodeUnitSize     = UTF8_GetCodePointSizeInCodeUnits((UTF8) Extracted);
                StringSize               += CodeUnitSize;
            }
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF8 *BitBuffer_ReadUTF8(BitBuffer *BitB, uint64_t StringSize) {
        UTF8 *ExtractedString                 = UTF8_Init(StringSize);
        UTF8 Extracted                        = 0;
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                Extracted                     = BitBuffer_Extract_FarByte_FarBit(BitB, 8);
                ExtractedString[CodeUnit]     = (UTF8) Extracted;
            }
        }
        return ExtractedString;
    }
    
    uint64_t BitBuffer_GetUTF16StringSize(BitBuffer *BitB) {
        uint64_t StringSize               = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset       = BitBuffer_GetPosition(BitB);
            uint64_t Extracted            = 1;
            while (Extracted != PlatformIO_NULLTerminator) {
                Extracted                 = BitBuffer_Extract_FarByte_FarBit(BitB, 16);
                uint8_t  CodeUnitSize     = UTF8_GetCodePointSizeInCodeUnits((UTF16) Extracted);
                StringSize               += CodeUnitSize;
            }
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF16 *BitBuffer_ReadUTF16(BitBuffer *BitB, uint64_t StringSize) {
        UTF16 *ExtractedString                = UTF16_Init(StringSize);
        UTF16 Extracted;
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                Extracted                     = BitBuffer_Extract_FarByte_FarBit(BitB, 16);
                ExtractedString[CodeUnit]     = (UTF16) Extracted;
            }
        }
        return ExtractedString;
    }
    
    uint8_t *BitBuffer_ReadGUUID(BitBuffer *BitB, BufferIO_GUUIDTypes GUUID2Read) {
        uint8_t *GUUID = NULL;
        if (GUUID2Read != GUUIDType_Unspecified && BitB != NULL && (BitBuffer_GetSize(BitB) - BitBuffer_GetPosition(BitB)) >= BinaryGUUID_Size) {
            if (GUUID2Read == GUUIDType_BinaryUUID || GUUID2Read == GUUIDType_BinaryGUID) {
                GUUID                    = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
                if (GUUID != NULL) {
                    for (uint8_t Byte = 0; Byte < BinaryGUUID_Size; Byte++) {
                        GUUID[Byte]      = (uint8_t) BitBuffer_Extract_FarByte_FarBit(BitB, 8);
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate GUUIDType_GUIDString"));
                }
            } else if (GUUID2Read == GUUIDType_UUIDString || GUUID2Read == GUUIDType_GUIDString) {
                if (GUUID != NULL) {
                    uint32_t Section1  = BitBuffer_Extract_FarByte_FarBit(BitB, 32);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section2  = BitBuffer_Extract_FarByte_FarBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section3  = BitBuffer_Extract_FarByte_FarBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section4  = BitBuffer_Extract_FarByte_FarBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint64_t Section5  = BitBuffer_Extract_FarByte_FarBit(BitB, 48);
                    for (uint8_t Byte = 0; Byte < GUUIDString_Size; Byte++) {
                        if (Byte <= 3) {
                            GUUID[Byte]  = Section1 & (0xFF << (Byte * 8));
                        } else if (Byte >= 5 && Byte <= 7) {
                            GUUID[Byte]  = Section2 & ((0xFF << (Byte / 4) * 8));
                        } else if (Byte >= 9 && Byte <= 11) {
                            GUUID[Byte]  = Section3 & ((0xFF << (Byte / 8) * 8));
                        } else if (Byte >= 13 && Byte <= 15) {
                            GUUID[Byte]  = Section4 & ((0xFF << (Byte / 12) * 8));
                        } else if (Byte >= 17 && Byte <= 21) {
                            GUUID[Byte]  = Section5 & ((0xFF << (Byte / 16) * 8));
                        } else {
                            GUUID[Byte]  = '-';
                        }
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate GUUIDType_UUIDString"));
                }
            }
        }
        return GUUID;
    }
    
    void BitBuffer_WriteBits(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, uint8_t NumBits2Write, uint64_t Bits2Write) {
        if (BitB != NULL) {
            if (ByteOrder == ByteOrder_LSByteIsNearest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    BitBuffer_Append_NearByte_NearBit(BitB, NumBits2Write, Bits2Write);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    BitBuffer_Append_NearByte_FarBit(BitB, NumBits2Write, Bits2Write);
                }
            } else if (ByteOrder == ByteOrder_LSByteIsFarthest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    BitBuffer_Append_FarByte_NearBit(BitB, NumBits2Write, Bits2Write);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    BitBuffer_Append_FarByte_FarBit(BitB, NumBits2Write, Bits2Write);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits2Write %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
        }
    }

    void BitBuffer_WriteBits2(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, uint8_t NumBits2Write, uint64_t Bits2Write) {
        if (BitB != NULL) {
            uint8_t NumBits                          = NumBits2Write;
            if (ByteOrder == ByteOrder_LSByteIsNearest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    while (NumBits > 0) {
                        uint8_t  BitsByteCanContain  = BitsAvailableInByte(BitB->BitOffset);
                        uint8_t  NumBits2Append      = (uint8_t) Minimum(BitsByteCanContain, NumBits);
                        uint8_t  MaskShift           = NumBits2Append - NumBits2Append;
                        uint64_t Mask                = (Exponentiate(2, NumBits2Append) - 1) << MaskShift;
                        uint8_t  ExtractedBits       = (Bits2Write & Mask) >> MaskShift;
                        uint64_t Byte                = Bits2Bytes(RoundingType_Down, BitB->BitOffset);
                        BitB->Buffer[Byte]          |= ExtractedBits;
                        NumBits                     -= NumBits2Append;
                        BitB->BitOffset             += NumBits2Append;
                    }
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    while (NumBits > 0) {
                        uint8_t  BitsByteCanContain   = BitsAvailableInByte(BitB->BitOffset);
                        uint8_t  Bits2Write          = (uint8_t) Minimum(BitsByteCanContain, NumBits);
                        uint8_t  Mask                = Exponentiate(2, Bits2Write) - 1;
                        uint8_t  Shift               = NumBits2Write - NumBits;
                        uint8_t  ExtractedBits       = (Bits2Write & Mask) >> Shift;
                        uint64_t Byte                = Bits2Bytes(RoundingType_Down, BitB->BitOffset);
                        BitB->Buffer[Byte]          |= ExtractedBits;
                        NumBits                     -= Bits2Write;
                        BitB->BitOffset             += NumBits;
                    }
                }
            } else if (ByteOrder == ByteOrder_LSByteIsFarthest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    while (NumBits > 0) {
                        uint8_t  BitsByteCanContain  = BitsAvailableInByte(BitB->BitOffset);
                        uint8_t  NumBits2Append      = (uint8_t) Minimum(BitsByteCanContain, NumBits);
                        uint8_t  Shift               = BitsByteCanContain - NumBits2Append;
                        uint8_t  ExtractBitMask      = (uint8_t) (Exponentiate(2, NumBits2Append) - 1) << Shift;
                        uint8_t  ExtractedBits       = (Bits2Write & ExtractBitMask) >> (NumBits - NumBits2Append);
                        uint64_t Byte                = Bits2Bytes(RoundingType_Down, BitB->BitOffset + NumBits);
                        BitB->Buffer[Byte]          |= ExtractedBits;
                        NumBits                     -= NumBits2Append;
                        BitB->BitOffset             += NumBits2Append;
                    }
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    while (NumBits > 0) {
                        uint8_t  BitsByteCanContain  = BitsAvailableInByte(BitB->BitOffset);
                        uint8_t  NumBits2Append      = (uint8_t) Minimum(BitsByteCanContain, NumBits);
                        uint8_t  ExtractBitMask      = (uint8_t) (Exponentiate(2, NumBits2Append) - 1);
                        uint8_t  ExtractedBits       = (Bits2Write & ExtractBitMask);
                        uint64_t Byte                = Bits2Bytes(RoundingType_Down, BitB->BitOffset + NumBits);
                        BitB->Buffer[Byte]          |= ExtractedBits;
                        NumBits                     -= NumBits2Append;
                        BitB->BitOffset             += NumBits2Append;
                    }
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits2Write %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
        }
    }
    
    void BitBuffer_WriteUnary(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, BufferIO_UnaryTypes UnaryType, BufferIO_UnaryTerminators StopBit, uint8_t UnaryBits2Write) {
        if (BitB != NULL) {
            uint8_t EndBit      = StopBit == UnaryTerminator_Zero ? 0 : 1;
            uint8_t Field2Write = UnaryBits2Write;
            if (UnaryType == UnaryType_Count) {
                Field2Write -= 1;
            }
            
            if (ByteOrder == ByteOrder_LSByteIsNearest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    BitBuffer_Append_FarByte_FarBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_FarByte_FarBit(BitB, 1, EndBit);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    BitBuffer_Append_FarByte_NearBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_FarByte_NearBit(BitB, 1, EndBit);
                }
            } else if (ByteOrder == ByteOrder_LSByteIsFarthest) {
                if (BitOrder == BitOrder_LSBitIsNearest) {
                    BitBuffer_Append_NearByte_FarBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_NearByte_FarBit(BitB, 1, EndBit);
                } else if (BitOrder == BitOrder_LSBitIsFarthest) {
                    BitBuffer_Append_NearByte_NearBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_NearByte_NearBit(BitB, 1, EndBit);
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteUTF8(BitBuffer *BitB, ImmutableString_UTF8 String2Write, BufferIO_StringTerminators WriteType) {
        if (BitB != NULL && String2Write != NULL) {
            uint64_t CodeUnit      = 0ULL;
            while (String2Write[CodeUnit] != PlatformIO_NULLTerminator) {
                BitBuffer_Append_NearByte_FarBit(BitB, UTF8CodeUnitSizeInBits, String2Write[CodeUnit]);
                CodeUnit          += 1;
            }
            if (WriteType == StringTerminator_NULL) {
                BitBuffer_Seek(BitB, 8); // Write the NULL terminator
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteUTF16(BitBuffer *BitB, ImmutableString_UTF16 String2Write, BufferIO_StringTerminators WriteType) {
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize = UTF16_GetStringSizeInCodeUnits(String2Write);
            if (WriteType == StringTerminator_NULL) {
                StringSize += UTF16CodeUnitSizeInBits / 8; // Size in bytes, not bits
            }

            int64_t  BitsAvailable = BitBuffer_GetBitsFree(BitB);
            uint64_t CodeUnit = 0ULL;
            if (BitsAvailable >= Bytes2Bits(StringSize)) {
                while (String2Write[CodeUnit] != PlatformIO_NULLTerminator) {
                    BitBuffer_Append_FarByte_FarBit(BitB, UTF16CodeUnitSizeInBits, String2Write[CodeUnit]);
                    CodeUnit += 1;
                }
                if (WriteType == StringTerminator_NULL) {
                    BitBuffer_Append_FarByte_FarBit(BitB, UTF16CodeUnitSizeInBits, 0); // NULL Terminator
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteGUUID(BitBuffer *BitB, BufferIO_GUUIDTypes GUUIDType, uint8_t *GUUID2Write) {
        if (BitB != NULL && GUUID2Write != NULL) {
            static const uint8_t GUUIDSizeInBits[4] = {168, 168, 128, 128};
            if (BitB->BitOffset + GUUIDSizeInBits[GUUIDType] <= BitB->NumBits) {
                uint8_t GUUIDSize = ((GUUIDType == GUUIDType_GUIDString || GUUIDType == GUUIDType_UUIDString) ? GUUIDString_Size : BinaryGUUID_Size);
                for (uint8_t Byte = 0; Byte < GUUIDSize; Byte++) {
                    BitBuffer_Append_FarByte_FarBit(BitB, 8, GUUID2Write[Byte]);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (GUUID2Write == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("GUUID2Write Pointer is NULL"));
        }
    }
    
    void BitBuffer_Deinit(BitBuffer *BitB) {
        if (BitB != NULL) {
            free(BitB->Buffer);
            free(BitB);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    /* BitBuffer Resource Management */
    /* End BitBuffer section */
    
    /* GUUID */
    uint8_t *GUUID_Generate(SecureRNG *Random, BufferIO_GUUIDTypes GUUIDType) {
        uint8_t *GUUID                   = 0;
        if (Random != NULL && GUUIDType != GUUIDType_Unspecified) {
            uint64_t LowBits             = SecureRNG_GenerateInteger(Random, 64);
            uint64_t HighBits            = SecureRNG_GenerateInteger(Random, 64);
            if (GUUIDType == GUUIDType_GUIDString || GUUIDType == GUUIDType_UUIDString) {
                GUUID                    = (uint8_t*) calloc(GUUIDString_Size, sizeof(uint8_t));
            } else if (GUUIDType == GUUIDType_BinaryGUID || GUUIDType == GUUIDType_BinaryUUID) {
                GUUID                    = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
            }
            if (GUUID != NULL) {
                if (GUUIDType == GUUIDType_GUIDString || GUUIDType == GUUIDType_UUIDString) { // String
                    for (uint8_t GUUIDByte = 0; GUUIDByte < GUUIDString_Size; GUUIDByte++) {
                        if (GUUIDByte != 4 && GUUIDByte != 7 && GUUIDByte != 10) {
                            if (GUUIDByte < 8) {
                                uint8_t Byte     = (LowBits  & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                                GUUID[GUUIDByte] = Byte;
                            } else {
                                uint8_t Byte     = (HighBits & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                                GUUID[GUUIDByte] = Byte;
                            }
                        }
                    }
                } else if (GUUIDType == GUUIDType_BinaryGUID || GUUIDType == GUUIDType_BinaryUUID) { // Binary
                    for (uint8_t GUUIDByte = 0; GUUIDByte < BinaryGUUID_Size; GUUIDByte++) {
                        if (GUUIDByte < 8) {
                            uint8_t Byte     = (LowBits  & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                            GUUID[GUUIDByte] = Byte;
                        } else {
                            uint8_t Byte     = (HighBits & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                            GUUID[GUUIDByte] = Byte;
                        }
                    }
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate GUUID"));
            }
        } else if (Random == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SecureRNG Pointer is NULL"));
        } else if (GUUIDType == GUUIDType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("GUUIDType_Unspecified is an invalid GUUIDType"));
        }
        return GUUID;
    }
    
    bool GUUID_Compare(BufferIO_GUUIDTypes Type2Compare, uint8_t *GUUID1, uint8_t *GUUID2) {
        uint8_t GUUIDSize       = ((Type2Compare == GUUIDType_GUIDString || Type2Compare == GUUIDType_UUIDString) ? BinaryGUUID_Size : BinaryGUUID_Size);
        bool GUUIDsMatch        = Yes;
        if (GUUID1 != NULL && GUUID2 != NULL && Type2Compare != GUUIDType_Unspecified) {
            for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < GUUIDSize; BinaryGUUIDByte++) {
                if (GUUID1[BinaryGUUIDByte] != GUUID2[BinaryGUUIDByte]) {
                    GUUIDsMatch = No;
                }
            }
        } else if (GUUID1 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("GUUID1 Pointer is NULL"));
        } else if (GUUID2 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("GUUID2 Pointer is NULL"));
        } else if (Type2Compare == GUUIDType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("GUUIDType_Unspecified is an invalid GUUID type"));
        }
        return GUUIDsMatch;
    }
    
    uint8_t *GUUID_Convert(BufferIO_GUUIDTypes InputType, BufferIO_GUUIDTypes OutputType, uint8_t *GUUID2Convert) {
        uint8_t  Dash = '-';
        uint8_t  OutputGUUIDSize = ((OutputType == GUUIDType_GUIDString || OutputType == GUUIDType_UUIDString) ? GUUIDString_Size : BinaryGUUID_Size);
        uint8_t *ConvertedGUUID  = (uint8_t*) calloc(OutputGUUIDSize, sizeof(uint8_t));
        if (ConvertedGUUID != NULL && GUUID2Convert != NULL && InputType != GUUIDType_Unspecified && OutputType != GUUIDType_Unspecified) {
            bool ByteOrderDiffers = (((InputType == GUUIDType_GUIDString && OutputType == GUUIDType_UUIDString) || (InputType == GUUIDType_UUIDString && OutputType == GUUIDType_GUIDString) || (InputType == GUUIDType_BinaryUUID && OutputType == GUUIDType_BinaryGUID) || (InputType == GUUIDType_BinaryGUID && OutputType == GUUIDType_BinaryUUID)) ? Yes : No);
            
            bool TypeDiffers      = (((InputType == GUUIDType_GUIDString && OutputType == GUUIDType_BinaryGUID) || (InputType == GUUIDType_BinaryGUID && OutputType == GUUIDType_GUIDString) || (InputType == GUUIDType_UUIDString && OutputType == GUUIDType_BinaryUUID) || (InputType == GUUIDType_BinaryUUID && OutputType == GUUIDType_UUIDString)) ? Yes : No);
            
            if (ByteOrderDiffers == Yes) {
                GUUID_Swap(InputType, GUUID2Convert);
            }
            
            if (TypeDiffers == Yes) {
                if ((InputType == GUUIDType_UUIDString || InputType == GUUIDType_GUIDString) && (OutputType == GUUIDType_BinaryUUID || OutputType == GUUIDType_BinaryGUID)) {
                    for (uint8_t StringByte = 0; StringByte < BinaryGUUID_Size; StringByte++) {
                        for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUID_Size; BinaryByte++) {
                            if (GUUID2Convert[StringByte] != Dash) {
                                ConvertedGUUID[BinaryByte] = GUUID2Convert[StringByte];
                            }
                        }
                    }
                } else if ((InputType == GUUIDType_BinaryUUID || InputType == GUUIDType_BinaryGUID) || (OutputType == GUUIDType_UUIDString || OutputType == GUUIDType_GUIDString)) {
                    for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUID_Size; BinaryByte++) {
                        for (uint8_t StringByte = 0; StringByte < BinaryGUUID_Size; StringByte++) {
                            if (BinaryByte != 4 && BinaryByte != 7 && BinaryByte != 10 && BinaryByte != 13) {
                                ConvertedGUUID[StringByte]  = GUUID2Convert[BinaryByte];
                            } else {
                                ConvertedGUUID[StringByte]  = Dash;
                            }
                        }
                    }
                }
            }
        } else if (ConvertedGUUID == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate ConvertedGUUID"));
        } else if (GUUID2Convert == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("GUUID Pointer is NULL"));
        } else if (InputType == GUUIDType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("InputType is invalid"));
        } else if (OutputType == GUUIDType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OutputType is invalid"));
        }
        return ConvertedGUUID;
    }
    
    uint8_t *GUUID_Swap(BufferIO_GUUIDTypes GUUIDType, uint8_t *GUUID2Swap) {
        uint8_t *SwappedGUUID = NULL;
        if (GUUIDType != GUUIDType_Unspecified && GUUIDType <= 4) {
            uint8_t Dash = '-';
            if (GUUIDType == GUUIDType_UUIDString || GUUIDType == GUUIDType_GUIDString) {
                SwappedGUUID          = (uint8_t*) calloc(GUUIDString_Size, sizeof(uint8_t));
                if (SwappedGUUID != NULL) {
                    SwappedGUUID[0]   = GUUID2Swap[3];
                    SwappedGUUID[1]   = GUUID2Swap[2];
                    SwappedGUUID[2]   = GUUID2Swap[1];
                    SwappedGUUID[3]   = GUUID2Swap[0];
                    
                    SwappedGUUID[4]   = Dash;
                    
                    SwappedGUUID[5]   = GUUID2Swap[6];
                    SwappedGUUID[6]   = GUUID2Swap[5];
                    
                    SwappedGUUID[7]   = Dash;
                    
                    SwappedGUUID[8]   = GUUID2Swap[9];
                    SwappedGUUID[9]   = GUUID2Swap[8];
                    
                    SwappedGUUID[10]  = Dash;
                    
                    SwappedGUUID[11]  = GUUID2Swap[12];
                    SwappedGUUID[12]  = GUUID2Swap[11];
                    
                    SwappedGUUID[13]  = Dash;
                    
                    for (uint8_t EndBytes = 13; EndBytes < GUUIDString_Size; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SwappedGUUID Pointer is NULL"));
                }
            } else if (GUUIDType == GUUIDType_BinaryUUID || GUUIDType == GUUIDType_BinaryGUID) {
                SwappedGUUID          = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
                if (SwappedGUUID != NULL) {
                    SwappedGUUID[0]   = GUUID2Swap[3];
                    SwappedGUUID[1]   = GUUID2Swap[2];
                    SwappedGUUID[2]   = GUUID2Swap[1];
                    SwappedGUUID[3]   = GUUID2Swap[0];
                    
                    SwappedGUUID[4]   = GUUID2Swap[5];
                    SwappedGUUID[5]   = GUUID2Swap[4];
                    
                    SwappedGUUID[6]   = GUUID2Swap[7];
                    SwappedGUUID[7]   = GUUID2Swap[6];
                    
                    SwappedGUUID[8]   = GUUID2Swap[9];
                    SwappedGUUID[9]   = GUUID2Swap[8];
                    for (uint8_t EndBytes = 10; EndBytes < BinaryGUUID_Size; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SwappedGUUID Pointer is NULL"));
                }
            }
        } else if (GUUIDType == GUUIDType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("GUUIDType_Unspecified is an invalid GUUID type"));
        } else if (GUUIDType > 4) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("GUUIDTypes are not ORable"));
        }
        return SwappedGUUID;
    }
    
    void GUUID_Deinit(uint8_t *GUUID) {
        free(GUUID);
    }
    /* GUUID */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
