/*!
 @header    BitIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2016, Marcus Johnson
 @version   1.0
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

//#include <complex.h>
#include <errno.h>
#include <glob.h>
//#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include <strings.h>
#include <string.h>
#include <sys/mman.h>   /* mmap */
#include <unistd.h>     /* STDIN, Getopt */
#include <iso646.h>     /* Defines operators as words. */
#include <tgmath.h>     /* Type Generic Math, includes Math.h and Complex.h */
#include <limits.h>
#include <signal.h>
#include <complex.h>
#include <fcntl.h>

#if defined(_WIN32) || defined(_WIN64) || defined(CYGWIN_NT) || defined(MINGW32_NT) || defined(MSYS_NT)
#define strcasecmp _stricmp
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    /*! @abstract                   "BitIO compile time constants".
     *  @remark                     "Change the buffer sizes and whatnot in here.".
     */
    enum BitIOConstants {
        BitInputBufferSize        = 4096,
        BitInputBufferSizeInBits  = BitInputBufferSize * 8,
        BitOutputBufferSize       = 4096,
        BitOutputBufferSizeInBits = BitOutputBufferSize * 8,
    };
    
    extern uint64_t BitIOCurrentArgument; // This HAS to start at one; Used by the Option and Inout/Output parses.
    
    //uint64_t BitIOCurrentArgument = 1; // TODO: Make this not suck.
    // WARNING: This *HAS* to be here to handle multiple inputs or outputs.
    
    /*! @abstract                   "List of error codes the various functions in BitIO set in ErrorStatus".
     *  @remark                     "FIXME: Should the error codes be negative or positive?".
     */
    enum ErrorCodes {
        Success             =  0,
        NotEnoughMemory     =  1,
        NumberNotInRange    =  2,
        FreadUnknownFailure =  3,
    };
    
    /*! @typedef  ErrorStatus
     *  @abstract                   "Allows checking of the error status of various functions".
     */
    typedef struct ErrorStatus {
        int64_t           ReadBits;
        int64_t           ReadRICE;
        int64_t          WriteRICE;
        int64_t           SeekBits;
        int64_t           SkipBits;
        int64_t           PeekBits;
        int64_t          WriteBits;
        int64_t         Power2Mask;
        int64_t     AlignBits2Byte;
        int64_t  UpdateInputBuffer;
        int64_t       InitBitInput;
        int64_t      InitBitOutput;
        int64_t  ParseInputOptions;
        int64_t ParseOutputOptions;
        int64_t  FindHighestBitSet;
    } ErrorStatus;
	
	/*! @typedef    BitInput
	 *  @abstract                    "Contains variables and buffers for reading bits".
	 *  @remark                      "The default internal representation in BitIO is unsigned".
	 *
	 *  @constant   File             "Input file to read bits from".
	 *  @constant   FileSize         "Size of File in bytes".
     *  @constant   FilePosition     "Current byte in the file".
     *  @constant   BitsUnavailable  "Number of previously read bits in Buffer".
     *  @constant   BitsAvailable    "Number of bits available for reading".
     *  @constant   CurrentArgument  "Where to start in Argv processing".
     *  @constant   Buffer           "Buffer of data from File".
	 */
    typedef struct BitInput {
        FILE                       *File;
        uint64_t                FileSize;
        uint64_t            FilePosition;
        uint64_t         BitsUnavailable;
        uint64_t           BitsAvailable;
        ErrorStatus         *ErrorStatus;
        uint8_t  Buffer[BitInputBufferSize];
    } BitInput;
	
	/*! @typedef    BitOutput
	 *  @abstract                    "Contains variables and buffers for writing bits".
	 *  @remark                      "The default internal representation in BitOutput is unsigned".
	 *
	 *  @constant   File             "Input file to read bits from".
     *  @constant   BitsUnavailable  "Number of previously read bits in Buffer".
	 *  @constant   BitsAvailable    "Number of bits available for writing".
     *  @constant   Buffer           "Buffer of BitIOBufferSize bits from File".
	 */
    typedef struct BitOutput {
        FILE                       *File;
        uint64_t         BitsUnavailable;
        uint64_t           BitsAvailable;
        ErrorStatus         *ErrorStatus;
        uint8_t  Buffer[BitOutputBufferSize];
    } BitOutput;
    
    /*! @typedef    HuffmanTree
     *  @abstract                    "Contains a complete Huffman tree".
     *  @constant   TreeSize         "Size of the Huffman tree, including the root node".
     *  @constant   Tree             "Array containing the Huffman tree".
     *  @constant   Probability      "Array containing the probability of each symbol's occurance".
     */
    typedef struct HuffmanTree {
        uint64_t TreeSize;
        uint8_t  Tree[256];
        uint8_t  Probability[256];
    } HuffmanTree;
    
    typedef struct StaticHuffman {
        uint64_t WAT;
    } StaticHuffman;
    
    typedef struct DynamicHuffman {
        uint16_t Length;
        uint16_t Distance;
        uint16_t CodeLength;
    } DynamicHuffman;
    
    typedef struct Huffman {
        StaticHuffman  *Static;
        DynamicHuffman *Dynamic;
    } Huffman;
    
    /*! @abstract                "Swap endian of 16 bit integers".
     *  @param    Data2Swap      "Data to swap endian".
     */
    uint16_t SwapEndian16(uint16_t Data2Swap);
    
    /*! @abstract                "Swap endian of 32 bit integers".
     *  @param    Data2Swap      "Data to swap endian".
     */
    uint32_t SwapEndian32(uint32_t Data2Swap);
    
    /*! @abstract                "Swap endian of 64 bit integers".
     *  @param    Data2Swap      "Data to swap endian".
     */
    uint64_t SwapEndian64(uint64_t Data2Swap);
    
    /*! @abstract                "Tells whether the input number is even or odd".
     *  @returns                 "True for odd, false for even".
     */
    bool IsOdd(int64_t Input);
    
    /*! @abstract                "Computes the number of bytes from the number of bits".
     *  @remark                  "Rounds up to the next byte if not multiple of 8 bits, 1 indexed instead of 0".
     *  @return                  "Converts the number of bits to the number of bytes".
     */
    uint64_t Bits2Bytes(uint64_t Bits);
    
    /*! @abstract                "Computes the number of bits from the number of bytes".
     *  @remark                  "Does not have sub-byte precision".
     *  @return                  "Converts the number of bytes to the number of bits".
     */
    uint64_t Bytes2Bits(uint64_t Bytes);
    
    /*! @abstract                "Computes the number of bits left over in a byte".
     *  @return                  "Returns the number of bits left".
     */
    uint8_t BitsRemaining(uint64_t BitsAvailable);
    
    /*! @abstract                "Converts an Signed int to a Unsigned int".
     *  @remark                  "The internal representation in BitIO is unsigned".
     *  @return                  "Returns the input data in Unsigned format".
     */
    uint64_t Signed2Unsigned(int64_t Signed);
    
    /*! @abstract                "Converts an Unsigned int to a Signed int".
     *  @remark                  "The internal representation in BitIO is unsigned".
     *  @return                  "Returns the input data in Signed format".
     */
    int64_t Unsigned2Signed(uint64_t Unsigned);
    
    /*! @abstract                "Converts numbers from One's compliment to Two's compliment".
     *  @return                  "Returns the Input in Two's compliment format".
     */
    uint64_t OnesCompliment2TwosCompliment(uint64_t Input);
    
    /*! @abstract                "Converts numbers from Two's compliment to One's compliment".
     *  @return                  "Returns the Input in One's compliment format".
     */
    uint64_t TwosCompliment2OnesCompliment(uint64_t Input);
    
    /*! @abstract                "Finds the highest bit set".
     *  @remark                  "Starts at MSB (BitIO is Big Endian internally; It's not my fault Intel is stupid)".
     *  @param    InputData      "is the input int to find the highest bit set".
     */
    uint8_t FindHighestBitSet(BitInput *BitI, uint64_t InputData);
    
    /*! @abstract                "Counts the number of bits set to 1"
     *  @param    Input          "integer to count the number of set bits".
     */
    uint8_t CountBits(BitInput *BitI, uint64_t Input);
    
    /*! @abstract                "Aligns bits for single-byte alignment".
     *  @remark                  "TODO: Should this be Input/Output neutral?".
     */
    void AlignInputBits2Byte(BitInput *BitI);
    
    /*! @abstract                "Aligns bits for multi-byte alignment".
     *  @remark                  "TODO: Should this be Input/Output neutral?".
     */
    void AlignInputBits2Bytes(BitInput *BitI, uint8_t Bytes2Align);
    
    /*! @abstract                "Aligns bits for single-byte alignment".
     *  @remark                  "TODO: Should this be Input/Output neutral?".
     */
    void AlignOutputBits2Byte(BitOutput *BitO);
    
    /*! @abstract                "Aligns bits for multi-byte alignment".
     *  @remark                  "TODO: Should this be Input/Output neutral?".
     */
    void AlignOutputBits2Bytes(BitOutput *BitO, uint8_t Bytes2Align);
    
    /*! @abstract                "Create bitmask from binary exponent".
     *
     *  @param    Exponent       "Power to be raised by 2".
     */
    uint64_t Power2Mask(BitInput *BitI, uint64_t Exponent);
	
	/*! @abstract                "Parses command line flags for BitInput".
	 */
    void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]);
	
	/*! @abstract                "Parses command line flags for BitOutput".
	 */
    void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]);
	
	/*! @abstract                "Initalizes BitInput".
	 */
    void InitBitInput(BitInput *BitI, int argc, const char *argv[]);
	
	/*! @abstract                "Initalizes BitOutput".
	 */
    void InitBitOutput(BitOutput *BitO, int argc, const char *argv[]);
    
    /*! @abstract                "Deallocates BitInput".
     *  @remark                  "For use when changing files, or exiting the program".
     */
    void CloseBitInput(BitInput *BitI);
    
    /*! @abstract                "Deallocates BitOutput"
     *  @remark                  "For use when changing files, or exiting the program".
     */
    void CloseBitOutput(BitOutput *BitO);
    
    /*! @abstract                "Updates BitInput->Buffer, for further reading".
     *
     *  @param    AbsoluteOffset "Number of bytes from the beginning of the current file".
     *  @remark                  "If AbsoluteOffset is 0, it's assumed to be from the current position".
     *  FIXME:                   "Change AbsoluteOffset to RelativeOffset".
     */
    void UpdateInputBuffer(BitInput *BitI, uint64_t AbsoluteOffset);
    
    /*! @abstract                "Manages InputBuffer and hands out the requested bits".
     *  @remark                  "DO NOT try reading backwards, it will not work. for that use SeekBits()".
     */
    uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read);
	
    /*! @abstract                "Reads Exponential Golomb, and Truncated Exponential Golomb codes".
	 *
     *  @return                  "Returns the decoded value of the Elias/Golomb code".
	 */
    uint64_t ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated);
    
    /*! @abstract                "Decodes RICE encoded data".
     *  @param    StopBit        "MUST be a 0 or a 1. none of this funny business about how true >= 0".
     *  @return                  "Returns the count of bits aka the value encoded by the encoder".
     */
    uint64_t ReadRICE(BitInput *BitI, bool StopBit);
    
    /*! @abstract                "Writes a number in RICE format".
     *  @param    StopBit        "Has to be a 0 or a 1".
     *  @param    Data2Write     "Number to encode into RICE format".
     */
    void WriteRICE(BitOutput *BitO, bool StopBit, uint64_t Data2Write);
    
    /*! @abstract                "Shows the next X bits, without recording it as a read".
     *  @param    Bits2Peek      "Number of bits to peek".
     */
    uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek);
	
	/*! @abstract                "Writes bits to BitOutput".
	 *
	 *  @param    Data2Write     "Is the actual data to write out".
	 *  @param    NumBits        "Is the number of bits to write".
	 */
	void WriteBits(BitOutput *BitO, uint64_t Data2Write, size_t NumBits);
	
	/*! @abstract                "Writes entire buffer to the output buffer, first come first serve".
	 *
	 *  @param    Buffer2Write   "Pointer to the buffer to be written to the output buffer".
	 *  @param    BufferSize     "Size of Buffer2Write in bytes".
	 */
	void WriteBuffer(BitOutput *BitO, uintptr_t *Buffer2Write, size_t BufferSize);
	
    /*! @abstract                "Seeks Forwards and backwards in BitInput"
	 *  @remark                  "To seek backwards just use a negative number, to seek forwards positive".
	 *
	 *  @param    Bits           "The number of bits to seek".
     */
    void SeekBits(BitInput *BitI, int64_t Bits);
    
    void SkipBits(BitInput *BitI, uint8_t Bits2Skip);
    
    /*! @abstract                "Generates CRC from data".
     *  @remark                  "Uses Reciprocal representation".
     *
     *  @param    DataBuffer     "Pointer to the buffer containing the data to be verified".
     *  @param    BufferSize     "Size of the buffer in bytes".
     *  @param    Poly           "Recriprocal of the CRC polynomial".
     *  @param    Init           "The bit pattern to initalize the generator with".
     */
    uint64_t GenerateCRC(uintptr_t *DataBuffer, size_t BufferSize, uint64_t Poly, bool Init);
    
    /*! @abstract                "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     *  @remark                  "Uses Reciprocal representation".
     *
     *  @param    DataBuffer     "Pointer to the buffer containing the data to be verified".
     *  @param    BufferSize     "Size of the buffer in bytes".
     *  @param    Poly           "Recriprocal of the CRC polynomial".
     *  @param    Init           "The bit pattern to initalize the generator with".
     *  @param    EmbeddedCRC    "Value to compare the data to, to be sure it was recieved correctly".
     */
    bool VerifyCRC(uintptr_t *DataBuffer, size_t BufferSize, uint64_t Poly, bool Init, uint64_t EmbeddedCRC);
    
    /*! @abstract                "Decodes Huffman encoded data".
     *  @remark                  "It's not even CLOSE to API/ABI compatible with zlib, because zlib is shit".
     */
    void DecodeHuffman(BitInput *BitI, Huffman *Huff);
    
    /*! @abstract                "Parses DEFLATE encoded block, and sends it off to the Huffman/LZ77 decoder".
     */
    void ParseDeflate(BitInput *BitI);
    
    /*! @abstract                "Decodes LZ77 encoded data".
     *
     */
    void LZ77Decoder(BitInput *BitI, size_t WindowSize, uint8_t WordSize);
    
    /*! @abstract                "Recreates a Huffman tree from an array".
     *  @return                  "Returns a pointer to a Huffman tree contained in a struct".
     */
    HuffmanTree ReconstructHuffmanTree(BitInput *BitI, uintptr_t Table, size_t TableSize);
    
    /*! @abstract                "Creates Adler32 checksum from input data".
     *  @return                  "Returns the Adler32 data from the data input".
     */
    uint32_t GenerateAdler32(BitInput *BitI, uintptr_t *Data, size_t DataSize);
    
    /*! @abstract                "Generates Adler32 from the input data, and compares it to the stored checksum".
     *  @return                  "Returns whether the input data matched the provided checksum or not".
     */
    bool VerifyAdler32(BitInput *BitI, uintptr_t Data, size_t DataSize, uint32_t EmbeddedAdler32);
    
    /*! @abstract                "Reads raw UUID/GUID from the bitstream".
     *  @remark                  "UUID and GUID Strings are ALWAYS 20 chars (inlucing terminating char)".
     *  @return                  "Returns a pointer to the UUID/GUID string (which is ALWAYS 20 bytes)"
     */
    uintptr_t ReadUUID(BitInput *BitI);
    
    /*! @abstract                "Compares UUID/GUID 1 to UUID/GUID 2".
     *  @return                  "Returns a bool specifiing if the UUIDs/GUIDs are equal or not".
     */
    bool CompareUUIDStrings(uintptr_t UUID1, uintptr_t UUID2);
    
    void GenerateUUID(void);
    
    /*! @abstract                "Write UUID/GUID string as hyphen-less blob".
     *  @example                 "AAAAAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE aka AAAA-BB-CC-DD-EEEEEE in bytes".
     */
    void WriteUUID(BitOutput *BitO, uintptr_t UUIDString[]);
#ifdef __cplusplus
}
#endif
