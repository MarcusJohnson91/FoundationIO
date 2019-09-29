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
#define                   UTF32String(Literal)                (UTF32*) U##Literal
#define                   UTF32Character(Literal)             (UTF32) U##Literal
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
        UTF32Character('0'), UTF32Character('1')
    };
    
    static const UTF32 IntegerTableBase8[IntegerTableBase8Size] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'),
        UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7')
    };
    
    static const UTF32 IntegerTableBase10[IntegerTableBase10Size] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'),
        UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'),
        UTF32Character('8'), UTF32Character('9')
    };
    
    static const UTF32 IntegerTableUppercaseBase16[IntegerTableBase16Size] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'),
        UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'),
        UTF32Character('8'), UTF32Character('9'), UTF32Character('A'), UTF32Character('B'),
        UTF32Character('C'), UTF32Character('D'), UTF32Character('E'), UTF32Character('F')
    };
    
    static const UTF32 IntegerTableLowercaseBase16[IntegerTableBase16Size] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'),
        UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'),
        UTF32Character('8'), UTF32Character('9'), UTF32Character('a'), UTF32Character('b'),
        UTF32Character('c'), UTF32Character('d'), UTF32Character('e'), UTF32Character('f')
    };
    
    static const UTF32 DecimalTable[DecimalTableBase10Size] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'), UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'), UTF32Character('8'), UTF32Character('9'), UTF32Character('.')
    };
    
    static const UTF32 DecimalScientificUppercase[DecimalTableScientificSize] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'), UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'), UTF32Character('8'), UTF32Character('9'), UTF32Character('E'), UTF32Character('.'), UTF32Character('+'), UTF32Character('-')
    };
    
    static const UTF32 DecimalScientificLowercase[DecimalTableScientificSize] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'), UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'), UTF32Character('8'), UTF32Character('9'), UTF32Character('e'), UTF32Character('.'), UTF32Character('+'), UTF32Character('-')
    };
    
    static const UTF32 DecimalHexUppercase[DecimalTableHexadecimalSize] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'), UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'), UTF32Character('8'), UTF32Character('9'), UTF32Character('A'), UTF32Character('B'), UTF32Character('C'), UTF32Character('D'),
        UTF32Character('E'), UTF32Character('F'), UTF32Character('P'), UTF32Character('X'), UTF32Character('.'), UTF32Character('+'), UTF32Character('-')
    };
    
    static const UTF32 DecimalHexLowercase[DecimalTableHexadecimalSize] = {
        UTF32Character('0'), UTF32Character('1'), UTF32Character('2'), UTF32Character('3'), UTF32Character('4'), UTF32Character('5'), UTF32Character('6'), UTF32Character('7'), UTF32Character('8'), UTF32Character('9'), UTF32Character('a'), UTF32Character('b'), UTF32Character('c'), UTF32Character('d'),
        UTF32Character('e'), UTF32Character('f'), UTF32Character('p'), UTF32Character('x'), UTF32Character('.'), UTF32Character('+'), UTF32Character('-')
    };
    
    static const uint8_t BitMaskTable[BitMaskTableSize] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_NumberTables_H */
