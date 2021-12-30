#include "../include/BufferIO.h"          /* Included for our declarations */

#include "../include/AsynchronousIO.h"    /* Included for AsyncIOStream */
#include "../include/CryptographyIO.h"    /* Included for InsecurePRNG_CreateInteger */
#include "../include/MathIO.h"            /* Included for Integer functions */
#include "../include/TextIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/TextIO/LogIO.h"      /* Included for Logging */
#include "../include/TextIO/StringIO.h"   /* Included for StringIO's declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        AsyncIOStream *Input;
        AsyncIOStream *Output;
        uint8_t       *Buffer;
        size_t         BitOffset;
        size_t         NumBits;
    } BitBuffer;
    
    BitBuffer *BitBuffer_Init(size_t BitBufferSize) {
        BitBuffer *BitB                  = (BitBuffer*) calloc(1, sizeof(BitBuffer));
        if (BitB != NULL && BitBufferSize > 0) {
            BitB->Buffer                 = (uint8_t*) calloc(BitBufferSize, sizeof(uint8_t));
            if (BitB->Buffer != NULL) {
                BitB->NumBits            = Bytes2Bits(BitBufferSize);
            } else {
                BitBuffer_Deinit(BitB);
                BitB                     = NULL;
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate %zu bits for BitBuffer's buffer"), BitBufferSize);
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
    
    size_t BitBuffer_GetSize(BitBuffer *BitB) {
        size_t BitBufferSize = 0;
        if (BitB != NULL) {
            BitBufferSize    = BitB->NumBits;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return BitBufferSize;
    }
    
    void BitBuffer_SetSize(BitBuffer *BitB, size_t NumBits) {
        if (BitB != NULL) {
            BitB->NumBits = NumBits;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    size_t BitBuffer_GetPosition(BitBuffer *BitB) {
        size_t Position = 0ULL;
        if (BitB != NULL) {
            Position = BitB->BitOffset;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Position;
    }
    
    void BitBuffer_SetPosition(BitBuffer *BitB, size_t OffsetInBits) {
        if (BitB != NULL) {
            BitB->BitOffset = OffsetInBits;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    size_t BitBuffer_GetBitsFree(BitBuffer *BitB) {
        size_t BitsFree = 0ULL;
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

    void BitBuffer_SetArray(BitBuffer *BitB, uint8_t *Buffer, size_t BufferSizeInBytes) {
        if (BitB != NULL && Buffer != NULL && BufferSizeInBytes > 0) {
            BitB->Buffer  = Buffer;
            BitB->NumBits = Bytes2Bits(BufferSizeInBytes);
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Buffer == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Buffer Pointer is NULL"));
        } else if (BufferSizeInBytes == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BufferSizeInBytes is 0"));
        }
    }
    
    bool BitBuffer_IsAligned(BitBuffer *BitB, size_t AlignmentSizeInBytes) {
        bool AlignmentStatus    = No;
        if (BitB != NULL && AlignmentSizeInBytes >= 1 && AlignmentSizeInBytes % 2 == 0) {
            if (BitB->BitOffset % AlignmentSizeInBytes == 0) {
                AlignmentStatus = Yes;
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (AlignmentSizeInBytes != 1 && AlignmentSizeInBytes % 2 != 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("AlignmentSizeInBytes: %zu isn't 1 or an integer power of 2"), AlignmentSizeInBytes);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, size_t AlignmentSizeInBytes) {
        if (BitB != NULL && (AlignmentSizeInBytes == 1 || AlignmentSizeInBytes % 2 == 0)) {
            size_t AlignmentSizeInBits   = Bytes2Bits(AlignmentSizeInBytes);
            size_t Bits2Align            = AlignmentSizeInBits - (BitB->BitOffset % AlignmentSizeInBits);
            if (BitB->BitOffset + Bits2Align > BitB->NumBits) {
                BitB->Buffer             = (uint8_t*) realloc(BitB->Buffer, Bits2Bytes(RoundingType_Up, BitB->NumBits + Bits2Align));
                BitB->NumBits           += Bits2Align;
            }
            BitB->BitOffset             += Bits2Align;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (AlignmentSizeInBytes != 1 && AlignmentSizeInBytes % 2 != 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("AlignmentSizeInBytes: %zu isn't a multiple of 2"), AlignmentSizeInBytes);
        }
    }
    
    void BitBuffer_Seek(BitBuffer *BitB, ssize_t Bits2Seek) {
        if (BitB != NULL) {
            if (Bits2Seek > 0) {
                BitB->BitOffset += Bits2Seek;
            } else {
                BitB->BitOffset -= AbsoluteI(Bits2Seek);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void BitBuffer_Read(BitBuffer *BitB) {
        if (BitB != NULL) {
            // save up to 1 byte
            if (BitB->BitOffset != 0) {
                size_t BitOffset  = BitB->BitOffset; // Saved for restoration
                size_t NumBits    = BitB->NumBits;
                size_t Bytes2Save = Bits2Bytes(RoundingType_Up, BitOffset);
                size_t BufferSize = Bits2Bytes(RoundingType_Up, NumBits);

                for (size_t Byte2Keep = BufferSize - Bytes2Save; Byte2Keep < BufferSize; Byte2Keep++) {
                    BitB->Buffer[BufferSize - (Bytes2Save + Byte2Keep)] = BitB->Buffer[Byte2Keep];
                }

                // Now we need to read in BufferSize - Bytes2Save, starting the array at Bytes2Save
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
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
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
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
    
    bool BitBuffer_Erase(BitBuffer *BitB, uint8_t NewValue) {
        bool ErasedBitBufferCompletely = false;
        if (BitB != NULL) {
            size_t NumBytesToErase  = Bits2Bytes(RoundingType_Up, BitB->NumBits);
            size_t NumBytesErased   = BufferIO_MemorySet8(BitB->Buffer, 0, NumBytesToErase);
            ErasedBitBufferCompletely = NumBytesErased == NumBytesToErase ? true : false;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return ErasedBitBufferCompletely;
    }
    
    void BitBuffer_Resize(BitBuffer *BitB, size_t NewSizeInBits) {
        if (BitB != NULL && Bytes2Bits(NewSizeInBits) >= BitB->BitOffset) {
            BitB->Buffer  = (uint8_t*) realloc(BitB->Buffer, Bits2Bytes(RoundingType_Up, NewSizeInBits));
            BitB->NumBits = Bytes2Bits(NewSizeInBits);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Copy(BitBuffer *Source, BitBuffer *Destination, size_t StartInBits, size_t EndInBits) {
        if (Source != NULL && Destination != NULL && StartInBits < EndInBits && StartInBits <= Source->NumBits && EndInBits <= Source->NumBits) {
            size_t NumBits2Copy = EndInBits - StartInBits;
            if (StartInBits % 8 == 0 && EndInBits % 8 == 0 && NumBits2Copy % 8 == 0) {
                Destination->NumBits = NumBits2Copy;
                for (size_t Byte = StartInBits / 8; Byte < EndInBits / 8; Byte++) {
                    Destination->Buffer[Byte - (StartInBits / 8)] = Source->Buffer[Byte];
                }
            } else {
                for (size_t Bit = StartInBits; Bit < EndInBits / 8; Bit++) {
                    Destination->Buffer[Bit / 8] = Source->Buffer[Bit / 8];
                }
            }
        } else if (Source == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Source Pointer is NULL"));
        } else if (Destination == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Destination Pointer is NULL"));
        } else if (StartInBits >= EndInBits) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StartInBits %zu is greater than or equal to EndInBits %zu"), StartInBits, EndInBits);
        } else if (StartInBits >= Source->NumBits || EndInBits >= Source->NumBits) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StartInBits %zu or EndInBits %zu is greater than there are bits in Source %zu"), StartInBits, EndInBits, Source->NumBits);
        }
    }

    void BitBuffer_ReadStream(BitBuffer *BitB) {
        if (BitB != NULL) {
            size_t ArraySizeInBits   = BitBuffer_GetSize(BitB);
            size_t ArrayOffset       = BitBuffer_GetPosition(BitB);
            uint8_t *Array           = BitBuffer_GetArray(BitB);
            size_t Bytes2Read        = Bits2Bytes(RoundingType_Down, ArraySizeInBits - ArrayOffset);
            size_t  Bits2Save        = ArrayOffset % 8;
            if (Bits2Save > 0) {
                Array[0]             = 0;
                uint8_t Saved        = Array[Bytes2Read + 1] & (Exponentiate(2, Bits2Save) - 1); // Todo: Add shift
                Array[0]             = Saved;
                BitBuffer_SetPosition(BitB, Bits2Save);
                for (size_t Byte = Bits2Bytes(RoundingType_Down, ArrayOffset); Byte < Bits2Bytes(RoundingType_Down, ArraySizeInBits - ArrayOffset); Byte++) {
                    Array[Byte]      = 0;
                }
            }
            size_t BytesRead         = AsyncIOStream_Read(BitB->Input, Array, sizeof(Array[0]), Bytes2Read);
            if (BytesRead == Bytes2Read) {
                BitBuffer_SetSize(BitB, BytesRead + ArrayOffset);
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BytesRead %zu does not match num bytes requested %zu"), BytesRead, Bytes2Read);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void BitBuffer_WriteStream(BitBuffer *BitB) {
        if (BitB != NULL) {
            size_t ArraySizeInBits   = BitBuffer_GetSize(BitB);
            size_t ArrayOffset       = BitBuffer_GetPosition(BitB);
            uint8_t *Array           = BitBuffer_GetArray(BitB);
            size_t Bytes2Write       = Bits2Bytes(RoundingType_Down, BitBuffer_GetPosition(BitB));
            size_t Bits2Keep         = ArrayOffset % 8;
            size_t BytesWritten      = AsyncIOStream_Write(BitB->Output, Array, sizeof(Array[0]), Bytes2Write);
            if (BytesWritten == Bytes2Write) {
                Array[0]             = 0;
                Array[0]             = Array[Bytes2Write + 1] & (Exponentiate(2, Bits2Keep) << (8 - Bits2Keep));
                BitBuffer_SetPosition(BitB, Bits2Keep + 1);
                for (size_t Byte = Bits2Bytes(RoundingType_Up, ArrayOffset); Byte < Bits2Bytes(RoundingType_Down, ArraySizeInBits); Byte++) {
                    Array[Byte]      = 0;
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Wrote %zu of %zu bits"), Bytes2Bits(BytesWritten), Bytes2Bits(Bytes2Write));
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
         BitOrder =  01234567
         Buffer   = [AAAAAAAA] [BBBBBBBB] [CCCCCCCC] [DDDDDDDD] [EEEEEEEE] [FFFFFFFF] [GGGGGGGG] [HHHHHHHH]
         Append   = [SSSSSSSS] [TTTTTTTT] [UUUUUUUU] [VVVVVVVV] [WWWWWWWW] [XXXXXXXX] [YYYYYYYY] [ZZZZZZZZ]
         
         NearByte_FarBit = start at byte S, and Bit 7
         
         So, if we need to write 6 bits and there's 6 bits available

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
            size_t Data      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Length);
            BitBufferString  = UTF32_Format(UTF32String("BitBuffer: %X, NumBits: %zu, BitOffset: %zu, Buffer: %zu"), &BitB, BitB->NumBits, BitB->BitOffset, Data);
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Bits2Peek %d is greater than BitBuffer can provide %zu, or greater than BitBuffer_PeekBits can satisfy 0-64"), NumBits, BitB->BitOffset);
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits %d is greater than BitBuffer can provide %zu, or greater than can be satisfied 0-64"), NumBits, BitB->BitOffset);
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
    
    size_t BitBuffer_GetUTF8StringSize(BitBuffer *BitB) {
        size_t StringSize               = 0ULL;
        if (BitB != NULL) {
            size_t OriginalOffset       = BitBuffer_GetPosition(BitB);
            size_t Extracted            = 1;
            while (Extracted != TextIO_NULLTerminator) {
                Extracted                 = BitBuffer_Extract_FarByte_FarBit(BitB, 8);
                StringSize               += UTF8_GetCodePointSizeInCodeUnits((UTF8) Extracted);;
            }
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF8 *BitBuffer_ReadUTF8(BitBuffer *BitB, size_t StringSize) {
        UTF8 *ExtractedString                 = UTF8_Init(StringSize);
        if (BitB != NULL && ExtractedString != NULL) {
            for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                ExtractedString[CodeUnit]     = (UTF8) BitBuffer_Extract_FarByte_FarBit(BitB, 8);
            }
        }
        return ExtractedString;
    }
    
    size_t BitBuffer_GetUTF16StringSize(BitBuffer *BitB) {
        size_t StringSize               = 0ULL;
        if (BitB != NULL) {
            size_t OriginalOffset       = BitBuffer_GetPosition(BitB);
            size_t Extracted            = 1;
            while (Extracted != TextIO_NULLTerminator) {
                Extracted               = BitBuffer_Extract_FarByte_FarBit(BitB, 16);
                StringSize             += UTF16_GetCodePointSizeInCodeUnits(Extracted);
            }
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF16 *BitBuffer_ReadUTF16(BitBuffer *BitB, size_t StringSize) {
        UTF16 *ExtractedString            = UTF16_Init(StringSize);
        if (BitB != NULL && ExtractedString != NULL) {
            for (size_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                ExtractedString[CodeUnit] = BitBuffer_Extract_FarByte_FarBit(BitB, 16);
            }
        }
        return ExtractedString;
    }
    
    uint8_t *BitBuffer_ReadGUUID(BitBuffer *BitB, GUUIDTypes GUUID2Read) {
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

    uint32_t BitBuffer_CalculateCRC32(BitBuffer *BitB, size_t OffsetInBits, size_t NumBytes, uint32_t Polynomial) {
        uint32_t Output = -1;
        if (BitB != NULL && OffsetInBits * 8 < BitBuffer_GetSize(BitB) && (OffsetInBits + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            for (size_t Byte = OffsetInBits; Byte < NumBytes - 1; Byte++) {
                uint8_t  Data       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                Output             ^= Data;
                for (uint8_t Bit = 0; Bit < 8; Bit++) {
                    if (Output & 1) {
                        Output = (Output >> 1) ^ Polynomial;
                    } else {
                        Output >>= 1;
                    }
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (OffsetInBits * 8 < BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Start: %zu is larger than the BitBuffer %zu"), OffsetInBits * 8, BitBuffer_GetSize(BitB));
        } else if ((OffsetInBits + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("End: %zu is larger than the BitBuffer %zu"), (OffsetInBits + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return ~Output;
    }

    uint32_t BitBuffer_CalculateAdler32(BitBuffer *BitB, size_t OffsetInBits, size_t NumBytes) {
        uint32_t Output = 0;
        if (BitB != NULL && OffsetInBits * 8 < BitBuffer_GetSize(BitB) && (OffsetInBits + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            uint16_t A = 1;
            uint16_t B = 0;

            for (size_t Byte = OffsetInBits; Byte < NumBytes - 1; Byte++) {
                uint8_t Value = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                A = (A + Value) % 65521;
                B = (B + A)     % 65521;
            }

            Output = (B << 16) | A;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (OffsetInBits < BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Start: %zu is larger than the BitBuffer %zu"), OffsetInBits, BitBuffer_GetSize(BitB));
        } else if (OffsetInBits + NumBytes <= BitBuffer_GetSize(BitB)) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("End: %zu is larger than the BitBuffer %zu"), (OffsetInBits + NumBytes), BitBuffer_GetSize(BitB));
        }
        return Output;
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits2Write %d is greater than BitBuffer can provide %zu, or greater than BitBuffer_WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
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
            size_t CodeUnit        = 0ULL;
            while (String2Write[CodeUnit] != TextIO_NULLTerminator) {
                BitBuffer_Append_NearByte_FarBit(BitB, UTF8CodeUnitSizeInBits, String2Write[CodeUnit]);
                CodeUnit          += 1;
            }
            if (WriteType == StringTerminator_NULL) {
                BitBuffer_Seek(BitB, UTF8CodeUnitSizeInBits); // Write the NULL terminator
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteUTF16(BitBuffer *BitB, ImmutableString_UTF16 String2Write, BufferIO_StringTerminators WriteType) {
        if (BitB != NULL && String2Write != NULL) {
            size_t CodeUnit        = 0ULL;
            while (String2Write[CodeUnit] != TextIO_NULLTerminator) {
                BitBuffer_Append_NearByte_FarBit(BitB, UTF16CodeUnitSizeInBits, String2Write[CodeUnit]);
                CodeUnit          += 1;
            }
            if (WriteType == StringTerminator_NULL) {
                BitBuffer_Seek(BitB, UTF16CodeUnitSizeInBits); // Write the NULL terminator
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType, uint8_t *GUUID2Write) {
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

    /* Standard Functions */
    size_t BufferIO_MemoryCopy8(uint8_t *restrict Destination, const uint8_t *restrict Source, const size_t NumElements2Copy) { // memcpy
        size_t ElementsCopied   = 0;
#pragma omp simd
        for (size_t Byte = 0; Byte < NumElements2Copy; Byte++) {
            Destination[Byte]   = Source[Byte];
            ElementsCopied     += 1;
        }
        return ElementsCopied;
    }

    size_t BufferIO_MemoryCopy16(uint64_t *restrict Destination, const uint64_t *restrict Source, const size_t NumElements2Copy) {
        size_t   ElementsCopied = 0;
#pragma omp simd
        for (size_t Element = 0; Element < NumElements2Copy; Element++) {
            Destination[Element] = Source[Element];
            ElementsCopied      += 1;
        }
        return ElementsCopied;
    }

    size_t BufferIO_MemoryCopy32(uint64_t *restrict Destination, const uint64_t *restrict Source, const size_t NumElements2Copy) {
        size_t   ElementsCopied = 0;
#pragma omp simd
        for (size_t Element = 0; Element < NumElements2Copy; Element++) {
            Destination[Element] = Source[Element];
            ElementsCopied      += 1;
        }
        return ElementsCopied;
    }

    size_t BufferIO_MemoryCopy64(uint64_t *restrict Destination, const uint64_t *restrict Source, const size_t NumElements2Copy) {
        size_t   ElementsCopied = 0;
#pragma omp simd
        for (size_t Element = 0; Element < NumElements2Copy; Element++) {
            Destination[Element] = Source[Element];
            ElementsCopied      += 1;
        }
        return ElementsCopied;
    }

    size_t BufferIO_MemorySet8(uint8_t *Array, uint8_t Value2Set, size_t NumElements2Set) { // memset
        size_t NumElementsSet = 0;
#pragma omp simd
        for (size_t Element = 0; Element < NumElementsSet; Element++) {
            Array[Element]    = Value2Set;
            NumElementsSet   += 1;
        }
        return NumElementsSet;
    }

    size_t BufferIO_MemorySet16(uint16_t *Array, uint16_t Value2Set, size_t NumElements2Set) {
        size_t NumElementsSet = 0;
#pragma omp simd
        for (size_t Element = 0; Element < NumElementsSet; Element++) {
            Array[Element]    = Value2Set;
            NumElementsSet   += 1;
        }
        return NumElementsSet;
    }

    size_t BufferIO_MemorySet32(uint32_t *Array, uint16_t Value2Set, size_t NumElements2Set) {
        size_t NumElementsSet = 0;
#pragma omp simd
        for (size_t Element = 0; Element < NumElementsSet; Element++) {
            Array[Element]    = Value2Set;
            NumElementsSet   += 1;
        }
        return NumElementsSet;
    }

    size_t BufferIO_MemorySet64(uint64_t *Array, uint16_t Value2Set, size_t NumElements2Set) {
        size_t NumElementsSet = 0;
#pragma omp simd
        for (size_t Element = 0; Element < NumElementsSet; Element++) {
            Array[Element]    = Value2Set;
            NumElementsSet   += 1;
        }
        return NumElementsSet;
    }
    /* Standard Functions */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
