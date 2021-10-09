/*!
 @header          TextIOTables.h
 @author          Marcus Johnson
 @copyright       2018+
 @version         1.3.0
 @brief           This header contains table declarations used across FoundationIO for Unicode and character set conversion.
 @remark          ScriptHash is to know if the script has changed since the tables were last generated.
 */

#include "../../PlatformIO.h"  /* Included for Platform Independence macros */
#include "../TextIOTypes.h"    /* Included for the Text types */

#pragma once

#ifndef FoundationIO_TextIO_TextIOTables_H
#define FoundationIO_TextIO_TextIOTables_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define ScriptHash 2372a6e60ae0feb9db7dd1240c6b32825f86a5a5

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

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase2[IntegerTableBase2Size];

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase8[IntegerTableBase8Size];

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase10[IntegerTableBase10Size];

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase16Uppercase[IntegerTableBase16Size];

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase16Lowercase[IntegerTableBase16Size];

    PlatformIO_HiddenSymbol extern uint64_t const     IntegerValueTable[IntegerValueTableSize][2];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalTableScientificUppercase[DecimalTableScientificSize];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalTableScientificLowercase[DecimalTableScientificSize];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalTableHexadecimalUppercase[DecimalTableHexadecimalSize];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalTableHexadecimalLowercase[DecimalTableHexadecimalSize];

    PlatformIO_HiddenSymbol extern int32_t const      DecimalValueTable[DecimalValueTableSize][3];

    PlatformIO_HiddenSymbol extern UTF32 const        MathSeperators[MathSeperatorTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        LineBreakTable[LineBreakTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        BiDirectionalControlsTable[BiDirectionalControlsTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        WordBreakTable[WordBreakTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        CurrencyTable[CurrencyTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        CombiningCharacterClassTable[CombiningCharacterClassTableSize][2];

    PlatformIO_HiddenSymbol extern UTF32 const        GraphemeExtensionTable[GraphemeExtensionTableSize];

    PlatformIO_HiddenSymbol extern const UTF32 *const KompatibleNormalizationTable[KompatibleNormalizationTableSize][2];

    PlatformIO_HiddenSymbol extern const UTF32 *const CaseFoldTable[CaseFoldTableSize][2];

    PlatformIO_HiddenSymbol extern const UTF32 *const CanonicalNormalizationTable[CanonicalNormalizationTableSize][2];

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_TextIOTables_H */
