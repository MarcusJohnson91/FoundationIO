//#include <complex.h>
#include <errno.h>
#include <glob.h>
//#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/mman.h> // mmap
#include <unistd.h> // STDIN, Getopt, 
#include <iso646.h> // Defines operators as words.
#include <tgmath.h> // Type Generic Math, includes Math.h and Complex.h

#pragma once

/*!
 @header    BitIO.h
 @author    Marcus Johnson
 @copyright 2016, Marcus Johnson
 @version   1.0
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

#if defined(_WIN32) || defined(_WIN64)
#define strcasecmp _stricmp
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    #define BitIOBufferSize 4096
    #define BitIOBufferSizeInBits (BitIOBufferSize * 8)
	
	/*! @typedef    BitInput
	 *  @abstract   Contains variables and buffers for reading bits.
	 *  @discussion Tracks bits in relation to buffer and file to facilitate reading bits.
	 *  @remark     The default internal representation in BitIO is unsigned.
	 *
	 *  @constant   File             Input file to read bits from.
	 *  @constant   FileSize         Size of File in bytes.
     *  @constant   FilePosition     Current byte in the file.
     *  @constant   BitsUnavailable  Number of previously read bits in Buffer.
	 *  @constant   BitsLeftInBuffer Number of readable bits remaining in Buffer.
     *  @constant   CurrentArgument  Where to start in Argv processing.
     *  @constant   Buffer           Buffer of BitIOBufferSizeInBits from File.
	 */
    typedef struct BitInput {
        FILE                       *File;
        uint64_t                FileSize;
        uint64_t            FilePosition;
        uint64_t         BitsUnavailable; // BitIndex
        uint64_t           BitsAvailable; // BitsLeftInBuffer
        uint64_t         CurrentArgument; // Which argument are we currently on?
        uint8_t  Buffer[BitIOBufferSize];
    } BitInput;
	
	/*! @typedef    BitOutput
	 *  @abstract   Contains variables and buffers for writing bits.
	 *  @discussion Tracks bits in relation to buffer and file to facilitate writing bits.
	 *  @remark     The default internal representation in BitOutput is unsigned.
	 *
	 *  @constant   File             Input file to read bits from.
     *  @constant   BitIndex         Number of previously read bits in Buffer.
	 *  @constant   BitsLeftInBuffer Number of readable bits remaining in Buffer.
     *  @constant   CurrentArgument  Where to start in Argv processing.
     *  @constant   Buffer           Buffer of BitIOBufferSize bits from File.
	 */
    typedef struct BitOutput {
        FILE                       *File;
        uint64_t         BitsUnavailable; // BitIndex
        uint64_t           BitsAvailable; // BitsLeftInBuffer
        uint64_t         CurrentArgument;
        uint8_t  Buffer[BitIOBufferSize];
    } BitOutput;
	
    /*! @typedef  ErrorStatus
     *  @abstract Allows check of the error status of various functions
	 *
	 *  @constant @ReadBits           Error code returned from ReadBits()
	 *  @constant @SeekBits           Error code returned from SeekBits()
	 *  @constant @PeekBits           Error code returned from PeekBits()
	 *  @constant @WriteBits          Error code returned from WriteBits()
	 *  @constant @AlignBits2Byte     Error code returned from AlignBits2Bytes()
     *  @constant @UpdateInputBuffer  Error code returned from UpdateInputBuffer()
     */
    typedef struct ErrorStatus {
        int64_t           ReadBits;
        int64_t           SeekBits;
		int64_t           PeekBits;
		int64_t          WriteBits;
        int64_t     AlignBits2Byte;
        int64_t  UpdateInputBuffer;
    } ErrorStatus;
    
    /*! @typedef  UUID
     *  @abstract Contains     UUID in text and raw format
     *
     *  @constant RawUUIDData  Array of raw UUID bytes without separators
     *  @constant UUIDString   Text string of UUID with separators.
     */
	
    typedef struct UUID {
        uint8_t RawUUIDData[16];
        uint8_t  UUIDString[21];
    } UUID;
    
    /*! @abstract   Computes the number of bytes from the number of bits.
     *  @remark     Rounds up to the next byte if not multiple of 8 bits, 1 indexed instead of 0.
     */
    uint64_t Bits2Bytes(uint64_t Bits);
    
    /*! @abstract   Computes the number of bits from the number of bytes.
     *  @remark     Does not have sub-byte precision.
     */
    uint64_t Bytes2Bits(uint64_t Bytes);
    
    /*! @abstract   Computes the number of bits left over in a byte.
     *  @return     Returns a number NOT a bitmask
     */
    uint8_t BitsRemaining(uint8_t Bits);
    
    /*! @abstract   Converts an Signed int to a Unsigned int.
     *  @remark     The internal representation in BitIO is unsigned.
     */
    uint64_t Signed2Unsigned(int64_t Signed);
    
    /*! @abstract   Converts an Unsigned int to a Signed int.
     *  @remark     The internal representation in BitIO is unsigned.
     */
    int64_t Unsigned2Signed(uint64_t Unsigned);
	
	/*! @abstract Pads bits to bytes
	 *
	 *  @param BytesOfPadding The number of bytes of padding you want
	 */
    void PadInputBits(BitInput *BitI, uint8_t BytesOfPadding);
	
	/*! @abstract Pads bits to bytes
	 *
	 *  @param BytesOfPadding The number of bytes of padding you want
	 */
    void PadOutputBits(BitOutput *BitO, uint8_t BytesOfPadding);
    
    /*! @abstract   Finds the highest bit set; starts at MSB
     *  @param      InputData is the input int to find the highest bit set
     */
    uint64_t FindHighestBitSet(uint64_t InputData);
	
	/*! @abstract  Tells weather the input number is even or odd
     *  @returns   true for odd, false for even.
	 */
	bool IsOdd(int64_t Input);
    
    /*! @abstract   Counts the number of bits set to 1
     *  @param      Input integer to count the number of set bits
     */
    uint64_t CountBits(uint64_t Input);
    
    /*! @abstract   Swap endian of 16 bit integers
     */
    uint16_t SwapEndian16(uint16_t X);
    
    /*! @abstract   Swap endian of 32 bit integers
     */
    uint32_t SwapEndian32(uint32_t X);
    
    /*! @abstract   Swap endian of 64 bit integers
     */
    uint64_t SwapEndian64(uint64_t X);
	
	/*! @abstract   Parses command line flags
	 */
    void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]); // BitInput *BitI
	
	/*! @abstract   Parses command line flags
	 */
    void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]);
	
	/*! @abstract   Initalizes BitInput struct
	 */
    void InitBitInput(BitInput *BitI, int argc, const char *argv[]);
	
	/*! @abstract   Initalizes BitOutput struct
	 */
    void InitBitOutput(BitOutput *BitO, int argc, const char *argv[]);
    
    /*! @abstract   Empties Input BitIO
     *  @remark     For use when changing files, skipping backwards, or exiting the program.
     */
    void CloseBitInput(BitInput *BitI);
    
    /*! @abstract   Empties Output BitIO, writes and empties the OutputBuffer (and pads to the next byte if nessacary)
     *  @remark     For use when changing files, skipping backwards, or exiting the program.
     */
    void CloseBitOutput(BitOutput *BitO);
    
    /*! @abstract  Basically fseek+fread, for moving around in the input file for any reason.
     *
     *  @param     AbsoluteOffset number of bytes from the beginning of the current file
     *  @remark    if AbsoluteOffset is 0, it's assumed to be from the current position.
     */
    void UpdateInputBuffer(BitInput *BitI, uint64_t AbsoluteOffset);
    
    /*! @abstract   Manages InputBuffer and hands out the requested bits.
     *  @remark     DO NOT try reading backwards, it will not work. for that use SeekBits()
     */
    uint64_t ReadBits(BitInput *BitI, int8_t Bits2Read);
	
    /*! @abstract Reads Exponential Golomb, and Truncated Exponential Golomb codes.
	 *
     *  @return   Returns the decoded value of the Elias/Golomb code.
	 */
    uint64_t ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated);
    
    /*! @abstract Shows the next X bits, without recording it as a read.
     *  @param    Bits2Peek Number of bits to peek.
     */
    uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek);
	
	/*! @abstract Writes bits to BitOutput
	 *
	 *  @param Bits2Write is the actual data to write out
	 *  @param Bits       is the number of bits to write
	 */
	void WriteBits(BitOutput *BitO, uint64_t Bits2Write, uint64_t Bits);
	
	/*! @abstract Writes entire buffer to the output buffer, first come first serve.
	 *
	 *  @param Buffer2Write Pointer to the buffer to be written to the output buffer.
	 *  @param BufferSize   Size of Buffer2Write in bytes
	 */
	void WriteBuffer(BitOutput *BitO, uint64_t *Buffer2Write, size_t BufferSize);
	
    /*! @abstract Seeks Forwards and backwards in BitInput
	 *  @remark   To seek backwards just use a negative number, to seek forwards positive.
	 *
	 *  @param    Bits The number of bits to seek.
     */
    void SeekBits(BitInput *BitI, int64_t Bits);
	
	/*! @abstract   Search input file for binary string
	 *
	 *  @param HexPattern      Hex string to search for.
	 *  @param SearchWholeFile Should the whole be searched for the string, or just the current buffer?
	 */
    uint64_t BinarySearch(BitInput *BitI, int64_t HexPattern, bool SearchWholeFile);
    
    /*! @abstract Returns UUID struct
     */
	// UUID ReadUUID(BitInput *BitI);
    
    /*! @abstract   Generates CRC from data
     *  @remark               Uses Reciprocal representation.
     *
     *  @param  DataBuffer    Pointer to the buffer containing the data to be verified.
     *  @param  BufferSize    Size of the buffer in bytes.
     *  @param  IsBigEndian   Is the data big endian? if not it needs to be swapped.
     *  @param  Polynomial    Recriprocal of the CRC polynomial.
     *  @param  CRCSize       Length of the CRC in bits.
     *  @param  InitialValue  The bit pattern to initalize the generator with
     */
    uint64_t GenerateCRC(uint64_t *DataBuffer, uint64_t BufferSize, bool IsBigEndian, uint64_t Polynomial, uint64_t CRCSize, uint64_t InitialValue);
    
    /*! @abstract             Computes the CRC of a given buffer, using the submitted polynomial and compares it to the submitted CRC.
     *  @remark               Uses Reciprocal representation.
     *
     *  @param  DataBuffer    Pointer to the buffer containing the data to be verified.
     *  @param  BufferSize    Size of the buffer in bytes.
     *  @param  IsBigEndian   Is the data big endian? if not it needs to be swapped.
     *  @param  Polynomial    Recriprocal of the CRC polynomial.
     *  @param  CRCSize       Length of the CRC in bits.
     *  @param  InitialValue  The bit pattern to initalize the generator with
     *  @param  EmbeddedCRC   Value to compare the data to, to be sure it was recieved and decoded correctly.
     */
    bool VerifyCRC(uint64_t *DataBuffer, uint64_t BufferSize, bool IsBigEndian, uint64_t Polynomial, uint64_t CRCSize, uint64_t InitialValue, uint64_t EmbeddedCRC);
    
#ifdef __cplusplus
}
#endif
