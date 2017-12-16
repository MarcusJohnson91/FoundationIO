#include <limits.h>
#include <float.h>

#pragma once

#ifndef LIBBITIO_BitIOMacros_H
#define LIBBITIO_BitIOMacros_H

#ifdef  __cplusplus
extern  "C" {
#endif
    
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
static   const char BitIONewLine[2]  = {"\x0D\x00"}; /* \r */
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
static   const char BitIONewLine[2]  = {"\x0A\x00"}; /* \n */
#endif
    
#elif    defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WINDOWS__)

#ifndef             BitIOTargetOS
#define             BitIOTargetOS (BitIOWindowsOS)
#endif

#ifndef             BitIONewLineSize
#define             BitIONewLineSize    2
static   const char BitIONewLine[3]  = {"\x0D\x0A\x00"}; /* \r\n */
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
    
#define BitIOArraySizeInElements(Array) (sizeof(Array)/sizeof(Array[0])) /* DOES NOT WORK FOR DYNAMIC ARRAYS */
    
#ifdef  __cplusplus
}
#endif

#endif  /* LIBBITIO_BitIOMacros_H */
