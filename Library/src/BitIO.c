#include <assert.h>     /* Included for static_assert */
#include <stdlib.h>     /* Included for calloc, realloc, and free */
#include <string.h>     /* Included for strlen */
#include <sys/socket.h> /* Included for connect, socket, sockaddr (ptr only) */

#include "../include/BitIO.h"
#include "../include/BitIOMath.h"
#include "../include/BitIOLog.h"

#if   (BitIOTargetOS == BitIOPOSIXOS)
#include           <sys/socket.h> /* Included for the socket support */
#include           <unistd.h>     /* Included for read and shit */
#elif (BitIOTargetOS == BitIOWindowsOS)
#include           <winsock.h>    /* Included for the socket support on Windows */
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    static inline uint8_t NumBits2ExtractFromByte(const uint64_t BitOffset, const uint8_t Bits2Extract) {
        uint8_t Bits2ExtractFromThisByte = 0;
        uint8_t BitsInThisByte           = BitOffset % 8;
        if (Bits2Extract >= BitsInThisByte) {
            Bits2ExtractFromThisByte     = BitsInThisByte;
        } else {
            Bits2ExtractFromThisByte     = Bits2Extract;
        }
        return Bits2ExtractFromThisByte;
    }
    
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
    
    /* Pure GUUID Management */
    bool CompareGUUIDStrings(const uint8_t GUUIDString1[BitIOGUUIDStringSize], const uint8_t GUUIDString2[BitIOGUUIDStringSize]) {
        bool GUUIDStringsMatch = Yes;
        if (GUUIDString1 != NULL && GUUIDString2 != NULL) {
            for (uint8_t GUUIDStringByte = 0; GUUIDStringByte < BitIOGUUIDStringSize - BitIONULLStringSize; GUUIDStringByte++) {
                if (GUUIDString1[GUUIDStringByte] != GUUIDString2[GUUIDStringByte]) {
                    GUUIDStringsMatch = No;
                }
            }
        } else if (GUUIDString1 == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "GUUIDString1 Pointer is NULL");
        } else if (GUUIDString2 == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "GUUIDString2 Pointer is NULL");
        }
        
        return GUUIDStringsMatch;
    }
    
    bool CompareBinaryGUUIDs(const uint8_t BinaryGUUID1[BitIOBinaryGUUIDSize], const uint8_t BinaryGUUID2[BitIOBinaryGUUIDSize]) {
        bool BinaryGUUIDsMatch = Yes;
        if (BinaryGUUID1 != NULL && BinaryGUUID2 != NULL) {
            for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < BitIOBinaryGUUIDSize; BinaryGUUIDByte++) {
                if (BinaryGUUID1[BinaryGUUIDByte] != BinaryGUUID2[BinaryGUUIDByte]) {
                    BinaryGUUIDsMatch = No;
                }
            }
        } else if (BinaryGUUID1 == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BinaryGUUID1 Pointer is NULL");
        } else if (BinaryGUUID2 == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BinaryGUUID2 Pointer is NULL");
        }
        
        return BinaryGUUIDsMatch;
    }
    
    uint8_t *ConvertGUUIDString2BinaryGUUID(const uint8_t GUUIDString[BitIOGUUIDStringSize]) {
        uint8_t *BinaryGUUID = calloc(BitIOGUUIDStringSize, sizeof(uint8_t));
        if (GUUIDString != NULL && BinaryGUUID != NULL) {
            for (uint8_t StringByte = 0; StringByte < BitIOGUUIDStringSize; StringByte++) {
                for (uint8_t BinaryByte = 0; BinaryByte < BitIOBinaryGUUIDSize; BinaryByte++) {
                    if (GUUIDString[StringByte] != 0x2D) {
                        BinaryGUUID[BinaryByte]  = GUUIDString[StringByte];
                    }
                }
            }
        } else if (GUUIDString == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "GUUIDString Pointer is NULL");
        } else if (BinaryGUUID == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate %d bytes", BitIOGUUIDStringSize);
        }
        return BinaryGUUID;
    }
    
    uint8_t *ConvertBinaryGUUID2GUUIDString(const uint8_t BinaryGUUID[BitIOBinaryGUUIDSize]) {
        uint8_t *GUUIDString                     = calloc(BitIOGUUIDStringSize, sizeof(uint8_t));
        if (BinaryGUUID != NULL && GUUIDString != NULL) {
            uint8_t ASCIIHyphen = 0x2D;
            for (uint8_t BinaryByte = 0; BinaryByte < BitIOBinaryGUUIDSize; BinaryByte++) {
                for (uint8_t StringByte = 0; StringByte < BitIOGUUIDStringSize; StringByte++) {
                    if (BinaryByte != 4 && BinaryByte != 7 && BinaryByte != 10 && BinaryByte != 13) {
                        GUUIDString[StringByte]  = BinaryGUUID[BinaryByte];
                    } else {
                        GUUIDString[StringByte]  = ASCIIHyphen;
                    }
                }
            }
        } else if (BinaryGUUID == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BinaryGUUID Pointer is NULL");
        } else if (GUUIDString == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate %d bytes", BitIOGUUIDStringSize);
        }
        return GUUIDString;
    }
    
    uint8_t *SwapGUUIDString(const uint8_t GUUIDString[BitIOGUUIDStringSize]) {
        uint8_t *SwappedGUUIDString = calloc(BitIOGUUIDStringSize, sizeof(uint8_t));
        
        if (GUUIDString != NULL && SwappedGUUIDString != NULL) {
            SwappedGUUIDString[0]   = GUUIDString[3];
            SwappedGUUIDString[1]   = GUUIDString[2];
            SwappedGUUIDString[2]   = GUUIDString[1];
            SwappedGUUIDString[3]   = GUUIDString[0];
            
            SwappedGUUIDString[4]   = GUUIDString[4];
            
            SwappedGUUIDString[5]   = GUUIDString[6];
            SwappedGUUIDString[6]   = GUUIDString[5];
            
            SwappedGUUIDString[7]   = GUUIDString[7];
            
            SwappedGUUIDString[8]   = GUUIDString[9];
            SwappedGUUIDString[9]   = GUUIDString[8];
            
            SwappedGUUIDString[10]  = GUUIDString[10];
            
            SwappedGUUIDString[11]  = GUUIDString[12];
            SwappedGUUIDString[12]  = GUUIDString[11];
            
            for (uint8_t EndBytes = 13; EndBytes < BitIOGUUIDStringSize - BitIONULLStringSize; EndBytes++) {
                SwappedGUUIDString[EndBytes] = GUUIDString[EndBytes];
            }
        } else if (GUUIDString == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "GUUIDString Pointer is NULL");
        } else if (SwappedGUUIDString == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate %d bytes", BitIOGUUIDStringSize);
        }
        return SwappedGUUIDString;
    }
    
    uint8_t *SwapBinaryGUUID(const uint8_t BinaryGUUID[BitIOBinaryGUUIDSize]) {
        uint8_t *SwappedBinaryGUUID = calloc(BitIOBinaryGUUIDSize, sizeof(uint8_t));
        if (BinaryGUUID != NULL && SwappedBinaryGUUID != NULL) {
            SwappedBinaryGUUID[0]   = BinaryGUUID[3];
            SwappedBinaryGUUID[1]   = BinaryGUUID[2];
            SwappedBinaryGUUID[2]   = BinaryGUUID[1];
            SwappedBinaryGUUID[3]   = BinaryGUUID[0];
            
            SwappedBinaryGUUID[4]   = BinaryGUUID[5];
            SwappedBinaryGUUID[5]   = BinaryGUUID[4];
            
            SwappedBinaryGUUID[6]   = BinaryGUUID[7];
            SwappedBinaryGUUID[7]   = BinaryGUUID[6];
            
            SwappedBinaryGUUID[8]   = BinaryGUUID[9];
            SwappedBinaryGUUID[9]   = BinaryGUUID[8];
            
            for (uint8_t EndBytes = 10; EndBytes < BitIOBinaryGUUIDSize; EndBytes++) {
                SwappedBinaryGUUID[EndBytes] = BinaryGUUID[EndBytes];
            }
        } else if (BinaryGUUID == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BinaryGUUID Pointer is NULL");
        } else if (SwappedBinaryGUUID == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate %d bytes", BitIOBinaryGUUIDSize);
        }
        return SwappedBinaryGUUID;
    }
    
    void GUUID_Deinit(uint8_t *GUUID) {
        if (GUUID != NULL) {
            free(GUUID);
        }
    }
    /* Pure GUUID Management */
    
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        uint64_t              NumBits;
        uint64_t              BitOffset;
        uint8_t              *Buffer;
    } BitBuffer;
    
    BitBuffer *BitBuffer_Init(const uint64_t BitBufferSize) {
        BitBuffer *BitB                  = calloc(1, sizeof(BitBuffer));
        if (BitB != NULL) {
            BitB->Buffer                 = calloc(BitBufferSize, sizeof(uint8_t));
            if (BitB->Buffer == NULL) {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate %d bits for BitBuffer's buffer", BitBufferSize);
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate this instance of BitBuffer");
        }
        return BitB;
    }
    
    uint64_t BitBuffer_GetSize(BitBuffer *BitB) {
        uint64_t BitBufferSize = 0ULL;
        if (BitB != NULL) {
            BitBufferSize      = BitB->NumBits;
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return BitBufferSize;
    }
    
    uint64_t BitBuffer_GetPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB != NULL) {
            Position = BitB->BitOffset;
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
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
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (BytesOfAlignment % 2 == 0 || BytesOfAlignment == 1) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BytesOfAlignment: %d isn't an integer power of 2", BytesOfAlignment);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        if (BitB != NULL && (BytesOfAlignment % 2 == 0 || BytesOfAlignment == 1)) {
            uint8_t Bits2Align = (BytesOfAlignment * 8) - (BitB->BitOffset % (BytesOfAlignment * 8)); // 127 bits, 64 bits 2 align
            if ((BitB->BitOffset + Bits2Align > BitB->NumBits) || (BitB->BitOffset + Bits2Align < BitB->NumBits)) {
                BitB->Buffer   = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, Yes));
                BitB->NumBits += Bits2Align;
            }
            BitB->BitOffset   += Bits2Align;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (BytesOfAlignment % 2 == 0 || BytesOfAlignment == 1) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BytesOfAlignment: %d isn't a power of 2 (or 1)", BytesOfAlignment);
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
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void BitBuffer_Deinit(BitBuffer *BitB) {
        if (BitB != NULL) {
            free(BitB->Buffer);
            free(BitB);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    /* BitBuffer Resource Management */
    static inline void InsertBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        uint8_t Bits = NumBits2Insert;
        while (Bits > 0) {
            uint64_t Bits2Put      = NumBits2ExtractFromByte(BitB->BitOffset, Bits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)] & CreateBitMaskLSBit(Bits2Put);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
            // Extract as is
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
            uint8_t FinalByte      = SwapBits(Data);
            // Extract and flip bit order
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
            // Extract and flip byte order
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
            uint8_t FinalByte      = SwapBits(Data);
            // Extract and flip the byte order AND bit order
#endif
            Bits                  -= Bits2Put;
        }
    }
    
    static inline void InsertBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // Write data from LSByte (LSBit is default) to LSByte,LSBit
        // What variables do we need to take into account? Just swapping bit order, BitsAvailable, and looping...
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
    }
    
    static inline void InsertBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // Write data from LSByte (LSBit is default) to MSByte, LSBit.
        // What variables do we need to take into account? Just swapping byte order, BitsAvailable, and looping...
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
    }
    
    static inline void InsertBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // Write data from LSByte (LSBit is default) to MSByte, LSBit.
        // What variables do we need to take into account? Just swapping byte and bit order, BitsAvailable, and looping...
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
    }
    
    static inline uint64_t ExtractBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Extract) {
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        uint8_t  FinalByte         = 0;
        uint8_t  Times2Shift       = 0;
        uint8_t  Data              = 0;
        
        while (UserRequestedBits > 0) {
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            Data                   = BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)] & CreateBitMaskLSBit(Bits2Get);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
            // Ok, so the byte and bit order is the same, so we need to just loop over the bits normally.
            OutputData           <<= Bits2Get;
            OutputData            += Data;
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
            FinalByte              = SwapBits(Data);
            Times2Shift            = Bits2Bytes(Bits2Get, Yes);
            FinalByte            >>= Times2Shift;
            OutputData           <<= Bits2Get;
            OutputData            += Data;
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
            OutputData           <<= Bits2Get;
            OutputData            += Data;
            OutputData             = SwapEndian64(OutputData);
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
            FinalByte              = SwapBits(Data);
            OutputData           >>= Bits2Get;
            OutputData            += Data;
            OutputData             = SwapEndian64(OutputData);
#endif
            UserRequestedBits     -= Bits2Get;
        }
        return OutputData;
    }
    
    static inline uint64_t ExtractBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Extract) {
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        uint8_t  FinalByte         = 0;
        uint8_t  Times2Shift       = 0;
        uint8_t  Data              = 0;
        
        while (UserRequestedBits > 0) {
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            Data                   = BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)] & CreateBitMaskMSBit(Bits2Get);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
            OutputData            &= (0xFF << (Bits2Extract - Bits2Get)); // Byte Shift
            Data                   = SwapBits(Data);
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
            OutputData             & (0xFF << (Bits2Extract - Bits2Get)); // Byte Shift
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
            Data                   = SwapBits(Data);
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
            //Just extract
            OutputData           >>= Bits2Get;
            OutputData            += Data;
#endif
            OutputData           >>= BitB->BitOffset - Bits2Get;
            OutputData            += Data;
            UserRequestedBits     -= Bits2Get;
        }
        return 0ULL;
    }
    
    static inline uint64_t ExtractBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Extract) {
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        
        while (UserRequestedBits > 0) {
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)] & CreateBitMaskMSBit(Bits2Get);
            
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
            uint8_t FinalByte      = SwapBits(Data);
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
            uint8_t FinalByte      = SwapBits(Data);
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
            OutputData           >>= BitB->BitOffset - Bits2Get;
            OutputData            += Data;
            UserRequestedBits     -= Bits2Get;
        }
        return 0ULL;
    }
    
    static inline uint64_t ExtractBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Extract) {
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        while (UserRequestedBits > 0) {
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset, No)] & CreateBitMaskLSBit(Bits2Get);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
            /*
             Extract data from Big Endian MSBit first, to little endian least significant bit first
             So, if we need to extract 3 bits because the buffer is full, we need to extract them from the left aka mask with 0xE0
             and we need to apply them as 0x7, aka rightshift 8 - Bits2Read=3 aka 5.
             */
            // SO the bits need to be extracted from BitBuffer as LSBit? and applied as LSBit
            // It is 0x7C6E
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
            // Extract the bits as LSBit, and apply them as MSBit.
            uint8_t FinalByte      = SwapBits(Data);
            // Is is 0x3E76
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
            // Extract the bits as LSBit and apply them as LSBit
            // Swap Bytes
            // It is 0x6E7C
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
            // Extract the bits as LSBit and apply them as MSBit
            uint8_t FinalByte      = SwapBits(Data);
            // It is 0x763E
#endif
            OutputData           >>= BitB->BitOffset - Bits2Get;
            OutputData            += Data;
            UserRequestedBits     -= Bits2Get;
        }
        return OutputData;
    }
    
    uint64_t PeekBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsLSByteLSBit(BitB, Bits2Peek);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t PeekBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsLSByteMSBit(BitB, Bits2Peek);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t PeekBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsMSByteLSBit(BitB, Bits2Peek);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t PeekBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsMSByteMSBit(BitB, Bits2Peek);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsLSByteLSBit(BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsLSByteMSBit(BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsMSByteLSBit(BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            OutputData         = ExtractBitsAsMSByteMSBit(BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t ReadUnaryAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
        uint64_t Value = 0ULL;
        if (BitB != NULL) {
            if (UnaryType == CountUnary) {
                Value += 1;
            }
            while (ExtractBitsAsLSByteLSBit(BitB, 1) != StopBit) {
                Value += 1;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return Value;
    }
    
    uint64_t ReadUnaryAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
        uint64_t Value = 0ULL;
        if (BitB != NULL) {
            if (UnaryType == CountUnary) {
                Value += 1;
            }
            while (ExtractBitsAsLSByteMSBit(BitB, 1) != StopBit) {
                Value += 1;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return Value;
    }
    
    uint64_t ReadUnaryAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
        uint64_t Value = 0ULL;
        if (BitB != NULL) {
            if (UnaryType == CountUnary) {
                Value += 1;
            }
            while (ExtractBitsAsMSByteLSBit(BitB, 1) != StopBit) {
                Value += 1;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return Value;
    }
    
    uint64_t ReadUnaryAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit) {
        uint64_t Value = 0ULL;
        if (BitB != NULL) {
            if (UnaryType == CountUnary) {
                Value += 1;
            }
            while (ExtractBitsAsMSByteMSBit(BitB, 1) != StopBit) {
                Value += 1;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return Value;
    }
    
    uint64_t ReadExpGolombAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit) {
        int64_t ExtractedBits      = 0LL;
        if (BitB != NULL) {
            uint64_t Bits2Read     = ReadUnaryAsLSByteLSBit(BitB, UnaryType, StopBit);
            ExtractedBits          = ExtractBitsAsLSByteLSBit(BitB, Bits2Read);
            if (ExtractedBits < 0) {
                ExtractedBits      = (Absolute(ExtractedBits) / 2) + 1;
            } else {
                ExtractedBits      = ExtractedBits / 2;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return ExtractedBits;
    }
    
    uint64_t ReadExpGolombAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit) {
        int64_t ExtractedBits      = 0LL;
        if (BitB != NULL) {
            uint64_t Bits2Read     = ReadUnaryAsLSByteLSBit(BitB, UnaryType, StopBit);
            ExtractedBits          = ExtractBitsAsLSByteLSBit(BitB, Bits2Read);
            if (ExtractedBits <= 0) {
                ExtractedBits      = (Absolute(ExtractedBits) / 2) + 1;
            } else {
                ExtractedBits      = ExtractedBits / 2;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return ExtractedBits;
    }
    
    uint64_t ReadExpGolombAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit) {
        int64_t ExtractedBits  = 0LL;
        if (BitB != NULL) {
            uint64_t Bits2Read     = ReadUnaryAsMSByteLSBit(BitB, UnaryType, StopBit);
            ExtractedBits          = ExtractBitsAsMSByteLSBit(BitB, Bits2Read);
            if (ExtractedBits > 0) {
                ExtractedBits      = ExtractedBits / 2;
            } else {
                ExtractedBits      = (Absolute(ExtractedBits) / 2) + 1;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return ExtractedBits;
    }
    
    uint64_t ReadExpGolombAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit) {
        int64_t ExtractedBits  = 0LL;
        if (BitB != NULL) {
            uint64_t Bits2Read     = ReadUnaryAsMSByteMSBit(BitB, UnaryType, StopBit);
            ExtractedBits          = ExtractBitsAsMSByteMSBit(BitB, Bits2Read);
            if (ExtractedBits > 0) {
                ExtractedBits      = ExtractedBits / 2;
            } else {
                ExtractedBits      = (Absolute(ExtractedBits) / 2) + 1;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return ExtractedBits;
    }
    
    void     WriteBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            InsertBitsAsLSByteLSBit(BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        }
    }
    
    void     WriteBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            InsertBitsAsLSByteMSBit(BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        }
    }
    
    void     WriteBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            InsertBitsAsMSByteLSBit(BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        }
    }
    
    void     WriteBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB != NULL && NumBits2Write >= 1 && NumBits2Write <= 64) {
            InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Bits2Write);
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        }
    }
    
    void     WriteUnaryAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, uint8_t Field2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            if (StopBit == 0) {
                InsertBitsAsLSByteLSBit(BitB, Field2Write, Power(2, Field2Write) + 1);
            } else {
                InsertBitsAsLSByteLSBit(BitB, Field2Write, 0);
            }
            InsertBitsAsLSByteLSBit(BitB, 1, StopBit);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteUnaryAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, uint8_t Field2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            if (StopBit == 0) {
                InsertBitsAsLSByteMSBit(BitB, Field2Write, Power(2, Field2Write) + 1);
            } else {
                InsertBitsAsLSByteMSBit(BitB, Field2Write, 0);
            }
            InsertBitsAsLSByteMSBit(BitB, 1, StopBit);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteUnaryAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, uint8_t Field2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            if (StopBit == 0) {
                InsertBitsAsMSByteLSBit(BitB, Field2Write, Power(2, Field2Write) + 1);
            } else {
                InsertBitsAsMSByteLSBit(BitB, Field2Write, 0);
            }
            InsertBitsAsMSByteLSBit(BitB, 1, StopBit);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteUnaryAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, uint8_t Field2Write) {
        if (BitB != NULL) {
            StopBit         &= 1;
            if (UnaryType == CountUnary) {
                Field2Write -= 1;
            }
            if (StopBit == 0) {
                InsertBitsAsMSByteMSBit(BitB, Field2Write, Power(2, Field2Write) + 1);
            } else {
                InsertBitsAsMSByteMSBit(BitB, Field2Write, 0);
            }
            InsertBitsAsMSByteMSBit(BitB, 1, StopBit);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteExpGolombAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write) {
        if (BitB != NULL) {
            uint8_t NumBits2Write = IntegerLog2(Field2Write);
            WriteUnaryAsLSByteLSBit(BitB, UnaryType, StopBit, NumBits2Write);
            if (UnaryType == CountUnary) {
                InsertBitsAsLSByteLSBit(BitB, NumBits2Write, Field2Write);
            } else if (UnaryType == WholeUnary) {
                if (Field2Write > 0) {
                    InsertBitsAsLSByteLSBit(BitB, NumBits2Write, Field2Write * 2);
                } else {
                    InsertBitsAsLSByteLSBit(BitB, NumBits2Write, (Absolute(Field2Write) * 2) - 1);
                }
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteExpGolombAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write) {
        if (BitB != NULL) {
            uint8_t NumBits2Write = IntegerLog2(Field2Write);
            WriteUnaryAsLSByteMSBit(BitB, UnaryType, StopBit, NumBits2Write);
            if (UnaryType == CountUnary) {
                InsertBitsAsLSByteMSBit(BitB, NumBits2Write, Field2Write);
            } else if (UnaryType == WholeUnary) {
                if (Field2Write > 0) {
                    InsertBitsAsLSByteMSBit(BitB, NumBits2Write, Field2Write * 2);
                } else {
                    InsertBitsAsLSByteMSBit(BitB, NumBits2Write, (Absolute(Field2Write) * 2) - 1);
                }
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteExpGolombAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write) {
        if (BitB != NULL) {
            uint8_t NumBits2Write = IntegerLog2(Field2Write);
            WriteUnaryAsMSByteLSBit(BitB, UnaryType, StopBit, NumBits2Write);
            if (UnaryType == CountUnary) {
                InsertBitsAsMSByteLSBit(BitB, NumBits2Write, Field2Write);
            } else if (UnaryType == WholeUnary) {
                if (Field2Write > 0) {
                    InsertBitsAsMSByteLSBit(BitB, NumBits2Write, Field2Write * 2);
                } else {
                    InsertBitsAsMSByteLSBit(BitB, NumBits2Write, (Absolute(Field2Write) * 2) - 1);
                }
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void     WriteExpGolombAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write) {
        if (BitB != NULL) {
            uint8_t NumBits2Write = IntegerLog2(Field2Write);
            WriteUnaryAsMSByteMSBit(BitB, UnaryType, StopBit, NumBits2Write);
            if (UnaryType == CountUnary) {
                InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Field2Write);
            } else if (UnaryType == WholeUnary) {
                if (Field2Write > 0) {
                    InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Field2Write * 2);
                } else {
                    InsertBitsAsMSByteMSBit(BitB, NumBits2Write, (Absolute(Field2Write) * 2) - 1);
                }
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    /* BitBuffer Resource Management */
    
    /* BitBuffer GUUID Management */
    uint8_t *ReadGUUIDAsUUIDString(BitBuffer *BitB) {
        uint8_t *UUIDString          = NULL;
        if (BitB != NULL) {
            UUIDString               = calloc(BitIOGUUIDStringSize, sizeof(uint8_t));
            if (UUIDString != NULL) {
                uint32_t Section1    = ExtractBitsAsMSByteLSBit(BitB, 32);
                BitBuffer_Skip(BitB, 8);
                uint16_t Section2    = ExtractBitsAsMSByteLSBit(BitB, 16);
                BitBuffer_Skip(BitB, 8);
                uint16_t Section3    = ExtractBitsAsMSByteLSBit(BitB, 16);
                BitBuffer_Skip(BitB, 8);
                uint16_t Section4    = ExtractBitsAsMSByteLSBit(BitB, 16);
                BitBuffer_Skip(BitB, 8);
                uint64_t Section5    = ExtractBitsAsMSByteLSBit(BitB, 48);
                sprintf((char*)UUIDString, "%d-%d-%d-%d-%llu", Section1, Section2, Section3, Section4, Section5);
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate UUIDString");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return UUIDString;
    }
    
    uint8_t *ReadGUUIDAsGUIDString(BitBuffer *BitB) {
        uint8_t *GUIDString          = NULL;
        if (BitB != NULL) {
            GUIDString               = calloc(BitIOGUUIDStringSize, sizeof(uint8_t));
            if (GUIDString != NULL) {
                uint32_t Section1    = ExtractBitsAsLSByteLSBit(BitB, 32);
                BitBuffer_Skip(BitB, 8);
                uint16_t Section2    = ExtractBitsAsLSByteLSBit(BitB, 16);
                BitBuffer_Skip(BitB, 8);
                uint16_t Section3    = ExtractBitsAsLSByteLSBit(BitB, 16);
                BitBuffer_Skip(BitB, 8);
                uint16_t Section4    = ExtractBitsAsLSByteLSBit(BitB, 16);
                BitBuffer_Skip(BitB, 8);
                uint64_t Section5    = ExtractBitsAsMSByteLSBit(BitB, 48);
                sprintf((char*)GUIDString, "%d-%d-%d-%d-%llu", Section1, Section2, Section3, Section4, Section5);
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate GUIDString");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return GUIDString;
    }
    
    uint8_t *ReadGUUIDAsBinaryUUID(BitBuffer *BitB) {
        uint8_t *BinaryUUID  = NULL;
        if (BitB != NULL) {
            BinaryUUID       = calloc(BitIOBinaryGUUIDSize, sizeof(uint8_t));
            if (BinaryUUID != NULL) {
                for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
                    BinaryUUID[Byte] = ExtractBitsAsMSByteLSBit(BitB, 8);
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate BinaryUUID");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return BinaryUUID;
    }
    
    uint8_t *ReadGUUIDAsBinaryGUID(BitBuffer *BitB) {
        uint8_t *BinaryGUID = NULL;
        if (BitB != NULL) {
            BinaryGUID      = calloc(BitIOBinaryGUUIDSize, sizeof(uint8_t));
            if (BinaryGUID != NULL) {
                for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
                    BinaryGUID[Byte] = ExtractBitsAsLSByteLSBit(BitB, 8);
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate BinaryGUID");
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
        return BinaryGUID;
    }
    
    void WriteGUUIDAsUUIDString(BitBuffer *BitB, const uint8_t *UUIDString) {
        if (BitB != NULL && UUIDString != NULL) {
            for (uint8_t Byte = 0; Byte < BitIOGUUIDStringSize - BitIONULLStringSize; Byte++) {
                InsertBitsAsMSByteLSBit(BitB, 8, UUIDString[Byte]);
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (UUIDString == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "UUIDString Pointer is NULL");
        }
    }
    
    void WriteGUUIDAsGUIDString(BitBuffer *BitB, const uint8_t *GUIDString) {
        if (BitB != NULL && GUIDString != NULL) {
            for (uint8_t Byte = 0; Byte < BitIOGUUIDStringSize - BitIONULLStringSize; Byte++) {
                InsertBitsAsLSByteLSBit(BitB, 8, GUIDString[Byte]);
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (GUIDString == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "GUIDString Pointer is NULL");
        }
    }
    
    void WriteGUUIDAsBinaryUUID(BitBuffer *BitB, const uint8_t *BinaryUUID) {
        if (BitB != NULL && BinaryUUID != NULL) {
            for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
                InsertBitsAsMSByteLSBit(BitB, 8, BinaryUUID[Byte]);
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (BinaryUUID == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BinaryUUID Pointer is NULL");
        }
    }
    
    void WriteGUUIDAsBinaryGUID(BitBuffer *BitB, const uint8_t *BinaryGUID) {
        if (BitB != NULL && BinaryGUID != NULL) {
            for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
                InsertBitsAsLSByteLSBit(BitB, 8, BinaryGUID[Byte]);
            }
        } else if (BitB == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (BinaryGUID == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BinaryGUID Pointer is NULL");
        }
    }
    /* BitBuffer GUUID Management */
    /* End BitBuffer section */
    
    /* BitInput */
    typedef struct BitInput {
        BitInputOutputFileTypes FileType;
        int                     Socket;
        FILE                   *File;
        fpos_t                  FileSize;
        fpos_t                  FilePosition;
        uint64_t                FileSpecifierNum;
    } BitInput;
    
    BitInput *BitInput_Init(void) {
        BitInput *BitI = calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate this instance of BitInput");
        }
        return BitI;
    }
    
    void BitInput_OpenFile(BitInput *BitI, const char *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType          = BitIOFile;
            uint64_t Path2OpenSize  = strlen(Path2Open) + BitIONULLStringSize;
            char *NewPath           = calloc(Path2OpenSize, sizeof(unsigned char));
            snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitI->FileSpecifierNum); // FIXME: HANDLE FORMAT STRINGS BETTER
            BitI->FileSpecifierNum += 1;
            BitI->File = fopen(Path2Open, "rb");
            if (BitI->File == NULL) {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Couldn't open file: Check that the file exists and the permissions are correct");
            } else {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            }
            free(NewPath);
        } else if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        } else if (Path2Open == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Path2Open Pointer is NULL");
        }
    }
    
    void BitInput_OpenSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol) {
        if (BitI != NULL) {
            BitI->FileType   = BitIOSocket;
            BitI->Socket     = socket(Domain, Type, Protocol);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        }
    }
    
    void BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitI != NULL && SocketAddress != NULL) {
            BitI->FileType = BitIOSocket;
            connect(BitI->Socket, SocketAddress, SocketSize);
        } else if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        } else if (SocketAddress == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "SocketAddress Pointer is NULL");
        }
    }
    
    void BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const uint64_t Bytes2Read) {
        uint64_t BytesRead            = 0ULL;
        if (BitI != NULL && Buffer2Read != NULL && Bytes2Read > (uint64_t)(BitI->FileSize - BitI->FilePosition)) {
            if (Buffer2Read->Buffer  != NULL) {
                free(Buffer2Read->Buffer);
            }
            Buffer2Read->Buffer       = calloc(Bytes2Read, sizeof(uint8_t));
            if (Buffer2Read->Buffer != NULL) {
                if (BitI->FileType   == BitIOFile) {
                    BytesRead         = fread(Buffer2Read->Buffer, 1, Bytes2Read, BitI->File);
                } else if (BitI->FileType == BitIOSocket) {
                    BytesRead         = read(BitI->Socket, Buffer2Read->Buffer, Bytes2Read);
                }
                if (BytesRead == Bytes2Read) {
                    Buffer2Read->NumBits = Bytes2Bits(BytesRead);
                } else {
                    BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Fread read: %d bytes, but you requested: %d", BytesRead, Bytes2Read);
                }
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate Buffer in BitBuffer");
            }
        } else if (BitI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        } else if (Buffer2Read == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else if (Bytes2Read > (uint64_t)(BitI->FileSize - BitI->FilePosition)) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "You tried reading more data: % than is available: %d in the file", Bytes2Read, BitI->FileSize - BitI->FilePosition);
        }
    }
    
    fpos_t BitInput_BytesRemainingInFile(BitInput *BitI) {
        fpos_t BytesLeft = 0LL;
        if (BitI != NULL) {
            BytesLeft = BitI->FileSize - BitI->FilePosition;
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        }
        return BytesLeft;
    }
    
    static void BitInput_FindFileSize(BitInput *BitI) {
        if (BitI != NULL) {
            fseek(BitI->File, 0, SEEK_END);
            BitI->FileSize = ftell(BitI->File);
            fseek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition = ftell(BitI->File);
            //fgetpos(BitI->File, &BitI->FilePosition);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        }
    }
    
    fpos_t BitInput_GetFileSize(BitInput *BitI) {
        fpos_t InputSize = 0LL;
        if (BitI != NULL) {
            if (BitI->FileSize != 0) {
                InputSize = BitI->FileSize;
            } else {
                BitInput_FindFileSize(BitI);
            }
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
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
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        }
        return Position;
    }
    
    void BitInput_Deinit(BitInput *BitI) {
        if (BitI != NULL) {
            if (BitI->FileType == BitIOFile) {
                fclose(BitI->File);
            } else if (BitI->FileType == BitIOSocket) {
                close(BitI->Socket);
            }
            free(BitI);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        }
    }
    /* BitInput */
    
    
    /* BitOutput */
    typedef struct BitOutput {
        BitInputOutputFileTypes FileType;
        int                     Socket;
        FILE                   *File;
        fpos_t                  FilePosition;
        uint64_t                FileSpecifierNum;
    } BitOutput;
    
    BitOutput *BitOutput_Init(void) {
        BitOutput *BitO = calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Not enough memory to allocate this instance of BitOutput");
        }
        return BitO;
    }
    
    void BitOutput_OpenFile(BitOutput *BitO, const char *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType          = BitIOFile;
            uint64_t Path2OpenSize  = strlen(Path2Open) + BitIONULLStringSize;
            char *NewPath           = calloc(Path2OpenSize, sizeof(char));
            snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitO->FileSpecifierNum); // FIXME: HANDLE FORMAT STRINGS BETTER
            BitO->FileSpecifierNum += 1;
            BitO->File              = fopen(Path2Open, "wb");
            if (BitO->File != NULL) {
                setvbuf(BitO->File, NULL, _IONBF, 0);
            } else {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Couldn't open output file; Check that the path exists and the permissions are correct");
            }
            free(NewPath);
        } else if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitOutput Pointer is NULL");
        } else if (Path2Open == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Path2Open Pointer is NULL");
        }
    }
    
    void BitOutput_OpenSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol) {
        if (BitO != NULL) {
            BitO->FileType  = BitIOSocket;
            BitO->Socket    = socket(Domain, Type, Protocol);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        }
    }
    
    void BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
        if (BitO != NULL && SocketAddress != NULL) {
            BitO->FileType = BitIOSocket;
            connect(BitO->Socket, SocketAddress, SocketSize);
        } else if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitOutput Pointer is NULL");
        } else if (SocketAddress == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "SocketAddress Pointer is NULL");
        }
    }
    
    void BitOutput_WriteBitBuffer(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write) {
        uint64_t BytesWritten          = 0ULL;
        if (BitO != NULL && Buffer2Write != NULL) {
            uint64_t BufferBytes       = Bits2Bytes(Buffer2Write->NumBits, Yes);
            uint64_t NumBytes2Write    = Bytes2Write > BufferBytes ? Bytes2Write : BufferBytes;
            if (BitO->FileType == BitIOFile) {
                BytesWritten           = fwrite(Buffer2Write->Buffer, 1, NumBytes2Write, BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
                BytesWritten           = write(BitO->Socket, Buffer2Write->Buffer, NumBytes2Write);
            }
            if (BytesWritten != NumBytes2Write) {
                BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "Fwrite wrote: %d bytes, but you requested: %d", BytesWritten, NumBytes2Write);
            } else {
                Buffer2Write->NumBits -= Bytes2Bits(BytesWritten);
            }
        } else if (BitO == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitInput Pointer is NULL");
        } else if (Buffer2Write == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitBuffer Pointer is NULL");
        }
    }
    
    void BitOutput_Deinit(BitOutput *BitO) {
        if (BitO != NULL) {
            fflush(BitO->File);
            if (BitO->FileType == BitIOFile) {
                fclose(BitO->File);
            } else if (BitO->FileType == BitIOSocket) {
                close(BitO->Socket);
            }
            free(BitO);
        } else {
            BitIOLog(BitIOLog_ERROR, BitIOLogLibraryName, __func__, "BitOutput Pointer is NULL");
        }
    }
    /* BitOutput */
    
#ifdef __cplusplus
}
#endif
