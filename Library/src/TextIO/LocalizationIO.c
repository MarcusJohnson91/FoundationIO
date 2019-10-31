#include "../../include/Macros.h"                           /* Included for FoundationIOSTDVersion */
#include "../../include/Private/UnicodeTables.h"            /* Included for Currency tables */
#include "../../include/UnicodeIO/LocalizationIO.h"         /* Included for our declarations */
#include "../../include/UnicodeIO/LogIO.h"                  /* Included for error logging */
#include "../../include/UnicodeIO/StringIO.h"               /* Included for UTF8_GetStringSizeInCideUnits */

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
        uint8_t EndOffset      = UTF8_FindSubString(LocaleAll, UTF8String("_"), 0, 1);
        UTF8 *LanguageString   = UTF8_ExtractSubString(LocaleAll, 0, EndOffset);
        uint64_t StringSize    = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize == 2) { // ISO-639-1
            if (UTF8_Compare(LanguageString, UTF8String("en"))) {
                LanguageID     = LanguageID_English;
            } else if (UTF8_Compare(LanguageString, UTF8String("de"))) {
                LanguageID     = LanguageID_German;
            } else if (UTF8_Compare(LanguageString, UTF8String("sv"))) {
                LanguageID     = LanguageID_Swedish;
            } else if (UTF8_Compare(LanguageString, UTF8String("da"))) {
                LanguageID     = LanguageID_Danish;
            } else if (UTF8_Compare(LanguageString, UTF8String("is"))) {
                LanguageID     = LanguageID_Icelandic;
            }
        } else if (StringSize == 3) { // ISO-639-2
            
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Invalid Language string length %llu"), StringSize);
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
        UTF8 *LocaleString              = getenv(UTF8String("LANG"));
        if (LocaleString != NULL) {
            uint64_t StringSize         = UTF8_GetStringSizeInCodeUnits(LocaleString);
            uint64_t Offset             = UTF8_FindSubString(LocaleString, UTF8String("."), 0, 1);
            UTF8    *EncodingString     = UTF8_ExtractSubString(LocaleString, Offset, StringSize - Offset);
            uint64_t EncodingStringSize = UTF8_GetStringSizeInCodeUnits(EncodingString);
            if (EncodingStringSize == 4) {
                if (UTF8_Compare(EncodingString, UTF8String("utf8")) || UTF8_Compare(EncodingString, UTF8String("UTF8"))) {
                    Encoding            = EncodingID_UTF8;
                }
            } else if (EncodingStringSize == 5) {
                if (UTF8_Compare(EncodingString, UTF8String("utf-8")) || UTF8_Compare(EncodingString, UTF8String("UTF-8")) || UTF8_Compare(EncodingString, UTF8String("utf_8")) || UTF8_Compare(EncodingString, UTF8String("UTF_8"))) {
                    Encoding            = EncodingID_UTF8;
                } else if (UTF8_Compare(EncodingString, UTF8String("utf16")) || UTF8_Compare(EncodingString, UTF8String("UTF16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF8_Compare(EncodingString, UTF8String("utf32")) || UTF8_Compare(EncodingString, UTF8String("UTF32"))) {
                    Encoding            = EncodingID_UTF32;
                }
            } else if (EncodingStringSize == 6) {
                if (UTF8_Compare(EncodingString, UTF8String("utf-16")) || UTF8_Compare(EncodingString, UTF8String("UTF-16")) || UTF8_Compare(EncodingString, UTF8String("utf_16")) || UTF8_Compare(EncodingString, UTF8String("UTF_16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF8_Compare(EncodingString, UTF8String("utf-32")) || UTF8_Compare(EncodingString, UTF8String("UTF-32")) || UTF8_Compare(EncodingString, UTF8String("utf_32")) || UTF8_Compare(EncodingString, UTF8String("UTF_32"))) {
                    Encoding            = EncodingID_UTF32;
                }
            }
        }
#elif (FoundationIOTargetOS == FoundtionIOWindowsOS)
        lconv_Init();
        UTF16 *LocaleString             = getenv(UTF16String("LANG"));
        if (LocaleString != NULL) {
            uint64_t StringSize         = UTF16_GetStringSizeInCodeUnits(LocaleString);
            uint64_t Offset             = UTF16_FindSubString(LocaleString, UTF16String("."), 0, 1);
            UTF8    *EncodingString     = UTF16_ExtractSubString(LocaleString, Offset, StringSize - Offset);
            uint64_t EncodingStringSize = UTF16_GetStringSizeInCodeUnits(EncodingString);
            if (EncodingStringSize == 4) {
                if (UTF16_Compare(EncodingString, UTF16String("utf8")) || UTF16_Compare(EncodingString, UTF16String("UTF8"))) {
                    Encoding            = EncodingID_UTF8;
                }
            } else if (EncodingStringSize == 5) {
                if (UTF16_Compare(EncodingString, UTF16String("utf-8")) || UTF16_Compare(EncodingString, UTF16String("UTF-8")) || UTF16_Compare(EncodingString, UTF16String("utf_8")) || UTF16_Compare(EncodingString, UTF16String("UTF_8"))) {
                    Encoding            = EncodingID_UTF8;
                } else if (UTF16_Compare(EncodingString, UTF16String("utf16")) || UTF16_Compare(EncodingString, UTF16String("UTF16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF16_Compare(EncodingString, UTF16String("utf32")) || UTF16_Compare(EncodingString, UTF16String("UTF32"))) {
                    Encoding            = EncodingID_UTF32;
                }
            } else if (EncodingStringSize == 6) {
                if (UTF16_Compare(EncodingString, UTF16String("utf-16")) || UTF16_Compare(EncodingString, UTF16String("UTF-16")) || UTF16_Compare(EncodingString, UTF16String("utf_16")) || UTF16_Compare(EncodingString, UTF16String("UTF_16"))) {
                    Encoding            = EncodingID_UTF16;
                } else if (UTF16_Compare(EncodingString, UTF16String("utf-32")) || UTF16_Compare(EncodingString, UTF16String("UTF-32")) || UTF16_Compare(EncodingString, UTF16String("utf_32")) || UTF16_Compare(EncodingString, UTF16String("UTF_32"))) {
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
        UTF8 *Delimiters[]              = {UTF8String("/"), UTF8String("\\")};
        GroupingSize                    = UTF8_Split(GroupingSizeString, Delimiters);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16 *GroupingSizeString       = Locale->grouping;
        UTF16 *Delimiters[2]            = {UTF16String("/"), UTF16String("\\")};
        
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
        UTF8 *Delimiters[]              = {UTF8String("/"), UTF8String("\\")};
        
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
    
    UTF8 *UTF8_DelocalizeInteger(UTF8 *String) {
        UTF8 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF8_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeInteger(String32);
            free(String32);
            Delocalized          = UTF8_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF16 *UTF16_DelocalizeInteger(UTF16 *String) {
        UTF16 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF16_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeInteger(String32);
            free(String32);
            Delocalized          = UTF16_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF32 *UTF32_DelocalizeInteger(UTF32 *String) {
        UTF32 *Delocalized       = NULL;
        if (String != NULL) {
            uint64_t OGCodePoint = 0ULL;
            uint64_t DeCodePoint = 0ULL;
            uint64_t NumDigits   = UTF32_GetNumDigits(Base_Integer_Radix10, String, 0);
            Delocalized          = UTF32_Init(NumDigits);
            if (Delocalized != NULL) {
                do {
                    if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                        OGCodePoint += 1;
                        Delocalized[DeCodePoint] = String[OGCodePoint];
                    }
                } while (String[OGCodePoint] != FoundationIONULLTerminator && DeCodePoint < NumDigits);
            } else {
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate delocaized string"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF8 *UTF8_DelocalizeDecimal(UTF8 *String) {
        UTF8 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF8_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeDecimal(String32);
            free(String32);
            Delocalized          = UTF8_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF16 *UTF16_DelocalizeDecimal(UTF16 *String) {
        UTF16 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF16_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeDecimal(String32);
            free(String32);
            Delocalized          = UTF16_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF32 *UTF32_DelocalizeDecimal(UTF32 *String) {
        UTF32 *Delocalized       = NULL;
        if (String != NULL) {
            uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(String);
            uint64_t CodePoint        = StringSize;
            UTF32    DecimalSeperator = 0ULL;
            do {
                if (
                    String[CodePoint] != UTF32Character('0') ||
                    String[CodePoint] != UTF32Character('1') ||
                    String[CodePoint] != UTF32Character('2') ||
                    String[CodePoint] != UTF32Character('3') ||
                    String[CodePoint] != UTF32Character('4') ||
                    String[CodePoint] != UTF32Character('5') ||
                    String[CodePoint] != UTF32Character('6') ||
                    String[CodePoint] != UTF32Character('7') ||
                    String[CodePoint] != UTF32Character('8') ||
                    String[CodePoint] != UTF32Character('9') ||
                    String[CodePoint] != UTF32Character('e') ||
                    String[CodePoint] != UTF32Character('E') ||
                    String[CodePoint] != UTF32Character('+') ||
                    String[CodePoint] != UTF32Character('-')
                    ) {
                    DecimalSeperator   = String[CodePoint];
                    break;
                }
                CodePoint             -= 1;
            } while (CodePoint > 0);
            
            uint64_t NumDigits         = UTF32_GetNumDigits(Base_Decimal_Hex_Uppercase, String, 0);
            Delocalized                = UTF32_Init(NumDigits);
            
            CodePoint                  = 0ULL;
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (
                    String[CodePoint] == UTF32Character('0') ||
                    String[CodePoint] == UTF32Character('1') ||
                    String[CodePoint] == UTF32Character('2') ||
                    String[CodePoint] == UTF32Character('3') ||
                    String[CodePoint] == UTF32Character('4') ||
                    String[CodePoint] == UTF32Character('5') ||
                    String[CodePoint] == UTF32Character('6') ||
                    String[CodePoint] == UTF32Character('7') ||
                    String[CodePoint] == UTF32Character('8') ||
                    String[CodePoint] == UTF32Character('9') ||
                    String[CodePoint] == UTF32Character('e') ||
                    String[CodePoint] == UTF32Character('E') ||
                    String[CodePoint] == UTF32Character('+') ||
                    String[CodePoint] == UTF32Character('-') ||
                    String[CodePoint] == DecimalSeperator
                    ) {
                    Delocalized[CodePoint] = String[CodePoint];
                }
                CodePoint             += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        
        return Delocalized;
    }
    
#ifdef __cplusplus
}
#endif
