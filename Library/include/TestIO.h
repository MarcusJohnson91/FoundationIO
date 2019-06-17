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
    
#ifndef TestIO_RegisterTest
#define TestIO_RegisterTest(FunctionName, ...)
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
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_TestIO_H */
