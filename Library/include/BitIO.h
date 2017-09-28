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

#ifndef   BITIOGUUIDTYPES
#define   BITIOGUUIDTYPES
#ifndef   BitIOGUIDString
#define   BitIOGUIDString    ULLONG_MAX
typedef   unsigned long long GUIDString_t;
#endif /* BitIOGUIDString */
#ifndef   BitIOUUIDString
#define   BitIOUUIDString    LLONG_MAX
typedef   signed long long   UUIDString_t;
#endif /* BitIOUUIDString */
#ifndef   BitIOBinaryGUID
#define   BitIOBinaryGUID    FLT_MIN
typedef   float              BinaryGUID_t;
#endif /* BitIOBinaryGUID */
#ifndef   BitIOBinaryUUID
#define   BitIOBinaryUUID    DBL_MAX
typedef   double             BinaryUUID_t;
#endif /* BITIOGUUIDTYPES */
#endif /* BITIOGUUIDTYPES */

#pragma once

#ifndef LIBBITIO_BitIO_H
#define LIBBITIO_BitIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef Macintosh
#define BitIONewLineSize 1
    static const char BitIOLineEnding[1]  = {"\r"};
#elif _POSIX_VERSION
#define BitIONewLineSize 1
    static const char BitIOLineEnding[1]  = {"\n"};
#elif  _WIN32
#define BitIONewLineSize 2
    static const char BitIOLineEnding[2]  = {"\r\n"};
#endif
    
    /*!
     @enum				BitIOConstants
     @abstract									"BitIO compile time constants".
     @constant			BitIOGUUIDStringSize	"Size of a UUIDString or GUIDString including dashes, and null terminator".
     @constant			BitIOBinaryGUUIDSize	"Size of a BinaryUUID or BinaryGUID".
     */
    enum BitIOConstants {
        				BitIOGUUIDStringSize	= (21),
        				BitIOBinaryGUUIDSize	= (16),
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
     @constant			File					"Input File/Socket to read into a BitBuffer".
     @constant			Socket					"Socket number".
     @constant			IsFileOrSocket			"Is this BitInput for a file or socket"?
     @constant			FileSize				"Size of the File in bytes".
     @constant			FilePosition			"Current byte in the file".
     @constant			FileSpecifierNum		"Which file are we currently on?".
     */
    typedef struct		BitInput				BitInput;
    
    /*!
     @typedef			BitOutput
     @abstract									"Contains File/Socket pointers for writing from a BitBuffer".
     @constant			File					"Input File/Socket to write a BitBuffer into".
     @constant			Socket					"Socket number".
     @constant			IsFileOrSocket			"Is this BitInput for a file or socket"?
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
     @abstract									"Swap bits in a byte".
     @param				Byte					"Byte to swap bits".
     @return									"Returns swapped byte".
     */
 	uint8_t      		SwapBitsInByte(const uint8_t Byte);
    
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
     @abstract									"Computes the number of bits required to hold a certain amount of symbols".
     @remark									"Rounds up to the next integer number of bits to ensure all symbols can be contained in single integer".
     @param				NumSymbols				"The number of symbols you're looking to contain in a binary number".
     @return									"Returns the number of bits required to read a symbol".
     */
 	uint64_t     		NumBits2ReadSymbols(const uint64_t NumSymbols);
    
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
    fpos_t              BytesRemainingInBitInput(BitInput *BitI);
    
    /*!
     @abstract									"Gets the size of the file pointed to by BitI"
     @param				BitI					"Pointer to the instance of BitInput".
     @return									"Returns the value in BitI->FileSize if it exists".
     */
    fpos_t              GetBitInputFileSize(BitInput *BitI);
    
    /*!
     @abstract									"Gets the position of the Input file from the start".
     @param				BitI					"Pointer to the instance of BitInput".
     @return									"Returns the position of the file in bytes from the beginning"
     */
    fpos_t              GetBitInputFilePosition(BitInput *BitI);
    
    /*!
     @abstract									"Gets the offset of the BitBuffer".
     @param				BitB					"Pointer to the instance of BitBuffer".
     @return									"Returns the position offset from the start of BitBuffer".
     */
    uint64_t            GetBitBufferPosition(BitBuffer *BitB);
    
    /*!
     @abstract									"Gets the size of the BitBuffer".
     @param				BitB					"Pointer to the instance of BitBuffer".
     @return									"Returns the number of bits the buffer can hold max".
     */
    uint64_t            GetBitBufferSize(BitBuffer *BitB);
    
    /*!
     @abstract									"Seeks to the end of BitI->File to determine the size".
     @param				BitI					"Pointer to the instance of BitInput".
     */
    void				FindFileSize(BitInput *BitI);
    
    /*!
     @abstract									"Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
     @param				BitI					"Pointer to the instance of BitInput".
     @param				Path2Open				 "Path to the input file to open".
     @param				OpenForReadWrite		"Should the File be opened for both reading and writing?".
     */
    void				OpenInputFile(BitInput *BitI, const char *Path2Open, bool OpenForReadWrite);
    
    /*!
     @abstract									"Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitO->File".
     @param				BitO					"Pointer to the instance of BitOutput".
     @param				Path2Open				"Path to the output file to open".
     */
    void				OpenOutputFile(BitOutput *BitO, const char *Path2Open);
    
    /*!
     @abstract									"Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitIOGlobalLogFile".
     @param				LogFilePath				"Path to the log file to open/create".
     */
    void				OpenLogFile(const char *LogFilePath);
    
    /*!
     @abstract									"Opens a socket for reading".
     */
    void				OpenInputSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol);
    
    /*!
     @abstract									"Opens a socket for writing".
     */
    void				OpenOutputSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol);
    
    /*!
     @abstract									"Seeks Forwards and backwards in BitInput".
     @remark									"To seek backwards just use a negative number, to seek forwards positive".
     @param				BitB					"Pointer to the instance of BitBuffer".
     @param				Bits2Skip				"The number of bits to skip".
     */
    void				SkipBits(BitBuffer *BitB, const int64_t Bits2Skip);
    
    uint64_t		    ReadBitsFromLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    uint64_t		    ReadBitsFromLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    uint64_t		    ReadBitsFromMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    uint64_t		    ReadBitsFromMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    
#define ReadBits(BitBByteOrder,BitBBitOrder,BitB,Bits2Read)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadBitsFromLSByteLSBit,BitBMSBit_t:ReadBitsFromLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadBitsFromMSByteLSBit,BitBMSBit_t:ReadBitsFromMSByteMSBit))(BitB,Bits2Read)
    
    uint64_t		    PeekBitsFromLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    uint64_t		    PeekBitsFromLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    uint64_t		    PeekBitsFromMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    uint64_t		    PeekBitsFromMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    
#define PeekBits(BitBByteOrder,BitBBitOrder,BitB,Bits2Peek)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:PeekBitsFromLSByteLSBit,BitBMSBit_t:PeekBitsFromLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:PeekBitsFromMSByteLSBit,BitBMSBit_t:PeekBitsFromMSByteMSBit))(BitB,Bits2Peek)
    
    uint64_t		    ReadUnaryFromLSByteLSBit(BitBuffer *BitB, const bool IsStrictlyPositive, const bool StopBit);
    uint64_t		    ReadUnaryFromLSByteMSBit(BitBuffer *BitB, const bool IsStrictlyPositive, const bool StopBit);
    uint64_t		    ReadUnaryFromMSByteLSBit(BitBuffer *BitB, const bool IsStrictlyPositive, const bool StopBit);
    uint64_t		    ReadUnaryFromMSByteMSBit(BitBuffer *BitB, const bool IsStrictlyPositive, const bool StopBit);
    
#define ReadUnary(BitBByteOrder,BitBBitOrder,BitB,IsStrictlyPositive,StopBit)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadUnaryFromLSByteLSBit,BitBMSBit_t:ReadUnaryFromLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadUnaryFromMSByteLSBit,BitBMSBit_t:ReadUnaryFromMSByteMSBit))(BitB,IsStrictlyPositive,StopBit)
    
    uint64_t		    ReadExpGolombFromLSByteLSBit(BitBuffer *BitB, const bool IsSigned);
    uint64_t		    ReadExpGolombFromLSByteMSBit(BitBuffer *BitB, const bool IsSigned);
    uint64_t		    ReadExpGolombFromMSByteLSBit(BitBuffer *BitB, const bool IsSigned);
    uint64_t		    ReadExpGolombFromMSByteMSBit(BitBuffer *BitB, const bool IsSigned);
    
#define ReadExpGolomb(BitBByteOrder,BitBBitOrder,BitB,IsSigned)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadExpGolombFromLSByteLSBit,BitBMSBit_t:ReadExpGolombFromLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadExpGolombFromMSByteLSBit,BitBMSBit_t:ReadExpGolombFromMSByteMSBit))(BitB,IsSigned)
    
    uint8_t		       *ReadGUUIDAsUUIDString(BitBuffer *BitB);
    uint8_t		       *ReadGUUIDAsGUIDString(BitBuffer *BitB);
    uint8_t		       *ReadGUUIDAsBinaryUUID(BitBuffer *BitB);
    uint8_t		       *ReadGUUIDAsBinaryGUID(BitBuffer *BitB);
    
#define ReadGUUID(GUUIDType,BitB)_Generic((GUUIDType),UUIDString_t:ReadGUUIDAsUUIDString,GUIDString_t:ReadGUUIDAsGUIDString,BinaryUUID_t:ReadGUUIDAsBinaryUUID,BinaryGUID_t:ReadGUUIDAsBinaryGUID)(BitB)
    
    void				WriteBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    void				WriteBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    void				WriteBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    void				WriteBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
#define WriteBits(BitBByteOrder,BitBBitOrder,BitB,NumBits2Write,Bits2Insert)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteBitsAsLSByteLSBit,BitBMSBit_t:WriteBitsAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteBitsAsMSByteLSBit,BitBMSBit_t:WriteBitsAsMSByteMSBit))(BitB,NumBits2Write,Bits2Insert)
    
    uint8_t		       *WriteGUUIDAsUUIDString(BitBuffer *BitB, const uint8_t *UUIDString);
    uint8_t		       *WriteGUUIDAsGUIDString(BitBuffer *BitB, const uint8_t *GUIDString);
    uint8_t		       *WriteGUUIDAsBinaryUUID(BitBuffer *BitB, const uint8_t *BinaryUUID);
    uint8_t		       *WriteGUUIDAsBinaryGUID(BitBuffer *BitB, const uint8_t *BinaryGUID);
    
#define WriteGUUID(GUUIDType,BitB,GUUID)_Generic((GUUIDType),UUIDString_t:WriteGUUIDAsUUIDString,GUIDString_t:WriteGUUIDAsGUIDString,BinaryUUID_t:WriteGUUIDAsBinaryUUID,BinaryGUID_t:WriteGUUIDAsBinaryGUID)(BitB,GUUID)
    
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
    bool				IsBitBufferAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract									"Aligns bits for multi-byte alignment".
     @remark									"Aligns the stream on a BytesOfAlignment boundary, it does NOT add (BytesOfAlignment * 8) bits of padding".
     @param				BitB					"Pointer to the instance of BitBuffer".
     @param				BytesOfAlignment		"Align BitB to X byte boundary".
     */
    void				AlignBitBuffer(BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    
    
    /*!
     @abstract									"Reads Bytes2Read into a buffer pointed to by BitB from InputFile".
     @remark									"If the pointer to BitBuffer is not new, all the old contents will be lost".
     @param				BitI					"Pointer to BitInput".
     @param				BitB					"Pointer to BitBuffer to put the bytes into".
     @param				Bytes2Read				"The number of bytes to read from the InputFile into the Buffer".
     */
    void				ReadBitInput2BitBuffer(BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read);
    
    /*!
     @abstract									"Writes a BitBuffer to a file, kinda shitty tho".
     @param				BitO					"Pointer to BitOutput to write the buffer to".
     @param				Buffer2Write		    "The buffer to be written to the output file".
     @param				Bytes2Write				"The number of bytes from the buffer to write to the file".
     */
    void				WriteBitBuffer2BitOutput(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write);
    
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
