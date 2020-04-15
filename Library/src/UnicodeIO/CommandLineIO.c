#include "../../include/Macros.h"
#include "../../include/MathIO.h"                   /* Included for Logarithm */
#include "../../include/UnicodeIO/CommandLineIO.h"  /* Included for our declarations */
#include "../../include/UnicodeIO/FormatIO.h"       /* Included for Formatter */
#include "../../include/UnicodeIO/LogIO.h"          /* Included for Logging */
#include "../../include/UnicodeIO/StringIO.h"       /* Included for StringIO's declarations */

#if (((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS) && (((FoundationIOTargetOS & FoundationIOLinuxOS) != FoundationIOLinuxOS)))
#include <signal.h>                    /* Included for SIGWINCH handling */
#include <sys/ioctl.h>                 /* Included for the terminal size */
#include <sys/ttycom.h>                /* Included for winsize, TIOCGWINSZ */
#elif (((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS) && (((FoundationIOTargetOS & FoundationIOLinuxOS) == FoundationIOLinuxOS)))
#include <signal.h>                    /* Included for SIGWINCH handling */
#include <sys/ioctl.h>                 /* Included for the terminal size */
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#include <wincon.h>                    /* Included for getting the terminal size */
#endif

#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
extern "C" {
#endif
    
    /*
     Automatic Option Shortening (-Input = -I or -In)
     
     Automatic Option Collapsing (-abcd = -a -b -c -d)
     
     Incompatible Options (-Interlace vs -No-Interlace)
     */
    
    typedef struct CommandLineOption {
        UTF32                        *Name;
        UTF32                        *Description;
        UTF32                        *Argument;
        uint64_t                     *IncompatibleOptions;
        uint64_t                     *Slaves;
        uint64_t                      OptionID;
        uint64_t                      NumIncompatibleOptions;
        uint64_t                      NumOptionSlaves;
        CommandLineIO_ArgumentTypes   ArgumentType;
        CommandLineIO_OptionTypes     OptionType;
        CommandLineIO_OptionStatuses  Status;
    } CommandLineOption;
    
    typedef struct CommandLineIO {
        UTF32                      *ProgramName;
        UTF32                      *ProgramAuthor;
        UTF32                      *ProgramDescription;
        UTF32                      *ProgramVersion;
        UTF32                      *ProgramCopyright;
        UTF32                      *ProgramLicenseName;
        UTF32                      *ProgramLicenseDescription;
        UTF32                      *ProgramLicenseURL;
        CommandLineOption          *OptionIDs;
        uint64_t                    NumOptions;
        uint64_t                    MinOptions;
        uint64_t                    HelpOption;
        CommandLineIO_LicenseTypes  LicenseType;
    } CommandLineIO;
    
    CommandLineIO *CommandLineIO_Init(uint64_t NumOptions) {
        CommandLineIO *CLI       = NULL;
        CLI                      = (CommandLineIO*) calloc(1, sizeof(CommandLineIO));
        if (CLI != NULL) {
            CLI->OptionIDs       = (CommandLineOption*) calloc(NumOptions, sizeof(CommandLineOption));
            if (CLI->OptionIDs != NULL) {
                CLI->NumOptions = NumOptions;
            } else {
                CommandLineIO_Deinit(CLI);
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate %llu Options"), NumOptions);
            }
        } else {
            CommandLineIO_Deinit(CLI);
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate CommandLineIO"));
        }
        return CLI;
    }
    
    uint64_t       CommandLineIO_GetTerminalWidth(void) {
        uint64_t Width = 0ULL;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Width          = WindowSize.ws_row;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Width          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Width;
    }
    
    uint64_t       CommandLineIO_GetTerminalHeight(void) {
        uint64_t Height = 0ULL;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Height          = WindowSize.ws_row;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Height          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Height;
    }
    
    bool           CommandLineIO_TerminalWasResized(void) {
        bool SizeChanged = No;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
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
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        SizeChanged      = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return SizeChanged;
    }
    
    void CommandLineIO_SetName(CommandLineIO *CLI, UTF32 *Name) {
        if (CLI != NULL && Name != NULL) {
            UTF32 *Normalized    = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded    = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramName     = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("The Name string is NULL"));
        }
    }
    
    void CommandLineIO_SetVersion(CommandLineIO *CLI, UTF32 *Version) {
        if (CLI != NULL && Version != NULL) {
            UTF32 *Normalized    = UTF32_Normalize(Version, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded    = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramVersion  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Version == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("The Version String is NULL"));
        }
    }
    
    void CommandLineIO_SetDescription(CommandLineIO *CLI, UTF32 *Description) {
        if (CLI != NULL && Description != NULL) {
            UTF32 *Normalized        = UTF32_Normalize(Description, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded        = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramDescription  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("The Description String is NULL"));
        }
    }
    
    void CommandLineIO_SetAuthor(CommandLineIO *CLI, UTF32 *Author) {
        if (CLI != NULL && Author != NULL) {
            UTF32 *Normalized   = UTF32_Normalize(Author, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded   = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramAuthor  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Author == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("The Author String is NULL"));
        }
    }
    
    void CommandLineIO_SetCopyright(CommandLineIO *CLI, UTF32 *Copyright) {
        if (CLI != NULL && Copyright != NULL) {
            UTF32 *Normalized     = UTF32_Normalize(Copyright, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded     = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramCopyright = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Copyright == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("The Copyright String is NULL"));
        }
    }
    
    void CommandLineIO_SetLicense(CommandLineIO *CLI, CommandLineIO_LicenseTypes LicenseType, UTF32 *Name, UTF32 *LicenseURL) {
        if (CLI != NULL && LicenseType != LicenseType_Unknown && Name != NULL && LicenseURL != NULL) {
            CLI->LicenseType                = LicenseType;
            UTF32 *NormalizedName           = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFoldedName           = UTF32_CaseFold(NormalizedName);
            free(NormalizedName);
            CLI->ProgramLicenseName         = CaseFoldedName;
            
            UTF32 *NormalizedLicenseURL     = UTF32_Normalize(LicenseURL, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFoldedLicenseURL     = UTF32_CaseFold(NormalizedLicenseURL);
            free(NormalizedLicenseURL);
            CLI->ProgramLicenseURL          = CaseFoldedLicenseURL;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (LicenseType == LicenseType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("The LicenseType Is Invalid"));
        } else if (Name == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("License Name Pointer is Invalid"));
        } else if (LicenseURL == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("The License URL must be set"));
        }
    }
    
    void CommandLineIO_SetMinOptions(CommandLineIO *CLI, uint64_t MinOptions) {
        if (CLI != NULL && MinOptions < CLI->NumOptions) {
            CLI->MinOptions = MinOptions;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (MinOptions > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("MinOptions %lld is invalid, it should be between 0 and %lld"), MinOptions, CLI->NumOptions);
        }
    }
    
    void CommandLineIO_SetHelpOption(CommandLineIO *CLI, uint64_t HelpOption) {
        if (CLI != NULL && HelpOption < CLI->NumOptions) {
            CLI->HelpOption = HelpOption;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (HelpOption > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("HelpSwitch %lld is invalid, it should be between 0 and %lld"), HelpOption, CLI->NumOptions);
        }
    }
    
    void CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, UTF32 **Strings, uint64_t *Numerator, uint64_t *Denominator) {
        if (CLI != NULL) {
            uint64_t *StringSize = (uint64_t*) calloc(NumItems2Display + FoundationIONULLTerminatorSize, sizeof(uint64_t));
            for (uint8_t Item = 0; Item < NumItems2Display; Item++) {
                StringSize[Item] = UTF32_GetStringSizeInCodePoints(Strings[Item]);
            }
            
            uint64_t *NumProgressIndicatorsPerString = (uint64_t*) calloc(NumItems2Display, sizeof(uint64_t));
            UTF8     *ActualStrings2Print            = (UTF8*) calloc(NumItems2Display, sizeof(UTF8));
            for (uint8_t String = 0; String < NumItems2Display; String++) {
                NumProgressIndicatorsPerString[String] = CommandLineIO_GetTerminalWidth() - (2 + StringSize[String]);
                uint64_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
                UTF8    *Indicator           = UTF8_Init(CommandLineIO_GetTerminalWidth());
                UTF8    *IndicatorFinal      = UTF8_Insert(Indicator, UTF8String("-"), 0);
                UTF8    *FormattedString     = UTF8_Format(UTF8String("[%s%Us %llu/%llu %llu/%s%s]"), IndicatorFinal, *Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, FoundationIONewLine8);
                UTF8_WriteLine(stdout, FormattedString);
                free(Indicator);
            }
            free(StringSize);
            free(NumProgressIndicatorsPerString);
            free(ActualStrings2Print);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    void CommandLineIO_Option_SetName(CommandLineIO *CLI, uint64_t OptionID, UTF32 *Name) {
        if (CLI != NULL && OptionID < CLI->NumOptions && Name != NULL) {
            UTF32 *Normalized             = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded             = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->OptionIDs[OptionID].Name = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Name String is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumOptions - 1);
        }
    }
    
    void CommandLineIO_Option_SetDescription(CommandLineIO *CLI, uint64_t OptionID, UTF32 *Description) {
        if (CLI != NULL && Description != NULL && OptionID < CLI->NumOptions) {
            UTF32 *Normalized                    = UTF32_Normalize(Description, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded                    = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->OptionIDs[OptionID].Description = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Description String is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumOptions - 1);
        }
    }
    
    void CommandLineIO_Option_SetSlave(CommandLineIO *CLI, uint64_t MasterID, uint64_t SlaveID) {
        if (CLI != NULL && MasterID < CLI->NumOptions - 1 && SlaveID < CLI->NumOptions - 1) {
            if (CLI->OptionIDs[MasterID].OptionType == OptionType_PotentialSlaves) {
                CLI->OptionIDs[MasterID].NumOptionSlaves  += 1;
                uint64_t NumSlaves                         = CLI->OptionIDs[MasterID].NumOptionSlaves - 1;
                CLI->OptionIDs[MasterID].Slaves            = (uint64_t*) realloc(CLI->OptionIDs[MasterID].Slaves, CLI->OptionIDs[MasterID].NumOptionSlaves * sizeof(uint64_t));
                CLI->OptionIDs[MasterID].Slaves[NumSlaves] = SlaveID;
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("MasterID %lld can not have any slaves"), MasterID);
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (MasterID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("MasterID %lld is invalid, it should be between 0 and %lld"), MasterID, CLI->NumOptions - 1);
        } else if (SlaveID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("SlaveID %lld is invalid, it should be between 0 and %lld"), SlaveID, CLI->NumOptions - 1);
        }
    }
    
    void CommandLineIO_Option_SetType(CommandLineIO *CLI, uint64_t OptionID, CommandLineIO_OptionTypes OptionType) {
        if (CLI != NULL && OptionType != OptionType_Unknown && OptionID < CLI->NumOptions - 1) {
            CLI->OptionIDs[OptionID].OptionType = OptionType;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionType == OptionType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("You can not set OptionID %lld to OptionType_Unknown"), OptionID);
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumOptions - 1);
        }
    }
    
    void CommandLineIO_Option_SetStatus(CommandLineIO *CLI, uint64_t OptionID, CommandLineIO_OptionStatuses Status) {
        if (CLI != NULL && Status != OptionStatus_Unknown) {
            if (OptionID < CLI->NumOptions) {
                CLI->OptionIDs[OptionID].Status = Status;
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OptionID %llu is invalid"), OptionID);
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Status == OptionStatus_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Status is Unknown"));
        }
    }
    
    void CommandLineIO_Option_SetArgumentType(CommandLineIO *CLI, uint64_t OptionID, CommandLineIO_ArgumentTypes ArgumentType) {
        if (CLI != NULL && ArgumentType != ArgumentType_Unknown && OptionID < CLI->NumOptions) {
            CLI->OptionIDs[OptionID].ArgumentType = ArgumentType;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (ArgumentType == ArgumentType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("You can not set OptionID %lld to ArgumentType_Unknown"), OptionID);
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumOptions - 1);
        }
    }
    
    static void CommandLineIO_DisplayHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name = UTF8_Encode(CLI->ProgramName);
            
            UTF8 *ProgramsOptions = UTF8_Format(UTF8String("%s's Options (-|--|/):%s"), Name, FoundationIONewLine8);
            UTF8_WriteLine(stdout, ProgramsOptions);
            free(Name);
            free(ProgramsOptions);
            for (uint64_t Switch = 0ULL; Switch < CLI->NumOptions; Switch++) {
                CommandLineIO_OptionTypes CurrentSwitchType = CLI->OptionIDs[Switch].OptionType;
                
                UTF8 *SwitchName        = UTF8_Encode(CLI->OptionIDs[Switch].Name);
                UTF8 *SwitchDescription = UTF8_Encode(CLI->OptionIDs[Switch].Description);
                
                UTF8 *SwitchInfo        = UTF8_Format(UTF8String("%s: %s%s"), SwitchName, SwitchDescription, FoundationIONewLine8);
                
                UTF8_WriteLine(stdout, SwitchInfo);
                if (CurrentSwitchType == OptionType_PotentialSlaves && CLI->OptionIDs[Switch].NumOptionSlaves > 0) {
                    for (uint64_t SlaveSwitch = 0ULL; SlaveSwitch < CLI->OptionIDs[Switch].NumOptionSlaves; SlaveSwitch++) {
                        UTF8 *SlaveName        = UTF8_Encode(CLI->OptionIDs[SlaveSwitch].Name);
                        UTF8 *SlaveDescription = UTF8_Encode(CLI->OptionIDs[SlaveSwitch].Description);
                        
                        UTF8 *SlaveSwitchInfo  = UTF8_Format(UTF8String("\t%s: %s%s"), SlaveName, SlaveDescription, FoundationIONewLine8);
                        
                        UTF8_WriteLine(stdout, SlaveSwitchInfo);
                        
                        free(SlaveName);
                        free(SlaveDescription);
                        free(SlaveSwitchInfo);
                    }
                }
                free(SwitchName);
                free(SwitchDescription);
                free(SwitchInfo);
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    static void CommandLineIO_ShowBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name                  = UTF8_Encode(CLI->ProgramName);
            UTF8 *Version               = UTF8_Encode(CLI->ProgramVersion);
            UTF8 *Author                = UTF8_Encode(CLI->ProgramAuthor);
            UTF8 *Copyright             = UTF8_Encode(CLI->ProgramCopyright);
            UTF8 *Description           = UTF8_Encode(CLI->ProgramDescription);
            UTF8 *LicenseName           = UTF8_Encode(CLI->ProgramLicenseName);
            UTF8 *LicenseDescription    = UTF8_Encode(CLI->ProgramLicenseDescription);
            UTF8 *LicenseURL            = UTF8_Encode(CLI->ProgramLicenseURL);
            CommandLineIO_LicenseTypes LicenseType = CLI->LicenseType;
            
            if (Name != NULL) {
                UTF8 *NameString      = UTF8_Format(UTF8String("%s,"), Name);
                UTF8_WriteLine(stdout, NameString);
                free(NameString);
                
                UTF8 *FormattedString = UTF8_Format(UTF8String(" %s, v. %s by %s © %s%s"), Name, Version, Author, Copyright, FoundationIONewLine8);
                UTF8_WriteLine(stdout, FormattedString);
            }
            
            if (Version != NULL) {
                UTF8 *VersionString   = UTF8_Format(UTF8String(" v. %s"), Version);
                UTF8_WriteLine(stdout, VersionString);
            }
            
            if (Author != NULL) {
                UTF8 *AuthorString   = UTF8_Format(UTF8String(" by %s"), Author);
                UTF8_WriteLine(stdout, AuthorString);
            }
            
            if (Copyright != NULL) {
                UTF8 *CopyrightString   = UTF8_Format(UTF8String(" © %s\n"), Copyright);
                UTF8_WriteLine(stdout, CopyrightString);
            }
            
            if (LicenseType == LicenseType_Permissive || LicenseType == LicenseType_Copyleft) {
                if (Description != NULL) {
                    UTF8 *DescriptionString   = UTF8_Format(UTF8String("%s"), Description);
                    UTF8_WriteLine(stdout, DescriptionString);
                }
                
                if (LicenseName != NULL) {
                    UTF8 *LicenseNameString   = UTF8_Format(UTF8String(" Released under the \"%s\" license"), LicenseName);
                    UTF8_WriteLine(stdout, LicenseNameString);
                }
                
                if (LicenseDescription != NULL) {
                    UTF8 *LicenseDescriptionString   = UTF8_Format(UTF8String(" %s,"), LicenseDescription);
                    UTF8_WriteLine(stdout, LicenseDescriptionString);
                }
                
                if (LicenseURL != NULL) {
                    UTF8 *LicenseURLString   = UTF8_Format(UTF8String(" %s\n"), LicenseURL);
                    UTF8_WriteLine(stdout, LicenseURLString);
                }
            } else if (LicenseType == LicenseType_Proprietary) {
                if (Description != NULL) {
                    UTF8 *DescriptionString   = UTF8_Format(UTF8String("%s"), Description);
                    UTF8_WriteLine(stdout, DescriptionString);
                }
                
                if (LicenseDescription != NULL) {
                    UTF8 *LicenseDescriptionString   = UTF8_Format(UTF8String(" By using this software, you agree to the End User License Agreement %s,"), LicenseDescription);
                    UTF8_WriteLine(stdout, LicenseDescriptionString);
                }
                
                if (LicenseURL != NULL) {
                    UTF8 *LicenseURLString   = UTF8_Format(UTF8String(" available at: %s\n"), LicenseURL);
                    UTF8_WriteLine(stdout, LicenseURLString);
                }
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    static UTF32 *ArgumentString2OptionFlag(UTF32 *ArgumentString) {
        UTF32 *ArgumentSwitch = NULL;
        if (ArgumentString != NULL) {
            uint8_t  ArgumentStringPrefixSize = 0;
            uint64_t ArgumentStringSize       = UTF32_GetStringSizeInCodePoints(ArgumentString);
            
            if (ArgumentStringSize >= 2) {
                if (ArgumentString[0] == UTF32Character('-') && ArgumentString[1] == UTF32Character('-')) {
                    ArgumentStringPrefixSize  = 2;
                } else if (ArgumentString[0] == UTF32Character('/') || ArgumentString[0] == UTF32Character('\\') || ArgumentString[0] == UTF32Character('-')) {
                    ArgumentStringPrefixSize  = 1;
                }
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("ArgumentString is not an Argument string"));
            }
            uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
            ArgumentSwitch                 = UTF32_ExtractSubString(UTF32_CaseFold(ArgumentString), ArgumentStringPrefixSize, ArgumentSwitchSize);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return ArgumentSwitch;
    }
    
    uint64_t CommandLineIO_GetNumArguments(int argc) {
        uint64_t NumArguments = 0ULL;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
        NumArguments          = (uint64_t) argc;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        NumArguments          = (uint64_t) __argc;
#endif
        return NumArguments;
    }
    
    UTF32 **CommandLineIO_GetArgumentArray(void **Arguments) {
        UTF32 **ArgumentArray           = NULL;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
        ArgumentArray                   = (UTF32**) UTF8_StringArray_Decode((UTF8**) Arguments);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        uint64_t NumArguments           = (uint64_t) __argc;
        ArgumentArray                   = UTF32_StringArray_Init(NumArguments);
        if (ArgumentArray != NULL) {
            for (uint64_t Argument = 0ULL; Argument < NumArguments; Argument++) {
                ArgumentArray[Argument] = UTF16_Decode((UTF16*) Arguments);
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldnt allocate %llu arguments"), NumArguments);
        }
#endif
        return ArgumentArray;
    }
    
    static void CommandLineIO_UTF32_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, UTF32 **Arguments) {
        if (CLI != NULL && CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions) {
            uint64_t     CurrentArgument  = 1LL;
            do {
                UTF32   *Argument         = Arguments[CurrentArgument];
                UTF32   *ArgumentFlag     = ArgumentString2OptionFlag(Argument);
                
                for (uint64_t Switch = 0ULL; Switch < CLI->NumOptions; Switch++) {
                    if (UTF32_CompareSubString(ArgumentFlag, CLI->OptionIDs[Switch].Name, 0, 0) == Yes) {
                        
                        CLI->NumOptions   += 1;
                        if (CLI->NumOptions == 1) {
                            CLI->OptionIDs = (CommandLineOption*) calloc(1, sizeof(CommandLineOption));
                        } else {
                            CLI->OptionIDs = (CommandLineOption*) realloc(CLI->OptionIDs, CLI->NumOptions * sizeof(CommandLineOption));
                        }
                        CLI->OptionIDs[CLI->NumOptions - 1].OptionID = Switch;
                        
                        for (uint64_t ArgSlave = 1ULL; ArgSlave < CLI->OptionIDs[Switch].NumOptionSlaves; ArgSlave++) {
                            UTF32 *PotentialSlaveArg        = Arguments[CurrentArgument + ArgSlave];
                            for (uint64_t Slave = 0ULL; Slave < CLI->OptionIDs[Switch].NumOptionSlaves; Slave++) {
                                UTF32 *PotentialSlaveFlag   = CLI->OptionIDs[CLI->OptionIDs[Switch].Slaves[Slave]].Name;
                                
                                if (UTF32_CompareSubString(PotentialSlaveArg, PotentialSlaveFlag, 0, 0) == Yes) {
                                    
                                    CLI->OptionIDs[CLI->NumOptions - 1].NumOptionSlaves += 1;
                                    CLI->OptionIDs[CLI->NumOptions - 1].Slaves[CLI->OptionIDs[CLI->NumOptions - 1].NumOptionSlaves - 1] = CLI->OptionIDs[Switch].Slaves[Slave];
                                }
                                free(PotentialSlaveFlag);
                            }
                            free(PotentialSlaveArg);
                        }
                    } else {
                        // Tell the user that their string wasn't found as a valid argument, or suggest a similar one, or support argument compaction automtically.
                    }
                }
                free(Argument);
                free(ArgumentFlag);
            } while (CurrentArgument < NumArguments);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("You entered %lld options, the minimum is %lld"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    void CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, UTF8 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32     = UTF32_StringArray_Init(NumArguments);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                UTF32 *Decoded      = UTF8_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFold(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_Normalize(CaseFolded, NormalizationForm_KompatibleCompose);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, Arguments32);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            CommandLineIO_DisplayHelp(CLI);
        }
    }
    
    void CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, UTF16 **Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32     = UTF32_StringArray_Init(NumArguments);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                UTF32 *Decoded      = UTF16_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFold(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_Normalize(CaseFolded, NormalizationForm_KompatibleCompose);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, Arguments32);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            CommandLineIO_DisplayHelp(CLI);
        }
    }
    
    uint64_t CommandLineIO_GetNumMatchingOptions(CommandLineIO *CLI, uint64_t OptionID, uint64_t NumSlaves, uint64_t *SlaveIDs) {
        uint64_t NumMatchingOptions = 0LL;
        if (CLI != NULL && OptionID <= CLI->NumOptions - 1 && NumSlaves < CLI->NumOptions) {
            for (uint64_t Option = 0ULL; Option <= CLI->NumOptions; Option++) {
                if (CLI->OptionIDs[Option].OptionID == OptionID) {
                    if (NumSlaves == 0) {
                        NumMatchingOptions               += 1;
                    } else if (NumSlaves > 0 && CLI->OptionIDs[Option].NumOptionSlaves > 0) {
                        for (uint64_t ParamSlave = 0ULL; ParamSlave < NumSlaves; ParamSlave++) {
                            for (uint64_t OptionSlave = 0ULL; OptionSlave < CLI->OptionIDs[Option].NumOptionSlaves; OptionSlave++) {
                                if (SlaveIDs[ParamSlave] == CLI->OptionIDs[Option].Slaves[OptionSlave]) {
                                    NumMatchingOptions   += 1;
                                }
                            }
                        }
                    }
                }
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumOptions - 1);
        } else if (NumSlaves > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("NumSlaves %lld is invalid, it should be between 0 and %lld"), NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        }
        return NumMatchingOptions;
    }
    
    uint64_t CommandLineIO_GetOptionNum(CommandLineIO *CLI, uint64_t OptionID, uint64_t NumSlaves, uint64_t *SlaveIDs) {
        uint64_t MatchingOption = -1LL;
        if (CLI != NULL && OptionID <= CLI->NumOptions - 1 && NumSlaves <= CLI->NumOptions - 1) {
            bool AllOptionsMatch   = No;
            for (uint64_t Option = 0ULL; Option < CLI->NumOptions; Option++) {
                if (CLI->OptionIDs[Option].OptionID == OptionID) {
                    if (NumSlaves == 0 && CLI->OptionIDs[Option].NumOptionSlaves == 0) {
                        AllOptionsMatch       = Yes;
                        MatchingOption        = Option;
                    } else {
                        for (uint64_t ParamSlave = 0ULL; ParamSlave < NumSlaves; ParamSlave++) {
                            for (uint64_t OptionSlave = 0ULL; OptionSlave < CLI->OptionIDs[Option].NumOptionSlaves; OptionSlave++) {
                                if (SlaveIDs[ParamSlave] == CLI->OptionIDs[Option].Slaves[OptionSlave]) {
                                    AllOptionsMatch       = Yes;
                                    MatchingOption        = Option;
                                }
                            }
                        }
                    }
                }
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), OptionID, CLI->NumOptions - 1);
        } else if (NumSlaves > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("NumSlaves %lld is invalid, it should be between 0 and %lld"), NumSlaves, CLI->OptionIDs[OptionID].NumOptionSlaves - 1);
        }
        return MatchingOption;
    }
    
    UTF8 *CommandLineIO_UTF8_GetOptionResult(CommandLineIO *CLI, uint64_t OptionID) {
        UTF8 *Result = NULL;
        if (CLI != NULL && OptionID <= CLI->NumOptions - 1) {
            Result = UTF8_Encode(CLI->OptionIDs[OptionID].Argument);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Option %lld is outside the range 0 - %lld"), OptionID, CLI->NumOptions - 1);
        }
        return Result;
    }
    
    UTF16 *CommandLineIO_UTF16_GetOptionResult(CommandLineIO *CLI, uint64_t OptionID) {
        UTF16 *Result = NULL;
        if (CLI != NULL && OptionID <= CLI->NumOptions - 1) {
            Result = UTF16_Encode(CLI->OptionIDs[OptionID].Argument);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Option %lld is outside the range 0 - %lld"), OptionID, CLI->NumOptions - 1);
        }
        return Result;
    }
    
    static UTF32 *CommandLineIO_UTF32_GetExtension(UTF32 *Path) {
        UTF32 *Extension = NULL;
        if (Path != NULL) {
            uint64_t StringSize    = UTF32_GetStringSizeInCodePoints(Path);
            uint64_t ExtensionSize = 0ULL;
            for (uint64_t CodePoint = StringSize; CodePoint > 0; CodePoint--) {
                if (Path[CodePoint] == UTF32Character('.')) {
                    ExtensionSize  = CodePoint - StringSize;
                }
            }
            Extension              = UTF32_Init(ExtensionSize);
            if (Extension != NULL) {
                for (uint64_t ExtCodePoint = 0ULL; ExtCodePoint < ExtensionSize; ExtCodePoint++) {
                    for (uint64_t PathCodePoint = StringSize - ExtensionSize; PathCodePoint < StringSize; PathCodePoint++) {
                        Extension[ExtCodePoint] = Path[PathCodePoint];
                    }
                }
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate %lld codepoints for the Extension"), ExtensionSize);
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Path Pointer is NULL"));
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
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Path Pointer is NULL"));
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
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Path Pointer is NULL"));
        }
        return Extension;
    }
    
    UTF8 *CommandLineIO_UTF8_Colorize(UTF8 *String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF8 *Colorized    = NULL;
        UTF32 *String32    = UTF8_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF8_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF16 *CommandLineIO_UTF16_Colorize(UTF16 *String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF16 *Colorized   = NULL;
        UTF32 *String32    = UTF16_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF16_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF32 *CommandLineIO_UTF32_Colorize(UTF32 *String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF32 *Colorized = NULL;
        uint8_t DigitSize = 0;
        DigitSize       += Logarithm(10, Red);
        DigitSize       += Logarithm(10, Green);
        DigitSize       += Logarithm(10, Blue);
        UTF32 *IntegerR  = UTF32_Integer2String(Base_Integer | Base_Radix10, Red);
        UTF32 *IntegerG  = UTF32_Integer2String(Base_Integer | Base_Radix10, Green);
        UTF32 *IntegerB  = UTF32_Integer2String(Base_Integer | Base_Radix10, Blue);
        DigitSize       += 8;
        if ((ColorType & ColorType_Foreground) == ColorType_Foreground) {
            UTF32 *Formatted = UTF32_Format(UTF32String("%c[38;2;%d;%d;%d;m"), UTF32Character('\x1B'), IntegerR, IntegerG, IntegerB);
            Colorized        = UTF32_Insert(String, Formatted, 0);
            UTF32_Deinit(Formatted);
        } else if ((ColorType & ColorType_Background) == ColorType_Background) {
            UTF32 *Formatted = UTF32_Format(UTF32String("%c[48;2;%d;%d;%dm"), UTF32Character('\x1B'), IntegerR, IntegerG, IntegerB);
            Colorized        = UTF32_Insert(String, Formatted, 0);
            UTF32_Deinit(Formatted);
        }
        UTF32_Deinit(IntegerR);
        UTF32_Deinit(IntegerG);
        UTF32_Deinit(IntegerB);
        return Colorized;
    }
    
    UTF8 *CommandLineIO_UTF8_Decolorize(UTF8 *String) {
        UTF8  *Decolorized   = NULL;
        UTF32 *String32      = UTF8_Decode(String);
        UTF32 *Decolorized32 = CommandLineIO_UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF8_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF16 *CommandLineIO_UTF16_Decolorize(UTF16 *String) {
        UTF16 *Decolorized   = NULL;
        UTF32 *String32      = UTF16_Decode(String);
        UTF32 *Decolorized32 = CommandLineIO_UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF16_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF32 *CommandLineIO_UTF32_Decolorize(UTF32 *String) {
        UTF32    *Decolorized          = NULL;
        uint64_t  EscapeSequenceSize   = 0;
        uint64_t  EscapeSequenceOffset = 0;
        uint64_t  CodePoint            = 0;
        while (String[CodePoint] != FoundationIONULLTerminator) {
            if (String[CodePoint] == UTF32Character('\x1B')) {
                EscapeSequenceOffset   = CodePoint;
                EscapeSequenceSize    += 1;
                CodePoint             += 1;
            } else if (String[CodePoint] == UTF32Character('m')) {
                EscapeSequenceSize    += 1;
                CodePoint             += 1;
            } else {
                CodePoint             += 1;
            }
        }
        Decolorized                    = UTF32_StitchSubString(String, EscapeSequenceOffset, EscapeSequenceSize);
        return Decolorized;
    }
    
    void CommandLineIO_Deinit(CommandLineIO *CLI) {
        if (CLI != NULL) {
            for (uint64_t Option = 0ULL; Option < CLI->NumOptions; Option++) {
                free(CLI->OptionIDs[Option].Name);
                free(CLI->OptionIDs[Option].Description);
                free(CLI->OptionIDs[Option].Slaves);
                free(CLI->OptionIDs[Option].Argument);
            }
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
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
}
#endif
