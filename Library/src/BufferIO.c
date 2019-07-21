#include "../include/BitIO.h"          /* Included for our declarations */

#include "../include/Log.h"            /* Included for Log declarations */
#include "../include/Math.h"           /* Included for Integer functions */
#include "../include/StringIO.h"       /* Included for StringIO's declarations */
#include "../include/CryptographyIO.h" /* Included for GenerateIntegerInRange for GUUID_Generate */

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum                      GUUIDConstants
     @abstract                                                  "Constants for GUUID types".
     @constant                  GUUIDStringSize                 "The size of a GUUID string".
     @constant                  BinaryGUUIDSize                 "The dize of a BinaryGUUID".
     */
    enum GUUIDConstants {
                                GUUIDStringSize                 = 20,
                                BinaryGUUIDSize                 = 16,
    };
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        uint8_t   *Buffer;
        uint64_t   BitOffset;
        uint64_t   NumBits;
    } BitBuffer;
    
    typedef struct BitInput {
        FILE           *File;
        uint64_t        FilePosition;
        uint64_t        FileSize;
        int             Socket;
        BitIOFileTypes  FileType;
    } BitInput;
    
    typedef struct BitOutput {
        FILE           *File;
        uint64_t        FilePosition;
        int             Socket;
        BitIOFileTypes  FileType;
    } BitOutput;
    
    BitBuffer *BitBuffer_Init(uint64_t BitBufferSize) {
        BitBuffer *BitB                  = calloc(1, sizeof(BitBuffer));
        if (BitB != NULL && BitBufferSize > 0) {
            BitB->Buffer                 = calloc(BitBufferSize, sizeof(uint8_t));
            if (BitB->Buffer != NULL) {
                BitB->NumBits            = Bytes2Bits(BitBufferSize);
            } else {
                BitBuffer_Deinit(BitB);
                BitB                     = NULL;
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld bits for BitBuffer's buffer"), BitBufferSize);
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate BitBuffer"));
        }
        return BitB;
    }
    
    void BitBuffer_Read(BitBuffer *BitB, BitInput *BitI) {
        if (BitB != NULL && BitI != NULL) {
            uint64_t Bytes2Read    = Bits2Bytes(BitB->NumBits - BitB->BitOffset, RoundingType_Down);
            uint8_t  Bits2Save     = BitB->BitOffset % 8;
            if (Bits2Save > 0) {
                BitB->Buffer[0]    = 0;
                uint8_t Saved      = BitB->Buffer[Bytes2Read + 1] & CreateBitMaskLSBit(Bits2Save);
                BitB->Buffer[0]    = Saved;
                BitB->BitOffset    = Bits2Save;
                for (uint64_t Byte = (uint64_t) Bits2Bytes(BitB->BitOffset, RoundingType_Up); Byte < (uint64_t) Bits2Bytes(BitB->NumBits, RoundingType_Down); Byte++) {
                    BitB->Buffer[Byte] = 0;
                }
            }
            uint64_t BytesRead     = 0ULL;
            if (BitI->FileType == BitIOFile) {
                BytesRead          = FoundationIO_File_Read(BitB->Buffer, 1, Bytes2Read, BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
                BytesRead          = FoundationIO_Socket_Read(BitI->Socket, BitB->Buffer, Bytes2Read);
            }
            if (BytesRead == Bytes2Read) {
                BitB->NumBits      = (BytesRead * 8) + BitB->BitOffset;
            } else {
                Log(Log_DEBUG, __func__, U8("Num bytes read %llu does not match num bytes requested %llu"), BytesRead, Bytes2Read);
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitI == NULL) {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetSize(BitBuffer *BitB) {
        uint64_t BitBufferSize = 0ULL;
        if (BitB != NULL) {
            BitBufferSize      = BitB->NumBits;
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return BitBufferSize;
    }
    
    void BitBuffer_SetSize(BitBuffer *BitB, uint64_t Bits) {
        if (BitB != NULL) {
            BitB->NumBits = Bits;
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB != NULL) {
            Position = BitB->BitOffset;
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Position;
    }
    
    void BitBuffer_SetPosition(BitBuffer *BitB, uint64_t Offset) {
        if (BitB != NULL) {
            BitB->BitOffset = Offset;
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetBitsFree(BitBuffer *BitB) {
        uint64_t BitsFree = 0ULL;
        if (BitB != NULL) {
            BitsFree      = BitB->NumBits - BitB->BitOffset;
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return BitsFree;
    }
    
    bool BitBuffer_IsAligned(BitBuffer *BitB, uint8_t AlignmentSize) {
        bool AlignmentStatus    = No;
        if (BitB != NULL && (AlignmentSize != 1 || AlignmentSize % 2 != 0)) {
            if (BitB->BitOffset % (AlignmentSize * 8) == 0) {
                AlignmentStatus = Yes;
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize != 1 || AlignmentSize % 2 != 0) {
            Log(Log_DEBUG, __func__, U8("AlignmentSize: %d isn't 1 or an integer power of 2"), AlignmentSize);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, uint8_t AlignmentSize) {
        if (BitB != NULL && (AlignmentSize == 1 || AlignmentSize % 2 == 0)) {
            int64_t  AlignmentSizeInBits = Bytes2Bits(AlignmentSize);
            int64_t  Bits2Align          = AlignmentSizeInBits - (BitB->BitOffset % AlignmentSizeInBits);
            if (BitB->BitOffset + Bits2Align > BitB->NumBits) {
                BitB->Buffer             = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, RoundingType_Up));
                BitB->NumBits           += Bits2Align;
            }
            BitB->BitOffset             += Bits2Align;
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize == 1 || AlignmentSize % 2 == 0) {
            Log(Log_DEBUG, __func__, U8("AlignmentSize: %d isn't a multiple of 2"), AlignmentSize);
        }
    }
    
    void BitBuffer_Seek(BitBuffer *BitB, int64_t Bits2Seek) {
        if (BitB != NULL) {
            if (Bits2Seek > 0 && BitB->NumBits > BitB->BitOffset + Bits2Seek) {
                BitB->NumBits  += Bits2Seek;
            } else if (Bits2Seek < 0 && BitB->NumBits > BitB->BitOffset - Bits2Seek) {
                BitB->NumBits  += Bits2Seek;
            } else {
                Log(Log_DEBUG, __func__, U8("There's not enough bits in BitBuffer %lld to seek %lld bits"), BitB->NumBits, Bits2Seek);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Erase(BitBuffer *BitB) {
        if (BitB != NULL) {
            uint64_t BufferSize = Bits2Bytes(BitB->NumBits, RoundingType_Up);
            for (uint64_t Byte = 0ULL; Byte < BufferSize - 1; Byte++) {
                BitB->Buffer[Byte] = 0;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Resize(BitBuffer *BitB, uint64_t NewSize) {
        if (BitB != NULL && NewSize * 8 >= BitB->BitOffset) {
            uint8_t *NewBuffer  = realloc(BitB->Buffer, NewSize);
            if (NewBuffer != NULL) {
                BitB->Buffer    = NewBuffer;
                BitB->BitOffset = 0;
                BitB->NumBits   = NewSize * 8;
            } else {
                Log(Log_DEBUG, __func__, U8("Reallocing the BitBuffer failed"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Refresh(BitBuffer *BitB, BitInput *BitI) {
        if (BitB != NULL && BitI != NULL) {
            uint64_t Bytes2Read      = BitB->NumBits / 8;
            uint64_t BytesRead       = 0ULL;
            if (BitI->FileType == BitIOFile) {
                BytesRead            = FoundationIO_File_Read(BitB->Buffer, 1, Bytes2Read, BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
                BytesRead            = FoundationIO_Socket_Read(BitI->Socket, BitB->Buffer, Bytes2Read);
            }
            if (BytesRead != Bytes2Read) {
                uint8_t *Reallocated = realloc(BitB->Buffer, BytesRead);
                if (Reallocated != NULL) {
                    BitB->Buffer     = Reallocated;
                    BitB->BitOffset  = 0;
                    BitB->NumBits    = BytesRead * 8;
                } else {
                    Log(Log_DEBUG, __func__, U8("Reallocating the BitBuffer failed"));
                }
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitI == NULL) {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
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
            Log(Log_DEBUG, __func__, U8("Source Pointer is NULL"));
        } else if (Destination == NULL) {
            Log(Log_DEBUG, __func__, U8("Destination Pointer is NULL"));
        } else if (BitStart >= BitEnd) {
            Log(Log_DEBUG, __func__, U8("BitStart %lld is greater than or equal to BitEnd %lld"), BitStart, BitEnd);
        } else if (BitStart >= Source->NumBits || BitEnd >= Source->NumBits) {
            Log(Log_DEBUG, __func__, U8("BitStart %lld or BitEnd %lld is greater than there are bits in Source %lld"), BitStart, BitEnd, Source->NumBits);
        }
    }
    
    static uint64_t BitBuffer_Extract_LSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            // Value = 0x0000AC44, LSByte, LSBit, 32 bits, written as 0x44AC0000
            /*
             Value = 0x0000AC44
             Wrote = 0x44AC0000
             So, Start reading from BitOffset + NumBits2Read, end reading at BitOffset
             */
            uint8_t Bits2Read                     = NumBits2Extract;
            do {
                uint8_t  BitsInByte               = Bits2ExtractFromByte(BitB->BitOffset + Bits2Read);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Read);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = (uint8_t) (CreateBitMaskLSBit(Bits2Get) << Shift);
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset + Bits2Read, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                uint8_t  ApplyBits                = ExtractedBits >> Shift;
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ApplyBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Read > 0);
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_LSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            uint8_t Bits2Read                     = NumBits2Extract;
            do {
                uint8_t  BitsInByte               = Bits2ExtractFromByte(BitB->BitOffset + Bits2Read);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Read);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = CreateBitMaskMSBit(Bits2Get) >> Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset + Bits2Read, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ApplyBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Read > 0);
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_MSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            uint8_t Bits2Read                     = NumBits2Extract;
            do {
                uint8_t  BitsInByte               = Bits2ExtractFromByte(BitB->BitOffset);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Read);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = (uint8_t) (CreateBitMaskLSBit(Bits2Get) << Shift);
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                uint8_t  ApplyBits                = ExtractedBits >> Shift;
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ApplyBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Read > 0);
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_MSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            uint8_t Bits2Read                     = NumBits2Extract;
            do {
                uint8_t  BitsInByte               = Bits2ExtractFromByte(BitB->BitOffset);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Read);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = CreateBitMaskMSBit(Bits2Get) >> Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ApplyBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Read > 0);
        }
        return Extracted;
    }
    
    static void BitBuffer_Append_LSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t Bits2Write                    = NumBits2Append;
            do {
                uint8_t  BitsInByte               = BitB->BitOffset % 8;
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Write);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = CreateBitMaskMSBit(Bits2Get) >> Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = Data2Append & BitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                BitB->Buffer[Byte]               |= ApplyBits;
                
                Bits2Write                       -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Write > 0);
        }
    }
    
    static void BitBuffer_Append_LSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t Bits2Write                    = NumBits2Append;
            do {
                uint8_t  BitsInByte               = BitB->BitOffset % 8;
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Write);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = CreateBitMaskMSBit(Bits2Get) >> Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = Data2Append & BitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                BitB->Buffer[Byte]               |= ApplyBits;
                
                Bits2Write                       -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Write > 0);
        }
    }
    
    static void BitBuffer_Append_MSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t Bits2Write                    = NumBits2Append;
            do {
                uint8_t  BitsInByte               = BitB->BitOffset % 8;
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Write);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = CreateBitMaskMSBit(Bits2Get) >> Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = Data2Append & BitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                BitB->Buffer[Byte]               |= ApplyBits;
                
                Bits2Write                       -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Write > 0);
        }
    }
    
    static void BitBuffer_Append_MSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t Bits2Write                    = NumBits2Append;
            do {
                uint8_t  BitsInByte               = BitB->BitOffset % 8;
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInByte, Bits2Write);
                uint8_t  Shift                    = 8 - Bits2Get;
                uint8_t  BitMask                  = CreateBitMaskMSBit(Bits2Get) >> Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = Data2Append & BitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                BitB->Buffer[Byte]               |= ApplyBits;
                
                Bits2Write                       -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            } while (Bits2Write > 0);
        }
    }
    
    uint64_t BitBuffer_PeekBits(BitBuffer *BitB, ByteOrders ByteOrder, BitOrders BitOrder, uint8_t Bits2Peek) {
        uint64_t OutputData  = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == LSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    OutputData          = BitBuffer_Extract_LSByteLSBit(BitB, Bits2Peek);
                } else if (BitOrder == MSBitFirst) {
                    OutputData          = BitBuffer_Extract_LSByteMSBit(BitB, Bits2Peek);
                }
            } else if (ByteOrder == MSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    // Start the byte at the end, and the bit at the beginning
                    OutputData         = BitBuffer_Extract_MSByteLSBit(BitB, Bits2Peek);
                } else if (BitOrder == MSBitFirst) {
                    // Start reading from the beginning, read to the end
                    OutputData         = BitBuffer_Extract_MSByteMSBit(BitB, Bits2Peek);
                }
            }
            BitB->BitOffset -= Bits2Peek;
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            Log(Log_DEBUG, __func__, U8("Bits2Peek %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_PeekBits can satisfy 1-64"), Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t BitBuffer_ReadBits(BitBuffer *BitB, ByteOrders ByteOrder, BitOrders BitOrder, uint8_t Bits2Read) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == LSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    OutputData          = BitBuffer_Extract_LSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == MSBitFirst) {
                    OutputData          = BitBuffer_Extract_LSByteMSBit(BitB, Bits2Read);
                }
            } else if (ByteOrder == MSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    // Start the byte at the end, and the bit at the beginning
                    OutputData         = BitBuffer_Extract_MSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == MSBitFirst) {
                    // Start reading from the beginning, read to the end
                    OutputData         = BitBuffer_Extract_MSByteMSBit(BitB, Bits2Read);
                }
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            Log(Log_DEBUG, __func__, U8("Bits2Read %d is greater than BitBuffer can provide %lld, or greater than can be satisfied 1-64"), Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t BitBuffer_ReadUnary(BitBuffer *BitB, ByteOrders ByteOrder, BitOrders BitOrder, UnaryTypes UnaryType, Unary_StopBits StopBit) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL) {
            uint8_t CurrentBit = StopBit ^ 1;
            do {
                if (ByteOrder == LSByteFirst) {
                    if (BitOrder == LSBitFirst) {
                        CurrentBit          = (uint8_t) BitBuffer_Extract_LSByteLSBit(BitB, 1);
                    } else if (BitOrder == MSBitFirst) {
                        CurrentBit          = (uint8_t) BitBuffer_Extract_LSByteMSBit(BitB, 1);
                    }
                } else if (ByteOrder == MSByteFirst) {
                    if (BitOrder == LSBitFirst) {
                        // Start the byte at the end, and the bit at the beginning
                        CurrentBit         = (uint8_t) BitBuffer_Extract_MSByteLSBit(BitB, 1);
                    } else if (BitOrder == MSBitFirst) {
                        // Start reading from the beginning, read to the end
                        CurrentBit         = (uint8_t) BitBuffer_Extract_MSByteMSBit(BitB, 1);
                    }
                }
                OutputData    += 1;
            } while (CurrentBit != StopBit);
            BitBuffer_Seek(BitB, 1);
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return (UnaryType == CountUnary ? OutputData + 1 : OutputData);
    }
    
    uint64_t BitBuffer_GetUTF8StringSize(BitBuffer *BitB) {
        uint64_t StringSize           = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset   = BitBuffer_GetPosition(BitB);
            UTF8     CodeUnit8        = 1;
            do {
                CodeUnit8             = (UTF8) BitBuffer_Extract_LSByteLSBit(BitB, 8);
                uint8_t  CodeUnitSize = UTF8_GetCodePointSizeInCodeUnits(CodeUnit8);
                StringSize           += CodeUnitSize;
            } while (CodeUnit8 != FoundationIONULLTerminator);
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF8 *BitBuffer_ReadUTF8(BitBuffer *BitB, uint64_t StringSize) {
        UTF8 *ExtractedString             = calloc(StringSize + FoundationIONULLTerminatorSize, sizeof(UTF8));
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                ExtractedString[CodeUnit] = (UTF8) BitBuffer_Extract_LSByteLSBit(BitB, 8);
            }
        }
        return ExtractedString;
    }
    
    uint64_t BitBuffer_GetUTF16StringSize(BitBuffer *BitB) {
        uint64_t StringSize           = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset   = BitBuffer_GetPosition(BitB);
            UTF16    CodeUnit16       = 1;
            do {
                CodeUnit16            = (UTF16) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                uint8_t  CodeUnitSize = UTF16_GetCodePointSizeInCodeUnits(CodeUnit16);
                StringSize           += CodeUnitSize;
            } while (CodeUnit16 != FoundationIONULLTerminator);
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF16 *BitBuffer_ReadUTF16(BitBuffer *BitB, uint64_t StringSize) {
        UTF16 *ExtractedString            = calloc(StringSize, sizeof(UTF16));
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize - 1; CodeUnit++) {
                ExtractedString[CodeUnit] = (UTF16) BitBuffer_Extract_LSByteLSBit(BitB, 16);
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return ExtractedString;
    }
    
    uint8_t *BitBuffer_ReadGUUID(BitBuffer *BitB, GUUIDTypes GUUID2Read) {
        uint8_t *GUUID = NULL;
        if (GUUID2Read != UnknownGUUID && BitB != NULL && (BitBuffer_GetSize(BitB) - BitBuffer_GetPosition(BitB)) >= BinaryGUUIDSize) {
            if (GUUID2Read == BinaryUUID || GUUID2Read == BinaryGUID) {
                // Read it from the BitBuffer as a string.
                GUUID = calloc(BinaryGUUIDSize, sizeof(uint8_t));
                if (GUUID != NULL) {
                    for (uint8_t Byte = 0; Byte < BinaryGUUIDSize - 1; Byte++) {
                        GUUID[Byte]      = (uint8_t) BitBuffer_Extract_LSByteLSBit(BitB, 8);
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("Couldn't allocate GUIDString"));
                }
            } else if (GUUID2Read == UUIDString || GUUID2Read == GUIDString) {
                if (GUUID != NULL) {
                    uint32_t Section1    = (uint32_t) BitBuffer_Extract_LSByteLSBit(BitB, 32);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section2    = (uint16_t) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section3    = (uint16_t) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section4    = (uint16_t) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint64_t Section5    = (uint64_t) BitBuffer_Extract_LSByteLSBit(BitB, 48);
                    GUUID                = UTF8_Format(U8("%d-%d-%d-%d-%llu"), Section1, Section2, Section3, Section4, Section5);
                } else {
                    Log(Log_DEBUG, __func__, U8("Couldn't allocate UUIDString"));
                }
            }
        }
        return GUUID;
    }
    
    void BitBuffer_WriteBits(BitBuffer *BitB, ByteOrders ByteOrder, BitOrders BitOrder, uint8_t NumBits2Write, uint64_t Bits2Write) {
        if (BitB != NULL) {
            if (ByteOrder == LSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    BitBuffer_Append_LSByteLSBit(BitB, NumBits2Write, Bits2Write);
                } else if (BitOrder == MSBitFirst) {
                    BitBuffer_Append_LSByteMSBit(BitB, NumBits2Write, Bits2Write);
                }
            } else if (ByteOrder == MSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    BitBuffer_Append_MSByteLSBit(BitB, NumBits2Write, Bits2Write);
                } else if (BitOrder == MSBitFirst) {
                    BitBuffer_Append_MSByteMSBit(BitB, NumBits2Write, Bits2Write);
                }
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(Log_DEBUG, __func__, U8("NumBits2Write %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
        }
    }
    
    void BitBuffer_WriteUnary(BitBuffer *BitB, ByteOrders ByteOrder, BitOrders BitOrder, UnaryTypes UnaryType, Unary_StopBits StopBit, uint8_t UnaryBits2Write) {
        if (BitB != NULL) {
            StopBit            &= 1;
            uint8_t Field2Write = UnaryBits2Write;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            
            if (ByteOrder == LSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    BitBuffer_Append_LSByteLSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_LSByteLSBit(BitB, 1, StopBit);
                } else if (BitOrder == MSBitFirst) {
                    BitBuffer_Append_LSByteMSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_LSByteMSBit(BitB, 1, StopBit);
                }
            } else if (ByteOrder == MSByteFirst) {
                if (BitOrder == LSBitFirst) {
                    BitBuffer_Append_MSByteLSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_MSByteLSBit(BitB, 1, StopBit);
                } else if (BitOrder == MSBitFirst) {
                    BitBuffer_Append_MSByteMSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_MSByteMSBit(BitB, 1, StopBit);
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteUTF8(BitBuffer *BitB, UTF8 *String2Write) {
        if (BitB != NULL && String2Write != NULL) {
            int64_t  StringSize    = UTF8_GetStringSizeInCodeUnits(String2Write);
            int64_t  BitsAvailable = BitBuffer_GetBitsFree(BitB);
            uint64_t CodeUnit      = 0ULL;
            if (BitsAvailable >= Bytes2Bits(StringSize)) {
                do {
                    BitBuffer_Append_MSByteLSBit(BitB, 8, String2Write[CodeUnit]);
                    CodeUnit         += 1;
                } while (String2Write[CodeUnit] != FoundationIONULLTerminator);
            } else {
                Log(Log_DEBUG, __func__, U8("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteUTF16(BitBuffer *BitB, UTF16 *String2Write) {
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(String2Write);
            uint64_t BitsAvailable = BitBuffer_GetBitsFree(BitB);
            if (BitsAvailable >= (uint64_t) Bytes2Bits(StringSize)) {
                uint64_t CodeUnit = 0ULL;
                do {
                    BitBuffer_Append_LSByteLSBit(BitB, 16, String2Write[CodeUnit]);
                    CodeUnit     += 1;
                } while (String2Write[CodeUnit] != FoundationIONULLTerminator);
            } else {
                Log(Log_DEBUG, __func__, U8("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType, uint8_t *GUUID2Write) {
        if (BitB != NULL && GUUID2Write != NULL) { // TODO: Make sure that the BitBuffer can hold the GUUID
            uint8_t GUUIDSize = ((GUUIDType == GUIDString || GUUIDType == UUIDString) ? GUUIDStringSize : BinaryGUUIDSize);
            for (uint8_t Byte = 0; Byte < GUUIDSize - 1; Byte++) {
                BitBuffer_Append_LSByteLSBit(BitB, 8, GUUID2Write[Byte]);
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (GUUID2Write == NULL) {
            Log(Log_DEBUG, __func__, U8("GUUID2Write Pointer is NULL"));
        }
    }
    
    void BitBuffer_Write(BitBuffer *BitB, BitOutput *BitO) {
        if (BitB != NULL && BitO != NULL) {
            uint64_t Bytes2Write  = Bits2Bytes(BitBuffer_GetPosition(BitB), RoundingType_Down);
            uint64_t Bits2Keep    = BitB->BitOffset % 8;
            uint64_t BytesWritten = 0ULL;
            if (BitO->FileType == BitIOFile) {
                BytesWritten = FoundationIO_File_Write(BitB->Buffer, 1, Bytes2Write, BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
                BytesWritten = FoundationIO_Socket_Write(BitO->Socket, BitB->Buffer, Bytes2Write);
            }
            if (BytesWritten == Bytes2Write) {
                BitB->Buffer[0] = 0;
                BitB->Buffer[0] = BitB->Buffer[Bytes2Write + 1] & (Exponentiate(2, Bits2Keep) << (8 - Bits2Keep));
                BitB->BitOffset = Bits2Keep + 1;
                for (uint64_t Byte = (uint64_t) Bits2Bytes(BitB->BitOffset, RoundingType_Up); Byte < (uint64_t) Bits2Bytes(BitB->NumBits, RoundingType_Down); Byte++) {
                    BitB->Buffer[Byte] = 0;
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Wrote %lld of %lld bits"), BytesWritten * 8, Bytes2Write * 8);
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitO == NULL) {
            Log(Log_DEBUG, __func__, U8("BitOutput Pointer is NULL"));
        }
    }
    
    void BitBuffer_Deinit(BitBuffer *BitB) {
        if (BitB != NULL) {
            free(BitB->Buffer);
            free(BitB);
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    /* BitBuffer Resource Management */
    /* End BitBuffer section */
    
    /* BitInput */
    BitInput *BitInput_Init(void) {
        BitInput *BitI = calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            BitInput_Deinit(BitI);
            Log(Log_DEBUG, __func__, U8("Couldn't allocate BitInput"));
        }
        return BitI;
    }
    
    void BitInput_UTF8_OpenFile(BitInput *BitI, UTF8 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType                   = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS) || (FoundationIOTargetOS == FoundationIOAppleOS)
            bool PathHasBOM                  = UTF8_HasBOM(Path2Open);
            if (PathHasBOM == No) {
                BitI->File                   = FoundationIO_File_Open(Path2Open, U8("rb"));
            } else {
                UTF8 *BOMLess                = UTF8_RemoveBOM(Path2Open);
                BitI->File                   = FoundationIO_File_Open(BOMLess, U8("rb"));
                free(BOMLess);
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            bool  StringHasBOM               = UTF8_HasBOM(Path2Open);
            UTF32 *WinPath32                 = UTF8_Decode(Path2Open);
            UTF32 *WinPathLong32             = UTF32_Insert(WinPath32, UNCPathPrefix, StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16                 = UTF16_Encode(WinPathLong32);
            free(WinPathLong32);
            BitI->File                       = FoundationIO_File_Open(WinPath16, U16("rb"));
            free(WinPath16);
#endif
            if (BitI->File != NULL) {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't open file \"%s\": Check that the file exists and the permissions are correct"), Path2Open);
            }
        } else if (BitI == NULL) {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void BitInput_UTF16_OpenFile(BitInput *BitI, UTF16 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType                   = BitIOFile;
            UTF32 *Path32                    = NULL;
            UTF8  *Path8                     = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS) || (FoundationIOTargetOS == FoundationIOAppleOS)
            Path32                           = UTF16_Decode(Path2Open);
            bool   PathHasBOM                = UTF16_HasBOM(Path2Open);
            //bool   PathIsAbsolute            = UTF16_PathIsAbsolute(Path2Open);
            bool   PathHasWinPrefix          = UTF16_HasUNCPathPrefix(Path2Open);
            
            if (PathHasBOM == Yes && PathHasWinPrefix == Yes) {
                UTF32 *BOMLess               = UTF32_RemoveBOM(Path32);
                free(Path32);
                Path32                       = BOMLess;
            } else if (PathHasBOM == Yes && PathHasWinPrefix == No) {
                // Add WinPathPrefix, Remove BOM
                UTF32 *BOMLess               = UTF32_RemoveBOM(Path32);
                UTF32 *Prefix                = UTF32_Insert(BOMLess, UNCPathPrefix, 0);
                free(Path32);
                Path32                       = Prefix;
            } else if (PathHasBOM == No && PathHasWinPrefix == No) {
                // Add WinPathPrefix
                UTF32 *Prefix                = UTF32_Insert(Path32, UNCPathPrefix, 0);
                free(Path32);
                Path32                       = Prefix;
            }
            Path8                            = UTF8_Encode(Path32);
            BitI->File                       = FoundationIO_File_Open(Path8, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            bool  StringHasBOM               = UTF16_HasBOM(Path2Open);
            UTF32 *WinPath32                 = UTF16_Decode(Path2Open);
            UTF32 *WinPathLong32             = UTF32_Insert(WinPath32, UNCPathPrefix, StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16                 = UTF16_Encode(WinPathLong32);
            free(WinPathLong32);
            BitI->File                       = FoundationIO_File_Open(WinPath16, U16("rb"));
            free(WinPath16);
#endif
            if (BitI->File != NULL) {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            } else {
                Path32                       = UTF16_Decode(Path2Open);
                Path8                        = UTF8_Encode(Path32);
                free(Path32);
                Log(Log_DEBUG, __func__, U8("Couldn't open file \"%s\": Check that the file exists and the permissions are correct"), Path8);
                free(Path8);
            }
        } else if (BitI == NULL) {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void BitInput_OpenSocket(BitInput *BitI, int Domain, int Type, int Protocol) {
        if (BitI != NULL) {
            BitI->FileType = BitIOSocket;
            BitI->Socket   = FoundationIO_Socket_Create(Domain, Type, Protocol);
        } else {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    void BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, uint32_t SocketSize) {
        if (BitI != NULL && SocketAddress != NULL) {
            BitI->FileType = BitIOSocket;
            FoundationIO_Socket_Connect(BitI->Socket, SocketAddress, SocketSize);
        } else if (BitI == NULL) {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Log_DEBUG, __func__, U8("SocketAddress Pointer is NULL"));
        }
    }
    
    static void BitInput_FindFileSize(BitInput *BitI) {
        if (BitI != NULL) {
            FoundationIO_File_Seek(BitI->File, 0, SEEK_END);
            BitI->FileSize     = (uint64_t) FoundationIO_File_GetSize(BitI->File);
            FoundationIO_File_Seek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition = (uint64_t) FoundationIO_File_GetSize(BitI->File);
        } else {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    uint64_t BitInput_GetFileSize(BitInput *BitI) {
        uint64_t InputSize = 0ULL;
        if (BitI != NULL) {
            if (BitI->FileSize == 0) {
                BitInput_FindFileSize(BitI);
            }
            InputSize     = BitI->FileSize;
        } else {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
        return InputSize;
    }
    
    uint64_t BitInput_GetFilePosition(BitInput *BitI) {
        uint64_t Position = 0ULL;
        if (BitI != NULL) {
            if (BitI->FilePosition == 0) {
                BitInput_FindFileSize(BitI);
            }
            Position    = BitI->FilePosition;
        } else {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
        return Position;
    }
    
    uint64_t BitInput_BytesRemaining(BitInput *BitI) {
        uint64_t BytesLeft = 0ULL;
        if (BitI != NULL) {
            if (BitI->FilePosition == 0) {
                BitInput_FindFileSize(BitI);
            }
            BytesLeft    = BitI->FileSize - BitI->FilePosition;
        } else {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
        return BytesLeft;
    }
    
    void BitInput_Deinit(BitInput *BitI) {
        if (BitI != NULL) {
            if (BitI->FileType == BitIOFile) {
                FoundationIO_File_Close(BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
                FoundationIO_Socket_Close(BitI->Socket);
            }
            free(BitI);
        } else {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    /* BitInput */
    
    
    /* BitOutput */
    BitOutput *BitOutput_Init(void) {
        BitOutput *BitO = calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            BitOutput_Deinit(BitO);
            Log(Log_DEBUG, __func__, U8("Couldn't allocate BitOutput"));
        }
        return BitO;
    }
    
    void BitOutput_UTF8_OpenFile(BitOutput *BitO, UTF8 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType                   = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS) || (FoundationIOTargetOS == FoundationIOAppleOS)
            bool PathHasBOM                  = UTF8_HasBOM(Path2Open);
            if (PathHasBOM == No) {
                BitO->File                   = FoundationIO_File_Open(Path2Open, U8("rb"));
            } else {
                UTF8 *BOMLess                = UTF8_RemoveBOM(Path2Open);
                BitO->File                   = FoundationIO_File_Open(BOMLess, U8("rb"));
                free(BOMLess);
            }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            bool   PathIsAbsolute       = UTF8_PathIsAbsolute(Path2Open);
            bool   PathHasUNCPrefix     = UTF8_HasUNCPathPrefix(Path2Open);
            UTF32 *Path32               = UTF8_Decode(Path2Open);
            UTF16 *Path16               = NULL;
            if (PathIsAbsolute == Yes && PathHasUNCPrefix == No) {
                UTF32 *UNCPrefixed      = UTF32_Insert(Path32, UNCPathPrefix, 0);
                Path16                  = UTF16_Encode(UNCPrefixed);
                free(UNCPrefixed);
                FoundationIO_File_Open(Path16, U16("rb"));
            } else if (PathIsAbsolute == No && PathHasUNCPrefix == Yes) {
                UTF32 *Removed          = UTF32_RemoveSubString(Path32, UNCPathPrefix, 1);
                Path16                  = UTF16_Encode(Removed);
                free(Removed);
                FoundationIO_File_Open(Path16, U16("rb"));
            } else {
                Path16                  = UTF16_Encode(Path32);
                FoundationIO_File_Open(Path16, U16("rb"));
            }
#endif
        } else if (BitO == NULL) {
            Log(Log_DEBUG, __func__, U8("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_DEBUG, __func__, U8("Path2Open Pointer is NULL"));
        }
    }
    
    void BitOutput_UTF16_OpenFile(BitOutput *BitO, UTF16 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType              = BitIOFile;
            bool  PathHasBOM            = No;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS) || (FoundationIOTargetOS == FoundationIOAppleOS)
            // Convert to UTF-8, and remove the BOM because fopen will silently fail if there's a BOM.
            UTF32 *Decoded              = UTF16_Decode(Path2Open);
            PathHasBOM                  = UTF32_HasBOM(Decoded);
            UTF8 *Fixed                 = NULL;
            if (PathHasBOM == Yes) {
                UTF32 *Fixed32          = UTF32_RemoveBOM(Decoded);
                Fixed                   = UTF8_Encode(Fixed32);
            }
            FoundationIO_File_Open(Fixed, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            bool   StringHasPathPrefix  = UTF16_HasUNCPathPrefix(Path2Open);
            
            UTF32 *Path32               = UTF16_Decode(Path2Open);
            UTF16 *Path16               = NULL;
            if (StringHasPathPrefix == No) {
                bool   StringHasBOM     = UTF16_HasBOM(Path2Open);
                UTF32 *PrefixPath       = UTF32_Insert(Path32, UNCPathPrefix, StringHasBOM == Yes ? UTF16BOMSizeInCodeUnits : 0);
                Path16                  = UTF16_Encode(PrefixPath);
            } else {
                Path16                  = UTF16_Encode(Path32);
            }
#endif
        } else if (BitO == NULL) {
            Log(Log_DEBUG, __func__, U8("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_DEBUG, __func__, U8("Path2Open Pointer is NULL"));
        }
    }
    
    void BitOutput_OpenSocket(BitOutput *BitO, int Domain, int Type, int Protocol) {
        if (BitO != NULL) {
            BitO->FileType  = BitIOSocket;
            BitO->Socket    = FoundationIO_Socket_Create(Domain, Type, Protocol);
        } else {
            Log(Log_DEBUG, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    void BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, uint64_t SocketSize) {
        if (BitO != NULL && SocketAddress != NULL) {
            BitO->FileType = BitIOSocket;
            FoundationIO_Socket_Connect(BitO->Socket, SocketAddress, SocketSize);
        } else if (BitO == NULL) {
            Log(Log_DEBUG, __func__, U8("BitOutput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Log_DEBUG, __func__, U8("SocketAddress Pointer is NULL"));
        }
    }
    
    void BitOutput_Deinit(BitOutput *BitO) {
        if (BitO != NULL) {
            fflush(BitO->File);
            if (BitO->FileType == BitIOFile) {
                FoundationIO_File_Close(BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
                FoundationIO_Socket_Close(BitO->Socket);
            }
            free(BitO);
        } else {
            Log(Log_DEBUG, __func__, U8("BitOutput Pointer is NULL"));
        }
    }
    /* BitOutput */
    
    /* GUUID */
    uint8_t *GUUID_Generate(Entropy *Random, GUUIDTypes GUUIDType) {
        uint8_t *GUUID                   = 0;
        if (Random != NULL && GUUIDType != UnknownGUUID) {
            uint64_t LowBits             = Entropy_GenerateIntegerInRange(Random, 0, 0xFFFFFFFFFFFFFFFF);
            uint64_t HighBits            = Entropy_GenerateIntegerInRange(Random, 0, 0xFFFFFFFFFFFFFFFF);
            // Honestly I'll just convert from BinaryGUUID to GUUIDString if I have to
            uint8_t *BinaryGUUIDData     = calloc(BinaryGUUIDSize, sizeof(uint8_t));
            if (GUUID != NULL) {
                for (uint8_t GUUIDByte = 0; GUUIDByte < BinaryGUUIDSize - 1; GUUIDByte++) {
                    if (GUUIDByte < 8) {
                        uint8_t Byte     = (LowBits  & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                        BinaryGUUIDData[GUUIDByte] = Byte;
                    } else {
                        uint8_t Byte     = (HighBits & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                        BinaryGUUIDData[GUUIDByte] = Byte;
                    }
                }
                if (GUUIDType == GUIDString || GUUIDType == UUIDString) {
                    GUUID                = GUUID_Convert(BinaryGUID, GUIDString, BinaryGUUIDData);
                } else {
                    GUUID                = BinaryGUUIDData;
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate GUUID"));
            }
        } else if (Random == NULL) {
            Log(Log_DEBUG, __func__, U8("Entropy Pointer is NULL"));
        } else if (GUUIDType == UnknownGUUID) {
            Log(Log_DEBUG, __func__, U8("UnknownGUUID is an invalid GUUIDType"));
        }
        return GUUID;
    }
    
    bool GUUID_Compare(GUUIDTypes Type2Compare, uint8_t *GUUID1, uint8_t *GUUID2) {
        uint8_t GUUIDSize       = ((Type2Compare == GUIDString || Type2Compare == UUIDString) ? BinaryGUUIDSize : BinaryGUUIDSize);
        bool GUUIDsMatch        = Yes;
        if (GUUID1 != NULL && GUUID2 != NULL && Type2Compare != UnknownGUUID) {
            for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < GUUIDSize - 1; BinaryGUUIDByte++) {
                if (GUUID1[BinaryGUUIDByte] != GUUID2[BinaryGUUIDByte]) {
                    GUUIDsMatch = No;
                }
            }
        } else if (GUUID1 == NULL) {
            Log(Log_DEBUG, __func__, U8("GUUID1 Pointer is NULL"));
        } else if (GUUID2 == NULL) {
            Log(Log_DEBUG, __func__, U8("GUUID2 Pointer is NULL"));
        } else if (Type2Compare == UnknownGUUID) {
            Log(Log_DEBUG, __func__, U8("UnknownGUUID is an invalid GUUID type"));
        }
        return GUUIDsMatch;
    }
    
    uint8_t *GUUID_Convert(GUUIDTypes InputType, GUUIDTypes OutputType, uint8_t *GUUID2Convert) {
        uint8_t  Dash = '-';
        uint8_t  OutputGUUIDSize = ((OutputType == GUIDString || OutputType == UUIDString) ? GUUIDStringSize : BinaryGUUIDSize);
        uint8_t *ConvertedGUUID  = calloc(OutputGUUIDSize, sizeof(uint8_t));
        if (ConvertedGUUID != NULL && GUUID2Convert != NULL && InputType != UnknownGUUID && OutputType != UnknownGUUID) {
            bool ByteOrderDiffers = (((InputType == GUIDString && OutputType == UUIDString) || (InputType == UUIDString && OutputType == GUIDString) || (InputType == BinaryUUID && OutputType == BinaryGUID) || (InputType == BinaryGUID && OutputType == BinaryUUID)) ? Yes : No);
            
            bool TypeDiffers      = (((InputType == GUIDString && OutputType == BinaryGUID) || (InputType == BinaryGUID && OutputType == GUIDString) || (InputType == UUIDString && OutputType == BinaryUUID) || (InputType == BinaryUUID && OutputType == UUIDString)) ? Yes : No);
            
            if (ByteOrderDiffers == Yes) {
                GUUID_Swap(InputType, GUUID2Convert);
            }
            
            if (TypeDiffers == Yes) {
                if ((InputType == UUIDString || InputType == GUIDString) && (OutputType == BinaryUUID || OutputType == BinaryGUID)) {
                    for (uint8_t StringByte = 0; StringByte < BinaryGUUIDSize - 1; StringByte++) {
                        for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUIDSize - 1; BinaryByte++) {
                            if (GUUID2Convert[StringByte] != Dash) {
                                ConvertedGUUID[BinaryByte] = GUUID2Convert[StringByte];
                            }
                        }
                    }
                } else if ((InputType == BinaryUUID || InputType == BinaryGUID) || (OutputType == UUIDString || OutputType == GUIDString)) {
                    for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUIDSize - 1; BinaryByte++) {
                        for (uint8_t StringByte = 0; StringByte < BinaryGUUIDSize - 1; StringByte++) {
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
            Log(Log_DEBUG, __func__, U8("Insufficent memory to allocate ConvertedGUUID"));
        } else if (GUUID2Convert == NULL) {
            Log(Log_DEBUG, __func__, U8("GUUID Pointer is NULL"));
        } else if (InputType == UnknownGUUID) {
            Log(Log_DEBUG, __func__, U8("InputType is invalid"));
        } else if (OutputType == UnknownGUUID) {
            Log(Log_DEBUG, __func__, U8("OutputType is invalid"));
        }
        return ConvertedGUUID;
    }
    
    uint8_t *GUUID_Swap(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap) {
        uint8_t *SwappedGUUID = NULL;
        if (GUUID2Swap != NULL && GUUIDType != UnknownGUUID) {
            uint8_t Dash = '-';
            if (GUUIDType == UUIDString || GUUIDType == GUIDString) {
                SwappedGUUID = calloc(BinaryGUUIDSize, sizeof(uint8_t));
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
                    
                    for (uint8_t EndBytes = 13; EndBytes < GUUIDStringSize - 1; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("SwappedGUUID Pointer is NULL"));
                }
            } else if (GUUIDType == BinaryUUID || GUUIDType == BinaryGUID) {
                SwappedGUUID = calloc(BinaryGUUIDSize, sizeof(uint8_t));
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
                    for (uint8_t EndBytes = 10; EndBytes < BinaryGUUIDSize - 1; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("SwappedGUUID Pointer is NULL"));
                }
            }
        } else if (GUUID2Swap == NULL) {
            Log(Log_DEBUG, __func__, U8("GUUID2Swap Pointer is NULL"));
        } else if (GUUIDType == UnknownGUUID) {
            Log(Log_DEBUG, __func__, U8("UnknownGUUID is an invalid GUUID type"));
        }
        return SwappedGUUID;
    }
    
    void GUUID_Deinit(uint8_t *GUUID) {
        if (GUUID != NULL) {
            free(GUUID);
        }
    }
    /* GUUID */
    
#ifdef __cplusplus
}
#endif
