#include "../include/BitIOMacros.h"
#include "../include/StringIO.h"

#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(push, 0)
#endif
#include <stdbool.h> /* Included for the bool type */
#include <stdint.h>  /* Included for the u/intX_t types */
#include <stdio.h>   /* Included for fpos_t */
#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(pop)
#endif

#pragma  once

#ifndef  LIBBITIO_BitIO_H
#define  LIBBITIO_BitIO_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*!
     @header    BitIO.h
     @author    Marcus Johnson
     @copyright 2015 - 2017, Marcus Johnson
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
                                BitIONULLString                 = 0x0,
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
     @enum                      UnaryTypes
     @constant                  CountUnary                      "Supports only positive integers (excluding zero), Truncates the last bit before the stop bit".
     @constant                  WholeUnary                      "Supports all the whole integers including zero and negatives (up to 2^63 -1 anyway)".
     */
    typedef enum UnaryTypes {
                                CountUnary                      = 1,
                                WholeUnary                      = 2,
    } UnaryTypes;
    
    /* Pure GUUID */
    /*!
     @abstract                                                  "Compares GUUIDStrings for equilivence".
     @param                     GUUIDString1                    "Pointer to GUUIDString or BinaryGUUID to be compared".
     @param                     GUUIDString2                    "Pointer to GUUIDString or BinaryGUUID to be compared".
     @return                                                    "Returns Yes if GUUID1 and GUUID2 match, No otherwise".
     */
    bool                        CompareGUUIDStrings(const uint8_t GUUIDString1[BitIOGUUIDStringSize], const uint8_t GUUIDString2[BitIOGUUIDStringSize]);
    
    /*!
     @abstract                                                  "Compares BinaryGUUIDs for equilivence".
     @param                     BinaryGUUID1                    "Pointer to GUUIDString or BinaryGUUID to be compared".
     @param                     BinaryGUUID2                    "Pointer to GUUIDString or BinaryGUUID to be compared".
     @return                                                    "Returns Yes if GUUID1 and GUUID2 match, No otherwise".
     */
    bool                        CompareBinaryGUUIDs(const uint8_t BinaryGUUID1[BitIOBinaryGUUIDSize], const uint8_t BinaryGUUID2[BitIOBinaryGUUIDSize]);
    
#define GUUID_Compare(GUUIDType,GUUID1,GUUID2)_Generic((GUUIDType),GUUIDString_t:CompareGUUIDStrings,BinaryGUUID_t:CompareGUUIDStrings)(GUUID1,GUUID2)
    
    
    /*!
     @abstract                                                  "Converts a GUID/UUIDString to a BinaryGUID/UUID".
     @param                     GUUIDString                     "Pointer to a GUID/UUIDString".
     @return                                                    "Returns a pointer to a BinaryGUID/UUID".
     */
    uint8_t                    *ConvertGUUIDString2BinaryGUUID(const uint8_t GUUIDString[BitIOGUUIDStringSize]);
    
    /*!
     @abstract                                                  "Converts a BinaryGUID/UUID to a GUID/UUIDString".
     @param                     BinaryGUUID                     "Pointer to a BinaryGUUID".
     @return                                                    "Returns a pointer to a GUUIDString".
     */
    uint8_t                    *ConvertBinaryGUUID2GUUIDString(const uint8_t BinaryGUUID[BitIOBinaryGUUIDSize]);
    
    /*!
     @abstract                                                  "Converts a GUIDString to a UUIDString or vice versa"
     */
    uint8_t                    *SwapGUUIDString(const uint8_t GUUIDString[BitIOGUUIDStringSize]);
    
    /*!
     @abstract                                                  "Converts a BinaryGUID to a BinaryUUID or vice versa".
     */
    uint8_t                    *SwapBinaryGUUID(const uint8_t BinaryGUUID[BitIOBinaryGUUIDSize]);
    
    /*!
     @abstract                                                  "Converts a BinaryGUID/UUID to a BinaryUUID/GUID".
     @param                     GUUIDType                       "Is this a GUUIDString or BinaryGUUID"?
     @param                     GUUID2Swap                      "GUUID Pointer to swap".
     @return                                                    "Returns a pointer to a swapped GUUID".
     */
#define GUUID_Swap(GUUIDType,GUUID2Swap)_Generic((GUUIDType),BinaryUUID_t:SwapBinaryGUUID,BinaryGUID_t:SwapBinaryGUUID,UUIDString_t:SwapGUUIDString,GUIDString_t:SwapGUUIDString)(GUUID2Swap)
    
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
     @abstract                                                  "Peeks (read but without recording that it's been read) bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to Peek from the BitBuffer, as LSByte, LSBit format, to the runtime byte/bit order".
     */
    uint64_t                    PeekBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    
    /*!
     @abstract                                                  "Peeks (read but without recording that it's been read) bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to Peek from the BitBuffer, as LSByte, MSBit format, to the runtime byte/bit order".
     */
    uint64_t                    PeekBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    
    /*!
     @abstract                                                  "Peeks (read but without recording that it's been read) bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to Peek from the BitBuffer, as MSByte, LSBit format, to the runtime byte/bit order".
     */
    uint64_t                    PeekBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    
    /*!
     @abstract                                                  "Peeks (read but without recording that it's been read) bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to Peek from the BitBuffer, as MSByte, MSBit format, to the runtime byte/bit order".
     */
    uint64_t                    PeekBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek);
    
    /*!
     @abstract                                                  "Peeks (read but without recording that it's been read) bits from BitBuffer".
     @param                     BitBByteOrder                   "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitBBitOrder                    "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to peek from the BitBuffer".
     */
#define PeekBits(BitBByteOrder,BitBBitOrder,BitB,Bits2Peek)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:PeekBitsAsLSByteLSBit,BitBMSBit_t:PeekBitsAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:PeekBitsAsMSByteLSBit,BitBMSBit_t:PeekBitsAsMSByteMSBit))(BitB,Bits2Peek)
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer, as LSByte, LSBit format, to the runtime byte/bit order".
     */
    uint64_t                    ReadBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer, as LSByte, MSBit format, to the runtime byte/bit order".
     */
    uint64_t                    ReadBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer, as MSByte, LSBit format, to the runtime byte/bit order".
     */
    uint64_t                    ReadBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer, as MSByte, MSBit format, to the runtime byte/bit order".
     */
    uint64_t                    ReadBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read);
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     BitBByteOrder                   "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitBBitOrder                    "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer".
     */
#define ReadBits(BitBByteOrder,BitBBitOrder,BitB,Bits2Read)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadBitsAsLSByteLSBit,BitBMSBit_t:ReadBitsAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadBitsAsMSByteLSBit,BitBMSBit_t:ReadBitsAsMSByteMSBit))(BitB,Bits2Read)
    
    /*!
     @abstract                                                  "Reads a BinaryGUUID/GUUIDString from the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @return                                                    "Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes, there is no NULL terminator".
     */
    uint8_t                    *ReadGUUIDAsUUIDString(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Reads a BinaryGUUID/GUUIDString from the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @return                                                    "Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes, there is no NULL terminator".
     */
    uint8_t                    *ReadGUUIDAsGUIDString(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Reads a BinaryGUUID/GUUIDString from the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @return                                                    "Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes".
     */
    uint8_t                    *ReadGUUIDAsBinaryUUID(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Reads a BinaryGUUID/GUUIDString from the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @return                                                    "Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes".
     */
    uint8_t                    *ReadGUUIDAsBinaryGUID(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Reads a BinaryGUUID/GUUIDString from the BitBuffer".
     @param                     GUUIDType                       "GUUIDType is BitIOGUUIDString or BitIOBinaryGUUID".
     @param                     ByteOrder                       "ByteOrder is BitIOLSByte or BitIOMSByte".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @return                                                    "Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BitIOBinaryGUUIDSize or BitIOGUUIDStringSize bytes".
     */
#define ReadGUUID(GUUIDType,ByteOrder,BitB)_Generic((GUUIDType),GUUIDString_t:_Generic((ByteOrder),BitBLSByte_t:ReadGUUIDAsGUIDString,BitBMSByte_t:ReadGUUIDAsUUIDString),BinaryGUUID_t:_Generic((ByteOrder),BitBLSByte_t:ReadGUUIDAsBinaryGUID,BitBMSByte_t:ReadGUUIDAsBinaryUUID))(BitB)
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
    uint64_t                    ReadUnaryAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
    uint64_t                    ReadUnaryAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
    uint64_t                    ReadUnaryAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
    uint64_t                    ReadUnaryAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     BitBByteOrder                   "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitBBitOrder                    "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
#define ReadUnary(BitBByteOrder,BitBBitOrder,BitB,UnaryType,StopBit)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadUnaryAsLSByteLSBit,BitBMSBit_t:ReadUnaryAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadUnaryAsMSByteLSBit,BitBMSBit_t:ReadUnaryAsMSByteMSBit))(BitB,UnaryType,StopBit)
    
    /*!
     @abstract                                                  "Reads Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     */
    uint64_t                    ReadExpGolombAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     */
    uint64_t                    ReadExpGolombAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     */
    uint64_t                    ReadExpGolombAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     */
    uint64_t                    ReadExpGolombAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitBByteOrder                   "What byte order should the bits be in the BitBuffer"?
     @param                     BitBBitOrder                    "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     */
#define ReadExpGolomb(BitBByteOrder,BitBBitOrder,BitB,UnaryType,StopBit)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadExpGolombAsLSByteLSBit,BitBMSBit_t:ReadExpGolombAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:ReadExpGolombAsMSByteLSBit,BitBMSBit_t:ReadExpGolombAsMSByteMSBit))(BitB,UnaryType,StopBit)
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "Contains the data to write into the BitBuffer".
     */
    void                        WriteBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "Contains the data to write into the BitBuffer".
     */
    void                        WriteBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "Contains the data to write into the BitBuffer".
     */
    void                        WriteBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer, from the runtime byte/bit order to the specified byte/bit order".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "Contains the data to write into the BitBuffer".
     */
    void                        WriteBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer".
     @param                     BitBByteOrder                   "What byte order are the bits in the BitBuffer for this field stored as"?
     @param                     BitBBitOrder                    "What bit order are the bits in the BitBuffer for this field stored as"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "Contains the data to write into the BitBuffer".
     */
#define WriteBits(BitBByteOrder,BitBBitOrder,BitB,NumBits2Write,Bits2Write)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteBitsAsLSByteLSBit,BitBMSBit_t:WriteBitsAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteBitsAsMSByteLSBit,BitBMSBit_t:WriteBitsAsMSByteMSBit))(BitB,NumBits2Write,Bits2Write)
    
    /*!
     @abstract                                                  "Writes unary encoded fields to the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     Field2Write                     "Value to be written as Unary encoded".
     */
    void                        WriteUnaryAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, uint8_t Field2Write);
    
    /*!
     @abstract                                                  "Writes unary encoded fields to the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     Field2Write                     "Value to be written as Unary encoded".
     */
    void                        WriteUnaryAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, uint8_t Field2Write);
    
    /*!
     @abstract                                                  "Writes unary encoded fields to the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     Field2Write                     "Value to be written as Unary encoded".
     */
    void                        WriteUnaryAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, uint8_t Field2Write);
    
    /*!
     @abstract                                                  "Writes unary encoded fields to the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     Field2Write                     "Value to be written as Unary encoded".
     */
    void                        WriteUnaryAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, uint8_t Field2Write);
    
    /*!
     @abstract                                                  "Writes unary encoded bits to the BitBuffer".
     @param                     BitBByteOrder                   "What byte order should the bits be in the BitBuffer"?
     @param                     BitBBitOrder                    "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     Field2Write                     "Value to be written as Unary encoded".
     */
#define WriteUnary(BitBByteOrder,BitBBitOrder,BitB,UnaryType,StopBit,Field2Write)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteUnaryAsLSByteLSBit,BitBMSBit_t:WriteUnaryAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteUnaryAsMSByteLSBit,BitBMSBit_t:WriteUnaryAsMSByteMSBit))(BitB,UnaryType,StopBit,Field2Write)
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     Field2Write                     "Value to be encoded as Exp-Golomb and written".
     */
    void                        WriteExpGolombAsLSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, const int64_t Field2Write);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     @param                     Field2Write                     "Value to be encoded as Exp-Golomb and written".
     */
    void                        WriteExpGolombAsLSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, const int64_t Field2Write);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     @param                     Field2Write                     "Value to be encoded as Exp-Golomb and written".
     */
    void                        WriteExpGolombAsMSByteLSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, const int64_t Field2Write);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     @param                     Field2Write                     "Value to be encoded as Exp-Golomb and written".
     */
    void                        WriteExpGolombAsMSByteMSBit(BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit, const int64_t Field2Write);
    
    /*!
     @abstract                                                  "Writes Exp-Golomb encoded fields to the BitBuffer".
     @param                     BitBByteOrder                   "What byte order should the bits be in the BitBuffer"?
     @param                     BitBBitOrder                    "What bit order should the bits be in the BitBuffer"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "What bit is the stop bit"?
     @param                     Field2Write                     "Value to be encoded as Exp-Golomb and written".
     */
#define WriteExpGolomb(BitBByteOrder,BitBBitOrder,BitB,UnaryType,StopBit,Field2Write)_Generic((BitBByteOrder),BitBLSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteExpGolombAsLSByteLSBit,BitBMSBit_t:WriteExpGolombAsLSByteMSBit),BitBMSByte_t:_Generic((BitBBitOrder),BitBLSBit_t:WriteExpGolombAsMSByteLSBit,BitBMSBit_t:WriteExpGolombAsMSByteMSBit))(BitB,UnaryType,StopBit,Field2Write)
    
    /* BitBuffer GUUID */
    
    /*!
     @abstract                                                  "Writes a BinaryGUUID/GUUIDString to the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @param                     UUIDString                      "Pointer to the UUIDString you want to write".
     */
    void                        WriteGUUIDAsUUIDString(BitBuffer *BitB, const uint8_t *UUIDString);
    
    /*!
     @abstract                                                  "Writes a BinaryGUUID/GUUIDString to the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @param                     GUIDString                      "Pointer to the UUIDString you want to write".
     */
    void                        WriteGUUIDAsGUIDString(BitBuffer *BitB, const uint8_t *GUIDString);
    
    /*!
     @abstract                                                  "Writes a BinaryGUUID/GUUIDString to the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @param                     BinaryUUID                      "Pointer to the UUIDString you want to write".
     */
    void                        WriteGUUIDAsBinaryUUID(BitBuffer *BitB, const uint8_t *BinaryUUID);
    
    /*!
     @abstract                                                  "Writes a BinaryGUUID/GUUIDString to the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @param                     BinaryGUID                      "Pointer to the UUIDString you want to write".
     */
    void                        WriteGUUIDAsBinaryGUID(BitBuffer *BitB, const uint8_t *BinaryGUID);
    
#define WriteGUUID(GUUIDType,ByteOrder,BitB,GUUID2Write)_Generic((GUUIDType),GUUIDString_t:_Generic((ByteOrder),BitBLSByte_t:WriteGUUIDAsGUIDString,BitBMSByte_t:WriteGUUIDAsUUIDString),BinaryGUUID_t:_Generic((ByteOrder),BitBLSByte_t:WriteGUUIDAsBinaryGUID,BitBMSByte_t:WriteGUUIDAsBinaryUUID))(BitB,GUUID2Write)
    
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
    void                        BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const uint64_t Bytes2Read);
    
    /*!
     @abstract                                                  "Opens an input file, pointed to by InputSwitch in CMD and stores the resulting pointer in BitI->File".
     @param                     BitI                            "BitInput Pointer".
     @param                     Path2Open                       "Path to the input file to open".
     */
    void                        BitInput_OpenFile(BitInput *BitI, const UTF8String Path2Open);
    
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
    void                        BitOutput_OpenFile(BitOutput *BitO, const UTF8String Path2Open);
    
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
