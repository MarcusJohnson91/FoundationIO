#include "../include/Macros.h"         /* Included for FoundationIO's macros */
#include "../include/LocalizationIO.h" /* Included for our Declarations */

#include "../include/Log.h"            /* Included for error logging */
#include "../include/UnicodeTables.h"  /* Included for Currency tables */

#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef LocalizationIONULLTerminator
#define LocalizationIONULLTerminator (0)
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
                // Get the location of the Dash, create a string that size, copy.
                // Read from 0 to Location
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
        /* Read from UTF8, convert to UTF16 */
        UTF8 *LanguageName8                            = NULL;
        UTF8 *LocaleAll                                = setlocale(LC_ALL, NULL);
        uint64_t StringSize                            = UTF8_GetStringSizeInCodeUnits(LocaleAll);
        if (StringSize > 0) {
            uint64_t Location                          = 0ULL;
            Location                                   = UTF8_FindSubString(LocaleAll, U8("-"), 0, -1);
            if (Location != 0xFFFFFFFFFFFFFFFF) {
                // Get the location of the Dash, create a string that size, copy.
                // Read from 0 to Location
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
        UTF8 *Region        = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        /* Find the Start offset as the location of a - or _; find the end location as . */
        UTF8 *LocaleAll     = setlocale(LC_ALL, NULL);
        int64_t Start       = UTF8_FindSubString(LocaleAll, U8("."), 0, -1);
        uint64_t StringSize = UTF8_GetStringSizeInCodePoints(LocaleAll);
        for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize - 1; CodeUnit++) {
            if (LocaleAll[CodeUnit] == '-') {
                
            }
        }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#endif
        return Region;
    }
    
    UTF16 *Localize_UTF16_GetRegion(void) {
        UTF16 *Region = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#endif
        return Region;
    }
    
    UTF8 *Localize_UTF8_GetEncoding(void) {
        UTF8 *Region = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        /* Get the StringSize, get the offset of . read everything between . and the end */
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#endif
        return Region;
    }
    
    UTF16 *Localize_UTF16_GetEncoding(void) {
        UTF16 *Region = NULL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        /* Get the StringSize, get the offset of . read everything between . and the end */
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#endif
        return Region;
    }
    
    UTF8 *Localize_UTF8_GetDecimalSeperator(void) {
        UTF8 *DecimalSeperator = NULL;
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
        UTF8 *GroupingSeperator = NULL;
        lconv_Init();
        struct lconv *Locale           = localeconv();
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
        struct lconv *Locale           = localeconv();
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
        struct lconv *Locale           = localeconv();
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
        UTF8 *GroupingSizeString        = Locale->grouping;
        UTF8 *Delimiters[]              = {U8("/"), U8("\\")};
        
        GroupingSize                    = UTF8_SplitString(GroupingSizeString, Delimiters);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        UTF16 *GroupingSizeString       = Locale->grouping;
        UTF16 *Delimiters[]             = {U16("/"), U16("\\")};
        
        UTF16 **GroupingSize16          = UTF16_SplitString(GroupingSizeString, Delimiters);
        UTF32 **GroupingSize32          = UTF16_StringArray_Decode(GroupingSize16);
        UTF16_StringArray_Deinit(GroupingSize16);
        GroupingSize                    = UTF8_StringArray_Encode(GroupingSize32);
        UTF32_StringArray_Deinit(GroupingSize32);
#endif
        return GroupingSize;
    }
    
    UTF16 **Localize_UTF16_GetGroupingSize(void) {
        UTF16 **GroupingSize             = NULL;
        lconv_Init();
        struct lconv *Locale           = localeconv();
        UTF8 *GroupingSizeString        = Locale->grouping;
        UTF8 *Delimiters[]              = {U8("/"), U8("\\")};
        
        UTF8 **GroupingSize8            = UTF8_SplitString(GroupingSizeString, Delimiters);
        UTF32 **GroupingSize32          = UTF8_StringArray_Decode(GroupingSize8);
        UTF8_StringArray_Deinit(GroupingSize8);
        GroupingSize                    = UTF16_StringArray_Encode(GroupingSize32);
        UTF32_StringArray_Deinit(GroupingSize32);
        return GroupingSize;
    }
    
    /* Currency Section */
    
    // Generic Currency Symbol: ¤
    
    UTF8 *Localize_UTF8_GetCurrencySymbol(void) {
        UTF8 *CurrencySymbol = NULL;
        return CurrencySymbol;
    }
    
    UTF16 *Localize_UTF16_GetCurrencySymbol(void) {
        UTF16 *CurrencySymbol = NULL;
        return CurrencySymbol;
    }
    
    /* Currency Section */
    
    
    /* Delocalize - To remove localization information from a string */
    /* What kinds of localization information should we remove? Strip currency formatting for one, possibly base conversion as well? */
    
    UTF8 *Delocalize_UTF8_Currency(UTF8 *String) {
        UTF8 *Stripped = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF8_Decode(String);
            UTF32 *Stripped32 = Delocalize_UTF32_Currency(String32);
            free(String32);
            Stripped          = UTF8_Encode(Stripped32);
            free(Stripped32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Stripped;
    }
    
    UTF16 *Delocalize_UTF16_Currency(UTF16 *String) {
        UTF16 *Stripped = NULL;
        if (String != NULL) {
            UTF32 *String32   = UTF16_Decode(String);
            UTF32 *Stripped32 = Delocalize_UTF32_Currency(String32);
            free(String32);
            Stripped          = UTF16_Encode(Stripped32);
            free(Stripped32);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Stripped;
    }
    
    UTF32 *Delocalize_UTF32_Currency(UTF32 *String) { // This name is vague, what exactly are we doing, what is the point?
                                                // This function exists to remove currency symbols and any brackets, parenthesis, etc and return only numbers in a language agnostic way
        UTF32 *Stripped = NULL;
        if (String != NULL) {
            // Basically just loop over the string looking for any currency symbol or brackets or anything, and remove all of that
            
            // do while loop is all that's nessicary, if we see any codepoints to be removed smply increment the mount of the shit to remove variable and we'll allocate the coorect sized string afterwards
            
            uint64_t CodePoint         = 0ULL;
            uint64_t CodePoints2Remove = 0ULL;
            UTF32    CurrentCodePoint  = 1ULL;
            do {
                CurrentCodePoint       = String[CodePoint];
                if (CurrentCodePoint == U32('(') || CurrentCodePoint == U32(')') || CurrentCodePoint == U32('[') || CurrentCodePoint == U32(']')) {
                    CodePoints2Remove += 1;
                }
                for (uint64_t CurrencyCodePoint = 0ULL; CurrentCodePoint < CurrencyTableSize - 1; CurrentCodePoint++) {
                    if (CurrentCodePoint == CurrencyTable[CurrencyCodePoint]) {
                        CodePoints2Remove += 1;
                    }
                }
                CodePoint += 1;
            } while (String[CodePoint] != LocalizationIONULLTerminator); // Get the number of CodePoints to remove
            
            Stripped = calloc(CodePoint - CodePoints2Remove, sizeof(UTF32));
            
            if (Stripped != NULL) {
                // Ok, so now we need to do the same thing but this time copying
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate %lld codepoints"), CodePoint - CodePoints2Remove);
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return Stripped;
    }
    
#ifdef __cplusplus
}
#endif
