/*!
 @header          AssertIO.h
 @author          Marcus Johnson
 @copyright       2022+
 @version         1.0.0
 @brief           This header contains code assertions.
 */

#pragma once

#ifndef FoundationIO_AssertIO_H
#define FoundationIO_AssertIO_H

#include "TextIO/TextIOTypes.h"     /* Included for Text types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#if PlatformIO_Language == PlatformIO_LanguageIsC && PlatformIO_LanguageVersionC >= PlatformIO_LanguageVersionC11
#define static_assert _Static_assert
#endif

    void AssertIO_Message(const UTF8 *FileName, const UTF8 *FunctionName, UTF8 *ExpressionString, ...);

#if PlatformIO_BuildType == PlatformIO_BuildTypeIsDebug
#define AssertIO(Expression, ...) ((void)((Expression)||(AssertIO_Message(PlatformIO_FileName, PlatformIO_FunctionName, #Expression, ##__VA_ARGS__),0)))
#else
#define AssertIO(Expression, ...) ((void)0)
#endif /* Build type */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_AssertIO_H */
