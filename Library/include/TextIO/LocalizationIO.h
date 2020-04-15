/*!
 @header              LocalizationIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for cryptography and hashing.
 */

#include "../Macros.h"
#include "../Constants.h"
#include "UnicodeTypes.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_LocalizationIO_H
#define FoundationIO_UnicodeIO_LocalizationIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Windows appears to use ISO 639-2, Mac appears to use a mix of ISO-639 1 and 2.
     */
    
    /*!
     @enum       LocalizationIO_WrittenLanguages
     @abstract   What language is in use, for colation, sorting, etc.
     */
    typedef enum LocalizationIO_WrittenLanguages {
                 WrittenLanguage_Unspecified  = 0,
                 WrittenLanguage_English      = 1,
                 WrittenLanguage_Swedish      = 2,
                 WrittenLanguage_German       = 3,
                 WrittenLanguage_Norwegian    = 4,
                 WrittenLanguage_Icelandic    = 5,
                 WrittenLanguage_Welsh        = 6,
                 WrittenLanguage_French       = 7,
                 WrittenLanguage_Lithuanian   = 8,
                 WrittenLanguage_Russian      = 9,
                 WrittenLanguage_Polish       = 10,
                 WrittenLanguage_Danish       = 11,
                 WrittenLanguage_Scottish     = 12,
                 WrittenLanguage_Irish        = 13,
                 WrittenLanguage_Afrikaans    = 14,
                 WrittenLanguage_Faroese      = 15,
                 WrittenLanguage_Croatian     = 16,
                 WrittenLanguage_Estonian     = 17,
                 WrittenLanguage_Finnish      = 18,
                 WrittenLanguage_Hungarian    = 19,
                 WrittenLanguage_Sami         = 20,
                 WrittenLanguage_Komi         = 21,
                 WrittenLanguage_Bulgarian    = 22,
                 WrittenLanguage_Cornish      = 23,
                 WrittenLanguage_Dutch        = 24,
                 WrittenLanguage_Flemish      = 25,
                 WrittenLanguage_Lithuianian  = 26,
                 WrittenLanguage_Latvian      = 27,
                 WrittenLanguage_Macedonian   = 28,
    } LocalizationIO_WrittenLanguages;
    
    /*!
     @enum       LocalizationIO_SpokenLanguages
     @abstract   What language is in use, for colation, sorting, etc.
     @constant   SpokenLanguage_Unspecified    Unspecified, falls back to locale.
     */
    typedef enum LocalizationIO_SpokenLanguages {
                 SpokenLanguage_Unspecified = 0,
                 SpokenLanguage_English     = 1,
                 SpokenLanguage_Swedish     = 2,
                 SpokenLanguage_German      = 3,
                 SpokenLanguage_Norwegian   = 4,
                 SpokenLanguage_Icelandic   = 5,
                 SpokenLanguage_Russian     = 6,
                 SpokenLanguage_Polish      = 7,
                 SpokenLanguage_Danish      = 8,
                 SpokenLanguage_Lithuanian  = 9,
                 SpokenLanguage_Komi        = 10,
                 SpokenLanguage_Bulgarian   = 11,
                 SpokenLanguage_Cornish     = 12,
                 SpokenLanguage_Faroese     = 13,
                 SpokenLanguage_Estonian    = 14,
                 SpokenLanguage_Finnish     = 15,
                 SpokenLanguage_Sami        = 16,
                 SpokenLanguage_Dutch       = 17,
                 SpokenLanguage_Lithuianian = 18,
                 SpokenLanguage_Latvian     = 19,
                 SpokenLanguage_Macedonian  = 20,
                 SpokenLanguage_Scottish    = 21,
                 SpokenLanguage_Irish       = 22,
                 SpokenLanguage_Welsh       = 23,
                 SpokenLanguage_French      = 24,
                 SpokenLanguage_Afrikaans   = 25,
                 SpokenLanguage_Croatian    = 26,
                 SpokenLanguage_Hungarian   = 27,
                 SpokenLanguage_Flemish     = 28,
    } LocalizationIO_SpokenLanguages;
    
    /*!
     @enum                      LocalizationIO_RegionIDs
     @abstract                                                  "What region is in use, for collation, sorting, etc".
     @constant                  LanguageID_Unknown              "Unknown file type".
     @constant                  LanguageID_Default              "Equilivent to C locale".
     @constant                  LanguageID_English              "The language is English".
     @constant                  LanguageID_Swedish              "The language is Swedish".
     @constant                  LanguageID_Norwegian            "The language is Norwegian".
     @constant                  LanguageID_Icelandic            "The language is Icelandic".
     @constant                  LanguageID_German               "The language is German".
     @constant                  LanguageID_Welsh                "The language is Welsh".
     @constant                  LanguageID_French               "The language is French".
     @constant                  LanguageID_Lithuanian           "The language is Lithuanian".
     @constant                  LanguageID_Russian              "The language is Russian".
     @constant                  LanguageID_Polish               "The language is Polish".
     @constant                  LanguageID_Danish               "The language is Danish".
     @constant                  LanguageID_Scottish             "The language is Scottish".
     @constant                  LanguageID_Irish                "The language is Irish".
     @constant                  LanguageID_Afrikaans            "The language is Afrikaans".
     @constant                  LanguageID_Faroese              "The language is Faroese".
     @constant                  LanguageID_Croatian             "The language is Croatian".
     */
    typedef enum LocalizationIO_RegionIDs {
                                RegionID_Unknown            = 0,
                                RegionID_Default            = 1, // Equilivent to "C" locale
                                RegionID_UnitedStates       = 2,
                                RegionID_Canada             = 3,
                                RegionID_Mexico             = 4,
                                RegionID_UnitedKingdom      = 5,
                                RegionID_Sweden             = 6,
                                RegionID_Denmark            = 7,
                                RegionID_Norway             = 8,
                                RegionID_Finland            = 9,
                                RegionID_Estonia            = 10,
                                RegionID_Latvia             = 11,
                                RegionID_Lithuania          = 12,
                                RegionID_Poland             = 13,
                                RegionID_Russia             = 14,
                                RegionID_Croatia            = 15,
    } LocalizationIO_RegionIDs;
    
    typedef enum LocalizationIO_EncodingIDs {
                                EncodingID_Unknown          = 0,
                                EncodingID_UTF8             = 1,
                                EncodingID_UTF16            = 2,
                                EncodingID_UTF32            = 3,
    } LocalizationIO_EncodingIDs;
    
    typedef enum LocalizationIO_DecimalSeperators {
                                DecimalSeperator_Unknown    = 0,
                                DecimalSeperator_Period     = 1,
                                DecimalSeperator_Comma      = 2,
                                DecimalSeperator_Apostrophe = 3,
    } LocalizationIO_DecimalSeperators;
    
    typedef enum LocalizationIO_GroupSeperators {
                                GroupSeperator_Unknown      = 0,
                                GroupSeperator_Comma        = 1,
                                GroupSeperator_Period       = 2,
                                GroupSeperator_Underscore   = 3,
                                GroupSeperator_Space        = 4,
    } LocalizationIO_GroupSeperators;
    
    /*!
     @abstract                                              "Creates a string containing the language code (en for englisn, etc)".
     @return                                                "Returns the LanguageID".
     */
     LocalizationIO_WrittenLanguages Localize_GetWrittenLanguageID(void);
    
    /*!
     @abstract                                              "Creates a string containing the region code (us for America, etc)".
     @return                                                "Returns the RegionID".
     */
    LocalizationIO_RegionIDs    Localize_GetRegionID(void);
    
    /*!
     @abstract                                              "Gets the encoding".
     @return                                                "Returns the EncodingID".
     */
    FoundationIO_StringTypes  Localize_GetEncodingID(void);
    
    /*!
     @abstract                                              "Creates a string containing the symbol to seperate the main value from the decimal (XXX.YYY)".
     @return                                                "Returns the string".
     */
    UTF8                       *Localize_UTF8_GetDecimalSeperator(void);
    
    /*!
     @abstract                                              "Creates a string containing the symbol to seperate the main value from the decimal (XXX.YYY)".
     @return                                                "Returns the string".
     */
    UTF16                      *Localize_UTF16_GetDecimalSeperator(void);
    
    /*!
     @abstract                                              "Creates a string containing the symbol to seperate groups (1,000,000)".
     @return                                                "Returns the string".
     */
    UTF8                       *Localize_UTF8_GetGroupingSeperator(void);
    
    /*!
     @abstract                                              "Creates a string containing the symbol to seperate groups (1,000,000)".
     @return                                                "Returns the string".
     */
    UTF16                      *Localize_UTF16_GetGroupingSeperator(void);
    
    /*!
     @abstract                                              "Creates a string array containing the number of digits for each group (1,000,000)".
     @return                                                "Returns the string".
     */
    UTF8                      **Localize_UTF8_GetGroupingSize(void);
    
    /*!
     @abstract                                              "Creates a string array containing the number of digits for each group (1,000,000)".
     @return                                                "Returns the string".
     */
    UTF16                     **Localize_UTF16_GetGroupingSize(void);
    
    /*!
     @abstract                                              "Creates a string containing the currency symbol ($1)".
     @return                                                "Returns the string".
     */
    UTF8                       *Localize_UTF8_GetCurrencySymbol(void);
    
    /*!
     @abstract                                              "Creates a string containing the currency symbol ($1)".
     @return                                                "Returns the string".
     */
    UTF16                      *Localize_UTF16_GetCurrencySymbol(void);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     Base                        "What base is the integer in"?
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF8                       *UTF8_DelocalizeInteger(FoundationIO_Bases Base, UTF8 *String);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     Base                        "What base is the integer in"?
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF16                      *UTF16_DelocalizeInteger(FoundationIO_Bases Base, UTF16 *String);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     Base                        "What base is the integer in"?
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF32                      *UTF32_DelocalizeInteger(FoundationIO_Bases Base, UTF32 *String);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF8                       *UTF8_DelocalizeDecimal(UTF8 *String);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF16                      *UTF16_DelocalizeDecimal(UTF16 *String);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF32                      *UTF32_DelocalizeDecimal(UTF32 *String);
    
#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_UnicodeIO_LocalizationIO_H */
