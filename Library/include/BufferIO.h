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
#include <string.h>

#pragma once

#ifndef LIBBITIO_BITIO_H
#define LIBBITIO_BITIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum              BitIOConstants
     @abstract                                    "BitIO compile time constants".
     @remark                                      "Change the buffer sizes here".
     
     @constant          BitIOStringSize           "Maximum size of a string in BitIO".
     @constant          BitIOUUIDStringSize       "Size of a UUIDString including dashes and null terminator".
     @constant          BitIOGUIDStringSize       "Size of a GUIDString including dashes and null terminator".
     @constant          BitIOBinaryUUIDSize       "Size of a binary UUID string".
     @constant          BitIOBinaryGUIDSize       "Size of a binary GUID string".
     @constant          BitIOMD5Size              "Size of a MD5 string".
     @constant          BitIOCRCMaxBufferSize     "Max size for the CRC buffer at a time, in bytes".
     @constant          BitIONumHuffmanSymbols    "Number of Huffman symbols to hold in the Huffman tree".
     @constant          BitIOLZ77MaxDistanceCode  "The maximum distance back in the buffer for the pointer to be in Deflate's LZ77".
     @constant          BitIOLZ77MaxWindowSize    "The maximum size the sliding window can be in Deflate's LZ77."
     */
    enum BitIOConstants {
                        BitIOStringSize          = 4096,
                        BitIOUUIDStringSize      = 21,
                        BitIOGUIDStringSize      = BitIOUUIDStringSize,
                        BitIOBinaryUUIDSize      = 16,
                        BitIOBinaryGUIDSize      = BitIOBinaryUUIDSize,
                        BitIOMD5Size             = 16,
                        BitIOCRCMaxBufferSize    = 4096,
                        BitIONumHuffmanSymbols   = 288,   // 0-255 = literal, 256 = end of block,
                        BitIOLZ77MaxDistanceCode = 258,   // - 3
                        BitIOLZ77MaxWindowSize   = 32768, // - 1
    };
    
    /*!
     @enum              Endian
     @constant          UnknownEndian             "The endian of the machine is currently unknown".
     @constant          BigEndian                 "The machine is Big endian".
     @constant          LittleEndian              "The machine is little endian".
     */
    enum Endian {
                        UnknownEndian = 0,
                        BigEndian     = 1,
                        LittleEndian  = 2,
    };
    
    /*!
     @enum             FormatSpecifierBase
     @constant         Octal                      "Base  2 format specifier".
     @constant         Decimal                    "Base 10 format specifier".
     @constant         Hexadecimal                "Base 16 format specifier".
     */
    enum FormatSpecifierBase {
        Octal       = 1,
        Decimal     = 2,
        Hexadecimal = 3,
    };
    
    /*!
     @enum              PathType
     @constant          File                      "The submitted path is a regular file"
     @constant          URL                       "The submitted path is to a network resource or IP address"
     */
    enum PathType {
                        File = 0,
                        URL  = 1,
    };
    
    /*!
     @typedef           BitBuffer
     @abstract                                    "Contains variables and a pointer to a buffer for reading and writing bits".
     @constant          BitsAvailable             "The number of bits available for reading".
     @constant          BitsUnavailable           "The number of bits previously read, or available for writing".
     @constant          Buffer                    "A pointer to an unsigned byte buffer".
     */
    typedef struct      BitBuffer                 BitBuffer;
    
    /*!
     @typedef           BitInput
     @abstract                                    "Contains File/Socket pointers for reading to a BitBuffer.
     @constant          File                      "Input File/Socket to read into a BitBuffer".
     @constant          FileSize                  "Size of the File in bytes".
     @constant          FilePosition              "Current byte in the file".
     @constant          SystemEndian              "Endian of the running system".
     @constant          FormatSpecifier           "A string containing the format specifier for printf".
     @constant          CurrentFileSpecifierNum   "Which file are we currently on?".
     */
    typedef struct      BitInput                  BitInput;
    
    /*!
     @typedef           BitOutput
     @abstract                                    "Contains File/Socket pointers for writing from a BitBuffer".
     @constant          File                      "Input File/Socket to write a BitBuffer into".
     @constant          FilePosition              "Current byte in the file".
     @constant          SystemEndian              "Endian of the running system".
     @constant          FormatSpecifier           "A string containing the format specifier for printf".
     @constant          CurrentFileSpecifierNum   "Which file are we currently on?".
     */
    typedef struct      BitOutput                 BitOutput;
    
    /*!
     @typedef           CommandLineSwitch
     @abstract                                    "Contains the data to support a single switch".
     @remark                                      "You MUST include the NULL padding at the end of @Switch".
     @constant          SwitchFound               "If the switch was found in argv, this will be set to true".
     @constant          IsThereAResult            "Is there a trailing option after the flag? if so, set to true".
     @constant          Flag                      "Actual flag, WITHOUT dash(s) or backslash, Flags are case insensitive".
     @constant          FlagSize                  "Size of the flag in bytes".
     @constant          SwitchDescription         "Message to print explaining what the switch does".
     @constant          SwitchResult              "String to contain the result of this switch, NULL if not found or not included".
     @constant          DependsOn                 "What switch is this one dependent on?".
     */
    typedef struct      CommandLineSwitch         CommandLineSwitch;
    
    /*!
     @typedef           CommandLineOptions
     @abstract                                    "Type to contain a variable amount of CLSwitches".
     @remark                                      "The switches are zero indexed, and @NumSwitches is zero indexed, so count from 0".
     @remark                                      "The last switch MUST be the help option".
     @constant          NumSwitches               "The number of switches".
     @constant          MinSwitches               "The minimum number of switches this program requires to run".
     @constant          ProgramName               "The name you want output when the help is printed".
     @constant          ProgramDescription        "The description of the program when the help is printed".
     @constant          Author                    "The author of the program".
     @constant          Copyright                 "The starting and ending copyright years".
     @constant          License                   "The license this program is released under".
     @constant          Switch                    "A pointer to an array of CLSwitch instances containing the properties of the switches".
     */
    typedef struct      CommandLineOptions        CommandLineOptions;
    
    typedef struct      HuffmanTree               HuffmanTree;
    
    /*!
     @abstract                                    "Initializes a BitInput structure".
     @return                                      "Returns a pointer to said BitInput structure".
     */
    BitInput           *InitBitInput(void);
    
    /*!
     @abstract                                    "Initializes a BitOutput structure".
     @return                                      "Returns a pointer to said BitOutput structure".
     */
    BitOutput          *InitBitOutput(void);
    
    /*!
     @abstract                                    "Initializes a BitBuffer structure".
     @remark                                      "The buffer MUST be unread".
     @return                                      "Returns a pointer to said BitBuffer structure".
     */
    BitBuffer          *InitBitBuffer(void);
    
    /*!
     @abstract                                    "Creates an empty buffer for putting data into"
     @param             EmptyBufferSize           "Size of the buffer to create in bytes".
     */
    BitBuffer          *CreateEmptyBitBuffer(const size_t EmptyBufferSize);
    
    /*!
     @abstract                                    "Initializes a CommandLineOptions instance".
     @return                                      "Returns a pointer to an initialized CommandLineOptions instance".
     @param             NumSwitches               "The number of CommandLineSwitch structures to initalize".
     */
    CommandLineOptions *InitCommandLineOptions(const size_t NumSwitches);
    
    /*!
     @abstract                                    "Deallocates BitInput".
     @remark                                      "For use when changing files, or exiting the program".
     @param             BitI                      "Pointer to the instance of BitInput you want to delete".
     */
    void                CloseBitInput(BitInput *BitI);
    
    /*!
     @abstract                                    "Deallocates the instance of BitOutput pointed to by BitI"
     @remark                                      "For use when changing files, or exiting the program".
     @param             BitO                      "Pointer to the instance of BitOutput you want to delete".
     */
    void                CloseBitOutput(BitOutput *BitO);
    
    /*!
     @abstract                                    "Deallocates the instance of BitBuffer pointed to by BitB"
     @param             BitB                      "Pointer to the instance of BitBuffer you want to delete"
     */
    void                CloseBitBuffer(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Deallocates the instance of CommandLineOptions pointed to by CMD".
     @param             CMD                       "Pointer to the instance of CommandLineOptions you want to delete".
     */
    void                CloseCommandLineOptions(CommandLineOptions *CMD);
    
    /*!
     @abstract                                    "Swap endian of 16 bit integers".
     @param             Data2Swap                 "Data to swap endian".
     */
    uint16_t            SwapEndian16(const uint16_t Data2Swap);
    
    /*!
     @abstract                                    "Swap endian of 32 bit integers".
     @param             Data2Swap                 "Data to swap endian".
     */
    uint32_t            SwapEndian32(const uint32_t Data2Swap);
    
    /*!
     @abstract                                    "Swap endian of 64 bit integers".
     @param             Data2Swap                 "Data to swap endian".
     */
    uint64_t            SwapEndian64(const uint64_t Data2Swap);
    
    /*!
     @abstract                                    "Computes the number of bits from the number of bytes".
     @remark                                      "Does not have sub-byte precision".
     @return                                      "Returns the number of bits".
     @param             Bytes                     "The number of bytes you want to intrepret as bits".
     */
    int64_t             Bytes2Bits(const int64_t Bytes);
    
    /*!
     @abstract                                    "Computes the number of bytes from the number of bits".
     @return                                      "Returns The number of bytes".
     @param             Bits                      "The bits to convert to bytes".
     @param             RoundUp                   "Should the resulting bytes be rounded up or down?"
     */
    int64_t             Bits2Bytes(const int64_t Bits, const bool RoundUp);
    
    /*!
     @abstract                                    "Raise a base to the exponent".
     @param             Base                      "The base to be multiplied by it's self Exponent times".
     @param             Exponent                  "The number of times to multiply Base by it's self".
     */
    int64_t             Powi(const int64_t Base, const int64_t Exponent);
    
    /*!
     @abstract                                    "Integer floor function".
     @param             Number2Floor              "The input number as a floating point value".
     */
    int64_t             Floori(const double Number2Floor);
    
    /*!
     @abstract                                    "Integer ceil function".
     @todo                                        "We should extract the decimal part and drop the mantissa, it would make it an int-only op, and faster".
     @param             Number2Ceil               "The input number to Ceil as a floating point number".
     */
    int64_t             Ceili(const double Number2Ceil);
    
    /*!
     @abstract                                    "Computes the number of bits required to hold a certain amount of symbols".
     @remark                                      "Rounds up to the next integer bits to ensure all symbols can be contained in simple binary"
     @param             NumSymbols                "The number of symbols you're looking to contain in a binary number".
     */
    uint64_t            NumBits2ReadSymbols(const uint64_t NumSymbols);
    
    /*!
     @abstract                                    "Counts the number of bits that are set in a int".
     @return                                      "The number of bits set in Bits2Count".
     @param             Bits2Count                "The int who's bits should be counted".
     */
    uint8_t             CountBitsSet(const uint64_t Bits2Count);
    
    /*!
     @abstract                                    "Create bit-mask from binary exponent".
     @return                                      "A bit mask generated from a power".
     @param             Exponent                  "Power to be raised by 2".
     */
    uint64_t            Power2Mask(const uint8_t Exponent);
    
    /*!
     @abstract                                    "Converts numbers from One's compliment to Two's compliment"
     @return                                      "Returns OnesCompliment in 2's compliment format".
     @param             OnesCompliment            "Int in 1's compliment form to convert into 2's compliment".
     */
    int64_t             OnesCompliment2TwosCompliment(const int64_t OnesCompliment);
    
    /*!
     @abstract                                    "Converts numbers from Two's compliment to One's compliment".
     @return                                      "Returns the TwosCompliment in 1's compliment format".
     @param             TwosCompliment            "Int in 2's compliment form to convert into 1's compliment".
     */
    int64_t             TwosCompliment2OnesCompliment(const int64_t TwosCompliment);
    
    /*!
     @abstract                                    "Tells whether Input is even or odd".
     @return                                      "True for odd, false for even".
     @param             Number2Check              "The number to see if it's odd or even".
     */
    bool                IsOdd(const int64_t Number2Check);
    
    /*!
     @abstract                                    "Finds the highest set bit in an int".
     @todo                                        "We should have a flag for if it's positive or negative, and if neg, search instead for the highest 0, called FindHighestSetBit"
     @remark                                      "Will NOT WORK WITH SIGNED INTS. it will ALWAYS return the sign bit".
     @return                                      "Returns the position of the highest set bit".
     @param             UnsignedInt2Search        "Unsigned integer to search for the highest set bit".
     */
    uint8_t             FindHighestBitSet(const uint64_t UnsignedInt2Search);
    
    /*!
     @abstract                                    "Computes the number of bytes left in the file".
     @returm                                      "Returns the number of bytes left in the file".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    size_t              BytesRemainingInInputFile(const BitInput *BitI);
    
    /*!
     @abstract                                    "Gets the size of the file pointed to by BitI"
     @return                                      "Returns the value in BitI->FileSize".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    size_t              GetBitInputFileSize(const BitInput *BitI);
    
    /*!
     @abstract                                    "Returns the position of the current file".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    size_t              GetBitInputFilePosition(const BitInput *BitI);
    
    /*!
     @abstract                                    "Returns the number of bits used in BitB".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    size_t              GetBitBufferPosition(const BitBuffer *BitB);
    
    /*!
     @abstract                                    "Gets the size of the BitBuffer".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    size_t              GetBitBufferSize(const BitBuffer *BitB);
    
    /*!
     @abstract                                    "The BitOutput type was made private, this function was added so users can still get this information".
     @return                                      "Returns the endian of the running system".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    uint8_t             GetBitInputSystemEndian(const BitInput *BitI);
    
    /*!
     @abstract                                    "The BitOutput type was made private, this function was added so users can still get this information".
     @return                                      "Returns the endian of the running system".
     @param             BitO                      "Pointer to the instance of BitOutput".
     */
    uint8_t             GetBitOutputSystemEndian(const BitOutput *BitO);
    
    /*!
     @abstract                                    "Parses argv for switches matching the ones contained in CMD".
     @remark                                      "Argv[0] (the path for the original executable) is NEVER searched or used".
     @param             CMD                       "Pointer to CommandLineOptions".
     @param             argc                      "Main's argc, for the number of arguments entered".
     @param             argv                      "Main's argv, for the actual arguments the user has entered".
     */
    void                ParseCommandLineArguments(const CommandLineOptions *CMD, int argc, const char *argv[]);
    
    /*!
     @abstract                                    "Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
     @param             BitI                      "Pointer to the instance of BitInput".
     @param             Path2Open                 "Path to the input file to open".
     */
    void                OpenInputFile(BitInput *BitI, const char *Path2Open);
    
    /*!
     @abstract                                    "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitO->File".
     @param             BitO                      "Pointer to the instance of BitOutput".
     @param             Path2Open                 "Path to the output file to open".
     */
    void                OpenOutputFile(BitOutput *BitO, const char *Path2Open);
    
    /*!
     @abstract                                    "Sets the name of the program".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             Name                      "Pointer to a C string containing the name of the program you're building"
     */
    void                SetCMDName(CommandLineOptions *CMD, const char *Name);
    
    /*!
     @abstract                                    "Sets the name of the program".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             VersionString             "Pointer to a C string contining the version of the program you're building"
     */
    void                SetCMDVersion(CommandLineOptions *CMD, const char *VersionString);
    
    /*!
     @abstract                                    "Sets the description of the program".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             Description               "Description of what the program does".
     */
    void                SetCMDDescription(CommandLineOptions *CMD, const char *Description);
    
    /*!
     @abstract                                    "Sets the author of the program".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             Author                    "Author of this program".
     */
    void                SetCMDAuthor(CommandLineOptions *CMD, const char *Author);
    
    /*!
     @abstract                                    "Sets the copyright years of the program".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             Copyright                 "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                SetCMDCopyright(CommandLineOptions *CMD, const char *Copyright);
    
    /*!
     @abstract                                    "Sets the license of the program".
     @remark                                      "If your program is closed source, do NOT use the License options, use the EULA functions".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             License                   "The license this program is licensed under".
     @param             IsEULA                    "Is this program released under an open source license, or a EULA?"
     */
    void                SetCMDLicense(CommandLineOptions *CMD, const char *License, const bool IsEULA);
    
    /*!
     @abstract                                    "Sets the URL for the license, in the main program banner".
     @remark                                      "If your program is closed source, do NOT use the License options, use the EULA functions".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             LicenseURL                "the actual URL for the license".
     @param             IsEULA                    "Is this program released under an open source license, or a EULA?"
     */
    void                SetCMDLicenseURL(CommandLineOptions *CMD, const char *LicenseURL, const bool IsEULA);
    
    /*!
     @abstract                                    "What is the minimum number of switches your program needs to operate?".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             MinSwitches               "The minimum number of switches".
     */
    void                SetCMDMinSwitches(CommandLineOptions *CMD, const uint64_t MinSwitches);
    
    /*!
     @abstract                                    "Sets SwitchNum's flag in the CommandLineOptions instance pointed by CMD".
     @remark                                      "Just enter the number of characters you typed into the string not counting the quotes".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             SwitchNum                 "The switch to set".
     @param             Flag                      "The flag to identify an option with".
     @param             FlagSize                  "Size of the flag string".
     */
    void                SetCMDSwitchFlag(CommandLineOptions *CMD, const uint64_t SwitchNum, const char *Flag, const size_t FlagSize);
    
    /*!
     @abstract                                    "What switch is SwitchNum dependent on?".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             SwitchNum                 "The switch to set".
     @param             DependsOn                 "The flag SwitchNum needs in order to be valid".
     */
    void                SetCMDSwitchDependency(CommandLineOptions *CMD, const uint64_t SwitchNum, const uint64_t DependsOn);
    
    /*!
     @abstract                                    "Sets SwitchDescription's flag in the CommandLineOptions instance pointed by CMD".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             SwitchNum                 "The switch to set".
     @param             Description               "Pointer to a C string containing the description of what this program does"
     */
    void                SetCMDSwitchDescription(CommandLineOptions *CMD, const uint64_t SwitchNum, const char *Description);
    
    /*!
     @abstract                                    "Sets SwitchResult's flag in the CommandLineOptions instance pointed by CMD".
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             SwitchNum                 "The switch to set".
     @param             IsThereAResult            "Are you expecting this switch to contain data, or are you just testing for it's presence?".
     */
    void                SetCMDSwitchResultStatus(CommandLineOptions *CMD, const uint64_t SwitchNum, const bool IsThereAResult);
    
    /*!
     @abstract                                    "Gets the data contained in Switch->Result"
     @return                                      "Returns the data after the switch, if the switch is resultless it will return 0"
     @param             CMD                       "Pointer to the instance of CommandLineOptions".
     @param             SwitchNum                 "The switch to check".
     */
    const char         *GetCMDSwitchResult(const CommandLineOptions *CMD, const uint64_t SwitchNum);
    
    /*!
     @abstract                                    "Tells if a certain switch has been found".
     @param             CMD                       "Pointer to CommandLineOptions instance".
     @param             SwitchNum                 "The switch to check".
     */
    bool                GetCMDSwitchPresence(const CommandLineOptions *CMD, const uint64_t SwitchNum);
    
    /*!
     @abstract                                    "Manages InputBuffer and hands out the requested bits".
     @remark                                      "DO NOT try reading backwards, it will not work. for that use SkipBits()".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             Bits2Read                 "Number of bits to read".
     @param             ReadFromMSB               "Should ReadBits start at the most significant bit in this byte?"
     */
    uint64_t            ReadBits(BitBuffer *BitB, const uint8_t Bits2Read, const bool ReadFromMSB);
    
    /*!
     @abstract                                    "Shows the next X bits, without recording it as a read".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             Bits2Peek                 "Number of bits to peek".
     @param             ReadFromMSB               "Should PeekBits start at the most significant bit in this byte?"
     */
    uint64_t            PeekBits(BitBuffer *BitB, const uint8_t Bits2Peek, const bool ReadFromMSB);
    
    /*!
     @abstract                                    "Reads and Decodes unary/RICE encoded data from BitInput stream".
     @return                                      "Returns the count of bits aka the value encoded by the encoder".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             Truncated                 "Should the StopBit be included in the count?"
     @param             StopBit                   "MUST be a 0 or a 1. none of this funny business about how true > 0".
     */
    uint64_t            ReadRICE(const BitBuffer *BitB, const bool Truncated, const bool StopBit);
    
    /*!
     @abstract                                    "Reads data encoded as Exponential-Golomb aka Elias Gamma".
     @return                                      "Returns the decoded value of the Elias/Golomb code".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             IsSigned                  "Should it be read as signed or unsigned?".
     */
    int64_t             ReadExpGolomb(const BitBuffer *BitB, const bool IsSigned);
    
    /*!
     @abstract                                    "Seeks Forwards and backwards in BitInput"
     @remark                                      "To seek backwards just use a negative number, to seek forwards positive".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             Bits2Skip                 "The number of bits to skip".
     */
    void                SkipBits(BitBuffer *BitB, const int64_t Bits2Skip);
    
    /*!
     @abstract                                    "Writes bits to BitBuffer".
     @todo                                        "We should replace WriteFromMSB with a WriteLittleEndian or WriteBigEndian".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             Data2Write                "Is the actual data to write out".
     @param             NumBits                   "Is the number of bits to write".
     @param             WriteFromMSB              "Should we write the bits from the most significant bit?".
     */
    void                WriteBits(BitBuffer *BitB, const uint64_t Data2Write, const uint8_t NumBits, const bool WriteFromMSB);
    
    /*!
     @abstract                                    "Encodes and writes data in unary/RICE format to a BitOutput stream".
     @todo                                        "We should replace WriteFromMSB with a WriteLittleEndian or WriteBigEndian".
     @param             BitB                      "Pointer to BitBuffer, the output buffer".
     @param             Truncated                 "Should the stop bit be pruned?"
     @param             StopBit                   "Has to be a 0 or a 1".
     @param             Data2Write                "Number to encode into RICE format".
     @param             WriteFromMSB              "Should Data2Write be written from the MSB or the LSB?".
     */
    void                WriteRICE(BitBuffer *BitB, const bool Truncated, const bool StopBit, const uint64_t Data2Write, const bool WriteFromMSB);
    
    /*!
     @abstract                                    "Writes data encoded as Exponential-Golomb aka Elias Gamma codes to BitO".
     @todo                                        "We should replace WriteFromMSB with a WriteLittleEndian or WriteBigEndian".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             IsSigned                  "Is Data2Write signed?".
     @param             Data2Write                "The actual data to write to the output file".
     @param             WriteFromMSB              "Should Data2Write be written from the MSB or the LSB?".
     */
    void                WriteExpGolomb(BitBuffer *BitB, const bool IsSigned, const uint64_t Data2Write, const bool WriteFromMSB);
    
    /*!
     @abstract                                    "Tells if the stream/buffer is byte aligned or not".
     @param             BytesOfAlignment          "Are you trying to see if it's aligned to a byte, short, word, etc alignment? Specify in number of bytes".
     */
    bool                IsBitBufferAligned(const BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                                    "Aligns bits for multi-byte alignment".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             BytesOfAlignment          "Align BitB to X byte boundary".
     */
    void                AlignBitBuffer(BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                                    "Generates CRC from data".
     @param             Data2CRC                  "Pointer to a data buffer containing data to run the CRC algorithm over".
     @param             DataSize                  "Size of the data chunk to generate the CRC for, in bytes".
     @param             ReciprocalPoly            "The Polynomial in Normal representation".
     @param             PolySize                  "The size of the polynomial in bits".
     @param             PolyInit                  "Initialization value".
     */
    uint64_t            GenerateCRC(const uint8_t *Data2CRC, const size_t DataSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit);
    
    /*!
     @abstract                                    "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     @param             Data2CRC                  "Pointer to a data buffer containing data to run the CRC algorithm over".
     @param             Data2CRCSize              "Size of the data chunk to generate the CRC for".
     @param             ReciprocalPoly            "The Polynomial in Reciprocal representation".
     @param             PolySize                  "The size of the polynomial in bits".
     @param             PolyInit                  "Initialization value".
     @param             PrecomputedCRC            "The precomputed resulting CRC of Data2CRC, to compare the generated CRC with".
     */
    bool                VerifyCRC(const uint8_t *Data2CRC, const size_t Data2CRCSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit, const uint64_t PrecomputedCRC);
    
    /*!
     @abstract                                    "Creates Adler32 checksum from input data".
     @return                                      "Returns the Adler32 data from the data input".
     @param             Data                      "Pointer to the data to generate the Adler hash from".
     @param             DataSize                  "Size of data".
     */
    uint32_t            GenerateAdler32(const uint8_t *Data, const size_t DataSize);
    
    /*!
     @abstract                                    "Generates Adler32 from the input data, and compares it to the submitted checksum".
     @return                                      "Returns whether the input data matched the provided checksum or not".
     @param             Data                      "Pointer to the data to generate the Adler hash from".
     @param             DataSize                  "Size of data".
     @param             EmbeddedAdler32           "Embedded Adler32 to compare the generated one to".
     */
    bool                VerifyAdler32(const uint8_t *Data, const size_t DataSize, const uint32_t EmbeddedAdler32);
    
    /*!
     @abstract                                    "Reads raw UUID/GUID from the bitstream".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint8_t            *ReadUUID(const BitBuffer *BitB);
    
    /*!
     @abstract                                    "Converts a UUIDString to a GUIDString by swapping the endian of each section".
     @param             UUIDString                "An array containing the UUID in string form".
     */
    uint8_t            *ConvertUUID2GUID(const uint8_t *UUIDString);
    
    /*!
     @abstract                                    "Converts a GUIDString to a UUIDString by swapping the endian of each section".
     @param             GUIDString                "An array containing the converted UUID in string form".
     */
    uint8_t            *ConvertGUID2UUID(const uint8_t *GUIDString);
    
    /*!
     @abstract                                    "Verify two UUIDs match each other".
     @remark                                      "Matches hyphens and NULL terminating character as well, it's pretty literal".
     @param             UUIDString1               "Pointer to a UUIDString to be matched against".
     @param             UUIDString2               "Pointer to a UUIDString to match against UUIDString1".
     */
    bool                CompareUUIDs(const uint8_t *UUIDString1, const uint8_t *UUIDString2);
    
    /*!
     @abstract                                    "Write UUID/GUID string as hyphen-less blob".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             UUIDString                "UUID string to write to the file as a binary blob, aka remove hyphens and null terminating char".
     */
    void                WriteUUID(const BitBuffer *BitB, const uint8_t *UUIDString);
    
    /*!
     @abstract                                    "Reads Bytes2Read into a buffer pointed to by BitB from InputFile"
     @remark                                      "If the pointer to BitBuffer is not new, all the old contents will be lost".
     @param             BitI                      "Pointer to BitInput".
     @param             BitB                      "Pointer to BitBuffer to put the bytes into".
     @param             Bytes2Read                "The number of bytes to read from the InputFile into the Buffer"
     */
    void                ReadBitInput2BitBuffer(const BitInput *BitI, BitBuffer *BitB, const size_t Bytes2Read);
    
    /*!
     @abstract                                    "Writes a BitBuffer to a file, kinda shitty tho".
     @param             BitO                      "Pointer to BitOutput to write the buffer to".
     @param             BitB                      "The buffer to be written to the output file".
     @param             Bytes2Write               "The number of bytes from the buffer to write to the file"
     */
    void                WriteBitBuffer2BitOutput(BitOutput *BitO, BitBuffer *BitB, const size_t Bytes2Write);
    
#ifndef _POSIX_VERSION
    /*!
     @enum              LogTypes
     @constant          LOG_EMERG                 "The system is unusable, the program is quitting (equivalent to panic)".
     @constant          LOG_ALERT                 "Immediate action is required".
     @constant          LOG_CRIT                  "Critical condition encountered".
     @constant          LOG_ERR                   "Error condition encountered".
     @constant          LOG_WARNING               "Warning condition encountered".
     @constant          LOG_NOTICE                "Normal, but important condition encountered".
     @constant          LOG_INFO                  "Informational message logged".
     @constant          LOG_DEBUG                 "Testing information logged".
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
     @abstract                                    "Logs errors to log files, and stderr; and mail if Critical/Panic."
     @param             ErrorLevel                "Error message prefixed by SYS in ErrorCodes".
     @param             LibraryOrProgram          "Name of the program or library that called this function".
     @param             Function                  "Which function is calling Log?".
     @param             ErrorDescription          "String describing what went wrong / error code".
     */
    void                Log(const uint8_t ErrorLevel, const char *LibraryOrProgram, const char *Function, const char *ErrorDescription, ...);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_BITIO_H */
