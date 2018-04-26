#include <stdarg.h>                    /* Included for va_start, va_end */
#include <stdint.h>                    /* Included for u/intX_t */
#include <stdlib.h>                    /* Included for calloc, and free */

#include "../include/StringIOTables.h" /* Included for the tables, and our declarations */
#include "../include/Math.h"           /* Included for Integer functions */
#include "../include/Log.h"            /* Included for error logging */

#ifdef  __cplusplus
extern  "C" {
#endif
    
    /* Private Functions */
    static uint8_t UTF8_GetCodeUnitSize(UTF8 CodeUnit) {
        uint8_t CodePointSize      = 0;
        if (((CodeUnit & 0x80) >> 7) == 0) {
            CodePointSize          = 1;
        } else if (((CodeUnit & 0xE0) >> 5) == 6) {
            CodePointSize          = 2;
        } else if (((CodeUnit & 0xF0) >> 4) == 14) {
            CodePointSize          = 3;
        } else if (((CodeUnit & 0xF8) >> 3) == 30) {
            CodePointSize          = 4;
        }
        return CodePointSize;
    }
    
    static uint8_t UTF16_GetCodeUnitSize(UTF16 CodeUnit) {
        uint8_t CodePointSize = 0;
        if (CodeUnit >= UTF16HighSurrogateStart && CodeUnit <= UTF16LowSurrogateEnd) {
            CodePointSize     = 2;
        } else {
            CodePointSize     = 1;
        }
        return CodePointSize;
    }
    
    static uint64_t UTF32_GetSizeInUTF8CodeUnits(UTF32 *String2Count) {
        uint64_t CodePoint            = 0ULL;
        uint64_t UTF8CodeUnits        = 0ULL;
        if (String2Count != NULL) {
            do {
                if (String2Count[CodePoint] > UnicodeMaxCodePoint || (String2Count[CodePoint] >= UTF16HighSurrogateStart && String2Count[CodePoint] <= UTF16LowSurrogateEnd)) {
                    String2Count[CodePoint] = InvalidCodePointReplacementCharacter;
                } else if (String2Count[CodePoint] <= 0x7F) {
                    UTF8CodeUnits          += 1;
                } else if (String2Count[CodePoint] >= 0x80 && String2Count[CodePoint] <= 0x7FF) {
                    UTF8CodeUnits          += 2;
                } else if (String2Count[CodePoint] >= 0x800 && String2Count[CodePoint] <= UTF16MaxCodePoint) {
                    UTF8CodeUnits          += 3;
                } else if (String2Count[CodePoint] > UTF16MaxCodePoint && String2Count[CodePoint] <= UnicodeMaxCodePoint) {
                    UTF8CodeUnits          += 4;
                }
                CodePoint                  += 1;
            } while (String2Count[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return UTF8CodeUnits;
    }
    
    static uint64_t UTF32_GetSizeInUTF16CodeUnits(UTF32 *String2Count) {
        uint64_t CodePoint          = 0ULL;
        uint64_t UTF16CodeUnits     = 0ULL;
        if (String2Count != NULL) {
            do {
                if (String2Count[CodePoint] >= UTF16HighSurrogateStart && String2Count[CodePoint] <= UTF16LowSurrogateEnd) {
                    UTF16CodeUnits += 2;
                } else {
                    UTF16CodeUnits += 1;
                }
                CodePoint          += 1;
            } while (String2Count[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return UTF16CodeUnits;
    }
    
    uint64_t UTF32_GetSizeInCodePoints(UTF32 *String2Count) {
        uint64_t NumCodePoints = 0ULL;
        if (String2Count != NULL) {
            do {
                NumCodePoints += 1;
            } while (String2Count[NumCodePoints] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String2Count Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    static uint64_t UTF32_GetSizeInGraphemes(UTF32 *String) {
        uint64_t NumGraphemes  = 0ULL;
        uint64_t CodePoint     = 0ULL;
        if (String != NULL) {
            do {
                for (uint64_t GraphemeExtension = 0; GraphemeExtension < GraphemeExtensionTableSize; GraphemeExtension++) {
                    if (String[CodePoint] == GraphemeExtensionTable[GraphemeExtension]) {
                        NumGraphemes += 1;
                    }
                }
                CodePoint     += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    static UTF32 *UTF32_ReplaceSubString(UTF32 *String, UTF32 *Replacement, uint64_t Offset, uint64_t Length) {
        UTF32 *ReplacedString                 = NULL;
        if (String != NULL && Replacement != NULL && Length >= 1) {
            uint64_t StringSize               = UTF32_GetSizeInCodePoints(String);
            uint64_t ReplacementStringSize    = UTF32_GetSizeInCodePoints(Replacement);
            uint64_t NewStringSize            = 0ULL;
            if (ReplacementStringSize < Length) {
                NewStringSize                 = StringSize - (ReplacementStringSize - Length);
            } else if (ReplacementStringSize >= Length) {
                NewStringSize                 = StringSize + (ReplacementStringSize - Length);
            }
            ReplacedString                    = calloc(NewStringSize, sizeof(UTF32));
            for (uint64_t CodePoint = 0ULL; CodePoint < NewStringSize; CodePoint++) {
                if (CodePoint < Offset) {
                    ReplacedString[CodePoint] = String[CodePoint];
                }
                for (uint64_t ReplacementCodePoint = 0ULL; ReplacementCodePoint < ReplacementStringSize; ReplacementCodePoint++) {
                    ReplacedString[CodePoint] = Replacement[ReplacementCodePoint];
                }
                if (CodePoint > ReplacementStringSize) {
                    ReplacedString[CodePoint] = String[CodePoint];
                }
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (Replacement == NULL) {
            Log(Log_ERROR, __func__, U8("Replacement Pointer is NULL"));
        } else if (Length == 0) {
            Log(Log_ERROR, __func__, U8("Length %llu is too short"), Length);
        }
        return ReplacedString;
    }
    
    /*!
     When Instance is set to -1, all instances will be removed.
     */
    static UTF32 *UTF32_RemoveSubString(UTF32 *String, UTF32 *SubString2Remove, int64_t Instance2Remove) {
        uint64_t SubStringSize     = UTF32_GetSizeInCodePoints(SubString2Remove);
        if (String != NULL && SubString2Remove != NULL) {
            // Ok well we need to start counting how many instances there are, if instance = -1, we need to remove all.
            uint64_t CodePoint     = 0ULL;
            int64_t  FoundInstance = 0ULL;
            /*
             We need to know the size of the substring that way we can loop over the string properly.
             
             Then we need 2 loops, one for the main string, and one for the substring.
             
             Each time a non-matching codepoint is found in the string, we need to reset the substring back to the beginning.
             */
            
            
            do { // We need to get the size of SubString2Remove, and loop over it comparing
                if (Instance2Remove == -1) {
                    // Remove each and every instance of the substring
                } else if (FoundInstance == Instance2Remove) {
                    // Count the instances
                }
                CodePoint += 1;
            } while (String[CodePoint] != NULLTerminator);
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString2Remove == NULL) {
            Log(Log_ERROR, __func__, U8("SubString2Remove Pointer is NULL"));
        }
        return NULL;
    }
    
    static UTF32 *UTF32_Compose(UTF32 *String, bool Kompatibility) {
        if (String != NULL && (Kompatibility == No || Kompatibility == Yes)) {
            // Compose the bitch.
            if (Kompatibility == Yes) {
                // Use the Kompatibility table
            } else {
                // Do not use the Kompatibility table
            }
        } else {
            Log(Log_ERROR, __func__, "String Pointer is NULL");
        }
        return NULL;
    }
    
    static UTF32 *UTF32_Decompose(UTF32 *String, const bool Kompatibility) {
        uint64_t CodePoint      = UTF1632BOMSizeInCodePoints;
        UTF32 *DecomposedString = NULL;
        if (String != NULL && (Kompatibility == No || Kompatibility == Yes)) {
            do {
                for (uint64_t DecomposeCodePoint = 0; DecomposeCodePoint < DecompositionTableSize; DecomposeCodePoint++) {
                    if (String[DecomposeCodePoint] == *DecompositionTable[DecomposeCodePoint][0]) {
                        DecomposedString = UTF32_ReplaceSubString(String, *DecompositionTable[DecomposeCodePoint][1], DecomposeCodePoint, 1);
                    }
                }
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecomposedString;
    }
    
    static UTF32 *UTF32_NormalizeString(UTF32 *String2Normalize, StringIONormalizationForms NormalizedForm) {
        UTF32 *NormalizedString = NULL;
        if (String2Normalize != NULL && NormalizedForm != UnknownNormalizationForm) {
            if (NormalizedForm == NormalizationFormC) {
                UTF32 *Decomposed = UTF32_Decompose(String2Normalize, No);
                NormalizedString  = UTF32_Compose(Decomposed, No);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationFormKC) {
                UTF32 *Decomposed = UTF32_Decompose(String2Normalize, Yes);
                NormalizedString  = UTF32_Compose(Decomposed, Yes);
                free(Decomposed);
            } else if (NormalizedForm == NormalizationFormD) {
                NormalizedString  = UTF32_Decompose(String2Normalize, No);
            } else if (NormalizedForm == NormalizationFormKD) {
                NormalizedString  = UTF32_Decompose(String2Normalize, Yes);
            }
        } else if (String2Normalize == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (NormalizedForm == UnknownNormalizationForm) {
            Log(Log_ERROR, __func__, U8("Unknown Normalization form"));
        }
        return NormalizedString;
    }
    
    static int64_t UTF32_String2Integer(UTF32 *String) {
        uint64_t CodePoint =  0ULL;
        int8_t   Sign      =  1;
        uint8_t  Base      = 10;
        int64_t  Value     =  0;
        if (String != NULL) {
            for (uint64_t WhiteSpace = 0; WhiteSpace < WhiteSpaceTableSize; WhiteSpace++) {
                if (String[CodePoint] == WhiteSpaceTable[WhiteSpace]) {
                    CodePoint    += 1;
                }
            }
            if (String[CodePoint] == '0') {
                if (String[CodePoint + 1] == 'b' || String[CodePoint + 1] == 'B') {
                    Base   = 2;
                } else if (String[CodePoint + 1] == 'o' || String[CodePoint] == 'O') {
                    Base   = 8;
                } else if (String[CodePoint + 1] == 'x' || String[CodePoint] == 'X') {
                    Base   = 16;
                }
            } else if (String[CodePoint] == '-') {
                Sign       = -1;
            }
            do {
                Value     *= Base;
                if (Base == 16 && (String[CodePoint] >= 0x41 && String[CodePoint] <= 0x46)) {
                    Value += (String[CodePoint] - 50);
                } else if (Base == 16 && (String[CodePoint] >= 0x61 && String[CodePoint] <= 0x66)) {
                    Value += (String[CodePoint] - 82);
                } else if ((Base == 10 || Base == 16) && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x39)) {
                    Value += (String[CodePoint] - 48);
                } else if (Base == 8 && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x37)) {
                    Value += (String[CodePoint] - 48);
                } else if (Base == 2 && (String[CodePoint] >= 0x30 && String[CodePoint] <= 0x31)) {
                    Value += (String[CodePoint] - 48);
                }
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return (Sign < 0 ? Value * Sign : Value);
    }
    
    static UTF32 *UTF32_Integer2String(const StringIOBases Base, const bool UpperCase, int64_t Integer2Convert) {
        int64_t  Sign            = 0LL;
        int64_t  Num             = Integer2Convert;
        uint8_t  NumDigits       = 0;
        while (Num > 0) {
            Num                 /= Base;
            NumDigits           += 1;
        }
        if (GlobalByteOrder == LSByteFirst) {
            Integer2Convert      = SwapEndian64(Integer2Convert);
        }
        if (Integer2Convert < 0 && Base == Decimal) {
            Sign                 = -1;
            NumDigits           +=  1;
        }
        UTF32 *NumberString      = calloc(NumDigits + NULLTerminatorSize, sizeof(UTF32));
        UTF32  UpperNumerals[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        UTF32  LowerNumerals[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        if (NumberString != NULL) {
            for (uint64_t CodePoint = NumDigits - 1; CodePoint > 0; CodePoint--) {
                // Ok, so we've clamped the looping, now all we need to do is malloc a string with NumDigits as it's size.
                // Now we just AND the number with Bit to get it's value, if the value is a 1 write a 1, otherwise write a zero
                int64_t CurrentDigit    = (Base == Decimal ? Absolute(Integer2Convert %= Base) : (Integer2Convert %= Base));
                NumberString[CodePoint] = (UpperCase == Yes ? UpperNumerals[CurrentDigit] : LowerNumerals[CurrentDigit]);
            }
        }
        return NumberString;
    }
    
    static double UTF32_String2Decimal(UTF32 *String) { // Replaces strtod and atof
        /*
         Invalid strings result in 0.0 since Mac doesn't support NaN.
         */
        uint64_t CodePoint = 0;
        if (String != NULL) {
            // Extract the sign bit
            // Extract the Significand
            // Extract the Base
            // Extract the Exponent
            bool IsNegative        = 0;
            for (uint8_t Whitespace = 0; Whitespace < 99; Whitespace++) {
                do {
                    if (String[CodePoint] == '-') {
                        IsNegative = Yes;
                    }
                    CodePoint     += 1;
                } while (String[CodePoint] != NULLTerminator);
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return 0.0;
    }
    
    static UTF32 *UTF32_Decimal2String(double Decimal) {
        /*
         Should we support Binary Coded Decimal BCD?
         Invalid doubles return a NULL string
         */
        UTF32   *OutputString = NULL;
        int8_t   Sign         = Decimal < 0.0 ? -1 : 1;
        int32_t *Exponent     = 0;
        int32_t  Exponent2    = Exponent;
        float    Mantissa     = frexp(Decimal, Exponent);
        float    Mantissa2    = Mantissa;
        
        // Ok now we just need to get the strings size
        uint64_t StringSize   = UTF1632BOMSizeInCodePoints; // 1 to account for the BOM
        if (Sign == -1) {
            StringSize       += 1;
        }
        while (Exponent2 > 10) {
            Exponent2        /= 10;
            StringSize       +=  1;
        }
        while (Mantissa2 > 0) {
            Mantissa2        /= 10;
            StringSize       +=  1;
        }
        OutputString          = calloc(StringSize, sizeof(UTF32));
        // Now we go ahead and create the string
        if (GlobalByteOrder == LSByteFirst) {
            OutputString[0]   = UTF32LE;
        } else if (GlobalByteOrder == MSByteFirst) {
            OutputString[0]   = UTF32BE;
        }
        if (Sign == -1) {
            OutputString[1]   = '-';
        }
        // Now we start popping in the other variables, first is the Exponent.
        while (*Exponent > 0) { // TODO: This assumes there's only 1 codepoint nessicary to express teh exponent
            OutputString[2]   = *Exponent /= 10;
        }
        OutputString[3]       = '.';
        // Now let's start popping in the Mantissa
        while (Mantissa > 0) { // TODO: This assumes there's only 1 codepoint nessicary to express teh exponent
            OutputString[4]   = Mantissa /= 10;
        }
        return OutputString;
    }
    
    static UTF32 *UTF32_FormatString(UTF32 *Format, const uint64_t NumArguments, va_list Arguments) {
        /* So, first we need to break the Format string into the various printf specifiers, which start with a percent symbol
         
         Get the size of the format string in CodePoints, normalize the string, then start counting the format specifiers */
        uint64_t  CodePoint                   = 0ULL;
        uint64_t  SpecifierStart              = 0ULL;
        uint64_t  SpecifierEnd                = 0ULL;
        uint64_t  CurrentSpecifier            = 0ULL;
        uint8_t   FormatSpecifierIndex        = 0ULL;
        uint64_t *SpecifierOffset             = calloc(NumArguments, sizeof(uint64_t));
        uint64_t *SpecifierSize               = calloc(NumArguments, sizeof(uint64_t));
        uint64_t FormatStringSize             = UTF32_GetSizeInCodePoints(Format);
        
        if (Format != NULL) {
            do {
                if (Format[CodePoint] == '%') {
                    // Check Flags
                    if (Format[CodePoint] == '-') {
                        // Left align
                    } else if (Format[CodePoint] == '+') {
                        // Add a sign to the number
                    } else if (Format[CodePoint] == '0') {
                        // Pad with leading zeros
                    } else if (Format[CodePoint] == ' ') { // Space
                        // Pad with spaces
                    } else if (Format[CodePoint] == '#') {
                        // Prefix with base specifier, o for octal, 0x or 0X for Hex
                    }
                    
                    // Check Width
                    if (Format[CodePoint] == '*') { // Read the number from the Arguments
                        
                    } else if (Format[CodePoint] == '0' || Format[CodePoint] == '1' || Format[CodePoint] == '2' || Format[CodePoint] == '3') {
                        
                    }
                    
                    // Check Precision
                }
                CodePoint += 1;
            } while (Format[CodePoint] != NULLTerminator && Format[CodePoint + 1] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("Format Pointer is NULL"));
        }
        return NULL;
        
        // %05d means the format specifier is decimal, with 5 digits, with leading zeros if nessicary.
        
        /* The general idea is to loop looking for a percent codepoint, then loop until you find a valid format specifier character, everything in between those 2 is the format string. */
        /* List of Format specifiers:
         
         */
        if (Format != NULL) {
            do {
                /* Loop over the string looking for a format specifier start and end point, recording the offset in the string and length of the format specifier when found */
                if (Format[CodePoint] == '%') {
                    // Record the location of the
                    SpecifierStart            = CodePoint;
                }
                if (Format[CodePoint + 1] == '%' && CodePoint > SpecifierStart) {
                    SpecifierEnd              = CodePoint;
                }
                CodePoint                    += 1;
            } while (Format[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NULL;
    }
    
    int64_t UTF32_FindSubString(UTF32 *String, UTF32 *SubString, uint64_t Offset, int64_t Length) {
        uint64_t StringSize            = UTF32_GetSizeInCodePoints(String);
        uint64_t SubStringSize         = UTF32_GetSizeInCodePoints(SubString);
        int64_t  MatchingOffset        = 0LL;
        if (String != NULL && SubString != NULL && StringSize < Offset + Length) {
            for (uint64_t SubCodePoint = 0; SubCodePoint < SubStringSize; SubCodePoint++) {
                for (uint64_t StringCodePoint = Offset; StringCodePoint < Offset + Length; StringCodePoint++) {
                    if (String[StringCodePoint] != SubString[SubCodePoint]) {
                        MatchingOffset = -1;
                    }
                }
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (SubString == NULL) {
            Log(Log_ERROR, __func__, U8("SubString Pointer is NULL"));
        } else if (Length >= 1 && StringSize < Offset + Length) {
            Log(Log_ERROR, __func__, U8("Offset %llu + Length %lld is larger than String %llu"), Offset, Length, StringSize);
        }
        return MatchingOffset;
    }
    
    UTF32 *UTF32_ExtractSubString(UTF32 *String, uint64_t Offset, uint64_t Length) {
        uint64_t  StringSize                            = UTF32_GetSizeInCodePoints(String);
        uint64_t  ExtractedStringSize                   = Length + UTF1632BOMSizeInCodePoints + NULLTerminatorSize;
        UTF32    *ExtractedString                       = NULL;
        if (String != NULL && StringSize >= Length + Offset) {
            ExtractedString                             = calloc(ExtractedStringSize, sizeof(UTF32));
            if (ExtractedString != NULL) {
                for (uint64_t CodePoint = Offset; CodePoint < Offset + Length; CodePoint++) {
                    ExtractedString[CodePoint - Offset] = String[CodePoint];
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate ExtractedString"));
            }
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (StringSize < Length + Offset) {
            Log(Log_ERROR, __func__, U8("Offset %llu + Length %lld is larger than String %llu"), Length, Offset, StringSize);
        }
        return ExtractedString;
    }
    
    /*!
     Delimiters: [=][..][:]
     Delimiter Array: UTF32 **DelimiterArray = {{-},{=},{:},{--},{..}}
     Example String1: --Input=/Users/Marcus/Desktop/ElephantsDream_%05d.png;        [2][Input][/Users/Marcus/Desktop/ElephantsDream_%05d.png]
     Example String2: --LeftEye=/Users/Marcus/Desktop/ElephantsDream_Left_%05d.png; [2][LeftEye][/Users/Marcus/Desktop/ElephantsDream_Left_%05d.png]
     Example String3: --FrameRate=24000:1001;                                       [3][FrameRate][24000][1001]
     Example String4: --FrameRange=0..15691;                                        [3][FrameRange][0][15691]
     */
    
    static UTF32 **UTF32_SplitString(UTF32 *String2Split, uint64_t NumDelimiters, UTF32 **Delimiters) {
        // We once again need to loop over the string getting the number of substrings and their sizes.
        uint64_t NumSubStrings = 1ULL; // 1 so we can put the count as the first variable
        uint64_t StringSize    = UTF32_GetSizeInCodePoints(String2Split);
        uint64_t CodePoint     = 0ULL;
        if (String2Split != NULL && NumDelimiters > 0 && Delimiters != NULL) {
            // The aasy way would be to just loop over the string NumDelimiters times, counting the number of occurances for each delimiter in the string
            
            
            /* ALL of the logic: The string and delimiter strings must not be NULL, the strings needs to be at least as long as the longest delimiter,  */
            
            
            for (uint64_t Delimiter = 0ULL; Delimiter < NumDelimiters; Delimiter++) {
                for (uint64_t StringCodePoint = 0ULL; StringCodePoint < StringSize; StringCodePoint++) {
                    if (String2Split[StringCodePoint] == Delimiters[Delimiter][0]) { // We've found the start of a delimiter, now we just ned to make sure it fits the rest
                        for (uint64_t DelimiterCodePoint = 0ULL; DelimiterCodePoint < UTF32_GetSizeInCodePoints(Delimiters[Delimiter]); DelimiterCodePoint++) {
                            // Start looping?
                        }
                    }
                }
            }
        } else if (String2Split == NULL) {
            Log(Log_ERROR, __func__, U8("String2Split Pointer is NULL"));
        } else if (NumDelimiters == 0) {
            Log(Log_ERROR, __func__, U8("There are no delimiters"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF32 *AppendString(UTF32 *String1, UTF32 *String2) {
        UTF32 *NewString = NULL;
        if (String1 != NULL && String2 != NULL) {
            // Get the size of String1, and String2; then add them together + 1 to get the final string, actually plus nothing because there's an extra BOM in string2.
            uint64_t String1Size = UTF32_GetSizeInCodePoints(String1);
            uint64_t String2Size = UTF32_GetSizeInCodePoints(String2);
            uint64_t NewStringSize = String1Size + String2Size;// By pure happenstance, I don't need to account for the BOM or NULL.
            NewString = calloc(NewStringSize, sizeof(UTF32));
            // Ok, well then let's go ahead and start copying String1 until the end then String2 until it's end
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 is NULL"));
        }
        return NewString;
    }
    
    UTF32 *UTF32_CaseFoldString(UTF32 *String) {
        uint64_t CodePoint        = UTF1632BOMSizeInCodePoints;
        UTF32   *CaseFoldedString = NULL;
        if (String != NULL) {
            do {
                for (uint64_t CaseFoldCodePoint = 0; CaseFoldCodePoint < CaseFoldTableSize; CaseFoldCodePoint++) {
                    // We need to compare these as strings
                    if (String[CodePoint] == CaseFoldTable[CaseFoldCodePoint][0]) {
                        CaseFoldedString = UTF32_ReplaceSubString(String, *CaseFoldTable[CaseFoldCodePoint][1], CaseFoldCodePoint, 1);
                    }
                }
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return CaseFoldedString;
    }
    
    static bool UTF32_Compare(UTF32 *String1, UTF32 *String2) {
        uint64_t CodePoint           = 0ULL;
        bool StringsMatch            = Yes;
        if (String1 != NULL && String2 != NULL) {
            uint64_t String1Size     = UTF32_GetSizeInCodePoints(String1);
            uint64_t String2Size     = UTF32_GetSizeInCodePoints(String2);
            if (String1Size != String2Size) {
                StringsMatch         = No;
            } else {
                do {
                    if (String1[CodePoint] != String2[CodePoint]) {
                        StringsMatch = No;
                        break;
                    }
                } while (String1[CodePoint] != NULLTerminator && String2[CodePoint] != NULLTerminator);
            }
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    void UTF32_ReorderString(UTF32 *String) {
        uint64_t CodePoint        = 0UL;
        uint32_t CurrentCodePoint = 0UL;
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        if (String != NULL) {
            // The idea here is to loop over the string, checking if each codepoint is a combining character
            uint32_t StringByteOrder = String[0];
            do {
                if ((StringByteOrder == UTF32LE && GlobalByteOrder == MSByteFirst) || (StringByteOrder == UTF32BE && GlobalByteOrder == LSByteFirst)) {
                    // We need to swap the byte order for each cdepoint
                    CurrentCodePoint = SwapEndian32(String[CodePoint]);
                } else {
                    CurrentCodePoint = String[CodePoint];
                }
                // If the current codepoint is a combining codepoint, we need to save the position of the last base character, and move it around based on that.
            } while (String[CodePoint] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
    }
    /* Private Functions */
    
    /* Public Functions */
    uint64_t UTF8_GetSizeInCodeUnits(UTF8 *String2Count) {
        uint64_t NumCodeUnits    = 0ULL;
        uint64_t CurrentCodeUnit = 0ULL;
        if (String2Count != NULL) {
            do {
                NumCodeUnits    += UTF8_GetCodeUnitSize(String2Count[CurrentCodeUnit]);
            } while (String2Count[CurrentCodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String2Count Pointer is NULL"));
        }
        return NumCodeUnits;
    }
    
    uint64_t UTF16_GetSizeInCodeUnits(UTF16 *String2Count) {
        uint64_t NumCodeUnits = 0ULL;
        if (String2Count != NULL) {
            do {
                NumCodeUnits += UTF16_GetCodeUnitSize(String2Count[NumCodeUnits]);
            } while (String2Count[NumCodeUnits] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String2Count Pointer is NULL"));
        }
        return NumCodeUnits;
    }
    
    uint64_t UTF8_GetSizeInCodePoints(UTF8 *String2Count) {
        uint64_t NumCodePoints   = 0ULL;
        uint64_t CurrentCodeUnit = 0ULL;
        if (String2Count != NULL) {
            do {
                CurrentCodeUnit += UTF8_GetCodeUnitSize(String2Count[CurrentCodeUnit]);
                NumCodePoints   += 1;
            } while (String2Count[CurrentCodeUnit] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String2Count Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF16_GetSizeInCodePoints(UTF16 *String2Count) {
        uint64_t NumCodeUnits  = 0ULL;
        uint64_t NumCodePoints = 0ULL;
        if (String2Count != NULL) {
            do {
                NumCodeUnits  += UTF16_GetCodeUnitSize(String2Count[NumCodeUnits]);
                NumCodePoints += 1;
            } while (String2Count[NumCodePoints] != NULLTerminator);
        } else {
            Log(Log_ERROR, __func__, U8("String2Count Pointer is NULL"));
        }
        return NumCodePoints;
    }
    
    uint64_t UTF8_GetSizeInGraphemes(UTF8 *String) {
        uint64_t NumGraphemes  = 0ULL;
        if (String != NULL) {
            UTF32 *Decoded     = UTF8_Decode(String);
            NumGraphemes       = UTF32_GetSizeInGraphemes(Decoded);
            free(Decoded);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    uint64_t UTF16_GetSizeInGraphemes(UTF16 *String) {
        uint64_t NumGraphemes  = 0ULL;
        if (String != NULL) {
            UTF32 *Decoded     = UTF16_Decode(String);
            NumGraphemes       = UTF32_GetSizeInGraphemes(Decoded);
            free(Decoded);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return NumGraphemes;
    }
    
    UTF32 *UTF8_Decode(UTF8 *String) {
        uint8_t  CodePointSize                       = 0;
        uint64_t CodeUnitNum                         = 0ULL;
        uint64_t CodePoint                           = 0ULL;
        UTF32   *DecodedString                       = NULL;
        if (String != NULL) {
            uint64_t NumCodePoints                   = UTF1632BOMSizeInCodePoints + UTF8_GetSizeInCodePoints(String) + NULLTerminatorSize;
            if (String[0] == 0xEF && String[1] == 0xBB && String[2] == 0xBF) { // We need to remove the BOM, and subtract it's size from the Num codepoints
                NumCodePoints                       -= UTF8BOMSizeInCodePoints;
                CodeUnitNum                         += UTF8BOMSizeInCodeUnits;
            }
            DecodedString                            = calloc(NumCodePoints, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if (CodePoint == 0) {
                        // set the Unicode BOM
                        if (GlobalByteOrder == LSByteFirst) {
                            DecodedString[0]         = UTF32LE;
                        } else if (GlobalByteOrder == MSByteFirst) {
                            DecodedString[0]         = UTF32BE;
                        }
                    }
                    switch (UTF8_GetCodeUnitSize(String[CodeUnitNum])) { // UTF-8 is MSB first, if the platform is LSB first, we need to swap as we read
                        case 1:
                            DecodedString[CodePoint] =  String[CodeUnitNum];
                            CodeUnitNum             += 1;
                            break;
                        case 2:
                            DecodedString[CodePoint] = (String[CodeUnitNum] & 0x1F) << 6;
                            DecodedString[CodePoint] =  String[CodeUnitNum] & 0x3F;
                            CodeUnitNum             += 2;
                            break;
                        case 3:
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x0F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x1F) << 6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 2] & 0x1F);
                            CodeUnitNum             += 3;
                            break;
                        case 4:
                            DecodedString[CodePoint] = (String[CodeUnitNum]     & 0x07) << 18;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 1] & 0x3F) << 12;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 2] & 0x3F) <<  6;
                            DecodedString[CodePoint] = (String[CodeUnitNum + 3] & 0x3F);
                            CodeUnitNum             += 4;
                            break;
                    }
                    if (GlobalByteOrder == LSByteFirst) {
                        DecodedString[CodePoint]     = SwapEndian32(String[CodePoint]);
                    }
                    
                    if (CodePointSize > 1 && DecodedString[CodePoint] <= 0x7F) {
                        DecodedString[CodePoint]     = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("CodePoint %d is overlong, replaced with U+FFFD"), DecodedString[CodePoint]);
                    } else if (DecodedString[CodePoint] >= UTF16HighSurrogateStart && DecodedString[CodePoint] <= UTF16LowSurrogateEnd && DecodedString[CodePoint] != UTF32LE && DecodedString[CodePoint] != UTF32BE) {
                        DecodedString[CodePoint]     = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, because it overlaps the Surrogate Pair Block, it was replaced with U+FFFD"), DecodedString[CodePoint]);
                    }
                } while (String[CodeUnitNum] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate DecodedString"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF32 *UTF16_Decode(UTF16 *String) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        uint64_t CodePoint                       = 0ULL;
        uint64_t CodeUnitNum                     = 0ULL;
        UTF16    StringByteOrder                 = 0U;
        UTF32   *DecodedString                   = NULL;
        if (String != NULL) {
            StringByteOrder                      = String[0];
            uint64_t UTF16CodeUnits              = UTF16_GetSizeInCodePoints(String) + NULLTerminatorSize;
            DecodedString                        = calloc(UTF16CodeUnits, sizeof(UTF32));
            if (DecodedString != NULL) {
                do {
                    if ((StringByteOrder == UTF16LE && GlobalByteOrder == MSByteFirst) || (StringByteOrder == UTF16BE && GlobalByteOrder == LSByteFirst)) {
                        String[CodePoint]        = SwapEndian16(String[CodePoint]);
                        if (CodePoint == 0) {
                            if (GlobalByteOrder == LSByteFirst) {
                                DecodedString[0] = UTF16LE;
                            } else if (GlobalByteOrder == MSByteFirst) {
                                DecodedString[0] = UTF16BE;
                            }
                        }
                    }
                    if (String[CodeUnitNum] <= UTF16HighSurrogateStart - 1 || (String[CodeUnitNum] >= UTF16LowSurrogateEnd + 1 && String[CodeUnitNum] <= UTF16MaxCodePoint)) {
                        DecodedString[CodePoint] = String[CodeUnitNum];
                    } else {
                        uint16_t HighSurrogate   = (String[CodeUnitNum]     - UTF16HighSurrogateStart) * UTF16SurrogatePairModDividend;
                        uint16_t LowSurrogate    =  String[CodeUnitNum + 1] - UTF16LowSurrogateStart;
                        DecodedString[CodePoint] = HighSurrogate + LowSurrogate + UTF16SurrogatePairStart;
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("DecodedString Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return DecodedString;
    }
    
    UTF8 *UTF8_Encode(UTF32 *String, const bool IncludeBOM) {
        if (GlobalByteOrder == UnknownByteFirst || GlobalBitOrder == UnknownBitFirst) {
            GetRuntimeByteBitOrder();
        }
        uint64_t CodePoint                             = 0ULL;
        uint64_t CodeUnitNum                           = 0ULL;
        UTF8    *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t UTF8CodeUnits                     = NULLTerminatorSize + UTF32_GetSizeInUTF8CodeUnits(String) + (IncludeBOM == true ? UTF8BOMSizeInCodeUnits : 0);
            EncodedString                              = calloc(UTF8CodeUnits, sizeof(UTF8));
            if (EncodedString != NULL) {
                do {
                    if (GlobalByteOrder == LSByteFirst) {
                        String[CodePoint]              = SwapEndian32(String[CodePoint]);
                    }
                    if (CodeUnitNum == 0 && IncludeBOM == Yes) {
                        EncodedString[CodeUnitNum]     = 0xEF;
                        EncodedString[CodeUnitNum + 1] = 0xBB;
                        EncodedString[CodeUnitNum + 2] = 0xBF;
                        CodeUnitNum                   += UTF8BOMSizeInCodeUnits;
                    }
                    if (String[CodePoint] <= 0x7F) {
                        EncodedString[CodeUnitNum]     = String[CodePoint];
                        CodeUnitNum                   += 1;
                    } else if (String[CodePoint] >= 0x80 && String[CodePoint] <= 0x7FF) {
                        EncodedString[CodeUnitNum]     = (0xC0 & ((String[CodePoint] & 0x7C0) >> 6));
                        EncodedString[CodeUnitNum + 1] = (0x80 &  (String[CodePoint] & 0x03F));
                        CodeUnitNum                   += 2;
                    }  else if (String[CodePoint] >= 0x800 && String[CodePoint] <= UTF16MaxCodePoint && String[CodePoint] < UTF16HighSurrogateStart && String[CodePoint] > UTF16LowSurrogateEnd && String[CodePoint] != UTF16LE && String[CodePoint] != UTF16BE) {
                        EncodedString[CodeUnitNum]     = (0xE0 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 1] = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 2] = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 3;
                    } else if (String[CodePoint] > UTF16MaxCodePoint && String[CodePoint] <= UnicodeMaxCodePoint) {
                        EncodedString[CodeUnitNum]     = (0xF0 & ((String[CodePoint] & 0x1C0000) >> 18));
                        EncodedString[CodeUnitNum + 1] = (0x80 & ((String[CodePoint] & 0x03F000) >> 12));
                        EncodedString[CodeUnitNum + 2] = (0x80 & ((String[CodePoint] & 0x000FC0) >>  6));
                        EncodedString[CodeUnitNum + 3] = (0x80 &  (String[CodePoint] & 0x00003F));
                        CodeUnitNum                   += 4;
                    } else if (String[CodePoint] >= UTF16HighSurrogateStart && String[CodePoint] <= UTF16LowSurrogateEnd && String[CodePoint] != UTF32LE && String[CodePoint] != UTF32BE) {
                        String[CodePoint]              = InvalidCodePointReplacementCharacter;
                        Log(Log_ERROR, __func__, U8("Codepoint %d is invalid, overlaps Surrogate Pair Block, replacing with U+FFFD"));
                    }
                } while (String[CodePoint] != NULLTerminator);
            } else {
                Log(Log_ERROR, __func__, U8("String Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF16 *UTF16_Encode(UTF32 *String, StringIOByteOrders OutputByteOrder) {
        if (GlobalByteOrder == UnknownByteFirst) {
            GetRuntimeByteBitOrder();
        }
        UTF16   *EncodedString                         = NULL;
        if (String != NULL) {
            uint64_t CodeUnitNum                       = 0ULL;
            uint64_t UTF16NumCodeUnits                 = UTF1632BOMSizeInCodePoints + UTF32_GetSizeInUTF16CodeUnits(String) + NULLTerminatorSize;
            EncodedString                              = calloc(UTF16NumCodeUnits, sizeof(UTF16));
            if (EncodedString != NULL) {
                for (uint64_t CodeUnit = 0ULL; CodeUnit < UTF16NumCodeUnits; CodeUnit++) {
                    if ((OutputByteOrder == UseBEByteOrder && GlobalByteOrder == LSByteFirst) || (OutputByteOrder == UseLEByteOrder && GlobalByteOrder == MSByteFirst)) {
                        String[CodeUnit]               = SwapEndian32(String[CodeUnit]);
                    }
                    if (CodeUnit == 0) {
                        if (OutputByteOrder == UseNativeByteOrder) {
                            if (GlobalByteOrder == LSByteFirst) {
                                EncodedString[0]       = UTF16LE;
                            } else if (GlobalByteOrder == MSByteFirst) {
                                EncodedString[0]       = UTF16BE;
                            }
                        } else if (OutputByteOrder == UseBEByteOrder) {
                            EncodedString[0]           = UTF16BE;
                        } else if (OutputByteOrder == UseLEByteOrder) {
                            EncodedString[0]           = UTF16LE;
                        }
                    }
                    if (String[CodeUnitNum] < UTF16HighSurrogateStart || (String[CodeUnitNum] > UTF16LowSurrogateEnd && String[CodeUnitNum] <= UTF16MaxCodePoint)) { // Single code point
                        EncodedString[CodeUnitNum]     = String[CodeUnit];
                    } else {
                        uint16_t HighSurrogate         = ((String[CodeUnitNum] - UTF16SurrogatePairStart) / UTF16SurrogatePairModDividend) + UTF16HighSurrogateStart;
                        uint16_t LowSurrogate          = ((String[CodeUnitNum] - UTF16SurrogatePairStart) % UTF16SurrogatePairModDividend) + UTF16LowSurrogateStart;
                        if ((OutputByteOrder == UseLEByteOrder && GlobalByteOrder == MSByteFirst) || (OutputByteOrder == UseBEByteOrder && GlobalByteOrder == LSByteFirst)) {
                            HighSurrogate              = SwapEndian16(HighSurrogate);
                            LowSurrogate               = SwapEndian16(LowSurrogate);
                        }
                        EncodedString[CodeUnitNum]     = HighSurrogate;
                        EncodedString[CodeUnitNum + 1] = LowSurrogate;
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("EncodedString Pointer is NULL, not enough memory"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return EncodedString;
    }
    
    UTF8 *UTF8_NormalizeString(UTF8 *String2Normalize, StringIONormalizationForms NormalizedForm) {
        UTF32 *String32           = UTF8_Decode(String2Normalize);
        UTF32 *NormalizedString32 = UTF32_NormalizeString(String32, NormalizedForm);
        free(String32);
        UTF8  *NormalizedString8  = UTF8_Encode(NormalizedString32, No);
        return NormalizedString8;
    }
    
    UTF16 *UTF16_NormalizeString(UTF16 *String2Normalize, StringIONormalizationForms NormalizedForm) {
        UTF32 *String32           = UTF16_Decode(String2Normalize);
        UTF32 *NormalizedString32 = UTF32_NormalizeString(String32, NormalizedForm);
        free(String32);
        UTF16 *NormalizedString16  = UTF16_Encode(NormalizedString32, UseNativeByteOrder);
        return NormalizedString16;
    }
    
    int64_t UTF8_String2Integer(UTF8 *String) {
        UTF32 *String32 = UTF8_Decode(String);
        int64_t Value   = UTF32_String2Integer(String32);
        free(String32);
        return Value;
    }
    
    int64_t UTF16_String2Integer(UTF16 *String) {
        UTF32 *String32 = UTF16_Decode(String);
        int64_t Value   = UTF32_String2Integer(String32);
        free(String32);
        return Value;
    }
    
    UTF8 *UTF8_Integer2String(const StringIOBases Base, const bool UpperCase, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, UpperCase, Integer2Convert);
        UTF8  *IntegerString8  = UTF8_Encode(IntegerString32, No);
        free(IntegerString32);
        return IntegerString8;
    }
    
    UTF16 *UTF16_Integer2String(const StringIOBases Base, const bool UpperCase, int64_t Integer2Convert) {
        UTF32 *IntegerString32 = UTF32_Integer2String(Base, UpperCase, Integer2Convert);
        UTF16 *IntegerString16 = UTF16_Encode(IntegerString32, UseNativeByteOrder);
        free(IntegerString32);
        return IntegerString16;
    }
    
    double UTF8_String2Decimal(UTF8 *String) {
        double Decimal = 0.0;
        if (String != NULL) {
            // Decode the string, then send it on over to UTF32_String2Decimal, then free the utf32 string
            UTF32 *String32 = UTF8_Decode(String);
            Decimal         = UTF32_String2Decimal(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    double UTF16_String2Decimal(UTF16 *String) {
        double Decimal = 0.0;
        if (String != NULL) {
            // Decode the string, then send it on over to UTF32_String2Decimal, then free the utf32 string
            UTF32 *String32 = UTF16_Decode(String);
            Decimal         = UTF32_String2Decimal(String32);
            free(String32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Decimal;
    }
    
    UTF8 *UTF8_Decimal2String(double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Decimal);
        UTF8  *String8  = UTF8_Encode(String32, No);
        free(String32);
        return String8;
    }
    
    UTF16 *UTF16_Decimal2String(double Decimal) {
        UTF32 *String32 = UTF32_Decimal2String(Decimal);
        UTF16 *String16 = UTF16_Encode(String32, UseNativeByteOrder);
        free(String32);
        return String16;
    }
    
    void UTF8_WriteString2File(UTF8 *String, FILE *OutputFile) {
        if (String != NULL && OutputFile != NULL) {
#if   (FoundationIOTargetOS == POSIXOS)
            fputs(String, OutputFile);
#elif (FoundationIOTargetOS == WindowsOS)
            UTF32 *StringUTF32   = UTF8_Decode(String);
            UTF16 *StringUTF16   = UTF16_Encode(StringUTF32, UseNativeByteOrder);
            free(StringUTF32);
            fputws(StringUTF16, OutputFile);
            free(StringUTF16);
#endif
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_ERROR, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    void UTF16_WriteString2File(UTF16 *String, FILE *OutputFile) {
        if (String != NULL && OutputFile != NULL) {
#if   (FoundationIOTargetOS == POSIXOS)
            // Convert from UTF16 to UTF8 then output
            UTF32 *StringUTF32 = UTF16_Decode(String);
            UTF8  *StringUTF8  = UTF8_Encode(StringUTF32, No);
            free(StringUTF32);
            fputs(StringUTF8, OutputFile);
#elif (FoundationIOTargetOS == WindowsOS)
            fputws(String, OutputFile);
#endif
        } else if (String == NULL) {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        } else if (OutputFile == NULL) {
            Log(Log_ERROR, __func__, U8("FILE Pointer is NULL"));
        }
    }
    
    bool UTF8_Compare(UTF8 *String1, UTF8 *String2, StringIONormalizationForms NormalizedForm, bool CaseInsensitive) {
        bool StringsMatch          = No;
        // Well we're always going to assume that the strings have been casefolded and normalized before hand.
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32    = UTF8_Decode(String1);
            UTF32 *String2UTF32    = UTF8_Decode(String2);
            UTF32 *Normalized1     = UTF32_NormalizeString(String1UTF32, NormalizedForm);
            free(String1UTF32);
            UTF32 *Normalized2     = UTF32_NormalizeString(String2UTF32, NormalizedForm);
            free(String2UTF32);
            if (CaseInsensitive == Yes) {
                UTF32 *CaseFolded1 = UTF32_CaseFoldString(Normalized1);
                free(Normalized1);
                UTF32 *CaseFolded2 = UTF32_CaseFoldString(Normalized2);
                free(Normalized2);
                StringsMatch       = UTF32_Compare(CaseFolded1, CaseFolded2);
            } else {
                StringsMatch       = UTF32_Compare(Normalized1, Normalized2);
                free(Normalized1);
                free(Normalized2);
            }
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    bool UTF16_Compare(UTF16 *String1, UTF16 *String2, StringIONormalizationForms NormalizedForm, bool CaseInsensitive) {
        bool StringsMatch          = No;
        if (String1 != NULL && String2 != NULL) {
            UTF32 *String1UTF32    = UTF16_Decode(String1);
            UTF32 *String2UTF32    = UTF16_Decode(String2);
            UTF32 *Normalized1     = UTF32_NormalizeString(String1UTF32, NormalizedForm);
            free(String1UTF32);
            UTF32 *Normalized2     = UTF32_NormalizeString(String2UTF32, NormalizedForm);
            free(String2UTF32);
            if (CaseInsensitive == Yes) {
                UTF32 *CaseFolded1 = UTF32_CaseFoldString(Normalized1);
                free(Normalized1);
                UTF32 *CaseFolded2 = UTF32_CaseFoldString(Normalized2);
                free(Normalized2);
                StringsMatch       = UTF32_Compare(CaseFolded1, CaseFolded2);
            } else {
                StringsMatch       = UTF32_Compare(Normalized1, Normalized2);
                free(Normalized1);
                free(Normalized2);
            }
        } else if (String1 == NULL) {
            Log(Log_ERROR, __func__, U8("String1 Pointer is NULL"));
        } else if (String2 == NULL) {
            Log(Log_ERROR, __func__, U8("String2 Pointer is NULL"));
        }
        return StringsMatch;
    }
    
    UTF8 *UTF8_FormatString(UTF8 *Format, ...) {
        UTF8 *Format8 = NULL;
        if (Format != NULL) {
            // Decode the Format string to UTF32, and try to see if any strings in the argument list are UTF8 encoded and decode them too.
            UTF32 *Format32           = UTF8_Decode(Format);
            va_list VariadicArguments;
            va_start(VariadicArguments, Format);
            UTF32 *FormattedString    = UTF32_FormatString(Format32, VariadicArguments);
            free(Format32);
            va_end(VariadicArguments);
            Format8                   = UTF8_Encode(FormattedString, No);
            free(FormattedString);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Format8;
    }
    
    UTF16 *UTF16_FormatString(UTF16 *Format, ...) {
        UTF16 *Format16 = NULL;
        if (Format != NULL) {
            UTF32 *Format32           = UTF16_Decode(Format);
            va_list VariadicArguments;
            va_start(VariadicArguments, Format);
            UTF32 *FormattedString    = UTF32_FormatString(Format32, VariadicArguments);
            free(Format32);
            va_end(VariadicArguments);
            Format16                  = UTF16_Encode(FormattedString, UseNativeByteOrder);
            free(FormattedString);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Format16;
    }
    
    UTF8 **UTF8_SplitString(UTF8 *String2Split, uint64_t NumDelimiters, UTF8 **Delimiters) {
        if (String2Split != NULL && NumDelimiters > 0 && Delimiters != NULL) {
            // Decode String2Split, and make an array of decoded strings for the Delimiters
            UTF32  *String2Split32      = UTF8_Decode(String2Split);
            UTF32 **Delimiters32        = calloc(NumDelimiters, sizeof(UTF32*));
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                Delimiters32[Delimiter] = UTF8_Decode(Delimiters[Delimiter]);
            }
            UTF32_SplitString(String2Split32, NumDelimiters, Delimiters32);
            free(String2Split32);
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                free(Delimiters32[Delimiter]);
            }
            free(Delimiters32);
        } else if (String2Split == NULL) {
            Log(Log_ERROR, __func__, U8("String2Split Pointer is NULL"));
        } else if (NumDelimiters == 0) {
            Log(Log_ERROR, __func__, U8("There are no delimiters"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF16 **UTF16_SplitString(UTF16 *String2Split, uint64_t NumDelimiters, UTF16 **Delimiters) {
        if (String2Split != NULL && NumDelimiters > 0 && Delimiters != NULL) {
            UTF32  *String2Split32      = UTF16_Decode(String2Split);
            UTF32 **Delimiters32        = calloc(NumDelimiters, sizeof(UTF32*));
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                Delimiters32[Delimiter] = UTF16_Decode(Delimiters[Delimiter]);
            }
            UTF32_SplitString(String2Split32, NumDelimiters, Delimiters32);
            free(String2Split32);
            for (uint64_t Delimiter = 0; Delimiter < NumDelimiters; Delimiter++) {
                free(Delimiters32[Delimiter]);
            }
            free(Delimiters32);
        } else if (String2Split == NULL) {
            Log(Log_ERROR, __func__, U8("String2Split Pointer is NULL"));
        } else if (NumDelimiters == 0) {
            Log(Log_ERROR, __func__, U8("There are no delimiters"));
        } else if (Delimiters == NULL) {
            Log(Log_ERROR, __func__, U8("Delimiters Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF8 *UTF8_CaseFoldString(UTF8 *String2CaseFold) {
        if (String2CaseFold != NULL) {
            // IDEK what I need to do, look up codepoints in a table and replace them with the found variants?
        } else {
            Log(Log_ERROR, __func__, U8("String2CaseFold Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF16 *UTF16_CaseFoldString(UTF16 *String2CaseFold) {
        if (String2CaseFold != NULL) {
            // IDEK what I need to do, look up codepoints in a table and replace them with the found variants?
        } else {
            Log(Log_ERROR, __func__, U8("String2CaseFold Pointer is NULL"));
        }
        return NULL;
    }
    
    void UTF8_DeinitStringArray(UTF8 **Strings, uint64_t NumStrings) {
        if (Strings != NULL && NumStrings > 0) {
            for (uint64_t String = 0; String < NumStrings - 1; String++) {
                free(Strings[String]);
            }
        } else if (Strings == NULL) {
            Log(Log_ERROR, __func__, U8("Strings Pointer is NULL"));
        } else if (NumStrings == 0) {
            Log(Log_ERROR, __func__, U8("There are no strings to deinit"));
        }
    }
    
    void UTF16_DeinitStringArray(UTF16 **Strings, uint64_t NumStrings) {
        if (Strings != NULL && NumStrings > 0) {
            for (uint64_t String = 0; String < NumStrings - 1; String++) {
                free(Strings[String]);
            }
        } else if (Strings == NULL) {
            Log(Log_ERROR, __func__, U8("Strings Pointer is NULL"));
        } else if (NumStrings == 0) {
            Log(Log_ERROR, __func__, U8("There are no strings to deinit"));
        }
    }
    /* Public Functions */
    
#ifdef  __cplusplus
}
#endif
