#include <stdbool.h>                  /* Included for bool */
#include <stdint.h>                   /* Included for u/intX_t */

#pragma once

#ifndef FoundationIO_Macros_H
#define FoundationIO_Macros_H

#ifdef  __cplusplus
extern  "C" {
#endif
    
    /*!
     @header    Macros.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2017+
     @version   1.0.0
     @brief     This header contains preprocessor macros for generic functions in FoundationIO, and cross-platform compatibility.
     */
    
#ifndef   BoolYesNoTrueFalse
#define   BoolYesNoTrueFalse
#ifndef    Yes
#define    Yes                 1
#endif  /* Yes */
#ifndef    No
#define    No                  0
#endif  /* No */
#ifndef    True
#define    True                1
#endif  /* True */
#ifndef    False
#define    False               0
#endif  /* False */
#endif  /* BoolYesNoTrueFalse */
    
    static unsigned char  GlobalByteOrder = 0;
    static unsigned char  GlobalBitOrder  = 0;
    
#ifndef             UnknownOS
#define             UnknownOS    0
#endif           /* UnknownOS */
    
#ifndef             POSIXOS
#define             POSIXOS      1
#endif           /* POSIXOS */
    
#ifndef             WindowsOS
#define             WindowsOS    2
#endif           /* WindowsOS */
    
#ifndef             MacClassicOS
#define             MacClassicOS 3
#endif           /* MacClassicOS */
    
#if      defined(macintosh) || defined(Macintosh)
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS (MacClassicOS)
#endif
    
#ifndef             NewLineWithNULLSize
#define             NewLineWithNULLSize    1
#define             NewLineUTF8 u8"\r"
#define             NewLineUTF16 u"\r"
#define             NewLineUTF32 U"\r"
#endif
    
#elif    defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__MACH__) || defined(BSD) || defined(linux) || defined(__linux)
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS (POSIXOS)
#endif
    
#ifndef             NewLineWithNULLSize
#define             NewLineWithNULLSize    1
#define             NewLineUTF8 u8"\n"
#define             NewLineUTF16 u"\n"
#define             NewLineUTF32 U"\n"
#endif
    
#ifndef typeof
#define             typeof(Variable)   __typeof__(Variable)
#endif
    
#ifndef qNAN
#define qNANBE 0x7FC00001
#define qNANLE 0x1000C07F
#endif
    
#ifndef NAN
#define NAN qNANBE
#endif
    
#ifndef             FoundationIO_Tell
#define             FoundationIO_Tell(File) ftello(File)
#endif
    
#ifndef             FoundationIO_Seek
#define             FoundationIO_Seek(File, Offset, Origin) fseeko(File, Offset, Origin)
#endif
    
#elif    defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WINDOWS__)
    
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS (WindowsOS)
#endif
    
#ifndef             NewLineWithNULLSize
#define             NewLineWithNULLSize    2
#define             NewLineUTF8 u8"\r\n"
#define             NewLineUTF16 u"\r\n"
#define             NewLineUTF32 U"\r\n"
#endif
    
#ifndef typeof
#define             typeof(Variable)        __typeof(Variable)
#endif
    
#ifndef             restrict
#define             restrict                __restrict
#endif
    
#ifndef             FoundationIO_Tell
#define             FoundationIO_Tell(File)  _ftelli64(File)
#endif
    
#ifndef             FoundationIO_Seek
#define             FoundationIO_Seek(File, Offset, Origin) _fseeki64(File, Offset, Origin)
#endif
    
#elif
#ifndef             FoundationIOTargetOS
#define             FoundationIOTargetOS (UnknownOS)
#endif
    
#endif   /* End OS detection */
    
#define PrivateCountVarArgs(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, Count, ...)(Count)
    
#define CountVariadicArguments(...) PrivateCountVarArgs(0, __VA_ARGS__,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
    
    /*!
     @enum                      ByteBitOrders
     @constant                  UnknownBitFirst            "Unknown bit order".
     @constant                  UnknownByteFirst           "Unknown byte order".
     @constant                  LSBitFirst                 "Read from the Least Significant Bit  to the Most  Significant, aka right to left".
     @constant                  LSByteFirst                "Read from the Least Significant Byte to the Most Significant, aka right to left".
     @constant                  MSBitFirst                 "Read from the Most  Significant Bit  to the Least Significant, aka left to right".
     @constant                  MSByteFirst                "Read from the Most  Significant Byte to the Least Significant, aka left to right".
     */
    typedef enum ByteBitOrders {
                                UnknownBitFirst            = 0,
                                UnknownByteFirst           = 1,
                                LSBitFirst                 = 2,
                                LSByteFirst                = 3,
                                MSBitFirst                 = 4,
                                MSByteFirst                = 5,
    } ByteBitOrders;
    
    void GetRuntimeByteBitOrder(void);
    
    /*!
     @enum                      FoundationIOConstants
     @constant                  NULLTerminator             "The value of a NULL terminator".
     @constant                  NULLTerminatorSize         "The size of a NULL terminator".
     */
    typedef enum FoundationIOConstants {
                                NULLTerminator             = 0,
                                NULLTerminatorSize = 1,
    } FoundationIOConstants;
    
#ifdef  __cplusplus
}
#endif

#endif  /* FoundationIO_Macros_H */
