/*!
 @header:                  ExecutableIO.h
 @author:                  Marcus Johnson
 @copyright:               2021+
 @version:                 2.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains macros for compile-time registration via creating named sections in the generated binary.
 */

#pragma once

#include "PlatformIO.h"                    /* Included for Platform Independence macros */

#ifndef FoundationIO_ExecutableIO_H
#define FoundationIO_ExecutableIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifndef
ExecutableIO_Register

#if (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)

/*!
@abstract General purpose registration macro, it's at the top of this header so it can be used by ExecutableIO_Header and ExecutableIO_Footer, in user code those macros must be called at the top and bottom of their translation units respectively.
*/
#define ExecutableIO_Register(SectionName) \
_Pragma(section("\""SectionName"\"", read)); \
__declspec(allocate("\"" SectionName "\""))

#else /* Compiler is Clang or gcc */

#if (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsMachO)

#define ExecutableIO_Register(SectionName) \
__attribute__((section("\""__DATA,SectionName"\""), used)) 

#elif (PlatformIO_ExecutableFormat != PlatformIO_ExecutableFormatIsMachO)

#define ExecutableIO_Register(SectionName) \
__attribute__((section("\""SectionName"\""), used)) 

#endif /* Executable Format */
#endif /* Compiler */
#endif /* ExecutableIO_Register */


#ifndef ExecutableIO_Header

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
/*!
@abstract Call this macro once per section per translation unit, this macro exists only to create a symbol that represents the beginning of the named section.
*/
#define ExecutableIO_Header(SectionName) \
ExecutableIO_Register(SectionName) extern UTF8 *ExecutableIO_Start_##SectionName = u8"\""SectionName"\"";

#endif /* ExecutableIO_Header */

#ifndef RegistrationIO_Footer

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
/*!
@abstract Call this macro once per section per translation unit, this macro exists only to create a symbol that represents the beginning of the named section.
*/
#define ExecutableIO_Footer(SectionName) \
ExecutableIO_Register(SectionName) extern UTF8 *ExecutableIO_Stop_##SectionName = u8"\""SectionName"\"";

#endif /* ExecutableIO_Footer */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_ExecutableIO_H */
