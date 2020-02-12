#include "Macros.h"

#pragma once

#ifndef FoundationIO_Constants_H
#define FoundationIO_Constants_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if   (((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS) && (FoundationIOStandardVersion >= FoundationIOStandardVersionC2X))
	typedef               char32_t                             UTF32;
#else
	typedef               uint_least32_t                       UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */

#define BitMaskTableSize             8

#define IntegerTableBase2Size        2

#define IntegerTableBase8Size        8

#define TableBase10Size             10

#define IntegerTableBase16Size      16

#define DecimalTableBase10Size      11

#define DecimalTableScientificSize  14

#define DecimalTableHexadecimalSize 21

#define MathSeperatorTableSize       4
    
    /*!
     @enum       FoundationIOBases
     @abstract                       "Defines the type of option".
     */
    typedef enum FoundationIOBases {
                 Base_Unknown        = 0,
                 Base_Integer        = 1,   // Integer
                 Base_Radix2         = 2,   // Integer Only
                 Base_Radix8         = 4,   // Integer Only
                 Base_Decimal        = 8,   // Decimal
                 Base_Radix10        = 16,  // Integer | Decimal
                 Base_Radix16        = 32,  // Integer | Decimal
                 Base_Uppercase      = 64,  // Only compatible with Base_Radix16
                 Base_Lowercase      = 128, // Only compatible with Base_Radix16
                 Base_Scientific     = 256, // Decimal Only
                 Base_Shortest       = 512, // Decimal Only
    } FoundationIOBases;

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

#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_Constants_H */
