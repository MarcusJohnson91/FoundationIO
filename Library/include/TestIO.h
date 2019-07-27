/*!
 @header              TestIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for automated testing.
 */

#include <stdbool.h>
#include <stdint.h>

#include "Macros.h"

#pragma once

#ifndef FoundationIO_TestIO_H
#define FoundationIO_TestIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__ && __STDC_VERSION__ >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char32_t         UTF32;
#else
    typedef               uint_least32_t   UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
#ifndef TestIO_RegisterTest
#define TestIO_RegisterTest(FunctionName, ...)
#endif
    
    /*!
     @enum                      TestIO_TestStates
     @abstract                                                  "Defines the state of each test".
     @constant                  TestState_Unknown               "Invalid state".
     @constant                  TestState_Enabled               "The test is enabled".
     @constant                  TestState_Disabled              "The test is disabled".
     */
    typedef enum TestIO_TestStates {
                                TestState_Unknown               = 0,
                                TestState_Enabled               = 1,
                                TestState_Disabled              = 2,
    } TestIO_TestStates;
    
    /*!
     @enum                      TestIO_TestResults
     @abstract                                                  "Defines the result of each test".
     @constant                  TestResult_Unknown              "Invalid state".
     @constant                  TestResult_Passed               "The test is enabled".
     @constant                  TestResult_Failed               "The test is disabled".
     @constant                  TestResult_Untested             "The test wasn't ran".
     */
    typedef enum TestIO_TestResults {
                                TestResult_Unknown              = 0,
                                TestResult_Passed               = 1,
                                TestResult_Failed               = 2,
                                TestResult_Untested             = 3,
    } TestIO_TestResults;
    
    typedef struct Entropy Entropy; // Forward declare Entropy from CryptographyIO
    
    /*!
     @abstract                                                  "Gets how accurate the clock is".
     */
    uint64_t                    GetTimerFrequency(void);
    
    /*!
     @abstract                                                  "Gets the time from the highest frequency timer for each platform".
     */
    uint64_t                    GetTime(void);
    
    /*!
     @abstract                                                  "Generates a valid UTF-32 string, containing up to 8192 CodePoints".
     @param                     Random                          "Pointer to an instance of Entropy, from CryptographyIO".
     @param                     NumCodePoints                   "The number of CodePoints, for the String's size".
     */
    UTF32                      *UTF32_GenerateString(Entropy *Random, uint64_t NumCodePoints);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_TestIO_H */
