#include "../include/CommandLineIO.h"
#include "../include/BitIO.h"

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*
     Maybe what I should do, is remove the NULL pointer checking from the functions that set/get the value, to the functions that actually initalize the types in the first place.
     */
    
    /*!
     @struct                 CommandLineSwitch
     @abstract                                           "Contains the data to support a switch".
     @constant               SwitchType                  "What type of switch is this"?
     @constant               ArgumentType                "What type of argument does this switch accept"?
     @constant               Name                        "What is this switch called, how do we identify it"?
     @constant               NameSize                    "What is the number of bytes (if ASCII)/ code points (if UTF) of this switch"?
     @constant               Description                 "Describe to the user what this switch does".
     @constant               DescriptionSize             "What is the number of bytes/code points of this switch"?
     @constant               NumPotentialSlaves          "How many potential slaves are there"?
     @constant               MaxConcurrentSlaves         "How many Slave switches can be active at once"?
     @constant               PotentialSlaves             "Pointer to an array that contains the list of aloowable Slave switches".
     */
    typedef struct CommandLineSwitch {
        CLISwitchTypes       SwitchType;
        CLIArgumentTypes     ArgumentType;
        char                *Name;
        uint8_t              NameSize;
        char                *Description;
        uint8_t              DescriptionSize;
        int64_t              NumPotentialSlaves;
        int64_t              MaxConcurrentSlaves;
        int64_t             *PotentialSlaves;
    } CommandLineSwitch;
    
    /*!
     @struct                 CommandLineOption
     @abstract                                           "Contains the data to support a single argument".
     @constant               SwitchID                    "Which switch does this argument correspond to"?
     @constant               NumPotentialSlaves          "How many Slave Options were found in this argument?".
     @constant               OptionSlaves                "Array of Slave SwitchIDs, to look up in CLI->SwitchIDs".
     @constant               Argument                    "If there is a path or other result expected for this switch's argument, it'll be here".
     */
    typedef struct CommandLineOption {
        int64_t              SwitchID;
        int64_t              NumOptionSlaves;
        int64_t             *OptionSlaves;
        char                *Argument;
    } CommandLineOption;
    
    /*!
     @struct                 CommandLineIO
     @abstract                                           "Contains all the information, and relationships between switches on the command line".
     @constant               NumSwitches                 "How many switches are there?".
     @constant               NumOptions                  "The number of Options present in argv, after extracting any Slave switches".
     @constant               MinOptions                  "The minimum number of switches to accept without dumping the help".
     @constant               HelpSwitch                  "Which switch displays the help"?
     @constant               Switches                    "Pointer to an array of switches".
     @constant               Options                     "Pointer to an array of Options".
     @constant               IsProprietary               "Is this program proprietary?".
     @constant               ProgramName                 "What is the name of this program?".
     @constant               ProgramAuthor               "Who wrote this program?".
     @constant               ProgramDescription          "What does this program do?".
     @constant               ProgramVersion              "What is the version of this program?".
     @constant               ProgramCopyright            "String containing the copyright years like "2015 - 2017"".
     @constant               ProgramLicenseName          "Short name of the license, like "3-clause BSD", etc".
     @constant               ProgramLicenseDescription   "Describe the license or EULA".
     @constant               ProgramLicenseURL           "URL for the EULA, ToS, or Open source license".
     */
    typedef struct CommandLineIO {
        int64_t              NumSwitches;
        int64_t              NumOptions;
        int64_t              MinOptions;
        int64_t              HelpSwitch;
        uint16_t             ConsoleWidth;
        uint16_t             ConsoleHeight;
        char                *ProgramName;
        char                *ProgramAuthor;
        char                *ProgramDescription;
        char                *ProgramVersion;
        char                *ProgramCopyright;
        char                *ProgramLicenseName;
        char                *ProgramLicenseDescription;
        char                *ProgramLicenseURL;
        CommandLineSwitch   *SwitchIDs;
        CommandLineOption   *OptionIDs;
        bool                 IsProprietary;
    } CommandLineIO;
    
    CommandLineIO *CommandLineIO_Init(const size_t NumSwitches) {
        CommandLineIO *CLI   = calloc(1, sizeof(CommandLineIO));
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Couldn't allocate CommandLineIO");
        } else if (NumSwitches == 0) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "NumSwitches %d must be greater than or equal to 1", NumSwitches);
        } else {
            CLI->NumSwitches = (int64_t) NumSwitches;
            CLI->SwitchIDs   = calloc(NumSwitches, sizeof(CommandLineSwitch));
            if (CLI->SwitchIDs == NULL) {
                free(CLI);
                BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Couldn't allocate %d CommandLineSwitches", NumSwitches);
                exit(EXIT_FAILURE);
            }
#if   (BitIOTargetOS == BitIOWindowsOS)
            // Get the size of the console with Windows' bullshit
            CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
            CLI->ConsoleHeight = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;
            CLI->ConsoleWidth = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#elif (BitIOTargetOS == BitIOPOSIXOS)
            // Get the size of the console with IOCTL
            struct winsize WindowSize;
            ioctl(0, TIOCGWINSZ, &WindowSize);
            CLI->ConsoleWidth  = WindowSize.ws_row;
            CLI->ConsoleHeight = WindowSize.ws_col;
#endif
        }
        return CLI;
    }
    
    void CLISetName(CommandLineIO *CLI, char *Name) {
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else if (Name == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "The Name string is NULL");
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
                CLI->IsProprietary         = Yes;
            } else {
                CLI->IsProprietary         = No;
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
    
    void CLISetSwitchName(CommandLineIO *CLI, const int64_t SwitchID, char *Name) {
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else if (Name == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "Name String is NULL");
        } else if (SwitchID > CLI->NumSwitches || SwitchID < 0) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches);
        } else {
            CLI->SwitchIDs[SwitchID].Name     = Name;
            CLI->SwitchIDs[SwitchID].NameSize = (uint8_t) strlen(Name);
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
            CLI->SwitchIDs[SwitchID].Description     = Description;
            CLI->SwitchIDs[SwitchID].DescriptionSize = (uint8_t) strlen(Description);
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
            CLI->SwitchIDs[SwitchID].SwitchType              = SwitchType;
            if (SwitchType == SwitchCantHaveSlaves || SwitchType == SwitchIsASlave || SwitchType == ExistentialSwitch) {
                CLI->SwitchIDs[SwitchID].MaxConcurrentSlaves = 0;
                CLI->SwitchIDs[SwitchID].NumPotentialSlaves  = 0;
                CLI->SwitchIDs[SwitchID].PotentialSlaves     = NULL;
            }
        }
    }
    
    void CLISetSwitchArgumentType(CommandLineIO *CLI, const int64_t SwitchID, CLIArgumentTypes ArgumentType) {
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else if (SwitchID > CLI->NumSwitches || SwitchID < 0) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches);
        } else if (ArgumentType == UnknownArgumentType) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You can not set SwitchID %d to UnknownArgumentType", SwitchID);
        } else {
            CLI->SwitchIDs[SwitchID].ArgumentType = ArgumentType;
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
            CLI->SwitchIDs[MasterID].NumPotentialSlaves          += 1;
            
            CLI->SwitchIDs[MasterID].PotentialSlaves              = realloc(CLI->SwitchIDs[MasterID].PotentialSlaves, CLI->SwitchIDs[MasterID].NumPotentialSlaves * sizeof(int64_t));
            CLI->SwitchIDs[MasterID].PotentialSlaves[CLI->SwitchIDs[MasterID].NumPotentialSlaves - 1] = SlaveID;
            //BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "MasterID %d, SlaveID %d, NumSlaves %d, ValidSlaveID %d", MasterID, SlaveID, CLI->SwitchIDs[MasterID].NumPotentialSlaves, CLI->SwitchIDs[MasterID].SlaveIDs[CLI->SwitchIDs[MasterID].NumPotentialSlaves - 1]);
        }
    }
    
    void CLISetSwitchMaxConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxConcurrentSlaves) {
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else if (MasterID > CLI->NumSwitches || MasterID < 0) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MasterID %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches);
        } else if (MaxConcurrentSlaves > CLI->NumSwitches || MaxConcurrentSlaves < 0) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "MaxConcurrentSlaves %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches);
        } else {
            CLI->SwitchIDs[MasterID].MaxConcurrentSlaves = MaxConcurrentSlaves;
        }
    }
    
    static inline void DisplayCLIHelp(CommandLineIO *CLI) {
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else {
            fprintf(stdout, "%s Options (-|--|/):%s", CLI->ProgramName, BitIONewLine);
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches; Switch++) {
                CLISwitchTypes CurrentSwitchType = CLI->SwitchIDs[Switch].SwitchType;
                fprintf(stdout, "%s: %s%s", CLI->SwitchIDs[Switch].Name, CLI->SwitchIDs[Switch].Description, BitIONewLine);
                if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->SwitchIDs[Switch].NumPotentialSlaves > 0) {
                    for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; SlaveSwitch++) {
                        fprintf(stdout, "\t%s: %s%s", CLI->SwitchIDs[SlaveSwitch].Name, CLI->SwitchIDs[SlaveSwitch].Description, BitIONewLine);
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
            
            uint32_t ArgumentStringSize    = strlen(ArgumentString);
            
            if (ArgumentStringSize >= 2) {
                //BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "ArgumentString[0] = 0x%X, ArgumentString[1] = 0x%X", ArgumentString[0], ArgumentString[1]);
                if (ArgumentString[0] == ASCIIHyphen && ArgumentString[1] == ASCIIHyphen) {
                    ArgumentStringPrefixSize  = 2;
                } //else if (ArgumentString[0] == ASCIIBSlash || ArgumentString[0] == ASCIIFSlash || ArgumentString[0] == ASCIIHyphen) {
                //ArgumentStringPrefixSize  = 1;
                //}
            } else {
                BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "OptionString is not an option string");
            }
            uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
            ArgumentSwitch                 = calloc(ArgumentSwitchSize + BitIONULLStringSize, sizeof(uint8_t));
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
    
    void ParseCommandLineOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else if (argc < CLI->MinOptions || argc <= 1) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "You entered %d options, the minimum is %d", argc - 1, CLI->MinOptions);
        } else {
            /* Ok, so the first thing we need to do is loop over the arguments, and loop over the actual bytes of each argument */
            int32_t  CurrentArgument        = 1LL;
            
            do {
                // Extract the first argument as a switch.
                char    *Argument         = (char*) argv[CurrentArgument];
                char    *ArgumentFlag     = ArgumentString2SwitchFlag(Argument);
                uint64_t ArgumentFlagSize = strlen(ArgumentFlag);
                // now loop over the switches
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches; Switch++) {
                    // now compare ArgumentFlag to Switch
                    // Which string is smaller?
                    uint64_t SmallestStringSize = ArgumentFlagSize <= CLI->SwitchIDs[Switch].NameSize ? ArgumentFlagSize : CLI->SwitchIDs[Switch].NameSize;
                    if (strncasecmp(ArgumentFlag, CLI->SwitchIDs[Switch].Name, SmallestStringSize) == 0) { // ArgumentFlag matches this switch
                                                                                                           // Set up the Option here
                        CLI->NumOptions += 1;
                        if (CLI->NumOptions == 1) {
                            CLI->OptionIDs = calloc(1, sizeof(CommandLineOption));
                        } else {
                            CLI->OptionIDs = realloc(CLI->OptionIDs, CLI->NumOptions * sizeof(CommandLineOption));
                        }
                        CLI->OptionIDs[CLI->NumOptions - 1].SwitchID = Switch;
                        
                        
                        // so now we need to look over the current switch's MaxConcurrentSlaves and NumPotentialSlaves wait num potential slaves will be indexxed by the loop
                        for (int64_t ArgSlave = 1LL; ArgSlave < CLI->SwitchIDs[Switch].MaxConcurrentSlaves; ArgSlave++) {
                            char *PotentialSlaveArg        = (char*) argv[CurrentArgument + ArgSlave];
                            uint64_t PotentialSlaveArgSize = strlen(PotentialSlaveArg);
                            for (int64_t Slave = 0LL; Slave < CLI->SwitchIDs[Switch].NumPotentialSlaves; Slave++) {
                                // Ok so now we loop over the values stores in PoentialSlaves, looking up their switch flags.
                                char *PotentialSlaveFlag   = CLI->SwitchIDs[CLI->SwitchIDs[Switch].PotentialSlaves[Slave]].Name;
                                // Now compare PotentialSlaveFlag to PotentialSlaveArg
                                uint64_t SmallestSlaveFlag = PotentialSlaveArgSize <= CLI->SwitchIDs[CLI->SwitchIDs[Switch].PotentialSlaves[Slave]].NameSize ? PotentialSlaveArgSize : CLI->SwitchIDs[CLI->SwitchIDs[Switch].PotentialSlaves[Slave]].NameSize;
                                if (strncasecmp(PotentialSlaveArg, PotentialSlaveFlag, SmallestSlaveFlag) == 0) {
                                    // We found a slave in the next argument so set the option up.
                                    CLI->OptionIDs[CLI->NumOptions - 1].NumOptionSlaves += 1;
                                    CLI->OptionIDs[CLI->NumOptions - 1].OptionSlaves[CLI->OptionIDs[CLI->NumOptions - 1].NumOptionSlaves - 1] = CLI->SwitchIDs[Switch].PotentialSlaves[Slave];
                                }
                                free(PotentialSlaveFlag);
                            }
                            free(PotentialSlaveArg);
                        }
                    }
                }
                free(Argument);
                free(ArgumentFlag);
            } while (CurrentArgument < argc);
        }
    }
    
    static char **SplitInlineArgument(const char *ArgumentString, const uint64_t ArgumentStringSize, uint64_t NumSplitArguments) {
        /*
         Ok, so we take in an argument string and it's size.
         We output whatever the argument contains in multiple strings.
         
         if our InlineArgument is: --Input:LeftEye=/Users/Marcus/Desktop/ElephantsDream_Left_%05d.png
         then we output          : Input LeftEye /Users/Marcus/Desktop/ElephantsDream_Left_%05d.png
         Symbols to break strings apart with: (:|=|-|..|--|)
         Theoretically this is simple.
         
         You simply loop over the string comparing each byte to those symbols until you get to the end of the string and you keep a count of the number of delimiters found, each's size, and offset from 0.
         We need to do this in 2 passes, the first to find the number of delimiters there are.
         */
        
        uint64_t  NumDelimitersFound = 0ULL;
        uint64_t  FoundStrings       = 0ULL;
        uint64_t *SubStringSize      = NULL;
        uint64_t *SubStringOffset    = NULL;
        
        uint64_t  CurrentDelimiter   = 0ULL;
        
        enum Delimiters { // 3d, 3a, 2e 2e
            Colon     = 0x3A,
            Equal     = 0x3D,
            Hyphen    = 0x2D,
            Period    = 0x2E,
        };
        
        for (uint64_t ArgumentByte = 0ULL; ArgumentByte < ArgumentStringSize; ArgumentByte++) {
            uint8_t CurrentByte = ArgumentString[ArgumentByte];
            uint8_t NextByte    = ArgumentString[ArgumentByte + 1];
            if (CurrentByte == Colon || CurrentByte == Equal || (CurrentByte == Hyphen && NextByte == Hyphen) || (CurrentByte == Period && NextByte == Period) || CurrentByte == Hyphen || CurrentByte == Period) {
                NumDelimitersFound += 1;
                if ((CurrentByte == Hyphen && NextByte == Hyphen) || (CurrentByte == Period && NextByte == Period)) {
                }
            }
        }
        
        SubStringSize   = calloc(NumDelimitersFound, sizeof(uint64_t));
        SubStringOffset = calloc(NumDelimitersFound, sizeof(uint64_t));
        
        for (uint64_t ArgumentByte = 0ULL; ArgumentByte < ArgumentStringSize; ArgumentByte++) {
            uint8_t CurrentByte = ArgumentString[ArgumentByte];
            uint8_t NextByte    = ArgumentString[ArgumentByte + 1];
            if (CurrentByte == Colon || CurrentByte == Equal || (CurrentByte == Hyphen && NextByte == Hyphen) || (CurrentByte == Period && NextByte == Period) || CurrentByte == Hyphen || CurrentByte == Period) {
                CurrentDelimiter += 1;
                if ((CurrentByte == Hyphen && NextByte == Hyphen) || (CurrentByte == Period && NextByte == Period)) {
                    SubStringOffset[CurrentDelimiter] = NextByte;
                    SubStringSize[CurrentDelimiter]   = SubStringOffset[CurrentDelimiter] - ArgumentStringSize;
                } else {
                    SubStringOffset[CurrentDelimiter] = CurrentByte;
                    SubStringSize[CurrentDelimiter]   = SubStringOffset[CurrentDelimiter] - ArgumentStringSize;
                }
            }
        }
        
        // Ok, now we just go ahead and copy the substrings out
        // and now we go ahead and create the number of srings found * their size, then put pointers to those strings into an array of pointers
        FoundStrings         = NumDelimitersFound - 1;
        char *StringPointers = calloc(FoundStrings, sizeof(char*));
        for (uint64_t String = 0ULL; String < FoundStrings; String++) {
            // In here allocate a string for each found string
            // Allocate the string here
            char *StringPointer    = calloc(SubStringSize[String], sizeof(uint8_t));
            StringPointers[String] = *StringPointer;
        }
        
        NumSplitArguments = FoundStrings;
        return &StringPointers;
    }
    
    int64_t CLIGetNumMatchingOptions(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
        int64_t NumMatchingOptions = 0LL;
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else if (OptionID > CLI->NumSwitches || OptionID < 0) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "SwitchID %d is invalid, it should be between 0 and %d", OptionID, CLI->NumSwitches);
        } else if (NumSlaves > CLI->NumSwitches || NumSlaves < 0) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        } else {
            for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) {
                if (CLI->OptionIDs[Option].SwitchID == OptionID) {
                    if (NumSlaves == 0) {
                        NumMatchingOptions               += 1;
                    } else if (NumSlaves > 0 && CLI->OptionIDs[Option].NumOptionSlaves > 0)
                        for (int64_t ParamSlave = 0LL; ParamSlave < NumSlaves - 1; ParamSlave++) {
                            for (int64_t OptionSlave = 0LL; OptionSlave < CLI->OptionIDs[Option].NumOptionSlaves - 1; OptionSlave++) {
                                if (SlaveIDs[ParamSlave] == CLI->OptionIDs[Option].OptionSlaves[OptionSlave]) {
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
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        } else {
            bool AllOptionsMatch   = No;
            for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) { // Loop over all the options
                if (CLI->OptionIDs[Option].SwitchID == OptionID) { // When a potential match is found, go ahead and check the slaves
                    if (NumSlaves == 0 && CLI->OptionIDs[Option].NumOptionSlaves == 0) {
                        AllOptionsMatch       = Yes;
                        MatchingOption        = Option;
                    } else {
                        for (int64_t ParamSlave = 0LL; ParamSlave < NumSlaves - 1; ParamSlave++) {
                            for (int64_t OptionSlave = 0LL; OptionSlave < CLI->OptionIDs[Option].NumOptionSlaves - 1; OptionSlave++) {
                                if (SlaveIDs[ParamSlave] == CLI->OptionIDs[Option].OptionSlaves[OptionSlave]) {
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
            Result = CLI->OptionIDs[OptionID].Argument;
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
    
    void CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, char *Strings, uint64_t *Numerator, uint64_t *Denominator) {
        /*
         How do we get the window size? I want to be able to resize the window
         I want the bar to have an even number of dashes on each side with the number in the middle.
         like this:
         [--                        Shot U/V 2%                              ]
         [-----                    Frame W/X 10%                             ] 10.5% gets rounded down to 10 which is 5 dashes
         [-------------------------Block Y/Z 51%-                            ] 50.5% gets rounded up to 51 which is 26 dashes
         Also we'll need to know the size of the center string so we can keep both bars equal lengths
         */
        /*
         Ok, so we know the width of the console, now we need to figure out the sizes of each of the strings
         */
        
        size_t *StringSize = calloc(NumItems2Display, sizeof(size_t));
        for (uint8_t Item = 0; Item < NumItems2Display; Item++) { // Get the size of the strings
            StringSize[Item] = strlen(&Strings[Item]);
            // huh, well we need 2 characters for the brackets.
            
        }
        // Number of seperators for each string
        size_t *NumProgressIndicatorsPerString = calloc(NumItems2Display, sizeof(size_t));
        char   *ActualStrings2Print            = calloc(NumItems2Display * CLI->ConsoleWidth, sizeof(char));
        for (uint8_t String = 0; String < NumItems2Display; String++) { // Actually create the strings
            // Subtract 2 for the brackets, + the size of each string from the actual width of the console window
            NumProgressIndicatorsPerString[String] = CLI->ConsoleWidth - (2 + StringSize[String]); // what if it's not even?
            
            
            
            uint8_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
            
            uint8_t HalfOfTheIndicators = (PercentComplete / 2);
            
            // Now we go ahead and memset a string with the proper number of indicators
            char *Indicator = calloc(CLI->ConsoleWidth, sizeof(char));
            memset(Indicator, '-', HalfOfTheIndicators);
            
            sprintf(&ActualStrings2Print[String], "[%s%s %d/%d %hhu/% %s]", Indicator, Strings[String], Numerator, Denominator, PercentComplete, Indicator);
            fprintf(stdout, "%s%s", &ActualStrings2Print[String], BitIONewLine);
            free(Indicator);
        }
        free(StringSize);
        free(NumProgressIndicatorsPerString);
        free(ActualStrings2Print);
    }
    
    void CommandLineIO_Deinit(CommandLineIO *CLI) {
        if (CLI == NULL) {
            BitIOLog(BitIOLog_ERROR, BitIOLibraryName, __func__, "CommandLineIO Pointer is NULL");
        } else {
            if (CLI->SwitchIDs != NULL) {
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches; Switch++) {
                    free(CLI->SwitchIDs[Switch].Name);
                    free(CLI->SwitchIDs[Switch].Description);
                    free(CLI->SwitchIDs[Switch].PotentialSlaves);
                }
                free(CLI->SwitchIDs);
            }
            if (CLI->OptionIDs != NULL) {
                for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) {
                    free(CLI->OptionIDs[Option].OptionSlaves);
                    free(CLI->OptionIDs[Option].Argument);
                }
                free(CLI->OptionIDs);
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
            if (CLI->OptionIDs[Option].SwitchID >= 0) {
                BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "OptionID %d, OptionSwitchID %d, OptionFlag %s, OptionType %d, OptionArgument %s", Option, CLI->OptionIDs[Option].SwitchID, CLI->SwitchIDs[CLI->OptionIDs[Option].SwitchID].Name, CLI->SwitchIDs[CLI->OptionIDs[Option].SwitchID].SwitchType, CLI->OptionIDs[Option].Argument);
                if (CLI->OptionIDs[Option].NumOptionSlaves >= 1) {
                    for (int64_t OptionSlave = 0LL; OptionSlave < CLI->OptionIDs[Option].NumOptionSlaves; OptionSlave++) {
                        BitIOLog(BitIOLog_DEBUG, BitIOLibraryName, __func__, "SlaveID %d", OptionSlave);
                    }
                }
            }
        }
    }
    
#ifdef   __cplusplus
}
#endif
