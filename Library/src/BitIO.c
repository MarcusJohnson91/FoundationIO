#include <stdbool.h>                  /* Included for bool true/false, Yes/No are in BitIOMacros */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for the FILE type, SEEK SET/END/CUR macros */
#include <stdlib.h>                   /* Included for calloc, realloc, and free */

#include "../include/BitIOMacros.h"
#include "../include/BitIOMath.h"
#include "../include/StringIO.h"
#include "../include/BitIOLog.h"
#include "../include/BitIO.h"

#if   (BitIOTargetOS == BitIOWindowsOS)
#include <io.h>                       /* Actual Socket functions like _read, _write */
#include <winsock.h>                  /* Included for the socket support on Windows */
#elif (BitIOTargetOS == BitIOPOSIXOS)
#include <sys/socket.h>               /* Included for connect, socket, sockaddr */
#include <unistd.h>                   /* Included for read and shit */
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    static inline uint8_t SwapBits(const uint8_t Byte) {
        return ((Byte & 0x80 >> 7)|(Byte & 0x40 >> 5)|(Byte & 0x20 >> 3)|(Byte & 0x10 >> 1)|(Byte & 0x8 << 1)|(Byte & 0x4 << 3)|(Byte & 0x2 << 5)|(Byte & 0x1 << 7));
    }
    
    static inline uint8_t CreateBitMaskLSBit(const uint8_t Bits2Extract) {
        return (uint8_t) Power(2, Bits2Extract) << (8 - Bits2Extract);
    }
    
    static inline uint8_t CreateBitMaskMSBit(const uint8_t Bits2Extract) {
        return (uint8_t) Power(2, Bits2Extract) >> (8 - Bits2Extract);
    }
    
    inline uint16_t SwapEndian16(const uint16_t Data2Swap) {
        return ((Data2Swap & 0xFF00) >> 8) | ((Data2Swap & 0x00FF) << 8);
    }
    
    inline uint32_t SwapEndian32(const uint32_t Data2Swap) {
        return ((Data2Swap & 0xFF000000) >> 24) | ((Data2Swap & 0x00FF0000) >> 8) | ((Data2Swap & 0x0000FF00) << 8) | ((Data2Swap & 0x000000FF) << 24);
    }
    
    inline uint64_t SwapEndian64(const uint64_t Data2Swap) {
        return (((Data2Swap & 0xFF00000000000000) >> 56) | ((Data2Swap & 0x00FF000000000000) >> 40) | \
                ((Data2Swap & 0x0000FF0000000000) >> 24) | ((Data2Swap & 0x000000FF00000000) >>  8) | \
                ((Data2Swap & 0x00000000FF000000) <<  8) | ((Data2Swap & 0x0000000000FF0000) << 24) | \
                ((Data2Swap & 0x000000000000FF00) << 40) | ((Data2Swap & 0x00000000000000FF) << 56));
    }
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        uint8_t              *Buffer;
        uint64_t              NumBits;
        uint64_t              BitOffset;
    } BitBuffer;
    
    BitBuffer *BitBuffer_Init(const uint64_t BitBufferSize) {
        if (BitIOGlobalByteOrder == NULL || BitIOGlobalBitOrder == NULL) {
            BitIOGetRuntimeByteBitOrder();
        }
        BitBuffer *BitB                  = calloc(1, sizeof(BitBuffer));
        if (BitB != NULL && BitBufferSize > 0) {
            BitB->Buffer                 = calloc(BitBufferSize, sizeof(uint8_t));
            if (BitB->Buffer != NULL) {
                BitB->NumBits            = Bytes2Bits(BitBufferSize);
            } else {
                BitIOLog(BitIOLog_ERROR, __func__, u8"Not enough memory to allocate %d bits for BitBuffer's buffer", BitBufferSize);
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"Not enough memory to allocate this instance of BitBuffer");
        }
        return BitB;
    }
    
    uint64_t BitBuffer_GetSize(BitBuffer *BitB) {
        uint64_t BitBufferSize = 0ULL;
        if (BitB != NULL) {
            BitBufferSize      = BitB->NumBits;
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
        return BitBufferSize;
    }
    
    uint64_t BitBuffer_GetPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB != NULL) {
            Position = BitB->BitOffset;
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
        return Position;
    }
    
    bool BitBuffer_IsAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        bool AlignmentStatus = No;
        if (BitB != NULL && (BytesOfAlignment % 2 == 0 || BytesOfAlignment == 1)) {
            if (BitB->BitOffset % (BytesOfAlignment * 8) == 0) {
                AlignmentStatus = Yes;
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        } else if (BytesOfAlignment % 2 == 0 || BytesOfAlignment == 1) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BytesOfAlignment: %d isn't an integer power of 2", BytesOfAlignment);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        if (BitB != NULL && (BytesOfAlignment % 2 == 0 || BytesOfAlignment == 1)) {
            uint8_t Bits2Align = (BytesOfAlignment * 8) - (BitB->BitOffset % (BytesOfAlignment * 8));
            if ((BitB->BitOffset + Bits2Align > BitB->NumBits) || (BitB->BitOffset + Bits2Align < BitB->NumBits)) {
                BitB->Buffer   = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, Yes));
                BitB->NumBits += Bits2Align;
            }
            BitB->BitOffset   += Bits2Align;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        } else if (BytesOfAlignment % 2 == 0 || BytesOfAlignment == 1) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BytesOfAlignment: %d isn't a power of 2 (or 1)", BytesOfAlignment);
        }
    }
    
    void BitBuffer_Skip(BitBuffer *BitB, const int64_t Bits2Skip) {
        if (BitB != NULL) {
            if ((BitB->BitOffset + Bits2Skip > BitB->NumBits) || (BitB->BitOffset + Bits2Skip < BitB->NumBits)) {
                BitB->Buffer    = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Skip, Yes));
                BitB->NumBits  += Bits2Skip;
            }
            BitB->BitOffset    += Bits2Skip;
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
    }
    
    void BitBuffer_Deinit(BitBuffer *BitB) {
        if (BitB != NULL) {
            free(BitB->Buffer);
            free(BitB);
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
    }
    
    /* BitBuffer Resource Management */
    static inline void InsertBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
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
                    uint64_t ByteOffset               = (ByteOrder == BitIOGlobalByteOrder ? Bits2Bytes(BitB->BitOffset + 1, No):Bits2Bytes(BitB->BitOffset + 1 + NumBits2Insert, No));
                    uint8_t  Bits2InsertForThisByte   = 8 - (BitB->BitOffset % 8);
                    uint8_t  BitMask                  = 0;
                    uint8_t  Byte                     = 0;
                    if (BitOrder == BitIOLSBitFirst) {
                        BitMask                       = CreateBitMaskLSBit(Bits2InsertForThisByte);
                    } else if (BitOrder == BitIOMSBitFirst) {
                        BitMask                       = CreateBitMaskMSBit(Bits2InsertForThisByte);
                    }
                    Byte                              = BitB->Buffer[ByteOffset] & BitMask;
                    if (BitOrder == BitIOLSBitFirst) {
                        // Right shift the Byte
                        Byte                          = Byte >> (8 - Bits2InsertForThisByte);
                    } else if (BitOrder == BitIOMSBitFirst) {
                        // Left shift the byte by 8 - he number of bits we extracted from this byte
                        Byte                          = Byte << (8 - Bits2InsertForThisByte);
                    }
                }
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, "BitB's Pointer is NULL");
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Insert) {
            BitIOLog(BitIOLog_ERROR, __func__, "Not enough room in BitB to insert %d bits", NumBits2Insert);
        }
    }
    
    static inline uint64_t ExtractBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Extract) {
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
                    uint64_t ByteOffset               = (ByteOrder == BitIOGlobalByteOrder ? Bits2Bytes(BitB->BitOffset + 1, No):Bits2Bytes(BitB->BitOffset + 1 + NumBits2Extract, No));
                    uint8_t  Bits2ExtractFromThisByte = 8 - (BitB->BitOffset % 8); //uint64_t Bits2Get = NumBits2ExtractFromByte(BitB->BitOffset, Bits);
                    uint8_t  BitMask                  = 0;
                    uint8_t  Byte                     = 0;
                    if (BitOrder == BitIOLSBitFirst) {
                        BitMask                       = CreateBitMaskLSBit(Bits2ExtractFromThisByte);
                    } else if (BitOrder == BitIOMSBitFirst) {
                        BitMask                       = CreateBitMaskMSBit(Bits2ExtractFromThisByte);
                    }
                    Byte                              = BitB->Buffer[ByteOffset] & BitMask;
                    if (BitOrder == BitIOLSBitFirst) {
                        // Right shift the Byte
                        Byte                          = Byte >> (8 - Bits2ExtractFromThisByte);
                    } else if (BitOrder == BitIOMSBitFirst) {
                        // Left shift the byte by 8 - he number of bits we extracted from this byte
                        Byte                          = Byte << (8 - Bits2ExtractFromThisByte);
                    }
                    ExtractedBits                     = Byte; // if the byte order matches the systems append it at the low side?
                }
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, "BitB's Pointer is NULL");
        } else if (BitB->NumBits >= BitB->BitOffset + NumBits2Extract) {
            BitIOLog(BitIOLog_ERROR, __func__, "Not enough bits in BitB to extract %d bits", NumBits2Extract);
        }
        return ExtractedBits;
    }
    
    uint64_t PeekBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData      = ExtractBits(ByteOrder, BitOrder, BitB, Bits2Peek);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBits(ByteOrder, BitOrder, BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadUnary(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL) {
            do {
                OutputData    += 1;
            } while (ExtractBits(ByteOrder, BitOrder, BitB, 1) != StopBit);
            BitB->BitOffset   += OutputData;
            BitBuffer_Skip(BitB, 1); // Skip the stop bit
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
        return (UnaryType == CountUnary ? OutputData + 1 : OutputData);
    }
    
    uint64_t ReadExpGolomb(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL && (StopBit == 0 || StopBit == 1)) {
            uint8_t Bits2Read  = IntegerLog2(ReadUnary(ByteOrder, BitOrder, BitB, UnaryType, StopBit));
            OutputData         = ReadBits(ByteOrder, BitOrder, BitB, Bits2Read);
            BitB->BitOffset   += OutputData;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
        return OutputData;
    }
    
    void     WriteBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            InsertBits(ByteOrder, BitOrder, BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        }
    }
    
    void     WriteUnary(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const uint8_t UnaryBits2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            uint8_t Field2Write = UnaryBits2Write;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            InsertBits(ByteOrder, BitOrder, BitB, IntegerLog2(Field2Write), ~StopBit); // Writing the unary pary
            InsertBits(ByteOrder, BitOrder, BitB, 1, StopBit); // Writing the stop bit
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteExpGolomb(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write) {
        if (BitB != NULL) {
            uint8_t NumBits2Write = IntegerLog2(Field2Write);
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
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
    }
    /* BitBuffer Resource Management */
    
    /* BitBuffer GUUID Management */
    bool CompareGUUIDs(GUUIDTypes GUUIDType, const uint8_t *GUUID1, const uint8_t *GUUID2) {
        uint8_t GUUIDSize = ((GUUIDType == GUIDString || GUUIDType == UUIDString) ? BitIOGUUIDStringSize - BitIONULLStringSize : BitIOBinaryGUUIDSize);
        bool GUUIDsMatch = Yes;
        if (GUUID1 != NULL && GUUID2 != NULL && GUUIDType != UnknownGUUID) {
            for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < GUUIDSize; BinaryGUUIDByte++) {
                if (GUUID1[BinaryGUUIDByte] != GUUID2[BinaryGUUIDByte]) {
                    GUUIDsMatch = No;
                }
            }
        } else if (GUUID1 == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"GUUID1 Pointer is NULL");
        } else if (GUUID2 == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"GUUID2 Pointer is NULL");
        } else if (GUUIDType == UnknownGUUID) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"UnknownGUUID is an invalid GUUID type");
        }
        return GUUIDsMatch;
    }
    
    uint8_t *ConvertGUUID(GUUIDTypes InputGUUIDType, GUUIDTypes OutputGUUIDType, const uint8_t *GUUID2Convert) {
        uint8_t  OutputGUUIDSize  = ((OutputGUUIDType == GUIDString || OutputGUUIDType == UUIDString) ? BitIOGUUIDStringSize : BitIOBinaryGUUIDSize);
        
        bool     ByteOrderDiffers = (((InputGUUIDType == GUIDString && OutputGUUIDType == UUIDString) || (InputGUUIDType == UUIDString && OutputGUUIDType == GUIDString) || (InputGUUIDType == BinaryUUID && OutputGUUIDType == BinaryGUID) || (InputGUUIDType == BinaryGUID && OutputGUUIDType == BinaryUUID)) ? Yes : No);
        
        bool     TypeDiffers      = (((InputGUUIDType == GUIDString && OutputGUUIDType == BinaryGUID) || (InputGUUIDType == BinaryGUID && OutputGUUIDType == GUIDString) || (InputGUUIDType == UUIDString && OutputGUUIDType == BinaryUUID) || (InputGUUIDType == BinaryUUID && OutputGUUIDType == UUIDString)) ? Yes : No);
        
        uint8_t *ConvertedGUUID   = calloc(OutputGUUIDSize, sizeof(uint8_t));
        if (ConvertedGUUID != NULL && GUUID2Convert != NULL && InputGUUIDType != UnknownGUUID && OutputGUUIDType != UnknownGUUID) {
            if (ByteOrderDiffers == Yes) {
                SwapGUUID(InputGUUIDType, *GUUID2Convert);
            }
            
            if (TypeDiffers == Yes) {
                // Convert from a string to a binary, or vice versa.
                if ((InputGUUIDType == UUIDString || InputGUUIDType == GUIDString) && (OutputGUUIDType == BinaryUUID || OutputGUUIDType == BinaryGUID)) {
                    // Convert from string to binary
                    for (uint8_t StringByte = 0; StringByte < BitIOGUUIDStringSize; StringByte++) {
                        for (uint8_t BinaryByte = 0; BinaryByte < BitIOBinaryGUUIDSize; BinaryByte++) {
                            if (GUUID2Convert[StringByte] != 0x2D) {
                                ConvertedGUUID[BinaryByte]  = GUUID2Convert[StringByte];
                            }
                        }
                    }
                } else if ((InputGUUIDType == BinaryUUID || InputGUUIDType == BinaryGUID) || (OutputGUUIDType == UUIDString || OutputGUUIDType == GUIDString)) {
                    // Convert from binary to string
                    for (uint8_t BinaryByte = 0; BinaryByte < BitIOBinaryGUUIDSize; BinaryByte++) {
                        for (uint8_t StringByte = 0; StringByte < BitIOGUUIDStringSize; StringByte++) {
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
            BitIOLog(BitIOLog_ERROR, __func__, u8"Insufficent memory to allocate ConvertedGUUID");
        } else if (GUUID2Convert == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"GUUID2Convert Pointer is NULL");
        } else if (InputGUUIDType == UnknownGUUID) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"InputGUUIDType is invalid");
        } else if (OutputGUUIDType) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"OutputGUUIDType is invalid");
        }
        return ConvertedGUUID;
    }
    
    uint8_t *SwapGUUID(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap) {
        uint8_t *SwappedGUUID = NULL;
        if (GUUID2Swap != NULL && GUUIDType != UnknownGUUID) {
            if (GUUIDType == UUIDString || GUUIDType == GUIDString) {
                SwappedGUUID = calloc(BitIOGUUIDStringSize, sizeof(uint8_t));
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
                    for (uint8_t EndBytes = 13; EndBytes < BitIOGUUIDStringSize - BitIONULLStringSize; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    BitIOLog(BitIOLog_ERROR, __func__, "SwappedGUUID's Pointer is NULL");
                }
            } else if (GUUIDType == BinaryUUID || GUUIDType == BinaryGUID) {
                SwappedGUUID = calloc(BitIOBinaryGUUIDSize, sizeof(uint8_t));
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
                    for (uint8_t EndBytes = 10; EndBytes < BitIOBinaryGUUIDSize; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    BitIOLog(BitIOLog_ERROR, __func__, "SwappedGUUID's Pointer is NULL");
                }
            }
        } else if (GUUID2Swap == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, "GUUID2Swap's Pointer is NULL");
        } else if (GUUIDType == UnknownGUUID) {
            BitIOLog(BitIOLog_ERROR, __func__, "UnknownGUUID is an invalid GUUID type");
        }
        return SwappedGUUID;
    }
    
    uint8_t *ReadGUUID(GUUIDTypes GUUIDType, BitBuffer *BitB) {
        uint8_t ByteOrder = ((GUUIDType == GUIDString || GUUIDType == BinaryGUID) ? BitIOLSByteFirst : BitIOMSByteFirst);
        uint8_t *GUUID = NULL;
        if (GUUIDType != UnknownGUUID && BitB != NULL && (BitB->NumBits - BitB->BitOffset) >= BitIOGUUIDStringSize) {
            if (GUUIDType == BinaryUUID || GUUIDType == BinaryGUID) {
                // Read it from the BitBuffer as a string.
                GUUID = calloc(BitIOBinaryGUUIDSize, sizeof(uint8_t));
                if (GUUID != NULL) {
                    for (uint8_t Byte = 0; Byte < BitIOGUUIDStringSize - BitIONULLStringSize; Byte++) {
                        GUUID[Byte] = ExtractBits(ByteOrder, BitIOLSBitFirst, BitB, 8);
                    }
                } else {
                    BitIOLog(BitIOLog_ERROR, __func__, u8"Not enough memory to allocate GUIDString");
                }
            } else if (GUUIDType == UUIDString || GUUIDType == GUIDString) {
                GUUID = calloc(BitIOGUUIDStringSize, sizeof(uint8_t));
                if (GUUID != NULL) {
                    uint32_t Section1    = ExtractBits(ByteOrder, BitIOLSBitFirst, BitB, 32);
                    BitBuffer_Skip(BitB, 8);
                    uint16_t Section2    = ExtractBits(ByteOrder, BitIOLSBitFirst, BitB, 16);
                    BitBuffer_Skip(BitB, 8);
                    uint16_t Section3    = ExtractBits(ByteOrder, BitIOLSBitFirst, BitB, 16);
                    BitBuffer_Skip(BitB, 8);
                    uint16_t Section4    = ExtractBits(ByteOrder, BitIOLSBitFirst, BitB, 16);
                    BitBuffer_Skip(BitB, 8);
                    uint64_t Section5    = ExtractBits(ByteOrder, BitIOLSBitFirst, BitB, 48);
                    sprintf((char*)UUIDString, "%d-%d-%d-%d-%llu", Section1, Section2, Section3, Section4, Section5);
                } else {
                    BitIOLog(BitIOLog_ERROR, __func__, u8"Not enough memory to allocate UUIDString");
                }
            }
        }
        return GUUID;
    }
    
    void WriteGUUID(GUUIDTypes GUUIDType, BitBuffer *BitB, const uint8_t *GUUID2Write) {
        if (BitB != NULL && BitBuffer_GetPosition(BitB)  && GUUID2Write != NULL) { // TODO: Make sure that the BitBuffer can hold the GUUID
            uint8_t GUUIDSize = ((GUUIDType == GUIDString || GUUIDType == UUIDString) ? BitIOGUUIDStringSize - BitIONULLStringSize : BitIOBinaryGUUIDSize);
            uint8_t ByteOrder = ((GUUIDType == GUIDString || GUUIDType == BinaryGUID) ? BitIOLSByteFirst : BitIOMSByteFirst);
            for (uint8_t Byte = 0; Byte < GUUIDSize; Byte++) {
                InsertBits(ByteOrder, BitIOLSBitFirst, BitB, 8, GUUID2Write[Byte]);
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer's Pointer is NULL");
        } else if (GUUID2Write == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"GUUID2Write's Pointer is NULL");
        }
    }
    
    void GUUID_Deinit(uint8_t *GUUID) {
        if (GUUID != NULL) {
            free(GUUID);
        }
    }
    /* BitBuffer GUUID Management */
    /* End BitBuffer section */
    
    /* BitInput */
    typedef struct BitInput {
        FILE                   *File;
        uint64_t                FileSpecifierNum;
        fpos_t                  FileSize;
        fpos_t                  FilePosition;
        int                     Socket;
        BitInputOutputFileTypes FileType;
        bool                    FileSpecifierExists;
    } BitInput;
    
    BitInput *BitInput_Init(void) {
        BitInput *BitI = calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"Not enough memory to allocate this instance of BitInput");
        }
        return BitI;
    }
    
    void BitInput_OpenFile(BitInput *BitI, const UTF8 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType          = BitIOFile;
            uint64_t Path2OpenSize  = UTF8_GetSizeInCodePoints(*Path2Open) + BitIONULLStringSize;
            if (BitI->FileSpecifierExists == Yes) {
                UTF8 *NewPath       = calloc(Path2OpenSize, sizeof(UTF8));
                snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitI->FileSpecifierNum); // FIXME: HANDLE FORMAT STRINGS BETTER
                free(NewPath);
            }
#if   (BitIOTargetOS == BitIOPOSIXOS)
            BitI->File = fopen(Path2Open, "rb");
#elif (BitIOTargetOS == BitIOWindowsOS)
            UTF32String WidePath    = UTF8_Decode(Path2Open, Path2OpenSize);
            BitI->File              = _wfopen(WidePath, "rb");
            free(WidePath);
#endif
            if (BitI->File == NULL) {
                BitIOLog(BitIOLog_ERROR, __func__, u8"Couldn't open file: Check that the file exists and the permissions are correct");
            } else {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            }
            
        } else if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        } else if (Path2Open == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"Path2Open Pointer is NULL");
        }
    }
    
    void BitInput_OpenSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol) {
        if (BitI != NULL) {
            BitI->FileType   = BitIOSocket;
            BitI->Socket     = socket(Domain, Type, Protocol);
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        }
    }
    
    void BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitI != NULL && SocketAddress != NULL) {
            BitI->FileType = BitIOSocket;
            connect(BitI->Socket, SocketAddress, SocketSize);
        } else if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        } else if (SocketAddress == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"SocketAddress Pointer is NULL");
        }
    }
    
    void BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const int64_t Bytes2Read) {
        uint64_t BytesRead            = 0ULL;
        if (BitI != NULL && Buffer2Read != NULL && Bytes2Read > (BitI->FileSize - BitI->FilePosition)) {
            if (Buffer2Read->Buffer  != NULL) {
                free(Buffer2Read->Buffer);
            }
            Buffer2Read->Buffer       = calloc(Bytes2Read, sizeof(uint8_t));
            if (Buffer2Read->Buffer != NULL) {
                if (BitI->FileType   == BitIOFile) {
                    BytesRead         = fread(Buffer2Read->Buffer, 1, Bytes2Read, BitI->File);
                } else if (BitI->FileType == BitIOSocket) {
#if   (BitIOTargetOS == BitIOPOSIXOS)
                    BytesRead         = read(BitI->Socket, Buffer2Read->Buffer, Bytes2Read);
#elif (BitIOTargetOS == BitIOWindowsOS)
                    BytesRead         = _read(BitI->Socket, Buffer2Read->Buffer, Bytes2Read);
#endif
                }
                if (BytesRead == Bytes2Read) {
                    Buffer2Read->NumBits = Bytes2Bits(BytesRead);
                } else {
                    BitIOLog(BitIOLog_ERROR, __func__, u8"Fread read: %d bytes, but you requested: %d", BytesRead, Bytes2Read);
                }
            } else {
                BitIOLog(BitIOLog_ERROR, __func__, u8"Not enough memory to allocate Buffer in BitBuffer");
            }
        } else if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        } else if (Buffer2Read == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        } else if (Bytes2Read > (BitI->FileSize - BitI->FilePosition)) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"You tried reading more data: % than is available: %d in the file", Bytes2Read, BitI->FileSize - BitI->FilePosition);
        }
    }
    
    fpos_t BitInput_BytesRemainingInFile(BitInput *BitI) {
        fpos_t BytesLeft = 0LL;
        if (BitI != NULL) {
            BytesLeft = BitI->FileSize - BitI->FilePosition;
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        }
        return BytesLeft;
    }
    
    static void BitInput_FindFileSize(BitInput *BitI) {
        if (BitI != NULL) {
            fseek(BitI->File, 0, SEEK_END);
            BitI->FileSize     = ftell(BitI->File);
            fseek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition = ftell(BitI->File);
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
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
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
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
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        }
        return Position;
    }
    
    void BitInput_Deinit(BitInput *BitI) {
        if (BitI != NULL) {
            if (BitI->FileType == BitIOFile) {
                fclose(BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
#if   (BitIOTargetOS == BitIOPOSIXOS)
                close(BitI->Socket);
#elif (BitIOTargetOS == BitIOWindowsOS)
                _close(BitI->Socket);
#endif          
            }
            free(BitI);
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        }
    }
    /* BitInput */
    
    
    /* BitOutput */
    typedef struct BitOutput {
        FILE                   *File;
        uint64_t                FileSpecifierNum;
        fpos_t                  FilePosition;
        int                     Socket;
        BitInputOutputFileTypes FileType;
        bool                    FileSpecifierExists;
    } BitOutput;
    
    BitOutput *BitOutput_Init(void) {
        BitOutput *BitO = calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"Not enough memory to allocate this instance of BitOutput");
        }
        return BitO;
    }
    
    void BitOutput_OpenFile(BitOutput *BitO, UTF8 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType          = BitIOFile;
            uint64_t Path2OpenSize  = UTF8_GetSizeInCodePoints(Path2Open) + BitIONULLStringSize;
            if (BitO->FileSpecifierExists == Yes) {
                UTF8 *NewPath       = calloc(Path2OpenSize, sizeof(UTF8));
                snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitO->FileSpecifierNum); // FIXME: HANDLE FORMAT STRINGS BETTER
                free(NewPath);
            }
#if   (BitIOTargetOS == BitIOPOSIXOS)
            BitO->File = fopen(Path2Open, "wb");
#elif (BitIOTargetOS == BitIOWindowsOS)
            UTF32String WidePath    = UTF8_Decode(Path2Open, Path2OpenSize);
            BitO->File              = _wfopen(WidePath, "rb");
            free(WidePath);
#endif
            if (BitO->File != NULL) {
                setvbuf(BitO->File, NULL, _IONBF, 0);
            } else {
                BitIOLog(BitIOLog_ERROR, __func__, u8"Couldn't open output file; Check that the path exists and the permissions are correct");
            }
        } else if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitOutput Pointer is NULL");
        } else if (Path2Open == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"Path2Open Pointer is NULL");
        }
    }
    
    void BitOutput_OpenSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol) {
        if (BitO != NULL) {
            BitO->FileType  = BitIOSocket;
            BitO->Socket    = socket(Domain, Type, Protocol);
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        }
    }
    
    void BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitO != NULL && SocketAddress != NULL) {
            BitO->FileType = BitIOSocket;
            connect(BitO->Socket, SocketAddress, SocketSize);
        } else if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitOutput Pointer is NULL");
        } else if (SocketAddress == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"SocketAddress Pointer is NULL");
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
#if   (BitIOTargetOS == BitIOPOSIXOS)
                BytesWritten           = write(BitO->Socket, Buffer2Write->Buffer, NumBytes2Write);
#elif (BitIOTargetOS == BitIOWindowsOS)
                BytesWritten           = _write(BitO->Socket, Buffer2Write->Buffer, NumBytes2Write);
#endif
            }
            if (BytesWritten != NumBytes2Write) {
                BitIOLog(BitIOLog_ERROR, __func__, u8"Fwrite wrote: %d bytes, but you requested: %d", BytesWritten, NumBytes2Write);
            } else {
                Buffer2Write->NumBits -= Bytes2Bits(BytesWritten);
            }
        } else if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitInput Pointer is NULL");
        } else if (Buffer2Write == NULL) {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitBuffer Pointer is NULL");
        }
    }
    
    void BitOutput_Deinit(BitOutput *BitO) {
        if (BitO != NULL) {
            fflush(BitO->File);
            if (BitO->FileType == BitIOFile) {
                fclose(BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
#if   (BitIOTargetOS == BitIOPOSIXOS)
                close(BitO->Socket);
#elif (BitIOTargetOS == BitIOWindowsOS)
                _close(BitO->Socket);
#endif  
            }
            free(BitO);
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"BitOutput Pointer is NULL");
        }
    }
    /* BitOutput */
    
#ifdef __cplusplus
}
#endif
