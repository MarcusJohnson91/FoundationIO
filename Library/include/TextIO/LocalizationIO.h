/*!
 @header           LocalizationIO.h
 @author           Marcus Johnson
 @copyright        2019+
 @version          1.0.0
 @brief            This header contains types, functions, and tables for cryptography and hashing.
 */

#include "../PlatformIO.h"  /* Included for Platform Independence macros */
#include "TextIOTypes.h"    /* Included for Text types */

#pragma once

#ifndef FoundationIO_TextIO_LocalizationIO_H
#define FoundationIO_TextIO_LocalizationIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @enum         LocalizationIO_WrittenLanguages
     @abstract                                      What language is in use, for colation, sorting, etc.
     @constant     WrittenLanguage_Unspecified      Unspecified, falls back to locale.
     */
    typedef enum LocalizationIO_WrittenLanguages {
                   WrittenLanguage_Unspecified      = 0,
                   WrittenLanguage_English          = 1,
                   WrittenLanguage_Swedish          = 2,
                   WrittenLanguage_German           = 3,
                   WrittenLanguage_Norwegian        = 4,
                   WrittenLanguage_Icelandic        = 5,
                   WrittenLanguage_Welsh            = 6,
                   WrittenLanguage_French           = 7,
                   WrittenLanguage_Lithuanian       = 8,
                   WrittenLanguage_Russian          = 9,
                   WrittenLanguage_Polish           = 10,
                   WrittenLanguage_Danish           = 11,
                   WrittenLanguage_Scottish         = 12,
                   WrittenLanguage_Irish            = 13,
                   WrittenLanguage_Afrikaans        = 14,
                   WrittenLanguage_Faroese          = 15,
                   WrittenLanguage_Croatian         = 16,
                   WrittenLanguage_Estonian         = 17,
                   WrittenLanguage_Finnish          = 18,
                   WrittenLanguage_Hungarian        = 19,
                   WrittenLanguage_Sami             = 20,
                   WrittenLanguage_Komi             = 21,
                   WrittenLanguage_Bulgarian        = 22,
                   WrittenLanguage_Cornish          = 23,
                   WrittenLanguage_Dutch            = 24,
                   WrittenLanguage_Flemish          = 25,
                   WrittenLanguage_Lithuianian      = 26,
                   WrittenLanguage_Latvian          = 27,
                   WrittenLanguage_Macedonian       = 28,
                   WrittenLanguage_Yoruba           = 29,
                   WrittenLanguage_Malay            = 30,
                   WrittenLanguage_Kannada          = 31,
                   WrittenLanguage_Hausa            = 32,
                   WrittenLanguage_Jin              = 33,
                   WrittenLanguage_Hakka            = 34,
                   WrittenLanguage_Persian          = 35,
                   WrittenLanguage_Minnan           = 36,
                   WrittenLanguage_Bhojpuri         = 37,
                   WrittenLanguage_Gujarati         = 38,
                   WrittenLanguage_Italian          = 39,
                   WrittenLanguage_Javanese         = 40,
                   WrittenLanguage_Tamil            = 41,
                   WrittenLanguage_Yue              = 42,
                   WrittenLanguage_Vietnamese       = 43,
                   WrittenLanguage_Korean           = 44,
                   WrittenLanguage_Turkish          = 45,
                   WrittenLanguage_Wu               = 46,
                   WrittenLanguage_Telugu           = 47,
                   WrittenLanguage_Marathi          = 48,
                   WrittenLanguage_Punjabi          = 49,
                   WrittenLanguage_Japanese         = 50,
                   WrittenLanguage_Bengali          = 51,
                   WrittenLanguage_Hindu            = 52,
                   WrittenLanguage_Spanish          = 53,
                   WrittenLanguage_Portugese        = 54,
                   WrittenLanguage_Mandarin         = 55,
    } LocalizationIO_WrittenLanguages;
    
    /*!
     @enum         LocalizationIO_SpokenLanguages
     @abstract                                      What language is in use, for colation, sorting, etc.
     @constant     SpokenLanguage_Unspecified       Unspecified, falls back to locale.
     */
    typedef enum LocalizationIO_SpokenLanguages {
                   SpokenLanguage_Unspecified       = 0,
                   SpokenLanguage_English           = 1,
                   SpokenLanguage_Swedish           = 2,
                   SpokenLanguage_German            = 3,
                   SpokenLanguage_Norwegian         = 4,
                   SpokenLanguage_Icelandic         = 5,
                   SpokenLanguage_Russian           = 6,
                   SpokenLanguage_Polish            = 7,
                   SpokenLanguage_Danish            = 8,
                   SpokenLanguage_Lithuanian        = 9,
                   SpokenLanguage_Komi              = 10,
                   SpokenLanguage_Bulgarian         = 11,
                   SpokenLanguage_Cornish           = 12,
                   SpokenLanguage_Faroese           = 13,
                   SpokenLanguage_Estonian          = 14,
                   SpokenLanguage_Finnish           = 15,
                   SpokenLanguage_Sami              = 16,
                   SpokenLanguage_Dutch             = 17,
                   SpokenLanguage_Lithuianian       = 18,
                   SpokenLanguage_Latvian           = 19,
                   SpokenLanguage_Macedonian        = 20,
                   SpokenLanguage_Scottish          = 21,
                   SpokenLanguage_Irish             = 22,
                   SpokenLanguage_Welsh             = 23,
                   SpokenLanguage_French            = 24,
                   SpokenLanguage_Afrikaans         = 25,
                   SpokenLanguage_Croatian          = 26,
                   SpokenLanguage_Hungarian         = 27,
                   SpokenLanguage_Flemish           = 28,
                   SpokenLanguage_Arabic            = 29,
                   SpokenLanguage_Spanish           = 30,
    } LocalizationIO_SpokenLanguages;
    
    /*!
     @enum       LocalizationIO_RegionIDs
     @abstract                                      What region is in use, for collation, sorting, etc.
     @constant   RegionID_Unspecified               Unspecified, falls back to locale.
     */
    typedef enum LocalizationIO_RegionIDs {
                   RegionID_Unspecified             = 0,
                   RegionID_UnitedStates            = 1,
                   RegionID_Canada                  = 2,
                   RegionID_Mexico                  = 3,
                   RegionID_UnitedKingdom           = 4,
                   RegionID_Sweden                  = 5,
                   RegionID_Denmark                 = 6,
                   RegionID_Norway                  = 7,
                   RegionID_Finland                 = 8,
                   RegionID_Estonia                 = 9,
                   RegionID_Latvia                  = 10,
                   RegionID_Lithuania               = 11,
                   RegionID_Poland                  = 12,
                   RegionID_Russia                  = 13,
                   RegionID_Croatia                 = 14,
                   RegionIO_Russia                  = 15,
                   RegionID_Germany                 = 16,
                   RegionID_Austria                 = 17,
    } LocalizationIO_RegionIDs;
    
    /*!
     @enum         LocalizationIO_DecimalSeperators
     @abstract                                      What decimal seperator is in use, for collation, sorting, etc.
     */
    typedef enum LocalizationIO_DecimalSeperators {
                   DecimalSeperator_Unspecified     = 0,
                   DecimalSeperator_Period          = 1,
                   DecimalSeperator_Comma           = 2,
                   DecimalSeperator_Apostrophe      = 3,
    } LocalizationIO_DecimalSeperators;
    
    /*!
     @enum         LocalizationIO_GroupSeperators
     @abstract                                      What group seperator is in use, for collation, sorting, etc.
     */
    typedef enum LocalizationIO_GroupSeperators {
                   GroupSeperator_Unspecified       = 0,
                   GroupSeperator_Comma             = 1,
                   GroupSeperator_Period            = 2,
                   GroupSeperator_Underscore        = 3,
                   GroupSeperator_Space             = 4,
                   GroupSeperator_Apostrophe        = 5,
    } LocalizationIO_GroupSeperators;
    
    /*!
     @abstract                                      Creates a string containing the language code (en for englisn, etc).
     @return                                        Returns the LanguageID.
     */
    LocalizationIO_WrittenLanguages Localize_GetWrittenLanguageID(void);
    
    /*!
     @abstract                                      Creates a string containing the region code (us for America, etc).
     @return                                        Returns the RegionID.
     */
    LocalizationIO_RegionIDs    Localize_GetRegionID(void);
    
    /*!
     @abstract                                      Gets the encoding.
     @return                                        Returns the EncodingID.
     */
    UnicodeIO_StringTypes     Localize_GetEncodingID(void);
    
    /*!
     @abstract                                      Creates a string containing the symbol to seperate the main value from the decimal (XXX.YYY).
     @return                                        Returns the string.
     */
    UTF8                       *Localize_UTF8_GetDecimalSeperator(void);
    
    /*!
     @abstract                                      Creates a string containing the symbol to seperate the main value from the decimal (XXX.YYY).
     @return                                        Returns the string.
     */
    UTF16                      *Localize_UTF16_GetDecimalSeperator(void);
    
    /*!
     @abstract                                      Creates a string containing the symbol to seperate groups (1,000,000).
     @return                                        Returns the string.
     */
    UTF8                       *Localize_UTF8_GetGroupingSeperator(void);
    
    /*!
     @abstract                                      Creates a string containing the symbol to seperate groups (1,000,000).
     @return                                        Returns the string.
     */
    UTF16                      *Localize_UTF16_GetGroupingSeperator(void);
    
    /*!
     @abstract                                      Creates a StringSet containing the number of digits for each group (1,000,000).
     @return                                        Returns the StringSet.
     */
    UTF8                      **Localize_UTF8_GetGroupingSize(void);
    
    /*!
     @abstract                                      Creates a StringSet containing the number of digits for each group (1,000,000).
     @return                                        Returns the string.
     */
    UTF16                     **Localize_UTF16_GetGroupingSize(void);
    
    /*!
     @abstract                                      Creates a string containing the currency symbol ($1).
     @return                                        Returns the string.
     */
    UTF8                       *Localize_UTF8_GetCurrencySymbol(void);
    
    /*!
     @abstract                                      Creates a string containing the currency symbol ($1).
     @return                                        Returns the string.
     */
    UTF16                      *Localize_UTF16_GetCurrencySymbol(void);
    
    /*!
     @abstract                                      Creates a string containing just digits of Base from String.
     @param                     String              The string to Delocalize.
     @param                     Base                What base is the integer in?
     @return                                        Returns the delocalized string.
     */
    UTF8                       *UTF8_DelocalizeInteger(PlatformIO_Immutable(UTF8 *) String, UnicodeIO_Bases Base);
    
    /*!
     @abstract                                      Creates a string containing just digits of Base from String.
     @param                     String              The string to Delocalize.
     @param                     Base                What base is the integer in?
     @return                                        Returns the delocalized string.
     */
    UTF16                      *UTF16_DelocalizeInteger(PlatformIO_Immutable(UTF16 *) String, UnicodeIO_Bases Base);
    
    /*!
     @abstract                                      Creates a string containing just digits of Base from String.
     @param                     String              The string to Delocalize.
     @param                     Base                What base is the integer in?
     @return                                        Returns the delocalized string.
     */
    UTF32                      *UTF32_DelocalizeInteger(PlatformIO_Immutable(UTF32 *) String, UnicodeIO_Bases Base);
    
    /*!
     @abstract                                      Creates a string containing just digits of Base from String.
     @param                     String              The string to Delocalize.
     @param                     Base                What base is the Decimal in?
     @return                                        Returns the delocalized string.
     */
    UTF8                       *UTF8_DelocalizeDecimal(PlatformIO_Immutable(UTF8 *) String, UnicodeIO_Bases Base);
    
    /*!
     @abstract                                      Creates a string containing just digits of Base from String.
     @param                     String              The string to Delocalize.
     @param                     Base                What base is the Decimal in?
     @return                                        Returns the delocalized string.
     */
    UTF16                      *UTF16_DelocalizeDecimal(PlatformIO_Immutable(UTF16 *) String, UnicodeIO_Bases Base);
    
    /*!
     @abstract                                      Creates a string containing just digits of Base from String.
     @param                     String              The string to Delocalize.
     @param                     Base                What base is the Decimal in?
     @return                                        Returns the delocalized string.
     */
    UTF32                      *UTF32_DelocalizeDecimal(PlatformIO_Immutable(UTF32 *) String, UnicodeIO_Bases Base);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_LocalizationIO_H */
