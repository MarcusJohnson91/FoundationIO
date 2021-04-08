#!/usr/bin/env bash

# Usage (the HeaderFile WILL BE IRRECOVERABLY DELETED): ./TextIOTables_Create.sh
# Dependencies: Curl, XMLStarlet (On Mac install Homebrew from brew.sh then call brew install xmlstarlet)

CreateHeaderFile() {
    IntegerTableBase2Size=2
    IntegerTableBase8Size=8
    TableBase10Size=10
    IntegerTableBase16Size=16
    DecimalTableBase10Size=11
    DecimalTableScientificSize=14
    DecimalTableHexadecimalSize=21
    MathSeperatorTableSize=4
    NumLineBreakCodePoints=7
    NumWordBreakCodePoints=18
    NumBiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Bidi_C='Y'])" "$UCD_Data")
    NumCurrencyCodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@gc='Sc'])" "$UCD_Data")
    DecimalTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and contains(@nv, '/')])" "$UCD_Data")
    CombiningCharacterClassTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@ccc != '0'])" "$UCD_Data")
    IntegerTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')])" "$UCD_Data")
    GraphemeExtensionSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Gr_Ext = 'Y' or @EComp = 'Y' or @EBase = 'Y'])" "$UCD_Data")
    KompatibleNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none'])" -n "$UCD_Data")
    CaseFoldTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')])" "$UCD_Data")
    CanonicalNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dt = 'can'])" -n "$UCD_Data")
    {
        printf "/*!\n"
        printf " @header          TextIOTables.h\n"
        printf " @author          Marcus Johnson\n"
        printf " @copyright       2018+\n"
        printf " @version         1.2.0\n"
        printf " @brief           This header contains table declarations used across FoundationIO for Unicode and character set conversion.\n"
        printf " @remark          ScriptHash is to know if the script has changed since the tables were last generated.\n"
        printf " */\n\n"
        printf "#include \"../../PlatformIO.h\"  /* Included for Platform Independence macros */\n"
        printf "#include \"../TextIOTypes.h\"    /* Included for the Text types */\n\n"
        printf "#pragma once\n\n"
        printf "#ifndef FoundationIO_TextIO_TextIOTables_H\n"
        printf "#define FoundationIO_TextIO_TextIOTables_H\n\n"
        printf "#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)\n"
        printf "extern \"C\" {\n"
        printf "#endif\n\n"
        printf "#define ScriptHash %s\n\n" "$ScriptSHA"
        printf "#define UnicodeVersion %s\n\n" "$ReadmeUnicodeVersion"
        printf "    typedef enum TextIOConstants {\n"
        printf "        IntegerTableBase2Size            = %u,\n" "$IntegerTableBase2Size"
        printf "        IntegerTableBase8Size            = %u,\n" "$IntegerTableBase8Size"
        printf "        TableBase10Size                  = %u,\n" "$TableBase10Size"
        printf "        IntegerTableBase16Size           = %u,\n" "$IntegerTableBase16Size"
        printf "        DecimalTableBase10Size           = %u,\n" "$DecimalTableBase10Size"
        printf "        DecimalTableScientificSize       = %u,\n" "$DecimalTableScientificSize"
        printf "        DecimalTableHexadecimalSize      = %u,\n" "$DecimalTableHexadecimalSize"
        printf "        MathSeperatorTableSize           = %u,\n" "$MathSeperatorTableSize"
        printf "        LineBreakTableSize               = %u,\n" "$NumLineBreakCodePoints"
        printf "        BiDirectionalControlsTableSize   = %u,\n" "$NumBiDirectionalControls"
        printf "        WordBreakTableSize               = %u,\n" "$NumWordBreakCodePoints"
        printf "        CurrencyTableSize                = %u,\n" "$NumCurrencyCodePoints"
        printf "        DecimalTableSize                 = %u,\n" "$DecimalTableSize"
        printf "        CombiningCharacterClassTableSize = %u,\n" "$CombiningCharacterClassTableSize"
        printf "        IntegerTableSize                 = %u,\n" "$IntegerTableSize"
        printf "        GraphemeExtensionTableSize       = %u,\n" "$GraphemeExtensionSize"
        printf "        KompatibleNormalizationTableSize = %u,\n" "$KompatibleNormalizationTableSize"
        printf "        CaseFoldTableSize                = %u,\n" "$CaseFoldTableSize"
        printf "        CanonicalNormalizationTableSize  = %u,\n" "$CanonicalNormalizationTableSize"
        printf "    } TextIOConstants;\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase2[IntegerTableBase2Size];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableBase8[IntegerTableBase8Size];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        TableBase10[TableBase10Size];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableUppercaseBase16[IntegerTableBase16Size];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        IntegerTableLowercaseBase16[IntegerTableBase16Size];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        DecimalScientificUppercase[DecimalTableScientificSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        DecimalScientificLowercase[DecimalTableScientificSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        DecimalHexUppercase[DecimalTableHexadecimalSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        DecimalHexLowercase[DecimalTableHexadecimalSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        MathSeperators[MathSeperatorTableSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        LineBreakTable[LineBreakTableSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        BiDirectionalControlsTable[BiDirectionalControlsTableSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        WordBreakTable[WordBreakTableSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        CurrencyTable[CurrencyTableSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern int32_t const      DecimalTable[DecimalTableSize][3];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        CombiningCharacterClassTable[CombiningCharacterClassTableSize][2];\n\n"
        printf "    PlatformIO_HiddenSymbol extern uint64_t const     IntegerTable[IntegerTableSize][2];\n\n"
        printf "    PlatformIO_HiddenSymbol extern UTF32 const        GraphemeExtensionTable[GraphemeExtensionTableSize];\n\n"
        printf "    PlatformIO_HiddenSymbol extern const UTF32 *const KompatibleNormalizationTable[KompatibleNormalizationTableSize][2];\n\n"
        printf "    PlatformIO_HiddenSymbol extern const UTF32 *const CaseFoldTable[CaseFoldTableSize][2];\n\n"
        printf "    PlatformIO_HiddenSymbol extern const UTF32 *const CanonicalNormalizationTable[CanonicalNormalizationTableSize][2];\n\n"
        printf "#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)\n"
        printf "}\n"
        printf "#endif /* Extern C */\n\n"
        printf "#endif /* FoundationIO_TextIO_TextIOTables_H */\n"
    } >> "$HeaderFile"
}

CreateSourceFileTop() {
    {
        printf "#include \"../../../include/TextIO/Private/TextIOTables.h\" /* Included for our declarations */\n\n"
        printf "#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)\n"
        printf "extern \"C\" {\n"
        printf "#endif\n\n"
    } >> "$SourceFile"
}

CreateSourceFileBottom() {
    {
        printf "#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)\n"
        printf "}\n"
        printf "#endif /* Extern C */\n"
    } >> "$SourceFile"
}

CreateMathConstantTables() {
    printf "    const UTF32 IntegerTableBase2[IntegerTableBase2Size] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "    };\n\n"
    printf "    const UTF32 IntegerTableBase8[IntegerTableBase8Size] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "    };\n\n"
    printf "    const UTF32 TableBase10[TableBase10Size] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "        U'8',\n"
    printf "        U'9',\n"
    printf "    };\n\n"
    printf "    const UTF32 IntegerTableUppercaseBase16[IntegerTableBase16Size] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "        U'8',\n"
    printf "        U'9',\n"
    printf "        U'A',\n"
    printf "        U'B',\n"
    printf "        U'C',\n"
    printf "        U'D',\n"
    printf "        U'E',\n"
    printf "        U'F',\n"
    printf "    };\n\n"
    printf "    const UTF32 IntegerTableLowercaseBase16[IntegerTableBase16Size] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "        U'8',\n"
    printf "        U'9',\n"
    printf "        U'a',\n"
    printf "        U'b',\n"
    printf "        U'c',\n"
    printf "        U'd',\n"
    printf "        U'e',\n"
    printf "        U'f',\n"
    printf "    };\n\n"
    printf "    const UTF32 DecimalScientificUppercase[DecimalTableScientificSize] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "        U'8',\n"
    printf "        U'9',\n"
    printf "        U'E',\n"
    printf "    };\n\n"
    printf "    const UTF32 DecimalScientificLowercase[DecimalTableScientificSize] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "        U'8',\n"
    printf "        U'9',\n"
    printf "        U'e',\n"
    printf "    };\n\n"
    printf "    const UTF32 DecimalHexUppercase[DecimalTableHexadecimalSize] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "        U'8',\n"
    printf "        U'9',\n"
    printf "        U'A',\n"
    printf "        U'B',\n"
    printf "        U'C',\n"
    printf "        U'D',\n"
    printf "        U'E',\n"
    printf "        U'F',\n"
    printf "        U'P',\n"
    printf "        U'X',\n"
    printf "    };\n\n"
    printf "    const UTF32 DecimalHexLowercase[DecimalTableHexadecimalSize] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "        U'8',\n"
    printf "        U'9',\n"
    printf "        U'a',\n"
    printf "        U'b',\n"
    printf "        U'c',\n"
    printf "        U'd',\n"
    printf "        U'e',\n"
    printf "        U'f',\n"
    printf "        U'p',\n"
    printf "        U'x',\n"
    printf "    };\n\n"
    printf "    const UTF32 MathSeperators[MathSeperatorTableSize] = {\n"
    printf "        U',',\n"
    printf "        U'.',\n"
    printf "        U'\\\'',\n"
    printf "        U'\\\x20',\n"
    printf "    };\n\n"
} >> "$SourceFile"

CreateLineBreakTable() {
    printf "    const UTF32 LineBreakTable[LineBreakTableSize] = {\n"
    printf "        0x00000A,\n"
    printf "        0x00000B,\n"
    printf "        0x00000C,\n"
    printf "        0x00000D,\n"
    printf "        0x000085,\n"
    printf "        0x002028,\n"
    printf "        0x002029,\n"
    printf "    };\n\n"
} >> "$SourceFile"

CreateBiDirectionalControlsTable() {
    printf "    const UTF32 BiDirectionalControlsTable[BiDirectionalControlsTableSize] = {\n" >> "$SourceFile"
    BiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Bidi_C = 'Y']" -o '0x' -v @cp -n "$UCD_Data")
    for line in $BiDirectionalControls; do
        echo "$line" | awk -F ':' '{printf "        0x%06X,\n", $1}' >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateWordBreakTable() {
    printf "    const UTF32 WordBreakTable[WordBreakTableSize] = {\n"
    printf "        0x000020,\n"
    printf "        0x0000A0,\n"
    printf "        0x001680,\n"
    printf "        0x002000,\n"
    printf "        0x002000,\n"
    printf "        0x002001,\n"
    printf "        0x002002,\n"
    printf "        0x002003,\n"
    printf "        0x002004,\n"
    printf "        0x002005,\n"
    printf "        0x002006,\n"
    printf "        0x002007,\n"
    printf "        0x002008,\n"
    printf "        0x002009,\n"
    printf "        0x00200A,\n"
    printf "        0x00202F,\n"
    printf "        0x00205F,\n"
    printf "        0x003000,\n"
    printf "    };\n\n"
} >> "$SourceFile"

CreateCurrencyTable() {
    printf "    const UTF32 CurrencyTable[CurrencyTableSize] = {\n" >> "$SourceFile"
    Currency=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@gc = 'Sc']" -o '0x' -v @cp -n "$UCD_Data")
    for line in $Currency; do
        echo "$line" | awk -F ':' '{printf "        0x%06X,\n", $1}' >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateDecimalTable() {
    CodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and contains(@nv, '/') and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -o '0x' -v @cp -o ':' -v @nv -n "$UCD_Data")
    printf "    const int32_t DecimalTable[DecimalTableSize][3] = {\n" >> "$SourceFile"
    for line in $CodePointAndValue; do
        echo "$line" | awk -F '[:/]' '{printf "        {0x%06X, %i, %i},\n", $1, $2, $3}' >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateCombiningCharacterClassTable() {
    printf "    const UTF32 CombiningCharacterClassTable[CombiningCharacterClassTableSize][2] = {\n" >> "$SourceFile"
    CombiningCharacterClassCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@ccc != '0']" -o '0x' -v @cp -o ':' -v @ccc -n "$UCD_Data")
    for line in $CombiningCharacterClassCodePointAndValue; do
        echo "$line" | awk -F '[: ]' '{printf "        {0x%06X, %d},\n", $1, $2}' >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateIntegerTable() {
    IntegerTable=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -o '0x' -v @cp -o ':' -v @nv -n "$UCD_Data")
    printf "    const uint64_t IntegerTable[IntegerTableSize][2] = {\n" >> "$SourceFile"
    for line in $IntegerTable; do
        echo "$line" | awk -F ':' '{printf "        {0x%06X, %i},\n", $1, $2}' >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateGraphemeExtensionTable() {
    printf "    const UTF32 GraphemeExtensionTable[GraphemeExtensionTableSize] = {\n" >> "$SourceFile"
    GraphemeExtensions=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Gr_Ext = 'Y' or @EComp = 'Y']" -o '0x' -v @cp -n "$UCD_Data")
    for line in $GraphemeExtensions; do
        echo "$line" | awk -F ':' '{printf "        0x%06X,\n", $1}' >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

AddUnicodePrefix2CodePoint() {
    if [ "$1" -le 160 ]; then
        UnicodePrefixedCodePoint=$(printf "\\\\x%X" "$1")
    elif [ "$1" -le 65535 ]; then
        UnicodePrefixedCodePoint=$(printf "\\\\u%04X" "$1")
    elif [ "$1" -le 1114111 ]; then
        UnicodePrefixedCodePoint=$(printf "\\\\U%08X" "$1")
    fi
}

AddUnicodePrefix2String() {
    if [ "$1" -le 160 ]; then
        UnicodeReplacementString=$(printf "%s\\\\x%X" "$UnicodeReplacementString" "$1")
    elif [ "$1" -le 65535 ]; then
        UnicodeReplacementString=$(printf "%s\\\\u%04X" "$UnicodeReplacementString" "$1")
    elif [ "$1" -le 1114111 ]; then
        UnicodeReplacementString=$(printf "%s\\\\U%08X" "$UnicodeReplacementString" "$1")
    fi
}

CreateKompatibleNormalizationTable() {
    Kompatible=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none']" -o '0x' -v @cp -o ': ' -v @dm -n "$UCD_Data" | sed -e 's/ / 0x/g' -e 's/: /:/g' -e 's/ /|/g')
    printf "    const UTF32 *const KompatibleNormalizationTable[KompatibleNormalizationTableSize][2] = {\n" >> "$SourceFile"
    for line in $Kompatible; do
        CodePoint2Replace=$(echo "$line" | awk -F ':' '{printf "%u", $1}')
        AddUnicodePrefix2CodePoint "$CodePoint2Replace"
        CodePoint="$UnicodePrefixedCodePoint"
        NumCodePoints=$(echo "$line" | awk -F '[:|]' '{print NF}')
        UnicodeReplacementString=""
        for Index in $(seq 2 "$NumCodePoints"); do
            ReplacementCodePoint=$(echo "$line" | awk -F '[:|]' -v Index="$Index" '{printf "%u", $Index}')
            AddUnicodePrefix2String "$ReplacementCodePoint"
        done
        printf "        {U\"%s\", U\"%s\"},\n" "$UnicodePrefixedCodePoint" "$UnicodeReplacementString" >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateCaseFoldTable() {
    CaseFold=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')]" -o '0x' -v @cp -o ': ' -v @NFKC_CF -n "$UCD_Data" | sed -e 's/ / 0x/g' -e 's/: /:/g' -e 's/ /|/g')
    printf "    const UTF32 *const CaseFoldTable[CaseFoldTableSize][2] = {\n" >> "$SourceFile"
    for line in $CaseFold; do
        CodePoint2Replace=$(echo "$line" | awk -F ':' '{printf "%u", $1}')
        AddUnicodePrefix2CodePoint "$CodePoint2Replace"
        CodePoint="$UnicodePrefixedCodePoint"
        NumCodePoints=$(echo "$line" | awk -F '[:|]' '{print NF}')
        UnicodeReplacementString=""
        for Index in $(seq 2 "$NumCodePoints"); do
            ReplacementCodePoint=$(echo "$line" | awk -F '[:|]' -v Index="$Index" '{printf "%u", $Index}')
            AddUnicodePrefix2String "$ReplacementCodePoint"
        done
        printf "        {U\"%s\", U\"%s\"},\n" "$UnicodePrefixedCodePoint" "$UnicodeReplacementString" >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateCanonicalNormalizationTable() {
    Canonical=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dm != @cp and @dm != '#' and @dt = 'can']" -o '0x' -v @cp -o ': ' -v @dm -n "$UCD_Data" | sed -e 's/ / 0x/g' -e 's/: /:/g' -e 's/ /|/g')
    printf "    const UTF32 *const CanonicalNormalizationTable[CanonicalNormalizationTableSize][2] = {\n" >> "$SourceFile"
    for line in $Canonical; do
        CodePoint2Replace=$(echo "$line" | awk -F ':' '{printf "%u", $1}')
        AddUnicodePrefix2CodePoint "$CodePoint2Replace"
        CodePoint="$UnicodePrefixedCodePoint"
        NumCodePoints=$(echo "$line" | awk -F '[:|]' '{print NF}')
        UnicodeReplacementString=""
        for Index in $(seq 2 "$NumCodePoints"); do
            ReplacementCodePoint=$(echo "$line" | awk -F '[:|]' -v Index="$Index" '{printf "%u", $Index}')
            AddUnicodePrefix2String "$ReplacementCodePoint"
        done
        printf "        {U\"%s\", U\"%s\"},\n" "$CodePoint" "$UnicodeReplacementString" >> "$SourceFile"
    done
    printf "    };\n\n" >> "$SourceFile"
}

CreateTables() {
    CreateHeaderFile

    CreateSourceFileTop
    CreateMathConstantTables
    CreateLineBreakTable
    CreateBiDirectionalControlsTable
    CreateWordBreakTable
    CreateCurrencyTable
    CreateDecimalTable
    CreateCombiningCharacterClassTable
    CreateIntegerTable
    CreateGraphemeExtensionTable
    CreateKompatibleNormalizationTable
    CreateCaseFoldTable
    CreateCanonicalNormalizationTable
    CreateSourceFileBottom
}

DownloadUCD() {
	ZipFileSize=$(curl -sI "https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip" | grep "Content-Length: " | awk '{printf $2}' | sed "s/$(printf '\r')\$//")
    if [ "$ZipFileSize" -lt "$FreeSpaceInBytes" ]; then
        curl -s -N "https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip" -o "$TempFolder/ucd.all.flat.zip"
        ZipUncompressedSize=$(zipinfo "$TempFolder/ucd.all.flat.zip" | tail -n 1 | awk '{printf $3}' | sed "s/$(printf '\r')\$//")
        if [ "$ZipUncompressedSize" -le "$FreeSpaceInBytes" ]; then
            unzip -q "$TempFolder/ucd.all.flat.zip" -d "$TempFolder"
            rm -f "$TempFolder/ucd.all.flat.zip"
            UCD_Data="$TempFolder/ucd.all.flat.xml"
		else
			echo "Couldn't extract UCD, Exiting." >&2
			exit 0
		fi
	else
		echo "Couldn't download UCD, Exiting." >&2
		exit 0
	fi
}

CheckUnicodeVersion() {
    # Download the ReadMe, read the Tables Unicode version, do the thang
    ReadmeSize=$(curl -sI "https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt" | grep "Content-Length: " | awk '{printf $2}' | sed "s/$(printf '\r')\$//")
    if [ "$ReadmeSize" -lt "$FreeSpaceInBytes" ]; then
        ReadmeUnicodeVMajor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $1}')
        ReadmeUnicodeVMinor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $2}')
        ReadmeUnicodeVPatch=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $3}')
        HeaderUnicodeVersion=$(grep '#define UnicodeVersion ' "$HeaderFile" | awk '{printf $3}')
        HeaderUnicodeVMajor=$(echo "$HeaderUnicodeVersion" | awk -F "." '{printf $1}')
        HeaderUnicodeVMinor=$(echo "$HeaderUnicodeVersion" | awk -F "." '{printf $2}')
        HeaderUnicodeVPatch=$(echo "$HeaderUnicodeVersion" | awk -F "." '{printf $3}')

        if [ "$HeaderUnicodeVPatch" -lt "$ReadmeUnicodeVPatch" ] || [ "$HeaderUnicodeVMinor" -lt "$ReadmeUnicodeVMinor" ] || [ "$HeaderUnicodeVMajor" -lt "$ReadmeUnicodeVMajor" ]; then
            if [ -f "$HeaderFile" ] && [ -s "$HeaderFile" ]; then
                echo "The following two files will be deleted, press Control-C within 5 seconds to abort..."
                echo "$HeaderFile"
                echo "$SourceFile"
                sleep 5
                echo "Deleting..."
                rm -rf "$HeaderFile"
                rm -rf "$SourceFile"
                echo "Creating Tables..."
                touch "$HeaderFile"
                touch "$SourceFile"
                DownloadUCD
                CreateTables
            else
                DownloadUCD
                echo "Creating Tables..."
                CreateTables
            fi
        else
            echo "The tables are already up to date."
            exit 1
        fi
    else
        echo "Not enough free space to download the ReadMe, Exiting." >&2
        exit 0 # -2 is not enough free space
    fi
}

# Check that there's exactly 1 argument to the program
# If there is 1 argument, check if the Tables exists,
# if the file exists check the version number against the latest release of Unicode
# If the file does not exist create it and start downloading the UCd and writing the tables

LibraryPath="$(dirname "$(dirname "$(dirname "$(echo "$(cd "$(dirname "$0")"; pwd)/$(basename "$1")")")")")"
HeaderFile=$(printf "%s/%s" "$LibraryPath" "include/TextIO/Private/TextIOTables.h")
SourceFile=$(printf "%s/%s" "$LibraryPath" "src/TextIO/Private/TextIOTables.c")
TempFolder=$(mktemp -d)
FreeSpaceInBytes=$(df -H "$TempFolder" | awk '{printf $7}' | cut -c 6-)
curl -s "https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt" -o "$TempFolder/readme.txt"
ReadmeUnicodeVersion=$(grep 'XML Representation of Unicode ' "$TempFolder/readme.txt" | awk '{printf $5}')
SHAPath=$(command -v shasum)
CurlPath=$(command -v curl)
UnzipPath=$(command -v unzip)
XMLStarletPath=$(command -v xmlstarlet)

if [ $# -eq 0 ]; then
    if test -f "$HeaderFile"; then
        # Tables exists, check the version
        if test -x "$SHAPath"; then
            HeaderScriptHash=$(grep '#define ScriptHash ' "$HeaderFile" | awk '{printf $3}')
            ScriptSHA=$(shasum "$0" | awk '{print $1}')
            if [ "$HeaderScriptHash" != "$ScriptSHA" ]; then
                # Update the Tables
                echo "The following two files will be deleted, press Control-C within 5 seconds to abort..."
                echo "$HeaderFile"
                echo "$SourceFile"
                sleep 5
                echo "Deleting..."
                rm -rf "$HeaderFile"
                rm -rf "$SourceFile"
                echo "Creating Tables..."
                touch "$HeaderFile"
                touch "$SourceFile"
                DownloadUCD
                CreateTables
            else
                # Script hash matches, but we still need to know if the tables are current
                # Check the Unicode version
                CheckUnicodeVersion
            fi
        else
            echo "We need 'shasum', install it then re-run the command"
        fi
    else
        if [[ -n "$SHAPath" ]] && [[ -n "$CurlPath" ]] && [[ -n "$UnzipPath" ]] && [[ -n "$XMLStarletPath" ]]; then #! - z
            echo "Creating Tables..."
            touch "$HeaderFile"
            touch "$SourceFile"
            DownloadUCD
            CreateTables
        else
            if [[ -z "$SHAPath" ]]; then
                echo "We need 'shasum', install it then re-run the command"
            fi

            if [[ -z "$CurlPath" ]]; then
                echo "We need 'curl', install it then re-run the command"
            fi

            if [[ -z "$UnzipPath" ]]; then
                echo "We need 'unzip', install it then re-run the command"
            fi
            if [[ -z "$XMLStarletPath" ]]; then
                echo "We need 'XMLStarlet', install it then re-run the command"
            fi
        fi
    fi
fi

# Unicode CLDR Parsing too; download: http://unicode.org/Public/cldr/latest/cldr-common-X.X.zip en.xml from /common/main
