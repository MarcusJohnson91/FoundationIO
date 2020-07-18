/*!
 @header              Constants.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains tables and enums used across FoundationIO.
 */

#include "../PlatformIO.h"               /* Included for Platform Independence macros */
#include "../UnicodeIO/UnicodeIOTypes.h" /* Included for UTFX types */

#pragma once

#ifndef FoundationIO_Constants_H
#define FoundationIO_Constants_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define BitMaskTableSize             8

	static const uint8_t BitMaskTable[BitMaskTableSize] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
	};

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_Constants_H */
