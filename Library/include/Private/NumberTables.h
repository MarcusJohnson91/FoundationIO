#include <stdint.h>

#pragma once

#ifndef FoundationIO_NumberTables_H
#define FoundationIO_NumberTables_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char32_t                             UTF32;
#else
    typedef               uint_least32_t                       UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32            (4)
#define                   U32(QuotedLiteral)                  U##QuotedLiteral
#endif /* FoundationIO_Unicodize32 */
    
#define IntegerTableBase2Size        2
#define IntegerTableBase8Size        8
#define IntegerTableBase10Size      10
#define IntegerTableBase16Size      16
#define DecimalTableBase10Size      11
#define DecimalTableScientificSize  14
#define DecimalTableHexadecimalSize 21
#define BitMaskTableSize             8
    
    static const UTF32 IntegerTableBase2[IntegerTableBase2Size] = {
        U32('0'), U32('1')
    };
    
    static const UTF32 IntegerTableBase8[IntegerTableBase8Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7')
    };
    
    static const UTF32 IntegerTableBase10[IntegerTableBase10Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9')
    };
    
    static const UTF32 IntegerTableUppercaseBase16[IntegerTableBase16Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'), U32('E'), U32('F')
    };
    
    static const UTF32 IntegerTableLowercaseBase16[IntegerTableBase16Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'), U32('e'), U32('f')
    };
    
    static const UTF32 DecimalTable[DecimalTableBase10Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('.')
    };
    
    static const UTF32 DecimalScientificUppercase[DecimalTableScientificSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('E'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 DecimalScientificLowercase[DecimalTableScientificSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('e'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 DecimalHexUppercase[DecimalTableHexadecimalSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'),
        U32('E'), U32('F'), U32('P'), U32('X'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 DecimalHexLowercase[DecimalTableHexadecimalSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'),
        U32('e'), U32('f'), U32('p'), U32('x'), U32('.'), U32('+'), U32('-')
    };
    
    static const uint8_t MSBitMaskTable[BitMaskTableSize] = {0xFF, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
    // {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
    
    static const uint8_t LSBitMaskTable[BitMaskTableSize] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_NumberTables_H */
