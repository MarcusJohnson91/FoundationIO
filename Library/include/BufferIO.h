/*!
 @header    BitIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2016, Marcus Johnson
 @version   0.9.2
 FIXME:     The Version number needs to be FeatureLevel.ABI.BugFix
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#pragma once

#ifndef LIBBITIO_BITIO_H
#define LIBBITIO_BITIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
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
    enum BitIOConstants {
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
    };
    
    typedef struct BitInput BitInput;
    
    typedef struct BitOutput BitOutput;
    
    typedef struct CommandLineOptions CommandLineOptions;
    
    /*!
     @enum     Endiam
     @constant UnknownEndian           "The endian of the machine is currently unknown".
     @constant BigEndian               "The machine is Big endian".
     @constant LittleEndian            "The machine is little endian".
     */
    enum Endian {
        UnknownEndian = 0,
        BigEndian     = 1,
        LittleEndian  = 2,
    };
    
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
    enum LogTypes {
        LOG_EMERG   = 0,
        LOG_ALERT   = 1,
        LOG_CRIT    = 2,
        LOG_ERR     = 3,
        LOG_WARNING = 4,
        LOG_NOTICE  = 5,
        LOG_INFO    = 6,
        LOG_DEBUG   = 7,
    };
#endif
    
    /*!
     @abstract                              "Swap endian of 16 bit integers".
     @param             Data2Swap           "Data to swap endian".
     */
    uint16_t            SwapEndian16(const uint16_t Data2Swap);
    
    /*!
     @abstract                              "Swap endian of 32 bit integers".
     @param             Data2Swap           "Data to swap endian".
     */
    uint32_t            SwapEndian32(const uint32_t Data2Swap);
    
    /*!
     @abstract                              "Swap endian of 64 bit integers".
     @param             Data2Swap           "Data to swap endian".
     */
    uint64_t            SwapEndian64(const uint64_t Data2Swap);
    
    /*!
     @abstract                              "Computes the number of bytes from the number of bits, rounding up".
     @return                                "The number of bytes".
     @param             Bits                "The bits to convert to bytes".
     @param             RoundUp             "Should the resulting bytes be rounded up or down?"
     */
    int64_t             Bits2Bytes(const int64_t Bits, const bool RoundUp);
    
    /*!
     @abstract                              "Computes the number of bits from the number of bytes".
     @remark                                "Does not have sub-byte precision".
     @return                                "Converts the number of bytes to the number of bits".
     */
    uint64_t            Bytes2Bits(const uint64_t Bytes);
    
    /*!
     @abstract                              "Computes the number of bits until the next byte".
     @return                                "Returns the number of bits left in this byte".
     */
    uint8_t             BitsRemainingInByte(const uint64_t BitsAvailable);
    
    /*!
     @abstract                              "Computes the number of bytes left in the file".
     @returm                                "Returns the number of bytes left in the file".
     */
    uint64_t            BytesRemainingInFile(BitInput *BitI);
    
    /*!
     @return                                "Returns the value in BitI->FileSize".
     */
    uint64_t            GetInputFileSize(BitInput *BitI);
    
    /*!
     @abstract                              "Converts an Signed int to a Unsigned int".
     @param             Signed              "Signed data to convert to unsigned".
     @return                                "Returns the input data in Unsigned format".
     */
    uint64_t            Signed2Unsigned(const int64_t Signed);
    
    /*!
     @abstract                              "Converts an Unsigned int to a Signed int".
     @param             Unsigned            "Unsigned data to convert to signed".
     @return                                "Returns the input data in Signed format".
     */
    int64_t             Unsigned2Signed(const uint64_t Unsigned);
    
    /*!
     @abstract                              "Raise a base to the exponent".
     @param             Base                "The base to be multiplied by it's self Exponent times".
     @param             Exponent            "The number of times to multiply Base by it's self".
     */
    int64_t             Powi(int64_t Base, const int64_t Exponent);
    
    /*!
     @abstract                              "Integer floor function"
     */
    int64_t             Floori(const long double Number2Floor);
    
    /*!
     @abstract                              "Integer ceil function"
     */
    int64_t             Ceili(const long double Number2Ceil);
    
    /*!
     @abstract                              "Counts the number of bits that are set in a int".
     @param             Bits2Count          "The int who's bits should be counted".
     @return                                "The number of bits set in Bits2Count".
     */
    uint8_t             CountBitsSet(const uint64_t Bits2Count);
    
    /*!
     @abstract                              "Create bitmask from binary exponent".
     @param             Exponent            "Power to be raised by 2".
     @return                                "A bit mask generated from a power".
     */
    uint64_t            Power2Mask(const uint8_t Exponent);
    
    /*!
     @abstract                              "Converts numbers from One's compliment to Two's compliment"
     @param             OnesCompliment      "Int in 1's compliment form to convert into 2's compliment".
     @return                                "Returns OnesCompliment in 2's compliment format".
     */
    uint64_t            OnesCompliment2TwosCompliment(const int64_t OnesCompliment);
    
    /*!
     @abstract                              "Converts numbers from Two's compliment to One's compliment".
     @param             TwosCompliment      "Int in 2's compliment form to convert into 1's compliment".
     @return                                "Returns the TwosCompliment in 1's compliment format".
     */
    uint64_t            TwosCompliment2OnesCompliment(const int64_t TwosCompliment);
    
    /*!
     @abstract                              "Tells whether Input is even or odd".
     @returns                               "True for odd, false for even".
     */
    bool                IsOdd(const int64_t Number2Check);
    
    /*!
     @abstract                              "Finds the highest set bit in an int".
     @return                                "Returns the position of the highest set bit".
     */
    uint8_t             FindHighestBitSet(const uint64_t Integer2Search);
    
    /*!
     @abstract                              "Determins the endian-ness of the current system at runtime".
     @return                                'An int matching @enum Endian, defining the endian of the running system'.
     */
    uint8_t             DetectSystemEndian(void);
    
    /*!
     @abstract                              "Loops over the user's provided CLSwitches to print the help information.".
     @remark                                "Command Line Switches are case insensitive".
     @param             CMD                 "Pointer to CommandLineOptions".
     */
    void                DisplayCMDHelp(const CommandLineOptions *CMD);
    
    /*!
     @abstract                              "Parses argv for switches matching the ones contained in CMD".
     @remark                                "Automatically supports `-h`, `--h`, and `/?`".
     @param             CMD                 "Pointer to CommandLineOptions".
     */
    void                ParseCommandLineArguments(const CommandLineOptions *CMD, int argc, const char *argv[]);
    
    /*!
     @abstract                              "Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
     @param             BitI                "Pointer to instance of BitInput".
     @param             CMD                 "Pointer to CommandLineOptions".
     @param             InputSwitch         "Number of the switch that contains the Input file"
     */
    void                OpenCMDInputFile(BitInput *BitI, const CommandLineOptions *CMD, const uint8_t InputSwitch);
    
    /*!
     @abstract                         "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitO->File".
     @param             BitO                "Pointer to instance of BitOutput".
     @param             CMD                 "Pointer to CommandLineOptions".
     @param             OutputSwitch        "Number of the switch that contains the Output file"
     */
    void                OpenCMDOutputFile(BitOutput *BitO, const CommandLineOptions *CMD, const uint8_t OutputSwitch);
    
    /*!
     @abstract                              "Manages InputBuffer and hands out the requested bits".
     @remark                                "DO NOT try reading backwards, it will not work. for that use SkipBits()".
     @param             BitI                "Pointer to instance of BitInput".
     @param             Bits2Read           "Number of bits to read".
     @param             ReadFromMSB         "Should ReadBits start at the most significant bit in this byte?"
     */
    uint64_t            ReadBits(BitInput *BitI, const uint8_t Bits2Read, bool ReadFromMSB);
    
    /*!
     @abstract                              "Shows the next X bits, without recording it as a read".
     @param             BitI                "Pointer to instance of BitInput".
     @param             Bits2Peek           "Number of bits to peek".
     @param             ReadFromMSB         "Should PeekBits start at the most significant bit in this byte?"
     */
    uint64_t            PeekBits(BitInput *BitI, const uint8_t Bits2Peek, bool ReadFromMSB);
    
    /*!
     @abstract                              "Reads and Decodes unary/RICE encoded data from BitInput stream".
     @return                                "Returns the count of bits aka the value encoded by the encoder".
     @param             BitI                "Pointer to instance of BitInput".
     @param             Truncated           "Shoould the StopBit be included in the count?"
     @param             StopBit             "MUST be a 0 or a 1. none of this funny business about how true > 0".
     */
    uint64_t            ReadRICE(BitInput *BitI, const bool Truncated, const bool StopBit);
    
    /*!
     @abstract                              "Reads data encoded as Exponential-Golomb aka Elias Gamma".
     @return                                "Returns the decoded value of the Elias/Golomb code".
     @param             BitI                "Pointer to instance of BitInput".
     @param             IsSigned            "Should it be read as signed or unsigneed?".
     */
    int64_t             ReadExpGolomb(BitInput *BitI, const bool IsSigned);
    
    /*!
     @abstract                              "Seeks Forwards and backwards in BitInput"
     @remark                                "To seek backwards just use a negative number, to seek forwards positive".
     @param             BitI                "Pointer to instance of BitInput".
     @param             Bits2Skip           "The number of bits to skip".
     */
    void                SkipBits(BitInput *BitI, const int64_t Bits2Skip);
    
    /*!
     @abstract                              "Pads output buffer so it is aligned to whatever number of bytes you want".
     @param             BitI                "Pointer to instance of BitInput".
     @param             BytesOfAlignment    "Align BitI to X byte boundary".
     */
    void                AlignInput(BitInput *BitI, const uint8_t BytesOfAlignment);
    
    /*!
     @avstact                               "Initializes a BitInput structure".
     @return                                "Returns a pointer to said BitInput structure".
     */
    BitInput           *InitBitInput(void);
    
    /*!
     @avstact                               "Initializes a BitOutput structure".
     @return                                "Returns a pointer to said BitOutput structure".
     */
    BitOutput          *InitBitOutput(void);
    
    /*!
     @abstract                              "Deallocates BitInput".
     @remark                                "For use when changing files, or exiting the program".
     @param             BitI                "Pointer to instance of BitInput".
     */
    void                CloseBitInput(BitInput *BitI);
    
    /*!
     @abstract                              "Tells if the stream/buffer is byte aligned or not".
     @param             BytesOfAlignment    "Are you trying to see if it's aligned to a byte, short, word, etc alignment? Specify in number of bytes".
     */
    bool                IsInputStreamByteAligned(BitInput *BitI, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                              "Tells if the stream/buffer is byte aligned or not".
     @param             BytesOfAlignment    "Are you trying to see if it's aligned to a byte, short, word, etc alignment? Specify in number of bytes".
     */
    bool                IsOutputStreamByteAligned(BitOutput *BitO, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                              "Aligns bits for multi-byte alignment".
     @param             BitO                "Pointer to instance of BitOutput".
     @param             BytesOfAlignment    "Align BitI to X byte boundary".
     */
    void                AlignOutput(BitOutput *BitO, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                              "Writes bits to BitOutput->File".
     @param             BitO                "Pointer to instance of BitOutput".
     @param             Data2Write          "Is the actual data to write out".
     @param             NumBits             "Is the number of bits to write".
     */
    void                WriteBits(BitOutput *BitO, const uint64_t Data2Write, uint8_t NumBits, const bool WriteFromMSB);
    
    /*!
     @abstract                              "Encodes and writes data in unary/RICE format to a BitOutput stream".
     @param             BitO                "Pointer to BitOutput, the output stream".
     @param             Truncated           "Should the stop bit be pruned?"
     @param             StopBit             "Has to be a 0 or a 1".
     @param             Data2Write          "Number to encode into RICE format".
     @param             WriteFromMSB        "Should Data2Write be written from the MSB or the LSB?".
     */
    void                WriteRICE(BitOutput *BitO, const bool Truncated, const bool StopBit, const uint64_t Data2Write, const bool WriteFromMSB);
    
    /*!
     @abstract                              "Writes data encoded as Exponential-Golomb aka Elias Gamma codes to BitO".
     @param             BitO                "Pointer to instance of BitOutput".
     @param             IsSigned            "Is Data2Write signed?".
     @param             Data2Write          "The actual data to write to the output file".
     @param             WriteFromMSB        "Should Data2Write be written from the MSB or the LSB?".
     */
    void                WriteExpGolomb(BitOutput *BitO, const bool IsSigned, const uint64_t Data2Write, const bool WriteFromMSB);
    
    /*!
     @abstract                              "Deallocates BitOutput"
     @remark                                "For use when changing files, or exiting the program".
     @param             BitO                "Pointer to instance of BitOutput".
     */
    void                CloseBitOutput(BitOutput *BitO);
    
    /*!
     @abstract                              "Generates CRC from data".
     @param             Data2CRC            "Pointer to a data buffer containing data to run the CRC algorithm over".
     @param             Data2CRCSize        "Size of the data chunk to generate the CRC for".
     @param             ReciprocalPoly      "The Polynomial in Normal representation".
     @param             PolySize            "The size of the polynomial in bits".
     @param             PolyInit            "Initalization value".
     */
    uint64_t            GenerateCRC(const uint8_t *Data2CRC, const size_t Data2CRCSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit);
    
    /*!
     @abstract                              "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     @param             Data2CRC            "Pointer to a data buffer containing data to run the CRC algorithm over".
     @param             Data2CRCSize        "Size of the data chunk to generate the CRC for".
     @param             ReciprocalPoly      "The Polynomial in Reciprocal representation".
     @param             PolySize            "The size of the polynomial in bits".
     @param             PolyInit            "Initalization value".
     @param             PrecomputedCRC      "The precomputed resulting CRC of Data2CRC, to compare the generated CRC with".
     */
    bool                VerifyCRC(const uint8_t *Data2CRC, const size_t Data2CRCSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit, const uint64_t PrecomputedCRC);
    
    /*!
     @abstract                              "Creates Adler32 checksum from input data".
     @return                                "Returns the Adler32 data from the data input".
     @param             Data                "Pointer to the data to generate the Adler hash from".
     @param             DataSize            "Size of data".
     */
    uint32_t            GenerateAdler32(const uint8_t *Data, const size_t DataSize);
    
    /*!
     @abstract                              "Generates Adler32 from the input data, and compares it to the submitted checksum".
     @return                                "Returns whether the input data matched the provided checksum or not".
     @param             Data                "Pointer to the data to generate the Adler hash from".
     @param             DataSize            "Size of data".
     @param             EmbeddedAdler32     "Embedded Adler32 to compare the generated one to.".
     */
    bool                VerifyAdler32(const uint8_t *Data, const size_t DataSize, const uint32_t EmbeddedAdler32);
    
    /*!
     @abstract                              "Logs errors to log files, and stderr; and mail if Critical/Panic."
     @param             ErrorLevel          "Error message prefixed by SYS in ErrorCodes".
     @param             LibraryOrProgram    "Name of the program or library that called this function, to name the logfile".
     @param             Function            "Which function is calling Log?".
     @param             ErrorDescription    "String describing what went wrong / error code".
     */
    void                Log(const uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription, ...);
    
    /*!
     @abstract                              "Reads raw UUID/GUID from the bitstream".
     @remark                                "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitI                "Pointer to instance of BitInput".
     @param             UUIDString          "Character array to read UUID string into".
     */
    void                ReadUUID(BitInput *BitI, uint8_t *UUIDString);
    
    /*!
     @abstract                              "Endian swaps a UUID into a GUID and vice versa".
     @param             UUIDString2Convert  "Pointer to a UUIDString to be converted".
     @param             ConvertedUUIDString "The resulting UUID or GUID".
     */
    void                SwapUUID(const uint8_t *UUIDString2Convert, uint8_t *ConvertedUUIDString);
    
    /*!
     @abstract                              "Write UUID/GUID string as hyphen-less blob".
     @remark                                "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitO                "Pointer to instance of BitOutput".
     @param             UUIDString          "UUID string to write to the file as a binary blob, aka remove hyphens and null terminating char".
     */
    uint8_t             WriteUUID(BitOutput *BitO, const uint8_t *UUIDString);
    
    /*!
     @abstract                              "Verify two UUIDs match each other".
     @remark                                "Matches hyphens and NULL terminating character as well, it's pretty literal".
     @param             UUIDString1         "Pointer to a UUIDString to be matched against".
     @param             UUIDString2         "Pointer to a UUIDString to match against UUIDString1".
     */
    bool                CompareUUIDs(const uint8_t *UUIDString1, const uint8_t *UUIDString2);
    
    
    /*!
     @abstract                              "Initalizes a CommandLineOptions instance".
     @return                                "Returns a pointer to an initalized CommandLineOptions instance".
     */
    CommandLineOptions *InitCommandLineOptions(void);
    
    /*!
     @abstract                              "Frees CommandLineOptions instance".
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     */
    void                CloseCommandLineOptions(CommandLineOptions *CMD);
    
    /*!
     @abstract                              "Initalizes NumSwitches CommandLineSwitch's, and attaches them to CommandLineOptions".
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             NumSwitches         "The number of switches to initalize and attach to CommandLineOptions pointer CMD"
     */
    CommandLineOptions *InitCommandLineSwitches(CommandLineOptions *CMD, uint64_t NumSwitches);
    
    /*!
     @abstract                              "Sets the name of the program".
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             Name                "Pointer to a C string contining the name of the program you're building"
     */
    void                SetCMDName(CommandLineOptions *CMD, const char *Name);
    
    /*!
     @abstract                              "Sets the description of the program".
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             Description         "Description of what the program does".
     */
    void                SetCMDDescription(CommandLineOptions *CMD, const char *Description);
    
    /*!
     @abstract                              "Sets the author of the program".
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             Author              "Author of this program".
     */
    void                SetCMDAuthor(CommandLineOptions *CMD, const char *Author);
    
    /*!
     @abstract                              "Sets the copyright years of the program".
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             Copyright           "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                SetCMDCopyright(CommandLineOptions *CMD, const char *Copyright);
    
    /*!
     @abstract                              "Sets the license of the program".
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             License             "The license this program is licensed under".
     */
    void                SetCMDLicense(CommandLineOptions *CMD, const char *License);
    
    /*!
     @abstract                              "Sets SwitchNum's flag in the CommandLineOptions instance pointed by CMD"
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             SwitchNum           "The switch to set".
     @param             Flag                "The flag to identify an option with".
     */
    void                SetSwitchFlag(CommandLineOptions *CMD, uint64_t SwitchNum, const char *Flag);
    
    /*!
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             SwitchNum           "The switch to set".
     @param             Description         "Pointer to a C string containing the description of what this program does"
     */
    void                SetSwitchDescription(CommandLineOptions *CMD, uint64_t SwitchNum, const char *Description);
    
    /*!
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             SwitchNum           "The switch to set".
     @param             IsSwitchResultless  "Are you expecting this switch to contain data, or are you just testing for it's presence?".
     */
    void                SetSwitchResultStatus(CommandLineOptions *CMD, uint64_t SwitchNum, bool IsSwitchResultless);
    
    /*!
     @abstract                              "Gets the data contained in Switch->Result"
     @return                                "Returns the data after the switch, if the switch is resultless it will return 0"
     @param             CMD                 "Pointer to instance of CommandLineOptions".
     @param             SwitchNum           "The switch to check".
     */
    const char         *GetSwitchResult(CommandLineOptions *CMD, uint64_t SwitchNum);
    
    /*!
     @abstract                              "Tells if a certain switch has been found".
     @param             CMD                 "Pointer to CommandLineOptions instance".
     @param             SwitchNum           "The switch to check".
     */
    bool                IsSwitchPresent(CommandLineOptions *CMD, uint64_t SwitchNum);
    
    /*!
     @abstract                              "Gets the size of the buffer in an instance of BitInput".
     @return                                "Returns the size of BitInput->Buffer"
     @param             BitI                "Pointer to instance of BitInput".
     */
    size_t              GetBitInputBufferSize(BitInput *BitI);
    
    /*!
     @abstract                              "Gets the size of the buffer in an instance of BitOutput".
     @return                                "Returns the size of BitOutput->Buffer"
     @param             BitO                "Pointer to instance of BitOutput".
     */
    size_t              GetBitOutputBufferSize(BitOutput *BitO);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_BITIO_H */
