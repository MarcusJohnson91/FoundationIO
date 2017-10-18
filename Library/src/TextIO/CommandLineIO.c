#include "../include/BitIO.h"
#include "../include/CommandLineIO.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#ifdef __cplusplus
extern "C" {
#endif
	
	/*!
	 @struct				CommandLineSwitch
	 @abstract											"Contains the data to support a switch".
	 @constant				IsIndependentSwitch			"Can this switch have dependent arguments"?
	 @constant				SwitchFlag					"What is this switch called, how do we identify it?".
	 @constant				SwitchFlagSize				"What is the number of bytes (if ASCII)/ code points (if UTF) of this switch?".
	 @constant				SwitchDescription			"Describe to the user what this switch does".
	 @constant				MaxActiveDependents			"How many dependent switches can be active at once"?
	 @constant				ValidDependents				"Pointer to an array that contains the list of aloowable dependent switches".
	 */
	typedef struct CommandLineSwitch {
		bool                 IsIndependentSwitch;
		uint8_t              SwitchFlagSize;
		char                *SwitchFlag;
		char                *SwitchDescription;
		uint64_t             MaxActiveDependents;
		uint64_t             NumDependentSwitches;
		uint64_t            *ValidDependents;
	} CommandLineSwitch;
	
	/*!
	 @struct				CommandLineArgument
	 @abstract											"Contains the data to support a single argument".
	 @constant				Switch						"Which switch does this argument correspond to"?
	 @constant				NumDependentArguments		"How many child arguments were found in this argument?".
	 @constant				DependentArguments			"Array of child argument numbers, to look up in CLI->Switches".
	 @constant				ArgumentResult				"If there is a path or other result expected for this switch's argument, it'll be here".
	 */
	typedef struct CommandLineArgument {
		uint64_t             Switch;
		uint64_t             NumDependentArguments;
		uint64_t            *DependentArguments;
		char                *ArgumentResult;
	} CommandLineArgument;
	
	/*!
	 @struct				CommandLineIO
	 @abstract											"Contains all the information, and relationships between switches on the command line".
	 @constant				NumSwitches					"How many switches are there?".
	 @constant				MinSwitches					"The minimum number of switches to accept without dumping the help".
	 @constant				NumArguments				"The number of arguments present in argv, after extracting any child switches".
	 @constant				HelpSwitch					"Which switch displays the help"?
	 @constant				Switches					"Pointer to an array of switches".
	 @constant				Arguments					"Pointer to an array of arguments".
	 @constant				IsProprietary				"Is this program proprietary?".
	 @constant				ProgramName					"What is the name of this program?".
	 @constant				ProgramAuthor				"Who wrote this program?".
	 @constant				ProgramDescription			"What does this program do?".
	 @constant				ProgramVersion				"What is the version of this program?".
	 @constant				ProgramCopyright			"String containing the copyright years like "2015 - 2017"".
	 @constant				ProgramLicenseName			"Short name of the license, like "3-clause BSD", etc".
	 @constant				ProgramLicenseDescription	"Describe the license or EULA".
	 @constant				ProgramLicenseURL			"URL for the EULA, ToS, or Open source license".
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
		CommandLineSwitch   *Switches;
		CommandLineArgument *Arguments;
	};
	
	CommandLineIO *InitCommandLineIO(const uint64_t NumSwitches) {
		CommandLineIO *CLI = calloc(1, sizeof(CommandLineIO));
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "InitCommandLineIO", "Not enough memory to allocate CommandLineIO");
		} else {
			CLI->NumSwitches          = NumSwitches;
			CLI->Switches             = calloc(NumSwitches, sizeof(CommandLineSwitch));
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
	
	void SetCLISwitchAsIndependent(CommandLineIO *CLI, const uint64_t Switch) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsIndependent", "Pointer to CommandLineIO is NULL");
		} else {
			CLI->Switches[Switch].IsIndependentSwitch = true;
		}
	}
	
	void SetCLISwitchMaxActiveDependents(CommandLineIO *CLI, const uint64_t Independent, const uint64_t MaxActiveDependents) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchMaxActiveDependents", "Pointer to CommandLineIO is NULL");
		} else if (Independent > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchMaxActiveDependents", "Independent %d, should be between 0 and %d", Independent, CLI->NumSwitches);
		} else {
			CLI->Switches[Independent].MaxActiveDependents = MaxActiveDependents;
		}
	}
	
	void SetCLISwitchAsDependent(CommandLineIO *CLI, const uint64_t Independent, const uint64_t Dependent) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsDependent", "Pointer to CommandLineIO is NULL");
		} else if (Independent > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsDependent", "Independent %d, should be between 0 and %d", Independent, CLI->NumSwitches);
		} else if (Dependent > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "SetCLISwitchAsDependent", "Dependent %d, should be between 0 and %d", Dependent, CLI->NumSwitches);
		} else {
			CLI->Switches[Independent].NumDependentSwitches += 1;
			if (CLI->Switches[Independent].ValidDependents == NULL) {
				CLI->Switches[Independent].ValidDependents = calloc(CLI->Switches[Independent].NumDependentSwitches, sizeof(uint64_t));
			} else {
				uint64_t DependentSwitchesSize = sizeof(uint64_t) * CLI->Switches[Independent].NumDependentSwitches;
				CLI->Switches[Independent].ValidDependents = realloc(CLI->Switches[Independent].ValidDependents, DependentSwitchesSize);
			}
			uint64_t CurrentDependentSwitch = CLI->Switches[Independent].NumDependentSwitches;
			CLI->Switches[Independent].ValidDependents[CurrentDependentSwitch] = Dependent;
		}
	}
	
	static inline void DisplayCLIHelp(CommandLineIO *CLI) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "DisplayCLIHelp", "Pointer to CommandLineIO is NULL");
		} else {
			printf("%s Options: (-|--|/)", CLI->ProgramName);
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				if (CLI->Switches[Switch].IsIndependentSwitch == true) {
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
	/*
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
	 */
	
	static char *ConvertArgumentString2SwitchFlag(const char *ArgumentString) {
		uint8_t  Bytes2RemoveFromArg = 0;
		uint64_t ArgumentStringSize  = strlen(ArgumentString);
		char    *ArgumentSwitch = NULL;
		if (strncasecmp(ArgumentString, "/", 1) == 0) {
			Bytes2RemoveFromArg = 1;
		} else if (strncasecmp(ArgumentString, "--", 2) == 0) {
			Bytes2RemoveFromArg = 2;
		} else if (strncasecmp(ArgumentString, "-", 1) == 0) {
			Bytes2RemoveFromArg = 1;
		}
		
		uint64_t ArgumentSwitchSize = (ArgumentStringSize - Bytes2RemoveFromArg) + BitIOStringNULLSize;
		
		ArgumentSwitch = calloc(1, ArgumentSwitchSize);
		
		if (ArgumentSwitch == NULL) {
			Log(LOG_ERR, "libBitIO", "ConvertArgumentString2SwitchFlag", "Not enough memory to allocate ArgumentSwitch which needs %ulld bytes of memory", ArgumentSwitchSize);
		} else {
			for (uint64_t Byte = Bytes2RemoveFromArg - 1; Byte < ArgumentStringSize; Byte++) {
				// Ok, so we just start copying the bytes
				ArgumentSwitch[Byte] = ArgumentString[Byte];
			}
		}
		return ArgumentSwitch;
	}
	
	void ParseCommandLineArguments(CommandLineIO *CLI, const int argc, const char *argv[]) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "ParseCommandLineArguments", "Pointer to CommandLineIO is NULL");
		} else if (argc <= 1 || (argc < (int) CLI->MinSwitches && CLI->MinSwitches > 1) || argv == NULL || strcasecmp(argv[1], CLI->Switches[CLI->NumSwitches].SwitchFlag)) {
			DisplayProgramBanner(CLI);
			DisplayCLIHelp(CLI);
		} else {
			DisplayProgramBanner(CLI);
			/*
			 Ok, so we need to loop over argc, and compare each argv string to each switch, when a match is found, enter it into CLI->Arguments
			 */
			for (int argvArgument = 1; argvArgument < argc; argvArgument++) {
				for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
					// Convert argvArgument to a switch string, then compare it to all Switch flags.
					char *ArgvArgumentSwitch = ConvertArgumentString2SwitchFlag(argv[argvArgument]);
					if (CLI->Switches[Switch].IsIndependentSwitch == true) {
						if (strcasecmp(ArgvArgumentSwitch, CLI->Switches[Switch].SwitchFlag) == 0) { // Found a match
							// Now we go ahead and compare all the following arguments to this argument's Dependencies.
							for (uint64_t DependentSwitch = 0ULL; DependentSwitch < CLI->Switches[Switch].NumDependentSwitches; DependentSwitch++) {
							}
						}
					}
				}
			}
		}
	}
	
	/*
	uint64_t GetCLINumArgsMatchingSwitch(CommandLineIO const *CLI, const uint64_t IndependentSwitch, const uint64_t NumDependents, const uint64_t *DependentSwitches) {
		uint64_t SwitchContainingMetaArg = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "GetCLINumArgsMatchingSwitch", "Pointer to CommandLineIO is NULL");
		} else if (IndependentSwitch > CLI->NumSwitches) {
			Log(LOG_ERR, "libBitIO", "GetCLINumArgsMatchingSwitch", "IndependentSwitch %d is greater than there are switches %d", IndependentSwitch, CLI->NumSwitches);
		} else if (NumDependents > CLI->Switches[IndependentSwitch].MaxActiveDependents) {
			Log(LOG_ERR, "libBitIO", "GetCLINumArgsMatchingSwitch", "More dependent switches than allowed");
		} else {
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				for (uint64_t DependentArgument = 0ULL; DependentArgument < CLI->Arguments[Argument].NumDependentArguments; DependentArgument++) {
					const char *ArgumentSwitch = ConvertArgument2SwitchFlag(CLI->Arguments[Argument].Switch);
					if (CLI->Arguments[Argument].Switch == CLI->Switches[Argument]) {
						
						
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
				
				
				
				
				
				
				
				
				
				if (CLI->Arguments[Argument].Switch == Master && CLI->Arguments[Argument].NumChildArguments >= 1) { // If there are child arguments, look further into it.
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
	 */
	
	void DeinitCommandLineIO(CommandLineIO *CLI) {
		if (CLI == NULL) {
			Log(LOG_ERR, "libBitIO", "DeinitCommandLineIO", "Pointer to CommandLineIO is NULL");
		} else {
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				free(CLI->Switches[Switch].SwitchFlag);
				free(CLI->Switches[Switch].SwitchDescription);
				free(CLI->Switches[Switch].ValidDependents);
			}
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				free(CLI->Arguments[Argument].Switch);
				free(CLI->Arguments[Argument].DependentArguments);
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
