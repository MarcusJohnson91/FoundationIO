#include "../../include/TextIO/LocalizationIO.h"       /* Included for our declarations */
#include "../../include/TextIO/LogIO.h"                /* Included for error logging */
#include "../../include/TextIO/Private/TextIOTables.h" /* Included for Currency tables */
#include "../../include/TextIO/StringIO.h"             /* Included for UTF8_GetStringSizeInCideUnits */
#include <locale.h>

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum LocalizationIO_DateFormats {
                   DateFormat_Unspecified = 0,
                   DateFormat_Day1        = 1,
                   DateFormat_Day2        = 2,
                   DateFormat_Day3        = 4,
                   DateFormat_Month1      = 8,
                   DateFormat_Month2      = 16,
                   DateFormat_Month3      = 32,
                   DateFormat_Year1       = 64,
                   DateFormat_Year2       = 128,
                   DateFormat_Year3       = 256,
    } LocalizationIO_DateFormats;
    
    typedef enum LocalizationIO_TimeFormats {
                   TimeFormat_Unspecified = 0,
                   TimeFormat_Hour1       = 1,
                   TimeFormat_Hour2       = 2,
                   TimeFormat_Hour3       = 4,
                   TimeFormat_Minute1     = 8,
                   TimeFormat_Minute2     = 16,
                   TimeFormat_Minute3     = 32,
                   TimeFormat_Second1     = 64,
                   TimeFormat_Second2     = 128,
                   TimeFormat_Second3     = 256,
    } LocalizationIO_TimeFormats;
    
    static void LocalizationIO_Init(void) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        setlocale(LC_ALL, NULL);
        setlocale(LC_COLLATE, NULL);
        setlocale(LC_CTYPE, NULL);
        setlocale(LC_MONETARY, NULL);
        setlocale(LC_NUMERIC, NULL);
        setlocale(LC_TIME, NULL);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#define __initialize_lconv_for_unsigned_char
#endif
    }
    
    LocalizationIO_WrittenLanguages Localize_GetWrittenLanguageID(void) {
        LocalizationIO_WrittenLanguages LanguageID = WrittenLanguage_Unspecified;
        LocalizationIO_Init();
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        /* POSIX uses ISO 639-1 if possible, otherwise ISO 639-2 */
#if (PlatformIO_Language == PlatformIO_LanguageIsC)
        UTF8 *LocaleAll = (UTF8 *) setlocale(LC_ALL, NULL);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        UTF8 *LocaleAll    = reinterpret_cast<UTF8 *>(setlocale(LC_ALL, NULL));
#endif
        uint64_t EndOffset      = UTF8_FindSubString(LocaleAll, UTF8String("_"), 0, 1);
        UTF8 *   LanguageString = UTF8_ExtractSubString(LocaleAll, 0, EndOffset);
        uint64_t StringSize     = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize == 2) {
            if (UTF8_Compare(LanguageString, UTF8String("en"))) {
                LanguageID = WrittenLanguage_English;
            } else if (UTF8_Compare(LanguageString, UTF8String("de"))) {
                LanguageID = WrittenLanguage_German;
            } else if (UTF8_Compare(LanguageString, UTF8String("sv"))) {
                LanguageID = WrittenLanguage_Swedish;
            } else if (UTF8_Compare(LanguageString, UTF8String("da"))) {
                LanguageID = WrittenLanguage_Danish;
            } else if (UTF8_Compare(LanguageString, UTF8String("is"))) {
                LanguageID = WrittenLanguage_Icelandic;
            }
        } else if (StringSize == 3) {
            
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid Language string length %llu"), StringSize);
        }
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#endif
        return LanguageID;
    }
    
    LocalizationIO_RegionIDs Localize_GetRegionID(void) {
        LocalizationIO_RegionIDs RegionID = RegionID_Unspecified;
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#if (PlatformIO_Language == PlatformIO_LanguageIsC)
        UTF8 *LocaleAll = (UTF8 *) setlocale(LC_ALL, NULL);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        UTF8 *LocaleAll    = reinterpret_cast<UTF8 *>(setlocale(LC_ALL, NULL));
#endif
        
        uint64_t StringSize = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize == 2) {
            
        } else if (StringSize == 3) {
        }
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#endif
        return RegionID;
    }
    
    TextIO_StringTypes Localize_GetEncodingID(void) {
        TextIO_StringTypes Encoding = StringType_Unspecified;
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        LocalizationIO_Init();
#if (PlatformIO_Language == PlatformIO_LanguageIsC)
        UTF8 *LocaleString = (UTF8 *) getenv((const char *) "LANG");
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        UTF8 *LocaleString = reinterpret_cast<UTF8 *>(getenv(reinterpret_cast<const char *>("LANG")));
#endif
        if (LocaleString != NULL) {
            uint64_t StringSize         = UTF8_GetStringSizeInCodeUnits(LocaleString);
            uint64_t Offset             = UTF8_FindSubString(LocaleString, UTF8String("."), 0, 1);
            UTF8    *EncodingString     = UTF8_ExtractSubString(LocaleString, Offset, StringSize - Offset);
            uint64_t EncodingStringSize = UTF8_GetStringSizeInCodeUnits(EncodingString);
            if (EncodingStringSize == 4) {
                if (UTF8_Compare(EncodingString, UTF8String("utf8")) || UTF8_Compare(EncodingString, UTF8String("UTF8"))) {
                    Encoding = StringType_UTF8;
                }
            } else if (EncodingStringSize == 5) {
                if (UTF8_Compare(EncodingString, UTF8String("utf-8")) || UTF8_Compare(EncodingString, UTF8String("UTF-8")) || UTF8_Compare(EncodingString, UTF8String("utf_8")) || UTF8_Compare(EncodingString, UTF8String("UTF_8"))) {
                    Encoding = StringType_UTF8;
                } else if (UTF8_Compare(EncodingString, UTF8String("utf16")) || UTF8_Compare(EncodingString, UTF8String("UTF16"))) {
                    Encoding = StringType_UTF16;
                } else if (UTF8_Compare(EncodingString, UTF8String("utf32")) || UTF8_Compare(EncodingString, UTF8String("UTF32"))) {
                    Encoding = StringType_UTF32;
                }
            } else if (EncodingStringSize == 6) {
                if (UTF8_Compare(EncodingString, UTF8String("utf-16")) || UTF8_Compare(EncodingString, UTF8String("UTF-16")) || UTF8_Compare(EncodingString, UTF8String("utf_16")) || UTF8_Compare(EncodingString, UTF8String("UTF_16"))) {
                    Encoding = StringType_UTF16;
                } else if (UTF8_Compare(EncodingString, UTF8String("utf-32")) || UTF8_Compare(EncodingString, UTF8String("UTF-32")) || UTF8_Compare(EncodingString, UTF8String("utf_32")) || UTF8_Compare(EncodingString, UTF8String("UTF_32"))) {
                    Encoding = StringType_UTF32;
                }
            }
        }
#elif (PlatformIO_TargetOS == FoundtionIOWindowsOS)
        LocalizationIO_Init();
#if (PlatformIO_Language == PlatformIO_LanguageIsC)
        UTF16 *LocaleString = (UTF16 *) _wgetenv(L"LANG");
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        UTF16 *LocaleString = reinterpret_cast<UTF16 *>(_wgetenv(L"LANG"));
#endif
        
        UTF16 *LocaleString = getenv((ImmutableString_UTF8) "LANG");
        if (LocaleString != NULL) {
            uint64_t StringSize         = UTF16_GetStringSizeInCodeUnits(LocaleString);
            uint64_t Offset             = UTF16_FindSubString(LocaleString, UTF16String("."), 0, 1);
            UTF8 *   EncodingString     = UTF16_ExtractSubString(LocaleString, Offset, StringSize - Offset);
            uint64_t EncodingStringSize = UTF16_GetStringSizeInCodeUnits(EncodingString);
            if (EncodingStringSize == 4) {
                if (UTF16_Compare(EncodingString, UTF16String("utf8")) || UTF16_Compare(EncodingString, UTF16String("UTF8"))) {
                    Encoding = StringType_UTF8;
                }
            } else if (EncodingStringSize == 5) {
                if (UTF16_Compare(EncodingString, UTF16String("utf-8")) || UTF16_Compare(EncodingString, UTF16String("UTF-8")) || UTF16_Compare(EncodingString, UTF16String("utf_8")) || UTF16_Compare(EncodingString, UTF16String("UTF_8"))) {
                    Encoding = StringType_UTF8;
                } else if (UTF16_Compare(EncodingString, UTF16String("utf16")) || UTF16_Compare(EncodingString, UTF16String("UTF16"))) {
                    Encoding = StringType_UTF16;
                } else if (UTF16_Compare(EncodingString, UTF16String("utf32")) || UTF16_Compare(EncodingString, UTF16String("UTF32"))) {
                    Encoding = StringType_UTF32;
                }
            } else if (EncodingStringSize == 6) {
                if (UTF16_Compare(EncodingString, UTF16String("utf-16")) || UTF16_Compare(EncodingString, UTF16String("UTF-16")) || UTF16_Compare(EncodingString, UTF16String("utf_16")) || UTF16_Compare(EncodingString, UTF16String("UTF_16"))) {
                    Encoding = StringType_UTF16;
                } else if (UTF16_Compare(EncodingString, UTF16String("utf-32")) || UTF16_Compare(EncodingString, UTF16String("UTF-32")) || UTF16_Compare(EncodingString, UTF16String("utf_32")) || UTF16_Compare(EncodingString, UTF16String("UTF_32"))) {
                    Encoding = StringType_UTF32;
                }
            }
        }
#endif
        return Encoding;
    }
    
    UTF8 *Localize_UTF8_GetDecimalSeperator(void) {
        UTF8 *DecimalSeperator = NULL;
        LocalizationIO_Init();
        struct lconv *Locale = localeconv();
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        DecimalSeperator = UTF8_Clone((ImmutableString_UTF8) Locale->decimal_point);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        UTF16 *DecimalSeperator16 = UTF16_Clone((ImmutableString_UTF16) Locale->_W_decimal_point);
        if (DecimalSeperator16 != NULL) {
            UTF32 *DecimalSeperator32 = UTF16_Decode(DecimalSeperator16);
            DecimalSeperator          = UTF8_Encode(DecimalSeperator32);
            free(DecimalSeperator32);
        }
        free(DecimalSeperator16);
#endif
        return DecimalSeperator;
    }
    
    UTF16 *Localize_UTF16_GetDecimalSeperator(void) {
        UTF16 *DecimalSeperator = NULL;
        LocalizationIO_Init();
        struct lconv *Locale = localeconv();
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        UTF8 *DecimalSeperator8 = UTF8_Clone((ImmutableString_UTF8) Locale->decimal_point);
        if (DecimalSeperator8 != NULL) {
            DecimalSeperator = UTF8_Convert(DecimalSeperator8);
        }
        free(DecimalSeperator8);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        DecimalSeperator           = UTF16_Clone((ImmutableString_UTF16) Locale->_W_decimal_point);
#endif
        return DecimalSeperator;
    }
    
    UTF8 *Localize_UTF8_GetGroupingSeperator(void) {
        UTF8 *GroupingSeperator = NULL;
        LocalizationIO_Init();
        struct lconv *Locale = localeconv();
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        GroupingSeperator = UTF8_Clone((ImmutableString_UTF8) Locale->thousands_sep);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        UTF16 *GroupingSeperator16 = UTF16_Clone((ImmutableString_UTF16) Locale->_W_thousands_sep);
        if (GroupingSeperator16 != NULL) {
            UTF32 *GroupingSeperator32 = UTF16_Decode(GroupingSeperator16);
            GroupingSeperator          = UTF8_Encode(GroupingSeperator32);
            free(GroupingSeperator32);
        }
        free(GroupingSeperator16);
#endif
        return GroupingSeperator;
    }
    
    UTF16 *Localize_UTF16_GetGroupingSeperator(void) {
        UTF16 *GroupingSeperator = NULL;
        LocalizationIO_Init();
        struct lconv *Locale = localeconv();
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        UTF8 *GroupingSeperator8 = UTF8_Clone((ImmutableString_UTF8) Locale->thousands_sep);
        if (GroupingSeperator8 != NULL) {
            GroupingSeperator = UTF8_Convert(GroupingSeperator8);
        }
        free(GroupingSeperator8);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        GroupingSeperator = UTF16_Clone((ImmutableString_UTF16) Locale->_W_thousands_sep);
#endif
        return GroupingSeperator;
    }
    
    UTF8 **Localize_UTF8_GetGroupingSize(void) {
        UTF8 **GroupingSize = NULL;
        LocalizationIO_Init();
        struct lconv *Locale                            = localeconv();
        ImmutableString_UTF8 GroupingSizeString = UTF8_Clone((ImmutableString_UTF8) Locale->grouping);
        ImmutableString_UTF8 Delimiters[2]      = PlatformIO_MakeStringSet(2, UTF8String("/"), UTF8String("\\"));
#if (PlatformIO_Language == PlatformIO_LanguageIsC)
        GroupingSize = UTF8_Split(GroupingSizeString, (ImmutableStringSet_UTF8) Delimiters);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        GroupingSize      = UTF8_Split((ImmutableString_UTF8) GroupingSizeString, (ImmutableStringSet_UTF8) Delimiters);
#endif
        return GroupingSize;
    }
    
    UTF16 **Localize_UTF16_GetGroupingSize(void) {
        UTF16 **GroupingSize = NULL;
        LocalizationIO_Init();
        struct lconv *Locale                       = localeconv();
        UTF8 *        GroupingSizeString           = UTF8_Clone((ImmutableString_UTF8) Locale->grouping);
        ImmutableString_UTF8 Delimiters[2] = PlatformIO_MakeStringSet(2, UTF8String("/"), UTF8String("\\"));
        
        UTF8 ** GroupingSize8  = UTF8_Split((ImmutableString_UTF8) GroupingSizeString, (ImmutableStringSet_UTF8) Delimiters);
        UTF32 **GroupingSize32 = UTF8_StringSet_Decode((ImmutableStringSet_UTF8) GroupingSize8);
        UTF8_StringSet_Deinit(GroupingSize8);
        GroupingSize = UTF16_StringSet_Encode((ImmutableStringSet_UTF32) GroupingSize32);
        UTF32_StringSet_Deinit(GroupingSize32);
        return GroupingSize;
    }
    
    UTF8 *Localize_UTF8_GetCurrencySymbol(void) {
        UTF8 *CurrencySymbol = NULL;
        LocalizationIO_Init();
        struct lconv *Locale = localeconv();
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        CurrencySymbol = UTF8_Clone((ImmutableString_UTF8) Locale->currency_symbol);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        CurrencySymbol    = UTF16_Convert((ImmutableString_UTF16) Locale->_W_currency_symbol);
#endif
        return CurrencySymbol;
    }
    
    UTF16 *Localize_UTF16_GetCurrencySymbol(void) {
        UTF16 *CurrencySymbol = NULL;
        LocalizationIO_Init();
        struct lconv *Locale = localeconv();
#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        CurrencySymbol = UTF8_Convert((ImmutableString_UTF8) Locale->currency_symbol);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        CurrencySymbol    = UTF16_Clone((ImmutableString_UTF16) Locale->_W_currency_symbol);
#endif
        return CurrencySymbol;
    }
    
    UTF8 *UTF8_DelocalizeInteger(TextIO_Bases Base, ImmutableString_UTF8 String) {
        UTF8 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF8_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeInteger(Base, String32);
            free(String32);
            Delocalized          = UTF8_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF16 *UTF16_DelocalizeInteger(TextIO_Bases Base, ImmutableString_UTF16 String) {
        UTF16 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF16_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeInteger(Base, String32);
            free(String32);
            Delocalized          = UTF16_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF32 *UTF32_DelocalizeInteger(TextIO_Bases Base, ImmutableString_UTF32 String) {
        UTF32 *Delocalized       = NULL;
        if (String != NULL) {
            uint64_t OGCodePoint = 0ULL;
            uint64_t DeCodePoint = 0ULL;
            uint64_t NumDigits   = UTF32_GetNumDigits(Base, String);
            Delocalized          = UTF32_Init(NumDigits);
            if (Delocalized != NULL) {
                if ((Base & Base_Integer) == Base_Integer) {
                    if ((Base & Base_Radix2) == Base_Radix2) {
                        while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                            if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('1')) {
                                Delocalized[DeCodePoint] = IntegerTableBase2[String[OGCodePoint] - 0x30];
                                DeCodePoint += 1;
                                OGCodePoint += 1;
                            } else {
                                OGCodePoint += 1;
                            }
                        }
                    } else if ((Base & Base_Radix8) == Base_Radix8) {
                        while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                            if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('7')) {
                                Delocalized[DeCodePoint] = IntegerTableBase8[String[OGCodePoint] - 0x30];
                                DeCodePoint += 1;
                                OGCodePoint += 1;
                            } else {
                                OGCodePoint += 1;
                            }
                        }
                    } else if ((Base & Base_Radix10) == Base_Radix10) {
                        while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                            if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                Delocalized[DeCodePoint] = TableBase10[String[OGCodePoint] - 0x30];
                                DeCodePoint += 1;
                                OGCodePoint += 1;
                            } else {
                                OGCodePoint += 1;
                            }
                        }
                    } else if ((Base & Base_Radix16) == Base_Radix16) {
                        if ((Base & Base_Uppercase) == Base_Uppercase) {
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = IntegerTableUppercaseBase16[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] >= UTF32Character('A') && String[OGCodePoint] <= UTF32Character('F')) {
                                    Delocalized[DeCodePoint] = IntegerTableUppercaseBase16[String[OGCodePoint] - 0x37];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        } else if ((Base & Base_Lowercase) == Base_Lowercase) {
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') || String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = IntegerTableUppercaseBase16[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] >= UTF32Character('a') && String[OGCodePoint] <= UTF32Character('f')) {
                                    Delocalized[DeCodePoint] = IntegerTableUppercaseBase16[String[OGCodePoint] - 0x57];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        }
                    }
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate delocalized string"));
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF8 *UTF8_DelocalizeDecimal(TextIO_Bases Base, ImmutableString_UTF8 String) {
        UTF8 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF8_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeDecimal(Base, String32);
            free(String32);
            Delocalized          = UTF8_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    UTF16 *UTF16_DelocalizeDecimal(TextIO_Bases Base, ImmutableString_UTF16 String) {
        UTF16 *Delocalized = NULL;
        if (String != NULL) {
            UTF32 *String32      = UTF16_Decode(String);
            UTF32 *Delocalized32 = UTF32_DelocalizeDecimal(Base, String32);
            free(String32);
            Delocalized          = UTF16_Encode(Delocalized32);
            free(Delocalized32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
    static UTF32 UTF32_DiscoverDecimalSeperator(ImmutableString_UTF32 String) {
        UTF32 DiscoveredSeperator = PlatformIO_NULLTerminator;
        if (String != PlatformIO_NULLTerminator) {
            uint64_t StringSize = UTF32_GetStringSizeInCodePoints(String);
            uint64_t CodePoint = StringSize;
            while (CodePoint > 0 && DiscoveredSeperator == PlatformIO_NULLTerminator) {
                if (String[CodePoint] == UTF32Character('.') || String[CodePoint] == UTF32Character(',') || String[CodePoint] == UTF32Character('\'')) {
                    DiscoveredSeperator = String[CodePoint];
                }
                CodePoint -= 1;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return DiscoveredSeperator;
    }
    
    UTF32 *UTF32_DelocalizeDecimal(TextIO_Bases Base, ImmutableString_UTF32 String) {
        UTF32 *Delocalized       = NULL;
        if (String != NULL) {
            uint64_t OGCodePoint = 0ULL;
            uint64_t DeCodePoint = 0ULL;
            uint64_t NumDigits   = UTF32_GetNumDigits(Base, String);
            Delocalized          = UTF32_Init(NumDigits);
            UTF32    Seperator   = UTF32_DiscoverDecimalSeperator(String);
            if (Delocalized != NULL) {
                if ((Base & Base_Decimal) == Base_Decimal) {
                    if ((Base & Base_Radix10) == Base_Radix10) {
                        while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                            if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                Delocalized[DeCodePoint] = TableBase10[String[OGCodePoint] - 0x30];
                                DeCodePoint += 1;
                                OGCodePoint += 1;
                            } else if (String[OGCodePoint] == Seperator || String[OGCodePoint] == UTF32Character('E')) {
                                Delocalized[DeCodePoint] = String[OGCodePoint];
                                DeCodePoint += 1;
                                OGCodePoint += 1;
                            } else {
                                OGCodePoint += 1;
                            }
                        }
                    } else if ((Base & Base_Uppercase) == Base_Uppercase) {
                        if ((Base & Base_Radix16) == Base_Radix16) {
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = DecimalHexUppercase[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] >= UTF32Character('A') && String[OGCodePoint] <= UTF32Character('F')) {
                                    Delocalized[DeCodePoint] = DecimalHexUppercase[String[OGCodePoint] - 0x37];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] == UTF32Character('P') || String[OGCodePoint] == UTF32Character('X')) {
                                    Delocalized[DeCodePoint] = String[OGCodePoint];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] == Seperator) {
                                    Delocalized[DeCodePoint] = String[OGCodePoint];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        } else if ((Base & Base_Shortest) == Base_Shortest) { // Base10/Scientific
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = TableBase10[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] >= UTF32Character('E')) {
                                    Delocalized[DeCodePoint] = TableBase10[String[OGCodePoint] - 0x3B];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] == Seperator) {
                                    Delocalized[DeCodePoint] = String[OGCodePoint];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        } else if ((Base & Base_Scientific) == Base_Scientific) {
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = DecimalScientificUppercase[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] >= UTF32Character('A') && String[OGCodePoint] <= UTF32Character('F')) {
                                    Delocalized[DeCodePoint] = DecimalScientificUppercase[String[OGCodePoint] - 0x37];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] == UTF32Character('P') || String[OGCodePoint] == UTF32Character('X')) {
                                    Delocalized[DeCodePoint] = String[OGCodePoint];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        }
                    } else if ((Base & Base_Lowercase) == Base_Lowercase) {
                        if ((Base & Base_Radix16) == Base_Radix16) {
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = DecimalHexLowercase[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] >= UTF32Character('a') && String[OGCodePoint] <= UTF32Character('f')) {
                                    Delocalized[DeCodePoint] = DecimalHexLowercase[String[OGCodePoint] - 0x37];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] == UTF32Character('p') || String[OGCodePoint] == UTF32Character('x')) {
                                    Delocalized[DeCodePoint] = String[OGCodePoint];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        } else if ((Base & Base_Shortest) == Base_Shortest) { // Base10/Scientific
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = TableBase10[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] == UTF32Character('e') || String[OGCodePoint] == UTF32Character('.')) {
                                    Delocalized[DeCodePoint] = String[OGCodePoint];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        } else if ((Base & Base_Scientific) == Base_Scientific) {
                            while (String[OGCodePoint] != PlatformIO_NULLTerminator && Delocalized[DeCodePoint] != PlatformIO_NULLTerminator) {
                                if (String[OGCodePoint] >= UTF32Character('0') && String[OGCodePoint] <= UTF32Character('9')) {
                                    Delocalized[DeCodePoint] = DecimalHexUppercase[String[OGCodePoint] - 0x30];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] >= UTF32Character('A') && String[OGCodePoint] <= UTF32Character('F')) {
                                    Delocalized[DeCodePoint] = DecimalHexUppercase[String[OGCodePoint] - 0x37];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else if (String[OGCodePoint] == UTF32Character('P') || String[OGCodePoint] == UTF32Character('X')) {
                                    Delocalized[DeCodePoint] = String[OGCodePoint];
                                    DeCodePoint += 1;
                                    OGCodePoint += 1;
                                } else {
                                    OGCodePoint += 1;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return Delocalized;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
