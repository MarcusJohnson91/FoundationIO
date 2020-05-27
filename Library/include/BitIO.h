/*!
 @header         BitIO.h
 @author         Marcus Johnson
 @copyright      2015+
 @version        1.0.0
 @brief          This header contains code for reading and writing files and sockets, and utilities to manage them.
 */

#include "PlatformIO.h"               /* Included for Platform Independence macros */
#include "UnicodeIO/UnicodeIOTypes.h" /* Included for UTFX types */

#pragma once

#ifndef FoundationIO_BitIO_H
#define FoundationIO_BitIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum                      BitIO_FileTypes
     @abstract                                                   Is this BitInput or BitOutput connected to a File or Socket?
     @constant                  FileType_Unknown                 Unknown file type.
     @constant                  FileType_File                    This instance of BitInput/BitOutput is connected to a File.
     @constant                  FileType_Socket                  This instance of BitInput/BitOutput is connected to a Socket.
     */
    typedef enum BitIO_FileTypes {
                                FileType_Unknown                = 0,
                                FileType_File                   = 1,
                                FileType_Socket                 = 2,
    } BitIO_FileTypes;
    
    /*!
     @enum                      BitIO_UnaryTypes
     @constant                  UnaryType_Unknown                Invalid.
     @constant                  UnaryType_Count                  Supports whole numbers (including zero).
     @constant                  UnaryType_Truncated              Supports counting numbers (excluding zero).
     @constant                  UnaryType_Whole                  Supports all the integers including zero and negatives (up to 2^63 anyway).
     */
    typedef enum BitIO_UnaryTypes {
                                UnaryType_Unknown               = 0,
                                UnaryType_Count                 = 1,
                                UnaryType_Truncated             = 2,
                                UnaryType_Whole                 = 3,
    } BitIO_UnaryTypes;
    
    /*!
     @enum                      BitIO_UnaryTerminatorTypes
     @constant                  UnaryTerminator_Zero             The stop bit is 0.
     @constant                  UnaryTerminator_One              The stop bit is 1.
     */
    typedef enum BitIO_UnaryTerminatorTypes {
                                UnaryTerminator_Zero            = 0,
                                UnaryTerminator_One             = 1,
    } BitIO_UnaryTerminatorTypes;
    
    /*!
     @enum                      ByteOrders
     @constant                  BitIO_ByteOrder_Unknown          Unknown byte order.
     @constant                  BitIO_ByteOrder_LSByte           Read from the Least Significant Byte to the Most  Significant, aka right to left.
     @constant                  BitIO_ByteOrder_MSByte           Read from the Most  Significant Byte to the Least Significant, aka left to right.
     */
    typedef enum BitIO_ByteOrders {
                                BitIO_ByteOrder_Unknown         = 0,
                                BitIO_ByteOrder_LSByte          = 1,
                                BitIO_ByteOrder_MSByte          = 2,
    } BitIO_ByteOrders;
    
    /*!
     @enum                      BitIO_BitOrders
     @constant                  BitIO_BitOrder_Unknown           Unknown bit order.
     @constant                  BitIO_BitOrder_LSBit             Read from the Least Significant Bit to the Most  Significant, aka right to left.
     @constant                  BitIO_BitOrder_MSBit             Read from the Most  Significant Bit to the Least Significant, aka left  to right.
     */
    typedef enum BitIO_BitOrders {
                                BitIO_BitOrder_Unknown          = 0,
                                BitIO_BitOrder_LSBit            = 1,
                                BitIO_BitOrder_MSBit            = 2,
    } BitIO_BitOrders;
    
    /*!
     @enum                      BitIO_GUUIDTypes
     @constant                  GUUIDType_Unknown                The GUUID type is unknown, invalid.
     @constant                  GUUIDType_GUIDString             The GUUID is a GUID string, aka LSByte first UUID with hyphens.
     @constant                  GUUIDType_UUIDString             The GUUID is a UUID string, aka MSByte first UUID with hyphens.
     @constant                  GUUIDType_BinaryGUID             The GUUID is a Binary GUID, aka LSByte first UUID without hyphens.
     @constant                  GUUIDType_BinaryUUID             The GUUID is a Binary UUID, aka MSByte first UUID without hypthns.
     */
    typedef enum BitIO_GUUIDTypes {
                                GUUIDType_Unknown               = 0,
                                GUUIDType_GUIDString            = 1,
                                GUUIDType_UUIDString            = 2,
                                GUUIDType_BinaryGUID            = 3,
                                GUUIDType_BinaryUUID            = 4,
    } BitIO_GUUIDTypes;

    /*!
     @enum                      BitIO_StringTerminatorTypes
     @constant                  StringTerminator_NULL            Write the NULL Terminator.
     @constant                  StringTerminator_Sized           Do not write the null terminaotr, there's a size field.
     */
    typedef enum BitIO_StringTerminatorTypes {
                                StringTerminator_NULL           = 1,
                                StringTerminator_Sized          = 2,
    } BitIO_StringTerminatorTypes;
    
    /*!
     @typedef                   BitInput
     @abstract                                                   Contains File/Socket pointers for reading to a BitBuffer.
     */
    typedef struct              BitInput                        BitInput;
    
    /*!
     @typedef                   BitOutput
     @abstract                                                   Contains File/Socket pointers for writing from a BitBuffer.
     */
    typedef struct              BitOutput                       BitOutput;
    
    /*!
     @typedef                   BitBuffer
     @abstract                                                   Contains variables and a pointer to a buffer for reading and writing bits.
     */
    typedef struct              BitBuffer                       BitBuffer;

	/*!
	@typedef                    Entropy
	@abstract                                                    Forward declaration from CryptographyIO.
	*/
	typedef struct              Entropy                         Entropy;
    
    /* BitBuffer */
    /*!
     @abstract                                                   Creates a BitBuffer.
     @param                     BitBufferSize                    The number of bytes to create the BitBuffer with.
     @return                                                     Returns a pointer to the BitBuffer.
     */
    BitBuffer                  *BitBuffer_Init(uint64_t BitBufferSize);
    
    /*!
     @abstract                                                   Reads data from BitInput to BitBuffer.
     @param                     BitB                             The BitBuffer to read to.
     @param                     BitI                             The BitInput to read from.
     */
    void                        BitBuffer_Read(BitBuffer *BitB, BitInput *BitI);
    
    /*!
     @abstract                                                   Gets the size of the BitBuffer in bits.
     @param                     BitB                             BitBuffer Pointer.
     @return                                                     Returns the number of bits the buffer can hold max.
     */
    uint64_t                    BitBuffer_GetSize(BitBuffer *BitB);
    
    /*!
     @abstract                                                   Sets the size of the BitBuffer in bits.
     @param                     BitB                             BitBuffer Pointer.
     @param                     Bits                             The size of the BitBuffer in Bits.
     */
    void                        BitBuffer_SetSize(BitBuffer *BitB, uint64_t Bits);
    
    /*!
     @abstract                                                   Gets the offset of the BitBuffer in bits.
     @param                     BitB                             BitBuffer Pointer.
     @return                                                     Returns the offset from the start of the BitBuffer in bits.
     */
    uint64_t                    BitBuffer_GetPosition(BitBuffer *BitB);
    
    /*!
     @abstract                                                   Sets the offset of the BitBuffer in bits.
     @param                     BitB                             BitBuffer Pointer.
     @param                     Offset                           The offset of the BitBuffer in Bits.
     */
    void                        BitBuffer_SetPosition(BitBuffer *BitB, uint64_t Offset);
    
    /*!
     @abstract                                                   Gets the number of free bits in the BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @return                                                     Returns the offset from the start of BitBuffer in bits - the current offset.
     */
    uint64_t                    BitBuffer_GetBitsFree(BitBuffer *BitB);
    
    /*!
     @abstract                                                   Gets if the BitBuffer's offset is aligned to boundary AlignmentSize.
     @remark                                                     Alignment means the offset of a BitBuffer is a multiple of AlignmentSize.
     @param                     BitB                             BitBuffer Pointer.
     @param                     AlignmentSize                    The number of bytes to check the alignment of.
     @return                                                     Returns whether the BitBuffer is aligned to AlignmentSize or not.
     */
    bool                        BitBuffer_IsAligned(BitBuffer *BitB, uint8_t AlignmentSize);
    
    /*!
     @abstract                                                   Aligns BitBuffer.
     @remark                                                     Zero pads the BitBuffer's offset to a multiple of AlignmentSize.
     @param                     BitB                             BitBuffer Pointer.
     @param                     AlignmentSizeInBytes             The alignment size in bytes.
     */
    void                        BitBuffer_Align(BitBuffer *BitB, uint8_t AlignmentSizeInBytes);
    
    /*!
     @abstract                                                   Seeks forwards and backwards in BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     Bits2Seek                        The number of bits to seek.
     */
    void                        BitBuffer_Seek(BitBuffer *BitB, int64_t Bits2Seek);
    
    /*!
     @abstract                                   Securely erases the BitBuffer.
     @param                     BitB             BitBuffer Pointer.
     @param                     NewValue         The value to set each byte to while erasing.
     @return                                     Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    uint8_t                     BitBuffer_Erase(BitBuffer *BitB, uint8_t NewValue);
    
    /*!
     @abstract                                                   Changes the size of an already initialized BitBuffer.
     @remark                                                     Resizing a BitBuffer IS NOT destructive.
     @param                     BitB                             BitBuffer Pointer to resize.
     @param                     NewSize                          The new size of the internal buffer in bytes.
     */
    void                        BitBuffer_Resize(BitBuffer *BitB, uint64_t NewSize);
    
    /*!
     @abstract                                                   Reads fresh data to a BitBuffer from a BitInput.
     @remark                                                     IS destructive, it will NOT keep any data in the buffer.
     @param                     BitB                             The BitBuffer to refresh.
     @param                     BitI                             The BitInput to refresh the BitBuffer with.
     */
    void                        BitBuffer_Refresh(BitBuffer *BitB, BitInput *BitI);
    
    /*!
     @abstract                                                   Copies BitBuffer Source to Destination.
     @remark                                                     Is NOT destructive, it will keep any unread data in the buffer.
     @param                     Source                           The BitBuffer to copy from.
     @param                     Destination                      The BitBuffer to copy to.
     @param                     BitStart                         The bit to start copying from (inclusive).
     @param                     BitEnd                           The bit to end copying (inclusive).
     */
    void                        BitBuffer_Copy(BitBuffer *Source, BitBuffer *Destination, uint64_t BitStart, uint64_t BitEnd);
    
    /*!
     @abstract                                                   Peeks (reads but without recording that it's been read) bits from BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     ByteOrder                        What byte order are the bits to be peeked?
     @param                     BitOrder                         What bit order are the bits to be peeked?
     @param                     Bits2Peek                        The number of bits to peek from the BitBuffer.
     */
    uint64_t                    BitBuffer_PeekBits(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Peek);
    
    /*!
     @abstract                                                   Reads bits from BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     ByteOrder                        What byte order are the bits to be read?
     @param                     BitOrder                         What bit order are the bits to be read?
     @param                     Bits2Read                        The number of bits to read from the BitBuffer.
     */
    /*{*/
    uint8_t                     BitBuffer_ReadBits8(BitBuffer *BitB,  BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read);
    uint16_t                    BitBuffer_ReadBits16(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read);
    uint32_t                    BitBuffer_ReadBits32(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read);
    uint64_t                    BitBuffer_ReadBits64(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read);
#ifndef                         BitBuffer_ReadBits
#define                         BitBuffer_ReadBits(BitBuffer, ByteOrder, BitOrder, Bits2Read)
#if   (Bits2Read <= 8)
#undef                          BitBuffer_ReadBits
#define                         BitBuffer_ReadBits(BitBuffer, ByteOrder, BitOrder, Bits2Read) BitBuffer_ReadBits8(BitBuffer, ByteOrder, BitOrder, Bits2Read)
#elif (Bits2Read <= 16)
#undef                          BitBuffer_ReadBits
#define                         BitBuffer_ReadBits(BitBuffer, ByteOrder, BitOrder, Bits2Read) BitBuffer_ReadBits16(BitBuffer, ByteOrder, BitOrder, Bits2Read)
#elif (Bits2Read <= 32)
#undef                          BitBuffer_ReadBits
#define                         BitBuffer_ReadBits(BitBuffer, ByteOrder, BitOrder, Bits2Read) BitBuffer_ReadBits32(BitBuffer, ByteOrder, BitOrder, Bits2Read)
#elif (Bits2Read <= 64)
#undef                          BitBuffer_ReadBits
#define                         BitBuffer_ReadBits(BitBuffer, ByteOrder, BitOrder, Bits2Read) BitBuffer_ReadBits64(BitBuffer, ByteOrder, BitOrder, Bits2Read)
#else
#error                          Bits2Read is invalid
#endif /* Bits2Read */
#endif /* BitBuffer_ReadBits */
    /*}*/
    
    /*!
     @abstract                                                   Reads unary encoded fields from the BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     ByteOrder                        What byte order are the bits to be read?
     @param                     BitOrder                         What bit order are the bits to be read?
     @param                     UnaryType                        What type of Unary coding are we reading?
     @param                     StopBit                          Is the stop bit a one or a zero?
     */
    uint64_t                    BitBuffer_ReadUnary(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, BitIO_UnaryTypes UnaryType, BitIO_UnaryTerminatorTypes StopBit);
    
    /*!
     @abstract                                                   Reads data from the Bitbuffer until it stops matching the UTF-8 format.
     @remark                                                     Does NOT count any potential NULL terminator.
     @param                     BitB                             BitBuffer Pointer.
     */
    uint64_t                    BitBuffer_GetUTF8StringSize(BitBuffer *BitB);
    
    /*!
     @abstract                                                   Reads a UTF-8 encoded string from the BitBuffer.
     @remark                                                     We read exactly StringSize bytes, you need to account for any null terminators, etc.
     @param                     BitB                             BitBuffer Pointer.
     @param                     StringSize                       Size of the string in CodeUnits, not counting an optional null.
     */
    UTF8                       *BitBuffer_ReadUTF8(BitBuffer *BitB, uint64_t StringSize);
    
    /*!
     @abstract                                                   Reads data from the Bitbuffer until it stops matching the UTF-8 format.
     @remark                                                     Does NOT count any potential NULL terminator.
     @param                     BitB                             BitBuffer Pointer.
     */
    uint64_t                    BitBuffer_GetUTF16StringSize(BitBuffer *BitB);
    
    /*!
     @abstract                                                   Reads a UTF-16 encoded string from the BitBuffer.
     @remark                                                     We read exactly StringSize * 2 bytes, you need to account for any null terminators, etc.
     @param                     BitB                             BitBuffer Pointer.
     @param                     StringSize                       Size of the string in CodeUnits, not counting an optional null.
     */
    UTF16                      *BitBuffer_ReadUTF16(BitBuffer *BitB, uint64_t StringSize);
    
    /*!
     @abstract                                                   Reads a BinaryGUUID/GUUIDString from the BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     GUUIDType                        The type of GUUID to read.
     @return                                                     Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BinaryGUUIDSize or BinaryGUUIDSize bytes.
     */
    uint8_t                    *BitBuffer_ReadGUUID(BitBuffer *BitB, BitIO_GUUIDTypes GUUIDType);
    
    /*!
     @abstract                                                   Writes bits to the BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     ByteOrder                        What byte order should the Bits2Write be in?
     @param                     BitOrder                         What bit order should the Bits2Write be in?
     @param                     NumBits2Write                    How many bits from Bits2Write should we write?
     @param                     Bits2Write                       the actual data to write.
     */
    void                        BitBuffer_WriteBits(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t NumBits2Write, uint64_t Bits2Write);
    
    /*!
     @abstract                                                   Writes unary encoded bits to the BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     ByteOrder                        What byte order should the Bits2Write be in?
     @param                     BitOrder                         What bit order should the Bits2Write be in?
     @param                     UnaryType                        What type of Unary coding should we use?
     @param                     StopBit                          Is the stop bit a one or a zero?
     @param                     Bits2Write                       the actual data to write.
     */
    void                        BitBuffer_WriteUnary(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, BitIO_UnaryTypes UnaryType, BitIO_UnaryTerminatorTypes StopBit, uint8_t Bits2Write);
    
    /*!
     @abstract                                                   Writes a UTF-8 encoded string to the BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     String2Write                     The string to write to the BitBuffer.
     @param                     WriteType                        Should the NULL terminator be written?
     */
    void                        BitBuffer_WriteUTF8(BitBuffer *BitB, UTF8 *String2Write, BitIO_StringTerminatorTypes WriteType);
    
    /*!
     @abstract                                                   Writes a UTF-16 encoded string to the BitBuffer.
     @remark                                                     Convert the string to UTF16BOM_BE or UTF16BOM_LE first.
     @param                     BitB                             BitBuffer Pointer.
     @param                     String2Write                     The string to write to the BitBuffer.
     @param                     WriteType                        Should the NULL terminator be written?
     */
    void                        BitBuffer_WriteUTF16(BitBuffer *BitB, UTF16 *String2Write, BitIO_StringTerminatorTypes WriteType);
    
    /*!
     @abstract                                                   Writes a GUUID to the BitBuffer.
     @param                     BitB                             BitBuffer Pointer.
     @param                     GUUID2Write                      Pointer to the GUUID you want to write.
     */
    void                        BitBuffer_WriteGUUID(BitBuffer *BitB, BitIO_GUUIDTypes GUUIDType, uint8_t *GUUID2Write);
    
    /*!
     @abstract                                                   Writes a BitBuffer to BitOutput.
     @remark                                                     Only writes actually used water.
     @param                     BitB                             The BitBuffer to write.
     @param                     BitO                             The BitOutput to write from.
     */
    void                        BitBuffer_Write(BitBuffer *BitB, BitOutput *BitO);
    
    /*!
     @abstract                                                   Deletes the BitBuffer.
     @param                     BitB                             BitBuffer Pointer you want to deinit.
     */
    void                        BitBuffer_Deinit(BitBuffer *BitB);
    /* BitBuffer */
    
    /* BitInput */
    /*!
     @abstract                                                   Creates a BitInput.
     @return                                                     Returns a pointer to said BitInput structure.
     */
    BitInput                   *BitInput_Init(void);
    
    /*!
     @abstract                                                   Opens an input file, pointed to by Path2Open.
     @param                     BitI                             BitInput Pointer.
     @param                     Path2Open                        Path to the input file to open.
     */
    void                        BitInput_UTF8_OpenFile(BitInput *BitI, UTF8 *Path2Open);
    
    /*!
     @abstract                                                   Opens an input file, pointed to by Path2Open.
     @param                     BitI                             BitInput Pointer.
     @param                     Path2Open                        Path to the input file to open.
     */
    void                        BitInput_UTF16_OpenFile(BitInput *BitI, UTF16 *Path2Open);
    
    /*!
     @abstract                                                   Opens a socket for reading.
     @param                     BitI                             BitInput Pointer.
     @param                     Domain                           What domain should communication take place on?
     @param                     Type                             What type of Socket are you trying to open?
     @param                     Protocol                         What protocol are you trying to use over this socket?
     */
    void                        BitInput_OpenSocket(BitInput *BitI, int Domain, int Type, int Protocol);
    
    /*!
     @abstract                                                   Connects BitInput to a socket.
     @param                     BitI                             BitInput Pointer.
     @param                     SocketAddress                    The address of the socket.
     @param                     SocketSize                       I honest have no idea what this means.
     */
    void                        BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, uint32_t SocketSize);
    
    /*!
     @abstract                                                   Gets the size of the BitInput file.
     @param                     BitI                             BitInput Pointer.
     @return                                                     Returns the value in BitI->FileSize if it exists.
     */
    uint64_t                    BitInput_GetFileSize(BitInput *BitI);
    
    /*!
     @abstract                                                   Gets the position of the BitInput file from the start.
     @param                     BitI                             BitInput Pointer.
     @return                                                     Returns the position of the file in bytes from the beginning
     */
    uint64_t                    BitInput_GetFilePosition(BitInput *BitI);
    
    /*!
     @abstract                                                   Computes the number of bytes left in the file.
     @param                     BitI                             BitInput Pointer.
     @return                                                     Returns the number of bytes left in the file.
     */
    uint64_t                    BitInput_BytesRemaining(BitInput *BitI);
    
    /*!
     @abstract                                                   Deallocates BitInput.
     @remark                                                     For use when changing files, or exiting the program.
     @param                     BitI                             BitInput Pointer you want to free.
     */
    void                        BitInput_Deinit(BitInput *BitI);
    /* BitInput */
    
    /* BitOutput */
    /*!
     @abstract                                                   Creates a BitOutput structure.
     @return                                                     Returns a pointer to said BitOutput structure.
     */
    BitOutput                  *BitOutput_Init(void);
    
    /*!
     @abstract                                                   Opens an output file for writing.
     @param                     BitO                             BitOutput Pointer.
     @param                     Path2Open                        Path to the output file to open.
     */
    void                        BitOutput_UTF8_OpenFile(BitOutput *BitO, UTF8 *Path2Open);
    
    /*!
     @abstract                                                   Opens an output file for writing.
     @param                     BitO                             BitOutput Pointer.
     @param                     Path2Open                        Path to the output file to open.
     */
    void                        BitOutput_UTF16_OpenFile(BitOutput *BitO, UTF16 *Path2Open);
    
    /*!
     @abstract                                                   Opens a socket for writing.
     @param                     BitO                             BitOutput Pointer.
     @param                     Domain                           What domain should communication take place on?
     @param                     Type                             What type of Socket are you trying to open?
     @param                     Protocol                         What protocol are you trying to use over this socket?
     */
    void                        BitOutput_OpenSocket(BitOutput *BitO, int Domain, int Type, int Protocol);
    
    /*!
     @abstract                                                   Connects BitOutput to a socket.
     @param                     BitO                             BitOutput Pointer.
     @param                     SocketAddress                    The address of the socket.
     @param                     SocketSize                       I honestly have no idea what this means.
     */
    void                        BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, uint64_t SocketSize);
    
    /*!
     @abstract                                                   Deallocates the instance of BitOutput pointed to by BitO.
     @remark                                                     For use when changing files, or exiting the program.
     @param                     BitO                             BitOutput Pointer you want to free.
     */
    void                        BitOutput_Deinit(BitOutput *BitO);
    /* BitOutput */
    
    /* GUUID */
    /*!
     @abstract                                                   Generates a random GUUID.
     @param                     Random                           Pointer to Entropy.
     @return                                                     Returns the generated GUUID.
     */
    uint8_t                    *GUUID_Generate(Entropy *Random, BitIO_GUUIDTypes GUUIDType);
    
    /*!
     @abstract                                                   Compares GUUIDs for equivalence, GUUID1 and 2 HAVE to be the same type.
     @param                     GUUID1                           Pointer to a GUUID to be compared.
     @param                     GUUID2                           Pointer to a GUUID to be compared.
     @return                                                     Returns Yes if GUUID1 and GUUID2 match, No otherwise.
     */
    bool                        GUUID_Compare(BitIO_GUUIDTypes GUUIDType, uint8_t *GUUID1, uint8_t *GUUID2);
    
    /*!
     @abstract                                                   Converts a GUUID from one representation to another (String/Binary) || (UUID/GUID).
     @remark                                                     The original GUUID is untouched, the user needs to deinit it.
     @param                     InputType                        What type of GUUID are we converting from?
     @param                     OutputType                       What type of GUUID are we converting to?
     @return                                                     Returns the converted GUUID.
     */
    uint8_t                    *GUUID_Convert(BitIO_GUUIDTypes InputType, BitIO_GUUIDTypes OutputType, uint8_t *GUUID2Convert);
    
    /*!
     @abstract                                                   Swaps the byte order of a BinaryGUUID or GUUIDString.
     @remark                                                     The original GUUID is untouched, the user needs to deinit it.
     @param                     GUUIDType                        Is this a GUUIDString or BinaryGUUID?
     @param                     GUUID2Swap                       GUUID Pointer to swap.
     @return                                                     Returns a pointer to a swapped GUUID.
     */
    uint8_t                    *GUUID_Swap(BitIO_GUUIDTypes GUUIDType, uint8_t *GUUID2Swap);
    
    /*!
     @abstract                                                   Frees a GUUID.
     */
    void                        GUUID_Deinit(uint8_t *GUUID);
    /* GUUID */
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_BitIO_H */
