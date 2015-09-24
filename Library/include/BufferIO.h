#include <assert.h>
#include <dirent.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#pragma once

#define BitIO_Version "0.1"

#define BufferSize 4096
#define BufferSizeInBits 32768

#ifdef __cplusplus
extern "C" {
#endif
    
    static const int BitMaskOffset[8] = {
        0xFF, 0x7F, 0x3F, 0x1F,
        0x0F, 0x07, 0x03, 0x01
    };
    
    enum Endian {
        Undetected = 0,
        Big        = 1,
        Little     = 2,
    };
    
    /*!
     *  Main structure for reading and writing data
     */
    struct BitIO {
        // Common
        enum Endian         NativeEndian; // SystemEndian
        // Input
        FILE                    *InputFP; // File to read from
        DIR                    *InputDir; // Folder to read from
        uint64_t             InputFPSize; // How large is the input file in bytes?
        uint8_t  InputBuffer[BufferSize]; // the actual InputBuffer
        uint64_t          InputBitOffset; // Number of bits read
        uint64_t      InputBitsRemaining; // BufferSizeInBits - Number of bits read
        
        // Output
        FILE                   *OutputFP; // File to write to
        DIR                   *OutputDir; // Folder to write to
        uint8_t OutputBuffer[BufferSize]; // Buffer
        uint64_t         OutputBitOffset; // Number of bits written
        uint64_t     OutputBitsRemaining; // Number of open bits remaining in the buffer
    } BitIO;
    
    struct UUID { /// TODO: Should I include room for hyphens?
        uint64_t TimeLow;
        uint64_t TimeMid;
        uint64_t TimeHigh_Version;
        uint64_t ClockHigh_Version;
        uint64_t ClockLow;
        uint8_t  Node[6];
        uint8_t  Dash[4]; // room for hyphens
    };
    
    //void Init_BitIO(const char *InputFile, const char *InputMode, const char *OutputFile, const char *OutputMode);
    uint64_t FileSequence(int argc, char *argv, int WhichArg);
    uint64_t LookupBitMask(uint64_t WhichByte);
    uint64_t WriteBits(uint64_t Bits2Write);
    uintptr_t ReadUUID(void);
    void Deallocate_BitIO(void);
    void OptionParser(int argc, char *argv, const char *OptString);
    void SkipBits(uint64_t Bits2Skip);
    
    /*!
     *  Internal: Byte offset in buffer
     */
    uint64_t ByteOffset(uint64_t Bits) {
        return Bits / 8;
    };
    
    /*!
     *  Internal: Number of bits to skip in the current byte, to be used with ByteOffset in bit reading and writing
     */
    uint64_t BitOffset(uint64_t Bits) {
        uint64_t x = 0;
        if (Bits < 8) {
            x = Bits;
        } else {
            x = Bits % 8;
        }
        return x;
    };
    
    /*!
     *  Internal: Convert bits to bytes, round down if not divisible by 8
     */
    uint64_t Bits2Bytes(uint64_t Bits) {
        return Bits / 8;
    };
    
    /*!
     *  Skip X number of bits
     */
    void SkipBits(uint64_t Bits2Skip) {
        BitIO.InputBitOffset     += Bits2Skip;
        BitIO.InputBitsRemaining -= Bits2Skip;
    };
    
    /*!
     *  Writes BitIO.OutputBitOffset + padding bits to BitIO.OutputFP
     */
    void FlushBits2OutputFile(void) {
        if ((BitIO.OutputBitOffset % 8) == 0) {
            fwrite(BitIO.OutputBuffer, 1, ByteOffset(BitIO.OutputBitOffset), BitIO.OutputFP);
        } else {
            uint64_t PaddingBits = BitOffset(BitIO.OutputBitOffset);
            
            for (uint64_t i = ByteOffset(BitIO.OutputBitOffset); i < 2; i++) {
                BitIO.OutputBuffer[i] = BitIO.OutputBuffer[i] & BitMaskOffset[PaddingBits]; // FixME: Does this actually work?
            }
        }
    };
    
    /*!
     *  Initalize BitIO for use. this must be done for each and every file to be read or written to.
     *  can only read or write to 1 file sequentially
     */
    void Init_BitIO(const char *InputFile, const char *InputMode, const char *OutputFile, const char *OutputMode) {
        uint64_t BytesRead = 0;
        
        BitIO.InputFP  = fopen(InputFile, InputMode);
        BitIO.OutputFP = fopen(OutputFile, OutputMode);
        
        if (BitIO.InputFP == NULL) {
            fprintf(stderr, "Init_BitIO: InputFile is NULL\n");
        }
        if (BitIO.OutputFP == NULL) {
            fprintf(stderr, "Init_BitIO: OutputFile couldn't be opened\n");
        }
        
        fseek(BitIO.InputFP, 0, SEEK_END);
        BitIO.InputFPSize = ftell(BitIO.InputFP);
        fseek(BitIO.InputFP, 0, SEEK_SET);
        
        memset(BitIO.InputBuffer, 0, 4096); ///* zero buffer before use.
        
        if (BitIO.InputFPSize >= BufferSize) { ///* buffer will be full.
            BytesRead = fread(BitIO.InputBuffer, 1, BufferSize, BitIO.InputFP);
            BitIO.InputBitOffset     = 0;
            BitIO.InputBitsRemaining = BufferSizeInBits; // Previously BufferLength, FixME: Verify this is still correct
            if (BytesRead < BufferSize) {
                // Retry reading in the rest of the missed data
                fprintf(stderr, "Init_BitIO read %llu of %d bytes\n", BytesRead, BufferSize);
            }
        } else {
            BytesRead = fread(BitIO.InputBuffer, 1, BitIO.InputFPSize, BitIO.InputFP);
            BitIO.InputBitOffset = 0;
            BitIO.InputBitsRemaining = BufferSizeInBits;
            if (BytesRead < BitIO.InputFPSize) {
                // Retry reading in the rest of the missed data
                fprintf(stderr, "Init_BitIO read %llu of %llu bytes\n", BytesRead, BitIO.InputFPSize);
            }
        }
        
        // Endian Detection
        int Number = 1;
        if (*(char *)&Number == 1) {
            BitIO.NativeEndian = Little;
        } else {
            BitIO.NativeEndian = Big;
        }
    };
    
    /*!
     *  Internal: Converts BitsRemaining into a bitmask
     */
    uint64_t LookupBitMask(uint64_t WhichByte) {
        return BitMaskOffset[BitIO.InputBitsRemaining - (WhichByte * 8) % 8];
    };
    
    /*!
     *  Internal: Multiply bytes by 8 to get number of bits
     */
    uint64_t Bytes2Bits(uint64_t Bytes) {
        return Bytes * 8;
    };
    
    /*!
     *  Internal: to count the number of bits in X
     */
    uint64_t CountBits(uint64_t Bits2Count) {
        uint64_t NumberOfBits = 0;
        while (Bits2Count) {
            Bits2Count &= (Bits2Count - 1);
            NumberOfBits++;
        }
        return NumberOfBits;
    };
    
    /*!
     *  Internal: Remove leading bits
     */
    int64_t PruneBits(int64_t Data2Prune) {
        uint64_t NumberOfBits = CountBits(Data2Prune);
        uint64_t PrunedData = 0;
        PrunedData = Data2Prune << (64 - NumberOfBits);
        return PrunedData;
    };
    
    /*!
     *  Internal: Converts an Signed int to a Unsigned int.
     *  Keep in mind that the internal representation in BitIO is unsigned.
     */
    uint64_t Signed2Unsigned(int64_t Signed) {
        return (uint64_t) Signed;
    };
    
    /*!
     *  Internal: Converts an Unsigned int to a Signed int.
     *  Keep in mind that the internal representation in BitIO is unsigned.
     */
    int64_t Unsigned2Signed(uint64_t Unsigned) {
        return (uint64_t) Unsigned;
    };
    
    /*!
     *  Internal: Swap endian of 16 byte integers
     */
    uint16_t SwapEndian16(uint16_t X) {
        return ((X & 0xFF00) >> 8) | ((X & 0x00FF) << 8);
    };
    
    /*!
     *  Internal: Swap endian of 32 bit integers
     */
    uint32_t SwapEndian32(uint32_t X) {
        return ((X & 0xFF000000) >> 24) | ((X & 0x00FF0000) >> 8) | ((X & 0x0000FF00) << 8) | ((X & 0x000000FF) << 24);
    };
    
    /*!
     *  Internal: Swap endian of 64 bit integers
     */
    uint64_t SwapEndian64(uint64_t X) {
        return (((X & 0xFF00000000000000) >> 56) | ((X & 0x00FF000000000000) >> 40) |
                ((X & 0x0000FF0000000000) >> 24) | ((X & 0x000000FF00000000) >> 8)  |
                ((X & 0x00000000FF000000) << 8)  | ((X & 0x0000000000FF0000) << 24) |
                ((X & 0x000000000000FF00) << 40) | ((X & 0x00000000000000FF) << 56));
    };
    
    /*!
     *  Empties the InputBuffer, writes and empties the OutputBuffer (and pads to the next byte if nessacary)
     *  For use when changing files, or when skipping backwards
     */
    void FlushBitIO(void) {
        FlushBits2OutputFile();
        fclose(BitIO.InputFP);
        fclose(BitIO.OutputFP);
        BitIO.InputBitOffset      = 0;
        BitIO.InputBitsRemaining  = 0;
        BitIO.InputFPSize         = 0;
        BitIO.OutputBitOffset     = 0;
        BitIO.OutputBitsRemaining = 0;
        for (uint64_t i = 0; i < BufferSize; i++) { // Zero buffers
            BitIO.InputBuffer[i]  = 0;
            BitIO.OutputBuffer[i] = 0;
        }
    };
    
    /*!
     *  Deallocate BitIO
     */
    void Deallocate_BitIO(void) {
        free(&BitIO);
        fclose(BitIO.InputFP);
        fclose(BitIO.OutputFP);
        closedir(BitIO.InputDir);
        closedir(BitIO.OutputDir);
    };
    
    /*!
     *  Internal: handles file sequences
     */
    uint64_t FileSequence(int argc, char *argv, int WhichArg) {
        // small function to return the filename sequence specifier, aka take %03d and return leading 0, 3 digit, decimal, etc.
        if (argv) {
        }
        strcmp(&argv[WhichArg -1], "%03d");
        
        return 0;
    };
    
    /*!
     *  Internal: parses command line flags
     *  Replace all of this with calls to Sscanf
     */
    /*
    void OptionParser(int argc, char *argv, const char *OptString) {
        int GetOptCount = 0, InputArg = 0, OutputArg = 0;
        while ((GetOptCount = getopt(argc, &argv, OptString) != -1)) {
            switch (GetOptCount) {
                case 'i':
                    // Input
                    InputArg  = GetOptCount;
                    break;
                case 'I':
                    // Input
                    InputArg  = GetOptCount;
                    break;
                case 'f':
                    // Folder input
                    InputArg  = GetOptCount;
                    FileSequence(argc, argv, InputArg);
                    BitIO.InputDir;
                    
                    break;
                case 'F':
                    // Folder input
                    InputArg  = GetOptCount;
                    break;
                case 'o':
                    // Output
                    OutputArg = GetOptCount;
                    break;
                case 'O':
                    // Output
                    OutputArg = GetOptCount;
                    break;
                default:
                    break;
            }
            Init_BitIO(&argv[InputArg], "rb", &argv[OutputArg], "wb");
        }
    };
     */
    
    /*!
     *  Reads bits into buffer, and breaks them into the requested bits
     *  Reading backwards isn't possible so don't even try.
     *  If you need to skip backwards use fseek then flush BitIO
     *
     *  I will probably have to endian swap all rdata read in, to get it to conform to the big endian format of the array
     *
     *  IF THERE IS LESS THAN OR EXACTLY 64 BITS UNTIL THE BUFFER HAS BEEN READ OUT, RE-READ DATA FROM THE FILE.
     *  try to keep it from locking up during the next read, by preemptivly reading in the required data. Do the same for WriteBits.
     *
     *  FIXME: bits to bytes calculation: (bits + 7) >> 3
     */
    uint64_t ReadBits(uint64_t Bits2Read) {
        //uint64_t BytesRead = 0, SelectedBits = 0, ErrorCode = 0, OutputData = 0;
        
        uint64_t OutputData = 0;
        
        if (Bits2Read <= 0) {
            fprintf(stderr, "BitIO: You requested too few bits: %llu\n", Bits2Read);
        } else if (Bits2Read > 64) {
            fprintf(stderr, "BitIO: You requested too many bits: %llu\n", Bits2Read);
        } else if (Bits2Read > BitIO.InputBitsRemaining) {
            // Update buffer
        } else {
            for (uint64_t i = BitIO.InputBitOffset / 8; i < (BitIO.InputBitOffset + Bits2Read) / 8; i++) { // BYTE boundary
                /* Mask = 0, A = 0;
                 A       = BitIO.InputBitsRemaining;
                 Mask    = A % 8;
                 BitMask = BitMaskOffset[Mask];
                 A      -= 8;
                 
                 if (BitOffset(BitIO.InputBitsRemaining) == 0) { // First byte, due to masking operations
                 OutputData = BitIO.InputBuffer[i];
                 } else {
                 for (uint64_t y = BitOffset(BitIO.InputBitsRemaining); y < 8; y++) { // BIT boundary // FIXME: Verify correctness
                 OutputData = BitIO.InputBuffer[i] & BitMaskOffset[y]; // LookupBitMask(y);
                 }
                 }
                 */
                uint8_t BitMask = BitMaskOffset[i % 8];
                OutputData += BitIO.InputBuffer[i] & BitMask;
            }
            BitIO.InputBitsRemaining -= Bits2Read;
            BitIO.InputBitOffset     += Bits2Read;
        }
        return OutputData;
    };
    
    /*!
     *  Write bits to buffer, and when full write OutputBuffer to disk
     */
    uint64_t WriteBits(uint64_t Bits2Write) {
        uint64_t BitsWritten = 0;
        
        // convert Bits2Write to array
        for (int i = 0; i < 8; i++) {
            BitIO.OutputBuffer[i] = (Bits2Write & 0x00000000000000FF);
        }
        
        uint64_t Times2Loop = Bits2Bytes(Bits2Write);
        uint64_t ArrayOffset = ByteOffset(BitIO.OutputBitsRemaining); // BitIO.OutputBitsRemaining
        
        for (uint64_t i = ArrayOffset; i < Times2Loop; i++) {
            BitIO.OutputBuffer[i] = (Bits2Write & 0x00FF);
            Bits2Write >>= 8;
        }
        fwrite(BitIO.OutputBuffer, 1, 4096, BitIO.OutputFP);
        for (int i = 0; i < Bits2Bytes(Bits2Write); i++) {
            uint64_t SelectedBits = Bits2Write << 8;
            BitIO.OutputBuffer[i] = SelectedBits;
        }
        
        if (BitIO.OutputBitsRemaining == BufferSizeInBits) { // if Outputbuffer is full, write data to file, and reset UnwrittenBits
            fwrite(BitIO.OutputBuffer, 1, 4096, BitIO.OutputFP);
            BitIO.OutputBitsRemaining -= BufferSizeInBits;
        }
        
        return BitsWritten;
    };
    
    /*!
     *  Returns pointer to UUID struct containing UUID
     */
    uintptr_t ReadUUID(void) {
        uint64_t BytesActuallyRead = 0;
        
        struct UUID UUID;
        
        UUID.TimeLow              = ReadBits(32);
        UUID.TimeMid              = ReadBits(16);
        UUID.TimeHigh_Version     = ReadBits(16);
        UUID.ClockHigh_Version    = ReadBits(8);
        UUID.ClockLow             = ReadBits(8);
        BytesActuallyRead         = fread(UUID.Node, 1, 8, BitIO.InputFP);
        
        BitIO.InputBitOffset     += Bytes2Bits(BytesActuallyRead);
        BitIO.InputBitsRemaining -= Bytes2Bits(BytesActuallyRead);
        
        return (uintptr_t) &UUID;
    };
    
#ifdef __cplusplus
}
#endif
