#include "../include/CommandLineIO.h"
#include "../include/BitIO.h"

#ifdef   __cplusplus
extern   "C" {
#endif
	
	/*!
	 @struct				CommandLineSwitch
	 @abstract											"Contains the data to support a switch".
	 @constant				SwitchType					"What type of switch is this"?
	 @constant				SwitchFlag					"What is this switch called, how do we identify it?".
	 @constant				SwitchFlagSize				"What is the number of bytes (if ASCII)/ code points (if UTF) of this switch?".
	 @constant				SwitchDescription			"Describe to the user what this switch does".
	 @constant				MaxActiveSlaves				"How many Slave switches can be active at once"?
	 @constant				SlaveIDs					"Pointer to an array that contains the list of aloowable Slave switches".
	 */
	typedef struct CommandLineSwitch {
		int64_t             *SlaveIDs;
		char                *SwitchFlag;
		char                *SwitchDescription;
		int64_t              MaxActiveSlaves;
		int64_t              NumSlaveIDs;
		CLISwitchTypes       SwitchType;
		uint8_t              SwitchFlagSize;
		uint8_t              DescriptionSize;
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
		char                *Argument2Option;
		char                *RangeString1;
		char                *RangeString2;
		int64_t             *SlaveIDs;
		int64_t              SwitchID;
		int64_t              NumOptionSlaves;
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
		} else if (MinOptions > CLI->NumSwitches || MinOptions < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MinOptions %s is invalid, it should be between 0 and %d", MinOptions, CLI->NumSwitches);
		} else {
			CLI->MinOptions = MinOptions;
		}
	}
	
	void CLISetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (HelpSwitch > CLI->NumSwitches || HelpSwitch < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "HelpSwitch %d is invalid, it should be between 0 and %d", HelpSwitch, CLI->NumSwitches);
		} else {
			CLI->HelpSwitch = HelpSwitch;
		}
	}
	
	void CLISetSwitchFlag(CommandLineIO *CLI, const int64_t SwitchID, char *Flag) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (Flag == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Flag String is NULL");
		} else if (SwitchID > CLI->NumSwitches || SwitchID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches);
		} else {
			CLI->Switches[SwitchID].SwitchFlag     = Flag;
			CLI->Switches[SwitchID].SwitchFlagSize = (uint8_t) strlen(Flag);
		}
	}
	
	void CLISetSwitchDescription(CommandLineIO *CLI, const int64_t SwitchID, char *Description) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (SwitchID > CLI->NumSwitches || SwitchID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches);
		} else if (Description == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Description String is NULL");
		} else {
			CLI->Switches[SwitchID].SwitchDescription = Description;
			CLI->Switches[SwitchID].DescriptionSize   = (uint8_t) strlen(Description);
		}
	}
	
	void CLISetSwitchType(CommandLineIO *CLI, const int64_t SwitchID, CLISwitchTypes SwitchType) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (SwitchID > CLI->NumSwitches || SwitchID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches);
		} else if (SwitchType == UnknownSwitchType) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You can not set SwitchID %d to UnknownSwitchType", SwitchID);
		} else {
			CLI->Switches[SwitchID].SwitchType           = SwitchType;
			if (SwitchType == SwitchCantHaveSlaves || SwitchType == SwitchIsASlave || SwitchType == ExistentialSwitch) {
				CLI->Switches[SwitchID].MaxActiveSlaves  = 0;
				CLI->Switches[SwitchID].NumSlaveIDs      = 0;
				CLI->Switches[SwitchID].SlaveIDs         = NULL;
			}
		}
	}
	
	void CLISetSwitchAsSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t SlaveID) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (MasterID > CLI->NumSwitches || MasterID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MasterID %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches);
		} else if (SlaveID > CLI->NumSwitches || SlaveID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SlaveID %d is invalid, it should be between 0 and %d", SlaveID, CLI->NumSwitches);
		} else {
			CLI->Switches[MasterID].NumSlaveIDs          += 1;
			
			CLI->Switches[MasterID].SlaveIDs              = realloc(CLI->Switches[MasterID].SlaveIDs, CLI->Switches[MasterID].NumSlaveIDs * sizeof(int64_t));
			CLI->Switches[MasterID].SlaveIDs[CLI->Switches[MasterID].NumSlaveIDs - 1] = SlaveID;
			//BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "MasterID %d, SlaveID %d, NumSlaves %d, ValidSlaveID %d", MasterID, SlaveID, CLI->Switches[MasterID].NumSlaveIDs, CLI->Switches[MasterID].SlaveIDs[CLI->Switches[MasterID].NumSlaveIDs - 1]);
		}
	}
	
	void CLISetSwitchMaxActiveSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxActiveSlaves) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (MasterID > CLI->NumSwitches || MasterID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MasterID %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches);
		} else if (MaxActiveSlaves > CLI->NumSwitches || MaxActiveSlaves < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MaxActiveSlaves %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches);
		} else {
			CLI->Switches[MasterID].MaxActiveSlaves = MaxActiveSlaves;
		}
	}
	
	static inline void DisplayCLIHelp(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			fprintf(stdout, "%s Options (-|--|/):%s", CLI->ProgramName, BitIONewLine);
			for (int64_t Switch = 0LL; Switch < CLI->NumSwitches; Switch++) {
				CLISwitchTypes CurrentSwitchType = CLI->Switches[Switch].SwitchType;
				fprintf(stdout, "%s: %s%s", CLI->Switches[Switch].SwitchFlag, CLI->Switches[Switch].SwitchDescription, BitIONewLine);
				if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->Switches[Switch].NumSlaveIDs > 0) {
					for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->Switches[Switch].NumSlaveIDs - 1; SlaveSwitch++) {
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
			if (CLI->ProgramName != NULL && CLI->ProgramVersion != NULL && CLI->ProgramAuthor != NULL && CLI->ProgramCopyright != NULL) {
				fprintf(stdout, "%s, v. %s by %s © %s%s", CLI->ProgramName, CLI->ProgramVersion, CLI->ProgramAuthor, CLI->ProgramCopyright, BitIONewLine);
			}
			
			if (CLI->ProgramDescription != NULL && CLI->ProgramLicenseName != NULL && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL && CLI->IsProprietary == No) {
				fprintf(stdout, "%s, Released under the \"%s\" %s, available at: %s%s", CLI->ProgramDescription, CLI->ProgramLicenseName, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, BitIONewLine);
			} else if (CLI->ProgramDescription != NULL && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL && CLI->IsProprietary == Yes) {
				fprintf(stdout, "%s, By using this software, you agree to the End User License Agreement %s, available at: %s%s", CLI->ProgramDescription, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, BitIONewLine);
			}
		}
	}
	
	static char *ArgumentString2SwitchFlag(const char *ArgumentString) {
		char *ArgumentSwitch = NULL;
		if (ArgumentString == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "OptionString Pointer is NULL");
		} else {
			uint8_t  ArgumentStringPrefixSize = 0;
			enum {
				ASCIIHyphen = 0x2D,
				ASCIIFSlash = 0x5C,
				ASCIIBSlash = 0x2F,
			};
			
			uint32_t ArgumentStringSize    = strlen(ArgumentString);
			
			if (ArgumentStringSize >= 2) {
				//BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "ArgumentString[0] = 0x%X, ArgumentString[1] = 0x%X", ArgumentString[0], ArgumentString[1]);
				if (ArgumentString[0] == ASCIIHyphen && ArgumentString[1] == ASCIIHyphen) {
					ArgumentStringPrefixSize  = 2;
				} else if (ArgumentString[0] == ASCIIBSlash || ArgumentString[0] == ASCIIFSlash || ArgumentString[0] == ASCIIHyphen) {
					ArgumentStringPrefixSize  = 1;
				}
			} else {
				BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "OptionString is not an option string");
			}
			uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
			ArgumentSwitch                 = calloc(1, ArgumentSwitchSize + BitIONULLStringSize);
			strncpy(ArgumentSwitch, &ArgumentString[ArgumentStringPrefixSize], ArgumentSwitchSize);
		}
		return ArgumentSwitch;
	}
	
	static int64_t GetSubStringsAbsolutePosition(int64_t StartOffset, int64_t StringSize, char *OptionString, char *SubString) {
		int64_t SubStringPosition = -1LL;
		int64_t SubStringSize = strlen(OptionString);
		int64_t MatchingChars = 0ULL;
		for (int64_t Char = StartOffset; Char < StringSize; Char++) {
			for (int64_t SubChar = 0; SubChar < SubStringSize; SubChar++) {
				if (OptionString[Char] == SubString[SubChar]) { // IDK how to specify that the whole substring matches
					MatchingChars += 1;
					/*
					 Loop over the string and substring, count the matching characters if you've got X matches and the SubString is X long, you've found the match. simply record the current position - X as the start
					 */
				} else {
					MatchingChars = 0;
				}
				if (MatchingChars == SubStringSize) {
					SubStringPosition = (Char + StartOffset) - SubChar;
				}
			}
			
		}
		return SubStringPosition;
	}
	
	static char *SplitInlineOption(int64_t OptionID, char *OptionString, uint64_t OptionStringSize, uint64_t NumSplitOptions) {
		/*
		 Ok so we loop over the argument string and look for the various delimiters (=|:|..)
		 */
		char *OutputStrings = NULL;
		enum Delimiters { // 3d, 3a, 2e 2e
			Colon     = 0x3A,
			Equal     = 0x3D,
			Period    = 0x2E,
		};
		
		typedef enum DelimiterTypes {
			UnknownDelimiter = 0,
			RangeDelimiter   = 1,
			EqualDelimiter   = 2,
			ColonDelimiter   = 3,
		} DelimiterTypes;
		
		for (uint64_t Byte = 0ULL; Byte < OptionStringSize; Byte++) {
			uint64_t DelimiterOffset = 0ULL;
			DelimiterTypes DelimiterType = UnknownDelimiter;
			if (OptionString[Byte] == Colon) {
				DelimiterType        = ColonDelimiter;
				DelimiterOffset      = Byte - 1;
			} else if (OptionString[Byte] == Equal) {
				DelimiterType        = EqualDelimiter;
				DelimiterOffset      = Byte - 1;
			} else if (Byte <= (OptionStringSize - 1) && (OptionString[Byte] == Period && OptionString[Byte + 1] == Period)) {
				DelimiterType        = RangeDelimiter;
				DelimiterOffset      = Byte - 1;
			}
			/* Ok so we need to go ahead and split the option, I guess it depends on the kind of delimiter */
			if (DelimiterType == RangeDelimiter || DelimiterType == ColonDelimiter) {
				/* Well I guess we need to start at the beginning of the range/colon operator, and work our way back until we find a non-ASCII numeral character, or we reach the beginning of the string ofc */
				uint64_t FirstRangeOffset = 0ULL;
				for (uint64_t RangeByte = DelimiterOffset; RangeByte > 0; RangeByte++) {
					if (OptionString[RangeByte] < 30 || OptionString[RangeByte] > 39) {
						FirstRangeOffset  = DelimiterOffset - RangeByte;
					}
				}
				// Now copy from FirstRangeOffset to DelimiterOffset
				uint64_t FirstRangeSize   = FirstRangeOffset - 0;
				char    *FirstRangeString = calloc(1, FirstRangeSize);
				for (uint64_t Byte = 0; Byte < FirstRangeSize; Byte++) {
					for (uint64_t RangeByte = FirstRangeOffset; Byte < FirstRangeSize; Byte++) {
						FirstRangeString[Byte] = OptionString[RangeByte];
					}
				}
				
				uint64_t SecondRangeOffset = 0ULL;
				for (uint64_t RangeByte = DelimiterOffset; RangeByte > 0; RangeByte++) {
					if (OptionString[RangeByte] < 30 || OptionString[RangeByte] > 39) {
						SecondRangeOffset  = DelimiterOffset - RangeByte;
					}
				}
				// Now copy from SecondRangeOffset to DelimiterOffset
				uint64_t SecondRangeSize   = FirstRangeOffset - 0;
				char    *SecondRangeString = calloc(1, SecondRangeSize);
				for (uint64_t Byte = 0; Byte < SecondRangeSize; Byte++) {
					for (uint64_t RangeByte = SecondRangeSize; Byte < SecondRangeSize; Byte++) {
						SecondRangeString[Byte] = OptionString[RangeByte];
					}
				}
				//uint64_t LongestSplitString = FirstRangeSize >= SecondRangeSize ? FirstRangeSize : SecondRangeSize;
				//OutputStrings = calloc(2, LongestSplitString);
				OutputStrings[0] = *FirstRangeString;
				OutputStrings[1] = *SecondRangeString;
			} else if (DelimiterType == ColonDelimiter) {
				
			} else if (DelimiterType == EqualDelimiter) {
				
			}
		}
		return OutputStrings;
	}
	
	void ParseCommandLineOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (argc < CLI->MinOptions || argc <= 1) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You entered %d options, the minimum is %d", argc - 1, CLI->MinOptions);
		} else {
			char    *FirstArgumentAsFlag                                         = ArgumentString2SwitchFlag(argv[1]);
			uint64_t FirstArgumentSize                                           = strlen(argv[1]);
			char    *HelpFlag                                                    = CLI->Switches[CLI->HelpSwitch].SwitchFlag;
			if (strncasecmp(FirstArgumentAsFlag, HelpFlag, FirstArgumentSize) == 0) {
				DisplayCLIHelp(CLI);
			} else {
				free(FirstArgumentAsFlag);
				DisplayProgramBanner(CLI);
				
				typedef enum ArgumentTypes {
					UnknownArg           = 0,
					OptionArg            = 1,
					SlaveArg             = 2,
					ResultArg            = 3,
				} ArgumentTypes;
				
				ArgumentTypes *ArgumentType                     = calloc((size_t)argc, sizeof(ArgumentTypes));
				for (int32_t ArgvArg1 = 1LL; ArgvArg1 < argc - 1; ArgvArg1++) {
					
					/*  Extract the argv string and remove the prefix */
					char     *CurrentArgumentString             = ArgumentString2SwitchFlag(argv[ArgvArg1]);
					int64_t   CurrentArgumentStringSize         = (int64_t) strlen(CurrentArgumentString);
					
					/* Look for inline delimiters */
					int64_t EqualPosition = GetSubStringsAbsolutePosition(0, CurrentArgumentStringSize, CurrentArgumentString, "=");
					if (EqualPosition != -1) { // We found an equal's sign!!! now we just need to make sure it's actually valid.
						
					}
					
					int64_t DoubleDotPosition = GetSubStringsAbsolutePosition(0, CurrentArgumentStringSize, CurrentArgumentString, "..");
					if (DoubleDotPosition != -1) { // We found range notation!!! now we just need to mke sure it's valid
						
					}
					
					for (int64_t Switch = 0LL; Switch < CLI->NumSwitches; Switch++) {
						CLISwitchTypes CurrentArgvSwitchType    = CLI->Switches[Switch].SwitchType;
						uint64_t CurrentArgvStringSize          = strlen(CurrentArgumentString);
						uint64_t ShortestStringSize             = CLI->Switches[Switch].SwitchFlagSize >= CurrentArgvStringSize ? CurrentArgvStringSize : CLI->Switches[Switch].SwitchFlagSize;
						int64_t SwitchFlagMatchesArgumentString = strncasecmp(CLI->Switches[Switch].SwitchFlag, CurrentArgumentString, ShortestStringSize);
						
						if  (SwitchFlagMatchesArgumentString == 0 && CurrentArgvSwitchType != UnknownSwitchType) {
							if (CurrentArgvSwitchType == SwitchMayHaveSlaves || CurrentArgvSwitchType == SwitchCantHaveSlaves || CurrentArgvSwitchType == ExistentialSwitch) {
								ArgumentType[ArgvArg1]          = OptionArg;
							} else if (CurrentArgvSwitchType == SwitchIsASlave) {
								ArgumentType[ArgvArg1]          = SlaveArg;
							}
						} else if (Switch == CLI->NumSwitches && SwitchFlagMatchesArgumentString != 0) { // SwitchFlagMatchesArgumentString != 0 &&
							ArgumentType[ArgvArg1]              = ResultArg;
						}
					}
				}
				
				int64_t NumOptionsFound = 0ULL;
				for (int32_t ArgType = 1LL; ArgType < argc - 1; ArgType++) {
					if (ArgumentType[ArgType] == OptionArg) {
						NumOptionsFound += 1;
					}
					BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "Argument = %d, ArgumentType = %d", ArgType, ArgumentType[ArgType]);
				}
				CLI->NumOptions = NumOptionsFound;
				BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "NumOptionsFound %d", NumOptionsFound);
				
				// So allocate a list of the number of slaves per option based on the number of options found
				CLI->Options                     = calloc(NumOptionsFound, sizeof(CommandLineOption));
				for (int32_t Arg = 1L; Arg < argc - 1; Arg++) {
					for (int64_t Option = 0LL; Option < NumOptionsFound; Option++) {
						if (ArgumentType[Arg] == SlaveArg) {
							// Ok so we loop over all the Options in ArgumentType
							// we record the latest Option, then record how many slaves there are until the next option.
							//NumSlavesForEachOption[Option] += 1;
							CLI->Options[Option].NumOptionSlaves += 1;
						}
						BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "Option %d, NumSlaves %d", Option, CLI->Options[Option].NumOptionSlaves);
					}
				}
				
				for (int64_t Option = 0LL; Option < CLI->NumOptions; Option++) {
					// Allocate the slaves
					if (CLI->Options[Option].NumOptionSlaves >= 1) {
						CLI->Options[Option].SlaveIDs = calloc(CLI->Options[Option].NumOptionSlaves, sizeof(int64_t));
					}
				}
				
				for (int64_t Option = 0LL; Option < CLI->NumOptions; Option++) {
					for (int32_t ArgvArg2 = 1; ArgvArg2 < argc - 1; ArgvArg2++) {
						if ((ArgumentType[ArgvArg2] == ResultArg || ArgumentType[ArgvArg2] == UnknownArg) && CLI->Options[Option].Argument2Option == NULL) {
							// Now we just set the options and do verification
							CLI->Options[Option].Argument2Option = (char*) argv[ArgvArg2];
							BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "Option %d ArgumentString = %s", Option, CLI->Options[Option].Argument2Option);
						}
					}
				}
				free(ArgumentType);
			}
		}
	}
	
	int64_t CLIGetNumMatchingOptions(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
		int64_t NumMatchingOptions = 0LL;
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (OptionID > CLI->NumSwitches || OptionID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", OptionID, CLI->NumSwitches);
		} else if (NumSlaves > CLI->NumSwitches || NumSlaves < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->Options[OptionID].NumOptionSlaves - 1);
		} else {
			for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) {
				if (CLI->Options[Option].SwitchID == OptionID) {
					if (NumSlaves == 0) {
						NumMatchingOptions               += 1;
					} else if (NumSlaves > 0 && CLI->Options[Option].NumOptionSlaves > 0)
						for (int64_t ParamSlave = 0LL; ParamSlave < NumSlaves - 1; ParamSlave++) {
							for (int64_t OptionSlave = 0LL; OptionSlave < CLI->Options[Option].NumOptionSlaves - 1; OptionSlave++) {
								if (SlaveIDs[ParamSlave] == CLI->Options[Option].SlaveIDs[OptionSlave]) {
									NumMatchingOptions   += 1;
								}
							}
						}
				}
			}
		}
		return NumMatchingOptions;
	}
	
	int64_t CLIGetOptionNum(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
		int64_t MatchingOption = -1LL;
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else if (OptionID > CLI->NumSwitches || OptionID < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", OptionID, CLI->NumSwitches);
		} else if (NumSlaves > CLI->NumSwitches || NumSlaves < 0) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->Options[OptionID].NumOptionSlaves - 1);
		} else {
			bool AllOptionsMatch   = No;
			for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) { // Loop over all the options
				if (CLI->Options[Option].SwitchID == OptionID) { // When a potential match is found, go ahead and check the slaves
					if (NumSlaves == 0 && CLI->Options[Option].NumOptionSlaves == 0) {
						AllOptionsMatch       = Yes;
						MatchingOption        = Option;
					} else {
						for (int64_t ParamSlave = 0LL; ParamSlave < NumSlaves - 1; ParamSlave++) {
							for (int64_t OptionSlave = 0LL; OptionSlave < CLI->Options[Option].NumOptionSlaves - 1; OptionSlave++) {
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
	
	static void CommandLineIO_ShowProgress(uint8_t PercentDone) {
		// we have a reolution of half a percent, the values 0-200 are acceptable.
		/*
		 How do we get the window size? I want to be able to resize the window
		 I want the bar to have an even number of dashes on each side with the number in the middle.
		 like this:
		 [-----                    Frame W/X 10.5%                           ] 10.5% gets rounded down to 10 which is 5 dashes
		 [-------------------------Cluster Y/Z 50.5%                         ] 50.5% gets rounded down to 50 which is 25 dashes
		 Also we'll need to know the size of the center string so we can keep both bars equal lengths
		 */
	}
	
	void CommandLineIO_Deinit(CommandLineIO *CLI) {
		if (CLI == NULL) {
			BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
		} else {
			if (CLI->Switches != NULL) {
				for (int64_t Switch = 0LL; Switch < CLI->NumSwitches; Switch++) {
					free(CLI->Switches[Switch].SwitchFlag);
					free(CLI->Switches[Switch].SwitchDescription);
					free(CLI->Switches[Switch].SlaveIDs);
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
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	void PrintCommandLineOptions(CommandLineIO *CLI) {
		BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "NumOptions %d", CLI->NumOptions);
		for (int64_t Option = 0LL; Option < CLI->NumOptions; Option++) {
			if (CLI->Options[Option].SwitchID >= 0) {
				BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "OptionID %d, OptionSwitchID %d, OptionFlag %s, OptionType %d, OptionArgument %s", Option, CLI->Options[Option].SwitchID, CLI->Switches[CLI->Options[Option].SwitchID].SwitchFlag, CLI->Switches[CLI->Options[Option].SwitchID].SwitchType, CLI->Options[Option].Argument2Option);
				if (CLI->Options[Option].NumOptionSlaves >= 1) {
					for (int64_t OptionSlave = 0LL; OptionSlave < CLI->Options[Option].NumOptionSlaves; OptionSlave++) {
						BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "SlaveID %d", OptionSlave);
					}
				}
			}
		}
	}
	
#ifdef   __cplusplus
}
#endif
