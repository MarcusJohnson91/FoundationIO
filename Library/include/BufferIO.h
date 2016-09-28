/*!
 @header    BitIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2016, Marcus Johnson
 @version   0.9.1
				FIXME: The Version number needs to be FeatureLevel.ABI.BugFix
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
	
#pragma GCC poison gets puts strcpy strcat tempfile mktemp sprintf // Not secure
 
#pragma GCC poison gethostbyaddr gethostbyname                     // Not thread safe
	
#pragma GCC poison bzero                                           // Not portable

#pragma GCC poison strcmp                                          // misses cases that it shouldn't

	/*
	typedef enum Truth {
		YES =  true,
		NO  = false,
	} Truth;
	
	 */

    /*! 
	 @abstract                            "BitIO compile time constants".
     @remark                              "Change the buffer sizes here".
	 
	 @constant  BitInputBufferSize        "Inital size of BitInput buffer".
	 @constant  BitInputBufferSizeInBits  "Inital size of BitInput buffer in bits".
	 @constant  BitOutputBufferSize       "Inital size of BitOutput buffer".
	 @constant  BitOutputBufferSizeInBits "Inital size of BitOutput buffer in bits".
	 @constant  BitIOStringSize           "Maximum size of a string in BitIO".
	 @constant  BitIOPathSize             "Maximum size of a path in the filesystem in BitIO".
	 @constant  BitIOUUIDSize             "size of a UUIDString including null terminator".
     */
    typedef enum BitIOConstants {
        BitInputBufferSize        = 4096,
        BitInputBufferSizeInBits  = BitInputBufferSize * 8,
        BitOutputBufferSize       = 4096,
        BitOutputBufferSizeInBits = BitOutputBufferSize * 8,
		BitIOStringSize           = 1024,
		BitIOPathSize             = BitIOStringSize,
		BitIOUUIDSize             = 21,
		BitIOGUIDSize             = BitIOUUIDSize,
    } BitIOConstants;
    
    extern uint64_t BitIOCurrentArgument; // This HAS to start at one; Used by the Option and Input/Output parsers.

	typedef enum SystemErrors {
		SYSEmergency               = 1,
		SYSPanic                   = 1,
		SYSAlert                   = 2,
		SYSCritical                = 3,
		SYSError                   = 4,
		SYSWarning                 = 5,
		SYSNotice                  = 6,
		SYSInformation             = 7,
		SYSDebug                   = 8,
	} SystemErrors;
    
    /*! 
	 @abstract                     "List of error codes the various functions in BitIO set in ErrorStatus".
     @remark                       "FIXME: Should the error codes be negative or positive?".
     */
    typedef enum ErrorCodes {
		/* End Syslog-type eror codes, begin BitIO specific error codes */
        Success                    = 9,
        NotEnoughMemory            = 10,
        NumberNotInRange           = 11,
        TriedReadingTooManyBits    = 12,
        TriedReadingTooFewBits     = 13,
        TriedWritingTooManyBits    = 14,
        TriedWritingTooFewBits     = 15,
        ReallocFailed              = 16,
        WrongStringSize            = 17,
		FopenFailed                = 18,
		FreadReturnedTooLittleData = 19,
		InvalidData                = 20,
		InvalidCRC                 = 21,
    } ErrorCodes;
    
    /*! 
	 @typedef  ErrorStatus
     @abstract                     "Allows checking of the error status of various functions".
     */
    typedef struct ErrorStatus {
        int64_t      SkipBits;
        int64_t      PeekBits;
        int64_t      ReadBits;
		int64_t      ReadRICE;
		int64_t      ReadUUID;
		int64_t      ReadBitBuffer;
        int64_t      UpdateInputBuffer;
        int64_t      InitBitInput;
        int64_t      InitBitOutput;
        int64_t      ParseInputOptions;
        int64_t      ParseOutputOptions;
		int64_t      DecodeHuffman;
		int64_t      WriteUUID;
		int64_t      WriteBits;
		int64_t      WriteBuffer;
		int64_t      WriteRICE;
    } ErrorStatus;
	
	/*! 
	 @typedef    BitInput
	 @abstract                     "Contains variables and buffers for reading bits".
	 @remark                       "The default internal representation in BitIO is unsigned, Big Endian".
	 
	 @constant   File              "Input file to read bits from".
	 @constant   FileSize          "Size of File in bytes".
     @constant   FilePosition      "Current byte in the file".
     @constant   BitsUnavailable   "Number of previously read bits in Buffer".
     @constant   BitsAvailable     "Number of bits available for reading".
	 @constant   BasePath          "Location and start of the name of the file, for use with format specified streams aka %03d".
	 @constant   DigitsInSpecifier "Maybe this should be called number of digits?".
	 @constant   FileNumber        "The number in the current specifier".
	 @constant   Extension         "Everything after the format specifier to regenerate the complete path".
	 @constant   ErrorStatus       "Pointer to supplied ErrorStatus".
     @constant   Buffer            "Buffer of data from File".
	 
	 FIXME: What if we just used snprintf to increment the format number instead of splitting the string and whatnot?
	 */
    typedef struct BitInput {
        FILE        *File;
        uint64_t     FileSize;
        uint64_t     FilePosition;
        uint64_t     BitsUnavailable;
		uint64_t     BitsAvailable;
		uint8_t      DigitsInSpecifier;
		int64_t      FileNumber;
		char         BasePath[BitIOPathSize];
		char         Extension[BitIOPathSize];
        ErrorStatus *ErrorStatus;
        uint8_t      Buffer[BitInputBufferSize];
    } BitInput;

	typedef struct BitBuffer {
		uint64_t     BitsUnavailable;
		uint64_t     BitsAvailable;
		ErrorStatus *ES;
		uint8_t     *Buffer;
	} BitBuffer;

	typedef struct BitFile {
		FILE        *File;
		uint64_t     FileSize;
		uint64_t     FilePosition;
	} BitFile;

	typedef struct BitIOPath {
		char         BasePath[BitIOPathSize];
		uint8_t      DigitsInSpecifier;
		int64_t      FileNumber;
		char         Extension[BitIOPathSize];
	} BitIOPath;

	typedef struct BitInput2 {
		BitIOPath   *Path;
		BitFile     *Source; // aka File
		BitBuffer   *Data;   // Buffer stuff
		ErrorStatus *Error;
	} BitInput2;

	typedef struct BitOutput2 {
		BitIOPath   *Path;
		BitFile     *Source;
		BitBuffer   *Data;
		ErrorStatus *Error;
	} BitOutput2;

	void InitBitInput2(BitInput2 *Input, ErrorStatus *Error, int argc, const char *argv[]) {
		Input->Error = Error;
		Input->Path  = calloc(sizeof(BitIOPath), 1);
	}
	
	/*! 
	 @typedef    BitOutput
	 @abstract                     "Contains variables and buffers for writing bits".
	 @remark                       "The default internal representation in BitOutput is unsigned".
	 
	 @constant   File              "Input file to read bits from".
     @constant   BitsUnavailable   "Number of previously read bits in Buffer".
	 @constant   BitsAvailable     "Number of bits available for writing".
	 @constant   BasePath          "Location and start of the name of the file, for use with format specified streams aka %03d".
	 @constant   DigitsInSpecifier "Maybe this should be called number of digits?".
	 @constant   FileNumber        "The number in the current specifier".
	 @constant   Extension         "Everything after the format specifier to regenerate the complete path".
     @constant   Buffer            "Buffer of BitIOBufferSize bits from File".
	 */
    typedef struct BitOutput {
        FILE        *File;
        uint64_t     BitsUnavailable;
        uint64_t     BitsAvailable;
		char         BasePath[BitIOPathSize];
		uint8_t      DigitsInSpecifier;
		int64_t      FileNumber;
		char         Extension[BitIOPathSize];
		int64_t      CurrentFormatSpecifier;
        ErrorStatus *ErrorStatus;
        uint8_t      Buffer[BitOutputBufferSize];
    } BitOutput;

	typedef struct BitIOString {
/*
 Features I want:
 UTF-8.
 Variable length array to save space.
 2 arrays, one to hold the number of bytes for each character, the second to actually hold the UTF-8 bytes.
 */
		uint8_t  CharSize[65535]; // first definition to hold the number of bytes for the first character, max 65535 chars in string
		uint8_t  CodePoint[262140]; // String of Unicode bytes
		size_t   StringSize; // number of letters in the string, NOT bytes
	} BitIOString;
	
	typedef enum Base {
		Octal       =  8,
		Decimal     = 10,
		Hexadecimal = 16,
	} Base;
    
    /*! 
	 @abstract                     "Swap endian of 16 bit integers".
     @param    Data2Swap           "Data to swap endian".
     */
    uint16_t SwapEndian16(uint16_t Data2Swap);
    
    /*! 
	 @abstract                     "Swap endian of 32 bit integers".
     @param    Data2Swap           "Data to swap endian".
     */
    uint32_t SwapEndian32(uint32_t Data2Swap);
    
    /*! 
	 @abstract                     "Swap endian of 64 bit integers".
     @param    Data2Swap           "Data to swap endian".
     */
    uint64_t SwapEndian64(uint64_t Data2Swap);
    
    /*! 
	 @abstract                     "Tells whether the input number is even or odd".
     @returns                      "True for odd, false for even".
     */
    bool IsOdd(int64_t Input);
    
    /*! 
	 @abstract                     "Computes the number of bytes from the number of bits".
     @remark                       "Rounds up to the next byte if not multiple of 8 bits, 1 indexed instead of 0".
     @return                       "Converts the number of bits to the number of bytes".
     */
    uint64_t Bits2Bytes(uint64_t Bits);
    
    /*! 
	 @abstract                     "Computes the number of bits from the number of bytes".
     @remark                       "Does not have sub-byte precision".
     @return                       "Converts the number of bytes to the number of bits".
     */
    uint64_t Bytes2Bits(uint64_t Bytes);
    
    /*! 
	 @abstract                     "Computes the number of bits until the next byte".
     @return                       "Returns the number of bits left".
     */
    uint8_t BitsRemaining(uint64_t BitsAvailable);
    
    /*! 
	 @abstract                     "Converts an Signed int to a Unsigned int".
     @remark                       "The internal representation in BitIO is unsigned".
     @return                       "Returns the input data in Unsigned format".
     */
    uint64_t Signed2Unsigned(int64_t Signed);
    
    /*! 
	 @abstract                     "Converts an Unsigned int to a Signed int".
     @remark                       "The internal representation in BitIO is unsigned".
     @return                       "Returns the input data in Signed format".
     */
    int64_t Unsigned2Signed(uint64_t Unsigned);
    
    /*! 
	 @abstract                     "Converts numbers from One's compliment to Two's compliment".
     @return                       "Returns the Input in Two's compliment format".
     */
    uint64_t OnesCompliment2TwosCompliment(uint64_t Input);
    
    /*! 
	 @abstract                     "Converts numbers from Two's compliment to One's compliment".
     @return                       "Returns the Input in One's compliment format".
     */
    uint64_t TwosCompliment2OnesCompliment(uint64_t Input);
    
    /*! @deprecated
	 @abstract                     "Finds the highest bit set".
     @remark                       "Starts at MSB (BitIO is Big Endian internally; It's not my fault Intel is stupid)".
     @param    InputData           "is the input int to find the highest bit set".
     */
    uint8_t FindHighestBitSet(uint64_t InputData) __attribute__((deprecated));
    
    /*! @deprecated
	 @abstract                     "Counts the number of bits set to 1"
	 
     @param    Input               "integer to count the number of set bits".
     */
    uint8_t CountBits(uint64_t Input) __attribute__((deprecated));
    
    /*! 
	 @abstract                     "Aligns bits for single-byte alignment".
     @remark                       "TODO: Should this be Input/Output neutral?".
	 
	 @param    BitI                "Pointer to BitInput".
     */
    void AlignInputBits2Byte(BitInput *BitI);
    
    /*! 
	 @abstract                     "Aligns bits for multi-byte alignment".
     @remark                       "TODO: Should this be Input/Output neutral?".
	 
	 @param    BitI                "Pointer to BitInput".
	 @param    Bytes2Align         "Number of bytes of padding to align it to. BytesOfPadding - 1, + AlignBits2Bytes"
     */
    void AlignInputBits2Bytes(BitInput *BitI, uint8_t Bytes2Align);
    
    /*! 
	 @abstract                     "Aligns bits for single-byte alignment".
     @remark                       "TODO: Should this be Input/Output neutral?".
	 
	 @param    BitO                "Pointer to BitOutput".
     */
    void AlignOutputBits2Byte(BitOutput *BitO);
    
    /*! 
	 @abstract                     "Aligns bits for multi-byte alignment".
     @remark                       "TODO: Should this be Input/Output neutral?".
	 
	 @param    BitO                "Pointer to BitOutput".
	 @param    Bytes2Align         "Number of bytes of padding to align it to. BytesOfPadding - 1, + AlignBits2Bytes"
     */
    void AlignOutputBits2Bytes(BitOutput *BitO, uint8_t Bytes2Align);
    
    /*! 
	 @abstract                     "Create bitmask from binary exponent".
     
	 @param    Base                "Base to multiply base by, Exponent times."
     @param    Exponent            "Power to be raised by 2".
	 
	 @return                       "If 0 is returned, then either Base or Exponent were too large".
     */
	uint64_t Power(uint8_t Base, uint8_t Exponent);
	
	/*! 
	 @abstract                     "Create bitmask from binary exponent".
	 
	 @param    Exponent            "Power to be raised by 2".
	 @return                       "If 0 is returned, then Exponent was too large".
	 */
    uint64_t Power2Mask(uint8_t Exponent);
	
	/*! 
	 @abstract                     "Parses command line flags for BitInput".
	 
	 @param    BitI                "Pointer to BitInput".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
    void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]);
	
	/*! 
	 @abstract                     "Parses command line flags for BitOutput".
	 
	 @param    BitO                "Pointer to BitOutput".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
    void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]);
	
	/*! 
	 @abstract                     "Initalizes BitInput".
	 
	 @param    BitI                "Pointer to BitInput".
	 @param    ES                  "Pointer to ErrorStatus".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
    void InitBitInput(BitInput *BitI, ErrorStatus *ES, int argc, const char *argv[]);
	
	/*! 
	 @abstract                     "Initalizes BitOutput".
	 
	 @param    BitO                "Pointer to BitOutput".
	 @param    ES                  "Pointer to ErrorStatus".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
    void InitBitOutput(BitOutput *BitO, ErrorStatus *ES, int argc, const char *argv[]);
    
    /*! 
	 @abstract                     "Deallocates BitInput".
     @remark                       "For use when changing files, or exiting the program".
	 
	 @param    BitI                "Pointer to BitInput".
     */
    void CloseBitInput(BitInput *BitI);
    
    /*! 
	 @abstract                     "Deallocates BitOutput"
     @remark                       "For use when changing files, or exiting the program".
	 
	 @param    BitO                "Pointer to BitOutput".
     */
    void CloseBitOutput(BitOutput *BitO);
    
    /*! 
	 @abstract                     "Updates BitInput->Buffer, for further reading".
	 
	 @param    BitI                "Pointer to BitInput".
     @param    RelativeOffset      "Number of bytes from the beginning of the current file".
     */
    static void UpdateInputBuffer(BitInput *BitI, int64_t RelativeOffset);
    
    /*! 
	 @abstract                     "Manages InputBuffer and hands out the requested bits".
     @remark                       "DO NOT try reading backwards, it will not work. for that use SkipBits()".
	 
	 @param    BitI                "Pointer to BitInput struct".
	 @param    Bits2Read           "Number of bits to read".
	 
	 TODO: Consider - Maybe it's not a good idea to buffer, because it limits you to streaming formats only...
	 TODO: Grow the buffer to accomidate a buffered formats.
     */
    uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read);
	
    /*! 
	 @abstract                     "Reads Exponential Golomb, and Truncated Exponential Golomb codes".
	 @return                       "Returns the decoded value of the Elias/Golomb code".
	 
	 @param    BitI                "Pointer to BitInput".
	 @param    IsSigned            "Should it be read as signed or unsigneed?".
     @param    IsTruncated         "Should it be read as the truncated variant of ExpGolomb coding?"
	 */
    uint64_t ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated);
    
    /*! 
	 @abstract                     "Decodes RICE encoded data".
	 @return                       "Returns the count of bits aka the value encoded by the encoder".
	 
	 @param    BitI                "Pointer to BitInput".
     @param    StopBit             "MUST be a 0 or a 1. none of this funny business about how true > 0".
     */
    uint64_t ReadRICE(BitInput *BitI, bool StopBit);
    
    /*! 
	 @abstract                     "Writes a number in RICE format".
	 
	 @param    BitO                "Pointer to BitOutput".
     @param    StopBit             "Has to be a 0 or a 1".
     @param    Data2Write          "Number to encode into RICE format".
     */
    void WriteRICE(BitOutput *BitO, bool StopBit, uint64_t Data2Write);
    
    /*!
	 @abstract                     "Shows the next X bits, without recording it as a read".
	 @param    BitI                "Pointer to BitInput".
     @param    Bits2Peek           "Number of bits to peek".
     */
    uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek);
	
	/*! 
	 @abstract                     "Writes bits to BitOutput->File".
	 
	 @param    BitO                "Pointer to BitOutput".
	 @param    Data2Write          "Is the actual data to write out".
	 @param    NumBits             "Is the number of bits to write".
	 */
	void WriteBits(BitOutput *BitO, uint64_t Data2Write, size_t NumBits);
	
	/*!
	 @abstract                     "Writes entire buffer to the output buffer, first come first serve".
	 
	 @param    BitO                "Pointer to BitOutput".
	 @param    Buffer2Write        "Pointer to the buffer to be written to the output buffer".
	 @param    BufferSize          "Size of Buffer2Write in bytes".
	 */
	void WriteBitBuffer(BitOutput *BitO, uint8_t *Buffer2Write, size_t BufferSize);
	
    /*! 
	 @abstract                     "Seeks Forwards and backwards in BitInput"
	 @remark                       "To seek backwards just use a negative number, to seek forwards positive".
	 
	 @param    BitI                "Pointer to BitInput".
	 @param    Bits                "The number of bits to skip".
     */
    void SkipBits(BitInput *BitI, int64_t Bits);
    
    /*! 
	 @abstract                     "Generates CRC from data".
     @remark                       "Uses Reciprocal representation".
     
     @param    DataBuffer          "Pointer to the buffer containing the data to be verified".
     @param    BufferSize          "Size of the buffer in bytes".
     @param    Poly                "Recriprocal of the CRC polynomial".
     @param    Init                "The bit pattern to initalize the generator with".
	 @param    CRCSize             "Number of bits that should be output as the CRC".
     */
	uint64_t GenerateCRC(uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, uint64_t Init, uint8_t CRCSize);
    
    /*! 
	 @abstract                     "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     @remark                       "Uses Reciprocal representation".
     
     @param    DataBuffer          "Pointer to the buffer containing the data to be verified".
     @param    BufferSize          "Size of the buffer in bytes".
     @param    Poly                "Recriprocal of the CRC polynomial".
     @param    Init                "The bit pattern to initalize the generator with".
	 @param    CRCSize             "Number of bits that should be output as the CRC".
     @param    EmbeddedCRC         "Value to compare the data to, to be sure it was recieved correctly".
     */
	bool VerifyCRC(uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, uint64_t Init, uint8_t CRCSize, uint64_t EmbeddedCRC);
    
    /*! 
	 @abstract                     "Decodes Huffman encoded data".
     @remark                       "It's not even CLOSE to API/ABI compatible with zlib, because zlib is shit".
     */
	//void DecodeHuffman(BitInput *BitI, Huffman *Huff);
    
    /*! 
	 @abstract                     "Parses DEFLATE encoded block, and sends it off to the Huffman/LZ77 decoder".
	 
	 @param    BitI                "Pointer to BitInput".
     */
	void ParseDeflate(BitInput *BitI);
    
    /*! 
	 @abstract                     "Creates Adler32 checksum from input data".
     @return                       "Returns the Adler32 data from the data input".
	 
	 @param   Data                 "Pointer to the data to generate the Adler hash from".
	 @param   DataSize             "Size of data".
     */
    uint32_t GenerateAdler32(uint8_t *Data, size_t DataSize);
    
    /*! 
	 @abstract                     "Generates Adler32 from the input data, and compares it to the submitted checksum".
     @return                       "Returns whether the input data matched the provided checksum or not".
	 
	 @param   Data                 "Pointer to the data to generate the Adler hash from".
	 @param   DataSize             "Size of data".
	 @param   EmbeddedAdler32      "Embedded Adler32 to compare the generated one to.".
     */
    bool VerifyAdler32(uint8_t *Data, size_t DataSize, uint32_t EmbeddedAdler32);
    
    /*! 
	 @abstract                     "Reads raw UUID/GUID from the bitstream".
     @remark                       "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
	 
	 @param   BitI                 "Pointer to BitInput".
	 @param   UUIDString           "Character array to read UUID string into".
     */
    void ReadUUID(BitInput *BitI, char *UUIDString[BitIOUUIDSize]);
    
    /*! 
	 @abstract                     "Write UUID/GUID string as hyphen-less blob".
	 @remark                       "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
	 
	 @param    BitO                "Pointer to BitOutput".
	 @param    UUIDString          "UUID string to write to the file as a binary blob, aka remove hyphens and null terminating char".
     */
    void WriteUUID(BitOutput *BitO, char UUIDString[BitIOUUIDSize]);
	
	/*!
	 @abstract                     "Logs errors to log files, and stderr; and mail if Critical/Panic."
	 
	 @param    SYSError            "Error message prefixed by SYS in ErrorCodes".
	 @param    ES                  "Pointer to the variable in ErrorStatus that should be set to ESError.".
	 @param    ESError             "Error code to add to ErrorStatus".
	 @param    Library             "Name of the program or library that called this function, to name the logfile".
	 @param    Function            "Which function is calling Log?".
	 @param    Description         "String describing what went wrong, if you need to use format specifiers, call snprintf".
	 */
	void Log(int64_t SYSError, int64_t *ES, int64_t ESError, char Library[BitIOStringSize], char Function[BitIOStringSize], char Description[BitIOStringSize]);
	
	/*!
	 @abstract                     "Tells if the stream/buffer is byte aligned of not".
	 
	 @param    BitsUnavailable     "Bits read from the stream, to calculate if it's on a byte aligned address".
	 */
	bool IsStreamByteAligned(uint64_t BitsUnavailable);
	
	/*! 
	 @abstract                     "Read/Write bits from a buffer".
	 
	 @param    Bits                "Pointer to BitBuffer struct".
	 @param    Buffer              "Pointer to the buffer to read from".
	 @param    BufferSize          "Size of Buffer".
	 */
	void InitBitBuffer(BitBuffer *Bits, uint8_t *Buffer, size_t BufferSize);
	
	/*!
	 @abstract                     "Reads bits from a buffer".
	 
	 @param    Bits                "Pointer to BitBuffer struct".
	 @param    Bits2Read           "Number of bits to read from the buffer pointed to by Bits".
	 */
	uint64_t ReadBitBuffer(BitBuffer *Bits, uint8_t Bits2Read);

	typedef struct Probabilities {
		double Maximum;
		double Minimum;
	} Probabilities;

	uint64_t ReadArithmetic(BitInput *Input, uint64_t *MaximumTable, uint64_t *MinimumTable, size_t TableSize, uint64_t Bits2Decode);
	
	void WriteArithmetic(BitOutput *BitO, Probabilities *Probability, uint64_t Bits2Encode);
	
	void ArithmeticDecoder(BitInput *BitI);
	
	void CloseBitBuffer(BitBuffer *Bits);
	
#ifdef __cplusplus
}
#endif
