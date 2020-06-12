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

#define IntegerTableBase2Size        2

#define IntegerTableBase8Size        8

#define TableBase10Size             10

#define IntegerTableBase16Size      16

#define DecimalTableBase10Size      11

#define DecimalTableScientificSize  14

#define DecimalTableHexadecimalSize 21

#define MathSeperatorTableSize       4

	static const uint8_t BitMaskTable[BitMaskTableSize] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
	};

	static const UTF32 IntegerTableBase2[IntegerTableBase2Size] = {
		U'0', U'1'
	};

	static const UTF32 IntegerTableBase8[IntegerTableBase8Size] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7'
	};

	static const UTF32 TableBase10[TableBase10Size] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9'
	};

	static const UTF32 IntegerTableUppercaseBase16[IntegerTableBase16Size] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'A', U'B',
		U'C', U'D', U'E', U'F'
	};

	static const UTF32 IntegerTableLowercaseBase16[IntegerTableBase16Size] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'a', U'b',
		U'c', U'd', U'e', U'f'
	};

	static const UTF32 DecimalScientificUppercase[DecimalTableScientificSize] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'E'
	};

	static const UTF32 DecimalScientificLowercase[DecimalTableScientificSize] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'e'
	};

	static const UTF32 DecimalHexUppercase[DecimalTableHexadecimalSize] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'A',
		U'B', U'C', U'D', U'E', U'F', U'P', U'X'
	};

	static const UTF32 DecimalHexLowercase[DecimalTableHexadecimalSize] = {
		U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'a',
		U'b', U'c', U'd', U'e', U'f', U'p', U'x'
	};

	static const UTF32 MathSeperators[MathSeperatorTableSize] = {
		U',', U'.', U'\'', U' '
	};

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* FoundationIO_Constants_H */
