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
	 @constant				Type						"What type of switch is this"?
	 @constant				SwitchFlag					"What is this switch called, how do we identify it?".
	 @constant				SwitchFlagSize				"What is the number of bytes (if ASCII)/ code points (if UTF) of this switch?".
	 @constant				SwitchDescription			"Describe to the user what this switch does".
	 @constant				MaxActiveSlaves				"How many Slave switches can be active at once"?
	 @constant				ValidSlaves					"Pointer to an array that contains the list of aloowable Slave switches".
	 */
	typedef struct CommandLineSwitch {
		CLISwitchTypes       Type;
		uint8_t              SwitchFlagSize;
		uint64_t             NumSlaveIDs;
		uint64_t             MaxActiveSlaves;
		uint64_t            *ValidSlaves;
		char                *SwitchFlag;
		char                *SwitchDescription;
	} CommandLineSwitch;
	
	/*!
	 @struct				CommandLineOption
	 @abstract											"Contains the data to support a single argument".
	 @constant				SwitchID					"Which switch does this argument correspond to"?
	 @constant				NumSlaveArgs				"How many Slave Options were found in this argument?".
	 @constant				SlaveIDs					"Array of Slave argument numbers, to look up in CLI->Switches".
	 @constant				OptionResult				"If there is a path or other result expected for this switch's argument, it'll be here".
	 */
	typedef struct CommandLineOption {
		uint64_t             SwitchID;
		uint64_t             NumSlaveIDs;
		uint64_t            *SlaveIDs;
		char                *OptionResult;
	} CommandLineOption;
	
	/*!
	 @struct				CommandLineIO
	 @abstract											"Contains all the information, and relationships between switches on the command line".
	 @constant				NumSwitches					"How many switches are there?".
	 @constant				MinOptions					"The minimum number of switches to accept without dumping the help".
	 @constant				NumOptions					"The number of Options present in argv, after extracting any Slave switches".
	 @constant				HelpSwitch					"Which switch displays the help"?
	 @constant				Switches					"Pointer to an array of switches".
	 @constant				Options						"Pointer to an array of Options".
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
	typedef struct CommandLineIO {
		uint64_t             NumSwitches;
		uint64_t             MinOptions;
		uint64_t             NumOptions;
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
		CommandLineOption   *Options;
	} CommandLineIO;
	
	CommandLineIO *CommandLineIO_Init(const uint64_t NumSwitches) {
		CommandLineIO *CLI            = calloc(1, sizeof(CommandLineIO));
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Not enough memory to allocate CommandLineIO");
		} else {
			CLI->NumSwitches          = NumSwitches;
			CLI->Switches             = calloc(NumSwitches, sizeof(CommandLineSwitch));
			if (CLI->Switches == NULL) {
				BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Not enough memory to allocate CommandLineSwitch, NumSwitches = %d", NumSwitches);
			}
		}
		return CLI;
	}
	
	void CLISetName(CommandLineIO *CLI, char *Name) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Name == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Name String is NULL");
		} else {
			CLI->ProgramName = Name;
		}
	}
	
	void CLISetVersion(CommandLineIO *CLI, char *Version) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Version == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Version String is NULL");
		} else {
			CLI->ProgramVersion = Version;
		}
	}
	
	void CLISetDescription(CommandLineIO *CLI, char *Description) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Description == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Description String is NULL");
		} else {
			CLI->ProgramDescription = Description;
		}
	}
	
	void CLISetAuthor(CommandLineIO *CLI, char *Author) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Author == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Author String is NULL");
		} else {
			CLI->ProgramAuthor = Author;
		}
	}
	
	void CLISetCopyright(CommandLineIO *CLI, char *Copyright) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Copyright == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Copyright String is NULL");
		} else {
			CLI->ProgramCopyright = Copyright;
		}
	}
	
	void CLISetLicense(CommandLineIO *CLI, char *Name, char *LicenseDescription, char *LicenseURL, const bool IsProprietary) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Name == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Name String is NULL");
		} else if (LicenseDescription == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "LicenseDescription String is NULL");
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
	
	void CLISetMinOptions(CommandLineIO *CLI, const uint64_t MinOptions) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			CLI->MinOptions = MinOptions;
		}
	}
	
	void CLISetHelpSwitch(CommandLineIO *CLI, const uint64_t HelpSwitch) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (HelpSwitch > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "HelpSwitch %d is too high, there are only %d switches", HelpSwitch, CLI->NumSwitches);
		} else {
			CLI->HelpSwitch = HelpSwitch;
		}
	}
	
	void CLISetSwitchFlag(CommandLineIO *CLI, const uint64_t SwitchNum, char *Flag) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Flag == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Flag String is NULL");
		} else if (SwitchNum > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "SwitchNum %d is too high, there are only %d switches", SwitchNum, CLI->NumSwitches);
		} else {
			CLI->Switches[SwitchNum].SwitchFlag     = Flag;
			CLI->Switches[SwitchNum].SwitchFlagSize = strlen(Flag) + 1;
		}
	}
	
	void CLISetSwitchDescription(CommandLineIO *CLI, const uint64_t SwitchNum, char *Description) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (SwitchNum > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "SwitchNum %d is too high, there are only %d switches", SwitchNum, CLI->NumSwitches);
		} else if (Description == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Description String is NULL");
		} else {
			CLI->Switches[SwitchNum].SwitchDescription = Description;
		}
	}
	
	void CLISetSwitchType(CommandLineIO *CLI, uint64_t Switch, CLISwitchTypes SwitchType) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			CLI->Switches[Switch].Type                = SwitchType;
			if (SwitchType == SingleSwitchWithResult || SwitchType == SingleSwitchNoResult || SwitchType == SlaveSwitch) {
				CLI->Switches[Switch].MaxActiveSlaves = 0;
				CLI->Switches[Switch].NumSlaveIDs     = 0;
				CLI->Switches[Switch].ValidSlaves     = NULL;
			}
		}
	}
	
	void CLISetSwitchAsSlave(CommandLineIO *CLI, const uint64_t Master, const uint64_t Slave) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Master > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Master %d, should be between 0 and %d", Master, CLI->NumSwitches);
		} else if (Slave > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Slave %d, should be between 0 and %d", Slave, CLI->NumSwitches);
		} else {
			CLI->Switches[Master].NumSlaveIDs               += 1;
			
			uint64_t NumSlaves                               = CLI->Switches[Master].NumSlaveIDs;
			
			CLI->Switches[Master].ValidSlaves                = realloc(CLI->Switches[Master].ValidSlaves, NumSlaves);
			CLI->Switches[Master].ValidSlaves[NumSlaves - 1] = Slave;
		}
	}
	
	void CLISetSwitchMaxActiveSlaves(CommandLineIO *CLI, const uint64_t Master, const uint64_t MaxActiveSlaves) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Master > CLI->NumSwitches) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Master %d, should be between 0 and %d", Master, CLI->NumSwitches);
		} else {
			CLI->Switches[Master].MaxActiveSlaves = MaxActiveSlaves;
		}
	}
	
	static inline void DisplayCLIHelp(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			printf("%s Options (-|--|/):%s", CLI->ProgramName, BitIOLineEnding);
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				CLISwitchTypes CurrentSwitchType = CLI->Switches[Switch].Type;
				printf("%s: %s%s", CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchDescription, BitIOLineEnding);
				if (CurrentSwitchType == MasterSwitch) {
					for (uint64_t SlaveSwitch = 0ULL; SlaveSwitch < CLI->Switches[Switch].NumSlaveIDs; SlaveSwitch++) {
						printf("\t%s: %s%s", CLI->Switches[SlaveSwitch].SwitchFlag, CLI->Switches[SlaveSwitch].SwitchDescription, BitIOLineEnding);
					}
				}
			}
		}
	}
	
	static inline void DisplayProgramBanner(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			if (CLI->ProgramName != NULL) {
				printf("%s%s", CLI->ProgramName, BitIOLineEnding);
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
				printf("%s", BitIOLineEnding);
			}
		}
	}
	
	static char *ConvertOptionString2SwitchFlag(const char *OptionString) {
		uint8_t  Bytes2RemoveFromArg = 0;
		uint64_t OptionStringSize    = strlen(OptionString);
		if (strncasecmp(OptionString, "/", 1) == 0) {
			Bytes2RemoveFromArg      = 1;
		} else if (strncasecmp(OptionString, "--", 2) == 0) {
			Bytes2RemoveFromArg      = 2;
		} else if (strncasecmp(OptionString, "-", 1) == 0) {
			Bytes2RemoveFromArg      = 1;
		}
		uint64_t OptionSwitchSize    = (OptionStringSize - Bytes2RemoveFromArg) + BitIONULLStringSize;
		char    *OptionSwitch        = calloc(OptionSwitchSize, sizeof(char));
		if (OptionSwitch == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Not enough memory to allocate OptionSwitch which needs %ulld bytes of memory", OptionSwitchSize);
		} else {
			for (uint64_t Byte = Bytes2RemoveFromArg; Byte < (OptionStringSize + BitIONULLStringSize) - Bytes2RemoveFromArg; Byte++) {
				OptionSwitch[Byte]   = OptionString[Byte];
			}
		}
		return OptionSwitch;
	}
	
	void ParseCommandLineOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
		uint64_t CurrentArg = 0ULL;
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			char *FirstArgumentAsFlag = ConvertOptionString2SwitchFlag(argv[1]);
			char *HelpFlag            = ConvertOptionString2SwitchFlag(CLI->Switches[CLI->HelpSwitch].SwitchFlag);
			if (argc < (int) CLI->MinOptions || strncasecmp(FirstArgumentAsFlag, HelpFlag, CLI->Switches[CLI->HelpSwitch].SwitchFlagSize) == 0) {
				DisplayCLIHelp(CLI);
			} else {
				DisplayProgramBanner(CLI);
				uint64_t NumCommandLineOptionsNeeded = 0ULL;
				// Count how many Options we're gonna need total, then go through and make the
				for (uint32_t ArgvCount = 1; ArgvCount < (uint32_t) argc; ArgvCount++) {
					// ok so check the type of argument by looking up the switch
					char *CurrentArgvStringAsFlag    = ConvertOptionString2SwitchFlag(argv[ArgvCount]);
					CLISwitchTypes CurrentSwitchType = UnknownSwitchType;
					// Now compare the SwitchFlag with CurrentArgvStringAsFlag
					for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
						if (strcasecmp(CurrentArgvStringAsFlag, CLI->Switches[Switch].SwitchFlag) == 0) {
							CurrentSwitchType = CLI->Switches[Switch].Type;
							switch (CurrentSwitchType) {
								case UnknownSwitchType:
									BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Unknown Switch type %d", CurrentSwitchType);
									break;
								case SingleSwitchWithResult:
									NumCommandLineOptionsNeeded += 1;
									ArgvCount += 1;
									break;
								case SingleSwitchNoResult:
									NumCommandLineOptionsNeeded += 1;
									break;
								case MasterSwitch:
									NumCommandLineOptionsNeeded += 1;
									for (uint64_t Slave = 0ULL; Slave < CLI->Switches[Switch].NumSlaveIDs; Slave++) {
										// compare the next argument to the list of slaves
										char *SlaveArgvAsFlag    = ConvertOptionString2SwitchFlag(argv[ArgvCount + (Slave + 1)]);
										// Now try to match the slave to a switch
										for (uint64_t SlaveSwitch = 0ULL; SlaveSwitch < CLI->NumSwitches; SlaveSwitch++) {
											if (strcasecmp(SlaveArgvAsFlag, CLI->Switches[SlaveSwitch].SwitchFlag) == 0) {
												// Ok, we found a matching SlaveSwitch
												ArgvCount += 1;
											}
										}
									}
									break;
								case SlaveSwitch:
									CLI->Options[Switch].SlaveIDs += 1; // FIXME: WHAT
									break;
							}
						}
					}
				}
				CLI->Options = calloc(CLI->NumOptions, sizeof(CommandLineOption));
				for (int ArgvArg = 1L; ArgvArg < argc; ArgvArg++) {
					char *CurrentArgvStringAsFlag = ConvertOptionString2SwitchFlag(argv[ArgvArg]);
					for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
						if (strcasecmp(CurrentArgvStringAsFlag, CLI->Switches[Switch].SwitchFlag) == 0) {
							// So we found the switch, now we need to find out the type of switch it is so we can continue processing
							CLI->NumOptions                  += 1;
							CurrentArg                        = CLI->NumOptions;
							CLI->Options                      = realloc(CLI->Options, CLI->NumOptions * sizeof(CommandLineOption));
							CLISwitchTypes CurrentSwitchType  = CLI->Switches[Switch].Type;
							
							CLI->Options[CurrentArg].SwitchID = Switch;
							
							switch (CurrentSwitchType) {
								case UnknownSwitchType:
									BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Unknown Switch type %d", CurrentSwitchType);
									break;
								case SingleSwitchWithResult:
									CLI->Options[CurrentArg].OptionResult = (char*) argv[ArgvArg + 1];
									break;
								case MasterSwitch:
									CLI->Options[CurrentArg].SlaveIDs = calloc(CLI->Options[CurrentArg].NumSlaveIDs, sizeof(uint64_t));
									for (uint64_t SlaveSwitch = 0ULL; SlaveSwitch < CLI->Switches[Switch].NumSlaveIDs; SlaveSwitch++) {
										uint64_t NumArgsLeft = argc - (ArgvArg + SlaveSwitch);
										if (NumArgsLeft < CLI->Options[CurrentArg].NumSlaveIDs + 1) { // + 1 for the Argument result
																									// idk
										}
										char *SlaveArg  = ConvertOptionString2SwitchFlag(argv[ArgvArg + (SlaveSwitch + 1)]);
										// Ok, so we're looping over the slave args, we just need to compare the Slave to CLI->Switches[CLI->Switches[Switch].ValidSlaves[Slave]].SwitchFlag
										if (strcasecmp(SlaveArg, CLI->Switches[CLI->Switches[Switch].ValidSlaves[SlaveSwitch]].SwitchFlag) == 0) {
											// If the SlaveArg string matches A valid slave, add it to the argument
											CLI->Options[CurrentArg].NumSlaveIDs          += 1;
											uint64_t NumSlaves                             = CLI->Options[CurrentArg].NumSlaveIDs;
											CLI->Options[CurrentArg].SlaveIDs[SlaveSwitch] = CLI->Switches[CLI->Switches[Switch].ValidSlaves[SlaveSwitch]].SwitchFlag; // Set the slave as the switch that it matches
											/*
											 As we loop over the Slaves, we need to keep in mind the MaxActiveSlaves, as we as the number of Options left.
											 */
										}
									}
									break;
								case SlaveSwitch:
									// Ok, all slave switches are appended to the latest Option, until a non-Slave switch comes up
									// Just attach the slaves to the latest Option
									break;
							}
						}
					}
				}
			}
		}
	}
	
	int64_t CLIGetOptionNum(CommandLineIO *CLI, const uint64_t Master, const uint64_t NumSlaves, const uint64_t *Slaves) {
		/* Keep track of the instance of the matcing argument, loop over the Options and slaves of each argument,
		 and compare them, when you find a matching one increment the instance count, compare the instance count to Instance, if they match, you found the argument you're looking for
		 so return that result */
		int64_t MatchingArgNum = 0ULL;
		for (uint64_t Option = 0ULL; Option < CLI->NumOptions; Option++) {
			CLISwitchTypes CurrentArgumentType = CLI->Switches[CLI->Options[Option].SwitchID].Type;
			if (CLI->Options[Option].SwitchID == Master) {
				if (CurrentArgumentType == MasterSwitch && NumSlaves > 0) {
					for (uint64_t ArgSlave = 0ULL; ArgSlave < CLI->Options[Option].NumSlaveIDs; ArgSlave++) {
						for (uint64_t ParamSlave = 0ULL; ParamSlave < NumSlaves; ParamSlave++) {
							if (CLI->Options[Option].SlaveIDs[ArgSlave] == Slaves[ParamSlave]) {
								MatchingArgNum = (int64_t) Option;
							}
						}
					}
				} else if (CurrentArgumentType == SingleSwitchWithResult || CurrentArgumentType == SingleSwitchNoResult || (CurrentArgumentType == MasterSwitch && NumSlaves == 0)) {
					// Make sure that the argument matches, keep a count of the instances found, and return.
					MatchingArgNum = (int64_t) Option;
				}
			} else {
				MatchingArgNum = -1;
			}
		}
		return MatchingArgNum;
	}
	
	char *CLIGetOptionResult(CommandLineIO const *CLI, const uint64_t Option) {
		char *Result = NULL;
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Option > CLI->NumOptions) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Option %d is greater than there are Options %d", Option, CLI->NumOptions);
		} else {
			Result = CLI->Options[Option].OptionResult;
		}
		return Result;
	}
	
	char *GetExtensionFromPath(const char *Path) { // Do not include the seperator aka period.
		uint64_t PathSize        = strlen(Path);
		uint64_t ExtensionOffset = PathSize; // From the end
		uint64_t ExtensionSize   = 0ULL;
		for (uint64_t PathByte = PathSize; PathByte > 0ULL; PathByte--) {
			if (Path[PathByte]  != 0x2E) { // 0x2E is a period in ASCII
				ExtensionOffset -= 1;
				ExtensionSize   += 1;
			}
		}
		char *ExtensionString    = calloc(ExtensionSize + BitIONULLStringSize, sizeof(char));
		if (ExtensionString == NULL) {
			BitIOLog(LOG_ERROR, "CommandLineIO", __func__, "Couldn't allocate %lld bytes for the Extension String", ExtensionSize);
		} else {
			for (uint64_t ExtensionByte = ExtensionOffset; ExtensionByte < PathSize; ExtensionByte++) {
				ExtensionString[ExtensionByte] = Path[ExtensionByte];
			}
		}
		return ExtensionString;
	}
	
	void CommandLineIO_Deinit(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			if (CLI->Switches != NULL) {
				for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
					free(CLI->Switches[Switch].SwitchFlag);
					free(CLI->Switches[Switch].SwitchDescription);
					free(CLI->Switches[Switch].ValidSlaves);
				}
				free(CLI->Switches);
			}
			if (CLI->Options != NULL) {
				for (uint64_t Option = 0ULL; Option < CLI->NumOptions; Option++) {
					free(CLI->Options[Option].SlaveIDs);
					free(CLI->Options[Option].OptionResult);
				}
				free(CLI->Options);
			}
			if (CLI->ProgramName != NULL) {
				free(CLI->ProgramName);
			}
			if (CLI->ProgramAuthor != NULL) {
				free(CLI->ProgramAuthor);
			}
			if (CLI->ProgramDescription != NULL) {
				free(CLI->ProgramDescription);
			}
			if (CLI->ProgramVersion != NULL) {
				free(CLI->ProgramVersion);
			}
			if (CLI->ProgramCopyright != NULL) {
				free(CLI->ProgramCopyright);
			}
			if (CLI->ProgramLicenseName != NULL) {
				free(CLI->ProgramLicenseName);
			}
			if (CLI->ProgramLicenseDescription != NULL) {
				free(CLI->ProgramLicenseDescription);
			}
			if (CLI->ProgramLicenseURL != NULL) {
				free(CLI->ProgramLicenseURL);
			}
			free(CLI);
		}
	}
	
#ifdef __cplusplus
}
#endif

