#include "../include/BitIO.h"

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>
#else
#include <winbase.h>
#include <winsock.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp stricmp
#endif
    
    /*!
     @typedef           BitBuffer
     @abstract                              "Contains variables and a pointer to a buffer for reading and writing bits".
     @constant          BitsAvailable       "The number of bits available for reading".
     @constant          BitsUnavailable     "The number of bits previously read, or available for writing".
     @constant          Buffer              "A pointer to an unsigned byte buffer".
     */
    typedef struct BitBuffer {
        size_t             BitsAvailable;
        size_t             BitsUnavailable;
        uint8_t           *Buffer;
    } BitBuffer;
    
    /*!
     @typedef           BitInput
     @abstract                              "Contains File/Socket pointers for reading to a BitBuffer.
     @constant          File                "Input File/Socket to read into a BitBuffer".
     @constant          FileSize            "Size of the File in bytes".
     @constant          FilePosition        "Current byte in the file".
     @constant          SystemEndian        "Endian of the running system".
     @constant          BitB                "Pointer to an instance of BitBuffer".
     */
    typedef struct BitInput {
        FILE              *File;
        size_t             FileSize;
        size_t             FilePosition;
        uint8_t            SystemEndian:2;
        BitBuffer         *BitB;
    } BitInput;
    
    /*!
     @typedef           BitOutput
     @abstract                              "Contains File/Socket pointers for writing from a BitBuffer".
     @constant          File                "Input File/Socket to write a BitBuffer into".
     @constant          FilePosition        "Current byte in the file".
     @constant          SystemEndian        "Endian of the running system".
     @constant          BitB                "Pointer to an instance of BitBuffer".
     */
    typedef struct BitOutput {
        FILE              *File;
        size_t             FilePosition;
        uint8_t            SystemEndian:2;
        BitBuffer         *BitB;
    } BitOutput;
    
    /*!
     @typedef           CommandLineSwitch
     @abstract                              "Contains the data to support a single switch".
     @remark                                "You MUST include the NULL padding at the end of @Switch".
     @constant          SwitchFound         "If the switch was found in argv, this will be set to true".
     @constant          Resultless          "Is the mere presence of the switch what you're looking for? if so, set to true"
     @constant          Flag                "Actual flag, WITHOUT dash(s) or backslash, Flags are case insensitive".
     @constant          SwitchDescription   "Message to print explaining what the switch does".
     @constant          SwitchResult        "String to contain the result of this switch, NULL if not found".
     */
    typedef struct CommandLineSwitch {
        bool               SwitchFound;
        bool               Resultless;
        size_t             FlagSize;
        const char        *Flag;
        const char        *SwitchDescription;
        const char        *SwitchResult;
    } CommandLineSwitch;
    
    /*!
     @typedef           CommandLineOptions
     @abstract                              "Type to contain a variable amount of CLSwitches".
     @remark                                "The switches are zero indexed, and @NumSwitches is zero indexed, so count from 0".
     @constant          NumSwitches         "The number of switches".
     @constant          MinSwitches         "The minimum number of switches this program requires to run".
     @constant          ProgramName         "The name you want output when the help is printed".
     @constant          ProgramDescription  "The description of the program when the help is printed".
     @constant          Author              "The author of the program".
     @constant          Copyright           "The starting and ending copyright years".
     @constant          License             "The license this program is released under".
     @constant          Switch              "A pointer to an array of CLSwitch instances containing the properties of the switches".
     */
    typedef struct CommandLineOptions {
        size_t              NumSwitches;
        uint64_t            MinSwitches;
        const char         *Name;
        const char         *Version;
        const char         *Description;
        const char         *Author;
        const char         *Copyright;
        bool                IsOpenSource; // if open source, then set the license and URL, if not set a warning, and EULA url
        const char         *License;
        const char         *LicenseURL;
        const char         *EULAWarning;
        const char         *EULAURL;
        CommandLineSwitch  *Switch;
    } CommandLineOptions;
    
    BitInput *InitBitInput(void) {
        BitInput *BitI = calloc(1, sizeof(BitInput));
        return BitI;
    }
    
    BitOutput *InitBitOutput(void) {
        BitOutput *BitO = calloc(1, sizeof(BitOutput));
        return BitO;
    }
    
    BitBuffer *InitBitBuffer(const size_t BufferSize) {
        BitBuffer *BitB       = calloc(1, sizeof(BitBuffer));
        BitB->BitsAvailable   = Bytes2Bits(BufferSize);
        BitB->BitsUnavailable = 0;
        return BitB;
    }
    
    CommandLineOptions *InitCommandLineOptions(void) {
        CommandLineOptions *CMD = calloc(1, sizeof(CommandLineOptions));
        return CMD;
    }
    
    void InitCommandLineSwitches(CommandLineOptions *CMD, uint64_t NumSwitches) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "InitCommandLineSwitches", "Pointer to CommandLineOptions is NULL\n");
        } else {
            CMD->NumSwitches           = NumSwitches + 1; // Help option
            CMD->Switch                = calloc(NumSwitches, sizeof(CommandLineSwitch));
            /*
             for (uint64_t Option = 0; Option <= NumSwitches; Option++) {
             CMD->Switch[Option]    = calloc(1, sizeof(CommandLineSwitch*));
             CMD->Switch[Option]    = calloc(1, sizeof(CommandLineSwitch));
             }
             */
        }
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
            if (IsBitBufferAligned(BitO->BitB, 1) == false) {
                AlignBitBuffer(BitO->BitB, 1);
            }
            fwrite(BitO->BitB->Buffer, Bits2Bytes(BitO->BitB->BitsUnavailable, true), 1, BitO->File);
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
            for (uint64_t Option = 0; Option < CMD->NumSwitches; Option++) {
                free(CMD->Switch);
            }
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
        return (Bytes * 8);
    }
    
    int64_t Bits2Bytes(const int64_t Bits, const bool RoundUp) {
        if (RoundUp == true) {
            return (Bits / 8) + (8 - (Bits % 8));
        } else {
            return (Bits / 8);
        }
    }
    
    uint64_t Signed2Unsigned(const int64_t Signed) {
        return ~Signed + 1;
    }
    
    int64_t Unsigned2Signed(const uint64_t Unsigned) {
        return ~Unsigned - 1;
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
            HighestBit          = Powi(2, Exponent - 1);
            Fill                = HighestBit - 1;
            Mask                = HighestBit + Fill;
        }
        return Mask;
    }
    
    uint64_t OnesCompliment2TwosCompliment(const int64_t OnesCompliment) {
        return ~OnesCompliment + 1;
    }
    
    uint64_t TwosCompliment2OnesCompliment(const int64_t TwosCompliment) {
        return TwosCompliment ^ 0xFFFFFFFFFFFFFFFF;
    }
    
    bool IsOdd(const int64_t Number2Check) {
        return Number2Check % 2 == 0 ? true : false;
    }
    
    uint8_t  FindHighestBitSet(const uint64_t UnsignedInt2Search) {
        uint8_t  HighestBitSet = 0;
        uint64_t Shift         = 0ULL;
        
        for (uint8_t Bit = 64; Bit > 0; Bit--) {
            Shift = ((1ULL << Bit) - 1);
            if (((UnsignedInt2Search & Shift) >> Shift) == 1) {
                HighestBitSet = Bit;
                break;
            }
        }
        return HighestBitSet;
    }
    
    static uint8_t DetectSystemEndian(void) { // MARK: This function needs to remain internal
        uint8_t SystemEndian = 0;
        uint16_t Endian      = 0xFFFE;
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
            Endian = BitI->SystemEndian;;
        }
        return Endian;
    }
    
    uint8_t GetBitOutputSystemEndian(BitOutput *BitO) {
        uint8_t Endian = 0;
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitOutputSystemEndian", "Pointer to BitOutput is NULL\n");
        } else {
            Endian = BitO->SystemEndian;;
        }
        return Endian;
    }
    
    static void DisplayCMDHelp(CommandLineOptions *CMD) { // MARK: This function needs to remain internal
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "DisplayCMDHelp", "Pointer to CommandLineOptions is NULL\n");
        } else {
            printf("Options:\n");
            for (uint8_t Option = 0; Option < CMD->NumSwitches; Option++) {
                printf("%s\t", CMD->Switch[Option].Flag);
                // Options:
                // -Input, --Input, or /Input:
                // (-|--|/) Input: Input file or stdin with '-'
                // Ok, so we should show all the prefixes, then the long option, then in parentheses the short option is there is one, but how do we repesent short options?
                printf("%s\n", CMD->Switch[Option].SwitchDescription);
            }
        }
    }
    
    static void DisplayProgramBanner(CommandLineOptions *CMD) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "DisplayProgramBanner", "Pointer to CommandLineOptions is NULL\n");
        } else {
            if (CMD->IsOpenSource == true) { // License stuff
                printf("%s version %s by %s © %s: %s, Released under the \"%s\" license: %s\n\n", CMD->Name, CMD->Version, CMD->Author, CMD->Copyright, CMD->Description, CMD->License, CMD->LicenseURL);
                // ModernPNG by BumbleBritches57 © 2017-2017: PNG encoder/decoder written from scratch in modern C, Released under the "Revised BSD (3 clause)" license
            } else { // EULA stuff
                printf("%s version %s by %s © %s: %s, By using this software, you agree to the End User License Agreement, available at: %s\n\n", CMD->Name, CMD->Version, CMD->Author, CMD->Copyright, CMD->Description, CMD->EULAURL);
            }
        }
    }
    
    void ParseCommandLineArguments(CommandLineOptions *CMD, int argc, const char *argv[]) {
        // TODO : Scan for equals signs as well, if found, after the equal sign is the result, everything before is the switch.
        // TODO : add support for generating the short versions of the arguments.
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Pointer to CommandLineOptions is NULL\n");
        } else {
            if (CMD->NumSwitches < CMD->MinSwitches && CMD->MinSwitches > 0) {
                DisplayCMDHelp(CMD);
            } else {
                DisplayProgramBanner(CMD);
                SetSwitchFlag(CMD, CMD->NumSwitches, "Help", 5);
                SetSwitchDescription(CMD, CMD->NumSwitches, "Prints all the command line options\n");
                SetSwitchResultStatus(CMD, CMD->NumSwitches, true);
                
                for (uint8_t Argument = 1; Argument < argc; Argument++) { // the executable path is skipped over
                    for (uint8_t Switch = 0; Switch < CMD->NumSwitches; Switch++) {
                        // Once the switch is found, we should skip over this argument.
                        
                        size_t SingleDashSize = CMD->Switch[Switch].FlagSize + 1;
                        size_t DoubleDashSize = CMD->Switch[Switch].FlagSize + 2;
                        size_t SlashSize      = CMD->Switch[Switch].FlagSize + 1;
                        
                        char *SingleDash                             = calloc(1, SingleDashSize);
                        snprintf(SingleDash, SingleDashSize, "-%s", CMD->Switch[Switch].Flag);
                        
                        char *DoubleDash                             = calloc(1, DoubleDashSize);
                        snprintf(DoubleDash, DoubleDashSize, "--%s", CMD->Switch[Switch].Flag);
                        
                        char *Slash                                  = calloc(1, SlashSize);
                        snprintf(Slash,SlashSize, "/%s", CMD->Switch[Switch].Flag);
                        
                        if (strcasecmp(SingleDash, argv[Argument]) == 0 || strcasecmp(DoubleDash, argv[Argument]) == 0 || strcasecmp(Slash, argv[Argument]) == 0) {
                            if (Argument == CMD->NumSwitches) {
                                DisplayCMDHelp(CMD);
                            } else {
                                CMD->Switch[Switch].SwitchFound      = true;
                                if (CMD->Switch[Switch].Resultless == false) {
                                    char *SwitchResult               = calloc(1, strlen(argv[Argument]));
                                    snprintf(SwitchResult, strlen(argv[Argument] + 1), "%s", argv[Argument + 1]);
                                    CMD->Switch[Switch].SwitchResult = SwitchResult;
                                }
                            }
                            Argument += 1;
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
        
        // Shouuld this function be more general to simply tell if a path is a URL?
        // Also, we need to support IPv4 and IPv6 addresses
        if (strcasecmp(&Path[0], "/") == 0 || strcasecmp(&Path[0], "~") || strcasecmp(Path, "file://" || strcasecmp(Path, "%c:/")) == 0) {
            return File;
        } else if (strcasecmp(Path, "http://") == 0 || strcasecmp(Path, "https://") == 0 || strcasecmp(Path, "www.") == 0 || strcasecmp(Path, "ftp://") == 0) {
            
        }
        return false;
    }
    
    void OpenCMDInputFile(BitInput *BitI, CommandLineOptions *CMD, const uint8_t InputSwitch) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDInputFile", "Pointer to CommandLineOptions is NULL\n");
        } else if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDInputFile", "Pointer to BitInput is NULL\n");
        } else {
            BitI->File                  = fopen(CMD->Switch[InputSwitch].SwitchResult, "rb");
            fseek(BitI->File, 0, SEEK_END);
            BitI->FileSize              = (uint64_t)ftell(BitI->File);
            fseek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition          = ftell(BitI->File);
            uint64_t Bytes2Read         = BitI->FileSize > BitInputBufferSize ? BitInputBufferSize : BitI->FileSize;
            uint64_t BytesRead          = fread(BitI->BitB->Buffer, 1, Bytes2Read, BitI->File);
            BitI->BitB->BitsAvailable   = Bytes2Bits(BytesRead);
            BitI->BitB->BitsUnavailable = 0;
            DetectSystemEndian();
        }
    }
    
    void OpenCMDInputSocket() {
        
    }
    
    void OpenCMDOutputFile(BitOutput *BitO, CommandLineOptions *CMD, const uint8_t OutputSwitch) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDOutputFile", "Pointer to CommandLineOptions is NULL\n");
        } else if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "OpenCMDOutputFile", "Pointer to BitOutput is NULL\n");
        } else {
            BitO->File                  = fopen(CMD->Switch[OutputSwitch].SwitchResult, "rb");
            BitO->BitB->BitsAvailable   = BitOutputBufferSizeInBits;
            BitO->BitB->BitsUnavailable = 0;
            DetectSystemEndian();
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
    
    void SetCMDLicense(CommandLineOptions *CMD, const char *License) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicense", "Pointer to CommandLineOptions is NULL\n");
        } else if (License == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicense", "Pointer to License is NULL\n");
        } else {
            CMD->License      = License;
            CMD->IsOpenSource = true;
        }
    }
    
    void SetCMDLicenseURL(CommandLineOptions *CMD, const char *LicenseURL) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicenseURL", "Pointer to CommandLineOptions is NULL\n");
        } else if (LicenseURL == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDLicenseURL", "Pointer to LicenseURL is NULL\n");
        } else {
            CMD->LicenseURL   = LicenseURL;
            CMD->IsOpenSource = true;
        }
    }
    
    void SetCMDEULAWarning(CommandLineOptions *CMD, const char *EULAWarning) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDEULAWarning", "Pointer to CommandLineOptions is NULL\n");
        } else if (EULAWarning == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDEULAWarning", "Pointer to EULAWarning is NULL\n");
        } else {
            CMD->EULAWarning  = EULAWarning;
            CMD->IsOpenSource = false;
        }
    }
    
    void SetCMDEULAURL(CommandLineOptions *CMD, const char *EULAURL) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDEULAURL", "Pointer to CommandLineOptions is NULL\n");
        } else if (EULAURL == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDEULAURL", "Pointer to EULAURL is NULL\n");
        } else {
            CMD->EULAURL      = EULAURL;
            CMD->IsOpenSource = false;
        }
    }
    
    void SetCMDMinSwitches(CommandLineOptions *CMD, const uint64_t MinSwitches) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetCMDMinSwitches", "Pointer to CommandLineOptions is NULL\n");
        } else {
            CMD->MinSwitches = MinSwitches;
        }
    }
    
    void SetSwitchFlag(CommandLineOptions *CMD, uint64_t SwitchNum, const char *Flag, const size_t FlagSize) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetSwitchFlag", "Pointer to CommandLineOptions is NULL\n");
        } else if (Flag == NULL) {
            Log(LOG_ERR, "libBitIO", "SetSwitchFlag", "Pointer to switch Flag is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetSwitchFlag", "SwitchNum %d is too high, there are only %d switches\n", SwitchNum, CMD->NumSwitches);
        } else {
            CMD->Switch[SwitchNum].Flag = Flag;
            CMD->Switch[SwitchNum].FlagSize = FlagSize;
        }
    }
    
    void SetSwitchDescription(CommandLineOptions *CMD, uint64_t SwitchNum, const char *Description) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetSwitchDescription", "Pointer to CommandLineOptions is NULL\n");
        } else if (Description == NULL) {
            Log(LOG_ERR, "libBitIO", "SetSwitchDescription", "Pointer to switch Description is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetSwitchDescription", "SwitchNum %d is too high, there are only %d switches\n", SwitchNum, CMD->NumSwitches);
        } else {
            CMD->Switch[SwitchNum].SwitchDescription = Description;
        }
    }
    
    void SetSwitchResultStatus(CommandLineOptions *CMD, uint64_t SwitchNum, bool IsSwitchResultless) {
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "SetSwitchResultStatus", "Pointer to CommandLineOptions is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) { // - 1 so the hidden help option isn't exposed
            Log(LOG_ERR, "libBitIO", "SetSwitchResultStatus", "SwitchNum: %d, should be between 0 and %d\n", SwitchNum, CMD->NumSwitches);
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "SetSwitchResultStatus", "SwitchNum %d is too high, there are only %d switches\n", SwitchNum, CMD->NumSwitches);
        } else {
            CMD->Switch[SwitchNum].Resultless = (IsSwitchResultless & 1);
        }
    }
    
    const char *GetSwitchResult(CommandLineOptions *CMD, uint64_t SwitchNum) {
        const char *Result = NULL;
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "GetSwitchResult", "Pointer to CommandLineOptions is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) {
            Log(LOG_ERR, "libBitIO", "GetSwitchResult", "SwitchNum: %d, should be between 0 and %d\n", SwitchNum, CMD->NumSwitches);
        } else {
            Result = CMD->Switch[SwitchNum].SwitchResult;
        }
        return Result;
    }
    
    bool GetSwitchPresence(CommandLineOptions *CMD, uint64_t SwitchNum) {
        bool Status = 0;
        if (CMD == NULL) {
            Log(LOG_ERR, "libBitIO", "GetSwitchPresence", "Pointer to CommandLineOptions is NULL\n");
        } else if (SwitchNum > CMD->NumSwitches) { // - 1 so the hidden help option isn't exposed
            Log(LOG_ERR, "libBitIO", "GetSwitchPresence", "SwitchNum: %d, should be between 0 and %d\n", SwitchNum, CMD->NumSwitches);
        } else {
            Status = CMD->Switch[SwitchNum].SwitchFound;
        }
        return Status;
    }
    
    static void UpdateInputBuffer(BitInput *BitI, const int64_t RelativeOffsetInBytes) { // MARK: This function needs to remain internal
        uint64_t Bytes2Read = 0, BytesRead = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "UpdateInputBuffer", "Pointer to BitInput is NULL\n");
        } else {
            fseek(BitI->File, RelativeOffsetInBytes, SEEK_CUR);
            BitI->FilePosition = ftell(BitI->File);
            memset(BitI->BitB->Buffer, 0, sizeof(BitI->BitB->Buffer));
            Bytes2Read = BitI->FileSize - BitI->FilePosition >= BitInputBufferSize ? BitInputBufferSize : BitI->FileSize - BitI->FilePosition;
            BytesRead  = fread(BitI->BitB->Buffer, 1, Bytes2Read, BitI->File);
            if (BytesRead != Bytes2Read) {
                Log(LOG_WARNING, "libBitIO", "UpdateInputBuffer", "Supposed to read %llu bytes, but read %llu\n", Bytes2Read, BytesRead);
            }
            uint64_t NEWBitsUnavailable = BitI->BitB->BitsUnavailable % 8; // FIXME: This assumes UpdateBuffer was called with at most 7 unused bits in the buffer...
            
            BitI->BitB->BitsUnavailable = NEWBitsUnavailable;
            BitI->BitB->BitsAvailable   = Bytes2Bits(BytesRead);
        }
    }
    
    uint64_t ReadBits(BitBuffer *BitB, const uint8_t Bits2Read, bool ReadFromMSB) {
        uint8_t Bits = Bits2Read, UserBits = 0, SystemBits = 0, Mask = 0, Data = 0, Mask2Shift = 0;
        uint64_t OutputData = 0;
        
        if ((Bits2Read <= 0) || (Bits2Read > 64)) {
            Log(LOG_ERR, "libBitIO", "ReadBits", "ReadBits: %d, only supports reading 1-64 bits\n", Bits2Read);
        } else {
            SystemBits             = 8 - (BitB->BitsUnavailable % 8);
            UserBits               = Bits > 8 ? 8 : Bits;
            Bits                   = SystemBits >= UserBits  ? UserBits : SystemBits;
            Mask2Shift             = ReadFromMSB ? BitB->BitsAvailable % 8 : 0;
            if (ReadFromMSB == true) {
                Mask2Shift         = SystemBits <= UserBits  ? 0 : SystemBits - UserBits;
                Mask               = (Power2Mask(Bits) << Mask2Shift);
            } else {
                Mask               = (Powi(2, Bits) - 1) << BitB->BitsUnavailable % 8;
            }
            Data                   = BitB->Buffer[BitB->BitsUnavailable / 8] & Mask;
            Data                 >>= Mask2Shift;
            OutputData           <<= SystemBits >= UserBits ? UserBits : SystemBits;
            OutputData            += Data;
            BitB->BitsAvailable   -= SystemBits >= UserBits ? UserBits : SystemBits;
            BitB->BitsUnavailable += SystemBits >= UserBits ? UserBits : SystemBits;
            Bits                  -= SystemBits >= UserBits ? UserBits : SystemBits;
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
    
    void WriteBits(BitBuffer *BitB, const uint64_t Data2Write, uint8_t NumBits, const bool WriteFromMSB) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBits", "Pointer to BitBuffer is NULL\n");
        } else {
            // FIXME: WriteBits currently copies NumBits bits to the file, even if the input is shorter than that. we need to prepend 0 bits if that's the case
            
            uint8_t Bits2Write2BufferByte, Bits2ShiftMask, Mask;
            
            // so we're trying to write 9 bits, 0x1FF.
            // There are only 5 bits available in the current byte.
            // So, that means we have to pull out 5 bits from Data2Write, and pop it into BitO->Buffer.
            // The Mask should be 0x1F if LSBfirst, or 0xF8 if MSB first.
            // The buffer's representation is MSB first.
            // if we're supposed to write this data LSB first we need to shift it after extraction to get it ready to be applied.
            while (NumBits > 0) {
                Bits2Write2BufferByte  = 8 - (BitB->BitsAvailable % 8); // extract 5 bits from the buffer
                Bits2ShiftMask         = WriteFromMSB ? BitB->BitsAvailable % 8 : 0;
                Mask                   = Power2Mask(Bits2Write2BufferByte) << Bits2ShiftMask;
                BitB->Buffer[Bits2Bytes(BitB->BitsAvailable, false)] = Data2Write & Mask;
                NumBits               -= Bits2Write2BufferByte;
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
    
    size_t GetBitInputBufferSize(BitInput *BitI) { // FIXME: Is this even relevent anymore?
        uint64_t BufferSize = 0;
        if (BitI == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitInputBufferSize", "Pointer to BitOutput is NULL\n");
        } else {
            BufferSize = sizeof(BitI->BitB->Buffer);
        }
        return BufferSize;
    }
    
    size_t GetBitOutputBufferSize(BitOutput *BitO) { // FIXME: Is this even relevent anymore?
        uint64_t BufferSize = 0;
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "GetBitOutputBufferSize", "Pointer to BitOutput is NULL\n");
        } else {
            BufferSize = sizeof(BitO->BitB->Buffer);
        }
        return BufferSize;
    }
    
    void FlushOutputBuffer(BitOutput *BitO, const size_t Bytes2Flush) {
        if (BitO == NULL) {
            Log(LOG_ERR, "libBitIO", "FlushOutputBuffer", "Pointer to BitOutput is NULL\n");
        } else if (Bytes2Flush <= 0) {
            Log(LOG_ERR, "libBitIO", "FlushOutputBuffer", "Bytes2Flush is %d, should be >= 1\n", Bytes2Flush);
        } else {
            size_t BytesWritten = 0;
            BytesWritten = fwrite(BitO->BitB->Buffer, 1, Bytes2Flush, BitO->File);
            if (BytesWritten != Bytes2Flush) {
                Log(LOG_EMERG, "libBitIO", "FlushOutputBuffer", "Wrote %d bytes out of %d", BytesWritten, Bytes2Flush);
            }
        }
    }
    
    uint64_t GenerateCRC(const uint8_t *Data2CRC, const size_t DataSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit) {
        if (PolySize % 8 != 0) {
            // Ok, so we also need to add the ability to do incremental CRC generation for the iDAT/fDAT chunks in PNG
            
            
            // You have to do it bitwise
        } else if (DataSize % PolySize || DataSize > PolySize) {
            // do it word wise aka grab PolySize bits from Data2CRC at once
        } else {
            // Do it bytewise
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
    
    static void ConvertBinaryUUID2UUIDString(const uint8_t *BinaryUUID, uint8_t *UUIDString) {
        if (sizeof(BinaryUUID) != BitIOBinaryUUIDSize) {
            Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "BinaryUUID size should be: %d, but is: %d\n", BitIOBinaryUUIDSize, sizeof(BinaryUUID));
        } else if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertBinaryUUID2UUIDString", "UUIDString size should be: %d, but is: %d\n", BitIOUUIDStringSize, sizeof(UUIDString));
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
    
    static void ConvertUUIDString2BinaryUUID(const uint8_t *UUIDString, uint8_t *BinaryUUID) {
        if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "UUIDString size should be: %d, but is: %d\n", BitIOUUIDStringSize, sizeof(UUIDString));
        } else if (sizeof(BinaryUUID) != BitIOBinaryUUIDSize) {
            Log(LOG_ERR, "libBitIO", "ConvertUUIDString2BinaryUUID", "BinaryUUID size should be: %d, but is: %d\n", BitIOBinaryUUIDSize, sizeof(BinaryUUID));
        } else {
            for (uint8_t Byte = 0; Byte < BitIOUUIDStringSize; Byte++) {
                if (Byte != 4 || Byte != 7 || Byte != 10 || Byte != 13 || Byte != 20) {
                    BinaryUUID[Byte] = UUIDString[Byte];
                }
            }
        }
    }
    
    static void SwapBinaryUUID(const uint8_t *BinaryUUID, uint8_t *SwappedBinaryUUID) { // Should I make wrappers called ConvertUUID2GUID and SwapGUID2UUID?
        if (sizeof(BinaryUUID) != BitIOBinaryUUIDSize) {
            Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "BinaryUUID size should be: %d, but is: %d\n", BitIOBinaryUUIDSize, sizeof(BinaryUUID));
        } else if (sizeof(SwappedBinaryUUID) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "SwapBinaryUUID", "SwappedBinaryUUID size should be: %d, but is: %d\n", BitIOUUIDStringSize, sizeof(SwappedBinaryUUID));
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
    
    void ReadUUID(BitBuffer *BitB, uint8_t *UUIDString) {
        if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ReadUUID", "UUIDString is: %d bytes long, it should be: %d\n", sizeof(UUIDString), BitIOUUIDStringSize);
        } else {
            uint8_t BinaryUUID[BitIOBinaryUUIDSize];
            for (uint8_t UUIDByte = 0; UUIDByte < BitIOBinaryUUIDSize; UUIDByte++) {
                BinaryUUID[UUIDByte] = ReadBits(BitB, 8, true);
            }
            ConvertBinaryUUID2UUIDString(BinaryUUID, UUIDString);
        }
    }
    
    void ConvertUUID2GUID(const uint8_t *UUIDString, uint8_t *GUIDString) {
        if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertUUID2GUID", "UUIDString size should be: %d, but is: %d\n", BitIOUUIDStringSize, sizeof(UUIDString));
        } else if (sizeof(GUIDString) != BitIOGUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertUUID2GUID", "BinaryUUID size should be: %d, but is: %d\n", BitIOBinaryUUIDSize, sizeof(GUIDString));
        } else {
            // So, first we call the function that'll convert a UUIDString to a BinaryUUID, then call SwapBinaryUUID, then convert the BinaryGUID back to a UUIDString.
            uint8_t BinaryUUID[BitIOBinaryUUIDSize];
            uint8_t BinaryGUID[BitIOBinaryUUIDSize];
            ConvertUUIDString2BinaryUUID(UUIDString, BinaryUUID);
            SwapBinaryUUID(BinaryUUID, BinaryGUID);
            ConvertBinaryUUID2UUIDString(BinaryGUID, GUIDString);
        }
    }
    
    void ConvertGUID2UUID(const uint8_t *GUIDString, uint8_t *UUIDString) {
        if (sizeof(GUIDString) != BitIOGUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertGUID2UUID", "GUIDString size should be: %d, but is: %d\n", BitIOGUIDStringSize, sizeof(GUIDString));
        } else if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "ConvertGUID2UUID", "UUIDString size should be: %d, but is: %d\n", BitIOUUIDStringSize, sizeof(UUIDString));
        } else {
            uint8_t BinaryGUID[BitIOBinaryUUIDSize];
            ConvertUUIDString2BinaryUUID(GUIDString, BinaryGUID);
            uint8_t BinaryUUID[BitIOBinaryUUIDSize];
            SwapBinaryUUID(BinaryGUID, BinaryUUID);
            ConvertBinaryUUID2UUIDString(BinaryUUID, UUIDString);
        }
    }
    
    bool CompareUUIDs(const uint8_t *UUIDString1, const uint8_t *UUIDString2) {
        bool UUIDsMatch = 0;
        
        if (sizeof(UUIDString1) != BitIOUUIDStringSize || sizeof(UUIDString2) != BitIOUUIDStringSize) {
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
            Log(LOG_ERR, "BitIO", "WriteUUID", "Pointer to instance of BitBuffer is NULL\n");
        } else if (sizeof(UUIDString) != BitIOUUIDStringSize) {
            Log(LOG_ERR, "libBitIO", "WriteUUID", "UUIDString is %d bytes long, should be 21\n", sizeof(UUIDString));
        } else {
            // Convert UUIDString to BinaryUUID, then write that.
            uint8_t BinaryUUID[BitIOBinaryUUIDSize];
            ConvertUUIDString2BinaryUUID(UUIDString, BinaryUUID);
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
    
    void ReadFile2Buffer(FILE *InputFile, BitBuffer *Buffer2Read, size_t Bytes2Read) {
        // Should this just take in BitInput?
        if (InputFile == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadFile2Buffer", "InputFile pointer is NULL\n");
        } else if (Buffer2Read == NULL) {
            Log(LOG_ERR, "libBitIO", "ReadFile2Buffer", "Buffer2Read pointer is NULL\n");
        } else {
            size_t BytesRead = 0;
            memset(&Buffer2Read->Buffer, 0, sizeof(Buffer2Read->Buffer));
            BytesRead                    = fread(Buffer2Read->Buffer, 1, Bytes2Read, InputFile);
            Buffer2Read->BitsAvailable   = Bytes2Bits(BytesRead);
            Buffer2Read->BitsUnavailable = 0;
            
        }
    }
    
    void ReadSocket2Buffer(BitInput *BitI, size_t Bytes2Read) { // Define it in the header when it's done
        
    }
    
    void WriteBuffer2File(FILE *OutputFile, BitBuffer *Buffer2Write, size_t Bytes2Write) {
        size_t BytesWritten = 0;
        if (OutputFile == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBuffer2File", "OutputFile pointer is NULL\n");
        } else if (Buffer2Write == NULL) {
            Log(LOG_ERR, "libBitIO", "WriteBuffer2File", "Buffer2Write pointer is NULL\n");
        } else {
            BytesWritten                  = fwrite(Buffer2Write->Buffer, 1, Bytes2Write, OutputFile);
            memset(&Buffer2Write->Buffer, 0, BytesWritten);
            if (BytesWritten != Bytes2Write) {
                Log(LOG_CRIT, "libBitIO", "WriteBuffer2File", "Wrote %d bytes instead of %d\n", BytesWritten, Bytes2Write);
            }
            Buffer2Write->BitsAvailable   = 0;
            Buffer2Write->BitsUnavailable = 0;
        }
    }
    
    void WriteBuffer2Socket(BitOutput *BitO, BitBuffer *BitB, int Socket) { // Define it in the header when it's done
        
    }
    
    void Log(const uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription, ...) {
        char ErrorString[BitIOStringSize];
        char VariadicArguments[BitIOStringSize];
        
        va_list ExtraArguments;
        va_start(ExtraArguments, ErrorDescription);
        //va_arg(ExtraArguments, const char);
        //vsnprintf(VariadicArguments, BitIOStringSize, "%s", ExtraArguments);
        char *VarArgString = va_arg(ExtraArguments, char);
        vsnprintf(VariadicArguments, BitIOStringSize, "%s", VarArgString);
        va_end(ExtraArguments);
        
        snprintf(ErrorString, BitIOStringSize, "%s - %s: %s - %s\n", LibraryOrProgram, Function, ErrorDescription, VariadicArguments);
#ifdef _WIN32
        uintptr_t *EventLog = RegisterEventSource(NULL, LibraryOrProgram);
        uint32_t ErrorCode  = ReportEvent(EventLog, ErrorLevel, 1, 0xF000FFFF, NULL, 1, strlen(ErrorString), ErrorString, NULL);
        if (ErrorCode != 0) {
            fprintf(stderr, "BitIO - Log: Windows version of Logger failed with error: %s\n", ErrorString);
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
    
#ifdef __cplusplus
}
#endif
