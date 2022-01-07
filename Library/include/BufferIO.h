/*!
 @header          BufferIO.h
 @author          Marcus Johnson
 @copyright       2015+
 @version         1.0.0
 @brief           This header contains code for reading and writing buffers, and utilities to manage them.
 @todo            Maybe BufferIO should be renamed ArrayIO?
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
     @constant     UnaryType_Unspecified           Invalid.
     @constant     UnaryType_Count                 Supports whole numbers (including zero).
     @constant     UnaryType_Truncated             Supports counting numbers (excluding zero).
     @constant     UnaryType_Whole                 Supports all the integers including zero and negatives (up to 2^63 anyway).
     */
    typedef enum BufferIO_UnaryTypes {
                   UnaryType_Unspecified           = 0,
                   UnaryType_Count                 = 1,
                   UnaryType_Truncated             = 2,
                   UnaryType_Whole                 = 3,
    } BufferIO_UnaryTypes;

    /*!
     @enum         BufferIO_UnaryTerminators
     @constant     UnaryTerminator_Zero            The stop bit is 0.
     @constant     UnaryTerminator_One             The stop bit is 1.
     */
    typedef enum BufferIO_UnaryTerminators {
                   UnaryTerminator_Zero            = 0,
                   UnaryTerminator_One             = 1,
    } BufferIO_UnaryTerminators;

    /*!
     @enum        BufferIO_ByteOrders
     @constant    ByteOrder_Unspecified            Invalid/Native for Strings.
     @constant    ByteOrder_LSByteIsNearest        The least significant byte is the closest to the current offset; previously LSByte.
     @constant    ByteOrder_LSByteIsFarthest       The least significant byte is the farthest from the current offset; previously MSByte.
     */
    typedef enum BufferIO_ByteOrders {
                  ByteOrder_Unspecified            = 0,
                  ByteOrder_LSByteIsNearest        = 1,
                  ByteOrder_LSByteIsFarthest       = 2,
    } BufferIO_ByteOrders;

    /*!
     @enum         BufferIO_BitOrders
     @constant     BitOrder_Unspecified            Invalid bit order.
     @constant     BitOrder_LSBitIsNearest         The least significant byte is the closest to the current offset; previously LSBit.
     @constant     BitOrder_LSBitIsFarthest        The least significant byte is the farthest from the current offset; previously MSBit.
     */
    typedef enum BufferIO_BitOrders {
                   BitOrder_Unspecified            = 0,
                   BitOrder_LSBitIsNearest         = 1,
                   BitOrder_LSBitIsFarthest        = 2,
    } BufferIO_BitOrders;

    /*!
     @enum         BufferIO_StringTerminators
     @constant     StringTerminator_NULL           Write the NULL Terminator.
     @constant     StringTerminator_Sized          Do not write the null terminaotr, there's a size field.
     */
    typedef enum BufferIO_StringTerminators {
                   StringTerminator_NULL           = 1,
                   StringTerminator_Sized          = 2,
    } BufferIO_StringTerminators;

    /*!
     @enum         BufferIO_CRCPolynomials
     @constant     CRCPolynomial_Unspecified       Invalid Polynomial.
     @constant     CRCPolynomial_IEEE802_3         CRC32 Polynomial used in PNG, MPEG-4, Zip, etc.
     */
    typedef enum BufferIO_CRCPolynomials : uint64_t {
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
     @abstract                                     Reads unary encoded fields from the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        ByteOrder                       What byte order are the bits to be read?
     @param        BitOrder                        What bit order are the bits to be read?
     @param        UnaryType                       What type of Unary coding are we reading?
     @param        UnaryTerminator                 Is the stop bit a one or a zero?
     */
    uint64_t       BitBuffer_ReadUnary(BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, BufferIO_UnaryTypes UnaryType, BufferIO_UnaryTerminators UnaryTerminator);

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
     @param        StringSize                      Size of the string in CodeUnits, not counting an optional null.
     */
    UTF8          *BitBuffer_ReadUTF8(BitBuffer *BitB, size_t StringSize);

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
     @param        StringSize                      Size of the string in CodeUnits, not counting an optional null.
     */
    UTF16         *BitBuffer_ReadUTF16(BitBuffer *BitB, size_t StringSize);

    /*!
     @abstract                                     Reads a BinaryGUUID/GUUIDString from the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        GUUIDType                       The type of GUUID to read.
     @return                                       Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BinaryGUUIDSize or BinaryGUUIDSize bytes.
     */
    uint8_t       *BitBuffer_ReadGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType);

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
    void           BitBuffer_WriteUTF8(BitBuffer *BitB, ImmutableString_UTF8 String2Write, BufferIO_StringTerminators WriteType);

    /*!
     @abstract                                     Writes a UTF-16 encoded string to the BitBuffer.
     @remark                                       Convert the string to UTF16BOM_BE or UTF16BOM_LE first.
     @param        BitB                            BitBuffer Pointer.
     @param        String2Write                    The string to write to the BitBuffer.
     @param        WriteType                       Should the NULL terminator be written?
     */
    void           BitBuffer_WriteUTF16(BitBuffer *BitB, ImmutableString_UTF16 String2Write, BufferIO_StringTerminators WriteType);

    /*!
     @abstract                                     Writes a GUUID to the BitBuffer.
     @param        BitB                            BitBuffer Pointer.
     @param        GUUID2Write                     Pointer to the GUUID you want to write.
     */
    void           BitBuffer_WriteGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType, uint8_t *GUUID2Write);

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
