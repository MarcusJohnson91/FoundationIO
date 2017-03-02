#include "../include/BitIO.h"

#include <math.h>
#include <string.h>

#if defined(_POSIX_VERSION)||(__APPLE__)
#include <syslog.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp stricmp
#endif
    
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
    
    int64_t Bits2Bytes(const int64_t Bits, const bool RoundUp) {
        if (RoundUp == true) {
            return (Bits / 8) + (8 - (Bits % 8));
        } else {
            return (Bits / 8);
        }
    }
    
    uint64_t Bytes2Bits(const uint64_t Bytes) { 
        return (Bytes * 8);
    }
    
    uint8_t BitsRemaining(const uint64_t BitsAvailable) { 
        return BitsAvailable > 8 ? 8 : BitsAvailable;
    }
    
    uint64_t Signed2Unsigned(const int64_t Signed) { 
        return (uint64_t)Signed;
    }
    
    int64_t Unsigned2Signed(const uint64_t Unsigned) {  
        return (int64_t)Unsigned;
    }
    
    int64_t Powi(int64_t Base, const int64_t Exponent) {
        return Base *= Exponent;
    }
    
    int64_t Floori(const long double Number2Floor) {
        return (int64_t)floor(Number2Floor);
    }
    
    int64_t Ceili(const long double Number2Ceil) {
        return (int64_t)ceil(Number2Ceil);
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
        if ((Exponent <= 0) || (Exponent > 64)) { // Exponent = 1
            return EXIT_FAILURE;
        } else {
            if (Exponent == 1) {
                return 1;
            } else if (Exponent == 64) {
                return ((1ULL << Exponent) - 1) + (((1ULL << Exponent) - 1) - 1);
            } else {
                return (1ULL << Exponent) - 1;
            }
        }
    }
    
    uint64_t OnesCompliment2TwosCompliment(const int64_t OnesCompliment) {
        return ~OnesCompliment + 1;
    }
    
    uint64_t TwosCompliment2OnesCompliment(const int64_t TwosCompliment) {
        return TwosCompliment ^ 0xFFFFFFFFFFFFFFFF;
    }
    
    bool IsOdd(const int64_t Number2Check) {
        return Number2Check % 2 == 0 ? false : true;
    }
    
    uint8_t  FindHighestBitSet(const uint64_t Integer2Search) {
        uint8_t HighestBitSet = 0;
        for (uint8_t Bit = sizeof(Integer2Search); Bit > 0; Bit--) {
            if (((Integer2Search & Bit) >> (sizeof(Integer2Search) - Bit)) == 1) {
                HighestBitSet = Bit;
                break;
            }
        }
        return HighestBitSet;
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
    
    void DisplayCMDHelp(const CommandLineOptions *CMD) {
        printf("%s: %s, %s\n\n", CMD->ProgramName, CMD->ProgramDescription, CMD->AuthorCopyrightLicense);
        printf("Options:\n");
        for (uint8_t Option = 0; Option < CMD->NumSwitches; Option++) {
            printf("%s\t", CMD->Switch[Option]->Switch);
            printf("%s\n", CMD->Switch[Option]->SwitchDescription);
        }
    }
    
    void ParseCommandLineArguments(const CommandLineOptions *CMD, int argc, const char *argv[]) {
        // TODO: Scan for equals signs as well, if found, after after the equal sign is the result, everything before is the switch.
        for (uint8_t Argument = 0; Argument < argc; Argument++) {
            for (uint8_t Switch = 0; Switch < CMD->NumSwitches; Switch++) {
                if (strcasecmp("-h", argv[Argument]) == 0 || strcasecmp("--h", argv[Argument]) == 0 || strcasecmp("/?", argv[Argument]) == 0) {
                    DisplayCMDHelp(CMD);
                }
                if (strcasecmp(CMD->Switch[Switch]->Switch, argv[Argument]) == 0) {
                    CMD->Switch[Switch]->SwitchFound = true;
                    if (CMD->Switch[Switch]->Resultless == false) {
                        char SwitchResult[BitIOStringSize];
                        snprintf(SwitchResult, BitIOStringSize, "%s", argv[Argument + 1]);
                        CMD->Switch[Switch]->SwitchResult = SwitchResult;
                    }
                }
            }
        }
    }
    
    void OpenCMDInputFile(BitInput *BitI, const CommandLineOptions *CMD, const uint8_t InputSwitch) {
        BitI->File             = fopen(CMD->Switch[InputSwitch]->SwitchResult, "rb");
        fseek(BitI->File, 0, SEEK_END);
        BitI->FileSize = (uint64_t)ftell(BitI->File);
        fseek(BitI->File, 0, SEEK_SET);
        BitI->FilePosition     = ftell(BitI->File);
        uint64_t Bytes2Read    = BitI->FileSize > BitInputBufferSize ? BitInputBufferSize : BitI->FileSize;
        uint64_t BytesRead     = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
        BitI->BitsAvailable    = Bytes2Bits(BytesRead);
        BitI->BitsUnavailable  = 0;
    }
    
    void OpenCMDOutputFile(BitOutput *BitO, const CommandLineOptions *CMD, const uint8_t OutputSwitch) {
        BitO->File             = fopen(CMD->Switch[OutputSwitch]->SwitchResult, "rb");
        BitO->BitsAvailable    = BitOutputBufferSizeInBits;
        BitO->BitsUnavailable  = 0;
    }
    
    void UpdateInputBuffer(BitInput *BitI, const int64_t RelativeOffsetInBytes) { // INTERNAL ONLY, // You assume that there are 0 bits left.
        uint64_t Bytes2Read = 0, BytesRead = 0;
        fseek(BitI->File, RelativeOffsetInBytes, SEEK_CUR);
        BitI->FilePosition = ftell(BitI->File);
        memset(BitI->Buffer, 0, sizeof(BitI->Buffer));
        Bytes2Read = BitI->FileSize - BitI->FilePosition >= BitInputBufferSize ? BitInputBufferSize : BitI->FileSize - BitI->FilePosition;
        BytesRead = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
        if (BytesRead != Bytes2Read) {
            char ErrorDescription[BitIOStringSize];
            snprintf(ErrorDescription, BitIOStringSize, "Supposed to read %llu bytes, but read %llu\n", Bytes2Read, BytesRead);
            Log(LOG_WARNING, "libBitIO", "UpdateInputBuffer", ErrorDescription);
        }
        uint64_t NEWBitsUnavailable = BitI->BitsUnavailable % 8; // FIXME: This assumes UpdateBuffer was called with at most 7 unread bits...
        
        BitI->BitsUnavailable = NEWBitsUnavailable;
        BitI->BitsAvailable   = Bytes2Bits(BytesRead);
    }
    
    uint64_t ReadBits(BitInput *BitI, const uint8_t Bits2Read, bool ReadFromMSB) {
        uint8_t Bits = Bits2Read, UserBits = 0, SystemBits = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0;
        
        if ((Bits2Read <= 0) || (Bits2Read > 64)) {
            char Description[BitIOStringSize];
            snprintf(Description, BitIOStringSize, "ReadBits only supports reading 1-64 bits at a time, you tried reading: %d bits\n", Bits2Read);
            Log(LOG_CRIT, "libBitIO", "ReadBits", Description);
            exit(EXIT_FAILURE);
        } else {
            if (BitI->BitsAvailable < Bits) {
                UpdateInputBuffer(BitI, 0);
            }
            SystemBits             = 8 - (BitI->BitsUnavailable % 8);
            UserBits               = BitsRemaining(Bits);
            Bits                   = SystemBits >= UserBits  ? UserBits : SystemBits;
            if (ReadFromMSB == true) {
                Mask2Shift         = SystemBits <= UserBits  ? 0 : SystemBits - UserBits;
                Mask               = (Power2Mask(Bits) << Mask2Shift);
            } else {
                Mask               = (Powi(2, Bits) - 1) << BitI->BitsUnavailable % 8;
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
    
    uint64_t PeekBits(BitInput *BitI, const uint8_t Bits2Peek, bool ReadFromMSB) {
        uint64_t OutputData = 0ULL;
        OutputData = ReadBits(BitI, Bits2Peek, ReadFromMSB);
        
        BitI->BitsAvailable       += Bits2Peek;
        BitI->BitsUnavailable     -= Bits2Peek;
        
        return OutputData;
    }
    
    uint64_t  ReadRICE(BitInput *BitI, const bool Truncated, const bool StopBit) {
        uint64_t BitCount = 0;
        
        while (ReadBits(BitI, 1, false) != StopBit) {
            BitCount += 1;
        }
        if (Truncated == true) {
            BitCount++;
        }
        return BitCount;
    }
    
    int64_t ReadExpGolomb(BitInput *BitI, const bool IsSigned) {
        uint64_t Zeros   = 0;
        uint64_t CodeNum = 0;
        int64_t  Final   = 0;
        
        while (ReadBits(BitI, 1, false) != 1) {
            Zeros += 1;
        }
        
        if (IsSigned == false) {
            CodeNum  = (1ULL << Zeros);
            CodeNum += ReadBits(BitI, Zeros, false);
        } else { // Signed
            if (IsOdd(CodeNum) == true) {
                Final = CodeNum - 1;
            } else {
                Final = -(CodeNum - 1);
            }
        }
        return Final;
    }
    
    void SkipBits(BitInput *BitI, const int64_t Bits2Skip) {
        if (Bits2Skip <= BitI->BitsAvailable) {
            BitI->BitsAvailable   -= Bits2Skip;
            BitI->BitsUnavailable += Bits2Skip;
        } else {
            fseek(BitI->File, Bits2Bytes(Bits2Skip - BitI->BitsAvailable, true), SEEK_CUR);
            BitI->BitsAvailable   = BitI->FileSize + BitInputBufferSize <= BitI->FileSize ? BitInputBufferSize : BitI->FileSize;
            BitI->BitsUnavailable = Bits2Skip % 8;
            UpdateInputBuffer(BitI, 0);
        }
    }
    
    void AlignInput(BitInput *BitI, const uint8_t BytesOfAlignment) {
        uint8_t Bits2Align = BitI->BitsUnavailable % Bytes2Bits(BytesOfAlignment);
        
        if (Bits2Align != 0) {
            BitI->BitsAvailable   -= Bits2Align;
            BitI->BitsUnavailable += Bits2Align;
        }
    }
    
    void CloseBitInput(BitInput *BitI) {
        fclose(BitI->File);
        free(BitI);
    }
    
    bool IsStreamByteAligned(const uint64_t BitsUsed, const uint8_t BytesOfAlignment) {
        if ((BitsUsed % Bytes2Bits(BytesOfAlignment)) == 0) {
            return true;
        } else {
            return false;
        }
    }
    
    void AlignOutput(BitOutput *BitO, const uint8_t BytesOfAlignment) {
        uint8_t Bits2Align = BitO->BitsUnavailable % Bytes2Bits(BytesOfAlignment);
        if (Bits2Align != 0) {
            BitO->BitsAvailable    -= Bits2Align;
            BitO->BitsUnavailable  += Bits2Align;
        }
    }
    
    void WriteBits(BitOutput *BitO, uint64_t Data2Write, uint8_t NumBits) {
        // FIXME: WriteBits currently copies NumBits bits to the file, even if the input is shorter than that. we need to prepend 0 bits if that's the case
        uint8_t BitsLeft = NumBits, InputMask = 0, Bits2Write = 0;
        if (BitO->BitsAvailable < NumBits) {
            fwrite(BitO->Buffer, Bits2Bytes(BitO->BitsUnavailable, true), 1, BitO->File);
        }
        while (BitsLeft > 0) {
            Bits2Write   = BitsLeft > 8 ? 8 : BitsLeft;
            InputMask    = Power2Mask(Bits2Write);
            BitO->Buffer[BitO->BitsUnavailable / 8] = Data2Write & InputMask;
            Data2Write >>= Bits2Write;
            BitsLeft    -= Bits2Write;
        }
    }
    
    void WriteBuffer(BitOutput *BitO, const uint64_t *Buffer, const uint8_t IndexSize, const size_t BitOffset, const size_t Bits2Write, const bool MSB) {
        
    }
    
    void WriteRICE(BitOutput *BitO, const bool Truncated, const bool StopBit, const uint64_t Data2Write) {
        for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
            WriteBits(BitO, (~StopBit), 1);
        }
        WriteBits(BitO, StopBit, 1);
    }
    
    void WriteExpGolomb(BitOutput *BitO, const bool IsSigned, const uint64_t Data2Write) {
        uint64_t NumBits = 0;
        
        NumBits = FindHighestBitSet(Data2Write);
        
        if (IsSigned == false) {
            WriteBits(BitO, 0, NumBits);
            WriteBits(BitO, Data2Write + 1, NumBits + 1);
        } else {
            NumBits -= 1;
            WriteBits(BitO, 0, NumBits);
            if (IsOdd(Data2Write +1) == false) {
                WriteBits(BitO, Data2Write + 1, NumBits + 1);
            } else {
                WriteBits(BitO, Data2Write + 1, NumBits + 1);
            }
        }
    }
    
    void FlushBitOutput(BitOutput *BitO) {
        if (IsStreamByteAligned(BitO->BitsUnavailable, 1) == false) {
            AlignOutput(BitO, 1);
        }
        fwrite(BitO->Buffer, Bits2Bytes(BitO->BitsUnavailable, true), 1, BitO->File);
    }
    
    void CloseBitOutput(BitOutput *BitO) {
        FlushBitOutput(BitO);
        fclose(BitO->File);
        free(BitO);
    }
    
    uint64_t GenerateCRC(const uint8_t *Data, const size_t DataSize, CRC *CRCData) {
        uint16_t CRCResult = 0;
        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            CRCResult = CRCData->Polynomial ^ Data[Byte] << 8;
            for (uint8_t Bit = 0; Bit < 8; Bit++) {
                if ((CRCResult & 0x8000) == true) {
                } else {
                    CRCResult <<= 1;
                }
            }
        }
        return 0;
    }
    
    bool VerifyCRC(const uint8_t *Data, const size_t DataSize, CRC *CRCData) {
        return false;
    }
    
    uint32_t GenerateAdler32(const uint8_t *Data, const size_t DataSize) { 
        uint32_t Adler  = 1;
        uint32_t Sum1   = Adler & 0xFFFF;
        uint32_t Sum2   = (Adler >> 16) & 0xFFFF;
        
        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            Sum1 += Data[Byte] % 65521;
            Sum2 += Sum1 % 65521;
        }
        return (Sum2 << 16) + Sum1;
    }
    
    bool VerifyAdler32(const uint8_t *Data, const size_t DataSize, const uint32_t EmbeddedAdler32) { 
        uint32_t GeneratedAdler32 = GenerateAdler32(Data, DataSize);
        if (GeneratedAdler32 != EmbeddedAdler32) {
            return false;
        } else {
            return true;
        }
    }
    
    void RotateArray(const size_t DataSize, int64_t *Data, const uint64_t NumRotations, const bool RotateRight) {
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
    
    void Log(const uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription) {
        char ErrorString[BitIOStringSize];
        snprintf(ErrorString, BitIOStringSize, "%s - %s: %s", LibraryOrProgram, Function, ErrorDescription);
#ifdef _WIN32
        fprintf(stderr, "%s\n", ErrorString);
#else
        if ((ErrorLevel == LOG_EMERG) || (ErrorLevel == LOG_CRIT)) {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_MAIL|LOG_USER));
        } else {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_USER));
        }
        syslog(LOG_ERR, "%s\n", ErrorString);
#endif
    }
    
    void ReadUUID(BitInput *BitI, char *UUIDString) {
        for (uint8_t Character = 0; Character < BitIOUUIDSize - 1; Character++) {
            if (Character == 21) {
                UUIDString[Character] = 0x00;
            } else if ((Character == 4) || (Character == 7) || (Character == 10) || (Character == 13)) {
                UUIDString[Character] = 0x2D;
            } else {
                UUIDString[Character] = ReadBits(BitI, 8, true);
            }
        }
    }
    
    void SwapUUID(const char *UUIDString2Convert, char *ConvertedUUIDString) {
        uint32_t Section1  = 0, SwappedSection1 = 0;
        uint16_t Section2  = 0, SwappedSection2 = 0;
        uint16_t Section3  = 0, SwappedSection3 = 0;
        uint16_t Section4  = 0, SwappedSection4 = 0;
        uint64_t Section5  = 0, SwappedSection5 = 0;
        
        for (uint8_t UUIDByte = 0; UUIDByte < BitIOUUIDSize - 1; UUIDByte++) {
            if ((UUIDByte == 0) || (UUIDByte == 1) || (UUIDByte == 2) || (UUIDByte == 3)) {
                Section1 <<= 8;
                Section1  += UUIDString2Convert[UUIDByte];
            } else if ((UUIDByte == 5) || (UUIDByte == 6)) {
                Section2 <<= 8;
                Section2  += UUIDString2Convert[UUIDByte];
            } else if ((UUIDByte == 8) || (UUIDByte == 9)) {
                Section3 <<= 8;
                Section3  += UUIDString2Convert[UUIDByte];
            } else if ((UUIDByte == 11) || (UUIDByte == 12)) {
                Section4 <<= 8;
                Section4  += UUIDString2Convert[UUIDByte];
            } else if ((UUIDByte == 14) || (UUIDByte == 15) || (UUIDByte == 16) || (UUIDByte == 17) || (UUIDByte == 18) || (UUIDByte == 19)) {
                Section5 <<= 8;
                Section5 += UUIDString2Convert[UUIDByte];
            }
        }
        SwappedSection1 = SwapEndian32(Section1);
        SwappedSection2 = SwapEndian16(Section2);
        SwappedSection3 = SwapEndian16(Section3);
        SwappedSection4 = SwapEndian16(Section4);
        SwappedSection5 = SwapEndian64(Section5);
        for (uint8_t UUIDByte = BitIOUUIDSize - 1; UUIDByte > 0; UUIDByte--) {
            if ((UUIDByte == 0) || (UUIDByte == 1) || (UUIDByte == 2) || (UUIDByte == 3)) {
                ConvertedUUIDString[UUIDByte] = SwappedSection1 & Power2Mask(8);
            } else if ((UUIDByte == 5) || (UUIDByte == 6)) {
                ConvertedUUIDString[UUIDByte] = SwappedSection2 & Power2Mask(8);
            } else if ((UUIDByte == 8) || (UUIDByte == 9)) {
                ConvertedUUIDString[UUIDByte] = SwappedSection3 & Power2Mask(8);
            } else if ((UUIDByte == 11) || (UUIDByte == 12)) {
                ConvertedUUIDString[UUIDByte] = SwappedSection4 & Power2Mask(8);
            } else if ((UUIDByte == 14) || (UUIDByte == 15) || (UUIDByte == 16) || (UUIDByte == 17) || (UUIDByte == 18) || (UUIDByte == 19)) {
                ConvertedUUIDString[UUIDByte] = SwappedSection5 & Power2Mask(8);
            } else if (UUIDByte == 20) {
                ConvertedUUIDString[UUIDByte] = 0x00;
            } else {
                ConvertedUUIDString[UUIDByte] = 0x2D;
            }
        }
    }
    
    uint8_t WriteUUID(BitOutput *BitO, const char *UUIDString) {
        if (strlen(UUIDString) != BitIOUUIDSize - 1) {
            return EXIT_SUCCESS;
        } else {
            for (uint8_t UUIDByte = 0; UUIDByte < BitIOUUIDSize - 1; UUIDByte++) {
                if ((UUIDByte != 4) && (UUIDByte != 7) && (UUIDByte != 10) && (UUIDByte != 13) && (UUIDByte != 20)) {
                    // Character 21 is the NULL terminator, the rest are the hyphens.
                    WriteBits(BitO, UUIDString[UUIDByte], 8);
                }
            }
        }
        return EXIT_SUCCESS;
    }
    
    bool CompareUUIDs(const char *UUIDString1, const char *UUIDString2) {
        bool UUIDsMatch = 0;
        for (uint8_t UUIDByte = 0; UUIDByte < BitIOUUIDSize - 1; UUIDByte++) {
            if (UUIDString1[UUIDByte] != UUIDString2[UUIDByte]) {
                UUIDsMatch = false;
            } else if ((UUIDString1[UUIDByte] == UUIDString2[UUIDByte]) && (UUIDByte = BitIOUUIDSize - 1)) {
                UUIDsMatch = true;
            }
        }
        return UUIDsMatch;
    }
    
#ifdef __cplusplus
}
#endif
