/*!
 @header:                  TestIO.h
 @author:                  Marcus Johnson
 @copyright:               2021+
 @version:                 2.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains types, functions, and tables for automated testing.
 @terminology:
  Case:                    Individual test.
  Fixture:                 Operate on a user defined type.
  Suite:                   Group of similar test cases.
 */

#pragma once

#ifndef FoundationIO_TestIO_H
#define FoundationIO_TestIO_H

#include "TextIO/TextIOTypes.h" /* Included for Text types */
#include "ExecutableIO.h"     /* Included for creating binary sections */

#include <setjmp.h>

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifndef TestIO_Internal_SectionAttibute
#if (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#else
#define TestIO_Internal_SectionAttibute __attribute__(section("TestIO"), used)
#endif /* Compiler */

#ifndef TestIO_Internal_Section_Create
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define TestIO_Internal_Section_Create                                                \
    __pragma(section(".TestIO$a", read));                                             \
    __declspec(allocate(".TestIO$a")) extern UTF8 *__start_TestIO = u8"TestIO_Start"; \
    __pragma(section(".TestIO$b", read));                                             \
    __declspec(allocate(".TestIO$b")) size_t __TestIO_NumSuites = TestIO_NumSuites;   \
    __pragma(section(".TestIO$z", read));                                             \
    __declspec(allocate(".TestIO$z")) UTF8 *__stop_TestIO = u8"TestIO_Stop";
#elif (PlatformIO_Compiler == PlatformIO_CompilerIsClang || PlatformIO_Compiler == PlatformIO_CompilerIsGCC)
#if   (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsMachO)
#define TestIO_Internal_Section_Create                                                \
    __attribute__((section("__DATA,TestIO"), used)) extern UTF8 *__start_TestIO = u8"TestIO_Start"; \
    __attribute__((section("__DATA,TestIO"), used)) extern UTF8 *__stop_TestIO  = u8"TestIO_Stop";
#else
    // Elf, PE regardless of compiler
    __attribute__((section("TestIO"), used)) extern UTF8 *__start_TestIO = u8"TestIO_Start"; \
    __attribute__((section("TestIO"), used)) extern UTF8 *__stop_TestIO  = u8"TestIO_Stop";
#endif /* Executable format */
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
    typedef enum TestIO_TestStates : uint8_t {
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
    typedef enum TestIO_TestOutcomes : uint8_t {
        Outcome_Unspecified            = 0,
        Outcome_Passed                 = 1,
        Outcome_Failed                 = 2,
    } TestIO_TestOutcomes;

    /*!
     @typedef      InsecurePRNG
     @abstract                                     Forward declaration from CryptographyIO.
     */
    typedef struct InsecurePRNG                    InsecurePRNG;
    
    typedef        TestIO_TestOutcomes          (*TestIO_TestFunction)(InsecurePRNG *Insecure);

    typedef struct TestIO_Enviroment {
        jmp_buf Enviroment;
    } TestIO_Enviroment;

    typedef void                                (*TestIO_FixtureFunction)(void *Suite); // PlatformIO_Unused(TestIO_Enviroment *Enviroment)

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
        const UTF8                    *Name;
        TestIO_TestFunction            Function;
        TestIO_TestStates              State;
        TestIO_TestOutcomes            Expectation;
        TestIO_TestOutcomes            Result;
    } TestIO_Case;

    typedef struct TestIO_Suite {
        const UTF8                    *Name;
        void                          *FixtureStruct;
        const TestIO_Case             *Tests;
        TestIO_TestFunction            Init;
        TestIO_TestFunction            Deinit;
        size_t                         NumTests;
    } TestIO_Suite;

    extern const TestIO_Case  TestIO_NULLCase;
    extern const TestIO_Suite TestIO_NULLSuite;

#ifndef          TestIO_Internal_NumSuites
#define          TestIO_Internal_NumSuites 0
#endif /* TestIO_Internal_NumSuites */

#ifndef          TestIO_Internal_SuiteNames
#define          TestIO_Internal_SuiteNames TestIO_NULLSuite
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

#ifndef TestIO_RegisterTest
#define TestIO_RegisterTest(Suite_TestCount, Suite_TestList, Suite, Function2Test, TestState, Outcome) \
_Pragma("redefine_macro(\"Suite_TestCount Suite_TestCount + 1\")")       \
_Pragma("push_macro("\""Suite_TestList"\"")"")                           \
_Pragma("redefine_macro(\"Suite_TestList TestIOCase_##Function2Test\")") \
const TestIO_Case TestIOCase_##Function2Test TestIO_Internal_Section = { \
.Name        = PlatformIO_Stringify(Function2Test),                      \
.Function    = Function2Test,                                            \
.State       = TestState,                                                \
.Expectation = Outcome,                                                  \
};
#endif /* TestIO_RegisterTest */

    /* We also need to create a counter variable to store the number of cases */
#ifndef TestIO_CreateTestCounterForSuite
#define TestIO_CreateTestCounterForSuite(TestSuite_Name)
#ifndef TestIOSuite_##TestSuite_Name##_NumTests
#define TestIOSuite_##TestSuite_Name##_NumTests
#else
#error "TestIOSuite_##TestSuite_Name##_NumTests is already registered, are you trying to create two suites with the same name?"
#endif /* TestIO_CreateTestCounterForSuite */

    /*
     When registering a test we need the function pointer of the test, the state, the expected outcome, the suitename, and the list macro
     */

#ifndef TestIO_RegisterSuite
#define TestIO_RegisterSuite(TestSuite_Name)                                                                                        \
TestIO_ReserveCasesForSuite(TestSuite_Name)                                                                                         \
TestIO_CreateTestCounterForSuite(TestSuite_Name)                                                                                    \
static const TestIO_Suite PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name)) TestIO_Internal_Section = {            \
.Init   = NULL,                                                                                                                     \
.Deinit = NULL,                                                                                                                     \
.Name   = PlatformIO_Stringify8(PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))),                                \
};                                                                                                                                  \
_Pragma("redefine_macro(\"TestIO_Internal_NumSuites TestIO_Internal_NumSuites + 1\")")                                              \
#define TestIO_Internal_SuiteNames PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))
    _Pragma("push_macro(\"TestIO_Internal_SuiteNames\")")
#undef TestIO_Internal_SuiteNames
#endif /* TestIO_RegisterSuite */

#ifndef TestIO_RegisterSuiteWithFixtures
#define TestIO_RegisterSuiteWithFixtures(TestSuite_Name, Fixture_Init, Fixture_Deinit)                                              \
TestIO_ReserveCasesForSuite(TestSuite_Name)                                                                                         \
TestIO_CreateTestCounterForSuite(TestSuite_Name)                                                                                    \
const TestIO_Suite PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name)) TestIO_Internal_Section = {                   \
.Init   = Fixture_Init,                                                                                                             \
.Deinit = Fixture_Deinit,                                                                                                           \
.Name   = PlatformIO_Stringify8(PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))),                                \
};                                                                                                                                  \
_Pragma("redefine_macro(\"TestIO_Internal_NumSuites TestIO_Internal_NumSuites + 1\")")                                              \
#define TestIO_Internal_SuiteNames PlatformIO_Concat(TestIOSuite_, PlatformIO_Expand(TestSuite_Name))
    _Pragma("push_macro(\"TestIO_Internal_SuiteNames\")")
#undef TestIO_Internal_SuiteNames
#endif /* TestIO_RegisterSuiteWithFixtures */

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
     @param        Insecure                       Pointer to an instance of Insecure, from CryptographyIO.
     @param        NumCodePoints                  The number of CodePoints, for the String's size.
     */
    UTF32         *TestIO_Random_GetString(InsecurePRNG *Insecure, uint64_t NumCodePoints);

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
