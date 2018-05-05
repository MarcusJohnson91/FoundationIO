#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for fpos_t */

#include "Macros.h"                   /* Included for FoundationIOTargetOS */
#include "StringIO.h"                 /* Included for UTF8, UTF16 */

#if   (FoundationIOTargetOS == POSIXOS)
#include <sys/socket.h>               /* Included for connect, socket, sockaddr */
#include <unistd.h>                   /* Included for read and shit */
#elif (FoundationIOTargetOS == WindowsOS)
#include <io.h>                       /* Included because WinCon needs it */
#include <winsock.h>                  /* Included for the socket support on Windows */
#endif

#pragma  once

#ifndef  FoundationIO_BitIO_H
#define  FoundationIO_BitIO_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*!
     @header    BitIO.h
     @author    Marcus Johnson
     @copyright 2015+
     @version   1.0.0
     @brief     This header contains code related to reading and writing files and sockets, and utility functions to manage those details.
     */
    
    /*!
     @enum                      BitIOConstants
     @abstract                                                  "BitIO compile time constants".
     @constant                  BitIONullString                 "String terminating NULL character".
     @constant                  BitIONULLStringSize             "How large is the NULL terminator for a string"?
     */
    enum BitIOConstants {
                                BitIONULLString                 =  0,
                                BitIONULLStringSize             =  1,
    };
    
    /*!
     @enum                      BitInputOutputFileTypes
     @abstract                                                  "Is this BitInput or BitOutput connected to a File or Socket?".
     @constant                  BitIOUnknownFileType            "Unknown file type".
     @constant                  BitIOFile                       "This instance of BitInput/BitOutput is connected to a File".
     @constant                  BitIOSocket                     "This instance of BitInput/BitOutput is connected to a Socket".
     */
    typedef enum BitInputOutputFileTypes {
                                BitIOUnknownFileType            = 0,
                                BitIOFile                       = 1,
                                BitIOSocket                     = 2,
    } BitInputOutputFileTypes;
    
    /*!
     @enum                      UnaryTypes
     @constant                  CountUnary                      "Supports whole numbers (including zero)".
     @constant                  TruncatedCountUnary             "Supports counting numbers (excluding zero)".
     @constant                  WholeUnary                      "Supports all the integers including zero and negatives (up to 2^63 -1 anyway)".
     */
    typedef enum UnaryTypes {
                                UnknownUnary                    = 0,
                                CountUnary                      = 1,
                                TruncatedCountUnary             = 2,
                                WholeUnary                      = 3,
    } UnaryTypes;
    
    /*!
     @typedef                   BitInput
     @abstract                                                  "Contains File/Socket pointers for reading to a BitBuffer".
     */
    typedef struct              BitInput                        BitInput;
    
    /*!
     @typedef                   BitOutput
     @abstract                                                  "Contains File/Socket pointers for writing from a BitBuffer".
     */
    typedef struct              BitOutput                       BitOutput;
    
    /*!
     @typedef                   BitBuffer
     @abstract                                                  "Contains variables and a pointer to a buffer for reading and writing bits".
     */
    typedef struct              BitBuffer                       BitBuffer;
    
    /* BitBuffer */
    /*!
     @abstract                                                  "Initializes a BitBuffer".
     @param                     BitBufferSize                   "The number of bytes to create BitBuffer with".
     @return                                                    "Returns a pointer to the BitBuffer".
     */
    BitBuffer                  *BitBuffer_Init(const uint64_t BitBufferSize);
    
    /*!
     @abstract                                                  "Gets the size of the BitBuffer in bits".
     @param                     BitB                            "BitBuffer Pointer".
     @return                                                    "Returns the number of bits the buffer can hold max".
     */
    uint64_t                    BitBuffer_GetSize(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Gets the offset of the BitBuffer in bits".
     @param                     BitB                            "BitBuffer Pointer".
     @return                                                    "Returns the offset from the start of BitBuffer in bits".
     */
    uint64_t                    BitBuffer_GetPosition(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Gets the number of bits that are free in the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @return                                                    "Returns the offset from the start of BitBuffer in bits - the current offset".
     */
    uint64_t                    BitBuffer_GetBitsFree(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Get the alignment status of the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     AlignmentSize                   "The number of bits to check the alignment of".
     @return                                                    "Returns whether the BitBuffer is aligned or not".
     */
    bool                        BitBuffer_IsAligned(BitBuffer *BitB, const uint8_t AlignmentSize); // BytesOfAlignment = AlignmentSize
    
    /*!
     @abstract                                                  "Aligns BitBuffer".
     @remark                                                    "Zero pads the BitBuffer's offset to a multiple of AlignmentSize".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     AlignmentSize                   "The alignment size in bits".
     */
    void                        BitBuffer_Align(BitBuffer *BitB, const uint8_t AlignmentSize);
    
    /*!
     @abstract                                                  "Seeks Forwards and backwards in BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Skip                       "The number of bits to skip".
     */
    void                        BitBuffer_Skip(BitBuffer *BitB, const int64_t Bits2Skip);
    
    /*!
     @abstract                                                  "Changes the size of an already initalized BitBuffer".
     @remark                                                    "Resizing a BitBuffer IS destructive, call BitBuffer_Update after this".
     @param                     BitB                            "BitBuffer Pointer to resize".
     @param                     NewSize                         "The new size of the internal buffer in bits".
     */
    void                        BitBuffer_Resize(BitBuffer *BitB, const uint64_t NewSize);
    
    /*!
     @abstract                                                  "Reads fresh data to a BitBuffer from a BitInput".
     @remark                                                    "Is NOT destructive, it will keep any unread data in the buffer".
     @param                     BitB                            "The BitBuffer to update".
     @param                     BitI                            "BitInput Pointer".
     */
    void                        BitBuffer_Update(BitBuffer *BitB, BitInput *BitI);
    
    /*!
     @abstract                                                  "Peeks (reads but without recording that it's been read) bits from BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to peek from the BitBuffer".
     */
    uint64_t                    PeekBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Peek);
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer".
     */
    uint64_t                    ReadBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Read);
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
    uint64_t                    ReadUnary(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads a Exp-Golomb encoded field from the BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     */
    uint64_t                    ReadExpGolomb(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit);
    
    /*!
     @abstract                                                  "Reads a UTF-8 encoded string from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     StringSize                      "Size of the string in CodeUnits".
     */
    UTF8                       *ReadUTF8(BitBuffer *BitB, uint64_t StringSize);
    
    /*!
     @abstract                                                  "Reads a UTF-16 encoded string from the BitBuffer".
     @param                     StringByteOrder                 "The Byte order of the string to be written IF there's no BOM, if there is, use UnknownByteFirst".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     StringSize                      "Size of the string in CodeUnits".
     */
    UTF16                      *ReadUTF16(ByteBitOrders StringByteOrder, BitBuffer *BitB, uint64_t StringSize);
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "Contains the data to write into the BitBuffer".
     */
    void                        WriteBits(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes unary encoded bits to the BitBuffer".
     @param                     ByteOrder                       "What byte order should the bits be in the BitBuffer"?
     @param                     BitOrder                        "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding should we use"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     UnaryBits2Write                 "Value to be written as Unary encoded".
     */
    void                        WriteUnary(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const uint8_t UnaryBits2Write);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields to the BitBuffer".
     @param                     ByteOrder                       "What byte order should the bits be in the BitBuffer"?
     @param                     BitOrder                        "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding should we use"?
     @param                     StopBit                         "What bit is the stop bit"?
     @param                     Field2Write                     "Value to be encoded as Exp-Golomb and written".
     */
    void                        WriteExpGolomb(ByteBitOrders ByteOrder, ByteBitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write);
    
    /*!
     @abstract                                                  "Reads a UTF-8 encoded string from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     String2Write                    "The string to write to the BitBuffer".
     */
    void                        WriteUTF8(BitBuffer *BitB, UTF8 *String2Write);
    
    /*!
     @abstract                                                  "Writes a UTF-16 encoded string to the BitBuffer".
     @param                     StringByteOrder                 "The byte order of the string to be read IF there's no BOM, if there is, use UnknownByteFirst".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     String2Write                    "The string to write to the BitBuffer".
     */
    void                        WriteUTF16(ByteBitOrders StringByteOrder, BitBuffer *BitB, UTF16 *String2Write);
    
    /*!
     @abstract                                                  "Deallocates the instance of BitBuffer pointed to by BitB".
     @param                     BitB                            "BitBuffer Pointer you want to free".
     */
    void                        BitBuffer_Deinit(BitBuffer *BitB);
    /* BitBuffer */
    
    /* BitInput */
    /*!
     @abstract                                                  "Initializes a BitInput structure".
     @return                                                    "Returns a pointer to said BitInput structure".
     */
    BitInput                   *BitInput_Init(void);
    
    /*!
     @abstract                                                  "Reads Bytes2Read into Buffer2Read from BitInput".
     @remark                                                    "If the BitBuffer Pointer is not new, all the old contents will be lost".
     @param                     BitI                            "BitInput Pointer".
     @param                     Buffer2Read                     "BitBuffer Pointer".
     @param                     Bytes2Read                      "The number of bytes to read from the BitInput into the BitBuffer".
     */
    void                        BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const int64_t Bytes2Read);
    
    /*!
     @abstract                                                  "Opens an input file, pointed to by Path2Open".
     @param                     BitI                            "BitInput Pointer".
     @param                     Path2Open                       "Path to the input file to open".
     */
    void                        BitInput_OpenFile(BitInput *BitI, UTF8 *Path2Open);
    
    /*!
     @abstract                                                  "Opens a socket for reading".
     @param                     BitI                            "BitInput Pointer".
     @param                     Domain                          "What domain should communication take place on"?
     @param                     Type                            "What type of Socket are you trying to open"?
     @param                     Protocol                        "What protocol are you trying to use over this socket"?
     */
    void                        BitInput_OpenSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol);
    
    /*!
     @abstract                                                  "Connects BitInput to a socket".
     @param                     BitI                            "BitInput Pointer".
     @param                     SocketAddress                   "The address of the socket".
     @param                     SocketSize                      "I honest have no idea what this means".
     */
    void                        BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, const uint64_t SocketSize);
    
    /*!
     @abstract                                                  "Gets the size of the BitInput file".
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the value in BitI->FileSize if it exists".
     */
    fpos_t                      BitInput_GetFileSize(BitInput *BitI);
    
    /*!
     @abstract                                                  "Gets the position of the BitInput file from the start".
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the position of the file in bytes from the beginning"
     */
    fpos_t                      BitInput_GetFilePosition(BitInput *BitI);
    
    /*!
     @abstract                                                  "Computes the number of bytes left in the file".
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the number of bytes left in the file".
     */
    fpos_t                      BitInput_BytesRemaining(BitInput *BitI);
    
    /*!
     @abstract                                                  "Deallocates BitInput".
     @remark                                                    "For use when changing files, or exiting the program".
     @param                     BitI                            "BitInput Pointer you want to free".
     */
    void                        BitInput_Deinit(BitInput *BitI);
    /* BitInput */
    
    /* BitOutput */
    /*!
     @abstract                                                  "Initializes a BitOutput structure".
     @return                                                    "Returns a pointer to said BitOutput structure".
     */
    BitOutput                  *BitOutput_Init(void);
    
    /*!
     @abstract                                                  "Opens an output file for writing".
     @param                     BitO                            "BitOutput Pointer".
     @param                     Path2Open                       "Path to the output file to open".
     */
    void                        BitOutput_OpenFile(BitOutput *BitO, UTF8 *Path2Open);
    
    /*!
     @abstract                                                  "Opens an output file for writing".
     @param                     OriginalPath                    "BitOutput Pointer".
     @param                     NewPath                         "Path to the output file to open".
     @return                                                    "Returns an error code from rename or _wrename".
     */
    int32_t                     BitOutput_RenameFile(UTF8 *OriginalPath, UTF8 *NewPath);
    
    /*!
     @abstract                                                  "Opens a socket for writing".
     @param                     BitO                            "BitOutput Pointer".
     @param                     Domain                          "What domain should communication take place on"?
     @param                     Type                            "What type of Socket are you trying to open"?
     @param                     Protocol                        "What protocol are you trying to use over this socket"?
     */
    void                        BitOutput_OpenSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol);
    
    /*!
     @abstract                                                  "Connects BitOutput to a socket".
     @param                     BitO                            "BitOutput Pointer".
     @param                     SocketAddress                   "The address of the socket".
     @param                     SocketSize                      "I honestly have no idea what this means".
     */
    void                        BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, const uint64_t SocketSize);
    
    /*!
     @abstract                                                  "Writes a BitBuffer to a file".
     @param                     BitO                            "BitOutput Pointer to write the buffer to".
     @param                     Buffer2Write                    "The buffer to be written to the output file".
     @param                     Bytes2Write                     "The number of bytes from the buffer to write to the file".
     */
    void                        BitOutput_WriteBitBuffer(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write);
    
    /*!
     @abstract                                                  "Deallocates the instance of BitOutput pointed to by BitO".
     @remark                                                    "For use when changing files, or exiting the program".
     @param                     BitO                            "BitOutput Pointer you want to free".
     */
    void                        BitOutput_Deinit(BitOutput *BitO);
    /* BitOutput */
    
#ifdef   __cplusplus
}
#endif

#endif   /* FoundationIO_BitIO_H */
