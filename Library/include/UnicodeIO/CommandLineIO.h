/*!
 @header    CommandLineIO.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2017, Marcus Johnson
 @version   2.0.0
 @brief     This header contains code related to parsing the command line arguments.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#pragma once

#ifndef LIBBITIO_CommandLineIO_H
#define LIBBITIO_CommandLineIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct        CommandLineSwitch         CommandLineSwitch;
    
    /*!
     @typedef             CommandLineArgument
     @abstract                                      "Contains the data to support a single switch".
     @constant            SwitchNum                 "Which switch is this argument?".
     @constant            NumChildSwitches           "How many meta switches are part of this argument?".
     @constant            ChildSwitches              "Pointer to an array that contains the numbers of the meta switches".
     @constant            ArgumentResult            "If there is a path or other result expected for this switch's argument, it'll be here".
     */
    typedef struct        CommandLineArgument       CommandLineArgument;
    
    typedef struct        CommandLineIO             CommandLineIO;
    
    /*!
     @abstract                                      "Initializes a CommandLineIO instance".
     @return                                        "Returns a pointer to an initialized CommandLineIO instance".
     @param               NumSwitches               "The number of CommandLineSwitch structures to initalize".
     */
    CommandLineIO        *InitCommandLineIO(const uint64_t NumSwitches);
    
    /*!
     @abstract                                      "Sets the name of the program".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               Name                      "Pointer to a C string containing the name of the program you're building"
     */
    void                  SetCLIName(CommandLineIO *CLI, char *Name);
    
    /*!
     @abstract                                      "Sets the name of the program".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               VersionString             "Pointer to a C string contining the version of the program you're building"
     */
    void                  SetCLIVersion(CommandLineIO *CLI, char *VersionString);
    
    /*!
     @abstract                                      "Sets the description of the program".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               Description               "Description of what the program does".
     */
    void                  SetCLIDescription(CommandLineIO *CLI, char *Description);
    
    /*!
     @abstract                                      "Sets the author of the program".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               Author                    "Author of this program".
     */
    void                  SetCLIAuthor(CommandLineIO *CLI, char *Author);
    
    /*!
     @abstract                                      "Sets the copyright years of the program".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               Copyright                 "The starting year this program was written dash (CURRENTYEAR)".
     */
    void                  SetCLICopyright(CommandLineIO *CLI, char *Copyright);
    
    /*!
     @abstract                                      "Sets the license of the program".
     @remark                                        "If your program is closed source, do NOT use the License options, use the EULA functions".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               Name                      "What is the name of the license?".
     @param               LicenseDescription        "Describe the license this program is licensed under".
     @param               IsProprietary             "Is this program released under an open source license, or a EULA?"
     */
    void                  SetCLILicense(CommandLineIO *CLI, char *Name, char *LicenseDescription, const bool IsProprietary);
    
    /*!
     @abstract                                      "Sets the URL for the license, in the main program banner".
     @remark                                        "If your program is closed source, do NOT use the License options, use the EULA functions".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               LicenseURL                "the actual URL for the license".
     */
    void                  SetCLILicenseURL(CommandLineIO *CLI, char *LicenseURL);
    
    /*!
     @abstract                                      "What is the minimum number of switches your program needs to operate?".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               MinSwitches               "The minimum number of switches".
     */
    void                  SetCLIMinSwitches(CommandLineIO *CLI, const uint64_t MinSwitches);
    
    /*!
     @abstract                                      "Sets SwitchNum's flag in the CommandLineIO instance pointed by CLI".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               Switch                    "Which switch are we talking about?"
     @param               IsMaster                  "Is Switch a master switch?"
     */
    void                  SetCLISwitchAsMaster(CommandLineIO *CLI, const uint64_t Switch, const bool IsMaster);
    
    /*!
     @abstract                                      "Sets SwitchNum's flag in the CommandLineIO instance pointed by CLI".
     @remark                                        "Just enter the number of characters you typed into the string not counting the quotes".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               SwitchNum                 "The switch to set".
     @param               Flag                      "The flag to identify an option with".
     */
    void                  SetCLISwitchFlag(CommandLineIO *CLI, const uint64_t SwitchNum, char *Flag);
    
    /*!
     @abstract                                      "Sets MetaFlag switch as a meta flag for switch SwitchNum".
     @param               CLI                       "Pointer to CommandLineIO".
     @param               ParentSwitch              "Which switch does the child/meta switch depend on?".
     @param               ChildSwitch               "Which switch is the child switch?".
     */
    void                  SetCLISwitchAsChild(CommandLineIO *CLI, const uint64_t ParentSwitch, const uint64_t ChildSwitch);
    
    /*!
     @abstract                                      "Sets SwitchDescription's flag in the CommandLineIO instance pointed by CLI".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               SwitchNum                 "The switch to set".
     @param               Description               "Pointer to a C string containing the description of what this program does"
     */
    void                  SetCLISwitchDescription(CommandLineIO *CLI, const uint64_t SwitchNum, char *Description);
    
    /*!
     @abstract                                      "Sets SwitchResult's flag in the CommandLineIO instance pointed by CLI".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               SwitchNum                 "The switch to set".
     @param               IsThereAResult            "Are you expecting this switch to contain data, or are you just testing for it's presence?".
     */
    void                  SetCLISwitchResultStatus(CommandLineIO *CLI, const uint64_t SwitchNum, const bool IsThereAResult);
    
    /*!
     @abstract                                      "Parses argv for switches matching the ones contained in CLI".
     @remark                                        "Argv[0] (the path for the original executable) is NEVER searched or used".
     @param               CLI                       "Pointer to CommandLineIO".
     @param               argc                      "Main's argc, for the number of arguments entered".
     @param               argv                      "Main's argv, for the actual arguments the user has entered".
     */
    void                  ParseCommandLineArguments(CommandLineIO *CLI, int argc, const char *argv[]);
    
    /*!
     @abstract                                      "Parses CommandLine arguments and counts the copies of a given switch found".
     @param               CLI                       "Pointer to CommandLineIO".
     @param               Switch                    "Which switch should we look for?".
     */
    uint64_t              GetCLINumArgumentsMatchingSwitch(CommandLineIO *CLI, const uint64_t Switch);
    
    /*!
     @abstract                                      "Finds the argument that has both ParentSwitch and ChildSwitch present"
     @return                                        "If no argument is found with that switch set, 0xFFFFFFFFFFFFFFFF is returned as the invalid result".
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               ParentSwitch              "The switch MetaSwitch should be in"
     @param               ChildSwitch               "MetaSwitch to find in the arguments"
     */
    uint64_t              GetCLIChildSwitchArgument(CommandLineIO *CLI, const uint64_t ParentSwitch, const uint64_t ChildSwitch);
    
    /*!
     @return              Returns the switch number if it was found, if it was not, it returns 0xFFFFFFFFFFFFFFFF
     */
    uint64_t              GetCLIArgumentNumFromFlag(CommandLineIO *CLI, const char *Flag);
    
    /*!
     @abstract                                      "Gets the data contained in Switch->Result"
     @return                                        "Returns the data after the switch, if the switch is resultless it will return 0"
     @param               CLI                       "Pointer to the instance of CommandLineIO".
     @param               ArgumentNum               "The argument's result to return".
     */
    char                 *GetCLIArgumentResult(CommandLineIO *CLI, const uint64_t ArgumentNum);
    
    /*!
     @abstract                                      "Deallocates the instance of CommandLineIO pointed to by CLI".
     @param               CLI                       "Pointer to the instance of CommandLineIO you want to delete".
     */
    void                  DeinitCommandLineIO(CommandLineIO *CLI);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_CommandLineIO_H */
