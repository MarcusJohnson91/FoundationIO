#!/usr/bin/env sh

# Usage (the HeaderFile WILL BE IRRECOVERABLY DELETED): ./MakeUnicodeTables.sh /HeaderPath/HeaderFile.h
# Dependencies: Curl, XMLStarlet (On Mac install Homebrew from brew.sh then call brew install xmlstarlet)

CreateOutputFileTop() {
    printf "#include \"StringIO.h\" /* Included for our declarations */\n\n" >> "$OutputFile"
    printf "#pragma once\n\n" >> "$OutputFile"
    printf "#ifndef FoundationIO_StringIOTables_H\n" >> "$OutputFile"
    printf "#define FoundationIO_StringIOTables_H\n\n" >> "$OutputFile"
    printf "#ifdef __cplusplus\n" >> "$OutputFile"
    printf "extern \"C\" {\n" >> "$OutputFile"
    printf "#endif\n\n" >> "$OutputFile"
    printf "#define UnicodeVersion %s\n\n" "$ReadmeUnicodeVersion" >> "$OutputFile"
    NumWhiteSpaceCodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@WSpace='Y'])" "$UCD_Data")
    printf "#define WhiteSpaceTableSize %d\n\n" "$NumWhiteSpaceCodePoints" >> "$OutputFile"
    NumBiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Bidi_C='Y'])" "$UCD_Data")
    printf "#define BiDirectionalControlsTableSize %d\n\n" "$NumBiDirectionalControls" >> "$OutputFile"
    CombiningCharacterClassTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@ccc != '0'])" "$UCD_Data")
    printf "#define CombiningCharacterClassTableSize %d\n\n" "$CombiningCharacterClassTableSize" >> "$OutputFile"
    IntegerTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')])" "$UCD_Data")
    printf "#define IntegerTableSize %d\n\n" "$IntegerTableSize" >> "$OutputFile"
    GraphemeExtensionSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Gr_Ext = 'Y'])" "$UCD_Data")
    printf "#define GraphemeExtensionTableSize %d\n\n" "$GraphemeExtensionSize" >> "$OutputFile"
    CaseFoldTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')])" "$UCD_Data")
    printf "#define CaseFoldTableSize %d\n\n" "$CaseFoldTableSize" >> "$OutputFile"
    KompatibleNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none'])" -n "$UCD_Data")
    printf "#define KompatibleNormalizationTableSize %d\n\n" "$KompatibleNormalizationTableSize" >> "$OutputFile"
    CanonicalNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dt = 'can'])" -n "$UCD_Data")
    printf "#define CanonicalNormalizationTableSize %d\n\n" "$CanonicalNormalizationTableSize" >> "$OutputFile"
}

CreateWhiteSpaceTable() {
    IFS='
'
    printf "    static const UTF32    WhiteSpaceTable[WhiteSpaceTableSize] = {\n" >> "$OutputFile"
    WhiteSpace=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@WSpace = 'Y']" -v @cp -n "$UCD_Data")
    for line in $WhiteSpace; do
        Value=$(echo "$line" | sed -e 's/^/0x/g')
        printf "        0x%06X,\n" "$Value" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateBiDirectionalControlsTable() {
    IFS='
'
    printf "    static const UTF32    BiDirectionalControlsTable[BiDirectionalControlsTableSize] = {\n" >> "$OutputFile"
    BiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Bidi_C = 'Y']" -v @cp -n "$UCD_Data")
    for line in $BiDirectionalControls; do
        Value=$(echo "$line" | sed -e 's/^/0x/g')
        printf "        0x%06X,\n" "$Value" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateCombiningCharacterClassTable() {
    IFS='
'
    printf "    static const UTF32    CombiningCharacterClassTable[CombiningCharacterClassTableSize][2] = {\n" >> "$OutputFile"
    CombiningCharacterClassCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@ccc != '0']" -v @cp -o : -v @ccc -n "$UCD_Data")
    for line in $CombiningCharacterClassCodePointAndValue; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        Value=$(echo "$line" | awk -F '[: ]' '{printf $2}')
        printf "        {0x%06X, %d},\n" "$CodePoint" "$Value" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateIntegerTable() {
    IFS='
'
    Colon=':'
    SortedCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -v "@cp" -o : -v "@nv" -n "$UCD_Data")
    printf "    static const UTF32    IntegerCodePoints[IntegerTableSize] = {\n" >> "$OutputFile"
    for line in $SortedCodePointAndValue; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    printf "    static const uint64_t IntegerValues[IntegerTableSize] = {\n" >> "$OutputFile"
    for line2 in $SortedCodePointAndValue; do
        Value=$(echo "$line2" | awk -F ":" '{printf $2}')
        printf "        %d,\n" "$Value" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateGraphemeExtensionTable() {
    IFS='
'
    printf "    static const UTF32    GraphemeExtensionTable[GraphemeExtensionTableSize] = {\n" >> "$OutputFile"
    CodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Gr_Ext = 'Y']" -v "@cp" -n "$UCD_Data")
    for line in $CodePoints; do
        Value=$(echo "$line" | sed -e 's/^/0x/g')
        printf "        0x%06X,\n" "$Value" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateCanonicalNormalizationTables() {
    IFS='
'
    CanonicalNormalizationCodePointsAndStrings=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dm != @cp and @dm != '#' and @dt = 'can']" -v @cp -o : -v @dm -n "$UCD_Data")
    printf "    static const UTF32    CanonicalNormalizationCodePoints[CanonicalNormalizationTableSize] = {\n" >> "$OutputFile"
    for line in $CanonicalNormalizationCodePointsAndStrings; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    printf "    static const UTF32   *CanonicalNormalizationStrings[CanonicalNormalizationTableSize] = {\n" >> "$OutputFile"
    for line2 in $CanonicalNormalizationCodePointsAndStrings; do
        CanonicalNormalizationString=$(echo "$line2" | awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}')
        ReplacementString=""
        for CodePoint2 in $CanonicalNormalizationString; do # CodePoint2 is already prefixed with 0x
            DecimalCodePoint=$(printf "%d" "$CodePoint2")
            if [ "$DecimalCodePoint" -le 160 ]; then
                ReplacementString+=$(printf '\\x%X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -le 65535 ]; then
                ReplacementString+=$(printf '\\u%04X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -gt 65535 ]; then
                ReplacementString+=$(printf '\\U%08X' "$CodePoint2")
            fi
        done
    printf "        U\"%s\",\n" $ReplacementString >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateKompatibleNormalizationTables() {
    IFS='
'
    KompatibleNormalizationCodePointsAndStrings=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none']" -v @cp -o : -v @dm -n "$UCD_Data")
    printf "    static const UTF32    KompatibleNormalizationCodePoints[KompatibleNormalizationTableSize] = {\n" >> "$OutputFile"
    for line in $KompatibleNormalizationCodePointsAndStrings; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    printf "    static const UTF32   *KompatibleNormalizationStrings[KompatibleNormalizationTableSize] = {\n" >> "$OutputFile"
    for line2 in $KompatibleNormalizationCodePointsAndStrings; do
        KompatibleNormalizationString=$(echo "$line2" | awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}')
        ReplacementString=""
        for CodePoint2 in $KompatibleNormalizationString; do
            DecimalCodePoint=$(printf "%d" "$CodePoint2")
            if [ "$DecimalCodePoint" -le 160 ]; then
                ReplacementString+=$(printf '\\x%X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -le 65535 ]; then
                ReplacementString+=$(printf '\\u%04X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -gt 65535 ]; then
                ReplacementString+=$(printf '\\U%08X' "$CodePoint2")
            fi
        done
    printf "        U\"%s\",\n" $ReplacementString >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateCaseFoldTables() {
    IFS='
'
    CodePointAndReplacement=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')]" -v @cp -o : -v @NFKC_CF -n "$UCD_Data")
    printf "    static const UTF32    CaseFoldCodePoints[CaseFoldTableSize] = {\n" >> "$OutputFile"
    for line in $CodePointAndReplacement; do
        CodePoint2BeReplaced=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint2BeReplaced" >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    printf "    static const UTF32   *CaseFoldStrings[CaseFoldTableSize] = {\n" >> "$OutputFile"
    for line2 in $CodePointAndReplacement; do
        ReplacementCodePoints=$(echo "$line2" | awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}')
        ReplacementString=""
        for CodePoint2 in $ReplacementCodePoints; do
            DecimalCodePoint=$(printf "%d" "$CodePoint2")
            if [ "$DecimalCodePoint" -le 160 ]; then
                ReplacementString+=$(printf '\\x%X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -le 65535 ]; then
                ReplacementString+=$(printf '\\u%04X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -gt 65535 ]; then
                ReplacementString+=$(printf '\\U%08X' "$CodePoint2")
            fi
	    done
        printf "        U\"%s\",\n" $ReplacementString >> "$OutputFile"
    done
    printf "    };\n\n" >> "$OutputFile"
    unset IFS
}

CreateOutputFileBottom() {
    printf "#ifdef __cplusplus\n" >> "$OutputFile"
    printf "}\n" >> "$OutputFile"
    printf "#endif /* C++ */\n\n" >> "$OutputFile"
    printf "#endif /* FoundationIO_StringIOTables_H */\n" >> "$OutputFile"
}

XMLStarletPath=$(command -v xmlstarlet)

if [ -z "$XMLStarletPath" ]; then
    echo "You need to install XMLStarlet, aborting."
    exit 1
fi

OutputFile=$@

if [ "$#" -ne 1 ] || [ -z "$1" ]; then
    echo "The first and only argument needs to be the file to write the tables. (if it exists, it WILL be IRRECOVERABLY DELETED)"
else
    if [ -e "$OutputFile" ]; then
        HeaderUnicodeVersion=$(grep '#define UnicodeVersion ' "$OutputFile" | awk '{printf $3}')
    else
        HeaderUnicodeVersion="-1.-1.-1"
    fi

    HeaderUnicodeVMajor=$(echo $HeaderUnicodeVersion | awk -F "." '{printf $1}')
    HeaderUnicodeVMinor=$(echo $HeaderUnicodeVersion | awk -F "." '{printf $2}')
    HeaderUnicodeVPatch=$(echo $HeaderUnicodeVersion | awk -F "." '{printf $3}')

    ReadmeSize=$(curl -sI "https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt" | grep "Content-Length: " | awk '{printf $2}' | sed "s/$(printf '\r')\$//")

    UCD_Folder=$(mktemp -d)

    FreeSpaceInBlocks=$(df "$UCD_Folder" | tail -n 1 | awk '{printf $4}')
    FreeSpaceInBytes=$(echo "scale=0; $FreeSpaceInBlocks * 512;" | bc)

    if [ "$ReadmeSize" -lt "$FreeSpaceInBytes" ]; then
        curl -s -N "https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt" -o "$UCD_Folder/readme.txt"

        ReadmeUnicodeVersion=$(grep 'XML Representation of Unicode ' "$UCD_Folder/readme.txt" | awk '{printf $5}')

        ReadmeUnicodeVMajor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $1}')
        ReadmeUnicodeVMinor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $2}')
        ReadmeUnicodeVPatch=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $3}')
    else
        echo "Not enough free space to download the Readme, aborting."
        exit 1
    fi

    if [ "$HeaderUnicodeVMajor" -ge "$ReadmeUnicodeVMajor" ] && [ "$HeaderUnicodeVMinor" -ge "$ReadmeUnicodeVMinor" ] && [ "$HeaderUnicodeVPatch" -ge "$ReadmeUnicodeVPatch" ]; then
        echo "The Unicode tables are already up to date, exiting."
    fi

    if [ "$HeaderUnicodeVMajor" -lt "$ReadmeUnicodeVMajor" ] || (( [ "$HeaderUnicodeVMajor" -eq "$ReadmeUnicodeVMajor" ] && [ "$HeaderUnicodeVMinor" -lt "$ReadmeUnicodeVMinor" ] )) || (( [ "$HeaderUnicodeVMajor" -eq "$ReadmeUnicodeVMajor" ] && [ "$HeaderUnicodeVMinor" -eq "$ReadmeUnicodeVMinor" ] && [ "$HeaderUnicodeVPatch" -lt "$ReadmeUnicodeVPatch" ] )); then #Update the header

        ZipFileSize=$(curl -sI "https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip" | grep "Content-Length: " | awk '{printf $2}' | sed "s/$(printf '\r')\$//")
        if [ "$ZipFileSize" -lt "$FreeSpaceInBytes" ]; then
            curl -s -N "https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip" -o "$UCD_Folder/ucd.all.flat.zip"
            ZipUncompressedSize=$(zipinfo "$UCD_Folder/ucd.all.flat.zip" | tail -n 1 | awk '{printf $3}' | sed "s/$(printf '\r')\$//")
            if [ "$ZipUncompressedSize" -le "$FreeSpaceInBytes" ]; then
                echo "Generating the Unicode tables..."

                rm -rf "$OutputFile"
                touch "$OutputFile"

                unzip -q "$UCD_Folder/ucd.all.flat.zip" -d "$UCD_Folder"
                rm -f "$UCD_Folder/ucd.all.flat.zip"

                UCD_Data="$UCD_Folder/ucd.all.flat.xml"

                CreateOutputFileTop
                CreateWhiteSpaceTable
                CreateBiDirectionalControlsTable
                CreateCombiningCharacterClassTable
                CreateIntegerTable
                CreateGraphemeExtensionTable
                CreateKompatibleNormalizationTables
                CreateCanonicalNormalizationTables
                CreateCaseFoldTables
                CreateOutputFileBottom
            else
                echo "Not enough free space to extract the XML UCD, aborting"
                exit 1
            fi
        else
            echo "Not enough free space to download the UCD zip file, aborting"
            exit 1
        fi
    else
        echo "The Unicode tables are already up to date, exiting."
        exit 0
    fi
fi
