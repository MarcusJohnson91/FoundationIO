/*!
 @header          TextIOTables.h
 @author          Marcus Johnson
 @copyright       2018+
 @version         1.2.0
 @brief           This header contains tables used across FoundationIO for Unicode and character set conversion.
 @remark          ScriptHash is to know if the script has changed since the header was last created.
 */

#include "../../PlatformIO.h"  /* Included for Platform Independence macros */
#include "../TextIOTypes.h"    /* Included for the Text types */

#pragma once

#ifndef FoundationIO_TextIO_TextIOTables_H
#define FoundationIO_TextIO_TextIOTables_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define ScriptHash 981f8b4f3d56a52757f44e78ddee9636e06bd856

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
    
    extern const UTF32    GraphemeExtensionTable[GraphemeExtensionTableSize];

    extern const UTF32    IntegerTableBase2[IntegerTableBase2Size];

    extern const UTF32    IntegerTableBase8[IntegerTableBase8Size];

    extern const UTF32    TableBase10[TableBase10Size];

    extern const UTF32    IntegerTableUppercaseBase16[IntegerTableBase16Size];

    extern const UTF32    IntegerTableLowercaseBase16[IntegerTableBase16Size];

    extern const UTF32    DecimalScientificUppercase[DecimalTableScientificSize];

    extern const UTF32    DecimalScientificLowercase[DecimalTableScientificSize];

    extern const UTF32    DecimalHexUppercase[DecimalTableHexadecimalSize];

    extern const UTF32    DecimalHexLowercase[DecimalTableHexadecimalSize];

    extern const UTF32    MathSeperators[MathSeperatorTableSize];

    extern const UTF32    LineBreakTable[LineBreakTableSize];

    extern const UTF32    BiDirectionalControlsTable[BiDirectionalControlsTableSize];

    extern const UTF32    WordBreakTable[WordBreakTableSize];

    extern const UTF32    CurrencyTable[CurrencyTableSize];

    extern const int32_t  DecimalTable[DecimalTableSize][3];

    extern const UTF32    CombiningCharacterClassTable[CombiningCharacterClassTableSize][2];

    extern const uint64_t IntegerTable[IntegerTableSize][2];

    extern const UTF32    GraphemeExtensionTable[GraphemeExtensionTableSize];

    extern const UTF32   *KompatibleNormalizationTable[KompatibleNormalizationTableSize][2];

    extern const UTF32   *CaseFoldTable[CaseFoldTableSize][2];

    extern const UTF32   *CanonicalNormalizationTable[CanonicalNormalizationTableSize][2];

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_TextIOTables_H */
