/*!
 @header              TestIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for automated testing.
 @terminology         Test Case: individual test; Test Suite: group of similar test cases;
 */

#include "PlatformIO.h"               /* Included for Platform Independence macros */
#include "CryptographyIO.h"           /* Included for SecureRNG */
#include "UnicodeIO/UnicodeIOTypes.h" /* Included for UTFX types */

#pragma once

#ifndef FoundationIO_TestIO_H
#define FoundationIO_TestIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Test Grouping:
     So, Each Module needs to be tested, BufferIO, FileIO, CommandLineIO, etc
     within each Module there needs to be tests for each data structure and function.
     within each function there neds to be an individual test case.
     */
    
    /*!
     @enum         TestIO_TestStates
     @abstract                                    Defines the state of each test.
     @constant     TestState_Unspecified          Invalid state.
     @constant     TestState_Enabled              The test is enabled.
     @constant     TestState_Disabled             The test is disabled.
     */
    typedef enum TestIO_TestStates {
                   TestState_Unspecified          = 0,
                   TestState_Enabled              = 1,
                   TestState_Disabled             = 2,
    } TestIO_TestStates;
    
    /*!
     @enum         TestIO_TestResults
     @abstract                                    Defines the result of each test.
     @constant     TestResult_Unspecified         Invalid state.
     @constant     TestResult_Passed              The test is enabled.
     @constant     TestResult_Failed              The test is disabled.
     @constant     TestResult_Untested            The test wasn't ran.
     */
    typedef enum TestIO_TestResults {
                   TestResult_Unspecified         = 0,
                   TestResult_Passed              = 1,
                   TestResult_Failed              = 2,
                   TestResult_Untested            = 3,
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
     @abstract                                    Gets how accurate the clock is.
     */
    uint64_t       GetTimerFrequency(void);
    
    /*!
     @abstract                                    Gets the time from the highest frequency timer for each platform.
     */
    uint64_t       GetTime_Elapsed(void);
    
    /*!
     @abstract                                    Generates a valid UTF-32 string, containing up to 8192 CodePoints.
     @param        Random                         Pointer to an instance of SecureRNG, from CryptographyIO.
     @param        NumCodePoints                  The number of CodePoints, for the String's size.
     */
    UTF32         *UTF32_GenerateString(SecureRNG *Random, uint64_t NumCodePoints);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TestIO_H */
