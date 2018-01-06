#pragma once

#ifndef LIBBITIO_BitIOMacros_H
#define LIBBITIO_BitIOMacros_H

#ifdef  __cplusplus
extern  "C" {
#endif
    
    /*!
     @header    BitIOMacros.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2017+
     @version   1.0.0
     @brief     This header contains preprocessor macros for generic functions in BitIO, and cross-platform compatibility.
     */
    
#ifndef BitIOUnknownOS
#define BitIOUnknownOS    0
#endif
    
#ifndef BitIOPOSIXOS
#define BitIOPOSIXOS      1
#endif
    
#ifndef BitIOWindowsOS
#define BitIOWindowsOS    2
#endif
    
#ifndef BitIOMacClassicOS
#define BitIOMacClassicOS 3
#endif
    
#if      defined(macintosh) || defined(Macintosh)
#ifndef             BitIOTargetOS
#define             BitIOTargetOS (BitIOMacClassicOS)
#endif
    
#ifndef             BitIONewLineSize
#define             BitIONewLineSize    1
    static   const char BitIONewLine[2]  = {0x0D, 0x00}; /* \r */
#endif
    
#elif    defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__MACH__) || defined(BSD) || defined(linux) || defined(__linux)
    
#ifndef             BitIOTargetOS
#define             BitIOTargetOS (BitIOPOSIXOS)
#endif
    
#undef              fseek
#define             fseek               fseeko
    
#undef              ftell
#define             ftell               ftello
    
#ifndef             BitIONewLineSize
#define             BitIONewLineSize    1
    static   const char BitIONewLine[2]  = {0x0A, 0x00}; /* \n */
#endif
    
#elif    defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WINDOWS__)
    
#ifndef             BitIOTargetOS
#define             BitIOTargetOS (BitIOWindowsOS)
#endif
    
#ifndef             BitIONewLineSize
#define             BitIONewLineSize    2
    static   const char BitIONewLine[3]  = {0x0D, 0x0A, 0x00}; /* \r\n */
#endif
    
#ifndef             restrict
#define             restrict          __restrict
#endif
    
#ifndef             strncasecmp
#define             strncasecmp        _strnicmp
#endif
    
#undef              fseek
#define             fseek              _fseeki64
    
#undef              ftell
#define             ftell              _ftelli64
    
#ifndef             strcasecmp
#define             strcasecmp          stricmp
#endif
    
#endif   /* End OS detection */
    
#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(push, 0)
#endif
#include <limits.h>
#include <float.h>
#if    (BitIOTargetOS == BitIOWindowsOS)
#pragma warning(pop)
#endif
    
#ifndef   BitIOByteOrders
#define   BitIOByteOrders
#define    UnknownByteOrder    0
#define    LSByte              1
#define    MSByte              2
#endif  /* BitIOEndianByteOrders */
    
#ifndef   BitIOBitOrders
#define   BitIOBitOrders
#define    UnknownBitOrder     0
#define    LSBit               1
#define    MSBit               2
#endif  /* BitIOBitOrders */
    
#ifndef   BitIOYesNo
#define   BitIOYesNo
#ifndef    Yes
#define    Yes                 1
#endif  /* Yes */
#ifndef    No
#define    No                  0
#endif  /* No */
#endif  /* BitIOYesNo */
    
#ifndef    BitIOByteBitOrders
#define    BitIOByteBitOrders
#ifndef    BitIOLSByte
#define    BitIOLSByte         ULLONG_MAX
typedef    unsigned long long  BitBLSByte_t;
#endif  /* BitIOLittleEndian */
#ifndef    BitIOMSByte
#define    BitIOMSByte         LLONG_MIN
typedef    signed long long    BitBMSByte_t;
#endif  /* BitIOBigEndian */
#ifndef    BitIOLSBit
#define    BitIOLSBit          ULLONG_MAX
typedef    unsigned long long  BitBLSBit_t;
#endif  /* BitIOLSBit */
#ifndef    BitIOMSBit
#define    BitIOMSBit          LLONG_MIN
typedef    signed long long    BitBMSBit_t;
#endif  /* BitIOMSBit */
#endif  /* BitIOByteBitOrders */
    
#ifndef    BitIOGUUIDTypes
#define    BitIOGUUIDTypes
#ifndef    BitIOGUUIDString
#define    BitIOGUUIDString    ULLONG_MAX
typedef    unsigned long long  GUUIDString_t;
#endif  /* BitIOGUIDString */
#ifndef    BitIOBinaryGUUID
#define    BitIOBinaryGUUID    FLT_MIN
typedef    float               BinaryGUUID_t;
#endif  /* BitIOBinaryGUID */
#endif  /* BitIOGUUIDTypes */
    
    
#if     defined(__LITTLE_ENDIAN__) || (__ARMEL__) || (__THUMBEL__) || (__AARCH64EL__) || (_MIPSEL) || (__MIPSEL) || (__MIPSEL__) || (__BYTE_ORDER==(__LITTLE_ENDIAN || __ORDER_LITTLE_ENDIAN))
#define    RuntimeByteOrder    LSByte
#define    RuntimeBitOrder     LSBit
#elif   defined(__BIG_ENDIAN__) || (__ARMEB__) || (__THUMBEB__) || (__AARCH64EB__) || (_MIPSEB) || (__MIPSEB) || (__MIPSEB__) || (__BYTE_ORDER==(__BIG_ENDIAN || __ORDER_BIG_ENDIAN))
#define    RuntimeByteOrder    MSByte
#define    RuntimeBitOrder     MSBit
#endif
    
    /*
     Ok so we need to know the position of a bit, byte, and word in the runtime system.
     the Most Significant Bit  means which bit  has the highest value, the one on the left or the right?
     the Most Significant Byte means which byte has the highest value, the one on the left or the right?
     
     In order to test the bit  order, we need to set a byte  to 0x80   and read it back, if the value is   128 then the left-most bit  is the most significant, if it's   1 it's the rightmost bit.
     In order to test the byte order, we need to set a short to 0xFF00 and read it back, if the value is 65280 then the left-most byte is the most significant, it it's 255 it's the rightmost byte.
     */
    
    /*
     For now, let's just worry about setting up runtime functions to detect the bit and byte orders, we'll worry about optimizing it with compile time generics later on.
     */
    
    /*
     First we need to come up with a new way to describe bit/byte order.
     What we're trying to describe is the position or side in which the most value bit/bye is located.
     if the most significant bit  is on the left, then blah blah blah.
     if the most significant byte is on the left, then blah blah blah.
     
     Now, the least signifiicant bit/byte is NESSICARILY on the opposite side as the most significant.
     
     WhichSideIsTheMostSignificantBitOn  Left/Right
     WhichSideIsTheMostSignificantByteOn Left/Right
     
     How do we shorten that?
     
     BitIOMSByte = Left/Right
     BitIOLSByte = Left/Right
     BitIOMSBit  = Left/Right
     BitIOLSBit  = Left/Right
     
     or what about
     
     BitIORuntimeByteOrder = BitIOMSByteLeft/BitIOMSByteRight
     BitIORuntimeBitOrder  = BitIOMSBitLeft/BitIOMSBitRight
     
     so
     
     BitIORuntimeByteOrder
     BitIORuntimeBitOrder
     
     BitIOMSByteLeft
     BitIOMSByteRight
     
     BitIOMSBitLeft
     BitIOMSBitRight
     
     BitIOLSByteLeft
     BitIOLSByteRight
     
     BitIOLSBitLeft
     BitIOLSBitRight
     
     Byte order values should be W/X
     Bit  order values should be Y/Z
     
     I want it so that the most signfiicant and least significant bits and bytes can be ANDed together to create a simple bitmask that describes the order of bits/bytes
     
     so that RuntimeByteOrder == (MostSignificantByteDirection|LeastSignificantByteDirection)
     
     so if the MSBit is on the left and the LSBit is on the right, we're looking at a LSBitFirst byte
     
     so
     
     MSBitSide  = (Left||Right)
     LSBitSide  = (Left||Right)
     
     MSByteSide = (Left||Right)
     LSByteSide = (Left||Right)
     
     what if instead we used the position? so that where teh left or right is in relation to each other tells us where the MSB is?
     
     ByteOrder  = (Left|Right) // Left side is the MSB Right is the LSB, and therefore the byte order is left to right
     BitOrder   = (Right|Left) // The rightmost bit is the MSB and the leftmost bit is the LSB.
     
     Well if we set Left's and Right's value to it's position, we may have something...
     
     so Left = 8 because it's bit 8, and right to 1 because it's bit 1 when hey're ORed together we get 9
     the only problem is that we'll also get 9 if we OR 1 and 8.
     
     
     
     Ok, so we need to record where the LSBit/LSByte is directionally.
     
     is it to the far left or far right?
     
     Ok so we have A Unicode French symbol, U+269C, in BigEndian it's 0x269C, in LittleEndian it's 0x9C26.
     
     on a little endian machine, you just read the bytes for the LE version.
     on a big    endian machine, you just read the bytes for the BE version.
     
     on a little endian machine, you byteswap the BE version.
     on a big    endian machine, you byteswap the LE version.
     
     To properly read the data, we need to know the format the data is written in, and the format the target machine requires.
     
     
     */
    
    typedef enum BitIODirection {
        BitIODirectionUnknown      = 0,
        BitIODirectionRight2Left   = 1,
        BitIODirectionLeft2Right   = 2,
    } BitIODirection;
    
#define BitIOByteOrder BitIODirectionRight2Left // aka read from the LSB to the MSB
#define BitIOBitOrder  BitIODirectionLeft2Right // aka read from the MSB to the LSB
    
    static void DetectRuntimeBitOrder() {
        uint8_t Value = 0x80;
        if (Value == 128) {
            //BIT_ORDER = MSBit;
        } else if (Value == 1) {
            //BIT_ORDER = LSBit;
        }
    }
    
    static void DetectRuntimeByteOrder() {
        uint16_t Value = 0xFF00;
        if (Value == 65280) {
            //BYTE_ORDER = MSByte;
        } else if (Value == 255) {
            //BYTE_ORDER = LSByte;
        }
    }
    
#ifdef  __cplusplus
}
#endif

#endif  /* LIBBITIO_BitIOMacros_H */
