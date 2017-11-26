/*!
 @header    CommandLineIO.h
 @author    Marcus Johnson
 @copyright 2017, Marcus Johnson
 @version   4.0.0
 @brief     This header contains code for parsing command line Options.
 */

#include <assert.h>     /* Included for static_assert */
#include <stdarg.h>     /* Included for the variadic argument support macros */
#include <stdbool.h>    /* Included for the bool type */
#include <stdint.h>     /* Included for the u/intX_t types */
#include <stdlib.h>     /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, and free */

#include "BitIOMacros.h"

#pragma  once

#ifndef  LIBBITIO_CommandLineIO_H
#define  LIBBITIO_CommandLineIO_H

#ifdef   __cplusplus
extern   "C" {
#endif
	
	/*!
	 @enum					CLISwitchTypes
	 @abstract										"Defines the type of switch".
	 @constant				UnknownSwitchType		"Unknown Switch type".
	 @constant				SwitchMayHaveSlaves		"The switch may have slaves, but it may not too".
	 @constant				SwitchCantHaveSlaves	"The switch can not have any slaves".
	 @constant				SwitchIsASlave			"The switch is a slave".
	 @constant				ExistentialSwitch		"The switch can not have any slaves, or any arguments, it just exists or doesn't exist".
	 */
	typedef enum CLISwitchTypes {
							UnknownSwitchType       = 0,
							SwitchMayHaveSlaves     = 1,
							SwitchCantHaveSlaves    = 2,
							SwitchIsASlave          = 3,
							ExistentialSwitch       = 4,
	} CLISwitchTypes;
	
	/*!
	 @typedef				CommandLineIO
	 @abstract										"Contains all the information, and relationships between switches and Options on the command line".
	 */
	typedef struct			CommandLineIO			CommandLineIO;
	
	/*!
	 @abstract										"Initializes a CommandLineIO instance".
	 @param					NumSwitches				"The number of CommandLineSwitch structures to initalize".
	 @return										"Returns a pointer to an initialized CommandLineIO instance".
	 */
	CommandLineIO			*CommandLineIO_Init(const int64_t NumSwitches);
	
	/*!
	 @abstract										"Sets the name of the program".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					Name					"Pointer to a C string containing the name of the program you're building".
	 */
	void					CLISetName(CommandLineIO *CLI, char *Name);
	
	/*!
	 @abstract										"Sets the name of the program".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					Version					"Pointer to a C string contining the version of the program you're building".
	 */
	void					CLISetVersion(CommandLineIO *CLI, char *Version);
	
	/*!
	 @abstract										"Sets the description of the program".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					Description				"Description of what the program does".
	 */
	void					CLISetDescription(CommandLineIO *CLI, char *Description);
	
	/*!
	 @abstract										"Sets the author of the program".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					Author					"Author of this program".
	 */
	void					CLISetAuthor(CommandLineIO *CLI, char *Author);
	
	/*!
	 @abstract										"Sets the copyright years of the program".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					Copyright				"The starting year this program was written dash (CURRENTYEAR)".
	 */
	void					CLISetCopyright(CommandLineIO *CLI, char *Copyright);
	
	/*!
	 @abstract										"Sets the license of the program".
	 @remark                                        "If your program is closed source, do NOT use the License options, use the EULA functions".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					Name					"What is the name of the license?".
	 @param					LicenseDescription		"Describe the license this program is licensed under".
	 @param					LicenseURL				"The actual URL for the license".
	 @param					IsProprietary			"Is this program released under an open source license, or a EULA?".
	 */
	void					CLISetLicense(CommandLineIO *CLI, char *Name, char *LicenseDescription, char *LicenseURL, const bool IsProprietary);
	
	/*!
	 @abstract										"What is the minimum number of switches your program needs to operate?".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					MinOptions				"The minimum number of switches".
	 */
	void					CLISetMinOptions(CommandLineIO *CLI, const int64_t MinOptions);
	
	/*!
	 @abstract				CLI						"CommandLineIO Pointer".
	 @param					HelpSwitch				"Which switch is the one to print all the Options"?
	 */
	void					CLISetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch);
	
	/*!
	 @abstract										"Sets SwitchID's flag in the CommandLineIO instance pointed by CLI".
	 @remark                                        "Just enter the number of characters you typed into the string not counting the quotes".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					SwitchID				"The switch to set".
	 @param					Flag					"The flag to identify an option with".
	 */
	void					CLISetSwitchFlag(CommandLineIO *CLI, const int64_t SwitchID, char *Flag);
	
	/*!
	 @abstract										"Sets SwitchDescription's flag in the CommandLineIO instance pointed by CLI".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					SwitchID				"The switch to set".
	 @param					Description				"Pointer to a C string containing the description of what this program does".
	 */
	void					CLISetSwitchDescription(CommandLineIO *CLI, const int64_t SwitchID, char *Description);
	
	/*!
	 @abstract										"Sets SwitchID's flag in the CommandLineIO instance pointed by CLI".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					SwitchID				"Which switch are we talking about?".
	 */
	void					CLISetSwitchType(CommandLineIO *CLI, int64_t SwitchID, CLISwitchTypes SwitchType);
	
	/*!
	 @abstract										"Sets MetaFlag switch as a meta flag for switch SwitchID".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					MasterID				"Which switch does the child/meta switch depend on?".
	 @param					Slave					"Which switch is the child switch?".
	 */
	void					CLISetSwitchAsSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t Slave);
	
	/*!
	 @abstract										"How many Slave switches can be active in a master argument at once"?
	 @param					CLI						"CommandLineIO Pointer".
	 @param					MasterID				"Which switch are we talking about?".
	 @param					MaxActiveSlaves			"How many Slave switches can be active in an argument at once"?
	 */
	void					CLISetSwitchMaxActiveSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxActiveSlaves);
	
	/*!
	 @abstract										"Parses argv for switches matching the ones contained in CLI".
	 @remark										"Argv[0] (the path for the original executable) is NEVER searched or used".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					argc					"Main's argc, for the number of Options entered".
	 @param					argv					"Main's argv, for the actual Options the user has entered".
	 */
	void					ParseCommandLineOptions(CommandLineIO *CLI, const int argc, const char *argv[]);
	
	/*!
	 @abstract										"How many matching options are present in CommandLineIO (will also check for slave switches if present)".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					SwitchID				"The switch to look for".
	 @param					NumSlaves				"How many slave Options should we look for"?
	 @param					SlaveIDs				"Pointer to an array with all the slaves you want to mke sure are present".
	 @return										"Returns the argument number if there is no matching argument it will return -1".
	 */
	int64_t 				CLIGetNumMatchingOptions(CommandLineIO *CLI, const int64_t SwitchID, const int64_t NumSlaves, const int64_t *SlaveIDs);
	
	/*!
	 @abstract										"How many Master switches are present in the Options (will also check for slave switches if present)".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					SwitchID				"The Switch to look for".
	 @param					NumSlaves				"How many slave Options should we look for"?
	 @param					SlaveIDs				"Pointer to an array with all the slaves you want to mke sure are present".
	 @return										"Returns the argument number if there is no matching argument it will return -1".
	 */
	int64_t					CLIGetOptionNum(CommandLineIO *CLI, const int64_t SwitchID, const int64_t NumSlaves, const int64_t *SlaveIDs);
	
	/*!
	 @abstract										"Gets the data contained in Argument2Option".
	 @param					CLI						"CommandLineIO Pointer".
	 @param					OptionID				"The option's result to return".
	 @return										"Returns the data after the switch, if the switch is resultless it will return 0".
	 */
	char					*CLIGetOptionResult(CommandLineIO const *CLI, const int64_t OptionID);
	
	/*!
	 @abstract										"Returns the extension from Path as a string".
	 @param					Path					"The string to work on".
	 */
	char					*GetExtensionFromPath(const char *Path);
	
	/*!
	 @abstract										"Deallocates the instance of CommandLineIO pointed to by CLI".
	 @param					CLI						"Pointer to the instance of CommandLineIO you want to delete".
	 */
	void					CommandLineIO_Deinit(CommandLineIO *CLI);
	
#ifdef   __cplusplus
}
#endif

#endif   /* LIBBITIO_CommandLineIO_H */
