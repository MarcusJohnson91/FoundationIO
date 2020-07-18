#!/usr/bin/env bash

# Usage (the HeaderFile WILL BE IRRECOVERABLY DELETED): ./UnicodeTables_Create.sh /HeaderPath/HeaderFile.h
# Dependencies: Curl, XMLStarlet (On Mac install Homebrew from brew.sh then call brew install xmlstarlet)

CreateHeaderFileTop() {
    ScriptSHA=$(shasum "$0" | awk '{print $1}')
    IntegerTableBase2Size=2
    IntegerTableBase8Size=8
    TableBase10Size=10
    IntegerTableBase16Size=16
    DecimalTableBase10Size=11
    DecimalTableScientificSize=14
    DecimalTableHexadecimalSize=21
    MathSeperatorTableSize=4
    NumLineBreakCodePoints=7
    NumBiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Bidi_C='Y'])" "$UCD_Data")
    NumWordBreakCodePoints=18
    NumCurrencyCodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@gc='Sc'])" "$UCD_Data")
    DecimalTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and contains(@nv, '/')])" "$UCD_Data")
    CombiningCharacterClassTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@ccc != '0'])" "$UCD_Data")
    IntegerTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')])" "$UCD_Data")
    GraphemeExtensionSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Gr_Ext = 'Y'])" "$UCD_Data")
    KompatibleNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none'])" -n "$UCD_Data")
    CaseFoldTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')])" "$UCD_Data")
    CanonicalNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dt = 'can'])" -n "$UCD_Data")
    {
        printf "#include \"../../PlatformIO.h\"  /* Included for Platform Independence macros */\n"
        printf "#include \"../UnicodeIOTypes.h\" /* Included for UTFX types */\n\n"
        printf "#pragma once\n\n"
        printf "#ifndef FoundationIO_UnicodeIO_UnicodeTables_H\n"
        printf "#define FoundationIO_UnicodeIO_UnicodeTables_H\n\n"
        printf "#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)\n"
        printf "extern \"C\" {\n"
        printf "#endif\n\n"
        printf "#define ScriptHash %s\n\n" "$ScriptSHA"
        printf "#define UnicodeVersion %s\n\n" "$ReadmeUnicodeVersion"
        printf "#define IntegerTableBase2Size %d\n\n" "$IntegerTableBase2Size"
        printf "#define IntegerTableBase8Size %d\n\n" "$IntegerTableBase8Size"
        printf "#define TableBase10Size %d\n\n" "$TableBase10Size"
        printf "#define IntegerTableBase16Size %d\n\n" "$IntegerTableBase16Size"
        printf "#define DecimalTableBase10Size %d\n\n" "$DecimalTableBase10Size"
        printf "#define DecimalTableScientificSize %d\n\n" "$DecimalTableScientificSize"
        printf "#define DecimalTableHexadecimalSize %d\n\n" "$DecimalTableHexadecimalSize"
        printf "#define MathSeperatorTableSize %d\n\n" "$MathSeperatorTableSize"
        printf "#define LineBreakTableSize %d\n\n" "$NumLineBreakCodePoints"
        printf "#define BiDirectionalControlsTableSize %d\n\n" "$NumBiDirectionalControls"
        printf "#define WordBreakTableSize %d\n\n" "$NumWordBreakCodePoints"
        printf "#define CurrencyTableSize %d\n\n" "$NumCurrencyCodePoints"
        printf "#define DecimalTableSize %d\n\n" "$DecimalTableSize"
        printf "#define CombiningCharacterClassTableSize %d\n\n" "$CombiningCharacterClassTableSize"
        printf "#define IntegerTableSize %d\n\n" "$IntegerTableSize"
        printf "#define GraphemeExtensionTableSize %d\n\n" "$GraphemeExtensionSize"
        printf "#define KompatibleNormalizationTableSize %d\n\n" "$KompatibleNormalizationTableSize"
        printf "#define CaseFoldTableSize %d\n\n" "$CaseFoldTableSize"
        printf "#define CanonicalNormalizationTableSize %d\n\n" "$CanonicalNormalizationTableSize"
    } >> "$HeaderFile"
}

CreateMathConstantTables() {
    printf "    static const UTF32 IntegerTableBase2[IntegerTableBase2Size] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "    };\n\n"
    printf "    static const UTF32 IntegerTableBase8[IntegerTableBase8Size] = {\n"
    printf "        U'0',\n"
    printf "        U'1',\n"
    printf "        U'2',\n"
    printf "        U'3',\n"
    printf "        U'4',\n"
    printf "        U'5',\n"
    printf "        U'6',\n"
    printf "        U'7',\n"
    printf "    };\n\n"
    printf "    static const UTF32 TableBase10[TableBase10Size] = {\n"
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
    printf "    static const UTF32 IntegerTableUppercaseBase16[IntegerTableBase16Size] = {\n"
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
    printf "    static const UTF32 IntegerTableLowercaseBase16[IntegerTableBase16Size] = {\n"
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
    printf "    static const UTF32 DecimalScientificUppercase[DecimalTableScientificSize] = {\n"
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
    printf "    static const UTF32 DecimalScientificLowercase[DecimalTableScientificSize] = {\n"
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
    printf "    static const UTF32 DecimalHexUppercase[DecimalTableHexadecimalSize] = {\n"
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
    printf "    static const UTF32 DecimalHexLowercase[DecimalTableHexadecimalSize] = {\n"
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
    printf "    static const UTF32 MathSeperators[MathSeperatorTableSize] = {\n"
    printf "        U',',\n"
    printf "        U'.',\n"
    printf "        U'\\\'',\n"
    printf "        U'\\\x20',\n"
    printf "    };\n\n"
} >> "$HeaderFile"

CreateLineBreakTable() {
    printf "    static const UTF32 LineBreakTable[LineBreakTableSize] = {\n"
    printf "        0x00000A,\n"
    printf "        0x00000B,\n"
    printf "        0x00000C,\n"
    printf "        0x00000D,\n"
    printf "        0x000085,\n"
    printf "        0x002028,\n"
    printf "        0x002029,\n"
    printf "    };\n\n"
} >> "$HeaderFile"

CreateBiDirectionalControlsTable() {
    printf "    static const UTF32 BiDirectionalControlsTable[BiDirectionalControlsTableSize] = {\n" >> "$HeaderFile"
    BiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Bidi_C = 'Y']" -o '0x' -v @cp -n "$UCD_Data")
    for line in $BiDirectionalControls; do
        echo "$line" | awk -F ':' '{printf "        0x%06X,\n", $1}' >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateWordBreakTable() {
    printf "    static const UTF32 WordBreakTable[WordBreakTableSize] = {\n"
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
} >> "$HeaderFile"

CreateCurrencyTable() {
    printf "    static const UTF32 CurrencyTable[CurrencyTableSize] = {\n" >> "$HeaderFile"
    Currency=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@gc = 'Sc']" -o '0x' -v @cp -n "$UCD_Data")
    for line in $Currency; do
        echo "$line" | awk -F ':' '{printf "        0x%06X,\n", $1}' >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateDecimalTable() {
    CodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and contains(@nv, '/') and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -o '0x' -v @cp -o ':' -v @nv -n "$UCD_Data")
    printf "    static const int32_t DecimalTable[DecimalTableSize][3] = {\n" >> "$HeaderFile"
    for line in $CodePointAndValue; do
        echo "$line" | awk -F '[:/]' '{printf "        {0x%06X, %i, %i},\n", $1, $2, $3}' >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateCombiningCharacterClassTable() {
    printf "    static const UTF32 CombiningCharacterClassTable[CombiningCharacterClassTableSize][2] = {\n" >> "$HeaderFile"
    CombiningCharacterClassCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@ccc != '0']" -o '0x' -v @cp -o ':' -v @ccc -n "$UCD_Data")
    for line in $CombiningCharacterClassCodePointAndValue; do
        echo "$line" | awk -F '[: ]' '{printf "        {0x%06X, %d},\n", $1, $2}' >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateIntegerTable() {
    IntegerTable=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -o '0x' -v @cp -o ':' -v @nv -n "$UCD_Data")
    printf "    static const uint64_t IntegerTable[IntegerTableSize][2] = {\n" >> "$HeaderFile"
    for line in $IntegerTable; do
        echo "$line" | awk -F ':' '{printf "        {0x%06X, %i},\n", $1, $2}' >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateGraphemeExtensionTable() {
    printf "    static const UTF32 GraphemeExtensionTable[GraphemeExtensionTableSize] = {\n" >> "$HeaderFile"
    GraphemeExtensions=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Gr_Ext = 'Y']" -o '0x' -v @cp -n "$UCD_Data")
    for line in $GraphemeExtensions; do
        echo "$line" | awk -F ':' '{printf "        0x%06X,\n", $1}' >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
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
    printf "    static const UTF32 *KompatibleNormalizationTable[KompatibleNormalizationTableSize][2] = {\n" >> "$HeaderFile"
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
        printf "        {U\"%s\", U\"%s\"},\n" "$UnicodePrefixedCodePoint" "$UnicodeReplacementString" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateCaseFoldTable() {
    CaseFold=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')]" -o '0x' -v @cp -o ': ' -v @NFKC_CF -n "$UCD_Data" | sed -e 's/ / 0x/g' -e 's/: /:/g' -e 's/ /|/g')
    printf "    static const UTF32 *CaseFoldTable[CaseFoldTableSize][2] = {\n" >> "$HeaderFile"
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
        printf "        {U\"%s\", U\"%s\"},\n" "$UnicodePrefixedCodePoint" "$UnicodeReplacementString" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateCanonicalNormalizationTable() {
    Canonical=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dm != @cp and @dm != '#' and @dt = 'can']" -o '0x' -v @cp -o ': ' -v @dm -n "$UCD_Data" | sed -e 's/ / 0x/g' -e 's/: /:/g' -e 's/ /|/g')
    printf "    static const UTF32 *CanonicalNormalizationTable[CanonicalNormalizationTableSize][2] = {\n" >> "$HeaderFile"
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
        printf "        {U\"%s\", U\"%s\"},\n" "$CodePoint" "$UnicodeReplacementString" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
}

CreateHeaderFileBottom() {
    printf "#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)\n"
    printf "}\n"
    printf "#endif /* Extern C */\n\n"
    printf "#endif /* FoundationIO_UnicodeIO_UnicodeTables_H */\n"
} >> "$HeaderFile"

DownloadUCD() {
	ZipFileSize=$(curl -sI "https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip" | grep "Content-Length: " | awk '{printf $2}' | sed "s/$(printf '\r')\$//")
    if [ "$ZipFileSize" -lt "$FreeSpaceInBytes" ]; then
        curl -s -N "https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip" -o "$UCD_Folder/ucd.all.flat.zip"
        ZipUncompressedSize=$(zipinfo "$UCD_Folder/ucd.all.flat.zip" | tail -n 1 | awk '{printf $3}' | sed "s/$(printf '\r')\$//")
        if [ "$ZipUncompressedSize" -le "$FreeSpaceInBytes" ]; then
            echo "Generating the Unicode tables..."

            rm -rf "$HeaderFile"
            touch "$HeaderFile"

            unzip -q "$UCD_Folder/ucd.all.flat.zip" -d "$UCD_Folder"
            rm -f "$UCD_Folder/ucd.all.flat.zip"

            UCD_Data="$UCD_Folder/ucd.all.flat.xml"
		else
			echo "Couldn't extract UCD, Exiting."
			exit 0
		fi
	else
		echo "Couldn't download UCD, Exiting."
		exit 0
	fi
}

CreateTables() {
	CreateHeaderFileTop

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

    CreateHeaderFileBottom
}

# Check that there's exactly 1 argument to the program
# If there is 1 argument, check if the file exists, 
# if the file exists check the version number against the latest release of Unicode
# If the file does not exist create it and start downloading the UCd and writing the tables

HeaderFile=$1
TempFolder=$(mktemp -d)
TempPath=$(dirname "$TempFolder")
rmdir "$TempFolder"
FreeSpaceInBytes=$(df -H "$TempPath" | awk '{printf $7}' | cut -c 6-)

if [ $# -eq 1 ]; then
	XMLStarletPath=$(command -v xmlstarlet)
	UnzipPath=$(command -v unzip)
	CurlPath=$(command -v curl)
	SHAPath=$(command -v shasum)
	
	if [ -n "$XMLStarletPath" ] && [ -n "$UnzipPath" ] && [ -n "$CurlPath" ] && [ -n "$SHAPath" ]; then
		# We're good, we can now test if the headerfile exists.
		UCD_Folder=$(mktemp -d)
        
        ReadmeSize=$(curl -sI "https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt" | grep "Content-Length: " | awk '{printf $2}' | sed "s/$(printf '\r')\$//")
        if [ "$ReadmeSize" -lt "$FreeSpaceInBytes" ]; then
            curl -s "https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt" -o "$UCD_Folder/readme.txt"
            ReadmeUnicodeVersion=$(grep 'XML Representation of Unicode ' "$UCD_Folder/readme.txt" | awk '{printf $5}')
            ReadmeUnicodeVMajor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $1}')
            ReadmeUnicodeVMinor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $2}')
            ReadmeUnicodeVPatch=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $3}')
            
            if [ -e "$HeaderFile" ]; then
                HeaderScriptHash=$(grep '#define ScriptHash ' "$HeaderFile" | awk '{printf $3}')
                
                HeaderUnicodeVersion=$(grep '#define UnicodeVersion ' "$HeaderFile" | awk '{printf $3}')
                HeaderUnicodeVMajor=$(echo "$HeaderUnicodeVersion" | awk -F "." '{printf $1}')
                HeaderUnicodeVMinor=$(echo "$HeaderUnicodeVersion" | awk -F "." '{printf $2}')
                HeaderUnicodeVPatch=$(echo "$HeaderUnicodeVersion" | awk -F "." '{printf $3}')
                
                if [ "$HeaderScriptHash" != "$ScriptSHA" ]; then
                    rm "$HeaderFile"
                    touch "$HeaderFile"
                    DownloadUCD
                    CreateTables
                elif [ "$HeaderUnicodeVPatch" -lt "$ReadmeUnicodeVPatch" ] || [ "$HeaderUnicodeVMinor" -lt "$ReadmeUnicodeVMinor" ] || [ "$HeaderUnicodeVMajor" -lt "$ReadmeUnicodeVMajor" ]; then
                    rm "$HeaderFile"
                    touch "$HeaderFile"
                    DownloadUCD
                    CreateTables
                else
                    echo "The Unicode tables are already up to date, Exiting."
                    exit 0
                fi
            else
                touch "$HeaderFile"
                DownloadUCD
                CreateTables
            fi
        else
            echo "Not enough free space to download the ReadMe, Exiting."
            exit 0
        fi
    else
        echo "You must install the following tools:"
        if [ -z "$CurlPath" ]; then
            echo "Curl"
        fi
        
        if [ -z "$SHAPath" ]; then
            echo "SHASum"
        fi
        
        if [ -z "$UnzipPath" ]; then
            echo "Unzip"
        fi
        
        if [ -z "$XMLStarletPath" ]; then
            echo "XMLStarlet"
        fi
    fi
else
	echo "The first and only argument must be the header file to overwrite."
fi

# Unicode CLDR Parsing too; download: http://unicode.org/Public/cldr/latest/cldr-common-X.X.zip en.xml from /common/main
