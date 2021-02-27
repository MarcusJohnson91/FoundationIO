/*!
 @header          TextIOTables.h
 @author          Marcus Johnson
 @copyright       2018+
 @version         1.2.0
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

#define ScriptHash caca09a112c1ee277f12b2d6d46fedf81f39bb81

#define UnicodeVersion 13.0.0

    typedef enum TextIOConstants {
        IntegerTableBase2Size            = 2,
        IntegerTableBase8Size            = 8,
        TableBase10Size                  = 10,
        IntegerTableBase16Size           = 16,
        DecimalTableBase10Size           = 11,
        DecimalTableScientificSize       = 14,
        DecimalTableHexadecimalSize      = 21,
        MathSeperatorTableSize           = 4,
        LineBreakTableSize               = 7,
        BiDirectionalControlsTableSize   = 12,
        WordBreakTableSize               = 18,
        CurrencyTableSize                = 62,
        DecimalTableSize                 = 123,
        CombiningCharacterClassTableSize = 872,
        IntegerTableSize                 = 1739,
        GraphemeExtensionTableSize       = 2149,
        KompatibleNormalizationTableSize = 2056,
        CaseFoldTableSize                = 6156,
        CanonicalNormalizationTableSize  = 13233,
    } TextIOConstants;

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase2[IntegerTableBase2Size];

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase8[IntegerTableBase8Size];

    PlatformIO_HiddenSymbol extern UTF32 const        TableBase10[TableBase10Size];

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableUppercaseBase16[IntegerTableBase16Size];

    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableLowercaseBase16[IntegerTableBase16Size];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalScientificUppercase[DecimalTableScientificSize];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalScientificLowercase[DecimalTableScientificSize];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalHexUppercase[DecimalTableHexadecimalSize];

    PlatformIO_HiddenSymbol extern UTF32 const        DecimalHexLowercase[DecimalTableHexadecimalSize];

    PlatformIO_HiddenSymbol extern UTF32 const        MathSeperators[MathSeperatorTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        LineBreakTable[LineBreakTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        BiDirectionalControlsTable[BiDirectionalControlsTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        WordBreakTable[WordBreakTableSize];

    PlatformIO_HiddenSymbol extern UTF32 const        CurrencyTable[CurrencyTableSize];

    PlatformIO_HiddenSymbol extern int32_t const      DecimalTable[DecimalTableSize][3];

    PlatformIO_HiddenSymbol extern UTF32 const        CombiningCharacterClassTable[CombiningCharacterClassTableSize][2];

    PlatformIO_HiddenSymbol extern uint64_t const     IntegerTable[IntegerTableSize][2];

    PlatformIO_HiddenSymbol extern UTF32 const        GraphemeExtensionTable[GraphemeExtensionTableSize];

    PlatformIO_HiddenSymbol extern const UTF32 *const KompatibleNormalizationTable[KompatibleNormalizationTableSize][2];

    PlatformIO_HiddenSymbol extern const UTF32 *const CaseFoldTable[CaseFoldTableSize][2];

    PlatformIO_HiddenSymbol extern const UTF32 *const CanonicalNormalizationTable[CanonicalNormalizationTableSize][2];

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_TextIOTables_H */
