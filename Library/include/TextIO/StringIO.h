/*!
 @header          StringIO.h
 @author          Marcus Johnson
 @copyright       2017+
 @version         1.0.0
 @brief           This header contains types, functions, and tables for Unicode support, including UTF-8, UTF-16, and our internal format, UTF-32.
 */

#include "TextIOTypes.h"    /* Included for Text types */

#pragma once

#ifndef FoundationIO_TextIO_StringIO_H
#define FoundationIO_TextIO_StringIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @enum         StringIOCommon
     @constant     UTF8CodeUnitSizeInBits                The size of a single code unit in bits.
     @constant     UTF16CodeUnitSizeInBits               The size of a single code unit in bits.
     @constant     UTF32CodeUnitSizeInBits               The size of a single code unit in bits.
     @constant     UTF8BOMSizeInCodeUnits                The number of code units (8 bits) the UTF8 BOM takes.
     @constant     UTF16BOMSizeInCodeUnits               The number of code units (16 bits) the UTF16 BOM takes.
     @constant     UnicodeBOMSizeInCodePoints            The number of CodePoints in a BOM.
     @constant     UTF16BOM_LE                           UTF16BOM_LE byte order mark.
     @constant     UTF16BOM_BE                           UTF16BOM_BE byte order mark.
     @constant     UTF32BOM_LE                           UTF32BOM_LE byte order mark.
     @constant     UTF32BOM_BE                           UTF32BOM_BE byte order mark.
     @constant     UnicodeUNCPathPrefixSize              Size of "//?/" or "\\?\" in CodePoints and CodeUnits.
     @constant     UTF16HighSurrogateStart               The value that marks the start of the High Surrogate range.
     @constant     UTF16HighSurrogateEnd                 The value that marks the end   of the High Surrogate range.
     @constant     UTF16LowSurrogateStart                The value that marks the start of the Low  Surrogate range.
     @constant     UTF16LowSurrogateEnd                  The value that marks the end   of the Low  Surrogate range.
     @constant     UTF16MaxCodeUnitValue                 The highest value that can be stored in a single UTF16 CodeUnit.
     @constant     UTF16SurrogatePairModDividend         The value to modulo the surrogate pair by to decode a High Surrogate.
     @constant     UTF16SurrogatePairStart               The first UTF-32 CodePoint to require Surrogate Pairs in UTF-16.
     @constant     InvalidReplacementCodePoint           The CodePoint to replace invalid codeunits.
     @constant     UnicodeMaxCodePoint                   The highest CodePoint possible in Unicode, 1,114,111.
     @constant     UTF8MaxCodeUnitsInCodePoint           The maximum number of codeunits per codepoint.
     @constant     UTF16MaxCodeUnitsInCodePoint          The maximum number of codeunits per codepoint.
     */
    typedef enum StringIOCommon {
                   UTF8CodeUnitSizeInBits                = 8,
                   UTF16CodeUnitSizeInBits               = 16,
                   UTF32CodeUnitSizeInBits               = 32,
                   UTF8BOMSizeInCodeUnits                = 3,
                   UTF16BOMSizeInCodeUnits               = 1,
                   UnicodeBOMSizeInCodePoints            = 1,
                   UTF8BOM_1                             = 0xEF,
                   UTF8BOM_2                             = 0xBB,
                   UTF8BOM_3                             = 0xBF,
                   UTF16BOM_LE                           = 0xFFFE,
                   UTF16BOM_BE                           = 0xFEFF,
                   UTF32BOM_LE                           = 0xFFFE,
                   UTF32BOM_BE                           = 0xFEFF,
                   UnicodeUNCPathPrefixSize              = 4,
                   UTF16SurrogateMask                    = 0x3FF,
                   UTF16SurrogateShift                   = 10,
                   UTF16HighSurrogateStart               = 0xD800,
                   UTF16HighSurrogateEnd                 = 0xDBFF,
                   UTF16LowSurrogateStart                = 0xDC00,
                   UTF16LowSurrogateEnd                  = 0xDFFF,
                   UTF16MaxCodeUnitValue                 = 0xFFFF,
                   UTF16SurrogatePairModDividend         = 0x400,
                   UTF16SurrogatePairStart               = 0x10000,
                   InvalidReplacementCodePoint           = 0xFFFD,
                   UnicodeMaxCodePoint                   = 0x10FFFF,
                   UTF8MaxCodeUnitsInCodePoint           = 4,
                   UTF16MaxCodeUnitsInCodePoint          = 2,
    } StringIOCommon;

    /*!
     @enum         StringIO_CharSets
     @constant     CharSet_Unspecified                   Invalid conversion type.
     @constant     CharSet_ISO_8859_1                    ISO/IEC 8859-1 to/from Unicode.
     */
    typedef enum StringIO_CharSets {
                   CharSet_Unspecified                   = 0,
                   CharSet_ISO_8859_1                    = 1,
    } StringIO_CharSets;
    
    /*!
     @enum         StringIO_NormalizationForms
     @constant     NormalizationForm_Unspecified         Unknown Normalization Form.
     @constant     NormalizationForm_CanonicalDecompose  Decompose the String.
     @constant     NormalizationForm_CanonicalCompose    Decompose, then Compose the String.
     @constant     NormalizationForm_KompatibleDecompose NormalizationForm_CanonicalDecompose, plus Kompatibility decompositions.
     @constant     NormalizationForm_KompatibleCompose   NormalizationForm_CanonicalDecompose, plus Kompatibility compositions.
     */
    typedef enum StringIO_NormalizationForms {
                   NormalizationForm_Unspecified         = 0,
                   NormalizationForm_CanonicalDecompose  = 1,
                   NormalizationForm_CanonicalCompose    = 2,
                   NormalizationForm_KompatibleDecompose = 3,
                   NormalizationForm_KompatibleCompose   = 4,
    } StringIO_NormalizationForms;
    
    /*!
     @enum         StringIO_BOMs
     @abstract                                           What byte order does the string use?
     @constant     StringIO_BOM_Unspecified              Byte order is unknown.
     @constant     StringIO_BOM_Native                   Use the byte order of the host.
     @constant     StringIO_BOM_Little                   Use the little endian, Least-Significant-Byte first order.
     @constant     StringIO_BOM_Big                      Use the big endian, Most-Significant-Byte first order.
     */
    typedef enum StringIO_BOMs {
                   StringIO_BOM_Unspecified              = 0,
                   StringIO_BOM_Native                   = 1,
                   StringIO_BOM_Little                   = 2,
                   StringIO_BOM_Big                      = 3,
    } StringIO_BOMs;
    
    /*!
     @enum         StringIO_TruncationTypes
     @abstract                                           Is this BitInput or BitOutput connected to a File or Socket?
     @constant     TruncationType_Unspecified            Unknown TrimString command.
     @constant     TruncationType_All                    Trim at the beginning and end, removing all occurrences found there.
     @constant     TruncationType_Most1                  Trim between start and end, removing all but 1 occurrence found between non-removable CodePoints
     */
    typedef enum StringIO_TruncationTypes {
                   TruncationType_Unspecified            = 0,
                   TruncationType_All                    = 1,
                   TruncationType_Most1                  = 2,
    } StringIO_TruncationTypes;
    
    /*!
     @enum         StringIO_WhitespaceTypes
     @abstract                                           Should whitespace be ignored or not?
     @constant     WhitespaceType_Unspecified            Unknown WhiteSpace type.
     @constant     WhitespaceType_Insignificant          Ignore whitespace.
     @constant     WhitespaceType_Significant            Whitespace matters, don't ignore it.
     */
    typedef enum StringIO_WhitespaceTypes {
                   WhitespaceType_Unspecified            = 0,
                   WhitespaceType_Insignificant          = 1,
                   WhitespaceType_Significant            = 2,
    } StringIO_WhitespaceTypes;

    /*!
     @enum         StringIO_LineBreakTypes
     @abstract                                           What kind of whitespace is this?
     @constant     LineBreakType_Unspecified             Unknown LineBreak.
     @constant     LineBreakType_LineFeed                Line Feed aka 0xA '\n'
     @constant     LineBreakType_VerticalTab             Vertical Tab aka 0xB '\v'
     @constant     LineBreakType_FormFeed                Form Feed aka 0xC '\f'
     @constant     LineBreakType_CarriageReturn          Carriage Return aka 0xD '\r'
     @constant     LineBreakType_CRLF                    CR+LF aka 0xDA '\r\n'
     @constant     LineBreakType_LFCR                    LF+CR aka 0xAD '\n\r'
     @constant     LineBreakType_NextLine                Next Line aka 0x85 '\u0085'
     @constant     LineBreakType_LineSeparator           Line Seperator aka 0x2028 '\u2028'
     @constant     LineBreakType_ParagraphSeparator      Paragraph Seperator aka 0x2029 '\u2029'
     */
    typedef enum StringIO_LineBreakTypes {
                   LineBreakType_Unspecified             = 0,
                   LineBreakType_LineFeed                = 1,
                   LineBreakType_VerticalTab             = 2,
                   LineBreakType_FormFeed                = 3,
                   LineBreakType_CarriageReturn          = 4,
                   LineBreakType_CRLF                    = 5,
                   LineBreakType_LFCR                    = 6,
                   LineBreakType_NextLine                = 7,
                   LineBreakType_LineSeparator           = 8,
                   LineBreakType_ParagraphSeparator      = 9,
    } StringIO_LineBreakTypes;

    /*!
     @enum         CodePointClass
     @abstract                                           Should whitespace be ignored or not?
     @constant     CodePointClass_Unspecified            Unknown TrimString command.
     */
    typedef enum CodePointClass {
        CodePointClass_Unspecified                       = 0,
        CodePointClass_CCCOverlay                        = 1,
        CodePointClass_CCCNukta                          = 7,
        CodePointClass_CCCKanaVoicing                    = 8,
        CodePointClass_CCCVirama                         = 9,
        CodePointClass_CCC10                             = 10,
        CodePointClass_CCC11                             = 11,
        CodePointClass_CCC12                             = 12,
        CodePointClass_CCC13                             = 13,
        CodePointClass_CCC14                             = 14,
        CodePointClass_CCC15                             = 15,
        CodePointClass_CCC16                             = 16,
        CodePointClass_CCC17                             = 17,
        CodePointClass_CCC18                             = 18,
        CodePointClass_CCC19                             = 19,
        CodePointClass_CCC20                             = 20,
        CodePointClass_CCC21                             = 21,
        CodePointClass_CCC22                             = 22,
        CodePointClass_CCC23                             = 23,
        CodePointClass_CCC24                             = 24,
        CodePointClass_CCC25                             = 25,
        CodePointClass_CCC26                             = 26,
        CodePointClass_CCC27                             = 27,
        CodePointClass_CCC28                             = 28,
        CodePointClass_CCC29                             = 29,
        CodePointClass_CCC30                             = 30,
        CodePointClass_CCC31                             = 31,
        CodePointClass_CCC32                             = 32,
        CodePointClass_CCC33                             = 33,
        CodePointClass_CCC34                             = 34,
        CodePointClass_CCC35                             = 35,
        CodePointClass_CCC36                             = 36,
        CodePointClass_CCC84                             = 84,
        CodePointClass_CCC91                             = 91,
        CodePointClass_CCC103                            = 103,
        CodePointClass_CCC107                            = 107,
        CodePointClass_CCC118                            = 118,
        CodePointClass_CCC122                            = 122,
        CodePointClass_CCC129                            = 129,
        CodePointClass_CCC130                            = 130,
        CodePointClass_CCC132                            = 132,
        CodePointClass_CCCAttachBelowLeft                = 200,
        CodePointClass_CCCAttachBelow                    = 202,
        CodePointClass_CCCAttachAbove                    = 214,
        CodePointClass_CCCAttachAboveRight               = 216,
        CodePointClass_CCCAttachBelowLeft2               = 218,
        CodePointClass_CCCAttachBelow2                   = 220,
        CodePointClass_CCCAttachBelowRight2              = 222,
        CodePointClass_CCCAttachLeft                     = 224,
        CodePointClass_CCCAttachRight                    = 226,
        CodePointClass_CCCAttachAboveLeft2               = 228,
        CodePointClass_CCCAttachAbove2                   = 230,
        CodePointClass_CCCAttachAboveRight2              = 232,
        CodePointClass_CCCAttachDoubleBelow              = 233,
        CodePointClass_CCCAttachDoubleAbove              = 234,
        CodePointClass_CCCAttachIOTASubscript            = 240,
    } CodePointClass;

    /*!
     @abstract                                           Creates a UTF8 string plus a NULL terminator.
     @param            NumCodeUnits                      The size of the string not counting the NULL terminator.
     */
    UTF8              *UTF8_Init(size_t NumCodeUnits);
    
    /*!
     @abstract                                           Creates a UTF16 string plus a NULL terminator.
     @param            NumCodeUnits                      The size of the string not counting the NULL terminator.
     */
    UTF16             *UTF16_Init(size_t NumCodeUnits);
    
    /*!
     @abstract                                           Creates a UTF32 string plus a NULL terminator.
     @param            NumCodePoints                     The size of the string not counting the NULL terminator.
     */
    UTF32             *UTF32_Init(size_t NumCodePoints);

    /*!
     @abstract                                           Hashes a string, useful for using switch statements with strings.
     @param            String                            The string to hash.
     @return                                             Returns the hash of the string.
     */
    uint64_t           UTF8_Hash(UTF8 *String);

    /*!
     @abstract                                           Hashes a string, useful for using switch statements with strings.
     @param            String                            The string to hash.
     @return                                             Returns the hash of the string.
     */
    uint64_t           UTF16_Hash(UTF16 *String);

    /*!
     @abstract                                           Hashes a string, useful for using switch statements with strings.
     @param            String                            The string to hash.
     @return                                             Returns the hash of the string.
     */
    uint64_t           UTF32_Hash(UTF32 *String);
    
    /*!
     @abstract                                           Sets NumCodeUnits to Value.
     @param            String                            The string to set.
     @param            Value                             The value to set each CodeUnit to.
     @param            NumCodeUnits                      The size of the string not counting the NULL terminator.
     */
    void               UTF8_Set(UTF8 *String, UTF8 Value, size_t NumCodeUnits);
    
    /*!
     @abstract                                           Sets NumCodeUnits to Value.
     @param            String                            The string to set.
     @param            Value                             The value to set each CodeUnit to.
     @param            NumCodeUnits                      The size of the string not counting the NULL terminator.
     */
    void               UTF16_Set(UTF16 *String, UTF16 Value, size_t NumCodeUnits);
    
    /*!
     @abstract                                           Sets NumCodeUnits to Value.
     @param            String                            The string to set.
     @param            Value                             The value to set each CodePoint to.
     @param            NumCodePoints                     The size of the string not counting the NULL terminator.
     */
    void               UTF32_Set(UTF32 *String, UTF32 Value, size_t NumCodePoints);
    
    /*!
     @abstract                                           Returns the number of CodeUnits in the CodePoint.
     @param            CodeUnit                          The leading CodeUnit for a CodePoint.
     */
    uint8_t            UTF8_GetCodePointSizeInCodeUnits(UTF8 CodeUnit);
    
    /*!
     @abstract                                           Returns the number of CodeUnits in the CodePoint.
     @param            CodeUnit                          A CodeUnit for a CodePoint.
     */
    uint8_t            UTF16_GetCodePointSizeInCodeUnits(UTF16 CodeUnit);
    
    /*!
     @abstract                                           Decodes a CodePoint from UTF-8 CodeUnits.
     @param            CodeUnits                         Must start at a leading codeunit and be followed by at least as many trailing codeunits as indicitated by the leading code unit.
     */
    UTF32              UTF8_ExtractCodePoint(ImmutableString_UTF8 CodeUnits);
    
    /*!
     @abstract                                           Decodes a CodePoint from UTF-16 CodeUnits.
     @param            CodeUnits                         Must start at a leading codeunit and be followed by at least as many trailing codeunits as indicitated by the leading code unit.
     */
    UTF32              UTF16_ExtractCodePoint(ImmutableString_UTF16 CodeUnits);
    
    /*!
     @abstract                                           Gets the number of Unicode codeunits in the UTF8 string.
     @remark                                             This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings.
     @param            String                            The string to get the number of codeunits in.
     */
    size_t             UTF8_GetStringSizeInCodeUnits(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Gets the number of Unicode codeunits in the UTF8.
     @remark                                             This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings.
     @param            String                            The string to get the number of CodePoints in.
     */
    size_t             UTF16_GetStringSizeInCodeUnits(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Gets the number of Unicode CodePoints in the string.
     @remark                                             This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings.
     @param            String                            The string to get the number of CodePoints in.
     */
    size_t             UTF8_GetStringSizeInCodePoints(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Gets the number of Unicode CodePoints in the string.
     @remark                                             This function is optimized to skip over continuation code units, and will fail MISERABLY with invalid strings.
     @param            String                            The string to get the number of CodePoints in.
     */
    size_t             UTF16_GetStringSizeInCodePoints(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Gets the number of Unicode CodePoints in the string.
     @param            String                            The string to get the number of CodePoints in.
     */
    size_t             UTF32_GetStringSizeInCodePoints(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Gets the number of user visible characters in a UTF-8 string.
     @remark                                             This function just decodes the string and sends it off to the UTF32 version.
     @param            String                            The string to get the number of graphemes in.
     */
    size_t             UTF8_GetStringSizeInGraphemes(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Gets the number of user visible characters in a UTF-16 string.
     @remark                                             This function just decodes the string and sends it off to the UTF32 version.
     @param            String                            The string to get the number of graphemes in.
     */
    size_t             UTF16_GetStringSizeInGraphemes(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Gets the number of user visible characters in a UTF-32 string.
     @param            String                            The string to get the number of graphemes in.
     */
    size_t             UTF32_GetStringSizeInGraphemes(ImmutableString_UTF32 String);

    /*!
     @abstract                                           Gets the number of codepoints until a wordbreak is found.
     @param            String                            The String the operate on.
     @return                                             The number of CodePoints before the word break.
     */
    size_t             UTF8_GetWordSizeInCodePoints(ImmutableString_UTF8 String);

    /*!
     @abstract                                           Gets the number of codepoints until a wordbreak is found.
     @param            String                            The String the operate on.
     @return                                             The number of CodePoints before the word break.
     */
    size_t             UTF16_GetWordSizeInCodePoints(ImmutableString_UTF16 String);

    /*!
     @abstract                                           Gets the number of wordbreaks until a non-wordbreak is found.
     @param            String                            The String the operate on.
     @return                                             The number of CodePoints in the word break.
     */
    size_t             UTF8_GetWordBreakSizeInCodePoints(ImmutableString_UTF8 String);

    /*!
     @abstract                                           Gets the number of wordbreaks until a non-wordbreak is found.
     @param            String                            The String the operate on.
     @return                                             The number of CodePoints in the word break.
     */
    size_t             UTF16_GetWordBreakSizeInCodePoints(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Converts yes/no/true/false/on/off/1/0 to true or false.
     @param            String                            The string to convert to a bool.
     */
    bool               UTF8_String2Bool(UTF8 *String);
    
    /*!
     @abstract                                           Converts yes/no/true/false/on/off/1/0 to true or false.
     @param            String                            The string to convert to a bool.
     */
    bool               UTF16_String2Bool(UTF16 *String);
    
    /*!
     @abstract                                           Converts yes/no/true/false/on/off/1/0 to true or false.
     @param            String                            The string to convert to a bool.
     */
    bool               UTF32_String2Bool(UTF32 *String);
    
    /*!
     @abstract                                           Tells if the string pointed to by String has "//?/" right after the BOM, if it exists.
     @param            String                            The string to check.
     @return                                             Returns Yes if the string contains "//?/", otherwise it returns No.
     */
    bool               UTF8_IsUNCPath(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Tells if the string pointed to by String has "//?/" right after the BOM, if it exists.
     @param            String                            The string to check.
     @return                                             Returns Yes if the string contains "//?/", otherwise it returns No.
     */
    bool               UTF16_IsUNCPath(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Tells if the string pointed to by String has "//?/" right after the BOM, if it exists.
     @param            String                            The string to check.
     @return                                             Returns Yes if the string contains "//?/", otherwise it returns No.
     */
    bool               UTF32_IsUNCPath(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Tells if the String starts with '/' on POSIX, or the second character is ':' on Windows.
     @param            String                            The string to check.
     @return                                             Returns Yes if the string starts with '/' or the second character is ':' on Windows, otherwise it returns No.
     */
    bool               UTF8_IsAbsolutePath(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Tells if the String starts with '/' on POSIX, or the second character is ':' on Windows.
     @param            String                            The string to check.
     @return                                             Returns Yes if the string starts with '/' or the second character is ':' on Windows, otherwise it returns No.
     */
    bool               UTF16_IsAbsolutePath(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Tells if the String starts with '/' on POSIX, or the second character is ':' on Windows.
     @param            String                            The string to check.
     @return                                             Returns Yes if the string starts with '/' or the second character is ':' on Windows, otherwise it returns No.
     */
    bool               UTF32_IsAbsolutePath(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Tells if the string pointed to by String contains a Windows or UNIX style line ending.
     @param            String                            The string to check.
     @return                                             Returns Yes if the String contains a Windows or UNIX style line ending, otherwise no.
     */
    bool               UTF8_HasNewLine(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Tells if the string pointed to by String contains a Windows or UNIX style line ending.
     @param            String                            The string to check.
     @return                                             Returns Yes if the String contains a Windows or UNIX style line ending, otherwise no.
     */
    bool               UTF16_HasNewLine(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Tells if the string pointed to by String contains a Windows or UNIX style line ending.
     @param            String                            The string to check.
     @return                                             Returns Yes if the String contains a Windows or UNIX style line ending, otherwise no.
     */
    bool               UTF32_HasNewLine(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Tells if the CodePoint is Uppercase; Unicode replacement of isupper
     @param            CodePoint                         The string to check.
     @return                                             Returns Yes if the CodePoint represents an uppercase character, otherwise No.
     */
    bool               UTF32_IsUpperCase(UTF32 CodePoint);
    
    /*!
     @abstract                                           Tells if the UTF-8 string pointed to by String is a valid UTF-8 encoded string.
     @param            String                            The string to get the validity status from.
     @return                                             Returns Yes if the string is valid, otherwise it returns No.
     */
    bool               UTF8_IsValid(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Tells if the UTF-16 string pointed to by String is a valid UTF-16 encoded string.
     @param            String                            The string to get the validity status from.
     @return                                             Returns Yes if the string is valid, otherwise it returns No.
     */
    bool               UTF16_IsValid(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Tells if the UTF-32 string pointed to by String is a valid UTF-32 encoded string.
     @param            String                            The string to get the validity status from.
     @return                                             Returns Yes if the string is valid, otherwise it returns No.
     */
    bool               UTF32_IsValid(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Tells if the UTF-8 string pointed to by String has a Byte Order Mark at the beginning.
     @param            String                            The string to get the BOM status from.
     @return                                             Returns Yes if the string contains a BOM, otherwise it returns No.
     */
    bool               UTF8_HasBOM(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Tells if the UTF-16 string pointed to by String has a Byte Order Mark at the beginning.
     @param            String                            The string to get the BOM status from.
     @return                                             Returns Yes if the string contains a BOM, otherwise it returns No.
     */
    bool               UTF16_HasBOM(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Tells if the UTF-32 string pointed to by String has a Byte Order Mark at the beginning.
     @param            String                            The string to get the BOM status from.
     @return                                             Returns Yes if the string contains a BOM, otherwise it returns No.
     */
    bool               UTF32_HasBOM(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Is this codepoint a wordbreak.
     @param            CodePoint                         The CodePoint to get the status from.
     @return                                             Returns Yes if the CodePoint is a wordbreak, otherwise it returns No.
     */
    bool               UTF8_IsWordBreak(const UTF8 *CodePoint);
    
    /*!
     @abstract                                           Is this codepoint a wordbreak.
     @param            CodePoint                         The CodePoint to get the status from.
     @return                                             Returns Yes if the CodePoint is a wordbreak, otherwise it returns No.
     */
    bool               UTF16_IsWordBreak(const UTF16 CodePoint);
    
    /*!
     @abstract                                           Is this codepoint a wordbreak.
     @param            CodePoint                         The CodePoint to get the status from.
     @return                                             Returns Yes if the CodePoint is a wordbreak, otherwise it returns No.
     */
    bool               UTF32_IsWordBreak(const UTF32 CodePoint);
    
    /*!
     @abstract                                           Adds the BOM to the UTF-8 string, UTF-8's only valid BOM is BE.
     @param            String                            The string to add the BOM to.
     */
    UTF8              *UTF8_AddBOM(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Adds the specified BOM to the string.
     @param            String                            The string to add the BOM to.
     */
    UTF16             *UTF16_AddBOM(ImmutableString_UTF16 String, StringIO_BOMs BOM2Add);
    
    /*!
     @abstract                                           Adds the specified BOM to the string.
     @param            String                            The string to add the BOM to.
     */
    UTF32             *UTF32_AddBOM(ImmutableString_UTF32 String, StringIO_BOMs BOM2Add);
    
    /*!
     @abstract                                           Removes the BOM from the string.
     @param            String                            The string to remove the BOM from.
     */
    UTF8              *UTF8_RemoveBOM(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Removes the BOM from the string.
     @param            String                            The string to remove the BOM from.
     */
    UTF16             *UTF16_RemoveBOM(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Removes the BOM from the string.
     @param            String                            The string to remove the BOM from.
     */
    UTF32             *UTF32_RemoveBOM(ImmutableString_UTF32 String);
    /* Basic String Property Functions */
    
    /*!
     @abstract                                           Decodes a UTF8 string to a UTF32 string.
     @param            String                            The string to decode.
     */
    UTF32             *UTF8_Decode(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Decodes a UTF16 string to a UTF32 string.
     @param            String                            The string to decode.
     */
    UTF32             *UTF16_Decode(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Encodes a UTF32 string to a UTF8 string.
     @param            String                            The UTF32 string to encode to a UTF8 string.
     */
    UTF8              *UTF8_Encode(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Encodes a UTF32 string to a UTF16 string.
     @param            String                            The string to encode.
     */
    UTF16             *UTF16_Encode(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Converts a UTF8 string to a UTF16 string.
     @param            String                            The string to convert.
     */
    UTF16             *UTF8_Convert(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Converts a UTF16 string to a UTF8 string.
     @param            String                            The string to convert.
     */
    UTF8              *UTF16_Convert(ImmutableString_UTF16 String);

    /* TextIOTables Operations */
    /*!
     @abstract                                           Checks if a given CodePoint is in the specified Table from TextIO.
     @param            Table                             The table to check.
     @param            TableSize                         The size of the table.
     @param            CodePoint                         The codepoint to check.
     */
    bool               UTF32_IsCodePointInTable(const UTF32 *Table, size_t TableSize, UTF32 CodePoint);

    /*!
     @abstract                                           Get the CodePoint's CombiningCharacterClass value.
     @param            CodePoint                         The codepoint to check.
     */
    CodePointClass     UTF32_GetCharacterClassOfCodePoint(UTF32 CodePoint);

    /*!
     @abstract                                           Get the CodePoint's integer value; if not an integer returns 0.
     @param            CodePoint                         The codepoint to check.
     */
    int64_t            UTF32_GetIntegerValueOfCodePoint(UTF32 CodePoint);

    /*!
     @abstract                                           Get the CodePoint's decimal value; Specialized for `float`.
     @param            CodePoint                         The codepoint to check.
     */
    float              UTF32_GetDecimalValueOfCodePoint32(UTF32 CodePoint);

    /*!
     @abstract                                           Get the CodePoint's decimal value; Specialized for `double`.
     @param            CodePoint                         The codepoint to check.
     */
    double             UTF32_GetDecimalValueOfCodePoint64(UTF32 CodePoint);

    /*!
     @abstract                                           Do a string search on the casefold/normalization tables.
     @param            Table                             CaseFold/Normalizaton table.
     @param            TableSize                         The number of entries in the table.
     @param            String2Replace                    The codepoint to check.
     @param            String2ReplaceSize                The size of the string2replace, makes lookup way faster.
     */
    UTF32             *UTF32_GetReplacementStringFromTable(UTF32 *Table, size_t TableSize, UTF32 *String2Replace, size_t String2ReplaceSize);
    /*!
     @abstract                                           Casefolds string for case insensitive comparison.
     @remark                                             Uppercase CodePoints are connverted to lowercase for the comparison
     @param            String                            The string to be casefolded.
     @return                                             Returns the case folded string.
     */
    UTF8              *UTF8_CaseFold(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Casefolds string for case insensitive comparison.
     @remark                                             Uppercase CodePoints are connverted to lowercase for the comparison
     @param            String                            The string to be casefolded.
     @return                                             Returns the case folded string.
     */
    UTF16             *UTF16_CaseFold(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Casefolds string for case insensitive comparison.
     @remark                                             Uppercase CodePoints are connverted to lowercase for the comparison
     @param            String                            The string to be casefolded.
     @return                                             Returns the case folded string.
     */
    UTF32             *UTF32_CaseFold(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Converts string to use precomposed forms, otherwise it orders the combining CodePoints in lexiographic order.
     @remark                                             The string is reallocated at the end to remove unused space.
     @remark                                             This function simply decodes the string, sends it to the UTF32 version, then reencodes it.
     @param            String                            The string to be normalized.
     @param            NormalizedForm                    The type of normalization to use on the String.
     */
    UTF8              *UTF8_Normalize(ImmutableString_UTF8 String, StringIO_NormalizationForms NormalizedForm);
    
    /*!
     @abstract                                           Converts string to use precomposed forms, otherwise it orders the combining CodePoints in lexiographic order.
     @remark                                             The string is reallocated at the end to remove unused space.
     @remark                                             This function simply decodes the string, sends it to the UTF32 version, then reencodes it.
     @param            String                            The string to be normalized.
     @param            NormalizedForm                    The type of normalization to use on the String.
     */
    UTF16             *UTF16_Normalize(ImmutableString_UTF16 String, StringIO_NormalizationForms NormalizedForm);
    
    /*!
     @abstract                                           Converts string to use precomposed forms, otherwise it orders the combining CodePoints in lexiographic order.
     @remark                                             The string is reallocated at the end to remove unused space.
     @param            String                            The string to be normalized.
     @param            NormalizedForm                    The type of normalization to use on the String.
     */
    UTF32             *UTF32_Normalize(ImmutableString_UTF32 String, StringIO_NormalizationForms NormalizedForm);
    /* TextIOTables Operations */
    
    /*!
     @abstract                                           Extracts a Grapheme from String.
     @param            String                            The string to extract from.
     @param            Grapheme                          The Grapheme Index to start extracting from.
     */
    UTF8              *UTF8_ExtractGrapheme(ImmutableString_UTF8 String, size_t Grapheme);
    
    /*!
     @abstract                                           Extracts a Grapheme from String.
     @param            String                            The string to extract from.
     @param            Grapheme                          The Grapheme Index to start extracting from.
     */
    UTF16             *UTF16_ExtractGrapheme(ImmutableString_UTF16 String, size_t Grapheme);
    
    /*!
     @abstract                                           Extracts a Grapheme from String.
     @param            String                            The string to extract from.
     @param            Grapheme                          The Grapheme Index to start extracting from.
     */
    UTF32             *UTF32_ExtractGrapheme(ImmutableString_UTF32 String, size_t Grapheme);

    /*!
     @abstract                                           Moves CodePoints to the right
     @remark                                             Always start at the end of the string and work towards the beginning to reduce workload.
     @remark                                             This function is low level, do not use with untrusted input.
     @param            String                            The string to shift.
     @param            StringSize                        The number of CodePoints usefully occupied.
     @param            StringCapacity                    The actual allocated size of the string, not counting NULLTerminator.
     @param            Start                             Where should we start moving the CodePoints from?
     @param            NumCodePointsToShift              How far should Start (and all following CodePoints) be moved?
     @eturn                                              Returns true if the shift was sucessful, false otherwise.
     */
    bool               UTF32_ShiftCodePoints(UTF32 *String, size_t StringSize, size_t StringCapacity, size_t Start, size_t NumCodePointsToShift);

    /*!
     @abstract                                           Replaces String[Start] .. String[SizeOf(Replacement)] with replacement.
     @param            String                            The string to modify.
     @param            Replacement                       What to be replaced with.
     @param            Start                             Where should we start replacing the CodePoints?
     @eturn                                              Returns true if the replacement was sucessful, false otherwise.
     */
    bool               UTF32_ReplaceInPlace(UTF32 *String, const UTF32 *const Replacement, size_t Start);
    
    /*!
     @abstract                                           Compares String1 to String2.
     @param            String1                           String1 for comparison.
     @param            String2                           String2 for comparison.
     @return                                             Returns true if the strings match exactly, otherwise false.
     */
    bool               UTF8_Compare(ImmutableString_UTF8 String1, ImmutableString_UTF8 String2);
    
    /*!
     @abstract                                           Compares String1 to String2.
     @param            String1                           String1 for comparison.
     @param            String2                           String2 for comparison.
     @return                                             Returns true if the strings match exactly, otherwise false.
     */
    bool               UTF16_Compare(ImmutableString_UTF16 String1, ImmutableString_UTF16 String2);
    
    /*!
     @abstract                                           Compares String1 to String2.
     @param            String1                           String1 for comparison.
     @param            String2                           String2 for comparison.
     @return                                             Returns true if the strings match exactly, otherwise false.
     */
    bool               UTF32_Compare(ImmutableString_UTF32 String1, ImmutableString_UTF32 String2);
    
    /*!
     @abstract                                           Creates a String composed of Padding.
     @param            Padding                           The string to create the string with.
     @param            Times2Pad                         The number of times for Padding to be duplicated.
     @return                                             Returns a new string containing Padding * Times2Pad.
     */
    UTF8              *UTF8_Create(ImmutableString_UTF8 Padding, size_t Times2Pad);
    
    /*!
     @abstract                                           Creates a String composed of Padding.
     @param            Padding                           The string to create the string with.
     @param            Times2Pad                         The number of times for Padding to be duplicated.
     @return                                             Returns a new string containing Padding * Times2Pad.
     */
    UTF16             *UTF16_Create(ImmutableString_UTF16 Padding, size_t Times2Pad);
    
    /*!
     @abstract                                           Creates a String composed of Padding.
     @param            Padding                           The string to create the string with.
     @param            Times2Pad                         The number of times for Padding to be duplicated.
     @return                                             Returns a new string containing Padding * Times2Pad.
     */
    UTF32             *UTF32_Create(ImmutableString_UTF32 Padding, size_t Times2Pad);
    
    /*!
     @abstract                                           Finds a substring within string, starting at CodePoint Offset, and ending at Offset + Length.
     @remark                                             We do NOT casefold, or normalize the String or SubString, that's your job.
     @param            String                            The string to search for SubString in.
     @param            SubString                         The SubString to find in String.
     @param            Offset                            Where in the string should we start looking for the substring?
     @param            Length                            How many CodePoints should we search for the substring? -1 means all CodePoints.
     @return                                             Returns the offset of the start of the substring in String, or -1 if a match wasn't found..
     */
    size_t             UTF8_FindSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Finds a substring within string, starting at CodePoint Offset, and ending at Offset + Length.
     @remark                                             We do NOT casefold, or normalize the String or SubString, that's your job.
     @param            String                            The string to search for SubString in.
     @param            SubString                         The SubString to find in String.
     @param            Offset                            Where in the string should we start looking for the substring?
     @param            Length                            How many CodePoints should we search for the substring? -1 means all CodePoints.
     @return                                             Returns the offset of the start of the substring in String, or -1 if a match wasn't found..
     */
    size_t             UTF16_FindSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Finds a substring within string, starting at CodePoint Offset, and ending at Offset + Length.
     @remark                                             We do NOT casefold, or normalize the String or SubString, that's your job.
     @param            String                            The string to search for SubString in.
     @param            SubString                         The SubString to find in String.
     @param            Offset                            Where in the string should we start looking for the substring?
     @param            Length                            How many CodePoints should we search for the substring? -1 means all CodePoints.
     @return                                             Returns the offset of the start of the substring in String, or -1 if a match wasn't found..
     */
    size_t             UTF32_FindSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Extracts a SubString from String.
     @param            String                            The string to extract from.
     @param            Offset                            The CodePoint to start extracting from.
     @param            NumCodeUnits                      The number of CodePoints to extract.
     */
    UTF8              *UTF8_ExtractSubString(ImmutableString_UTF8 String, size_t Offset, size_t NumCodeUnits);
    
    /*!
     @abstract                                           Extracts a SubString from String.
     @param            String                            The string to extract from.
     @param            Offset                            The CodePoint to start extracting from.
     @param            NumCodeUnits                      The number of CodePoints to extract.
     */
    UTF16             *UTF16_ExtractSubString(ImmutableString_UTF16 String, size_t Offset, size_t NumCodeUnits);
    
    /*!
     @abstract                                           Extracts a SubString from String.
     @param            String                            The string to extract from.
     @param            Offset                            The CodePoint to start extracting from.
     @param            NumCodePoints                     The number of CodePoints to extract.
     */
    UTF32             *UTF32_ExtractSubString(ImmutableString_UTF32 String, size_t Offset, size_t NumCodePoints);
    
    /*!
     @abstract                                           Substitutes a section in String starting at Offset and ending at Offset + Length with Replacement.
     @param            String                            The string to edit.
     @param            Substitution                      The string to splice in.
     @param            Offset                            Where to start replacing String with Substituion.
     @param            Length                            The number of CodePoints to substitute, can be more or less than Substituion.
     */
    UTF8              *UTF8_SubstituteSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substitution, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Substitutes a section in String starting at Offset and ending at Offset + Length with Replacement.
     @param            String                            The string to edit.
     @param            Substitution                      The string to splice in.
     @param            Offset                            Where to start replacing String with Substituion.
     @param            Length                            The number of CodePoints to substitute, can be more or less than Substituion.
     */
    UTF16             *UTF16_SubstituteSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substitution, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Substitutes a section in String starting at Offset and ending at Offset + Length with Replacement.
     @param            String                            The string to edit.
     @param            Substitution                      The string to splice in.
     @param            Offset                            Where to start replacing String with Substituion.
     @param            Length                            The number of CodePoints to substitute, can be more or less than Substituion.
     */
    UTF32             *UTF32_SubstituteSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substitution, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Reallocates String and copies it except for the CodePoints between Offset and Length (inclusive).
     @param            String                            The string to operate on.
     @param            Offset                            The first CodePoint to remove.
     @param            Length                            The last CodePoint to remove minus Offset.
     */
    UTF8              *UTF8_StitchSubString(ImmutableString_UTF8 String, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Reallocates String and copies it except for the CodePoints between Offset and Length (inclusive).
     @param            String                            The string to operate on.
     @param            Offset                            The first CodePoint to remove.
     @param            Length                            The last CodePoint to remove minus Offset.
     */
    UTF16             *UTF16_StitchSubString(ImmutableString_UTF16 String, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Reallocates String and copies it except for the CodePoints between Offset and Length (inclusive).
     @param            String                            The string to operate on.
     @param            Offset                            The first CodePoint to remove.
     @param            Length                            The last CodePoint to remove minus Offset.
     */
    UTF32             *UTF32_StitchSubString(ImmutableString_UTF32 String, size_t Offset, size_t Length);
    
    /*!
     @abstract                                           Reallocates String and copies it except for the instance (-1 for all instances) of the substring.
     @param            String                            The string to operate on.
     @param            SubString2Remove                  The substring to remove from the string.
     @param            Instance2Remove                   The instance (0 for all) of the substring in the string to remove.
     */
    UTF8              *UTF8_RemoveSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 SubString2Remove, size_t Instance2Remove);
    
    /*!
     @abstract                                           Reallocates String and copies it except for the instance (-1 for all instances) of the substring.
     @param            String                            The string to operate on.
     @param            SubString2Remove                  The substring to remove from the string.
     @param            Instance2Remove                   The instance (0 for all) of the substring in the string to remove.
     */
    UTF16             *UTF16_RemoveSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 SubString2Remove, size_t Instance2Remove);
    
    /*!
     @abstract                                           Reallocates String and copies it except for the instance (-1 for all instances) of the substring.
     @param            String                            The string to operate on.
     @param            SubString2Remove                  The substring to remove from the string.
     @param            Instance2Remove                   The instance (0 for all) of the substring in the string to remove.
     */
    UTF32             *UTF32_RemoveSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 SubString2Remove, size_t Instance2Remove);
    
    /*!
     @abstract                                           Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence.
     @remark                                             The caller needs to handle casefolding and normalization.
     @param            String                            The string to check.
     @param            Substring                         The substring to check.
     @return                                             Returns whether Substring matches at the given offsets.
     */
    bool               UTF8_CompareSubString(ImmutableString_UTF8 String, ImmutableString_UTF8 Substring, size_t StringOffset, size_t SubstringOffset);
    
    /*!
     @abstract                                           Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence.
     @remark                                             The caller needs to handle casefolding and normalization.
     @param            String                            The string to check.
     @param            Substring                         The substring to check.
     @return                                             Returns whether Substring matches at the given offsets.
     */
    bool               UTF16_CompareSubString(ImmutableString_UTF16 String, ImmutableString_UTF16 Substring, size_t StringOffset, size_t SubstringOffset);
    
    /*!
     @abstract                                           Compares String1 at StringOffset and Substring at SubstringOffset until the end of String or Substring for equivalence.
     @remark                                             The caller needs to handle casefolding and normalization.
     @param            String                            The string to check.
     @param            Substring                         The substring to check.
     @return                                             Returns whether Substring matches at the given offsets.
     */
    bool               UTF32_CompareSubString(ImmutableString_UTF32 String, ImmutableString_UTF32 Substring, size_t StringOffset, size_t SubstringOffset);
    
    /*!
     @abstract                                           Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process.
     @remark                                             Replaces strtok from the standard library.
     @param            String                            The string you want to be split.
     @param            Delimiters                        An StringSet containing the delimiters, one delimiter per string.
     */
    UTF8             **UTF8_Split(ImmutableString_UTF8 String, ImmutableStringSet_UTF8 Delimiters);
    
    /*!
     @abstract                                           Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process.
     @remark                                             Replaces strtok from the standard library.
     @param            String                            The string you want to be split.
     @param            Delimiters                        An StringSet containing the delimiters, one delimiter per string.
     */
    UTF16            **UTF16_Split(ImmutableString_UTF16 String, ImmutableStringSet_UTF16 Delimiters);
    
    /*!
     @abstract                                           Splits string into X substrings at delimiters, removing any delimiters found from the substrings in the process.
     @remark                                             Replaces strtok from the standard library.
     @param            String                            The string you want to be split.
     @param            Delimiters                        An StringSet containing the delimiters, one delimiter per string.
     */
    UTF32            **UTF32_Split(ImmutableString_UTF32 String, ImmutableStringSet_UTF32 Delimiters);
    
    /*!
     @abstract                                           Converts a string to an integer; replaces atoi, atol, strtol, strtoul.
     @remark                                             If the number can be negatiev, check for a minus before extracting the digits as a substring.
     @param            Base                              The base to output the integer in.
     @param            String                            The string to extract a number from.
     */
    int64_t            UTF8_String2Integer(TextIO_Bases Base, ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Converts a string to an integer; replaces atoi, atol, strtol, strtoul.
     @remark                                             If the number can be negatiev, check for a minus before extracting the digits as a substring.
     @param            Base                              The base to output the integer in.
     @param            String                            The string to extract a number from.
     */
    int64_t            UTF16_String2Integer(TextIO_Bases Base, ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Converts a string to an integer; replaces atoi, atol, strtol, strtoul.
     @remark                                             If the number can be negatiev, check for a minus before extracting the digits as a substring.
     @param            Base                              The base to output the integer in.
     @param            String                            The string to extract a number from.
     */
    int64_t            UTF32_String2Integer(TextIO_Bases Base, ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Converts an integer to a string; replaces itoa.
     @param            Base                              The base to output the integer in.
     @param            Integer2Convert                   The number to convert into a string.
     */
    UTF8              *UTF8_Integer2String(TextIO_Bases Base, int64_t Integer2Convert);
    
    /*!
     @abstract                                           Converts an integer to a string; replaces itoa.
     @param            Base                              The base to output the integer in.
     @param            Integer2Convert                   The number to convert into a string.
     */
    UTF16             *UTF16_Integer2String(TextIO_Bases Base, int64_t Integer2Convert);
    
    /*!
     @abstract                                           Converts an integer to a string; replaces itoa.
     @param            Base                              The base to output the integer in.
     @param            Integer2Convert                   The number to convert into a string.
     */
    UTF32             *UTF32_Integer2String(TextIO_Bases Base, int64_t Integer2Convert);
    
    /*!
     @abstract                                           Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l.
     @param            Base                              The radix to convert the Decimal into.
     @param            String                            The string composed of a decimal number to convert to a decimal.
     */
    double             UTF8_String2Decimal(TextIO_Bases Base, ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l.
     @param            Base                              The radix to convert the Decimal into.
     @param            String                            The string composed of a decimal number to convert to a decimal.
     */
    double             UTF16_String2Decimal(TextIO_Bases Base, ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Converts a string to a double; replaces strtod, strtof, strold, atof, and atof_l.
     @param            Base                              The radix to convert the Decimal into.
     @param            String                            The string composed of a decimal number to convert to a decimal.
     */
    double             UTF32_String2Decimal(TextIO_Bases Base, ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Converts a double to a string; replaces dtostr.
     @param            Base                              The radix to convert the Decimal into.
     @param            Decimal                           The decimal number to convert to a string.
     */
    UTF8              *UTF8_Decimal2String(TextIO_Bases Base, double Decimal);
    
    /*!
     @abstract                                           Converts a double to a string; replaces dtostr.
     @param            Base                              The radix to convert the Decimal into.
     @param            Decimal                           The decimal number to convert to a string.
     */
    UTF16             *UTF16_Decimal2String(TextIO_Bases Base, double Decimal);
    
    /*!
     @abstract                                           Converts a double to a string; replaces dtostr.
     @param            Base                              The radix to convert the Decimal into.
     @param            Decimal                           The decimal number to convert to a string.
     */
    UTF32             *UTF32_Decimal2String(TextIO_Bases Base, double Decimal);
    
    /*!
     @abstract                                           Removes substrings (including single CodePoints) from a string.
     @param            String                            The string to perform the trimming operations on.
     @param            Strings2Remove                    An StringSet to remove from the String.
     */
    UTF8              *UTF8_Trim(ImmutableString_UTF8 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF8 Strings2Remove);
    
    /*!
     @abstract                                           Removes substrings (including single CodePoints) from a string.
     @param            String                            The string to perform the trimming operations on.
     @param            Strings2Remove                    An StringSet to remove from the String.
     */
    UTF16             *UTF16_Trim(ImmutableString_UTF16 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF16 Strings2Remove);
    
    /*!
     @abstract                                           Removes substrings (including single CodePoints) from a string.
     @param            String                            The string to perform the trimming operations on.
     @param            Strings2Remove                    An StringSet to remove from the String.
     */
    UTF32             *UTF32_Trim(ImmutableString_UTF32 String, StringIO_TruncationTypes Type, ImmutableStringSet_UTF32 Strings2Remove);
    
    /*!
     @abstract                                           Strips all instances of Strings2Remove from String.
     @param            String                            The string to perform the strip operation on.
     @param            Strings2Remove                    An StringSet to remove from the String.
     */
    UTF8              *UTF8_Strip(ImmutableString_UTF8 String, ImmutableStringSet_UTF8 Strings2Remove);
    
    /*!
     @abstract                                           Strips all instances of Strings2Remove from String.
     @param            String                            The string to perform the strip operation on.
     @param            Strings2Remove                    An StringSet to remove from the String.
     */
    UTF16             *UTF16_Strip(ImmutableString_UTF16 String, ImmutableStringSet_UTF16 Strings2Remove);
    
    /*!
     @abstract                                           Strips all instances of Strings2Remove from String.
     @param            String                            The string to perform the strip operation on.
     @param            Strings2Remove                    An StringSet to remove from the String.
     */
    UTF32             *UTF32_Strip(ImmutableString_UTF32 String, ImmutableStringSet_UTF32 Strings2Remove);
    
    /*!
     @abstract                                           Copies the String.
     @remark                                             The caller needs to handle casefolding and normalization.
     @param            String                            Pointer to the String to be copied.
     @return                                             Returns A pointer to a copy of the String.
     */
    UTF8              *UTF8_Clone(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Copies the String.
     @remark                                             The caller needs to handle casefolding and normalization.
     @param            String                            Pointer to the String to be copied.
     @return                                             Returns A pointer to a copy of the String.
     */
    UTF16             *UTF16_Clone(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Copies the String.
     @remark                                             The caller needs to handle casefolding and normalization.
     @param            String                            Pointer to the String to be copied.
     @return                                             Returns A pointer to a copy of the String.
     */
    UTF32             *UTF32_Clone(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Securely erases a string.
     @param            String                            Pointer to the String to be erased.
     @param            NewValue                          The value to set each codeunit to while erasing.
     @return                                             Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    UTF8               UTF8_Erase(UTF8 *String, UTF8 NewValue);
    
    /*!
     @abstract                                           Securely erases a string.
     @param            String                            Pointer to the String to be erased.
     @param            NewValue                          The value to set each codeunit to while erasing.
     @return                                             Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    UTF16              UTF16_Erase(UTF16 *String, UTF16 NewValue);
    
    /*!
     @abstract                                           Securely erases a string.
     @param            String                            Pointer to the String to be erased.
     @param            NewValue                          The value to set each codeunit to while erasing.
     @return                                             Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    UTF32              UTF32_Erase(UTF32 *String, UTF32 NewValue);
    
    /*!
     @abstract                                           Cuts a string down to MaxCodeUnits CodeUnits.
     @remark                                             MaxCodeUnits DOES NOT include the NULL Terminator.
     @param            String                            The string to truncate.
     @param            NumGraphemes                      The maxiumum amount of CodeUnits in the string, not counting the null terminator.
     @return                                             Returns the truncated, null terminated copy of String.
     */
    UTF8              *UTF8_Truncate(ImmutableString_UTF8 String, size_t NumGraphemes);
    
    /*!
     @abstract                                           Cuts a string down to MaxCodeUnits CodeUnits.
     @remark                                             MaxCodeUnits DOES NOT include the NULL Terminator.
     @param            String                            The string to truncate.
     @param            NumGraphemes                      The maxiumum amount of CodeUnits in the string, not counting the null terminator.
     @return                                             Returns the truncated, null terminated copy of String.
     */
    UTF16             *UTF16_Truncate(ImmutableString_UTF16 String, size_t NumGraphemes);
    
    /*!
     @abstract                                           Cuts a string down to MaxCodePoints CodePoints.
     @remark                                             MaxCodePoints DOES NOT include the NULL Terminator.
     @param            String                            The string to truncate.
     @param            NumGraphemes                      The maxiumum amount of CodePoints in the string, not counting the null terminator.
     @return                                             Returns the truncated, null terminated copy of String.
     */
    UTF32             *UTF32_Truncate(ImmutableString_UTF32 String, size_t NumGraphemes);
    
    /*!
     @abstract                                           Creates a copy of String, with String2Insert starting at Offset.
     @remark                                             An offset of 0xFFFFFFFFFFFFFFFF means the end of the string.
     @param            String                            The string to manipulate.
     @param            String2Insert                     The string to be inserted into the String.
     @param            OffsetInCodePoints                Where should String2Insert be inserted?
     @return                                             Returns a pointer to a new string containing the original, and String2Insert at Offset.
     */
    UTF8              *UTF8_Insert(ImmutableString_UTF8 String, ImmutableString_UTF8 String2Insert, size_t OffsetInCodePoints);
    
    /*!
     @abstract                                           Creates a copy of String, with String2Insert starting at Offset.
     @remark                                             An offset of 0xFFFFFFFFFFFFFFFF means the end of the string.
     @param            String                            The string to manipulate.
     @param            String2Insert                     The string to be inserted into the String.
     @param            OffsetInCodePoints                Where should String2Insert be inserted?
     @return                                             Returns a pointer to a new string containing the original, and String2Insert at Offset.
     */
    UTF16             *UTF16_Insert(ImmutableString_UTF16 String, ImmutableString_UTF16 String2Insert, size_t OffsetInCodePoints);
    
    /*!
     @abstract                                           Creates a copy of String, with String2Insert starting at Offset.
     @remark                                             An offset of 0xFFFFFFFFFFFFFFFF means the end of the string.
     @param            String                            The string to manipulate.
     @param            String2Insert                     The string to be inserted into the String.
     @param            OffsetInCodePoints                Where should String2Insert be inserted?
     @return                                             Returns a pointer to a new string containing the original, and String2Insert at Offset.
     */
    UTF32             *UTF32_Insert(ImmutableString_UTF32 String, ImmutableString_UTF32 String2Insert, size_t OffsetInCodePoints);
    
    /*!
     @abstract                                           Reverses a string Grapheme by Grapheme.
     @remark                                             It is your responsibility to free the returned string when you're done using it.
     @param            String                            A string to reverse.
     */
    UTF8              *UTF8_Reverse(ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Reverses a string Grapheme by Grapheme.
     @remark                                             It is your responsibility to free the returned string when you're done using it.
     @param            String                            A string to reverse.
     */
    UTF16             *UTF16_Reverse(ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Reverses a string Grapheme by Grapheme.
     @remark                                             It is your responsibility to free the returned string when you're done using it.
     @param            String                            A string to reverse.
     */
    UTF32             *UTF32_Reverse(ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Finds a substring
     @param            String                            The String to search.
     @param            StringLength                      The size of the String in CodeUnits.
     @param            String2Find                       What to search the String for.
     @param            WhitespaceType                    How to handle whitespace.
     @return                                             Returns the offset of the start of String2Find, or -1.
     */
    size_t             UTF8_Find(UTF8 *String, size_t StringLength, UTF8 *String2Find, StringIO_WhitespaceTypes WhitespaceType);
    
    /*!
     @abstract                                           Finds a substring
     @param            String                            The String to search.
     @param            StringLength                      The size of the String in CodeUnits.
     @param            String2Find                       What to search the String for.
     @param            WhitespaceType                    How to handle whitespace.
     @return                                             Returns the offset of the start of String2Find, or -1.
     */
    size_t             UTF16_Find(UTF16 *String, size_t StringLength, UTF16 *String2Find, StringIO_WhitespaceTypes WhitespaceType);
    
    /*!
     @abstract                                           Finds a substring
     @param            String                            The String to search.
     @param            StringLength                      The size of the String in CodeUnits.
     @param            String2Find                       What to search the String for.
     @param            WhitespaceType                    How to handle whitespace.
     @return                                             Returns the offset of the start of String2Find, or -1.
     */
    size_t             UTF32_Find(UTF32 *String, size_t StringLength, UTF32 *String2Find, StringIO_WhitespaceTypes WhitespaceType);
    
    /*!
     @abstract                                           Reads a Grapheme from Source.
     @remark                                             Replaces Fgetc and getc.
     @param            Source                            The file to read from.
     @return                                             Returns the UTF-8 encoded CodePoint which will be between 1-4 CodeUnits.
     */
    UTF8              *UTF8_ReadGrapheme(FILE *Source);
    
    /*!
     @abstract                                           Reads a Grapheme from Source.
     @remark                                             Replaces Fgetwc and getwc.
     @param            Source                            The file to read from.
     @return                                             Returns the UTF-16 encoded CodePoint which will be between 1-2 CodeUnits.
     */
    UTF16             *UTF16_ReadGrapheme(FILE *Source);

    /*!
     @abstract                                           How large is the line break starting at String[CodePoint]?
     @param            String                            The string to check.
     @return                                             The size of the Line Break in CodePoints.
     */
    size_t             UTF32_LineBreakSize(UTF32 *String);
    
    /*!
     @abstract                                           Writes a CodePoint to Source.
     @remark                                             Replaces Fputc and putc.
     @param            Source                            The file to write to.
     @param            Grapheme                          An array of CodeUnits containing one CodePoint.
     */
    void               UTF8_WriteGrapheme(FILE *Source, ImmutableString_UTF8 Grapheme);
    
    /*!
     @abstract                                           Writes a CodePoint to Source.
     @remark                                             Replaces Fputwc and putwc.
     @param            Source                            The file to write to.
     @param            Grapheme                          The Grapheme to write.
     */
    void               UTF16_WriteGrapheme(FILE *Source, ImmutableString_UTF16 Grapheme);

    /*!
     @abstract                                           Reads a single CodeUnit from a FILE
     @param            Source                            Which FILE to read the CodeUnit from.
     @return                                             Returns the CodeUnit that was read.
     */
    UTF8               UTF8_ReadCodeUnit(FILE *Source);

    /*!
     @abstract                                           Reads a single CodeUnit from a FILE
     @param            Source                            Which FILE to read the CodeUnit from.
     @return                                             Returns the CodeUnit that was read.
     */
    UTF16              UTF16_ReadCodeUnit(FILE *Source);

    /*!
     @abstract                                           Reads a single CodePoint from a FILE
     @param            Source                            Which FILE to read the CodeUnit from.
     @return                                             Returns the CodePoint that was read.
     */
    UTF32              UTF8_ReadCodePoint(FILE *Source);

    /*!
     @abstract                                           Reads a single CodePoint from a FILE
     @param            Source                            Which FILE to read the CodeUnit from.
     @return                                             Returns the CodePoint that was read.
     */
    UTF32              UTF16_ReadCodePoint(FILE *Source);

    /*!
     @abstract                                           Reads a single CodePoint from a FILE
     @param            Source                            Which FILE to read the CodeUnit from.
     @return                                             Returns the CodePoint that was read.
     */
    UTF32              UTF32_ReadCodePoint(FILE *Source);
    
    /*!
     @abstract                                           Reads a Sentence (Until it hits a newline, includes the newline) from Source.
     @remark                                             Replaces Fgets and gets.
     @param            Source                            The file to read from.
     @return                                             Returns the UTF-8 encoded line.
     */
    UTF8              *UTF8_ReadSentence(FILE *Source);
    
    /*!
     @abstract                                           Reads a Sentence (Until it hits a newline, includes the newline) from Source.
     @remark                                             Replaces Fgetws and getws.
     @param            Source                            The file to read from.
     @return                                             Returns the UTF-16 encoded line.
     */
    UTF16             *UTF16_ReadSentence(FILE *Source);
    
    /*!
     @abstract                                           Writes a String to Source.
     @remark                                             DOES NOT terminate the written string with a newline
     @remark                                             Automatically converts string to the streams orientation
     @remark                                             Replaces Fputs completely.
     @param            OutputFile                        The file to write the string to.
     @param            String                            The String to write.
     */
    void               UTF8_File_WriteString(FILE *OutputFile, ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Writes a String to Source.
     @remark                                             DOES NOT terminate the written string with a newline
     @remark                                             Automatically converts string to the streams orientation
     @remark                                             Replaces Fputws.
     @param            OutputFile                        The file to write the string to.
     @param            String                            The String to write.
     */
    void               UTF16_File_WriteString(FILE *OutputFile, ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Writes a String to Source.
     @remark                                             DOES NOT terminate the written string with a newline
     @remark                                             Automatically converts string to the streams orientation
     @remark                                             Replaces Fputws and putws.
     @param            OutputFile                        The file to write the string to.
     @param            String                            The String to write.
     */
    void               UTF32_File_WriteString(FILE *OutputFile, ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Counts the number of Digits in String starting at Offset (inclusive).
     @param            Base                              The base the string is in.
     @param            String                            The string to check.
     @return                                             Returns the number of format specifiers found.
     */
    size_t             UTF8_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF8 String);
    
    /*!
     @abstract                                           Counts the number of Digits in String starting at Offset (inclusive).
     @param            Base                              The base the string is in.
     @param            String                            The string to check.
     @return                                             Returns the number of format specifiers found.
     */
    size_t             UTF16_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF16 String);
    
    /*!
     @abstract                                           Counts the number of Digits in String starting at Offset (inclusive).
     @param            Base                              The base the string is in.
     @param            String                            The string to check.
     @return                                             Returns the number of format specifiers found.
     */
    size_t             UTF32_GetNumDigits(TextIO_Bases Base, ImmutableString_UTF32 String);
    
    /*!
     @abstract                                           Gets a substring from Offset to where Format and Formatted start matching.
     @param            Format                            The base the string is in.
     @param            Formatted                         The string to check.
     @param            Offset                            Where to start looking for digits.
     @return                                             Returns the SubString.
     */
    size_t             UTF32_GetSubStringLength(ImmutableString_UTF32 Format, ImmutableString_UTF32 Formatted, size_t Offset);

    /* Unicode Conversion */
    /*!
     @abstract                                           Converts a UTF-8 encoded String to the specified CodePage.
     @param            String                            The Unicode String to convert.
     @param            CharSet                           The character set to convert the string into.
     @return                                             Returns the equivalent (or as close as possible) string in the new character set.
     */
    CharSet8          *UTF8_ConvertUnicode2CharSet(ImmutableString_UTF8 String, StringIO_CharSets CharSet);

    /*!
     @abstract                                           Converts a UTF-16 encoded String to the specified CodePage.
     @param            String                            The Unicode String to convert.
     @param            CharSet                           The character set to convert the string into.
     @return                                             Returns the equivalent (or as close as possible) string in the new character set.
     */
    CharSet16         *UTF16_ConvertUnicode2CharSet(ImmutableString_UTF16 String, StringIO_CharSets CharSet);

    /*!
     @abstract                                           Converts a UTF-32 encoded String to the specified CodePage.
     @param            String                            The Unicode String to convert.
     @param            CharSet                           The character set to convert the string into.
     @return                                             Returns the equivalent (or as close as possible) string in the new character set.
     */
    CharSet32         *UTF32_ConvertUnicode2CharSet(ImmutableString_UTF32 String, StringIO_CharSets CharSet);

    /*!
     @abstract                                           Converts a UTF-8 encoded String to the specified CodePage.
     @param            String                            The Unicode String to convert.
     @param            CharSet                           The character set to convert the string into.
     @return                                             Returns the equivalent (or as close as possible) string in the new character set.
     */
    UTF8              *UTF8_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet8 *) String, StringIO_CharSets CharSet);

    /*!
     @abstract                                           Converts a UTF-16 encoded String to the specified CodePage.
     @param            String                            The Unicode String to convert.
     @param            CharSet                           The character set to convert the string into.
     @return                                             Returns the equivalent (or as close as possible) string in the new character set.
     */
    UTF16             *UTF16_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet16 *) String, StringIO_CharSets CharSet);

    /*!
     @abstract                                           Converts a UTF-32 encoded String to the specified CodePage.
     @param            String                            The Unicode String to convert.
     @param            CharSet                           The character set to convert the string into.
     @return                                             Returns the equivalent (or as close as possible) string in the new character set.
     */
    UTF32             *UTF32_ConvertCharSet2Unicode(PlatformIO_Immutable(CharSet32 *) String, StringIO_CharSets CharSet);
    /* Unicode Conversion */
    
    /*!
     @abstract                                           Deletes String.
     @param            String                            The string to deinitialize.
     */
    void               UTF8_Deinit(UTF8 *String);
    
    /*!
     @abstract                                           Deletes String.
     @param            String                            The string to deinitialize.
     */
    void               UTF16_Deinit(UTF16 *String);
    
    /*!
     @abstract                                           Deletes String.
     @param            String                            The string to deinitialize.
     */
    void               UTF32_Deinit(UTF32 *String);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_StringIO_H */
