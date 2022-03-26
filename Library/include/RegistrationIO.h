/*!
 @header      RegistrationIO.h
 @author      Marcus Johnson
 @copyright   2021+
 @version     1.0.0
 @brief       This header contains macros for compile-time registration.
 */

#pragma once

#include "PlatformIO.h"                    /* Included for Platform Independence macros */

#ifndef FoundationIO_RegistrationIO_H
#define FoundationIO_RegistrationIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

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
__declspec(allocate(#SectionName$z)) extern SectionType __stop_##SectionName;           \
static SectionType RegistrationIO_Internal_Start##SectionName = &__start_##SectionName; \
static SectionType RegistrationIO_Internal_Stop##SectionName  = &__stop_##SectionName;
#define RegistrationIO_Section_Start (RegistrationIO_Internal_Start##SectionName)
#define RegistrationIO_Section_Stop  (RegistrationIO_Internal_Stop##SectionName)
#else /**Compiler is Clang or gcc */
#if   (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsMachO)
#define RegistrationIO_Section_Create(SectionName, SectionType)                                                                                \
__attribute__((section("__DATA," #SectionName), used)) extern SectionType __start_##SectionName = "TestIO_Start";                              \
__attribute__((section("__DATA," #SectionName), used)) extern SectionType __stop_##SectionName  = "TestIO_Stop";                               \
__attribute__((section("__DATA," #SectionName), used)) static SectionType RegistrationIO_Internal_Start##SectionName = &__start_##SectionName; \
__attribute__((section("__DATA," #SectionName), used)) static SectionType RegistrationIO_Internal_Stop##SectionName  = &__stop_##SectionName;
#define RegistrationIO_Section_Start (RegistrationIO_Internal_Start##SectionName)
#define RegistrationIO_Section_Stop  (RegistrationIO_Internal_Stop##SectionName)
#elif (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsElf || PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsPE)
#define RegistrationIO_Section_Create(SectionName, SectionType)                                                                      \
__attribute__((section(#SectionName), used)) extern SectionType __start_##SectionName;                                               \
__attribute__((section(#SectionName), used)) extern SectionType __stop_##SectionName;                                                \
__attribute__((section(#SectionName), used)) static SectionType RegistrationIO_Internal_Start##SectionName = &__start_##SectionName; \
__attribute__((section(#SectionName), used)) static SectionType RegistrationIO_Internal_Stop##SectionName  = &__stop_##SectionName;
#define RegistrationIO_Section_Start (RegistrationIO_Internal_Start##SectionName)
#define RegistrationIO_Section_Stop  (RegistrationIO_Internal_Stop##SectionName)
#endif /* Executable Format */
#endif /* Compiler */
#endif /* RegistrationIO_Section_Create */

#ifndef RegistrationIO_Section_GetAddress
#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
#define RegistrationIO_Section_GetStartAddress(SectionName) __pragma(section(#SectionName$a, read)) extern SectionType &__start_##SectionName;
#define RegistrationIO_Section_GetStopAddress(SectionName) __pragma(section(#SectionName$z, read))  extern SectionType &__stop_##SectionName;
#else /* Compiler == Clang or gcc */
#if   (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsMachO)
#define RegistrationIO_Section_GetStartAddress(SectionName) __attribute__((section(#SectionName), used)) extern SectionType &__start_##SectionName;
#define RegistrationIO_Section_GetStopAddress(SectionName)  __attribute__((section(#SectionName), used)) extern SectionType &__stop_##SectionName;
#elif (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsElf || PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsPE)
#define RegistrationIO_Section_GetStartAddress(SectionName) __attribute__((section(#SectionName), used)) extern SectionType &__start_##SectionName;
#define RegistrationIO_Section_GetStopAddress(SectionName)  __attribute__((section(#SectionName), used)) extern SectionType &__stop_##SectionName;
#endif /* Executable Format */
#endif /* Compiler */
#endif /* RegistrationIO_Section_GetAddress */

/*
 if Clang or gcc: extern int &__start_SectionName; extern int &__stop_SectionName
 if MSVC:
 */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_RegistrationIO_H */
