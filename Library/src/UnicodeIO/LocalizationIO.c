#include "../include/Macros.h"         /* Included for FoundationIOSTDVersion */
#include "../include/LocalizationIO.h" /* Included for our Declarations */

#include "../include/Log.h"            /* Included for error logging */
#include "../include/UnicodeTables.h"  /* Included for Currency tables */
#include "../include/StringIO.h"       /* Included for UTF8_GetStringSizeInCideUnits */
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    static void lconv_Init(void) {
        setlocale(LC_ALL, NULL);
        setlocale(LC_COLLATE, NULL);
        setlocale(LC_CTYPE, NULL);
        setlocale(LC_MONETARY, NULL);
        setlocale(LC_NUMERIC, NULL);
        setlocale(LC_TIME, NULL);
    }
    
    LocalizationIO_LanguageIDs Localize_GetLanguageID(void) {
        LocalizationIO_LanguageIDs LanguageID = LanguageID_Unknown;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8 *LocaleAll        = setlocale(LC_ALL, NULL);
        uint8_t EndOffset      = UTF8_FindSubString(LocaleAll, U8("_"), 0, 1);
        // Now copy the string from 0 to EndOffset
        UTF8 *LanguageString   = UTF8_ExtractSubString(LocaleAll, 0, EndOffset);
        // Now we get the size of the language string
        uint64_t StringSize    = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize == 2) { // ISO-639-1
            
            
            
            
            
            
            
            if (UTF8_Compare(LanguageString, U8("en"))) {
                LanguageID     = LanguageID_English;
            } else if (UTF8_Compare(LanguageString, U8("de"))) {
                LanguageID     = LanguageID_German;
            } else if (UTF8_Compare(LanguageString, U8("sv"))) {
                LanguageID     = LanguageID_Swedish;
            } else if (UTF8_Compare(LanguageString, U8("da"))) {
                LanguageID     = LanguageID_Danish;
            } else if (UTF8_Compare(LanguageString, U8("is"))) {
                LanguageID     = LanguageID_Icelandic;
            }
        } else if (StringSize == 3) { // ISO-639-2
            
        } else {
            Log(Log_DEBUG, __func__, U8("Invalid Language string length %llu"), StringSize);
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#endif
        return LanguageID;
    }
    
    LocalizationIO_RegionIDs Localize_GetRegionID(void) {
        LocalizationIO_RegionIDs RegionID = RegionID_Unknown;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8 *LocaleAll        = setlocale(LC_ALL, NULL);
        
        
        
        
        
        uint64_t StringSize    = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize == 2) { // ISO-639-1
            
        } else if (StringSize == 3) { // ISO-639-2
            
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#endif
        return RegionID;
    }
    
    LocalizationIO_EncodingIDs Localize_GetEncodingID(void) {
        LocalizationIO_EncodingIDs Encoding = EncodingID_Unknown;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        lconv_Init();
        UTF8 *LocaleString              = getenv(U8("LANG"));
        if (LocaleString != NULL) {
            uint64_t StringSize         = UTF8_GetStringSizeInCodeUnits(LocaleString);
            uint64_t Offset             = UTF8_FindSubString(LocaleString, U8("."), 0, 1);
            UTF8    *EncodingString     = UTF8_ExtractSubString(LocaleString, Offset, StringSize - Offset);
            uint64_t EncodingStringSize = UTF8_GetStringSizeInCodeUnits(EncodingString);
            if (EncodingStringSize == 4) {
                if (UTF8_Compare(EncodingString, U8("utf8")) || UTF8_Compare(EncodingString, U8("UTF8"))) {
                    Encoding            = EncodingID_UTF8;
                }
            } else if (EncodingStringSize == 5) {
                if (UTF8_Compare(EncodingString, U8("utf-8")) || UTF8_Compare(EncodingString, U8("UTF-8")) || UTF8_Compare(EncodingString, U8("utf_8")) || UTF8_Compare(EncodingString, U8("UTF_8"))) {
                    Encoding            = EncodingID_UTF8;
                } else if (UTF8_Compare(EncodingString, U8("utf16")) || UTF8_Compare(EncodingString, U8("UTF16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF8_Compare(EncodingString, U8("utf32")) || UTF8_Compare(EncodingString, U8("UTF32"))) {
                    Encoding            = EncodingID_UTF32;
                }
            } else if (EncodingStringSize == 6) {
                if (UTF8_Compare(EncodingString, U8("utf-16")) || UTF8_Compare(EncodingString, U8("UTF-16")) || UTF8_Compare(EncodingString, U8("utf_16")) || UTF8_Compare(EncodingString, U8("UTF_16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF8_Compare(EncodingString, U8("utf-32")) || UTF8_Compare(EncodingString, U8("UTF-32")) || UTF8_Compare(EncodingString, U8("utf_32")) || UTF8_Compare(EncodingString, U8("UTF_32"))) {
                    Encoding            = EncodingID_UTF32;
                }
            }
        }
#elif (FoundationIOTargetOS == FoundtionIOWindowsOS)
        lconv_Init();
        UTF16 *LocaleString             = getenv(U16("LANG"));
        if (LocaleString != NULL) {
            uint64_t StringSize         = UTF16_GetStringSizeInCodeUnits(LocaleString);
            uint64_t Offset             = UTF16_FindSubString(LocaleString, U16("."), 0, 1);
            UTF8    *EncodingString     = UTF16_ExtractSubString(LocaleString, Offset, StringSize - Offset);
            uint64_t EncodingStringSize = UTF16_GetStringSizeInCodeUnits(EncodingString);
            if (EncodingStringSize == 4) {
                if (UTF16_Compare(EncodingString, U16("utf8")) || UTF16_Compare(EncodingString, U16("UTF8"))) {
                    Encoding            = EncodingID_UTF8;
                }
            } else if (EncodingStringSize == 5) {
                if (UTF16_Compare(EncodingString, U16("utf-8")) || UTF16_Compare(EncodingString, U16("UTF-8")) || UTF16_Compare(EncodingString, U16("utf_8")) || UTF16_Compare(EncodingString, U16("UTF_8"))) {
                    Encoding            = EncodingID_UTF8;
                } else if (UTF16_Compare(EncodingString, U16("utf16")) || UTF16_Compare(EncodingString, U16("UTF16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF16_Compare(EncodingString, U16("utf32")) || UTF16_Compare(EncodingString, U16("UTF32"))) {
                    Encoding            = EncodingID_UTF32;
                }
            } else if (EncodingStringSize == 6) {
                if (UTF16_Compare(EncodingString, U16("utf-16")) || UTF16_Compare(EncodingString, U16("UTF-16")) || UTF16_Compare(EncodingString, U16("utf_16")) || UTF16_Compare(EncodingString, U16("UTF_16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF16_Compare(EncodingString, U16("utf-32")) || UTF16_Compare(EncodingString, U16("UTF-32")) || UTF16_Compare(EncodingString, U16("utf_32")) || UTF16_Compare(EncodingString, U16("UTF_32"))) {
                    Encoding            = EncodingID_UTF32;
                }
            }
        }
#endif
        return Encoding;
    }
    
    UTF8 *Localize_UTF8_GetDecimalSeperator(void) {
        UTF8 *DecimalSeperator         = NULL;
        lconv_Init();
        struct lconv *Locale           = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        DecimalSeperator               = UTF8_Clone(Locale->decimal_point);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16 *DecimalSeperator16      = UTF16_Clone(Locale->_W_decimal_point);
        if (DecimalSeperator16 != NULL) {
            UTF32 *DecimalSeperator32  = UTF16_Decode(DecimalSeperator16);
            DecimalSeperator           = UTF8_Encode(DecimalSeperator32);
            free(DecimalSeperator32);
        }
        free(DecimalSeperator16);
#endif
        return DecimalSeperator;
    }
    
    UTF16 *Localize_UTF16_GetDecimalSeperator(void) {
        UTF16 *DecimalSeperator        = NULL;
        lconv_Init();
        struct lconv *Locale           = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8  *DecimalSeperator8       = UTF8_Clone(Locale->decimal_point);
        if (DecimalSeperator8 != NULL) {
            UTF32 *DecimalSeperator32  = UTF8_Decode(DecimalSeperator8);
            DecimalSeperator           = UTF16_Encode(DecimalSeperator32);
            free(DecimalSeperator32);
        }
        free(DecimalSeperator8);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        DecimalSeperator               = UTF16_Clone(Locale->_W_decimal_point);
#endif
        return DecimalSeperator;
    }
    
    UTF8 *Localize_UTF8_GetGroupingSeperator(void) {
        UTF8 *GroupingSeperator         = NULL;
        lconv_Init();
        struct lconv *Locale            = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        GroupingSeperator               = UTF8_Clone(Locale->thousands_sep);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16 *GroupingSeperator16      = UTF16_Clone(Locale->_W_thousands_sep);
        if (GroupingSeperator16 != NULL) {
            UTF32 *GroupingSeperator32  = UTF16_Decode(GroupingSeperator16);
            GroupingSeperator           = UTF8_Encode(GroupingSeperator32);
            free(GroupingSeperator32);
        }
        free(GroupingSeperator16);
#endif
        return GroupingSeperator;
    }
    
    UTF16 *Localize_UTF16_GetGroupingSeperator(void) {
        UTF16 *GroupingSeperator        = NULL;
        lconv_Init();
        struct lconv *Locale            = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8  *GroupingSeperator8       = UTF8_Clone(Locale->thousands_sep);
        if (GroupingSeperator8 != NULL) {
            UTF32 *GroupingSeperator32  = UTF8_Decode(GroupingSeperator8);
            GroupingSeperator           = UTF16_Encode(GroupingSeperator32);
            free(GroupingSeperator32);
        }
        free(GroupingSeperator8);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        GroupingSeperator              = UTF16_Clone(Locale->_W_thousands_sep);
#endif
        return GroupingSeperator;
    }
    
    UTF8 **Localize_UTF8_GetGroupingSize(void) {
        UTF8    **GroupingSize          = NULL;
        lconv_Init();
        struct lconv *Locale            = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8 *GroupingSizeString        = Locale->grouping;
        UTF8 *Delimiters[]              = {U8("/"), U8("\\")};
        GroupingSize                    = UTF8_Split(GroupingSizeString, Delimiters);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16 *GroupingSizeString       = Locale->grouping;
        UTF16 *Delimiters[]             = {U16("/"), U16("\\")};
        
        UTF16 **GroupingSize16          = UTF16_Split(GroupingSizeString, Delimiters);
        UTF32 **GroupingSize32          = UTF16_StringArray_Decode(GroupingSize16);
        UTF16_StringArray_Deinit(GroupingSize16);
        GroupingSize                    = UTF8_StringArray_Encode(GroupingSize32);
        UTF32_StringArray_Deinit(GroupingSize32);
#endif
        return GroupingSize;
    }
    
    UTF16 **Localize_UTF16_GetGroupingSize(void) {
        UTF16 **GroupingSize            = NULL;
        lconv_Init();
        struct lconv *Locale            = localeconv();
        UTF8 *GroupingSizeString        = Locale->grouping;
        UTF8 *Delimiters[]              = {U8("/"), U8("\\")};
        
        UTF8 **GroupingSize8            = UTF8_Split(GroupingSizeString, Delimiters);
        UTF32 **GroupingSize32          = UTF8_StringArray_Decode(GroupingSize8);
        UTF8_StringArray_Deinit(GroupingSize8);
        GroupingSize                    = UTF16_StringArray_Encode(GroupingSize32);
        UTF32_StringArray_Deinit(GroupingSize32);
        return GroupingSize;
    }
    
    UTF8 *Localize_UTF8_GetCurrencySymbol(void) {
        UTF8 *CurrencySymbol            = NULL;
        lconv_Init();
        struct lconv *Locale            = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        CurrencySymbol                  = UTF8_Clone(Locale->currency_symbol);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF8  *CurrencySymbol8          = UTF8_Clone(Locale->currency_symbol);
        if (CurrencySymbol8 != NULL) {
            UTF32 *CurrencySymbol32     = UTF8_Decode(CurrencySymbol8);
            CurrencySymbol              = UTF16_Encode(CurrencySymbol32);
            free(CurrencySymbol32);
        }
        free(CurrencySymbol8);
#endif
        return CurrencySymbol;
    }
    
    UTF16 *Localize_UTF16_GetCurrencySymbol(void) {
        UTF16 *CurrencySymbol           = NULL;
        lconv_Init();
        struct lconv *Locale            = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8  *CurrencySymbol8          = UTF8_Clone(Locale->currency_symbol);
        if (CurrencySymbol8 != NULL) {
            UTF32 *CurrencySymbol32     = UTF8_Decode(CurrencySymbol8);
            CurrencySymbol              = UTF16_Encode(CurrencySymbol32);
            free(CurrencySymbol32);
        }
        free(CurrencySymbol8);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        CurrencySymbol                  = UTF8_Clone(Locale->currency_symbol);
#endif
        return CurrencySymbol;
    }
    
    UTF8 *Delocalize_UTF8_Currency(UTF8 *String) {
        UTF8 *Stripped        = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Stripped32 = Delocalize_UTF32_Currency(String32);
            free(String32);
            Stripped          = UTF8_Encode(Stripped32);
            free(Stripped32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Stripped;
    }
    
    UTF16 *Delocalize_UTF16_Currency(UTF16 *String) {
        UTF16 *Stripped       = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Stripped32 = Delocalize_UTF32_Currency(String32);
            free(String32);
            Stripped          = UTF16_Encode(Stripped32);
            free(Stripped32);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Stripped;
    }
    
    UTF32 *Delocalize_UTF32_Currency(UTF32 *String) {
        UTF32 *Stripped                    = NULL;
        if (String != NULL) {
            uint64_t StringSize            = UTF32_GetStringSizeInCodePoints(String);
            uint64_t CodePoints2Remove     = 0ULL;
            
            for (uint64_t CodePoint = 0ULL; CodePoint < StringSize; CodePoint++) {
                for (uint8_t CurrencySymbol = 0; CurrencySymbol < CurrencyTableSize; CurrencySymbol++) {
                    if (String[CodePoint] == CurrencyTable[CurrencySymbol]) {
                        CodePoints2Remove += 1;
                    }
                }
            }
            
            uint64_t StrippedStringSize    = StringSize - CodePoints2Remove;
            
            Stripped                       = calloc(StrippedStringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (Stripped != NULL) {
                for (uint64_t Original = 0ULL; Original < StringSize; Original++) {
                    for (uint64_t StrippedCodePoint = 0ULL; StrippedCodePoint < StrippedStringSize; StrippedCodePoint++) {
                        for (uint8_t CurrencySymbol = 0ULL; CurrencySymbol < CurrencyTableSize; CurrencySymbol++) {
                            if (String[Original] != CurrencyTable[CurrencySymbol]) {
                                 Stripped[StrippedCodePoint] = String[Original];
                            }
                        }
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate stripped string"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Stripped;
    }
    
#ifdef __cplusplus
}
#endif
