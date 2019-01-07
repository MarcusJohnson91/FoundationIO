#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/CommandLineIO.h"  /* Included for the CommandLineIO declarations */
#include "../include/Log.h"            /* Included for Log */
#include "../include/StringIO.h"       /* Included for StringIO's declarations */

#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
#include <signal.h>                    /* Included for SIGWINCH handling */
#include <sys/ioctl.h>                 /* Included for the terminal size */
#include <sys/ttycom.h>                /* Included for winsize, TIOCGWINSZ */
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
#include <wincon.h>                    /* Included for getting the terminal size, MAKE SURE Macros.h is included before this, it includes Windows.h */
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @struct                 CommandLineSwitch
     @abstract                                           "Contains the data to support a switch".
     @constant               Name                        "What is this switch called, how do we identify it"?
     @constant               Description                 "Describe to the user what this switch does".
     @constant               PotentialSlaves             "Pointer to an array that contains the list of aloowable Slave switches".
     @constant               NumPotentialSlaves          "How many potential slaves are there"?
     @constant               MinConcurrentSlaves         "How many Slave switches must be active at once"?
     @constant               MaxConcurrentSlaves         "How many Slave switches can be active at once"?
     @constant               SwitchType                  "What type of switch is this"?
     @constant               ArgumentType                "What type of argument does this switch accept"?
     */
    typedef struct CommandLineSwitch {
        UTF32               *Name;
        UTF32               *Description;
        int64_t             *PotentialSlaves;
        int64_t              NumPotentialSlaves;
        int64_t              MinConcurrentSlaves;
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
        UTF32               *Argument;
        int64_t             *OptionSlaves;
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
        CommandLineSwitch   *SwitchIDs;
        CommandLineOption   *OptionIDs;
        UTF32               *ProgramName;
        UTF32               *ProgramAuthor;
        UTF32               *ProgramDescription;
        UTF32               *ProgramVersion;
        UTF32               *ProgramCopyright;
        UTF32               *ProgramLicenseName;
        UTF32               *ProgramLicenseDescription;
        UTF32               *ProgramLicenseURL;
        int64_t              NumSwitches;
        int64_t              NumOptions;
        int64_t              MinOptions;
        int64_t              HelpSwitch;
        CLILicenseTypes      LicenseType;
    } CommandLineIO;
    
    CommandLineIO *CommandLineIO_Init(const int64_t NumSwitches) {
        CommandLineIO *CLI           = NULL;
        if (NumSwitches >= 0) {
            CLI                      = calloc(1, sizeof(CommandLineIO));
            if (CLI != NULL) {
                CLI->SwitchIDs       = calloc(NumSwitches, sizeof(CommandLineSwitch));
                if (CLI->SwitchIDs != NULL) {
                    CLI->NumSwitches = NumSwitches;
                } else {
                    CommandLineIO_Deinit(CLI);
                    Log(Log_ERROR, __func__, U8("Couldn't allocate %lld CommandLineSwitches"), NumSwitches);
                }
            } else {
                CommandLineIO_Deinit(CLI);
                Log(Log_ERROR, __func__, U8("Couldn't allocate CommandLineIO"));
            }
        } else if (NumSwitches <= 0) {
            Log(Log_ERROR, __func__, U8("NumSwitches %lld must be greater than or equal to 1"), NumSwitches);
        }
        return CLI;
    }
    
    uint64_t       CommandLineIO_GetTerminalWidth(void) {
        uint64_t Width = 0ULL;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Width          = WindowSize.ws_row;
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Width          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Width;
    }
    
    uint64_t       CommandLineIO_GetTerminalHeight(void) {
        uint64_t Height = 0ULL;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Height          = WindowSize.ws_row;
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Height          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Height;
    }
    
    bool           CommandLineIO_TerminalWasResized(void) {
        bool SizeChanged = No;
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
        /*
         We're creating a text UI to show the progress of the program.
         The user resizes the window
         that means we now need to re-get the size of the window so we can start redrawing it.
         we get the signal in ShowProgress, if we get that signal, we need to call the Width and Height functions.
         SIGWINCH
         */
        
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        SizeChanged      = WindowSize.ws_row;
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        SizeChanged      = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return SizeChanged;
    }
    
    void CommandLineIO_UTF8_SetName(CommandLineIO *CLI, UTF8 *Name) {
        if (CLI != NULL && Name != NULL) {
            UTF32 *Decoded       = UTF8_Decode(Name);
            UTF32 *Normalized    = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded    = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramName     = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, U8("The Name string is NULL"));
        }
    }
    
    void CommandLineIO_UTF16_SetName(CommandLineIO *CLI, UTF16 *Name) {
        if (CLI != NULL && Name != NULL) {
            UTF32 *Decoded       = UTF16_Decode(Name);
            UTF32 *Normalized    = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded    = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramName     = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, U8("The Name string is NULL"));
        }
    }
    
    void CommandLineIO_UTF8_SetVersion(CommandLineIO *CLI, UTF8 *Version) {
        if (CLI != NULL && Version != NULL) {
            UTF32 *Decoded       = UTF8_Decode(Version);
            UTF32 *Normalized    = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded    = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramVersion  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Version == NULL) {
            Log(Log_ERROR, __func__, U8("The Version String is NULL"));
        }
    }
    
    void CommandLineIO_UTF16_SetVersion(CommandLineIO *CLI, UTF16 *Version) {
        if (CLI != NULL && Version != NULL) {
            UTF32 *Decoded       = UTF16_Decode(Version);
            UTF32 *Normalized    = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded    = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramVersion  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Version == NULL) {
            Log(Log_ERROR, __func__, U8("The Version String is NULL"));
        }
    }
    
    void CommandLineIO_UTF8_SetDescription(CommandLineIO *CLI, UTF8 *Description) {
        if (CLI != NULL && Description != NULL) {
            UTF32 *Decoded           = UTF8_Decode(Description);
            UTF32 *Normalized        = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded        = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramDescription  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, U8("The Description String is NULL"));
        }
    }
    
    void CommandLineIO_UTF16_SetDescription(CommandLineIO *CLI, UTF16 *Description) {
        if (CLI != NULL && Description != NULL) {
            UTF32 *Decoded           = UTF16_Decode(Description);
            UTF32 *Normalized        = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded        = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramDescription  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, U8("The Description String is NULL"));
        }
    }
    
    void CommandLineIO_UTF8_SetAuthor(CommandLineIO *CLI, UTF8 *Author) {
        if (CLI != NULL && Author != NULL) {
            UTF32 *Decoded      = UTF8_Decode(Author);
            UTF32 *Normalized   = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded   = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramAuthor  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Author == NULL) {
            Log(Log_ERROR, __func__, U8("The Author String is NULL"));
        }
    }
    
    void CommandLineIO_UTF16_SetAuthor(CommandLineIO *CLI, UTF16 *Author) {
        if (CLI != NULL && Author != NULL) {
            UTF32 *Decoded      = UTF16_Decode(Author);
            UTF32 *Normalized   = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded   = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramAuthor  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Author == NULL) {
            Log(Log_ERROR, __func__, U8("The Author String is NULL"));
        }
    }
    
    void CommandLineIO_UTF8_SetCopyright(CommandLineIO *CLI, UTF8 *Copyright) {
        if (CLI != NULL && Copyright != NULL) {
            UTF32 *Decoded        = UTF8_Decode(Copyright);
            UTF32 *Normalized     = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded     = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramCopyright = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Copyright == NULL) {
            Log(Log_ERROR, __func__, U8("The Copyright String is NULL"));
        }
    }
    
    void CommandLineIO_UTF16_SetCopyright(CommandLineIO *CLI, UTF16 *Copyright) {
        if (CLI != NULL && Copyright != NULL) {
            UTF32 *Decoded        = UTF16_Decode(Copyright);
            UTF32 *Normalized     = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded     = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->ProgramCopyright = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Copyright == NULL) {
            Log(Log_ERROR, __func__, U8("The Copyright String is NULL"));
        }
    }
    
    void CommandLineIO_UTF8_SetLicense(CommandLineIO *CLI, CLILicenseTypes LicenseType, UTF8 *Name, UTF8 *LicenseURL) {
        if (CLI != NULL && LicenseType != UnknownLicenseType && Name != NULL && LicenseURL != NULL) {
            CLI->LicenseType                = LicenseType;
            UTF32 *DecodedName              = UTF8_Decode(Name);
            UTF32 *NormalizedName           = UTF32_NormalizeString(DecodedName, NormalizationFormKC);
            free(DecodedName);
            UTF32 *CaseFoldedName           = UTF32_CaseFoldString(NormalizedName);
            free(NormalizedName);
            CLI->ProgramLicenseName         = CaseFoldedName;
            
            UTF32 *DecodedLicenseURL        = UTF8_Decode(LicenseURL);
            UTF32 *NormalizedLicenseURL     = UTF32_NormalizeString(DecodedLicenseURL, NormalizationFormKC);
            free(DecodedLicenseURL);
            UTF32 *CaseFoldedLicenseURL     = UTF32_CaseFoldString(NormalizedLicenseURL);
            free(NormalizedLicenseURL);
            CLI->ProgramLicenseURL          = CaseFoldedLicenseURL;
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
    
    void CommandLineIO_UTF16_SetLicense(CommandLineIO *CLI, CLILicenseTypes LicenseType, UTF16 *Name, UTF16 *LicenseURL) {
        if (CLI != NULL && LicenseType != UnknownLicenseType && Name != NULL && LicenseURL != NULL) {
            CLI->LicenseType                = LicenseType;
            UTF32 *DecodedName              = UTF16_Decode(Name);
            UTF32 *NormalizedName           = UTF32_NormalizeString(DecodedName, NormalizationFormKC);
            free(DecodedName);
            UTF32 *CaseFoldedName           = UTF32_CaseFoldString(NormalizedName);
            free(NormalizedName);
            CLI->ProgramLicenseName         = CaseFoldedName;
            
            UTF32 *DecodedLicenseURL        = UTF16_Decode(LicenseURL);
            UTF32 *NormalizedLicenseURL     = UTF32_NormalizeString(DecodedLicenseURL, NormalizationFormKC);
            free(DecodedLicenseURL);
            UTF32 *CaseFoldedLicenseURL     = UTF32_CaseFoldString(NormalizedLicenseURL);
            free(NormalizedLicenseURL);
            CLI->ProgramLicenseURL          = CaseFoldedLicenseURL;
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
    
    void CommandLineIO_SetMinOptions(CommandLineIO *CLI, const int64_t MinOptions) {
        if (CLI != NULL && MinOptions >= 0 && MinOptions <= CLI->NumSwitches - 1) {
            CLI->MinOptions = MinOptions;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MinOptions < 0 || MinOptions > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MinOptions %lld is invalid, it should be between 0 and %lld"), MinOptions, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_SetHelpSwitch(CommandLineIO *CLI, const int64_t HelpSwitch) {
        if (CLI != NULL && (HelpSwitch >= 0 && HelpSwitch <= CLI->NumSwitches - 1)) {
            CLI->HelpSwitch = HelpSwitch;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (HelpSwitch < 0 || HelpSwitch > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("HelpSwitch %lld is invalid, it should be between 0 and %lld"), HelpSwitch, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_UTF8_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF8 **Strings, uint64_t *Numerator, uint64_t *Denominator) {
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
            for (uint8_t Item = 0; Item < NumItems2Display - 1; Item++) { // Get the size of the strings
                StringSize[Item] = UTF8_GetStringSizeInCodePoints(Strings[Item]);
                // huh, well we need 2 characters for the brackets.
            }
            // Number of seperators for each string
            uint64_t *NumProgressIndicatorsPerString = calloc(NumItems2Display, sizeof(uint64_t));
            UTF8     *ActualStrings2Print            = calloc(NumItems2Display, sizeof(UTF8));
            for (uint8_t String = 0; String < NumItems2Display - 1; String++) { // Actually create the strings
                                                                            // Subtract 2 for the brackets, + the size of each string from the actual width of the console window
                NumProgressIndicatorsPerString[String] = CommandLineIO_GetTerminalWidth() - (2 + StringSize[String]); // what if it's not even?
                uint64_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
                uint64_t HalfOfTheIndicators = (PercentComplete / 2);
                UTF8    *Indicator           = calloc(CommandLineIO_GetTerminalWidth(), sizeof(UTF8));
                UTF8_Insert(Indicator, '-', 0);
                UTF8    *FormattedString     = UTF8_FormatString(U8("[%s%s %lld/%lld %hhu/%s %s]"), Indicator, Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, NewLineUTF8);
                UTF8_WriteString(FormattedString, stdout);
                free(Indicator);
            }
            free(StringSize);
            free(NumProgressIndicatorsPerString);
            free(ActualStrings2Print);
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    void CommandLineIO_UTF16_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF16 **Strings, uint64_t *Numerator, uint64_t *Denominator) {
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
            for (uint8_t Item = 0; Item < NumItems2Display - 1; Item++) { // Get the size of the strings
                StringSize[Item] = UTF16_GetStringSizeInCodePoints(Strings[Item]);
                // huh, well we need 2 characters for the brackets.
            }
            // Number of seperators for each string
            uint64_t *NumProgressIndicatorsPerString = calloc(NumItems2Display, sizeof(uint64_t));
            UTF16    *ActualStrings2Print            = calloc(NumItems2Display, sizeof(UTF16));
            for (uint8_t String = 0; String < NumItems2Display - 1; String++) { // Actually create the strings
                                                                            // Subtract 2 for the brackets, + the size of each string from the actual width of the console window
                NumProgressIndicatorsPerString[String] = CommandLineIO_GetTerminalWidth() - (2 + StringSize[String]); // what if it's not even?
                uint8_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
                UTF16  *Indicator           = calloc(CommandLineIO_GetTerminalWidth, sizeof(UTF16));
                UTF8_Insert(Indicator, U16('-'), 0);
                UTF16  *FormattedString     = UTF16_FormatString(U16("[%s%s %lld/%lld %hhu/%s %s]"), Indicator, Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, NewLineUTF16);
                UTF16_WriteString(FormattedString, stdout);
                free(Indicator);
            }
            free(StringSize);
            free(NumProgressIndicatorsPerString);
            free(ActualStrings2Print);
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    void CommandLineIO_UTF8_Switch_SetName(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Name) {
        if (CLI != NULL && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) && Name != NULL) {
            UTF32 *Decoded                = UTF8_Decode(Name);
            UTF32 *Normalized             = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded             = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->SwitchIDs[SwitchID].Name = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, U8("Name String is NULL"));
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_UTF16_Switch_SetName(CommandLineIO *CLI, const int64_t SwitchID, UTF16 *Name) {
        if (CLI != NULL && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1) && Name != NULL) {
            UTF32 *Decoded                = UTF16_Decode(Name);
            UTF32 *Normalized             = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded             = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->SwitchIDs[SwitchID].Name = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Log_ERROR, __func__, U8("Name String is NULL"));
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_UTF8_Switch_SetDescription(CommandLineIO *CLI, const int64_t SwitchID, UTF8 *Description) {
        if (CLI != NULL && Description != NULL && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1)) {
            UTF32 *Decoded                       = UTF8_Decode(Description);
            UTF32 *Normalized                    = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded                    = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->SwitchIDs[SwitchID].Description = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, U8("Description String is NULL"));
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_UTF16_Switch_SetDescription(CommandLineIO *CLI, const int64_t SwitchID, UTF16 *Description) {
        if (CLI != NULL && Description != NULL && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1)) {
            UTF32 *Decoded                       = UTF16_Decode(Description);
            UTF32 *Normalized                    = UTF32_NormalizeString(Decoded, NormalizationFormKC);
            free(Decoded);
            UTF32 *CaseFolded                    = UTF32_CaseFoldString(Normalized);
            free(Normalized);
            CLI->SwitchIDs[SwitchID].Description = CaseFolded;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Log_ERROR, __func__, U8("Description String is NULL"));
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_Switch_SetType(CommandLineIO *CLI, const int64_t SwitchID, CLISwitchTypes SwitchType) {
        if (CLI != NULL && SwitchType != UnknownSwitchType && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1)) {
            CLI->SwitchIDs[SwitchID].SwitchType = SwitchType;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (SwitchType == UnknownSwitchType) {
            Log(Log_ERROR, __func__, U8("You can not set SwitchID %lld to UnknownSwitchType"), SwitchID);
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_Switch_SetArgumentType(CommandLineIO *CLI, const int64_t SwitchID, CLIArgumentTypes ArgumentType) {
        if (CLI != NULL && ArgumentType != UnknownArgumentType && (SwitchID >= 0 && SwitchID <= CLI->NumSwitches - 1)) {
            CLI->SwitchIDs[SwitchID].ArgumentType = ArgumentType;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (ArgumentType == UnknownArgumentType) {
            Log(Log_ERROR, __func__, U8("You can not set SwitchID %lld to UnknownArgumentType"), SwitchID);
        } else if (SwitchID < 0 || SwitchID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_Switch_SetSlave(CommandLineIO *CLI, const int64_t MasterID, const int64_t SlaveID) {
        if (CLI != NULL && (MasterID >= 0 && MasterID <= CLI->NumSwitches - 1) && (SlaveID >= 0 && SlaveID <= CLI->NumSwitches - 1)) {
            if (CLI->SwitchIDs[MasterID].SwitchType == SwitchMayHaveSlaves && (CLI->SwitchIDs[MasterID].NumPotentialSlaves <= CLI->SwitchIDs[MasterID].MaxConcurrentSlaves)) {
                CLI->SwitchIDs[MasterID].NumPotentialSlaves  += 1;
                CLI->SwitchIDs[MasterID].PotentialSlaves      = realloc(CLI->SwitchIDs[MasterID].PotentialSlaves, CLI->SwitchIDs[MasterID].NumPotentialSlaves * sizeof(int64_t));
                CLI->SwitchIDs[MasterID].PotentialSlaves[CLI->SwitchIDs[MasterID].NumPotentialSlaves - 1] = SlaveID;
            } else {
                Log(Log_ERROR, __func__, U8("MasterID %lld can not have any slaves"), MasterID);
            }
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MasterID < 0 || MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MasterID %lld is invalid, it should be between 0 and %lld"), MasterID, CLI->NumSwitches - 1);
        } else if (SlaveID < 0 || SlaveID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("SlaveID %lld is invalid, it should be between 0 and %lld"), SlaveID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_Switch_SetMinConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MinConcurrentSlaves) {
        if (CLI != NULL && (MasterID >= 0 && MasterID <= CLI->NumSwitches - 1)) {
            CLI->SwitchIDs[MasterID].MaxConcurrentSlaves = MinConcurrentSlaves;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MasterID < 0 || MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MasterID %lld is invalid, it should be between 0 and %lld"), MasterID, CLI->NumSwitches - 1);
        }
    }
    
    void CommandLineIO_Switch_SetMaxConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxConcurrentSlaves) {
        if (CLI != NULL && (MasterID >= 0 && MasterID <= CLI->NumSwitches - 1)) {
            CLI->SwitchIDs[MasterID].MaxConcurrentSlaves = MaxConcurrentSlaves;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MasterID < 0 || MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MasterID %lld is invalid, it should be between 0 and %lld"), MasterID, CLI->NumSwitches - 1);
        }
    }
    
    static inline void CommandLineIO_UTF8_DisplayHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name = UTF8_Encode(CLI->ProgramName);
            
            UTF8 *ProgramsOptions = UTF8_FormatString(U8("%s's Options (-|--|/):%s"), Name, NewLineUTF8);
            UTF8_WriteString(ProgramsOptions, stdout);
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                CLISwitchTypes CurrentSwitchType = CLI->SwitchIDs[Switch].SwitchType;
                
                UTF8 *SwitchName        = UTF8_Encode(CLI->SwitchIDs[Switch].Name);
                UTF8 *SwitchDescription = UTF8_Encode(CLI->SwitchIDs[Switch].Description);
                
                UTF8 *SwitchInfo        = UTF8_FormatString(U8("%s: %s%s"), SwitchName, SwitchDescription, NewLineUTF8);
                
                UTF8_WriteString(SwitchInfo, stdout);
                if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->SwitchIDs[Switch].NumPotentialSlaves > 0) {
                    for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; SlaveSwitch++) {
                        UTF8 *SlaveName        = UTF8_Encode(CLI->SwitchIDs[SlaveSwitch].Name);
                        UTF8 *SlaveDescription = UTF8_Encode(CLI->SwitchIDs[SlaveSwitch].Description);
                        
                        UTF8 *SlaveSwitchInfo  = UTF8_FormatString(U8("\t%s: %s%s"), SlaveName, SlaveDescription, NewLineUTF8);
                        
                        UTF8_WriteString(SlaveSwitchInfo, stdout);
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static inline void CommandLineIO_UTF16_DisplayHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF16 *Name = UTF16_Encode(CLI->ProgramName);
            
            UTF16 *ProgramsOptions = UTF16_FormatString(U16("%s's Options (-|--|/):%s"), Name, NewLineUTF16);
            UTF16_WriteString(ProgramsOptions, stdout);
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                CLISwitchTypes CurrentSwitchType = CLI->SwitchIDs[Switch].SwitchType;
                
                UTF16 *SwitchName        = UTF16_Encode(CLI->SwitchIDs[Switch].Name);
                UTF16 *SwitchDescription = UTF16_Encode(CLI->SwitchIDs[Switch].Description);
                
                UTF16 *SwitchInfo        = UTF16_FormatString(U16("%s: %s%s"), SwitchName, SwitchDescription, NewLineUTF16);
                
                UTF16_WriteString(SwitchInfo, stdout);
                if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->SwitchIDs[Switch].NumPotentialSlaves > 0) {
                    for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; SlaveSwitch++) {
                        UTF16 *SlaveName        = UTF16_Encode(CLI->SwitchIDs[SlaveSwitch].Name);
                        UTF16 *SlaveDescription = UTF16_Encode(CLI->SwitchIDs[SlaveSwitch].Description);
                        
                        UTF16 *SlaveSwitchInfo = UTF16_FormatString(U16("\t%s: %s%s"), SlaveName, SlaveDescription, NewLineUTF16);
                        
                        UTF16_WriteString(SlaveSwitchInfo, stdout);
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static inline void CommandLineIO_UTF8_DisplayBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name                  = UTF8_Encode(CLI->ProgramName);
            UTF8 *Version               = UTF8_Encode(CLI->ProgramVersion);
            UTF8 *Author                = UTF8_Encode(CLI->ProgramAuthor);
            UTF8 *Copyright             = UTF8_Encode(CLI->ProgramCopyright);
            UTF8 *Description           = UTF8_Encode(CLI->ProgramDescription);
            UTF8 *LicenseName           = UTF8_Encode(CLI->ProgramLicenseName);
            UTF8 *LicenseDescription    = UTF8_Encode(CLI->ProgramLicenseDescription);
            UTF8 *LicenseURL            = UTF8_Encode(CLI->ProgramLicenseURL);
            CLILicenseTypes LicenseType = CLI->LicenseType;
            
            if (Name != NULL) { // TrimSilence, v. 0.1.1 by Marcus Johnson © 2018+
                UTF8 *NameString      = UTF8_FormatString(U8("%s,"), Name);
                UTF8_WriteString(NameString, stdout);
                
                UTF8 *FormattedString = UTF8_FormatString(U8(" %s, v. %s by %s © %s%s"), Name, Version, Author, Copyright, NewLineUTF16);
                UTF8_WriteString(FormattedString, stdout);
            }
            
            if (Version != NULL) {
                UTF8 *VersionString   = UTF8_FormatString(U8(" v. %s"), Version);
                UTF8_WriteString(VersionString, stdout);
            }
            
            if (Author != NULL) {
                UTF8 *AuthorString   = UTF8_FormatString(U8(" by %s"), Author);
                UTF8_WriteString(AuthorString, stdout);
            }
            
            if (Copyright != NULL) {
                UTF8 *CopyrightString   = UTF8_FormatString(U8(" © %s\n"), Copyright);
                UTF8_WriteString(CopyrightString, stdout);
            }
            
            if (LicenseType == PermissiveLicense || LicenseType == CopyleftLicense) {
                if (Description != NULL) {
                    UTF8 *DescriptionString   = UTF8_FormatString(U8("%s"), Description);
                    UTF8_WriteString(DescriptionString, stdout);
                }
                
                if (LicenseName != NULL) { // Released under the "BSD 3 clause" license
                    UTF8 *LicenseNameString   = UTF8_FormatString(U8(" Released under the \"%s\" license"), LicenseName);
                    UTF8_WriteString(LicenseNameString, stdout);
                }
                
                if (LicenseDescription != NULL) {
                    UTF8 *LicenseDescriptionString   = UTF8_FormatString(U8(" %s,"), LicenseDescription);
                    UTF8_WriteString(LicenseDescriptionString, stdout);
                }
                
                if (LicenseURL != NULL) {
                    UTF8 *LicenseURLString   = UTF8_FormatString(U8(" %s\n"), LicenseURL);
                    UTF8_WriteString(LicenseURLString, stdout);
                }
            } else if (LicenseType == ProprietaryLicense) {
                if (Description != NULL) {
                    UTF8 *DescriptionString   = UTF8_FormatString(U8("%s"), Description);
                    UTF8_WriteString(DescriptionString, stdout);
                }
                
                if (LicenseDescription != NULL) {
                    UTF8 *LicenseDescriptionString   = UTF8_FormatString(U8(" By using this software, you agree to the End User License Agreement %s,"), LicenseDescription);
                    UTF8_WriteString(LicenseDescriptionString, stdout);
                }
                
                if (LicenseURL != NULL) {
                    UTF8 *LicenseURLString   = UTF8_FormatString(U8(" available at: %s\n"), LicenseURL);
                    UTF8_WriteString(LicenseURLString, stdout);
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static inline void CommandLineIO_UTF16_DisplayBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF16 *Name                 = UTF16_Encode(CLI->ProgramName);
            UTF16 *Version              = UTF16_Encode(CLI->ProgramVersion);
            UTF16 *Author               = UTF16_Encode(CLI->ProgramAuthor);
            UTF16 *Copyright            = UTF16_Encode(CLI->ProgramCopyright);
            UTF16 *Description          = UTF16_Encode(CLI->ProgramDescription);
            UTF16 *LicenseName          = UTF16_Encode(CLI->ProgramLicenseName);
            UTF16 *LicenseDescription   = UTF16_Encode(CLI->ProgramLicenseDescription);
            UTF16 *LicenseURL           = UTF16_Encode(CLI->ProgramLicenseURL);
            CLILicenseTypes LicenseType = CLI->LicenseType;
            
            if (Name != NULL) {
                UTF16 *NameString      = UTF16_FormatString(U16("%s,"), Name);
                UTF16_WriteString(NameString, stdout);
                
                UTF16 *FormattedString = UTF16_FormatString(U16(" %s, v. %s by %s © %s%s"), Name, Version, Author, Copyright, NewLineUTF16);
                UTF16_WriteString(FormattedString, stdout);
            }
            
            if (Version != NULL) {
                UTF16 *VersionString   = UTF16_FormatString(U16(" v. %s"), Version);
                UTF16_WriteString(VersionString, stdout);
            }
            
            if (Author != NULL) {
                UTF16 *AuthorString   = UTF16_FormatString(U16(" by %s"), Author);
                UTF16_WriteString(AuthorString, stdout);
            }
            
            if (Copyright != NULL) {
                UTF16 *CopyrightString   = UTF16_FormatString(U16(" © %s\n"), Copyright);
                UTF16_WriteString(CopyrightString, stdout);
            }
            
            if (LicenseType == PermissiveLicense || LicenseType == CopyleftLicense) {
                if (Description != NULL) {
                    UTF16 *DescriptionString   = UTF16_FormatString(U16("%s"), Description);
                    UTF16_WriteString(DescriptionString, stdout);
                }
                
                if (LicenseName != NULL) {
                    UTF16 *LicenseNameString   = UTF16_FormatString(U16(" Released under the \"%s\" license"), LicenseName);
                    UTF16_WriteString(LicenseNameString, stdout);
                }
                
                if (LicenseDescription != NULL) {
                    UTF16 *LicenseDescriptionString   = UTF16_FormatString(U16(" %s,"), LicenseDescription);
                    UTF16_WriteString(LicenseDescriptionString, stdout);
                }
                
                if (LicenseURL != NULL) {
                    UTF16 *LicenseURLString   = UTF16_FormatString(U16(" %s\n"), LicenseURL);
                    UTF16_WriteString(LicenseURLString, stdout);
                }
            } else if (LicenseType == ProprietaryLicense) {
                if (Description != NULL) {
                    UTF16 *DescriptionString   = UTF16_FormatString(U16("%s"), Description);
                    UTF16_WriteString(DescriptionString, stdout);
                }
                
                if (LicenseDescription != NULL) {
                    UTF16 *LicenseDescriptionString   = UTF16_FormatString(U16(" By using this software, you agree to the End User License Agreement %s,"), LicenseDescription);
                    UTF16_WriteString(LicenseDescriptionString, stdout);
                }
                
                if (LicenseURL != NULL) {
                    UTF16 *LicenseURLString   = UTF16_FormatString(U16(" available at: %s\n"), LicenseURL);
                    UTF16_WriteString(LicenseURLString, stdout);
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static UTF32 *ArgumentString2SwitchFlag(UTF32 *ArgumentString) {
        UTF32 *ArgumentSwitch = NULL;
        if (ArgumentString != NULL) {
            uint8_t  ArgumentStringPrefixSize = 0;
            uint64_t ArgumentStringSize       = UTF32_GetStringSizeInCodePoints(ArgumentString);
            
            if (ArgumentStringSize >= 2) {
                if (ArgumentString[0] == U32('-') && ArgumentString[1] == U32('-')) {
                    ArgumentStringPrefixSize  = 2;
                } else if (ArgumentString[0] == U32('/') || ArgumentString[0] == U32('\\') || ArgumentString[0] == U32('-')) {
                    ArgumentStringPrefixSize  = 1;
                }
            } else {
                Log(Log_ERROR, __func__, U8("ArgumentString is not an Argument string"));
            }
            uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
            ArgumentSwitch                 = UTF32_ExtractSubString(UTF32_CaseFoldString(ArgumentString), ArgumentStringPrefixSize, ArgumentStringSize);
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return ArgumentSwitch;
    }
    
    static void CommandLineIO_UTF32_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF32 **Arguments) {
        if (CLI != NULL && CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions) {
            int32_t  CurrentArgument      = 1LL;
            do {
                UTF32   *Argument         = Arguments[CurrentArgument];
                UTF32   *ArgumentFlag     = ArgumentString2SwitchFlag(Argument);
                
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                    if (UTF32_CompareSubstring(ArgumentFlag, CLI->SwitchIDs[Switch].Name, 0, 0) == Yes) {
                        
                        CLI->NumOptions   += 1;
                        if (CLI->NumOptions == 1) {
                            CLI->OptionIDs = calloc(1, sizeof(CommandLineOption));
                        } else {
                            CLI->OptionIDs = realloc(CLI->OptionIDs, CLI->NumOptions * sizeof(CommandLineOption));
                        }
                        CLI->OptionIDs[CLI->NumOptions - 1].SwitchID = Switch;
                        
                        for (int64_t ArgSlave = 1LL; ArgSlave < CLI->SwitchIDs[Switch].MaxConcurrentSlaves; ArgSlave++) {
                            UTF32 *PotentialSlaveArg        = Arguments[CurrentArgument + ArgSlave];
                            for (int64_t Slave = 0LL; Slave < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; Slave++) {
                                UTF32 *PotentialSlaveFlag   = CLI->SwitchIDs[CLI->SwitchIDs[Switch].PotentialSlaves[Slave]].Name;
                                
                                if (UTF32_CompareSubstring(PotentialSlaveArg, PotentialSlaveFlag, 0, 0) == Yes) {
                                    
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
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            Log(Log_ERROR, __func__, U8("You entered %lld options, the minimum is %lld"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    void CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF8 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32     = calloc(NumArguments, sizeof(UTF32*));
            for (int64_t Arg = 0LL; Arg < NumArguments - 1; Arg++) {
                UTF32 *Decoded      = UTF8_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFoldString(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_NormalizeString(CaseFolded, NormalizationFormKC);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, Arguments32);
            for (int64_t Arg = 0LL; Arg < NumArguments - 1; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            CommandLineIO_UTF8_DisplayHelp(CLI);
        }
    }
    
    void CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF16 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32     = calloc(NumArguments, sizeof(UTF32*));
            for (int64_t Arg = 0LL; Arg < NumArguments - 1; Arg++) {
                UTF32 *Decoded      = UTF16_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFoldString(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_NormalizeString(CaseFolded, NormalizationFormKC);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, Arguments32);
            for (int64_t Arg = 0LL; Arg < NumArguments - 1; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            CommandLineIO_UTF16_DisplayHelp(CLI);
        }
    }
    
    int64_t CommandLineIO_GetNumMatchingOptions(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
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
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumSwitches - 1);
        } else if (NumSlaves < 0 || NumSlaves > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("NumSlaves %lld is invalid, it should be between 0 and %lld"), NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        }
        return NumMatchingOptions;
    }
    
    int64_t CommandLineIO_GetOptionNum(CommandLineIO *CLI, const int64_t OptionID, const int64_t NumSlaves, const int64_t *SlaveIDs) {
        int64_t MatchingOption = -1LL;
        if (CLI != NULL && OptionID >= 0 && OptionID <= CLI->NumOptions - 1 && NumSlaves >= 0 && NumSlaves <= CLI->NumOptions - 1) {
            bool AllOptionsMatch   = No;
            for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) {
                if (CLI->OptionIDs[Option].SwitchID == OptionID) {
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
            }
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("SwitchID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumSwitches - 1);
        } else if (NumSlaves < 0 || NumSlaves > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("NumSlaves %lld is invalid, it should be between 0 and %lld"), NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        }
        return MatchingOption;
    }
    
    UTF8 *CommandLineIO_UTF8_GetOptionResult(CommandLineIO const *CLI, const int64_t OptionID) {
        UTF8 *Result = NULL;
        if (CLI != NULL && OptionID >= 0 && OptionID <= CLI->NumOptions - 1) {
            Result = UTF8_Encode(CLI->OptionIDs[OptionID].Argument);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("Option %lld is outside the range 0 - %lld"), OptionID, CLI->NumOptions - 1);
        }
        return Result;
    }
    
    UTF16 *CommandLineIO_UTF16_GetOptionResult(CommandLineIO const *CLI, const int64_t OptionID) {
        UTF16 *Result = NULL;
        if (CLI != NULL && OptionID >= 0 && OptionID <= CLI->NumOptions - 1) {
            Result = UTF16_Encode(CLI->OptionIDs[OptionID].Argument);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (OptionID < 0 || OptionID > CLI->NumOptions - 1) {
            Log(Log_ERROR, __func__, U8("Option %lld is outside the range 0 - %lld"), OptionID, CLI->NumOptions - 1);
        }
        return Result;
    }
    
    static UTF32 *CommandLineIO_UTF32_GetExtension(UTF32 *Path) {
        UTF32 *Extension = NULL;
        if (Path != NULL) {
            uint64_t StringSize    = UTF32_GetStringSizeInCodePoints(Path);
            uint64_t ExtensionSize = 0ULL;
            for (uint64_t CodePoint = StringSize; CodePoint > 0; CodePoint--) {
                if (Path[CodePoint] == U32('.')) {
                    ExtensionSize = CodePoint - StringSize;
                }
            }
            Extension = calloc(UnicodeBOMSizeInCodePoints + ExtensionSize + NULLTerminatorSize , sizeof(UTF32));
            if (Extension != NULL) {
                for (uint64_t ExtCodePoint = 0ULL; ExtCodePoint < ExtensionSize - 1; ExtCodePoint++) {
                    for (uint64_t PathCodePoint = StringSize - ExtensionSize; PathCodePoint < StringSize - 1; PathCodePoint++) {
                        Extension[ExtCodePoint] = Path[PathCodePoint];
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate %lld codepoints for the Extension"), ExtensionSize);
            }
        } else {
            Log(Log_ERROR, __func__, U8("Path Pointer is NULL"));
        }
        return Extension;
    }
    
    UTF8 *CommandLineIO_UTF8_GetExtension(UTF8 *Path) {
        UTF8 *Extension = NULL;
        if (Path != NULL) {
            UTF32 *Decoded     = UTF8_Decode(Path);
            UTF32 *Extension32 = CommandLineIO_UTF32_GetExtension(Decoded);
            free(Decoded);
            Extension          = UTF8_Encode(Extension32);
        } else {
            Log(Log_ERROR, __func__, U8("Path Pointer is NULL"));
        }
        return Extension;
    }
    
    UTF16 *CommandLineIO_UTF16_GetExtension(UTF16 *Path) {
        UTF16 *Extension       = NULL;
        if (Path != NULL) {
            UTF32 *Decoded     = UTF16_Decode(Path);
            UTF32 *Extension32 = CommandLineIO_UTF32_GetExtension(Decoded);
            free(Decoded);
            Extension          = UTF16_Encode(Extension32);
            free(Extension32);
        } else {
            Log(Log_ERROR, __func__, U8("Path Pointer is NULL"));
        }
        return Extension;
    }
    
    void CommandLineIO_Deinit(CommandLineIO *CLI) {
        if (CLI != NULL) {
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                free(CLI->SwitchIDs[Switch].Name);
                free(CLI->SwitchIDs[Switch].Description);
                free(CLI->SwitchIDs[Switch].PotentialSlaves);
            }
            for (int64_t Option = 0LL; Option < CLI->NumOptions - 1; Option++) {
                free(CLI->OptionIDs[Option].OptionSlaves);
                free(CLI->OptionIDs[Option].Argument);
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
    
#ifdef __cplusplus
}
#endif
