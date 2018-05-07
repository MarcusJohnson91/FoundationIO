#!/bin/bash -x

# License: Public Domain, Creative Commons 0 for municipalities that do not have the notion of a public domain

# We NEED XMLStarlet to work, so install it with: /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
# Then call: brew install XMLStarlet

# xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN']" -v "@cp" -v "@nv" ucd.all.flat.xmlstarletTHIS WORKS

# xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@gc = 'Lu' or @gc = flat.xmlv "@cp" ucd.all.flat.xml

# Then we're off, download the XMLUCD readme to check if it's newer than our version of Unicode.

# Extract WhiteSpace: xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[$CodePoint @WSpace == 'Y']" -v "@cp" $UCD_Data

# Ok we're parsing the xml UCD, not straight text version.
# for the number tables, look for class gc="Nd" (Numeral, Decimal), and get it's value from nv="X"

#and generate the tables we need from there.

#List of tables we need to extract:

# List of tables we need
# Whitespace
# CaseFold
# All the codepoints to be found, and the replacements for Compatibile Equilivilence, both Decomposed and Composed forms
# All the codepoints to be found, and the replacements for Canonical   Equilivilence, both Decomposed and Composed forms
# For the casefolding and Normalization tables, maybe I should just have it be string 2 string, so just a table of input strings, and output strings.
# The only concern is the ordering of the codepoints in the input string when I'm scanning, I'd have to come up with a better algorithm to ensure that it's order independent
# List of Grapheme Extension codepoints.

function CreateOutputFileTop {
    # The output file does not exist, or it's version is out of date, so we need to generate it
    rm -rf $OutputFile
    touch $OutputFile
#UCD_URL="https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip"
#$(curl -N $UCD_URL -o "$UCD_Folder/ucd.all.flat.zip")
#$(cd "$UCD_Folder")
#$(unzip "$UCD_Folder/ucd.all.flat.zip" -d "$UCD_Folder")
#$(rm -f "$UCD_Folder/ucd.all.flat.zip")
    UCD_Data="$UCD_Folder/ucd.all.flat.xml"
    printf "#include <stdint.h>                    /* Included for u/intX_t */\n\n" >> $OutputFile
    printf "#include \"StringIO.h\"                  /* Included for UTF32 */\n\n" >> $OutputFile
    printf "#pragma once\n\n" >> $OutputFile
    printf "#ifndef FoundationIO_StringIOTables_H\n" >> $OutputFile
    printf "#define FoundationIO_StringIOTables_H\n\n" >> $OutputFile
    printf "#ifdef   __cplusplus\n" >> $OutputFile
    printf "extern   \"C\" {\n" >> $OutputFile
    printf "#endif\n\n" >> $OutputFile
    printf "#define UnicodeVersion %s\n\n" $ReadmeVersion >> $OutputFile
    NumWhiteSpaceCodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@WSpace='Y'])" $UCD_Data)
    printf "#define WhiteSpaceTableSize %d\n\n" $NumWhiteSpaceCodePoints >> $OutputFile
    CombiningCharacterClassTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@ccc!='0'])" $UCD_Data)
    printf "#define CombiningCharacterClassTableSize %d\n\n" $CombiningCharacterClassTableSize >> $OutputFile
    IntegerTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')])" $UCD_Data)
    printf "#define IntegerTableSize %d\n\n" $IntegerTableSize >> $OutputFile
    GraphemeExtensionSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Gr_Ext = 'Y'])" $UCD_Data)
    printf "#define GraphemeExtensionTableSize %d\n\n" $GraphemeExtensionSize >> $OutputFile
    CaseFoldTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')])" $UCD_Data)
    printf "#define CaseFoldTableSize %d\n\n" $CaseFoldTableSize >> $OutputFile
    DecompositionTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dm != '' and @dm != '#' and (@dt = 'can' or @dt = 'com' or @dt = 'enc' or @dt = 'fin' or @dt = 'font' or @dt = 'fra' or @dt = 'init' or @dt = 'iso' or @dt = 'med' or @dt = 'nar' or @dt = 'nb' or @dt = 'sml' or @dt = 'sqr' or @dt = 'sub' or @dt = 'sup' or @dt = 'vert' or @dt = 'wide' or @dt = 'none')])" $UCD_Data)
    KompatibleDecompositionTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dt = 'can'])" -n $UCD_Data)
    printf "#define KompatibleDecompositionTableSize %d\n\n" $KompatibleDecompositionTableSize >> $OutputFile
    CanonicalDecompositionTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dt = 'can'])" -n $UCD_Data)
    printf "#define CanonicalDecompositionTableSize %d\n\n" $CanonicalDecompositionTableSize >> $OutputFile
}

function CreateWhiteSpaceTable {
    IFS=$'\n'
    printf "\tstatic const UTF32 WhiteSpaceTable[WhiteSpaceTableSize] = {\n" >> $OutputFile
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
    printf "\tstatic const UTF32 CombiningCharacterClassTable[CombiningCharacterClassTableSize][2] = {\n" >> $OutputFile
    CombiningCharacterClassCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@ccc != '0']" -v @cp -o : -v @ccc -n $UCD_Data | sort -s -n -k 2 -t :)
    for line in $CombiningCharacterClassCodePointAndValue; do
        CodePoint=$(awk -F '[: ]' '{printf $1}' <<< "$line" | sed -e 's/^/0x/g')
        Value=$(awk -F '[: ]' '{printf $2}' <<< "$line")
        $(printf "\t\t{0x%06X, %d},\n" $CodePoint $Value >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateKompatibleDecompositionTable {
    IFS=$'\n'
    printf "\tstatic const UTF32 KompatibleDecompositionTable[KompatibleDecompositionTableSize][2] = {\n" >> $OutputFile
    CodePointAndKompatibleDecompositionString=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dt == 'can']" -v @cp -o : -v @ccc -n $UCD_Data | sort -s -n -k 2 -t :)
    for line in $CodePointAndKompatibleDecompositionString; do
        CodePoint2BeReplaced=$(awk -F '[: ]' '{printf $1}' <<< "$line" | sed -e 's/^0*//g' -e 's/^/0x/')
        KompatibleDecompositionString=$(awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}' <<< "$line")
        ReplacementString=""
        for CodePoint in $KompatibleDecompositionString; do
            Base10CodePoint=$(printf "%d" $CodePoint)
            if [ $Base10CodePoint -le 160 ]; then
                ReplacementString+=$(printf "\x%X" $CodePoint)
            elif [ $Base10CodePoint -le 65535 ]; then
                ReplacementString+=$(printf "\u%04X" $CodePoint)
            elif [ $Base10CodePoint -gt 65535 ]; then
                ReplacementString+=$(printf "\U%08X" $CodePoint)
            fi
        done
    $(printf "\t\t{0x%06X, U\"%s%s\"},\n" $CodePoint2BeReplaced $ReplacementString $NULLTerminator >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateCanonicalDecompositionTable {
    IFS=$'\n'
    printf "\tstatic const UTF32 CanonicalDecompositionTable[CanonicalDecompositionTableSize][2] = {\n" >> $OutputFile
    CombiningCharacterClassCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dt == 'can']" -v @cp -o : -v @ccc -n $UCD_Data | sort -s -n -k 2 -t :)
    for line in $CombiningCharacterClassCodePointAndValue; do
        CodePoint=$(awk -F '[: ]' '{printf $1}' <<< "$line" | sed -e 's/^/0x/g')
        Value=$(awk -F '[: ]' '{printf $2}' <<< "$line")
        $(printf "\t\t{0x%06X, %d},\n" $CodePoint $Value >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateCaseFoldTable {
    IFS=$'\n'
    printf "\tstatic const UTF32 *CaseFoldTable[CaseFoldTableSize][2] = {\n" >> $OutputFile
    CodePointAndReplacement=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#']" -v @cp -o : -v @NFKC_CF -n $UCD_Data)
    NULLTerminator=$(echo "\x0")
#Addditional properties:  [or @CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')]
    for line in $CodePointAndReplacement; do
        CodePoint2BeReplaced=$(awk -F '[: ]' '{printf $1}' <<< "$line" | sed -e 's/^0*//g' -e 's/^/0x/')
        ReplacementCodePoints=$(awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}' <<< "$line")
        ReplacementString=""
        for CodePoint in $ReplacementCodePoints; do
            if [ $(printf "%d" ${CodePoint}) -le 160 ]; then
                ReplacementString+=$(sed -e 's/^0x//g' -e 's/^0*//g' -e 's/^/\\x/g' <<< $CodePoint)
            elif [ $(printf "%d" ${CodePoint}) -le 65535 ]; then
                ReplacementString+=$(sed -e 's/^0x//g' -e 's/^/\\u/g' <<< $CodePoint)
            elif [ $(printf "%d" ${CodePoint}) -gt 65535 ]; then
                ReplacementString+=$(sed -e 's/^0x//g' -e 's/[0-9a-fA-F]\{1,\}/0000000&/g;s/0*\([0-9a-fA-F]\{8,\}\)/\1/g' -e 's/^/\\U/g' <<< $CodePoint)
            fi
	    done
        $(printf "\t\t{0x%06X, U\"%s%s\"},\n" $CodePoint2BeReplaced $ReplacementString $NULLTerminator >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateNormalizationTables {
    IFS=$'\n'
    printf "\tstatic const UTF32 *DecompositionTable[DecompositionTableSize][2] = {\n" >> $OutputFile
    CodePointAndReplacement=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dm != '#' and (@dt = 'can' or @dt = 'com' or @dt = 'enc' or @dt = 'fin' or @dt = 'font' or @dt = 'fra' or @dt = 'init' or @dt = 'iso' or @dt = 'med' or @dt = 'nar' or @dt = 'nb' or @dt = 'sml' or @dt = 'sqr' or @dt = 'sub' or @dt = 'sup' or @dt = 'vert' or @dt = 'wide' or @dt = 'none')]" -v @cp -o : -v @dm -n $UCD_Data)
    NULLTerminator=$(echo "\x0")
    for line in $CodePointAndReplacement; do
        CodePoint2BeReplaced=$(awk -F '[: ]' '{printf $1}' <<< "$line" | sed -e 's/^0*//g' -e 's/^/0x/')
        ReplacementCodePoints=$(awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}' <<< "$line")
        ReplacementString=""
        for CodePoint in $ReplacementCodePoints; do
            if [ $(printf "%d" ${CodePoint}) -le 160 ]; then
                ReplacementString+=$(sed -e 's/^0x//g' -e 's/^0*//g' -e 's/^/\\x/g' <<< $CodePoint)
            elif [ $(printf "%d" ${CodePoint}) -le 65535 ]; then
                ReplacementString+=$(sed -e 's/^0x//g' -e 's/^/\\u/g' <<< $CodePoint)
            elif [ $(printf "%d" ${CodePoint}) -gt 65535 ]; then
                ReplacementString+=$(sed -e 's/^0x//g' -e 's/[0-9a-fA-F]\{1,\}/0000000&/g;s/0*\([0-9a-fA-F]\{8,\}\)/\1/g' -e 's/^/\\U/g' <<< $CodePoint)
            fi
        done
    $(printf "\t\t{0x%06X, U\"%s%s\"},\n" $CodePoint2BeReplaced $ReplacementString $NULLTerminator >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateIntegerTable {
    IFS=$'\n'
    SortedCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -v "@cp" -o : -v "@nv" -n $UCD_Data | sort -s -n -k 2 -t $':')
    printf "\tstatic const UTF32 IntegerTableCodePoints[IntegerTableSize] = {\n" >> $OutputFile
    for line in $SortedCodePointAndValue; do
        CodePoint=$(awk -F '[: ]' '{printf $1}' <<< $line | sed -e 's/^0*//g' -e 's/^/0x/')
        $(printf "\t\t0x%06X,\n" $CodePoint >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    printf "\tstatic const uint64_t IntegerTableValues[IntegerTableSize] = {\n" >> $OutputFile
    for line in $SortedCodePointAndValue; do
        Value=$(sed -e 's/[^:]*://' <<< $line -e 's/[[:space:]]/\\x/g')
        $(printf "\t\t%d,\n" $Value >> $OutputFile)
    done
    printf "\t};\n\n" >> $OutputFile
    unset IFS
}

function CreateGraphemeExtensionTable {
    IFS=$'\n'
    printf "\tstatic const UTF32 GraphemeExtensionTable[GraphemeExtensionTableSize] = {\n" >> $OutputFile
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

function GetStringIOUnicodeVersion {
#UCD_README="https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt"
#UCD_Folder=$(mktemp -d)
    UCD_Folder="/Users/Marcus/Desktop/UCDXML"
#$(curl -N $UCD_README -o "$UCD_Folder/readme.txt")
    ReadmeVersion=$(awk 'NR==1 {print $5}' $UCD_Folder/readme.txt)
    if [ ! -f "$OutputFile" ]
    then
        StringIOVersion="0.0.0"
    else
        StringIOVersion=$(awk 'NR==12 {print $3}' $OutputFile)
    fi
}

if [ $# -ne 1 ]
    then echo "You forgot to include the output path for the tables"
else
    ARGUMENTS=$@
    OutputFile=$ARGUMENTS
    GetStringIOUnicodeVersion
#UCD_Folder="~/Desktop/UCDXML"
    if [ $ReadmeVersion -eq $StringIOVersion ]; then
        exit 0
    else
        # The output file does not exist, or it's version is out of date, so we need to generate it
        CreateOutputFileTop
        CreateWhiteSpaceTable
        CreateCombiningCharacterClassTable
        CreateIntegerTable
        CreateGraphemeExtensionTable
        CreateCaseFoldTable
        CreateNormalizationTables
        CreateOutputFileBottom
    fi
fi
