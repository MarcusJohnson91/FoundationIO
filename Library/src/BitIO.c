#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE


#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>
#endif

#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    static FILE           *BitIOGlobalLogFile = NULL;
    
    struct                 BitBuffer {
        uint64_t           NumBits;
        uint64_t           BitOffset;
        uint8_t           *Buffer;
    };
    
    struct BitInput {
        FILE              *File;
        int                Socket;
        bool               IsFileOrSocket;
        fpos_t             FileSize;
        fpos_t             FilePosition;
        uint64_t           FileSpecifierNum;
    };
    
    struct BitOutput {
        FILE              *File;
        int                Socket;
        bool               IsFileOrSocket;
        fpos_t             FilePosition;
        uint64_t           FileSpecifierNum;
    };
    
    BitInput *InitBitInput(void) {
        BitInput *BitI        = calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "InitBitInput", "Not enough memory to allocate this instance of BitInput");
        }
        return BitI;
    }
    
    BitOutput *InitBitOutput(void) {
        BitOutput *BitO       = calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "InitBitOutput", "Not enough memory to allocate this instance of BitOutput");
        }
        return BitO;
    }
    
    BitBuffer *InitBitBuffer(const uint64_t BitBufferSize) {
        BitBuffer *BitB       = calloc(1, sizeof(BitBuffer));
        BitB->Buffer          = calloc(1, BitBufferSize);
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "InitBitBuffer", "Not enough memory to allocate this instance of BitBuffer");
        } else if (BitB->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "InitBitBuffer", "Not enough memory to allocate %d bytes for BitBuffer's buffer", BitBufferSize);
        }
        return BitB;
    }
    
    void DeinitBitInput(BitInput *BitI) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "DeinitBitInput", "Pointer to BitInput is NULL");
        } else {
            fclose(BitI->File);
            free(BitI);
        }
    }
    
    void DeinitBitOutput(BitOutput *BitO) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "DeinitBitOutput", "Pointer to BitOutput is NULL");
        } else {
            fflush(BitO->File);
            fclose(BitO->File);
            free(BitO);
        }
    }
    
    void DeinitBitBuffer(BitBuffer *BitB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "DeinitBitBuffer", "Pointer to BitBuffer is NULL");
        } else {
            free(BitB->Buffer);
            free(BitB);
        }
    }
    
    void DeinitBitIOGlobalLogFile(void) {
        fclose(BitIOGlobalLogFile);
    }
    
    inline uint8_t SwapBitsInByte(const uint8_t Byte) {
        return ((Byte & 0x80 >> 7)|(Byte & 0x40 >> 5)|(Byte & 0x20 >> 3)|(Byte & 0x10 >> 1)|(Byte & 0x8 << 1)|(Byte & 0x4 << 3)|(Byte & 0x2 << 5)|(Byte & 0x1 << 7));
    }
    
    inline uint8_t SwapNibble(const uint8_t Byte2Swap) {
        return ((Byte2Swap & 0xF0 >> 4) | (Byte2Swap & 0x0F << 4));
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
    
    inline int64_t Bytes2Bits(const int64_t Bytes) {
        return Bytes * 8;
    }
    
    inline int64_t Bits2Bytes(const int64_t Bits, const bool RoundUp) {
        if (RoundUp == true) {
            return (Bits / 8) + (8 - (Bits % 8));
        } else {
            return Bits / 8;
        }
    }
    
    inline uint64_t NumBits2ReadSymbols(const uint64_t NumSymbols) {
        // Use a binary logarithm, that you round up, in order to get the number of bits required to read a certain number of symbols.
        return ceil(log2(NumSymbols));
    }
    
    inline bool IsOdd(const int64_t Number2Check) {
        bool X = 0;
        if (Number2Check % 2 == 0) {
            X = true;
        } else {
            X = false;
        }
        return X;
    }
    
    fpos_t BytesRemainingInBitInput(BitInput *BitI) {
        fpos_t BytesLeft = 0ULL;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "BytesRemainingInBitInput", "Pointer to BitInput is NULL");
        } else {
            BytesLeft = BitI->FileSize - BitI->FilePosition;
        }
        return BytesLeft;
    }
    
    fpos_t GetBitInputFileSize(BitInput *BitI) {
        fpos_t InputSize = 0LL;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFileSize", "Pointer to BitInput is NULL");
        } else {
            if (BitI->FileSize == 0) {
                FindFileSize(BitI);
            } else {
                InputSize = BitI->FileSize;
            }
        }
        return InputSize;
    }
    
    fpos_t GetBitInputFilePosition(BitInput *BitI) {
        fpos_t Position = 0LL;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFilePosition", "Pointer to BitInput is NULL");
        } else {
            if (BitI->FilePosition == 0) {
                FindFileSize(BitI);
            } else {
                Position = BitI->FilePosition;
            }
        }
        return Position;
    }
    
    uint64_t GetBitBufferPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitBufferPosition", "Pointer to BitBuffer is NULL");
        } else {
            Position = BitB->BitOffset;
        }
        return Position;
    }
    
    uint64_t GetBitBufferSize(BitBuffer *BitB) {
        uint64_t BitBufferSize = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitBufferSize", "Pointer to BitBuffer is NULL");
        } else {
            BitBufferSize = sizeof(BitB->Buffer);
        }
        return BitBufferSize;
    }
    
    void FindFileSize(BitInput *BitI) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "FindFileSize", "Pointer to BitInput is NULL");
        } else {
            fseek(BitI->File, 0, SEEK_END);
            fgetpos(BitI->File, (uint64_t)BitI->FileSize);
            fseek(BitI->File, 0, SEEK_SET);
            fgetpos(BitI->File, &BitI->FilePosition);
        }
    }
    
    void OpenInputFile(BitInput *BitI, const char *Path2Open, bool OpenForReadWrite) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenInputFile", "Pointer to BitInput is NULL");
        } else if (Path2Open == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenInputFile", "Pointer to Path2Open is NULL");
        } else {
            /*
             Now I need to write a function to tell if there is a format specifier in the path, and where it is.
             Well, we need to look through the string for %09d or whateve.
             Or, maybe we should just print Path2Open to another string that is 5 bytes bigger than the current one?
             No, that's a bad idea, but IDK how to do it better.
             
             Also, we need to be able to store when/if FileSpecifierNum has been updated.
             */
            uint64_t Path2OpenSize = strlen(Path2Open) + 1;
            char *NewPath = calloc(1, Path2OpenSize);
            snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitI->FileSpecifierNum += 1); // FIXME: HANDLE FORMAT STRINGS BETTER
            BitI->FileSpecifierNum += 1;
            if (OpenForReadWrite == true) {
                BitI->File = fopen(Path2Open, "rb+");
            } else {
                BitI->File = fopen(Path2Open, "rb");
            }
            if (BitI->File == NULL) {
                Log(LOG_ERR, "libBitIO", "OpenInputFile", "Couldn't open file: Check that the file exists and the permissions are correct");
            }
            free(NewPath);
        }
    }
    
    void OpenOutputFile(BitOutput *BitO, const char *Path2Open) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenOutputFile", "Pointer to BitOutput is NULL");
        } else if (Path2Open == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenOutputFile", "Pointer to Path2Open is NULL");
        } else {
            uint64_t Path2OpenSize = strlen(Path2Open) + 1;
            char *NewPath          = calloc(1, Path2OpenSize);
            snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitO->FileSpecifierNum += 1); // FIXME: HANDLE FORMAT STRINGS BETTER
            
            BitO->File = fopen(Path2Open, "wb");
            if (BitO->File == NULL) {
                Log(LOG_ALERT, "libBitIO", "OpenOutputFile", "Couldn't open output file; Check that the path exists and the permissions are correct");
            }
            free(NewPath);
        }
    }
    
    void OpenLogFile(const char *LogFilePath) {
        if (LogFilePath == NULL) {
            BitIOGlobalLogFile = NULL;
            Log(LOG_ERR, "libBitIO", "OpenLogFile", "Pointer to LogFilePath is NULL");
        } else {
            BitIOGlobalLogFile = fopen(LogFilePath, "a+");
        }
    }
    
    bool IsBitBufferAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        bool AlignmentStatus = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "IsBitBufferAligned", "Pointer to BitBuffer is NULL");
        } else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
            Log(LOG_ERR, "libBitIO", "IsBitBufferAligned", "BytesOfAlignment: %d isn't an integer power of 2", BytesOfAlignment);
        } else {
            if (Bytes2Bits(BytesOfAlignment) - (8 - (BitB->BitOffset % 8)) == 0) {
                AlignmentStatus = true;
            } else {
                AlignmentStatus = false;
            }
        }
        return AlignmentStatus;
    }
    
    void AlignBitBuffer(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "AlignBitBuffer", "Pointer to BitBuffer is NULL");
        } else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
            Log(LOG_ERR, "libBitIO", "AlignBitBuffer", "BytesOfAlignment: %d isn't a power of 2 (or 1)", BytesOfAlignment);
        } else {
            uint8_t Bits2Align = Bytes2Bits(BytesOfAlignment) - (8 - (BitB->BitOffset % 8));
            if (Bits2Align + BitB->BitOffset > BitB->NumBits) {
                BitB->Buffer = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, true));
            }
            BitB->BitOffset   += Bits2Align;
        }
    }
    
    void SkipBits(BitBuffer *BitB, const int64_t Bits2Skip) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "SkipBits", "Pointer to BitBuffer is NULL");
        } else {
            if (Bits2Skip + BitB->BitOffset > BitB->NumBits) {
                BitB->Buffer = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Skip, true));
            }
            BitB->BitOffset += Bits2Skip;
        }
    }
    
    static inline uint8_t CreateBitMaskLSBit(const uint8_t Bits2Extract) {
        return (uint8_t) pow(2, Bits2Extract) << (8 - Bits2Extract);
    }
    
    static inline uint8_t CreateBitMaskMSBit(const uint8_t Bits2Extract) {
        return (uint8_t) pow(2, Bits2Extract) >> (8 - Bits2Extract);
    }
    
    static inline uint8_t NumBits2ExtractFromByte(const uint64_t BitOffset, const uint8_t Bits2Extract) { // BitOffset = 6, Bits2Extract = 3
        uint8_t Bits2ExtractFromThisByte = 0;
        uint8_t BitsInThisByte           = BitOffset % 8; // 2
        if (Bits2Extract >= BitsInThisByte) { // 3 > 6 = false
            Bits2ExtractFromThisByte = BitsInThisByte;
        } else {
            Bits2ExtractFromThisByte = Bits2Extract; // 3
        }
        return Bits2ExtractFromThisByte; // 3 accurate
    }
    
    static inline uint8_t SwapBits(const uint8_t Byte) {
        return ((Byte & 0x80 >> 7)|(Byte & 0x40 >> 5)|(Byte & 0x20 >> 3)|(Byte & 0x10 >> 1)|(Byte & 0x8 << 1)|(Byte & 0x4 << 3)|(Byte & 0x2 << 5)|(Byte & 0x1 << 7));
    }
    
    static inline uint64_t ExtractBitsFromLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Extract) { // So this function reads data FROM Little endian, Least Significant Bit first
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        
        while (UserRequestedBits > 0) { // What happens when the user requests more bits than are available in this byte? bit offset % 8 = 6, user requests 3 bits,only 2 bits can be fulfilled from this byte. wait, this means we shouldn't assume 8 bits are availble, but calculate it as part of the loop.
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskLSBit(Bits2Get);
#ifdef RuntimeLSByte
#ifdef RuntimeLSBit
            OutputData           <<= Bits2Get;
            // Extract as is
#elif  RuntimeMSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
            OutputData           >>= Bits2Get;
            // Extract and flip bit order
#endif
#elif  RuntimeMSByte
#ifdef RuntimeLSBit
            // Extract and flip byte order
            OutputData             & (0xFF << (Bits2Extract - Bits2Get)); // Byte shift
            OutputData           <<= Bits2Get; // Bit shift
#elif  RuntimeMSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
            OutputData             & (0xFF << (Bits2Extract - Bits2Get)); // Byte Shift
            OutputData           >>= Bits2Get; // Bit shift
                                               // Extract and flip the byte order AND bit order
#endif
#endif
            OutputData            += Data;
            UserRequestedBits     -= Bits2Get;
        }
        return OutputData;
    }
    
    static inline uint64_t ExtractBitsFromMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Extract) { // So the bits are in MSByte, MSBit format.
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        
        while (UserRequestedBits > 0) {
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskMSBit(Bits2Get);
#ifdef RuntimeLSByte
#ifdef RuntimeLSBit
            OutputData             & (0xFF << (Bits2Extract - Bits2Get)); // Byte Shift
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
            // Convert MSByte MSBit to LSByte LSBit
#elif  RuntimeMSBit
            OutputData             & (0xFF << (Bits2Extract - Bits2Get)); // Byte Shift
                                                                          // Convert MSByte MSBit to LSByte MSBit
#endif
#elif  RuntimeMSByte
#ifdef RuntimeLSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
            // Convert MSByte MSBit to MSByte LSBit
#elif  RuntimeMSBit
            // Convert MSByte MSBit to MSByte MSBit aka extract.
#endif
#endif
            OutputData           >>= BitB->BitOffset - Bits2Get;
            OutputData            += Data;
            UserRequestedBits     -= Bits2Get;
        }
        return 0ULL;
    }
    
    static inline uint64_t ExtractBitsFromLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Extract) {
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        
        while (UserRequestedBits > 0) {
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskMSBit(Bits2Get);
#ifdef RuntimeLSByte
#ifdef RuntimeLSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
#elif  RuntimeMSBit
            //
#endif
#elif  RuntimeMSByte
#ifdef RuntimeLSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
#elif  RuntimeMSBit
            //
#endif
#endif
            OutputData           >>= BitB->BitOffset - Bits2Get;
            OutputData            += Data;
            UserRequestedBits     -= Bits2Get;
        }
        return 0ULL;
    }
    
    static inline uint64_t ExtractBitsFromMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Extract) { // So the data needs to be 0x6E7C
        uint64_t OutputData        = 0ULL;
        uint8_t  UserRequestedBits = Bits2Extract;
        while (UserRequestedBits > 0) {
            uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskLSBit(Bits2Get);
#ifdef RuntimeLSByte
#ifdef RuntimeLSBit
            /*
             Extract data from Big Endian MSBit first, to little endian least significant bit first
             So, if we need to extract 3 bits because the buffer is full, we need to extract them from the left aka mask with 0xE0
             and we need to apply them as 0x7, aka rightshift 8 - Bits2Read=3 aka 5.
             */
            // SO the bits need to be extracted from BitBuffer as LSBit? and applied as LSBit
            // It is 0x7C6E
#elif  RuntimeMSBit
            // Extract the bits as LSBit, and apply them as MSBit.
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
            // Is is 0x3E76
#endif
#elif  RuntimeMSByte
#ifdef RuntimeLSBit
            // Extract the bits as LSBit and apply them as LSBit
            // Swap Bytes
            // It is 0x6E7C
#elif  RuntimeMSBit
            // Extract the bits as LSBit and apply them as MSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Get);
            // It is 0x763E
#endif
#endif
            OutputData           >>= BitB->BitOffset - Bits2Get;
            OutputData            += Data;
            UserRequestedBits     -= Bits2Get;
        }
        return OutputData;
    }
    
    uint64_t PeekBitsFromLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromLSByteLSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromLSByteLSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        } else {
            OutputData = ExtractBitsFromLSByteLSBit(BitB, Bits2Peek);
        }
        return OutputData;
    }
    
    uint64_t PeekBitsFromLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromLSByteMSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromLSByteMSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        } else {
            OutputData = ExtractBitsFromLSByteMSBit(BitB, Bits2Peek);
        }
        return OutputData;
    }
    
    uint64_t PeekBitsFromMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromMSByteLSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromMSByteLSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        } else {
            OutputData = ExtractBitsFromMSByteLSBit(BitB, Bits2Peek);
        }
        return OutputData;
    }
    
    uint64_t PeekBitsFromMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromMSByteMSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "PeekBitsFromMSByteMSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
        } else {
            OutputData = ExtractBitsFromMSByteMSBit(BitB, Bits2Peek);
        }
        return OutputData;
    }
    
    uint64_t ReadBitsFromLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromLSByteLSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromLSByteLSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        } else {
            // Ok, we need to do absolutely nothing.
            OutputData         = ExtractBitsFromLSByteLSBit(BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        }
        return OutputData;
    }
    
    uint64_t ReadBitsFromLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromLSByteMSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromLSByteMSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        } else {
            // Ok, we need to just swap the bit order
            OutputData         = ExtractBitsFromLSByteMSBit(Bits2Read, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        }
        return OutputData;
    }
    
    uint64_t ReadBitsFromMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromMSByteLSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromMSByteLSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        } else {
            // Ok, we need to read it, and swap endian.
            OutputData         = ExtractBitsFromMSByteLSBit(BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        }
        return OutputData;
    }
    
    uint64_t ReadBitsFromMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromMSByteMSBit", "Pointer to BitBuffer is NULL");
        } else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
            Log(LOG_ERR, "libBitIO", "ReadBitsFromMSByteMSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
        } else {
            // Ok, we need to read it normally except swap the bit order in a byte, and swap endian.
            OutputData         = ExtractBitsFromMSByteMSBit(BitB, Bits2Read);
            BitB->BitOffset   += Bits2Read;
        }
        return OutputData;
    }
    
    static inline void InsertBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // Write data from LSByte (LSBit is default) to LSByte,LSBit
        // What variables do we need to take into account? Just the BitsAvailable, and looping...
        uint8_t Bits = NumBits2Insert;
        
        while (Bits > 0) {
            uint64_t Bits2Put      = NumBits2ExtractFromByte(BitB->BitOffset, Bits);
            uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskLSBit(Bits2Put);
#ifdef RuntimeLSByte
#ifdef RuntimeLSBit
            // Extract as is
#elif  RuntimeMSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Put);
            // Extract and flip bit order
#endif
#elif  RuntimeMSByte
#ifdef RuntimeLSBit
            // Extract and flip byte order
#elif  RuntimeMSBit
            uint8_t FinalByte      = SwapBits(Data, Bits2Put);
            // Extract and flip the byte order AND bit order
#endif
#endif
            Bits                  -= Bits2Put;
        }
    }
    
    static inline void InsertBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // Write data from LSByte (LSBit is default) to LSByte,LSBit
        // What variables do we need to take into account? Just swapping bit order, BitsAvailable, and looping...
    }
    
    static inline void InsertBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // Write data from LSByte (LSBit is default) to MSByte, LSBit.
        // What variables do we need to take into account? Just swapping byte order, BitsAvailable, and looping...
    }
    
    static inline void InsertBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
        // Write data from LSByte (LSBit is default) to MSByte, LSBit.
        // What variables do we need to take into account? Just swapping byte and bit order, BitsAvailable, and looping...
    }
    
    void WriteBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteLSBit", "Pointer to BitBuffer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteLSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        } else {
            InsertBitsAsLSByteLSBit(BitB, NumBits2Write, Bits2Write);
        }
    }
    
    void WriteBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteMSBit", "Pointer to BitBuffer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteMSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        } else {
            InsertBitsAsLSByteMSBit(BitB, NumBits2Write, Bits2Write);
        }
    }
    
    void WriteBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteLSBit", "Pointer to BitBuffer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteLSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        } else {
            InsertBitsAsMSByteLSBit(BitB, NumBits2Write, Bits2Write);
        }
    }
    
    void WriteBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteMSBit", "Pointer to BitBuffer is NULL");
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteMSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
        } else {
            InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Bits2Write);
        }
    }
    
    uint8_t *ReadGUUIDAsUUIDString(BitBuffer *BitB) {
        uint8_t *UUIDString      = calloc(1, BitIOGUUIDStringSize);
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadGUUIDAsUUIDString", "Pointer to BitBuffer is NULL");
        } else {
            uint32_t Section1    = ExtractBitsFromMSByteLSBit(BitB, 32);
            SkipBits(BitB, 8);
            uint16_t Section2    = ExtractBitsFromMSByteLSBit(BitB, 16);
            SkipBits(BitB, 8);
            uint16_t Section3    = ExtractBitsFromMSByteLSBit(BitB, 16);
            SkipBits(BitB, 8);
            uint16_t Section4    = ExtractBitsFromMSByteLSBit(BitB, 16);
            SkipBits(BitB, 8);
            uint64_t Section5    = ExtractBitsFromMSByteLSBit(BitB, 48);
            sprintf((char*)UUIDString, "%d-%d-%d-%d-%llu%d", Section1, Section2, Section3, Section4, Section5, 0x0);
        }
        return UUIDString;
    }
    
    uint8_t *ReadGUUIDAsGUIDString(BitBuffer *BitB) {
        uint8_t *GUIDString      = calloc(1, BitIOGUUIDStringSize);
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadGUUIDAsGUIDString", "Pointer to BitBuffer is NULL");
        } else {
            uint32_t Section1    = ExtractBitsFromLSByteLSBit(BitB, 32);
            SkipBits(BitB, 8);
            uint16_t Section2    = ExtractBitsFromLSByteLSBit(BitB, 16);
            SkipBits(BitB, 8);
            uint16_t Section3    = ExtractBitsFromLSByteLSBit(BitB, 16);
            SkipBits(BitB, 8);
            uint16_t Section4    = ExtractBitsFromLSByteLSBit(BitB, 16);
            SkipBits(BitB, 8);
            uint64_t Section5    = ExtractBitsFromMSByteLSBit(BitB, 48);
            sprintf((char*)GUIDString, "%d-%d-%d-%d-%llu%d", Section1, Section2, Section3, Section4, Section5, 0x0);
        }
        return GUIDString;
    }
    
    uint8_t *ReadGUUIDAsBinaryUUID(BitBuffer *BitB) {
        uint8_t *BinaryUUID  = calloc(1, BitIOBinaryGUUIDSize);
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadGUUIDAsBinaryUUID", "Pointer to BitBuffer is NULL");
        } else {
            for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
                BinaryUUID[Byte] = ExtractBitsFromLSByteLSBit(BitB, 8);
            }
        }
        return BinaryUUID;
    }
    
    uint8_t *ReadGUUIDAsBinaryGUID(BitBuffer *BitB) {
        uint8_t *BinaryGUID = calloc(1, BitIOBinaryGUUIDSize);
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadGUUIDAsBinaryGUID", "Pointer to BitBuffer is NULL");
        } else {
            for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
                BinaryGUID[Byte] = ExtractBitsFromMSByteLSBit(BitB, 8);
            }
        }
        return BinaryGUID;
    }
    
    bool     CompareGUUIDs(const uint8_t *GUUID1, const uint8_t *GUUID2, const uint8_t GUUIDSize) { // The only restriction is that you can only compare GUUIDStrings to GUUIDStrings or BinaryGUUIDs to BinaryGUUIDs.
        bool GUUIDsMatch = true;
        for (uint8_t GUUIDByte = 0; GUUIDByte < GUUIDSize; GUUIDByte++) {
            if (GUUID1[GUUIDByte] != GUUID2[GUUIDByte]) {
                GUUIDsMatch = false;
            }
        }
        return GUUIDsMatch;
    }
    
    uint8_t *ConvertGUUIDString2BinaryGUUID(const uint8_t *GUUIDString) {
        uint8_t *BinaryGUUID = NULL;
        if (GUUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertGUUIDString2BinaryGUUID", "Pointer to GUUIDString is NULL");
        } else {
            BinaryGUUID = calloc(1, BitIOBinaryGUUIDSize);
            if (BinaryGUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertGUUIDString2BinaryGUUID", "Not enough memory to calloc %d bytes", BitIOBinaryGUUIDSize);
            } else {
                BinaryGUUID[0] = GUUIDString[0];
                BinaryGUUID[1] = GUUIDString[1];
                BinaryGUUID[2] = GUUIDString[2];
                BinaryGUUID[3] = GUUIDString[3];
                
                BinaryGUUID[4] = GUUIDString[5];
                BinaryGUUID[5] = GUUIDString[6];
                
                BinaryGUUID[6] = GUUIDString[8];
                BinaryGUUID[7] = GUUIDString[9];
                
                BinaryGUUID[8] = GUUIDString[11];
                BinaryGUUID[9] = GUUIDString[12];
                
                BinaryGUUID[10] = GUUIDString[14];
                BinaryGUUID[11] = GUUIDString[15];
                BinaryGUUID[12] = GUUIDString[16];
                BinaryGUUID[13] = GUUIDString[17];
                BinaryGUUID[14] = GUUIDString[18];
                BinaryGUUID[15] = GUUIDString[19];
            }
        }
        return NULL;
    }
    
    uint8_t *ConvertBinaryGUUID2GUUIDString(const uint8_t *BinaryGUUID) {
        uint8_t *GUUIDString   = NULL;
        if (BinaryGUUID == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertBinaryGUUID2GUUIDString", "Pointer to BinaryGUUID is NULL");
        } else {
            GUUIDString        = calloc(1, BitIOGUUIDStringSize);
            if (GUUIDString == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertBinaryGUUID2GUUIDString", "Not enough memory to calloc %d bytes", BitIOGUUIDStringSize);
            } else {
                GUUIDString[0] = BinaryGUUID[0];
                GUUIDString[1] = BinaryGUUID[1];
                GUUIDString[2] = BinaryGUUID[2];
                GUUIDString[3] = BinaryGUUID[3];
                
                GUUIDString[4] = 0x2D;
                
                GUUIDString[5] = BinaryGUUID[4];
                GUUIDString[6] = BinaryGUUID[5];
                
                GUUIDString[7] = 0x2D;
                
                GUUIDString[8] = BinaryGUUID[6];
                GUUIDString[9] = BinaryGUUID[7];
                
                GUUIDString[10] = 0x2D;
                
                GUUIDString[11] = BinaryGUUID[8];
                GUUIDString[12] = BinaryGUUID[9];
                
                GUUIDString[13] = 0x2D;
                
                GUUIDString[14] = BinaryGUUID[10];
                GUUIDString[15] = BinaryGUUID[11];
                GUUIDString[16] = BinaryGUUID[12];
                GUUIDString[17] = BinaryGUUID[13];
                GUUIDString[18] = BinaryGUUID[14];
                GUUIDString[19] = BinaryGUUID[15];
                GUUIDString[20] = 0x00;
            }
        }
        return GUUIDString;
    }
    
    uint8_t *SwapGUUIDString(const uint8_t *GUUIDString) {
        uint8_t *SwappedGUUIDString    = NULL;
        if (GUUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "SwapGUUIDString", "Pointer to GUUIDString is NULL");
        } else {
            SwappedGUUIDString         = calloc(1, BitIOGUUIDStringSize);
            if (SwappedGUUIDString == NULL) {
                Log(LOG_ERR, "libBitIO", "SwapGUUIDString", "Not enough memory to calloc %d bytes", BitIOGUUIDStringSize);
            } else {
                SwappedGUUIDString[0]  = GUUIDString[3];
                SwappedGUUIDString[1]  = GUUIDString[2];
                SwappedGUUIDString[2]  = GUUIDString[1];
                SwappedGUUIDString[3]  = GUUIDString[0];
                
                SwappedGUUIDString[4]  = GUUIDString[4];
                
                SwappedGUUIDString[5]  = GUUIDString[6];
                SwappedGUUIDString[6]  = GUUIDString[5];
                
                SwappedGUUIDString[7]  = GUUIDString[7];
                
                SwappedGUUIDString[8]  = GUUIDString[9];
                SwappedGUUIDString[9]  = GUUIDString[8];
                
                SwappedGUUIDString[10] = GUUIDString[10];
                
                SwappedGUUIDString[11] = GUUIDString[12];
                SwappedGUUIDString[12] = GUUIDString[11];
                
                for (uint8_t EndBytes = 13; EndBytes < BitIOGUUIDStringSize - 1; EndBytes++) {
                    SwappedGUUIDString[EndBytes] = GUUIDString[EndBytes];
                }
                SwappedGUUIDString[20] = 0x00;
            }
        }
        return SwappedGUUIDString;
    }
    
    uint8_t *SwapBinaryGUUID(const uint8_t *BinaryGUUID) {
        uint8_t *SwappedBinaryGUUID   = NULL;
        if (BinaryGUUID == NULL) {
            Log(LOG_ERR, "libBitIO", "SwapBinaryGUUID", "Pointer to BinaryGUUID is NULL");
        } else {
            SwappedBinaryGUUID        = calloc(1, BitIOBinaryGUUIDSize);
            if (SwappedBinaryGUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "SwapBinaryGUUID", "Not enough memory to calloc %d bytes", BitIOBinaryGUUIDSize);
            } else {
                SwappedBinaryGUUID[0] = BinaryGUUID[3];
                SwappedBinaryGUUID[1] = BinaryGUUID[2];
                SwappedBinaryGUUID[2] = BinaryGUUID[1];
                SwappedBinaryGUUID[3] = BinaryGUUID[0];
                
                SwappedBinaryGUUID[4] = BinaryGUUID[5];
                SwappedBinaryGUUID[5] = BinaryGUUID[4];
                
                SwappedBinaryGUUID[6] = BinaryGUUID[7];
                SwappedBinaryGUUID[7] = BinaryGUUID[6];
                
                SwappedBinaryGUUID[8] = BinaryGUUID[9];
                SwappedBinaryGUUID[9] = BinaryGUUID[8];
                
                for (uint8_t EndBytes = 10; EndBytes < BitIOBinaryGUUIDSize - 1; EndBytes++) {
                    SwappedBinaryGUUID[EndBytes] = BinaryGUUID[EndBytes];
                }
            }
        }
        return SwappedBinaryGUUID;
    }
    
    uint8_t *WriteGUUIDAsUUIDString(BitBuffer *BitB, const uint8_t *UUIDString) {
        return NULL;
    }
    
    uint8_t *WriteGUUIDAsGUIDString(BitBuffer *BitB, const uint8_t *GUIDString) {
        return NULL;
    }
    
    uint8_t *WriteGUUIDAsBinaryUUID(BitBuffer *BitB, const uint8_t *BinaryUUID) {
        return NULL;
    }
    
    uint8_t *WriteGUUIDAsBinaryGUID(BitBuffer *BitB, const uint8_t *BinaryGUID) {
        return NULL;
    }
    
    void DeinitGUUID(uint8_t *GUUID) {
        free(GUUID);
    }
    
    /*
     So, I need to accept packets of variable size, from the network, disk, whatever.
     
     Should BitInput and BitOutput store a value saying what type of "file" it's being written to? or just write to the file handle regardless?
     So, the gist is we need a function to read from a file or network stream, and create a buffer from that data, and pop it into the struct.
     Ok, so I need a function to read a file/socket into a buffer, and one to write a buffer to a file/socket.
     I don't know if FILE pointers work with sockets, but i'm going to ignore that for now.
     */
    void ReadBitInput2BitBuffer(BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read) { // It's the user's job to ensure buffers and files are kept in sync, not mine.
        uint64_t BytesRead              = 0ULL;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "Pointer to BitInput is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "Pointer to BitBuffer is NULL");
        } else if (Bytes2Read > (uint64_t)(BitI->FileSize - BitI->FilePosition)) {
            Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "You tried reading more data: % than is available: %d in the file", Bytes2Read, BitI->FileSize - BitI->FilePosition);
        } else {
            if (BitB->Buffer != NULL) {
                free(BitB->Buffer);
            }
            BitB->Buffer                  = calloc(1, Bytes2Read);
            if (BitB->Buffer == NULL) {
                Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "Not enough memory to allocate Buffer in BitBuffer");
            } else {
                BytesRead                 = fread(BitB->Buffer, 1, Bytes2Read, BitI->File);
                if (BytesRead            != Bytes2Read) {
                    Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "Fread read: %d bytes, but you requested: %d", BytesRead, Bytes2Read);
                } else {
                    BitB->NumBits         = Bytes2Bits(BytesRead);
                }
            }
        }
    }
    
    void WriteBitBuffer2BitOutput(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write) { // FIXME Assuming we wrote the whole buffer
        uint64_t BytesWritten           = 0ULL;
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitBuffer2BitOutput", "Pointer to BitInput is NULL");
        } else if (Buffer2Write == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitBuffer2BitOutput", "Pointer to BitBuffer is NULL");
        } else {
            BytesWritten               = fwrite(Buffer2Write->Buffer, 1, Bytes2Write, BitO->File);
            if (BytesWritten != Bytes2Write) {
                Log(LOG_ERR, "libBitIO", "WriteBitBuffer2BitOutput", "Fwrite wrote: %d bytes, but you requested: %d", BytesWritten, Bytes2Write);
            } else {
                Buffer2Write->NumBits -= Bytes2Bits(BytesWritten);
            }
        }
    }
    
    void OpenInputSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenInputSocket", "Pointer to BitInput is NULL");
        } else {
#ifdef _POSIX_VERSION
            BitI->Socket         = socket(Domain, Type, Protocol);
            BitI->IsFileOrSocket = &socket;
#elif  _WIN32
#endif
        }
    }
    
    void OpenOutputSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenOutputSocket", "Pointer to BitInput is NULL");
        } else {
#ifdef _POSIX_VERSION
            BitO->Socket         = socket(Domain, Type, Protocol);
            BitO->IsFileOrSocket = &socket;
#elif  _WIN32
#endif
        }
    }
    
    void ConnectInputSocket(BitInput *BitI) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "ConnectInputSocket", "Pointer to BitOutput is NULL");
        } else {
            
        }
    }
    
    void ConnectOutputSocket(BitOutput *BitO) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "ConnectOutputSocket", "Pointer to BitOutput is NULL");
        } else {
            
        }
    }
    
    void Log(const uint8_t ErrorSeverity, const char *__restrict LibraryOrProgram, const char *__restrict FunctionName, const char *__restrict Description, ...) {
        int   EasyStringSize = strlen(LibraryOrProgram) + strlen(FunctionName) + strlen(Description) + 1; // Plus 1 for the terminating NULL
        char *EasyString     = calloc(1, EasyStringSize); // the 1 is for the error severity + 2 for the NULs
        snprintf(EasyString, EasyStringSize, "%hhu: %s - %s", ErrorSeverity, FunctionName, Description);
        
        va_list Arguments;
        va_start(Arguments, Description);
        int HardStringSize = vsnprintf(NULL, 0, "%s", Arguments);
        char *HardString   = calloc(1, HardStringSize);
        vsprintf(HardString, "%s", Arguments);
        va_end(Arguments);
        
        uint64_t ErrorStringSize = EasyStringSize + HardStringSize + BitIONewLineSize;
        char *ErrorString = calloc(1, ErrorStringSize);
        snprintf(ErrorString, ErrorStringSize, "%s%s%s", EasyString, HardString, BitIOLineEnding);
        if (BitIOGlobalLogFile == NULL) {
            // Set STDERR As the output file
            fprintf(stderr, "%s", ErrorString);
        } else {
            // Use BitO->LogFile as the output file
            fprintf(BitIOGlobalLogFile, "%s", ErrorString);
        }
        free(EasyString);
        free(HardString);
        free(ErrorString);
    }
    
#ifdef __cplusplus
}
#endif
