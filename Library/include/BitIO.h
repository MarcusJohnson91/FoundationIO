#include <stdbool.h>                  /* Included for bool true/false, Yes/No are in BitIOMacros */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for fpos_t */

#include "../include/StringIO.h"
#include "../include/BitIOMacros.h"

#pragma  once

#ifndef  LIBBITIO_BitIO_H
#define  LIBBITIO_BitIO_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*!
     @header    BitIO.h
     @author    Marcus Johnson
     @copyright 2015+
     @version   0.9.0
     @brief     This header contains code related to reading and writing files and sockets, and utility functions to manage those details.
     */
    
    /*!
     @enum                      BitIOConstants
     @abstract                                                  "BitIO compile time constants".
     @constant                  BitIONullString                 "String terminating NULL character".
     @constant                  BitIONULLStringSize             "How large is the NULL terminator for a string"?
     @constant                  BitIOGUUIDStringSize            "Size of a UUIDString or GUIDString including dashes, and null terminator".
     @constant                  BitIOBinaryGUUIDSize            "Size of a BinaryUUID or BinaryGUID".
     */
    enum BitIOConstants {
                                BitIONULLString                 =  0,
                                BitIONULLStringSize             =  1,
                                BitIOGUUIDStringSize            = 20 + BitIONULLStringSize,
                                BitIOBinaryGUUIDSize            = 16,
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
     @enum                      GUUIDTypes
     @constant                  UnknownGUUID                    "The GUUID type is unknown, invalid".
     @constant                  GUIDString                      "The GUUID is a GUID string, aka little endian/Least Significant Byte first UUID with hyphens".
     @constant                  UUIDString                      "The GUUID is a UUID string, aka big endian   /Most  Significant Byte first UUID with hyphens".
     @constant                  BinaryGUID                      "The GUUID is a Binary GUID, aka little endian/Least Significant Byte first UUID without hyphens".
     @constant                  BinaryUUID                      "The GUUID is a Binary UUID, aka big endian   /Most  Significant Byte First UUID without hypthns".
     */
    typedef enum GUUIDTypes {
                                UnknownGUUID                    = 0,
                                GUIDString                      = 1,
                                UUIDString                      = 2,
                                BinaryGUID                      = 3,
                                BinaryUUID                      = 4,
    } GUUIDTypes;
    
    /*!
     @enum                      UnaryTypes
     @constant                  CountUnary                      "Supports only positive integers (including zero), The stop bit is excluded from the count".
     @constant                  TruncatedCountUnary             "Supports only positive integers (excluding zero), The stop bit is included in the count".
     @constant                  WholeUnary                      "Supports all the whole integers including zero and negatives (up to 2^63 -1 anyway)".
     */
    typedef enum UnaryTypes {
                                UnknownUnary                    = 0,
                                CountUnary                      = 1,
                                TruncatedCountUnary             = 2,
                                WholeUnary                      = 3,
    } UnaryTypes;
    
    /*!
     @abstract                                                  "Compares GUUIDs for equilivence, GUUID1 and 2 HAVE to be the same type".
     @param                     GUUID1                          "Pointer to a GUUID to be compared".
     @param                     GUUID2                          "Pointer to a GUUID to be compared".
     @return                                                    "Returns Yes if GUUID1 and GUUID2 match, No otherwise".
     */
    bool                        CompareGUUIDs(GUUIDTypes GUUIDType, const uint8_t *GUUID1, const uint8_t *GUUID2);
    
    /*!
     @abstract                                                  "Converts a GUUID from one representation to another (String->Binary<-String/UUIG->GUID<-UUID)".
     @param                     InputGUUIDType                  "What type of GUUID are we converting from"?
     @param                     OutputGUUIDType                 "What type of GUUID are we converting to"?
     @return                                                    "Returns the converted GUUID".
     */
    uint8_t                    *ConvertGUUID(GUUIDTypes InputGUUIDType, GUUIDTypes OutputGUUIDType, const uint8_t *GUUID2Convert);
    
    /*!
     @abstract                                                  "Swaps the byte order of a BinaryGUUID or GUUIDString".
     @param                     GUUIDType                       "Is this a GUUIDString or BinaryGUUID"?
     @param                     GUUID2Swap                      "GUUID Pointer to swap".
     @return                                                    "Returns a pointer to a swapped GUUID".
     */
    uint8_t                    *SwapGUUID(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap);
    
    /*!
     @abstract                                                 "Frees a BinaryGUUID aka BinaryGUID/BinaryUUID or GUUIDString, aka GUIDString/UUIDString".
     */
    void                        GUUID_Deinit(uint8_t *GUUID);
    /* GUUID */
    
    
    /* BitBuffer */
    /*!
     @typedef                   BitBuffer
     @abstract                                                  "Contains variables and a pointer to a buffer for reading and writing bits".
     @constant                  NumBits                         "The number of bits in the buffer".
     @constant                  BitOffset                       "The number of bits previously read/written".
     @constant                  Buffer                          "A pointer to an unsigned byte buffer".
     */
    typedef struct              BitBuffer                       BitBuffer;
    
    /*!
     @abstract                                                  "Initializes a BitBuffer structure".
     @param                     BitBufferSize                   "Number of bytes to create BitBuffer with".
     @return                                                    "Returns a pointer to said BitBuffer structure".
     */
    BitBuffer                  *BitBuffer_Init(const uint64_t BitBufferSize);
    
    /*!
     @abstract                                                  "Gets the size of the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @return                                                    "Returns the number of bits the buffer can hold max".
     */
    uint64_t                    BitBuffer_GetSize(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Gets the offset of the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @return                                                    "Returns the position offset from the start of BitBuffer in bits".
     */
    uint64_t                    BitBuffer_GetPosition(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Tells if the stream/buffer is byte aligned or not".
     @remark                                                    "Checks the stream is aligned on an BytesOfAlignment boundary, not that there are X bits of padding".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @param                     BytesOfAlignment                "Are you trying to see if it's aligned to a byte, short, word, etc alignment? Specify in number of bytes".
     @return                                                    "Returns whether the BitBuffer is aligned on a multiple of BytesOfAlignment".
     */
    bool                        BitBuffer_IsAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                                                  "Aligns bits for multi-byte alignment".
     @remark                                                    "Aligns the stream on a BytesOfAlignment boundary, it does NOT add (BytesOfAlignment * 8) bits of padding".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     BytesOfAlignment                "Align BitB to X byte boundary".
     */
    void                        BitBuffer_Align(BitBuffer *BitB, const uint8_t BytesOfAlignment);
    
    /*!
     @abstract                                                  "Seeks Forwards and backwards in BitBuffer".
     @remark                                                    "To seek backwards just use a negative number, to seek forwards positive".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Skip                       "The number of bits to skip".
     */
    void                        BitBuffer_Skip(BitBuffer *BitB, const int64_t Bits2Skip);
    
    /*!
     @abstract                                                  "Peeks (reads but without recording that it's been read) bits from BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to peek from the BitBuffer".
     */
    uint64_t                    PeekBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Peek);
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer".
     */
    uint64_t                    ReadBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Read);
    
    /*!
     @abstract                                                  "Reads a BinaryGUUID/GUUIDString from the BitBuffer".
     @param                     GUUIDType                       "The type of GUUID to read".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @return                                                    "Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes".
     */
    uint8_t                    *ReadGUUID(GUUIDTypes GUUIDType, BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
    uint64_t                    ReadUnary(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields from the BitBuffer".
     @param                     ByteOrder                       "What byte order should the bits be in the BitBuffer"?
     @param                     BitOrder                        "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     */
    uint64_t                    ReadExpGolomb(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit);
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitOrder                        "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "Contains the data to write into the BitBuffer".
     */
    void                        WriteBits(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes unary encoded bits to the BitBuffer".
     @param                     ByteOrder                       "What byte order should the bits be in the BitBuffer"?
     @param                     BitOrder                        "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     UnaryBits2Write                 "Value to be written as Unary encoded".
     */
    void                        WriteUnary(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const uint8_t UnaryBits2Write);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields to the BitBuffer".
     @param                     ByteOrder                       "What byte order should the bits be in the BitBuffer"?
     @param                     BitOrder                        "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     @param                     Field2Write                     "Value to be encoded as Exp-Golomb and written".
     */
    void                        WriteExpGolomb(BitIOBitByteOrders ByteOrder, BitIOBitByteOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const int64_t Field2Write);
    
    /*!
     @abstract                                                  "Writes a GUUID to the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @param                     GUUID2Write                     "Pointer to the GUUID you want to write".
     */
    void                        WriteGUUID(GUUIDTypes GUUIDType, BitBuffer *BitB, const uint8_t *GUUID2Write);
    
    /*!
     @abstract                                                  "Deallocates the instance of BitBuffer pointed to by BitB".
     @param                     BitB                            "BitBuffer Pointer you want to free".
     */
    void                        BitBuffer_Deinit(BitBuffer *BitB);
    /* BitBuffer */
    
    /* BitInput */
    /*!
     @typedef                   BitInput
     @abstract                                                  "Contains File/Socket pointers for reading to a BitBuffer".
     @constant                  FileType                        "Is this BitInput for a file or socket"?
     @constant                  File                            "Input File/Socket to read into a BitBuffer".
     @constant                  Socket                          "Socket number".
     @constant                  FileSize                        "Size of the File in bytes".
     @constant                  FilePosition                    "Current byte in the file".
     @constant                  FileSpecifierNum                "Which file are we currently on?".
     */
    typedef struct              BitInput                        BitInput;
    
    /*!
     @abstract                                                  "Initializes a BitInput structure".
     @return                                                    "Returns a pointer to said BitInput structure".
     */
    BitInput                   *BitInput_Init(void);
    
    /*!
     @abstract                                                  "Reads Bytes2Read into a BitBuffer from BitInput".
     @remark                                                    "If the BitBuffer Pointer is not new, all the old contents will be lost".
     @param                     BitI                            "BitInput Pointer".
     @param                     Buffer2Read                     "BitBuffer Pointer".
     @param                     Bytes2Read                      "The number of bytes to read from the BitInput into the BitBuffer".
     */
    void                        BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const int64_t Bytes2Read);
    
    /*!
     @abstract                                                  "Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
     @param                     BitI                            "BitInput Pointer".
     @param                     Path2Open                       "Path to the input file to open".
     */
    void                        BitInput_OpenFile(BitInput *BitI, const UTF8 *Path2Open);
    
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
     @abstract                                                  "Computes the number of bytes left in the file".
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the number of bytes left in the file".
     */
    fpos_t                      BitInput_BytesRemainingInFile(BitInput *BitI);
    
    /*!
     @abstract                                                  "Gets the size of the file pointed to by BitI"
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the value in BitI->FileSize if it exists".
     */
    fpos_t                      BitInput_GetFileSize(BitInput *BitI);
    
    /*!
     @abstract                                                  "Gets the position of the Input file from the start".
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the position of the file in bytes from the beginning"
     */
    fpos_t                      BitInput_GetFilePosition(BitInput *BitI);
    
    /*!
     @abstract                                                  "Deallocates BitInput".
     @remark                                                    "For use when changing files, or exiting the program".
     @param                     BitI                            "BitInput Pointer you want to free".
     */
    void                        BitInput_Deinit(BitInput *BitI);
    /* BitInput */
    
    /* BitOutput */
    /*!
     @typedef                   BitOutput
     @abstract                                                  "Contains File/Socket pointers for writing from a BitBuffer".
     @constant                  FileType                        "Is this BitOutput for a file or socket"?
     @constant                  File                            "Input File/Socket to write a BitBuffer into".
     @constant                  Socket                          "Socket number".
     @constant                  FilePosition                    "Current byte in the file".
     @constant                  FileSpecifierNum                "Which file are we currently on?".
     */
    typedef struct              BitOutput                       BitOutput;
    
    /*!
     @abstract                                                  "Initializes a BitOutput structure".
     @return                                                    "Returns a pointer to said BitOutput structure".
     */
    BitOutput                  *BitOutput_Init(void);
    
    /*!
     @abstract                                                  "Opens an output file, pointed to by OutputSwitch in CMD and stores the resulting pointer in BitO->File".
     @param                     BitO                            "BitOutput Pointer".
     @param                     Path2Open                       "Path to the output file to open".
     */
    void                        BitOutput_OpenFile(BitOutput *BitO, UTF8 *Path2Open);
    
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
     @abstract                                                  "Writes a BitBuffer to a file, kinda shitty tho".
     @param                     BitO                            "BitOutput Pointer to write the buffer to".
     @param                     Buffer2Write                    "The buffer to be written to the output file".
     @param                     Bytes2Write                     "The number of bytes from the buffer to write to the file".
     */
    void                        BitOutput_WriteBitBuffer(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write);
    
    /*!
     @abstract                                                  "Deallocates the instance of BitOutput pointed to by BitI".
     @remark                                                    "For use when changing files, or exiting the program".
     @param                     BitO                            "BitOutput Pointer you want to free".
     */
    void                        BitOutput_Deinit(BitOutput *BitO);
    /* BitOutput */
    
#ifdef   __cplusplus
}
#endif

#endif   /* LIBBITIO_BitIO_H */
