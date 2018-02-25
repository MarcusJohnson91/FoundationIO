#!/bin/sh

# License: Public Domain, Creative Commons 0 for municipalities that do not have the notion of a public domain

# xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@nv != 'NaN']" -v "@cp" -v "@nv" ucd.all.flat.xml THIS WORKS

# xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[@gc = 'Lu' or @gc = flat.xmlv "@cp" ucd.all.flat.xml

# We NEED XMLStarlet to work, so install it with: /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
# Then call: brew install XMLStarlet

# Then we're off, download the XMLUCD readme to check if it's newer than our version of Unicode.
OutputFile="../include/StringIOTables.h"
OutputFileExists=ls $OutputFile | echo $?
UCD_README="https://www.unicode.org/Public/UCD/latest/ucdxml/ucdxml.readme.txt"
curl $UCD_README -o "$UCD_Folder/readme.txt"

StringIOVersion=grep 'UnicodeVersion [0-9]*.[0-9]*.[0-9]*' $OutputFile | tail -c 7
ReadmeVersion=grep -Eom1 'Version [0-9]*.[0-9]*.[0-9]*' $UCD_README | tail -c 7

if [$OutputFileExists == 0] && [StringIOTablesVersion >= ReadmeVersion]
then
# Do not update it
exit 0
else
# Update
rm -rf $OutputFile
touch $OutputFile
UCD_Folder=mktemp -d
UCD_URL="https://www.unicode.org/Public/UCD/latest/ucdxml/ucd.all.flat.zip"
curl $UCD_URL -o "$UCD_Folder/ucd.all.flat.zip" | cd "$UCD_Folder" | unzip ucd.all.flat.zip | rm -f ucd.all.flat.zip
UCD_Data="$UCD_Folder/ucd.all.flat.xml"
UnicodeVersion=xmllint --xpath "//*[local-name()='ucd']/*[local-name()='description']" ucd.all.flat.xml | sed "/^\/ >/d" | sed "s/<[^>]*.//g" | sed "s/Unicode //g"

NumCharNodes=xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -c "count(//u:char)" ucd.all.flat.xml

# ok so we need to extract either cp="", or (first-cp="" AND last-cp="")

echo "#include <stdint.h>  /* Included for the u/intX_t types */" >> $OutputFile
echo "#pragma once" >> $OutputFile
echo "#ifndef LIBBITIO_StringIOTables_h" >> $OutputFile
echo "#define LIBBITIO_StringIOTables_h" >> $OutputFile
echo "#ifdef   __cplusplus" >> $OutputFile
echo "extern   \"C\" {" >> $OutputFile
echo "#endif" >> $OutputFile

echo "#define UnicodeVersion " $UnicodeVersion >> $OutputFile

CodePoint=0
NumericValue=0
while [CodePoint -lt $NumCharNodes]
do
# for each node,we need to check that it's attributes match the ones we're looking for, if there's a match, go ahead and extract the codepoint value and attribute value.
NumeralTable[$CodePoint][$NumericValue] = xmlstarlet select -N u="http://www.unicode.org/ns/2003/ucd/1.0" -t -m "//u:char[$CodePoint @nv != 'NaN']" -v "@cp" -v "@nv" ucd.all.flat.xml
done


echo "const static float Numerals[][2] = {" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile
echo "" >> $OutputFile

echo "#ifdef   __cplusplus" >> $OutputFile
echo "}" >> $OutputFile
echo "#endif /* C++ */" >> $OutputFile

echo "#endif  LIBBITIO_StringIOTables_h" >> $OutputFile
fi





# Ok we're parsing the XML UCD, not straight text version.
# for the number tables, look for class gc="Nd" (Numeral, Decimal), and get it's value from nv="X"

and generate the tables we need from there.

List of tables we need to extract:

Numerals
Precomposed
Uppercase
Lowercase
Combining