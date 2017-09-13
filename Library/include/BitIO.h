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
#include <syslog.h>
#include <unistd.h>
#endif

#ifndef BITIOBYTEBITORDERS
#define BITIOBYTEBITORDERS

#ifndef BITIOUNKNOWNENDIAN
#define BITIOUNKNOWNENDIAN   0
#endif  /* BITIOUNKNOWNENDIAN */

#ifndef BITIOUNKNOWNBITORDER
#define BITIOUNKNOWNBITORDER 1
#endif  /* BITIOUNKNOWNBITORDER */

#ifndef BITIOLITTLEENDIAN
#define BITIOLITTLEENDIAN    2
#endif  /* LITTLEENDIAN */

#ifndef BITIOBIGENDIAN
#define BITIOBIGENDIAN       3
#endif  /* BITIOBIGENDIAN */

#ifndef BITIOLSBitFIRST
#define BITIOLSBitFIRST      4
#endif  /* BITIOLSBitFIRST */

#ifndef BITIOMSBitFIRST
#define BITIOMSBitFIRST      5
#endif  /* BITIOMSBitFIRST */

#ifndef TARGETBYTEORDER
#define TARGETBYTEORDER
#endif  /* TARGETBYTEORDER */

#ifndef TARGETBITORDER
#define TARGETBITORDER
#endif  /* TARGETBITORDER */

#endif /* BITIOBYTEBITORDERS */

#pragma once

#ifndef LIBBITIO_BitIO_H
#define LIBBITIO_BitIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum              BitIOConstants
     @abstract                                    "BitIO compile time constants".
     @constant          BitIOUUIDStringSize       "Size of a UUIDString including dashes and null terminator".
     @constant          BitIOGUIDStringSize       "Size of a GUIDString including dashes and null terminator".
     @constant          BitIOBinaryUUIDSize       "Size of a binary UUID string".
     @constant          BitIOBinaryGUIDSize       "Size of a binary GUID string".
     */
    enum BitIOConstants {
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
    
#ifndef _POSIX_VERSION
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
#endif
    
    /*!
     @typedef           BitBuffer
     @abstract                                    "Contains variables and a pointer to a buffer for reading and writing bits".
     @constant          NumBits                   "The number of bits in the buffer".
     @constant          BitOffset                 "The number of bits previously read/written".
     @constant          Buffer                    "A pointer to an unsigned byte buffer".
     */
    typedef struct      BitBuffer                 BitBuffer;
    
    /*!
     @typedef           BitInput
     @abstract                                    "Contains File/Socket pointers for reading to a BitBuffer".
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
     @abstract                                    "Deallocates the instance of BitOutput pointed to by BitI".
     @remark                                      "For use when changing files, or exiting the program".
     @param             BitO                      "Pointer to the instance of BitOutput you want to delete".
     */
    void                DeinitBitOutput(BitOutput *BitO);
    
    /*!
     @abstract                                    "Deallocates the instance of BitBuffer pointed to by BitB".
     @param             BitB                      "Pointer to the instance of BitBuffer you want to delete".
     */
    void                DeinitBitBuffer(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Closes the GlobalLogFile".
     */
    void                DeinitBitIOGlobalLogFile(void);
    
    /*!
     @abstract                                    "Swap bits in a byte".
     @param             Byte                      "Byte to swap bits".
     */
    inline uint8_t      SwapBitsInByte(const uint8_t Byte);
    
    /*!
     @abstract                                    "Swap nibbles in a byte".
     @param             Byte2Swap                 "Byte to swap nibbles".
     */
    inline uint8_t      SwapNibble(const uint8_t Byte2Swap);
    
    /*!
     @abstract                                    "Swap endian of 16 bit integers".
     @param             Data2Swap                 "Data to swap endian".
     */
    inline uint16_t     SwapEndian16(const uint16_t Data2Swap);
    
    /*!
     @abstract                                    "Swap endian of 32 bit integers".
     @param             Data2Swap                 "Data to swap endian".
     */
    inline uint32_t     SwapEndian32(const uint32_t Data2Swap);
    
    /*!
     @abstract                                    "Swap endian of 64 bit integers".
     @param             Data2Swap                 "Data to swap endian".
     */
    inline uint64_t     SwapEndian64(const uint64_t Data2Swap);
    
    /*!
     @abstract                                    "Computes the number of bits from the number of bytes".
     @remark                                      "Does not have sub-byte precision".
     @return                                      "Returns the number of bits".
     @param             Bytes                     "The number of bytes you want to intrepret as bits".
     */
    inline int64_t      Bytes2Bits(const int64_t Bytes);
    
    /*!
     @abstract                                    "Computes the number of bytes from the number of bits".
     @return                                      "Returns The number of bytes".
     @param             Bits                      "The bits to convert to bytes".
     @param             RoundUp                   "Should the resulting bytes be rounded up or down?".
     */
    inline int64_t      Bits2Bytes(const int64_t Bits, const bool RoundUp);
    
    /*!
     @abstract                                    "Computes the number of bits required to hold a certain amount of symbols".
     @remark                                      "Rounds up to the next integer bits to ensure all symbols can be contained in simple binary".
     @param             NumSymbols                "The number of symbols you're looking to contain in a binary number".
     */
    inline uint64_t     NumBits2ReadSymbols(const uint64_t NumSymbols);
    
    /*!
     @abstract                                    "Tells whether Input is even or odd".
     @return                                      "True for odd, false for even".
     @param             Number2Check              "The number to see if it's odd or even".
     */
    inline bool         IsOdd(const int64_t Number2Check);
    
    /*!
     @abstract                                    "Computes the number of bytes left in the file".
     @return                                      "Returns the number of bytes left in the file".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    fpos_t              BytesRemainingInBitInput(BitInput *BitI);
    
    /*!
     @abstract                                    "Gets the size of the file pointed to by BitI"
     @return                                      "Returns the value in BitI->FileSize".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    fpos_t              GetBitInputFileSize(BitInput *BitI);
    
    /*!
     @abstract                                    "Returns the position of the current file".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    fpos_t              GetBitInputFilePosition(BitInput *BitI);
    
    /*!
     @abstract                                    "Returns the number of bits used in BitB".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint64_t            GetBitBufferPosition(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Gets the size of the BitBuffer".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint64_t            GetBitBufferSize(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Seeks to the end of BitI->File to determine the size".
     @param             BitI                      "Pointer to the instance of BitInput".
     */
    void                FindFileSize(BitInput *BitI);
    
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
     @abstract                                    "Opens a socket for reading".
     */
    void                OpenInputSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol);
    
    /*!
     @abstract                                    "Opens a socket for writing".
     */
    void                OpenOutputSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol);
    
    /*!
     @abstract                                    "Seeks Forwards and backwards in BitInput".
     @remark                                      "To seek backwards just use a negative number, to seek forwards positive".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             Bits2Skip                 "The number of bits to skip".
     */
    void                SkipBits(BitBuffer *BitB, const int64_t Bits2Skip);
    
    /*!
     @abstract                                    "Tells if the stream/buffer is byte aligned or not".
     @remark                                      "Checks the stream is aligned on an BytesOfAlignment boundary, not that there are X bits of padding".
     @param             BytesOfAlignment          "Are you trying to see if it's aligned to a byte, short, word, etc alignment? Specify in number of bytes".
     */
    bool                IsBitBufferAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                                    "Aligns bits for multi-byte alignment".
     @remark                                      "Aligns the stream on a BytesOfAlignment boundary, it does NOT add (BytesOfAlignment * 8) bits of padding".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             BytesOfAlignment          "Align BitB to X byte boundary".
     */
    void                AlignBitBuffer(BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                                    "Reads raw UUID/GUID from the bitstream".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint8_t            *ReadBinaryUUID(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Reads raw UUID/GUID from the bitstream".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint8_t            *ReadUUIDString(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Reads raw UUID/GUID from the bitstream".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint8_t            *ReadBinaryGUID(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Reads raw UUID/GUID from the bitstream".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     */
    uint8_t            *ReadGUIDString(BitBuffer *BitB);
    
    /*!
     @abstract                                    "Converts a UUIDString to a GUIDString by swapping the endian of each section".
     @param             UUIDString                "An array containing the UUID in string form".
     @return                                      "Returns a pointer to a GUIDString".
     */
    uint8_t            *ConvertUUIDString2GUIDString(const uint8_t *UUIDString);
    
    /*!
     @abstract                                    "Converts a GUIDString to a UUIDString by swapping the endian of each section".
     @param             GUIDString                "An array containing the converted UUID in string form".
     @return                                      "Returns a pointer to a UUIDString".
     */
    uint8_t            *ConvertGUIDString2UUIDString(const uint8_t *GUIDString);
    
    /*!
     @abstract                                    "Verify two UUIDStrings match each other".
     @remark                                      "Matches hyphens and NULL terminating character as well, it's pretty literal".
     @param             UUIDString1               "Pointer to a UUIDString to be matched against".
     @param             UUIDString2               "Pointer to a UUIDString to match against UUIDString1".
     */
    bool                CompareUUIDStrings(const uint8_t *UUIDString1, const uint8_t *UUIDString2);
    
    /*!
     @abstract                                    "Write UUID/GUID string as hyphen-less blob".
     @remark                                      "UUID and GUID Strings are ALWAYS 21 chars (including terminating char)".
     @param             BitB                      "Pointer to the instance of BitBuffer".
     @param             UUIDString                "UUID string to write to the file as a binary blob, aka remove hyphens and null terminating char".
     */
    void                WriteUUID(BitBuffer *BitB, const uint8_t *UUIDString);
    
    /*!
     @abstract                                    "Reads Bytes2Read into a buffer pointed to by BitB from InputFile".
     @remark                                      "If the pointer to BitBuffer is not new, all the old contents will be lost".
     @param             BitI                      "Pointer to BitInput".
     @param             BitB                      "Pointer to BitBuffer to put the bytes into".
     @param             Bytes2Read                "The number of bytes to read from the InputFile into the Buffer".
     */
    void                ReadBitInput2BitBuffer(BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read);
    
    /*!
     @abstract                                    "Writes a BitBuffer to a file, kinda shitty tho".
     @param             BitO                      "Pointer to BitOutput to write the buffer to".
     @param             Buffer2Write              "The buffer to be written to the output file".
     @param             Bytes2Write               "The number of bytes from the buffer to write to the file".
     */
    void                WriteBitBuffer2BitOutput(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write);
    
    /*!
     @abstract                                    "Logs errors to the user provided log file, or stderr".
     @param             ErrorSeverity             "Error message prefixed by SYS in ErrorCodes".
     @param             LibraryOrProgram          "Name of the program or library that called this function".
     @param             FunctionName              "Which function is calling Log?".
     @param             Description               "String describing what went wrong / error code".
     */
    void                Log(const uint8_t ErrorSeverity, const char *__restrict LibraryOrProgram, const char *__restrict FunctionName, const char *__restrict Description, ...);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_BitIO_H */
