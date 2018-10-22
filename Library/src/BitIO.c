#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/BitIO.h"          /* Included for our declarations */
#include "../include/Log.h"            /* Included for Log declarations */
#include "../include/Math.h"           /* Included for Integer functions */
#include "../include/StringIO.h"       /* Included for StringIO's declarations */

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
                                GUUIDStringSize                 = 20 + NULLTerminatorSize,
                                BinaryGUUIDSize                 = 16,
    };
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        uint8_t   *Buffer;
        uint64_t   NumBits;
        uint64_t   BitOffset;
    } BitBuffer;
    
    typedef struct BitInput {
        union InputPath { // and lets make sure to copy the path
            UTF32      *Path32;
            UTF16      *Path16;
            UTF8       *Path8;
        } InputPath;
        FILE           *File;
        int64_t         FileSpecifierNum;
        int64_t         FilePosition;
        int64_t         FileSize;
        int             Socket;
        bool            FileSpecifierExists;
        StringTypes     PathType;
        BitIOFileTypes  FileType;
    } BitInput;
    
    typedef struct BitOutput {
        union OutputPath {
            UTF32      *Path32;
            UTF16      *Path16;
            UTF8       *Path8;
        } OutputPath;
        FILE           *File;
        int64_t         FileSpecifierNum;
        int64_t         FilePosition;
        int             Socket;
        bool            FileSpecifierExists;
        StringTypes     PathType;
        BitIOFileTypes  FileType;
    } BitOutput;
    
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
    
    void BitBuffer_Read(BitBuffer *BitB, BitInput *BitI) {
        if (BitB != NULL && BitI != NULL) {
            uint64_t Bytes2Read    = Bits2Bytes(BitB->NumBits - BitB->BitOffset, No);
            uint8_t Bits2Save      = BitB->BitOffset % 8;
            if (Bits2Save > 0) {
                BitB->Buffer[0]    = 0;
                uint8_t Saved      = BitB->Buffer[Bytes2Read + 1] & CreateBitMask(Bits2Save);
                BitB->Buffer[0]    = Saved;
                BitB->BitOffset    = Bits2Save;
            } else {
                BitB->BitOffset    = 0;
            }
            for (int64_t Byte = Bits2Bytes(BitB->BitOffset, Yes); Byte < Bits2Bytes(BitB->NumBits, No); Byte++) {
                BitB->Buffer[Byte] = 0;
            }
            uint64_t Bytes2Read2   = Bits2Bytes(BitB->NumBits - BitB->BitOffset, No);
            uint64_t BytesRead     = 0ULL;
            if (BitI->FileType == BitIOFile) {
                BytesRead          = FoundationIO_FileRead(BitB->Buffer, 1, Bytes2Read2, BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
                BytesRead          = FoundationIO_SocketRead(BitI->Socket, BitB->Buffer, Bytes2Read2);
            }
            BitB->NumBits          = (BytesRead * 8) + BitB->BitOffset;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        }
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
    
    uint64_t BitBuffer_GetBitsFree(BitBuffer *BitB) {
        uint64_t BitsFree = 0ULL;
        if (BitB != NULL) {
            BitsFree      = BitB->NumBits - BitB->BitOffset;
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return BitsFree;
    }
    
    bool BitBuffer_IsAligned(BitBuffer *BitB, const uint8_t AlignmentSize) {
        bool AlignmentStatus    = No;
        if (BitB != NULL && (AlignmentSize != 1 || AlignmentSize % 2 != 0)) {
            if (BitB->BitOffset % (AlignmentSize * 8) == 0) {
                AlignmentStatus = Yes;
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize != 1 || AlignmentSize % 2 != 0) {
            Log(Log_ERROR, __func__, U8("AlignmentSize: %d isn't 1 or an integer power of 2"), AlignmentSize);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, const uint8_t AlignmentSize) {
        if (BitB != NULL && (AlignmentSize == 1 || AlignmentSize % 2 == 0)) {
            int64_t  AlignmentSizeInBits = Bytes2Bits(AlignmentSize);
            uint64_t Bits2Align          = AlignmentSizeInBits - (BitB->BitOffset % AlignmentSizeInBits);
            if (BitB->BitOffset + Bits2Align > BitB->NumBits) {
                BitB->Buffer             = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, Yes));
                BitB->NumBits           += Bits2Align;
            }
            BitB->BitOffset             += Bits2Align;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize == 1 || AlignmentSize % 2 == 0) {
            Log(Log_ERROR, __func__, U8("AlignmentSize: %d isn't a multiple of 2"), AlignmentSize);
        }
    }
    
    void BitBuffer_Seek(BitBuffer *BitB, const int64_t Bits2Seek) {
        if (BitB != NULL) {
            if (Bits2Seek > 0 && BitB->NumBits > BitB->BitOffset + Bits2Seek) {
                BitB->NumBits  += Bits2Seek;
            } else if (Bits2Seek < 0 && BitB->NumBits > BitB->BitOffset - Bits2Seek) {
                BitB->NumBits  += Bits2Seek;
            } else {
                Log(Log_ERROR, __func__, U8("There's not enough bits in BitBuffer %lld to seek %lld bits"), BitB->NumBits, Bits2Seek);
            }
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
    
    void BitBuffer_Resize(BitBuffer *BitB, const uint64_t NewSize) {
        if (BitB != NULL && NewSize * 8 >= BitB->BitOffset) {
            uint8_t *NewBuffer  = realloc(BitB->Buffer, NewSize);
            if (NewBuffer != NULL) {
                BitB->Buffer    = NewBuffer;
                BitB->BitOffset = 0;
                BitB->NumBits   = NewSize * 8;
            } else {
                Log(Log_ERROR, __func__, U8("Reallocing the BitBuffer failed"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Refresh(BitBuffer *BitB, BitInput *BitI) {
        if (BitB != NULL && BitI != NULL) {
            uint64_t Bytes2Read      = BitB->NumBits / 8;
            uint64_t BytesRead       = 0ULL;
            if (BitI->FileType == BitIOFile) {
                BytesRead            = FoundationIO_FileRead(BitB->Buffer, 1, Bytes2Read, BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
                BytesRead            = FoundationIO_SocketRead(BitI->Socket, BitB->Buffer, Bytes2Read);
            }
            if (BytesRead < Bytes2Read) {
                uint8_t *Reallocated = realloc(BitB->Buffer, BytesRead);
                if (Reallocated != NULL) {
                    BitB->Buffer     = Reallocated;
                    BitB->BitOffset  = 0;
                    BitB->NumBits    = BytesRead * 8;
                } else {
                    Log(Log_ERROR, __func__, U8("Reallocating the BitBuffer failed"));
                }
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
            Log(Log_ERROR, __func__, U8("Source Pointer is NULL"));
        } else if (Destination == NULL) {
            Log(Log_ERROR, __func__, U8("Destination Pointer is NULL"));
        } else if (BitStart >= BitEnd) {
            Log(Log_ERROR, __func__, U8("BitStart %lld is greater than or equal to BitEnd %lld"), BitStart, BitEnd);
        } else if (BitStart >= Source->NumBits || BitEnd >= Source->NumBits) {
            Log(Log_ERROR, __func__, U8("BitStart %lld or BitEnd %lld is greater than there are bits in Source %lld"), BitStart, BitEnd, Source->NumBits);
        }
    }
    
    /* BitBuffer Resource Management */
    static void BitBuffer_InsertBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        if (BitB != NULL && BitB->NumBits >= BitB->BitOffset + NumBits2Insert) {
            uint8_t  Bits                         = NumBits2Insert;
            while (Bits > 0) {
                if (NumBits2Insert % 8 == 0 && BitB->BitOffset % 8 == 0) {
                    for (uint8_t Byte = 0; Byte < Bits2Bytes(Bits, No); Byte++) {
                        BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)] = Data2Insert & 0xFF << Byte * 8;
                    }
                } else {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE) // FIXME: Assuming all platforms use LE byte order
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1, No);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1 + NumBits2Insert, No);
#endif
                    uint8_t  Bits2InsertForThisByte   = 8 - (BitB->BitOffset % 8);
                    uint8_t  BitMask                  = 0;
                    uint8_t  Byte                     = 0;
                    BitMask                           = BitB->Buffer[ByteOffset + 1] & CreateBitMask(Bits2InsertForThisByte);
                    if (BitOrder == LSBitFirst) {
                        Byte                        >>= 8 - Bits2InsertForThisByte;
                    } else if (BitOrder == MSBitFirst) {
                        Byte                        <<= 8 - Bits2InsertForThisByte;
                    }
                    BitB->Buffer[ByteOffset]          = Byte & BitMask;
                }
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Insert) {
            Log(Log_ERROR, __func__, U8("Not enough room in BitB to insert %d bits"), NumBits2Insert);
        }
    }
    
    static uint64_t BitBuffer_ExtractBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Extract) {
        uint64_t     ExtractedBits                = 0ULL;
        if (BitB != NULL && BitB->NumBits >= BitB->BitOffset + 1 + NumBits2Extract) {
            uint8_t  Bits                         = NumBits2Extract;
            while (Bits > 0) {
                if (NumBits2Extract % 8 == 0 && BitB->BitOffset % 8 == 0) {
                    for (uint8_t Byte = 0; Byte < Bits2Bytes(Bits, No); Byte++) {
                        ExtractedBits = BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)];
                    }
                } else {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE) // FIXME: Assuming all platforms use LE byte order
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1, No);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                    uint64_t ByteOffset               = Bits2Bytes(BitB->BitOffset + 1 + NumBits2Extract, No);
#endif
                    uint8_t  Bits2ExtractFromThisByte = 8 - (BitB->BitOffset % 8);
                    uint8_t  BitMask                  = 0;
                    uint8_t  Byte                     = 0;
                    BitMask                           = BitB->Buffer[ByteOffset + 1] & CreateBitMask(Bits2ExtractFromThisByte);
                    Byte                              = BitB->Buffer[ByteOffset] & BitMask;
                    if (BitOrder == LSBitFirst) {
                        Byte                        >>= 8 - Bits2ExtractFromThisByte;
                    } else if (BitOrder == MSBitFirst) {
                        Byte                        <<= 8 - Bits2ExtractFromThisByte;
                    }
                    ExtractedBits                     = Byte;
                }
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Extract) {
            Log(Log_ERROR, __func__, U8("Not enough bits in BitB to extract %d bits"), NumBits2Extract);
        }
        return ExtractedBits;
    }
    
    uint64_t BitBuffer_PeekBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData      = BitBuffer_ExtractBits(ByteOrder, BitOrder, BitB, Bits2Peek);
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            Log(Log_ERROR, __func__, U8("Bits2Peek %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_PeekBits can satisfy 1-64"), Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t BitBuffer_ReadBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = BitBuffer_ExtractBits(ByteOrder, BitOrder, BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            Log(Log_ERROR, __func__, U8("Bits2Read %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_ReadBits can satisfy 1-64"), Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t BitBuffer_ReadUnary(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL) {
            do {
                OutputData    += 1;
            } while (BitBuffer_ExtractBits(ByteOrder, BitOrder, BitB, 1) != StopBit);
            BitB->BitOffset   += OutputData;
            BitBuffer_Seek(BitB, 1);
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return (UnaryType == CountUnary ? OutputData + 1 : OutputData);
    }
    
    uint64_t BitBuffer_GetUTF8StringSize(BitBuffer *BitB) {
        uint64_t StringSize           = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset   = BitBuffer_GetPosition(BitB);
            uint16_t CodeUnit8        = 1;
            do {
                CodeUnit8             = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
                uint8_t  CodeUnitSize = UTF8_GetCodePointSizeInCodeUnits(CodeUnit8);
                StringSize           += CodeUnitSize;
            } while (CodeUnit8 != NULL);
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF8    *BitBuffer_ReadUTF8(BitBuffer *BitB, uint64_t StringSize) {
        UTF8 *ExtractedString             = calloc(StringSize, sizeof(UTF8));
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                ExtractedString[CodeUnit] = BitBuffer_ExtractBits(MSByteFirst, LSBitFirst, BitB, 8);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
            }
        }
        return ExtractedString;
    }
    
    uint64_t BitBuffer_GetUTF16StringSize(BitBuffer *BitB) {
        uint64_t StringSize           = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset   = BitBuffer_GetPosition(BitB);
            uint16_t CodeUnit16       = 1;
            do {
                CodeUnit16            = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
                uint8_t  CodeUnitSize = UTF16_GetCodePointSizeInCodeUnits(CodeUnit16);
                StringSize           += CodeUnitSize;
            } while (CodeUnit16 != NULL);
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF16   *BitBuffer_ReadUTF16(BitBuffer *BitB, uint64_t StringSize) {
        UTF16 *ExtractedString            = calloc(StringSize, sizeof(UTF16));
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
#if   (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                ExtractedString[CodeUnit] = BitBuffer_ExtractBits(LSByteFirst, LSBitFirst, BitB, 16);
#elif (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                ExtractedString[CodeUnit] = BitBuffer_ExtractBits(MSByteFirst, LSBitFirst, BitB, 16);
#endif
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return ExtractedString;
    }
    
    uint8_t *BitBuffer_ReadGUUID(BitBuffer *BitB, GUUIDTypes GUUID2Read) {
        uint8_t ByteOrder = ((GUUID2Read == GUIDString || GUUID2Read == BinaryGUID) ? LSByteFirst : MSByteFirst);
        uint8_t *GUUID = NULL;
        if (GUUID2Read != UnknownGUUID && BitB != NULL && (BitBuffer_GetSize(BitB) - BitBuffer_GetPosition(BitB)) >= BinaryGUUIDSize) {
            if (GUUID2Read == BinaryUUID || GUUID2Read == BinaryGUID) {
                // Read it from the BitBuffer as a string.
                GUUID = calloc(BinaryGUUIDSize, sizeof(uint8_t));
                if (GUUID != NULL) {
                    for (uint8_t Byte = 0; Byte < BinaryGUUIDSize - NULLTerminatorSize; Byte++) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                        GUUID[Byte] = BitBuffer_ReadBits(ByteOrder, LSBitFirst, BitB, 8);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate GUIDString"));
                }
            } else if (GUUID2Read == UUIDString || GUUID2Read == GUIDString) {
                if (GUUID != NULL) {
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(push)
#pragma warning(disable: 4090)
#endif
                    uint32_t Section1    = BitBuffer_ReadBits(ByteOrder, LSBitFirst, BitB, 32);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section2    = BitBuffer_ReadBits(ByteOrder, LSBitFirst, BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section3    = BitBuffer_ReadBits(ByteOrder, LSBitFirst, BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section4    = BitBuffer_ReadBits(ByteOrder, LSBitFirst, BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint64_t Section5    = BitBuffer_ReadBits(ByteOrder, LSBitFirst, BitB, 48);
#if   (FoundationIOCompiler == FoundationIOCompilerIsMSVC)
#pragma warning(pop)
#endif
                    GUUID                = UTF8_FormatString(U8("%d-%d-%d-%d-%llu"), Section1, Section2, Section3, Section4, Section5);
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate UUIDString"));
                }
            }
        }
        return GUUID;
    }
    
    void     BitBuffer_WriteBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            BitBuffer_InsertBits(ByteOrder, BitOrder, BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(Log_ERROR, __func__, U8("NumBits2Write %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
        }
    }
    
    void     BitBuffer_WriteUnary(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const uint8_t UnaryBits2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            uint8_t Field2Write = UnaryBits2Write;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            BitBuffer_InsertBits(ByteOrder, BitOrder, BitB, Logarithm(2, Field2Write), StopBit ^ 1);
            BitBuffer_InsertBits(ByteOrder, BitOrder, BitB, 1, StopBit);
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void     BitBuffer_WriteUTF8(BitBuffer *BitB, UTF8 *String2Write) {
        if (BitB != NULL && String2Write != NULL) {
            int64_t StringSize    = UTF8_GetStringSizeInCodeUnits(String2Write);
            int64_t BitsAvailable = BitBuffer_GetBitsFree(BitB);
            if (BitsAvailable >= Bytes2Bits(StringSize)) {
                for (int64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                    BitBuffer_InsertBits(MSByteFirst, LSBitFirst, BitB, 8, String2Write[CodeUnit]);
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
    
    void     BitBuffer_WriteUTF16(BitBuffer *BitB, UTF16 *String2Write) {
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(String2Write);
            uint64_t BitsAvailable = BitBuffer_GetBitsFree(BitB);
            if (BitsAvailable >= (uint64_t) Bytes2Bits(StringSize)) {
                UTF16 ByteOrder    = String2Write[0];
                if (ByteOrder == UTF16BOM_BE) {
                    for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
#if    (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                        BitBuffer_InsertBits(MSByteFirst, LSBitFirst, BitB, 16, String2Write[CodeUnit]);
#elif  (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                        BitBuffer_InsertBits(LSByteFirst, LSBitFirst, BitB, 16, String2Write[CodeUnit]);
#endif
                    }
                } else if (ByteOrder == UTF16BOM_LE) {
                    for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
#if    (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderLE)
                        BitBuffer_InsertBits(LSByteFirst, LSBitFirst, BitB, 16, String2Write[CodeUnit]);
#elif  (FoundationIOTargetByteOrder == FoundationIOCompileTimeByteOrderBE)
                        BitBuffer_InsertBits(MSByteFirst, LSBitFirst, BitB, 16, String2Write[CodeUnit]);
#endif
                    }
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
    
    void BitBuffer_WriteGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType, const uint8_t *GUUID2Write) {
        if (BitB != NULL && BitBuffer_GetPosition(BitB)  && GUUID2Write != NULL) { // TODO: Make sure that the BitBuffer can hold the GUUID
            uint8_t GUUIDSize = ((GUUIDType == GUIDString || GUUIDType == UUIDString) ? GUUIDStringSize - NULLTerminatorSize : BinaryGUUIDSize);
            uint8_t ByteOrder = ((GUUIDType == GUIDString || GUUIDType == BinaryGUID) ? LSByteFirst : MSByteFirst);
            for (uint8_t Byte = 0; Byte < GUUIDSize; Byte++) {
                BitBuffer_WriteBits(ByteOrder, LSBitFirst, BitB, 8, GUUID2Write[Byte]);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (GUUID2Write == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID2Write Pointer is NULL"));
        }
    }
    
    void BitBuffer_Write(BitBuffer *BitB, BitOutput *BitO) {
        if (BitB != NULL && BitO != NULL) {
            uint64_t Bytes2Write  = Bits2Bytes(BitB->BitOffset, No);
            uint64_t Bits2Keep    = BitB->BitOffset % 8;
            uint64_t BytesWritten = 0ULL;
            if (BitO->FileType == BitIOFile) {
                BytesWritten = FoundationIO_FileWrite(BitB->Buffer, 1, Bytes2Write, BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
                BytesWritten = FoundationIO_SocketWrite(BitO->Socket, BitB->Buffer, Bytes2Write);
            }
            if (BytesWritten == Bytes2Write) {
                BitB->Buffer[0] = 0;
                BitB->Buffer[0] = BitB->Buffer[Bytes2Write + 1] & (Exponentiate(2, Bits2Keep) << (8 - Bits2Keep));
                BitB->BitOffset = Bits2Keep + 1;
                for (int64_t Byte = Bits2Bytes(BitB->BitOffset, Yes); Byte < Bits2Bytes(BitB->NumBits, No); Byte++) {
                    BitB->Buffer[Byte] = 0;
                }
            } else {
                Log(Log_ERROR, __func__, U8("Wrote %lld of %lld bits"), BytesWritten * 8, Bytes2Write * 8);
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
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
    /* End BitBuffer section */
    
    /* BitInput */
    BitInput *BitInput_Init(void) {
        BitInput *BitI = calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("Couldn't allocate BitInput"));
        }
        return BitI;
    }
    
    void BitInput_UTF8_OpenFile(BitInput *BitI, UTF8 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType                   = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            UTF32 *Path32                    = UTF8_Decode(Path2Open);
            bool   PathHasBOM                = UTF32_StringHasBOM(Path32);
            if (PathHasBOM) {
                UTF32 *BOMLess               = UTF32_RemoveBOM(Path32);
                free(Path32);
                Path32                       = BOMLess;
            }
            bool  PathHasSpecifier           = UTF32_NumFormatSpecifiers(Path32);
            if (PathHasSpecifier) {
                UTF32 *Formatted             = UTF32_FormatString(Path32, BitI->FileSpecifierNum);
                BitI->FileSpecifierNum      += 1;
                free(Path32);
                Path32                       = Formatted;
            }
            UTF8  *Path8                     = UTF8_Encode(Path32);
            BitI->File                       = FoundationIO_FileOpen(Path8, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            bool  StringHasBOM               = UTF8_StringHasBOM(Path2Open);
            UTF32 *WinPath32                 = UTF8_Decode(Path2Open);
            UTF32 *WinPathLong32             = UTF32_Insert(WinPath32, U32("\\\\\?\\"), StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16                 = UTF16_Encode(WinPathLong32);
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
    
    void BitInput_UTF16_OpenFile(BitInput *BitI, UTF16 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType                   = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            UTF32 *Path32                    = UTF16_Decode(Path2Open);
            bool   PathHasBOM                = UTF32_StringHasBOM(Path32);
            if (PathHasBOM) {
                UTF32 *BOMLess               = UTF32_RemoveBOM(Path32);
                free(Path32);
                Path32                       = BOMLess;
            }
            bool  PathHasSpecifier           = UTF32_NumFormatSpecifiers(Path32);
            if (PathHasSpecifier) {
                UTF32 *Formatted             = UTF32_FormatString(Path32, BitI->FileSpecifierNum);
                BitI->FileSpecifierNum      += 1;
                free(Path32);
                Path32                       = Formatted;
            }
            UTF8  *Path8                     = UTF8_Encode(Path32);
            BitI->File                       = FoundationIO_FileOpen(Path8, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            bool  StringHasBOM               = UTF16_StringHasBOM(Path2Open);
            UTF32 *WinPath32                 = UTF16_Decode(Path2Open);
            UTF32 *WinPathLong32             = UTF32_Insert(WinPath32, U32("\\\\\?\\"), StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16                 = UTF16_Encode(WinPathLong32);
            free(WinPathLong32);
            BitI->File                       = FoundationIO_FileOpen(WinPath16, U16("rb"));
            free(WinPath16);
#endif
            if (BitI->File != NULL) {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            } else {
                UTF32 *Path32                = UTF16_Decode(Path2Open);
                UTF8  *Path8                 = UTF8_Encode(Path32);
                free(Path32);
                Log(Log_ERROR, __func__, U8("Couldn't open file \"%s\": Check that the file exists and the permissions are correct"), Path8);
                free(Path8);
            }
        } else if (BitI == NULL) {
            Log(Log_ERROR, __func__, U8("BitInput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
    }
    
    bool BitInput_IsChangable(BitInput *BitI) {
        bool BitOutputIsChangable    = No;
        if (BitI != NULL) {
            if (BitI->FileSpecifierExists == Yes) {
                BitOutputIsChangable = Yes;
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        }
        return BitOutputIsChangable;
    }
    
    void BitInput_ChangeFile(BitInput *BitI) {
        if (BitI != NULL) {
            if (BitI->FileSpecifierExists == Yes) {
                BitI->FilePosition      = 0;
                BitI->FileSpecifierNum += 1;
                fclose(BitI->File);
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
                UTF8  *NewPath     = UTF8_FormatString(BitI->InputPath.Path8, BitI->FileSpecifierNum);
                FoundationIO_FileOpen(NewPath, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
                UTF16 *NewPath     = UTF16_FormatString(BitI->InputPath.Path16, BitI->FileSpecifierNum);
                FoundationIO_FileOpen(NewPath, U16("rb"));
#endif
                free(NewPath);
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
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
            InputSize     = BitI->FileSize;
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
    BitOutput *BitOutput_Init(void) {
        BitOutput *BitO = calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("Couldn't allocate BitOutput"));
        }
        return BitO;
    }
    
    void BitOutput_UTF8_OpenFile(BitOutput *BitO, UTF8 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType              = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            BitO->FileSpecifierExists   = UTF8_NumFormatSpecifiers(Path2Open) >= 1 ? Yes : No;
            if (BitO->FileSpecifierExists == Yes) {
                BitO->OutputPath.Path8  = UTF8_Clone(Path2Open);
                UTF8 *Formatted         = UTF8_FormatString(Path2Open, BitO->FileSpecifierNum);
                BitO->File              = FoundationIO_FileOpen(Formatted, U8("rb"));
                BitO->FileSpecifierNum += 1;
            } else {
                BitO->File              = FoundationIO_FileOpen(BitO->OutputPath.Path8, U8("rb"));
            }
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            bool   StringHasPathPrefix  = UTF8_StringHasWinPathPrefix(Path2Open);
            BitO->FileSpecifierExists   = UTF8_NumFormatSpecifiers(Path2Open) >= 1 ? Yes : No;
            UTF32 *Path32               = UTF8_Decode(Path2Open);
            if (StringHasPathPrefix == No) {
                bool   StringHasBOM     = UTF8_StringHasBOM(Path2Open);
                UTF32 *PrefixPath       = UTF32_Insert(Path32, U32("\\\\\?\\"), StringHasBOM == Yes ? UTF16BOMSizeInCodeUnits : 0);
                BitO->OutputPath.Path16 = UTF16_Encode(PrefixPath);
            } else {
                BitO->OutputPath.Path16 = UTF16_Encode(Path32);
            }
            
            if (BitO->FileSpecifierExists == Yes) {
                UTF16 *Formatted        = UTF16_FormatString(BitO->OutputPath.Path16, BitO->FileSpecifierNum);
                BitO->File              = FoundationIO_FileOpen(Formatted, U16("rb"));
            } else {
                BitO->File              = FoundationIO_FileOpen(BitO->OutputPath.Path16, U16("rb"));
            }
#endif
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_ERROR, __func__, U8("Path2Open Pointer is NULL"));
        }
    }
    
    void BitOutput_UTF16_OpenFile(BitOutput *BitO, UTF16 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType              = BitIOFile;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
            // Convert to UTF-8, and remove the BOM because fopen will silently fail if there's a BOM.
            UTF32 *Decoded              = UTF16_Decode(Path2Open);
            bool   PathHasBOM           = UTF32_StringHasBOM(Decoded);
            BitO->FileSpecifierExists   = UTF32_NumFormatSpecifiers(Decoded) >= 1 ? Yes : No;
            if (PathHasBOM == Yes) {
                BitO->OutputPath.Path32 = UTF32_RemoveBOM(Decoded);
            }
            if (BitO->FileSpecifierExists == Yes) {
                UTF32 *Formatted        = UTF32_FormatString(BitO->OutputPath.Path32, BitO->FileSpecifierNum);
                UTF8  *Formatted8       = UTF8_Encode(Formatted);
                FoundationIO_FileOpen(Formatted8, U8("rb"));
            } else {
                UTF8 *Formatted8        = UTF8_Encode(BitO->OutputPath.Path32);
                FoundationIO_FileOpen(Formatted8, U8("rb"));
            }
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
            bool   StringHasPathPrefix  = UTF16_StringHasWinPathPrefix(Path2Open);
            BitO->FileSpecifierExists   = UTF16_NumFormatSpecifiers(Path2Open) >= 1 ? Yes : No;
            UTF32 *Path32               = UTF16_Decode(Path2Open);
            if (StringHasPathPrefix == No) {
                bool   StringHasBOM     = UTF16_StringHasBOM(Path2Open);
                UTF32 *PrefixPath       = UTF32_Insert(Path32, U32("\\\\\?\\"), StringHasBOM == Yes ? UTF16BOMSizeInCodeUnits : 0);
                BitO->OutputPath.Path16 = UTF16_Encode(PrefixPath);
            } else {
                BitO->OutputPath.Path16 = UTF16_Encode(Path32);
            }
#endif
        } else if (BitO == NULL) {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Log_ERROR, __func__, U8("Path2Open Pointer is NULL"));
        }
    }
    
    bool BitOutput_IsChangable(BitOutput *BitO) {
        bool BitOutputIsChangable    = No;
        if (BitO != NULL) {
            if (BitO->FileSpecifierExists == Yes) {
                BitOutputIsChangable = Yes;
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
        }
        return BitOutputIsChangable;
    }
    
    void BitOutput_ChangeFile(BitOutput *BitO) {
        if (BitO != NULL) {
            if (BitO->FileSpecifierExists == Yes) {
                BitO->FilePosition      = 0;
                BitO->FileSpecifierNum += 1;
                fclose(BitO->File);
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
                UTF8  *NewPath     = UTF8_FormatString(BitO->OutputPath.Path8, BitO->FileSpecifierNum);
                FoundationIO_FileOpen(NewPath, U8("rb"));
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
                UTF16 *NewPath     = UTF16_FormatString(BitO->OutputPath.Path16, BitO->FileSpecifierNum);
                FoundationIO_FileOpen(NewPath, U16("rb"));
#endif
                free(NewPath);
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitOutput Pointer is NULL"));
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
    bool GUUID_Compare(GUUIDTypes Type2Compare, const uint8_t *GUUID1, const uint8_t *GUUID2) {
        uint8_t GUUIDSize = ((Type2Compare == GUIDString || Type2Compare == UUIDString) ? BinaryGUUIDSize - NULLTerminatorSize : BinaryGUUIDSize);
        bool GUUIDsMatch        = Yes;
        if (GUUID1 != NULL && GUUID2 != NULL && Type2Compare != UnknownGUUID) {
            for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < GUUIDSize; BinaryGUUIDByte++) {
                if (GUUID1[BinaryGUUIDByte] != GUUID2[BinaryGUUIDByte]) {
                    GUUIDsMatch = No;
                }
            }
        } else if (GUUID1 == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID1 Pointer is NULL"));
        } else if (GUUID2 == NULL) {
            Log(Log_ERROR, __func__, U8("GUUID2 Pointer is NULL"));
        } else if (Type2Compare == UnknownGUUID) {
            Log(Log_ERROR, __func__, U8("UnknownGUUID is an invalid GUUID type"));
        }
        return GUUIDsMatch;
    }
    
    uint8_t *GUUID_Convert(GUUIDTypes InputType, GUUIDTypes OutputType, uint8_t *GUUID2Convert) {
        uint8_t  OutputGUUIDSize = ((OutputType == GUIDString || OutputType == UUIDString) ? GUUIDStringSize : BinaryGUUIDSize);
        uint8_t *ConvertedGUUID  = calloc(OutputGUUIDSize, sizeof(uint8_t));
        if (ConvertedGUUID != NULL && GUUID2Convert != NULL && InputType != UnknownGUUID && OutputType != UnknownGUUID) {
            bool ByteOrderDiffers = (((InputType == GUIDString && OutputType == UUIDString) || (InputType == UUIDString && OutputType == GUIDString) || (InputType == BinaryUUID && OutputType == BinaryGUID) || (InputType == BinaryGUID && OutputType == BinaryUUID)) ? Yes : No);
            
            bool TypeDiffers      = (((InputType == GUIDString && OutputType == BinaryGUID) || (InputType == BinaryGUID && OutputType == GUIDString) || (InputType == UUIDString && OutputType == BinaryUUID) || (InputType == BinaryUUID && OutputType == UUIDString)) ? Yes : No);
            
            if (ByteOrderDiffers == Yes) {
                GUUID_Swap(InputType, GUUID2Convert);
            }
            
            if (TypeDiffers == Yes) {
                // Convert from a string to a binary, or vice versa.
                if ((InputType == UUIDString || InputType == GUIDString) && (OutputType == BinaryUUID || OutputType == BinaryGUID)) {
                    // Convert from string to binary
                    for (uint8_t StringByte = 0; StringByte < BinaryGUUIDSize; StringByte++) {
                        for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUIDSize; BinaryByte++) {
                            if (GUUID2Convert[StringByte] != 0x2D) {
                                ConvertedGUUID[BinaryByte] = GUUID2Convert[StringByte];
                            }
                        }
                    }
                } else if ((InputType == BinaryUUID || InputType == BinaryGUID) || (OutputType == UUIDString || OutputType == GUIDString)) {
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
        } else if (InputType == UnknownGUUID) {
            Log(Log_ERROR, __func__, U8("InputType is invalid"));
        } else if (OutputType == UnknownGUUID) {
            Log(Log_ERROR, __func__, U8("OutputType is invalid"));
        }
        return ConvertedGUUID;
    }
    
    uint8_t *GUUID_Swap(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap) {
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
    
    void GUUID_Deinit(uint8_t *GUUID) {
        if (GUUID != NULL) {
            free(GUUID);
        }
    }
    /* GUUID */
    
#ifdef __cplusplus
}
#endif
