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
#include <xlocale.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC poison gets puts strcpy strcat tempfile mktemp sprintf // Not secure

#pragma GCC poison gethostbyaddr gethostbyname                     // Not thread safe

#pragma GCC poison bzero                                           // Not portable

#pragma GCC poison strcmp                                          // misses cases that it shouldn't
	
#pragma GCC poison malloc

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
		BitIOStringSize           = 1024,
		BitIOPathSize             = 1024,
		BitIOUUIDSize             = 21,
		BitIOEncodedUUIDSize      = 16,
		BitIOGUIDSize             = 21,
		BitIOEncodedGUIDSize      = 16,
		BitIOFlagSize             = 3,
		BitIOMD5Size              = 16,
		BitIOMaxSwitches          = 64,
	} BitIOConstants;
	
	extern uint64_t BitInputCurrentArgument;
	extern uint64_t BitOutputCurrentArgument;
	extern uint64_t BitInputCurrentSpecifier;
	extern uint64_t BitOutputCurrentSpecifier;
	
	/*!
	 @abstract                     "List of error codes the various functions in BitIO set in ErrorStatus".
	 @remark                       "FIXME: Should the error codes be negative or positive?".
	 */
	extern enum ErrorCodes {
		/* begin BitIO specific error codes */
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
		InvalidMarker              = 22,
	} ErrorCodes;

	/*!
	 @typedef        ErrorStatus
	 @abstract                         "Allows checking of the error status of various functions".
	 */
	typedef struct ErrorStatus {
		int64_t      SkipBits;
		int64_t      PeekBits;
		int64_t      ReadBits;
		int64_t      ReadRICE;
		int64_t      ReadUUID;
		int64_t      UpdateInputBuffer;
		int64_t      InitBitInput;
		int64_t      InitBitOutput;
		int64_t      ParseInputOptions;
		int64_t      ParseOutputOptions;
		int64_t      DecodeHuffman;
		int64_t      WriteUUID;
		int64_t      WriteBits;
		//int64_t      WriteBuffer;
		int64_t      WriteRICE;
		int64_t      VerifyCRC;
	} ErrorStatus __attribute__((packed));

	/*!
	 @typedef        BitInput
	 @abstract                         "Contains variables and buffers for reading bits".
	 @remark                           "The default internal representation in BitIO is unsigned, Big Endian".

	 @constant       File              "Input file to read bits from".
	 @constant       FileSize          "Size of File in bytes".
	 @constant       FilePosition      "Current byte in the file".
	 @constant       BitsUnavailable   "Number of previously read bits in Buffer".
	 @constant       BitsAvailable     "Number of bits available for reading".
	 @constant       ErrorStatus       "Pointer to supplied ErrorStatus".
	 @constant       Buffer            "Buffer of data from File".

	 FIXME: What if we just used snprintf to increment the format number instead of splitting the string and whatnot?
	 */
	typedef struct BitInput {
		bool         IsFileBased;
		FILE        *File;
		uintptr_t   *StartReadAddress;
		size_t       ExternalBufferSize;
		size_t       FileSize;
		size_t       FilePosition;
		size_t       BitsUnavailable;
		size_t       BitsAvailable;
		uint8_t      SystemEndian:2;
		ErrorStatus *ErrorStatus;
		uint8_t      Buffer[BitInputBufferSize];
	} BitInput __attribute__((packed));

	/*!
	 @typedef        BitOutput
	 @abstract                         "Contains variables and buffers for writing bits".
	 @remark                           "The default internal representation in BitOutput is unsigned".

	 @constant       File              "Input file to read bits from".
	 @constant       BitsUnavailable   "Number of previously read bits in Buffer".
	 @constant       BitsAvailable     "Number of bits available for writing".
	 @constant       ErrorStatus       "pointer to Error struct".
	 @constant       Buffer            "Buffer of BitIOBufferSize bits from File".
	 */
	typedef struct BitOutput {
		bool         IsFileBased;
		FILE        *File;
		uintptr_t   *StartWriteAddress;
		size_t       ExternalBufferSize;
		size_t       BitsUnavailable;
		size_t       BitsAvailable;
		uint8_t      SystemEndian;
		ErrorStatus *ErrorStatus;
		uint8_t      Buffer[BitOutputBufferSize];
	} BitOutput __attribute__((packed));
	
	/*!
	 @param SwitchOrderNum    "Order in which this switch should be printed".
	 @param Switch            "Actual switch, including dash(es)".
	 @param SwitchDescription "Message to print explaining what the switch does".
	 @param SwitchResult      "String to contain the result of this switch, NULL if not found".
	 */
	typedef struct CLSwitch { //
		char       *Switch;
		char       *SwitchDescription;
		char       *SwitchResult;
	} CLSwitch;
	
	typedef struct CommandLineOptions { // [BitIOStringSize]
		size_t      NumSwitches;
		char        *ProgramName;
		char        *ProgramDescription;
		char        *AuthorLicenseCopyright;
		// Program name
		// General line describing the program
		// Switches:
		// list of switches with their descriptions printed next to them
		CLSwitch   *Switch[BitIOMaxSwitches];
	} CommandLineOptions;

	extern enum Base {
		Octal       =  8,
		Decimal     = 10,
		Hexadecimal = 16,
	} Base;

	extern enum Endian {
		UnknownEndian = 0,
		BigEndian     = 1,
		LittleEndian  = 2,
	} Endian;

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
	 @abstract                         "Tells whether the input number is even or odd".
	 @returns                          "True for odd, false for even".
	 */
	bool           IsOdd(int64_t Input);

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
	 @abstract                     "Create bitmask from binary exponent".

	 @param    Exponent            "Power to be raised by 2".
	 @return                       "If 0 is returned, then Exponent was too large".
	 */
	uint64_t       Power2Mask(uint8_t Exponent);

	/*!
	 @abstract                     "Parses command line flags for BitInput".

	 @param    BitI                "Pointer to BitInput".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
	void           ParseInputOptions(BitInput *BitI, int argc, const char *argv[]);

	/*!
	 @abstract                     "Parses command line flags for BitOutput".

	 @param    BitO                "Pointer to BitOutput".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
	void           ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]);

	/*!
	 @abstract                     "Initalizes BitInput".

	 @param    BitI                "Pointer to BitInput".
	 @param    ES                  "Pointer to ErrorStatus".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
	void           InitBitInput(BitInput *BitI, ErrorStatus *ES, int argc, const char *argv[]);

	/*!
	 @abstract                     "Initalizes BitOutput".

	 @param    BitO                "Pointer to BitOutput".
	 @param    ES                  "Pointer to ErrorStatus".
	 @param    argc                "Argc from Main()".
	 @param    argv                "Argv from Main()".
	 */
	void           InitBitOutput(BitOutput *BitO, ErrorStatus *ES, int argc, const char *argv[]);

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
	 @abstract                     "Updates BitInput->Buffer, for further reading".

	 @param    BitI                  "Pointer to BitInput".
	 @param    RelativeOffsetInBytes "Number of bytes from the beginning of the current file".
	 */
	void           UpdateInputBuffer(BitInput *BitI, int64_t RelativeOffsetInBytes);

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
	 @param    IsTruncated         "Should it be read as the truncated variant of ExpGolomb coding?"
	 */
	int64_t        ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated);

	/*!
	 @abstract                     "Reads and Decodes unary/RICE encoded data from BitInput stream".
	 @return                       "Returns the count of bits aka the value encoded by the encoder".

	 @param    BitI                "Pointer to BitInput, the Input stream".
	 @param    IsTruncated         "Has the stop bit been pruned?"
	 @param    StopBit             "MUST be a 0 or a 1. none of this funny business about how true > 0".
	 */
	uint64_t       ReadRICE(BitInput *BitI, bool IsTruncated, bool StopBit);

	/*!
	 @abstract                     "Encodes and writes data in unary/RICE format to a BitOutput stream".

	 @param    BitO                "Pointer to BitOutput, the output stream".
	 @param    IsTruncated         "Should the stop bit be pruned?"
	 @param    StopBit             "Has to be a 0 or a 1".
	 @param    Data2Write          "Number to encode into RICE format".
	 */
	void           WriteRICE(BitOutput *BitO, bool IsTruncated, bool StopBit, uint64_t Data2Write);

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
	
	extern enum PolynomialType {
		Normal   = 0,
		Reversed = 1,
	} PolynomialType;
	
	typedef struct CRC {
		uint8_t *Buffer;
		size_t   BufferSize;
		uint64_t Polynomial;
		bool     PolynomialType;
		uint8_t  PolynomialSize;
		uint64_t Initalization;
		uint64_t PreCalculatedCRC;
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
	void           GenerateMD5(uint8_t *Buffer, size_t BufferSize, char MD5String[BitIOMD5Size]);

	/*!
	 @abstract                     "Logs errors to log files, and stderr; and mail if Critical/Panic."

	 @param    SYSError            "Error message prefixed by SYS in ErrorCodes".
	 @param    Library             "Name of the program or library that called this function, to name the logfile".
	 @param    Function            "Which function is calling Log?".
	 @param    Description         "String describing what went wrong, if you need to use format specifiers, call snprintf".
	 */
	void           Log(int64_t SYSError, char Library[BitIOStringSize], char Function[BitIOStringSize], char Description[BitIOStringSize]);

	/*!
	 @abstract                     "Tells if the stream/buffer is byte aligned of not".

	 @param    BitsUsed            "Bits read from the stream, to calculate if it's on a byte aligned address".
	 @param    BytesOfAlignment    "Number of bytes to check alignment with, aka is BitsUsed a multiple of BytesOfAlignment * 8?".
	 */
	bool           IsStreamByteAligned(uint64_t BitsUsed, uint8_t BytesOfAlignment);
	
	/*!
	 @abstract                     "Writes data encoded as Exponential-Golomb aka Elias Gamma".
	 */
	void WriteExpGolomb(BitOutput *BitO, bool IsTruncated, bool IsMapped, uint64_t Data2Write);
	
	uint8_t CountBitsSet(uint64_t Data);
	
	extern enum SystemErrors {
		SYSEmergency     = 1,
		SYSPanic         = 1,
		SYSAlert         = 2,
		SYSCritical      = 3,
		SYSError         = 4,
		SYSWarning       = 5,
		SYSNotice        = 6,
		SYSInformation   = 7,
		SYSDebug         = 8,
		/* End Syslog-type eror codes */
	} SystemErrors;
	
	typedef struct LinkedList {
		uint16_t           Value;
		struct LinkedList *Next;
	} LinkedList;
	
#ifdef __cplusplus
}
#endif
