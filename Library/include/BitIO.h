/*!
 @header    BitIO.h
 @author    Marcus Johnson
 @copyright 2016 - 2017
 @version   0.6.0
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#pragma once

#ifndef LIBBITIO_BitIO_H
#define LIBBITIO_BitIO_H

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
     */
    enum BitIOConstants {
                        BitIOStringSize           = 4096,
                        BitIOUUIDStringSize       = 21,
                        BitIOGUIDStringSize       = BitIOUUIDStringSize,
                        BitIOBinaryUUIDSize       = 16,
                        BitIOBinaryGUIDSize       = BitIOBinaryUUIDSize,
    };
    
    /*!
     @enum             BitIOByteBitOrders
     @constant         UnknownEndian              "THe endian of this system is unknown".
     @constant         BigEndianLSBit             "Read the data MSByte LSBit first".
     @constant         BigEndianMSBit             "Read the data MSByte MSBit first".
     @constant         LilEndianLSBit             "Read the data LSByte LSBit first".
     @constant         LilEndianMSBit             "Read the data LSByte MSBit first".
     @constant         LSBit                      "The bit order is least significant bit first".
     @constant         MSBit                      "The bit order is most  significant bit first".
     */
    enum BitIOByteBitOrders {
                       UnknownEndian              = 0,
                       BigEndianLSBit             = 1,
                       BigEndianMSBit             = 2,
                       LilEndianLSBit             = 3,
                       LilEndianMSBit             = 4,
                       LSBit                      = 5,
                       MSBit                      = 6,
    };
    
    /*!
     @enum              BitIOLogTypes
     @constant          LOG_EMERG                 "The system is unusable, the program is quitting (equivalent to panic)".
     @constant          LOG_ALERT                 "Immediate action is required".
     @constant          LOG_CRIT                  "Critical condition encountered".
     @constant          LOG_ERR                   "Error condition encountered".
     @constant          LOG_WARNING               "Warning condition encountered".
     @constant          LOG_NOTICE                "Normal, but important condition encountered".
     @constant          LOG_INFO                  "Informational message logged".
     @constant          LOG_DEBUG                 "Testing information logged".
     */
    enum BitIOLogTypes {
                        LOG_EMERG                 = 0,
                        LOG_ALERT                 = 1,
                        LOG_CRIT                  = 2,
                        LOG_ERR                   = 3,
                        LOG_WARNING               = 4,
                        LOG_NOTICE                = 5,
                        LOG_INFO                  = 6,
                        LOG_DEBUG                 = 7,
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
     @param             BitBufferSize             "Number of bytes to create Bitbuffer with".
     */
    BitBuffer          *InitBitBuffer(const uint64_t BitBufferSize);
    
    /*!
     @abstract                                    "Deallocates BitInput".
     @remark                                      "For use when changing files, or exiting the program".
     @param             BitI                      "Pointer to the instance of BitInput you want to delete".
     */
    void                DeinitBitInput(BitInput *BitI);
    
    /*!
     @abstract                                    "Deallocates the instance of BitOutput pointed to by BitI"
     @remark                                      "For use when changing files, or exiting the program".
     @param             BitO                      "Pointer to the instance of BitOutput you want to delete".
     */
    void                DeinitBitOutput(BitOutput *BitO);
    
    /*!
     @abstract                                    "Deallocates the instance of BitBuffer pointed to by BitB"
     @param             BitB                      "Pointer to the instance of BitBuffer you want to delete"
     */
    void                DeinitBitBuffer(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Swap bits in a byte".
     @param             Byte                      "Byte to swap bits".
     */
    uint8_t             SwapBitsInByte(const uint8_t Byte);
    
    /*!
     @abstract                                    "Swap nibbles in a byte".
     @param             Byte2Swap                 "Byte to swap nibbles".
     */
    uint8_t             SwapNibble(const uint8_t Byte2Swap);
    
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
     @param             BitOrder                  "Should the mask be shifted to the left or not?".
     @param             Exponent                  "Power to be raised by 2".
     */
    uint8_t             Power2Mask(const uint8_t BitOrder, const uint8_t Exponent);
    
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
     @return                                      "Returns the number of bytes left in the file".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    uint64_t            BytesRemainingInInputFile(const BitInput *BitI);
    
    /*!
     @abstract                                    "Gets the size of the file pointed to by BitI"
     @return                                      "Returns the value in BitI->FileSize".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    uint64_t            GetBitInputFileSize(const BitInput *BitI);
    
    /*!
     @abstract                                    "Returns the position of the current file".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    uint64_t            GetBitInputFilePosition(const BitInput *BitI);
    
    /*!
     @abstract                                    "Returns the number of bits used in BitB".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint64_t            GetBitBufferPosition(const BitBuffer *BitB);
    
    /*!
     @abstract                                    "Gets the size of the BitBuffer".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint64_t            GetBitBufferSize(const BitBuffer *BitB);
    
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
     @abstract                                    "Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
     @param             BitI                      "Pointer to the instance of BitInput".
     @param             Path2Open                 "Path to the input file to open".
     @param             OpenForReadWrite          "Should the File be opened for both reading and writing?".
     */
    void                OpenInputFile(BitInput *BitI, const char *Path2Open, bool OpenForReadWrite);
    
    /*!
     @abstract                                    "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitO->File".
     @param             BitO                      "Pointer to the instance of BitOutput".
     @param             Path2Open                 "Path to the output file to open".
     */
    void                OpenOutputFile(BitOutput *BitO, const char *Path2Open);
    
    /*!
     @abstract                                    "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitIOGlobalLogFile".
     @param             LogFilePath               "Path to the log file to open/create".
     */
    void                OpenLogFile(const char *LogFilePath);
    
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
    uint64_t            GenerateCRC(const uint8_t *Data2CRC, const uint64_t DataSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit);
    
    /*!
     @abstract                                    "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     @param             Data2CRC                  "Pointer to a data buffer containing data to run the CRC algorithm over".
     @param             Data2CRCSize              "Size of the data chunk to generate the CRC for".
     @param             ReciprocalPoly            "The Polynomial in Reciprocal representation".
     @param             PolySize                  "The size of the polynomial in bits".
     @param             PolyInit                  "Initialization value".
     @param             PrecomputedCRC            "The precomputed resulting CRC of Data2CRC, to compare the generated CRC with".
     */
    bool                VerifyCRC(const uint8_t *Data2CRC, const uint64_t Data2CRCSize, const uint64_t ReciprocalPoly, const uint8_t PolySize, const uint64_t PolyInit, const uint64_t PrecomputedCRC);
    
    /*!
     @abstract                                    "Reads raw UUID/GUID from the bitstream".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint8_t            *ReadUUID(BitBuffer *BitB);
    
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
    void                ReadBitInput2BitBuffer(const BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read);
    
    /*!
     @abstract                                    "Writes a BitBuffer to a file, kinda shitty tho".
     @param             BitO                      "Pointer to BitOutput to write the buffer to".
     @param             Buffer2Write              "The buffer to be written to the output file".
     @param             Bytes2Write               "The number of bytes from the buffer to write to the file"
     */
    void                WriteBitBuffer2BitOutput(const BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write);
    
    /*!
     @abstract                                    "Decodes Run-Length Encoded data".
     @param             Data2Decode               "Pointer to BitBuffer containing RLE encoded data".
     @param             Decoded                   "Pointer to BitBuffer containing decoded data".
     @param             LengthCodeSize            "The number of bits used to represent the length code".
     @param             SymbolSize                "The number of bits used to represent the symbol".
     */
    void                DecodeRLE(const BitBuffer *Data2Decode, BitBuffer *Decoded, const uint8_t LengthCodeSize, const uint8_t SymbolSize);
    
    /*!
     @abstract                                    "Logs errors to the user provided log file, or stderr".
     @param             ErrorSeverity             "Error message prefixed by SYS in ErrorCodes".
     @param             LibraryOrProgram          "Name of the program or library that called this function".
     @param             FunctionName              "Which function is calling Log?".
     @param             Description               "String describing what went wrong / error code".
     */
    void                Log(const uint8_t ErrorSeverity, const char *LibraryOrProgram, const char *FunctionName, const char *Description, ...);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_BitIO_H */
