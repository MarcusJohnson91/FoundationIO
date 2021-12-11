/*!
 @header      TestIO.h
 @author      Marcus Johnson
 @copyright   2021+
 @version     2.0.0
 @brief       This header contains types, functions, and tables for automated testing.
 @terminology
 Case:        Individual test.
 Fixture:     Operate on a user defined type.
 Suite:       Group of similar test cases.
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

    /* Disable warnings about redefining the Suite macro */
#if   PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsClang)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmacro-redefined"
#elif PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsGCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmacro-redefined"
#elif PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsMSVC)
#pragma warning (push)
#pragma warning (disable : 4005)
#endif

    /*!
     @enum         TestIO_TestStates
     @abstract                                    Is the test enabled or disabled?
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
     @abstract                                    Did the test pass or fail?
     @constant     Outcome_Unspecified            Invalid state.
     @constant     Outcome_Passed                 The test passed.
     @constant     Outcome_Failed                 The test failed.
     */
    typedef enum TestIO_TestOutcomes {
        Outcome_Unspecified            = 0,
        Outcome_Passed                 = 1,
        Outcome_Failed                 = 2,
    } TestIO_TestOutcomes;

    /*!
     @abstract                                    Forward Declaration of CryptographyIO's SecureRNG.
     */
    typedef struct SecureRNG                      SecureRNG;
    typedef        TestIO_TestOutcomes          (*TestIO_TestFunction)(SecureRNG *Secure);

    typedef struct TestIO_Enviroment {
        jmp_buf Enviroment;
    } TestIO_Enviroment;

    typedef void*                               (*TestIO_FixtureInit)(PlatformIO_Unused(TestIO_Enviroment *Enviroment));
    typedef void                                (*TestIO_FixtureDeinit)(void *Deinit, PlatformIO_Unused(TestIO_Enviroment *Enviroment));

    /*!
     @abstract                                    Defines a test case.
     @variable     Name                           The name of the test.
     @variable     Function                       A function pointer to the actual test.
     @variable     TimeElapsed                    The amount of time it took the test to run.
     @variable     State                          Is the test enbled or disabled?
     @variable     Expectation                    Should the test pass or fail, what is expected to happen?
     @variable     Result                         The actual observed value of the test after running.
     */
    typedef struct TestIO_Case {
        TestIO_TestFunction            Function;
        const UTF8                    *Name;
        uint64_t                       TimeElapsed;
        TestIO_TestStates              State;
        TestIO_TestOutcomes            Expectation;
        TestIO_TestOutcomes            Result;
    } TestIO_Case;

    typedef struct TestIO_Suite {
        void                          *FixtureState;
        TestIO_FixtureInit             Init;
        TestIO_FixtureDeinit           Deinit;
        const TestIO_Case             *Tests;
        size_t                         NumTests;
        const UTF8                    *Name;
    } TestIO_Suite;

    extern const TestIO_Suite NULLSuite;

#ifndef          TestIO_Internal_NumSuites
#define          TestIO_Internal_NumSuites 0
#endif /* TestIO_Internal_NumSuites */

#ifndef          TestIO_Internal_SuiteNames
#define          TestIO_Internal_SuiteNames NULLSuite
#endif /* TestIO_Internal_SuiteNames */

#ifndef          TestIO_Internal_PushMacro
#define          TestIO_Internal_PushMacro(MacroName) _Pragma("push_macro(" MacroName ")")
#endif /* TestIO_Internal_Suite_SaveNames */

#ifndef          TestIO_Internal_Suite_SaveNames
#define          TestIO_Internal_Suite_SaveNames _Pragma("push_macro(\"TestIO_Internal_SuiteNames\")")
#ifdef           TestIO_Internal_SuiteNames
#undef           TestIO_Internal_SuiteNames
#endif
#endif /* TestIO_Internal_Suite_SaveNames */

#ifndef          TestIO_Internal_Suite_RestoreNames
#define          TestIO_Internal_Suite_RestoreNames _Pragma("pop_macro(\"TestIO_Internal_SuiteNames\")")
#endif /* TestIO_Internal_Suite_RestoreNames */

#ifndef TestIO_Internal_List_AddSuite
#define TestIO_Internal_List_AddSuite(TestSuite_Name) TestIO_Internal_Suite_SaveNames
#define TestIO_Internal_SuiteNames TestSuite_Name
#endif /* TestIO_Internal_List_AddSuite */

    /* We also need to create a counter variable to store the number of cases */
#ifndef TestIO_ReserveCasesForSuite
#define TestIO_ReserveCasesForSuite(TestSuite_Name)
#if !defined( TestIO_Suite_CasesFor_##TestSuite_Name ) /* Must have the ugly ass space so it's not concatted */
#define TestIOSuite_CasesFor_##TestSuite_Name
#else
#error "TestIOSuite_CasesFor_##TestSuite_Name is already registered, are you trying to create two suites with the same name?"
#endif /* is TestIO_Suite_CasesFor_##TestSuiteName defined? */
#endif /* TestIO_ReserveCasesForSuite */

#ifndef TestIO_CreateTestCounterForSuite
#define TestIO_CreateTestCounterForSuite(TestSuite_Name)
#ifndef TestIOSuite_##TestSuite_Name##_NumTests
#define TestIOSuite_##TestSuite_Name##_NumTests
#else
#error "TestIOSuite_##TestSuite_Name##_NumTests is already registered, are you trying to create two suites with the same name?"
#endif /* TestIO_CreateTestCounterForSuite */

#ifndef TestIO_Internal_IncrementNumTests
#define TestIO_Internal_IncrementNumTests(TestSuite_Name)
    __increment(TestIOSuite_##TestSuite_Name##_NumTests)
#endif /* TestIO_Internal_IncrementNumTests */

#ifndef TestIO_Internal_RegisterTestWithSuite
#define TestIO_Internal_RegisterTestWithSuite(TestSuite_Name, TestCase_Name)
    _Pragma("push_macro(\"TestIOSuite_CasesFor_##TestSuite_Name\")")
#undef PlatformIO_Expand(TestIOSuite_CasesFor_##TestSuite_Name)
#define PlatformIO_Expand(TestIOSuite_CasesFor_##TestSuite_Name) TestCase_Name
#endif /* TestIO_Internal_RegisterTestWithSuite */

#ifndef TestIO_RegisterSuite
#define TestIO_RegisterSuite(TestSuite_Name)                                                                 \
TestIO_ReserveCasesForSuite(TestSuite_Name)                                                                  \
TestIO_CreateTestCounterForSuite(TestSuite_Name)                                                             \
static const TestIO_Suite PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name)) = {         \
.Init   = NULL,                                                                                      \
.Deinit = NULL,                                                                                      \
.Name   = PlatformIO_Stringify8(PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))), \
};
    __increment(TestIO_Internal_NumSuites) /* Increment the count of TestIO_Suites each time RegisterSuite* macros are called */
#define TestIO_Internal_SuiteNames PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))
    _Pragma("push_macro(\"TestIO_Internal_SuiteNames\")")
#undef TestIO_Internal_SuiteNames
#endif /* TestIO_RegisterSuite */

#ifndef TestIO_RegisterSuiteWithFixtures
#define TestIO_RegisterSuiteWithFixtures(TestSuite_Name, Fixture_Init, Fixture_Deinit)                      \
TestIO_ReserveCasesForSuite(TestSuite_Name)                                                                 \
TestIO_CreateTestCounterForSuite(TestSuite_Name)                                                            \
static const TestIO_Suite PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name)) = {        \
.Init   = Fixture_Init,                                                                             \
.Deinit = Fixture_Deinit,                                                                           \
.Name   = PlatformIO_Stringify8(PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))),\
};
    __increment(TestIO_Internal_NumSuites)
#define TestIO_Internal_SuiteNames PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))
    _Pragma("push_macro(\"TestIO_Internal_SuiteNames\")")
#undef TestIO_Internal_SuiteNames
#endif /* TestIO_RegisterSuiteWithFixtures */

#ifndef            TestIO_RegisterTest
#define            TestIO_RegisterTest(TestSuite_Name, Function2Test, TestState, Outcome) \
TestIO_Internal_IncrementNumTests(TestSuite_Name)                                         \
static const TestIO_Case TestIOCase_##Function2Test = {                                   \
.Function    = Function2Test,                                                             \
.State       = TestState,                                                                 \
.Expectation = Outcome,                                                                   \
.Name        = PlatformIO_Stringify(Function2Test),                                       \
};
#endif /* TestIO_RegisterTest */

    /*
     I need an ArraySet of SuiteNames

     #define TestIO_GetSuiteName _Pragma(pop_macro("TestIO_Internal_SuiteName"))
     static const TestIO_Suite **SuiteArraySet = {
     [__COUNTER__] = &TestIO_GetSuiteName;
     ];

     that's all fine and dandy; the problem is, how do I wrap the index to the address-of part into a damn loop at compile time?

     As for push_macro/pop_macro, each time the name macro is set, we could set a count variable
     */

    /*
     Register each suite:

     So, we have a macro that contains the number of suites, which is defined as __COUNTER__ + 1

     then we somehow have a macro that loops over the counter macro until it's 0, decrementing each time.

     we set each Test_Suite's name to a macro and use push_macro to add them all to the same variable.

     then we loop over the thing, using pop_macro to get the name of the registered suite.
     */

    void           TestIO_Summarize(TestIO_Suite *Suite);

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
    UTF32         *TestIO_Random_GetString(SecureRNG *Random, uint64_t NumCodePoints);

    /* Re-enable warnings about redefining macros */
#if   PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsClang)
#pragma clang diagnostic pop
#elif PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsGCC)
#pragma GCC diagnostic pop
#elif PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsMSVC)
#pragma warning(pop)
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TestIO_H */
