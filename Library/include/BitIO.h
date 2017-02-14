/*!
 @header    BitIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2016, Marcus Johnson
 @version   0.9.1
 FIXME: The Version number needs to be FeatureLevel.ABI.BugFix
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _POSIX_VERSION
#include <dirent.h>
#include <libgen.h>
#include <syslog.h>
#include <unistd.h>
#include <xlocale.h>
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
     @constant       BitIOPathSize             "Maximum size of a path in the filesystem in BitIO".
     @constant       BitIOUUIDSize             "size of a UUIDString including null terminator".
     */
    extern enum BitIOConstants {
        BitInputBufferSize        = 4096,
        BitInputBufferSizeInBits  = BitInputBufferSize * 8,
        BitOutputBufferSize       = 4096,
        BitOutputBufferSizeInBits = BitOutputBufferSize * 8,
        BitIOStringSize           = 4096,
        BitIOPathSize             = 4096,
        BitIOUUIDSize             = 21,
        BitIOEncodedUUIDSize      = 16,
        BitIOGUIDSize             = 21,
        BitIOEncodedGUIDSize      = 16,
        BitIOFlagSize             = 3,
        BitIOMD5Size              = 16,
        BitIOMaxSwitches          = 64,
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
        uint8_t      SystemEndian:2;
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
     @constant SwitchFound       "If the switch was found in argv, this will be set to 1".
     @constant Switch            "Actual switch, including dash(es)".
     @constant SwitchDescription "Message to print explaining what the switch does".
     @constant SwitchResult      "String to contain the result of this switch, NULL if not found".
     */
    typedef struct CLSwitch {
        bool        SwitchFound;
        bool        Resultless;
        char       *Switch;
        char       *SwitchDescription;
        char       *SwitchResult;
    } CLSwitch;
    
    /*!
     */
    typedef struct CommandLineOptions {
        size_t      NumSwitches;
        char       *ProgramName;
        char       *ProgramDescription;
        char       *AuthorCopyrightLicense;
        CLSwitch   *Switch[BitIOMaxSwitches];
    } CommandLineOptions;
    
    extern enum Endian {
        UnknownEndian = 0,
        BigEndian     = 1,
        LittleEndian  = 2,
    } Endian;
    
#ifndef _POSIX_VERSION
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
     @param    Data2Swap               "Data to swap endian".
     */
    uint16_t       SwapEndian16(uint16_t Data2Swap);
    
    /*!
     @abstract                         "Swap endian of 32 bit integers".
     @param    Data2Swap               "Data to swap endian".
     */
    uint32_t       SwapEndian32(uint32_t Data2Swap);
    
    /*!
     @abstract                         "Swap endian of 64 bit integers".
     @param    Data2Swap               "Data to swap endian".
     */
    uint64_t       SwapEndian64(uint64_t Data2Swap);
    
    /*!
     @abstract                         "Computes the number of bytes from the number of bits".
     @remark                           "ONLY USE FOR ARRAY INDEXING. DOES NOT ROUND, ANYTHING LESS THAN 8 = 0".
     @return                           "The number of bytes".
     */
    uint64_t       Bits2Bytes(uint64_t Bits);
    
    /*!
     @abstract                         "Computes the number of bits from the number of bytes".
     @remark                           "Does not have sub-byte precision".
     @return                           "Converts the number of bytes to the number of bits".
     */
    uint64_t       Bytes2Bits(uint64_t Bytes);
    
    /*!
     @abstract                         "Computes the number of bits until the next byte".
     @return                           "Returns the number of bits left".
     */
    uint8_t        BitsRemaining(uint64_t BitsAvailable);
    
    /*!
     @abstract                         "Converts an Signed int to a Unsigned int".
     @remark                           "The internal representation in BitIO is unsigned".
     @return                           "Returns the input data in Unsigned format".
     */
    uint64_t       Signed2Unsigned(int64_t Signed);
    
    /*!
     @abstract                         "Converts an Unsigned int to a Signed int".
     @remark                           "The internal representation in BitIO is unsigned".
     @return                           "Returns the input data in Signed format".
     */
    int64_t        Unsigned2Signed(uint64_t Unsigned);
    
    uint64_t       Powi(uint64_t Base, uint64_t Exponent);
    
    int64_t        Floori(double X);
    
    int64_t        Ceili(long double X);
    
    uint8_t        CountBitsSet(uint64_t Data);
    
    /*!
     @abstract                     "Create bitmask from binary exponent".
     
     @param    Exponent            "Power to be raised by 2".
     @return                       "If 0 is returned, then Exponent was too large".
     */
    uint64_t       Power2Mask(uint8_t Exponent);
    
    /*!
     @abstract                         "Converts numbers from One's compliment to Two's compliment".
     @return                           "Returns the Input in Two's compliment format".
     */
    uint64_t       OnesCompliment2TwosCompliment(int64_t Input);
    
    /*!
     @abstract                         "Converts numbers from Two's compliment to One's compliment".
     @return                           "Returns the Input in One's compliment format".
     */
    uint64_t       TwosCompliment2OnesCompliment(int64_t Input);
    
    uint8_t        DetectSystemEndian(void);
    
    /*!
     @abstract                     "Tells if the stream/buffer is byte aligned of not".
     
     @param    BitsUsed            "Bits read from the stream, to calculate if it's on a byte aligned address".
     @param    BytesOfAlignment    "Number of bytes to check alignment with, aka is BitsUsed a multiple of BytesOfAlignment * 8?".
     */
    bool           IsStreamByteAligned(uint64_t BitsUsed, uint8_t BytesOfAlignment);
    
    /*!
     @abstract                         "Aligns bits for multi-byte alignment".
     @remark                           "Aligns to specified byte without care about the current position".
     
     @param    BitI                    "Pointer to BitInput".
     @param    BytesOfAlignment        "Number of bytes of padding to align it to"
     */
    void           AlignInput(BitInput *BitI, uint8_t BytesOfAlignment);
    
    /*!
     @abstract                         "Aligns bits for multi-byte alignment".
     @remark                           "TODO: Should this be Input/Output neutral?".
     
     @param    BitO                    "Pointer to BitOutput".
     @param    BytesOfAlignment        "Number of bytes of padding to align it to"
     */
    void           AlignOutput(BitOutput *BitO, uint8_t BytesOfAlignment);
    
    /*!
     @abstract                         "Tells whether the input number is even or odd".
     @returns                          "True for odd, false for even".
     */
    bool           IsOdd(int64_t Input);
    
    uint8_t        FindHighestBitSet(uint64_t X);
    
    void           DisplayCMDHelp(CommandLineOptions *CMD);
    
    void           ParseCommandLineArguments(CommandLineOptions *CMD, int argc, const char *argv[]);
    
    void           OpenCMDInputFile(BitInput *BitI, CommandLineOptions *CMD, uint8_t InputSwitch);
    
    void           OpenCMDOutputFile(BitOutput *BitO, CommandLineOptions *CMD, uint8_t InputSwitch);
    
    /*!
     @abstract                         "Writes BitO->Buffer to BitO->File".
     */
    void           FlushBitOutput(BitOutput *BitO);
    
    /*!
     @abstract                     "Deallocates BitInput".
     @remark                       "For use when changing files, or exiting the program".
     
     @param    BitI                "Pointer to BitInput".
     */
    void           CloseBitInput(BitInput *BitI);
    
    /*!
     @abstract                     "Deallocates BitOutput"
     @remark                       "For use when changing files, or exiting the program".
     
     @param    BitO                "Pointer to BitOutput".
     */
    void           CloseBitOutput(BitOutput *BitO);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    /*!
     @abstract                     "Manages InputBuffer and hands out the requested bits".
     @remark                       "DO NOT try reading backwards, it will not work. for that use SkipBits()".
     
     @param    BitI                "Pointer to BitInput struct".
     @param    Bits2Read           "Number of bits to read".
     
     TODO: Consider - Maybe it's not a good idea to buffer, because it limits you to streaming formats only...
     TODO: Grow the buffer to accomidate a buffered formats.
     */
    uint64_t       ReadBits(BitInput *BitI, uint8_t Bits2Read);
    
    /*!
     @abstract                     "Reads data encoded as Exponential-Golomb aka Elias Gamma".
     @return                       "Returns the decoded value of the Elias/Golomb code".
     
     @param    BitI                "Pointer to BitInput".
     @param    IsSigned            "Should it be read as signed or unsigneed?".
     */
    int64_t        ReadExpGolomb(BitInput *BitI, bool IsSigned);
    
    /*!
     @abstract                     "Reads and Decodes unary/RICE encoded data from BitInput stream".
     @return                       "Returns the count of bits aka the value encoded by the encoder".
     
     @param    BitI                "Pointer to BitInput, the Input stream".
     @param    Truncated           "Shoould the StopBit be included in the count?"
     @param    StopBit             "MUST be a 0 or a 1. none of this funny business about how true > 0".
     */
    uint64_t       ReadRICE(BitInput *BitI, bool Truncated, uint8_t StopBit);
    
    /*!
     @abstract                     "Encodes and writes data in unary/RICE format to a BitOutput stream".
     
     @param    BitO                "Pointer to BitOutput, the output stream".
     @param    Truncated           "Should the stop bit be pruned?"
     @param    StopBit             "Has to be a 0 or a 1".
     @param    Data2Write          "Number to encode into RICE format".
     */
    void           WriteRICE(BitOutput *BitO, bool Truncated, bool StopBit, uint64_t Data2Write);
    
    /*!
     @abstract                     "Shows the next X bits, without recording it as a read".
     @param    BitI                "Pointer to BitInput".
     @param    Bits2Peek           "Number of bits to peek".
     */
    uint64_t       PeekBits(BitInput *BitI, uint8_t Bits2Peek);
    
    /*!
     @abstract                     "Writes bits to BitOutput->File".
     
     @param    BitO                "Pointer to BitOutput".
     @param    Data2Write          "Is the actual data to write out".
     @param    NumBits             "Is the number of bits to write".
     */
    void           WriteBits(BitOutput *BitO, uint64_t Data2Write, uint8_t NumBits);
    
    /*!
     @abstract                     "Seeks Forwards and backwards in BitInput"
     @remark                       "To seek backwards just use a negative number, to seek forwards positive".
     
     @param    BitI                "Pointer to BitInput".
     @param    Bits                "The number of bits to skip".
     */
    void           SkipBits(BitInput *BitI, int64_t Bits);
    
    /*!
     @abstract                     "Raise a base to the exponent".
     */
    uint64_t       Powi(uint64_t Base, uint64_t Exponent);
    
    /*!
     @abstract                     "Integer floor function"
     */
    int64_t        Floori(double X);
    
    /*!
     @abstract                     "Integer ceil function"
     */
    int64_t        Ceili(long double X);
    
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
    
    /*!
     @abstract                     "Generates CRC from data".
     
     @param    BitI                "Pointer to Initalized BitInput type to read from data".
     @param    DataSize            "Size of the data chunk to generate the CRC for".
     @param    CRCData             "Pointer to struct containing the CRC parameters, like Polynomial, etc"
     */
    uint64_t       GenerateCRC(BitInput *BitI, size_t DataSize, CRC *CRCData);
    
    /*!
     @abstract                     "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     
     @param    BitI                "Pointer to Initalized BitInput type to read from data".
     @param    DataSize            "Size of the data chunk to generate the CRC for".
     @param    CRCData             "Pointer to struct containing the CRC parameters, like Polynomial, etc"
     */
    bool           VerifyCRC(BitInput *BitI, size_t DataSize, CRC *CRCData);
    
    /*!
     @abstract                     "Creates Adler32 checksum from input data".
     @return                       "Returns the Adler32 data from the data input".
     
     @param   Data                 "Pointer to the data to generate the Adler hash from".
     @param   DataSize             "Size of data".
     */
    uint32_t       GenerateAdler32(uint8_t *Data, size_t DataSize);
    
    /*!
     @abstract                     "Generates Adler32 from the input data, and compares it to the submitted checksum".
     @return                       "Returns whether the input data matched the provided checksum or not".
     
     @param   Data                 "Pointer to the data to generate the Adler hash from".
     @param   DataSize             "Size of data".
     @param   EmbeddedAdler32      "Embedded Adler32 to compare the generated one to.".
     */
    bool           VerifyAdler32(uint8_t *Data, size_t DataSize, uint32_t EmbeddedAdler32);
    
    /*!
     @abstract                     "Generates an MD5 hash from the buffer of data you pass in".
     
     @param   BufferSize           "Number of bits in Buffer".
     */
    //void           GenerateMD5(uint8_t *Buffer, size_t BufferSize, char MD5String[BitIOMD5Size]);
    
    /*!
     @abstract                     "Logs errors to log files, and stderr; and mail if Critical/Panic."
     
     @param    ErrorLevel          "Error message prefixed by SYS in ErrorCodes".
     @param    LibraryOrProgram    "Name of the program or library that called this function, to name the logfile".
     @param    Function            "Which function is calling Log?".
     @param    ErrorDescription    "String describing what went wrong, if you need to use format specifiers, call snprintf".
     */
    void           Log(uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription);
    
    /*!
     @abstract                     "Writes data encoded as Exponential-Golomb aka Elias Gamma".
     */
    void WriteExpGolomb(BitOutput *BitO, bool IsSigned, uint64_t Data2Write);
    
    uint8_t CountBitsSet(uint64_t Data);
    
    typedef struct LinkedList {
        uint16_t           Value;
        struct LinkedList *Next;
    } LinkedList;
    
#ifdef __cplusplus
}
#endif
