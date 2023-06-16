/*!
 @header:                  RegistrationIO.h
 @author:                  Marcus Johnson
 @copyright:               2021+
 @version:                 2.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains macros for compile-time registration via creating named sections in the generated binary.
 */

#pragma once

#include "PlatformIO.h"                    /* Included for Platform Independence macros */

#ifndef FoundationIO_RegistrationIO_H
#define FoundationIO_RegistrationIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifndef
RegistrationIO_Section_Register

#if (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)

/*!
@abstract General purpose registration macro, it's at the top of this header so it can be used by RegistrationIO_Section_Start and Stop, in user code those macros must be called at the top and bottom of their translation units respectively.
*/
#define RegistrationIO_Section_Register(SectionName) \
_Pragma(section("\""SectionName"\"", read)); \
__declspec(allocate("\"" SectionName "\""))

#else /* Compiler is Clang or gcc */

#if (PlatformIO_ExecutableFormat == PlatformIO_ExecutableFormatIsMachO)

#define RegistrationIO_Section_Register(SectionName) \
__attribute__((section("\""__DATA,SectionName"\""), used)) 

#elif (PlatformIO_ExecutableFormat != PlatformIO_ExecutableFormatIsMachO)

#define RegistrationIO_Section_Register(SectionName) \
__attribute__((section("\""SectionName"\""), used)) 

#endif /* Executable Format */

#endif /* Compiler */
#endif /* RegistrationIO_Section_Register */


#ifndef RegistrationIO_Section_Header

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
/*!
@abstract Call this macro once per section per translation unit, this macro exists only to create a symbol that represents the beginning of the named section.
*/
#define RegistrationIO_Section_Header(SectionName) \
RegistrationIO_Section_Register(SectionName) extern UTF8 *RegistrationIO_Section_Start##SectionName = u8"\""SectionName"\"";

#endif /* RegistrationIO_Section_Header */






#ifndef RegistrationIO_Section_Footer

#if   (PlatformIO_Compiler == PlatformIO_CompilerIsMSVC)
/*!
@abstract Call this macro once per section per translation unit, this macro exists only to create a symbol that represents the beginning of the named section.
*/
#define RegistrationIO_Section_Footer(SectionName) \
RegistrationIO_Section_Register(SectionName) extern UTF8 *RegistrationIO_Section_Stop##SectionName = u8"\""SectionName"\"";

#endif /* RegistrationIO_Section_Footer */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_RegistrationIO_H */
