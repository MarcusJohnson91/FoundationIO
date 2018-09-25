#include <stdint.h>
#include <stdbool.h>

/* Forward declare StringIO's types */
#ifndef UTF8
typedef               uint_least8_t                        UTF8;
#endif

#ifndef UTF16
#ifdef __STD_UTF_16__
typedef               char16_t                             UTF16;
#else
typedef               uint_least16_t                       UTF16;
#endif
#endif

#ifndef UTF32
#ifdef __STD_UTF_32__
typedef               char32_t                             UTF32;
#else
typedef               uint_least32_t                       UTF32;
#endif
#endif

#pragma  once

#ifndef  FoundationIO_CommandLineIO_H
#define  FoundationIO_CommandLineIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header                    CommandLineIO.h
     @author                    Marcus Johnson
     @copyright                 2017+
     @version                   1.0.0
     @brief                     This header contains code for parsing command line options.
     */
    
    /*!
     @enum                      CLISwitchTypes
     @abstract                                                  "Defines the type of switch".
     @constant                  UnknownSwitchType               "Unknown Switch type".
     @constant                  SwitchMayHaveSlaves             "The switch may have slaves, but it may not too".
     @constant                  SwitchCantHaveSlaves            "The switch can not have any slaves".
     @constant                  SwitchIsASlave                  "The switch is a slave".
     @constant                  ExistentialSwitch               "The switch can not have any slaves, or any arguments, it just exists or doesn't exist".
     */
    typedef enum CLISwitchTypes {
                                UnknownSwitchType               = 0,
                                SwitchMayHaveSlaves             = 1,
                                SwitchCantHaveSlaves            = 2,
                                SwitchIsASlave                  = 4,
                                ExistentialSwitch               = 8,
    } CLISwitchTypes;
    
    /*!
     @enum                      CLIOptionTypes
     @abstract                                                  "Defines the type of Option".
     @constant                  UnknownSwitchType               "Unknown Option type".
     @constant                  OptionHasArgMayHaveSlaves       "The Option may have slaves, but it may not too".
     @constant                  OptionHasArgNoSlaves            "The Option can not have any slaves".
     @constant                  OptionNoArgMayHaveSlaves        "The Option is a slave".
     @constant                  OptionNoArgNoSlaves             "The Option can not have any slaves, or any arguments, it just exists or doesn't exist".
     */
    typedef enum CLIOptionTypes {
                                UnknownOptionType               = 0,
                                OptionHasArgMayHaveSlaves       = 1,
                                OptionHasArgNoSlaves            = 2,
                                OptionNoArgMayHaveSlaves        = 4,
                                OptionNoArgNoSlaves             = 8,
    } CLIOptionTypes;
    
    /*!
     @enum                      CLIArgumentTypes
     @abstract                                                  "Defines the type of argument accepted by the Option".
     @constant                  UnknownArgumentType             "Unknown Argument type".
     @constant                  ArgumentIsAPath                 "The Argument may have slaves, but it may not too".
     @constant                  ArgumentIsAString               "The Argument can not have any slaves".
     @constant                  ArgumentIsARange                "The Argument is a slave".
     @constant                  ArgumentIsARatio                "The Argument can not have any slaves, or any arguments, it just exists or doesn't exist".
     */
    typedef enum CLIArgumentTypes {
                                UnknownArgumentType             = 0,
                                ArgumentIsAPath                 = 1,
                                ArgumentIsAString               = 2,
                                ArgumentIsARange                = 4,
                                ArgumentIsARatio                = 8,
                                ArgumentNotAllowed              = 16,
    } CLIArgumentTypes;
    
    /*!
     @enum                      CLILicenseTypes
     @abstract                                                  "Defines the types of licenses".
     @constant                  UnknownLicenseType              "Unknown Argument type".
     @constant                  PermissiveLicense               "The Argument may have slaves, but it may not too".
     @constant                  ProprietaryLicense              "The Argument can not have any slaves".
     @constant                  CopyleftLicense                 "The Argument is a slave".
     */
    typedef enum CLILicenseTypes {
                                UnknownLicenseType              = 0,
                                PermissiveLicense               = 1,
                                ProprietaryLicense              = 2,
                                CopyleftLicense                 = 3,
    } CLILicenseTypes;
    
    /*!
     @enum                      CLILicenseTypes
     @abstract                                                  "Defines the types of licenses".
     @constant                  UnknownDelimiter                "Unknown delimiter".
     @constant                  RangeDelimiter                  "Denotes a range using the double dot syntax (X..Y)".
     @constant                  EqualDelimiter                  "Delimits 2 fields with an equal sign (SwitchName=Result)".
     @constant                  ColonDelimiter                  "Delimits 2 fields with a colon (SwitchName:Result)".
     */
    typedef enum CLIDelimiterTypes {
                                UnknownDelimiter                = 0,
                                RangeDelimiter                  = 1,
                                EqualDelimiter                  = 2,
                                ColonDelimiter                  = 3,
    } CLIDelimiterTypes;
    
    /*!
     @typedef                   CommandLineIO
     @abstract                                                  "Contains all the information, and relationships between switches and Options on the command line".
     */
    typedef struct              CommandLineIO                   CommandLineIO;
    
    /*!
     @abstract                                                  "Initializes a CommandLineIO instance".
     @param                     NumSwitches                     "The number of CommandLineSwitch structures to initalize".
     @return                                                    "Returns a pointer to an initialized CommandLineIO instance".
     */
    CommandLineIO              *CommandLineIO_Init(const int64_t NumSwitches);
    
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
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Name                            "The name of the program you're building".
     */
    void                        CommandLineIO_UTF8_SetName(CommandLineIO *CLI, UTF8 *Name);
    
    /*!
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Name                            "The name of the program you're building".
     */
    void                        CommandLineIO_UTF16_SetName(CommandLineIO *CLI, UTF16 *Name);
    
    /*!
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Version                         "The version of the program you're building".
     */
    void                        CommandLineIO_UTF8_SetVersion(CommandLineIO *CLI, UTF8 *Version);
    
    /*!
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Version                         "The version of the program you're building".
     */
    void                        CommandLineIO_UTF16_SetVersion(CommandLineIO *CLI, UTF16 *Version);
    
    /*!
     @abstract                                                  "Sets the description of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Description                     "Description of what the program does".
     */
    void                        CommandLineIO_UTF8_SetDescription(CommandLineIO *CLI, UTF8 *Description);
    
    /*!
     @abstract                                                  "Sets the description of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Description                     "Description of what the program does".
     */
    void                        CommandLineIO_UTF16_SetDescription(CommandLineIO *CLI, UTF16 *Description);
    
    /*!
     @abstract                                                  "Sets the author of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Author                          "Author of this program".
     */
    void                        CommandLineIO_UTF8_SetAuthor(CommandLineIO *CLI, UTF8 *Author);
    
    /*!
     @abstract                                                  "Sets the author of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Author                          "Author of this program".
     */
    void                        CommandLineIO_UTF16_SetAuthor(CommandLineIO *CLI, UTF16 *Author);
    
    /*!
     @abstract                                                  "Sets the copyright years of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Copyright                       "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                        CommandLineIO_UTF8_SetCopyright(CommandLineIO *CLI, UTF8 *Copyright);
    
    /*!
     @abstract                                                  "Sets the copyright years of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Copyright                       "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                        CommandLineIO_UTF16_SetCopyright(CommandLineIO *CLI, UTF16 *Copyright);
    
    /*!
     @abstract                                                  "Sets the license of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     LicenseType                     "What type of license is this software released under"?
     @param                     Name                            "What is the name of the license"?
     @param                     LicenseURL                      "The actual URL for the license".
     */
    void                        CommandLineIO_UTF8_SetLicense(CommandLineIO *CLI, CLILicenseTypes LicenseType, UTF8 *Name, UTF8 *LicenseURL);
    
    /*!
     @abstract                                                  "Sets the license of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     LicenseType                     "What type of license is this software released under"?
     @param                     Name                            "What is the name of the license"?
     @param                     LicenseURL                      "The actual URL for the license".
     */
    void                        CommandLineIO_UTF16_SetLicense(CommandLineIO *CLI, CLILicenseTypes LicenseType, UTF16 *Name, UTF16 *LicenseURL);
    
    /*!
     @abstract                                                  "What is the minimum number of options your program needs to operate"?
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MinOptions                      "The minimum number of switches".
     */
    void                        CommandLineIO_SetMinOptions(CommandLineIO *CLI, const int64_t MinOptions);
    
    /*!
     @abstract                  CLI                             "CommandLineIO Pointer".
     @param                     HelpSwitch                      "Which switch is the one to get help"?
     */
    void                        CommandLineIO_SetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch);
    
    /*!
     @abstract                                                  "Displays on the screen the progress of X actions that are taking place"
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumItems2Display                "The number of things you want to show the progress of".
     @param                     Strings                         "A pointer to an array of the strings describing what progress is being tracked".
     @param                     Numerator                       "A pointer to an array of the number of X tasks that have been completed".
     @param                     Denominator                     "A pointer to an array of the number of X thats that there are to do".
     */
    void                        CommandLineIO_UTF8_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF8 **Strings, uint64_t *Numerator, uint64_t *Denominator);
    
    /*!
     @abstract                                                  "Displays on the screen the progress of X actions that are taking place"
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumItems2Display                "The number of things you want to show the progress of".
     @param                     Strings                         "A pointer to an array of the strings describing what progress is being tracked".
     @param                     Numerator                       "A pointer to an array of the number of X tasks that have been completed".
     @param                     Denominator                     "A pointer to an array of the number of X thats that there are to do".
     */
    void                        CommandLineIO_UTF16_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF16 **Strings, uint64_t *Numerator, uint64_t *Denominator);
    
    /*!
     @abstract                                                  "Sets SwitchID's flag in the CommandLineIO instance pointed by CLI".
     @remark                                                    "Just enter the number of characters you typed into the string not counting the quotes".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to set".
     @param                     Name                            "The flag to identify an option with".
     */
    void                        CommandLineIO_UTF8_Switch_SetName(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Name);
    
    /*!
     @abstract                                                  "Sets SwitchID's flag in the CommandLineIO instance pointed by CLI".
     @remark                                                    "Just enter the number of characters you typed into the string not counting the quotes".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to set".
     @param                     Name                            "The flag to identify an option with".
     */
    void                        CommandLineIO_UTF16_Switch_SetName(CommandLineIO *CLI, const int64_t SwitchID, UTF16 *Name);
    
    /*!
     @abstract                                                  "Sets SwitchDescription's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to set".
     @param                     Description                     "Pointer to a UTF-8 encoded string containing the description of what this program does".
     */
    void                        CommandLineIO_UTF8_Switch_SetDescription(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Description);
    
    /*!
     @abstract                                                  "Sets SwitchDescription's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to set".
     @param                     Description                     "Pointer to a UTF-8 encoded string containing the description of what this program does".
     */
    void                        CommandLineIO_UTF16_Switch_SetDescription(CommandLineIO *CLI, const int64_t SwitchID, UTF16 *Description);
    
    /*!
     @abstract                                                  "Sets SwitchID's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "Which switch are we talking about"?
     */
    void                        CommandLineIO_Switch_SetType(CommandLineIO *CLI, const int64_t SwitchID, CLISwitchTypes SwitchType);
    
    /*!
     @abstract                                                  "Sets SwitchID as accepting arguments of only a certain type".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to apply the ArgumentType to".
     @param                     ArgumentType                    "The argument type from CLIArgumentTypes".
     */
    void                        CommandLineIO_Switch_SetArgumentType(CommandLineIO *CLI, const int64_t SwitchID, CLIArgumentTypes ArgumentType);
    
    /*!
     @abstract                                                  "Sets MetaFlag switch as a meta flag for switch SwitchID".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MasterID                        "Which switch does the child/meta switch depend on"?
     @param                     Slave                           "Which switch is the child switch"?
     */
    void                        CommandLineIO_Switch_SetSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t Slave);
    
    /*!
     @abstract                                                  "How many Slave switches can be active in a master argument at once"?
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MasterID                        "Which switch are we talking about"?
     @param                     MaxActiveSlaves                 "How many Slave switches can be active in an argument at once"?
     */
    void                        CommandLineIO_Switch_SetMaxConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxActiveSlaves);
    
    /*!
     @abstract                                                  "Parses the Command Line Options as UTF-8 encoded strings".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumArguments                    "The number of argument strings present in Arguments; equilivent to argc".
     @param                     Arguments                       "An array of UTF-8 encoded arguments; equilivent to argv"
     */
    void                        CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF8 **Arguments);
    
    /*!
     @abstract                                                  "Parses the Command Line Options as UTF-16 encoded strings".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumArguments                    "The number of argument strings present in Arguments; equilivent to __argc".
     @param                     Arguments                       "An array of UTF-16 encoded arguments; equilivent to __wargv"
     */
    void                        CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF16 **Arguments);
    
    /*!
     @abstract                                                  "How many matching options are present in CommandLineIO (will also check for slave switches if present)".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to look for".
     @param                     NumSlaves                       "How many slave Options should we look for"?
     @param                     SlaveIDs                        "Pointer to an array with all the slaves you want to mke sure are present".
     @return                                                    "Returns the argument number if there is no matching argument it will return -1".
     */
    int64_t                     CommandLineIO_GetNumMatchingOptions(CommandLineIO *CLI, const int64_t SwitchID, const int64_t NumSlaves, const int64_t *SlaveIDs);
    
    /*!
     @abstract                                                  "How many Master switches are present in the Options (will also check for slave switches if present)".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The Switch to look for".
     @param                     NumSlaves                       "How many slave Options should we look for"?
     @param                     SlaveIDs                        "Pointer to an array with all the slaves you want to mke sure are present".
     @return                                                    "Returns the argument number if there is no matching argument it will return -1".
     */
    int64_t                     CommandLineIO_GetOptionNum(CommandLineIO *CLI, const int64_t SwitchID, const int64_t NumSlaves, const int64_t *SlaveIDs);
    
    /*!
     @abstract                                                  "Gets the data contained in Argument2Option".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "The option's result to return".
     @return                                                    "Returns the data after the switch, if the switch is resultless it will return 0".
     */
    UTF8                       *CommandLineIO_UTF8_GetOptionResult(CommandLineIO const *CLI, const int64_t OptionID);
    
    /*!
     @abstract                                                  "Gets the data contained in Argument2Option".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "The option's result to return".
     @return                                                    "Returns the data after the switch, if the switch is resultless it will return 0".
     */
    UTF16                      *CommandLineIO_UTF16_GetOptionResult(CommandLineIO const *CLI, const int64_t OptionID);
    
    /*!
     @abstract                                                  "Returns the extension from Path as a string".
     @param                     Path                            "The UTF-8 encoded string to extract the extension from".
     */
    UTF8                       *CommandLineIO_UTF8_GetExtension(UTF8 *Path);
    
    /*!
     @abstract                                                  "Returns the extension from Path as a string".
     @param                     Path                            "The UTF-16 encoded string to extract the extension from".
     */
    UTF16                      *CommandLineIO_UTF16_GetExtension(UTF16 *Path);
    
    /*!
     @abstract                                                  "Deallocates the instance of CommandLineIO pointed to by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     */
    void                        CommandLineIO_Deinit(CommandLineIO *CLI);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_CommandLineIO_H */
