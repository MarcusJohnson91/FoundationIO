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
    
    UTF8 *Localize_UTF8_GetLanguage(void) {
        UTF8 *LanguageName     = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8 *LocaleAll        = setlocale(LC_ALL, NULL);
        uint64_t StringSize    = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize > 0) {
            uint64_t Location  = 0ULL;
            Location           = UTF8_FindSubString(LocaleAll, U8("-"), 0, -1);
            if (Location != 0xFFFFFFFFFFFFFFFF) {
                LanguageName   = calloc(Location + 1, sizeof(UTF8));
                for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                    LanguageName[LanguageCodeUnit] = LocaleAll[LanguageCodeUnit];
                }
            } else {
                Location       = UTF8_FindSubString(LocaleAll, U8("_"), 0, -1);
                if (Location != 0xFFFFFFFFFFFFFFFF) {
                    LanguageName   = calloc(Location + 1, sizeof(UTF8));
                    for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                        LanguageName[LanguageCodeUnit] = LocaleAll[LanguageCodeUnit];
                    }
                }
            }
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16 *LocaleAll       = _wsetlocale(LC_ALL, NULL);
        uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize > 0) {
            uint64_t Location  = 0ULL;
            Location           = UTF16_FindSubString(LocaleAll, U16("-"), 0, -1);
            if (Location != 0xFFFFFFFFFFFFFFFF) {
                LanguageName   = calloc(Location + 1, sizeof(UTF16));
                for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                    LanguageName[LanguageCodeUnit] = LocaleAll[LanguageCodeUnit];
                }
            } else {
                Location       = UTF16_FindSubString(LocaleAll, U16("_"), 0, -1);
                if (Location != 0xFFFFFFFFFFFFFFFF) {
                    LanguageName   = calloc(Location + 1, sizeof(UTF16));
                    for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                        LanguageName[LanguageCodeUnit] = LocaleAll[LanguageCodeUnit];
                    }
                }
            }
        }
#endif
        return LanguageName;
    }
    
    UTF16 *Localize_UTF16_GetLanguage(void) {
        UTF16 *LanguageName                            = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8 *LanguageName8                            = NULL;
        UTF8 *LocaleAll                                = setlocale(LC_ALL, NULL);
        uint64_t StringSize                            = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize > 0) {
            uint64_t Location                          = 0ULL;
            Location                                   = UTF8_FindSubString(LocaleAll, U8("-"), 0, -1);
            if (Location != 0xFFFFFFFFFFFFFFFF) {
                LanguageName8                          = calloc(Location + 1, sizeof(UTF8));
                for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                    LanguageName[LanguageCodeUnit]     = LocaleAll[LanguageCodeUnit];
                }
            } else {
                Location                               = UTF8_FindSubString(LocaleAll, U8("_"), 0, -1);
                if (Location != 0xFFFFFFFFFFFFFFFF) {
                    LanguageName8                      = calloc(Location + 1, sizeof(UTF8));
                    for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                        LanguageName[LanguageCodeUnit] = LocaleAll[LanguageCodeUnit];
                    }
                }
            }
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16 *LocaleAll       = _wsetlocale(LC_ALL, NULL);
        uint64_t StringSize    = UTF16_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize > 0) {
            uint64_t Location  = 0ULL;
            Location           = UTF16_FindSubString(LocaleAll, U8("-"), 0, -1);
            if (Location != 0xFFFFFFFFFFFFFFFF) {
                LanguageName   = calloc(Location + 1, sizeof(UTF16));
                for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                    LanguageName[LanguageCodeUnit] = LocaleAll[LanguageCodeUnit];
                }
            } else {
                Location       = UTF16_FindSubString(LocaleAll, U8("_"), 0, -1);
                if (Location != 0xFFFFFFFFFFFFFFFF) {
                    LanguageName   = calloc(Location + 1, sizeof(UTF16));
                    for (uint64_t LanguageCodeUnit = 0ULL; LanguageCodeUnit < Location; LanguageCodeUnit++) {
                        LanguageName[LanguageCodeUnit] = LocaleAll[LanguageCodeUnit];
                    }
                }
            }
        }
#endif
        return LanguageName;
    }
    
    UTF8 *Localize_UTF8_GetRegion(void) {
        UTF8 *Region                           = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8     *LocaleAll                    = setlocale(LC_ALL, NULL);
        uint64_t  StringSize                   = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        uint64_t  StartOffset                  = 0ULL;
        uint64_t  DotOffset                    = 0ULL;
        for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
            if (LocaleAll[CodeUnit] == '.') {
                DotOffset                      = CodeUnit;
            } else if (LocaleAll[CodeUnit] == '-' || LocaleAll[CodeUnit] == '_') {
                StartOffset                    = CodeUnit;
                break;
            }
        }
        Region                                 = calloc(DotOffset - StartOffset + FoundationIONULLTerminatorSize, sizeof(UTF8));
        if (Region != NULL) {
            for (uint64_t CodeUnit = StartOffset - 1; CodeUnit < DotOffset - 1; CodeUnit++) {
                Region[CodeUnit - StartOffset] = LocaleAll[CodeUnit - StartOffset];
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), DotOffset - StartOffset + FoundationIONULLTerminatorSize);
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16    *LocaleAll                      = _wsetlocale(LC_ALL, NULL);
        uint64_t  StringSize                     = UTF16_GetStringSizeInCodeUnits(LocaleAll);
        uint64_t  StartOffset                    = 0ULL;
        uint64_t  DotOffset                      = 0ULL;
        for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
            if (LocaleAll[CodeUnit] == '.') {
                DotOffset                        = CodeUnit;
            } else if (LocaleAll[CodeUnit] == '-' || LocaleAll[CodeUnit] == '_') {
                StartOffset                      = CodeUnit;
                break;
            }
        }
		UTF16 *Region16                          = calloc(DotOffset - StartOffset + FoundationIONULLTerminatorSize, sizeof(UTF16));
        if (Region16 != NULL) {
            for (uint64_t CodeUnit = StartOffset - 1; CodeUnit < DotOffset - 1; CodeUnit++) {
                Region16[CodeUnit - StartOffset] = LocaleAll[CodeUnit - StartOffset];
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), DotOffset - StartOffset + FoundationIONULLTerminatorSize);
        }
        UTF32 *Region32                          = UTF16_Decode(Region16);
        free(Region16);
        Region                                   = UTF8_Encode(Region32);
        free(Region32);
#endif
        return Region;
    }
    
    UTF16 *Localize_UTF16_GetRegion(void) {
        UTF16 *Region = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8     *LocaleAll                      = setlocale(LC_ALL, NULL);
        uint64_t  StringSize                     = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        uint64_t  StartOffset                    = 0ULL;
        uint64_t  DotOffset                      = 0ULL;
        for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
            if (LocaleAll[CodeUnit] == '.') {
                DotOffset                        = CodeUnit;
            } else if (LocaleAll[CodeUnit] == '-' || LocaleAll[CodeUnit] == '_') {
                StartOffset                      = CodeUnit;
                break;
            }
        }
        UTF8 *Region8                            = calloc(DotOffset - StartOffset + FoundationIONULLTerminatorSize, sizeof(UTF8));
        if (Region8 != NULL) {
            for (uint64_t CodeUnit = StartOffset - 1; CodeUnit < DotOffset - 1; CodeUnit++) {
                Region8[CodeUnit - StartOffset]  = LocaleAll[CodeUnit - StartOffset];
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), DotOffset - StartOffset + FoundationIONULLTerminatorSize);
        }
        UTF32 *Region32                          = UTF8_Decode(Region8);
        free(Region8);
        Region                                   = UTF16_Encode(Region32);
        free(Region32);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16    *LocaleAll                    = _wsetlocale(LC_ALL, NULL);
        uint64_t  StringSize                   = UTF16_GetStringSizeInCodeUnits(LocaleAll);
        uint64_t  StartOffset                  = 0ULL;
        uint64_t  DotOffset                    = 0ULL;
        for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
            if (LocaleAll[CodeUnit] == '.') {
                DotOffset                      = CodeUnit;
            } else if (LocaleAll[CodeUnit] == '-' || LocaleAll[CodeUnit] == '_') {
                StartOffset                    = CodeUnit;
                break;
            }
        }
        Region                                 = calloc(DotOffset - StartOffset + FoundationIONULLTerminatorSize, sizeof(UTF16));
        if (Region != NULL) {
            for (uint64_t CodeUnit = StartOffset - 1; CodeUnit < DotOffset - 1; CodeUnit++) {
                Region[CodeUnit - StartOffset] = LocaleAll[CodeUnit - StartOffset];
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), DotOffset - StartOffset + FoundationIONULLTerminatorSize);
        }
#endif
        return Region;
    }
    
    UTF8 *Localize_UTF8_GetEncoding(void) {
        UTF8 *Region                            = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8    *Lang                           = getenv(U8("LANG"));
        uint64_t DotOffset                      = 0ULL;
        
        if (Lang != NULL) {
            uint64_t StringSize                 = UTF8_GetStringSizeInCodeUnits(Lang);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Lang[CodeUnit] == '.') {
                    DotOffset                   = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize               = StringSize - DotOffset;
            Region                              = calloc(EncodingSize, sizeof(UTF8));
            if (Region != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region[EncodingCodePoint]   = Lang[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
            
        } else {
            UTF8     *Language                  = getenv(U8("LANGUAGE"));
            uint64_t  StringSize                = UTF8_GetStringSizeInCodeUnits(Language);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Language[CodeUnit] == '.') {
                    DotOffset                   = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize               = StringSize - DotOffset;
            Region                              = calloc(EncodingSize, sizeof(UTF8));
            if (Region != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region[EncodingCodePoint]   = Language[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16    *Lang                          = _wgetenv(U16("LANG"));
        uint64_t  DotOffset                     = 0ULL;
        
        if (Lang != NULL) {
            uint64_t StringSize                 = UTF16_GetStringSizeInCodeUnits(Lang);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Lang[CodeUnit] == U16('.')) {
                    DotOffset                   = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize               = StringSize - DotOffset;
            Region                              = calloc(EncodingSize, sizeof(UTF16));
            if (Region != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region[EncodingCodePoint]   = Lang[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
        } else {
            UTF16    *Language                  = _wgetenv(U16("LANGUAGE"));
            uint64_t  StringSize                = UTF16_GetStringSizeInCodeUnits(Language);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Language[CodeUnit] == U16('.')) {
                    DotOffset                   = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize               = StringSize - DotOffset;
            UTF16 *Region16                     = calloc(EncodingSize, sizeof(UTF16));
            if (Region16 != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region16[EncodingCodePoint] = Language[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
            UTF32 *Region32                     = UTF16_Decode(Region16);
            free(Region16);
            Region                              = UTF8_Encode(Region32);
            free(Region32);
            
        }
#endif
        return Region;
    }
    
    UTF16 *Localize_UTF16_GetEncoding(void) {
        UTF16 *Region                          = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8    *Lang                          = getenv(U8("LANG"));
        uint64_t DotOffset                     = 0ULL;
        
        if (Lang != NULL) {
            uint64_t StringSize                = UTF8_GetStringSizeInCodeUnits(Lang);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Lang[CodeUnit] == '.') {
                    DotOffset                  = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize              = StringSize - DotOffset;
            UTF8    *Region8                   = calloc(EncodingSize, sizeof(UTF8));
            if (Region8 != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region8[EncodingCodePoint] = Lang[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
            UTF32 *Region32                   = UTF8_Decode(Region8);
            free(Region8);
            Region                            = UTF16_Encode(Region32);
            free(Region32);
        } else {
            UTF8     *Language                = getenv(U8("LANGUAGE"));
            uint64_t  StringSize              = UTF8_GetStringSizeInCodeUnits(Language);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Language[CodeUnit] == '.') {
                    DotOffset                 = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize              = StringSize - DotOffset;
            UTF8    *Region8                   = calloc(EncodingSize, sizeof(UTF8));
            if (Region8 != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region8[EncodingCodePoint] = Language[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
            UTF32 *Region32                    = UTF8_Decode(Region8);
            free(Region8);
            Region                             = UTF16_Encode(Region32);
            free(Region32);
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16    *Lang                         = _wgetenv(U16("LANG"));
        uint64_t  DotOffset                    = 0ULL;
        
        if (Lang != NULL) {
            uint64_t StringSize                = UTF16_GetStringSizeInCodeUnits(Lang);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Lang[CodeUnit] == U16('.')) {
                    DotOffset                  = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize              = StringSize - DotOffset;
            Region                             = calloc(EncodingSize, sizeof(UTF16));
            if (Region != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region[EncodingCodePoint]  = Lang[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
        } else {
            UTF16    *Language                 = _wgetenv(U16("LANGUAGE"));
            uint64_t  StringSize               = UTF16_GetStringSizeInCodeUnits(Language);
            for (uint64_t CodeUnit = StringSize - 1; CodeUnit > 0; CodeUnit--) {
                if (Language[CodeUnit] == U16('.')) {
                    DotOffset                  = CodeUnit;
                    break;
                }
            }
            uint64_t EncodingSize              = StringSize - DotOffset;
            Region                             = calloc(EncodingSize, sizeof(UTF16));
            if (Region != NULL) {
                for (uint64_t EncodingCodePoint = 0ULL; EncodingCodePoint < EncodingSize - 1; EncodingCodePoint++) {
                    Region[EncodingCodePoint]  = Language[EncodingCodePoint + DotOffset];
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld CodeUnits"), EncodingSize);
            }
        }
#endif
        return Region;
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
            
            for (uint64_t CodePoint = 0ULL; CodePoint < StringSize - 1; CodePoint++) {
                for (uint8_t CurrencySymbol = 0; CurrencySymbol < CurrencyTableSize - 1; CurrencySymbol++) {
                    if (String[CodePoint] == CurrencyTable[CurrencySymbol]) {
                        CodePoints2Remove += 1;
                    }
                }
            }
            
            uint64_t StrippedStringSize    = StringSize - CodePoints2Remove;
            
            Stripped                       = calloc(StrippedStringSize + FoundationIONULLTerminatorSize, sizeof(UTF32));
            if (Stripped != NULL) {
                for (uint64_t Original = 0ULL; Original < StringSize - 1; Original++) {
                    for (uint64_t StrippedCodePoint = 0ULL; StrippedCodePoint < StrippedStringSize - 1; StrippedCodePoint++) {
                        for (uint8_t CurrencySymbol = 0ULL; CurrencySymbol < CurrencyTableSize - 1; CurrencySymbol++) {
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
