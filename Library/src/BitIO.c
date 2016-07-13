#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    uint16_t SwapEndian16(uint16_t Data2Swap) {
        return ((Data2Swap & 0xFF00) >> 8) || ((Data2Swap & 0x00FF) << 8);
    }
    
    uint32_t SwapEndian32(uint32_t Data2Swap) {
        return ((Data2Swap & 0xFF000000) >> 24) | ((Data2Swap & 0x00FF0000) >> 8) | ((Data2Swap & 0x0000FF00) << 8) | ((Data2Swap & 0x000000FF) << 24);
    }
    
    uint64_t SwapEndian64(uint64_t Data2Swap) {
        return (((Data2Swap & 0xFF00000000000000) >> 56) | ((Data2Swap & 0x00FF000000000000) >> 40) | ((Data2Swap & 0x0000FF0000000000) >> 24) | ((Data2Swap & 0x000000FF00000000) >> 8)  | ((Data2Swap & 0x00000000FF000000) << 8)  | ((Data2Swap & 0x0000000000FF0000) << 24) | ((Data2Swap & 0x000000000000FF00) << 40) | ((Data2Swap & 0x00000000000000FF) << 56));
    }
    
    bool IsOdd(int64_t Input) {
        return Input % 2;
    }
    
    uint64_t Bits2Bytes(uint64_t Bits) {
        return Bits / 8;
    }
    
    uint64_t Bytes2Bits(uint64_t Bytes) {
        return Bytes * 8;
    }
    
    uint8_t BitsRemaining(uint8_t Bits) {
        return 8 - (Bits % 8);
    }
    
    uint64_t Signed2Unsigned(int64_t Signed) {
        if (Signed == (-0|0)) {
            return 0;
        }
        return ~Signed + 1;
    }
    
    int64_t Unsigned2Signed(uint64_t Unsigned) {
        return (int64_t)Unsigned;
    }
    
    void PadInputBits(BitInput *BitI, uint8_t BytesOfPadding) {
        BitI->BitsUnavailable += Bytes2Bits(BytesOfPadding);
    }
    
    void PadOutputBits(BitOutput *BitO, uint8_t BytesOfPadding) {
        BitO->BitsUnavailable += (Bytes2Bits(BytesOfPadding) - BitsRemaining(BitO->BitsUnavailable));
    }
    
    uint8_t FindHighestBitSet(uint64_t InputData) {
        uint8_t HighestBit = 0;
        if (InputData == 0) {
            HighestBit = 0;
        } else {
            for (uint8_t Bit = 64; Bit > 0; Bit--) { // stop at the first 1 bit, and return that.
                if ((InputData & Bit) == 1) {
                    HighestBit = Bit;
                    break;
                }
            }
        }
        return HighestBit;
    }
    
    uint8_t CountBits(uint64_t Input) {
        if (Input == 0) {
            return 0;
        }
        uint8_t Count = 0;
        for (size_t Bit = 0; Bit < 64; Bit++) {
            uint64_t Mask = pow(2, Bit - 1);
            if ((Input & Mask) == 1) { // if ((Input & 1) == 1) {
                Count++;
            }
        }
        return Count;
    }
    
    uint64_t Power2Mask(ErrorStatus *ES, uint64_t Exponent) {
        if (Exponent > 64) {
            ES->Power2Mask = NUMBER_NOT_IN_RANGE;
        }
        uint64_t Mask = pow(2, Exponent);
        return Mask;
    }
    
    void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]) {
        glob_t *GlobBuffer = 0;
        errno = 0;
        
        uint8_t Argument = BitI->CurrentArgument;
        
        while ((Argument < argc) && (BitI->File == 0)) {
            if (strcasecmp(argv[Argument], "-i") == 0) {
                Argument += 1;
                if ((strcasecmp(argv[Argument], "-") || strcasecmp(argv[Argument], "–") || strcasecmp(argv[Argument], "—")) == 0) { // hyphen: -, en dash: –, em dash: —;
                    BitI->File = freopen(argv[Argument], "rb", stdin);
                } else if (strcasecmp(argv[Argument], "*")   == 0) {
                    glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
                } else if (strcasecmp(argv[Argument], "?")   == 0) {
                    glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
                } else {
                    BitI->File = fopen(argv[Argument], "rb");
                    if (errno != 0) {
                        printf("Error opening file %s: %d\n", argv[Argument + 1], errno);
                    } else {
                        printf("Errno is %s: %d\n", argv[Argument], errno);
                    }
                }
            }
            BitI->CurrentArgument = Argument;
        }
    }
    
    void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]) {
        glob_t *GlobBuffer = 0;
        while (BitO->File == 0) {
            for (uint8_t Argument = BitO->CurrentArgument; Argument < argc; Argument++) {
                if (strcasecmp(argv[Argument], "-o") == 0) {
                    if (strcasecmp(argv[Argument++], "-") == 0) {
                        BitO->File = stdout;
                    } else if (strcasecmp(argv[Argument], "*")   == 0) {
                        glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
                    } else if (strcasecmp(argv[Argument], "?")   == 0) {
                        glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
                    } else {
                        BitO->File = fopen(argv[Argument], "wb");
                    }
                }
            }
        }
    }
    
    void InitBitInput(BitInput *BitI, int argc, const char *argv[]) {
        if (argc < 3) {
            printf("Usage: -i <input> -o <output>\n");
        } else {
            ParseInputOptions(BitI, argc, argv);
            if (BitI->File == 0) {
                fprintf(stderr, "InitBitInput: InputFile couldn't be opened, check permissions\n");
            } else {
                BitI->CurrentArgument = 1;
                fseek(BitI->File, 0, SEEK_END);
                BitI->FileSize         = ftell(BitI->File);
                fseek(BitI->File, 0, SEEK_SET);
                
                uint64_t Bytes2Read    = BitI->FileSize > BitIOBufferSize ? BitIOBufferSize : BitI->FileSize;
                
                memset(BitI->Buffer, 0, Bytes2Read);
                uint64_t BytesRead     = fread(BitI->Buffer, 1, BitIOBufferSize, BitI->File);
                if (BytesRead < Bytes2Read) {
                    fprintf(stderr, "InitBitInput: Read: %llu of: %llu bytes, at position: %ld\n", BytesRead, Bytes2Read, ftell(BitI->File));
                }
                BitI->BitsAvailable = Bytes2Bits(BytesRead);
                BitI->BitsUnavailable         = 0;
                BitI->FilePosition  = ftell(BitI->File);
            }
        }
    }
    
    void InitBitOutput(BitOutput *BitO, int argc, const char *argv[]) {
        if (argc < 3) {
            printf("Usage: -i <input> -o <output>\n");
        } else {
            ParseOutputOptions(BitO, argc, argv);
            if (BitO->File == 0) {
                fprintf(stderr, "InitBitOutput: OutputFile couldn't be opened, check permissions\n");
            } else {
                BitO->CurrentArgument = 1;
                memset(BitO->Buffer,  0, BitIOBufferSize);
                BitO->BitsAvailable   = BitIOBufferSizeInBits;
                BitO->BitsUnavailable = 0;
            }
        }
    }
    
    void CloseBitInput(BitInput *BitI) {
        fclose(BitI->File);
        BitI->FileSize         = 0;
        BitI->FilePosition     = 0;
        BitI->BitsUnavailable  = 0;
        BitI->BitsAvailable    = 0;
        memset(BitI->Buffer,     0, BitIOBufferSize);
        free(BitI);
    }
    
    void CloseBitOutput(BitOutput *BitO) {
        fclose(BitO->File);
        BitO->BitsUnavailable = 0;
        BitO->BitsAvailable   = 0;
        memset(BitO->Buffer,    0, BitIOBufferSize);
        free(BitO);
    }
    
    uint64_t UpdateInputBuffer(BitInput *BitI, uint64_t AbsoluteOffset) {
        if (AbsoluteOffset == 0) {
            AbsoluteOffset = BitI->FilePosition;
        }
        
        uint64_t Bytes2Read = BitI->FileSize - BitI->FilePosition > BitIOBufferSize ? BitIOBufferSize : BitI->FileSize - BitI->FilePosition;
        
        fseek(BitI->File, AbsoluteOffset, SEEK_SET);
        memset(BitI, 0, Bytes2Read);
        uint64_t BytesRead = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
        if (BytesRead != Bytes2Read) {
            return FREAD_UNKNOWN_FAILURE;
        }
        return SUCCESS;
    }
    
    uint64_t ReadBits(BitInput *BitI, ErrorStatus *ES, int8_t Bits2Read) { // FIXME: May read backwards?
        uint64_t OutputData = 0;
        if (Bits2Read <= 0) {
            fprintf(stderr, "BitIO - ReadBits: You requested too few bits: %hhd\n", Bits2Read);
        } else if (Bits2Read > 64) {
            fprintf(stderr, "BitIO - ReadBits: You requested too many bits: %hhd\n", Bits2Read);
        } else {
            OutputData             = PeekBits(BitI, ES, Bits2Read);
            BitI->BitsUnavailable += Bits2Read;
            BitI->BitsAvailable   -= Bits2Read;
        }
        return OutputData;
    }
    
    uint64_t ReadExpGolomb(BitInput *BitI, ErrorStatus *ES, bool IsSigned, bool IsTruncated) {
        uint64_t Zeros = 0;
        int64_t  Data  = 0;
        
        Data = ReadBits(BitI, ES, 1);
        if ((Zeros == 0) && (Data == 1)) {
            while (ReadBits(BitI, ES, 1) == 0) {
                Zeros += 1;
            }
            Data = (1 << Zeros);
            Data += ReadBits(BitI, ES, Zeros);
        }
        
        if (IsSigned == true) {
            Data = Unsigned2Signed(Data);
        }
        
        if (IsTruncated == true) {
            Data = 2 * Zeros - 1; // Left bit first
            Data += ReadBits(BitI, ES, Zeros);
        }
        return Data;
    }
    
    uint64_t PeekBits(BitInput *BitI, ErrorStatus *ES, uint8_t Bits2Peek) { // Bit by bit.
        //WARNING: You CAN'T read byte by byte, it simply won't work so stop trying. Also, this is faster, instruction wise.
        uint64_t OutputData = 0;
        uint8_t  Data       = 0;
        uint8_t  BitsLeft   = Bits2Peek;
        uint8_t  BitMask    = 0;
        if ((BitsLeft <= 0) || (BitsLeft > 64)) {
            ES->PeekBits = NUMBER_NOT_IN_RANGE;
        }
        if (BitI->BitsUnavailable > (BitI->BitsAvailable - BitsLeft)) { // InputBuffer is damn near full, update it
            UpdateInputBuffer(BitI, 0);
        }
        while (BitsLeft > 0) {
            OutputData <<= 1;
            BitMask      = 1 << ((8 - (BitI->BitsUnavailable % 8)) -1);
            Data         = BitI->Buffer[Bits2Bytes(BitI->BitsUnavailable)] & BitMask;
            Data       >>= ((8 - (BitI->BitsUnavailable % 8)) -1);
            OutputData            += Data;
            BitI->BitsAvailable   -= 1;
            BitI->BitsUnavailable += 1;
            BitsLeft              -= 1;
        }
        BitI->BitsAvailable   += Bits2Peek;
        BitI->BitsUnavailable -= Bits2Peek;
        return OutputData;
    }
    
    void WriteBits(BitOutput *BitO, ErrorStatus *ES, uint64_t Bits2Write, uint64_t Bits) { // FIXME: Rewrite WriteBits entirely.
        if (Bits <= 0) {
            ES->WriteBits = NUMBER_NOT_IN_RANGE;
        } else {
            while (BitO->BitsUnavailable >= ((BitIOBufferSize - 16) * 8)) { // Write to disk because the buffer's full.
                // FIXME: We need to just enlarge the buffer, the call a function to flush it all out to disk.
                fwrite(BitO->Buffer, 1, Bits2Bytes(BitO->BitsUnavailable), BitO->File);
                fflush(BitO->File);
                memcpy(&BitO->Buffer, &BitO->Buffer + Bits2Bytes(BitO->BitsUnavailable), (BitIOBufferSize - Bits2Bytes(BitO->BitsUnavailable)));
                memset(BitO->Buffer, 0, BitIOBufferSize);
                BitO->BitsUnavailable = (BitO->BitsUnavailable - ((BitO->BitsUnavailable / 8) * 8));
            }
            while (Bits > 0) {
                uint64_t X = Bits2Write & BitsRemaining(BitO->BitsUnavailable);
                BitO->Buffer[Bits2Bytes(BitO->BitsUnavailable)] += X;
                BitO->BitsUnavailable++;
                Bits--;
            }
        }
    }
    
    void WriteBuffer(BitOutput *BitO, ErrorStatus *ES, uint64_t *Buffer2Write, size_t BufferSize) {
        // FIXME: This needs expand the buffer if nessicary.
        for (size_t Byte = 0; Byte < BufferSize; Byte++) {
            WriteBits(BitO, ES, Buffer2Write[Byte], 8);
        }
    }
    
    void SeekBits(BitInput *BitI, int64_t Bits) {
        if ((BitI->BitsUnavailable + Bits) >= BitI->BitsAvailable) {
            UpdateInputBuffer(BitI, 0);
        } else {
            uint64_t Bytes2Read = BitI->BitsAvailable<BitIOBufferSizeInBits?BitI->BitsAvailable:BitIOBufferSizeInBits;
            BitI->FilePosition = ftell(BitI->File);
            fseek(BitI->File, BitIOBufferSizeInBits - (BitI->BitsUnavailable + Bits), SEEK_CUR);
            memset(BitI->Buffer, 0, BitIOBufferSize);
            fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
        }
    }
    
    uint64_t BinarySearch(BitInput *BitI, int64_t HexPattern, bool SearchWholeFile) { // FIXME: replace BitIOBufferSize with BitI->BitsAvailable
        if (SearchWholeFile == false) {
            for (uint16_t Byte = 0; Byte < BitI->BitsAvailable; Byte++) {
                for (size_t HexPatternByte = 0; HexPatternByte < CountBits(HexPattern); HexPatternByte++) {
                    if (BitI->Buffer[Byte] == HexPattern) {
                        HexPatternByte++;
                        if (HexPatternByte >= CountBits(HexPattern)) {
                            return 1; // Basically, return the current position in the buffer, minus HexPatternSize,
                        }
                    }
                }
            }
        } else {
            for (uint16_t Byte = 0; Byte < BitI->BitsAvailable; Byte++) {
                for (size_t HexPatternByte = 0; HexPatternByte < CountBits(HexPattern); HexPatternByte++) {
                    if (BitI->Buffer[Byte] == HexPattern) {
                        HexPatternByte++;
                        if (HexPatternByte >= CountBits(HexPattern)) {
                            return 1; // Basically, return the current position in the buffer, minus HexPatternSize,
                        }
                    }
                }
            }
            if (BitI->BitsUnavailable >= BitI->BitsAvailable) {
                UpdateInputBuffer(BitI, 0);
            }
        }
        return 0;
    }
    
    uint64_t GenerateCRC(uint64_t *DataBuffer, uint64_t BufferSize, bool IsBigEndian, uint64_t Polynomial, uint64_t CRCSize, uint64_t InitialValue) {
        if (IsBigEndian == true) {
        }
        
        uint64_t DataInput  = 0;
        uint64_t DataOutput = 0;
        
        for (size_t BufferIndex = 0; BufferIndex < (BufferSize / CRCSize); BufferIndex++) { // needs to iterate over bytes the size of DataSize
            DataInput = DataBuffer[BufferIndex];
            for (uint8_t Bit = 0; Bit < CRCSize * 8; Bit++) {
                if ((InitialValue & 0x1) ^ (DataInput & 0x1)) {
                    InitialValue ^= Polynomial;
                } else {
                    InitialValue >>= 1;
                }
            }
        }
        return DataOutput;
    }
    
    bool VerifyCRC(uint64_t *DataBuffer, uint64_t BufferSize, bool IsBigEndian, uint64_t Polynomial, uint64_t CRCSize, uint64_t InitialValue, uint64_t EmbeddedCRC) {
        uint64_t GeneratedCRC = GenerateCRC(DataBuffer, BufferSize, IsBigEndian, Polynomial, CRCSize, InitialValue);
        
        if (GeneratedCRC == EmbeddedCRC) {
            return true;
        } else {
            return false;
        }
    }
    
    // tl;dr the most common symbols are assigned the smallest codes. How?
    void HuffmanDecoder(BitInput *BitI, uint64_t NumberOfCodes) {
    }
    
    uintptr_t ReconstructHuffmanTree(BitInput *BitI, uintptr_t TableID, size_t TableSize) {
        uintptr_t ReconstructedHuffmanTree = NULL;
        return ReconstructedHuffmanTree;
    }
    
    void HuffmanEncoder(BitInput *BitI, uintptr_t HuffmanBlock, uint64_t WindowSize) {
    }
    
    void ArithmeticEncoder(void) {
    }
    
    void ArithmeticDecoder(void) {
    }
#ifdef __cplusplus
}
#endif
