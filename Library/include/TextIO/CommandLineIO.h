/*!
 @header              CommandLineIO.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for creating a command line interface.
 */

#include "../../include/Macros.h"

#pragma  once

#ifndef  FoundationIO_UnicodeIO_CommandLineIO_H
#define  FoundationIO_UnicodeIO_CommandLineIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
#if (defined __STDC_UTF_8__ && defined __CHAR8_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef                     char8_t                         UTF8;
#else
    typedef                     unsigned char                   UTF8;
#endif /* __CHAR8_TYPE__ */
#endif /* FoundationIO_StringType8 */
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
#if (defined __STDC_UTF_16__ && defined __CHAR16_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef                     char16_t                        UTF16;
#else
    typedef                     uint_least16_t                  UTF16;
#endif /* __CHAR16_TYPE__ */
#endif /* FoundationIO_StringType16 */
    
#ifndef   FoundationIO_StringType32
#define   FoundationIO_StringType32 (4)
#ifdef    UTF32
#undef    UTF32
#endif /* UTF32 */
#if (defined __STDC_UTF_32__ && defined __CHAR32_TYPE__ && FoundationIOSTDVersion >= FoundationIOSTDVersionC2X) && (FoundationIOTargetOS != FoundationIOAppleOS)
    typedef                     char32_t                        UTF32;
#else
    typedef                     uint_least32_t                  UTF32;
#endif /* __CHAR32_TYPE__ */
#endif /* FoundationIO_StringType32 */
    
#ifndef                         FoundationIO_Unicodize8
#define                         FoundationIO_Unicodize8         (1)
#define                         UTF8String(Literal)             (UTF8*) u8##Literal
#define                         UTF8Character(Literal)          (UTF8)  u8##Literal
#endif /* FoundationIO_Unicodize8 */
    
#ifndef                         FoundationIO_Unicodize16
#define                         FoundationIO_Unicodize16        (2)
#define                         UTF16String(Literal)            (UTF16*) u##Literal
#define                         UTF16Character(Literal)         (UTF16)  u##Literal
#endif /* FoundationIO_Unicodize16 */
    
#ifndef                         FoundationIO_Unicodize32
#define                         FoundationIO_Unicodize32        (4)
#define                         UTF32String(Literal)            (UTF32*) U##Literal
#define                         UTF32Character(Literal)         (UTF32) U##Literal
#endif /* FoundationIO_Unicodize32 */
    
#ifndef                         FoundationIOFunctionName
#define                         FoundationIOFunctionName        (UTF8*) __func__
#endif /* FoundationIOFunctionName */
    
    /*!
     @enum                      CommandLineIO_OptionTypes
     @abstract                                                  "Defines the type of option".
     @constant                  OptionType_Unknown              "Unknown Option type".
     @constant                  OptionType_PotentialSlaves      "the option may have slaves, but it may not too".
     @constant                  OptionType_Slaveless            "the option can not have any slaves".
     @constant                  OptionType_Slave                "the option is a slave".
     @constant                  OptionType_Existential          "the option can not have any slaves, or any arguments, it just exists or doesn't exist".
     @constant                  OptionType_BoolString           "A boolstring is a string that contains Yes/No, true/false, 1/0 as arguments".
     */
    typedef enum CommandLineIO_OptionTypes {
                                OptionType_Unknown              = 0,
                                OptionType_PotentialSlaves      = 1,
                                OptionType_Slaveless            = 2,
                                OptionType_Slave                = 4,
                                OptionType_Existential          = 8,
    } CommandLineIO_OptionTypes;
    
    /*!
     @enum                      CommandLineIO_ArgumentTypes
     @abstract                                                  "Defines the type of argument accepted by the Option".
     @constant                  ArgumentType_Unknown            "Unknown Argument type".
     @constant                  ArgumentType_Disallowed         "The Argument is not allowed".
     @constant                  ArgumentType_Path               "The Argument is a path on the filesystem".
     @constant                  ArgumentType_String             "The Argument is a string".
     @constant                  ArgumentType_Range              "The Argument is a range seperated by .. or :".
     @constant                  ArgumentType_Ratio              "The Argument is a ratio seperated by a / or \".
     @constant                  ArgumentType_Slave              "The Argument is the name of a slave option".
     */
    typedef enum CommandLineIO_ArgumentTypes {
                                ArgumentType_Unknown            = 0,
                                ArgumentType_Disallowed         = 1,
                                ArgumentType_Path               = 2,
                                ArgumentType_URL                = 4,
                                ArgumentType_String             = 8,
                                ArgumentType_Range              = 16,
                                ArgumentType_Ratio              = 32,
                                ArgumentType_Slave              = 64,
                                ArgumentType_BoolString         = 128,
    } CommandLineIO_ArgumentTypes;
    
    /*!
     @enum                      CommandLineIO_OptionStatuses
     @abstract                                                  "Defines the status of each Option".
     @constant                  OptionStatus_Unknown            "Unknown Option status".
     @constant                  OptionStatus_Required           "The Option MUST be present".
     @constant                  OptionStatus_Optional           "The Option MAY be present".
     @constant                  OptionStatus_Incompatible       "The Option can only be present if none of it's incompatible options are present".
     */
    typedef enum CommandLineIO_OptionStatuses {
                                OptionStatus_Unknown            = 0,
                                OptionStatus_Required           = 1,
                                OptionStatus_Optional           = 2,
                                OptionStatus_Incompatible       = 3,
    } CommandLineIO_OptionStatuses;
    
    /*!
     @enum                      CommandLineIO_LicenseTypes
     @abstract                                                  "Defines the types of licenses".
     @constant                  LicenseType_Unknown             "Unknown Argument type".
     @constant                  LicenseType_Permissive          "The Argument may have slaves, but it may not too".
     @constant                  LicenseType_Proprietary         "The Argument can not have any slaves".
     @constant                  LicenseType_Copyleft            "The Argument is a slave".
     */
    typedef enum CommandLineIO_LicenseTypes {
                                LicenseType_Unknown             = 0,
                                LicenseType_Permissive          = 1,
                                LicenseType_Proprietary         = 2,
                                LicenseType_Copyleft            = 3,
    } CommandLineIO_LicenseTypes;
    
    /*!
     @typedef                   CommandLineIO
     @abstract                                                  "Contains all the information, and relationships between options and Options on the command line".
     */
    typedef struct              CommandLineIO                   CommandLineIO;
    
    /*!
     @abstract                                                  "Creates a CommandLineIO instance".
     @param                     NumOptions                      "The number of CommandLineOption structures to initalize".
     @return                                                    "Returns a pointer to an initialized CommandLineIO instance".
     */
    CommandLineIO              *CommandLineIO_Init(uint64_t NumOptions);
    
    /*!
     @abstract                                                  "Creating a Text UI".
     @return                                                    "Returns the width of the terminal in codepoints".
     */
    uint64_t                    CommandLineIO_GetTerminalWidth(void);
    
    /*!
     @abstract                                                  "Creating a Text UI".
     @return                                                    "Returns the height of the terminal in codepoints".
     */
    uint64_t                    CommandLineIO_GetTerminalHeight(void);
    
    /*!
     @abstract                                                  "Creating a Text UI".
     @return                                                    "Returns if the terminal has been resized, and therefore needs to be redrawn".
     */
    bool                        CommandLineIO_TerminalWasResized(void);
    
    /*!
     @abstract                                                  "Helper function to cast argc to uint64_t and handle Windows' globals".
     @return                                                    "Returns the number of arguments".
     */
    uint64_t                    CommandLineIO_GetNumArguments(int argc);
    
    /*!
     @abstract                                                  "Helper function to decode argv/wargv to UTF32 for use with parsing".
     @return                                                    "Returns the decoded Arguments".
     */
    UTF32                     **CommandLineIO_GetArgumentArray(void **Arguments);
    
    /*!
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Name                            "The name of the program you're building".
     */
    void                        CommandLineIO_SetName(CommandLineIO *CLI, UTF32 *Name);
    
    /*!
     @abstract                                                  "Sets the version of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Version                         "The version of the program you're building".
     */
    void                        CommandLineIO_SetVersion(CommandLineIO *CLI, UTF32 *Version);
    
    /*!
     @abstract                                                  "Sets the description of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Description                     "Description of what the program does".
     */
    void                        CommandLineIO_SetDescription(CommandLineIO *CLI, UTF32 *Description);
    
    /*!
     @abstract                                                  "Sets the author of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Author                          "Author of this program".
     */
    void                        CommandLineIO_SetAuthor(CommandLineIO *CLI, UTF32 *Author);
    
    /*!
     @abstract                                                  "Sets the copyright years of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Copyright                       "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                        CommandLineIO_SetCopyright(CommandLineIO *CLI, UTF32 *Copyright);
    
    /*!
     @abstract                                                  "Sets the license of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     LicenseType                     "What type of license is this software released under"?
     @param                     Name                            "What is the name of the license"?
     @param                     LicenseURL                      "The actual URL for the license".
     */
    void                        CommandLineIO_SetLicense(CommandLineIO *CLI, CommandLineIO_LicenseTypes LicenseType, UTF32 *Name, UTF32 *LicenseURL);
    
    /*!
     @abstract                                                  "What is the minimum number of options your program needs to operate"?
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MinOptions                      "The minimum number of options".
     */
    void                        CommandLineIO_SetMinOptions(CommandLineIO *CLI, uint64_t MinOptions);
    
    /*!
     @abstract                  CLI                             "CommandLineIO Pointer".
     @param                     HelpOption                      "Which option is the one to get help"?
     */
    void                        CommandLineIO_SetHelpOption(CommandLineIO *CLI, uint64_t HelpOption);
    
    /*!
     @abstract                                                  "Displays on the screen the progress of X actions that are taking place"
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumItems2Display                "The number of things you want to show the progress of".
     @param                     Strings                         "A pointer to an array of the strings describing what progress is being tracked".
     @param                     Numerator                       "A pointer to an array of the number of X tasks that have been completed".
     @param                     Denominator                     "A pointer to an array of the number of X thats that there are to do".
     */
    void                        CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF32 **Strings, uint64_t *Numerator, uint64_t *Denominator);
    
    /*!
     @abstract                                                  "Sets OptionID's flag in the CommandLineIO instance pointed by CLI".
     @remark                                                    "Just enter the number of characters you typed into the string not counting the quotes".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to set".
     @param                     Name                            "The flag to identify an option with".
     */
    void                        CommandLineIO_Option_SetName(CommandLineIO *CLI, uint64_t OptionID, UTF32 *Name);
    
    /*!
     @abstract                                                  "Sets OptionDescription's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to set".
     @param                     Description                     "Pointer to a UTF-8 encoded string containing the description of what this program does".
     */
    void                        CommandLineIO_Option_SetDescription(CommandLineIO *CLI, uint64_t OptionID, UTF32 *Description);
    
    /*!
     @abstract                                                  "Sets OptionDescription's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to set".
     @param                     Description                     "Pointer to a UTF-8 encoded string containing the description of what this program does".
     */
    void                        CommandLineIO_UTF16_Option_SetDescription(CommandLineIO *CLI, uint64_t OptionID, UTF16 *Description);
    
    /*!
     @abstract                                                  "Sets OptionID's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "Which option are we talking about"?
     */
    void                        CommandLineIO_Option_SetType(CommandLineIO *CLI, uint64_t OptionID, CommandLineIO_OptionTypes OptionType);
    
    /*!
     @abstract                                                  "Sets OptionID as accepting arguments of only a certain type".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to apply the ArgumentType to".
     @param                     ArgumentType                    "The argument type from CommandLineIO_ArgumentTypes".
     */
    void                        CommandLineIO_Option_SetArgumentType(CommandLineIO *CLI, uint64_t OptionID, CommandLineIO_ArgumentTypes ArgumentType);
    
    /*!
     @abstract                                                  "Sets MetaFlag option as a meta flag for OptionID".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MasterID                        "Which option does the child/meta option depend on"?
     @param                     Slave                           "Which option is the child option"?
     */
    void                        CommandLineIO_Option_SetSlave(CommandLineIO *CLI, uint64_t MasterID, uint64_t Slave);
    
    /*!
     @abstract                                                  "Parses the Command Line Options as UTF-8 encoded strings".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumArguments                    "The number of argument strings present in Arguments; equilivent to argc".
     @param                     Arguments                       "An array of UTF-8 encoded arguments; equilivent to argv"
     */
    void                        CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, UTF8 **Arguments);
    
    /*!
     @abstract                                                  "Parses the Command Line Options as UTF-16 encoded strings".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumArguments                    "The number of argument strings present in Arguments; equilivent to __argc".
     @param                     Arguments                       "An array of UTF-16 encoded arguments; equilivent to __wargv"
     */
    void                        CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, UTF16 **Arguments);
    
    /*!
     @abstract                                                  "How many matching options are present in CommandLineIO (will also check for slave options if present)".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to look for".
     @param                     NumSlaves                       "How many slave Options should we look for"?
     @param                     SlaveIDs                        "Pointer to an array with all the slaves you want to mke sure are present".
     @return                                                    "Returns the argument number if there is no matching argument it will return -1".
     */
    uint64_t                     CommandLineIO_GetNumMatchingOptions(CommandLineIO *CLI, uint64_t OptionID, uint64_t NumSlaves, uint64_t *SlaveIDs);
    
    /*!
     @abstract                                                  "How many Master options are present in the Options (will also check for slave options if present)".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to look for".
     @param                     NumSlaves                       "How many slave Options should we look for"?
     @param                     SlaveIDs                        "Pointer to an array with all the slaves you want to mke sure are present".
     @return                                                    "Returns the argument number if there is no matching argument it will return -1".
     */
    uint64_t                     CommandLineIO_GetOptionNum(CommandLineIO *CLI, uint64_t OptionID, uint64_t NumSlaves, uint64_t *SlaveIDs);
    
    /*!
     @abstract                                                  "Gets the data contained in Argument2Option".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "The option's result to return".
     @return                                                    "Returns the data after the option, if the option is resultless it will return 0".
     */
    UTF8                       *CommandLineIO_UTF8_GetOptionResult(CommandLineIO *CLI, uint64_t OptionID);
    
    /*!
     @abstract                                                  "Gets the data contained in Argument2Option".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "The option's result to return".
     @return                                                    "Returns the data after the option, if the option is resultless it will return 0".
     */
    UTF16                      *CommandLineIO_UTF16_GetOptionResult(CommandLineIO *CLI, uint64_t OptionID);
    
    /*!
     @abstract                                                  "Deallocates the instance of CommandLineIO pointed to by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     */
    void                        CommandLineIO_Deinit(CommandLineIO *CLI);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_UnicodeIO_CommandLineIO_H */
