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
	 @constant				SwitchType					"What type of switch is this"?
	 @constant				SwitchFlag					"What is this switch called, how do we identify it?".
	 @constant				SwitchFlagSize				"What is the number of bytes (if ASCII)/ code points (if UTF) of this switch?".
	 @constant				SwitchDescription			"Describe to the user what this switch does".
	 @constant				MaxActiveSlaves				"How many Slave switches can be active at once"?
	 @constant				ValidSlaves					"Pointer to an array that contains the list of aloowable Slave switches".
	 */
	typedef struct CommandLineSwitch {
		CLISwitchTypes       SwitchType;
		uint8_t              SwitchFlagSize;
		uint64_t             MaxActiveSlaves;
		uint64_t             NumValidSlaveIDs;
		int64_t             *ValidSlaves;
		char                *SwitchFlag;
		char                *SwitchDescription;
	} CommandLineSwitch;
	
	/*!
	 @struct				CommandLineOption
	 @abstract											"Contains the data to support a single argument".
	 @constant				SwitchID					"Which switch does this argument correspond to"?
	 @constant				NumSlaveIDs					"How many Slave Options were found in this argument?".
	 @constant				SlaveIDs					"Array of Slave argument numbers, to look up in CLI->Switches".
	 @constant				Argument2Option				"If there is a path or other result expected for this switch's argument, it'll be here".
	 */
	typedef struct CommandLineOption {
		int64_t              SwitchID;
		uint64_t             NumSlaveIDs;
		int64_t             *SlaveIDs;
		char                *Argument2Option;
	} CommandLineOption;
	
	/*!
	 @struct				CommandLineIO
	 @abstract											"Contains all the information, and relationships between switches on the command line".
	 @constant				NumSwitches					"How many switches are there?".
	 @constant				NumOptions					"The number of Options present in argv, after extracting any Slave switches".
	 @constant				MinOptions					"The minimum number of switches to accept without dumping the help".
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
		uint64_t             NumOptions;
		uint64_t             MinOptions;
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
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Couldn't allocate CommandLineIO");
		} else if (NumSwitches == 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Couldn't allocate CommandLineIO");
		} else {
			CLI->NumSwitches          = NumSwitches;
			CLI->Switches             = calloc(NumSwitches, sizeof(CommandLineSwitch));
			if (CLI->Switches == NULL) {
				BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Couldn't allocate %d CommandLineSwitches", NumSwitches);
			}
		}
		return CLI;
	}
	
	void CLISetName(CommandLineIO *CLI, char *Name) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Name == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Name String is NULL");
		} else {
			CLI->ProgramName = Name;
		}
	}
	
	void CLISetVersion(CommandLineIO *CLI, char *Version) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Version == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Version String is NULL");
		} else {
			CLI->ProgramVersion = Version;
		}
	}
	
	void CLISetDescription(CommandLineIO *CLI, char *Description) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Description == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Description String is NULL");
		} else {
			CLI->ProgramDescription = Description;
		}
	}
	
	void CLISetAuthor(CommandLineIO *CLI, char *Author) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Author == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Author String is NULL");
		} else {
			CLI->ProgramAuthor = Author;
		}
	}
	
	void CLISetCopyright(CommandLineIO *CLI, char *Copyright) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Copyright == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Copyright String is NULL");
		} else {
			CLI->ProgramCopyright = Copyright;
		}
	}
	
	void CLISetLicense(CommandLineIO *CLI, char *Name, char *LicenseDescription, char *LicenseURL, const bool IsProprietary) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Name == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Name String is NULL");
		} else if (LicenseDescription == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "LicenseDescription String is NULL");
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
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			CLI->MinOptions = MinOptions;
		}
	}
	
	void CLISetHelpSwitch(CommandLineIO *CLI, const uint64_t HelpSwitch) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (HelpSwitch > CLI->NumSwitches - 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "HelpSwitch %d is too high, there are only %d switches", HelpSwitch, CLI->NumSwitches);
		} else {
			CLI->HelpSwitch = HelpSwitch;
		}
	}
	
	void CLISetSwitchFlag(CommandLineIO *CLI, const uint64_t SwitchID, char *Flag) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Flag == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Flag String is NULL");
		} else if (SwitchID > CLI->NumSwitches - 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is too high, there are only %d switches", SwitchID, CLI->NumSwitches);
		} else {
			CLI->Switches[SwitchID].SwitchFlag     = Flag;
			CLI->Switches[SwitchID].SwitchFlagSize = (uint8_t) strlen(Flag);
		}
	}
	
	void CLISetSwitchDescription(CommandLineIO *CLI, const uint64_t SwitchID, char *Description) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (SwitchID > CLI->NumSwitches - 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchNum %d is too high, there are only %d switches", SwitchID, CLI->NumSwitches);
		} else if (Description == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Description String is NULL");
		} else {
			CLI->Switches[SwitchID].SwitchDescription = Description;
		}
	}
	
	void CLISetSwitchType(CommandLineIO *CLI, const uint64_t SwitchID, CLISwitchTypes SwitchType) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (SwitchType == UnknownSwitchType) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You can not set SwitchID %d to UnknownSwitchType", SwitchID);
		} else {
			CLI->Switches[SwitchID].SwitchType           = SwitchType;
			if (SwitchType == SwitchCantHaveSlaves || SwitchType == SwitchIsASlave || SwitchType == ExistentialSwitch) {
				CLI->Switches[SwitchID].MaxActiveSlaves  = 0;
				CLI->Switches[SwitchID].NumValidSlaveIDs = 0;
				CLI->Switches[SwitchID].ValidSlaves      = NULL;
			}
		}
	}
	
	void CLISetSwitchAsSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t Slave) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (MasterID > (int64_t) CLI->NumSwitches - 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Master %d, should be between 0 and %d", MasterID, CLI->NumSwitches);
		} else if (Slave > (int64_t) CLI->NumSwitches - 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Slave %d, should be between 0 and %d", Slave, CLI->NumSwitches);
		} else {
			CLI->Switches[MasterID].NumValidSlaveIDs          += 1;
			
			uint64_t NumSlaves                                 = CLI->Switches[MasterID].NumValidSlaveIDs + 1;
			
			CLI->Switches[MasterID].ValidSlaves                = realloc(CLI->Switches[MasterID].ValidSlaves, NumSlaves * sizeof(int64_t));
			CLI->Switches[MasterID].ValidSlaves[NumSlaves - 1] = Slave;
		}
	}
	
	void CLISetSwitchMaxActiveSlaves(CommandLineIO *CLI, const uint64_t MasterID, const uint64_t MaxActiveSlaves) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (MasterID > CLI->NumSwitches - 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Master %d, should be between 0 and %d", MasterID, CLI->NumSwitches);
		} else {
			CLI->Switches[MasterID].MaxActiveSlaves = MaxActiveSlaves;
		}
	}
	
	static inline void DisplayCLIHelp(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			fprintf(stdout, "%s Options (-|--|/):%s", CLI->ProgramName, BitIONewLine);
			for (uint64_t Switch = 0LLU; Switch < CLI->NumSwitches - 1; Switch++) {
				CLISwitchTypes CurrentSwitchType = CLI->Switches[Switch].SwitchType;
				fprintf(stdout, "%s: %s%s", CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchDescription, BitIONewLine);
				if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->Switches[Switch].NumValidSlaveIDs > 0) {
					for (uint64_t SlaveSwitch = 0LLU; SlaveSwitch < CLI->Switches[Switch].NumValidSlaveIDs; SlaveSwitch++) {
						fprintf(stdout, "\t%s: %s%s", CLI->Switches[SlaveSwitch].SwitchFlag, CLI->Switches[SlaveSwitch].SwitchDescription, BitIONewLine);
					}
				}
			}
		}
	}
	
	static inline void DisplayProgramBanner(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			if (CLI->ProgramName != NULL) {
				fprintf(stdout, "%s%s", CLI->ProgramName, BitIONewLine);
			} else if (CLI->ProgramVersion != NULL) {
				fprintf(stdout, "version %s", CLI->ProgramVersion);
			} else if (CLI->ProgramAuthor != NULL) {
				fprintf(stdout, " by %s", CLI->ProgramAuthor);
			} else if (CLI->ProgramCopyright != NULL) {
				fprintf(stdout, " © %s", CLI->ProgramCopyright);
			} else if (CLI->ProgramDescription != NULL) {
				fprintf(stdout, ": %s", CLI->ProgramDescription);
			} else if (CLI->IsProprietary == Yes && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL) {
				fprintf(stdout, ", Released under the \"%s\" license: %s, available at: %s", CLI->ProgramLicenseName, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL);
			} else if (CLI->IsProprietary == No && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL) {
				fprintf(stdout, ", By using this software, you agree to the End User License Agreement %s, available at: %s", CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL);
			} else {
				fprintf(stdout, "%s", BitIONewLine);
			}
		}
	}
	
	void PrintCommandLineOptions(CommandLineIO *CLI) {
		BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "NumOptions %d%s", CLI->NumOptions, BitIONewLine);
		for (uint64_t Option = 0LLU; Option < CLI->NumOptions; Option++) {
			if (CLI->Options[Option].SwitchID >= 0) {
				BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "OptionNum %d, OptionSwitchID %d, OptionFlag %s%s", Option, CLI->Options[Option].SwitchID, CLI->Switches[CLI->Options[Option].SwitchID].SwitchFlag, BitIONewLine);
				if (CLI->Options[Option].NumSlaveIDs >= 1) {
					for (uint64_t OptionSlave = 0LLU; OptionSlave < CLI->Options[Option].NumSlaveIDs; OptionSlave++) {
						BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "SlaveID %d%s", OptionSlave, BitIONewLine);
					}
				}
			}
		}
	}
	
	static char *ConvertOptionString2SwitchFlag(const char *OptionString) {
		uint8_t  Bytes2RemoveFromArg = 0;
		uint64_t OptionStringSize    = strlen(OptionString);
		char    *OptionStringPrefix  = calloc(3, sizeof(char));
		strncpy(OptionStringPrefix, OptionString, 2);
		if (strncasecmp((const char*)OptionStringPrefix, "/", 1) == 0) {
			Bytes2RemoveFromArg      = 1;
		} else if (strncasecmp((const char*)OptionStringPrefix, "--", 2) == 0) {
			Bytes2RemoveFromArg      = 2;
		} else if (strncasecmp((const char*)OptionStringPrefix, "-", 1) == 0) {
			Bytes2RemoveFromArg      = 1;
		}
		free(OptionStringPrefix);
		uint64_t OptionSwitchSize    = (OptionStringSize - Bytes2RemoveFromArg) + BitIONULLStringSize;
		char    *OptionSwitch        = calloc(OptionSwitchSize, sizeof(char));
		strncpy(OptionSwitch, OptionString + Bytes2RemoveFromArg, OptionStringSize - Bytes2RemoveFromArg);
		return OptionSwitch;
	}
	
	static void CLIAllocateOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
		for (uint32_t ArgvCount = 1; ArgvCount < (uint32_t) argc - 1; ArgvCount++) {
			char *CurrentArgvStringAsFlag                                  = ConvertOptionString2SwitchFlag(argv[ArgvCount]);
			for (uint64_t Switch = 0LLU; Switch < CLI->NumSwitches - 1; Switch++) {
				if (strcasecmp(CurrentArgvStringAsFlag, CLI->Switches[Switch].SwitchFlag) == 0) {
					CLISwitchTypes CurrentSwitchType                       = CLI->Switches[Switch].SwitchType;
					switch (CurrentSwitchType) {
						case SwitchMayHaveSlaves:
							CLI->NumOptions                               += 1;
							break;
						case SwitchCantHaveSlaves:
							CLI->NumOptions                               += 1;
							break;
						case SwitchIsASlave:
							CLI->Options[CLI->NumOptions - 1].NumSlaveIDs += 1;
							break;
						case ExistentialSwitch:
							CLI->NumOptions                               += 1;
							break;
						case UnknownSwitchType:
							BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You need to call CLISetSwitchType for SwitchID %d", Switch);
							break;
					}
				}
			}
		}
		CLI->Options                                                       = calloc(CLI->NumOptions, sizeof(CommandLineOption));
		if (CLI->Options != NULL) {
			for (uint64_t Option = 0LLU; Option < CLI->NumOptions - 1; Option++) {
				CLI->Options[Option].SwitchID                              = -1;
				CLI->Options[Option].SlaveIDs                              = calloc(CLI->Options[Option].NumSlaveIDs, sizeof(int64_t));
				if (CLI->Options[Option].NumSlaveIDs >= 1) {
					for (uint64_t OptionSlave = 0LLU; OptionSlave < CLI->Options[Option].NumSlaveIDs - 1; OptionSlave++) {
						CLI->Options[Option].SlaveIDs[OptionSlave]         = -1;
					}
				}
			}
		} else {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Couldn't allocate %d options", CLI->NumOptions);
		}
	}
	
	void ParseCommandLineOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
		uint64_t CurrentArg                                                                = 0LLU;
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			char *FirstArgumentAsFlag                                                      = ConvertOptionString2SwitchFlag(argv[1]);
			char *HelpFlag                                                                 = ConvertOptionString2SwitchFlag(CLI->Switches[CLI->HelpSwitch].SwitchFlag);
			if (argc < (int) CLI->MinOptions || strncasecmp(FirstArgumentAsFlag, HelpFlag, CLI->Switches[CLI->HelpSwitch].SwitchFlagSize) == 0) {
				DisplayCLIHelp(CLI);
			} else {
				DisplayProgramBanner(CLI);
				CLIAllocateOptions(CLI, argc, argv);
				for (int ArgvArg = 1L; ArgvArg < argc; ArgvArg++) {
					char *CurrentArgvStringAsFlag = ConvertOptionString2SwitchFlag(argv[ArgvArg]);
					for (uint64_t Switch = 0LLU; Switch < CLI->NumSwitches - 1; Switch++) {
						if (strcasecmp(CurrentArgvStringAsFlag, CLI->Switches[Switch].SwitchFlag) == 0) {
							CLI->NumOptions                                               += 1;
							CurrentArg                                                     = CLI->NumOptions - 1;
							CLISwitchTypes CurrentSwitchType                               = CLI->Switches[Switch].SwitchType;
							switch (CurrentSwitchType) {
								case ExistentialSwitch:
									CLI->Options[CurrentArg].SwitchID                      = (int64_t) Switch;
									break;
								case SwitchMayHaveSlaves:
									CLI->Options[CurrentArg].SwitchID                      = (int64_t) Switch;
									break;
								case SwitchCantHaveSlaves:
									CLI->Options[CurrentArg].SwitchID                      = (int64_t) Switch;
									CLI->Options[CurrentArg].Argument2Option               = (char*) argv[ArgvArg + 1];
									break;
								case SwitchIsASlave:
									for (uint64_t OptionSlave = 0LLU; OptionSlave < CLI->Options[Switch].NumSlaveIDs - 1; OptionSlave++) {
										uint64_t NumArgsLeft                               = argc - (ArgvArg + OptionSlave);
										if (NumArgsLeft < CLI->Options[CurrentArg].NumSlaveIDs + 1) { // + 1 for the Argument result
											// idk
										}
										char *SlaveArg                                     = ConvertOptionString2SwitchFlag(argv[ArgvArg + (OptionSlave + 1)]);
										if (strcasecmp(SlaveArg, CLI->Switches[CLI->Switches[Switch].ValidSlaves[OptionSlave]].SwitchFlag) == 0) {
											CLI->Options[CurrentArg].NumSlaveIDs          += 1;
											uint64_t NumSlaves                             = CLI->Options[CurrentArg].NumSlaveIDs;
											CLI->Options[CurrentArg].SlaveIDs[OptionSlave] = CLI->Switches[CLI->Switches[Switch].ValidSlaves[OptionSlave]].SwitchFlag;
										}
									}
									break;
							}
						} else if (CLI->Switches[CLI->Options[CLI->NumOptions - 1].SwitchID].SwitchType != ExistentialSwitch) {
							CLI->Options[CLI->NumOptions - 1].Argument2Option  = (char*) argv[ArgvArg];
						}
					}
				}
			}
		}
	}
	
	int64_t CLIGetOptionNum(CommandLineIO *CLI, const int64_t SwitchID, const uint64_t NumSlaves, const int64_t *SlaveIDs) {
		bool AllOptionsMatch   = Yes;
		int64_t MatchingArgNum = 0LLU;
		for (uint64_t Option = 0LLU; Option < CLI->NumOptions; Option++) {
			if (CLI->Options[Option].SwitchID == SwitchID) {
				for (uint64_t OptionSlave = 0LLU; OptionSlave < CLI->Options[Option].NumSlaveIDs; OptionSlave++) {
					for (uint64_t ParamSlave = 0LLU; ParamSlave < NumSlaves; ParamSlave++) {
						if (SlaveIDs[ParamSlave] == CLI->Options[Option].SlaveIDs[OptionSlave]) {
							
						}
					}
				}
			}
			
			CLISwitchTypes CurrentArgumentType = CLI->Switches[CLI->Options[Option].SwitchID].SwitchType;
			
			/*
			 Loop over the Options looking for SwitchID, when you find an option that matches SwitchID, check if the SlaveIDs match
			 if the SlaveIDs don't match all, then you need to go ahead and keep looping.
			 */
			
			switch (CurrentArgumentType) {
				case UnknownSwitchType:
					break;
				case SwitchMayHaveSlaves:
					break;
				case SwitchCantHaveSlaves:
					break;
				case SwitchIsASlave:
					break;
				case ExistentialSwitch:
					break;
			}
			/*
			if (CLI->Switches[CLI->Options[Option].SwitchID].Type == Master) {
				if (CurrentArgumentType == MasterSwitch && NumSlaves > 0) {
					for (uint64_t ArgSlave = 0LLU; ArgSlave < CLI->Options[Option].NumSlaveIDs; ArgSlave++) {
						for (uint64_t ParamSlave = 0LLU; ParamSlave < NumSlaves; ParamSlave++) {
							if (CLI->Options[Option].SlaveIDs[ArgSlave] == SlaveIDs[ParamSlave]) {
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
			 */
		}
		return MatchingArgNum;
	}
	
	char *CLIGetOptionResult(CommandLineIO const *CLI, const uint64_t Option) {
		char *Result = NULL;
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Option > CLI->NumOptions) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Option %d is greater than there are Options %d", Option, CLI->NumOptions);
		} else {
			Result = CLI->Options[Option].Argument2Option;
		}
		return Result;
	}
	
	char *GetExtensionFromPath(const char *Path) {
		uint64_t PathSize        = strlen(Path);
		uint64_t ExtensionOffset = PathSize;
		uint64_t ExtensionSize   = 0LLU;
		enum {
			ASCIIPeriod          = 0x2E,
		};
		for (uint64_t PathByte = PathSize; PathByte > 0LLU; PathByte--) {
			if (Path[PathByte]  != ASCIIPeriod) {
				ExtensionOffset -= 1;
				ExtensionSize   += 1;
			}
		}
		char *ExtensionString    = calloc(ExtensionSize + BitIONULLStringSize, sizeof(char));
		if (ExtensionString == NULL) {
			BitIOLog(BitIOLog_ERROR, "CommandLineIO", __func__, "Couldn't allocate %lld bytes for the Extension String", ExtensionSize);
		} else {
			for (uint64_t ExtensionByte = ExtensionOffset; ExtensionByte < PathSize; ExtensionByte++) {
				ExtensionString[ExtensionByte] = Path[ExtensionByte];
			}
		}
		return ExtensionString;
	}
	
	void CommandLineIO_Deinit(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			if (CLI->Switches != NULL) {
				for (uint64_t Switch = 0LLU; Switch < CLI->NumSwitches - 1; Switch++) {
					free(CLI->Switches[Switch].SwitchFlag);
					free(CLI->Switches[Switch].SwitchDescription);
					free(CLI->Switches[Switch].ValidSlaves);
				}
				free(CLI->Switches);
			}
			if (CLI->Options != NULL) {
				for (uint64_t Option = 0LLU; Option < CLI->NumOptions - 1; Option++) {
					free(CLI->Options[Option].SlaveIDs);
					free(CLI->Options[Option].Argument2Option);
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
