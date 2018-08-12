#include "../include/BitIO.h"          /* Included for our declarations */
#include "../include/Log.h"            /* Included for Log declarations */
#include "../include/Math.h"           /* Included for Integer functions */

#ifdef __cplusplus
extern "C" {
#endif
    
    static inline uint8_t SwapBits(const uint8_t Byte) {
        return ((Byte & 0x80 >> 7)|(Byte & 0x40 >> 5)|(Byte & 0x20 >> 3)|(Byte & 0x10 >> 1)|(Byte & 0x8 << 1)|(Byte & 0x4 << 3)|(Byte & 0x2 << 5)|(Byte & 0x1 << 7));
    }
    
    static inline uint8_t CreateBitMaskLSBit(const uint8_t Bits2Extract) {
        return (uint8_t) Exponentiate(2, Bits2Extract) << (8 - Bits2Extract);
    }
    
    static inline uint8_t CreateBitMaskMSBit(const uint8_t Bits2Extract) {
        return (uint8_t) Exponentiate(2, Bits2Extract) >> (8 - Bits2Extract);
    }
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        uint8_t   *Buffer;
        uint64_t   NumBits;
        uint64_t   BitOffset;
    } BitBuffer;
    
    BitBuffer *BitBuffer_Init(const uint64_t BitBufferSize) {
        BitBuffer *BitB                  = calloc(1, sizeof(BitBuffer));
        if (BitB != NULL && BitBufferSize > 0) {
            BitB->Buffer                 = calloc(BitBufferSize, sizeof(uint8_t));
            if (BitB->Buffer != NULL) {
                BitB->NumBits            = Bytes2Bits(BitBufferSize);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate %lld bits for BitBuffer's buffer"), BitBufferSize);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("Couldn't allocate BitBuffer"));
        }
        return BitB;
    }
    
    uint64_t BitBuffer_GetSize(BitBuffer *BitB) {
        uint64_t BitBufferSize = 0ULL;
        if (BitB != NULL) {
            BitBufferSize      = BitB->NumBits;
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return BitBufferSize;
    }
    
    void BitBuffer_SetSize(BitBuffer *BitB, uint64_t Bits) {
        if (BitB != NULL) {
            BitB->NumBits = Bits;
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB != NULL) {
            Position = BitB->BitOffset;
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Position;
    }
    
    void BitBuffer_SetPosition(BitBuffer *BitB, uint64_t Offset) {
        if (BitB != NULL) {
            BitB->BitOffset = Offset;
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetBitsFree(BitBuffer *BitB) { // GetBitsAvailable focuses to much on a writing perspective, I need a good, solid word that applies to both reading and writing. GetBitsFree?
        uint64_t BitsFree = 0ULL;
        if (BitB != NULL) {
            BitsFree      = BitB->NumBits - BitB->BitOffset;
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return BitsFree;
    }
    
    bool BitBuffer_IsAligned(BitBuffer *BitB, const uint8_t AlignmentSize) {
        bool AlignmentStatus = No;
        if (BitB != NULL && (AlignmentSize == 1 || AlignmentSize % 2 == 0)) {
            if (BitB->BitOffset % (AlignmentSize * 8) == 0) {
                AlignmentStatus = Yes;
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize == 1 || AlignmentSize % 2 == 0) {
            Log(Log_ERROR, __func__, U8("AlignmentSize: %d isn't an integer power of 2 (or 1)"), AlignmentSize);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, const uint8_t AlignmentSize) {
        if (BitB != NULL && (AlignmentSize == 1 || AlignmentSize % 2 == 0)) {
            int64_t AlignmentSizeInBits = Bytes2Bits(AlignmentSize);
            uint8_t Bits2Align          = AlignmentSizeInBits - (BitB->BitOffset % AlignmentSizeInBits);
            if (BitB->BitOffset + Bits2Align > BitB->NumBits) {
                BitB->Buffer            = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, Yes));
                BitB->NumBits          += Bits2Align;
            }
            BitB->BitOffset            += Bits2Align;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize == 1 || AlignmentSize % 2 == 0) {
            Log(Log_ERROR, __func__, U8("AlignmentSize: %d isn't a multiple of 2"), AlignmentSize);
        }
    }
    
    void BitBuffer_Skip(BitBuffer *BitB, const int64_t Bits2Skip) {
        if (BitB != NULL) {
            if (BitB->BitOffset + Bits2Skip > BitB->NumBits) {
                BitB->Buffer    = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Skip, Yes));
                BitB->NumBits  += Bits2Skip;
            }
            BitB->BitOffset    += Bits2Skip;
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Clear(BitBuffer *BitB) {
        if (BitB != NULL) {
            uint64_t BufferSize = BitB->NumBits / 8;
            for (uint64_t Byte = 0ULL; Byte < BufferSize; Byte++) {
                BitB->Buffer[Byte] = 0;
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Resize(BitBuffer *BitB, const uint64_t NewSize) { // We need to ensure that the new size is at least as large as the remaining bits
        if (BitB != NULL && NewSize >= BitBuffer_GetBitsFree(BitB)) {
            memset(BitB->Buffer, 0, Bits2Bytes(BitB->NumBits, No));
            free(BitB->Buffer);
            BitB->Buffer        = calloc(NewSize, sizeof(uint8_t));
            if (BitB->Buffer != NULL) {
                BitB->BitOffset = 0;
                BitB->NumBits   = Bits2Bytes(NewSize, No);
            } else {
                Log(Log_ERROR, __func__, U8("Allocating %lld bytes failed"), NewSize);
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Update(BitBuffer *BitB, BitInput *BitI) {
        if (BitI != NULL && BitB != NULL) {
            // Ok so we need to Get the number of bits that haven't yet been read from the buffer by subtracting BitOffset from NumBits.
            uint64_t NumBits2Keep      = BitBuffer_GetBitsFree(BitB);
            uint64_t BufferSizeInBytes = Bits2Bytes(BitB->NumBits, No);
            uint8_t *NewBuffer         = calloc(BufferSizeInBytes, sizeof(uint8_t));
            if (NewBuffer != NULL) {
                memmove(NewBuffer, BitB->Buffer + BufferSizeInBytes, Bits2Bytes(NumBits2Keep, No));
                memset(BitB->Buffer, 0, BufferSizeInBytes);
                free(BitB->Buffer);
                BitB->Buffer           = NewBuffer;
            } else {
                Log(Log_ERROR, __func__, U8("Allocating %lld bytes failed"), BufferSizeInBytes);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    void BitBuffer_Copy(BitBuffer *Source, BitBuffer *Destination, uint64_t BitStart, uint64_t BitEnd) {
        if (Source != NULL && Destination != NULL && BitStart < BitEnd && BitStart <= Source->NumBits && BitEnd <= Source->NumBits) {
            uint64_t NumBits2Copy = BitEnd - BitStart;
            if (BitStart % 8 == 0 && BitEnd % 8 == 0 && NumBits2Copy % 8 == 0) { // Multiple of 8, we can just copy the sumbitch as bytes
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
            Log(Log_ERROR, __func__, U8("Source Pointer is NULL"));
        } else if (Destination == NULL) {
            Log(Log_ERROR, __func__, U8("Destination Pointer is NULL"));
        } else if (BitStart >= BitEnd) {
            Log(Log_ERROR, __func__, U8("BitStart %lld is greater than or equal to BitEnd %lld"), BitStart, BitEnd);
        } else if (BitStart >= Source->NumBits || BitEnd >= Source->NumBits) {
            Log(Log_ERROR, __func__, U8("BitStart %lld or BitEnd %lld is greater than there are bits in Source %lld"), BitStart, BitEnd, Source->NumBits);
        }
    }
    
    void BitBuffer_Deinit(BitBuffer *BitB) {
        if (BitB != NULL) {
            free(BitB->Buffer);
            free(BitB);
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    /* BitBuffer Resource Management */
    static inline void InsertBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // The number of bits to pop into each loop of the byte is 8 - (BitB->BitOffset % 8)
        if (BitB != NULL && BitB->NumBits >= BitB->BitOffset + NumBits2Insert) {
            uint8_t  Bits                         = NumBits2Insert;
            while (Bits > 0) {
                if (NumBits2Insert % 8 == 0 && BitB->BitOffset % 8 == 0) {
                    // Just copy the bytes
                    for (uint8_t Byte = 0; Byte < Bits2Bytes(Bits, No); Byte++) {
                        BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)] = Data2Insert & 0xFF << Byte * 8;
                    }
                } else {
                    // Mask, Shift, etc.
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1, No);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1 + NumBits2Insert, No);
#endif
                    uint8_t  Bits2InsertForThisByte   = 8 - (BitB->BitOffset % 8);
                    uint8_t  BitMask                  = 0;
                    uint8_t  Byte                     = 0;
                    if (BitOrder == LSBitFirst) {
                        BitMask                       = CreateBitMaskLSBit(Bits2InsertForThisByte);
                    } else if (BitOrder == MSBitFirst) {
                        BitMask                       = CreateBitMaskMSBit(Bits2InsertForThisByte);
                    }
                    Byte                              = BitB->Buffer[ByteOffset] & BitMask;
                    if (BitOrder == LSBitFirst) {
                        // Right shift the Byte
                        Byte                          = Byte >> (8 - Bits2InsertForThisByte);
                    } else if (BitOrder == MSBitFirst) {
                        // Left shift the byte by 8 - he number of bits we extracted from this byte
                        Byte                          = Byte << (8 - Bits2InsertForThisByte);
                    }
                }
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Insert) {
            Log(Log_ERROR, __func__, U8("Not enough room in BitB to insert %d bits"), NumBits2Insert);
        }
    }
    
    static inline uint64_t ExtractBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Extract) {
        uint64_t     ExtractedBits                = 0ULL;
        if (BitB != NULL && BitB->NumBits >= BitB->BitOffset + 1 + NumBits2Extract) {
            uint8_t  Bits                         = NumBits2Extract;
            while (Bits > 0) {
                if (NumBits2Extract % 8 == 0 && BitB->BitOffset % 8 == 0) {
                    // Just copy the bytes
                    for (uint8_t Byte = 0; Byte < Bits2Bytes(Bits, No); Byte++) {
                        ExtractedBits = BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)];
                    }
                } else {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1, No);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1 + NumBits2Extract, No);
#endif
                    uint8_t  Bits2ExtractFromThisByte = 8 - (BitB->BitOffset % 8); //uint64_t Bits2Get = NumBits2ExtractFromByte(BitB->BitOffset, Bits);
                    uint8_t  BitMask                  = 0;
                    uint8_t  Byte                     = 0;
                    if (BitOrder == LSBitFirst) {
                        BitMask                       = CreateBitMaskLSBit(Bits2ExtractFromThisByte);
                    } else if (BitOrder == MSBitFirst) {
                        BitMask                       = CreateBitMaskMSBit(Bits2ExtractFromThisByte);
                    }
                    Byte                              = BitB->Buffer[ByteOffset] & BitMask;
                    if (BitOrder == LSBitFirst) {
                        // Right shift the Byte
                        Byte                          = Byte >> (8 - Bits2ExtractFromThisByte);
                    } else if (BitOrder == MSBitFirst) {
                        // Left shift the byte by 8 - he number of bits we extracted from this byte
                        Byte                          = Byte << (8 - Bits2ExtractFromThisByte);
                    }
                    ExtractedBits                     = Byte; // if the byte order matches the systems append it at the low side?
                }
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Extract) {
            Log(Log_ERROR, __func__, U8("Not enough bits in BitB to extract %d bits"), NumBits2Extract);
        }
        return ExtractedBits;
    }
    
    uint64_t PeekBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData      = ExtractBits(ByteOrder, BitOrder, BitB, Bits2Peek);
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            Log(Log_ERROR, __func__, U8("Bits2Peek %d is greater than BitBuffer can provide %lld, or greater than PeekBits can satisfy 1-64"), Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBits(ByteOrder, BitOrder, BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            Log(Log_ERROR, __func__, U8("Bits2Read %d is greater than BitBuffer can provide %lld, or greater than ReadBits can satisfy 1-64"), Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadUnary(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL) {
            do {
                OutputData    += 1;
            } while (ExtractBits(ByteOrder, BitOrder, BitB, 1) != StopBit);
            BitB->BitOffset   += OutputData;
            BitBuffer_Skip(BitB, 1); // Skip the stop bit
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return (UnaryType == CountUnary ? OutputData + 1 : OutputData);
    }
    
    UTF8    *ReadUTF8(BitBuffer *BitB, uint64_t StringSize) {
        UTF8 *ExtractedString             = calloc(StringSize, sizeof(UTF8));
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                ExtractedString[CodeUnit] = ExtractBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
        }
        return ExtractedString;
    }
    
    UTF16   *ReadUTF16(BitBuffer *BitB, uint64_t StringSize) {
        UTF16 *ExtractedString                    = calloc(StringSize, sizeof(UTF16));
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                ExtractedString[CodeUnit] = ExtractBits(LSByteFirst, LSBitFirst, BitB, 16);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                ExtractedString[CodeUnit] = ExtractBits(MSByteFirst, LSBitFirst, BitB, 16);
#endif
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return ExtractedString;
    }
    
    void     WriteBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            InsertBits(ByteOrder, BitOrder, BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(Log_ERROR, __func__, U8("NumBits2Write %d is greater than BitBuffer can provide %lld, or greater than WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
        }
    }
    
    void     WriteUnary(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const uint8_t UnaryBits2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            uint8_t Field2Write = UnaryBits2Write;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            InsertBits(ByteOrder, BitOrder, BitB, Logarithm(2, Field2Write), StopBit ^ 1); // Writing the unary pary
            InsertBits(ByteOrder, BitOrder, BitB, 1, StopBit); // Writing the stop bit
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void     WriteUTF8(BitBuffer *BitB, UTF8 *String2Write) {
        // Get the size of the string then write it out, after making sure the buffer is big enough to contain it
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize    = UTF8_GetStringSizeInCodeUnits(String2Write);
            uint64_t BitsAvailable = BitBuffer_GetBitsFree(BitB);
            if (BitsAvailable >= (uint64_t) Bytes2Bits(StringSize)) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                    InsertBits(MSByteFirst, LSBitFirst, BitB, 8, String2Write[CodeUnit]);
                }
            } else {
                Log(Log_ERROR, __func__, U8("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void     WriteUTF16(BitBuffer *BitB, UTF16 *String2Write) {
        // Get the size of the string then write it out, after making sure the buffer is big enough to contain it
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(String2Write);
            uint64_t BitsAvailable = BitBuffer_GetBitsFree(BitB);
            if (BitsAvailable >= (uint64_t) Bytes2Bits(StringSize)) { // If there's enough room to write the string, do it
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
#if    (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                    InsertBits(LSByteFirst, LSBitFirst, BitB, 16, String2Write[CodeUnit]);
#elif  (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                    InsertBits(MSByteFirst, LSBitFirst, BitB, 16, String2Write[CodeUnit]);
#endif
                }
            } else {
                Log(Log_ERROR, __func__, U8("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        
    }
    /* BitBuffer Resource Management */
    /* End BitBuffer section */
    
    /* BitInput */
    typedef struct BitInput {
        FILE                   *File;
        uint64_t                FileSpecifierNum;
        int64_t                 FileSize;
        int64_t                 FilePosition;
        int                     Socket;
        BitInputOutputFileTypes FileType;
        bool                    FileSpecifierExists;
    } BitInput;
    
    BitInput *BitInput_Init(void) {
        BitInput *BitI = calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("Couldn't allocate BitInput"));
        }
        return BitI;
    }
    
    void BitInput_OpenFile(BitInput *BitI, UTF8 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType                   = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            BitI->File                       = FoundationIO_FileOpen(Path2Open, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            bool  StringHasBOM               = UTF8_StringHasBOM(Path2Open);
            UTF32 *WinPath32                 = UTF8_Decode(Path2Open);
            UTF32 *WinPathLong32             = UTF32_Insert(WinPath32, U32("\\\\\?\\"), StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16                 = UTF16_Encode(WinPathLong32, UseLEByteOrder);
            free(WinPathLong32);
            BitI->File                       = FoundationIO_FileOpen(WinPath16, U16("rb"));
            free(WinPath16);
#endif
            if (BitI->File != NULL) {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't open file \"%s\": Check that the file exists and the permissions are correct"), Path2Open);
            }
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
    }
    
    void BitInput_OpenSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol) {
        if (BitI != NULL) {
            BitI->FileType = BitIOSocket;
            BitI->Socket   = FoundationIO_SocketCreate(Domain, Type, Protocol);
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    void BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitI != NULL && SocketAddress != NULL) {
            BitI->FileType = BitIOSocket;
            FoundationIO_SocketConnect(BitI->Socket, SocketAddress, SocketSize);
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Log_ERROR, __func__, U8("SocketAddress Pointer is NULL"));
        }
    }
    
    void BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const int64_t Bytes2Read) {
        if (BitI != NULL && Buffer2Read != NULL && Bytes2Read > (BitI->FileSize - BitI->FilePosition)) {
            if (Buffer2Read->Buffer  != NULL) {
                free(Buffer2Read->Buffer);
            }
            Buffer2Read->Buffer       = calloc(Bytes2Read, sizeof(uint8_t));
            if (Buffer2Read->Buffer != NULL) {
                int64_t BytesRead     = 0ULL;
                if (BitI->FileType   == BitIOFile) {
                    BytesRead         = FoundationIO_FileRead(Buffer2Read->Buffer, 1, Bytes2Read, BitI->File);
                } else if (BitI->FileType == BitIOSocket) {
                    FoundationIO_SocketRead(BitI->Socket, Buffer2Read->Buffer, Bytes2Read);
                }
                if (BytesRead == Bytes2Read) {
                    Buffer2Read->NumBits = Bytes2Bits(BytesRead);
                } else {
                    Log(Log_ERROR, __func__, U8("Fread read: %lld bytes, but you requested: %lld"), BytesRead, Bytes2Read);
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate BitBuffer's buffer"));
            }
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (Buffer2Read == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Bytes2Read > (BitI->FileSize - BitI->FilePosition)) {
            Log(Log_ERROR, __func__, U8("You tried reading more data: %lld than is available: %lld in the file"), Bytes2Read, BitI->FileSize - BitI->FilePosition);
        }
    }
    
    static void BitInput_FindFileSize(BitInput *BitI) {
        if (BitI != NULL) {
            FoundationIO_FileSeek(BitI->File, 0, SEEK_END);
            BitI->FileSize     = FoundationIO_FileGetSize(BitI->File);
            FoundationIO_FileSeek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition = FoundationIO_FileGetSize(BitI->File);
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    int64_t BitInput_GetFileSize(BitInput *BitI) {
        int64_t InputSize = 0LL;
        if (BitI != NULL) {
            if (BitI->FileSize == 0) {
                BitInput_FindFileSize(BitI);
            }
            InputSize    = BitI->FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
        return InputSize;
    }
    
    int64_t BitInput_GetFilePosition(BitInput *BitI) {
        int64_t Position = 0LL;
        if (BitI != NULL) {
            if (BitI->FilePosition == 0) {
                BitInput_FindFileSize(BitI);
            }
            Position    = BitI->FilePosition;
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
        return Position;
    }
    
    int64_t BitInput_BytesRemaining(BitInput *BitI) {
        int64_t BytesLeft = 0LL;
        if (BitI != NULL) {
            if (BitI->FilePosition == 0) {
                BitInput_FindFileSize(BitI);
            }
            BytesLeft    = BitI->FileSize - BitI->FilePosition;
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
        return BytesLeft;
    }
    
    void BitInput_Deinit(BitInput *BitI) {
        if (BitI != NULL) {
            if (BitI->FileType == BitIOFile) {
                FoundationIO_FileClose(BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
                FoundationIO_SocketClose(BitI->Socket);
            }
            free(BitI);
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    /* BitInput */
    
    
    /* BitOutput */
    typedef struct BitOutput {
        FILE                   *File;
        uint64_t                FileSpecifierNum;
        int64_t                 FilePosition;
        int                     Socket;
        BitInputOutputFileTypes FileType;
        bool                    FileSpecifierExists;
    } BitOutput;
    
    BitOutput *BitOutput_Init(void) {
        BitOutput *BitO = calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("Couldn't allocate BitOutput"));
        }
        return BitO;
    }
    
    void BitOutput_OpenFile(BitOutput *BitO, UTF8 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType         = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            BitO->File             = FoundationIO_FileOpen(Path2Open, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            bool  StringHasBOM     = UTF8_StringHasBOM(Path2Open);
            UTF32 *WinPath32       = UTF8_Decode(Path2Open);
            UTF32 *WinPathLong32   = UTF32_Insert(WinPath32, U32("\\\\\?\\"), StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16       = UTF16_Encode(WinPathLong32, UseLEByteOrder);
            free(WinPathLong32);
            BitO->File             = FoundationIO_FileOpen(WinPath16, U16("rb"));
            free(WinPath16);
#endif
            if (BitO->File != NULL) {
                setvbuf(BitO->File, NULL, _IONBF, 0);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't open file \"%s\": Check that the file exists and the permissions are correct"), Path2Open);
            }
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_ERROR, __func__, U8("Path2Open Pointer is NULL"));
        }
    }
    
    void BitOutput_OpenSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol) {
        if (BitO != NULL) {
            BitO->FileType  = BitIOSocket;
            BitO->Socket    = FoundationIO_SocketCreate(Domain, Type, Protocol);
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    void BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitO != NULL && SocketAddress != NULL) {
            BitO->FileType = BitIOSocket;
            FoundationIO_SocketConnect(BitO->Socket, SocketAddress, SocketSize);
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Log_ERROR, __func__, U8("SocketAddress Pointer is NULL"));
        }
    }
    
    void BitOutput_WriteBitBuffer(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write) {
        if (BitO != NULL && Buffer2Write != NULL) {
            uint64_t BytesWritten      = 0ULL;
            uint64_t BufferBytes       = Bits2Bytes(Buffer2Write->NumBits, Yes);
            uint64_t NumBytes2Write    = (Bytes2Write > BufferBytes ? Bytes2Write : BufferBytes);
            if (BitO->FileType == BitIOFile) {
                BytesWritten           = FoundationIO_FileWrite(Buffer2Write->Buffer, 1, NumBytes2Write, BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
                FoundationIO_SocketWrite(BitO->Socket, Buffer2Write->Buffer, NumBytes2Write);
            }
            if (BytesWritten != NumBytes2Write) {
                Log(Log_ERROR, __func__, U8("Fwrite wrote: %lld bytes, but you requested: %lld"), BytesWritten, NumBytes2Write);
            } else {
                Buffer2Write->NumBits -= Bytes2Bits(BytesWritten);
            }
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (Buffer2Write == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitOutput_Deinit(BitOutput *BitO) {
        if (BitO != NULL) {
            fflush(BitO->File);
            if (BitO->FileType == BitIOFile) {
                FoundationIO_FileClose(BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
                FoundationIO_SocketClose(BitO->Socket);
            }
            free(BitO);
        } else {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        }
    }
    /* BitOutput */
    
    /* GUUID */
    enum GUUIDConstants {
        GUUIDStringSize                 = 20 + NULLTerminatorSize,
        BinaryGUUIDSize                 = 16,
    };
    
    bool CompareGUUIDs(GUUIDTypes GUUIDType, const uint8_t *GUUID1, const uint8_t *GUUID2) {
        uint8_t GUUIDSize = ((GUUIDType == GUIDString || GUUIDType == UUIDString) ? BinaryGUUIDSize - NULLTerminatorSize : BinaryGUUIDSize);
        bool GUUIDsMatch        = Yes;
        if (GUUID1 != NULL && GUUID2 != NULL && GUUIDType != UnknownGUUID) {
            for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < GUUIDSize; BinaryGUUIDByte++) {
                if (GUUID1[BinaryGUUIDByte] != GUUID2[BinaryGUUIDByte]) {
                    GUUIDsMatch = No;
                }
            }
        } else if (GUUID1 == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID1 Pointer is NULL"));
        } else if (GUUID2 == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID2 Pointer is NULL"));
        } else if (GUUIDType == UnknownGUUID) {
            Log(Log_ERROR, __func__, U8("UnknownGUUID is an invalid GUUID type"));
        }
        return GUUIDsMatch;
    }
    
    uint8_t *ConvertGUUID(GUUIDTypes InputGUUIDType, GUUIDTypes OutputGUUIDType, uint8_t *GUUID2Convert) {
        uint8_t  OutputGUUIDSize = ((OutputGUUIDType == GUIDString || OutputGUUIDType == UUIDString) ? GUUIDStringSize : BinaryGUUIDSize);
        uint8_t *ConvertedGUUID  = calloc(OutputGUUIDSize, sizeof(uint8_t));
        if (ConvertedGUUID != NULL && GUUID2Convert != NULL && InputGUUIDType != UnknownGUUID && OutputGUUIDType != UnknownGUUID) {
            bool ByteOrderDiffers = (((InputGUUIDType == GUIDString && OutputGUUIDType == UUIDString) || (InputGUUIDType == UUIDString && OutputGUUIDType == GUIDString) || (InputGUUIDType == BinaryUUID && OutputGUUIDType == BinaryGUID) || (InputGUUIDType == BinaryGUID && OutputGUUIDType == BinaryUUID)) ? Yes : No);
            
            bool TypeDiffers      = (((InputGUUIDType == GUIDString && OutputGUUIDType == BinaryGUID) || (InputGUUIDType == BinaryGUID && OutputGUUIDType == GUIDString) || (InputGUUIDType == UUIDString && OutputGUUIDType == BinaryUUID) || (InputGUUIDType == BinaryUUID && OutputGUUIDType == UUIDString)) ? Yes : No);
            
            if (ByteOrderDiffers == Yes) {
                SwapGUUID(InputGUUIDType, GUUID2Convert);
            }
            
            if (TypeDiffers == Yes) {
                // Convert from a string to a binary, or vice versa.
                if ((InputGUUIDType == UUIDString || InputGUUIDType == GUIDString) && (OutputGUUIDType == BinaryUUID || OutputGUUIDType == BinaryGUID)) {
                    // Convert from string to binary
                    for (uint8_t StringByte = 0; StringByte < BinaryGUUIDSize; StringByte++) {
                        for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUIDSize; BinaryByte++) {
                            if (GUUID2Convert[StringByte] != 0x2D) {
                                ConvertedGUUID[BinaryByte] = GUUID2Convert[StringByte];
                            }
                        }
                    }
                } else if ((InputGUUIDType == BinaryUUID || InputGUUIDType == BinaryGUID) || (OutputGUUIDType == UUIDString || OutputGUUIDType == GUIDString)) {
                    // Convert from binary to string
                    for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUIDSize; BinaryByte++) {
                        for (uint8_t StringByte = 0; StringByte < BinaryGUUIDSize; StringByte++) {
                            if (BinaryByte != 4 && BinaryByte != 7 && BinaryByte != 10 && BinaryByte != 13) {
                                ConvertedGUUID[StringByte]  = GUUID2Convert[BinaryByte];
                            } else {
                                ConvertedGUUID[StringByte]  = '-';
                            }
                        }
                    }
                }
            }
        } else if (ConvertedGUUID == NULL) {
            Log(Log_ERROR, __func__, U8("Insufficent memory to allocate ConvertedGUUID"));
        } else if (GUUID2Convert == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID Pointer is NULL"));
        } else if (InputGUUIDType == UnknownGUUID) {
            Log(Log_ERROR, __func__, U8("InputGUUIDType is invalid"));
        } else if (OutputGUUIDType) {
            Log(Log_ERROR, __func__, U8("OutputGUUIDType is invalid"));
        }
        return ConvertedGUUID;
    }
    
    uint8_t *SwapGUUID(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap) {
        uint8_t *SwappedGUUID = NULL;
        if (GUUID2Swap != NULL && GUUIDType != UnknownGUUID) {
            if (GUUIDType == UUIDString || GUUIDType == GUIDString) {
                SwappedGUUID = calloc(BinaryGUUIDSize, sizeof(uint8_t));
                if (SwappedGUUID != NULL) {
                    SwappedGUUID[0]   = GUUID2Swap[3];
                    SwappedGUUID[1]   = GUUID2Swap[2];
                    SwappedGUUID[2]   = GUUID2Swap[1];
                    SwappedGUUID[3]   = GUUID2Swap[0];
                    
                    SwappedGUUID[4]   = GUUID2Swap[4]; // Dash
                    
                    SwappedGUUID[5]   = GUUID2Swap[6];
                    SwappedGUUID[6]   = GUUID2Swap[5];
                    
                    SwappedGUUID[7]   = GUUID2Swap[7]; // Dash
                    
                    SwappedGUUID[8]   = GUUID2Swap[9];
                    SwappedGUUID[9]   = GUUID2Swap[8];
                    
                    SwappedGUUID[10]  = GUUID2Swap[10]; // Dash
                    
                    SwappedGUUID[11]  = GUUID2Swap[12];
                    SwappedGUUID[12]  = GUUID2Swap[11];
                    for (uint8_t EndBytes = 13; EndBytes < GUUIDStringSize - NULLTerminatorSize; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("SwappedGUUID Pointer is NULL"));
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
                    for (uint8_t EndBytes = 10; EndBytes < BinaryGUUIDSize; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("SwappedGUUID Pointer is NULL"));
                }
            }
        } else if (GUUID2Swap == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID2Swap Pointer is NULL"));
        } else if (GUUIDType == UnknownGUUID) {
            Log(Log_ERROR, __func__, U8("UnknownGUUID is an invalid GUUID type"));
        }
        return SwappedGUUID;
    }
    
    uint8_t *ReadGUUID(GUUIDTypes GUUIDType, BitBuffer *BitB) {
        uint8_t ByteOrder = ((GUUIDType == GUIDString || GUUIDType == BinaryGUID) ? LSByteFirst : MSByteFirst);
        uint8_t *GUUID = NULL;
        if (GUUIDType != UnknownGUUID && BitB != NULL && (BitBuffer_GetSize(BitB) - BitBuffer_GetPosition(BitB)) >= BinaryGUUIDSize) {
            if (GUUIDType == BinaryUUID || GUUIDType == BinaryGUID) {
                // Read it from the BitBuffer as a string.
                GUUID = calloc(BinaryGUUIDSize, sizeof(uint8_t));
                if (GUUID != NULL) {
                    for (uint8_t Byte = 0; Byte < BinaryGUUIDSize - NULLTerminatorSize; Byte++) {
                        GUUID[Byte] = ReadBits(ByteOrder, LSBitFirst, BitB, 8);
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate GUIDString"));
                }
            } else if (GUUIDType == UUIDString || GUUIDType == GUIDString) {
                if (GUUID != NULL) {
                    uint32_t Section1    = ReadBits(ByteOrder, LSBitFirst, BitB, 32);
                    BitBuffer_Skip(BitB, 8);
                    uint16_t Section2    = ReadBits(ByteOrder, LSBitFirst, BitB, 16);
                    BitBuffer_Skip(BitB, 8);
                    uint16_t Section3    = ReadBits(ByteOrder, LSBitFirst, BitB, 16);
                    BitBuffer_Skip(BitB, 8);
                    uint16_t Section4    = ReadBits(ByteOrder, LSBitFirst, BitB, 16);
                    BitBuffer_Skip(BitB, 8);
                    uint64_t Section5    = ReadBits(ByteOrder, LSBitFirst, BitB, 48);
                    GUUID                = UTF8_FormatString(U8("%d-%d-%d-%d-%llu"), Section1, Section2, Section3, Section4, Section5);
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate UUIDString"));
                }
            }
        }
        return GUUID;
    }
    
    void WriteGUUID(GUUIDTypes GUUIDType, BitBuffer *BitB, const uint8_t *GUUID2Write) {
        if (BitB != NULL && BitBuffer_GetPosition(BitB)  && GUUID2Write != NULL) { // TODO: Make sure that the BitBuffer can hold the GUUID
            uint8_t GUUIDSize = ((GUUIDType == GUIDString || GUUIDType == UUIDString) ? GUUIDStringSize - NULLTerminatorSize : BinaryGUUIDSize);
            uint8_t ByteOrder = ((GUUIDType == GUIDString || GUUIDType == BinaryGUID) ? LSByteFirst : MSByteFirst);
            for (uint8_t Byte = 0; Byte < GUUIDSize; Byte++) {
                WriteBits(ByteOrder, LSBitFirst, BitB, 8, GUUID2Write[Byte]);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (GUUID2Write == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID2Write Pointer is NULL"));
        }
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
