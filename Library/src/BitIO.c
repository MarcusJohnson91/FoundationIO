#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>
#else
#include <winsock.h>
#endif

#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    typedef struct BitBuffer {
        size_t             BitsAvailable;
        size_t             BitsUnavailable;
        uint8_t           *Buffer;
    } BitBuffer;
    
    typedef struct BitInput {
        FILE              *File;
        size_t             FileSize;
        size_t             FilePosition;
        uint8_t            SystemEndian:2;
        const char        *FormatSpecifier;
        uint8_t            FormatSpecifierBase:2;
        uint64_t           CurrentFileSpecifierNumber;
    } BitInput;
    
    typedef struct BitOutput {
        FILE              *File;
        size_t             FilePosition;
        uint8_t            SystemEndian:2;
        const char        *FormatSpecifier;
        uint8_t            FormatSpecifierBase:2;
        uint64_t           CurrentFileSpecifierNumber;
    } BitOutput;
    
    /*!
     @typedef           CommandLineSwitch
     @abstract                              "Contains the data to support a single switch".
     @remark                                "You MUST include the NULL padding at the end of @Switch".
     @constant          SwitchFound         "If the switch was found in argv, this will be set to true".
     @constant          IsThereAResult      "Is there a trailing option after the flag? if so, set to true".
     @constant          Flag                "Actual flag, WITHOUT dash(s) or backslash, Flags are case insensitive".
     @constant          FlagSize            "size of the flag in bytes".
     @constant          SwitchDescription   "Message to print explaining what the switch does".
     @constant          SwitchResult        "String to contain the result of this switch, NULL if not found or not included".
     @constant          DependsOn         "What switch is this one dependent on?".
     */
    typedef struct CommandLineSwitch {
        bool               SwitchFound;
        bool               IsThereAResult;
        const char        *Flag;
        size_t             FlagSize;
        const char        *SwitchDescription;
        const char        *SwitchResult;
        bool               IsDependent;
        uint64_t           DependsOn;
    } CommandLineSwitch;
    
    typedef struct CommandLineOptions {
        size_t             NumSwitches;
        uint64_t           MinSwitches;
        const char        *Name;
        const char        *Version;
        const char        *Description;
        const char        *Author;
        const char        *Copyright;
        bool               IsOpenSource; // if open source, then set the license and URL, if not set a warning, and EULA url
        const char        *License;
        const char        *LicenseURL;
        CommandLineSwitch *Switch; // 1D array of CommandLineSwitch's
    } CommandLineOptions;
    
    typedef struct HuffmanNode {
        int64_t        LeftHuffmanCode;
        int64_t        RightHuffmanCode;
        int64_t       *LeftNode;
        int64_t       *RightNode;
    } HuffmanNode;
    
    typedef struct HuffmanTree {
        uint64_t       NumNodes;
        HuffmanNode   *Node;
        uint64_t      *SymbolFrequency;
        bool           TableIsUsable;
        const uint8_t *Table;
    } HuffmanTree;
    
    BitInput *InitBitInput(void) {
        errno = 0;
        BitInput *BitI        = (BitInput*)calloc(1, sizeof(BitInput));
        if (errno != 0) {
            const char ErrnoError[128];
            strerror_r(errno, ErrnoError, 128);
            Log(LOG_ERR, "libBitIO", "InitBitInput", "Errno error: %s\n", ErrnoError);
        }
        return BitI;
    }
    
    BitOutput *InitBitOutput(void) {
        errno = 0;
        BitOutput *BitO       = (BitOutput*)calloc(1, sizeof(BitOutput));
        if (errno != 0) {
            const char ErrnoError[128];
            strerror_r(errno, ErrnoError, 128);
            Log(LOG_ERR, "libBitIO", "InitBitOutput", "Errno error: %s\n", ErrnoError);
        }
        return BitO;
    }
    
    BitBuffer *InitBitBuffer(void) {
        errno = 0;
        BitBuffer *BitB       = (BitBuffer*)calloc(1, sizeof(BitBuffer));
        if (errno != 0) {
            const char ErrnoError[128];
            strerror_r(errno, ErrnoError, 128);
            Log(LOG_ERR, "libBitIO", "InitBitBuffer", "Errno error: %s\n", ErrnoError);
        }
        return BitB;
    }
    
    void CreateEmptyBuffer(BitBuffer *BitB, const size_t EmptyBufferSize) {
        errno = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "CreateEmptyBuffer", "Pointer to BitBuffer is NULL\n");
        } else if (EmptyBufferSize <= 0) {
            Log(LOG_ERR, "libBitIO", "CreateEmptyBuffer", "You tried creating a empty buffer of size: %d, which is invalid\n", EmptyBufferSize);
        } else {
            BitB->Buffer = (uint8_t*)calloc(1, EmptyBufferSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "CreateEmptyBuffer", "Errno error: %s\n", ErrnoError);
            }
        }
    }
    
    CommandLineOptions *InitCommandLineOptions(const size_t NumSwitches) {
        errno = 0;
        CommandLineOptions *CMD = (CommandLineOptions*)calloc(1, sizeof(CommandLineOptions));
        if (errno != 0) {
            const char ErrnoError[128];
            strerror_r(errno, ErrnoError, 128);
            Log(LOG_ERR, "libBitIO", "InitCommandLineOptions", "Errno Initing CommandLineOptions error: %s\n", ErrnoError);
        }
        errno = 0;
        CMD->NumSwitches        = NumSwitches;
        
        size_t CLSSize          = sizeof(CommandLineSwitch); // 40 bytes
        CMD->Switch             = (CommandLineSwitch*)calloc(NumSwitches, CLSSize);
        if (errno != 0) {
            const char ErrnoError[128];
            strerror_r(errno, ErrnoError, 128);
            Log(LOG_ERR, "libBitIO", "InitCommandLineOptions", "Errno Initing CommandLineSwitch error: %s\n", ErrnoError);
        }
        
        return CMD;
    }
    
    void CloseBitInput(BitInput *BitI) {
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "CloseBitInput", "Pointer to BitInput is NULL\n");
        } else {
            fclose(BitI->File);
            free(BitI);
        }
    }
    
    void CloseBitOutput(BitOutput *BitO) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "CloseBitOutput", "Pointer to BitOutput is NULL");
        } else {
            fflush(BitO->File);
            fclose(BitO->File);
            free(BitO);
        }
    }
    
    void CloseBitBuffer(BitBuffer *BitB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "CloseBitBuffer", "Pointer to BitBuffer is NULL");
        } else {
            free(BitB->Buffer);
            free(BitB);
        }
    }
    
    void CloseCommandLineOptions(CommandLineOptions *CMD) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "CloseCommandLineOptions", "Pointer to CommandLineOptions is NULL\n");
        } else {
            free(CMD);
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
    
    int64_t Powi(const int64_t Base, const int64_t Exponent) {
        int64_t Result = 0;
        
        if (Base == 0) {
            Result = 0;
        } else if (Exponent == 0) {
            Result = 1;
        } else {
            for (uint64_t Loop = 0; Loop < Exponent; Loop++) {
                Result += Base * Base;
            }
        }
        return Result;
    }
    
    int64_t Floori(const double Number2Floor) {
        return (int64_t)floor(Number2Floor);
    }
    
    int64_t Ceili(const double Number2Ceil) {
        return (int64_t)ceil(Number2Ceil);
    }
    
    int64_t ExtractIntegerPartFromDouble(const double Number2Extract) {
        return (int64_t)Number2Extract;
    }
    
    int64_t ExtractDecimalFromDouble(const double Number2Extract, const uint8_t Digits2Extract) {
        return 0;
    }
    
    uint64_t NumBits2ReadSymbols(const uint64_t NumSymbols) { // Use a binary logarithm, that you round up, in order to get the number of bits required to read a certain number of symbols.
        return ceil(log2(NumSymbols));
    }
    
    uint8_t CountBitsSet(const uint64_t Bits2Count) {
        uint8_t DataBit = 0, BitCount = 0;
        if (Bits2Count == 0) {
            return 0;
        } else {
            for (uint8_t Bit = 0; Bit < 64; Bit++) {
                DataBit = (Bits2Count & (1 << Bit)) >> Bit;
                if (DataBit == 1) {
                    BitCount += 1;
                }
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
            HighestBit          = (uint64_t)Powi(2, Exponent - 1);
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
    
    static uint8_t DetectSystemEndian(void) { // MARK: This function needs to remain internal
        uint8_t  SystemEndian = 0;
        uint16_t Endian       = 0xFFFE;
        if (Endian == 0xFFFE) {
            SystemEndian = LittleEndian;
        } else if (Endian == 0xFEFF) {
            SystemEndian = BigEndian;
        } else {
            SystemEndian = UnknownEndian;
        }
        return SystemEndian;
    }
    
    size_t BytesRemainingInInputFile(BitInput *BitI) {
        uint64_t BytesLeft = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "BytesRemainingInInputFile", "Pointer to BitInput is NULL\n");
        } else {
            BytesLeft = BitI->FileSize - BitI->FilePosition;
        }
        return BytesLeft;
    }
    
    size_t GetBitInputFileSize(BitInput *BitI) {
        uint64_t InputSize = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFileSize", "Pointer to BitInput is NULL\n");
        } else {
            InputSize = BitI->FileSize;
        }
        return InputSize;
    }
    
    size_t GetBitInputFilePosition(BitInput *BitI) {
        size_t Position = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFileSize", "Pointer to BitInput is NULL\n");
        } else {
            Position = BitI->FilePosition;
        }
        return Position;
    }
    
    size_t GetBitBufferPosition(BitBuffer *BitB) {
        size_t Position = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputFileSize", "Pointer to BitInput is NULL\n");
        } else {
            Position = BitB->BitsUnavailable;
        }
        return Position;
    }
    
    size_t GetBitBufferSize(BitBuffer *BitB) {
        size_t BitBufferSize = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitBufferSize", "Pointer to BitBuffer is NULL\n");
        } else {
            BitBufferSize = sizeof(BitB->Buffer);
        }
        return BitBufferSize;
    }
    
    uint8_t GetBitInputSystemEndian(BitInput *BitI) {
        uint8_t Endian = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputSystemEndian", "Pointer to BitInput is NULL\n");
        } else {
            Endian = BitI->SystemEndian;
        }
        return Endian;
    }
    
    uint8_t GetBitOutputSystemEndian(BitOutput *BitO) {
        uint8_t Endian = 0;
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitOutputSystemEndian", "Pointer to BitOutput is NULL\n");
        } else {
            Endian = BitO->SystemEndian;
        }
        return Endian;
    }
    
    static void DisplayCMDHelp(CommandLineOptions *CMD) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "DisplayCMDHelp", "Pointer to CommandLineOptions is NULL\n");
        } else {
            printf("Options: (-|--|/)\n");
            for (uint8_t SwitchNum = 0; SwitchNum < CMD->NumSwitches - 1; SwitchNum++) {
                printf("%s: %s\n", CMD->Switch[SwitchNum].Flag, CMD->Switch[SwitchNum].SwitchDescription);
                /*
                 (-|--|/) Input: Input file or stdin with: -
                 */
                // Options:
                // -Input, --Input, or /Input:
                // (-|--|/) Input: Input file or stdin with '-'
                // Ok, so we should show all the prefixes, then the long option, then in parentheses the short option if there is one, but how do we represent short options?
            }
        }
    }
    
    static void DisplayProgramBanner(CommandLineOptions *CMD) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "DisplayProgramBanner", "Pointer to CommandLineOptions is NULL\n");
        } else {
            printf("%s version %s by %s © %s: %s, ", CMD->Name, CMD->Version, CMD->Author, CMD->Copyright, CMD->Description); // Generic part of the string.
            if (CMD->IsOpenSource == true) {
                printf("Released under the \"%s\" license: %s\n\n", CMD->License, CMD->LicenseURL);
            } else {
                printf("By using this software, you agree to the End User License Agreement %s, available at: %s\n\n", CMD->License, CMD->LicenseURL);
            }
            
        }
    }
    
    void ParseCommandLineArguments(CommandLineOptions *CMD, int argc, const char *argv[]) {
        // TODO : Scan for equals signs as well, if found, after the equal sign is the result, everything before is the switch.
        // TODO : add support for generating the short versions of the flags.
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Pointer to CommandLineOptions is NULL\n");
        } else {
            if ((CMD->NumSwitches < CMD->MinSwitches && CMD->MinSwitches > 0) || argc == 1) {
                DisplayProgramBanner(CMD);
                DisplayCMDHelp(CMD);
            } else {
                DisplayProgramBanner(CMD);
                
                errno = 0;
                
                for (uint8_t SwitchNum = 0; SwitchNum < CMD->NumSwitches; SwitchNum++) {
                    for (uint8_t Argument = 1; Argument < argc - 1; Argument++) { // the executable path is skipped over
                                                                                  // Once the switch is found, we should skip over this argument.
                        
                        // Make sure the switch dependency was found before the depedent one.
                        // So, idk how to add this...
                        
                        size_t SingleDashSize                       = CMD->Switch[SwitchNum].FlagSize + 1;
                        size_t DoubleDashSize                       = CMD->Switch[SwitchNum].FlagSize + 2;
                        size_t SlashSize                            = CMD->Switch[SwitchNum].FlagSize + 1;
                        
                        char *SingleDash                            = (char*)calloc(1, SingleDashSize);
                        if (errno != 0) {
                            const char ErrnoError[128];
                            strerror_r(errno, ErrnoError, 128);
                            Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Errno SingleDash = %s, Arg = %d, Switch = %d, errno = %s", ErrnoError, Argument, SwitchNum);
                            errno = 0;
                        } else {
                            snprintf(SingleDash, SingleDashSize, "-%s", CMD->Switch[SwitchNum].Flag);
                        }
                        
                        char *DoubleDash                            = (char*)calloc(1, DoubleDashSize);
                        if (errno != 0) {
                            const char ErrnoError[128];
                            strerror_r(errno, ErrnoError, 128);
                            Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Errno DoubleDash = %s, Arg = %d, Switch = %d", ErrnoError, Argument, SwitchNum);
                            errno = 0;
                        } else {
                            snprintf(DoubleDash, DoubleDashSize, "--%s", CMD->Switch[SwitchNum].Flag);
                        }
                        
                        char *Slash                                 = (char*)calloc(1, SlashSize);
                        if (errno != 0) {
                            const char ErrnoError[128];
                            strerror_r(errno, ErrnoError, 128);
                            Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Errno Slash = %s, Arg = %d, Switch = %d", ErrnoError, Argument, SwitchNum);
                            errno = 0; // Here to reset to catch errors with the strcmp stuff below
                        } else {
                            snprintf(Slash, SlashSize, "/%s", CMD->Switch[SwitchNum].Flag);
                        }
                        
                        if (strcasecmp(SingleDash, argv[Argument]) == 0 || strcasecmp(DoubleDash, argv[Argument]) == 0 || strcasecmp(Slash, argv[Argument]) == 0) {
                            
                            size_t ArgumentSize = strlen(argv[Argument + 1]) + 1;
                            
                            CMD->Switch[SwitchNum].SwitchFound      = true;
                            if (CMD->Switch[SwitchNum].IsThereAResult == true) {
                                char *SwitchResult                  = (char*)calloc(1, ArgumentSize);
                                if (errno != 0) {
                                    const char ErrnoError[128];
                                    strerror_r(errno, ErrnoError, 128);
                                    Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Errno SwitchResult = %s, Arg = %d, Switch = %d", ErrnoError, Argument, SwitchNum);
                                } else {
                                    snprintf(SwitchResult, ArgumentSize, "%s", argv[Argument + 1]);
                                    CMD->Switch[SwitchNum].SwitchResult = SwitchResult;
                                }
                            }
                            SwitchNum += 1; // To break out of looking for this switch
                            Argument  += 1;
                        }
                    }
                }
            }
        }
    }
    
    bool GetPathType(const char *Path) {
        // If it starts with "file://", or a leading slash/tilde it's a file.
        // URLs should start with their protocol, like http:// or https://, but may start with www., or ftp://
        // and on Windows files are prepended by a single character, then a colon and a slash.
        
        // Should this function be more general to simply tell if a path is a URL?
        // Also, we need to support IPv4 and IPv6 addresses
        if (strcasecmp(&Path[0], "/") == 0 || strcasecmp(&Path[0], "~") == 0 || strcasecmp(Path, "file://") == 0 || strcasecmp(Path, "%c:/") == 0) {
            return File;
        } else if (strcasecmp(Path, "http://") == 0 || strcasecmp(Path, "https://") == 0 || strcasecmp(Path, "www.") == 0 || strcasecmp(Path, "ftp://") == 0) {
            
        }
        return false;
    }
    
    void OpenCMDInputFile(BitInput *BitI, CommandLineOptions *CMD, const uint8_t SwitchNum) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDInputFile", "Pointer to CommandLineOptions is NULL\n");
        } else if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDInputFile", "Pointer to BitInput is NULL\n");
        } else {
            errno = 0;
            size_t PathSize = strlen(CMD->Switch[SwitchNum].SwitchResult) + 1;
            
            char *InputFile = (char*)calloc(1, PathSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "OpenCMDInputFile", "Error: %s", ErrnoError);
            } else {
                snprintf(InputFile, PathSize, "%s", CMD->Switch[SwitchNum].SwitchResult);
                
                BitI->File                  = fopen(InputFile, "rb");
                fseek(BitI->File, 0, SEEK_END);
                BitI->FileSize              = (uint64_t)ftell(BitI->File);
                fseek(BitI->File, 0, SEEK_SET);
                BitI->FilePosition          = ftell(BitI->File);
                BitI->SystemEndian          = DetectSystemEndian();
            }
        }
    }
    
    void OpenCMDInputSocket() {
        
    }
    
    void OpenCMDOutputFile(BitOutput *BitO, CommandLineOptions *CMD, const uint8_t SwitchNum) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDOutputFile", "Pointer to CommandLineOptions is NULL\n");
        } else if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDOutputFile", "Pointer to BitOutput is NULL\n");
        } else {
            errno = 0;
            size_t PathSize = strlen(CMD->Switch[SwitchNum].SwitchResult) + 1;
            char *OutputFile = (char*)calloc(1, PathSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "OpenCMDOutputFile", "Error: %s", ErrnoError);
            } else {
                snprintf(OutputFile, PathSize, "%s", CMD->Switch[SwitchNum].SwitchResult);
                
                BitO->File                  = fopen(OutputFile, "wb");
                BitO->SystemEndian          = DetectSystemEndian();
            }
        }
    }
    
    void OpenCMDOutputSocket() {
        
    }
    
    void SetCMDName(CommandLineOptions *CMD, const char *Name) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDName", "Pointer to CommandLineOptions is NULL\n");
        } else if (Name == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDName", "Pointer to Name is NULL\n");
        } else {
            CMD->Name = Name;
        }
    }
    
    void SetCMDVersion(CommandLineOptions *CMD, const char *VersionString) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDVersion", "Pointer to CommandLineOptions is NULL\n");
        } else if (VersionString == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDVersion", "Pointer to VersionString is NULL\n");
        } else {
            CMD->Version = VersionString;
        }
    }
    
    void SetCMDDescription(CommandLineOptions *CMD, const char *Description) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDDescription", "Pointer to CommandLineOptions is NULL\n");
        } else if (Description == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDDescription", "Pointer to Description is NULL\n");
        } else {
            CMD->Description = Description;
        }
    }
    
    void SetCMDAuthor(CommandLineOptions *CMD, const char *Author) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDAuthor", "Pointer to CommandLineOptions is NULL\n");
        } else if (Author == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDAuthor", "Pointer to Author is NULL\n");
        } else {
            CMD->Author = Author;
        }
    }
    
    void SetCMDCopyright(CommandLineOptions *CMD, const char *Copyright) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDCopyright", "Pointer to CommandLineOptions is NULL\n");
        } else if (Copyright == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDCopyright", "Pointer to Copyright is NULL\n");
        } else {
            CMD->Copyright = Copyright;
        }
    }
    
    void SetCMDLicense(CommandLineOptions *CMD, const char *License, const bool IsEULA) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicense", "Pointer to CommandLineOptions is NULL\n");
        } else if (License == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicense", "Pointer to License is NULL\n");
        } else {
            CMD->License      = License;
            if (IsEULA == true) {
                CMD->IsOpenSource = false;
            } else {
                CMD->IsOpenSource = true;
            }
        }
    }
    
    void SetCMDLicenseURL(CommandLineOptions *CMD, const char *LicenseURL, const bool IsEULA) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicenseURL", "Pointer to CommandLineOptions is NULL\n");
        } else if (LicenseURL == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicenseURL", "Pointer to LicenseURL is NULL\n");
        } else {
            CMD->LicenseURL   = LicenseURL;
            if (IsEULA == true) {
                CMD->IsOpenSource = false;
            } else {
                CMD->IsOpenSource = true;
            }
        }
    }
    
    void SetCMDMinSwitches(CommandLineOptions *CMD, const uint64_t MinSwitches) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDMinSwitches", "Pointer to CommandLineOptions is NULL\n");
        } else {
            CMD->MinSwitches = MinSwitches;
        }
    }
    
    void SetCMDSwitchFlag(CommandLineOptions *CMD, const uint64_t SwitchNum, const char *Flag, const size_t FlagSize) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchFlag", "Pointer to CommandLineOptions is NULL\n");
        } else if (Flag == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchFlag", "Pointer to switch Flag is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchFlag", "SwitchNum %d is too high, there are only %d switches\n", SwitchNum, CMD->NumSwitches);
        } else {
            CMD->Switch[SwitchNum].Flag     = Flag;
            CMD->Switch[SwitchNum].FlagSize = FlagSize + 1; // add one for the trailing NULL
        }
    }
    
    void SetCMDSwitchDependency(CommandLineOptions *CMD, const uint64_t SwitchNum, const uint64_t DependsOn) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchDependency", "Pointer to CommandLineOptions is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchDependency", "SwitchNum: %d, should be between 0 and %d\n", SwitchNum, CMD->NumSwitches);
        } else if (DependsOn > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchDependency", "DependsOn: %d, should be between 0 and %d\n", DependsOn, CMD->NumSwitches);
        } else {
            CMD->Switch[SwitchNum].IsDependent = true;
            CMD->Switch[SwitchNum].DependsOn   = DependsOn;
        }
    }
    
    void SetCMDSwitchDescription(CommandLineOptions *CMD, const uint64_t SwitchNum, const char *Description) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchDescription", "Pointer to CommandLineOptions is NULL\n");
        } else if (Description == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchDescription", "Pointer to switch Description is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchDescription", "SwitchNum %d is too high, there are only %d switches\n", SwitchNum, CMD->NumSwitches);
        } else {
            CMD->Switch[SwitchNum].SwitchDescription = Description;
        }
    }
    
    void SetCMDSwitchResultStatus(CommandLineOptions *CMD, const uint64_t SwitchNum, const bool IsThereAResult) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchResultStatus", "Pointer to CommandLineOptions is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetCMDSwitchResultStatus", "SwitchNum: %d, should be between 0 and %d\n", SwitchNum, CMD->NumSwitches);
        } else {
            CMD->Switch[SwitchNum].IsThereAResult = IsThereAResult & 1;
        }
    }
    
    const char *GetCMDSwitchResult(CommandLineOptions *CMD, const uint64_t SwitchNum) {
        const char *Result = NULL;
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "GetCMDSwitchResult", "Pointer to CommandLineOptions is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "GetCMDSwitchResult", "SwitchNum: %d, should be between 0 and %d\n", SwitchNum, CMD->NumSwitches);
        } else {
            Result = CMD->Switch[SwitchNum].SwitchResult;
        }
        return Result;
    }
    
    bool GetCMDSwitchPresence(CommandLineOptions *CMD, const uint64_t SwitchNum) {
        bool Status = 0;
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "GetCMDSwitchPresence", "Pointer to CommandLineOptions is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) { // - 1 so the hidden help option isn't exposed
            Log(LOG_ERR, "libBitIO", "GetCMDSwitchPresence", "SwitchNum: %d, should be between 0 and %d\n", SwitchNum, CMD->NumSwitches);
        } else {
            Status = CMD->Switch[SwitchNum].SwitchFound;
        }
        return Status;
    }
    
    uint64_t ReadBits(BitBuffer *BitB, const uint8_t Bits2Read, const bool ReadFromMSB) {
        uint8_t Bits = Bits2Read, UserRequestBits = 0, BufferBitsAvailable = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0;
        
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Pointer to BitBuffer is NULL\n");
        } else if (BitB->Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "Pointer to Buffer in BitBuffer is NULL\n");
        } else if (Bits2Read > 64) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "ReadBits: %d, only supports reading 1-64 bits\n", Bits2Read);
        } else if (Bits2Read == 0) {
            return 0;
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
            Log(LOG_ERR, "libBitIO", "PeekBits", "Pointer to BitBuffer is NULL\n");
        } else {
            OutputData                 = ReadBits(BitB, Bits2Peek, ReadFromMSB);
            BitB->BitsAvailable       += Bits2Peek; // Backwards to set the counter back to where it was.
            BitB->BitsUnavailable     -= Bits2Peek;
        }
        return OutputData;
    }
    
    uint64_t  ReadRICE(BitBuffer *BitB, const bool Truncated, const bool StopBit) {
        uint64_t BitCount = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadRICE", "Pointer to BitBuffer is NULL\n");
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
        int64_t  Final = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadExpGolomb", "Pointer to BitBuffer is NULL\n");
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
            Log(LOG_ERR, "libBitIO", "SkipBits", "Pointer to BitBuffer is NULL\n");
        } else {
            BitB->BitsAvailable   -= Bits2Skip;
            BitB->BitsUnavailable += Bits2Skip;
            // The file/stream updating functions need to keep this in mind.
        }
    }
    
    void WriteBits(BitBuffer *BitB, const uint64_t Data2Write, const uint8_t NumBits, const bool WriteFromMSB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBits", "Pointer to BitBuffer is NULL\n");
        } else {
            // FIXME: WriteBits currently copies NumBits bits to the file, even if the input is shorter than that. we need to prepend 0 bits if that's the case
            
            uint8_t Bits2Write2BufferByte, Bits2ShiftMask, Mask, Bits2Write = NumBits;
            
            // so we're trying to write 9 bits, 0x1FF.
            // There are only 5 bits available in the current byte.
            // So, that means we have to pull out 5 bits from Data2Write, and pop it into BitO->Buffer.
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
            Log(LOG_ERR, "libBitIO", "WriteRICE", "Pointer to BitBuffer is NULL\n");
        } else {
            for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
                WriteBits(BitB, (~StopBit), 1, WriteFromMSB);
            }
            WriteBits(BitB, StopBit, 1, WriteFromMSB);
        }
    }
    
    void WriteExpGolomb(BitBuffer *BitB, const bool IsSigned, const uint64_t Data2Write, const bool WriteFromMSB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteExpGolomb", "Pointer to BitOutput is NULL\n");
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
    
    bool IsBitBufferAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
        bool AlignmentStatus = 0;
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "IsOutputStreamByteAligned", "Pointer to BitBuffer is NULL\n");
        } else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
            Log(LOG_ERR, "libBitIO", "IsOutputStreamByteAligned", "BytesOfAlignment: %d isn't a power of 2 (or 1)\n", BytesOfAlignment);
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
            Log(LOG_ERR, "libBitIO", "AlignBitBuffer", "Pointer to BitBuffer is NULL\n");
        } else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
            Log(LOG_ERR, "libBitIO", "AlignBitBuffer", "BytesOfAlignment: %d isn't a power of 2 (or 1)\n", BytesOfAlignment);
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
            Log(LOG_ERR, "libBitIO", "EncodeRABS", "Data2Encode pointer is NULL\n");
        } else if (EncodedData == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeRABS", "EncodedData pointer is NULL\n");
        } else {
            
        }
    }
    
    uint64_t GenerateCRC(const uint8_t *Data2CRC, const size_t DataSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit) {
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
    
    static uint8_t *ConvertBinaryUUID2UUIDString(const uint8_t *BinaryUUID) {
        uint8_t *UUIDString = NULL;
        
        if (BinaryUUID == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "Pointer to BinaryUUID is NULL\n");
        } else if (sizeof(BinaryUUID) != BitIOBinaryUUIDSize) {
            Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "BinaryUUID size should be: %d, but is: %d\n", BitIOBinaryUUIDSize, sizeof(BinaryUUID));
        } else {
            errno = 0;
            UUIDString = (uint8_t*)calloc(1, BitIOUUIDStringSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "Errno: %s", ErrnoError);
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
        
        if (UUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "Pointer to UUIDString is NULL\n");
        } else if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "UUIDString size should be: %d, but is: %d\n", BitIOUUIDStringSize, sizeof(UUIDString));
        } else {
            errno = 0;
            BinaryUUID = (uint8_t*)calloc(1, BitIOBinaryUUIDSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "Errno: %s", ErrnoError);
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
            Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "Pointer to BinaryUUID is NULL\n");
        } else if (sizeof(BinaryUUID) != BitIOBinaryUUIDSize) {
            Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "BinaryUUID size should be: %d, but is: %d\n", BitIOBinaryUUIDSize, sizeof(BinaryUUID));
        } else {
            errno = 0;
            SwappedBinaryUUID = (uint8_t*)calloc(1, BitIOBinaryUUIDSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "Errno: %s", ErrnoError);
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
            Log(LOG_ERR, "libBitIO", "ReadUUID", "Pointer to BitBuffer is NULL\n");
        } else {
            errno = 0;
            uint8_t *BinaryUUID = (uint8_t*)calloc(1, BitIOBinaryUUIDSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "ReadUUID", "Errno: %s", ErrnoError);
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
        } else if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertUUID2GUID", "UUIDString size should be: %d, but is: %d\n", BitIOUUIDStringSize, sizeof(UUIDString));
        } else {
            errno = 0;
            GUIDString = (uint8_t*)calloc(1, BitIOGUIDStringSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "ConvertUUID2GUID", "Errno: %s", ErrnoError);
            } else {
                BinaryUUID = ConvertUUIDString2BinaryUUID(UUIDString);
                BinaryGUID = SwapBinaryUUID(BinaryUUID);
                GUIDString = ConvertBinaryUUID2UUIDString(BinaryGUID);
            }
        }
        return GUIDString;
    }
    
    uint8_t *ConvertGUID2UUID(const uint8_t *GUIDString) {
        uint8_t *BinaryGUID = NULL;
        uint8_t *BinaryUUID = NULL;
        uint8_t *UUIDString = NULL;
        
        if (GUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "ConvertGUID2UUID", "Pointer to GUIDString is NULL");
        } else if (sizeof(GUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertGUID2UUID", "GUIDString size should be: %d, but is: %d\n", BitIOGUIDStringSize, sizeof(GUIDString));
        } else {
            errno = 0;
            UUIDString = (uint8_t*)calloc(1, BitIOUUIDStringSize);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "ConvertUUID2GUID", "Errno: %s", ErrnoError);
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
        } else if (sizeof(UUIDString1) != BitIOUUIDStringSize || sizeof(UUIDString2) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "CompareUUIDs", "UUIDString1 is %d bytes long, UUIDString2 is %d bytes long, should be 21\n", sizeof(UUIDString1), sizeof(UUIDString2));
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
            Log(LOG_ERR, "libBitIO", "WriteUUID", "Pointer to instance of BitBuffer is NULL\n");
        } else if (UUIDString == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteUUID", "Pointer to UUIDString is NULL");
        } else if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "WriteUUID", "UUIDString is %d bytes long, should be 21\n", sizeof(UUIDString));
        } else {
            // Convert UUIDString to BinaryUUID, then write that.
            uint8_t *BinaryUUID = NULL;
            BinaryUUID = ConvertUUIDString2BinaryUUID(UUIDString);
            for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                WriteBits(BitB, BinaryUUID[UUIDByte], 8, true);
            }
        }
    }
    
    void OpenSocket() { // Define it in the header when it's done
        
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
    
    void ReadInputFile2Buffer(BitInput *BitI, BitBuffer *BitB, const size_t Bytes2Read) { // It's the user's job to ensure buffers and files are kept in sync, not mine.
        size_t BytesRead              = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadInputFile2Buffer", "BitI pointer is NULL\n");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadInputFile2Buffer", "BitB pointer is NULL\n");
        } else if (Bytes2Read > BitI->FileSize - BitI->FilePosition) {
            Log(LOG_ERR, "libBitIO", "ReadInputFile2Buffer", "You tried reading more data: % than is available: %d in the file\n", Bytes2Read, BitI->FileSize - BitI->FilePosition);
        } else {
            if (BitB->Buffer != NULL) {
                free(BitB->Buffer);
            }
            errno = 0;
            BitB->Buffer                  = (uint8_t*)calloc(1, Bytes2Read);
            if (errno != 0) {
                const char ErrnoError[128];
                strerror_r(errno, ErrnoError, 128);
                Log(LOG_ERR, "libBitIO", "ReadInputFile2Buffer", "Errno: %s", ErrnoError);
            } else {
                BytesRead                 = fread(BitB->Buffer, 1, Bytes2Read, BitI->File);
                if (BytesRead            != Bytes2Read) {
                    Log(LOG_ERR, "libBitIO", "ReadInputFile2Buffer", "Fread read: %d bytes, but you requested: %d\n", BytesRead, Bytes2Read);
                } else {
                    BitB->BitsAvailable   = Bytes2Bits(BytesRead);
                    BitB->BitsUnavailable = 0;
                }
            }
        }
    }
    
    void WriteBuffer2OutputFile(BitOutput *BitO, BitBuffer *BitB, const size_t Bytes2Write) {
        size_t BytesWritten           = 0;
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBuffer2OutputFile", "BitI pointer is NULL\n");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBuffer2OutputFile", "BitB pointer is NULL\n");
        } else {
            // Write the bytes in BitB->Buffer to BitO->File
            BytesWritten              = fwrite(BitB->Buffer, 1, Bytes2Write, BitO->File);
            if (BytesWritten         != Bytes2Write) {
                Log(LOG_ERR, "libBitIO", "WriteBuffer2OutputFile", "Fwrite wrote: %d bytes, but you requested: %d\n", BytesWritten, Bytes2Write);
            } else {
                BitB->BitsAvailable   = Bytes2Bits(BytesWritten);
                BitB->BitsUnavailable = 0;
            }
        }
    }
    
    void ReadSocket2Buffer(BitInput *BitI, const size_t Bytes2Read) { // Define it in the header when it's done
                                                                      //sockaddr_in
    }
    
    void WriteBuffer2Socket(BitOutput *BitO, BitBuffer *BitB, const int Socket) { // Define it in the header when it's done
        
    }
    
    /*
     Huffman, Arthimetic, and ANS/ABS coding systems ALL require having a sorted list of symbol and their frequency (to keep it in the int domain)
     So writing a sorting algorithm is going to be the first thing i do, and I'm not going to fuck around with crazy sorters, just a real simple one that should optimize better.
     */
    uint64_t *MeasureSortSymbolFrequency(const uint16_t *Buffer2Measure, const size_t BufferSizeInElements, const uint8_t ElementSizeInBytes) {
        // This is MeasureSymbolFrequency + sorting as we go.
        errno = 0;
        uint64_t *SymbolFrequencies = (uint64_t*)calloc(1, BufferSizeInElements);
        if (errno != 0) {
            const char ErrnoError[128];
            strerror_r(errno, ErrnoError, 128);
            Log(LOG_ERR, "libBitIO", "MeasureSortSymbolFrequency", " Errno: SymbolFrequencies = %s, size = %d", ErrnoError, BufferSizeInElements);
            errno = 0;
        }
        uint64_t *FrequencyPosition = (uint64_t*)calloc(1, BufferSizeInElements);
        if (errno != 0) {
            const char ErrnoError[128];
            strerror_r(errno, ErrnoError, 128);
            Log(LOG_ERR, "libBitIO", "MeasureSortSymbolFrequency", " Errno: FrequencyPosition = %s, size = %d", ErrnoError, BufferSizeInElements);
            errno = 0;
        }
        
        if (Buffer2Measure == NULL) {
            Log(LOG_ERR, "libBitIO", "MeasureSortSymbolFrequency", "Buffer2Measure is NULL\n");
        } else {
            for (uint64_t Element = 0; Element < BufferSizeInElements; Element++) {
                SymbolFrequencies[Buffer2Measure[Element]] += 1; // The index is the symbol
                
                uint64_t Frequency = SymbolFrequencies[Buffer2Measure[Element]];
                
                while (SymbolFrequencies[Buffer2Measure[Element]] != Frequency) {
                    FrequencyPosition[SymbolFrequencies[Buffer2Measure[Element]]] += 1;
                }
            }
            /*
             Buffer2Measure contains the symbols to be measured.
             SymbolFrequency contains the frequency of each symbol
             From SymbolFrequency, we need to create a sorted list who's index is the most common to least common symbols, each element contains the actual symbol.
             We need anoth array to store the locations of the highest of each frequency (so if 0 and 1 both contain 4, store 0)
             then we need to use the frequency of list 1 in list 2 storing the position
             
             We do not know the total order, so we need to build it as we go.
             
             therefore the index in the position array needs to be the frequency, which stores the last element of that frequency (last being defined as the farthest from the top of the buffer)
             
             So each time we find an element in a buffer, we need to update the frequency position, use that position to set the result of the frequency buffer.
             
             So, to sort as we go, what we have to do is Read a symbol from the buffer, and find that symbol in the array by using it as an index into the array.
             then increment the frequency of that symbol.
             
             So Array1's index is the symbol, with the value being the frequency.
             
             Array2's index is the frequency.
             
             So we have to extract the value from array1 and use that as the index in array 2.
             then we need to loop over array 1 long enough to find a frequency smaller than this which shouldn't take too many loops.
             once we find a symbol with lower frequency, we simply use the last value as an array value.
             Well, it'll have tobe a while loop inside the for loop
             */
        }
        return SymbolFrequencies;
    }
    
    /* Deflate (encode deflate) */
    void ParseDeflateHeader(BitBuffer *DeflatedData) {
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
                Log(LOG_ERR, "libBitIO", "ParseDeflateHeader", "Invalid DEFLATE compression method %d\n", CompressionMethod);
            }
        }
    }
    
    void DecodeHuffman(BitBuffer *BitB, size_t HuffmanSize) {
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
    
    uint64_t GetHuffmanCode(HuffmanTree *Tree, int64_t **SymbolsAndProbabilities, int64_t Symbol, size_t NumSymbols) {
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
    
    HuffmanTree *BuildHuffmanTree(HuffmanTree *Tree2Build, int64_t **SymbolsAndProbabilities, size_t NumSymbols) {
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
    
    void ParseDeflateBlock(BitBuffer *BitB, uint16_t BlockSize) {
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
                    Log(LOG_EMERG, "libBitIO", "ParseDeflateBlock", "Invalid Deflate encoding method: %d\n", EncodingMethod);
                    break;
            }
        }
    }
    
    void EncodeLZ77(BitBuffer *RawBuffer, BitBuffer *LZ77Buffer, const size_t WindowSize, const size_t DistanceLength, const size_t SymbolSize) {
        // The dictionary is simply the current buffer, at the current buffer position -(WindowSize / 2) +(WindowSize / 2)
        // So, the first thing you write is the distance from the cursor to the previous string.
        // Then you write the length of the largest match you can find.
        // Then write the next byte in the stream.
        // Then move the cursor the length of the longest match + 1
        // When we're at the start of the match, simply record the byte and the length of the match (1).
        
        
        // We need to have a BitBuffer to read from, and a BitBuffer to write to.
        if (RawBuffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the raw buffer is NULL\n");
        } else if (LZ77Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the LZ77 buffer is NULL\n");
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
    
    void EncodeDeflate(BitBuffer *Data2Encode, BitBuffer *EncodedData) {
        
    }
    
    void DecodeDeflate(BitBuffer *Data2Decode, BitBuffer *DecodedData) {
        
    }
    
    void Log(const uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription, ...) {
        const char *VariadicArguments = (char*)NULL;
        
        va_list Argument;
        va_start(Argument, ErrorDescription);
        while (*ErrorDescription) {
            char *VarArgString = va_arg(Argument, char*);
            vprintf(VariadicArguments, Argument);
        }
        va_end(Argument);
        char *ErrorString = (char*)NULL;
        snprintf(ErrorString, BitIOStringSize, "%s - %s: %s - %s\n", LibraryOrProgram, Function, ErrorDescription, VariadicArguments);
        
#ifndef _WIN32
        if ((ErrorLevel == LOG_EMERG) || (ErrorLevel == LOG_CRIT)) {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_MAIL|LOG_USER));
        } else {
            openlog(LibraryOrProgram, ErrorLevel, (LOG_PERROR|LOG_USER));
        }
        syslog(LOG_ERR, "%s\n", ErrorString);
#else
        char *WinLibraryOrProgram;
        snprintf(WinLibraryOrProgram, strlen(LibraryOrProgram), "L%s", LibraryOrProgram);
        HANDLE EventLog = RegisterEventSourceA(NULL, (LPCSTR)WinLibraryOrProgram);
        uint32_t ErrorCode  = ReportEvent(EventLog, ErrorLevel, 1, 0xF000FFFF, NULL, 1, (DWORD)strlen(ErrorString), (LPCWSTR*)ErrorString, NULL);
        if (ErrorCode != 0) {
            fprintf(stderr, "BitIO - Log: Windows version of Logger failed with error: %s\n", ErrorString);
        }
        bool DeregisterSucceeded = DeregisterEventSource(EventLog);
        if (DeregisterSucceeded > 0) {
            fprintf(stderr, "BitIO - Log: Deregistering EventLog failed\n");
        }
#endif
    }
    
#ifdef __cplusplus
}
#endif
