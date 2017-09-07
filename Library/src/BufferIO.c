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
        bool               IsFileOrSocket;
        int                Socket;
        fpos_t             FileSize;
        fpos_t             FilePosition;
        uint64_t           FileSpecifierNum;
    };
    
    struct BitOutput {
        FILE              *File;
        bool               IsFileOrSocket;
        int                Socket;
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
    
    static inline uint8_t Power2MaskLSByte(const uint8_t Exponent) {
        return (pow(2, Exponent) - 1);
    }
    
    static inline uint8_t Power2MaskMSByte(const uint8_t Exponent) {
        return (((uint8_t) pow(2, Exponent) - 1) << (8 - Exponent));
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
            snprintf(NewPath, Path2OpenSize, "%s", Path2Open, BitI->FileSpecifierNum += 1); // FIXME: HANDLE FORMAT STRINGS BETTER
            BitI->FileSpecifierNum += 1;
            if (OpenForReadWrite == true) {
                BitI->File = fopen(Path2Open, "rb+");
            } else {
                BitI->File = fopen(Path2Open, "rb");
            }
            if (BitI->File == NULL) {
                Log(LOG_ERR, "libBitIO", "OpenInputFile", "Couldn't open file: Check that the file exists and the permissions are correct");
            }
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
            snprintf(NewPath, Path2OpenSize, "%s", Path2Open, BitO->FileSpecifierNum += 1); // FIXME: HANDLE FORMAT STRINGS BETTER
            
            BitO->File = fopen(Path2Open, "wb");
            if (BitO->File == NULL) {
                Log(LOG_ALERT, "libBitIO", "OpenOutputFile", "Couldn't open output file; Check that the path exists and the permissions are correct");
            }
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
    
    void SkipBits(BitBuffer *BitB, const int64_t Bits2Skip) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "SkipBits", "Pointer to BitBuffer is NULL");
        } else {
            if (Bits2Skip < 0) {
                BitB->BitOffset -= Bits2Skip;
            } else {
                BitB->BitOffset += Bits2Skip;
            }
        }
    }
    
    bool IsBitBufferAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        bool AlignmentStatus = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "IsBitBufferAligned", "Pointer to BitBuffer is NULL");
        } else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
            Log(LOG_ERR, "libBitIO", "IsBitBufferAligned", "BytesOfAlignment: %d isn't an integer power of 2", BytesOfAlignment);
        } else {
            if ((BitB->BitOffset % Bytes2Bits(BytesOfAlignment)) == 0) {
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
            uint8_t Bits2Align = BitB->BitOffset % Bytes2Bits(BytesOfAlignment);
            if (Bits2Align != 0) {
                BitB->BitOffset       += Bytes2Bits(BytesOfAlignment);
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
            UUIDString = calloc(1, BitIOUUIDStringSize);
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
    
    uint8_t *ReadBinaryUUID(BitBuffer *BitB) {
        uint8_t *UUIDString = NULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBinaryUUID", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t *BinaryUUID = calloc(1, BitIOBinaryUUIDSize);
            if (BinaryUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "ReadBinaryUUID", "Not enough memory to allocate BinaryUUID");
            } else {
                for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                    BinaryUUID[UUIDByte] = ReadBits(BitB, 8, true);
                }
                UUIDString = ConvertBinaryUUID2UUIDString(BinaryUUID);
            }
        }
        return UUIDString;
    }
    
    uint8_t *ReadUUIDString(BitBuffer *BitB) {
        uint8_t *UUIDString = NULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadUUIDString", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t *BinaryUUID = calloc(1, BitIOBinaryUUIDSize);
            
            if (BinaryUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "ReadUUIDString", "Not enough memory to allocate BinaryUUID");
            } else {
                for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                    BinaryUUID[UUIDByte] = ReadBits(BitB, 8, true);
                }
                UUIDString = ConvertBinaryUUID2UUIDString(BinaryUUID);
            }
        }
        return UUIDString;
    }
    
    uint8_t *ReadBinaryGUID(BitBuffer *BitB) {
        uint8_t *UUIDString = NULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBinaryGUID", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t *BinaryUUID = calloc(1, BitIOBinaryUUIDSize);
            
            if (BinaryUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "ReadBinaryGUID", "Not enough memory to allocate BinaryUUID");
            } else {
                for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                    BinaryUUID[UUIDByte] = ReadBits(BitB, 8, true);
                }
                UUIDString = ConvertBinaryUUID2UUIDString(BinaryUUID);
            }
        }
        return UUIDString;
    }
    
    uint8_t *ReadGUIDString(BitBuffer *BitB) {
        uint8_t *UUIDString = NULL;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadGUIDString", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t *BinaryUUID = calloc(1, BitIOBinaryUUIDSize);
            
            if (BinaryUUID == NULL) {
                Log(LOG_ERR, "libBitIO", "ReadGUIDString", "Not enough memory to allocate BinaryUUID");
            } else {
                for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                    BinaryUUID[UUIDByte] = ReadBits(BitB, 8, true);
                }
                UUIDString = ConvertBinaryUUID2UUIDString(BinaryUUID);
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
            BinaryUUID = calloc(1, BitIOBinaryUUIDSize);
            
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
            SwappedBinaryUUID = calloc(1, BitIOBinaryUUIDSize);
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
    
    uint8_t *ConvertUUIDString2GUIDString(const uint8_t *UUIDString) {
        uint8_t *BinaryUUID = NULL;
        uint8_t *BinaryGUID = NULL;
        uint8_t *GUIDString = NULL;
        
        if (UUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertUUIDString2GUIDString", "Pointer to UUIDString is NULL");
        } else {
            GUIDString = calloc(1, BitIOGUIDStringSize);
            
            if (GUIDString == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertUUIDString2GUIDString", "Not enough memory to allocate GUIDString");
            } else {
                BinaryUUID = ConvertUUIDString2BinaryUUID(UUIDString);
                BinaryGUID = SwapBinaryUUID(BinaryUUID);
                GUIDString = ConvertBinaryUUID2UUIDString(BinaryGUID);
            }
        }
        return GUIDString;
    }
    
    uint8_t *ConvertGUIDString2UUIDString(const uint8_t *GUIDString) {
        uint8_t *BinaryGUID = NULL;
        uint8_t *BinaryUUID = NULL;
        uint8_t *UUIDString = NULL;
        
        if (GUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertGUIDString2UUIDString", "Pointer to GUIDString is NULL");
        } else {
            UUIDString = calloc(1, BitIOUUIDStringSize);
            
            if (UUIDString == NULL) {
                Log(LOG_ERR, "libBitIO", "ConvertGUIDString2UUIDString", "Not enough memory to allocate UUIDString");
            } else {
                BinaryGUID = ConvertUUIDString2BinaryUUID(GUIDString);
                BinaryUUID = SwapBinaryUUID(BinaryGUID);
                UUIDString = ConvertBinaryUUID2UUIDString(BinaryUUID);
            }
        }
        return UUIDString;
    }
    
    bool CompareUUIDStrings(const uint8_t *UUIDString1, const uint8_t *UUIDString2) {
        bool UUIDsMatch = 0;
        
        if (UUIDString1 == NULL) {
            Log(LOG_ERR, "libBitIO", "CompareUUIDStrings", "Pointer to UUIDString1 is NULL");
        } else if (UUIDString2 == NULL) {
            Log(LOG_ERR, "libBitIO", "CompareUUIDStrings", "Pointer to UUIDString2 is NULL");
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
            Log(LOG_ERR, "libBitIO", "WriteUUID", "Pointer to BitBuffer is NULL");
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
        } else if (Bytes2Read > (BitI->FileSize - BitI->FilePosition)) {
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
            // Write the bytes in BitB->Buffer to BitO->File
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
    
    void Log(const uint8_t ErrorSeverity, const char *LibraryOrProgram, const char *FunctionName, const char *Description, ...) {
        int   EasyStringSize = strlen(LibraryOrProgram) + strlen(FunctionName) + strlen(Description) + 1; // Plus 1 for the terminating NULL
        char *EasyString     = calloc(1, EasyStringSize); // the 1 is for the error severity + 2 for the NULs
        snprintf(EasyString, EasyStringSize, "%hhu: %s - %s", ErrorSeverity, FunctionName, Description);
        
        va_list Arguments;
        va_start(Arguments, Description);
        int HardStringSize = 0;
        char *HardString = NULL;
        HardStringSize = vsnprintf(NULL, 0, "%s", Arguments);
        va_end(Arguments);
        
        uint64_t ErrorStringSize = EasyStringSize + HardStringSize + BitIONewLineSize;
        char *ErrorString = calloc(1, ErrorStringSize);
        snprintf(ErrorString, ErrorStringSize, "%s%s%s", EasyString, HardString, BitIONewLine);
        if (BitIOGlobalLogFile == NULL) {
            // Set STDERR As the output file
            fprintf(stderr, "%s", ErrorString);
        } else {
            // Use BitO->LogFile as the output file
            fprintf(BitIOGlobalLogFile, "%s", ErrorString);
        }
    }
    
#ifdef __cplusplus
}
#endif
