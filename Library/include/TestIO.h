/*!
 @header              TestIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for automated testing.
 */

#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifndef FoundationIO_TestIO_H
#define FoundationIO_TestIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @abstract                                                  "Gets how accurate the clock is".
     */
    uint64_t                    GetTimerFrequency(void);
    
    /*!
     @abstract                                                  "Gets the time from the highest frequency timer for each platform".
     */
    uint64_t                    GetTime(void);
    
    /*!
     @typedef                   Entropy
     @abstract                                                  "Contains random data".
     */
    typedef struct              Entropy                          Entropy;
    
    /*!
     @abstract                                                  "Initalizes our random data".
     @param                     Size                            "Size has 1 added, and multiplies it by 32 to work with AES-256; the max buffer size is 8192 bytes".
     */
    Entropy                    *Entropy_Init(uint8_t Size);
    
    
    /*!
     @abstract                                                  "Deinitalizes our random data".
     @param                     Entropy                         "Pointer to Entropy source to deinitalize".
     */
    void                        Entropy_Deinit(Entropy *Entropy);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_TestIO_H */
