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
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
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
            uint8_t AlignmentSizeInBits = Bytes2Bits(AlignmentSize);
            uint8_t Bits2Align          = AlignmentSizeInBits - (BitB->BitOffset % AlignmentSizeInBits);
            if (BitB->BitOffset + Bits2Align > BitB->NumBits) {
                BitB->Buffer            = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, Yes));
                BitB->NumBits          += Bits2Align;
            }
            BitB->BitOffset            += Bits2Align;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize == 1 || AlignmentSize % 2 == 0) {
            Log(Log_ERROR, __func__, U8("AlignmentSize: %d isn't a power of 2 (or 1)"), AlignmentSize);
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
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
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
    static inline void InsertBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
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
                    uint64_t ByteOffset               = (ByteOrder == GlobalByteOrder ? Bits2Bytes(BitB->BitOffset + 1, No):Bits2Bytes(BitB->BitOffset + 1 + NumBits2Insert, No));
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
            Log(Log_ERROR, __func__, U8("BitB's Pointer is NULL"));
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Insert) {
            Log(Log_ERROR, __func__, U8("Not enough room in BitB to insert %d bits"), NumBits2Insert);
        }
    }
    
    static inline uint64_t ExtractBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Extract) {
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
                    uint64_t ByteOffset               = (ByteOrder == GlobalByteOrder ? Bits2Bytes(BitB->BitOffset + 1, No):Bits2Bytes(BitB->BitOffset + 1 + NumBits2Extract, No));
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
            Log(Log_ERROR, __func__, U8("BitB's Pointer is NULL"));
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Extract) {
            Log(Log_ERROR, __func__, U8("Not enough bits in BitB to extract %d bits"), NumBits2Extract);
        }
        return ExtractedBits;
    }
    
    uint64_t PeekBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Peek) {
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
    
    uint64_t ReadBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Read) {
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
    
    uint64_t ReadUnary(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
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
    
    uint64_t ReadExpGolomb(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL && (StopBit == 0 || StopBit == 1)) {
            uint8_t Bits2Read  = Logarithm(2, ReadUnary(ByteOrder, BitOrder, BitB, UnaryType, StopBit));
            OutputData         = ReadBits(ByteOrder, BitOrder, BitB, Bits2Read);
            BitB->BitOffset   += OutputData;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return OutputData;
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
    
    UTF16   *ReadUTF16(ByteBitOrders StringByteOrder, BitBuffer *BitB, uint64_t StringSize) {
        /* How do we handle the byte order? Well if it was written with a BOM we can read the BOM to figure out the encoded byte and bit order */
        /* But if it wasn't... we'll need external information, ok so what information do we need? just the byte order. */
        /* Ok, so if the StringByteOrder is set to UnknownByteFirst, read it from the BOM. */
        UTF16 *ExtractedString                    = calloc(StringSize, sizeof(UTF16));
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                if (StringByteOrder != UnknownByteFirst) {
                    uint16_t BOM                  = PeekBits(MSByteFirst, LSBitFirst, BitB, 16);
                    if (BOM != UTF16BE && BOM != UTF16LE) {
                        ExtractedString[CodeUnit] = ExtractBits(LSByteFirst, LSBitFirst, BitB, 16);
                    } else {
                        ExtractedString[CodeUnit] = ExtractBits(MSByteFirst, LSBitFirst, BitB, 16);
                    }
                } else {
                    ExtractedString[CodeUnit]     = ExtractBits(StringByteOrder, LSBitFirst, BitB, 16);
                }
            }
        }
        return ExtractedString;
    }
    
    void     WriteBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            InsertBits(ByteOrder, BitOrder, BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(Log_ERROR, __func__, U8("NumBits2Write %d is greater than BitBuffer can provide %lld, or greater than WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
        }
    }
    
    void     WriteUnary(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const uint8_t UnaryBits2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            uint8_t Field2Write = UnaryBits2Write;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            InsertBits(ByteOrder, BitOrder, BitB, Logarithm(2, Field2Write), ~StopBit); // Writing the unary pary
            InsertBits(ByteOrder, BitOrder, BitB, 1, StopBit); // Writing the stop bit
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void     WriteExpGolomb(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write) {
        if (BitB != NULL) {
            uint8_t NumBits2Write = Logarithm(2, Field2Write);
            WriteUnary(ByteOrder, BitOrder, BitB, UnaryType, StopBit, NumBits2Write);
            if (UnaryType == CountUnary) {
                InsertBits(ByteOrder, BitOrder, BitB, NumBits2Write, Field2Write);
            } else if (UnaryType == WholeUnary) {
                if (Field2Write > 0) {
                    InsertBits(ByteOrder, BitOrder, BitB, NumBits2Write, Field2Write * 2);
                } else {
                    InsertBits(ByteOrder, BitOrder, BitB, NumBits2Write, Absolute(Field2Write * 2) - 1);
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    /*!
     @param BitB              "Pointer to BitBuffer the array will be written to".
     @param Array2Write       "The array to write to the BitBuffer".
     @param ElementSize       "The size of the type of array, uint16_t would be 16, etc".
     @param NumElements2Write "The number of elements from the array to write".
     @param ElementOffset     "which element should we start writing from"?
     @param OutputByteOrder   "the byte order the elements should be written in".
     @param OutputBitOrder    "the bit order the elements should be written in".
     @param Bits2Write        "The number of bits to write from each element, if only 14 bits are used, then just write 14 bits".
     */
    void     WriteArray2BitBuffer(BitBuffer *BitB, const void *Array2Write, const uint8_t ElementSize, const uint64_t NumElements2Write, const uint64_t ElementOffset) {
        
        if (BitB != NULL && Array2Write != NULL && ElementSize > 0 && NumElements2Write > 0) {
            
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Array2Write == NULL) {
            Log(Log_ERROR, __func__, U8("Array2Write Pointer is NULL"));
        } else if (ElementSize == 0 || ElementSize > 64) {
            Log(Log_ERROR, __func__, U8("ElementSize is %d, that doesn't make any sense"), ElementSize);
        } else if (NumElements2Write == 0 || NumElements2Write > ElementOffset) {
            Log(Log_ERROR, __func__, U8("NumElements2Write %lld makes no sense"), NumElements2Write);
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
            Log(Log_ERROR, __func__, U8("String2Write Pointer is NULL"));
        }
    }
    
    void     WriteUTF16(ByteBitOrders StringByteOrder, BitBuffer *BitB, UTF16 *String2Write) {
        // Get the size of the string then write it out, after making sure the buffer is big enough to contain it
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(String2Write);
            uint64_t BitsAvailable = BitBuffer_GetBitsFree(BitB);
            if (BitsAvailable >= (uint64_t) Bytes2Bits(StringSize)) { // If there's enough room to write the string, do it
                for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                    if ((StringByteOrder == MSByteFirst && String2Write[0] == UTF16LE) || (StringByteOrder == LSByteFirst && String2Write[0] == UTF16BE)) {
                        String2Write[CodeUnit] = SwapEndian16(String2Write[CodeUnit]);
                    }
                    InsertBits(StringByteOrder, LSBitFirst, BitB, 16, String2Write[CodeUnit]);
                }
            } else {
                Log(Log_ERROR, __func__, U8("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Log_ERROR, __func__, U8("String2Write Pointer is NULL"));
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
            BitI->FileType          = BitIOFile;
            if (BitI->FileSpecifierExists == Yes) {
                UTF8 *NewPath       = UTF8_FormatString(U8("%s%llu"), Path2Open, BitI->FileSpecifierNum);
                if (NewPath != NULL) {
                    BitI->File      = fopen(NewPath, U8("rb"));
                }
                free(NewPath);
            } else {
#if   (FoundationIOTargetOS == POSIXOS)
                BitI->File          = fopen(Path2Open, U8("rb"));
#elif (FoundationIOTargetOS == WindowsOS)
                UTF8  *WinPath8     = UTF8_FormatString("//?/%s", Path2Open);
                UTF32 *WinPath32    = UTF8_Decode(WinPath8);
                UTF16 *WinPath16    = UTF16_Encode(WinPath32, UseLEByteOrder);
                free(WinPath8);
                free(WinPath32);
                BitI->File          = _wfopen(Path16, U16("rb"));
                free(WinPath16);
#endif
            }
            if (BitI->File == NULL) {
                Log(Log_ERROR, __func__, U8("Couldn't open file: Check that the file exists and the permissions are correct"));
            } else {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            }
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_ERROR, __func__, U8("Path2Open Pointer is NULL"));
        }
    }
    
    void BitInput_OpenSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol) {
        if (BitI != NULL) {
            BitI->FileType   = BitIOSocket;
            BitI->Socket     = socket(Domain, Type, Protocol);
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    void BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitI != NULL && SocketAddress != NULL) {
            BitI->FileType = BitIOSocket;
            connect(BitI->Socket, SocketAddress, SocketSize);
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Log_ERROR, __func__, U8("SocketAddress Pointer is NULL"));
        }
    }
    
    void BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const int64_t Bytes2Read) {
        int64_t BytesRead             = 0ULL;
        if (BitI != NULL && Buffer2Read != NULL && Bytes2Read > (BitI->FileSize - BitI->FilePosition)) {
            if (Buffer2Read->Buffer  != NULL) {
                free(Buffer2Read->Buffer);
            }
            Buffer2Read->Buffer       = calloc(Bytes2Read, sizeof(uint8_t));
            if (Buffer2Read->Buffer != NULL) {
                if (BitI->FileType   == BitIOFile) {
                    BytesRead         = fread(Buffer2Read->Buffer, 1, Bytes2Read, BitI->File);
                } else if (BitI->FileType == BitIOSocket) {
#if   (FoundationIOTargetOS == POSIXOS)
                    BytesRead         = read(BitI->Socket, Buffer2Read->Buffer, Bytes2Read);
#elif (FoundationIOTargetOS == WindowsOS)
                    BytesRead         = _read(BitI->Socket, Buffer2Read->Buffer, Bytes2Read);
#endif
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
            FoundationIO_Seek(BitI->File, 0, SEEK_END);
            BitI->FileSize     = FoundationIO_Tell(BitI->File);
            FoundationIO_Seek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition = FoundationIO_Tell(BitI->File);
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    fpos_t BitInput_GetFileSize(BitInput *BitI) {
        fpos_t InputSize = 0LL;
        if (BitI != NULL) {
            if (BitI->FileSize == 0) {
                BitInput_FindFileSize(BitI);
            }
            InputSize = BitI->FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
        return InputSize;
    }
    
    fpos_t BitInput_GetFilePosition(BitInput *BitI) {
        fpos_t Position = 0LL;
        if (BitI != NULL) {
            if (BitI->FilePosition != 0) {
                Position = BitI->FilePosition;
            } else {
                BitInput_FindFileSize(BitI);
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
        return Position;
    }
    
    fpos_t BitInput_BytesRemaining(BitInput *BitI) {
        fpos_t BytesLeft = 0LL;
        if (BitI != NULL) {
            BytesLeft = BitI->FileSize - BitI->FilePosition;
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
        return BytesLeft;
    }
    
    void BitInput_Deinit(BitInput *BitI) {
        if (BitI != NULL) {
            if (BitI->FileType == BitIOFile) {
                fclose(BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
#if   (FoundationIOTargetOS == POSIXOS)
                close(BitI->Socket);
#elif (FoundationIOTargetOS == WindowsOS)
                _close(BitI->Socket);
#endif          
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
            BitO->FileType          = BitIOFile;
            uint64_t Path2OpenSize  = UTF8_GetStringSizeInCodePoints(Path2Open) + BitIONULLStringSize;
            if (BitO->FileSpecifierExists == Yes) {
                UTF8 *NewPath       = calloc(Path2OpenSize, sizeof(UTF8));
                snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitO->FileSpecifierNum); // FIXME: HANDLE FORMAT STRINGS BETTER
                free(NewPath);
            }
#if   (FoundationIOTargetOS == POSIXOS)
            BitO->File              = fopen(Path2Open, U8("wb"));
#elif (FoundationIOTargetOS == WindowsOS)
            UTF32 *Path32           = UTF8_Decode(Path2Open);
            UTF16 *Path16           = UTF16_Encode(Path32, UseLEByteOrder);
            free(Path32);
            BitO->File              = _wfopen(Path16, U16("rb"));
            free(Path16);
#endif
            if (BitO->File != NULL) {
                setvbuf(BitO->File, NULL, _IONBF, 0);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't open output file; Check that the path exists and the permissions are correct"));
            }
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_ERROR, __func__, U8("Path2Open Pointer is NULL"));
        }
    }
    
    int32_t BitOutput_RenameFile(UTF8 *OriginalPath, UTF8 *NewPath) {
        int32_t ErrorCode     = 0L;
        if (OriginalPath != NULL && NewPath != NULL) {
#if   (FoundationIOTargetOS == POSIXOS)
            ErrorCode         = rename(OriginalPath, NewPath);
#elif (FoundationIOTargetOS == WindowsOS)
            UTF32 *Original32 = UTF8_Decode(OriginalPath);
            UTF32 *New32      = UTF8_Decode(NewPath);
            UTF16 *Original16 = UTF16_Encode(Original32, UseLEByteOrder);
            free(Original32);
            UTF16 *New16      = UTF16_Encode(New32, UseLEByteOrder);
            ErrorCode         = _wrename(Original16, New16);
#endif
        } else if (OriginalPath == NULL) {
            Log(Log_ERROR, __func__, U8("OriginalPath Pointer is NULL"));
        } else if (NewPath == NULL) {
            Log(Log_ERROR, __func__, U8("NewPath Pointer is NULL"));
        }
        return ErrorCode;
    }
    
    void BitOutput_OpenSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol) {
        if (BitO != NULL) {
            BitO->FileType  = BitIOSocket;
            BitO->Socket    = socket(Domain, Type, Protocol);
        } else {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
    }
    
    void BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitO != NULL && SocketAddress != NULL) {
            BitO->FileType = BitIOSocket;
            connect(BitO->Socket, SocketAddress, SocketSize);
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Log_ERROR, __func__, U8("SocketAddress Pointer is NULL"));
        }
    }
    
    void BitOutput_WriteBitBuffer(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write) {
        uint64_t BytesWritten          = 0ULL;
        if (BitO != NULL && Buffer2Write != NULL) {
            uint64_t BufferBytes       = Bits2Bytes(Buffer2Write->NumBits, Yes);
            uint64_t NumBytes2Write    = (Bytes2Write > BufferBytes ? Bytes2Write : BufferBytes);
            if (BitO->FileType == BitIOFile) {
                BytesWritten           = fwrite(Buffer2Write->Buffer, 1, NumBytes2Write, BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
#if   (FoundationIOTargetOS == POSIXOS)
                BytesWritten           = write(BitO->Socket, Buffer2Write->Buffer, NumBytes2Write);
#elif (FoundationIOTargetOS == WindowsOS)
                BytesWritten           = _write(BitO->Socket, Buffer2Write->Buffer, NumBytes2Write);
#endif
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
                fclose(BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
#if   (FoundationIOTargetOS == POSIXOS)
                close(BitO->Socket);
#elif (FoundationIOTargetOS == WindowsOS)
                _close(BitO->Socket);
#endif  
            }
            free(BitO);
        } else {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        }
    }
    /* BitOutput */
    
#ifdef __cplusplus
}
#endif
