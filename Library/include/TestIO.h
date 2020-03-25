/*!
 @header              TestIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for automated testing.
 */

#include "Macros.h"                   /* Included for Platform Independence macros */
#include "CryptographyIO.h"           /* Included for Entropy */
#include "UnicodeIO/UnicodeTypes.h"

#pragma once

#ifndef FoundationIO_TestIO_H
#define FoundationIO_TestIO_H

#ifdef __cplusplus
extern "C" {
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
    
#ifndef TESTIO_ARGUMENT
#define TESTIO_ARGUMENT(...) __VA_ARGS__
#endif
    
#ifndef TESTIO_CAT
#define TESTIO_CAT(TOKEN1, TOKEN2, TOKEN3) TOKEN1 ## TOKEN2 ## _ ## TOKEN3
#endif
    
#ifndef TestIO_CreateFunctionName
#define TestIO_CreateFunctionName(Module, Function) TESTIO_CAT(Test_, Module, Function)
#endif
    
    /*!
     @abstract                                                  "Gets how accurate the clock is".
     */
    uint64_t                    GetTimerFrequency(void);
    
    /*!
     @abstract                                                  "Gets the time from the highest frequency timer for each platform".
     */
    uint64_t                    GetTime_Elapsed(void);
    
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
