#include "../include/BitIO.h"
#include "../include/CommandLineIO.h"

#ifdef _WIN32
#define strcasecmp   stricmp
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
		int64_t              MaxActiveSlaves;
		int64_t              NumValidSlaveIDs;
		int64_t             *ValidSlaves;
		char                *SwitchFlag;
		char                *SwitchDescription;
		CLISwitchTypes       SwitchType;
		uint8_t              SwitchFlagSize;
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
		int64_t              NumSlaveIDs;
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
		int64_t              NumSwitches;
		int64_t              NumOptions;
		int64_t              MinOptions;
		int64_t              HelpSwitch;
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
		bool                 IsProprietary;
	} CommandLineIO;
	
	CommandLineIO *CommandLineIO_Init(const int64_t NumSwitches) {
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
	
	void CLISetMinOptions(CommandLineIO *CLI, const int64_t MinOptions) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (MinOptions > CLI->NumSwitches - 1 || MinOptions < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MinOptions %s is invalid, it should be between 0 and %d", MinOptions, CLI->NumSwitches - 1);
		} else {
			CLI->MinOptions = MinOptions;
		}
	}
	
	void CLISetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (HelpSwitch > CLI->NumSwitches - 1 || HelpSwitch < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "HelpSwitch %d is invalid, it should be between 0 and %d", HelpSwitch, CLI->NumSwitches - 1);
		} else {
			CLI->HelpSwitch = HelpSwitch;
		}
	}
	
	void CLISetSwitchFlag(CommandLineIO *CLI, const int64_t SwitchID, char *Flag) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Flag == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Flag String is NULL");
		} else if (SwitchID > CLI->NumSwitches - 1 || SwitchID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches - 1);
		} else {
			CLI->Switches[SwitchID].SwitchFlag     = Flag;
			CLI->Switches[SwitchID].SwitchFlagSize = (uint8_t) strlen(Flag);
		}
	}
	
	void CLISetSwitchDescription(CommandLineIO *CLI, const int64_t SwitchID, char *Description) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (SwitchID > CLI->NumSwitches - 1 || SwitchID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches - 1);
		} else if (Description == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Description String is NULL");
		} else {
			CLI->Switches[SwitchID].SwitchDescription = Description;
		}
	}
	
	void CLISetSwitchType(CommandLineIO *CLI, const int64_t SwitchID, CLISwitchTypes SwitchType) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (SwitchID > CLI->NumSwitches - 1 || SwitchID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches - 1);
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
	
	void CLISetSwitchAsSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t SlaveID) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (MasterID > CLI->NumSwitches - 1 || MasterID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MasterID %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches - 1);
		} else if (SlaveID > CLI->NumSwitches - 1 || SlaveID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SlaveID %d is invalid, it should be between 0 and %d", SlaveID, CLI->NumSwitches - 1);
		} else {
			CLI->Switches[MasterID].NumValidSlaveIDs          += 1;
			
			int64_t NumSlaves                                  = CLI->Switches[MasterID].NumValidSlaveIDs + 1;
			
			CLI->Switches[MasterID].ValidSlaves                = realloc(CLI->Switches[MasterID].ValidSlaves, NumSlaves * sizeof(int64_t));
			CLI->Switches[MasterID].ValidSlaves[NumSlaves - 1] = SlaveID;
		}
	}
	
	void CLISetSwitchMaxActiveSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxActiveSlaves) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (MasterID > CLI->NumSwitches - 1 || MasterID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MasterID %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches - 1);
		} else if (MaxActiveSlaves > CLI->NumSwitches - 1 || MaxActiveSlaves < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MaxActiveSlaves %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches - 1);
		} else {
			CLI->Switches[MasterID].MaxActiveSlaves = MaxActiveSlaves;
		}
	}
	
	static inline void DisplayCLIHelp(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			fprintf(stdout, "%s Options (-|--|/):%s", CLI->ProgramName, BitIONewLine);
			for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
				CLISwitchTypes CurrentSwitchType = CLI->Switches[Switch].SwitchType;
				fprintf(stdout, "%s: %s%s", CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchDescription, BitIONewLine);
				if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->Switches[Switch].NumValidSlaveIDs > 0) {
					for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->Switches[Switch].NumValidSlaveIDs; SlaveSwitch++) {
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
			if ((CLI->ProgramName && CLI->ProgramVersion && CLI->ProgramAuthor && CLI->ProgramCopyright) != NULL) {
				fprintf(stdout, "%s, v. %s by %s © %s%s", CLI->ProgramName, CLI->ProgramVersion, CLI->ProgramAuthor, CLI->ProgramCopyright, BitIONewLine);
			}
			
			if ((CLI->ProgramDescription && CLI->ProgramLicenseName && CLI->ProgramLicenseDescription && CLI->ProgramLicenseURL) != NULL && CLI->IsProprietary == No) {
				fprintf(stdout, "%s, Released under the \"%s\" %s, available at: %s%s", CLI->ProgramDescription, CLI->ProgramLicenseName, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, BitIONewLine);
			} else if ((CLI->ProgramDescription && CLI->ProgramLicenseDescription && CLI->ProgramLicenseURL) != NULL && CLI->IsProprietary == Yes) {
				fprintf(stdout, "%s, By using this software, you agree to the End User License Agreement %s, available at: %s%s", CLI->ProgramDescription, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, BitIONewLine);
			}
		}
	}
	
	static char *ConvertOptionString2SwitchFlag(const char *OptionString) {
		char *OptionSwitch = NULL;
		if (OptionString == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "OptionString can't be NULL");
		} else {
			uint8_t  OptionStringPrefixSize = 0;
			enum {
				ASCIIHyphen = 0x2D,
				ASCIIFSlash = 0x5C,
				ASCIIBSlash = 0x2F,
			};
			
			if (OptionString[0] == ASCIIHyphen && OptionString[1] == ASCIIHyphen) {
				OptionStringPrefixSize  = 2;
			} else if (OptionString[0] == ASCIIBSlash || OptionString[0] == ASCIIFSlash || OptionString[0] == ASCIIHyphen) {
				OptionStringPrefixSize  = 1;
			}
			uint32_t OptionStringSize    = strlen(OptionString);
			uint64_t OptionSwitchSize    = (OptionStringSize - OptionStringPrefixSize) + BitIONULLStringSize;
			OptionSwitch                 = calloc(OptionSwitchSize, sizeof(char));
			strncpy(OptionSwitch, OptionString + OptionStringPrefixSize, OptionStringSize - OptionStringPrefixSize);
		}
		return OptionSwitch;
	}
	
	static void CLIAllocateOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			for (int ArgvCount = 1; ArgvCount < argc; ArgvCount++) {
				char *CurrentArgvStringAsFlag                              = ConvertOptionString2SwitchFlag(argv[ArgvCount]);
				for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
					if (strcasecmp(CurrentArgvStringAsFlag, CLI->Switches[Switch].SwitchFlag) == 0) { // TODO: Break out of this loop onec a match has been found.
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
						}
					}
				}
			}
			if (CLI->NumOptions >= CLI->MinOptions) {
				CLI->Options                                                   = calloc(CLI->NumOptions, sizeof(CommandLineOption));
				if (CLI->Options != NULL) {
					for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) {
						CLI->Options[Option].SwitchID                          = -1;
						if (CLI->Options[Option].NumSlaveIDs >= 1) {
							CLI->Options[Option].SlaveIDs                      = calloc(CLI->Options[Option].NumSlaveIDs, sizeof(int64_t));
							for (int64_t OptionSlave = 0LL; OptionSlave < CLI->Options[Option].NumSlaveIDs - 1; OptionSlave++) {
								CLI->Options[Option].SlaveIDs[OptionSlave]     = -1;
							}
						}
					}
				} else {
					BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Couldn't allocate %d options", CLI->NumOptions);
				}
			} else {
				BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You only specified %d options, %d is the minimum", CLI->NumOptions, CLI->MinOptions);
			}
		}
	}
	
	void ParseCommandLineOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (argc < CLI->MinOptions) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You entered %d options, the minimum is %d", argc - 1, CLI->MinOptions);
		} else {
			char *FirstArgumentAsFlag                                                      = ConvertOptionString2SwitchFlag(argv[1]);
			char *HelpFlag                                                                 = CLI->Switches[CLI->HelpSwitch].SwitchFlag;
			if (strncasecmp(FirstArgumentAsFlag, HelpFlag, CLI->Switches[CLI->HelpSwitch].SwitchFlagSize) == 0) {
				DisplayCLIHelp(CLI);
			} else {
				free(FirstArgumentAsFlag);
				DisplayProgramBanner(CLI);
				CLIAllocateOptions(CLI, argc, argv);
				
				int64_t CurrentOption = 0LL;
				
				while (CurrentOption < CLI->NumOptions) {
					for (int32_t ArgvArg = 1LL; ArgvArg < argc - 1; ArgvArg++) {
						for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
							char *CurrentArgvStringAsFlag  = ConvertOptionString2SwitchFlag(argv[ArgvArg]);
							uint64_t ComparisonResult      = strncasecmp(CurrentArgvStringAsFlag, CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchFlagSize);
							if (ComparisonResult == 0) {
								CLISwitchTypes CurrentSwitchType                                  = CLI->Switches[Switch].SwitchType;
								switch (CurrentSwitchType) {
									case ExistentialSwitch:
										CLI->Options[CurrentOption].SwitchID                      = Switch;
										break;
									case SwitchMayHaveSlaves:
										CLI->Options[CurrentOption].SwitchID                      = Switch;
										break;
									case SwitchCantHaveSlaves:
										CLI->Options[CurrentOption].SwitchID                      = Switch;
										break;
									case SwitchIsASlave:
										for (uint64_t OptionSlave = 0LL; OptionSlave < CLI->Options[CurrentOption].NumSlaveIDs - 1; OptionSlave++) {
											uint64_t NumArgsLeft                                  = argc - (ArgvArg + OptionSlave);
											if (NumArgsLeft < CLI->Options[ArgvArg].NumSlaveIDs + 1) { // + 1 for the Argument result
												// idk
											}
											char *SlaveArg                                        = ConvertOptionString2SwitchFlag(argv[CurrentOption + (OptionSlave + 1)]);
											if (strcasecmp(SlaveArg, CLI->Switches[CLI->Switches[CurrentOption].ValidSlaves[OptionSlave]].SwitchFlag) == 0) {
												CLI->Options[CurrentOption].NumSlaveIDs          += 1;
												uint64_t NumSlaves                                = CLI->Options[CurrentOption].NumSlaveIDs;
												CLI->Options[CurrentOption].SlaveIDs[OptionSlave] = CLI->Switches[CLI->Switches[Switch].ValidSlaves[OptionSlave]].SwitchFlag;
											}
										}
										break;
								}
								CurrentOption                                                    += 1;
							} else {
								CLI->Options[CurrentOption].Argument2Option                       = (char*)argv[ArgvArg + (CLI->Options[CurrentOption].NumSlaveIDs + 1)];
							}
						}
					}
				}
			}
		}
	}
	
	int64_t CLIGetNumMatchingOptions(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
		int64_t NumMatchingOptions = 0LL;
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (OptionID > CLI->NumSwitches - 1 || OptionID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", OptionID, CLI->NumSwitches - 1);
		} else if (NumSlaves > CLI->NumSwitches - 1 || NumSlaves < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->Options[OptionID].NumSlaveIDs - 1);
		} else {
			for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) { // Loop over all the options
				if (CLI->Options[Option].SwitchID == OptionID) { // When a potential match is found, go ahead and check the slaves
					if (NumSlaves == 0 && CLI->Options[Option].NumSlaveIDs == 0) {
						NumMatchingOptions               += 1;
					} else if (NumSlaves > 0 && CLI->Options[Option].NumSlaveIDs > 0) {
						for (int64_t ParamSlave = 0LL; ParamSlave < NumSlaves - 1; ParamSlave++) {
							for (int64_t OptionSlave = 0LL; OptionSlave < CLI->Options[Option].NumSlaveIDs - 1; OptionSlave++) {
								if (SlaveIDs[ParamSlave] == CLI->Options[Option].SlaveIDs[OptionSlave]) {
									NumMatchingOptions   += 1;
								}
							}
						}
					}
				}
				//TODO: Now we just need to make sure that for switches like "-Input -RightEye" it doesn't return -Input -LeftEye
			}
		}
		return NumMatchingOptions;
	}
	
	int64_t CLIGetOptionNum(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
		int64_t MatchingOption = -1LL;
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (OptionID > CLI->NumSwitches - 1 || OptionID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", OptionID, CLI->NumSwitches - 1);
		} else if (NumSlaves > CLI->NumSwitches - 1 || NumSlaves < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->Options[OptionID].NumSlaveIDs - 1);
		} else {
			bool AllOptionsMatch   = No;
			for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) { // Loop over all the options
				if (CLI->Options[Option].SwitchID == OptionID) { // When a potential match is found, go ahead and check the slaves
					if (NumSlaves == 0 && CLI->Options[Option].NumSlaveIDs == 0) {
						AllOptionsMatch       = Yes;
						MatchingOption        = Option;
					} else {
						for (int64_t ParamSlave = 0LL; ParamSlave < NumSlaves - 1; ParamSlave++) {
							for (int64_t OptionSlave = 0LL; OptionSlave < CLI->Options[Option].NumSlaveIDs - 1; OptionSlave++) {
								if (SlaveIDs[ParamSlave] == CLI->Options[Option].SlaveIDs[OptionSlave]) {
									AllOptionsMatch       = Yes;
									MatchingOption        = Option;
								}
							}
						}
					}
				}
				//TODO: Now we just need to make sure that for switches like "-Input -RightEye" it doesn't return -Input -LeftEye
			}
		}
		return MatchingOption;
	}
	
	char *CLIGetOptionResult(CommandLineIO const *CLI, const int64_t OptionID) {
		char *Result = NULL;
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (OptionID > CLI->NumOptions - 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Option %d is greater than there are Options %d", OptionID, CLI->NumOptions - 1);
		} else if (OptionID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Invalid OptionID %d, valid IDs range from 0 - %d", OptionID, CLI->NumOptions - 1);
		} else {
			Result = CLI->Options[OptionID].Argument2Option;
		}
		return Result;
	}
	
	char *GetExtensionFromPath(const char *Path) {
		char *ExtensionString                  = NULL;
		if (Path == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Path Pointer is NULL");
		} else {
			uint64_t PathSize                  = strlen(Path) + 1;
			uint64_t ExtensionSize             = PathSize;
			uint64_t ExtensionDistanceFromEnd  = 0ULL;
			while (Path[ExtensionDistanceFromEnd] != 0x2E) {
				ExtensionSize                 -= 1;
				ExtensionDistanceFromEnd      += 1;
			}
			ExtensionSize                      = PathSize - ExtensionDistanceFromEnd;
			ExtensionString                    = calloc(ExtensionSize + BitIONULLStringSize, sizeof(char));
			if (ExtensionString == NULL) {
				BitIOLog(BitIOLog_ERROR, "CommandLineIO", __func__, "Couldn't allocate %lld bytes for the Extension String", ExtensionSize);
			} else {
				for (uint64_t ExtensionByte = 0LLU; ExtensionByte < ExtensionSize; ExtensionByte++) {
					ExtensionString[ExtensionByte] = Path[ExtensionByte + ExtensionDistanceFromEnd];
				}
			}
		}
		return ExtensionString;
	}
	
	void CommandLineIO_Deinit(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			if (CLI->Switches != NULL) {
				for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
					free(CLI->Switches[Switch].SwitchFlag);
					free(CLI->Switches[Switch].SwitchDescription);
					free(CLI->Switches[Switch].ValidSlaves);
				}
				free(CLI->Switches);
			}
			if (CLI->Options != NULL) {
				for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) {
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
