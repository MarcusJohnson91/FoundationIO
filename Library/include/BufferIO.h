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

#define BufferSize 4096
#define BufferSizeInBits 32768

#ifdef __cplusplus
extern "C" {
#endif
    
    enum ShiftDirection {
        Undefined = 0,
        Right     = 1,
        Left      = 2,
    } ShiftDirection;
    
    enum Endian {
        Undetected = 0,
        Big        = 1,
        Little     = 2,
    } Endian;
    
    struct BitIO {
        // Common
        enum Endian          NativeEndian; // SystemEndian
        // Input
        FILE                     *InputFP; // File to read from
        DIR                     *InputDir; // Folder to read from
        uint64_t              InputFPSize; // How large is the input file in bytes?
        uint8_t   InputBuffer[BufferSize]; // the actual InputBuffer
        uint64_t             BufferLength; // tells if the entire buffer is full, or 0 padded. bits or bytes?
        uint64_t           InputBitOffset;
        uint64_t       InputBitsRemaining;
        
        // Output
        FILE                    *OutputFP; // File to write to
        DIR                    *OutputDir;
        uint8_t  OutputBuffer[BufferSize]; // Buffer
        uint64_t          OutputBitOffset;
    } BitIO;
    
    struct UUID { /// TODO: Should I include room for hyphens?
        uint64_t TimeLow;
        uint64_t TimeMid;
        uint64_t TimeHigh_Version;
        uint64_t ClockHigh_Version;
        uint64_t ClockLow;
        uint8_t   Node[6];
    } UUID;
    
    bool MultipleOf8(uint64_t IsMultiple);
    
    int64_t Init_BitIO(char *InputFile, const char *InputMode, char *OutputFile, const char *OutputMode);
    int64_t UpdateInputBuffer(uint64_t Bits2Update);
    int64_t InvertSign(int64_t Number2Invert);
    int64_t PruneBits(int64_t Data2Prune);
    
    void OptionParser(int argc, char *argv, const char *OptString);
    void SkipBits(uint64_t Bits2Skip);
    void Deallocate_BitIO(void);
    
    uint64_t FileSequence(int argc, char *argv, int WhichArg);
    uint64_t BufferManagment(uint64_t BitsRequested);
    uint64_t ReadRICE(int Bits2Read, bool StopBit);
    uint64_t CountBits(uint64_t Bits2Count);
    uint64_t WriteBits(uint64_t Bits2Write);
    //uint64_t ReadBits(uint64_t Bits2Read);
    uint64_t Bytes2Bits(uint64_t Bytes);
    uint64_t Bits2Bytes(uint64_t Bits);
    //uint64_t InputBufferManager(void);
    uint16_t EndianSwap16(uint16_t X);
    uint32_t EndianSwap32(uint32_t X);
    uint64_t EndianSwap64(uint64_t X);
    uint64_t ReadPosition();
    
    uintptr_t ReadUUID();
    
#ifdef __cplusplus
}
#endif
