/*!
 @header              LocalizationIO.h
 @author              Marcus Johnson
 @copyright           2019+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for cryptography and hashing.
 */

#include "../../include/Macros.h"

#pragma once

#ifndef FoundationIO_UnicodeIO_LocalizationIO_H
#define FoundationIO_UnicodeIO_LocalizationIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char8_t                               UTF8;
#else
    typedef               unsigned char                         UTF8;
#endif /* __CHAR8_TYPE__ */
#endif /* FoundationIO_StringType8 */
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
#if (defined __STDC_UTF_16__ && defined __CHAR16_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char16_t                              UTF16;
#else
    typedef               uint_least16_t                        UTF16;
#endif /* __CHAR16_TYPE__ */
#endif /* FoundationIO_StringType16 */
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef               char32_t                              UTF32;
#else
    typedef               uint_least32_t                        UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
#ifndef                   FoundationIO_Unicodize8
#define                   FoundationIO_Unicodize8               (1)
#define                   UTF8String(Literal)                   (UTF8*) u8##Literal
#define                   UTF8Character(Literal)                (UTF8)  u8##Literal
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                   FoundationIO_Unicodize16
#define                   FoundationIO_Unicodize16              (2)
#define                   UTF16String(Literal)                  (UTF16*) u##Literal
#define                   UTF16Character(Literal)               (UTF16)  u##Literal
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                   FoundationIO_Unicodize32
#define                   FoundationIO_Unicodize32              (4)
#define                   UTF32String(Literal)                  (UTF32*) U##Literal
#define                   UTF32Character(Literal)               (UTF32) U##Literal
#endif /* FoundationIO_Unicodize32 */
    
#ifndef                   FoundationIOFunctionName
#define                   FoundationIOFunctionName              (UTF8*) __func__
#endif /* FoundationIOFunctionName */
    
    /*
     Windows appears to use ISO 639-2, Mac appears to use a mix of ISO-639 1 and 2.
     */
    
    /*!
     @enum                      LocalizationIO_LanguageIDs
     @abstract                                                  "What language is in use, for colation, sorting, etc".
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
    typedef enum LocalizationIO_LanguageIDs {
                                LanguageID_Unknown          = 0,
                                LanguageID_Default          = 1,
                                LanguageID_English          = 2,
                                LanguageID_Swedish          = 3,
                                LanguageID_Norwegian        = 4,
                                LanguageID_Icelandic        = 5,
                                LanguageID_German           = 4,
                                LanguageID_Welsh            = 5,
                                LanguageID_French           = 6,
                                LanguageID_Lithuanian       = 7,
                                LanguageID_Russian          = 8,
                                LanguageID_Polish           = 9,
                                LanguageID_Danish           = 10,
                                LanguageID_Scottish         = 11,
                                LanguageID_Irish            = 12,
                                LanguageID_Afrikaans        = 13,
                                LanguageID_Faroese          = 14,
                                LanguageID_Croatian         = 15,
    } LocalizationIO_LanguageIDs;
    
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
     LocalizationIO_LanguageIDs Localize_GetLanguageID(void);
    
    /*!
     @abstract                                              "Creates a string containing the region code (us for America, etc)".
     @return                                                "Returns the RegionID".
     */
    LocalizationIO_RegionIDs    Localize_GetRegionID(void);
    
    /*!
     @abstract                                              "Gets the encoding".
     @return                                                "Returns the EncodingID".
     */
    LocalizationIO_EncodingIDs  Localize_GetEncodingID(void);
    
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
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF8                       *UTF8_DelocalizeInteger(UTF8 *String);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF16                      *UTF16_DelocalizeInteger(UTF16 *String);
    
    /*!
     @abstract                                              "Creates a string containing just the base 10 digits from String".
     @param                     String                      "The string to Delocalize".
     @return                                                "Returns the delocalized string".
     */
    UTF32                      *UTF32_DelocalizeInteger(UTF32 *String);
    
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
