/*!
 @header    BitIO.h
 @author    Marcus Johnson
 @copyright 2016 - 2017
 @version   0.6.0
 @brief     This header contains code related to reading and writing files, and utility functions to support that goal.
 */

#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>
#endif

#ifndef   BitIOYesNo
#define   BitIOYesNo
#ifndef    Yes
#define    Yes true
#endif  /* Yes */
#ifndef    No
#define    No false
#endif  /* No */
#endif /* BitIOYesNo */

#ifndef    BITIOBYTEBITORDERS
#define    BITIOBYTEBITORDERS
#ifndef    BitIOLSByte
#define    BitIOLSByte        ULLONG_MAX
typedef    unsigned long long BitBLSByte_t;
#endif  /* BitIOLittleEndian */
#ifndef    BitIOMSByte
#define    BitIOMSByte        LLONG_MIN
typedef    signed long long   BitBMSByte_t;
#endif  /* BitIOBigEndian */
#ifndef    BitIOLSBit
#define    BitIOLSBit         ULLONG_MAX
typedef    unsigned long long BitBLSBit_t;
#endif  /* BitIOLSBit */
#ifndef    BitIOMSBit
#define    BitIOMSBit         LLONG_MIN
typedef    signed long long   BitBMSBit_t;
#endif  /* BitIOMSBit */
#endif  /* BITIOBYTEBITORDERS */

#ifndef    BITIOGUUIDTYPES
#define    BITIOGUUIDTYPES
#ifndef    BitIOGUIDString
#define    BitIOGUIDString    ULLONG_MAX
typedef    unsigned long long GUIDString_t;
#endif  /* BitIOGUIDString */
#ifndef    BitIOUUIDString
#define    BitIOUUIDString    LLONG_MAX
typedef    signed long long   UUIDString_t;
#endif  /* BitIOUUIDString */
#ifndef    BitIOBinaryGUID
#define    BitIOBinaryGUID    FLT_MIN
typedef    float              BinaryGUID_t;
#endif  /* BitIOBinaryGUID */
#ifndef    BitIOBinaryUUID
#define    BitIOBinaryUUID    DBL_MAX
typedef    double             BinaryUUID_t;
#endif  /* BITIOGUUIDTYPES */
#endif  /* BITIOGUUIDTYPES */

#pragma once

#ifndef    LIBBITIO_BitIO_H
#define    LIBBITIO_BitIO_H

#ifdef __cplusplus
extern "C" {
#endif
	
#ifdef     Macintosh
#define    BitIONewLineSize 1
	static const char BitIOLineEnding[1]  = {"\r"};
#elif     _POSIX_VERSION
#define    BitIONewLineSize 1
	static const char BitIOLineEnding[1]  = {"\n"};
#elif     _WIN32
#define    BitIONewLineSize 2
	static const char BitIOLineEnding[2]  = {"\r\n"};
#endif
	
	/*!
	 @enum				BitIOConstants
	 @abstract									"BitIO compile time constants".
	 @constant			BitIOGUUIDStringSize	"Size of a UUIDString or GUIDString including dashes, and null terminator".
	 @constant			BitIOBinaryGUUIDSize	"Size of a BinaryUUID or BinaryGUID".
	 @constant			BitIOStringNULLSize		"How large is the NULL terminator for a string"?
	 */
	enum BitIOConstants {
						BitIOGUUIDStringSize	= (20),
						BitIOBinaryGUUIDSize	= (16),
						BitIOStringNULLSize		= (1),
	};
	
	/*!
	 @enum				BitIOSourceDrainTypes
	 @abstract									"Is this BitInput or BitOutput connected to a File or Socket?".
	 @constant			BitIOFile				"This instance of BitInput/BitOutput is connected to a File".
	 @constant			BitIOSocket				"This instance of BitInput/BitOutput is connected to a Socket".
	 */
	enum BitIOSourceDrainTypes {
						BitIOUnknownFileType	= 0,
						BitIOFile				= 1,
						BitIOSocket				= 2,
	};
	
#ifndef _POSIX_VERSION
	/*!
	 @enum				BitIOLogTypes
	 @constant			LOG_EMERG				"The system is unusable, the program is quitting (equivalent to panic)".
	 @constant			LOG_ALERT				"Immediate action is required".
	 @constant			LOG_CRIT				"Critical condition encountered".
	 @constant			LOG_ERR					"Error condition encountered".
	 @constant			LOG_WARNING				"Warning condition encountered".
	 @constant			LOG_NOTICE				"Normal, but important condition encountered".
	 @constant			LOG_INFO				"Informational message logged".
	 @constant			LOG_DEBUG				"Testing information logged".
	 */
	enum BitIOLogTypes {
						LOG_EMERG				= 0,
						LOG_ALERT				= 1,
						LOG_CRIT				= 2,
						LOG_ERR					= 3,
						LOG_WARNING				= 4,
						LOG_NOTICE				= 5,
						LOG_INFO				= 6,
						LOG_DEBUG				= 7,
	};
#endif
	
	/*!
	 @typedef			BitBuffer
	 @abstract									"Contains variables and a pointer to a buffer for reading and writing bits".
	 @constant			NumBits					"The number of bits in the buffer".
	 @constant			BitOffset				"The number of bits previously read/written".
	 @constant			Buffer					"A pointer to an unsigned byte buffer".
	 */
	typedef struct		BitBuffer				BitBuffer;
	
	/*!
	 @typedef			BitInput
	 @abstract									"Contains File/Socket pointers for reading to a BitBuffer".
	 @constant			SourceType				"Is this BitInput for a file or socket"?
	 @constant			File					"Input File/Socket to read into a BitBuffer".
	 @constant			Socket					"Socket number".
	 @constant			FileSize				"Size of the File in bytes".
	 @constant			FilePosition			"Current byte in the file".
	 @constant			FileSpecifierNum		"Which file are we currently on?".
	 */
	typedef struct		BitInput				BitInput;
	
	/*!
	 @typedef			BitOutput
	 @abstract									"Contains File/Socket pointers for writing from a BitBuffer".
	 @constant			DrainType				"Is this BitOutput for a file or socket"?
	 @constant			File					"Input File/Socket to write a BitBuffer into".
	 @constant			Socket					"Socket number".
	 @constant			FilePosition			"Current byte in the file".
	 @constant			FileSpecifierNum		"Which file are we currently on?".
	 */
	typedef struct		BitOutput				BitOutput;
	
	/*!
	 @abstract									"Initializes a BitInput structure".
	 @return									"Returns a pointer to said BitInput structure".
	 */
	BitInput		   *InitBitInput(void);
	
	/*!
	 @abstract									"Initializes a BitOutput structure".
	 @return									"Returns a pointer to said BitOutput structure".
	 */
	BitOutput		   *InitBitOutput(void);
	
	/*!
	 @abstract									"Initializes a BitBuffer structure".
	 @param				BitBufferSize			"Number of bytes to create BitBuffer with".
	 @return									"Returns a pointer to said BitBuffer structure".
	 */
	BitBuffer		   *InitBitBuffer(const uint64_t BitBufferSize);
	
	/*!
	 @abstract									"Deallocates BitInput".
	 @remark									"For use when changing files, or exiting the program".
	 @param				BitI					"Pointer to the instance of BitInput you want to delete".
	 */
	void				DeinitBitInput(BitInput *BitI);
	
	/*!
	 @abstract									"Deallocates the instance of BitOutput pointed to by BitI".
	 @remark									"For use when changing files, or exiting the program".
	 @param				BitO					"Pointer to the instance of BitOutput you want to delete".
	 */
	void				DeinitBitOutput(BitOutput *BitO);
	
	/*!
	 @abstract									"Deallocates the instance of BitBuffer pointed to by BitB".
	 @param				BitB					"Pointer to the instance of BitBuffer you want to delete".
	 */
	void				DeinitBitBuffer(BitBuffer *BitB);
	
	/*!
	 @abstract									"Closes the GlobalLogFile".
	 */
	void				DeinitBitIOGlobalLogFile(void);
	
	/*!
	 @abstract									"Integer Power function".
	 @param				Base					"What base should the power be calculated in"?
	 @param				Exponent				"How many times should the Base be raised"?
	 @return									"Returns the result of 1 *= Base, Exponent times".
	 */
	uint64_t            Power(const uint64_t Base, const uint64_t Exponent);
	
	/*!
	 @abstract									"Swap bits in a byte".
	 @param				Byte					"Byte to swap bits".
	 @return									"Returns swapped byte".
	 */
	uint8_t      		SwapBits(const uint8_t Byte);
	
	/*!
	 @abstract									"Swap nibbles in a byte".
	 @param				Byte2Swap				"Byte to swap nibbles".
	 @return									"Returns byte with swapped nibbles".
	 */
	uint8_t      		SwapNibble(const uint8_t Byte2Swap);
	
	/*!
	 @abstract									"Swap endian of 16 bit integers".
	 @param				Data2Swap				"Data to swap endian".
	 @return									"Returns swapped uint16_t".
	 */
	uint16_t     		SwapEndian16(const uint16_t Data2Swap);
	
	/*!
	 @abstract									"Swap endian of 32 bit integers".
	 @param				Data2Swap				"Data to swap endian".
	 @return									"Returns swapped uint32_t".
	 */
	uint32_t     		SwapEndian32(const uint32_t Data2Swap);
	
	/*!
	 @abstract									"Swap endian of 64 bit integers".
	 @param				Data2Swap				"Data to swap endian".
	 @return									"Returns swapped uint64_t".
	 */
	uint64_t     		SwapEndian64(const uint64_t Data2Swap);
	
	/*!
	 @abstract									"Computes the number of bits from the number of bytes".
	 @param				Bytes					"The number of bytes you want to interpret as bits".
	 @return									"Returns the number of bits".
	 */
	int64_t      		Bytes2Bits(const int64_t Bytes);
	
	/*!
	 @abstract									"Computes the number of bytes from the number of bits".
	 @param				Bits					"The bits to convert to bytes".
	 @param				RoundUp					"Should the resulting bytes be rounded up or down?".
	 @return									"Returns the number of bytes".
	 */
	int64_t      		Bits2Bytes(const int64_t Bits, const bool RoundUp);
	
	/*!
	 @abstract									"Computes the number of bits required to hold a certain symbol".
	 @remark									"Rounds up to the next integer number of bits to ensure all symbols can be contained in a single integer".
	 @param				Symbol					"The symbol you're looking to store".
	 @return									"Returns the number of bits required to store a symbol".
	 */
	uint64_t     		NumBits2StoreSymbol(const uint64_t Symbol);
	
	/*!
	 @abstract									"Tells whether Input is even or odd".
	 @param				Number2Check			"The number to see if it's odd or even".
	 @return									"True for odd, false for even".
	 */
	bool         		IsOdd(const int64_t Number2Check);
	
	/*!
	 @abstract									"Computes the number of bytes left in the file".
	 @param				BitI					"Pointer to the instance of BitInput".
	 @return									"Returns the number of bytes left in the file".
	 */
	fpos_t              BitInputBytesRemainingInFile(BitInput *BitI);
	
	/*!
	 @abstract									"Gets the size of the file pointed to by BitI"
	 @param				BitI					"Pointer to the instance of BitInput".
	 @return									"Returns the value in BitI->FileSize if it exists".
	 */
	fpos_t              BitInputGetFileSize(BitInput *BitI);
	
	/*!
	 @abstract									"Gets the position of the Input file from the start".
	 @param				BitI					"Pointer to the instance of BitInput".
	 @return									"Returns the position of the file in bytes from the beginning"
	 */
	fpos_t              BitInputGetFilePosition(BitInput *BitI);
	
	/*!
	 @abstract									"Gets the offset of the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @return									"Returns the position offset from the start of BitBuffer".
	 */
	uint64_t            BitBufferGetPosition(BitBuffer *BitB);
	
	/*!
	 @abstract									"Gets the size of the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @return									"Returns the number of bits the buffer can hold max".
	 */
	uint64_t            BitBufferGetSize(BitBuffer *BitB);
	
	/*!
	 @abstract									"Seeks to the end of BitI->File to determine the size".
	 @param				BitI					"Pointer to the instance of BitInput".
	 */
	void				BitInputFindFileSize(BitInput *BitI);
	
	/*!
	 @abstract									"Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
	 @param				BitI					"Pointer to the instance of BitInput".
	 @param				Path2Open				"Path to the input file to open".
	 */
	void				BitInputOpenFile(BitInput *BitI, const char *Path2Open);
	
	/*!
	 @abstract									"Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitO->File".
	 @param				BitO					"Pointer to the instance of BitOutput".
	 @param				Path2Open				"Path to the output file to open".
	 */
	void				BitOutputOpenFile(BitOutput *BitO, const char *Path2Open);
	
	/*!
	 @abstract									"Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitIOGlobalLogFile".
	 @param				LogFilePath				"Path to the log file to open/create".
	 */
	void				OpenLogFile(const char *LogFilePath);
	
	/*!
	 @abstract									"Opens a socket for reading".
	 */
	void				BitInputOpenSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol);
	
	/*!
	 @abstract									"Opens a socket for writing".
	 */
	void				BitOutputOpenSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol);
	
	/*!
	 @abstract									"Connects BitInput to a socket".
	 */
	void				BitInputConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, const uint64_t SocketSize);
	
	/*!
	 @abstract									"Connects BitOutput to a socket".
	 */
	void				BitOutputConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, const uint64_t SocketSize);
	
	/*!
	 @abstract									"Seeks Forwards and backwards in BitInput".
	 @remark									"To seek backwards just use a negative number, to seek forwards positive".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Skip				"The number of bits to skip".
	 */
	void				BitBufferSkip(BitBuffer *BitB, const int64_t Bits2Skip);
	
	/*!
	 @abstract									"Reads bits from BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Read				"The number of bits to read from the BitBuffer, as LSByte, LSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    ReadBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read);
	
	/*!
	 @abstract									"Reads bits from BitBuffer".
	 @param				BitB                 	"Pointer to the instance of BitBuffer".
	 @param				Bits2Read           	"The number of bits to read from the BitBuffer, as LSByte, MSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    ReadBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read);
	
	/*!
	 @abstract									"Reads bits from BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Read				"The number of bits to read from the BitBuffer, as MSByte, LSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    ReadBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read);
	
	/*!
	 @abstract									"Reads bits from BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Read				"The number of bits to read from the BitBuffer, as MSByte, MSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    ReadBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read);
	
	/*!
	 @abstract                                  "Reads bits from BitBuffer".
	 @param				BitBByteOrder			"What byte order are the bits in the BitBuffer for this field stored as"?
	 @param				BitBBitOrder			"What bit order are the bits in the BitBuffer for this field stored as"?
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Read				"The number of bits to read from the BitBuffer".
	 */
#define ReadBits(BitBByteOrder,BitBBitOrder,BitB,Bits2Read)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadBitsAsLSByteLSBit,BitBMSBit_t:ReadBitsAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadBitsAsMSByteLSBit,BitBMSBit_t:ReadBitsAsMSByteMSBit))(BitB,Bits2Read)
	
	/*!
	 @abstract                                  "Peeks (read but without recording that it's been read) bits from BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Peek				"The number of bits to Peek from the BitBuffer, as LSByte, LSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    PeekBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
	
	/*!
	 @abstract									"Peeks (read but without recording that it's been read) bits from BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Peek				"The number of bits to Peek from the BitBuffer, as LSByte, MSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    PeekBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
	
	/*!
	 @abstract									"Peeks (read but without recording that it's been read) bits from BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Peek				"The number of bits to Peek from the BitBuffer, as MSByte, LSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    PeekBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
	
	/*!
	 @abstract									"Peeks (read but without recording that it's been read) bits from BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Peek				"The number of bits to Peek from the BitBuffer, as MSByte, MSBit format, to the runtime byte/bit order".
	 */
	uint64_t		    PeekBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
	
	/*!
	 @abstract									"Peeks (read but without recording that it's been read) bits from BitBuffer".
	 @param				BitBByteOrder			"What byte order are the bits in the BitBuffer for this field stored as"?
	 @param				BitBBitOrder			"What bit order are the bits in the BitBuffer for this field stored as"?
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				Bits2Peek				"The number of bits to peek from the BitBuffer".
	 */
#define PeekBits(BitBByteOrder,BitBBitOrder,BitB,Bits2Peek)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:PeekBitsAsLSByteLSBit,BitBMSBit_t:PeekBitsAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:PeekBitsAsMSByteLSBit,BitBMSBit_t:PeekBitsAsMSByteMSBit))(BitB,Bits2Peek)
	
	/*!
	 @abstract									"Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				NumBits2Write			"How many bits from Bits2Write should we write?".
	 @param				Bits2Write				"The variable to read the bits into the BitBuffer".
	 */
	void                WriteBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
	
	/*!
	 @abstract									"Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				NumBits2Write			"How many bits from Bits2Write should we write?".
	 @param				Bits2Write				"The variable to read the bits into the BitBuffer".
	 */
	void                WriteBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
	
	/*!
	 @abstract									"Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				NumBits2Write			"How many bits from Bits2Write should we write?".
	 @param				Bits2Write				"The variable to read the bits into the BitBuffer".
	 */
	void                WriteBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
	
	/*!
	 @abstract									"Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				NumBits2Write			"How many bits from Bits2Write should we write?".
	 @param				Bits2Write				"The variable to read the bits into the BitBuffer".
	 */
	void                WriteBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
	
	/*!
	 @abstract									"Writes bits to the BitBuffer".
	 @param				BitBByteOrder			"What byte order are the bits in the BitBuffer for this field stored as"?
	 @param				BitBBitOrder			"What bit order are the bits in the BitBuffer for this field stored as"?
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				NumBits2Write			"How many bits from Bits2Write should we write?".
	 @param				Bits2Write				"The variable to read the bits into the BitBuffer".
	 */
#define WriteBits(BitBByteOrder,BitBBitOrder,BitB,NumBits2Write,Bits2Write)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteBitsAsLSByteLSBit,BitBMSBit_t:WriteBitsAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteBitsAsMSByteLSBit,BitBMSBit_t:WriteBitsAsMSByteMSBit))(BitB,NumBits2Write,Bits2Write)
	
	/*!
	 @abstract									"Reads unary encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 */
	uint64_t		    ReadUnaryAsLSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit);
	
	/*!
	 @abstract									"Reads unary encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 */
	uint64_t		    ReadUnaryAsLSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit);
	
	/*!
	 @abstract									"Reads unary encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 */
	uint64_t		    ReadUnaryAsMSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit);
	
	/*!
	 @abstract									"Reads unary encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 */
	uint64_t		    ReadUnaryAsMSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit);
	
	/*!
	 @abstract									"Reads unary encoded fields from the BitBuffer".
	 @param				BitBByteOrder			"What byte order are the bits in the BitBuffer for this field stored as"?
	 @param				BitBBitOrder			"What bit order are the bits in the BitBuffer for this field stored as"?
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 */
#define ReadUnary(BitBByteOrder,BitBBitOrder,BitB,IsZeroAvailable,IsTruncated,StopBit)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadUnaryAsLSByteLSBit,BitBMSBit_t:ReadUnaryAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadUnaryAsMSByteLSBit,BitBMSBit_t:ReadUnaryAsMSByteMSBit))(BitB,IsZeroAvailable,IsTruncated,StopBit)
	
	/*!
	 @abstract									"Writes unary encoded fields to the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 @param				Field2Write				"Value to be written as Unary encoded".
	 */
	void                WriteUnaryAsLSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write);
	
	/*!
	 @abstract									"Writes unary encoded fields to the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 @param				Field2Write				"Value to be written as Unary encoded".
	 */
	void                WriteUnaryAsLSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write);
	
	/*!
	 @abstract									"Writes unary encoded fields to the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 @param				Field2Write				"Value to be written as Unary encoded".
	 */
	void                WriteUnaryAsMSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write);
	
	/*!
	 @abstract									"Writes unary encoded fields to the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Will we need to read a zero? If so, set this to false".
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 @param				Field2Write				"Value to be written as Unary encoded".
	 */
	void                WriteUnaryAsMSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write);
	
	/*!
	 @abstract									"Writes unary encoded bits to the BitBuffer".
	 @param				BitBByteOrder			"What byte order should the bits be in the BitBuffer"?
	 @param				BitBBitOrder			"What bit order should the bits be in the BitBuffer"?
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsZeroAvailable			"Is writing a zero possible"?
	 @param				StopBit					"Is the stop bit a one or a zero"?
	 @param				Field2Write				"Value to be written as Unary encoded".
	 */
#define WriteUnary(BitBByteOrder,BitBBitOrder,BitB,IsZeroAvailable,IsTruncated,StopBit,Field2Write)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteUnaryAsLSByteLSBit,BitBMSBit_t:WriteUnaryAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteUnaryAsMSByteLSBit,BitBMSBit_t:WriteUnaryAsMSByteMSBit))(BitB,IsZeroAvailable,IsTruncated,StopBit,Field2Write)
	
	/*!
	 @abstract									"Reads Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				StopBit					"What bit is the stop bit"?
	 */
	uint64_t		    ReadExpGolombAsLSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit);
	
	/*!
	 @abstract									"Reads Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				StopBit					"What bit is the stop bit"?
	 */
	uint64_t		    ReadExpGolombAsLSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit);
	
	/*!
	 @abstract									"Reads Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				StopBit					"What bit is the stop bit"?
	 */
	uint64_t		    ReadExpGolombAsMSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit);
	
	/*!
	 @abstract									"Reads Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				StopBit					"What bit is the stop bit"?
	 */
	uint64_t		    ReadExpGolombAsMSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit);
	
	/*!
	 @abstract									"Writes Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitBByteOrder			"What byte order should the bits be in the BitBuffer"?
	 @param				BitBBitOrder			"What bit order should the bits be in the BitBuffer"?
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				StopBit					"What bit is the stop bit"?
	 */
#define ReadExpGolomb(BitBByteOrder,BitBBitOrder,BitB,IsSigned,StopBit)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadExpGolombAsLSByteLSBit,BitBMSBit_t:ReadExpGolombAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadExpGolombAsMSByteLSBit,BitBMSBit_t:ReadExpGolombAsMSByteMSBit))(BitB,IsSigned,StopBit)
	
	/*!
	 @abstract									"Writes Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				Field2Write				"Value to be encoded as Exp-Golomb and written".
	 */
	void                WriteExpGolombAsLSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write);
	
	/*!
	 @abstract									"Writes Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				Field2Write				"Value to be encoded as Exp-Golomb and written".
	 */
	void                WriteExpGolombAsLSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write);
	
	/*!
	 @abstract									"Writes Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				Field2Write				"Value to be encoded as Exp-Golomb and written".
	 */
	void                WriteExpGolombAsMSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write);
	
	/*!
	 @abstract									"Writes Exp-Golomb encoded fields from the BitBuffer".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field"?
	 @param				Field2Write				"Value to be encoded as Exp-Golomb and written".
	 */
	void                WriteExpGolombAsMSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write);
	
	/*!
	 @abstract									"Writes Exp-Golomb encoded fields to the BitBuffer".
	 @param				BitBByteOrder			"What byte order should the bits be in the BitBuffer"?
	 @param				BitBBitOrder			"What bit order should the bits be in the BitBuffer"?
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				IsSigned				"Can there be negative values in this field?".
	 @param				Field2Write				"Value to be encoded as Exp-Golomb and written".
	 */
#define WriteExpGolomb(BitBByteOrder,BitBBitOrder,BitB,IsSigned,StopBit,Field2Write)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteExpGolombAsLSByteLSBit,BitBMSBit_t:WriteExpGolombAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteExpGolombAsMSByteLSBit,BitBMSBit_t:WriteExpGolombAsMSByteMSBit))(BitB,IsSigned,StopBit,Field2Write)
	
	/*!
	 @abstract									"Reads a BinaryGUUID/GUUIDString from the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @return									"Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes, there is no NULL terminator".
	 */
	uint8_t		       *ReadGUUIDAsUUIDString(BitBuffer *BitB);
	
	/*!
	 @abstract									"Reads a BinaryGUUID/GUUIDString from the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @return									"Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes, there is no NULL terminator".
	 */
	uint8_t		       *ReadGUUIDAsGUIDString(BitBuffer *BitB);
	
	/*!
	 @abstract									"Reads a BinaryGUUID/GUUIDString from the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @return									"Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes".
	 */
	uint8_t		       *ReadGUUIDAsBinaryUUID(BitBuffer *BitB);
	
	/*!
	 @abstract									"Reads a BinaryGUUID/GUUIDString from the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @return									"Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes".
	 */
	uint8_t		       *ReadGUUIDAsBinaryGUID(BitBuffer *BitB);
	
	/*!
	 @abstract									"Reads a BinaryGUUID/GUUIDString from the BitBuffer".
	 @param				GUUIDType				"GUUIDType is BitIOUUIDString, BitIOGUIDString, BitIOBinaryUUID, or BitIOBinaryGUID"
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @return									"Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes".
	 */
#define ReadGUUID(GUUIDType,BitB)_Generic((GUUIDType),UUIDString_t:ReadGUUIDAsUUIDString,GUIDString_t:ReadGUUIDAsGUIDString,BinaryUUID_t:ReadGUUIDAsBinaryUUID,BinaryGUID_t:ReadGUUIDAsBinaryGUID)(BitB)
	
	/*!
	 @abstract									"Writes a BinaryGUUID/GUUIDString to the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @param				UUIDString				"Pointer to the UUIDString you want to write".
	 */
	void		        WriteGUUIDAsUUIDString(BitBuffer *BitB, const uint8_t *UUIDString);
	
	/*!
	 @abstract									"Writes a BinaryGUUID/GUUIDString to the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @param				GUIDString				"Pointer to the UUIDString you want to write".
	 */
	void		        WriteGUUIDAsGUIDString(BitBuffer *BitB, const uint8_t *GUIDString);
	
	/*!
	 @abstract									"Writes a BinaryGUUID/GUUIDString to the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @param				BinaryUUID				"Pointer to the UUIDString you want to write".
	 */
	void		        WriteGUUIDAsBinaryUUID(BitBuffer *BitB, const uint8_t *BinaryUUID);
	
	/*!
	 @abstract									"Writes a BinaryGUUID/GUUIDString to the BitBuffer".
	 @param				BitB					"Pointer to an instance of BitBuffer".
	 @param				BinaryGUID				"Pointer to the UUIDString you want to write".
	 */
	void		        WriteGUUIDAsBinaryGUID(BitBuffer *BitB, const uint8_t *BinaryGUID);
	
#define WriteGUUID(GUUIDType,BitB,GUUID)_Generic((GUUIDType),BinaryUUID_t:WriteGUUIDAsBinaryUUID,BinaryGUID_t:WriteGUUIDAsBinaryGUID,UUIDString_t:WriteGUUIDAsUUIDString,GUIDString_t:WriteGUUIDAsGUIDString)(BitB,GUUID)
	
	/*!
	 @abstract									"Compares GUUIDStrings or BinaryGUUIDs (but not a GUUIDString to a BinaryGUUID) for equilivence".
	 @param				GUUID1				    "Pointer to GUUIDString or BinaryGUUID to be compared".
	 @param				GUUID2				    "Pointer to GUUIDString or BinaryGUUID to be compared".
	 @param				GUUIDSize				"The size of the GUUIDs, either BitIOGUUIDStringSize or BitIOBinaryGUUIDSize"
	 @return									"Returns whether GUUID1 and GUUID2 match".
	 */
	bool				CompareGUUIDs(const uint8_t *GUUID1, const uint8_t *GUUID2, const uint8_t GUUIDSize);
	
	/*!
	 @abstract									"Converts a GUID/UUIDString to a BinaryGUID/UUID".
	 @param             GUUIDString             "Pointer to a GUID/UUIDString".
	 @return                                    "Returns a pointer to a BinaryGUID/UUID".
	 */
	uint8_t            *ConvertGUUIDString2BinaryGUUID(const uint8_t *GUUIDString);
	
	/*!
	 @abstract									"Converts a BinaryGUID/UUID to a GUID/UUIDString".
	 @param             BinaryGUUID             "Pointer to a BinaryGUID/UUID".
	 @return                                    "Returns a pointer to a GUID/UUIDString".
	 */
	uint8_t            *ConvertBinaryGUUID2GUUIDString(const uint8_t *BinaryGUUID);
	
	/*!
	 @abstract									"Converts a GUID/UUIDString to a UUID/GUIDString".
	 @param             GUUIDString             "Pointer to a GUID/UUIDString".
	 @return                                    "Returns a pointer to a UUID/GUIDString".
	 */
	uint8_t            *SwapGUUIDString(const uint8_t *GUUIDString);
	
	/*!
	 @abstract									"Converts a BinaryGUID/UUID to a BinaryUUID/GUID".
	 @param             BinaryGUUID             "Pointer to a BinaryGUID/UUID".
	 @return                                    "Returns a pointer to a BinaryUUID/GUID".
	 */
	uint8_t            *SwapBinaryGUUID(const uint8_t *BinaryGUUID);
	
	/*!
	 @abstract									"Frees a BinaryGUUID aka BinaryGUID/BinaryUUID or GUUIDString, aka GUIDString/UUIDString".
	 */
	void				DeinitGUUID(uint8_t *GUUID);
	
	/*!
	 @abstract									"Tells if the stream/buffer is byte aligned or not".
	 @remark									"Checks the stream is aligned on an BytesOfAlignment boundary, not that there are X bits of padding".
	 @param				BytesOfAlignment		"Are you trying to see if it's aligned to a byte, short, word, etc alignment? Specify in number of bytes".
	 @return									"Returns whether the BitBuffer is aligned on a multiple of BytesOfAlignment".
	 */
	bool				BitBufferIsAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment);
	
	/*!
	 @abstract									"Aligns bits for multi-byte alignment".
	 @remark									"Aligns the stream on a BytesOfAlignment boundary, it does NOT add (BytesOfAlignment * 8) bits of padding".
	 @param				BitB					"Pointer to the instance of BitBuffer".
	 @param				BytesOfAlignment		"Align BitB to X byte boundary".
	 */
	void				BitBufferMakeAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment);
	
	/*!
	 @abstract									"Reads Bytes2Read into a buffer pointed to by BitB from InputFile".
	 @remark									"If the pointer to BitBuffer is not new, all the old contents will be lost".
	 @param				BitB					"Pointer to BitBuffer to put the bytes into".
	 @param				BitI					"Pointer to BitInput".
	 @param				Bytes2Read				"The number of bytes to read from the InputFile into the Buffer".
	 */
	void				BitBufferReadFromBitInput(BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read);
	
	/*!
	 @abstract									"Writes a BitBuffer to a file, kinda shitty tho".
	 @param				BitO					"Pointer to BitOutput to write the buffer to".
	 @param				Buffer2Write		    "The buffer to be written to the output file".
	 @param				Bytes2Write				"The number of bytes from the buffer to write to the file".
	 */
	void				BitBufferWrite2BitOutput(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write); // BitBufferWrite2BitOutput
	
	/*!
	 @abstract									"Logs errors to the user provided log file, or stderr".
	 @param				ErrorSeverity			"Error message prefixed by SYS in ErrorCodes".
	 @param				LibraryOrProgram		"Name of the program or library that called this function".
	 @param				FunctionName		    "Which function is calling Log?".
	 @param				Description				"String describing what went wrong / error code".
	 */
	void				Log(const uint8_t ErrorSeverity, const char *__restrict LibraryOrProgram, const char *__restrict FunctionName, const char *__restrict Description, ...);
	
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_BitIO_H */
