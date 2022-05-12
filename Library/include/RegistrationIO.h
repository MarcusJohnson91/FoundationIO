/*!
 @header      RegistrationIO.h
 @author      Marcus Johnson
 @copyright   2021+
 @version     1.0.0
 @brief       This header contains macros for compile-time registration via creating named sections in the generated binary.
 */

#pragma once

#include "PlatformIO.h"                    /* Included for Platform Independence macros */

#ifndef FoundationIO_RegistrationIO_H
#define FoundationIO_RegistrationIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifndef RegistrationIO_Section_Create2
#if   PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsMSVC)

#define RegistrationIO_Section_Create2(SectionName) _Pragma("section("\"" #SectionName "\"", read)")

#elif PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsClang) || PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsGCC)
#endif /* Compiler */
#endif /* RegistrationIO_Section_Create2 */









#ifndef RegistrationIO_Section_Append
#if   PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsMSVC)
#define RegistrationIO_Section_Append(SectionName) __declspec(allocate(#SectionName))
#elif PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsClang) || PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsGCC)
#define RegistrationIO_Section_Append(SectionName) __attribute__((section(#SectionName)))
#endif /* Compiler */
#endif /* RegistrationIO_Section_Append */







#ifndef RegistrationIO_Section_FindStart
#if   PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsMSVC)

#define RegistrationIO_Section_FindStart(SectionName) __declspec(allocate(#SectionName))

#elif PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsClang) || PlatformIO_Is(PlatformIO_Compiler, PlatformIO_CompilerIsGCC)
#endif /* Compiler */
#endif /* RegistrationIO_Section_Append */


    /*
     #pragma section(".init$a")
     #pragma section(".init$u")
     #pragma section(".init$z")

     __declspec(allocate(".init$a")) int InitSectionStart = 0;
     __declspec(allocate(".init$z")) int InitSectionEnd   = 0;

     __declspec(allocate(".init$u")) int token1 = 0xdeadbeef;
     __declspec(allocate(".init$u")) int token2 = 0xdeadc0de;

     ----

     #pragma section(".TestIO$a") // Start of the section
     __declspec(allocate(".TestIO$a")) UTF8 *__start_TestIO = u8"TestIO_Start";

     #pragma section(".TestIO$b") // Number of Suites
     __declspec(allocate(".TestIO$b")) size_t __TestIO_NumSuites = TestIO_NumSuites;

     #pragma section(".TestIO$z") // End of the section
     __declspec(allocate(".TestIO$a")) UTF8 *__stop_TestIO = u8"TestIO_Stop";
     */

#ifndef RegistrationIO_Section_Create
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define RegistrationIO_Section_Create(SectionName, SectionType)                         \
__pragma(section("\"" PlatformIO_Concat(SectionName, $a) "\"", read));                  \
__declspec(allocate(PlatformIO_Concat(SectionName, $a))) extern UTF8 *__start_##SectionName;\
__pragma(section(#SectionName$z, read));                                                \
__declspec(allocate(#SectionName$z)) extern UTF8 *__stop_##SectionName;           \
static UTF8 *RegistrationIO_Internal_Start##SectionName = &__start_##SectionName; \
static UTF8 *RegistrationIO_Internal_Stop##SectionName  = &__stop_##SectionName;
#define RegistrationIO_Section_Start (RegistrationIO_Internal_Start##SectionName)
#define RegistrationIO_Section_Stop  (RegistrationIO_Internal_Stop##SectionName)
#else /* Compiler is Clang or gcc */
#if   (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsMachO)
#define RegistrationIO_Section_Create(SectionName)                                                                                \
__attribute__((section("__DATA," #SectionName), used)) extern UTF8 *__start_##SectionName = #SectionName;                                \
__attribute__((section("__DATA," #SectionName), used)) extern UTF8 *__stop_##SectionName  = #SectionName;                                \
__attribute__((section("__DATA," #SectionName), used)) static UTF8 *RegistrationIO_Internal_Start##SectionName = &__start_##SectionName; \
__attribute__((section("__DATA," #SectionName), used)) static UTF8 *RegistrationIO_Internal_Stop##SectionName  = &__stop_##SectionName;
#define RegistrationIO_Section_Start (RegistrationIO_Internal_Start##SectionName)
#define RegistrationIO_Section_Stop  (RegistrationIO_Internal_Stop##SectionName)
#elif (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsElf || PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsPE)
#define RegistrationIO_Section_Create(SectionName)                                                                            \
__attribute__((section(#SectionName), used)) extern UTF8 *__start_##SectionName;                                              \
__attribute__((section(#SectionName), used)) extern UTF8 *__stop_##SectionName;                                               \
__attribute__((section(#SectionName), used)) static UTF8 RegistrationIO_Internal_Start##SectionName = &__start_##SectionName; \
__attribute__((section(#SectionName), used)) static UTF8 RegistrationIO_Internal_Stop##SectionName  = &__stop_##SectionName;
#define RegistrationIO_Section_Start (RegistrationIO_Internal_Start##SectionName)
#define RegistrationIO_Section_Stop  (RegistrationIO_Internal_Stop##SectionName)
#endif /* Executable Format */
#endif /* Compiler */
#endif /* RegistrationIO_Section_Create */

#ifndef RegistrationIO_Section_GetAddress
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define RegistrationIO_Section_GetStartAddress(SectionName, SectionType) __pragma(section(#SectionName$a, read)) extern UTF8 *&__start_##SectionName;
#define RegistrationIO_Section_GetStopAddress(SectionName, SectionType) __pragma(section(#SectionName$z, read))  extern UTF8 *&__stop_##SectionName;
#else /* Compiler == Clang or gcc */
#if   (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsMachO)
#define RegistrationIO_Section_GetStartAddress(SectionName, SectionType) __attribute__((section(#SectionName), used)) extern UTF8 *&__start_##SectionName;
#define RegistrationIO_Section_GetStopAddress(SectionName, SectionType)  __attribute__((section(#SectionName), used)) extern UTF8 *&__stop_##SectionName;
#elif (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsElf || PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsPE)
#define RegistrationIO_Section_GetStartAddress(SectionName, SectionType) __attribute__((section(#SectionName), used)) extern UTF8 *&__start_##SectionName;
#define RegistrationIO_Section_GetStopAddress(SectionName, SectionType)  __attribute__((section(#SectionName), used)) extern UTF8 *&__stop_##SectionName;
#endif /* Executable Format */
#endif /* Compiler */
#endif /* RegistrationIO_Section_GetAddress */

/*
 if Clang or gcc: extern int &__start_SectionName; extern int &__stop_SectionName
 if MSVC:
 */

    /*
     Usage examples:

     TestIO section for test suites
     CodecIO section fo all registered codecs
     StreamIO section for all registered de/multiplexers

     So we need to know the section name which is unique, the type will always be UTF-8.


     ----

     So there's a few stages here, one is to create the section, then to add the variables to the section, and finally to find the section at runtime.

     RegistrationIO_Section_Create(SectionName)

     RegistrationIO_Section_Append(SectionName, VariableName)

     RegistrationIO_Section_FindStart(SectionName)
     RegistrationIO_Section_FindStop(SectionName)
     */

    /*

     Ok, so, to count the number of suites I'd do:
     _Pragma("redefine_macro(\"NumSuites NumSuites + 1\")")

     thn write the number of TestIO suites to the TestIO section by first:

     creating the section with: RegistrationIO_Section_Create(TestIO_Suites)

     then at the end of the header writing:

     size_t NumSuites RegistrationIO_Section_Append;

     then when creating each suite it'd be the same Append stage...
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_RegistrationIO_H */
