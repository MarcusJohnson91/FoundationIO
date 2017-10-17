#include "../include/BitIO.h"
#include "../include/CommandLineIO.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#ifdef __cplusplus
extern "C" {
#endif
	
	/*
	 We need to map the relationship between the different switches in order to extract the arguments properly.
	 There are 2 types of switches. Idk what to name them, but the first type is standalone, and the second is only allowed to exist in a contexted created by the standalone one.
	 
	 Non-standalone switches can not have results, they just provide further information to the standalone ones.
	 All switches have a flag to identify them.
	 Each standalone switch should have a variable saying the maximum number of concurrent non-stanalone switches.
	 All switches should have a bool saying if they are a standalone switch or not.
	 Is a switch is not standalone, it can not have a result.
	 btw what should we call the "result" aka argument to the switch
	 */
	
	/*!
	 @struct                 CommandLineSwitch
	 @abstract                                         "Contains the data to support a single switch".
	 @constant               IsMasterSwitch            "Does this switch depend on any others?"
	 @constant               SwitchHasResult           "Does this switch take any arguments, or is it's mere presence all that matters?".
	 @constant               FlagSize                  "What is the number of bytes (if ASCII)/ code points (if UTF) of this switch?".
	 @constant               NumChildSwitches          "How many child switches are there in ChildSwitches?".
	 @constant               ChildSwitches             "Pointer to an array that contains the numbers of the child switches".
	 @constant               Flag                      "What is this switch called, how do we identify it?".
	 @constant               SwitchDescription         "Describe to the user what this switch does".
	 */
	typedef struct CommandLineSwitch {
		bool                 IsMasterSwitch;
		bool                 SwitchHasResult;
		uint64_t             FlagSize;
		uint64_t             MaxChildSwitches;
		uint64_t             NumChildSwitches;
		uint64_t            *ChildSwitches;
		char                *Flag;
		char                *SwitchDescription;
	} CommandLineSwitch;
	
	/*!
	 @struct                 CommandLineArgument
	 @abstract                                         "Contains the data to support a single argument".
	 @constant               SwitchNum                 "Which switch does this argument correspond to"?
	 @constant               NumDependentArguments     "How many child arguments were found in this argument?".
	 @constant               DependentSwitches         "Array of child argument numbers, to look up in CLI->Switches".
	 @constant               ArgumentResult            "If there is a path or other result expected for this switch's argument, it'll be here".
	 */
	typedef struct CommandLineArgument {
		uint64_t             SwitchNum;
		uint64_t             NumDependentArguments;
		uint64_t            *DependentSwitches;
		char                *ArgumentResult;
	} CommandLineArgument;
	
	/*!
	 @struct                 CommandLineSwitch2
	 @abstract                                         "Contains the data to support a switch".
	 @constant               IsMasterSwitch            "Can this switch have dependent arguments"?
	 @constant               SwitchFlag                "What is this switch called, how do we identify it?".
	 @constant               SwitchFlagSize            "What is the number of bytes (if ASCII)/ code points (if UTF) of this switch?".
	 @constant               SwitchDescription         "Describe to the user what this switch does".
	 @constant               MaxActiveDependents       "How many dependent switches can be active at once"?
	 @constant               AllowableDependents       "Pointer to an array that contains the list of aloowable dependent switches".
	 */
	typedef struct CommandLineSwitch2 {
		bool                 IsMasterSwitch;
		uint8_t              SwitchFlagSize;
		char                *SwitchFlag;
		char                *SwitchDescription;
		uint64_t             MaxActiveDependents;
		uint64_t             NumDependentSwitches;
		uint64_t            *AllowableDependents;
	} CommandLineSwitch2;
	
	/*!
	 @struct                 CommandLineArgument
	 @abstract                                         "Contains the data to support a single argument".
	 @constant               SwitchNum                 "Which switch does this argument correspond to"?
	 @constant               NumDependentArguments     "How many child arguments were found in this argument?".
	 @constant               DependentArguments        "Array of child argument numbers, to look up in CLI->Switches".
	 @constant               ArgumentResult            "If there is a path or other result expected for this switch's argument, it'll be here".
	 */
	typedef struct CommandLineArgument2 {
		uint64_t            *Switch;
		uint64_t             NumDependentArguments;
		uint64_t            *DependentArguments;
		char                *ArgumentResult;
	} CommandLineArgument2;
	
	/*!
	 @struct                 CommandLineIO
	 @abstract                                         "Contains all the information, and relationships between switches on the command line".
	 @constant               NumSwitches               "How many switches are there?".
	 @constant               MinSwitches               "The minimum number of switches to accept without dumping the help".
	 @constant               NumArguments              "The number of arguments present in argv, after extracting any child switches".
	 @constant               HelpSwitch                "Which switch displays the help"?
	 @constant               Switches                  "Pointer to an array of switches".
	 @constant               Arguments                 "Pointer to an array of arguments".
	 @constant               IsProprietary             "Is this program proprietary?".
	 @constant               ProgramName               "What is the name of this program?".
	 @constant               ProgramAuthor             "Who wrote this program?".
	 @constant               ProgramDescription        "What does this program do?".
	 @constant               ProgramVersion            "What is the version of this program?".
	 @constant               ProgramCopyright          "String containing the copyright years like "2015 - 2017"".
	 @constant               ProgramLicenseName        "Short name of the license, like "3-clause BSD", etc".
	 @constant               ProgramLicenseDescription "Describe the license or EULA".
	 @constant               ProgramLicenseURL         "URL for the EULA, ToS, or Open source license".
	 */
	struct CommandLineIO {
		uint64_t             NumSwitches;
		uint64_t             MinSwitches;
		uint64_t             NumArguments;
		uint64_t             HelpSwitch;
		bool                 IsProprietary;
		char                *ProgramName;
		char                *ProgramAuthor;
		char                *ProgramDescription;
		char                *ProgramVersion;
		char                *ProgramCopyright;
		char                *ProgramLicenseName;
		char                *ProgramLicenseDescription;
		char                *ProgramLicenseURL;
		CommandLineSwitch2  *Switches;
		CommandLineArgument2 *Arguments;
	};
	
	CommandLineIO *InitCommandLineIO(const uint64_t NumSwitches) {
		CommandLineIO *CLI = calloc(1, sizeof(CommandLineIO));
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "InitCommandLineIO", "Not enough memory to allocate CommandLineIO");
		} else {
			CLI->NumSwitches          = NumSwitches;
			CLI->Switches             = calloc(NumSwitches, sizeof(CommandLineSwitch2));
			if (CLI->Switches == NULL) {
				Log(LOG_ERR, "libBitIO", "InitCommandLineIO", "Not enough memory to allocate CommandLineSwitch, NumSwitches = %d", NumSwitches);
			}
		}
		return CLI;
	}
	
	void SetCLIName(CommandLineIO *CLI, char *Name) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIName", "Pointer to CommandLineIO is NULL");
		} else if (Name == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIName", "Pointer to Name is NULL");
		} else {
			CLI->ProgramName = Name;
		}
	}
	
	void SetCLIVersion(CommandLineIO *CLI, char *VersionString) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIVersion", "Pointer to CommandLineIO is NULL");
		} else if (VersionString == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIVersion", "Pointer to VersionString is NULL");
		} else {
			CLI->ProgramVersion = VersionString;
		}
	}
	
	void SetCLIDescription(CommandLineIO *CLI, char *Description) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIDescription", "Pointer to CommandLineIO is NULL");
		} else if (Description == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIDescription", "Pointer to Description is NULL");
		} else {
			CLI->ProgramDescription = Description;
		}
	}
	
	void SetCLIAuthor(CommandLineIO *CLI, char *Author) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIAuthor", "Pointer to CommandLineIO is NULL");
		} else if (Author == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIAuthor", "Pointer to Author is NULL");
		} else {
			CLI->ProgramAuthor = Author;
		}
	}
	
	void SetCLICopyright(CommandLineIO *CLI, char *Copyright) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLICopyright", "Pointer to CommandLineIO is NULL");
		} else if (Copyright == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLICopyright", "Pointer to Copyright is NULL");
		} else {
			CLI->ProgramCopyright = Copyright;
		}
	}
	
	void SetCLILicense(CommandLineIO *CLI, char *Name, char *LicenseDescription, char *LicenseURL, const bool IsProprietary) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLILicense", "Pointer to CommandLineIO is NULL");
		} else if (Name == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLILicense", "Pointer to Name is NULL");
		} else if (LicenseDescription == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLILicense", "Pointer to LicenseDescription is NULL");
		} else {
			CLI->ProgramLicenseName        = Name;
			CLI->ProgramLicenseDescription = LicenseDescription;
			CLI->ProgramLicenseURL         = LicenseURL;
			if (IsProprietary == true) {
				CLI->IsProprietary         = true;
			} else {
				CLI->IsProprietary         = false;
			}
		}
	}
	
	void SetCLIMinSwitches(CommandLineIO *CLI, const uint64_t MinSwitches) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIMinSwitches", "Pointer to CommandLineIO is NULL");
		} else {
			CLI->MinSwitches = MinSwitches;
		}
	}
	
	void SetCLISwitchAsMaster(CommandLineIO *CLI, const uint64_t Switch) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsMaster", "Pointer to CommandLineIO is NULL");
		} else {
			CLI->Switches[Switch].IsMasterSwitch = true;
		}
	}
	
	void SetCLISwitchMaxActiveDependents(CommandLineIO *CLI, const uint64_t Master, const uint64_t MaxActiveDependents) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchMaxActiveDependents", "Pointer to CommandLineIO is NULL");
		} else if (Master > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchMaxActiveDependents", "Master %d, should be between 0 and %d", Master, CLI->NumSwitches);
		} else {
			CLI->Switches[Master].MaxActiveDependents = MaxActiveDependents;
		}
	}
	
	void SetCLISwitchAsDependent(CommandLineIO *CLI, const uint64_t Master, const uint64_t Dependent) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsDependent", "Pointer to CommandLineIO is NULL");
		} else if (Master > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsDependent", "Master %d, should be between 0 and %d", Master, CLI->NumSwitches);
		} else if (Dependent > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsDependent", "Dependent %d, should be between 0 and %d", Dependent, CLI->NumSwitches);
		} else {
			CLI->Switches[Master].NumDependentSwitches += 1;
			if (CLI->Switches[Master].AllowableDependents == NULL) {
				CLI->Switches[Master].AllowableDependents = calloc(CLI->Switches[Master].NumDependentSwitches, sizeof(uint64_t));
			} else {
				uint64_t DependentSwitchesSize = sizeof(uint64_t) * CLI->Switches[Master].NumDependentSwitches;
				CLI->Switches[Master].AllowableDependents = realloc(CLI->Switches[Master].AllowableDependents, DependentSwitchesSize);
			}
			uint64_t CurrentDependentSwitch = CLI->Switches[Master].NumDependentSwitches;
			CLI->Switches[Master].AllowableDependents[CurrentDependentSwitch] = Dependent;
		}
	}
	
	void SetCLISwitchFlag(CommandLineIO *CLI, const uint64_t SwitchNum, char *Flag) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchFlag", "Pointer to CommandLineIO is NULL");
		} else if (Flag == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchFlag", "Pointer to switch Flag is NULL");
		} else if (SwitchNum > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchFlag", "SwitchNum %d is too high, there are only %d switches", SwitchNum, CLI->NumSwitches);
		} else {
			CLI->Switches[SwitchNum].SwitchFlag     = Flag;
			CLI->Switches[SwitchNum].SwitchFlagSize = strlen(Flag) + 1;
		}
	}
	
	void SetCLISwitchDescription(CommandLineIO *CLI, const uint64_t SwitchNum, char *Description) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchDescription", "Pointer to CommandLineIO is NULL");
		} else if (SwitchNum > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchDescription", "SwitchNum %d is too high, there are only %d switches", SwitchNum, CLI->NumSwitches);
		} else if (Description == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchDescription", "Pointer to switch Description is NULL");
		} else {
			CLI->Switches[SwitchNum].SwitchDescription = Description;
		}
	}
	
	void SetCLIHelpSwitch(CommandLineIO *CLI, const uint64_t HelpSwitch) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLIHelpSwitch", "Pointer to CommandLineIO is NULL");
		} else if (HelpSwitch > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLIHelpSwitch", "HelpSwitch %d is too high, there are only %d switches", HelpSwitch, CLI->NumSwitches);
		} else {
			CLI->HelpSwitch = HelpSwitch;
		}
	}
	
	static inline void DisplayCLIHelp(CommandLineIO *CLI) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "DisplayCLIHelp", "Pointer to CommandLineIO is NULL");
		} else {
			printf("%s Options: (-|--|/)", CLI->ProgramName);
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				if (CLI->Switches[Switch].IsMasterSwitch == true) {
					printf("%s: %s", CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchDescription);
					for (uint64_t DependentSwitch = 0ULL; DependentSwitch < CLI->Switches[Switch].NumDependentSwitches; DependentSwitch++) {
						printf("\t%s: %s", CLI->Switches[DependentSwitch].SwitchFlag, CLI->Switches[DependentSwitch].SwitchDescription);
					}
				}
			}
		}
	}
	
	static inline void DisplayProgramBanner(CommandLineIO *CLI) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "DisplayProgramBanner", "Pointer to CommandLineIO is NULL");
		} else {
			if (CLI->ProgramName != NULL) {
				printf("%s ", CLI->ProgramName);
			} else if (CLI->ProgramVersion != NULL) {
				printf("version %s", CLI->ProgramVersion);
			} else if (CLI->ProgramAuthor != NULL) {
				printf(" by %s", CLI->ProgramAuthor);
			} else if (CLI->ProgramCopyright != NULL) {
				printf(" © %s", CLI->ProgramCopyright);
			} else if (CLI->ProgramDescription != NULL) {
				printf(": %s", CLI->ProgramDescription);
			} else if (CLI->IsProprietary == true && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL) {
				printf(", Released under the \"%s\" license: %s, available at: %s", CLI->ProgramLicenseName, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL);
			} else if (CLI->IsProprietary == false && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL) {
				printf(", By using this software, you agree to the End User License Agreement %s, available at: %s", CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL);
			}
		}
	}
	
	static bool *BuildMasterSwitchTable(CommandLineIO *CLI, const int argc, const char *argv[]) {
		char    *SingleDashFlag    = NULL;
		char    *DoubleDashFlag    = NULL;
		char    *SingleSlashFlag   = NULL;
		
		bool    *MasterSwitchTable = calloc(1, argc * sizeof(bool));
		
		for (int ArgvMap = 0; ArgvMap < argc; ArgvMap++) {
			for (uint64_t ArgvMapSwitch = 0; ArgvMapSwitch < CLI->NumSwitches; ArgvMapSwitch++) {
				uint64_t SingleDashFlagSize  = CLI->Switches[ArgvMapSwitch].SwitchFlagSize + 1;
				uint64_t DoubleDashFlagSize  = CLI->Switches[ArgvMapSwitch].SwitchFlagSize + 2;
				uint64_t SingleSlashFlagSize = CLI->Switches[ArgvMapSwitch].SwitchFlagSize + 1;
				
				SingleDashFlag     = calloc(1, SingleDashFlagSize);
				DoubleDashFlag     = calloc(1, DoubleDashFlagSize);
				SingleSlashFlag    = calloc(1, SingleSlashFlagSize);
				
				snprintf(SingleDashFlag, SingleDashFlagSize, "-%s", CLI->Switches[ArgvMapSwitch].SwitchFlag);
				snprintf(DoubleDashFlag, DoubleDashFlagSize, "--%s", CLI->Switches[ArgvMapSwitch].SwitchFlag);
				snprintf(SingleSlashFlag, SingleSlashFlagSize, "/%s", CLI->Switches[ArgvMapSwitch].SwitchFlag);
				strcasecmp(argv[ArgvMap], SingleDashFlag);
				strcasecmp(argv[ArgvMap], DoubleDashFlag);
				strcasecmp(argv[ArgvMap], SingleSlashFlag);
				if (strcasecmp(argv[ArgvMap], SingleDashFlag) == 0 || strcasecmp(argv[ArgvMap], DoubleDashFlag) == 0 || strcasecmp(argv[ArgvMap], SingleSlashFlag) == 0) {
					// Then this argv argument matches the current flag, all we need to do is verify if this switch is a Master switch.
					if (CLI->Switches[ArgvMapSwitch].IsMasterSwitch == true) {
						MasterSwitchTable[ArgvMap] = true;
					}
				}
				free(SingleDashFlag);
				free(DoubleDashFlag);
				free(SingleSlashFlag);
			}
		}
		return MasterSwitchTable;
	}
	/*
	 static uint64_t HowManyDependentSwitchesUntilNextIndependentSwitch(const bool *MasterSwitchTable, const uint64_t CurrentArgument, const uint64_t TotalArguments) {
	 uint64_t Answer = 0ULL;
	 for (uint64_t Argument = CurrentArgument + 1; Argument < TotalArguments; Argument++) {
	 if (MasterSwitchTable[Argument] == false) {
	 Answer += 1;
	 }
	 }
	 return Answer;
	 }
	 */
	
	static char *ConvertArgument2SwitchFlag(const char *Argument) {
		/*
		 We need to first find out if the Argument has 1 or 2 leading characters for - -- /
		 Once we find that, simply remove it from the string.
		 */
		uint8_t SingleDash  = memcmp(Argument, "-", 1);
		uint8_t DoubleDash  = memcmp(Argument, "--", 2);
		uint8_t SingleSlash = memcmp(Argument, "/", 1);
		
		uint8_t Bytes2Remove = 0;
		
		if (SingleDash == 0 || SingleSlash == 0) {
			Bytes2Remove = 1;
		} else if (DoubleDash == 0) {
			Bytes2Remove = 2;
		}
		
		uint64_t ArgumentSize = strlen(Argument);
		char *FixedArgument = calloc(1, ArgumentSize - Bytes2Remove);
		for (uint64_t Byte = Bytes2Remove - 1; Byte < ArgumentSize; Byte++) {
			FixedArgument[Byte] = Argument[Byte];
		}
		return FixedArgument;
	}
	
	void ParseCommandLineArguments(CommandLineIO *CLI, const int argc, const char *argv[]) {
		/*
		 Ok, so first we need to make a map of where all the master switches are in argv.
		 */
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Pointer to CommandLineIO is NULL");
		} else if (argc <= 1 || (argc < (int) CLI->MinSwitches && CLI->MinSwitches > 1) || argv == NULL || strcasecmp(argv[1], CLI->Switches[CLI->NumSwitches].SwitchFlag)) {
			DisplayProgramBanner(CLI);
			DisplayCLIHelp(CLI);
		} else {
			DisplayProgramBanner(CLI);
			
			bool *MasterSwitchTable = BuildMasterSwitchTable(CLI, argc, argv);
			
			/*
			 Ok, so if this current argument is a master switch, we need to find how many arguments til the next master switch.
			 */
			uint64_t NumNextDependentArguments = HowManyDependentSwitchesUntilNextIndependentSwitch(MasterSwitchTable, 0, argc);
			if (NumNextDependentArguments > 0) {
				for (uint64_t NextDependentArg = 1; NextDependentArg < NumNextDependentArguments; NextDependentArg++) {
					// Look over the next NumNextDependentArguments arguments for dependent switches.
					// Now, check if each of these arguments is listed as being dependent on the current one.
					/*
					 How do we do that tho?
					 Well, we need to loop over the NumNextDependentArguments and extract them as a flag, which I should probably make a function for.
					 So, we need to extract the flag contained in the argument and compare it to the list of Child Switches.
					 */
					char *NextChildArgument = ConvertArgument2SwitchFlag(argv[NextDependentArg]);
					if (strcasecmp(CLI->Switches[CLI->Arguments[NextDependentArg].Switch].SwitchFlag, NextChildArgument) == 0) {
						// Add the NextChildArgument as part of the CLI->Arguments
						CLI->Arguments[argc].ChildArguments[CLI->Arguments[argc].NumChildArguments] = NextDependentArg;
					}
				}
			}
		}
	}
	
	/*
	 void ParseCommandLineArguments(CommandLineIO *CLI, const int argc, const char *argv[]) {
	 if (CLI == NULL) {
	 Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Pointer to CommandLineIO is NULL");
	 } else if (argc <= 1 || (argc < (int) CLI->MinSwitches && CLI->MinSwitches > 1) || argv == NULL) {
	 DisplayProgramBanner(CLI);
	 DisplayCLIHelp(CLI);
	 } else {
	 DisplayProgramBanner(CLI);
	 char    *SingleDashFlag       = NULL;
	 char    *DoubleDashFlag       = NULL;
	 char    *SingleSlashFlag      = NULL;
	 char    *ChildSingleDashFlag  = NULL;
	 char    *ChildDoubleDashFlag  = NULL;
	 char    *ChildSingleSlashFlag = NULL;
	 
	 for (int Argument = 1; Argument < argc; Argument++) {
	 for (uint64_t CurrentSwitch = 0ULL; CurrentSwitch < CLI->NumSwitches; CurrentSwitch++) {
	 if (CLI->Switches[CurrentSwitch].IsMasterSwitch == true) {
	 uint64_t FlagSize = CLI->Switches[CurrentSwitch].FlagSize;
	 SingleDashFlag    = calloc(1, FlagSize + 1);
	 DoubleDashFlag    = calloc(1, FlagSize + 2);
	 SingleSlashFlag   = calloc(1, FlagSize + 1);
	 
	 snprintf(SingleDashFlag,  FlagSize + 1,  "-%s", CLI->Switches[CurrentSwitch].Flag);
	 snprintf(DoubleDashFlag,  FlagSize + 2, "--%s", CLI->Switches[CurrentSwitch].Flag);
	 snprintf(SingleSlashFlag, FlagSize + 1,  "/%s", CLI->Switches[CurrentSwitch].Flag);
	 
	 if (strcasecmp(argv[Argument], SingleDashFlag) == 0 || strcasecmp(argv[Argument], DoubleDashFlag) == 0 || strcasecmp(argv[Argument], SingleSlashFlag) == 0) {
	 if (CLI->NumArguments == 0) {
	 CLI->Arguments         = calloc(1, sizeof(CommandLineArgument));
	 } else {
	 CLI->NumArguments     += 1;
	 uint64_t ArgumentsSize = sizeof(CommandLineArgument) * CLI->NumArguments;
	 CLI->Arguments         = realloc(CLI->Arguments, ArgumentsSize);
	 }
	 if (CLI->Switches[CurrentSwitch].SwitchHasResult == true && CLI->Switches[CurrentSwitch].NumChildSwitches > 0) {
	 // Ok, so how abou if it's a master switch AND has a result, let's check to see if there's a child switch first, if not, then there's just a straight result.
	 // So, if the switch has a result, and has child switches, check the child switches first.
	 for (uint64_t ChildSwitch = 0ULL; ChildSwitch < CLI->Switches[CurrentSwitch].NumChildSwitches; ChildSwitch++) {
	 uint64_t ChildFlagSize  = CLI->Switches[CurrentSwitch].FlagSize + 1; // to account for the NULL pointer at the end
	 ChildSingleDashFlag     = calloc(1, ChildFlagSize + 1);
	 ChildDoubleDashFlag     = calloc(1, ChildFlagSize + 2);
	 ChildSingleSlashFlag    = calloc(1, ChildFlagSize + 1);
	 
	 snprintf(ChildSingleDashFlag,  ChildFlagSize + 1,  "-%s", CLI->Switches[CurrentSwitch + 1].Flag);
	 snprintf(ChildDoubleDashFlag,  ChildFlagSize + 2, "--%s", CLI->Switches[CurrentSwitch + 1].Flag);
	 snprintf(ChildSingleSlashFlag, ChildFlagSize + 1,  "/%s", CLI->Switches[CurrentSwitch + 1].Flag);
	 
	 if (strcasecmp(argv[Argument + (int) ChildSwitch], ChildSingleDashFlag) == 0 || strcasecmp(argv[Argument + (int) ChildSwitch], ChildDoubleDashFlag) == 0 || strcasecmp(argv[Argument + (int) ChildSwitch], ChildSingleSlashFlag) == 0) {
	 uint64_t ArgvPlus1SwitchNum       = GetCLISwitchNumFromFlag(CLI, argv[Argument + 1]);
	 uint64_t NumArgsMatchingArgvPlus1 = GetCLINumArgumentsMatchingIndependentSwitch(CLI, ArgvPlus1SwitchNum);
	 CLI->Arguments[ChildSwitch].NumChildArguments += NumArgsMatchingArgvPlus1;
	 for (uint64_t MatchingChildArgs = 0ULL; MatchingChildArgs < NumArgsMatchingArgvPlus1; MatchingChildArgs++) {
	 CLI->Arguments[ChildSwitch].ChildArguments[MatchingChildArgs] = GetCLISwitchNumFromFlag(CLI, argv[Argument + 1]);
	 }
	 }
	 free(ChildSingleDashFlag);
	 free(ChildDoubleDashFlag);
	 free(ChildSingleSlashFlag);
	 }
	 } else if (CLI->Switches[CurrentSwitch].SwitchHasResult == true) {
	 CLI->Arguments[CLI->NumArguments + 1].ArgumentResult = argv[Argument + 1];
	 }
	 }
	 free(SingleDashFlag);
	 free(DoubleDashFlag);
	 free(SingleSlashFlag);
	 }
	 }
	 }
	 }
	 }
	 */
	
	/*
	 New limitation: You can only search directly for arguments that match a master switch
	 
	 We need to be able to ask the arguments which argument matches switches X, Y, and Z.
	 */
	
	uint64_t GetCLINumArgsMatchingIndependentWithDepenents(CommandLineIO const *CLI, const uint64_t IndependentSwitch, const uint64_t NumDependents, const uint64_t *DependentSwitches) {
		uint64_t SwitchContainingMetaArg = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "GetCLINumArgsMatchingIndependentWithDepenents", "Pointer to CommandLineIO is NULL");
		} else if (IndependentSwitch > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "GetCLINumArgsMatchingIndependentWithDepenents", "IndependentSwitch %d is greater than there are switches %d", IndependentSwitch, CLI->NumSwitches);
		} else if (NumDependents > CLI->Switches[IndependentSwitch].MaxActiveDependents) {
			Log(LOG_ERR, "libBitIO", "GetCLIIndependentSwitchWithDependents", "More dependent switches than allowed");
		} else {
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				for (uint64_t DependentArgument = 0ULL; DependentArgument < CLI->Arguments[Argument].NumDependentArguments; DependentArgument++) {
					if (CLI->Arguments[Argument].Switch == ) {
						
					}
				}
			}
		}
	}
	
	uint64_t GetCLIChildSwitchArgument(CommandLineIO const *CLI, const uint64_t ParentSwitch, const uint64_t ChildSwitch) {
		uint64_t SwitchContainingMetaArg = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "GetCLIChildSwitchArgument", "Pointer to CommandLineIO is NULL");
		} else if (ParentSwitch > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "GetCLIChildSwitchArgument", "ParentSwitch %d is greater than there are switches %d", ParentSwitch, CLI->NumSwitches);
		} else if (ChildSwitch > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "GetCLIChildSwitchArgument", "ChildSwitch %d is greater than there are switches %d", ChildSwitch, CLI->NumSwitches);
		} else {
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				for (uint64_t ChildArg = 0ULL; ChildArg < CLI->Arguments[Argument].NumChildArguments; ChildArg++) {
					if (CLI->Arguments[Argument].ChildArguments[ChildArg] == ChildSwitch && CLI->Arguments[Argument].SwitchNum == ParentSwitch) {
						SwitchContainingMetaArg = Argument;
					}
				}
			}
		}
		return SwitchContainingMetaArg;
	}
	
	uint64_t GetCLIArgumentNumFromSwitch(CommandLineIO const *CLI, const uint64_t Switch) {
		uint64_t FoundArgument = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "GetCLIArgumentNumFromSwitch", "Pointer to CommandLineIO is NULL");
		} else if (Switch > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "GetCLIArgumentNumFromSwitch", "Switch %d is greater than there are switches %d", Switch, CLI->NumSwitches);
		} else {
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				// Loop over the arguments until you find CLI->Arguments[Argument].SwitchNum that matches SwitchNum
				if (CLI->Arguments[Argument].Switch == Switch) {
					FoundArgument = Argument;
				}
			}
		}
		return FoundArgument;
	}
	
	uint64_t GetCLIArgumentNumFromSwitches(CommandLineIO const *CLI, const uint64_t ParentSwitch, const uint64_t NumChildSwitches, const uint64_t *ChildSwitch) {
		
		return 0ULL;
	}
	
	char *GetCLIArgumentResult(CommandLineIO const *CLI, const uint64_t Argument) {
		char *Result = NULL;
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "GetCLIArgumentResult", "Pointer to CommandLineIO is NULL");
		} else if (Argument > CLI->NumArguments) {
			Log(LOG_ERR, "libBitIO", "GetCLIArgumentResult", "Argument %d is greater than there are arguments %d", Argument, CLI->NumArguments);
		} else {
			Result = CLI->Arguments[Argument].ArgumentResult;
		}
		return Result;
	}
	
	bool GetCLIArgumentPresenceFromSwitch(CommandLineIO const *CLI, const uint64_t Switch) {
		uint64_t FoundArgument = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "GetCLIArgumentPresenceFromSwitch", "Pointer to CommandLineIO is NULL");
		} else if (Switch > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "GetCLIArgumentPresenceFromSwitch", "SwitchNum %d is not a valid Switch", Switch);
		} else {
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				// Loop over the arguments until you find CLI->Arguments[Argument].SwitchNum that matches SwitchNum
				if (CLI->Arguments[Argument].Switch == Switch) {
					FoundArgument = Argument;
				}
			}
		}
		return FoundArgument;
	}
	
	char *GetCLIArgumentResultFromParentChildSwitch(CommandLineIO const *CLI, const uint64_t Master, const uint64_t Dependent) {
		// We need to find adjacent arguments that contain both the parent and child switches.
		char *ArgumentResult = NULL;
		for (uint64_t Argument = 0; Argument < CLI->NumArguments; Argument++) {
			for (uint64_t DependentArg = 0; DependentArg < CLI->Arguments[Argument].NumDependentArguments; DependentArg++) {
				if (CLI->Arguments[Argument].DependentSwitches[DependentArg]) {
					<#statements#>
				}
				
				
				
				
				
				
				
				
				
				if (CLI->Arguments[Argument].SwitchNum == Master && CLI->Arguments[Argument].NumChildArguments >= 1) { // If there are child arguments, look further into it.
					if (CLI->Arguments[Argument].ChildArguments[DependentArg] == Dependent) {
						// Found A argument that contains both, but what if there are multiple?
						uint64_t ChildArgResult = CLI->Arguments[Argument].ChildArguments[ChildArg];
						ArgumentResult = CLI->Arguments[ChildArgResult].ArgumentResult;
					}
				}
			}
		}
		return ArgumentResult;
	}
	
	void DeinitCommandLineIO(CommandLineIO *CLI) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "DeinitCommandLineIO", "Pointer to CommandLineIO is NULL");
		} else {
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				free(CLI->Switches[Switch].SwitchFlag);
				free(CLI->Switches[Switch].SwitchDescription);
				free(CLI->Switches[Switch].AllowableDependents);
			}
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				free(CLI->Arguments[Argument].DependentSwitches);
				free(CLI->Arguments[Argument].ArgumentResult);
			}
			free(CLI->Switches);
			free(CLI->Arguments);
			free(CLI->ProgramName);
			free(CLI->ProgramAuthor);
			free(CLI->ProgramDescription);
			free(CLI->ProgramVersion);
			free(CLI->ProgramCopyright);
			free(CLI->ProgramLicenseName);
			free(CLI->ProgramLicenseDescription);
			free(CLI->ProgramLicenseURL);
			free(CLI);
		}
	}
	
#ifdef __cplusplus
}
#endif
