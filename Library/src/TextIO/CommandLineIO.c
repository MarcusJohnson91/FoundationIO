#include "../include/Macros.h"
#include "../include/CommandLineIO.h"  /* Included for the CommandLineIO declarations */
#include "../include/Log.h"            /* Included for Log */

#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
#include <sys/ioctl.h>                 /* Included for the terminal size */
#include <sys/ttycom.h>                /* Included for winsize, TIOCGWINSZ */
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
#include <Windows.h>                   /* Included because WinCon needs it */
#include <Wincon.h>                    /* Included for getting the terminal size */
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
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
        UTF32               *Name;
        UTF32               *Description;
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
        UTF32               *Argument;
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
        uint16_t             ConsoleWidth;
        uint16_t             ConsoleHeight;
        bool                 IsProprietary;
    } CommandLineIO;
    
    CommandLineIO *CommandLineIO_Init(const int64_t NumSwitches) {
        CommandLineIO *CLI           = NULL;
        if (NumSwitches >= 0) {
            CLI                      = calloc(1, sizeof(CommandLineIO));
            if (CLI != NULL) {
#if   (FoundationIOTargetOS == FoundationIOOSPOSIX)
                struct winsize       WindowSize;
                ioctl(0, TIOCGWINSZ, &WindowSize);
                CLI->ConsoleWidth    = WindowSize.ws_row;
                CLI->ConsoleHeight   = WindowSize.ws_col;
#elif (FoundationIOTargetOS == FoundationIOOSWindows)
                CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
                GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
                CLI->ConsoleHeight   = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;
                CLI->ConsoleWidth    = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
                CLI->SwitchIDs       = calloc(NumSwitches, sizeof(CommandLineSwitch));
                if (CLI->SwitchIDs != NULL) {
                    CLI->NumSwitches = NumSwitches;
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate %lld CommandLineSwitches"), NumSwitches);
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate CommandLineIO"));
            }
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("Couldn't allocate CommandLineIO"));
        } else if (NumSwitches == 0) {
            Log(Log_ERROR, __func__, U8("NumSwitches %lld must be greater than or equal to 1"), NumSwitches);
        }
        return CLI;
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
            if (LicenseType == PermissiveLicense || LicenseType == CopyleftLicense) {
                CLI->IsProprietary          = No;
            } else if (LicenseType == ProprietaryLicense) {
                CLI->IsProprietary          = Yes;
            }
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
            if (LicenseType == PermissiveLicense || LicenseType == CopyleftLicense) {
                CLI->IsProprietary          = No;
            } else if (LicenseType == ProprietaryLicense) {
                CLI->IsProprietary          = Yes;
            }
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
            for (uint8_t Item = 0; Item < NumItems2Display; Item++) { // Get the size of the strings
                StringSize[Item] = UTF8_GetStringSizeInCodePoints(Strings[Item]);
                // huh, well we need 2 characters for the brackets.
            }
            // Number of seperators for each string
            uint64_t *NumProgressIndicatorsPerString = calloc(NumItems2Display, sizeof(uint64_t));
            UTF8     *ActualStrings2Print            = calloc(NumItems2Display, sizeof(UTF8));
            for (uint8_t String = 0; String < NumItems2Display; String++) { // Actually create the strings
                // Subtract 2 for the brackets, + the size of each string from the actual width of the console window
                NumProgressIndicatorsPerString[String] = CLI->ConsoleWidth - (2 + StringSize[String]); // what if it's not even?
                uint8_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
                uint8_t HalfOfTheIndicators = (PercentComplete / 2);
                // Now we go ahead and memset a string with the proper number of indicators
                UTF8 *Indicator             = calloc(CLI->ConsoleWidth, sizeof(UTF8));
                memset(Indicator, '-', HalfOfTheIndicators);
                UTF8 *FormattedString       = UTF8_FormatString(U8("[%s%s %lld/%lld %hhu/%s %s]"), Indicator, Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, NewLineUTF8);
                UTF8_WriteString2File(FormattedString, stdout);
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
            for (uint8_t Item = 0; Item < NumItems2Display; Item++) { // Get the size of the strings
                StringSize[Item] = UTF16_GetStringSizeInCodePoints(Strings[Item]);
                // huh, well we need 2 characters for the brackets.
            }
            // Number of seperators for each string
            uint64_t *NumProgressIndicatorsPerString = calloc(NumItems2Display, sizeof(uint64_t));
            UTF16    *ActualStrings2Print            = calloc(NumItems2Display, sizeof(UTF16));
            for (uint8_t String = 0; String < NumItems2Display; String++) { // Actually create the strings
                // Subtract 2 for the brackets, + the size of each string from the actual width of the console window
                NumProgressIndicatorsPerString[String] = CLI->ConsoleWidth - (2 + StringSize[String]); // what if it's not even?
                uint8_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
                uint8_t HalfOfTheIndicators = (PercentComplete / 2);
                // Now we go ahead and memset a string with the proper number of indicators
                UTF16 *Indicator            = calloc(CLI->ConsoleWidth, sizeof(UTF16));
                memset(Indicator, '-', HalfOfTheIndicators);
                UTF16 *FormattedString      = UTF16_FormatString(U16("[%s%s %lld/%lld %hhu/%s %s]"), Indicator, Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, NewLineUTF16);
                UTF16_WriteString2File(FormattedString, stdout);
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
    
    void CommandLineIO_Switch_SetMaxConcurrentSlaves(CommandLineIO *CLI, const int64_t MasterID, const int64_t MaxConcurrentSlaves) {
        if (CLI != NULL && (MasterID >= 0 && MasterID <= CLI->NumSwitches - 1) && (MaxConcurrentSlaves >= 0 && MaxConcurrentSlaves <= CLI->NumSwitches - 1)) {
            CLI->SwitchIDs[MasterID].MaxConcurrentSlaves = MaxConcurrentSlaves;
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (MasterID < 0 || MasterID > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MasterID %lld is invalid, it should be between 0 and %lld"), MasterID, CLI->NumSwitches - 1);
        } else if (MaxConcurrentSlaves < 0 || MaxConcurrentSlaves > CLI->NumSwitches - 1) {
            Log(Log_ERROR, __func__, U8("MaxConcurrentSlaves %lld is invalid, it should be between 0 and %lld"), MasterID, CLI->NumSwitches - 1);
        }
    }
    
    static inline void CommandLineIO_UTF8_DisplayHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name = UTF8_Encode(CLI->ProgramName);
            
            UTF8 *ProgramsOptions = UTF8_FormatString(U8("%s's Options (-|--|/):%s"), Name, NewLineUTF8);
            UTF8_WriteString2File(ProgramsOptions, stdout);
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                CLISwitchTypes CurrentSwitchType = CLI->SwitchIDs[Switch].SwitchType;
                
                UTF8 *SwitchName        = UTF8_Encode(CLI->SwitchIDs[Switch].Name);
                UTF8 *SwitchDescription = UTF8_Encode(CLI->SwitchIDs[Switch].Description);
                
                UTF8 *SwitchInfo        = UTF8_FormatString(U8("%s: %s%s"), SwitchName, SwitchDescription, NewLineUTF8);
                
                UTF8_WriteString2File(SwitchInfo, stdout);
                if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->SwitchIDs[Switch].NumPotentialSlaves > 0) {
                    for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; SlaveSwitch++) {
                        UTF8 *SlaveName        = UTF8_Encode(CLI->SwitchIDs[SlaveSwitch].Name);
                        UTF8 *SlaveDescription = UTF8_Encode(CLI->SwitchIDs[SlaveSwitch].Description);
                        
                        UTF8 *SlaveSwitchInfo  = UTF8_FormatString(U8("\t%s: %s%s"), SlaveName, SlaveDescription, NewLineUTF8);
                        
                        UTF8_WriteString2File(SlaveSwitchInfo, stdout);
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
            UTF16_WriteString2File(ProgramsOptions, stdout);
            for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                CLISwitchTypes CurrentSwitchType = CLI->SwitchIDs[Switch].SwitchType;
                
                UTF16 *SwitchName        = UTF16_Encode(CLI->SwitchIDs[Switch].Name);
                UTF16 *SwitchDescription = UTF16_Encode(CLI->SwitchIDs[Switch].Description);
                
                UTF16 *SwitchInfo        = UTF16_FormatString(U16("%s: %s%s"), SwitchName, SwitchDescription, NewLineUTF16);
                
                UTF16_WriteString2File(SwitchInfo, stdout);
                if (CurrentSwitchType == SwitchMayHaveSlaves && CLI->SwitchIDs[Switch].NumPotentialSlaves > 0) {
                    for (int64_t SlaveSwitch = 0LL; SlaveSwitch < CLI->SwitchIDs[Switch].NumPotentialSlaves - 1; SlaveSwitch++) {
                        UTF16 *SlaveName        = UTF16_Encode(CLI->SwitchIDs[SlaveSwitch].Name);
                        UTF16 *SlaveDescription = UTF16_Encode(CLI->SwitchIDs[SlaveSwitch].Description);
                        
                        UTF16 *SlaveSwitchInfo = UTF16_FormatString(U16("\t%s: %s%s"), SlaveName, SlaveDescription, NewLineUTF16);
                        
                        UTF16_WriteString2File(SlaveSwitchInfo, stdout);
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static inline void CommandLineIO_UTF8_DisplayBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name               = UTF8_Encode(CLI->ProgramName);
            UTF8 *Version            = UTF8_Encode(CLI->ProgramVersion);
            UTF8 *Author             = UTF8_Encode(CLI->ProgramAuthor);
            UTF8 *Copyright          = UTF8_Encode(CLI->ProgramCopyright);
            UTF8 *Description        = UTF8_Encode(CLI->ProgramDescription);
            UTF8 *LicenseName        = UTF8_Encode(CLI->ProgramLicenseName);
            UTF8 *LicenseDescription = UTF8_Encode(CLI->ProgramLicenseDescription);
            UTF8 *LicenseURL         = UTF8_Encode(CLI->ProgramLicenseURL);
            
            if (Name != NULL && Version != NULL && Author != NULL && Copyright != NULL) {
                UTF8 *FormattedString = UTF8_FormatString(U8("%s, v. %s by %s © %s%s"), Name, Version, Author, Copyright, NewLineUTF8);
                UTF8_WriteString2File(FormattedString, stdout);
            }
            if (CLI->IsProprietary == No && Description != NULL && LicenseName != NULL && LicenseDescription != NULL && LicenseURL != NULL) {
                UTF8 *FormattedString = UTF8_FormatString(U8("%s, Released under the \"%s\" %s, available at: %s%s"), Description, LicenseName, LicenseDescription, LicenseURL, NewLineUTF8);
                UTF8_WriteString2File(FormattedString, stdout);
            } else if (CLI->IsProprietary == Yes && Description != NULL && LicenseDescription != NULL && LicenseURL != NULL) {
                UTF8 *FormattedString = UTF8_FormatString(U8("%s, By using this software, you agree to the End User License Agreement %s, available at: %s%s"), Description, LicenseDescription, LicenseURL, NewLineUTF8);
                UTF8_WriteString2File(FormattedString, stdout);
            }
        } else {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        }
    }
    
    static inline void CommandLineIO_UTF16_DisplayBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF16 *Name               = UTF16_Encode(CLI->ProgramName);
            UTF16 *Version            = UTF16_Encode(CLI->ProgramVersion);
            UTF16 *Author             = UTF16_Encode(CLI->ProgramAuthor);
            UTF16 *Copyright          = UTF16_Encode(CLI->ProgramCopyright);
            UTF16 *Description        = UTF16_Encode(CLI->ProgramDescription);
            UTF16 *LicenseName        = UTF16_Encode(CLI->ProgramLicenseName);
            UTF16 *LicenseDescription = UTF16_Encode(CLI->ProgramLicenseDescription);
            UTF16 *LicenseURL         = UTF16_Encode(CLI->ProgramLicenseURL);
            
            if (Name != NULL && Version != NULL && Author != NULL && Copyright != NULL) {
                UTF16 *FormattedString = UTF16_FormatString(U16("%s, v. %s by %s © %s%s"), Name, Version, Author, Copyright, NewLineUTF16);
                UTF16_WriteString2File(FormattedString, stdout);
            }
            if (CLI->IsProprietary == No && Description != NULL && LicenseName != NULL && LicenseDescription != NULL && LicenseURL != NULL) {
                UTF16 *FormattedString = UTF16_FormatString(U16("%s, Released under the \"%s\" %s, available at: %s%s"), Description, LicenseName, LicenseDescription, LicenseURL, NewLineUTF16);
                UTF16_WriteString2File(FormattedString, stdout);
            } else if (CLI->IsProprietary == Yes && Description != NULL && LicenseDescription != NULL && LicenseURL != NULL) {
                UTF16 *FormattedString = UTF16_FormatString(U16("%s, By using this software, you agree to the End User License Agreement %s, available at: %s%s"), Description, LicenseDescription, LicenseURL, NewLineUTF16);
                UTF16_WriteString2File(FormattedString, stdout);
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
                Log(Log_DEBUG, __func__, U8("ArgumentString is not an Argument string"));
            }
            uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
            ArgumentSwitch                 = calloc(ArgumentSwitchSize + NULLTerminatorSize, sizeof(UTF32));
            for (uint64_t SwitchByte = ArgumentStringPrefixSize; SwitchByte < ArgumentSwitchSize; SwitchByte++) {
                ArgumentString[SwitchByte - ArgumentStringPrefixSize] = ArgumentString[SwitchByte];
            }
        } else {
            Log(Log_ERROR, __func__, U8("String Pointer is NULL"));
        }
        return ArgumentSwitch;
    }
    
    static void CommandLineIO_UTF32_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF32 **Arguments) {
        if (CLI != NULL && CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions) {
            /* Ok, so the first thing we need to do is loop over the arguments, and loop over the actual bytes of each argument */
            int32_t  CurrentArgument      = 1LL;
            do {
                // Extract the first argument as a switch.
                UTF32   *Argument         = Arguments[CurrentArgument];
                UTF32   *ArgumentFlag     = ArgumentString2SwitchFlag(Argument);
                // now loop over the switches
                for (int64_t Switch = 0LL; Switch < CLI->NumSwitches - 1; Switch++) {
                    // now compare ArgumentFlag to Switch
                    // Which string is smaller?
                    if (UTF32_Compare(ArgumentFlag, CLI->SwitchIDs[Switch].Name) == Yes) { // ArgumentFlag matches this switch
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
                                if (UTF32_Compare(PotentialSlaveArg, PotentialSlaveFlag) == Yes) {
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
            Log(Log_ERROR, __func__, U8("You entered %lld options, the minimum is %lld"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    void CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF8 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            // Basically now we just loop over the arguments, decode, normalize, and casefold them.
            UTF32 **Arguments32     = calloc(NumArguments, sizeof(UTF32*));
            for (int64_t Arg = 0LL; Arg < NumArguments; Arg++) {
                UTF32 *Decoded      = UTF8_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFoldString(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_NormalizeString(CaseFolded, NormalizationFormKC);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, Arguments32);
            for (int64_t Arg = 0LL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions <= 1 || NumArguments <= 1) {
            Log(Log_ERROR, __func__, U8("You entered %lld options, the minimum is %lld"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    void CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, const int64_t NumArguments, UTF16 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32     = calloc(NumArguments, sizeof(UTF32*));
            for (int64_t Arg = 0LL; Arg < NumArguments; Arg++) {
                UTF32 *Decoded      = UTF16_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFoldString(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_NormalizeString(CaseFolded, NormalizationFormKC);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, Arguments32);
            for (int64_t Arg = 0LL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Log_ERROR, __func__, U8("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions <= 1 || NumArguments <= 1) {
            Log(Log_ERROR, __func__, U8("You entered %lld options, the minimum is %lld"), NumArguments - 1, CLI->MinOptions);
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
            Extension = calloc(UnicodeBOMSizeInCodePoints + ExtensionSize + NULLTerminator , sizeof(UTF32));
            if (Extension != NULL) {
                for (uint64_t ExtCodePoint = 0ULL; ExtCodePoint < ExtensionSize; ExtCodePoint++) {
                    for (uint64_t PathCodePoint = StringSize - ExtensionSize; PathCodePoint < StringSize; PathCodePoint++) {
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
    
#ifdef __cplusplus
}
#endif
