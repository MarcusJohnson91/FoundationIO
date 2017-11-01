#include "../include/BitIO.h"
#include "../include/CommandLineIO.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
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
	
	typedef struct DependentArgument {
		uint64_t SwitchNum;
	} DependentArgument;
	
	/*!
	 @struct				CommandLineArgument
	 @abstract											"Contains the data to support a single argument".
	 @constant				Switch						"Which switch does this argument correspond to"?
	 @constant				NumDependentArguments		"How many Dependent arguments were found in this argument?".
	 @constant				Dependent					"Array of Dependent argument numbers, to look up in CLI->Switches".
	 @constant				ArgumentResult				"If there is a path or other result expected for this switch's argument, it'll be here".
	 */
	typedef struct CommandLineArgument {
		uint64_t             Switch;
		uint64_t             NumDependentArguments;
		DependentArgument   *Dependent;
		//uint64_t            *DependentArguments;
		char                *ArgumentResult;
	} CommandLineArgument;
	
	/*!
	 @struct				CommandLineIO
	 @abstract											"Contains all the information, and relationships between switches on the command line".
	 @constant				NumSwitches					"How many switches are there?".
	 @constant				MinArguments				"The minimum number of switches to accept without dumping the help".
	 @constant				NumArguments				"The number of arguments present in argv, after extracting any Dependent switches".
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
		uint64_t             MinArguments;
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
	
	CommandLineIO *CommandLineIOInit(const uint64_t NumSwitches) {
		CommandLineIO *CLI = calloc(1, sizeof(CommandLineIO));
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "CommandLineIOInit", "Not enough memory to allocate CommandLineIO");
		} else {
			CLI->NumSwitches          = NumSwitches;
			CLI->Switches             = calloc(NumSwitches, sizeof(CommandLineSwitch));
			if (CLI->Switches == NULL) {
				BitIOLog(LOG_ERROR, "libBitIO", "CommandLineIOInit", "Not enough memory to allocate CommandLineSwitch, NumSwitches = %d", NumSwitches);
			}
		}
		return CLI;
	}
	
	void SetCLIName(CommandLineIO *CLI, char *Name) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIName", "Pointer to CommandLineIO is NULL");
		} else if (Name == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIName", "Pointer to Name is NULL");
		} else {
			CLI->ProgramName = Name;
		}
	}
	
	void SetCLIVersion(CommandLineIO *CLI, char *VersionString) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIVersion", "Pointer to CommandLineIO is NULL");
		} else if (VersionString == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIVersion", "Pointer to VersionString is NULL");
		} else {
			CLI->ProgramVersion = VersionString;
		}
	}
	
	void SetCLIDescription(CommandLineIO *CLI, char *Description) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIDescription", "Pointer to CommandLineIO is NULL");
		} else if (Description == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIDescription", "Pointer to Description is NULL");
		} else {
			CLI->ProgramDescription = Description;
		}
	}
	
	void SetCLIAuthor(CommandLineIO *CLI, char *Author) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIAuthor", "Pointer to CommandLineIO is NULL");
		} else if (Author == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIAuthor", "Pointer to Author is NULL");
		} else {
			CLI->ProgramAuthor = Author;
		}
	}
	
	void SetCLICopyright(CommandLineIO *CLI, char *Copyright) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLICopyright", "Pointer to CommandLineIO is NULL");
		} else if (Copyright == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLICopyright", "Pointer to Copyright is NULL");
		} else {
			CLI->ProgramCopyright = Copyright;
		}
	}
	
	void SetCLILicense(CommandLineIO *CLI, char *Name, char *LicenseDescription, char *LicenseURL, const bool IsProprietary) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLILicense", "Pointer to CommandLineIO is NULL");
		} else if (Name == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLILicense", "Pointer to Name is NULL");
		} else if (LicenseDescription == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLILicense", "Pointer to LicenseDescription is NULL");
		} else {
			CLI->ProgramLicenseName        = Name;
			CLI->ProgramLicenseDescription = LicenseDescription;
			CLI->ProgramLicenseURL         = LicenseURL;
			if (IsProprietary == Yes) {
				CLI->IsProprietary         = true;
			} else {
				CLI->IsProprietary         = false;
			}
		}
	}
	
	void SetCLIMinArguments(CommandLineIO *CLI, const uint64_t MinArguments) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIMinArguments", "Pointer to CommandLineIO is NULL");
		} else {
			CLI->MinArguments = MinArguments;
		}
	}
	
	void SetCLIHelpSwitch(CommandLineIO *CLI, const uint64_t HelpSwitch) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIHelpSwitch", "Pointer to CommandLineIO is NULL");
		} else if (HelpSwitch > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLIHelpSwitch", "HelpSwitch %d is too high, there are only %d switches", HelpSwitch, CLI->NumSwitches);
		} else {
			CLI->HelpSwitch = HelpSwitch;
		}
	}
	
	void SetCLISwitchFlag(CommandLineIO *CLI, const uint64_t SwitchNum, char *Flag) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchFlag", "Pointer to CommandLineIO is NULL");
		} else if (Flag == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchFlag", "Pointer to switch Flag is NULL");
		} else if (SwitchNum > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchFlag", "SwitchNum %d is too high, there are only %d switches", SwitchNum, CLI->NumSwitches);
		} else {
			CLI->Switches[SwitchNum].SwitchFlag     = Flag;
			CLI->Switches[SwitchNum].SwitchFlagSize = strlen(Flag) + 1;
		}
	}
	
	void SetCLISwitchDescription(CommandLineIO *CLI, const uint64_t SwitchNum, char *Description) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchDescription", "Pointer to CommandLineIO is NULL");
		} else if (SwitchNum > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchDescription", "SwitchNum %d is too high, there are only %d switches", SwitchNum, CLI->NumSwitches);
		} else if (Description == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchDescription", "Pointer to switch Description is NULL");
		} else {
			CLI->Switches[SwitchNum].SwitchDescription = Description;
		}
	}
	
	void SetCLISwitchAsIndependent(CommandLineIO *CLI, const uint64_t Switch) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchAsIndependent", "Pointer to CommandLineIO is NULL");
		} else {
			CLI->Switches[Switch].IsIndependentSwitch = true;
		}
	}
	
	void SetCLISwitchMaxActiveDependents(CommandLineIO *CLI, const uint64_t Independent, const uint64_t MaxActiveDependents) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchMaxActiveDependents", "Pointer to CommandLineIO is NULL");
		} else if (Independent > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchMaxActiveDependents", "Independent %d, should be between 0 and %d", Independent, CLI->NumSwitches);
		} else {
			CLI->Switches[Independent].MaxActiveDependents = MaxActiveDependents;
		}
	}
	
	void SetCLISwitchAsDependent(CommandLineIO *CLI, const uint64_t Independent, const uint64_t Dependent) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchAsDependent", "Pointer to CommandLineIO is NULL");
		} else if (Independent > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchAsDependent", "Independent %d, should be between 0 and %d", Independent, CLI->NumSwitches);
		} else if (Dependent > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "SetCLISwitchAsDependent", "Dependent %d, should be between 0 and %d", Dependent, CLI->NumSwitches);
		} else {
			CLI->Switches[Independent].NumDependentSwitches += 1;
			if (CLI->Switches[Independent].ValidDependents == NULL) {
				CLI->Switches[Independent].ValidDependents = calloc(CLI->Switches[Independent].NumDependentSwitches, sizeof(uint64_t));
			} else {
				uint64_t DependentSwitchesSize = sizeof(uint64_t) * CLI->Switches[Independent].NumDependentSwitches;
				CLI->Switches[Independent].ValidDependents = realloc(CLI->Switches[Independent].ValidDependents, DependentSwitchesSize);
			}
			uint64_t CurrentDependentSwitch = CLI->Switches[Independent].NumDependentSwitches;
			CLI->Switches[Independent].ValidDependents[CLI->Switches[Independent].NumDependentSwitches - 1] = Dependent;
		}
	}
	
	static inline void DisplayCLIHelp(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "DisplayCLIHelp", "Pointer to CommandLineIO is NULL");
		} else {
			printf("%s Options: (-|--|/)\n", CLI->ProgramName);
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				if (CLI->Switches[Switch].IsIndependentSwitch == Yes) {
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
			BitIOLog(LOG_ERROR, "libBitIO", "DisplayProgramBanner", "Pointer to CommandLineIO is NULL");
		} else {
			if (CLI->ProgramName != NULL) {
				printf("%s \n", CLI->ProgramName);
			} else if (CLI->ProgramVersion != NULL) {
				printf("version %s", CLI->ProgramVersion);
			} else if (CLI->ProgramAuthor != NULL) {
				printf(" by %s", CLI->ProgramAuthor);
			} else if (CLI->ProgramCopyright != NULL) {
				printf(" © %s", CLI->ProgramCopyright);
			} else if (CLI->ProgramDescription != NULL) {
				printf(": %s", CLI->ProgramDescription);
			} else if (CLI->IsProprietary == Yes && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL) {
				printf(", Released under the \"%s\" license: %s, available at: %s", CLI->ProgramLicenseName, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL);
			} else if (CLI->IsProprietary == No && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL) {
				printf(", By using this software, you agree to the End User License Agreement %s, available at: %s", CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL);
			} else {
				printf("\n");
			}
		}
	}
	
	static inline char *ConvertArgumentString2SwitchFlag(const char *ArgumentString) {
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
			BitIOLog(LOG_ERROR, "libBitIO", "ConvertArgumentString2SwitchFlag", "Not enough memory to allocate ArgumentSwitch which needs %ulld bytes of memory", ArgumentSwitchSize);
		} else {
			for (uint64_t Byte = Bytes2RemoveFromArg - 1; Byte < ArgumentStringSize; Byte++) {
				ArgumentSwitch[Byte] = ArgumentString[Byte];
			}
		}
		return ArgumentSwitch;
	}
	
	void ParseCommandLineArguments(CommandLineIO *CLI, const int argc, const char *argv[]) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "ParseCommandLineArguments", "Pointer to CommandLineIO is NULL");
		} else if (argc < (int) CLI->MinArguments || strncasecmp(ConvertArgumentString2SwitchFlag(argv[1]), CLI->Switches[CLI->HelpSwitch].SwitchFlag, CLI->Switches[CLI->HelpSwitch].SwitchFlagSize) == 0) {
			DisplayCLIHelp(CLI);
		} else {
			DisplayProgramBanner(CLI);
			
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				for (int ArgvArg = 0L; ArgvArg < argc; ArgvArg++) {
					char *ArgvString        = ConvertArgumentString2SwitchFlag(argv[ArgvArg]);
					uint64_t ArgvStringSize = strlen(ArgvString);
					for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
						if (CLI->Switches[Switch].SwitchFlagSize == ArgvStringSize) {
							if (strncasecmp(ArgvString, CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchFlagSize) == 0) {
								if (CLI->Switches[Switch].IsIndependentSwitch == Yes) {
									for (uint64_t DependentSwitch = 0ULL; DependentSwitch < CLI->Switches[Switch].NumDependentSwitches; DependentSwitch++) {
										char *PotentialDependentArgument   = ConvertArgumentString2SwitchFlag(argv[ArgvArg + DependentSwitch]);
										uint64_t PotentialDependentArgSize = strlen(PotentialDependentArgument);
										if (CLI->Switches[Switch + DependentSwitch].SwitchFlagSize == PotentialDependentArgSize) {
											if (strncasecmp(PotentialDependentArgument, CLI->Switches[Switch + DependentSwitch].SwitchFlag, CLI->Switches[Switch + DependentSwitch].SwitchFlagSize) == 0) {
												CLI->Arguments[Argument].NumDependentArguments += 1;
												CLI->Arguments[Argument].Dependent[DependentSwitch].SwitchNum = Switch + DependentSwitch; // FIXME: Is this right?
											}
										}
									}
								}
							}
						}
					}
				}
				CLI->NumArguments += 1; // you have to do this as we build the argument list, or the loop will break
			}
		}
	}
	
	uint64_t GetCLINumArgsMatchingSwitch(CommandLineIO const *CLI, const uint64_t IndependentSwitch, const uint64_t NumDependents, const uint64_t *DependentSwitches) {
		uint64_t SwitchContainingMetaArg = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLINumArgsMatchingSwitch", "Pointer to CommandLineIO is NULL");
		} else if (IndependentSwitch > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLINumArgsMatchingSwitch", "IndependentSwitch %d is greater than there are switches %d", IndependentSwitch, CLI->NumSwitches);
		} else if (NumDependents > CLI->Switches[IndependentSwitch].MaxActiveDependents) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLINumArgsMatchingSwitch", "More dependent switches than allowed");
		} else {
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				for (uint64_t DependentArgument = 0ULL; DependentArgument < CLI->Arguments[Argument].NumDependentArguments; DependentArgument++) {
					const char *ArgumentSwitch = ConvertArgumentString2SwitchFlag(CLI->Arguments[Argument].Switch);
					if (CLI->Arguments[Argument].Switch == CLI->Switches[Argument].IsIndependentSwitch) {
						
					}
				}
			}
		}
		return NULL;
	}
	
	uint64_t GetCLIDependentSwitchArgument(CommandLineIO const *CLI, const uint64_t IndependentSwitch, const uint64_t DependentSwitch) {
		uint64_t SwitchContainingMetaArg = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIDependentSwitchArgument", "Pointer to CommandLineIO is NULL");
		} else if (IndependentSwitch > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIDependentSwitchArgument", "IndependentSwitch %d is greater than there are switches %d", IndependentSwitch, CLI->NumSwitches);
		} else if (DependentSwitch > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIDependentSwitchArgument", "DependentSwitch %d is greater than there are switches %d", DependentSwitch, CLI->NumSwitches);
		} else {
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				for (uint64_t DependentArg = 0ULL; DependentArg < CLI->Arguments[Argument].NumDependentArguments; DependentArg++) {
					if (CLI->Arguments[Argument].Dependent[DependentArg].SwitchNum == DependentSwitch && CLI->Arguments[Argument].Switch == IndependentSwitch) {
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
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIArgumentNumFromSwitch", "Pointer to CommandLineIO is NULL");
		} else if (Switch > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIArgumentNumFromSwitch", "Switch %d is greater than there are switches %d", Switch, CLI->NumSwitches);
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
	
	char *GetCLIArgumentResult(CommandLineIO const *CLI, const uint64_t Argument) {
		char *Result = NULL;
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIArgumentResult", "Pointer to CommandLineIO is NULL");
		} else if (Argument > CLI->NumArguments) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIArgumentResult", "Argument %d is greater than there are arguments %d", Argument, CLI->NumArguments);
		} else {
			Result = CLI->Arguments[Argument].ArgumentResult;
		}
		return Result;
	}
	
	bool GetCLIArgumentPresenceFromSwitch(CommandLineIO const *CLI, const uint64_t Switch) {
		uint64_t FoundArgument = 0xFFFFFFFFFFFFFFFFULL;
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIArgumentPresenceFromSwitch", "Pointer to CommandLineIO is NULL");
		} else if (Switch > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, "libBitIO", "GetCLIArgumentPresenceFromSwitch", "SwitchNum %d is not a valid Switch", Switch);
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
	
	uint64_t GetCLINumArgumentsWithIndependentAndDependents(CommandLineIO *CLI, const uint64_t Independent, const uint64_t NumDependents, ...) {
		/*
		 So, We need to go through all of the arguments, and see which argument is composed of Independent switch with the right number of dependent switches.
		 */
		uint64_t NumMatchingArguments = 0ULL;
		for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
			if (CLI->Arguments[Argument].Switch == Independent) {
				if (NumDependents > 0 && CLI->Arguments[Argument].NumDependentArguments > 0) { // Check the dependencies
					for (uint64_t DependentArg = 0ULL; DependentArg < CLI->Arguments[Argument].NumDependentArguments; DependentArg++) {
						va_list DependentArguments;
						va_start(DependentArguments, NumDependents);
						uint64_t *VariadicDependentArguments = calloc(1, NumDependents);
						for (uint64_t VariadicArgs = 0ULL; VariadicArgs < NumDependents; VariadicArgs++) {
							VariadicDependentArguments[VariadicArgs] = va_arg(DependentArguments, uint64_t);
						}
						va_end(DependentArguments);
						for (uint64_t DependentParam = 0ULL; DependentParam < NumDependents; DependentParam++) {
							if (CLI->Arguments[Argument].Dependent->SwitchNum == VariadicDependentArguments[DependentParam]) {
								NumMatchingArguments += 1;
							}
						}
						free(VariadicDependentArguments);
					}
				} else { // There are no dependencies
					NumMatchingArguments += 1;
				}
			}
		}
		return NumMatchingArguments;
	}
	
	uint64_t GetCLIArgumentNumWithIndependentAndDependents(CommandLineIO *CLI, const uint64_t Independent, const uint64_t NumDependents, ...) {
		uint64_t MatchingArgumentNum = 0ULL;
		for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
			if (CLI->Arguments[Argument].Switch == Independent) {
				if (NumDependents > 0 && CLI->Arguments[Argument].NumDependentArguments > 0) { // Check the dependencies
					for (uint64_t DependentArg = 0ULL; DependentArg < CLI->Arguments[Argument].NumDependentArguments; DependentArg++) {
						va_list DependentArguments;
						va_start(DependentArguments, NumDependents);
						uint64_t *VariadicDependentArguments = calloc(1, NumDependents);
						for (uint64_t VariadicArgs = 0ULL; VariadicArgs < NumDependents; VariadicArgs++) {
							VariadicDependentArguments[VariadicArgs] = va_arg(DependentArguments, uint64_t);
						}
						va_end(DependentArguments);
						for (uint64_t DependentParam = 0ULL; DependentParam < NumDependents; DependentParam++) {
							if (CLI->Arguments[Argument].Dependent->SwitchNum == VariadicDependentArguments[DependentParam]) {
								MatchingArgumentNum = Argument;
							}
						}
						free(VariadicDependentArguments);
					}
				} else {
					MatchingArgumentNum = Argument;
				}
			}
		}
		return MatchingArgumentNum;
	}
	
	char *GetCLIArgumentResultFromIndependentDependentSwitch(CommandLineIO const *CLI, const uint64_t Independent, const uint64_t Dependent) {
		// We need to find adjacent arguments that contain both the Independent and Dependent switches.
		char *ArgumentResult = NULL;
		for (uint64_t Argument = 0; Argument < CLI->NumArguments; Argument++) {
			for (uint64_t DependentArg = 0; DependentArg < CLI->Arguments[Argument].NumDependentArguments; DependentArg++) {
				if (CLI->Arguments[Argument].Dependent[DependentArg].SwitchNum) {
					
				}
				
				if (CLI->Arguments[Argument].Switch == Independent && CLI->Arguments[Argument].NumDependentArguments >= 1) { // If there are Dependent arguments, look further into it.
					if (CLI->Arguments[Argument].Dependent[DependentArg].SwitchNum == Dependent) {
						// Found A argument that contains both, but what if there are multiple?
						uint64_t DependentArgResult = CLI->Arguments[Argument].Dependent[DependentArg].SwitchNum;
						ArgumentResult = CLI->Arguments[DependentArgResult].ArgumentResult;
					}
				}
			}
		}
		return ArgumentResult;
	}
	
	char *GetExtensionFromPath(const char *Path) {
		uint64_t PathSize        = strlen(Path);
		uint64_t ExtensionOffset = 0ULL; // From the end
		uint64_t ExtensionSize   = PathSize - ExtensionOffset;
		for (uint64_t PathByte   = PathSize; PathByte > 0ULL; PathByte--) {
			if (Path[PathByte]  != 0x2E) { // 0x2E is a period in ASCII
				ExtensionOffset += 1;
			}
		}
		char *ExtensionString    = calloc(1, sizeof(ExtensionSize) + BitIOStringNULLSize);
		if (ExtensionString == NULL) {
			BitIOLog(LOG_ERROR, "CommandLineIO", "GetExtensionFromPath", "Couldn't allocate %lld bytes for the Extension String", ExtensionSize);
		} else {
			for (uint64_t ExtensionByte = ExtensionOffset; ExtensionByte < PathSize; ExtensionByte++) {
				ExtensionString[ExtensionByte] = Path[ExtensionByte];
			}
		}
		return ExtensionString;
	}
	
	void CommandLineIODeinit(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, "libBitIO", "CommandLineIODeinit", "Pointer to CommandLineIO is NULL");
		} else {
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				free(CLI->Switches[Switch].SwitchFlag);
				free(CLI->Switches[Switch].SwitchDescription);
				free(CLI->Switches[Switch].ValidDependents);
			}
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
				free(CLI->Arguments[Argument].Dependent);
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
