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
#include <unistd.h>
#endif

#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    extern FILE           *BitIOGlobalLogFile = NULL; // ONLY global variable in all of BitIO
    
    typedef struct BitBuffer {
        uint64_t           BitsAvailable;
        uint64_t           BitsUnavailable;
        uint8_t           *Buffer;
    } BitBuffer;
    
    typedef struct BitInput {
        FILE              *File;
        bool               IsFileOrSocket;
        int                Socket;
        fpos_t             FileSize;
        fpos_t             FilePosition;
        uint64_t           FileSpecifierNum;
        uint8_t            SystemEndian;
    } BitInput;
    
    typedef struct BitOutput {
        FILE              *File;
        bool               IsFileOrSocket;
        int                Socket;
        fpos_t             FilePosition;
        uint64_t           FileSpecifierNum;
        uint8_t            SystemEndian;
    } BitOutput;
    
    typedef struct SymbolFrequencies { // We assume that all the symbols are unsigned integers of varying size
        uint64_t           NumSymbols; // Probability is just the frequency of occurance divided by the number of symbols.
        uint8_t            SymbolSize; // number of bytes per symbol
        void              *Frequency;
    } SymbolFrequencies;
    
    BitInput *InitBitInput(void) {
        BitInput *BitI        = (BitInput*) calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "InitBitInput", "Not enough memory to allocate this instance of BitInput");
        }
        return BitI;
    }
    
    BitOutput *InitBitOutput(void) {
        BitOutput *BitO       = (BitOutput*) calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "InitBitOutput", "Not enough memory to allocate this instance of BitOutput");
        }
        return BitO;
    }
    
    BitBuffer *InitBitBuffer(const uint64_t BitBufferSize) {
        BitBuffer *BitB       = (BitBuffer*) calloc(1, sizeof(BitBuffer));
        BitB->Buffer          =              calloc(1, BitBufferSize);
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
    
    inline uint8_t CountBitsSet(const uint64_t Bits2Count) {
        uint8_t DataBit = 0, BitCount = 0;
        for (uint8_t Bit = 0; Bit < 64; Bit++) {
            DataBit = (Bits2Count & (1 << Bit)) >> Bit;
            if (DataBit == 1) {
                BitCount += 1;
            }
        }
        return BitCount;
    }
    
    inline uint8_t FindHighestBitSet(const uint64_t UnsignedInt2Search) { // UnsignedInt2Search = 0x8000
        uint8_t  HighestBitSet = 0;
        uint64_t Shift         = 0ULL;
        
        if (UnsignedInt2Search == 0) {
            HighestBitSet = 0;
        } else {
            for (uint8_t Bit = 1; Bit < 64; Bit++) {
                // We should count up tho, that way the last update is the last highest bit, that also means it requires 64 loops no matter what :(
                Shift = 1ULL << (Bit - 1);
                if (((UnsignedInt2Search & Shift) >> (Bit - 1)) == 1) {
                    HighestBitSet = Bit;
                }
            }
        }
        return HighestBitSet;
    }
    
    inline uint8_t Power2Mask(const uint8_t BitOrder, const uint8_t Exponent) {
        uint8_t Mask = 0;
        if (Exponent > 8) {
            Log(LOG_ERR, "libBitIO", "Power2Mask", "Exponent %d is too large", Exponent);
        } else {
            if (BitOrder == LSBit) {
                Mask = (uint8_t) (pow(2, Exponent) - 1);
            } else if (BitOrder == MSBit) {
                Mask = (uint8_t) (pow(2, Exponent) - 1) << (8 - Exponent);
            }
        }
        return Mask;
    }
    
    inline bool IsOdd(const int64_t Number2Check) {
        return Number2Check % 2 == 0 ? true : false;
    }
    
    static uint8_t DetectSystemEndian(void) {
        uint8_t  SystemEndian = 0;
        uint16_t Endian       = 0xFFFE;
        if (Endian == 0xFFFE) {
            SystemEndian = LilEndianLSBit;
        } else if (Endian == 0xFF7F) {
            SystemEndian = LilEndianMSBit;
        } else if (Endian == 0xFEFF) {
            SystemEndian = BigEndianLSBit;
        } else if (Endian == 0xBFFF) {
            SystemEndian = BigEndianMSBit;
        } else {
            SystemEndian = UnknownEndian;
        }
        return SystemEndian;
    }
    
    fpos_t BytesRemainingInInputFile(BitInput *BitI) {
        fpos_t BytesLeft = 0ULL;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "BytesRemainingInInputFile", "Pointer to BitInput is NULL");
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
                GetFileSize(BitI);
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
                GetFileSize(BitI);
            } else {
                Position = BitI->FilePosition;
            }
        }
        return Position;
    }
    
    uint64_t GetBitBufferPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitBufferPosition", "Pointer to BitInput is NULL");
        } else {
            Position = BitB->BitsUnavailable;
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
    
    uint8_t GetBitInputSystemEndian(BitInput *BitI) {
        uint8_t Endian = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputSystemEndian", "Pointer to BitInput is NULL");
        } else {
            Endian = BitI->SystemEndian;
        }
        return Endian;
    }
    
    uint8_t GetBitOutputSystemEndian(BitOutput *BitO) {
        uint8_t Endian = 0;
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitOutputSystemEndian", "Pointer to BitOutput is NULL");
        } else {
            Endian = BitO->SystemEndian;
        }
        return Endian;
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
            char *NewPath = (char*) calloc(1, Path2OpenSize);
            snprintf(NewPath, Path2OpenSize, "%s", Path2Open, BitI->FileSpecifierNum + 1);
            BitI->FileSpecifierNum += 1;
            
            if (OpenForReadWrite == true) {
                BitI->File = fopen(Path2Open, "rb+");
            } else {
                BitI->File = fopen(Path2Open, "rb");
            }
            
            if (BitI->File == NULL) {
                Log(LOG_ERR, "libBitIO", "OpenInputFile", "Couldn't open file: Check that the file exists and the permissions are correct");
            } else {
                BitI->SystemEndian = DetectSystemEndian();
            }
        }
    }
    
    void GetFileSize(BitInput *BitI) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetFileSize", "Pointer to BitInput is NULL");
        } else {
            fseek(BitI->File, 0, SEEK_END);
            fgetpos(BitI->File, (uint64_t)BitI->FileSize);
            fseek(BitI->File, 0, SEEK_SET);
            fgetpos(BitI->File, &BitI->FilePosition);
        }
    }
    
    void OpenOutputFile(BitOutput *BitO, const char *Path2Open) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenOutputFile", "Pointer to BitOutput is NULL");
        } else if (Path2Open == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenOutputFile", "Fopen error: Pointer is NULL");
        } else {
            uint64_t Path2OpenSize = strlen(Path2Open) + 1;
            char *NewPath = (char*) calloc(1, Path2OpenSize);
            snprintf(NewPath, Path2OpenSize, "%s", Path2Open, BitO->FileSpecifierNum);
            
            BitO->File = fopen(Path2Open, "wb");
            if (BitO->File == NULL) {
                Log(LOG_ALERT, "libBitIO", "OpenOutputFile", "Couldn't open file: Check that the file exists and the permissions are correct");
            }
            BitO->SystemEndian          = DetectSystemEndian();
        }
    }
    
    void OpenLogFile(const char *LogFilePath) {
        if (LogFilePath == NULL) {
            BitIOGlobalLogFile = NULL;
            Log(LOG_ERR, "libBitIO", "OpenLogFile", "LogFilePath is NULL");
        } else {
            BitIOGlobalLogFile = fopen(LogFilePath, "a+");
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
        
    }
    
    void ConnectOutputSocket(BitOutput *BitO) {
        
    }
    /*
    uint64_t ReadBits(const uint8_t ByteBitOrder, BitBuffer *BitB, const uint8_t Bits2Read) {
        uint8_t Bits = Bits2Read, UserRequestBits = 0, BufferBitsAvailable = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0ULL;
        
        if (ByteBitOrder == LSBit || ByteBitOrder == MSBit || ByteBitOrder > 6) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Invalid ByteBitOrder: %d", ByteBitOrder);
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Pointer to BitBuffer is NULL");
        } else if (BitB->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Pointer to Buffer in BitBuffer is NULL");
        } else if (Bits2Read <= 0 || Bits2Read > 64) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Bits2Read: %d, only supports reading 1-64 bits", Bits2Read);
        } else {
            uint8_t BitsAvailabeInCurrentByte = 8 - (BitB->BitsUnavailable % 8);
            uint8_t Bits2ReadFromCurrentByte  = Bits2Read > 8 ? BitsAvailabeInCurrentByte : Bits2Read;
            if (ByteBitOrder == LilEndianLSBit) { // Read Lil endian LSBit first
#if   (TARGETBYTEORDER == BITIOLITTLEENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Little Endian, Least Significant Bit First.
                ReadBitsTargetLELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Little Endian, Most Significant Bit First.
                ReadBitsTargetLEMSB(BitB, Bits2Read);
#endif
#elif (TARGETBYTEORDER == BITIOBIGENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Big Endian, Least Significant Bit First.
                ReadBitsTargetBELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Big Endian, Most Significant Bit First.
                ReadBitsTargetBEMSB(BitB, Bits2Read);
#endif
#endif
            } else if (ByteBitOrder == BigEndianLSBit) { // Read Big endian LSBit first
#if   (TARGETBYTEORDER == BITIOLITTLEENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Little Endian, Least Significant Bit First.
                ReadBitsTargetLELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Little Endian, Most Significant Bit First.
                ReadBitsTargetLEMSB(BitB, Bits2Read);
#endif
#elif (TARGETBYTEORDER == BITIOBIGENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Big Endian, Least Significant Bit First.
                ReadBitsTargetBELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Big Endian, Most Significant Bit First.
                ReadBitsTargetBEMSB(BitB, Bits2Read);
#endif
#endif
            } else if (ByteBitOrder == BigEndianMSBit) { // Read Big endian MSBit first
#if   (TARGETBYTEORDER == BITIOLITTLEENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Little Endian, Least Significant Bit First.
                ReadBitsTargetLELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Little Endian, Most Significant Bit First.
                ReadBitsTargetLEMSB(BitB, Bits2Read);
#endif
#elif (TARGETBYTEORDER == BITIOBIGENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Big Endian, Least Significant Bit First.
                ReadBitsTargetBELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Big Endian, Most Significant Bit First.
                ReadBitsTargetBEMSB(BitB, Bits2Read);
#endif
#endif
            } else if (ByteBitOrder == LilEndianMSBit) { // Read Lil endian MSBit first
#if   (TARGETBYTEORDER == BITIOLITTLEENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Little Endian, Least Significant Bit First.
                ReadBitsTargetLELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Little Endian, Most Significant Bit First.
                ReadBitsTargetLEMSB(BitB, Bits2Read);
#endif
#elif (TARGETBYTEORDER == BITIOBIGENDIAN)
#if   (TARGETBITORDER  == BITIOLSBitFIRST)
                // Target = Big Endian, Least Significant Bit First.
                ReadBitsTargetBELSB(BitB, Bits2Read);
#elif (TARGETBITORDER  == BITIOMSBitFIRST)
                // Target = Big Endian, Most Significant Bit First.
                ReadBitsTargetBEMSB(BitB, Bits2Read);
#endif
#endif
            }
        }
        return OutputData;
    }
    
    uint64_t PeekBits2(const uint8_t ByteBitOrder, BitBuffer *BitB, const uint8_t Bits2Peek) {
        uint64_t PeekedBits = 0ULL;
        if (ByteBitOrder == LSBit || ByteBitOrder == MSBit || ByteBitOrder > 6) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Invalid ByteBitOrder: %d", ByteBitOrder);
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Pointer to BitBuffer is NULL");
        } else if (BitB->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Pointer to Buffer in BitBuffer is NULL");
        } else if (Bits2Peek <= 0 || Bits2Peek > 64) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Bits2Peek: %d, only supports reading 1-64 bits", Bits2Peek);
        } else {
            if (ByteBitOrder == BigEndianLSBit) {
                
            } else if (ByteBitOrder == LilEndianLSBit) {
                
            } else if (ByteBitOrder == BigEndianMSBit) {
                
            } else if (ByteBitOrder == LilEndianMSBit) {
                
            }
        }
        return PeekedBits;
    }
    
    uint64_t ReadRICE2(const uint8_t ByteBitOrder, BitBuffer *BitB, const bool Truncated, const bool StopBit) {
        uint64_t DeRICEdBits = 0ULL;
        if (ByteBitOrder == LSBit || ByteBitOrder == MSBit || ByteBitOrder > 6) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Invalid ByteBitOrder: %d", ByteBitOrder);
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Pointer to BitBuffer is NULL");
        } else if (BitB->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Pointer to Buffer in BitBuffer is NULL");
        } else {
            if (ByteBitOrder == BigEndianLSBit) {
                
            } else if (ByteBitOrder == LilEndianLSBit) {
                
            } else if (ByteBitOrder == BigEndianMSBit) {
                
            } else if (ByteBitOrder == LilEndianMSBit) {
                
            }
        }
        return DeRICEdBits;
    }
    
    void *ReadExpGolomb2(const uint8_t ByteBitOrder, BitBuffer *BitB, const bool IsSigned) {
        uint64_t DeExpGolombedBits = 0ULL;
        if (ByteBitOrder == LSBit || ByteBitOrder == MSBit || ByteBitOrder > 6) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Invalid ByteBitOrder: %d", ByteBitOrder);
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Pointer to BitBuffer is NULL");
        } else if (BitB->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBits2", "Pointer to Buffer in BitBuffer is NULL");
        } else {
            if (ByteBitOrder == BigEndianLSBit) {
                
            } else if (ByteBitOrder == LilEndianLSBit) {
                
            } else if (ByteBitOrder == BigEndianMSBit) {
                
            } else if (ByteBitOrder == LilEndianMSBit) {
                
            }
        }
        return DeExpGolombedBits;
    }
    
    /*
    uint64_t ReadBits(BitBuffer *BitB, const uint8_t Bits2Read, const bool ReadFromMSB) {
        uint8_t Bits = Bits2Read, UserRequestBits = 0, BufferBitsAvailable = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0ULL;
        
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Pointer to BitBuffer is NULL");
        } else if (BitB->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Pointer to Buffer in BitBuffer is NULL");
        } else if (Bits2Read > 64 || Bits2Read == 0) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Bits2Read: %d, only supports reading 1-64 bits", Bits2Read);
        } else {
            while (Bits > 0) {
                BufferBitsAvailable = 8 - (BitB->BitsUnavailable % 8);
                UserRequestBits     = Bits > 8 ? 8 : Bits;
                Bits                = BufferBitsAvailable >= UserRequestBits  ? UserRequestBits : BufferBitsAvailable;
                Mask2Shift          = ReadFromMSB ? BitB->BitsAvailable % 8 : 0;
                if (ReadFromMSB == true) {
                    Mask2Shift      = BufferBitsAvailable <= UserRequestBits  ? 0 : BufferBitsAvailable - UserRequestBits;
                    Mask            = (Power2Mask(LSBit, Bits) << Mask2Shift);
                } else {
                    Mask            = (Power2Mask(LSBit, Bits) - 1) << BitB->BitsUnavailable % 8;
                }
                Data                = BitB->Buffer[BitB->BitsUnavailable / 8] & Mask;
                Data                 >>= Mask2Shift;
                OutputData           <<= BufferBitsAvailable >= UserRequestBits ? UserRequestBits : BufferBitsAvailable;
                OutputData            += Data;
                BitB->BitsAvailable   -= BufferBitsAvailable >= UserRequestBits ? UserRequestBits : BufferBitsAvailable;
                BitB->BitsUnavailable += BufferBitsAvailable >= UserRequestBits ? UserRequestBits : BufferBitsAvailable;
                Bits                  -= BufferBitsAvailable >= UserRequestBits ? UserRequestBits : BufferBitsAvailable;
            }
        }
        return OutputData;
    }
    
    uint64_t PeekBits(BitBuffer *BitB, const uint8_t Bits2Peek, const bool ReadFromMSB) {
        uint64_t OutputData = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "PeekBits", "Pointer to BitBuffer is NULL");
        } else {
            OutputData                 = ReadBits(BitB, Bits2Peek, ReadFromMSB);
            BitB->BitsAvailable       += Bits2Peek; // Backwards to set the counter back to where it was.
            BitB->BitsUnavailable     -= Bits2Peek;
        }
        return OutputData;
    }
    
    uint64_t  ReadRICE(BitBuffer *BitB, const bool Truncated, const bool StopBit) {
        uint64_t BitCount = 0ULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadRICE", "Pointer to BitBuffer is NULL");
        } else {
            while (ReadBits(BitB, 1, false) != (StopBit & 1)) {
                BitCount += 1;
            }
            if (Truncated == true) {
                BitCount++;
            }
        }
        return BitCount;
    }
    
    int64_t ReadExpGolomb(BitBuffer *BitB, const bool IsSigned) {
        int64_t  Final = 0LL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadExpGolomb", "Pointer to BitBuffer is NULL");
        } else {
            uint64_t Zeros   = 0ULL;
            uint64_t CodeNum = 0ULL;
            
            while (ReadBits(BitB, 1, false) != 1) {
                Zeros += 1;
            }
            
            if (IsSigned == false) {
                CodeNum  = (1ULL << Zeros);
                CodeNum += ReadBits(BitB, Zeros, false);
                Final    = CodeNum;
            } else { // Signed
                if (IsOdd(CodeNum) == true) {
                    Final = CodeNum - 1;
                } else {
                    Final = -(CodeNum - 1);
                }
            }
        }
        return Final;
    }
     */
    
    void SkipBits(BitBuffer *BitB, const int64_t Bits2Skip) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "SkipBits", "Pointer to BitBuffer is NULL");
        } else {
            BitB->BitsAvailable   -= Bits2Skip;
            BitB->BitsUnavailable += Bits2Skip;
            // The file/stream updating functions need to keep this in mind.
        }
    }
    
    void WriteBits(const uint8_t ByteBitOrder, BitBuffer *Buffer2Write, const uint8_t Bits2Write, const uint64_t Value2Write) {
        if (ByteBitOrder == LSBit || ByteBitOrder == MSBit || ByteBitOrder > 6) {
            Log(LOG_ERR, "libBitIO", "WriteBits", "Invalid ByteBitOrder: %d", ByteBitOrder);
        } else if (Buffer2Write == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBits", "Pointer to BitBuffer is NULL");
        } else if (Buffer2Write->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBits", "Pointer to Buffer in BitBuffer is NULL");
        } else {
            if (ByteBitOrder == BigEndianLSBit) {
                
            } else if (ByteBitOrder == LilEndianLSBit) {
                
            } else if (ByteBitOrder == BigEndianMSBit) {
                
            } else if (ByteBitOrder == LilEndianMSBit) {
                
            }
        }
    }
    
    void WriteRICE(const uint8_t ByteBitOrder, BitBuffer *Buffer2Write, const uint8_t NumBits2Write, const uint64_t Value2Write, const bool Truncate) {
        if (ByteBitOrder == LSBit || ByteBitOrder == MSBit || ByteBitOrder > 6) {
            Log(LOG_ERR, "libBitIO", "WriteRICE", "Invalid ByteBitOrder: %d", ByteBitOrder);
        } else if (Buffer2Write == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteRICE", "Pointer to BitBuffer is NULL");
        } else if (Buffer2Write->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteRICE", "Pointer to Buffer in BitBuffer is NULL");
        } else {
            if (ByteBitOrder == BigEndianLSBit) {
                
            } else if (ByteBitOrder == LilEndianLSBit) {
                
            } else if (ByteBitOrder == BigEndianMSBit) {
                
            } else if (ByteBitOrder == LilEndianMSBit) {
                
            }
        }
    }
    
    void WriteExpGolomb(const uint8_t ByteBitOrder, BitBuffer *Buffer2Write, const uint8_t NumBits2Write, const uint64_t Value2Write) {
        if (ByteBitOrder == LSBit || ByteBitOrder == MSBit || ByteBitOrder > 6) {
            Log(LOG_ERR, "libBitIO", "WriteExpGolomb", "Invalid ByteBitOrder: %d", ByteBitOrder);
        } else if (Buffer2Write == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteExpGolomb", "Pointer to BitBuffer is NULL");
        } else if (Buffer2Write->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteExpGolomb", "Pointer to Buffer in BitBuffer is NULL");
        } else {
            if (ByteBitOrder == BigEndianLSBit) {
                
            } else if (ByteBitOrder == LilEndianLSBit) {
                
            } else if (ByteBitOrder == BigEndianMSBit) {
                
            } else if (ByteBitOrder == LilEndianMSBit) {
                
            }
        }
    }
    
    /*
    void WriteBits(BitBuffer *BitB, const uint64_t Data2Write, const uint8_t NumBits, const bool WriteFromMSB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBits", "Pointer to BitBuffer is NULL");
        } else {
            // FIXME: WriteBits currently copies NumBits bits to the file, even if the input is shorter than that. we need to prepend 0 bits if that's the case
            
            uint8_t Bits2Write2BufferByte, Bits2ShiftMask, Mask, Bits2Write = NumBits;
            
            // so we're trying to write 9 bits, 0x1FF.
            // There are only 5 bits available in the current byte.
            // So, that means we have to pull out 5 bits from Data2Write, and pop it into BitB->Buffer.
            // The Mask should be 0x1F if LSBfirst, or 0xF8 if MSB first.
            // The buffer's representation is MSB first.
            // if we're supposed to write this data LSB first we need to shift it after extraction to get it ready to be applied.
            while (Bits2Write > 0) {
                Bits2Write2BufferByte  = 8 - (BitB->BitsAvailable % 8); // extract 5 bits from the buffer
                Bits2ShiftMask         = WriteFromMSB ? BitB->BitsAvailable % 8 : 0;
                Mask                   = Power2Mask(LSBit, Bits2Write2BufferByte) << Bits2ShiftMask;
                BitB->Buffer[Bits2Bytes(BitB->BitsAvailable, false)] = Data2Write & Mask;
                Bits2Write            -= Bits2Write2BufferByte;
                BitB->BitsAvailable   -= Bits2Write2BufferByte;
                BitB->BitsUnavailable += Bits2Write2BufferByte;
            }
        }
    }
    
    void WriteRICE(BitBuffer *BitB, const bool Truncated, const bool StopBit, const uint64_t Data2Write, const bool WriteFromMSB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteRICE", "Pointer to BitBuffer is NULL");
        } else {
            for (uint64_t Bit = 0ULL; Bit < Data2Write; Bit++) {
                WriteBits(BitB, (~StopBit), 1, WriteFromMSB);
            }
            WriteBits(BitB, StopBit, 1, WriteFromMSB);
        }
    }
    
    void WriteExpGolomb(BitBuffer *BitB, const bool IsSigned, const uint64_t Data2Write, const bool WriteFromMSB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteExpGolomb", "Pointer to BitOutput is NULL");
        } else {
            uint64_t NumBits = 0ULL;
            
            NumBits = FindHighestBitSet(Data2Write);
            
            if (IsSigned == false) {
                WriteBits(BitB, 0, NumBits, WriteFromMSB);
                WriteBits(BitB, Data2Write + 1, NumBits + 1, WriteFromMSB);
            } else {
                NumBits -= 1;
                WriteBits(BitB, 0, NumBits, WriteFromMSB);
                if (IsOdd(Data2Write +1) == false) {
                    WriteBits(BitB, Data2Write + 1, NumBits + 1, WriteFromMSB);
                } else {
                    WriteBits(BitB, Data2Write + 1, NumBits + 1, WriteFromMSB);
                }
            }
        }
    }
     */
    
    bool IsBitBufferAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        bool AlignmentStatus = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "IsBitBufferAligned", "Pointer to BitBuffer is NULL");
        } else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
            Log(LOG_ERR, "libBitIO", "IsBitBufferAligned", "BytesOfAlignment: %d isn't a power of 2 (or 1)", BytesOfAlignment);
        } else {
            if ((BitB->BitsUnavailable % Bytes2Bits(BytesOfAlignment)) == 0) {
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
            uint8_t Bits2Align = BitB->BitsUnavailable % Bytes2Bits(BytesOfAlignment);
            if (Bits2Align != 0) {
                BitB->BitsAvailable   -= Bits2Align;
                BitB->BitsUnavailable += Bits2Align;
            }
        }
    }
    
    static uint8_t *ConvertBinaryUUID2UUIDString(const uint8_t *BinaryUUID) {
        uint8_t *UUIDString = NULL;
        
        // 0xe6 0x1b 0xd7 0x90 - 0xa9 0xc4 - 0x4a 0xf8 - 0xa5 0x28 - 0x5a 0xfb 0x6a 0xcd 0x27 0x1b
        
        if (BinaryUUID == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "Pointer to BinaryUUID is NULL");
        } else if (sizeof(BinaryUUID) != BitIOBinaryUUIDSize) {
            Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "BinaryUUID size should be: %d, but is: %d", BitIOBinaryUUIDSize, sizeof(BinaryUUID));
        } else {
            UUIDString = (uint8_t*) calloc(1, BitIOUUIDStringSize);
            if (UUIDString == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "Not enough memory to allocate UUIDString");
            } else {
                for (uint8_t Byte = 0; Byte < BitIOBinaryUUIDSize; Byte++) {
                    if (Byte == 4 || Byte == 7 || Byte == 10 || Byte == 13) {
                        UUIDString[Byte] = 0x2D;
                    } else if (Byte == 20) {
                        UUIDString[Byte] = 0x0;
                    } else {
                        UUIDString[Byte] = BinaryUUID[Byte];
                    }
                }
            }
        }
        return UUIDString;
    }
    
    static uint8_t *ConvertUUIDString2BinaryUUID(const uint8_t *UUIDString) {
        uint8_t *BinaryUUID = NULL;
        
        // 0xe6 0x1b 0xd7 0x90 - 0xa9 0xc4 - 0x4a 0xf8 - 0xa5 0x28 - 0x5a 0xfb 0x6a 0xcd 0x27 0x1b
        
        if (UUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "Pointer to UUIDString is NULL");
        } else if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "UUIDString size should be: %d, but is: %d", BitIOUUIDStringSize, sizeof(UUIDString));
        } else {
            BinaryUUID = (uint8_t*) calloc(1, BitIOBinaryUUIDSize);
            
            if (BinaryUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "Not enough memory to allocate BinaryUUID");
            } else {
                for (uint8_t Byte = 0; Byte < BitIOUUIDStringSize; Byte++) {
                    if (Byte != 4 && Byte != 7 && Byte != 10 && Byte != 13 && Byte != 20) {
                        BinaryUUID[Byte] = UUIDString[Byte];
                    }
                }
            }
        }
        return BinaryUUID;
    }
    
    static uint8_t *SwapBinaryUUID(const uint8_t *BinaryUUID) { // Should I make wrappers called ConvertUUID2GUID and SwapGUID2UUID?
        uint8_t *SwappedBinaryUUID = NULL;
        
        if (BinaryUUID == NULL) {
            Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "Pointer to BinaryUUID is NULL");
        } else if (sizeof(BinaryUUID) != BitIOBinaryUUIDSize) {
            Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "BinaryUUID size should be: %d, but is: %d", BitIOBinaryUUIDSize, sizeof(BinaryUUID));
        } else {
            SwappedBinaryUUID = (uint8_t*) calloc(1, BitIOBinaryUUIDSize);
            if (SwappedBinaryUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "Not enough memory to allocate SwappedBinaryUUID");
            } else {
                SwappedBinaryUUID[0]        = BinaryUUID[3];
                SwappedBinaryUUID[1]        = BinaryUUID[2];
                SwappedBinaryUUID[2]        = BinaryUUID[1];
                SwappedBinaryUUID[3]        = BinaryUUID[0];
                SwappedBinaryUUID[4]        = BinaryUUID[5];
                SwappedBinaryUUID[5]        = BinaryUUID[4];
                SwappedBinaryUUID[6]        = BinaryUUID[7];
                SwappedBinaryUUID[7]        = BinaryUUID[6];
                SwappedBinaryUUID[8]        = BinaryUUID[9];
                SwappedBinaryUUID[9]        = BinaryUUID[8];
                for (uint8_t Byte = 10; Byte < 16; Byte++) {
                    SwappedBinaryUUID[Byte] = BinaryUUID[Byte];
                }
            }
        }
        return SwappedBinaryUUID;
    }
    /*
    uint8_t *ReadUUID(BitBuffer *BitB) {
        uint8_t *UUIDString = NULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadUUID", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t *BinaryUUID = (uint8_t*) calloc(1, BitIOBinaryUUIDSize);
            
            if (BinaryUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "ReadUUID", "Not enough memory to allocate BinaryUUID");
            } else {
                for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                    BinaryUUID[UUIDByte] = ReadBits(BitB, 8, true);
                }
                UUIDString = ConvertBinaryUUID2UUIDString(BinaryUUID);
            }
        }
        return UUIDString;
    }
     */
    
    uint8_t *ConvertUUID2GUID(const uint8_t *UUIDString) {
        uint8_t *BinaryUUID = NULL;
        uint8_t *BinaryGUID = NULL;
        uint8_t *GUIDString = NULL;
        
        if (UUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertUUID2GUID", "Pointer to UUIDString is NULL");
        } else {
            GUIDString = (uint8_t*) calloc(1, BitIOGUIDStringSize);
            
            if (GUIDString == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertUUID2GUID", "Not enough memory to allocate GUIDString");
            } else {
                BinaryUUID = ConvertUUIDString2BinaryUUID(UUIDString);
                BinaryGUID = SwapBinaryUUID(BinaryUUID);
                GUIDString = ConvertBinaryUUID2UUIDString(BinaryGUID);
            }
        }
        return GUIDString;
    }
    
    uint8_t *ConvertGUID2UUID(const uint8_t *GUIDString) { // TODO: We should probably deprecate this
        uint8_t *BinaryGUID = NULL;
        uint8_t *BinaryUUID = NULL;
        uint8_t *UUIDString = NULL;
        
        if (GUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertGUID2UUID", "Pointer to GUIDString is NULL");
        } else {
            UUIDString = (uint8_t*) calloc(1, BitIOUUIDStringSize);
            
            if (UUIDString == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertGUID2UUID", "Not enough memory to allocate UUIDString");
            } else {
                BinaryGUID = ConvertUUIDString2BinaryUUID(GUIDString);
                BinaryUUID = SwapBinaryUUID(BinaryGUID);
                UUIDString = ConvertBinaryUUID2UUIDString(BinaryUUID);
            }
        }
        return UUIDString;
    }
    
    bool CompareUUIDs(const uint8_t *UUIDString1, const uint8_t *UUIDString2) {
        bool UUIDsMatch = 0;
        
        if (UUIDString1 == NULL) {
            Log(LOG_ERR, "libBitIO", "CompareUUIDs", "Pointer to UUIDString1 is NULL");
        } else if (UUIDString2 == NULL) {
            Log(LOG_ERR, "libBitIO", "CompareUUIDs", "Pointer to UUIDString2 is NULL");
        } else {
            for (uint8_t UUIDByte = 0; UUIDByte < BitIOUUIDStringSize - 1; UUIDByte++) {
                if (UUIDString1[UUIDByte] != UUIDString2[UUIDByte]) {
                    UUIDsMatch = false;
                } else if ((UUIDString1[UUIDByte] == UUIDString2[UUIDByte]) && (UUIDByte = BitIOUUIDStringSize - 1)) {
                    UUIDsMatch = true;
                }
            }
        }
        return UUIDsMatch;
    }
    
    void WriteUUID(BitBuffer *BitB, const uint8_t *UUIDString) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteUUID", "Pointer to instance of BitBuffer is NULL");
        } else if (UUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteUUID", "Pointer to UUIDString is NULL");
        } else {
            uint8_t *BinaryUUID = NULL;
            BinaryUUID = ConvertUUIDString2BinaryUUID(UUIDString);
            for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                WriteBits(BitB, BinaryUUID[UUIDByte], 8, true);
            }
        }
    }
    
    void CreateSocket(const int Domain, const int Type, const int Protocol) { // Define it in the header when it's done
        int Socket;
#ifdef _POSIX_VERSION
        Socket = socket(Domain, Type, Protocol);
#elif _WIN32
#endif
    }
    
    // So, I need to accept packets of variable size, from the network, disk, whatever.
    
    // Should BitInput and BitOutput store a value saying what type of "file" it's being written to? or just write to the file handle regardless?
    // So, the gist is we need a function to read from a file or network stream, and create a buffer from that data, and pop it into the struct.
    // Ok, so I need a function to read a file/socket into a buffer, and one to write a buffer to a file/socket.
    // I don't know if FILE pointers work with sockets, but i'm going to ignore that for now.
    
    void ReadBitInput2BitBuffer(BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read) { // It's the user's job to ensure buffers and files are kept in sync, not mine.
        uint64_t BytesRead              = 0ULL;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "BitI pointer is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "BitB pointer is NULL");
        } else if (Bytes2Read > BitI->FileSize - BitI->FilePosition) {
            Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "You tried reading more data: % than is available: %d in the file", Bytes2Read, BitI->FileSize - BitI->FilePosition);
        } else {
            if (BitB->Buffer != NULL) {
                free(BitB->Buffer);
            }
            BitB->Buffer                  = (uint8_t*) calloc(1, Bytes2Read);
            if (BitB->Buffer == NULL) {
                Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "Not enough memory to allocate BitB->Buffer");
            } else {
                BytesRead                 = fread(BitB->Buffer, 1, Bytes2Read, BitI->File);
                if (BytesRead            != Bytes2Read) {
                    Log(LOG_ERR, "libBitIO", "ReadBitInput2BitBuffer", "Fread read: %d bytes, but you requested: %d", BytesRead, Bytes2Read);
                } else {
                    BitB->BitsAvailable   = Bytes2Bits(BytesRead);
                    BitB->BitsUnavailable = 0ULL;
                }
            }
        }
    }
    
    void WriteBitBuffer2BitOutput(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write) { // FIXME Assuming we wrote the whole buffer
        uint64_t BytesWritten           = 0ULL;
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitBuffer2BitOutput", "BitI pointer is NULL");
        } else if (Buffer2Write == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBitBuffer2BitOutput", "Buffer2Write pointer is NULL");
        } else {
            // Write the bytes in BitB->Buffer to BitO->File
            BytesWritten              = fwrite(Buffer2Write->Buffer, 1, Bytes2Write, BitO->File);
            if (BytesWritten         != Bytes2Write) {
                Log(LOG_ERR, "libBitIO", "WriteBitBuffer2BitOutput", "Fwrite wrote: %d bytes, but you requested: %d", BytesWritten, Bytes2Write);
            } else {
                Buffer2Write->BitsAvailable   = Bytes2Bits(BytesWritten);
                Buffer2Write->BitsUnavailable = 0ULL;
            }
        }
    }
    
    void ReadSocket2Buffer(BitInput *BitI, const uint64_t Bytes2Read) { // FIXME: Just a stub
        // Define it in the header when it's done
        //sockaddr_in
    }
    
    void WriteBuffer2Socket(BitOutput *BitO, BitBuffer *BitB, const int Socket) { // FIXME: Just a stub
        // Define it in the header when it's done
        
    }
    
    void Log(const uint8_t ErrorSeverity, const char *LibraryOrProgram, const char *FunctionName, const char *Description, ...) {
        uint64_t FunctionNameSize = 0ULL, DescriptionSize = 0ULL;
        
        char *EasyString = calloc(1, 1 + strlen(FunctionName) + strlen(Description) + 2); // the 1 is for the error severity + 2 for the NULs
        snprintf(EasyString, FunctionNameSize + DescriptionSize, "%hhu: %s - %s", ErrorSeverity, FunctionName, Description);
        
        char *HardString = calloc(1, BitIOStringSize); // FIXME: REMOVE BITIOSTRINGSIZE
        va_list Arguments;
        va_start(Arguments, Description);
        vsprintf(HardString, "%s", Arguments);
        va_end(Arguments);
        
        uint64_t ErrorStringSize = strlen(EasyString) + strlen(HardString) + 2;
        char *ErrorString = calloc(1, ErrorStringSize);
        snprintf(ErrorString, ErrorStringSize, "%s%s", EasyString, HardString);
        if (BitIOGlobalLogFile == NULL) {
            // Set STDERR As the output file
            fprintf(stderr, "%s%s", ErrorString, BitIONewLine);
        } else {
            // Use BitO->LogFile as the output file
            fprintf(BitIOGlobalLogFile, "%s%s", ErrorString, BitIONewLine);
        }
    }
    
#ifdef __cplusplus
}
#endif
