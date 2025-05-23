/*!
 @header:                  CommandLineIO.h
 @author:                  Marcus Johnson
 @copyright:               2017+
 @version:                 1.0.0
 @SPDX-License-Identifier: Apache-2.0
 @brief:                   This header contains code for creating a command line interface.
 @terminology
  Arguments:               StringSet of unparsed data.
  Argument:                A String from StringSetIO.
  Switch:                  Programmer defined behavior the user may or may not use.
  SwitchArgument:          The argument to a switch.
  Option:                  User selected switches to direct the program to do something.
 */

#pragma once

#ifndef  FoundationIO_TextIO_CommandLineIO_H
#define  FoundationIO_TextIO_CommandLineIO_H

#include "TextIOTypes.h"    /* Included for Text types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     Switch Examples:
     
     --Input SwitchArgument=Path
     --Input:LeftEye=Path ; Switch=Input, Subswitch=LeftEye, SubswitchArgument=Path
     
     SwitchStates: SwitchState_Unspecified, SwitchState_HasChildren, SwitchState_NoChildren
     Switch Name
     Switch Type:
     Existential: no arguments no children; e.g. Help
     Parent: Can only be the parent of another switch or standalone
     Child: Can only be the child of another switch
     
     SwitchStates: SwitchState_Unspecified, SwitchState_Existential, SwitchState_Parent, SwitchState_Child
     */
    
    /*!
     @enum         CommandLineIO_SwitchTypes
     @abstract                                     What kind of argument does the switch accept?
     @remark       Syntax:                         Parent:Child=SwitchArgument
     @constant     SwitchType_Unspecified          Invalid argument type
     @constant     SwitchType_Existential          This switch can not have parents, children, or arguments.
     @constant     SwitchType_Standalone           This switch can not have parents or children, tho it can accept arguments.
     @constant     SwitchType_Parent               This switch type denotes that it can not have any parents, it has 1 or more children.
     @constant     SwitchType_Child                This switch type denotes that it can not be a parent
     */
    typedef enum C
    SwitchTypes : uint8_t {
                   SwitchType_Unspecified          = 0,
                   SwitchType_Required             = 1,
                   SwitchType_Existential          = 1,
                   SwitchType_Standalone           = 2,
                   SwitchType_Parent               = 3,
                   SwitchType_Child                = 4,
    } SwitchTypes;
    
    /*!
     @enum         SwitchStatuses
     @abstract                                     Defines the status of each Switch.
     @constant     SwitchStatus_Unspecified        Invalid Switch status.
     @constant     SwitchStatus_Required           The Switch MUST be present.
     @constant     SwitchStatus_Optional           The Switch MAY be present.
     @constant     SwitchStatus_Incompatible       The Switch can only be present if none of it's incompatible Switch are present.
     */
    typedef enum SwitchStatuses : uint8_t {
                   SwitchStatus_Unspecified        = 0,
                   SwitchStatus_Required           = 1,
                   SwitchStatus_Optional           = 2,
                   SwitchStatus_Incompatible       = 3,
    } CommandLineIO_SwitchStatuses;
    
    /*!
     @enum         SwitchArguments
     @abstract                                     What kind of argument does the switch accept?
     @constant     SwitchArgument_Unspecified      Invalid argument type.
     @constant     SwitchArgument_Path             String that matches the location of something in the filesystem.
     @constant     SwitchArgument_URL              String that is composed of a URL like "https://www.tldrlegal.com/blah"
     @constant     SwitchArgument_Range            Number between X and Y inclusively using the syntax X..Y.
     @constant     SwitchArgument_Bool             String containing true, false, yes, no, 1, 0, with any capitalization.
     @constant     SwitchArgument_String           Generic string that the program's author will further process.
     */
    typedef enum SwitchArguments : uint8_t {
                   SwitchArgument_Unspecified      = 0,
                   SwitchArgument_Path             = 1,
                   SwitchArgument_URL              = 2,
                   SwitchArgument_Range            = 3,
                   SwitchArgument_Bool             = 4,
                   SwitchArgument_String           = 5,
    } SwitchArguments;
    
    /*!
     @enum         LicenseTypes
     @abstract                                     Defines the types of licenses.
     @constant     LicenseType_Unspecified         Unknown Argument type.
     @constant     LicenseType_Permissive          The Argument may have slaves, but it may not too.
     @constant     LicenseType_Proprietary         The Argument can not have any slaves.
     @constant     LicenseType_Copyleft            The Argument is a slave.
     */
    typedef enum LicenseTypes : uint8_t {
                   LicenseType_Unspecified         = 0,
                   LicenseType_Permissive          = 1,
                   LicenseType_Proprietary         = 2,
                   LicenseType_Copyleft            = 3,
    } LicenseTypes;
    
    /*!
     @enum         ColorTypes
     @abstract                                     Defines the types of colorizing.
     @constant     ColorType_Unspecified           Unknown colorize type.
     @constant     ColorType_Foreground            Make this color the foreground.
     @constant     ColorType_Background            Make this color the background.
     @constant     ColorType_Bold                  Make this text bold.
     @constant     ColorType_Underlined            Make this text underlined.
     */
    typedef enum ColorTypes : uint8_t {
                   ColorType_Unspecified           = 0,
                   ColorType_Foreground            = 1,
                   ColorType_Background            = 2,
                   ColorType_Bold                  = 4,
                   ColorType_Underlined            = 8,
    } ColorTypes;
    
    /*!
     @typedef      CommandLineIO
     @abstract                                     Contains all the information, and relationships between Arguments and Options on the command line.
     */
    typedef struct CommandLineIO                   CommandLineIO;
    
    /*!
     @abstract                                     Creates a CommandLineIO instance.
     @param        NumSwitches                     The number of CommandLineSwitch structures to initalize.
     @return                                       Returns a pointer to an initialized CommandLineIO instance.
     */
    CommandLineIO *CommandLineIO_Init(size_t NumSwitches);
    
    /*!
     @abstract                                     Creating a Text UI.
     @return                                       Returns the width of the terminal in codepoints.
     */
    size_t         GetTerminalWidth(void);
    
    /*!
     @abstract                                     Creating a Text UI.
     @return                                       Returns the height of the terminal in codepoints.
     */
    size_t         GetTerminalHeight(void);
    
    /*!
     @abstract                                     Creating a Text UI.
     @return                                       Returns if the terminal has been resized, and therefore needs to be redrawn.
     */
    bool           TerminalWasResized(void);
    
    /*!
     @abstract                                     Helper function to cast argc to size_t and handle Windows' globals.
     @return                                       Returns the number of arguments.
     */
    size_t         GetNumArguments(int argc);
    
    /*!
     @abstract                                     Helper function to decode argv/wargv to UTF32 for use with parsing.
     @return                                       Returns the decoded Arguments.
     */
    UTF32        **GetArgumentStringSet(void **Arguments);
    
    /*!
     @abstract                                     Sets the name of the program.
     @param        CLI                             CommandLineIO Pointer.
     @param        Name                            The name of the program you're building.
     */
    void           CommandLineIO_SetName(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Name);
    
    /*!
     @abstract                                     Sets the version of the program.
     @param        CLI                             CommandLineIO Pointer.
     @param        Version                         The version of the program you're building.
     */
    void           CommandLineIO_SetVersion(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Version);
    
    /*!
     @abstract                                     Sets the description of the program.
     @param        CLI                             CommandLineIO Pointer.
     @param        Description                     Description of what the program does.
     */
    void           CommandLineIO_SetDescription(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Description);
    
    /*!
     @abstract                                     Sets the author of the program.
     @param        CLI                             CommandLineIO Pointer.
     @param        Author                          Author of this program.
     */
    void           CommandLineIO_SetAuthor(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Author);
    
    /*!
     @abstract                                     Sets the copyright years of the program.
     @param        CLI                             CommandLineIO Pointer.
     @param        Copyright                       The starting year this program was written dash (CURRENTYEAR).
     */
    void           CommandLineIO_SetCopyright(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Copyright);
    
    /*!
     @abstract                                     Sets the license of the program.
     @param        CLI                             CommandLineIO Pointer.
     @param        LicenseType                     What type of license is this software released under?
     @param        Name                            What is the name of the license?
     @param        LicenseURL                      The actual URL for the license.
     */
    void           CommandLineIO_SetLicense(CommandLineIO *CLI, LicenseTypes LicenseType, PlatformIO_Immutable(UTF32 *) Name, PlatformIO_Immutable(UTF32 *) LicenseURL);
    
    /*!
     @abstract                                     What is the minimum number of options your program needs to operate?
     @param        CLI                             CommandLineIO Pointer.
     @param        MinOptions                      The minimum number of options.
     */
    void           CommandLineIO_SetMinOptions(CommandLineIO *CLI, size_t MinOptions);
    
    /*!
     @abstract                                     Sets the help switch.
     @param        CLI                             CommandLineIO Pointer.
     @param        HelpOption                      Which option is the one to get help?
     */
    void           CommandLineIO_SetHelpOption(CommandLineIO *CLI, size_t HelpOption);
    
    /*!
     @abstract                                     Displays on the screen the progress of X actions that are taking place
     @param        CLI                             CommandLineIO Pointer.
     @param        NumItems2Display                The number of things you want to show the progress of.
     @param        Strings                         A pointer to a StringSet describing what progress is being tracked.
     @param        Numerator                       A pointer to a StringSet of X tasks that have been completed.
     @param        Denominator                     A pointer to a StringSet of Y thats that there are to do.
     */
    void           CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, PlatformIO_Immutable(UTF32 **)  Strings, PlatformIO_Immutable(uint64_t *) Numerator, PlatformIO_Immutable(uint64_t *) Denominator);
    
    /*!
     @abstract                                     Sets OptionID's flag in the CommandLineIO instance pointed by CLI.
     @remark                                       Just enter the number of characters you typed into the string not counting the quotes.
     @param        CLI                             CommandLineIO Pointer.
     @param        OptionID                        The option to set.
     @param        Name                            The flag to identify an option with.
     */
    void           CommandLineIO_Switch_SetName(CommandLineIO *CLI, size_t OptionID, PlatformIO_Immutable(UTF32 *) Name);
    
    /*!
     @abstract                                     Sets OptionDescription's flag in the CommandLineIO instance pointed by CLI.
     @param        CLI                             CommandLineIO Pointer.
     @param        OptionID                        The option to set.
     @param        Description                     Pointer to a UTF-8 encoded string containing the description of what this program does.
     */
    void           CommandLineIO_Switch_SetDescription(CommandLineIO *CLI, size_t OptionID, PlatformIO_Immutable(UTF32 *) Description);
    
    /*!
     @abstract                                     Sets OptionID's flag in the CommandLineIO instance pointed by CLI.
     @param        CLI                             CommandLineIO Pointer.
     @param        OptionID                        Which option are we talking about?
     @param        SwitchType                      What kind of switch is this?
     */
    void           Switch_SetType(CommandLineIO *CLI, size_t OptionID, SwitchTypes SwitchType);
    
    /*!
     @abstract                                     Sets OptionID as accepting arguments of only a certain type.
     @param        CLI                             CommandLineIO Pointer.
     @param        OptionID                        The option to apply the ArgumentType to.
     @param        SwitchArgument                  The argument type from CommandLineIO_SwitchArguments.
     */
    void           Switch_SetArgumentType(CommandLineIO *CLI, size_t OptionID, SwitchArguments SwitchArgument);
    
    /*!
     @abstract                                     Sets MetaFlag option as a meta flag for OptionID.
     @param        CLI                             CommandLineIO Pointer.
     @param        MasterID                        Which option does the child/meta option depend on?
     @param        Slave                           Which option is the child option?
     */
    void         Switch_SetChild(CommandLineIO *CLI, size_t MasterID, size_t Slave);
    
    /*!
     @abstract                                     Parses the Command Line Options as UTF-8 encoded strings.
     @param        CLI                             CommandLineIO Pointer.
     @param        NumArguments                    The number of argument strings present in Arguments; equivalent to argc.
     @param        Arguments                       A StringSet of UTF-8 encoded arguments; equivalent to argv
     */
    void           UTF8_ParseOptions(CommandLineIO *CLI, size_t NumArguments, PlatformIO_Immutable(UTF8 **) Arguments);
    
    /*!
     @abstract                                     Parses the Command Line Options as UTF-16 encoded strings.
     @param        CLI                             CommandLineIO Pointer.
     @param        NumArguments                    The number of argument strings present in Arguments; equivalent to __argc.
     @param        Arguments                       A StringSet of UTF-16 encoded arguments; equivalent to __wargv on Windows
     */
    void           UTF16_ParseOptions(CommandLineIO *CLI, size_t NumArguments, PlatformIO_Immutable(UTF16 **) Arguments);
    
    /*!
     @abstract                                     How many Master options are present in the Options (will also check for slave options if present).
     @param        CLI                             CommandLineIO Pointer.
     @param        SwitchID                        The Switch to look for.
     @param        NumChildren                     How many elements are in the ChildIDs array?
     @param        ChildIDs                        Array of the the ChildIDs the option is required or allowed to have to match your search.
     @return                                       Returns the argument number if a match is found, otherwise -1.
     */
    size_t         Option_GetOptionID(CommandLineIO *CLI, size_t SwitchID, size_t NumChildren, size_t *ChildIDs);
    
    /*!
     @abstract                                     Gets the data contained in Argument2Option.
     @param        CLI                             CommandLineIO Pointer.
     @param        OptionID                        The option's result to return.
     @return                                       Returns the data after the option, if the option is resultless it will return 0.
     */
    UTF8          *UTF8_GetOptionResult(CommandLineIO *CLI, size_t OptionID);
    
    /*!
     @abstract                                     Gets the data contained in Argument2Option.
     @param        CLI                             CommandLineIO Pointer.
     @param        OptionID                        The option's result to return.
     @return                                       Returns the data after the option, if the option is resultless it will return 0.
     */
    UTF16         *UTF16_GetOptionResult(CommandLineIO *CLI, size_t OptionID);
    
    /*!
     @abstract                                     Gets a BoolString as a bool.
     @param        CLI                             CommandLineIO Pointer.
     @param        OptionID                        The Option's BoolString to convert.
     @return                                       Returns 1 for 1/yes/true, 0 for 0/no/false, -1 otherwise.
     */
    int8_t         ConvertBoolString(CommandLineIO *CLI, size_t OptionID);
    
    /*!
     @abstract                                     Makes a string colorful.
     @param        String                          The text to colorize.
     @param        ColorType                       The color options.
     @param        Red                             The amount of red in the color,   0-255.
     @param        Green                           The amount of green in the color, 0-255.
     @param        Blue                            The amount of blue in the color,  0-255.
     @return                                       Returns the text but with color.
     */
    UTF8          *UTF8_Colorize(PlatformIO_Immutable(UTF8 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue);
    
    /*!
     @abstract                                     Makes a string colorful.
     @param        String                          The text to colorize.
     @param        ColorType                       The color options.
     @param        Red                             The amount of red in the color,   0-255.
     @param        Green                           The amount of green in the color, 0-255.
     @param        Blue                            The amount of blue in the color,  0-255.
     @return                                       Returns the text but with color.
     */
    UTF16         *UTF16_Colorize(PlatformIO_Immutable(UTF16 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue);
    
    /*!
     @abstract                                     Makes a string colorful.
     @param        String                          The text to colorize.
     @param        ColorType                       The color options.
     @param        Red                             The amount of red in the color,   0-255.
     @param        Green                           The amount of green in the color, 0-255.
     @param        Blue                            The amount of blue in the color,  0-255.
     @return                                       Returns the text but with color.
     */
    UTF32         *UTF32_Colorize(PlatformIO_Immutable(UTF32 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue);
    
    /*!
     @abstract                                     Strips color escape sequences from the string.
     @param        String                          The string to decolorize.
     @return                                       Returns the text without color.
     */
    UTF8          *UTF8_Decolorize(PlatformIO_Immutable(UTF8 *) String);
    
    /*!
     @abstract                                     Strips color escape sequences from the string.
     @param        String                          The string to decolorize.
     @return                                       Returns the text without color.
     */
    UTF16         *UTF16_Decolorize(PlatformIO_Immutable(UTF16 *) String);
    
    /*!
     @abstract                                     Strips color escape sequences from the string.
     @param        String                          The string to decolorize.
     @return                                       Returns the text without color.
     */
    UTF32         *UTF32_Decolorize(PlatformIO_Immutable(UTF32 *) String);
    
    /*!
     @abstract                                     Deallocates the instance of CommandLineIO pointed to by CLI.
     @param        CLI                             CommandLineIO Pointer.
     */
    void           CommandLineIO_Deinit(CommandLineIO *CLI);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_TextIO_CommandLineIO_H */
