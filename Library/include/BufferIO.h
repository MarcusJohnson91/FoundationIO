/*!
 @header:                  BufferIO.h
 @author:                  Marcus Johnson
 @copyright:               2015+
 @version:                 1.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains code for reading and writing buffers, and utilities to manage them.
 */

#pragma once

#ifndef FoundationIO_BufferIO_H
#define FoundationIO_BufferIO_H

#include "TextIO/TextIOTypes.h" /* Included for Text types */
#include "GUUID.h"              /* Included for GUUIDTypes */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @enum         BufferIO_UnaryTypes
     Natural number 1 to +Infinity
     Whole number: 0 to +Infinity

     @constant     UnaryType_Unspecified           Invalid.
     @constant     UnaryType_Natural               Supports counting numbers (excluding zero).
     @constant     UnaryType_Whole                 Supports whole numbers (including zero).
     @constant     UnaryType_Truncated             Supports all the integers including zero and negatives (up to 2^63 anyway).
     */
    typedef enum UnaryTypes : uint8_t {
                   UnaryType_Unspecified           = 0,
                   UnaryType_Natural               = 1,
                   UnaryType_Whole                 = 3,
                   UnaryType_Truncated             = 2,
    } UnaryTypes;

    /*!
     @enum         UnaryTerminators
     @constant     UnaryTerminator_Zero            The stop bit is 0.
     @constant     UnaryTerminator_One             The stop bit is 1.
     */
    typedef enum UnaryTerminators : uint8_t {
                   UnaryTerminator_Zero            = 0,
                   UnaryTerminator_One             = 1,
    } UnaryTerminators;

    /*!
     @enum        ByteOrders
     @constant    ByteOrder_Unspecified            Invalid/Native for Strings.
     @rename      ByteOrder_LSByteIsNearest        Is now ByteOrder_Right2Left aka LSByte aka LSByteFirst aka little endian.
     @rename      ByteOrder_LSByteIsFarthest       Is now ByteOrder_Left2Right  aka MSByte aka MSByteFirst aka big endian.
     @constant    ByteOrder_Right2Left             The least significant byte is the closest to the current offset.
     @constant    ByteOrder_Left2Right             The least significant byte is the farthest from the current offset.
     */
    typedef enum ByteOrders : uint8_t {
                  ByteOrder_Unspecified          = 0,
                  ByteOrder_Right2Left           = 1,
                  ByteOrder_Left2Right           = 2,
    } ByteOrders;

    /*!
     @enum         BitOrders
     @constant     BitOrder_Unspecified            Invalid bit order.
     @rename       BitOrder_LSBitIsNearest         Is now BitOrder_Right2Left aka LSByte aka LSByteFirst aka little endian.
     @rename       BitOrder_LSBitIsFarthest        Is now BitOrder_Left2Right  aka MSByte aka MSByteFirst aka big endian.
     @constant     BitOrder_LSBitIsNearest         The least significant byte is the closest to the current offset; previously LSBit.
     @constant     BitOrder_Left2Right             The least significant byte is the farthest from the current offset; previously MSBit.
     */
    typedef enum BitOrders : uint8_t {
                   BitOrder_Unspecified            = 0,
                   BitOrder_Right2Left             = 1,
                   BitOrder_Left2Right             = 2,
    } BitOrders;

    /*!
     @enum         StringTerminators
     @constant     StringTerminator_NULL           Write the NULL Terminator.
     @constant     StringTerminator_Sized          Do not write the null terminaotr, there's a size field.
     */
    typedef enum StringTerminators : uint8_t {
                   StringTerminator_Unspecified    = 0,
                   StringTerminator_NULL           = 1,
                   StringTerminator_Sized          = 2,
    } StringTerminators;

    /*!
     @enum         CRCPolynomials
     @constant     CRCPolynomial_Unspecified       Invalid Polynomial.
     @constant     CRCPolynomial_IEEE802_3         CRC32 Polynomial used in PNG, MPEG-4, Zip, etc.
     */
    typedef enum CRCPolynomials : uint64_t {
                   CRCPolynomial_Unspecified       = 0,
                   CRCPolynomial_IEEE802_3         = 0xEDB88320,
    } BufferIO_CRCPolynomials;

    /*!
     @typedef      BitBuffer
     @abstract                                     Contains variables and a pointer to a buffer for reading and writing bits.
     */
    typedef struct BitBuffer                       BitBuffer;

    /*!
     @typedef      InsecurePRNG
     @abstract                                     Forward declaration from CryptographyIO.
     */
    typedef struct InsecurePRNG                    InsecurePRNG;

    /* BitBuffer */
    /*!
     @abstract                                     Creates a BitBuffer.
     @param        BitBufferSize                   The number of bytes to create the BitBuffer with.
     @return                                       Returns a pointer to the BitBuffer.
     */
    BitBuffer     *BitBuffer_Init(size_t BitBufferSize);

    /*!
     @abstract                                     Gets the size of the BitBuffer in bits.
     @param        BitB                            BitBuffer Pointer.
     @return                                       Returns the number of bits the buffer can hold max.
     */
    size_t         BitBuffer_GetSize(BitBuffer *BitB);

    /*!
     @abstract                                     Sets the size of the BitBuffer in bits.
     @param        BitB                            BitBuffer Pointer.
     @param        NumBits                         The size of the BitBuffer in Bits.
     */
    void           BitBuffer_SetSize(BitBuffer *BitB, size_t NumBits);

    /*!
     @abstract                                     Gets the offset of the BitBuffer in bits.
     @param        BitB                            BitBuffer Pointer.
     @return                                       Returns the offset from the start of the BitBuffer in bits.
     */
    size_t         BitBuffer_GetPosition(BitBuffer *BitB);

    /*!
     @abstract                                     Sets the offset of the BitBuffer in bits.
     @param        BitB                            BitBuffer Pointer.
     @param        OffsetInBits                    The offset of the BitBuffer in Bits.
     */
    void           BitBuffer_SetPosition(BitBuffer *BitB, size_t OffsetInBits);

    /*!
     @abstract                                     Gets the number of free bits in the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @return                                       Returns the offset from the start of BitBuffer in bits - the current offset.
     */
    size_t         BitBuffer_GetBitsFree(BitBuffer *BitB);

    /*!
     @abstract                                     Gets a pointer to BitBuffer's array.
     @remark                                       This is a low level function, only really useful for implementing say a Huffman reader/writer.
     @param        BitB                            BitBuffer Pointer.
     @return                                       Returns the pointer for BitBuffer's array.
     */
    uint8_t        *BitBuffer_GetArray(BitBuffer *BitB);

    /*!
     @abstract                                     Sets a pointer to BitBuffer's array.
     @remark                                       This is a low level function, only really useful for implementing say a Huffman reader/writer.
     @param        BitB                            BitBuffer Pointer.
     @param        Buffer                          Pointer to a byte array to set as BitBuffer's internal array.
     @param        BufferSizeInBytes               The size of Buffer in bytes.
     */
    void           BitBuffer_SetArray(BitBuffer *BitB, uint8_t *Buffer, size_t BufferSizeInBytes);

    /*!
     @abstract                                     Checks if the BitBuffer's offset is aligned to boundary AlignmentSize.
     @remark                                       Alignment means the offset of a BitBuffer is a multiple of AlignmentSize.
     @param        BitB                            BitBuffer Pointer.
     @param        AlignmentSizeInBytes            The number of bytes to check the alignment of.
     @return                                       Returns whether the BitBuffer is aligned to AlignmentSize or not.
     */
    bool           BitBuffer_IsAligned(BitBuffer *BitB, size_t AlignmentSizeInBytes);

    /*!
     @abstract                                     Aligns BitBuffer.
     @remark                                       Zero pads the BitBuffer's offset to a multiple of AlignmentSize.
     @remark                                       We might want to add a parameter to set the padding value, like SHA-3 requires paddng in 512 bit blocks with the first padding bit set to 1 and the rest to 0
     @param        BitB                            BitBuffer Pointer.
     @param        AlignmentSizeInBytes            The alignment size in bytes.
     */
    void           BitBuffer_Align(BitBuffer *BitB, size_t AlignmentSizeInBytes);

    /*!
     @abstract                                     Seeks forwards and backwards in BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        Bits2Seek                       The number of bits to seek.
     */
    void           BitBuffer_Seek(BitBuffer *BitB, ssize_t Bits2Seek);

    /*!
     @abstract                                     Securely erases the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        NewValue                        The value to set each byte to while erasing.
     @return                                       Returns true if the buffe was sucessfully erased, false otherwise
     */
    bool           BitBuffer_Erase(BitBuffer *BitB, uint8_t NewValue);

    /*!
     @abstract                                     Changes the size of an already initialized BitBuffer.
     @remark                                       Resizing a BitBuffer IS NOT destructive.
     @param        BitB                            BitBuffer Pointer to resize.
     @param        NewSizeInBits                   The new size of the internal buffer in bytes.
     */
    void           BitBuffer_Resize(BitBuffer *BitB, size_t NewSizeInBits);

    /*!
     @abstract                                     Copies BitBuffer Source to Destination.
     @remark                                       Is NOT destructive, it will keep any unread data in the buffer.
     @param        Source                          The BitBuffer to copy from.
     @param        Destination                     The BitBuffer to copy to.
     @param        BitStart                        The bit to start copying from (inclusive).
     @param        BitEnd                          The bit to end copying (inclusive).
     */
    void           BitBuffer_Copy(BitBuffer *Source, BitBuffer *Destination, size_t BitStart, size_t BitEnd);

    /*!
     @abstract                                     Reads BitBuffer's Input Stream into BitBuffer's array
     @param        BitB                            The BitBuffer to read to.
     */
    void           BitBuffer_ReadStream(BitBuffer *BitB);

    /*!
     @abstract                                     Writes BitBuffer's array to BitBuffer's Output Stream
     @remark                                       Only writes actually used data.
     @param        BitB                            The BitBuffer to write.
     */
    void           BitBuffer_WriteStream(BitBuffer *BitB);

    /* Ignore clang's warning: shorten-64-to-32 aka Wnarrowing */

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang) || (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#pragma gcc diagnostic push
#pragma gcc diagnostic ignored "-Wnarrowing"
#pragma gcc diagnostic ignored "-Wshorten-64-to-32"
#pragma gcc diagnostic ignored "-Wconversion"
#endif /* PlatformIO_Compiler */

    /*!
     @abstract                                     Peeks (reads but without recording that it's been read) bits from BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        ByteOrder                       What byte order are the bits to be peeked?
     @param        BitOrder                        What bit order are the bits to be peeked?
     @param        Bits2Peek                       The number of bits to peek from the BitBuffer.
     */
    uint64_t       BitBuffer_PeekBits(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, uint8_t Bits2Peek);

    /*!
     @abstract                                     Reads bits from BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        ByteOrder                       What byte order are the bits to be read?
     @param        BitOrder                        What bit order are the bits to be read?
     @param        Bits2Read                       The number of bits to read from the BitBuffer.
     */
    uint64_t       BitBuffer_ReadBits(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, uint8_t Bits2Read);

    /*!
     @abstract                                     Reads Rice encoded bits from BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        ByteOrder                       What byte order are the bits to be read?
     @param        BitOrder                        What bit order are the bits to be read?
     @param        Terminator                      Should we stop at zero or one?
     */
    uint64_t       BitBuffer_ReadRICE(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, BufferIO_UnaryTerminators Terminator);

    /*!
     @abstract                                     Reads unary encoded fields from the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        ByteOrder                       What byte order are the bits to be read?
     @param        BitOrder                        What bit order are the bits to be read?
     @param        UnaryType                       What type of Unary coding are we reading?
     @param        UnaryTerminator                 Is the stop bit a one or a zero?
     */
    uint64_t       BitBuffer_ReadUnary(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, BufferIO_UnaryTypes UnaryType, BufferIO_UnaryTerminators UnaryTerminator);

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsClang) || (PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#pragma gcc diagnostic pop
#endif /* PlatformIO_Compiler */

    /*!
     @abstract                                     Reads data from the BitBuffer until it stops matching the UTF-8 format.
     @remark                                       Does NOT count any potential NULL terminator.
     @param        BitB                            BitBuffer Pointer.
     */
    size_t         BitBuffer_GetUTF8StringSize(BitBuffer *BitB);

    /*!
     @abstract                                     Reads a UTF-8 encoded string from the BitBuffer.
     @remark                                       We read exactly StringSize bytes, you need to account for any null terminators, etc.
     @param        BitB                            BitBuffer Pointer.
     @param        StringSizeInCodeUnits           Size of the string in CodeUnits, not counting an optional null.
     */
    UTF8          *BitBuffer_ReadUTF8(BitBuffer *BitB, size_t StringSizeInCodeUnits);

    /*!
     @abstract                                     Reads data from the BitBuffer until it stops matching the UTF-8 format.
     @remark                                       Does NOT count any potential NULL terminator.
     @param        BitB                            BitBuffer Pointer.
     */
    size_t         BitBuffer_GetUTF16StringSize(BitBuffer *BitB);

    /*!
     @abstract                                     Reads a UTF-16 encoded string from the BitBuffer.
     @remark                                       We read exactly StringSize * 2 bytes, you need to account for any null terminators, etc.
     @param        BitB                            BitBuffer Pointer.
     @param        StringSizeInCodeUnits           Size of the string in CodeUnits, not counting an optional null.
     */
    UTF16         *BitBuffer_ReadUTF16(BitBuffer *BitB, size_t StringSizeInCodeUnits);

    /*!
     @abstract                                     Reads a BinaryGUUID/GUUIDString from the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        GUUIDType                       The type of GUUID to read.
     @return                                       Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BinaryGUUIDSize or BinaryGUUIDSize bytes.
     */
    uint8_t       *BitBuffer_ReadGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType);

    /*
     Ok, so fuck CRCs let's get this dumb shit fuckin done.

     width=16 poly=0x1021 init=0xffff refin=false refout=false xorout=0xffff check=0xd64e residue=0x1d0f name="CRC-16/GENIBUS"
     */

    typedef struct CRCOptions {
        uint64_t Preset; // value to initailize the CRC state machine with.
        uint64_t Polynomial;
        uint64_t XOROutput;
        uint64_t Residue; // The value of the CRC if it worked correctly
        uint8_t  SizeInBits;
        bool     ReflectInput;
        bool     ReflectOutput;
    } CRCOptions;

    /*!
     @abstract                                     Calculates CRC32 from BitBuffer starting at Offset ending at Offset + NumBytes
     @param        BitB                            BitBuffer Pointer.
     @param        OffsetInBits                    The Offset into the BitBuffer.
     @param        NumBytes                        The number of bytes to calculate.
     @return                                       Returns the calculated CRC.
     */
    uint32_t       BitBuffer_CalculateCRC(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, size_t OffsetInBits, size_t NumBytes, uint64_t InitValue, BufferIO_CRCPolynomials Polynomial);

    /*!
     @abstract                                     Runs Adler32 hash over the BitBuffer starting at Offset
     @param        BitB                            The BitBuffer pointer.
     @param        OffsetInBits                    The Offset into the BitBuffer
     @param        NumBytes                        The number of bytes to read
     @return                                       Returns the calculated Adler32.
     */
    uint32_t       BitBuffer_CalculateAdler32(BitBuffer *BitB, size_t OffsetInBits, size_t NumBytes);

    /*!
     @abstract                                     Writes bits to the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        ByteOrder                       What byte order should the Bits2Write be in?
     @param        BitOrder                        What bit order should the Bits2Write be in?
     @param        NumBits2Write                   How many bits from Bits2Write should we write?
     @param        Bits2Write                      The actual data to write.
     */
    void           BitBuffer_WriteBits(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, uint8_t NumBits2Write, uint64_t Bits2Write);

    /*!
     @abstract                                     Writes unary encoded bits to the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        ByteOrder                       What byte order should the Bits2Write be in?
     @param        BitOrder                        What bit order should the Bits2Write be in?
     @param        UnaryType                       What type of Unary coding should we use?
     @param        UnaryTerminator                 Is the stop bit a one or a zero?
     @param        Bits2Write                      The actual data to write.
     */
    void           BitBuffer_WriteUnary(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, BufferIO_UnaryTypes UnaryType, BufferIO_UnaryTerminators UnaryTerminator, uint8_t Bits2Write);

    /*!
     @abstract                                     Writes a UTF-8 encoded string to the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        String2Write                    The string to write to the BitBuffer.
     @param        WriteType                       Should the NULL terminator be written?
     */
    void           BitBuffer_WriteUTF8(BitBuffer *BitB, PlatformIO_Immutable(UTF8 *)  String2Write, BufferIO_StringTerminators WriteType);

    /*!
     @abstract                                     Writes a UTF-16 encoded string to the BitBuffer.
     @remark                                       Convert the string to UTF16BOM_BE or UTF16BOM_LE first.
     @param        BitB                            BitBuffer Pointer.
     @param        String2Write                    The string to write to the BitBuffer.
     @param        WriteType                       Should the NULL terminator be written?
     */
    void           BitBuffer_WriteUTF16(BitBuffer *BitB, PlatformIO_Immutable(UTF16 *) String2Write, BufferIO_StringTerminators WriteType);

    /*!
     @abstract                                     Writes a GUUID to the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        GUUID2Write                     Pointer to the GUUID you want to write.
     */
    void           BitBuffer_WriteGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType, const uint8_t *const GUUID2Write);

    /*!
     @abstract                                     Deletes the BitBuffer.
     @param        BitB                            BitBuffer Pointer you want to deinit.
     */
    void           BitBuffer_Deinit(BitBuffer *BitB);
    /* BitBuffer */

    /* Standard Replacements */
    size_t BufferIO_MemoryCopy8(uint8_t *restrict Destination, const uint8_t *restrict Source, const size_t NumElements2Copy);
    size_t BufferIO_MemoryCopy16(uint64_t *restrict Destination, const uint64_t *restrict Source, const size_t NumElements2Copy);
    size_t BufferIO_MemoryCopy32(uint64_t *restrict Destination, const uint64_t *restrict Source, const size_t NumElements2Copy);
    size_t BufferIO_MemoryCopy64(uint64_t *restrict Destination, const uint64_t *restrict Source, const size_t NumElements2Copy);

    size_t BufferIO_MemorySet8(uint8_t *Array, uint8_t Value2Set, size_t NumElements2Set);
    size_t BufferIO_MemorySet16(uint16_t *Array, uint16_t Value2Set, size_t NumElements2Set);
    size_t BufferIO_MemorySet32(uint32_t *Array, uint16_t Value2Set, size_t NumElements2Set);
    size_t BufferIO_MemorySet64(uint64_t *Array, uint16_t Value2Set, size_t NumElements2Set);
    /* Standard Replacements */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_BufferIO_H */
