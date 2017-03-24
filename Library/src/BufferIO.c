#include "../include/BitIO.h"

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _POSIX_VERSION
#include <syslog.h>
#include <unistd.h>
#elif defined _WIN32
#include <winbase.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp stricmp
#endif
    
    /*!
     @typedef        BitInput
     @abstract                         "Contains variables and buffers for reading bits".
     @remark                           "The default internal representation in BitIO is unsigned, Big Endian".
     @constant       File              "Input file to read bits from".
     @constant       FileSize          "Size of File in bytes".
     @constant       FilePosition      "Current byte in the file".
     @constant       BitsUnavailable   "Number of previously read bits in Buffer".
     @constant       BitsAvailable     "Number of bits available for reading".
     @constant       SystemEndian      "Endian of the running system".
     @constant       Buffer            "Buffer of data from File".
     */
    typedef struct BitInput {
        FILE        *File;
        size_t       FileSize;
        size_t       FilePosition;
        size_t       BitsUnavailable;
        size_t       BitsAvailable;
        unsigned     SystemEndian:2;
        uint8_t      Buffer[BitInputBufferSize];
    } BitInput;
    
    /*!
     @typedef        BitOutput
     @abstract                         "Contains variables and buffers for writing bits".
     @remark                           "The default internal representation in BitOutput is unsigned".
     @constant       File              "Input file to read bits from".
     @constant       BitsUnavailable   "Number of previously read bits in Buffer".
     @constant       BitsAvailable     "Number of bits available for writing".
     @constant       SystemEndian      "Endian of the running system".
     @constant       Buffer            "Buffer of BitIOBufferSize bits from File".
     */
    typedef struct BitOutput {
        FILE        *File;
        size_t       BitsUnavailable;
        size_t       BitsAvailable;
        uint8_t      SystemEndian;
        uint8_t      Buffer[BitOutputBufferSize];
        FILE        *LogFile;
    } BitOutput;
    
    typedef struct LinkedList {
        uint16_t           Value;
        struct LinkedList *Next;
    } LinkedList;
    
    uint16_t SwapEndian16(const uint16_t Data2Swap) {
        return ((Data2Swap & 0xFF00) >> 8) | ((Data2Swap & 0x00FF) << 8);
    }
    
    uint32_t SwapEndian32(const uint32_t Data2Swap) { // 0x0000011C, 0xC1010000
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
    
    uint8_t BitsRemainingInByte(const uint64_t BitsAvailable) {
        return BitsAvailable > 8 ? 8 : BitsAvailable;
    }
    
    uint64_t BytesRemainingInFile(BitInput *BitI) {
        return BitI->FilePosition - BitI->FileSize;
    }
    
    uint64_t GetInputFileSize(BitInput *BitI) {
        return BitI->FileSize;
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
        if ((Exponent <= 0) || (Exponent > 64)) {
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
        printf("%s by %s ©%s: %s, Released under the %s license\n\n", CMD->Name, CMD->Author, CMD->Copyright, CMD->Description, CMD->License);
        printf("Options:\n");
        for (uint8_t Option = 0; Option < CMD->NumSwitches; Option++) {
            printf("%s\t", CMD->Switch[Option]->Flag);
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
                char *SingleDash = calloc(1, strlen(CMD->Switch[Switch]->Flag + 2));
                snprintf(SingleDash, sizeof(SingleDash), "-%s\n", CMD->Switch[Switch]->Flag);
                
                char *DoubleDash = calloc(1, strlen(CMD->Switch[Switch]->Flag + 3));
                snprintf(DoubleDash, sizeof(DoubleDash), "--%s\n", CMD->Switch[Switch]->Flag);
                
                char *Slash      = calloc(1, strlen(CMD->Switch[Switch]->Flag + 2));
                snprintf(Slash, sizeof(Slash), "/%s\n", CMD->Switch[Switch]->Flag);
                
                if (strcasecmp(SingleDash, argv[Argument]) == 0 || strcasecmp(DoubleDash, argv[Argument]) == 0 || strcasecmp(Slash, argv[Argument]) == 0) {
                    CMD->Switch[Switch]->SwitchFound = true;
                    if (CMD->Switch[Switch]->Resultless == false) {
                        char *SwitchResult = calloc(sizeof(BitIOStringSize), 1);
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
            UserBits               = BitsRemainingInByte(Bits);
            Bits                   = SystemBits >= UserBits  ? UserBits : SystemBits;
            Mask2Shift             = ReadFromMSB ? BitI->BitsAvailable % 8 : 0;
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
    
    BitInput *InitBitInput(void) {
        BitInput *BitI = calloc(sizeof(BitInput), 1);
        return BitI;
    }
    
    bool IsInputStreamByteAligned(BitInput *BitI, const uint8_t BytesOfAlignment) {
        if ((BitI->BitsUnavailable % Bytes2Bits(BytesOfAlignment)) == 0) {
            return true;
        } else {
            return false;
        }
    }
    
    void CloseBitInput(BitInput *BitI) {
        fclose(BitI->File);
        free(BitI);
    }
    
    BitOutput *InitBitOutput(void) {
        BitOutput *BitO = calloc(sizeof(BitOutput), 1);
        return BitO;
    }
    
    bool IsOutputStreamByteAligned(BitOutput *BitO, const uint8_t BytesOfAlignment) {
        if ((BitO->BitsUnavailable % Bytes2Bits(BytesOfAlignment)) == 0) {
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
    
    void WriteBits(BitOutput *BitO, const uint64_t Data2Write, uint8_t NumBits, const bool WriteFromMSB) {
        // FIXME: WriteBits currently copies NumBits bits to the file, even if the input is shorter than that. we need to prepend 0 bits if that's the case
        
        uint8_t Bits2Write2BufferByte, Bits2ShiftMask, Mask, Bits2Write;
        
        if (BitO->BitsAvailable < NumBits) {
            fwrite(BitO->Buffer, Bits2Bytes(BitO->BitsUnavailable, true), 1, BitO->File);
        }
        
        // so we're trying to write 9 bits, 0x1FF.
        // There are only 5 bits available in the current byte.
        // So, that means we have to pull out 5 bits from Data2Write, and pop it into BitO->Buffer.
        // The Mask should be 0x1F if LSBfirst, or 0xF8 if MSB first.
        // The buffer's representation is MSB first.
        // if we're supposed to write this data LSB first we need to shift it after extraction to get it ready to be applied.
        while (NumBits > 0) {
            Bits2Write2BufferByte  = 8 - (BitO->BitsAvailable % 8); // extract 5 bits from the buffer
            Bits2ShiftMask         = WriteFromMSB ? BitO->BitsAvailable % 8 : 0;
            Mask                   = Power2Mask(Bits2Write2BufferByte) << Bits2ShiftMask; // 0x1F
            BitO->Buffer[Bits2Bytes(BitO->BitsAvailable, false)] = Data2Write & Mask;
            NumBits               -= Bits2Write2BufferByte;
            BitO->BitsAvailable   -= Bits2Write2BufferByte;
            BitO->BitsUnavailable += Bits2Write2BufferByte;
        }
    }
    
    void WriteRICE(BitOutput *BitO, const bool Truncated, const bool StopBit, const uint64_t Data2Write, const bool WriteFromMSB) {
        for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
            WriteBits(BitO, (~StopBit), 1, WriteFromMSB);
        }
        WriteBits(BitO, StopBit, 1, WriteFromMSB);
    }
    
    void WriteExpGolomb(BitOutput *BitO, const bool IsSigned, const uint64_t Data2Write, const bool WriteFromMSB) {
        uint64_t NumBits = 0;
        
        NumBits = FindHighestBitSet(Data2Write);
        
        if (IsSigned == false) {
            WriteBits(BitO, 0, NumBits, WriteFromMSB);
            WriteBits(BitO, Data2Write + 1, NumBits + 1, WriteFromMSB);
        } else {
            NumBits -= 1;
            WriteBits(BitO, 0, NumBits, WriteFromMSB);
            if (IsOdd(Data2Write +1) == false) {
                WriteBits(BitO, Data2Write + 1, NumBits + 1, WriteFromMSB);
            } else {
                WriteBits(BitO, Data2Write + 1, NumBits + 1, WriteFromMSB);
            }
        }
    }
    
    void FlushOutputBuffer(BitOutput *BitO, const size_t Bytes2Flush) {
        size_t BytesWritten = 0;
        BytesWritten = fwrite(BitO->Buffer, 1, Bytes2Flush, BitO->File);
        if (BytesWritten != Bytes2Flush) {
            Log(LOG_EMERG, "libBitIO", "FlushOutputBuffer", "Wrote %d bytes out of %d", BytesWritten, Bytes2Flush);
        }
    }
    
    void CloseBitOutput(BitOutput *BitO) {
        if (IsStreamByteAligned(BitO->BitsUnavailable, 1) == false) {
            AlignOutput(BitO, 1);
        }
        fwrite(BitO->Buffer, Bits2Bytes(BitO->BitsUnavailable, true), 1, BitO->File);
        fclose(BitO->File);
        free(BitO);
    }
    
    uint64_t GenerateCRC(const uint8_t *Data2CRC, const size_t Data2CRCSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit) {
        uint16_t CRCResult = 0;
        for (uint64_t Byte = 0; Byte < Data2CRCSize; Byte++) {
            CRCResult = ReciprocalPoly ^ Data2CRC[Byte] << 8;
            for (uint8_t Bit = 0; Bit < 8; Bit++) {
                if ((CRCResult & 0x8000) == true) {
                } else {
                    CRCResult <<= 1;
                }
            }
        }
        return 0;
    }
    
    bool VerifyCRC(const uint8_t *Data2CRC, const size_t Data2CRCSize, const uint64_t RecipricalPoly, const uint8_t PolySize, const uint64_t PolyInit, const uint64_t PrecomputedCRC) {
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
    
    void Log(const uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription, ...) {
        char ErrorString[BitIOStringSize];
        char VariadicArguments[BitIOStringSize];
        
        va_list ExtraArguments;
        va_start(ExtraArguments, ErrorDescription);
        vsnprintf(VariadicArguments, BitIOStringSize, "%s", ExtraArguments);
        va_end(ExtraArguments);
        
        snprintf(ErrorString, BitIOStringSize, "%s - %s: %s - %s\n", LibraryOrProgram, Function, ErrorDescription, VariadicArguments);
#ifdef _WIN32
        uintptr_t *EventLog = RegisterEventSource(NULL, LibraryOrProgram);
        uint32_t ErrorCode = ReportEvent(EventLog, ErrorLevel, 1, 0xF000FFFF, NULL, 1, strlen(ErrorString), ErrorString, NULL);
        if (ErrorCode != 0) {
            fprintf(stderr, "BitIO - Log: Windows version of Logger failed\n");
            fprintf(stderr, ErrorString);
        }
        bool DeregisterSucceeded = DeregisterEventSource(EventLog);
        if (DeregisterSucceeded > 0) {
            fprintf(stderr, "BitIO - Log: Deregistering EventLog failed\n");
        }
#else
        if ((ErrorLevel == LOG_EMERG) || (ErrorLevel == LOG_CRIT)) {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_MAIL|LOG_USER));
        } else {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_USER));
        }
        syslog(LOG_ERR, "%s\n", ErrorString);
#endif
    }
    
    void ReadUUID(BitInput *BitI, uint8_t *UUIDString) {
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
    
    void SwapUUID(const uint8_t *UUIDString2Convert, uint8_t *ConvertedUUIDString) {
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
    
    uint8_t WriteUUID(BitOutput *BitO, const uint8_t *UUIDString) {
        if (strlen(UUIDString) != BitIOUUIDSize - 1) {
            return EXIT_SUCCESS;
        } else {
            for (uint8_t UUIDByte = 0; UUIDByte < BitIOUUIDSize - 1; UUIDByte++) {
                if ((UUIDByte != 4) && (UUIDByte != 7) && (UUIDByte != 10) && (UUIDByte != 13) && (UUIDByte != 20)) {
                    // Character 21 is the NULL terminator, the rest are the hyphens.
                    WriteBits(BitO, UUIDString[UUIDByte], 8, true);
                }
            }
        }
        return EXIT_SUCCESS;
    }
    
    bool CompareUUIDs(const uint8_t *UUIDString1, const uint8_t *UUIDString2) {
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
    
    CommandLineOptions *InitCommandLineOptions(void) {
        CommandLineOptions *CMD = calloc(sizeof(CommandLineOptions), 1);
        return CMD;
    }
    
    void CloseCommandLineOptions(CommandLineOptions *CMD) {
        free(CMD);
    }
    
    CommandLineSwitch *InitCommandLineSwitch(void) {
        CommandLineSwitch *Switch = calloc(sizeof(CommandLineSwitch), 1);
        return Switch;
    }
    
    CommandLineOptions *BatchInitCommandLineSwitches(CommandLineOptions *CMD, uint64_t NumSwitches) {
        CMD->NumSwitches = NumSwitches;
        for (uint64_t Switch2Init = 0; Switch2Init < NumSwitches; Switch2Init++) {
            CMD->Switch[Switch2Init] = calloc(sizeof(CommandLineSwitch), 1);
        }
        return CMD;
    }
    
    void CloseCommandLineSwitch(CommandLineSwitch *Switch) {
        free(Switch);
    }
    
    void SetCMDName(CommandLineOptions *CMD, const char *Name) {
        CMD->Name = Name;
    }
    
    void SetCMDDescription(CommandLineOptions *CMD, const char *Description) {
        CMD->Description = Description;
    }
    
    void SetCMDAuthor(CommandLineOptions *CMD, const char *Author) {
        CMD->Author = Author;
    }
    
    void SetCMDCopyright(CommandLineOptions *CMD, const char *Copyright) {
        CMD->Copyright = Copyright;
    }
    
    void SetCMDLicense(CommandLineOptions *CMD, const char *License) {
        CMD->License = License;
    }
    
    void SetSwitchFlag(CommandLineOptions *CMD, uint64_t SwitchNum, const char *Flag) {
        CMD->Switch[SwitchNum]->Flag = Flag;
    }
    
    void SetSwitchDescription(CommandLineOptions *CMD, uint64_t SwitchNum, const char *Description) {
        CMD->Switch[SwitchNum]->SwitchDescription = Description;
    }
    
    void SetSwitchResultStatus(CommandLineOptions *CMD, uint64_t SwitchNum, bool IsSwitchResultless) {
        CMD->Switch[SwitchNum]->Resultless = IsSwitchResultless;
    }
    
    char *GetSwitchResult(CommandLineOptions *CMD, uint64_t SwitchNum) {
        return CMD->Switch[SwitchNum]->SwitchResult;
    }
    
    bool IsSwitchPresent(CommandLineOptions *CMD, uint64_t Switch) {
        return CMD->Switch[Switch]->SwitchFound;
    }
    
#ifdef __cplusplus
}
#endif
