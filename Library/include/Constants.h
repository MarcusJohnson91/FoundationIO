/*!
 @header              Constants.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains tables and enums used across FoundationIO.
 */

#include "PlatformIO.h"
#include "UnicodeIO/UnicodeTypes.h" /* Included for UTF32 */

#pragma once

#ifndef FoundationIO_Constants_H
#define FoundationIO_Constants_H

#ifdef __cplusplus
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
    
    /*!
     @enum       FoundationIO_StringTypes
     @constant   StringType_Unknown                    Invalid/Default value
     @constant   StringType_UTF8                       UTF-8
     @constant   StringType_UTF16                      UTF-16
     @constant   StringType_UTF32                      UTF-32
     */
    typedef enum FoundationIO_StringTypes {
                 StringType_Unknown                    = 0,
                 StringType_UTF8                       = 1,
                 StringType_UTF16                      = 2,
                 StringType_UTF32                      = 4,
    } FoundationIO_StringTypes;
    
    /*!
     @enum       FoundationIO_Bases
     @abstract                                         Defines the type of option.
     @constant   Base_Integer                          ORable mask for Integers.
     @constant   Base_Radix2                           Integer only, base-2/binary.
     @constant   Base_Radix8                           Integer only, base-8/octal.
     @constant   Base_Decimal                          ORable mask for floating point numbers.
     @constant   Base_Radix10                          Integer or Decimal, base-10.
     @constant   Base_Radix16                          Integer or Decimal, base-16/hexadecimal.
     @constant   Base_Uppercase                        Base-16 only.
     @constant   Base_Lowercase                        Base-16 only.
     @constant   Base_Scientific                       Decimal only.
     @constant   Base_Shortest                         Decimal only.
     */
    typedef enum FoundationIO_Bases {
                 Base_Unknown                          = 0,
                 Base_Integer                          = 1,
                 Base_Radix2                           = 2,
                 Base_Radix8                           = 4,
                 Base_Decimal                          = 8,
                 Base_Radix10                          = 16,
                 Base_Radix16                          = 32,
                 Base_Uppercase                        = 64,
                 Base_Lowercase                        = 128,
                 Base_Scientific                       = 256,
                 Base_Shortest                         = 512,
    } FoundationIO_Bases;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FoundationIO_Bases operator | (FoundationIO_Bases A, FoundationIO_Bases B) {
            uint16_t A1 = static_cast<uint16_t>(A);
            uint16_t B1 = static_cast<uint16_t>(B);
            return static_cast<FoundationIO_Bases>(A1 | B1);
        }
        
        constexpr inline FoundationIO_Bases operator & (FoundationIO_Bases A, FoundationIO_Bases B) {
            uint16_t A1 = static_cast<uint16_t>(A);
            uint16_t B1 = static_cast<uint16_t>(B);
            return static_cast<FoundationIO_Bases>(A1 & B1);
        }
        
        constexpr inline FoundationIO_Bases operator |= (FoundationIO_Bases A, FoundationIO_Bases B) {
            uint16_t A1 = static_cast<uint16_t>(A);
            uint16_t B1 = static_cast<uint16_t>(B);
            return static_cast<FoundationIO_Bases>(A1 |= B1);
        }
        
        constexpr inline FoundationIO_Bases operator &= (FoundationIO_Bases A, FoundationIO_Bases B) {
            uint16_t A1 = static_cast<uint16_t>(A);
            uint16_t B1 = static_cast<uint16_t>(B);
            return static_cast<FoundationIO_Bases>(A1 &= B1);
        }
    }
#endif /* PlatformIO_Language */

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
