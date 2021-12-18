/*!
 @header          TextIOTables.h
 @author          Marcus Johnson
 @copyright       2018+
 @version         1.3.0
 @brief           This header contains table declarations used across FoundationIO for Unicode and character set conversion.
 @remark          ScriptHash is to know if the script has changed since the tables were last generated.
 */

#include "../TextIOTypes.h"    /* Included for the Text types */

#pragma once

#ifndef FoundationIO_TextIO_TextIOTables_H
#define FoundationIO_TextIO_TextIOTables_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define ScriptHash d70e6a5e1fef4745a0815a012784e0871f5b0c4c

#define UnicodeVersion 14.0.0

    typedef enum TextIOConstants {
        IntegerTableBase2Size            = 2,
        IntegerTableBase8Size            = 8,
        IntegerTableBase10Size           = 10,
        IntegerTableBase16Size           = 16,
        IntegerValueTableSize            = 1749,
        DecimalTableBase10Size           = 11,
        DecimalTableScientificSize       = 14,
        DecimalTableHexadecimalSize      = 21,
        DecimalValueTableSize            = 123,
        MathSeperatorTableSize           = 4,
        LineBreakTableSize               = 7,
        BiDirectionalControlsTableSize   = 12,
        WordBreakTableSize               = 18,
        CurrencyTableSize                = 63,
        CombiningCharacterClassTableSize = 912,
        GraphemeExtensionTableSize       = 2270,
        KompatibleNormalizationTableSize = 2056,
        CaseFoldTableSize                = 6255,
        CanonicalNormalizationTableSize  = 13233,
    } TextIOConstants;

    PlatformIO_Private extern const UTF32        IntegerTableBase2[IntegerTableBase2Size];

    PlatformIO_Private extern const UTF32        IntegerTableBase8[IntegerTableBase8Size];

    PlatformIO_Private extern const UTF32        IntegerTableBase10[IntegerTableBase10Size];

    PlatformIO_Private extern const UTF32        IntegerTableBase16Uppercase[IntegerTableBase16Size];

    PlatformIO_Private extern const UTF32        IntegerTableBase16Lowercase[IntegerTableBase16Size];

    PlatformIO_Private extern const uint64_t     IntegerValueTable[IntegerValueTableSize][2];

    PlatformIO_Private extern const UTF32        DecimalTableScientificUppercase[DecimalTableScientificSize];

    PlatformIO_Private extern const UTF32        DecimalTableScientificLowercase[DecimalTableScientificSize];

    PlatformIO_Private extern const UTF32        DecimalTableHexadecimalUppercase[DecimalTableHexadecimalSize];

    PlatformIO_Private extern const UTF32        DecimalTableHexadecimalLowercase[DecimalTableHexadecimalSize];

    PlatformIO_Private extern const int32_t      DecimalValueTable[DecimalValueTableSize][3];

    PlatformIO_Private extern const UTF32        MathSeperators[MathSeperatorTableSize];

    PlatformIO_Private extern const UTF32        LineBreakTable[LineBreakTableSize];

    PlatformIO_Private extern const UTF32        BiDirectionalControlsTable[BiDirectionalControlsTableSize];

    PlatformIO_Private extern const UTF32        WordBreakTable[WordBreakTableSize];

    PlatformIO_Private extern const UTF32        CurrencyTable[CurrencyTableSize];

    PlatformIO_Private extern const UTF32        CombiningCharacterClassTable[CombiningCharacterClassTableSize][2];

    PlatformIO_Private extern const UTF32        GraphemeExtensionTable[GraphemeExtensionTableSize];

    PlatformIO_Private extern const UTF32 *const KompatibleNormalizationTable[KompatibleNormalizationTableSize][2];

    PlatformIO_Private extern const UTF32 *const CaseFoldTable[CaseFoldTableSize][2];

    PlatformIO_Private extern const UTF32 *const CanonicalNormalizationTable[CanonicalNormalizationTableSize][2];

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_TextIOTables_H */
