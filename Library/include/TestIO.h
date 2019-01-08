#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifndef FoundationIO_TestIO_H
#define FoundationIO_TestIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header              TestIO.h
     @author              Marcus Johnson
     @copyright           2019+
     @version             1.0.0
     @brief               This header contains types, functions, and tables for automated testing.
     */
    
    /*!
     @abstract                                                  "Gets how accurate the clock is".
     */
    uint64_t                    GetTimerFrequency(void);
    
    /*!
     @abstract                                                  "Gets the time from the highest frequency timer for each platform".
     */
    uint64_t                    GetTime(void);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_TestIO_H */
