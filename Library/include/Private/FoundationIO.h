/*!
 @header          FoundationIO.h
 @author          Marcus Johnson
 @copyright       2020+
 @version         1.0.0
 @brief           This header contains macros that define FoundationIO's version.
 */

#include "../PlatformIO.h"         /* Included for Platform Independence macros */

#pragma once

#ifndef FoundationIO_Version_H
#define FoundationIO_Version_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define FoundationIO_Version_Major 0
#define FoundationIO_Version_Minor 9
#define FoundationIO_Version_Patch 7

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_Version_H */
