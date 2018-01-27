#include <stdbool.h>
#include <stdint.h>

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
     @version   2.0.0
     @brief     This header contains preprocessor macros for generic functions in BitIO, and cross-platform compatibility.
     */
    
#ifndef   BitIOYesNo
#define   BitIOYesNo
#ifndef    Yes
#define    Yes                 1
#endif  /* Yes */
#ifndef    No
#define    No                  0
#endif  /* No */
#endif  /* BitIOYesNo */
    
static unsigned char  BitIOGlobalByteOrder = 0;
static unsigned char  BitIOGlobalBitOrder  = 0;
    
//#include <locale.h>
    
    //extern char *Locale = setlocale(LC_ALL, "en_US.UTF-8");
    
#ifndef             BitIOUnknownOS
#define             BitIOUnknownOS    0
#endif
    
#ifndef             BitIOPOSIXOS
#define             BitIOPOSIXOS      1
#endif
    
#ifndef             BitIOWindowsOS
#define             BitIOWindowsOS    2
#endif
    
#ifndef             BitIOMacClassicOS
#define             BitIOMacClassicOS 3
#endif
    
#if      defined(macintosh) || defined(Macintosh)
#ifndef             BitIOTargetOS
#define             BitIOTargetOS (BitIOMacClassicOS)
#endif
    
#ifndef             BitIONewLineWithNULLSize
#define             BitIONewLineWithNULLSize    1
static const char   BitIONewLine[2]  = {0x0D, 0x00}; /* \r */
#endif
    
#elif    defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__MACH__) || defined(BSD) || defined(linux) || defined(__linux)
    
#ifndef             BitIOTargetOS
#define             BitIOTargetOS (BitIOPOSIXOS)
#endif
    
#ifndef             BitIONewLineWithNULLSize
#define             BitIONewLineWithNULLSize    1
static const char   BitIONewLine[2]  = {0x0A, 0x00}; /* \n */
#endif
    
#elif    defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WINDOWS__)
    
#ifndef             BitIOTargetOS
#define             BitIOTargetOS (BitIOWindowsOS)
#endif
    
#ifndef             BitIONewLineWithNULLSize
#define             BitIONewLineWithNULLSize    2
static const char   BitIONewLine[3]  = {0x0D, 0x0A, 0x00}; /* \r\n */
#endif
    
#ifndef             restrict
#define             restrict          __restrict
#endif
    
#ifndef             strcasecmp
#define             strcasecmp          stricmp
#endif
    
#ifndef             strncasecmp
#define             strncasecmp        _strnicmp
#endif
    
#undef              fseek
#define             fseek              _fseeki64
    
#undef              ftell
#define             ftell              _ftelli64
    
#endif   /* End OS detection */
    
    /*!
     @enum                      BitIOBitByteOrders
     @constant                  BitIOUnknownBitFirst            "Unknown bit order".
     @constant                  BitIOUnknownByteFirst           "Unknown byte order".
     @constant                  BitIOLSBitFirst                 "Read from the Least Significant Bit  to the Most  Significant, aka right to left".
     @constant                  BitIOLSByteFirst                "Read from the Least Significant Byte to the Most Significant, aka right to left".
     @constant                  BitIOMSBitFirst                 "Read from the Most  Significant Bit  to the Least Significant, aka left to right".
     @constant                  BitIOMSByteFirst                "Read from the Most  Significant Byte to the Least Significant, aka left to right".
     */
    typedef enum BitIOBitByteOrders {
                                BitIOUnknownBitFirst            = 0,
                                BitIOUnknownByteFirst           = 1,
                                BitIOLSBitFirst                 = 2,
                                BitIOLSByteFirst                = 3,
                                BitIOMSBitFirst                 = 4,
                                BitIOMSByteFirst                = 5,
    } BitIOBitByteOrders;
    
    static inline void BitIOGetRuntimeByteBitOrder(void) {
        uint16_t ByteOrderNumber = 0xFF00;
        uint8_t *ByteOrder       = (uint8_t*)&ByteOrderNumber;
        
        uint8_t  BitOrderNumber  = 0x8;
        if (BitOrderNumber == 8) {
            BitIOGlobalBitOrder = BitIOLSBitFirst;
        } else if (BitOrderNumber == 16) {
            BitIOGlobalBitOrder = BitIOMSBitFirst;
        }
        
        if (ByteOrder[0] == 0) {
            BitIOGlobalByteOrder = BitIOLSByteFirst; // LittleEndian
        } else {
            BitIOGlobalByteOrder = BitIOMSByteFirst; // BigEndian
        }
    }
    
#ifdef  __cplusplus
}
#endif

#endif  /* LIBBITIO_BitIOMacros_H */
