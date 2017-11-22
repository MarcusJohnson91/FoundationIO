#pragma once

#ifndef   LIBBITIO_BitIOMacros_H
#define   LIBBITIO_BitIOMacros_H

#ifdef __cplusplus
extern "C" {
#endif

#if \
defined(macintosh)       ||\
defined(Macintosh)
#define    BitIONewLineSize 1
static const char BitIONewLine[2]  = {"\x0D\x00"}; // \r
#elif \
defined(_POSIX_C_SOURCE) ||\
defined(__APPLE__)       ||\
defined(__MACH__)        ||\
defined(BSD)             ||\
defined(linux)           ||\
defined(__linux)
#define BitIONewLineSize 1
static  const char BitIONewLine[2]  = {"\x0A\x00"}; // \n
#include <unistd.h>
#elif \
defined(_WIN32)          ||\
defined(__WIN32__)       ||\
defined(_WIN64)          ||\
defined(__WINDOWS__)
#define BitIONewLineSize 2
static  const char BitIONewLine[3]  = {"\x0D\x0A\x00"}; // \r\n
#define restrict   __restrict
#define strcasecmp   stricmp
#define strncasecmp _strnicmp
#endif

#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
    
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
  
#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS = 64
#elif
#define _FILE_OFFSET_BITS = 64
#endif
    
#ifndef   BitIOEndianByteOrders
#define   BitIOEndianByteOrders
#define    UnknownByteOrder 0
#define    LSByte           1
#define    MSByte           2
#endif /* BitIOEndianByteOrders */
    
#ifndef   BitIOEndianBitOrders
#define   BitIOEndianBitOrders
#define    UnknownBitOrder  0
#define    LSBit            1
#define    MSBit            2
#endif /* BitIOEndianBitOrders */
    
#ifndef   BitIOYesNo
#define   BitIOYesNo
#ifndef    Yes
#define    Yes 1
#endif  /* Yes */
#ifndef    No
#define    No  0
#endif  /* No */
#endif /* BitIOYesNo */
    
#ifndef    BITIOBYTEBITORDERS
#define    BITIOBYTEBITORDERS
#ifndef    BitIOLSByte
#define    BitIOLSByte            ULLONG_MAX
    typedef    unsigned long long BitBLSByte_t;
#endif  /* BitIOLittleEndian */
#ifndef    BitIOMSByte
#define    BitIOMSByte            LLONG_MIN
    typedef    signed long long   BitBMSByte_t;
#endif  /* BitIOBigEndian */
#ifndef    BitIOLSBit
#define    BitIOLSBit             ULLONG_MAX
    typedef    unsigned long long BitBLSBit_t;
#endif  /* BitIOLSBit */
#ifndef    BitIOMSBit
#define    BitIOMSBit             LLONG_MIN
    typedef    signed long long   BitBMSBit_t;
#endif  /* BitIOMSBit */
#endif  /* BITIOBYTEBITORDERS */
    
#ifndef    BITIOGUUIDTYPES
#define    BITIOGUUIDTYPES
#ifndef    BitIOGUUIDString
#define    BitIOGUUIDString       ULLONG_MAX
    typedef    unsigned long long GUUIDString_t;
#endif  /* BitIOGUIDString */
#ifndef    BitIOBinaryGUUID
#define    BitIOBinaryGUUID       FLT_MIN
    typedef    float              BinaryGUUID_t;
#endif  /* BitIOBinaryGUID */
#endif  /* BITIOGUUIDTYPES */
    
    
#if defined(__LITTLE_ENDIAN__)||(__ARMEL__)||(__THUMBEL__)||(__AARCH64EL__)||(_MIPSEL)||(__MIPSEL)||(__MIPSEL__)||(__BYTE_ORDER==(__LITTLE_ENDIAN||__ORDER_LITTLE_ENDIAN))
#define RuntimeByteOrder LSByte
#define RuntimeBitOrder  LSBit
#elif defined(__BIG_ENDIAN__)||(__ARMEB__)||(__THUMBEB__)||(__AARCH64EB__)||(_MIPSEB)||(__MIPSEB)||(__MIPSEB__)||(__BYTE_ORDER==(__BIG_ENDIAN||__ORDER_BIG_ENDIAN))
#define RuntimeByteOrder MSByte
#define RuntimeBitOrder  MSBit
#endif
    
#define BitIOArraySizeInElements(Array) (sizeof(Array)/sizeof(Array[0])) /* DOES NOT WORK FOR DYNAMIC ARRAYS */
    
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_BitIOMacros_H */
