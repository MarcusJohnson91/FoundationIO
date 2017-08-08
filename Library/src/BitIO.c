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
    extern FILE           *BitIOGlobalLogFile; // ONLY global variable in all of BitIO
    
    typedef struct BitBuffer {
        uint64_t           BitsAvailable;
        uint64_t           BitsUnavailable;
        uint8_t           *Buffer;
    } BitBuffer;
    
    typedef struct BitInput {
        FILE              *File;
        bool               IsFileOrSocket;
        int                Socket;
        uint64_t           FileSize;
        uint64_t           FilePosition;
        uint64_t           FileSpecifierNum;
        uint8_t            SystemEndian;
    } BitInput;
    
    typedef struct BitOutput {
        FILE              *File;
        bool               IsFileOrSocket;
        int                Socket;
        uint64_t           FilePosition;
        uint64_t           FileSpecifierNum;
        uint8_t            SystemEndian;
    } BitOutput;
    
    typedef struct SymbolFrequencies { // We assume that all the symbols are unsigned integers of varying size
        uint64_t           NumSymbols; // Probability is just the frequency of occurance divided by the number of symbols.
        uint8_t            SymbolSize; // number of bytes per symbol
        void              *Frequency;
    } SymbolFrequencies;
    
    typedef struct HuffmanNode {
        int64_t            LeftHuffmanCode;
        int64_t            RightHuffmanCode;
        int64_t           *LeftNode;
        int64_t           *RightNode;
    } HuffmanNode;
    
    typedef struct HuffmanTree {
        uint64_t           NumNodes;
        HuffmanNode       *Node;
        uint64_t          *SymbolFrequency;
        bool               TableIsUsable;
        const uint8_t     *Table;
    } HuffmanTree;
    
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
    
    uint16_t SwapEndian16(const uint16_t Data2Swap) {
        return ((Data2Swap & 0xFF00) >> 8) | ((Data2Swap & 0x00FF) << 8);
    }
    
    uint32_t SwapEndian32(const uint32_t Data2Swap) {
        return ((Data2Swap & 0xFF000000) >> 24) | ((Data2Swap & 0x00FF0000) >> 8) | ((Data2Swap & 0x0000FF00) << 8) | ((Data2Swap & 0x000000FF) << 24);
    }
    
    uint64_t SwapEndian64(const uint64_t Data2Swap) {
        return (((Data2Swap & 0xFF00000000000000) >> 56) | ((Data2Swap & 0x00FF000000000000) >> 40) | \
                ((Data2Swap & 0x0000FF0000000000) >> 24) | ((Data2Swap & 0x000000FF00000000) >>  8) | \
                ((Data2Swap & 0x00000000FF000000) <<  8) | ((Data2Swap & 0x0000000000FF0000) << 24) | \
                ((Data2Swap & 0x000000000000FF00) << 40) | ((Data2Swap & 0x00000000000000FF) << 56));
    }
    
    int64_t Bytes2Bits(const int64_t Bytes) {
        return Bytes * 8;
    }
    
    int64_t Bits2Bytes(const int64_t Bits, const bool RoundUp) {
        if (RoundUp == true) {
            return (Bits / 8) + (8 - (Bits % 8));
        } else {
            return Bits / 8;
        }
    }
    
    uint64_t NumBits2ReadSymbols(const uint64_t NumSymbols) {
        // Use a binary logarithm, that you round up, in order to get the number of bits required to read a certain number of symbols.
        return ceil(log2(NumSymbols));
    }
    
    uint8_t CountBitsSet(const uint64_t Bits2Count) {
        uint8_t DataBit = 0, BitCount = 0;
        for (uint8_t Bit = 0; Bit < 64; Bit++) {
            DataBit = (Bits2Count & (1 << Bit)) >> Bit;
            if (DataBit == 1) {
                BitCount += 1;
            }
        }
        return BitCount;
    }
    
    uint64_t Power2Mask(const uint8_t Exponent) {
        uint64_t Mask = 0ULL;
        if (Exponent > 64) {
            Log(LOG_ERR, "libBitIO", "Power2Mask", "Exponent %d is too large\n", Exponent);
        } else {
            uint64_t HighestBit = 0ULL, Fill = 0ULL;
            HighestBit          = (uint64_t)pow(2, Exponent - 1);
            Fill                = HighestBit - 1;
            Mask                = HighestBit + Fill;
        }
        return Mask;
    }
    
    int64_t OnesCompliment2TwosCompliment(const int64_t OnesCompliment) {
        return ~OnesCompliment + 1;
    }
    
    int64_t TwosCompliment2OnesCompliment(const int64_t TwosCompliment) {
        return TwosCompliment ^ 0xFFFFFFFFFFFFFFFF;
    }
    
    bool IsOdd(const int64_t Number2Check) {
        return Number2Check % 2 == 0 ? true : false;
    }
    
    uint8_t FindHighestBitSet(const uint64_t UnsignedInt2Search) { // UnsignedInt2Search = 0x8000
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
    
    uint64_t BytesRemainingInInputFile(const BitInput *BitI) {
        uint64_t BytesLeft = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "BytesRemainingInInputFile", "Pointer to BitInput is NULL");
        } else {
            BytesLeft = BitI->FileSize - BitI->FilePosition;
        }
        return BytesLeft;
    }
    
    uint64_t GetBitInputFileSize(const BitInput *BitI) {
        uint64_t InputSize = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFileSize", "Pointer to BitInput is NULL");
        } else {
            InputSize = BitI->FileSize;
        }
        return InputSize;
    }
    
    uint64_t GetBitInputFilePosition(const BitInput *BitI) {
        uint64_t Position = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFileSize", "Pointer to BitInput is NULL");
        } else {
            Position = BitI->FilePosition;
        }
        return Position;
    }
    
    uint64_t GetBitBufferPosition(const BitBuffer *BitB) {
        uint64_t Position = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFileSize", "Pointer to BitInput is NULL");
        } else {
            Position = BitB->BitsUnavailable;
        }
        return Position;
    }
    
    uint64_t GetBitBufferSize(const BitBuffer *BitB) {
        uint64_t BitBufferSize = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitBufferSize", "Pointer to BitBuffer is NULL");
        } else {
            BitBufferSize = sizeof(BitB->Buffer);
        }
        return BitBufferSize;
    }
    
    uint8_t GetBitInputSystemEndian(const BitInput *BitI) {
        uint8_t Endian = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputSystemEndian", "Pointer to BitInput is NULL");
        } else {
            Endian = BitI->SystemEndian;
        }
        return Endian;
    }
    
    uint8_t GetBitOutputSystemEndian(const BitOutput *BitO) {
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
            
            uint64_t Path2OpenSize = strlen(Path2Open);
            char *NewPath = (char*) calloc(1, Path2OpenSize);
            snprintf(NewPath, Path2OpenSize, "%s", Path2Open, BitI->FileSpecifierNum + 1);
            BitI->FileSpecifierNum += 1;
            
            if (OpenForReadWrite == true) {
                BitI->File = fopen(Path2Open, "rb+");
            } else {
                BitI->File = fopen(Path2Open, "rb");
            }
            
            if (BitI->File == NULL) {
                Log(LOG_ERR, "libBitIO", "OpenInputFile", "BitI->File error: Pointer to File is NULL");
            } else {
                fseek(BitI->File, 0, SEEK_END);
                BitI->FileSize     = (uint64_t) ftell(BitI->File);
                fseek(BitI->File, 0, SEEK_SET);
                BitI->FilePosition = (uint64_t) ftell(BitI->File);
                BitI->SystemEndian = DetectSystemEndian();
            }
        }
    }
    
    void OpenOutputFile(BitOutput *BitO, const char *Path2Open) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenOutputFile", "Pointer to BitOutput is NULL");
        } else if (Path2Open == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenOutputFile", "Fopen error: Pointer is NULL");
        } else {
            uint64_t Path2OpenSize = strlen(Path2Open);
            char *NewPath = (char*) calloc(1, Path2OpenSize);
            snprintf(NewPath, Path2OpenSize, "%s", Path2Open, BitO->FileSpecifierNum);
            
            BitO->File = fopen(Path2Open, "wb");
            if (BitO->File == NULL) {
                Log(LOG_ALERT, "libBitIO", "OpenOutputFile", "Failed opening Path2Open");
            }
            BitO->SystemEndian          = DetectSystemEndian();
        }
    }
    
    void OpenLogFile(const char *LogFilePath) {
        if (LogFilePath == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenLogFile", "LogFilePath is NULL");
        } else {
            BitIOGlobalLogFile = fopen(LogFilePath, "a+");
        }
    }
    
    void OpenInputSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenInputSocket", "Pointer to BitInput is NULL");
        } else {
#ifdef _WIN32
#else
            BitI->Socket         = socket(Domain, Type, Protocol);
            BitI->IsFileOrSocket = &socket;
#endif
        }
    }
    
    void OpenOutputSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenInputSocket", "Pointer to BitInput is NULL");
        } else {
#ifdef _WIN32
#else
            BitO->Socket         = socket(Domain, Type, Protocol);
            BitO->IsFileOrSocket = &socket;
#endif
        }
    }
    
    void ConnectInputSocket(BitInput *BitI) {
        
    }
    
    void ConnectOutputSocket(BitOutput *BitO) {
        
    }
    
    uint64_t ReadBits(BitBuffer *BitB, const uint8_t Bits2Read, const bool ReadFromMSB) {
        uint8_t Bits = Bits2Read, UserRequestBits = 0, BufferBitsAvailable = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0;
        
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
                    Mask            = (Power2Mask(Bits) << Mask2Shift);
                } else {
                    Mask            = (Power2Mask(Bits) - 1) << BitB->BitsUnavailable % 8;
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
    
    uint64_t  ReadRICE(const BitBuffer *BitB, const bool Truncated, const bool StopBit) {
        uint64_t BitCount = 0;
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
    
    int64_t ReadExpGolomb(const BitBuffer *BitB, const bool IsSigned) {
        int64_t  Final = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadExpGolomb", "Pointer to BitBuffer is NULL");
        } else {
            uint64_t Zeros   = 0;
            uint64_t CodeNum = 0;
            
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
    
    void SkipBits(BitBuffer *BitB, const int64_t Bits2Skip) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "SkipBits", "Pointer to BitBuffer is NULL");
        } else {
            BitB->BitsAvailable   -= Bits2Skip;
            BitB->BitsUnavailable += Bits2Skip;
            // The file/stream updating functions need to keep this in mind.
        }
    }
    
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
                Mask                   = Power2Mask(Bits2Write2BufferByte) << Bits2ShiftMask;
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
            for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
                WriteBits(BitB, (~StopBit), 1, WriteFromMSB);
            }
            WriteBits(BitB, StopBit, 1, WriteFromMSB);
        }
    }
    
    void WriteExpGolomb(BitBuffer *BitB, const bool IsSigned, const uint64_t Data2Write, const bool WriteFromMSB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteExpGolomb", "Pointer to BitOutput is NULL");
        } else {
            uint64_t NumBits = 0;
            
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
    
    bool IsBitBufferAligned(const BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        bool AlignmentStatus = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "IsOutputStreamByteAligned", "Pointer to BitBuffer is NULL");
        } else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
            Log(LOG_ERR, "libBitIO", "IsOutputStreamByteAligned", "BytesOfAlignment: %d isn't a power of 2 (or 1)", BytesOfAlignment);
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
    
    void EncodeRABS(BitBuffer *Data2Encode, BitBuffer *EncodedData, const uint64_t NumSymbols, const uint64_t Probabilities) {
        // Encoded symbols need to be written from the last to the beginning of the buffer, so we should do it backwards, so WriteBits can work unmodified
        // Aka, read from the last element in the unencoded buffer, and write to the first element in the buffer
        
        // So, for each buffer we re-measuere the probabilities of the symbols.
        
        if (Data2Encode == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeRABS", "Data2Encode pointer is NULL");
        } else if (EncodedData == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeRABS", "EncodedData pointer is NULL");
        } else {
            
        }
    }
    
    uint64_t GenerateCRC(const uint8_t *Data2CRC, const uint64_t DataSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit) {
        if (PolySize % 8 != 0) {
            // Ok, so we also need to add the ability to do incremental CRC generation for the iDAT/fDAT chunks in PNG
            
            
            // You have to do it bitwise
        } else if (DataSize % PolySize || DataSize > PolySize) {
            // do it word wise aka grab PolySize bits from Data2CRC at once
        } else {
            // Do it byte-wise
        }
        /*
         uint16_t CRCResult = 0;
         for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
         CRCResult = ReciprocalPoly ^ Data2CRC[Byte] << 8;
         for (uint8_t Bit = 0; Bit < 8; Bit++) {
         if ((CRCResult & 0x8000) == true) {
         } else {
         CRCResult <<= 1;
         }
         }
         }
         */
        return 0;
    }
    
    /*
     
     So, the CRC generator need to stream input, which means it needs to be aligned to the CRC size, so 16 bit alignment for CRC16, 32 for CRC32, etc.
     
     The real probelem is in the CRC function being called multiple times, or it handling it's own input and output itself...
     
     Out of those 2 choices, it being called multiple times is a simplier option.
     
     So, if that's the case, we should have a Previous CRC argument, and a bool saying if this has been repeated before?
     
     */
    
    void GenerateCRCFromStream(BitBuffer *Buffer2CRC, uint64_t Poly, uint8_t PolySize, uint64_t PolyInit, uint64_t PreviousCRC) {
        // I'm just going to do it bitwise
        if (PreviousCRC == 0) { // assume that this is the first chunk of the stream
            
        } else {
            
        }
    }
    
    bool VerifyCRC(const uint8_t *Data2CRC, const uint64_t Data2CRCSize, const uint64_t RecipricalPoly, const uint8_t PolySize, const uint64_t PolyInit, const uint64_t PrecomputedCRC) {
        return false;
    }
    
    uint32_t GenerateAdler32(const uint8_t *Data, const uint64_t DataSize) {
        uint32_t Adler  = 1;
        uint32_t Sum1   = Adler & 0xFFFF;
        uint32_t Sum2   = (Adler >> 16) & 0xFFFF;
        
        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            Sum1 += Data[Byte] % 65521;
            Sum2 += Sum1 % 65521;
        }
        return (Sum2 << 16) + Sum1;
    }
    
    bool VerifyAdler32(const uint8_t *Data, const uint64_t DataSize, const uint32_t EmbeddedAdler32) {
        uint32_t GeneratedAdler32 = GenerateAdler32(Data, DataSize);
        if (GeneratedAdler32 != EmbeddedAdler32) {
            return false;
        } else {
            return true;
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
    
    void WriteUUID(const BitBuffer *BitB, const uint8_t *UUIDString) {
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
#ifndef _WIN32
        Socket = socket(Domain, Type, Protocol);
#elif _WIN32
        
#endif
    }
    
    // So, I need to accept packets of variable size, from the network, disk, whatever.
    
    // Should BitInput and BitOutput store a value saying what type of "file" it's being written to? or just write to the file handle regardless?
    // So, the gist is we need a function to read from a file or network stream, and create a buffer from that data, and pop it into the struct.
    // Ok, so I need a function to read a file/socket into a buffer, and one to write a buffer to a file/socket.
    // I don't know if FILE pointers work with sockets, but i'm going to ignore that for now.
    
    void ReadBitInput2BitBuffer(const BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read) { // It's the user's job to ensure buffers and files are kept in sync, not mine.
        uint64_t BytesRead              = 0;
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
                    BitB->BitsUnavailable = 0;
                }
            }
        }
    }
    
    void WriteBitBuffer2BitOutput(const BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write) { // FIXME Assuming we wrote the whole buffer
        uint64_t BytesWritten           = 0;
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
                Buffer2Write->BitsUnavailable = 0;
            }
        }
    }
    
    void ReadSocket2Buffer(const BitInput *BitI, const uint64_t Bytes2Read) { // FIXME: Just a stub
        // Define it in the header when it's done
        //sockaddr_in
    }
    
    void WriteBuffer2Socket(const BitOutput *BitO, const BitBuffer *BitB, const int Socket) { // FIXME: Just a stub
        // Define it in the header when it's done
        
    }
    
    void EncodeRLE(BitBuffer *Data2Encode, BitBuffer *Encoded, uint8_t LengthCodeSize, uint8_t SymbolSize, bool IsReversed) {
        uint64_t CurrentSymbol = 0;
        uint64_t LengthSymbol  = 0;
        
        if (IsReversed == true) { // then the symbol is first and the length is second
            CurrentSymbol = ReadBits(Data2Encode, SymbolSize, true);
        }
    }
    
    void DecodeRLE(const BitBuffer *Data2Decode, BitBuffer *Decoded, const uint8_t LengthCodeSize, const uint8_t SymbolSize) {
        if (Data2Decode == NULL) {
            Log(LOG_ERR, "libBitIO", "DecodeRLE", "Pointer to Data2Decode is NULL");
        } else if (Decoded == NULL) {
            Log(LOG_ERR, "libBitIO", "DecodeRLE", "Pointer to Decoded is NULL");
        } else if (LengthCodeSize < 1) {
            Log(LOG_ERR, "libBitIO", "DecodeRLE", "LengthCodeSize %d is invalid", LengthCodeSize);
        } else if (SymbolSize < 1) {
            Log(LOG_ERR, "libBitIO", "DecodeRLE", "SymbolSize %d is invalid", SymbolSize);
        } else {
            uint64_t  CurrentSymbol = 0ULL;
            uint64_t  CurrentLength = 0ULL;
            uint64_t *OutputString  = NULL;
            for (uint64_t D2DSymbol = 0; D2DSymbol < Data2Decode->BitsUnavailable / SymbolSize; D2DSymbol++) { // FIXME: BitsUnavailable / SymbolSize needs some work
                CurrentSymbol = ReadBits(Data2Decode, SymbolSize, true);
                CurrentLength = ReadBits(Data2Decode, LengthCodeSize, true);
                // Now we simply need to create CurrentLength copies of CurrentSymbol, and append them to Decoded
                OutputString = calloc(CurrentLength, Bits2Bytes(SymbolSize, true));
                for (uint64_t OutputStringSymbol = 0; OutputStringSymbol < CurrentLength; OutputStringSymbol++) {
                    OutputString[OutputStringSymbol] = CurrentSymbol;
                }
                WriteBits(Decoded, *OutputString, SymbolSize, true);
            }
        }
    }
    
    /*
     Huffman, Arthimetic, and ANS/ABS coding systems ALL require having a sorted list of symbol and their frequency (to keep it in the int domain)
     So writing a sorting algorithm is going to be the first thing i do, and I'm not going to fuck around with crazy sorters, just a real simple one that should optimize better.
     */
    uint64_t *MeasureSortSymbolFrequency(const uint16_t *Buffer2Measure, const uint64_t BufferSizeInElements, const uint8_t ElementSizeInBytes) {
        uint64_t *SymbolFrequencies = NULL, *FrequencyPosition = NULL;
        // This is MeasureSymbolFrequency + sorting as we go.
        if (Buffer2Measure == NULL) {
            Log(LOG_ERR, "libBitIO", "MeasureSortSymbolFrequency", "Buffer2Measure is NULL");
        } else {
            SymbolFrequencies = (uint64_t*) calloc(1, BufferSizeInElements);
            if (SymbolFrequencies == NULL) {
                Log(LOG_ERR, "libBitIO", "MeasureSortSymbolFrequency", "Not enough memory to allocate SymbolFrequencies %d", BufferSizeInElements);
            }
            FrequencyPosition = (uint64_t*) calloc(1, BufferSizeInElements);
            if (FrequencyPosition == NULL) {
                Log(LOG_ERR, "libBitIO", "MeasureSortSymbolFrequency", "Not enough memory to allocate FrequencyPosition %d", BufferSizeInElements);
            }
            for (uint64_t Element = 0; Element < BufferSizeInElements; Element++) {
                SymbolFrequencies[Buffer2Measure[Element]] += 1; // The index is the symbol
                uint64_t Frequency = SymbolFrequencies[Buffer2Measure[Element]];
                while (SymbolFrequencies[Buffer2Measure[Element]] != Frequency) {
                    FrequencyPosition[SymbolFrequencies[Buffer2Measure[Element]]] += 1;
                }
            }
        }
        free(FrequencyPosition);
        return SymbolFrequencies;
    }
    
    /* Deflate (encode deflate) */
    void ParseDeflateHeader(const BitBuffer *DeflatedData) {
        if (DeflatedData == NULL) {
            Log(LOG_ERR, "libBitIO", "ParseDeflateHeader", "BitBuffer pointer is NULL");
        } else {
            // stored in big endian byte order, bits are stored LSB first
            uint8_t CompressionMethod  = ReadBits(DeflatedData, 4, true); // 8 = DEFLATE
            uint8_t CompressionInfo    = ReadBits(DeflatedData, 4, true); // 7 = LZ77 window size 32k
            uint8_t CheckCode          = ReadBits(DeflatedData, 5, true); // 1, for the previous 2 fields, MUST be multiple of 31
            bool    DictionaryPresent  = ReadBits(DeflatedData, 1, true); //
            uint8_t CompressionLevel   = ReadBits(DeflatedData, 2, true); // 0
            if (DictionaryPresent == true) {
                uint32_t DictionaryID  = ReadBits(DeflatedData, 32, true); // 0xEDC1
            }
            if (CompressionMethod == 8) {
                //ParseDeflateBlock(DeflatedData, BlockSize[CompressionInfo]);
            } else {
                Log(LOG_ERR, "libBitIO", "ParseDeflateHeader", "Invalid DEFLATE compression method %d", CompressionMethod);
            }
        }
    }
    
    void DecodeHuffman(const BitBuffer *BitB, uint64_t HuffmanSize) {
        // 3 alphabets, literal, "alphabet of bytes", or <length 8, distance 15> the first 2 are combined, 0-255 = literal, 256 = End of Block, 257-285 = length
        // FIXME: The Tilde ~ symbol is the negation symbol in C!!!!! XOR = ^
        
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "DecodeHuffman", "BitBuffer pointer is NULL");
        } else {
            uint8_t  DecodedData[32768]     = {0};
            bool     IsLastHuffmanBlock     = ReadBits(BitB, 1, true);
            uint8_t  HuffmanCompressionType = ReadBits(BitB, 2, true); // 0 = none, 1 = fixed, 2 = dynamic, 3 = invalid.
            uint32_t DataLength             = 0;
            uint32_t OnesComplimentOfLength = 0; // Ones Compliment of DataLength
            
            if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != HuffmanSize) { // Make sure the numbers match up
                Log(LOG_WARNING, "libBitIO", "DecodeHuffman", "One's Compliment of Length: %d != Length %d", OnesComplimentOfLength, DataLength);
            }
            
            if (IsLastHuffmanBlock == true) {
                
            }
            
            if (HuffmanCompressionType == 0) { // No compression.
                AlignBitBuffer(BitB, 1); // Skip the rest of the current byte
                DataLength             = ReadBits(BitB, 32, true);
                OnesComplimentOfLength = ReadBits(BitB, 32, true);
                if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != DataLength) {
                    // Exit because there's an issue.
                }
                for (uint32_t Byte = 0; Byte < DataLength; Byte++) {
                    DecodedData[Byte] = ReadBits(BitB, 8, true);
                }
            } else if (HuffmanCompressionType == 1) { // Static Huffman.
                uint8_t  Length   = ReadBits(BitB, 8, true) - 254;
                uint16_t Distance = ReadBits(BitB, 5, true);
                
            } else if (HuffmanCompressionType == 2) { // Dynamic Huffman.
                /*
                 Huff->Dynamic->Length     = ReadBits(BitI, 5) + 257;
                 Huff->Dynamic->Distance   = ReadBits(BitI, 5) + 1;
                 Huff->Dynamic->CodeLength = ReadBits(BitI, 4) + 4;
                 */
            } else { // Invalid.
                     // Reject the stream.
            }
            /*
             if compressed with dynamic Huffman codes
             read representation of code trees (see
             subsection below)
             loop (until end of block code recognized)
             decode literal/length value from input stream
             if value < 256
             copy value (literal byte) to output stream
             otherwise
             if value = end of block (256)
             break from loop
             otherwise (value = 257..285)
             decode distance from input stream
             
             move backwards distance bytes in the output
             stream, and copy length bytes from this
             position to the output stream.
             end loop
             while not last block
             /
             }
             */
        }
    }
    
    uint64_t GetHuffmanCode(HuffmanTree *Tree, int64_t **SymbolsAndProbabilities, int64_t Symbol, uint64_t NumSymbols) {
        if (Tree == NULL) {
            Log(LOG_ERR, "libBitIO", "GetHuffmanCode", "Pointer to HuffmanTree is NULL");
        } else if (SymbolsAndProbabilities == NULL) {
            Log(LOG_ERR, "libBitIO", "GetHuffmanCode", "Pointer to SymbolsAndProbabilities is NULL");
        } else {
            uint64_t SymbolPlace = 0;
            for (uint64_t Index = 0; Index < NumSymbols; Index++) {
                // Find the probability of Symbol, then build the actual huffman code from where in the tree that symbol was assigned
                
                // No, then you'd have to do 2 lookups per symbol instead of just one.
                // So, we should just take in a Symbol to find, and the tree to find it in
                // But, to do that, we need to store the symbol with the probability, also we should think about building a table from this tree and hitting that first?
                
                // No, we should simply do a search through all the nodes, and when we find the symbol, create the Huffman code.
                // Also, tree traversal for each fucking code will be slow should we just store the code alongside the symbol to speed up Decoding/Encoding?
                
                // Honestly, you have to build a table to embed in the PNG anyway, so I might as well store the table in the tree pointer.
                // Now, I just need to write a function to build a tree from the data, and include a bool so it knows when to use the tree.
                
                
                
                
                
                // Search through the nodes by finding the probability of the submitted symbol, then go down the tree tracing that step.
                // Lets say E is the symbol we're supposed to find, which is the fifth highest probability overall.
                // We go to the root node, which has a 0. then the right since we're in position 1, not 5.
                // the right branch of the root node has value 1, we go to the right node again since we're only as position 2.
                // the code so far is 01, we're at the second right node, and we need to go right again. the code is now 011
                // we're at position 4, so we need to go down 1 more node. the code is currently 0111
                // since we're at position 5, we need to go left? the code is now 01110
                // the Huffman code for the symbol E, is 0b01110, or 0xE (LOL HOLY SHIT WHA	T A COINCIDENCE!)
            }
        }
        return 0;
    }
    
    HuffmanTree *BuildHuffmanTree(HuffmanTree *Tree2Build, int64_t **SymbolsAndProbabilities, uint64_t NumSymbols) {
        if (Tree2Build == NULL) {
            Log(LOG_ERR, "libBitIO", "BuildHuffmanTree", "Pointer to HuffmanTree is NULL");
        } else if (SymbolsAndProbabilities == NULL) {
            Log(LOG_ERR, "libBitIO", "BuildHuffmanTree", "Pointer to SymbolsAndProbabilities is NULL");
        } else {
            // Well start by taking the lowest probability pair (the bottom 2 regardless of value) symbols, and assigning them to nodes.
            Tree2Build->NumNodes = NumSymbols / 2;
            
            for (uint64_t Symbol = NumSymbols; Symbol > 0; Symbol -= 2) { // What do we do if the number of symbols is uneven?
                Tree2Build->Node[0].LeftNode  = SymbolsAndProbabilities[Symbol]; // FIXME: Not sure if "SymbolPair - 1" is gonna work?
                Tree2Build->Node[0].RightNode = SymbolsAndProbabilities[Symbol + 1];
            }
        }
        return Tree2Build; // Is this seriously it?
                           // Make sure to remove any codes that do not occur (frequency = 0)
                           // When multiple values have the same frequency sort by intensity.
    }
    
    void ParseDeflateBlock(const BitBuffer *BitB, uint16_t BlockSize) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ParseDeflateBlock", "Pointer to BitBuffer is NULL");
        } else {
            bool    IsLastBlock    = ReadBits(BitB, 1, true); // no
            uint8_t EncodingMethod = ReadBits(BitB, 2, true); // 3
            
            switch (EncodingMethod) {
                case 0:
                    // Stored.
                    //DecodeStoredHuffman(BitB);
                    break;
                case 1:
                    // Static Huffman + preagreed table
                    //DecodeStaticHuffman(BitB);
                    break;
                case 2:
                    // Dynamic Huffman
                    //DecodeDynamicHuffman(BitB);
                    break;
                default:
                    Log(LOG_EMERG, "libBitIO", "ParseDeflateBlock", "Invalid Deflate encoding method: %d", EncodingMethod);
                    break;
            }
        }
    }
    
    void EncodeLZ77(const BitBuffer *RawBuffer, const BitBuffer *LZ77Buffer, const uint64_t WindowSize, const uint64_t DistanceLength, const uint64_t SymbolSize) {
        // The dictionary is simply the current buffer, at the current buffer position -(WindowSize / 2) +(WindowSize / 2)
        // So, the first thing you write is the distance from the cursor to the previous string.
        // Then you write the length of the largest match you can find.
        // Then write the next byte in the stream.
        // Then move the cursor the length of the longest match + 1
        // When we're at the start of the match, simply record the byte and the length of the match (1).
        
        
        // We need to have a BitBuffer to read from, and a BitBuffer to write to.
        if (RawBuffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the raw buffer is NULL");
        } else if (LZ77Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the LZ77 buffer is NULL");
        } else {
            uint64_t WindowBits   = NumBits2ReadSymbols(WindowSize);
            uint64_t DistanceBits = NumBits2ReadSymbols(DistanceLength);
            
            uint64_t CurrentSymbol = 0;
            uint64_t PreviousSymbol = 0;
            
            while (GetBitBufferPosition(RawBuffer) != GetBitBufferSize(RawBuffer)) {
                CurrentSymbol = ReadBits(RawBuffer, SymbolSize, true);
                if (CurrentSymbol == PreviousSymbol) {
                    // find the largest string of symbols that matches the current one
                }
            }
            
            
            
            /* ------------OLD SHIT----------------
             
             // Now, we just need to read the RawBuffer (which must contain ALL the data to be encoded) with ReadBits(SymbolSize)
             // then start writing LZ77Buffer with our discoveries.
             // So, loop over RawBuffer, if RawByte == 0, just code the longest string you can, or the first 3 bytes (if they're all different)
             for (uint64_t RawByte = 0; RawByte < RawBuffer->BitsUnavailable; RawByte++) {
             if (RawByte == 0) {
             if (RawBuffer->Buffer[RawByte] == RawBuffer->Buffer[RawByte + 1] || RawBuffer->Buffer[RawByte + 1] == RawBuffer->Buffer[RawByte + 2]) {
             
             }
             }
             }
             */
        }
    }
    
    void EncodeDeflate(const BitBuffer *Data2Encode, const BitBuffer *EncodedData) { // FIXME: Just a stub
        
    }
    
    void DecodeDeflate(const BitBuffer *Data2Decode, const BitBuffer *DecodedData) { // FIXME: Just a stub
        
    }
    
    static void EncodeAABS(SymbolFrequencies *Symbols, BitBuffer *EncodedData, uint64_t Symbol2Encode) { // Adaptive Asymmetric Binary System
        
    }
    
    void Log(const uint8_t ErrorSeverity, const char *LibraryOrProgram, const char *FunctionName, const char *Description, ...) {
        uint64_t FunctionNameSize = 0, DescriptionSize = 0;
        
        char *EasyString = calloc(1, 1 + strlen(FunctionName) + strlen(Description)); // the 1 is for the error severity
        snprintf(EasyString, FunctionNameSize + DescriptionSize, "%hhu: %s - %s", ErrorSeverity, FunctionName, Description);
        
        char *HardString = calloc(1, BitIOStringSize); // FIXME: REMOVE BITIOSTRINGSIZE
        va_list Arguments;
        va_start(Arguments, Description);
        vsprintf(HardString, "%s", Arguments);
        va_end(Arguments);
        
        
        uint64_t ErrorStringSize = strlen(EasyString) + strlen(HardString);
        char *ErrorString = calloc(1, ErrorStringSize);
        snprintf(ErrorString, ErrorStringSize, "%s%s", EasyString, HardString);
        if (BitIOGlobalLogFile == NULL) {
            // Set STDERR As the output file
            fprintf(stderr, "%s\n", ErrorString);
        } else {
            // Use BitO->LogFile as the output file
            fprintf(BitIOGlobalLogFile, "%s\n", ErrorString);
        }
    }
    
#ifdef __cplusplus
}
#endif
