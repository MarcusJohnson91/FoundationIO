/*!
 @header          TextIOTables.h
 @author          Marcus Johnson
 @copyright       2018+
 @version         1.3.0
 @brief           This header contains table declarations used across FoundationIO for Unicode and character set conversion.
 @remark          ScriptHash is to know if the script has changed since the tables were last generated.
 */

#pragma once

#ifndef FoundationIO_TextIO_TextIOTables_H
#define FoundationIO_TextIO_TextIOTables_H

#include "../TextIOTypes.h"    /* Included for the Text types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define ScriptHash a1f9ba8b56e6c976075d0d2dadb1b1fa340baa7f

#define UnicodeVersion 15.0.0

    typedef enum TextIOConstants {
        IntegerTableBase10Size           = 10,
        IntegerValueTableSize            = 1789,
        DecimalTableBase10Size           = 11,
        DecimalTableScientificSize       = 14,
        DecimalTableHexadecimalSize      = 21,
        DecimalValueTableSize            = 123,
        MathSeperatorTableSize           = 4,
        LineBreakTableSize               = 7,
        BiDirectionalControlsTableSize   = 12,
        WordBreakTableSize               = 18,
        CurrencyTableSize                = 63,
        CombiningCharacterClassTableSize = 922,
        GraphemeExtensionTableSize       = 2307,
        KompatibleNormalizationTableSize = 2082,
        CaseFoldTableSize                = 6317,
        CanonicalNormalizationTableSize  = 13233,
    } TextIOConstants;

    PlatformIO_Private extern const UTF32        IntegerTableBase10[IntegerTableBase10Size];

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
