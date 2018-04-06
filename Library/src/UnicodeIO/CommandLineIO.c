#include <stdarg.h>                    /* Included for the variadic argument support macros */
#include <stdint.h>                    /* Included for u/intX_t */
#include <stdlib.h>                    /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, realloc, and free */
#include <string.h>                    /* Included for memset */

#include "../include/CommandLineIO.h"  /* Included for the CommandLineIO declarations */
#include "../include/Macros.h"         /* Included for NewLineWithNULLSize, FoundationIOTargetOS */
#include "../include/Log.h"            /* Included for Log */


#if   (FoundationIOTargetOS == POSIXOS)
#include <sys/ioctl.h>                 /* Included for the terminal size */
#include <sys/ttycom.h>                /* Included for winsize, TIOCGWINSZ */
#elif (FoundationIOTargetOS == WindowsOS)
#include <Windows.h>                   /* Included because WinCon needs it */
#include <wincon.h>                    /* Included for getting the terminal size */
#endif

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
                Log(Log_ERROR, __func__, U8("Couldn't allocate %d CommandLineSwitches"), NumSwitches);
            }
#if   (FoundationIOTargetOS == POSIXOS)
            struct winsize WindowSize;
            ioctl(0, TIOCGWINSZ, &WindowSize);
            CLI->ConsoleWidth    = WindowSize.ws_row;
            CLI->ConsoleHeight   = WindowSize.ws_col;
            Log(Log_DEBUG, __func__, U8("WindowWidth: %d, WindowHeight %d"), CLI->ConsoleWidth, CLI->ConsoleHeight);
#elif (FoundationIOTargetOS == WindowsOS)
            CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
            CLI->ConsoleHeight   = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;
            CLI->ConsoleWidth    = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("Couldn't allocate CommandLineIO"));
        } else if (NumSwitches == 0) {
            Log(Log_ERROR, __func__, U8("NumSwitches %d must be greater than or equal to 1"), NumSwitches);
        }
        return CLI;
    }
    
    void CLISetName(CommandLineIO *CLI, UTF8 *Name) {
        if (CLI != NULL && Name != NULL) {
            CLI->ProgramName     = UTF8_NormalizeString(Name, NormalizationFormKC);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, U8("The Name string is NULL"));
        }
    }
    
    void CLISetVersion(CommandLineIO *CLI, UTF8 *Version) {
        if (CLI != NULL && Version != NULL) {
            CLI->ProgramVersion     = UTF8_NormalizeString(Version, NormalizationFormKC);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Version == NULL) {
            Log(Log_ERROR, __func__, U8("The Version String is NULL"));
        }
    }
    
    void CLISetDescription(CommandLineIO *CLI, UTF8 *Description) {
        if (CLI != NULL && Description != NULL) {
            CLI->ProgramDescription     = UTF8_NormalizeString(Description, NormalizationFormKC);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, U8("The Description String is NULL"));
        }
    }
    
    void CLISetAuthor(CommandLineIO *CLI, UTF8 *Author) {
        if (CLI != NULL && Author != NULL) {
            CLI->ProgramAuthor     = UTF8_NormalizeString(Author, NormalizationFormKC);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Author == NULL) {
            Log(Log_ERROR, __func__, U8("The Author String is NULL"));
        }
    }
    
    void CLISetCopyright(CommandLineIO *CLI, UTF8 *Copyright) {
        if (CLI != NULL && Copyright != NULL) {
            CLI->ProgramCopyright     = UTF8_NormalizeString(Copyright, NormalizationFormKC);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Copyright == NULL) {
            Log(Log_ERROR, __func__, U8("The Copyright String is NULL"));
        }
    }
    
    void CLISetLicense(CommandLineIO *CLI, CLILicenseTypes LicenseType, UTF8 *Name, UTF8 *LicenseURL) {
        if (CLI != NULL && LicenseType != UnknownLicenseType && Name != NULL && LicenseURL != NULL) {
            if (LicenseType == PermissiveLicense || LicenseType == CopyleftLicense) {
                CLI->IsProprietary             = No;
            } else if (LicenseType == ProprietaryLicense) {
                CLI->IsProprietary             = Yes;
            }
            CLI->ProgramLicenseName            = UTF8_NormalizeString(Name, NormalizationFormKC);
            UTF8 *CaseFoldedLicenseURL         = UTF8_CaseFoldString(LicenseURL);
            UTF8 *NormalizedLicenseURL         = UTF8_NormalizeString(CaseFoldedLicenseURL, NormalizationFormKC);
            free(CaseFoldedLicenseURL);
            CLI->ProgramLicenseURL             = NormalizedLicenseURL;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (LicenseType == UnknownLicenseType) {
            Log(Log_ERROR, __func__, U8("The LicenseType Is Invalid"));
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, U8("License Name Pointer is Invalid"));
        } else if (LicenseURL == NULL) {
            Log(Log_ERROR, __func__, U8("The License URL must be set"));
        }
    }
    
    void CLISetMinOptions(CommandLineIO *CLI, const int64_t MinOptions) {
        if (CLI != NULL && MinOptions >= 0 && MinOptions <= CLI->NumSwitches - 1) {
            CLI->MinOptions = MinOptions;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MinOptions <= 0 || MinOptions > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MinOptions %d is invalid, it should be between 0 and %d"), MinOptions, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch) {
        if (CLI != NULL && (HelpSwitch >= 0 && HelpSwitch <= CLI->NumSwitches - 1)) {
            CLI->HelpSwitch = HelpSwitch;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (HelpSwitch >= 0 && HelpSwitch <= CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("HelpSwitch %d is invalid, it should be between 0 and %d"), HelpSwitch, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchName(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Name) {
        if (CLI != NULL && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) && Name != NULL) {
            UTF8 *Normalized              = UTF8_NormalizeString(Name, NormalizationFormKC);
            CLI->SwitchIDs[SwitchID].Name = UTF8_CaseFoldString(Normalized);
            free(Normalized);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, U8("Name String is NULL"));
        } else if (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %d is invalid, it should be between 0 and %d"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchDescription(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Description) {
        if (CLI != NULL && Description != NULL && SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) {
            UTF8 *Normalized                     = UTF8_NormalizeString(Description, NormalizationFormKC);
            CLI->SwitchIDs[SwitchID].Description = UTF8_CaseFoldString(Normalized);
            free(Normalized);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, U8("Description String is NULL"));
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %d is invalid, it should be between 0 and %d"), SwitchID, CLI->NumSwitches - 1);
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
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (SwitchType == UnknownSwitchType) {
            Log(Log_ERROR, __func__, U8("You can not set SwitchID %d to UnknownSwitchType"), SwitchID);
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %d is invalid, it should be between 0 and %d"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchArgumentType(CommandLineIO *CLI, const int64_t SwitchID, CLIArgumentTypes ArgumentType) {
        if (CLI != NULL && ArgumentType != UnknownArgumentType && SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) {
            CLI->SwitchIDs[SwitchID].ArgumentType = ArgumentType;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (ArgumentType == UnknownArgumentType) {
            Log(Log_ERROR, __func__, U8("You can not set SwitchID %d to UnknownArgumentType"), SwitchID);
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %d is invalid, it should be between 0 and %d"), SwitchID, CLI->NumSwitches - 1);
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
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MasterID < 0 && MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MasterID %d is invalid, it should be between 0 and %d"), MasterID, CLI->NumSwitches - 1);
        } else if (SlaveID < 0 && SlaveID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SlaveID %d is invalid, it should be between 0 and %d"), SlaveID, CLI->NumSwitches - 1);
        }
    }
    
    void CLISetSwitchMaxConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxConcurrentSlaves) {
        if (CLI != NULL && MasterID >= 0 && MasterID <= CLI->NumSwitches - 1 && MaxConcurrentSlaves >= 0 && MaxConcurrentSlaves <= CLI->NumSwitches - 1) {
            CLI->SwitchIDs[MasterID].MaxConcurrentSlaves = MaxConcurrentSlaves;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MasterID < 0 && MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MasterID %d is invalid, it should be between 0 and %d"), MasterID, CLI->NumSwitches - 1);
        } else if (MaxConcurrentSlaves < 0 && MaxConcurrentSlaves > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MaxConcurrentSlaves %d is invalid, it should be between 0 and %d"), MasterID, CLI->NumSwitches - 1);
        }
    }
    
    static inline void DisplayCLIHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8  *ProgramName = FormatStringUTF8(U8("%s's Options (-|--|/):%s"), CLI->ProgramName, NewLineUTF8);
            UTF8_WriteString2File(ProgramName, stdout);
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                CLISwitchTypes CurrentSwitchType = CLI->SwitchIDs[Switch].SwitchType;
                UTF8 *SwitchInfo = FormatStringUTF8(U8("%s: %s%s"), CLI->SwitchIDs[Switch].Name, CLI->SwitchIDs[Switch].Description, NewLineUTF8);
                UTF8_WriteString2File(SwitchInfo, stdout);
                if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->SwitchIDs[Switch].NumPotentialSlaves > 0) {
                    for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; SlaveSwitch++) {
                        UTF8 *SlaveSwitchInfo = FormatStringUTF8(U8("\t%s: %s%s"), CLI->SwitchIDs[SlaveSwitch].Name, CLI->SwitchIDs[SlaveSwitch].Description, NewLineUTF8);
                        UTF8_WriteString2File(SlaveSwitchInfo, stdout);
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static inline void DisplayProgramBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            if (CLI->ProgramName != NULL && CLI->ProgramVersion != NULL && CLI->ProgramAuthor != NULL && CLI->ProgramCopyright != NULL) {
                UTF8 *FormattedString = FormatStringUTF8(U8("%s, v. %s by %s © %s%s"), CLI->ProgramName, CLI->ProgramVersion, CLI->ProgramAuthor, CLI->ProgramCopyright, NewLineUTF8);
                UTF8_WriteString2File(FormattedString, stdout);
            }
            if (CLI->ProgramDescription != NULL && CLI->ProgramLicenseName != NULL && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL && CLI->IsProprietary == No) {
                UTF8 *FormattedString = FormatStringUTF8(U8("%s, Released under the \"%s\" %s, available at: %s%s"), CLI->ProgramDescription, CLI->ProgramLicenseName, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, NewLineUTF8);
                UTF8_WriteString2File(FormattedString, stdout);
            } else if (CLI->ProgramDescription != NULL && CLI->ProgramLicenseDescription != NULL && CLI->ProgramLicenseURL != NULL && CLI->IsProprietary == Yes) {
                UTF8 *FormattedString = FormatStringUTF8(U8("%s, By using this software, you agree to the End User License Agreement %s, available at: %s%s"), CLI->ProgramDescription, CLI->ProgramLicenseDescription, CLI->ProgramLicenseURL, NewLineUTF8);
                UTF8_WriteString2File(FormattedString, stdout);
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static UTF8 ArgumentString2SwitchFlag(UTF32 *ArgumentString) {
        UTF8 ArgumentSwitch = NULL;
        if (ArgumentString != NULL) {
            uint8_t  ArgumentStringPrefixSize = 0;
            uint32_t ArgumentStringSize       = UTF8_GetSizeInCodePoints(ArgumentString);
            
            if (ArgumentStringSize >= 2) {
                //Log(Log_DEBUG, __func__, U8("ArgumentString[0] = 0x%X, ArgumentString[1] = 0x%X"), ArgumentString[0], ArgumentString[1]);
                if (ArgumentString[0] == '-' && ArgumentString[1] == '-') {
                    ArgumentStringPrefixSize  = 2;
                } else if (ArgumentString[0] == '/' || ArgumentString[0] == '\\' || ArgumentString[0] == '-') {
                    ArgumentStringPrefixSize    = 1;
                }
            } else {
                Log(Log_DEBUG, __func__, U8("OptionString is not an option string"));
            }
            uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
            ArgumentSwitch                 = calloc(ArgumentSwitchSize + NULLTerminatorSize, sizeof(UTF8));
            memcpy(ArgumentSwitch, &ArgumentString[ArgumentStringPrefixSize], ArgumentSwitchSize);
        } else {
            Log(Log_ERROR, __func__, U8("OptionString Pointer is NULL"));
        }
        return ArgumentSwitch;
    }
    
    static void UTF32_ParseCommandLineOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF32 **Arguments) {
        if (CLI != NULL && CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions) {
            /* Ok, so the first thing we need to do is loop over the arguments, and loop over the actual bytes of each argument */
            int32_t  CurrentArgument      = 1LL;
            do {
                // Extract the first argument as a switch.
                UTF32   *Argument         = Arguments[CurrentArgument];
                UTF32   *ArgumentFlag     = ArgumentString2SwitchFlag(Argument);
                uint64_t ArgumentFlagSize = UTF32_GetSizeInCodePoints(ArgumentFlag);
                // now loop over the switches
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                    // now compare ArgumentFlag to Switch
                    // Which string is smaller?
                    if (UTF8_Compare(ArgumentFlag, CLI->SwitchIDs[Switch].Name, NormalizationFormKC, Yes) == Yes) { // ArgumentFlag matches this switch
                        // Set up the Option here
                        CLI->NumOptions   += 1;
                        if (CLI->NumOptions == 1) {
                            CLI->OptionIDs = calloc(1, sizeof(CommandLineOption));
                        } else {
                            CLI->OptionIDs = realloc(CLI->OptionIDs, CLI->NumOptions * sizeof(CommandLineOption));
                        }
                        CLI->OptionIDs[CLI->NumOptions - 1].SwitchID = Switch;
                        
                        
                        // so now we need to look over the current switch's MaxConcurrentSlaves and NumPotentialSlaves wait num potential slaves will be indexxed by the loop
                        for (int64_t ArgSlave = 1LL; ArgSlave < CLI->SwitchIDs[Switch].MaxConcurrentSlaves; ArgSlave++) {
                            UTF32 *PotentialSlaveArg        = Arguments[CurrentArgument + ArgSlave];
                            for (int64_t Slave = 0LL; Slave < CLI->SwitchIDs[Switch].NumPotentialSlaves; Slave++) {
                                // Ok so now we loop over the values stores in PoentialSlaves, looking up their switch flags.
                                UTF32 *PotentialSlaveFlag   = CLI->SwitchIDs[CLI->SwitchIDs[Switch].PotentialSlaves[Slave]].Name;
                                // Now compare PotentialSlaveFlag to PotentialSlaveArg
                                if (UTF8_Compare(PotentialSlaveArg, PotentialSlaveFlag, NormalizationFormKC, Yes) == Yes) {
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
            } while (CurrentArgument < NumArguments);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions <= 1 || NumArguments <= 1) {
            Log(Log_ERROR, __func__, U8("You entered %d options, the minimum is %d"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    UTF8 *UTF8_CaseFoldString(UTF8 *String2CaseFold) {
        if (String2CaseFold != NULL) {
            // IDEK what I need to do, look up codepoints in a table and replace them with the found variants?
        } else {
            Log(Log_ERROR, __func__, U8("String2CaseFold Pointer is NULL"));
        }
        return NULL;
    }
    
    UTF16 *UTF16_CaseFoldString(UTF16 *String2CaseFold) {
        if (String2CaseFold != NULL) {
            // IDEK what I need to do, look up codepoints in a table and replace them with the found variants?
        } else {
            Log(Log_ERROR, __func__, U8("String2CaseFold Pointer is NULL"));
        }
        return NULL;
    }
    
    void UTF8_ParseCommandLineOptions(CommandLineIO *CLI, const int64_t NumArguments, const UTF8 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            // Basically now we just loop over the arguments, decode, normalize, and casefold them.
            UTF32 **Arguments32  = calloc(NumArguments, sizeof(UTF32*));
            for (int64_t Arg = 0LL; Arg < NumArguments; Arg++) {
                UTF8 *CaseFolded = UTF8_CaseFoldString(*Arguments[Arg]);
                UTF8 *Normalized = UTF8_NormalizeString(CaseFolded, NormalizationFormKC);
                free(CaseFolded);
                Arguments32[Arg] = UTF8_Decode(Normalized);
            }
            UTF32_ParseCommandLineOptions(CLI, NumArguments, Arguments32);
            for (int64_t Arg = 0LL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions <= 1 || NumArguments <= 1) {
            Log(Log_ERROR, __func__, U8("You entered %d options, the minimum is %d"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    void UTF16_ParseCommandLineOptions(CommandLineIO *CLI, const int64_t NumArguments, const UTF16 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32   = calloc(NumArguments, sizeof(UTF32*));
            for (int64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                UTF16 *CaseFolded = UTF16_CaseFoldString(*Arguments[Arg]);
                UTF16 *Normalized = UTF16_NormalizeString(CaseFolded, NormalizationFormKC);
                free(CaseFolded);
                Arguments32[Arg]  = UTF16_Decode(Normalized);
            }
            UTF32_ParseCommandLineOptions(CLI, NumArguments, Arguments32);
            for (int64_t Arg = 0LL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions <= 1 || NumArguments <= 1) {
            Log(Log_ERROR, __func__, U8("You entered %d options, the minimum is %d"), NumArguments - 1, CLI->MinOptions);
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
                if (CurrentByte == ':' || CurrentByte == '=' || (CurrentByte == '-' && NextByte == '-') || (CurrentByte == '.' && NextByte == '.') || CurrentByte == '-' || CurrentByte == '.') {
                    NumDelimitersFound += 1;
                    if ((CurrentByte == '-' && NextByte == '-') || (CurrentByte == '.' && NextByte == '.')) {
                    }
                }
            }
            SubStringSize   = calloc(NumDelimitersFound, sizeof(uint64_t));
            SubStringOffset = calloc(NumDelimitersFound, sizeof(uint64_t));
            for (uint64_t ArgumentByte = 0ULL; ArgumentByte < ArgumentStringSize; ArgumentByte++) {
                uint8_t CurrentByte                       = ArgumentString[ArgumentByte];
                uint8_t NextByte                          = ArgumentString[ArgumentByte + 1];
                if (CurrentByte == ':' || CurrentByte == '=' || (CurrentByte == '-' && NextByte == '-') || (CurrentByte == '.' && NextByte == '.') || CurrentByte == '-' || CurrentByte == '.') {
                    CurrentDelimiter                     += 1;
                    if ((CurrentByte == '-' && NextByte == '-') || (CurrentByte == '.' && NextByte == '.')) {
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
            Log(Log_ERROR, __func__, U8("ArgumentString Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %d is invalid, it should be between 0 and %d"), OptionID, CLI->NumSwitches - 1);
        } else if (NumSlaves < 0 || NumSlaves > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("NumSlaves %d is invalid, it should be between 0 and %d"), NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
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
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %d is invalid, it should be between 0 and %d"), OptionID, CLI->NumSwitches - 1);
        } else if (NumSlaves < 0 || NumSlaves > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("NumSlaves %d is invalid, it should be between 0 and %d"), NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        }
        return MatchingOption;
    }
    
    UTF8 CLIGetOptionResult(CommandLineIO const *CLI, const int64_t OptionID) {
        UTF8 Result = NULL;
        if (CLI != NULL && OptionID >= 0 && OptionID <= CLI->NumOptions - 1) {
            Result = CLI->OptionIDs[OptionID].Argument;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("Option %d is outside the range 0 - %d"), OptionID, CLI->NumOptions - 1);
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
                Log(Log_ERROR, __func__, U8("Couldn't allocate %lld bytes for the Extension String"), ExtensionSize);
            }
        } else {
            Log(Log_ERROR, __func__, U8("Path Pointer is NULL"));
        }
        return ExtensionString;
    }
    
    void CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF8 **Strings, uint64_t *Numerator, uint64_t *Denominator) {
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
                fprintf(stdout, "%s%s", &ActualStrings2Print[String], NewLineUTF8);
                free(Indicator);
            }
            free(StringSize);
            free(NumProgressIndicatorsPerString);
            free(ActualStrings2Print);
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    void CommandLineIO_Deinit(CommandLineIO *CLI) {
        if (CLI != NULL) {
            if (CLI->NumSwitches > 0) {
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                    free(CLI->SwitchIDs[Switch].Name);
                    free(CLI->SwitchIDs[Switch].Description);
                    free(CLI->SwitchIDs[Switch].PotentialSlaves);
                }
            }
            if (CLI->NumOptions > 0) {
                for (int64_t Option = 0LL; Option <= CLI->NumOptions - 1; Option++) {
                    free(CLI->OptionIDs[Option].OptionSlaves);
                    free(CLI->OptionIDs[Option].Argument);
                }
            }
            free(CLI->SwitchIDs);
            free(CLI->OptionIDs);
            free(CLI->ProgramName);
            free(CLI->ProgramAuthor);
            free(CLI->ProgramDescription);
            free(CLI->ProgramVersion);
            free(CLI->ProgramCopyright);
            free(CLI->ProgramLicenseName);
            free(CLI->ProgramLicenseDescription);
            free(CLI->ProgramLicenseURL);
            free(CLI);
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    void DEBUGCommandLineOptions(CommandLineIO *CLI) {
        Log(Log_DEBUG, __func__, U8("NumOptions %d"), CLI->NumOptions);
        for (int64_t Option = 0LL; Option <= CLI->NumOptions - 1; Option++) {
            if (CLI->OptionIDs[Option].SwitchID >= 0) {
                Log(Log_DEBUG, __func__, U8("OptionID %d, OptionSwitchID %d, OptionFlag %s, OptionType %d, OptionArgument %s"), Option, CLI->OptionIDs[Option].SwitchID, CLI->SwitchIDs[CLI->OptionIDs[Option].SwitchID].Name, CLI->SwitchIDs[CLI->OptionIDs[Option].SwitchID].SwitchType, CLI->OptionIDs[Option].Argument);
                if (CLI->OptionIDs[Option].NumOptionSlaves >= 1) {
                    for (int64_t OptionSlave = 0LL; OptionSlave < CLI->OptionIDs[Option].NumOptionSlaves; OptionSlave++) {
                        Log(Log_DEBUG, __func__, U8("SlaveID %d"), OptionSlave);
                    }
                }
            }
        }
    }
    
#ifdef   __cplusplus
}
#endif
