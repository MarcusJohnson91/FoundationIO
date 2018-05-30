#!/bin/bash

# License: Public Domain, Creative Commons 0 for municipalities that do not have the notion of a public domain.
# Usage (the HeaderFile WILL BE DELETED): ./MakeUnicodeTables.sh /HeaderPath/HeaderFile.h 
# Dependencies: XMLStarlet (On Mac install Homebrew from brew.sh then call brew install xmlstarlet)

function CreateOutputFileTop {
    printf "#include \"StringIO.h\"                  /* Included for our declarations */\n\n" >> $OutputFile
    printf "#pragma once\n\n" >> $OutputFile
    printf "#ifndef FoundationIO_StringIOTables_H\n" >> $OutputFile
    printf "#define FoundationIO_StringIOTables_H\n\n" >> $OutputFile
    printf "#ifdef   __cplusplus\n" >> $OutputFile
    printf "extern   \"C\" {\n" >> $OutputFile
    printf "#endif\n\n" >> $OutputFile
    printf "#define UnicodeVersion %s\n\n" $ReadmeVersion >> $OutputFile
    NumWhiteSpaceCodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@WSpace='Y'])" $UCD_Data)
    printf "#define WhiteSpaceTableSize %d\n\n" $NumWhiteSpaceCodePoints >> $OutputFile
    CombiningCharacterClassTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@ccc != '0'])" $UCD_Data)
    printf "#define CombiningCharacterClassTableSize %d\n\n" $CombiningCharacterClassTableSize >> $OutputFile
    IntegerTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')])" $UCD_Data)
    printf "#define IntegerTableSize %d\n\n" $IntegerTableSize >> $OutputFile
    GraphemeExtensionSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Gr_Ext = 'Y'])" $UCD_Data)
    printf "#define GraphemeExtensionTableSize %d\n\n" $GraphemeExtensionSize >> $OutputFile
    KompatibleNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none'])" -n $UCD_Data)
    printf "#define KompatibleNormalizationTableSize %d\n\n" $KompatibleNormalizationTableSize >> $OutputFile
    CaseFoldTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')])" $UCD_Data)
    printf "#define CaseFoldTableSize %d\n\n" $CaseFoldTableSize >> $OutputFile
    CanonicalNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dt = 'can'])" -n $UCD_Data)
    printf "#define CanonicalNormalizationTableSize %d\n\n" $CanonicalNormalizationTableSize >> $OutputFile
}

function CreateWhiteSpaceTable {
    IFS=$'\n'
    printf "\tstatic const UTF32    WhiteSpaceTable[WhiteSpaceTableSize] = {\n" >> $OutputFile
    WhiteSpace=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@WSpace = 'Y']" -v @cp -n $UCD_Data)
    for line in $WhiteSpace; do
        Value=$(sed -e 's/^/0x/g' <<< $line)
        $(printf "\t\t0x%06X,\n" $Value >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateCombiningCharacterClassTable {
    IFS=$'\n'
    printf "\tstatic const UTF32    CombiningCharacterClassTable[CombiningCharacterClassTableSize][2] = {\n" >> $OutputFile
    CombiningCharacterClassCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@ccc != '0']" -v @cp -o : -v @ccc -n $UCD_Data | sort -s -n -k 2 -t :)
    for line in $CombiningCharacterClassCodePointAndValue; do
        CodePoint=$(awk -F '[: ]' '{printf $1}' <<< $line | sed -e 's/^/0x/g')
        Value=$(awk -F '[: ]' '{printf $2}' <<< $line)
        $(printf "\t\t{0x%06X, %d},\n" $CodePoint $Value >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateCanonicalNormalizationTables {
    IFS=$'\n'
    NULLTerminator=$(echo "\x0")
    CanonicalNormalizationCodePointsAndStrings=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dm != @cp and @dm != '#' and @dt = 'can']" -v @cp -o : -v @dm -n $UCD_Data | sort -s -n -k 2 -t :)
    printf "\tstatic const UTF32    CanonicalNormalizationCodePoints[CanonicalNormalizationTableSize] = {\n" >> $OutputFile
    for line in $CanonicalNormalizationCodePointsAndStrings; do
        CodePoint=$(awk -F '[: ]' '{printf $1}' <<< $line | sed -e 's/^/0x/g')
        $(printf "\t\t0x%06X,\n" $CodePoint >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    printf "\tstatic const UTF32   *CanonicalNormalizationStrings[CanonicalNormalizationTableSize] = {\n" >> $OutputFile
    for line2 in $CanonicalNormalizationCodePointsAndStrings; do
        CanonicalNormalizationString=$(awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}' <<< $line2)
        ReplacementString=""
        for CodePoint2 in $CanonicalNormalizationString; do
            DecimalCodePoint=$(printf "%d" $CodePoint2)
            if [ $DecimalCodePoint -le 160 ]; then
                ReplacementString+=$(sed -e 's/^0x0*/\\x/g' <<< $CodePoint2)
            elif [ $DecimalCodePoint -le 65535 ]; then
                ReplacementString+=$(printf "\u%04X" $CodePoint2)
            elif [ $DecimalCodePoint -gt 65535 ]; then
                ReplacementString+=$(printf "\U%08X" $CodePoint2)
            fi
        done
    $(printf "\t\tU\"%s%s\",\n" $ReplacementString $NULLTerminator >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateKompatibleNormalizationTables {
    IFS=$'\n'
    NULLTerminator=$(echo "\x0")
    KompatibleNormalizationCodePointsAndStrings=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none']" -v @cp -o : -v @dm -n $UCD_Data | sort -s -n -k 2 -t :)
    printf "\tstatic const UTF32    KompatibleNormalizationCodePoints[KompatibleNormalizationTableSize] = {\n" >> $OutputFile
    for line in $KompatibleNormalizationCodePointsAndStrings; do
        CodePoint=$(awk -F '[: ]' '{printf $1}' <<< $line | sed -e 's/^/0x/g')
        $(printf "\t\t0x%06X,\n" $CodePoint >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    printf "\tstatic const UTF32   *KompatibleNormalizationStrings[KompatibleNormalizationTableSize] = {\n" >> $OutputFile
    for line2 in $KompatibleNormalizationCodePointsAndStrings; do
        KompatibleNormalizationString=$(awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}' <<< $line2)
        ReplacementString=""
        for CodePoint2 in $KompatibleNormalizationString; do
            DecimalCodePoint=$(printf "%d" $CodePoint2)
            if [ $DecimalCodePoint -le 160 ]; then
                ReplacementString+=$(sed -e 's/^0x0*/\\x/g' <<< $CodePoint2)
            elif [ $DecimalCodePoint -le 65535 ]; then
                ReplacementString+=$(printf "\u%04X" $CodePoint2)
            elif [ $DecimalCodePoint -gt 65535 ]; then
                ReplacementString+=$(printf "\U%08X" $CodePoint2)
            fi
        done
    $(printf "\t\tU\"%s%s\",\n" $ReplacementString $NULLTerminator >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateCaseFoldTables {
    IFS=$'\n'
    NULLTerminator=$(echo "\x0")
    CodePointAndReplacement=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')]" -v @cp -o : -v @NFKC_CF -n $UCD_Data)
    printf "\tstatic const UTF32    CaseFoldCodePoints[CaseFoldTableSize] = {\n" >> $OutputFile
    for line in $CodePointAndReplacement; do
        CodePoint2BeReplaced=$(awk -F '[: ]' '{printf $1}' <<< $line | sed -e 's/^0*/0x/g')
        $(printf "\t\t0x%06X,\n" $CodePoint2BeReplaced >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    printf "\tstatic const UTF32   *CaseFoldStrings[CaseFoldTableSize] = {\n" >> $OutputFile
    for line2 in $CodePointAndReplacement; do
        ReplacementCodePoints=$(awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}' <<< $line2)
        ReplacementString=""
        for CodePoint2 in $ReplacementCodePoints; do
            DecimalCodePoint=$(printf "%d" $CodePoint2)
            if [ $DecimalCodePoint -le 160 ]; then
                ReplacementString+=$(sed -e 's/^0x0*/\\x/g' <<< $CodePoint2)
            elif [ $DecimalCodePoint -le 65535 ]; then
                ReplacementString+=$(printf "\u%04X" $CodePoint2)
            elif [ $DecimalCodePoint -gt 65535 ]; then
                ReplacementString+=$(printf "\U%08X" $CodePoint2)
            fi
	    done
        $(printf "\t\tU\"%s%s\",\n" $ReplacementString $NULLTerminator >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateIntegerTable {
    IFS=$'\n'
    SortedCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -v "@cp" -o : -v "@nv" -n $UCD_Data | sort -s -n -k 2 -t $':')
    printf "\tstatic const UTF32    IntegerCodePoints[IntegerTableSize] = {\n" >> $OutputFile
    for line in $SortedCodePointAndValue; do
        CodePoint=$(awk -F '[: ]' '{printf $1}' <<< $line | sed -e 's/^0*/0x/g')
        $(printf "\t\t0x%06X,\n" $CodePoint >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    printf "\tstatic const uint64_t IntegerValues[IntegerTableSize] = {\n" >> $OutputFile
    for line2 in $SortedCodePointAndValue; do
        Value=$(awk -F ":" '{printf $2}' <<< $line2)
        $(printf "\t\t%d,\n" $Value >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateGraphemeExtensionTable {
    IFS=$'\n'
    printf "\tstatic const UTF32    GraphemeExtensionTable[GraphemeExtensionTableSize] = {\n" >> $OutputFile
    CodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Gr_Ext = 'Y']" -v "@cp" -n $UCD_Data)
    for line in $CodePoints; do
        Value=$(sed -e 's/^/0x/g' <<< $line)
        $(printf "\t\t0x%06X,\n" $Value >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateOutputFileBottom {
    printf "#ifdef   __cplusplus\n" >> $OutputFile
    printf "}\n" >> $OutputFile
    printf "#endif /* C++ */\n\n" >> $OutputFile
    printf "#endif /* FoundationIO_StringIOTables_H */\n" >> $OutputFile
}

OutputFile=$@
StringIOVersion=$(grep '#define UnicodeVersion ' $OutputFile | awk '{print $3}')
if [ -z $StringIOVersion ]; then
    StringIOVersion="0.0.0"
fi
StringIOVMajor=$(awk -F "." '{print $1}' <<< $StringIOVersion)
StringIOVMinor=$(awk -F "." '{print $2}' <<< $StringIOVersion)
StringIOVPatch=$(awk -F "." '{print $3}' <<< $StringIOVersion)
UCD_Folder=$(mktemp -d)
curl -s -N https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt -o "$UCD_Folder/readme.txt"
ReadMeVersion=$(grep 'XML Representation of Unicode ' "$UCD_Folder/readme.txt" | awk '{print $5}')
ReadMeVMajor=$(awk -F "." '{print $1}' <<< $ReadMeVersion)
ReadMeVMinor=$(awk -F "." '{print $2}' <<< $ReadMeVersion)
ReadMeVPatch=$(awk -F "." '{print $3}' <<< $ReadMeVersion)

if   [ $# -ne 1 ] || [ -z "$1" ]; then
    echo "The first argument needs to be the file to contain the tables."
elif [ $StringIOVMajor -eq $ReadMeVMajor ] && [ $StringIOVMinor -eq $ReadMeVMinor ] && [ $StringIOVPatch -eq $ReadMeVPatch ]; then
    echo "The Unicode tables are already up to date, exiting."
elif [ $StringIOVMajor -lt $ReadMeVMajor ] || (( [ $StringIOVMajor -eq $ReadMeVMajor ] && [ $StringIOVMinor -lt $ReadMeVMinor ] )) || (( [ $StringIOVMajor -eq $ReadMeVMajor ] && [ $StringIOVMinor -eq $ReadMeVMinor ] && [ $StringIOVPatch -lt $ReadMeVPatch ] )); then
    command -v xmlstarlet >/dev/null 2>&1 || { echo >&2 "You need to install XMLStarlet, Aborting."; exit 1; }
    echo "Generating the Unicode tables..."
    rm -rf $OutputFile
    touch $OutputFile
    $(curl -s -N "https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip" -o "$UCD_Folder/ucd.all.flat.zip")
    $(unzip -q "$UCD_Folder/ucd.all.flat.zip" -d "$UCD_Folder")
    $(rm -f "$UCD_Folder/ucd.all.flat.zip")
    UCD_Data="$UCD_Folder/ucd.all.flat.xml"
    CreateOutputFileTop
    CreateWhiteSpaceTable
    CreateCombiningCharacterClassTable
    CreateIntegerTable
    CreateGraphemeExtensionTable
    CreateKompatibleNormalizationTables
    CreateCaseFoldTables
    CreateCanonicalNormalizationTables
    CreateOutputFileBottom
fi
