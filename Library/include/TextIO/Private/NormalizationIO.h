/*!
 @header:                  NormalizationIO.h
 @author:                  Marcus Johnson
 @copyright:               2024+
 @version:                 1.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains code for TextIO_StringMap which is used by the casefolding, case mapping, and normalization tables.
 */

#pragma once

#ifndef FoundationIO_TextIO_NormalizationIO_H
#define FoundationIO_TextIO_NormalizationIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct TextIO_StringMap TextIO_StringMap;

#ifndef TextIO_StringMap_SetSize
#define TextIO_StringMap_SetSize(Size) 
#if (Size <= 8)
.Replacee |= ((Size - 1) & 0x7) << 21;
#elif (Size > 8)
.ReplacementSizeMinus1 = Size - 1;
#endif /* Size Check */
#endif /* TextIO_StringMap_SetSize */

#ifndef TextIO_StringMap_Init1
#define TextIO_StringMap_Init1(Replacee, Replacement1) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(1) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
};
#endif /* TextIO_StringMap_Init1 */

#ifndef TextIO_StringMap_Init2
#define TextIO_StringMap_Init2(Replacee, Replacement1, Replacement2) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(2) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
};
#endif /* TextIO_StringMap_Init2 */

#ifndef TextIO_StringMap_Init3
#define TextIO_StringMap_Init3(Replacee, Replacement1, Replacement2, Replacement3) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(3) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
};
#endif /* TextIO_StringMap_Init3 */

#ifndef TextIO_StringMap_Init4
#define TextIO_StringMap_Init4(Replacee, Replacement1, Replacement2, Replacement3, Replacement4) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(4) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
};
#endif /* TextIO_StringMap_Init4 */

#ifndef TextIO_StringMap_Init5
#define TextIO_StringMap_Init5(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(5) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
};
#endif /* TextIO_StringMap_Init5 */

#ifndef TextIO_StringMap_Init6
#define TextIO_StringMap_Init6(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(6) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
.Payload1 |= ((Replacement6 & 0x1FFFFF) << 1); \
};
#endif /* TextIO_StringMap_Init6 */

#ifndef TextIO_StringMap_Init7
#define TextIO_StringMap_Init7(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6, Replacement7) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(7) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
.Payload1 |= ((Replacement6 & 0x1FFFFF) << 1); \
.Payload1 |= ((Replacement7 & 0x1FFFFF) << 22); \
};
#endif /* TextIO_StringMap_Init7 */

#ifndef TextIO_StringMap_Init8
#define TextIO_StringMap_Init8(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6, Replacement7, Replacement8) \
TextIO_StringMap = { \
 TextIO_StringMap_SetSize(8) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
.Payload1 |= ((Replacement6 & 0x1FFFFF) << 1); \
.Payload1 |= ((Replacement7 & 0x1FFFFF) << 22); \
.Payload1 |= ((Replacement8 & 0x1FFFFF) << 43); \
};
#endif /* TextIO_StringMap_Init8 */

#ifndef TextIO_StringMap_InitExternal
#define TextIO_StringMap_InitExternal(Replacee, NumReplacements, String) \
TextIO_StringMap = { \
    .Replacee = (Replacee & UnicodeMaxCodePoint), \
    .ReplacementSizeMinus1 = NumReplacements, \
    .Replacement = U"/"String"/", \
};
#endif /* TextIO_StringMap_InitExternal */

#ifndef TextIO_StringMap_Init
#define TextIO_StringMap_Init(Replacee, NumReplacements, Replacements) \
#if (NumReplacements == 1) \
TextIO_StringMap_Init1(Replacee, Replacements)
#elif (NumReplacements == 2) \
TextIO_StringMap_Init2(Replacee, Replacements)
#elif (NumReplacements == 3) \
TextIO_StringMap_Init3(Replacee, Replacements)
#elif (NumReplacements == 4) \
TextIO_StringMap_Init4(Replacee, Replacements)
#elif (NumReplacements == 5) \
TextIO_StringMap_Init5(Replacee, Replacements)
#elif (NumReplacements == 6) \
TextIO_StringMap_Init6(Replacee, Replacements)
#elif (NumReplacements == 7) \
TextIO_StringMap_Init7(Replacee, Replacements)
#elif (NumReplacements == 8) \
TextIO_StringMap_Init8(Replacee, Replacements)
#else
TectIO_StringMap_InitExternal(Replacee, NumReplacements, String)
#endif /* NumReplacements */
#endif /* TextIO_StringMap_Init */

    size_t TextIO_StringMap_UTF8_GetNumCodeUnitsInReplacements(TextIO_StringMap Map);
    
    size_t TextIO_StringMap_UTF16_GetNumCodeUnitsInReplacements(TextIO_StringMap Map);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_NormalizationIO_H */