#include <stdarg.h>                   /* Included for the variadic argument support macros */
#include <stdint.h>                   /* Included for u/intX_t */
#include <stdio.h>                    /* Included for fprintf, STD IN/OUT/ERR, sprintf */
#include <stdlib.h>                   /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, realloc, and free */
#include <string.h>                   /* Included for memset */

#include "../include/Macros.h"        /* Included for NewLineWithNULLSize, FoundationIOTargetOS */
#include "../include/Log.h"           /* Included for LogTypes */
#include "../include/CommandLineIO.h"


#if    (FoundationIOTargetOS == POSIXOS)
#include <sys/ioctl.h>                /* Included for the terminal size */
#include <sys/ttycom.h>               /* Included for winsize, TIOCGWINSZ */
#elif  (FoundationIOTargetOS == WindowsOS)
#include <wincon.h>                   /* Included for getting the terminal size */
#endif

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*
     Maybe what I should do, is remove the NULL pointer checking from the functions that set/get the value, to the functions that actually initalize the types in the first place.
     */
    
    /*!
     */
    enum CommandLineIOConstants {
        NULLTerminator              = 0,
        NULLTerminatorSize          = 1,
    };
    
    /*!
     @struct                 CommandLineSwitch
     @abstract                                           "Contains the data to support a switch".
     @constant               SwitchType                  "What type of switch is this"?
     @constant               ArgumentType                "What type of argument does this switch accept"?
     @constant               Name                        "What is this switch called, how do we identify it"?
     @constant               Description                 "Describe to the user what this switch does".
     @constant               NameSize                    "What is the number of code units of this string"?
     @constant               DescriptionSize             "What is the number of code units of this string"?
     @constant               NumPotentialSlaves          "How many potential slaves are there"?
     @constant               MaxConcurrentSlaves         "How many Slave switches can be active at once"?
     @constant               PotentialSlaves             "Pointer to an array that contains the list of aloowable Slave switches".
     */
    typedef struct CommandLineSwitch {
        UTF8                *Name;
        UTF8                *Description;
        int64_t             *PotentialSlaves;
        int64_t              NumPotentialSlaves;
        int64_t              MaxConcurrentSlaves;
        CLISwitchTypes       SwitchType;
        CLIArgumentTypes     ArgumentType;
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
        int64_t             *OptionSlaves;
        UTF8                *Argument;
        int64_t              SwitchID;
        int64_t              NumOptionSlaves;
    } CommandLineOption;
    
    /*!
     @struct                 CommandLineIO
     @abstract                                             "Contains all the information, and relationships between switches on the command line".
     @constant               ProgramName                   "What is the name of this program"?
     @constant               ProgramAuthor                 "Who wrote this program"?
     @constant               ProgramDescription            "What does this program do"?
     @constant               ProgramVersion                "What is the version of this program"?
     @constant               ProgramCopyright              "String containing the copyright years like "2015 - 2017"".
     @constant               ProgramLicenseName            "Short name of the license, like "3-clause BSD", etc".
     @constant               ProgramLicenseDescription     "Describe the license or EULA".
     @constant               ProgramLicenseURL             "URL for the EULA, ToS, or Open source license".
     @constant               NumSwitches                   "How many switches are there?".
     @constant               NumOptions                    "The number of Options present in argv, after extracting any Slave switches".
     @constant               MinOptions                    "The minimum number of switches to accept without dumping the help".
     @constant               HelpSwitch                    "Which switch displays the help"?
     @constant               Switches                      "Pointer to an array of switches".
     @constant               Options                       "Pointer to an array of Options".
     @constant               IsProprietary                 "Is this program proprietary"?
     */
    typedef struct CommandLineIO {
        UTF8                *ProgramName;
        UTF8                *ProgramAuthor;
        UTF8                *ProgramDescription;
        UTF8                *ProgramVersion;
        UTF8                *ProgramCopyright;
        UTF8                *ProgramLicenseName;
        UTF8                *ProgramLicenseDescription;
        UTF8                *ProgramLicenseURL;
        CommandLineSwitch   *SwitchIDs;
        CommandLineOption   *OptionIDs;
        int64_t              NumSwitches;
        int64_t              NumOptions;
        int64_t              MinOptions;
        int64_t              HelpSwitch;
        uint16_t             ConsoleWidth;
        uint16_t             ConsoleHeight;
        bool                 IsProprietary;
    } CommandLineIO;
    
    CommandLineIO *CommandLineIO_Init(const int64_t NumSwitches) {
        CommandLineIO *CLI       = calloc(1, sizeof(CommandLineIO));
        if (CLI != NULL && NumSwitches >= 0) {
            CLI->SwitchIDs       = calloc(NumSwitches, sizeof(CommandLineSwitch));
            if (CLI->SwitchIDs != NULL) {
                CLI->NumSwitches = NumSwitches;
            } else {
                free(CLI);
                Log(Log_ERROR, __func__, u8"Couldn't allocate %d CommandLineSwitches", NumSwitches);
                exit(EXIT_FAILURE);
            }
#if   (FoundationIOTargetOS == POSIXOS)
            struct winsize WindowSize;
            ioctl(0, TIOCGWINSZ, &WindowSize);
            CLI->ConsoleWidth    = WindowSize.ws_row;
            CLI->ConsoleHeight   = WindowSize.ws_col;
#elif (FoundationIOTargetOS == WindowsOS)
            SetConsoleCP(WindowsUTF8CodePage);
            CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
            CLI->ConsoleHeight   = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;
            CLI->ConsoleWidth    = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"Couldn't allocate CommandLineIO");
        } else if (NumSwitches == 0) {
            Log(Log_ERROR, __func__, u8"NumSwitches %d must be greater than or equal to 1", NumSwitches);
        }
        return CLI;
    }
    
    void CLISetName(CommandLineIO *CLI, UTF8 *Name) {
        if (CLI != NULL && Name != NULL) {
            CLI->ProgramName     = Name;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, u8"The Name string is NULL");
        }
    }
    
    void CLISetVersion(CommandLineIO *CLI, UTF8 *Version) {
        if (CLI != NULL && Version != NULL) {
            CLI->ProgramVersion     = Version;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (Version == NULL) {
            Log(Log_ERROR, __func__, u8"The Version String is NULL");
        }
    }
    
    void CLISetDescription(CommandLineIO *CLI, UTF8 *Description) {
        if (CLI != NULL && Description != NULL) {
            CLI->ProgramDescription     = Description;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, u8"The Description String is NULL");
        }
    }
    
    void CLISetAuthor(CommandLineIO *CLI, UTF8 *Author) {
        if (CLI != NULL && Author != NULL) {
            CLI->ProgramAuthor     = Author;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (Author == NULL) {
            Log(Log_ERROR, __func__, u8"The Author String is NULL");
        }
    }
    
    void CLISetCopyright(CommandLineIO *CLI, UTF8 *Copyright) {
        if (CLI != NULL && Copyright != NULL) {
            CLI->ProgramCopyright     = Copyright;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (Copyright == NULL) {
            Log(Log_ERROR, __func__, u8"The Copyright String is NULL");
        }
    }
    
    void CLISetLicense(CommandLineIO *CLI, CLILicenseTypes LicenseType, UTF8 *Name, UTF8 *LicenseURL) {
        if (CLI != NULL && LicenseType != UnknownLicenseType && Name != NULL && LicenseURL != NULL) {
            if (LicenseType == PermissiveLicense || LicenseType == CopyleftLicense) {
                CLI->IsProprietary             = No;
            } else if (LicenseType == ProprietaryLicense) {
                CLI->IsProprietary             = Yes;
            }
            CLI->ProgramLicenseName            = Name;
            CLI->ProgramLicenseURL             = LicenseURL;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (LicenseType == UnknownLicenseType) {
            Log(Log_ERROR, __func__, u8"The LicenseType Is Invalid");
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, u8"License Name Pointer is Invalid");
        } else if (LicenseURL == NULL) {
            Log(Log_ERROR, __func__, u8"The License URL must be set");
        }
    }
    
    void CLISetMinOptions(CommandLineIO *CLI, const int64_t MinOptions) {
        if (CLI != NULL && MinOptions >= 0 && MinOptions <= CLI->NumSwitches - 1) {
            CLI->MinOptions = MinOptions;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (MinOptions <= 0 || MinOptions > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"MinOptions %d is invalid, it should be between 0 and %d", MinOptions, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch) {
        if (CLI != NULL && (HelpSwitch >= 0 && HelpSwitch <= CLI->NumSwitches - 1)) {
            CLI->HelpSwitch = HelpSwitch;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (HelpSwitch >= 0 && HelpSwitch <= CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"HelpSwitch %d is invalid, it should be between 0 and %d", HelpSwitch, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchName(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Name) {
        if (CLI != NULL && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) && Name != NULL) {
            CLI->SwitchIDs[SwitchID].Name = Name;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, u8"Name String is NULL");
        } else if (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchDescription(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Description) {
        if (CLI != NULL && Description != NULL && SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) {
            CLI->SwitchIDs[SwitchID].Description     = Description;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, u8"Description String is NULL");
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchType(CommandLineIO *CLI, const int64_t SwitchID, CLISwitchTypes SwitchType) {
        if (CLI != NULL && SwitchType != UnknownSwitchType && SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) {
            CLI->SwitchIDs[SwitchID].SwitchType              = SwitchType;
            if (SwitchType == SwitchCantHaveSlaves || SwitchType == SwitchIsASlave || SwitchType == ExistentialSwitch) {
                CLI->SwitchIDs[SwitchID].MaxConcurrentSlaves = 0;
                CLI->SwitchIDs[SwitchID].NumPotentialSlaves  = 0;
                CLI->SwitchIDs[SwitchID].PotentialSlaves     = NULL;
            }
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (SwitchType == UnknownSwitchType) {
            Log(Log_ERROR, __func__, u8"You can not set SwitchID %d to UnknownSwitchType", SwitchID);
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchArgumentType(CommandLineIO *CLI, const int64_t SwitchID, CLIArgumentTypes ArgumentType) {
        if (CLI != NULL && ArgumentType != UnknownArgumentType && SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) {
            CLI->SwitchIDs[SwitchID].ArgumentType = ArgumentType;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (ArgumentType == UnknownArgumentType) {
            Log(Log_ERROR, __func__, u8"You can not set SwitchID %d to UnknownArgumentType", SwitchID);
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"SwitchID %d is invalid, it should be between 0 and %d", SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchAsSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t SlaveID) {
        if (CLI != NULL && MasterID >= 0 && MasterID <= CLI->NumSwitches - 1 && SlaveID >= 0 && SlaveID <= CLI->NumSwitches - 1) {
            CLI->SwitchIDs[MasterID].NumPotentialSlaves += 1;
            if (CLI->SwitchIDs[MasterID].PotentialSlaves != NULL) {
                CLI->SwitchIDs[MasterID].PotentialSlaves = realloc(CLI->SwitchIDs[MasterID].PotentialSlaves, CLI->SwitchIDs[MasterID].NumPotentialSlaves * sizeof(int64_t));
            } else {
                CLI->SwitchIDs[MasterID].PotentialSlaves = calloc(1, sizeof(int64_t));
            }
            CLI->SwitchIDs[MasterID].PotentialSlaves[CLI->SwitchIDs[MasterID].NumPotentialSlaves - 1] = SlaveID;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (MasterID < 0 && MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"MasterID %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches - 1);
        } else if (SlaveID < 0 && SlaveID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"SlaveID %d is invalid, it should be between 0 and %d", SlaveID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchMaxConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxConcurrentSlaves) {
        if (CLI != NULL && MasterID >= 0 && MasterID <= CLI->NumSwitches - 1 && MaxConcurrentSlaves >= 0 && MaxConcurrentSlaves <= CLI->NumSwitches - 1) {
            CLI->SwitchIDs[MasterID].MaxConcurrentSlaves = MaxConcurrentSlaves;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (MasterID < 0 && MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"MasterID %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches - 1);
        } else if (MaxConcurrentSlaves < 0 && MaxConcurrentSlaves > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"MaxConcurrentSlaves %d is invalid, it should be between 0 and %d", MasterID, CLI->NumSwitches - 1);
        }
    }
    
    static inline void DisplayCLIHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            fprintf(stdout, "%s's Options (-|--|/):%s", CLI->ProgramName, NewLine);
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                CLISwitchTypes CurrentSwitchType = CLI->SwitchIDs[Switch].SwitchType;
                fprintf(stdout, "%s: %s%s", CLI->SwitchIDs[Switch].Name, CLI->SwitchIDs[Switch].Description, NewLine);
                if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->SwitchIDs[Switch].NumPotentialSlaves > 0) {
                    for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; SlaveSwitch++) {
                        fprintf(stdout, "\t%s: %s%s", CLI->SwitchIDs[SlaveSwitch].Name, CLI->SwitchIDs[SlaveSwitch].Description, NewLine);
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        }
    }
    
    static inline void DisplayProgramBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            if (CLI->ProgramName != NULL && CLI->ProgramVersion != NULL && CLI->ProgramAuthor != NULL && CLI->ProgramCopyright != NULL) {
                fprintf(stdout, "%s, v. %s by %s © %s%s", CLI->ProgramName, CLI->ProgramVersion, CLI->ProgramAuthor, CLI->ProgramCopyright, NewLine);
            }
            if (CLI->ProgramDescription != NULL && CLI->ProgramLicenseName != NULL && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL && CLI->IsProprietary == No) {
                fprintf(stdout, "%s, Released under the \"%s\" %s, available at: %s%s", CLI->ProgramDescription, CLI->ProgramLicenseName, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, NewLine);
            } else if (CLI->ProgramDescription != NULL && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL && CLI->IsProprietary == Yes) {
                fprintf(stdout, "%s, By using this software, you agree to the End User License Agreement %s, available at: %s%s", CLI->ProgramDescription, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, NewLine);
            }
        } else {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        }
    }
    
    static UTF8 ArgumentString2SwitchFlag(UTF8 *ArgumentString) {
        UTF8 ArgumentSwitch = NULL;
        if (ArgumentString != NULL) {
            uint8_t  ArgumentStringPrefixSize = 0;
            uint32_t ArgumentStringSize       = UTF8_GetSizeInCodePoints(ArgumentString);
            
            if (ArgumentStringSize >= 2) {
                //Log(Log_DEBUG, __func__, "ArgumentString[0] = 0x%X, ArgumentString[1] = 0x%X", ArgumentString[0], ArgumentString[1]);
                if (ArgumentString[0] == '-' && ArgumentString[1] == '-') {
                    ArgumentStringPrefixSize  = 2;
                } else if (ArgumentString[0] == "/" || ArgumentString[0] == "\\" || ArgumentString[0] == "-") {
                    ArgumentStringPrefixSize    = 1;
                }
            } else {
                Log(Log_DEBUG, __func__, u8"OptionString is not an option string");
            }
            uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
            ArgumentSwitch                 = calloc(ArgumentSwitchSize + NULLTerminatorSize, sizeof(UTF8));
            memcpy(ArgumentSwitch, &ArgumentString[ArgumentStringPrefixSize], ArgumentSwitchSize);
        } else {
            Log(Log_ERROR, __func__, u8"OptionString Pointer is NULL");
        }
        return ArgumentSwitch;
    }
    
    static int64_t GetSubStringsAbsolutePosition(int64_t StartOffset, int64_t StringSize, UTF8 *OptionString, UTF8 *SubString) {
        int64_t SubStringPosition = -1LL;
        if (OptionString != NULL && SubString != NULL) {
            int64_t SubStringSize = UTF8_GetSizeInCodePoints(OptionString);
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
        } else if (OptionString == NULL) {
            Log(Log_ERROR, __func__, "OptionString is NULL");
        } else if (SubString == NULL) {
            Log(Log_ERROR, __func__, "SubString is NULL");
        }
        return SubStringPosition;
    }
    
    void ParseCommandLineOptions(CommandLineIO *CLI, const int argc, const char *argv[]) {
        if (CLI != NULL && CLI->MinOptions >= 1 && argc >= CLI->MinOptions) {
            /* Ok, so the first thing we need to do is loop over the arguments, and loop over the actual bytes of each argument */
            int32_t  CurrentArgument        = 1LL;
            
            do {
                // Extract the first argument as a switch.
                UTF8 Argument         = argv[CurrentArgument];
                UTF8 ArgumentFlag     = ArgumentString2SwitchFlag(Argument);
                uint64_t ArgumentFlagSize = UTF8_GetSizeInCodePoints(ArgumentFlag);
                // now loop over the switches
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                    // now compare ArgumentFlag to Switch
                    // Which string is smaller?
                    if (UTF8_Compare(&ArgumentFlag, CLI->SwitchIDs[Switch].Name, Yes, Yes) == Yes) { // ArgumentFlag matches this switch
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
                            UTF8  PotentialSlaveArg        = argv[CurrentArgument + ArgSlave];
                            for (int64_t Slave = 0LL; Slave < CLI->SwitchIDs[Switch].NumPotentialSlaves; Slave++) {
                                // Ok so now we loop over the values stores in PoentialSlaves, looking up their switch flags.
                                UTF8 *PotentialSlaveFlag   = CLI->SwitchIDs[CLI->SwitchIDs[Switch].PotentialSlaves[Slave]].Name;
                                // Now compare PotentialSlaveFlag to PotentialSlaveArg
                                if (UTF8_Compare(&PotentialSlaveArg, PotentialSlaveFlag, Yes, Yes) == Yes) {
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
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (CLI->MinOptions <= 1 || argc <= 1) {
            Log(Log_ERROR, __func__, u8"You entered %d options, the minimum is %d", argc - 1, CLI->MinOptions);
        }
    }
    
    static UTF8 SplitInlineArgument(const UTF8 *ArgumentString, const uint64_t ArgumentStringSize, uint64_t NumSplitArguments) {
        UTF8 *StringPointers = NULL;
        if (ArgumentString != NULL) {
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
                if (CurrentByte == ":" || CurrentByte == "=" || (CurrentByte == "-" && NextByte == "-") || (CurrentByte == "." && NextByte == ".") || CurrentByte == "-" || CurrentByte == ".") {
                    NumDelimitersFound += 1;
                    if ((CurrentByte == "-" && NextByte == "-") || (CurrentByte == "." && NextByte == ".")) {
                    }
                }
            }
            SubStringSize   = calloc(NumDelimitersFound, sizeof(uint64_t));
            SubStringOffset = calloc(NumDelimitersFound, sizeof(uint64_t));
            for (uint64_t ArgumentByte = 0ULL; ArgumentByte < ArgumentStringSize; ArgumentByte++) {
                uint8_t CurrentByte                       = ArgumentString[ArgumentByte];
                uint8_t NextByte                          = ArgumentString[ArgumentByte + 1];
                if (CurrentByte == ":" || CurrentByte == "=" || (CurrentByte == "-" && NextByte == "-") || (CurrentByte == "." && NextByte == ".") || CurrentByte == "-" || CurrentByte == ".") {
                    CurrentDelimiter                     += 1;
                    if ((CurrentByte == "-" && NextByte == "-") || (CurrentByte == "." && NextByte == ".")) {
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
            FoundStrings              =  NumDelimitersFound - 1;
            StringPointers            =  calloc(FoundStrings, sizeof(UTF8));
            for (uint64_t String = 0ULL; String < FoundStrings; String++) {
                // In here allocate a string for each found string
                // Allocate the string here
                UTF8 *StringPointer    =  calloc(SubStringSize[String], sizeof(UTF8));
                StringPointers[String] = *StringPointer;
            }
            NumSplitArguments = FoundStrings;
        } else {
            Log(Log_ERROR, __func__, "ArgumentString Pointer is NULL");
        }
        return StringPointers;
    }
    
    int64_t CLIGetNumMatchingOptions(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
        int64_t NumMatchingOptions = 0LL;
        if (CLI != NULL && OptionID >= 0 && OptionID <= CLI->NumOptions - 1 && NumSlaves >= 0 && NumSlaves <= CLI->NumSwitches - 1) {
            for (int64_t Option = 0LL; Option <= CLI->NumOptions - 1; Option++) {
                if (CLI->OptionIDs[Option].SwitchID == OptionID) {
                    if (NumSlaves == 0) {
                        NumMatchingOptions               += 1;
                    } else if (NumSlaves > 0 && CLI->OptionIDs[Option].NumOptionSlaves > 0) {
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
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, u8"SwitchID %d is invalid, it should be between 0 and %d", OptionID, CLI->NumSwitches - 1);
        } else if (NumSlaves < 0 || NumSlaves > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, u8"NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        }
        return NumMatchingOptions;
    }
    
    int64_t CLIGetOptionNum(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
        int64_t MatchingOption = -1LL;
        if (CLI != NULL && OptionID >= 0 && OptionID <= CLI->NumOptions - 1 && NumSlaves >= 0 && NumSlaves <= CLI->NumOptions - 1) {
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
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, u8"SwitchID %d is invalid, it should be between 0 and %d", OptionID, CLI->NumSwitches - 1);
        } else if (NumSlaves < 0 || NumSlaves > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, u8"NumSlaves %d is invalid, it should be between 0 and %d", NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        }
        return MatchingOption;
    }
    
    UTF8 CLIGetOptionResult(CommandLineIO const *CLI, const int64_t OptionID) {
        UTF8 Result = NULL;
        if (CLI != NULL && OptionID >= 0 && OptionID <= CLI->NumOptions - 1) {
            Result = CLI->OptionIDs[OptionID].Argument;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, u8"Option %d is outside the range 0 - %d", OptionID, CLI->NumOptions - 1);
        }
        return Result;
    }
    
    UTF8 *GetExtensionFromPath(UTF8 *Path) {
        UTF8 *ExtensionString                  = NULL;
        if (Path != NULL) {
            uint64_t PathSize                  = UTF8_GetSizeInCodePoints(Path) + 1;
            uint64_t ExtensionSize             = PathSize;
            uint64_t ExtensionDistanceFromEnd  = 0ULL;
            while (Path[ExtensionDistanceFromEnd] != 0x2E) {
                ExtensionSize                 -= 1;
                ExtensionDistanceFromEnd      += 1;
            }
            ExtensionSize                      = PathSize - ExtensionDistanceFromEnd;
            ExtensionString                    = calloc(ExtensionSize + NULLTerminatorSize, sizeof(UTF8));
            if (ExtensionString != NULL) {
                for (uint64_t ExtensionByte = 0LLU; ExtensionByte < ExtensionSize; ExtensionByte++) {
                    ExtensionString[ExtensionByte] = Path[ExtensionByte + ExtensionDistanceFromEnd];
                }
            } else {
                Log(Log_ERROR, __func__, u8"Couldn't allocate %lld bytes for the Extension String", ExtensionSize);
            }
        } else {
            Log(Log_ERROR, __func__, u8"Path Pointer is NULL");
        }
        return ExtensionString;
    }
    
    void CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF8 *Strings[], uint64_t *Numerator, uint64_t *Denominator) {
        if (CLI != NULL) {
            /*
             How do we get the window size? I want to be able to resize the window
             I want the bar to have an even number of dashes on each side with the number in the middle.
             like this:
             [--                        Shot U/V 2%                              ]
             [-----                    Frame W/X 10%                             ] 10.5% gets rounded down to 10 which is 5 dashes
             [-------------------------Block Y/Z 51%-                            ] 50.5% gets rounded up to 51 which is 26 dashes
             FormatString: "[%s" "%s %d/%d %d%" "%s]", FirstDashes. String[Index], Numerator, Denominator, DerivedPercentage, LastDashes
             
             So we need to know the length of String[Index] in Graphemes, after being composed.
             */
            /*
             Ok, so we know the width of the console, now we need to figure out the sizes of each of the strings
             */
            
            uint64_t *StringSize = calloc(NumItems2Display, sizeof(uint64_t));
            for (uint8_t Item = 0; Item < NumItems2Display; Item++) { // Get the size of the strings
                StringSize[Item] = UTF8_GetSizeInCodePoints(Strings[Item]);
                // huh, well we need 2 characters for the brackets.
            }
            // Number of seperators for each string
            uint64_t *NumProgressIndicatorsPerString = calloc(NumItems2Display, sizeof(uint64_t));
            UTF8     *ActualStrings2Print            = calloc(NumItems2Display * CLI->ConsoleWidth, sizeof(UTF8));
            for (uint8_t String = 0; String < NumItems2Display; String++) { // Actually create the strings
                // Subtract 2 for the brackets, + the size of each string from the actual width of the console window
                NumProgressIndicatorsPerString[String] = CLI->ConsoleWidth - (2 + StringSize[String]); // what if it's not even?
                uint8_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
                uint8_t HalfOfTheIndicators = (PercentComplete / 2);
                // Now we go ahead and memset a string with the proper number of indicators
                UTF8 *Indicator             = calloc(CLI->ConsoleWidth, sizeof(UTF8));
                memset(Indicator, '-', HalfOfTheIndicators);
                sprintf(ActualStrings2Print[String], "[%s%s %d/%d %hhu/% %s]", Indicator, Strings[String], Numerator, Denominator, PercentComplete, Indicator);
                fprintf(stdout, "%s%s", &ActualStrings2Print[String], NewLine);
                free(Indicator);
            }
            free(StringSize);
            free(NumProgressIndicatorsPerString);
            free(ActualStrings2Print);
        } else {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        }
    }
    
    void CommandLineIO_Deinit(CommandLineIO *CLI) {
        if (CLI != NULL) {
            if (CLI->SwitchIDs != NULL) {
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                    free(CLI->SwitchIDs[Switch].Name);
                    free(CLI->SwitchIDs[Switch].Description);
                    free(CLI->SwitchIDs[Switch].PotentialSlaves);
                }
                free(CLI->SwitchIDs);
            }
            if (CLI->OptionIDs != NULL) {
                for (int64_t Option = 0LL; Option <= CLI->NumOptions - 1; Option++) {
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
        } else {
            Log(Log_ERROR, __func__, u8"CommandLineIO Pointer is NULL");
        }
    }
    
    void DEBUGCommandLineOptions(CommandLineIO *CLI) {
        Log(Log_DEBUG, __func__, u8"NumOptions %d", CLI->NumOptions);
        for (int64_t Option = 0LL; Option <= CLI->NumOptions - 1; Option++) {
            if (CLI->OptionIDs[Option].SwitchID >= 0) {
                Log(Log_DEBUG, __func__, u8"OptionID %d, OptionSwitchID %d, OptionFlag %s, OptionType %d, OptionArgument %s", Option, CLI->OptionIDs[Option].SwitchID, CLI->SwitchIDs[CLI->OptionIDs[Option].SwitchID].Name, CLI->SwitchIDs[CLI->OptionIDs[Option].SwitchID].SwitchType, CLI->OptionIDs[Option].Argument);
                if (CLI->OptionIDs[Option].NumOptionSlaves >= 1) {
                    for (int64_t OptionSlave = 0LL; OptionSlave < CLI->OptionIDs[Option].NumOptionSlaves; OptionSlave++) {
                        Log(Log_DEBUG, __func__, u8"SlaveID %d", OptionSlave);
                    }
                }
            }
        }
    }
    
#ifdef   __cplusplus
}
#endif
