/*!
 @header    BitIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2016, Marcus Johnson
 @version   0.9.1
 FIXME: The Version number needs to be FeatureLevel.ABI.BugFix
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_POSIX_VERSION)||(__APPLE__)
#include <syslog.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
#pragma once
    
    /*!
     @abstract                                 "BitIO compile time constants".
     @remark                                   "Change the buffer sizes here".
     
     @constant       BitInputBufferSize        "Inital size of BitInput buffer".
     @constant       BitInputBufferSizeInBits  "Inital size of BitInput buffer in bits".
     @constant       BitOutputBufferSize       "Inital size of BitOutput buffer".
     @constant       BitOutputBufferSizeInBits "Inital size of BitOutput buffer in bits".
     @constant       BitIOStringSize           "Maximum size of a string in BitIO".
     @constant       BitIOUUIDSize             "Size of a UUIDString including dashes and null terminator".
     @constant       BitIOEncodedUUIDSize      "Size of a binary UUID string".
     @constant       BitIOGUIDSize             "size of a GUIDString including dashes and null terminator".
     @constant       BitIOEncodedGUIDSize      "Size of a binary GUID string".
     @constant       BitIOMD5Size              "Size of a MD5 string".
     */
    extern enum BitIOConstants {
        BitInputBufferSize        = 4096,
        BitInputBufferSizeInBits  = BitInputBufferSize * 8,
        BitOutputBufferSize       = 4096,
        BitOutputBufferSizeInBits = BitOutputBufferSize * 8,
        BitIOStringSize           = 4096,
        BitIOUUIDSize             = 21,
        BitIOEncodedUUIDSize      = 16,
        BitIOGUIDSize             = BitIOUUIDSize,
        BitIOEncodedGUIDSize      = BitIOEncodedUUIDSize,
        BitIOMD5Size              = 16,
    } BitIOConstants;
    
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
    
    /*!
     @typedef  CLSwitch
     @abstract                         "Contains the data to support a single switch".
     @remark                           "You MUST include the null padding at the end of @Switch".
     @constant SwitchFound             "If the switch was found in argv, this will be set to true".
     @constant Resultless              "Is the mere presence of the switch what you're looking for? if so, set to true"
     @constant Switch                  "Actual switch, including dash(s), slash, etc.".
     @constant SwitchDescription       "Message to print explaining what the switch does".
     @constant SwitchResult            "String to contain the result of this switch, NULL if not found".
     */
    typedef struct CLSwitch {
        bool        SwitchFound:1;
        bool        Resultless:1;
        char       *Switch;
        char       *SwitchDescription;
        char       *SwitchResult;
    } CLSwitch;
    
    /*!
     @typedef  CommandLineOptions
     @abstract                         "Type to contain a variable amount of CLSwitches".
     @remark                           "The switches are zero indexed, and @NumSwitches is NOT zero indexed, so count from 1".
     @constant NumSwitches             "The number of switches".
     @constant ProgramName             "The name you want output when the help is printed".
     @constant ProgramDescription      "The description of the program when the help is printed".
     @constant AuthorCopyrightLicense  "The author, copyright years, and license of the program, or anything else you want printed".
     @constant Switch                  "A pointer to an array of CLSwitch instances containing the properties of the switches".
     */
    typedef struct CommandLineOptions {
        size_t      NumSwitches;
        char       *ProgramName;
        char       *ProgramDescription;
        char       *AuthorCopyrightLicense;
        CLSwitch  **Switch;
    } CommandLineOptions;
    
    typedef struct LinkedList {
        uint16_t           Value;
        struct LinkedList *Next;
    } LinkedList;
    
    /*!
     @enum     Endiam
     @constant UnknownEndian           "The endian of the machine is currently unknown".
     @constant BigEndian               "The machine is Big endian".
     @constant LittleEndian            "The machine is little endian".
     */
    extern enum Endian {
        UnknownEndian = 0,
        BigEndian     = 1,
        LittleEndian  = 2,
    } Endian;
    
    extern enum    PolynomialType {
        Normal     = 0,
        Reversed   = 1,
    } PolynomialType;
    
    typedef struct CRC {
        uint8_t   *Buffer;
        size_t     BufferSize;
        uint64_t   Polynomial;
        bool       PolynomialType;
        uint8_t    PolynomialSize;
        uint64_t   Initalization;
        uint64_t   PreCalculatedCRC;
    } CRC;
    
#ifndef _POSIX_VERSION
    /*!
     @enum     LogTypes
     @constant LOG_EMERG               "The system is unusable, the program is quitting (equlivent to panic)".
     @constant LOG_ALERT               "Immediate action is required".
     @constant LOG_CRIT                "Critical condition encountered".
     @constant LOG_ERR                 "Error condition encountered".
     @constant LOG_WARNING             "Warning condition encountered".
     @constant LOG_NOTICE              "Normal, but important condition encountered".
     @constant LOG_INFO                "Informational message logged".
     @constant LOG_DEBUG               "Testing information logged".
     */
    extern enum LogTypes {
        LOG_EMERG   = 0,
        LOG_ALERT   = 1,
        LOG_CRIT    = 2,
        LOG_ERR     = 3,
        LOG_WARNING = 4,
        LOG_NOTICE  = 5,
        LOG_INFO    = 6,
        LOG_DEBUG   = 7,
    } LogTypes;
#endif
    
    /*!
     @abstract                         "Swap endian of 16 bit integers".
     @param        Data2Swap           "Data to swap endian".
     */
    uint16_t       SwapEndian16(const uint16_t Data2Swap);
    
    /*!
     @abstract                         "Swap endian of 32 bit integers".
     @param        Data2Swap           "Data to swap endian".
     */
    uint32_t       SwapEndian32(const uint32_t Data2Swap);
    
    /*!
     @abstract                         "Swap endian of 64 bit integers".
     @param        Data2Swap           "Data to swap endian".
     */
    uint64_t       SwapEndian64(const uint64_t Data2Swap);
    
    /*!
     @abstract                         "Computes the number of bytes from the number of bits, rounding up".
     @return                           "The number of bytes".
     @param        Bits                "The bits to convert to bytes".
     @param        RoundUp             "Should the resulting bytes be rounded up or down?"
     */
    int64_t        Bits2Bytes(const int64_t Bits, const bool RoundUp);
    
    /*!
     @abstract                         "Computes the number of bits from the number of bytes".
     @remark                           "Does not have sub-byte precision".
     @return                           "Converts the number of bytes to the number of bits".
     */
    uint64_t       Bytes2Bits(const uint64_t Bytes);
    
    /*!
     @abstract                         "Computes the number of bits until the next byte".
     @return                           "Returns the number of bits left in this byte".
     */
    uint8_t        BitsRemaining(const uint64_t BitsAvailable);
    
    /*!
     @abstract                         "Converts an Signed int to a Unsigned int".
     @param        Signed              "Signed data to convert to unsigned".
     @return                           "Returns the input data in Unsigned format".
     */
    uint64_t       Signed2Unsigned(const int64_t Signed);
    
    /*!
     @abstract                         "Converts an Unsigned int to a Signed int".
     @param        Unsigned            "Unsigned data to convert to signed".
     @return                           "Returns the input data in Signed format".
     */
    int64_t        Unsigned2Signed(const uint64_t Unsigned);
    
    /*!
     @abstract                         "Raise a base to the exponent".
     @param        Base                "The base to be multiplied by it's self Exponent times".
     @param        Exponent            "The number of times to multiply Base by it's self".
     */
    int64_t        Powi(int64_t Base, const int64_t Exponent);
    
    /*!
     @abstract                         "Integer floor function"
     */
    int64_t        Floori(const long double Number2Floor);
    
    /*!
     @abstract                         "Integer ceil function"
     */
    int64_t        Ceili(const long double Number2Ceil);
    
    /*!
     @abstract                         "Counts the number of bits that are set in a int".
     @param        Bits2Count          "The int who's bits should be counted".
     @return                           "The number of bits set in Bits2Count".
     */
    uint8_t        CountBitsSet(const uint64_t Bits2Count);
    
    /*!
     @abstract                         "Create bitmask from binary exponent".
     @param        Exponent            "Power to be raised by 2".
     @return                           "A bit mask generated from a power".
     */
    uint64_t       Power2Mask(const uint8_t Exponent);
    
    /*!
     @abstract                         "Converts numbers from One's compliment to Two's compliment"
     @param        OnesCompliment      "Int in 1's compliment form to convert into 2's compliment".
     @return                           "Returns OnesCompliment in 2's compliment format".
     */
    uint64_t       OnesCompliment2TwosCompliment(const int64_t OnesCompliment);
    
    /*!
     @abstract                         "Converts numbers from Two's compliment to One's compliment".
     @param        TwosCompliment      "Int in 2's compliment form to convert into 1's compliment".
     @return                           "Returns the TwosCompliment in 1's compliment format".
     */
    uint64_t       TwosCompliment2OnesCompliment(const int64_t TwosCompliment);
    
    /*!
     @abstract                         "Tells whether Input is even or odd".
     @returns                          "True for odd, false for even".
     */
    bool           IsOdd(const int64_t Number2Check);
    
    /*!
     @abstract                         "Finds the highest set bit in an int".
     @return                           "Returns the position of the highest set bit".
     */
    uint8_t        FindHighestBitSet(const uint64_t Integer2Search);
    
    /*!
     @abstract                         "Determins the endian-ness of the current system at runtime".
     @return                           'An int matching @enum Endian, defining the endian of the running system'.
     */
    uint8_t        DetectSystemEndian(void);
    
    /*!
     @abstract                         "Loops over the user's provided CLSwitches to print the help information.".
     @remark                           "Command Line Switches are case insensitive".
     @param        CMD                 "Pointer to CommandLineOptions".
     */
    void           DisplayCMDHelp(const CommandLineOptions *CMD);
    
    /*!
     @abstract                         "Parses argv for switches matching the ones contained in CMD".
     @remark                           "Automatically supports `-h`, `--h`, and `/?`".
     @param        CMD                 "Pointer to CommandLineOptions".
     */
    void           ParseCommandLineArguments(const CommandLineOptions *CMD, int argc, const char *argv[]);
    
    /*!
     @abstract                         "Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
     @param        BitI                "Pointer to BitInput".
     @param        CMD                 "Pointer to CommandLineOptions".
     @param        InputSwitch         "Number of the switch that contains the Input file"
     */
    void           OpenCMDInputFile(BitInput *BitI, const CommandLineOptions *CMD, const uint8_t InputSwitch);
    
    /*!
     @abstract                         "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitO->File".
     @param        BitO                "Pointer to BitOutput".
     @param        CMD                 "Pointer to CommandLineOptions".
     @param        OutputSwitch        "Number of the switch that contains the Output file"
     */
    void           OpenCMDOutputFile(BitOutput *BitO, const CommandLineOptions *CMD, const uint8_t OutputSwitch);
    
    /*!
     @abstract                         "Manages InputBuffer and hands out the requested bits".
     @remark                           "DO NOT try reading backwards, it will not work. for that use SkipBits()".
     @param        BitI                "Pointer to BitInput".
     @param        Bits2Read           "Number of bits to read".
     @param        ReadFromMSB         "Should ReadBits start at the most significant bit in this byte?"
     */
    uint64_t       ReadBits(BitInput *BitI, const uint8_t Bits2Read, bool ReadFromMSB);
    
    /*!
     @abstract                         "Shows the next X bits, without recording it as a read".
     @param        BitI                "Pointer to BitInput".
     @param        Bits2Peek           "Number of bits to peek".
     @param        ReadFromMSB         "Should PeekBits start at the most significant bit in this byte?"
     */
    uint64_t       PeekBits(BitInput *BitI, const uint8_t Bits2Peek, bool ReadFromMSB);
    
    /*!
     @abstract                         "Reads and Decodes unary/RICE encoded data from BitInput stream".
     @return                           "Returns the count of bits aka the value encoded by the encoder".
     @param        BitI                "Pointer to BitInput".
     @param        Truncated           "Shoould the StopBit be included in the count?"
     @param        StopBit             "MUST be a 0 or a 1. none of this funny business about how true > 0".
     */
    uint64_t       ReadRICE(BitInput *BitI, const bool Truncated, const bool StopBit);
    
    /*!
     @abstract                         "Reads data encoded as Exponential-Golomb aka Elias Gamma".
     @return                           "Returns the decoded value of the Elias/Golomb code".
     @param        BitI                "Pointer to BitInput".
     @param        IsSigned            "Should it be read as signed or unsigneed?".
     */
    int64_t        ReadExpGolomb(BitInput *BitI, const bool IsSigned);
    
    /*!
     @abstract                         "Seeks Forwards and backwards in BitInput"
     @remark                           "To seek backwards just use a negative number, to seek forwards positive".
     @param        BitI                "Pointer to BitInput".
     @param        Bits2Skip           "The number of bits to skip".
     */
    void           SkipBits(BitInput *BitI, const int64_t Bits2Skip);
    
    /*!
     @abstract                         "Pads output buffer so it is aligned to whatever number of bytes you want".
     @param        BitI                "Pointer to BitInput".
     @param        BytesOfAlignment    "Align BitI to X byte boundary".
     */
    void           AlignInput(BitInput *BitI, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                         "Deallocates BitInput".
     @remark                           "For use when changing files, or exiting the program".
     @param        BitI                "Pointer to BitInput".
     */
    void           CloseBitInput(BitInput *BitI);
    
    /*!
     @abstract                         "Tells if the stream/buffer is byte aligned or not".
     @param        BitsUsed            "Number of bits previously read from the stream, to calculate if it's on a byte aligned address".
     @param        BytesOfAlignment    "Are you trying to see if it's aligned to a byte, short, word, etc alignment? Specify in number of bytes".
     */
    bool           IsStreamByteAligned(const uint64_t BitsUsed, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                         "Aligns bits for multi-byte alignment".
     @param        BitO                "Pointer to BitOutput".
     @param        BytesOfAlignment    "Align BitI to X byte boundary".
     */
    void           AlignOutput(BitOutput *BitO, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                         "Writes bits to BitOutput->File".
     @param        BitO                "Pointer to BitOutput".
     @param        Data2Write          "Is the actual data to write out".
     @param        NumBits             "Is the number of bits to write".
     */
    void           WriteBits(BitOutput *BitO, uint64_t Data2Write, uint8_t NumBits);
    
    /*!
     @abstract                         "Writes a buffer to a BitO pointer"
     @remark                           "Currently just a STUB".
     @param        BitO                "Pointer to BitOutput"
     @param        Buffer              "The buffer you want written to the disk"
     @param        IndexSize           "how many bits to read from a single buffer element, if the buffer is declared as uint16_t or int16_t, put 16, etc"
     @param        BitOffset           "Where in the buffer do you want to start writing from?"
     @param        Bits2Write          "The number of bits you want written from the buffer"
     @param        MSB                 "Should it read from the most significant bit of the byte?"
     */
    void           WriteBuffer(BitOutput *BitO, const uint64_t *Buffer, const uint8_t IndexSize, const size_t BitOffset, const size_t Bits2Write, const bool MSB);
    
    /*!
     @abstract                         "Encodes and writes data in unary/RICE format to a BitOutput stream".
     @param        BitO                "Pointer to BitOutput, the output stream".
     @param        Truncated           "Should the stop bit be pruned?"
     @param        StopBit             "Has to be a 0 or a 1".
     @param        Data2Write          "Number to encode into RICE format".
     */
    void           WriteRICE(BitOutput *BitO, const bool Truncated, const bool StopBit, const uint64_t Data2Write);
    
    /*!
     @abstract                         "Writes data encoded as Exponential-Golomb aka Elias Gamma codes to BitO".
     @param        BitO                "Pointer to BitOutput".
     */
    void           WriteExpGolomb(BitOutput *BitO, const bool IsSigned, const uint64_t Data2Write);
    
    /*!
     @abstract                         "Writes BitO->Buffer to BitO->File".
     @param        BitO                "Pointer to BitOutput".
     */
    void           FlushBitOutput(BitOutput *BitO);
    
    /*!
     @abstract                         "Deallocates BitOutput"
     @remark                           "For use when changing files, or exiting the program".
     @param        BitO                "Pointer to BitOutput".
     */
    void           CloseBitOutput(BitOutput *BitO);
    
    /*!
     @abstract                         "Generates CRC from data".
     @param        Data                "Pointer to a data buffer containing data to run the CRC algorithm over".
     @param        DataSize            "Size of the data chunk to generate the CRC for".
     @param        CRCData             "Pointer to struct containing the CRC parameters, like Polynomial, etc"
     */
    uint64_t       GenerateCRC(const uint8_t *Data, const size_t DataSize, CRC *CRCData);
    
    /*!
     @abstract                         "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     @param        Data                "Pointer to a data buffer containing data to run the CRC algorithm over".
     @param        DataSize            "Size of the data chunk to generate the CRC for".
     @param        CRCData             "Pointer to struct containing the CRC parameters, like Polynomial, etc"
     */
    bool           VerifyCRC(const uint8_t *Data, const size_t DataSize, CRC *CRCData);
    
    /*!
     @abstract                         "Creates Adler32 checksum from input data".
     @return                           "Returns the Adler32 data from the data input".
     @param        Data                "Pointer to the data to generate the Adler hash from".
     @param        DataSize            "Size of data".
     */
    uint32_t       GenerateAdler32(const uint8_t *Data, const size_t DataSize);
    
    /*!
     @abstract                         "Generates Adler32 from the input data, and compares it to the submitted checksum".
     @return                           "Returns whether the input data matched the provided checksum or not".
     @param        Data                "Pointer to the data to generate the Adler hash from".
     @param        DataSize            "Size of data".
     @param        EmbeddedAdler32     "Embedded Adler32 to compare the generated one to.".
     */
    bool           VerifyAdler32(const uint8_t *Data, const size_t DataSize, const uint32_t EmbeddedAdler32);
    
    /*!
     @abstract                         "Logs errors to log files, and stderr; and mail if Critical/Panic."
     @param        ErrorLevel          "Error message prefixed by SYS in ErrorCodes".
     @param        LibraryOrProgram    "Name of the program or library that called this function, to name the logfile".
     @param        Function            "Which function is calling Log?".
     @param        ErrorDescription    "String describing what went wrong / error code".
     */
    void           Log(const uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription);
    
#ifdef __cplusplus
}
#endif
