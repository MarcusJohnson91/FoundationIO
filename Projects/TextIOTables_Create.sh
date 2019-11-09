#!/usr/bin/env sh

# Usage (the HeaderFile WILL BE IRRECOVERABLY DELETED): ./UnicodeTables_Create.sh /HeaderPath/HeaderFile.h
# Dependencies: Curl, XMLStarlet (On Mac install Homebrew from brew.sh then call brew install xmlstarlet)

CreateHeaderFileTop() {
    printf "#include <stdint.h>\n\n" >> "$HeaderFile"
    printf "#ifndef   FoundationIO_StringType32\n" >> "$HeaderFile"
    printf "#define   FoundationIO_StringType32\n" >> "$HeaderFile"
    printf "#ifdef    UTF32\n" >> "$HeaderFile"
    printf "#undef    UTF32\n" >> "$HeaderFile"
    printf "#endif /* UTF32 */\n" >> "$HeaderFile"
    echo   "#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__) && (!defined __APPLE__) && (!defined __MACH__)" >> "$HeaderFile"
    # Printf doesn't want to work with the precending Macro, so using echo instead is my shitty workaround.
    printf "typedef   char32_t       UTF32;\n" >> "$HeaderFile"
    printf "#else\n" >> "$HeaderFile"
    printf "typedef   uint_least32_t UTF32;\n" >> "$HeaderFile"
    printf "#endif /* __CHAR32_TYPE__ */\n" >> "$HeaderFile"
    printf "#endif /* FoundationIO_StringType32 */\n\n" >> "$HeaderFile"
    printf "#pragma once\n\n" >> "$HeaderFile"
    printf "#ifndef FoundationIO_UnicodeTables_H\n" >> "$HeaderFile"
    printf "#define FoundationIO_UnicodeTables_H\n\n" >> "$HeaderFile"
    printf "#ifdef __cplusplus\n" >> "$HeaderFile"
    printf "extern \"C\" {\n" >> "$HeaderFile"
    printf "#endif\n\n" >> "$HeaderFile"
	printf "#define ScriptHash %s\n\n" $(md5 $0) >> "$HeaderFile"
    printf "#define UnicodeVersion %s\n\n" "$ReadmeUnicodeVersion" >> "$HeaderFile"
    NumBiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Bidi_C='Y'])" "$UCD_Data")
    printf "#define BiDirectionalControlsTableSize %d\n\n" "$NumBiDirectionalControls" >> "$HeaderFile"
    NumWhiteSpaceCodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@WSpace='Y'])" "$UCD_Data")
    printf "#define WhiteSpaceTableSize %d\n\n" "$NumWhiteSpaceCodePoints" >> "$HeaderFile"
    NumCurrencyCodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@gc='Sc'])" "$UCD_Data")
    printf "#define CurrencyTableSize %d\n\n" "$NumCurrencyCodePoints" >> "$HeaderFile"
    DecimalTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and contains(@nv, '/')])" "$UCD_Data")
    printf "#define DecimalTableSize %d\n\n" "$DecimalTableSize" >> "$HeaderFile"
    CombiningCharacterClassTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@ccc != '0'])" "$UCD_Data")
    printf "#define CombiningCharacterClassTableSize %d\n\n" "$CombiningCharacterClassTableSize" >> "$HeaderFile"
    IntegerTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')])" "$UCD_Data")
    printf "#define IntegerTableSize %d\n\n" "$IntegerTableSize" >> "$HeaderFile"
    GraphemeExtensionSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@Gr_Ext = 'Y'])" "$UCD_Data")
    printf "#define GraphemeExtensionTableSize %d\n\n" "$GraphemeExtensionSize" >> "$HeaderFile"
    KompatibleNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none'])" -n "$UCD_Data")
    printf "#define KompatibleNormalizationTableSize %d\n\n" "$KompatibleNormalizationTableSize" >> "$HeaderFile"
    CaseFoldTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')])" "$UCD_Data")
    printf "#define CaseFoldTableSize %d\n\n" "$CaseFoldTableSize" >> "$HeaderFile"
    CanonicalNormalizationTableSize=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char[@dm != @cp and @dt = 'can'])" -n "$UCD_Data")
    printf "#define CanonicalNormalizationTableSize %d\n\n" "$CanonicalNormalizationTableSize" >> "$HeaderFile"
}

CreateBiDirectionalControlsTable() {
    IFS='
'
    printf "    static const UTF32    BiDirectionalControlsTable[BiDirectionalControlsTableSize] = {\n" >> "$HeaderFile"
    BiDirectionalControls=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Bidi_C = 'Y']" -v @cp -n "$UCD_Data")
    for line in $BiDirectionalControls; do
        Value=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$Value" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateWhiteSpaceTable() {
    IFS='
'
    printf "    static const UTF32    WhiteSpaceTable[WhiteSpaceTableSize] = {\n" >> "$HeaderFile"
    WhiteSpace=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@WSpace = 'Y']" -v @cp -n "$UCD_Data")
    for line in $WhiteSpace; do
        Value=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$Value" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateCurrencyTable() {
    IFS='
'
    printf "    static const UTF32    CurrencyTable[CurrencyTableSize] = {\n" >> "$HeaderFile"
    Currency=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@gc = 'Sc']" -v @cp -n "$UCD_Data")
    for line in $Currency; do
        Value=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$Value" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateDecimalTables() {
    IFS='
'
    CodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and contains(@nv, '/') and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -v "@cp" -o : -v "@nv" -n "$UCD_Data")
    printf "    static const UTF32    DecimalCodePoints[DecimalTableSize] = {\n" >> "$HeaderFile"
    for line in $CodePointAndValue; do
        CodePoint=$(echo "$line" | awk -F '[:/]' '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    printf "    static const int8_t   DecimalNumerators[DecimalTableSize] = {\n" >> "$HeaderFile"
    for line in $CodePointAndValue; do
        Numerator=$(echo "$line" | awk -F '[:/]' '{printf $2}')
        printf "        %d,\n" "$Numerator" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    printf "    static const uint16_t DecimalDenominators[DecimalTableSize] = {\n" >> "$HeaderFile"
    for line in $CodePointAndValue; do
        Denominator=$(echo "$line" | awk -F '[:/]' '{printf $3}')
        printf "        %d,\n" "$Denominator" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateCombiningCharacterClassTable() {
    IFS='
'
    printf "    static const UTF32    CombiningCharacterClassTable[CombiningCharacterClassTableSize][2] = {\n" >> "$HeaderFile"
    CombiningCharacterClassCodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@ccc != '0']" -v @cp -o : -v @ccc -n "$UCD_Data")
    for line in $CombiningCharacterClassCodePointAndValue; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        Value=$(echo "$line" | awk -F '[: ]' '{printf $2}')
        printf "        {0x%06X, %d},\n" "$CodePoint" "$Value" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateIntegerTable() {
    IFS='
'
    CodePointAndValue=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN' and not(contains(@nv, '/')) and (@nt = 'None' or @nt = 'Di' or @nt = 'Nu' or @nt = 'De')]" -v "@cp" -o : -v "@nv" -n "$UCD_Data")
    printf "    static const UTF32    IntegerCodePoints[IntegerTableSize] = {\n" >> "$HeaderFile"
    for line in $CodePointAndValue; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    printf "    static const uint64_t IntegerValues[IntegerTableSize] = {\n" >> "$HeaderFile"
    for line2 in $CodePointAndValue; do
        Value=$(echo "$line2" | awk -F ":" '{printf $2}')
        printf "        %d,\n" "$Value" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateGraphemeExtensionTable() {
    IFS='
'
    printf "    static const UTF32    GraphemeExtensionTable[GraphemeExtensionTableSize] = {\n" >> "$HeaderFile"
    CodePoints=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@Gr_Ext = 'Y']" -v "@cp" -n "$UCD_Data")
    for line in $CodePoints; do
        Value=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$Value" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateCanonicalNormalizationTables() {
    IFS='
'
    CanonicalNormalizationCodePointsAndStrings=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@dm != @cp and @dm != '#' and @dt = 'can']" -v @cp -o : -v @dm -n "$UCD_Data")
    printf "    static const UTF32    CanonicalNormalizationCodePoints[CanonicalNormalizationTableSize] = {\n" >> "$HeaderFile"
    for line in $CanonicalNormalizationCodePointsAndStrings; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    printf "    static const UTF32   *CanonicalNormalizationStrings[CanonicalNormalizationTableSize] = {\n" >> "$HeaderFile"
    for line2 in $CanonicalNormalizationCodePointsAndStrings; do
        CanonicalNormalizationString=$(echo "$line2" | awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}')
        ReplacementString=""
        ReplacementCodePoint=""
        for CodePoint2 in $CanonicalNormalizationString; do # CodePoint2 is already prefixed with 0x
            DecimalCodePoint=$(printf "%d" "$CodePoint2")
            if [ "$DecimalCodePoint" -le 160 ]; then
                ReplacementCodePoint=$(printf '\\x%X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -le 65535 ]; then
                ReplacementCodePoint=$(printf '\\u%04X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -gt 65535 ]; then
                ReplacementCodePoint=$(printf '\\U%08X' "$CodePoint2")
            fi
            ReplacementString="$ReplacementString""$ReplacementCodePoint"
        done
    printf "        U\"%s\",\n" "$ReplacementString" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateKompatibleNormalizationTables() {
    IFS='
'
    KompatibleNormalizationCodePointsAndStrings=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[(@dt = 'com' or @dt = 'font' or @dt = 'nobreak' or @dt = 'initial' or @dt = 'medial' or @dt = 'final' or @dt = 'isolated' or @dt = 'circle' or @dt = 'super' or @dt = 'sub' or @dt = 'vertical' or @dt = 'wide' or @dt = 'narrow' or @dt = 'small' or @dt = 'square' or @dt = 'fraction' or @dt = 'compat') and @dt != '' and @dt != '#' and @dt != 'none']" -v @cp -o : -v @dm -n "$UCD_Data")
    printf "    static const UTF32    KompatibleNormalizationCodePoints[KompatibleNormalizationTableSize] = {\n" >> "$HeaderFile"
    for line in $KompatibleNormalizationCodePointsAndStrings; do
        CodePoint=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    printf "    static const UTF32   *KompatibleNormalizationStrings[KompatibleNormalizationTableSize] = {\n" >> "$HeaderFile"
    for line2 in $KompatibleNormalizationCodePointsAndStrings; do
        KompatibleNormalizationString=$(echo "$line2" | awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}')
        ReplacementString=""
        ReplacementCodePoint=""
        for CodePoint2 in $KompatibleNormalizationString; do
            DecimalCodePoint=$(printf "%d" "$CodePoint2")
            if [ "$DecimalCodePoint" -le 160 ]; then
                ReplacementCodePoint=$(printf '\\x%X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -le 65535 ]; then
                ReplacementCodePoint=$(printf '\\u%04X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -gt 65535 ]; then
                ReplacementCodePoint=$(printf '\\U%08X' "$CodePoint2")
            fi
            ReplacementString="$ReplacementString""$ReplacementCodePoint"
        done
    printf "        U\"%s\",\n" "$ReplacementString" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateCaseFoldTables() {
    IFS='
'
    CodePointAndReplacement=$(xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@NFKC_CF != @cp and @NFKC_CF != '' and @NFKC_CF != '#' and (@CWCF='Y' or @CWCM ='Y' or @CWL = 'Y' or @CWKCF = 'Y')]" -v @cp -o : -v @NFKC_CF -n "$UCD_Data")
    printf "    static const UTF32    CaseFoldCodePoints[CaseFoldTableSize] = {\n" >> "$HeaderFile"
    for line in $CodePointAndReplacement; do
        CodePoint2BeReplaced=$(echo "$line" | awk -F ":" '{printf "0x"$1}')
        printf "        0x%06X,\n" "$CodePoint2BeReplaced" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    printf "    static const UTF32   *CaseFoldStrings[CaseFoldTableSize] = {\n" >> "$HeaderFile"
    for line2 in $CodePointAndReplacement; do
        ReplacementCodePoints=$(echo "$line2" | awk -F '[: ]' '{for (i = 2; i <= NF; i++) print "0x"$i}')
        ReplacementString=""
        ReplacementCodePoint=""
        for CodePoint2 in $ReplacementCodePoints; do
            DecimalCodePoint=$(printf "%d" "$CodePoint2")
            if [ "$DecimalCodePoint" -le 160 ]; then
                ReplacementCodePoint=$(printf '\\x%X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -le 65535 ]; then
                ReplacementCodePoint=$(printf '\\u%04X' "$CodePoint2")
            elif [ "$DecimalCodePoint" -gt 65535 ]; then
                ReplacementCodePoint=$(printf '\\U%08X' "$CodePoint2")
            fi
            ReplacementString="$ReplacementString""$ReplacementCodePoint"
	    done
        printf "        U\"%s\",\n" "$ReplacementString" >> "$HeaderFile"
    done
    printf "    };\n\n" >> "$HeaderFile"
    unset IFS
}

CreateHeaderFileBottom() {
    printf "#ifdef __cplusplus\n" >> "$HeaderFile"
    printf "}\n" >> "$HeaderFile"
    printf "#endif /* C++ */\n\n" >> "$HeaderFile"
    printf "#endif /* FoundationIO_UnicodeTables_H */\n" >> "$HeaderFile"
}

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
			echo "Couldn't extract UCD, aborting."
			exit 0
		fi
	else
		echo "Couldn't download UCD, aborting."
		exit 0
	fi
}

CreateHeader() {
	CreateHeaderFileTop

    CreateBiDirectionalControlsTable
    CreateWhiteSpaceTable
    CreateCurrencyTable
    CreateDecimalTables
    CreateCombiningCharacterClassTable
    CreateIntegerTable
    CreateGraphemeExtensionTable
    CreateKompatibleNormalizationTables
    CreateCaseFoldTables
    CreateCanonicalNormalizationTables

    CreateHeaderFileBottom
}

# Check that there's exactly 1 argument to the program
# If there is 1 argument, check if the file exists, 
# if the file exists check the version number against the latest release of Unicode
# If the file does not exist create it and start downloading the UCd and writing the tables

HeaderFile=$1

if [ $# -eq 1 ]; then
	XMLStarletPath=$(which xmlstarlet)
	UnzipPath=$(which unzip)
	CurlPath=$(which curl)
	MD5Path=$(which md5)
	
	if [ "$XMLStarletPath" -ne "" -a "$UnzipPath" -ne "" -a "CurlPath" -ne "" -a "$MD5Path" -ne "" ]; then
		# We're good, we can now test if the headerfile exists.
		UCD_Folder=$(mktemp -d)
		if [ -e "$HeaderFile" ]; then
			HeaderScriptHash=$(grep '#define ScriptHash ' "$HeaderFile" | awk '{printf $3}')
			ScriptHash=$(md5 $0)
			
			if [ "$HeaderScriptHash" -ne "$ScriptHash" ]; then
				rm "$HeaderFile"
				touch "$HeaderFile"
				DownloadUCD
				CreateHeader
			else
				ReadmeSize=$(curl -sI "https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt" | grep "Content-Length: " | awk '{printf $2}' | sed "s/$(printf '\r')\$//")			
				
				ReadmeUnicodeVersion=$(grep 'XML Representation of Unicode ' "$UCD_Folder/readme.txt" | awk '{printf $5}')
				ReadmeUnicodeVMajor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $1}')
				ReadmeUnicodeVMinor=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $2}')
				ReadmeUnicodeVPatch=$(echo "$ReadmeUnicodeVersion" | awk -F "." '{printf $3}')
				
				HeaderUnicodeVersion=$(grep '#define UnicodeVersion ' "$HeaderFile" | awk '{printf $3}')
				HeaderUnicodeVMajor=$(echo $HeaderUnicodeVersion | awk -F "." '{printf $1}')
				HeaderUnicodeVMinor=$(echo $HeaderUnicodeVersion | awk -F "." '{printf $2}')
				HeaderUnicodeVPatch=$(echo $HeaderUnicodeVersion | awk -F "." '{printf $3}')
				
				if [ "$HeaderUnicodeVPatch" -lt "$ReadmeUnicodeVPatch" -o "$HeaderUnicodeVMinor" -lt "$ReadmeUnicodeVMinor" -o "$HeaderUnicodeVMajor" -lt "$ReadmeUnicodeVMajor" ]; then
					rm "$HeaderFile"
					touch "$HeaderFile"
					DownloadUCD
					CreateHeader
				else
					echo "The Unicode tables are already up to date, exiting."
					exit 0
				fi
			fi
		else
			touch "$HeaderFile"
			CreateHeader
		fi
	else
		echo "You must install the following packages:"
		if [ "$CurlPath" -eq "" ]; then
			echo "Curl"
		fi
		
		if [ "$MD5Path" -eq "" ]; then
			echo "MD5"
		fi
		
		if [ "$UnzipPath" -eq "" ]; then
			echo "Unzip"
		fi
		
		if [ "$XMLStarletPath" -eq "" ]; then
			echo "XMLStarlet"
		fi
	fi
else
	echo "The first argument must be the header file to overwrite, and the only argument present."
fi
