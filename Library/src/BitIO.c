#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#pragma GCC poison gets puts strcpy strcat tempfile mktemp sprintf gethostbyaddr gethostbyname bzero strcmp malloc
    
    uint16_t SwapEndian16(uint16_t Data2Swap) { // In UnitTest
        return ((Data2Swap & 0xFF00) >> 8) | ((Data2Swap & 0x00FF) << 8);
    }
    
    uint32_t SwapEndian32(uint32_t Data2Swap) { // In UnitTest
        return ((Data2Swap & 0xFF000000) >> 24) | ((Data2Swap & 0x00FF0000) >> 8) | ((Data2Swap & 0x0000FF00) << 8) | ((Data2Swap & 0x000000FF) << 24);
    }
    
    uint64_t SwapEndian64(uint64_t Data2Swap) { // In UnitTest
        return (((Data2Swap & 0xFF00000000000000) >> 56) | ((Data2Swap & 0x00FF000000000000) >> 40) | \
                ((Data2Swap & 0x0000FF0000000000) >> 24) | ((Data2Swap & 0x000000FF00000000) >>  8) | \
                ((Data2Swap & 0x00000000FF000000) <<  8) | ((Data2Swap & 0x0000000000FF0000) << 24) | \
                ((Data2Swap & 0x000000000000FF00) << 40) | ((Data2Swap & 0x00000000000000FF) << 56));
    }
    
    uint64_t Bits2Bytes(uint64_t Bits) { // In UnitTest
        return (Bits / 8);
    }
    
    uint64_t Bytes2Bits(uint64_t Bytes) { // In UnitTest
        return (Bytes * 8);
    }
    
    uint8_t BitsRemaining(uint64_t BitsAvailable) { // In UnitTest
        return BitsAvailable > 8 ? 8 : BitsAvailable;
    }
    
    uint64_t Signed2Unsigned(int64_t Signed) { // In UnitTest
        return (uint64_t)Signed;
    }
    
    int64_t Unsigned2Signed(uint64_t Unsigned) {  // In UnitTest
        return (int64_t)Unsigned;
    }
    
    uint64_t Powi(uint64_t Base, uint64_t Exponent) {
        uint64_t Result = 1;
        
        for (uint64_t Times = 0; Times < Exponent; Times++) {
            Result *= Base;
        }
        return Result;
    }
    
    int64_t Floori(double X) {
        return (int64_t)floor(X);
    }
    
    int64_t Ceili(long double X) {
        return (int64_t)ceil(X);
    }
    
    uint8_t CountBitsSet(uint64_t Data) {
        uint8_t DataBit = 0, BitCount = 0;
        for (uint8_t Bit = 0; Bit < Bits2Bytes(sizeof(Data)); Bit++) {
            DataBit = (Data & (1 << Bit)) >> Bit;
            if (DataBit == 1) {
                BitCount += 1;
            }
        }
        return BitCount;
    }
    
    uint64_t Power2Mask(uint8_t Exponent) { // In UnitTest
        if ((Exponent <= 0) || (Exponent > 64)) { // Exponent = 1
            return EXIT_FAILURE;
        } else {
            if (Exponent == 1) {
                return 1;
            } else if (Exponent == 64) {
                return ((1ULL << Exponent) - 1) + (((1ULL << Exponent) - 1) - 1);
            } else {
                return (1ULL << Exponent) - 1; // Exponent = 6 // ((1ULL << (Exponent - 1)) - 1)
            } // (1ULL << (Exponent - 1)) + ((1ULL << Exponent -1) -1);
        }
    }
    
    uint64_t OnesCompliment2TwosCompliment(int64_t Input) { // All unset bits ABOVE the set bit are set, including those originally set
        return ~Input + 1;
    }
    
    uint64_t TwosCompliment2OnesCompliment(int64_t Input) { // All unset bits become set, except those originally set
        return Input ^ 0xFFFFFFFFFFFFFFFF;
    }
    
    uint8_t DetectSystemEndian(void) {
        uint8_t SystemEndian = 0;
        uint16_t Endian = 0xFFFE;
        if (Endian == 0xFFFE) {
            SystemEndian = LittleEndian;
        } else if (Endian == 0xFEFF) {
            SystemEndian = BigEndian;
        } else {
            SystemEndian = UnknownEndian;
        }
        return SystemEndian;
    }
    
    bool IsStreamByteAligned(uint64_t BitsUsed, uint8_t BytesOfAlignment) {
        if ((BitsUsed % Bytes2Bits(BytesOfAlignment)) == 0) {
            return true;
        } else {
            return false;
        }
    }
    
    void AlignInput(BitInput *BitI, uint8_t BytesOfAlignment) {
        uint8_t Bits2Align = BitI->BitsUnavailable % Bytes2Bits(BytesOfAlignment);
        
        if (Bits2Align != 0) { // NOT aligned
            BitI->BitsAvailable   -= Bits2Align;
            BitI->BitsUnavailable += Bits2Align;
        }
    }
    
    void AlignOutput(BitOutput *BitO, uint8_t BytesOfAlignment) {
        uint8_t Bits2Align = BitO->BitsUnavailable % Bytes2Bits(BytesOfAlignment);
        if (Bits2Align != 0) {
            BitO->BitsAvailable    -= Bits2Align;
            BitO->BitsUnavailable  += Bits2Align;
        }
    }
    
    bool IsOdd(int64_t X) {
        return X % 2 == 0 ? false : true;
    }
    
    uint8_t  FindHighestBitSet(uint64_t X) {
        uint8_t HighestBitSet = 0;
        // use sizeof to get the size of the included bit (check to see if its always cast to 64 tho, if so just start at 64)
        // then count down from 64 to 0, stopping when you first find a 1 bit, mark the number of loops. that's the highest bit set.
        for (uint8_t Bit = sizeof(X); Bit > 0; Bit--) {
            if (((X & Bit) >> (sizeof(X) - Bit)) == 1) {
                HighestBitSet = Bit;
                break;
            }
        }
        return HighestBitSet;
    }
    
    void DisplayCMDHelp(CommandLineOptions *CMD) {
        printf("%s: %s, %s\n\n", CMD->ProgramName, CMD->ProgramDescription, CMD->AuthorCopyrightLicense);
        printf("Options:\n");
        for (uint8_t Option = 0; Option < CMD->NumSwitches; Option++) {
            printf("%s\t", CMD->Switch[Option]->Switch);
            printf("%s\n", CMD->Switch[Option]->SwitchDescription);
        }
    }
    
    void ParseCommandLineArguments(CommandLineOptions *CMD, int argc, const char *argv[]) {
        // Scan for equals signs as well, if found, after after the equal sign is the result, everything before is the switch.
        for (uint8_t Argument = 0; Argument < argc; Argument++) {
            for (uint8_t Switch = 0; Switch < CMD->NumSwitches; Switch++) {
                if (strcasecmp(CMD->Switch[Switch]->Switch, argv[Argument]) == 0) { // If the current switch matches one of the switches, set the IsFound bool to true.
                    CMD->Switch[Switch]->SwitchFound = true;
                    if (CMD->Switch[Switch]->Resultless == false) {
                        char *SwitchResult = calloc(BitIOStringSize, 1);
                        snprintf(SwitchResult, BitIOStringSize, "%s", argv[Argument + 1]);
                        CMD->Switch[Switch]->SwitchResult = SwitchResult;
                    }
                }
                /*
                 EqualsLocation = strchr(Argument, 0x3D); // 0x3D = "="
                 if (EqualsLocation != NULL) { // found
                 // Start reading the result of the switch from EqualLocation +1.
                 for (size_t Byte = EqualsLocation + 1; Byte < sizeof(argv[Index]); Byte++) {
                 for (size_t SwitchByte = 0; SwitchByte < sizeof(argv[Index]); SwitchByte++) {
                 CMD->Switch[Index]->SwitchResult[SwitchByte] = *argv[Index];
                 }
                 }
                 }
                 */
            }
        }
    }
    
    void OpenCMDInputFile(BitInput *BitI, CommandLineOptions *CMD, uint8_t InputSwitch) {
        BitI->File = fopen(CMD->Switch[InputSwitch]->SwitchResult, "rb");
        fseek(BitI->File, 0, SEEK_END);
        BitI->FileSize = (uint64_t)ftell(BitI->File);
        fseek(BitI->File, 0, SEEK_SET);
        BitI->FilePosition     = ftell(BitI->File);
        uint64_t Bytes2Read    = BitI->FileSize > BitInputBufferSize ? BitInputBufferSize : BitI->FileSize;
        uint64_t BytesRead     = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
        BitI->BitsAvailable    = Bytes2Bits(BytesRead);
        BitI->BitsUnavailable  = 0;
    }
    
    void OpenCMDOutputFile(BitOutput *BitO, CommandLineOptions *CMD, uint8_t InputSwitch) {
        BitO->File = fopen(CMD->Switch[InputSwitch]->SwitchResult, "rb");
        BitO->BitsAvailable    = BitOutputBufferSizeInBits;
        BitO->BitsUnavailable  = 0;
    }
    
    void FlushBitOutput(BitOutput *BitO) {
        if (IsStreamByteAligned(BitO->BitsUnavailable, 1) == false) {
            AlignOutput(BitO, 1);
        }
        fwrite(BitO->Buffer, Bits2Bytes(BitO->BitsUnavailable), 1, BitO->File);
    }
    
    void CloseBitInput(BitInput *BitI) {
        fclose(BitI->File);
        free(BitI);
    }
    
    void CloseBitOutput(BitOutput *BitO) {
        fwrite(BitO->Buffer, BitO->BitsUnavailable % 8, 1, BitO->File); // Make sure it's all written to disk
        fflush(BitO->File);
        fclose(BitO->File);
        free(BitO);
    }
    
    void UpdateInputBuffer(BitInput *BitI, int64_t RelativeOffsetInBytes) {
        uint64_t Bytes2Read = 0, BytesRead = 0;
        fseek(BitI->File, RelativeOffsetInBytes, SEEK_CUR);
        BitI->FilePosition = ftell(BitI->File);
        memset(BitI->Buffer, 0, sizeof(BitI->Buffer));
        Bytes2Read = BitI->FileSize - BitI->FilePosition >= BitInputBufferSize ? BitInputBufferSize : BitI->FileSize - BitI->FilePosition;
        BytesRead = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
        if (BytesRead != Bytes2Read) { // Bytes2Read
            char ErrorDescription[BitIOStringSize];
            snprintf(ErrorDescription, BitIOStringSize, "Supposed to read %llu bytes, but read %llu\n", Bytes2Read, BytesRead);
            Log(LOG_WARNING, "libBitIO", "UpdateInputBuffer", ErrorDescription);
        }
        uint64_t NEWBitsUnavailable = BitI->BitsUnavailable % 8; // FIXME: This assumes UpdateBuffer was called with at most 7 unread bits...
        
        BitI->BitsUnavailable = NEWBitsUnavailable;
        BitI->BitsAvailable   = Bytes2Bits(BytesRead);
    }
    
    uint64_t ReadBits2(BitInput *BitI, uint8_t Bits2Read, bool ReadFromMSB) { // Set this up so it can read from memory addresses, to support running on machines without an OS.
        uint8_t Bits = Bits2Read, UserBits = 0, SystemBits = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0;
        
        if ((Bits2Read <= 0) || (Bits2Read > 64)) {
            char Description[BitIOPathSize];
            snprintf(Description, BitIOPathSize, "ReadBits only supports reading 1-64 bits at a time, you tried reading: %d bits\n", Bits2Read);
            Log(LOG_CRIT, "libBitIO", "ReadBits2", Description);
            exit(EXIT_FAILURE);
        } else {
            if (BitI->BitsAvailable < Bits) {
                UpdateInputBuffer(BitI, 0);
            }
            SystemBits             = 8 - (BitI->BitsUnavailable % 8);
            UserBits               = BitsRemaining(Bits);
            Bits2Read              = SystemBits >= UserBits  ? UserBits : SystemBits;
            if (ReadFromMSB == true) {
                Mask2Shift         = SystemBits <= UserBits  ? 0 : SystemBits - UserBits;
                Mask               = (Power2Mask(Bits2Read) << Mask2Shift);
            } else { // read from LSB
                     // read 2 bits, 2 offset, so mask = 0b00001100
                     // BitsAvailable = 72, BitsUnavailable = 95
                     // SystemBits = 1
                     // UserBits   = 2
                     // Bits2Read  = 1
                     // Mask2Shift = -1?????
                     // Mask       = 1 << 0
                     // Data       = 0x1
                
                // Important variables:
                // The number of bits requested in general.
                // The number of bits available in general.
                // The number of bits to read from this byte, combinartion of those 2 variables.
                // Where to read the bits from in this byte (MSB/LSB here)
                // Shifting the read bits to accomadate the output (MSB/LSB here as well.)
                Mask               = (Power2Mask(Bits2Read) << SystemBits);
            }
            Data                   = BitI->Buffer[BitI->BitsUnavailable / 8] & Mask;
            Data                 >>= Mask2Shift;
            OutputData           <<= SystemBits >= UserBits ? UserBits : SystemBits;
            OutputData            += Data;
            BitI->BitsAvailable   -= SystemBits >= UserBits ? UserBits : SystemBits;
            BitI->BitsUnavailable += SystemBits >= UserBits ? UserBits : SystemBits;
            Bits                  -= SystemBits >= UserBits ? UserBits : SystemBits;
        }
        return OutputData;
    }
    
    uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read) { // Set this up so it can read from memory addresses, to support running on machines without an OS.
        uint8_t Bits = Bits2Read, UserBits = 0, SystemBits = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0;
        
        if ((Bits2Read <= 0) || (Bits2Read > 64)) {
            char Description[BitIOPathSize];
            snprintf(Description, BitIOPathSize, "ReadBits only supports reading 1-64 bits at a time, you tried reading: %d bits\n", Bits2Read);
            Log(LOG_CRIT, "libBitIO", "ReadBits", Description);
            exit(EXIT_FAILURE);
        } else {
            if (BitI->BitsAvailable < Bits) {
                UpdateInputBuffer(BitI, 0);
            }
            while (Bits > 0) {
                SystemBits             = 8 - (BitI->BitsUnavailable % 8);
                UserBits               = BitsRemaining(Bits);
                Bits2Read              = SystemBits >= UserBits  ? UserBits : SystemBits;
                Mask2Shift             = SystemBits <= UserBits  ? 0 : SystemBits - UserBits;
                Mask                   = (Power2Mask(Bits2Read) << Mask2Shift);
                Data                   = BitI->Buffer[BitI->BitsUnavailable / 8] & Mask;
                Data                 >>= Mask2Shift;
                
                OutputData           <<= SystemBits >= UserBits ? UserBits : SystemBits;
                OutputData            += Data;
                
                BitI->BitsAvailable   -= SystemBits >= UserBits ? UserBits : SystemBits;
                BitI->BitsUnavailable += SystemBits >= UserBits ? UserBits : SystemBits;
                Bits                  -= SystemBits >= UserBits ? UserBits : SystemBits;
            }
        }
        return OutputData;
    }
    
    uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek) {
        uint64_t OutputData = 0ULL;
        OutputData = ReadBits(BitI, Bits2Peek);
        
        BitI->BitsAvailable       += Bits2Peek;
        BitI->BitsUnavailable     -= Bits2Peek;
        
        return OutputData;
    }
    
    void WriteBits(BitOutput *BitO, uint64_t Data2Write, uint8_t NumBits) { // 12 bits 2 write, 0xFFF
        uint8_t BitsLeft = NumBits, InputMask = 0, OutputMask = 0, Bits2Write = 0;
        if (BitO->BitsAvailable < NumBits) {
            fwrite(BitO->Buffer, Bits2Bytes(BitO->BitsUnavailable), 1, BitO->File);
            // Save unused bits, memset, and recopy them to the start of the buffer
        }
        // in order to write bits to the buffer, I need to mask Data2Write, and apply it to the output buffer, also, with a mask.
        while (BitsLeft > 0) {
            // if BitsLeft is greater than 8, the input mask needs to be 8, otherwise, set it to BitsLeft
            Bits2Write   = BitsLeft > 8 ? 8 : BitsLeft;
            InputMask    = Power2Mask(Bits2Write);
            BitO->Buffer[BitO->BitsUnavailable / 8] = Data2Write & InputMask;
            Data2Write >>= Bits2Write;
            BitsLeft    -= Bits2Write;
        }
    }
    
    void SkipBits(BitInput *BitI, int64_t Bits) {
        if (Bits <= BitI->BitsAvailable) {
            BitI->BitsAvailable   -= Bits;
            BitI->BitsUnavailable += Bits;
        } else {
            fseek(BitI->File, Bits2Bytes(Bits - BitI->BitsAvailable), SEEK_CUR);
            BitI->BitsAvailable   = BitI->FileSize + BitInputBufferSize <= BitI->FileSize ? BitInputBufferSize : BitI->FileSize;
            BitI->BitsUnavailable = Bits % 8;
            UpdateInputBuffer(BitI, 0); // Bits2Bytes(Bits)
        }
    }
    
    uint64_t  ReadRICE(BitInput *BitI, bool Truncated, uint8_t StopBit) {
        uint64_t BitCount = 0;
        
        while (ReadBits(BitI, 1) != StopBit) {
            BitCount += 1;
        }
        if (Truncated == true) {
            BitCount++;
        }
        return BitCount;
    }
    
    void WriteRICE(BitOutput *BitO, bool Truncated, bool StopBit, uint64_t Data2Write) {
        for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
            WriteBits(BitO, (~StopBit), 1);
        }
        WriteBits(BitO, StopBit, 1);
    }
    
    int64_t ReadExpGolomb(BitInput *BitI, bool IsSigned) {
        uint64_t Zeros   = 0;
        uint64_t CodeNum = 0;
        int64_t  Temp    = 0;
        int64_t  Final   = 0;
        
        while (ReadBits(BitI, 1) != 1) {
            Zeros += 1;
        }
        
        if (IsSigned == false) {
            CodeNum  = (1ULL << Zeros);
            CodeNum += ReadBits(BitI, Zeros);
        } else { // Signed
            if (IsOdd(CodeNum) == true) {
                Final = CodeNum - 1;
            } else {
                Final = -(CodeNum - 1);
            }
        }
        return Final;
    }
    
    void WriteExpGolomb(BitOutput *BitO, bool IsSigned, uint64_t Data2Write) {
        uint64_t NumBits = 0;
        
        NumBits = FindHighestBitSet(Data2Write);
        
        if (IsSigned == false) { // Unsigned
            WriteBits(BitO, 0, NumBits);
            WriteBits(BitO, Data2Write + 1, NumBits + 1);
        } else { // Signed
                 // Neg numbers are even, odd numbers are pos
            NumBits -= 1; // Remove the sign bit
            WriteBits(BitO, 0, NumBits);
            if (IsOdd(Data2Write +1) == false) { // Negative
                WriteBits(BitO, Data2Write + 1, NumBits + 1);
            } else {
                WriteBits(BitO, Data2Write + 1, NumBits + 1); // TODO: Signed ExpGolomb
            }
        }
    }
    
    void RotateArray(size_t DataSize, int64_t *Data, uint64_t NumRotations, bool RotateRight) {
        // Theoretical speed up: just edit the pointer to each array element, and increment or decrement it by  NumRotation.
        if (RotateRight == true) {
            for (uint64_t Rotation = 0; Rotation < NumRotations; Rotation++) {
                for (uint64_t Element = 0; Element < DataSize; Element++) {
                    Data[Element + 1] = Data[Element]; // TODO: Make sure the last element wraps around to the end.
                }
            }
        } else { // Rotate left
            for (uint64_t Rotation = 0; Rotation < NumRotations; Rotation++) {
                for (uint64_t Element = DataSize; Element > 0; Element--) {
                    Data[Element] = Data[Element - 1];
                }
            }
        }
    }
    
    uint64_t GenerateCRC(BitInput *BitI, size_t DataSize, CRC *CRCData) {
        uint16_t CRCResult = 0;
        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            CRCResult = CRCData->Polynomial ^ BitI->Buffer[BitI->BitsUnavailable / 8] << 8;
            for (uint8_t Bit = 0; Bit < 8; Bit++) {
                if ((CRCResult & 0x8000) == true) {
                    //CRCResult = ((CRCResult <<= 1) ^ CRCData->Polynomial);
                } else {
                    CRCResult <<= 1;
                }
            }
        }
        return 0;
    }
    
    bool VerifyCRC(BitInput *BitI, size_t DataSize, CRC *CRCData) { // uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, bool PolyType, uint64_t Init, uint8_t CRCSize, uint64_t EmbeddedCRC
        /*
         uint64_t GeneratedCRC = GenerateCRC(CRCData); // DataBuffer, BufferSize, Poly, PolyType, Init, CRCSize
         if (GeneratedCRC == EmbeddedCRC) {
         return true;
         } else {
         return false;
         }
         */
        return false;
    }
    
    void Log(uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription) {
#ifdef _WIN32 // windows mode
        fprintf(stderr, "%s - %s: %s\n", Library, Function, ErrorDescription);
#else // UNIX Mode!!!
        if ((ErrorLevel == LOG_EMERG) || (ErrorLevel == LOG_CRIT)) {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_MAIL|LOG_USER));
        } else {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_USER));
        }
        syslog(LOG_ERR, "%s - %s: %s\n", LibraryOrProgram, Function, ErrorDescription);
#endif
    }
    
    uint32_t GenerateAdler32(uint8_t *Data, size_t DataSize) { // In UnitTest
        uint32_t Adler  = 1;
        uint32_t Sum1   = Adler & 0xFFFF;
        uint32_t Sum2   = (Adler >> 16) & 0xFFFF;
        
        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            Sum1 += Data[Byte] % 65521;
            Sum2 += Sum1 % 65521;
        }
        return (Sum2 << 16) + Sum1;
    }
    
    bool VerifyAdler32(uint8_t *Data, size_t DataSize, uint32_t EmbeddedAdler32) { // In UnitTest
        uint32_t GeneratedAdler32 = GenerateAdler32(Data, DataSize);
        if (GeneratedAdler32 != EmbeddedAdler32) {
            return false;
        } else {
            return true;
        }
    }
    
#ifdef __cplusplus
}
#endif
