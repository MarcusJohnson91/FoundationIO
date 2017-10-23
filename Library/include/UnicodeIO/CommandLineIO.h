/*!
 @header    CommandLineIO.h
 @author    Marcus Johnson
 @copyright 2017, Marcus Johnson
 @version   3.0.0
 @brief     This header contains code for parsing command line arguments.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#pragma once

#ifndef LIBBITIO_CommandLineIO_H
#define LIBBITIO_CommandLineIO_H

#ifdef __cplusplus
extern "C" {
#endif
	
	/*!
	 @typedef             CommandLineIO
	 @abstract                                      "Contains all the information, and relationships between switches on the command line".
	 */
	typedef struct        CommandLineIO             CommandLineIO;
	
	/*!
	 @abstract                                      "Initializes a CommandLineIO instance".
	 @return                                        "Returns a pointer to an initialized CommandLineIO instance".
	 @param               NumSwitches               "The number of CommandLineSwitch structures to initalize".
	 */
	CommandLineIO        *InitCommandLineIO(const uint64_t NumSwitches);
	
	/*!
	 @abstract                                      "Sets the name of the program".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Name                      "Pointer to a C string containing the name of the program you're building".
	 */
	void                  SetCLIName(CommandLineIO *CLI, char *Name);
	
	/*!
	 @abstract                                      "Sets the name of the program".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               VersionString             "Pointer to a C string contining the version of the program you're building".
	 */
	void                  SetCLIVersion(CommandLineIO *CLI, char *VersionString);
	
	/*!
	 @abstract                                      "Sets the description of the program".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Description               "Description of what the program does".
	 */
	void                  SetCLIDescription(CommandLineIO *CLI, char *Description);
	
	/*!
	 @abstract                                      "Sets the author of the program".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Author                    "Author of this program".
	 */
	void                  SetCLIAuthor(CommandLineIO *CLI, char *Author);
	
	/*!
	 @abstract                                      "Sets the copyright years of the program".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Copyright                 "The starting year this program was written dash (CURRENTYEAR)".
	 */
	void                  SetCLICopyright(CommandLineIO *CLI, char *Copyright);
	
	/*!
	 @abstract                                      "Sets the license of the program".
	 @remark                                        "If your program is closed source, do NOT use the License options, use the EULA functions".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Name                      "What is the name of the license?".
	 @param               LicenseDescription        "Describe the license this program is licensed under".
	 @param               LicenseURL                "the actual URL for the license".
	 @param               IsProprietary             "Is this program released under an open source license, or a EULA?".
	 */
	void                  SetCLILicense(CommandLineIO *CLI, char *Name, char *LicenseDescription, char *LicenseURL, const bool IsProprietary);
	
	/*!
	 @abstract                                      "What is the minimum number of switches your program needs to operate?".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               MinSwitches               "The minimum number of switches".
	 */
	void                  SetCLIMinSwitches(CommandLineIO *CLI, const uint64_t MinSwitches);
	
	/*!
	 @abstract                                      "Sets SwitchNum's flag in the CommandLineIO instance pointed by CLI".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Switch                    "Which switch are we talking about?".
	 */
	void                  SetCLISwitchAsIndependent(CommandLineIO *CLI, const uint64_t Switch);
	
	/*!
	 @abstract                                      "How many dependent switches can be active in a master argument at once"?
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Master                    "Which switch are we talking about?".
	 @param               MaxActiveDependents       "How many dependent switches can be active in an argument at once"?
	 */
	void                  SetCLISwitchMaxActiveDependents(CommandLineIO *CLI, const uint64_t Master, const uint64_t MaxActiveDependents);
	
	/*!
	 @abstract                                      "Sets MetaFlag switch as a meta flag for switch SwitchNum".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Master                    "Which switch does the child/meta switch depend on?".
	 @param               Dependent                 "Which switch is the child switch?".
	 */
	void                  SetCLISwitchAsDependent(CommandLineIO *CLI, const uint64_t Master, const uint64_t Dependent);
	
	/*!
	 @abstract                                      "Sets SwitchNum's flag in the CommandLineIO instance pointed by CLI".
	 @remark                                        "Just enter the number of characters you typed into the string not counting the quotes".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               SwitchNum                 "The switch to set".
	 @param               Flag                      "The flag to identify an option with".
	 */
	void                  SetCLISwitchFlag(CommandLineIO *CLI, const uint64_t SwitchNum, char *Flag);
	
	/*!
	 @abstract                                      "Sets SwitchDescription's flag in the CommandLineIO instance pointed by CLI".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               SwitchNum                 "The switch to set".
	 @param               Description               "Pointer to a C string containing the description of what this program does".
	 */
	void                  SetCLISwitchDescription(CommandLineIO *CLI, const uint64_t SwitchNum, char *Description);
	
	void                  SetCLIHelpSwitch(CommandLineIO *CLI, const uint64_t HelpSwitch);
	
	/*!
	 @abstract                                      "Parses argv for switches matching the ones contained in CLI".
	 @remark                                        "Argv[0] (the path for the original executable) is NEVER searched or used".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               argc                      "Main's argc, for the number of arguments entered".
	 @param               argv                      "Main's argv, for the actual arguments the user has entered".
	 */
	void                  ParseCommandLineArguments(CommandLineIO *CLI, const int argc, const char *argv[]);
	
	/*!
	 @abstract                                      "Finds the argument that contains SwitchNum".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Switch                    "The switch number to look for".
	 @return                                        "Returns the argument, or if it wasn't found, returns 0xFFFFFFFFFFFFFFFF".
	 */
	uint64_t              GetCLIArgumentNumFromSwitch(CommandLineIO const *CLI, const uint64_t Switch);
	
	/*!
	 @abstract                                      "Gets the data contained in Switch->Result".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Argument                  "The argument's result to return".
	 @return                                        "Returns the data after the switch, if the switch is resultless it will return 0".
	 */
	char                 *GetCLIArgumentResult(CommandLineIO const *CLI, const uint64_t Argument);
	
	/*!
	 @abstract                                      "Is SwitchNum present in the Argument list?".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Switch                    "The Switch to look for".
	 @return                                        "Returns the data after the switch, if the switch is resultless it will return 0".
	 */
	bool                  GetCLIArgumentPresenceFromSwitch(CommandLineIO const *CLI, const uint64_t Switch);
	
	/*!
	 @abstract										"Gets the number of Arguments in CLI that contain an Independent switch with all Dependents specified".
	 */
	uint64_t              GetCLINumArgumentsWithIndependentAndDependents(CommandLineIO *CLI, const uint64_t Independent, const uint64_t NumDependents, ...);
	
	/*!
	 @abstract										"Returns the argument number that matches all the switches in the parameters".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               Independent               "The Independent switch to look for".
	 @param               NumDependents				"MUST start at 1".
	 @param               ...						"A variable list of dependent arguments to check, all must be present to count".
	 */
	uint64_t              GetCLIArgumentNumWithIndependentAndDependents(CommandLineIO *CLI, const uint64_t Independent, const uint64_t NumDependents, ...);
	
	/*!
	 @abstract                                      "Extracts the path of the argument that contains both the parent and child switch, lazily evaluated".
	 @param               CLI                       "Pointer to CommandLineIO".
	 @param               ParentSwitch              "The switch that should be the lead switch".
	 @param               ChildSwitch               "The switch that is dependent on the parent switch".
	 @return                                        "Returns a pointer to a string contained by the child switch of this pair".
	 */
	char                 *GetCLIArgumentResultFromParentChildSwitch(CommandLineIO const *CLI, const uint64_t ParentSwitch, const uint64_t ChildSwitch);
	
	/*!
	 @abstract                                      "Deallocates the instance of CommandLineIO pointed to by CLI".
	 @param               CLI                       "Pointer to the instance of CommandLineIO you want to delete".
	 */
	void                  DeinitCommandLineIO(CommandLineIO *CLI);
	
#ifdef __cplusplus
}
#endif

#endif /* LIBBITIO_CommandLineIO_H */
