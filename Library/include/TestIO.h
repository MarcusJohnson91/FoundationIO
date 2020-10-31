/*!
 @header          TestIO.h
 @author          Marcus Johnson
 @copyright       2019+
 @version         1.0.0
 @brief           This header contains types, functions, and tables for automated testing.
 @terminology     Test Case: individual test; Test Suite: group of similar test cases;
 */

#include "PlatformIO.h"         /* Included for Platform Independence macros */
#include "TextIO/TextIOTypes.h" /* Included for Text types */
#include "TextIO/LogIO.h"       /* Included for Logging, tests need to log errors */

#pragma once

#ifndef FoundationIO_TestIO_H
#define FoundationIO_TestIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct TestIO_Function {

    } FunctionTest;

    /*
     Test Grouping:
     So, Each Module needs to be tested, BufferIO, FileIO, CommandLineIO, etc
     within each Module there needs to be tests for each data structure and function.
     within each function there neds to be an individual test case.

     So, what's the Hierarchy here?

     We can run very small tests, like making sure an individual function for example Minimum works.

     but how do we make sure that larger structures work, like making sure a JPEG decoder works?

     Bottom up testing is the approach I want to use with TestIO.

     Make sure everything works individually, then increase the complexity of the system til it's at full capacity and still works together.

     So, maybe we can break that into phases:

     The Individual Test

     The Compound Test

     FunctionTest and ComponentTest could be the struct names for the subsystems?

     A FunctionTest verifies the behavior of an individual function.

     A ComponentTest verifies everything works together

     A function test takes no arguments, they're just hard coded negative zero positive is the general pattern.

     A Component Test requires some state but how do we Genericify that?

     Well, we can approach it similar to MediaIO; void pointer, ElementSize, NumElements.

     but that would only work for 1D arrays? how do you take the address of a 4D array?

     Well, a 4D array would be an implementation detail that the component would need to create.

     Also, one other thing I'd like to do is store the time it takes for the test to run so we can track that information too.
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
     @enum         TestIO_TestOutcomes
     @abstract                                    Defines the result of each test.
     @constant     Outcome_Unspecified            Invalid state.
     @constant     Outcome_Passed                 The test is enabled.
     @constant     Outcome_Failed                 The test is disabled.
     */
    typedef enum TestIO_TestOutcomes {
                   Outcome_Unspecified     = 0,
                   Outcome_Passed          = 1,
                   Outcome_Failed          = 2,
    } TestIO_TestOutcomes;

    /*!
     @typedef Forward declared from CryptographyIO
     */
    typedef struct SecureRNG SecureRNG;

    typedef bool (*TestIO_TestFunction)(SecureRNG *Secure);

    typedef struct TestCase {
        TestIO_TestFunction  Function;
        TestIO_TestStates    TestState;
        TestIO_TestOutcomes  TestOutcome;
    } TestCase;

    typedef struct TestSuite {
        TestCase *Tests;
        uint64_t *UnexpectedFailues;
        uint64_t  NumTests;
        uint64_t  NumWorkedAsExpected;
        uint64_t  NumUnexpectedFailures;
        uint64_t  UnexpectedFailureSize;
    } TestSuite;

    static void RegisterTestCase(TestSuite *Suite, TestCase *Test) {
        if (Suite != NULL && Test != NULL) {

        } else {
            Suite = calloc(1, sizeof(TestSuite));
        }
    }
    /*
     What if We register each Test in just one Variadic call, then we'd know the size the make the array as well as each index
     */

    void TestIO_RunTests(TestSuite *Suite);
    
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
     @abstract                                     Forward Declaration of CryptographyIO's SecureRNG.
     */
    typedef struct SecureRNG                      SecureRNG;
    
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
