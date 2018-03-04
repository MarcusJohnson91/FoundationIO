#include <stdint.h>                   /* Included for u/intX_t */

#include "StringIO.h"                 /* Included for UTF8 */

#pragma  once

#ifndef  FoundationIO_CommandLineIO_H
#define  FoundationIO_CommandLineIO_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*!
     @header    CommandLineIO.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2017+
     @version   4.0.0
     @brief     This header contains code for parsing command line options.
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
     @constant                  SwitchHasArgMayHaveSlaves       "The Option may have slaves, but it may not too".
     @constant                  SwitchHasArgNoSlaves            "The Option can not have any slaves".
     @constant                  SwitchNoArgMayHaveSlaves        "The Option is a slave".
     @constant                  SwitchNoArgNoSlaves             "The Option can not have any slaves, or any arguments, it just exists or doesn't exist".
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
    
    typedef enum CLILicenseTypes {
                                UnknownLicenseType              = 0,
                                PermissiveLicense               = 1,
                                ProprietaryLicense              = 2,
                                CopyleftLicense                 = 3,
    } CLILicenseTypes;
    
    typedef enum DelimiterTypes {
                                UnknownDelimiter                = 0,
                                RangeDelimiter                  = 1,
                                EqualDelimiter                  = 2,
                                ColonDelimiter                  = 3,
    } DelimiterTypes;
    
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
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Name                            "Pointer to a C string containing the name of the program you're building".
     */
    void                        CLISetName(CommandLineIO *CLI, UTF8 *Name);
    
    /*!
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Version                         "Pointer to a C string contining the version of the program you're building".
     */
    void                        CLISetVersion(CommandLineIO *CLI, UTF8 *Version);
    
    /*!
     @abstract                                                  "Sets the description of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Description                     "Description of what the program does".
     */
    void                        CLISetDescription(CommandLineIO *CLI, UTF8 *Description);
    
    /*!
     @abstract                                                  "Sets the author of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Author                          "Author of this program".
     */
    void                        CLISetAuthor(CommandLineIO *CLI, UTF8 *Author);
    
    /*!
     @abstract                                                  "Sets the copyright years of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Copyright                       "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                        CLISetCopyright(CommandLineIO *CLI, UTF8 *Copyright);
    
    /*!
     @abstract                                                  "Sets the license of the program".
     @remark                                                    "If your program is closed source, do NOT use the License options, use the EULA functions".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     LicenseType                     "What type of license is this software released under"?
     @param                     Name                            "What is the name of the license"?
     @param                     LicenseURL                      "The actual URL for the license".
     */
    void                        CLISetLicense(CommandLineIO *CLI, CLILicenseTypes LicenseType, UTF8 *Name, UTF8 *LicenseURL);
    
    /*!
     @abstract                                                  "What is the minimum number of switches your program needs to operate"?
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MinOptions                      "The minimum number of switches".
     */
    void                        CLISetMinOptions(CommandLineIO *CLI, const int64_t MinOptions);
    
    /*!
     @abstract                  CLI                             "CommandLineIO Pointer".
     @param                     HelpSwitch                      "Which switch is the one to print all the Options"?
     */
    void                        CLISetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch);
    
    /*!
     @abstract                                                  "Sets SwitchID's flag in the CommandLineIO instance pointed by CLI".
     @remark                                                    "Just enter the number of characters you typed into the string not counting the quotes".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to set".
     @param                     Name                            "The flag to identify an option with".
     */
    void                        CLISetSwitchName(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Name);
    
    /*!
     @abstract                                                  "Sets SwitchDescription's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to set".
     @param                     Description                     "Pointer to a C string containing the description of what this program does".
     */
    void                        CLISetSwitchDescription(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Description);
    
    /*!
     @abstract                                                  "Sets SwitchID's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "Which switch are we talking about"?
     */
    void                        CLISetSwitchType(CommandLineIO *CLI, int64_t SwitchID, CLISwitchTypes SwitchType);
    
    void                        CLISetSwitchArgumentType(CommandLineIO *CLI, const int64_t SwitchID, CLIArgumentTypes ArgumentType);
    
    /*!
     @abstract                                                  "Sets MetaFlag switch as a meta flag for switch SwitchID".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MasterID                        "Which switch does the child/meta switch depend on"?
     @param                     Slave                           "Which switch is the child switch"?
     */
    void                        CLISetSwitchAsSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t Slave);
    
    /*!
     @abstract                                                  "How many Slave switches can be active in a master argument at once"?
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MasterID                        "Which switch are we talking about"?
     @param                     MaxActiveSlaves                 "How many Slave switches can be active in an argument at once"?
     */
    void                        CLISetSwitchMaxConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxActiveSlaves);
    
    /*!
     @abstract                                                  "Parses argv for switches matching the ones contained in CLI".
     @remark                                                    "Args[0] (the path for the original executable) is NEVER searched or used, and is assumed to exist, if it's not set it to NULL".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumArgs                         "The number of Arguments entered".
     @param                     Args                            "An array of UTF-8 encoded strings, for WMain call UTF16_Decode and UTF8_Encode on each string".
     */
    void                        ParseCommandLineOptions(CommandLineIO *CLI, const uint64_t NumArgs, const UTF8 **Args);
    
    /*!
     @abstract                                                  "Displays on the screen the progress of X actions that are taking place"
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumItems2Display                "The number of things you want to show the progress of".
     @param                     Strings                         "A pointer to an array of the strings describing what progress is being tracked".
     @param                     Numerator                       "A pointer to an array of the number of X tasks that have been completed".
     @param                     Denominator                     "A pointer to an array of the number of X thats that there are to do".
     */
    void                        CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF8 *Strings[], uint64_t *Numerator, uint64_t *Denominator);
    
    /*!
     @abstract                                                  "How many matching options are present in CommandLineIO (will also check for slave switches if present)".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The switch to look for".
     @param                     NumSlaves                       "How many slave Options should we look for"?
     @param                     SlaveIDs                        "Pointer to an array with all the slaves you want to mke sure are present".
     @return                                                    "Returns the argument number if there is no matching argument it will return -1".
     */
    int64_t                     CLIGetNumMatchingOptions(CommandLineIO *CLI, const int64_t SwitchID, const int64_t NumSlaves, const int64_t *SlaveIDs);
    
    /*!
     @abstract                                                  "How many Master switches are present in the Options (will also check for slave switches if present)".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The Switch to look for".
     @param                     NumSlaves                       "How many slave Options should we look for"?
     @param                     SlaveIDs                        "Pointer to an array with all the slaves you want to mke sure are present".
     @return                                                    "Returns the argument number if there is no matching argument it will return -1".
     */
    int64_t                     CLIGetOptionNum(CommandLineIO *CLI, const int64_t SwitchID, const int64_t NumSlaves, const int64_t *SlaveIDs);
    
    /*!
     @abstract                                                  "Gets the data contained in Argument2Option".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "The option's result to return".
     @return                                                    "Returns the data after the switch, if the switch is resultless it will return 0".
     */
    UTF8                        CLIGetOptionResult(CommandLineIO const *CLI, const int64_t OptionID);
    
    /*!
     @abstract                                                  "Returns the extension from Path as a string".
     @param                     Path                            "The string to work on".
     */
    UTF8                       *GetExtensionFromPath(UTF8 *Path);
    
    /*!
     @abstract                                                  "Deallocates the instance of CommandLineIO pointed to by CLI".
     @param                     CLI                             "Pointer to the instance of CommandLineIO you want to delete".
     */
    void                        CommandLineIO_Deinit(CommandLineIO *CLI);
    
    void                        DEBUGCommandLineOptions(CommandLineIO *CLI);
    
#ifdef   __cplusplus
}
#endif

#endif   /* FoundationIO_CommandLineIO_H */
