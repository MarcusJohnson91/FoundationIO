/*!
 @header              CommandLineIO.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for creating a command line interface.
 @description
 Terminology:
  Arguments:          StringSet of unparsed data.
  Argument:           A String from StringSet.
  Switch:             Programmer defined behavior the user may or may not use.
  SwitchArgument:     The argument to a switch.
  Option:             User selected switches to direct the program to do something.
 */

#include "../PlatformIO.h"
#include "UnicodeTypes.h"

#pragma  once

#ifndef  FoundationIO_UnicodeIO_CommandLineIO_H
#define  FoundationIO_UnicodeIO_CommandLineIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     
     Switch Examples:
     
     --Input SwitchArgument=Path
     --Input:LeftEye=Path ; Switch=Input, Subswitch=LeftEye, SubswitchArgument=Path
     
     SwitchStates: SwitchState_Unknown, SwitchState_HasChildren, SwitchState_NoChildren
     Switch Name
     Switch Type:
     Existential: no arguments no children; e.g. Help
     Parent: Can only be the parent of another switch or standalone
     Child: Can only be the child of another switch
     
     SwitchStates: SwitchState_Unknown, SwitchState_Existential, SwitchState_Parent, SwitchState_Child
     
     
     */
    
    /*!
     @enum       CommandLineIO_SwitchTypes
     @abstract                                What kind of argument does the switch accept?
     @remark     Syntax:                      Parent:Child=SwitchArgument
     @constant   SwitchType_Unknown           Invalid argument type
     @constant   SwitchType_Existential       This switch type denotes that all that matters is if the switch exists; it has no parents or children, help is a good example.
     @constant   SwitchType_Standalone        This switch type denotes that the switch has no parent or child, tho it can take an argument unlike Existential.
     @constant   SwitchType_Parent            This switch type denotes that it can not have any parents, it has 1 or more children.
     @constant   SwitchType_Child             This switch type denotes that it can not be a parent
     */
    typedef enum CommandLineIO_SwitchTypes {
                 SwitchType_Unknown     = 0,
                 SwitchType_Required    = 1,
                 SwitchType_Existential = 1,
                 SwitchType_Standalone  = 2,
                 SwitchType_Parent      = 3,
                 SwitchType_Child       = 4,
    } CommandLineIO_SwitchTypes;
    
    /*!
     @enum       CommandLineIO_SwitchStatuses
     @abstract                                   "Defines the status of each Switch".
     @constant   SwitchStatus_Unknown            "Invalid Switch status".
     @constant   SwitchStatus_Required           "The Switch MUST be present".
     @constant   SwitchStatus_Optional           "The Switch MAY be present".
     @constant   SwitchStatus_Incompatible       "The Switch can only be present if none of it's incompatible Switch are present".
     */
    typedef enum CommandLineIO_SwitchStatuses {
                 SwitchStatus_Unknown            = 0,
                 SwitchStatus_Required           = 1,
                 SwitchStatus_Optional           = 2,
                 SwitchStatus_Incompatible       = 3,
    } CommandLineIO_SwitchStatuses;
    
    /*!
     @enum       CommandLineIO_SwitchArguments
     @abstract                                What kind of argument does the switch accept?
     @constant   ArgumentType_Unknown         Invalid argument type.
     @constant   ArgumentType_Path            String that matches the location of something in the filesystem.
     @constant   ArgumentType_URL             String that is composed of a URL like "https://www.tldrlegal.com/blah"
     @constant   ArgumentType_Range           Number between X and Y inclusively using the syntax X..Y.
     @constant   ArgumentType_Bool            String containing true, false, yes, no, 1, 0, with any capitalization.
     @constant   ArgumentType_String          Generic string that the program's author will further process.
     */
    typedef enum CommandLineIO_SwitchArguments {
                 ArgumentType_Unknown         = 0,
                 ArgumentType_Path            = 1,
                 ArgumentType_URL             = 2,
                 ArgumentType_Range           = 3,
                 ArgumentType_Bool            = 4,
                 ArgumentType_String          = 5,
    } CommandLineIO_SwitchArguments;
    
    /*!
     @enum       CommandLineIO_LicenseTypes
     @abstract                                   "Defines the types of licenses".
     @constant   LicenseType_Unknown             "Unknown Argument type".
     @constant   LicenseType_Permissive          "The Argument may have slaves, but it may not too".
     @constant   LicenseType_Proprietary         "The Argument can not have any slaves".
     @constant   LicenseType_Copyleft            "The Argument is a slave".
     */
    typedef enum CommandLineIO_LicenseTypes {
                 LicenseType_Unknown             = 0,
                 LicenseType_Permissive          = 1,
                 LicenseType_Proprietary         = 2,
                 LicenseType_Copyleft            = 3,
    } CommandLineIO_LicenseTypes;
    
    /*!
     @enum       CommandLineIO_ColorTypes
     @abstract                                   "Defines the types of colorizing".
     @constant   ColorType_Unknown               "Unknown colorize type".
     @constant   ColorType_Foreground            "Make this color the foreground".
     @constant   ColorType_Background            "Make this color the background".
     @constant   ColorType_Bold                  "Make this text bold".
     @constant   ColorType_Underlined            "Make this text underlined".
     */
    typedef enum CommandLineIO_ColorTypes {
                 ColorType_Unknown               = 0,
                 ColorType_Foreground            = 1,
                 ColorType_Background            = 2,
                 ColorType_Bold                  = 4,
                 ColorType_Underlined            = 8,
    } CommandLineIO_ColorTypes;
    
    /*!
     @typedef                   CommandLineIO
     @abstract                                                  "Contains all the information, and relationships between options and Options on the command line".
     */
    typedef struct              CommandLineIO                   CommandLineIO;
    
    /*!
     @abstract                                                  "Creates a CommandLineIO instance".
     @param                     NumSwitches                     "The number of CommandLineSwitch structures to initalize".
     @return                                                    "Returns a pointer to an initialized CommandLineIO instance".
     */
    CommandLineIO              *CommandLineIO_Init(uint64_t NumSwitches);
    
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
    UTF32                     **CommandLineIO_GetArgumentStringSet(void **Arguments);
    
    /*!
     @abstract                                                  "Sets the name of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Name                            "The name of the program you're building".
     */
    void                        CommandLineIO_SetName(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Name);
    
    /*!
     @abstract                                                  "Sets the version of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Version                         "The version of the program you're building".
     */
    void                        CommandLineIO_SetVersion(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Version);
    
    /*!
     @abstract                                                  "Sets the description of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Description                     "Description of what the program does".
     */
    void                        CommandLineIO_SetDescription(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Description);
    
    /*!
     @abstract                                                  "Sets the author of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Author                          "Author of this program".
     */
    void                        CommandLineIO_SetAuthor(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Author);
    
    /*!
     @abstract                                                  "Sets the copyright years of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     Copyright                       "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                        CommandLineIO_SetCopyright(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Copyright);
    
    /*!
     @abstract                                                  "Sets the license of the program".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     LicenseType                     "What type of license is this software released under"?
     @param                     Name                            "What is the name of the license"?
     @param                     LicenseURL                      "The actual URL for the license".
     */
    void                        CommandLineIO_SetLicense(CommandLineIO *CLI, CommandLineIO_LicenseTypes LicenseType, PlatformIO_Immutable(UTF32 *) Name, PlatformIO_Immutable(UTF32 *) LicenseURL);
    
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
     @param                     Strings                         "A pointer to a StringSet describing what progress is being tracked".
     @param                     Numerator                       "A pointer to a StringSet of X tasks that have been completed".
     @param                     Denominator                     "A pointer to a StringSet of Y thats that there are to do".
     */
    void                        CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, PlatformIO_Immutable(UTF32 **) Strings, PlatformIO_Immutable(uint64_t *) Numerator, PlatformIO_Immutable(uint64_t *) Denominator);
    
    /*!
     @abstract                                                  "Sets OptionID's flag in the CommandLineIO instance pointed by CLI".
     @remark                                                    "Just enter the number of characters you typed into the string not counting the quotes".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to set".
     @param                     Name                            "The flag to identify an option with".
     */
    void                        CommandLineIO_Switch_SetName(CommandLineIO *CLI, uint64_t OptionID, PlatformIO_Immutable(UTF32 *) Name);
    
    /*!
     @abstract                                                  "Sets OptionDescription's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to set".
     @param                     Description                     "Pointer to a UTF-8 encoded string containing the description of what this program does".
     */
    void                        CommandLineIO_Switch_SetDescription(CommandLineIO *CLI, uint64_t OptionID, PlatformIO_Immutable(UTF32 *) Description);
    
    /*!
     @abstract                                                  "Sets OptionID's flag in the CommandLineIO instance pointed by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "Which option are we talking about"?
     */
    void                        CommandLineIO_Switch_SetType(CommandLineIO *CLI, uint64_t OptionID, CommandLineIO_SwitchTypes OptionType);
    
    /*!
     @abstract                                                  "Sets OptionID as accepting arguments of only a certain type".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "the option to apply the ArgumentType to".
     @param                     ArgumentType                    "The argument type from CommandLineIO_SwitchArguments".
     */
    void                        CommandLineIO_Switch_SetArgumentType(CommandLineIO *CLI, uint64_t OptionID, CommandLineIO_SwitchArguments ArgumentType);
    
    /*!
     @abstract                                                  "Sets MetaFlag option as a meta flag for OptionID".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     MasterID                        "Which option does the child/meta option depend on"?
     @param                     Slave                           "Which option is the child option"?
     */
    void                        CommandLineIO_Switch_SetChild(CommandLineIO *CLI, uint64_t MasterID, uint64_t Slave);
    
    /*!
     @abstract                                                  "Parses the Command Line Options as UTF-8 encoded strings".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumArguments                    "The number of argument strings present in Arguments; equilivent to argc".
     @param                     Arguments                       "A StringSet of UTF-8 encoded arguments; equilivent to argv"
     */
    void                        CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, PlatformIO_Immutable(UTF8 **) Arguments);
    
    /*!
     @abstract                                                  "Parses the Command Line Options as UTF-16 encoded strings".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     NumArguments                    "The number of argument strings present in Arguments; equilivent to __argc".
     @param                     Arguments                       "A StringSet of UTF-16 encoded arguments; equilivent to __wargv on Windows"
     */
    void                        CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, PlatformIO_Immutable(UTF16 **) Arguments);
    
    /*!
     @abstract                                                  "How many Master options are present in the Options (will also check for slave options if present)".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     SwitchID                        "The Switch to look for".
     @param                     NumChildren                     "How many elements are in the ChildIDs array"?
     @param                     ChildIDs                        "Array of the the ChildIDs the option is required or allowed to have to match your search".
     @return                                                    "Returns the argument number if a match is found, otherwise -1".
     */
    uint64_t                    CommandLineIO_Option_GetOptionID(CommandLineIO *CLI, uint64_t SwitchID, uint64_t NumChildren, uint64_t *ChildIDs);
    
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
     @abstract                                                  "Gets a BoolString as a bool".
     @param                     CLI                             "CommandLineIO Pointer".
     @param                     OptionID                        "The Option's BoolString to convert".
     @return                                                    "Returns 1 for 1/yes/true, 0 for 0/no/false, -1 otherwise".
     */
    int8_t                      CommandLineIO_ConvertBoolString(CommandLineIO *CLI, uint64_t OptionID);
    
    /*!
     @abstract                                                  "Makes a string colorful".
     @param                     String                          "The text to colorize".
     @param                     ColorType                       "The color options".
     @param                     Red                             "The amount of red in the color,   0-255".
     @param                     Green                           "The amount of green in the color, 0-255".
     @param                     Blue                            "The amount of blue in the color,  0-255".
     @return                                                    "Returns the text but with color".
     */
    UTF8                       *CommandLineIO_UTF8_Colorize(PlatformIO_Immutable(UTF8 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue);
    
    /*!
     @abstract                                                  "Makes a string colorful".
     @param                     String                          "The text to colorize".
     @param                     ColorType                       "The color options".
     @param                     Red                             "The amount of red in the color,   0-255".
     @param                     Green                           "The amount of green in the color, 0-255".
     @param                     Blue                            "The amount of blue in the color,  0-255".
     @return                                                    "Returns the text but with color".
     */
    UTF16                      *CommandLineIO_UTF16_Colorize(PlatformIO_Immutable(UTF16 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue);
    
    /*!
     @abstract                                                  "Makes a string colorful".
     @param                     String                          "The text to colorize".
     @param                     ColorType                       "The color options".
     @param                     Red                             "The amount of red in the color,   0-255".
     @param                     Green                           "The amount of green in the color, 0-255".
     @param                     Blue                            "The amount of blue in the color,  0-255".
     @return                                                    "Returns the text but with color".
     */
    UTF32                      *CommandLineIO_UTF32_Colorize(PlatformIO_Immutable(UTF32 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue);
    
    /*!
     @abstract                                                  Strips color escape sequences from the string".
     @param                     String                          "The string to decolorize".
     @return                                                    "Returns the text without color".
     */
    UTF8                       *CommandLineIO_UTF8_Decolorize(PlatformIO_Immutable(UTF8 *) String);
    
    /*!
     @abstract                                                  Strips color escape sequences from the string.
     @param                     String                          The string to decolorize.
     @return                                                    Returns the text without color.
     */
    UTF16                      *CommandLineIO_UTF16_Decolorize(PlatformIO_Immutable(UTF16 *) String);
    
    /*!
     @abstract                                                  Strips color escape sequences from the string".
     @param                     String                          "The string to decolorize".
     @return                                                    "Returns the text without color".
     */
    UTF32                      *CommandLineIO_UTF32_Decolorize(PlatformIO_Immutable(UTF32 *) String);
    
    /*!
     @abstract                                                  "Deallocates the instance of CommandLineIO pointed to by CLI".
     @param                     CLI                             "CommandLineIO Pointer".
     */
    void                        CommandLineIO_Deinit(CommandLineIO *CLI);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_UnicodeIO_CommandLineIO_H */
