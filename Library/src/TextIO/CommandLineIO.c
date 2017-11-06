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
		uint64_t             NumSlaves;
		uint64_t             MaxActiveSlaves;
		uint64_t            *ValidSlaves;
		char                *SwitchFlag;
		char                *SwitchDescription;
	} CommandLineSwitch;
	
	/*!
	 @struct				CommandLineArgument
	 @abstract											"Contains the data to support a single argument".
	 @constant				Switch						"Which switch does this argument correspond to"?
	 @constant				NumDependentArguments		"How many Slave arguments were found in this argument?".
	 @constant				Slave						"Array of Slave argument numbers, to look up in CLI->Switches".
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
	 @constant				MinArguments				"The minimum number of switches to accept without dumping the help".
	 @constant				NumArguments				"The number of arguments present in argv, after extracting any Slave switches".
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
	typedef struct CommandLineIO {
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
	} CommandLineIO;
	
	CommandLineIO *CommandLineIO_Init(const uint64_t NumSwitches) {
		CommandLineIO *CLI = calloc(1, sizeof(CommandLineIO));
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
	
	void CLISetMinArguments(CommandLineIO *CLI, const uint64_t MinArguments) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			CLI->MinArguments = MinArguments;
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
	
	void CLISetSwitchType(CommandLineIO *CLI, uint64_t Switch, CLISwitchTypes Type) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			CLI->Switches[Switch].Type = Type;
			if (Type == StandaloneWithResult || Type == StandaloneNoResult || Type == SlaveType) {
				CLI->Switches[Switch].MaxActiveSlaves = 0;
				CLI->Switches[Switch].NumSlaves       = 0;
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
			CLI->Switches[Master].NumSlaves      += 1;
			uint64_t NumSlaves                    = CLI->Switches[Master].NumSlaves;
			CLI->Switches[Master].ValidSlaves     = realloc(CLI->Switches[Master].ValidSlaves, NumSlaves);
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
				if (CurrentSwitchType == MasterType) {
					printf("%s: %s%s", CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchDescription, BitIOLineEnding);
					for (uint64_t SlaveSwitch = 0ULL; SlaveSwitch < CLI->Switches[Switch].NumSlaves; SlaveSwitch++) {
						printf("\t%s: %s%s", CLI->Switches[SlaveSwitch].SwitchFlag, CLI->Switches[SlaveSwitch].SwitchDescription, BitIOLineEnding);
					}
				} else if (CurrentSwitchType == StandaloneType) {
					printf("%s: %s%s", CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchDescription, BitIOLineEnding);
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
		uint64_t ArgumentSwitchSize = (ArgumentStringSize - Bytes2RemoveFromArg) + BitIONULLStringSize;
		ArgumentSwitch = calloc(1, ArgumentSwitchSize * sizeof(char));
		if (ArgumentSwitch == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Not enough memory to allocate ArgumentSwitch which needs %ulld bytes of memory", ArgumentSwitchSize);
		} else {
			for (uint64_t Byte = Bytes2RemoveFromArg - BitIONULLStringSize; Byte < ArgumentStringSize; Byte++) {
				ArgumentSwitch[Byte] = ArgumentString[Byte];
			}
		}
		return ArgumentSwitch;
	}
	
	void ParseCommandLineArguments(CommandLineIO *CLI, const int argc, const char *argv[]) {
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			char *FirstArgumentAsFlag = ConvertArgumentString2SwitchFlag(argv[1]);
			char *HelpFlag            = ConvertArgumentString2SwitchFlag(CLI->Switches[CLI->HelpSwitch].SwitchFlag);
			if (argc < (int) CLI->MinArguments || strncasecmp(FirstArgumentAsFlag, HelpFlag, CLI->Switches[CLI->HelpSwitch].SwitchFlagSize) == 0) {
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
									if (CLI->Switches[Switch].Type == MasterType) {
										for (uint64_t DependentSwitch = 0ULL; DependentSwitch < CLI->Switches[Switch].NumSlaves; DependentSwitch++) {
											char *PotentialDependentArgument   = ConvertArgumentString2SwitchFlag(argv[ArgvArg + DependentSwitch]);
											uint64_t PotentialDependentArgSize = strlen(PotentialDependentArgument);
											if (CLI->Switches[Switch + DependentSwitch].SwitchFlagSize == PotentialDependentArgSize) {
												if (strncasecmp(PotentialDependentArgument, CLI->Switches[Switch + DependentSwitch].SwitchFlag, CLI->Switches[Switch + DependentSwitch].SwitchFlagSize) == 0) {
													CLI->Arguments[Argument].NumDependentArguments += 1;
													CLI->Arguments[Argument].DependentArguments[DependentSwitch] = Switch + DependentSwitch; // FIXME: Is this right?
												}
											}
										}
									} else if (CLI->Switches[Switch].Type == StandaloneType) {
										
									}
								}
							}
						}
					}
					CLI->NumArguments += 1; // you have to do this as we build the argument list, or the loop will break
				}
			}
		}
	}
	
	uint64_t CLIGetNumMatchingArguments(CommandLineIO *CLI, const uint64_t Master, const uint64_t NumSlaves, const uint64_t *Slaves) { // Gets the number of arguments that are composed of Master, and all the Slaves
		uint64_t       NumMatchingArguments                          = 0ULL;
		CLISwitchTypes CurrentSwitchType                             = UnknownType;
		uint64_t       NumDependentArgsInArg                         = 0ULL;
		for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
			NumDependentArgsInArg                                    = CLI->Arguments[Argument].NumDependentArguments;
			CurrentSwitchType                                        = CLI->Switches[CLI->Arguments[Argument].Switch].Type;
			if (CurrentSwitchType == MasterType && NumSlaves > 0) {
				for (uint64_t DependentArg = 0ULL; DependentArg < NumDependentArgsInArg; DependentArg++) {
					for (uint64_t Slave = 0ULL; Slave < NumSlaves; Slave++) {
						if (CLI->Arguments[Argument].DependentArguments[Slave] == Slaves[Slave]) {
							NumMatchingArguments                    += 1;
						}
					}
				}
			} else if (CurrentSwitchType == StandaloneType && NumSlaves == 0) {
				if (CLI->Arguments[Argument].Switch == Master) {
					NumMatchingArguments                            += 1;
				}
			} else {
				BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Incompatible Switch type %d and NumSlaves %d", CurrentSwitchType, NumSlaves);
			}
		}
		return NumMatchingArguments;
	}
	
	uint64_t CLIGetMatchingArgumentNum(CommandLineIO *CLI, const uint64_t Instance, const uint64_t Master, const uint64_t NumSlaves, const uint64_t *Slaves) { // Gets the argument number matching Master
		uint64_t       MatchingArgumentNum                           = 0ULL;
		uint64_t       MatchingInstance                              = 0ULL;
		CLISwitchTypes CurrentSwitchType                             = UnknownType;
		uint64_t       NumDependentArgsInArg                         = 0ULL;
		// Make sure that we track the number of found matching arguments so we can compare it to Instance, if they match, return; otherwise, keep looping.
		for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
			NumDependentArgsInArg                                    = CLI->Arguments[Argument].NumDependentArguments;
			CurrentSwitchType                                        = CLI->Switches[CLI->Arguments[Argument].Switch].Type;
			if (CurrentSwitchType == MasterType && NumSlaves > 0) {
				for (uint64_t DependentArg = 0ULL; DependentArg < NumDependentArgsInArg; DependentArg++) {
					for (uint64_t Slave = 0ULL; Slave < NumSlaves; Slave++) {
						if (CLI->Arguments[Argument].DependentArguments[Slave] == Slaves[Slave]) {
							MatchingArgumentNum                      = Argument;
							MatchingInstance                        += 1;
						}
					}
				}
			} else if (CurrentSwitchType == StandaloneType && NumSlaves == 0) {
				if (CLI->Arguments[Argument].Switch == Master) {
					MatchingArgumentNum                      = Argument;
					MatchingInstance                        += 1;
				}
			} else {
				BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Incompatible Switch type %d and NumSlaves %d", CurrentSwitchType, NumSlaves);
			}
		}
		return MatchingArgumentNum;
	}
	
	char *CLIGetArgumentResult(CommandLineIO const *CLI, const uint64_t Argument) {
		char *Result = NULL;
		if (CLI == NULL) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Argument > CLI->NumArguments) {
			BitIOLog(LOG_ERROR, BitIOLibraryName, __func__, "Argument %d is greater than there are arguments %d", Argument, CLI->NumArguments);
		} else {
			Result = CLI->Arguments[Argument].ArgumentResult;
		}
		return Result;
	}
	
	char *GetExtensionFromPath(const char *Path) { // Do not include the seperator aka period.
		uint64_t PathSize        = strlen(Path);
		uint64_t ExtensionOffset = 0ULL; // From the end
		uint64_t ExtensionSize   = PathSize - ExtensionOffset;
		for (uint64_t PathByte   = PathSize; PathByte > 0ULL; PathByte--) {
			if (Path[PathByte]  != 0x2E) { // 0x2E is a period in ASCII
				ExtensionOffset += 1;
			}
		}
		char *ExtensionString    = calloc(1, (ExtensionSize + BitIONULLStringSize) * sizeof(char));
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
			for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
				free(CLI->Switches[Switch].SwitchFlag);
				free(CLI->Switches[Switch].SwitchDescription);
				free(CLI->Switches[Switch].ValidSlaves);
			}
			for (uint64_t Argument = 0ULL; Argument < CLI->NumArguments; Argument++) {
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
