#include "StringIO.h"                 /* Included for UTF8, UTF16, FoundationIOTargetOS in Macros */

#pragma  once

#ifndef  FoundationIO_BitIO_H
#define  FoundationIO_BitIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header                    BitIO.h
     @author                    Marcus Johnson
     @copyright                 2015+
     @version                   1.0.0
     @brief                     This header contains code for reading and writing files and sockets, and utilities to manage them.
     */
    
    /*!
     @enum                      BitIOFileTypes
     @abstract                                                  "Is this BitInput or BitOutput connected to a File or Socket?".
     @constant                  BitIOUnknownFileType            "Unknown file type".
     @constant                  BitIOFile                       "This instance of BitInput/BitOutput is connected to a File".
     @constant                  BitIOSocket                     "This instance of BitInput/BitOutput is connected to a Socket".
     */
    typedef enum BitIOFileTypes {
                                BitIOUnknownFileType            = 0,
                                BitIOFile                       = 1,
                                BitIOSocket                     = 2,
    } BitIOFileTypes;
    
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
     @param                     BitBufferSize                   "The number of bytes to create the BitBuffer with".
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
     @abstract                                                  "Sets the size of the BitBuffer in bits".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits                            "The size of the BitBuffer in Bits".
     */
    void                        BitBuffer_SetSize(BitBuffer *BitB, uint64_t Bits);
    
    /*!
     @abstract                                                  "Gets the offset of the BitBuffer in bits".
     @param                     BitB                            "BitBuffer Pointer".
     @return                                                    "Returns the offset from the start of the BitBuffer in bits".
     */
    uint64_t                    BitBuffer_GetPosition(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Sets the offset of the BitBuffer in bits".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Offset                          "The offset of the BitBuffer in Bits".
     */
    void                        BitBuffer_SetPosition(BitBuffer *BitB, uint64_t Offset);
    
    /*!
     @abstract                                                  "Gets the number of free bits in the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @return                                                    "Returns the offset from the start of BitBuffer in bits - the current offset".
     */
    uint64_t                    BitBuffer_GetBitsFree(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Gets if the BitBuffer's offset is aligned to boundary AlignmentSize".
     @remark                                                    "Alignment means the offset of a BitBuffer is a multiple of AlignmentSize".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     AlignmentSize                   "The number of bytes to check the alignment of".
     @return                                                    "Returns whether the BitBuffer is aligned to AlignmentSize or not".
     */
    bool                        BitBuffer_IsAligned(BitBuffer *BitB, const uint8_t AlignmentSize);
    
    /*!
     @abstract                                                  "Aligns BitBuffer".
     @remark                                                    "Zero pads the BitBuffer's offset to a multiple of AlignmentSize".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     AlignmentSize                   "The alignment size in bytes".
     */
    void                        BitBuffer_Align(BitBuffer *BitB, const uint8_t AlignmentSize);
    
    /*!
     @abstract                                                  "Seeks forwards and backwards in BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Seek                       "The number of bits to seek".
     */
    void                        BitBuffer_Seek(BitBuffer *BitB, const int64_t Bits2Seek);
    
    /*!
     @abstract                                                  "Zeros the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     */
    void                        BitBuffer_Clear(BitBuffer *BitB);
    
    /*!
     @abstract                                                  "Changes the size of an already initialized BitBuffer".
     @remark                                                    "Resizing a BitBuffer IS NOT destructive".
     @param                     BitB                            "BitBuffer Pointer to resize".
     @param                     NewSize                         "The new size of the internal buffer in bytes".
     */
    void                        BitBuffer_Resize(BitBuffer *BitB, const uint64_t NewSize);
    
    /*!
     @abstract                                                  "Reads fresh data to a BitBuffer from a BitInput".
     @remark                                                    "IS destructive, it will NOT keep any data in the buffer".
     @param                     BitB                            "The BitBuffer to refresh".
     @param                     BitI                            "The BitInput to refresh the BitBuffer with".
     */
    void                        BitBuffer_Refresh(BitBuffer *BitB, BitInput *BitI);
    
    /*!
     @abstract                                                  "Copies BitBuffer Source to Destination".
     @remark                                                    "Is NOT destructive, it will keep any unread data in the buffer".
     @param                     Source                          "The BitBuffer to copy from".
     @param                     Destination                     "The BitBuffer to copy to".
     @param                     BitStart                        "The bit to start copying from (inclusive)".
     @param                     BitEnd                          "The bit to end copying (inclusive)".
     */
    void                        BitBuffer_Copy(BitBuffer *Source, BitBuffer *Destination, uint64_t BitStart, uint64_t BitEnd);
    
    /*!
     @abstract                                                  "Peeks (reads but without recording that it's been read) bits from BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits to be peeked"?
     @param                     BitOrder                        "What bit order are the bits to be peeked"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Peek                       "The number of bits to peek from the BitBuffer".
     */
    uint64_t                    BitBuffer_PeekBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Peek);
    
    /*!
     @abstract                                                  "Reads bits from BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits to be read"?
     @param                     BitOrder                        "What bit order are the bits to be read"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     Bits2Read                       "The number of bits to read from the BitBuffer".
     */
    uint64_t                    BitBuffer_ReadBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t Bits2Read);
    
    /*!
     @abstract                                                  "Reads unary encoded fields from the BitBuffer".
     @param                     ByteOrder                       "What byte order are the bits to be read"?
     @param                     BitOrder                        "What bit order are the bits to be read"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding are we reading"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     */
    uint64_t                    BitBuffer_ReadUnary(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, const bool StopBit);
    
    /*!
     @abstract                                                  "Reads a UTF-8 encoded string from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     StringSize                      "Size of the string in CodeUnits".
     */
    UTF8                       *BitBuffer_ReadUTF8(BitBuffer *BitB, uint64_t StringSize);
    
    /*!
     @abstract                                                  "Reads a UTF-16 encoded string from the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     StringSize                      "Size of the string in CodeUnits".
     */
    UTF16                      *BitBuffer_ReadUTF16(BitBuffer *BitB, uint64_t StringSize);
    
    /*!
     @abstract                                                  "Reads a BinaryGUUID/GUUIDString from the BitBuffer".
     @param                     GUUIDType                       "The type of GUUID to read".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @return                                                    "Returns a pointer to the BinaryGUUID/GUUIDString, it will contain BinaryGUUIDSize or BinaryGUUIDSize bytes".
     */
    uint8_t                    *BitBuffer_ReadGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType);
    
    /*!
     @abstract                                                  "Writes bits to the BitBuffer".
     @param                     ByteOrder                       "What byte order should the Bits2Write be in"?
     @param                     BitOrder                        "What bit order should the Bits2Write be in"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     NumBits2Write                   "How many bits from Bits2Write should we write?".
     @param                     Bits2Write                      "the actual data to write".
     */
    void                        BitBuffer_WriteBits(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes unary encoded bits to the BitBuffer".
     @param                     ByteOrder                       "What byte order should the Bits2Write be in"?
     @param                     BitOrder                        "What bit order should the Bits2Write be in"?
     @param                     BitB                            "BitBuffer Pointer".
     @param                     UnaryType                       "What type of Unary coding should we use"?
     @param                     StopBit                         "Is the stop bit a one or a zero"?
     @param                     Bits2Write                      "the actual data to write".
     */
    void                        BitBuffer_WriteUnary(ByteOrders ByteOrder, BitOrders BitOrder, BitBuffer *BitB, UnaryTypes UnaryType, bool StopBit, const uint8_t Bits2Write);
    
    /*!
     @abstract                                                  "Writes a UTF-8 encoded string to the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     String2Write                    "The string to write to the BitBuffer".
     */
    void                        BitBuffer_WriteUTF8(BitBuffer *BitB, UTF8 *String2Write);
    
    /*!
     @abstract                                                  "Writes a UTF-16 encoded string to the BitBuffer".
     @remark                                                    "Convert the string to UTF16BE or UTF16LE first".
     @param                     BitB                            "BitBuffer Pointer".
     @param                     String2Write                    "The string to write to the BitBuffer".
     */
    void                        BitBuffer_WriteUTF16(BitBuffer *BitB, UTF16 *String2Write);
    
    /*!
     @abstract                                                  "Writes a GUUID to the BitBuffer".
     @param                     BitB                            "Pointer to an instance of BitBuffer".
     @param                     GUUID2Write                     "Pointer to the GUUID you want to write".
     */
    void                        BitBuffer_WriteGUUID(BitBuffer *BitB, GUUIDTypes GUUIDType, const uint8_t *GUUID2Write);
    
    /*!
     @abstract                                                  "Deinitalizes the BitBuffer".
     @param                     BitB                            "BitBuffer Pointer you want to deinit".
     */
    void                        BitBuffer_Deinit(BitBuffer *BitB);
    /* BitBuffer */
    
    /* BitInput */
    /*!
     @abstract                                                  "Initializes a BitInput".
     @return                                                    "Returns a pointer to said BitInput structure".
     */
    BitInput                   *BitInput_Init(void);
    
    /*!
     @abstract                                                  "Opens an input file, pointed to by Path2Open".
     @param                     BitI                            "BitInput Pointer".
     @param                     Path2Open                       "Path to the input file to open".
     */
    void                        BitInput_UTF8_OpenFile(BitInput *BitI, UTF8 *Path2Open);
    
    /*!
     @abstract                                                  "Opens an input file, pointed to by Path2Open".
     @param                     BitI                            "BitInput Pointer".
     @param                     Path2Open                       "Path to the input file to open".
     */
    void                        BitInput_UTF16_OpenFile(BitInput *BitI, UTF16 *Path2Open);
    
    /*!
     @abstract                                                  "Is the file variable"?
     @param                     BitI                            "BitInput Pointer".
     */
    bool                        BitInput_IsChangable(BitInput *BitI);
    
    /*!
     @abstract                                                  "Closes the current file, and opens the next one in it's place".
     @param                     BitI                            "BitInput Pointer".
     */
    void                        BitInput_ChangeFile(BitInput *BitI);
    
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
    int64_t                     BitInput_GetFileSize(BitInput *BitI);
    
    /*!
     @abstract                                                  "Reads Bytes2Read into Buffer2Read from BitInput".
     @remark                                                    "If the BitBuffer Pointer is not new, all the old contents will be lost".
     @param                     BitI                            "BitInput Pointer".
     @param                     Buffer2Read                     "BitBuffer Pointer".
     @param                     Bytes2Read                      "The number of bytes to read from the BitInput into the BitBuffer".
     */
    void                        BitInput_Read2BitBuffer(BitInput *BitI, BitBuffer *Buffer2Read, const int64_t Bytes2Read);
    
    /*!
     @abstract                                                  "Gets the position of the BitInput file from the start".
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the position of the file in bytes from the beginning"
     */
    int64_t                     BitInput_GetFilePosition(BitInput *BitI);
    
    /*!
     @abstract                                                  "Computes the number of bytes left in the file".
     @param                     BitI                            "BitInput Pointer".
     @return                                                    "Returns the number of bytes left in the file".
     */
    int64_t                     BitInput_BytesRemaining(BitInput *BitI);
    
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
    void                        BitOutput_UTF8_OpenFile(BitOutput *BitO, UTF8 *Path2Open);
    
    /*!
     @abstract                                                  "Opens an output file for writing".
     @param                     BitO                            "BitOutput Pointer".
     @param                     Path2Open                       "Path to the output file to open".
     */
    void                        BitOutput_UTF16_OpenFile(BitOutput *BitO, UTF16 *Path2Open);
    
    /*!
     @abstract                                                  "Is the file variable"?
     @param                     BitO                            "BitOutput Pointer".
     */
    bool                        BitOutput_IsChangable(BitOutput *BitO);
    
    /*!
     @abstract                                                  "Closes the current file, and opens the next one in it's place".
     @param                     BitO                            "BitOutput Pointer".
     */
    void                        BitOutput_ChangeFile(BitOutput *BitO);
    
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
    
    /* GUUID */
    /*!
     @abstract                                                  "Compares GUUIDs for equivalence, GUUID1 and 2 HAVE to be the same type".
     @param                     GUUID1                          "Pointer to a GUUID to be compared".
     @param                     GUUID2                          "Pointer to a GUUID to be compared".
     @return                                                    "Returns Yes if GUUID1 and GUUID2 match, No otherwise".
     */
    bool                        GUUID_Compare(GUUIDTypes GUUIDType, const uint8_t *GUUID1, const uint8_t *GUUID2);
    
    /*!
     @abstract                                                  "Converts a GUUID from one representation to another (String/Binary) || (UUID/GUID)".
     @remark                                                    "The original GUUID is untouched, the user needs to deinit it".
     @param                     InputType                       "What type of GUUID are we converting from"?
     @param                     OutputType                      "What type of GUUID are we converting to"?
     @return                                                    "Returns the converted GUUID".
     */
    uint8_t                    *GUUID_Convert(GUUIDTypes InputType, GUUIDTypes OutputType, uint8_t *GUUID2Convert);
    
    /*!
     @abstract                                                  "Swaps the byte order of a BinaryGUUID or GUUIDString".
     @remark                                                    "The original GUUID is untouched, the user needs to deinit it".
     @param                     GUUIDType                       "Is this a GUUIDString or BinaryGUUID"?
     @param                     GUUID2Swap                      "GUUID Pointer to swap".
     @return                                                    "Returns a pointer to a swapped GUUID".
     */
    uint8_t                    *GUUID_Swap(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap);
    
    /*!
     @abstract                                                  "Frees a GUUID".
     */
    void                        GUUID_Deinit(uint8_t *GUUID);
    /* GUUID */
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_BitIO_H */
