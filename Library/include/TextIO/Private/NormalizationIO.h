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

    typedef struct NormalizationMap NormalizationMap;

#ifndef NormalizationMap_SetSize
#define NormalizationMap_SetSize(Size) 
#if (Size <= 8)
.Replacee |= ((Size - 1) & 0x7) << 21;
#elif (Size > 8)
.ReplacementSizeMinus1 = Size - 1;
#endif /* Size Check */
#endif /* TextIO_StringMap_SetSize */

#ifndef NormalizationMap_Init1
#define NormalizationMap_Init1(Replacee, Replacement1) \
NormalizationMap = { \
 NormalizationMap_SetSize(1) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
};
#endif /* TextIO_StringMap_Init1 */

#ifndef NormalizationMap_Init2
#define NormalizationMap_Init2(Replacee, Replacement1, Replacement2) \
NormalizationMap = { \
 NormalizationMap_SetSize(2) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Internal.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
};
#endif /* TextIO_StringMap_Init2 */

#ifndef NormalizationMap_Init3
#define NormalizationMap_Init3(Replacee, Replacement1, Replacement2, Replacement3) \
NormalizationMap = { \
 NormalizationMap_SetSize(3) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
};
#endif /* NormalizationMap_Init3 */

#ifndef NormalizationMap_Init4
#define NormalizationMap_Init4(Replacee, Replacement1, Replacement2, Replacement3, Replacement4) \
NormalizationMap = { \
 NormalizationMap_SetSize(4) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
};
#endif /* NormalizationMap_Init4 */

#ifndef NormalizationMap_Init5
#define NormalizationMap_Init5(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5) \
NormalizationMap = { \
 NormalizationMap_SetSize(5) \
.Replacee |= (Replacee & 0x1FFFFF); \
.Replacee |= ((Replacement1 & 0x1FFFFF) << 24); \
.Replacee |= ((Replacement2 & 0x7FFFF) << 45); \
.Payload2 |= ((Replacement2 & 0x180000) >> 19); \
.Payload2 |= ((Replacement3 & 0x1FFFFF) << 2); \
.Payload2 |= ((Replacement4 & 0x1FFFFF) << 23); \
.Payload2 |= ((Replacement5 & 0xFFFFF) << 44); \
.Payload1 |= ((Replacement5 & 0x100000) >> 20); \
};
#endif /* NormalizationMap_Init5 */

#ifndef NormalizationMap_Init6
#define NormalizationMap_Init6(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6) \
NormalizationMap = { \
 NormalizationMap_SetSize(6) \
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
#endif /* NormalizationMap_Init6 */

#ifndef NormalizationMap_Init7
#define NormalizationMap_Init7(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6, Replacement7) \
NormalizationMap = { \
 NormalizationMap_SetSize(7) \
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
#endif /* NormalizationMap_Init7 */

#ifndef NormalizationMap_Init8
#define NormalizationMap_Init8(Replacee, Replacement1, Replacement2, Replacement3, Replacement4, Replacement5, Replacement6, Replacement7, Replacement8) \
NormalizationMap = { \
 NormalizationMap_SetSize(8) \
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
#endif /* NormalizationMap_Init8 */

#ifndef NormalizationMap_InitExternal
#define NormalizationMap_InitExternal(Replacee, NumReplacements, String) \
NormalizationMap = { \
    .Replacee = (Replacee & UnicodeMaxCodePoint), \
    .ReplacementSizeMinus1 = NumReplacements, \
    .Replacement = U"/"String"/", \
};
#endif /* NormalizationMap_InitExternal */

#ifndef NormalizationMap_Init
#define NormalizationMap_Init(Replacee, NumReplacements, Replacements) \
#if (NumReplacements == 1) \
NormalizationMap_Init1(Replacee, Replacements)
#elif (NumReplacements == 2) \
NormalizationMap_Init2(Replacee, Replacements)
#elif (NumReplacements == 3) \
NormalizationMap_Init3(Replacee, Replacements)
#elif (NumReplacements == 4) \
NormalizationMap_Init4(Replacee, Replacements)
#elif (NumReplacements == 5) \
NormalizationMap_Init5(Replacee, Replacements)
#elif (NumReplacements == 6) \
NormalizationMap_Init6(Replacee, Replacements)
#elif (NumReplacements == 7) \
NormalizationMap_Init7(Replacee, Replacements)
#elif (NumReplacements == 8) \
NormalizationMap_Init8(Replacee, Replacements)
#else
NormalizationMap_InitExternal(Replacee, NumReplacements, String)
#endif /* NumReplacements */
#endif /* NormalizationMap_Init */

    size_t NormalizationMap_UTF8_GetNumCodeUnitsInReplacements(NormalizationMap Map);
    
    size_t NormalizationMap_UTF16_GetNumCodeUnitsInReplacements(NormalizationMap Map);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_NormalizationIO_H */