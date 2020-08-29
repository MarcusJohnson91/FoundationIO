#include "../../include/TextIO/CommandLineIO.h" /* Included for our declarations */
#include "../../include/MathIO.h"               /* Included for Logarithm */
#include "../../include/TextIO/FormatIO.h"      /* Included for Formatter */
#include "../../include/TextIO/LogIO.h"         /* Included for Logging */
#include "../../include/TextIO/StringIO.h"      /* Included for StringIO's declarations */

#if (((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS) && (((PlatformIO_TargetOS & PlatformIO_LinuxOS) != PlatformIO_LinuxOS)))
#include <signal.h>                             /* Included for SIGWINCH handling */
#include <sys/ioctl.h>                          /* Included for the terminal size */
#include <sys/ttycom.h>                         /* Included for winsize, TIOCGWINSZ */
#elif (((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS) && (((PlatformIO_TargetOS & PlatformIO_LinuxOS) == PlatformIO_LinuxOS)))
#include <signal.h>                             /* Included for SIGWINCH handling */
#include <sys/ioctl.h>                          /* Included for the terminal size */
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
#include <wincon.h>                             /* Included for getting the terminal size */
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     Automatic Option Shortening (-Input = -I or -In)
     
     Automatic Option Collapsing (-abcd = -a -b -c -d)
     
     Incompatible Options (-Interlace vs -No-Interlace)
     */

    /*
     User walkthrough:

     I'm a user that's developing a program that does something

     so first I create an enum that contains all the switches a user can enter.

     then I'm creating an interface for CommandLineIO that describes their interactions.

     Switches X and Y are incompatible

     Switches A and B must be used together.

     Switches
     */
    
    typedef struct CommandLineOption {
        // ok so each option is based on a SwitchID, it may need to hold any used children, as well as argument strings
        // How do we handle bool strings, and ranges?
        // Maybe we should just have the user pass in the OptionID to a dedicated function that will handle BoolStrings, Ranges, etc?
        UTF32    *Argument;
        uint64_t  SwitchID;    // the number matching the switch that this option corresponds to
        uint64_t  NumChildren; // Number of active children for this option
        uint64_t *Children;
    } CommandLineOption;
    
    typedef struct CommandLineSwitch {
        UTF32                        *Name;
        UTF32                        *Description;
        UTF32                        *Argument;
        uint64_t                     *IncompatibleOptions;
        uint64_t                     *Children;
        uint64_t                      OptionID;
        uint64_t                      NumIncompatibleOptions;
        uint64_t                      NumChildren; // NumOptionSlaves
        CommandLineIO_SwitchArguments ArgumentType;
        CommandLineIO_SwitchTypes     SwitchType; // OptionType
        CommandLineIO_SwitchStatuses  Status;
    } CommandLineSwitch;
    
    typedef struct CommandLineIO {
        UTF32                     **Tokens;
        UTF32                      *ProgramName;
        UTF32                      *ProgramAuthor;
        UTF32                      *ProgramDescription;
        UTF32                      *ProgramVersion;
        UTF32                      *ProgramCopyright;
        UTF32                      *ProgramLicenseName;
        UTF32                      *ProgramLicenseDescription;
        UTF32                      *ProgramLicenseURL;
        CommandLineSwitch          *Switches; // OptionID -> Switches
        CommandLineOption          *Options;
        uint64_t                    NumSwitches;
        uint64_t                    NumOptions;
        uint64_t                    MinOptions;
        uint64_t                    HelpOption;
        CommandLineIO_LicenseTypes  LicenseType;
    } CommandLineIO;
    
    CommandLineIO *CommandLineIO_Init(uint64_t NumSwitches) {
        CommandLineIO *CLI           = NULL;
        if (NumSwitches > 0) {
            CLI                      = (CommandLineIO*) calloc(1, sizeof(CommandLineIO));
            if (CLI != NULL) {
                CLI->Switches        = (CommandLineSwitch*) calloc(NumSwitches, sizeof(CommandLineSwitch));
                if (CLI->Switches != NULL) {
                    CLI->NumSwitches = NumSwitches;
                } else {
                    CommandLineIO_Deinit(CLI);
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate %llu Options"), NumSwitches);
                }
            } else {
                CommandLineIO_Deinit(CLI);
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate CommandLineIO"));
            }
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("NumSwitches is invalid"));
        }
        return CLI;
    }
    
    uint64_t       CommandLineIO_GetTerminalWidth(void) {
        uint64_t Width = 0ULL;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Width          = WindowSize.ws_row;
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Width          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Width;
    }
    
    uint64_t       CommandLineIO_GetTerminalHeight(void) {
        uint64_t Height = 0ULL;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Height          = WindowSize.ws_row;
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Height          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Height;
    }
    
    bool CommandLineIO_TerminalWasResized(void) {
        bool SizeChanged = No;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
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
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        SizeChanged      = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return SizeChanged;
    }
    
    void CommandLineIO_SetName(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Name) {
        if (CLI != NULL && Name != NULL) {
            UTF32 *Normalized    = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded    = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramName     = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The Name string is NULL"));
        }
    }
    
    void CommandLineIO_SetVersion(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Version) {
        if (CLI != NULL && Version != NULL) {
            UTF32 *Normalized    = UTF32_Normalize(Version, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded    = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramVersion  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Version == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The Version String is NULL"));
        }
    }
    
    void CommandLineIO_SetDescription(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Description) {
        if (CLI != NULL && Description != NULL) {
            UTF32 *Normalized        = UTF32_Normalize(Description, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded        = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramDescription  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The Description String is NULL"));
        }
    }
    
    void CommandLineIO_SetAuthor(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Author) {
        if (CLI != NULL && Author != NULL) {
            UTF32 *Normalized   = UTF32_Normalize(Author, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded   = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramAuthor  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Author == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The Author String is NULL"));
        }
    }
    
    void CommandLineIO_SetCopyright(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Copyright) {
        if (CLI != NULL && Copyright != NULL) {
            UTF32 *Normalized     = UTF32_Normalize(Copyright, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded     = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->ProgramCopyright = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Copyright == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The Copyright String is NULL"));
        }
    }
    
    void CommandLineIO_SetLicense(CommandLineIO *CLI, CommandLineIO_LicenseTypes LicenseType, PlatformIO_Immutable(UTF32 *) Name, PlatformIO_Immutable(UTF32 *) LicenseURL) {
        if (CLI != NULL && LicenseType != LicenseType_Unspecified && Name != NULL && LicenseURL != NULL) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (LicenseType == LicenseType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The LicenseType Is Invalid"));
        } else if (Name == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("License Name Pointer is Invalid"));
        } else if (LicenseURL == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The License URL must be set"));
        }
    }
    
    void CommandLineIO_SetMinOptions(CommandLineIO *CLI, uint64_t MinOptions) {
        if (CLI != NULL && MinOptions < CLI->NumSwitches) {
            CLI->MinOptions = MinOptions;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (MinOptions > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("MinOptions %lld is invalid, it should be between 0 and %lld"), MinOptions, CLI->NumSwitches);
        }
    }
    
    void CommandLineIO_SetHelpOption(CommandLineIO *CLI, uint64_t HelpOption) {
        if (CLI != NULL && HelpOption < CLI->NumSwitches) {
            CLI->HelpOption = HelpOption;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (HelpOption > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("HelpSwitch %lld is invalid, it should be between 0 and %lld"), HelpOption, CLI->NumSwitches);
        }
    }
    
    void CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, PlatformIO_Immutable(UTF32 **) Strings, PlatformIO_Immutable(uint64_t *) Numerator, PlatformIO_Immutable(uint64_t *) Denominator) {
        if (CLI != NULL) {
            uint64_t *StringSize = (uint64_t*) calloc(NumItems2Display + PlatformIO_NULLTerminatorSize, sizeof(uint64_t));
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
                UTF8    *FormattedString     = UTF8_Format(UTF8String("[%s%Us %llu/%llu %llu/%s%s]"), IndicatorFinal, *Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, PlatformIO_NewLine8);
                UTF8_WriteSentence(stdout, FormattedString);
                free(Indicator);
            }
            free(StringSize);
            free(NumProgressIndicatorsPerString);
            free(ActualStrings2Print);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    static void CommandLineIO_ShowHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name = UTF8_Encode(CLI->ProgramName);
            
            UTF8 *ProgramsOptions = UTF8_Format(UTF8String("%s's Options (-|--|/):%s"), Name, PlatformIO_NewLine8);
            UTF8_WriteSentence(stdout, ProgramsOptions);
            free(Name);
            free(ProgramsOptions);
            
            uint64_t StringSetSize = 0;
            for (uint64_t Switch = 0; Switch < CLI->NumSwitches; Switch++) {
                if (CLI->Switches[Switch].SwitchType == SwitchType_Existential || CLI->Switches[Switch].SwitchType == SwitchType_Standalone) {
                    StringSetSize += 1;
                } else if (CLI->Switches[Switch].SwitchType == SwitchType_Parent) {
                    StringSetSize += CLI->Switches[Switch].NumChildren;
                }
            }
            
            UTF32 **GeneratedHelp = UTF32_StringSet_Init(StringSetSize);
            
            for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
                CommandLineIO_SwitchTypes CurrentSwitchType = CLI->Switches[Switch].SwitchType;
                GeneratedHelp[Switch]   = UTF32_Format(UTF32String("%U32s: %U32s%U32s"), CLI->Switches[Switch].Name, CLI->Switches[Switch].Description, PlatformIO_NewLine32);
                
                if (CurrentSwitchType == SwitchType_Parent && CLI->Switches[Switch].NumChildren > 0) {
                    for (uint64_t Child = 0ULL; Child < CLI->Switches[Switch].NumChildren; Child++) {
                        GeneratedHelp[Switch + Child] = UTF32_Format(UTF32String("\t%U32s: %U32s%U32s"), CLI->Switches[Child].Name, CLI->Switches[Child].Description, PlatformIO_NewLine32);
                    }
                }
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
                UTF8 **GeneratedHelp8 = UTF8_StringSet_Encode((PlatformIO_Immutable(UTF32**)) GeneratedHelp);
                for (uint64_t String = 0; String < StringSetSize; String++) {
                    UTF8_WriteSentence(stdout, GeneratedHelp8[String]);
                }
                UTF8_StringSet_Deinit(GeneratedHelp8);
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
                UTF16 **GeneratedHelp16 = UTF16_StringSet_Encode((PlatformIO_Immutable(UTF32 **)) GeneratedHelp);
                for (uint64_t String = 0; String < StringSetSize; String++) {
                    UTF16_WriteSentence(stdout, GeneratedHelp16[String]);
                }
                UTF16_StringSet_Deinit(GeneratedHelp16);
#endif
                UTF32_StringSet_Deinit(GeneratedHelp);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    static void CommandLineIO_ShowBanner(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF32 *License = NULL;
            if (CLI->LicenseType == LicenseType_Permissive || CLI->LicenseType == LicenseType_Copyleft) {
                License = UTF32_Format(UTF32String("Released under the \"%s\" license, you can see the details of this license at: %U32s"), CLI->ProgramLicenseName != NULL ? CLI->ProgramLicenseName : PlatformIO_InvisibleString32, CLI->ProgramLicenseURL != NULL ? CLI->ProgramLicenseURL : PlatformIO_InvisibleString32);
            } else if (CLI->LicenseType == LicenseType_Proprietary) {
                License = UTF32_Format(UTF32String("By using this software, you agree to the End User License Agreement:%U32s%U32s%U32s"), PlatformIO_NewLine32, PlatformIO_NewLine32, CLI->ProgramLicenseURL != NULL ? CLI->ProgramLicenseURL : PlatformIO_InvisibleString32);
            } else {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("LicenseType isn't set"));
            }
            
            UTF32 *Banner32 = UTF32_Format(UTF32String("%U32s, v. %U32s by %U32s © %U32s, %U32s, %U32s"),
                                           CLI->ProgramName        != NULL ? CLI->ProgramName        : PlatformIO_InvisibleString32,
                                           CLI->ProgramVersion     != NULL ? CLI->ProgramVersion     : PlatformIO_InvisibleString32,
                                           CLI->ProgramAuthor      != NULL ? CLI->ProgramAuthor      : PlatformIO_InvisibleString32,
                                           CLI->ProgramCopyright   != NULL ? CLI->ProgramCopyright   : PlatformIO_InvisibleString32,
                                           CLI->ProgramDescription != NULL ? CLI->ProgramDescription : PlatformIO_InvisibleString32,
                                           License                 != NULL ? License                 : PlatformIO_InvisibleString32
                                           );
            UTF32_Deinit(License);
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
            UTF8 *Banner8 = UTF8_Encode(Banner32);
            UTF8_WriteSentence(stdout, Banner8);
            UTF8_Deinit(Banner8);
#elif  (PlatformIO_TargetOS == PlatformIO_WindowsOS)
            UTF16 *Banner16 = UTF16_Encode(Banner32);
            UTF16_WriteSentence(stdout, Banner16);
            UTF16_Deinit(Banner16);
#endif
            UTF32_Deinit(Banner32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    static UTF32 *ArgumentString2OptionFlag(PlatformIO_Immutable(UTF32 *) ArgumentString) {
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
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ArgumentString is not an Argument string"));
            }
            uint64_t ArgumentSwitchSize    = ArgumentStringSize - ArgumentStringPrefixSize;
            ArgumentSwitch                 = UTF32_ExtractSubString(UTF32_CaseFold(ArgumentString), ArgumentStringPrefixSize, ArgumentSwitchSize);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("String Pointer is NULL"));
        }
        return ArgumentSwitch;
    }
    
    uint64_t CommandLineIO_GetNumArguments(int argc) {
        uint64_t NumArguments = 0ULL;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
        NumArguments          = (uint64_t) argc;
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
        NumArguments          = (uint64_t) __argc;
#endif
        return NumArguments;
    }
    
    UTF32 **CommandLineIO_GetArgumentStringSet(void **Arguments) {
        UTF32 **StringSet               = NULL;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
        StringSet                       = (UTF32**) UTF8_StringSet_Decode((PlatformIO_Immutable(UTF8**)) Arguments);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        StringSet                       = (UTF32**) UTF8_StringSet_Decode(reinterpret_cast<PlatformIO_Immutable(UTF8 **)>( const_cast<PlatformIO_Immutable(void **)>(Arguments)));
#endif
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
        uint64_t NumArguments           = (uint64_t) __argc;
        StringSet                       = UTF32_StringSet_Init(NumArguments);
        if (Arguments != NULL) {
            for (uint64_t Argument = 0ULL; Argument < NumArguments; Argument++) {
                StringSet[Argument]     = UTF16_Decode((UTF16*) Arguments);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldnt allocate %llu arguments"), NumArguments);
        }
#endif
        return StringSet;
    }
    
    void CommandLineIO_Switch_SetName(CommandLineIO *CLI, uint64_t SwitchID, PlatformIO_Immutable(UTF32 *) Name) {
        if (CLI != NULL && SwitchID < CLI->NumSwitches && Name != NULL) {
            UTF32 *Normalized             = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded             = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->Switches[SwitchID].Name  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Name == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Name String is NULL"));
        } else if (SwitchID > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches);
        }
    }
    
    void CommandLineIO_Switch_SetDescription(CommandLineIO *CLI, uint64_t SwitchID, PlatformIO_Immutable(UTF32 *) Description) {
        if (CLI != NULL && Description != NULL && SwitchID < CLI->NumSwitches) {
            UTF32 *Normalized                    = UTF32_Normalize(Description, NormalizationForm_KompatibleCompose);
            UTF32 *CaseFolded                    = UTF32_CaseFold(Normalized);
            free(Normalized);
            CLI->Switches[SwitchID].Description  = CaseFolded;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (Description == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Description String is NULL"));
        } else if (SwitchID > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches);
        }
    }
    
    void CommandLineIO_Switch_SetChild(CommandLineIO *CLI, uint64_t ParentID, uint64_t ChildID) {
        if (CLI != NULL && ParentID < CLI->NumSwitches && ChildID < CLI->NumSwitches) {
            if (CLI->Switches[ParentID].SwitchType == SwitchType_Parent) {
                CLI->Switches[ParentID].NumChildren       += 1;
                uint64_t OLD_NumChildren                   = CLI->Switches[ParentID].NumChildren;
                uint64_t *Children_OLD                     = CLI->Switches[ParentID].Children;
                CLI->Switches[ParentID].Children           = (uint64_t*) realloc(CLI->Switches[ParentID].Children, CLI->Switches[ParentID].NumChildren * sizeof(CLI->Switches[ParentID].Children));
                if (CLI->Switches[ParentID].Children != NULL) {
                    free(Children_OLD);
                } else {
                    CLI->Switches[ParentID].Children       = Children_OLD;
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Realloc failed"));
                }
                CLI->Switches[ParentID].Children[OLD_NumChildren] = ChildID;
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ParentID %lld can not have any slaves"), ParentID);
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (ParentID > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ParentID %lld is invalid, it should be between 0 and %lld"), ParentID, CLI->NumSwitches);
        } else if (ChildID > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SlaveID %lld is invalid, it should be between 0 and %lld"), ChildID, CLI->NumSwitches);
        }
    }
    
    void CommandLineIO_Switch_SetType(CommandLineIO *CLI, uint64_t SwitchID, CommandLineIO_SwitchTypes SwitchType) {
        if (CLI != NULL && SwitchType != SwitchType_Unspecified && SwitchID < CLI->NumSwitches) {
            CLI->Switches[SwitchID].SwitchType = SwitchType;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (SwitchType == SwitchType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("You can not set OptionID %lld to SwitchType_Unspecified"), SwitchID);
        } else if (SwitchID > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OptionID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches);
        }
    }
    
    void CommandLineIO_Switch_SetArgumentType(CommandLineIO *CLI, uint64_t SwitchID, CommandLineIO_SwitchArguments ArgumentType) {
        /*
         TODO: Parser - If there is Children, the Argument is after the Child
         */
        if (CLI != NULL && SwitchID < CLI->NumSwitches && ArgumentType != ArgumentType_Unspecified) {
            CLI->Switches[SwitchID].ArgumentType = ArgumentType;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (SwitchID > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches);
        } else if (ArgumentType == ArgumentType_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("You can not set SwitchID %lld to ArgumentType_Unspecified"), SwitchID);
        }
    }
    
    /*
     Argc: 2
     Argv: /Users/Marcus/Library/Developer/Xcode/DerivedData/ArgvTest-eafeawojmembowekykhmfyvegzvu/Build/Products/Debug/ArgvTest
     Argv: --Input:LeftEye=Path
     */
    
    /*
     Argc: 7
     Argv: /Users/Marcus/Library/Developer/Xcode/DerivedData/ArgvTest-eafeawojmembowekykhmfyvegzvu/Build/Products/Debug/ArgvTest
     Argv: --
     Argv: Input
     Argv: :
     Argv: LeftEye
     Argv: =
     Argv: Path
     */
    
    static uint64_t CommandLineIO_UTF32_GetNumTokens(uint64_t NumArguments, PlatformIO_Immutable(UTF32 **) Arguments) {
        uint64_t NumTokens = 0;
        if (Arguments != NULL) {
            // Loop over all the arguments, finding tokens
            uint64_t *StringSizes = (uint64_t*) calloc(NumArguments, sizeof(uint64_t));
            StringSizes           = UTF32_StringSet_GetStringSizesInCodePoints(Arguments);
            for (uint64_t Argument = 1ULL; Argument < NumArguments - 1; Argument++) {
                for (uint64_t CodePoint = 1ULL; CodePoint < StringSizes[Argument]; CodePoint++) {
                    if (Arguments[Argument][CodePoint - 1] == UTF32Character('-') && Arguments[Argument][CodePoint] == UTF32Character('-')) { // "--" prefix
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('-') && Arguments[Argument][CodePoint] != UTF32Character('-')) { // "-" prefix?
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('/') && Arguments[Argument][CodePoint] != UTF32Character('/')) { // "/" prefix?
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('\\') && Arguments[Argument][CodePoint] != UTF32Character('\\')) { // "\" prefix?
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character(' ')) { // " " seperator, we should normalize the spaces as well as the case
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('=')) { // "=" argument seperator?
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character(':')) { // ":" Parent:Child seperator?
                        NumTokens += 1;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Arguments Pointer is NULL"));
        }
        return NumTokens;
    }
    
    static void CommandLineIO_UTF32_TokenizeArguments(CommandLineIO *CLI, uint64_t NumArguments, PlatformIO_Immutable(UTF32 **) Arguments) {
        if (CLI != NULL && Arguments != NULL) {
            uint64_t NumTokens = CommandLineIO_UTF32_GetNumTokens(NumArguments, Arguments);
            CLI->Tokens        = UTF32_StringSet_Init(NumTokens);
            for (uint64_t Argument = 1ULL; Argument < NumArguments - 1; Argument++) {
                
                
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("You entered %lld options, the minimum is %lld"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    static void CommandLineIO_UTF32_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, PlatformIO_Immutable(UTF32 **) Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            uint64_t     CurrentArgument  = 1LL;
            if (NumArguments <= 1) {
                CommandLineIO_ShowBanner(CLI);
            }
            while (CurrentArgument < NumArguments) {
                /*
                 Game plan: Look for Switch introducers aka '--', '-', '/', then find a word that doesn't end with a Colon or space
                 How do we handle Paths tho, Windows uses Colons to seperate the drive from the path, and POSIX uses '/' as a directory seperator, while windows uses the backslash to introduce switches.
                 
                 Well a drive letter on Windows is prefixed by a-zA-Z and followed by a '/' or '\'
                 
                 on POSIX a '/' is the first character or second of a path
                 
                 Windows style switch: "ren /?"
                 Windows style path: "C:/Users"
                 
                 Paths in CommandLineIO need to be introduced with an equals or a space
                 
                 so look for "--", "-", "/", "\", then read until you hit a space or equals, then casefold that, then after it's been casefolded do a substring search, seeing if there are any switches that haven't been ruled out, then check if it matches the actual path.
                 
                 or maybe we should tokenize the Arguments first.
                 
                 how would tokenization work tho?
                 
                 the variable could be called Tokenized Arguments
                 
                 so the tokenization parameters are, a token break occurs at a space, colon, equals, or the supported switch introducers.
                 
                 so loop over all the arguments in argv, except argv[0]
                 */
                
                
                
                
                
                
                
                UTF32   *Argument         = (UTF32*) Arguments[CurrentArgument];
                UTF32   *ArgumentFlag     = ArgumentString2OptionFlag(Argument);
                
                for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
                    if (UTF32_CompareSubString(ArgumentFlag, CLI->Switches[Switch].Name, 0, 0) == Yes) {
                        
                        CLI->NumOptions   += 1;
                        if (CLI->NumOptions == 1) {
                            CLI->Options   = (CommandLineOption*) calloc(1, sizeof(CommandLineOption));
                        } else {
                            CommandLineOption *Options_Old = CLI->Options;
                            CLI->Options                   = (CommandLineOption*) realloc(CLI->Options, CLI->NumOptions * sizeof(CommandLineOption));
                            if (CLI->Options != NULL) {
                                free(Options_Old);
                            } else {
                                CLI->Options               = Options_Old;
                                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Realloc failed"));
                            }
                        }
                        CLI->Options[Switch].SwitchID      = Switch;
                        
                        for (uint64_t Child = 0ULL; Child < CLI->Options[Switch].NumChildren; Child++) {
                            UTF32 *PotentialChildArg        = (UTF32 *) Arguments[CurrentArgument + Child];
                            UTF32 *PotentialChildFlag       = CLI->Switches[Child].Name;
                            if (UTF32_CompareSubString(PotentialChildArg, PotentialChildFlag, 0, 0) == Yes) {
                                CLI->Switches[CLI->NumOptions - 1].NumChildren += 1;
                                CLI->Switches[CLI->NumOptions - 1].Children[CLI->Switches[CLI->NumSwitches - 1].NumChildren - 1] = CLI->Switches[Switch].Children[Child];
                            }
                            free(PotentialChildArg);
                        }
                    } else {
                        // Tell the user that their string wasn't found as a valid argument, or suggest a similar one, or support argument compaction automtically.
                    }
                }
                free(Argument);
                free(ArgumentFlag);
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("You entered %lld options, the minimum is %lld"), NumArguments - 1, CLI->MinOptions);
        }
    }
    
    void CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, PlatformIO_Immutable(UTF8 **) Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32     = UTF32_StringSet_Init(NumArguments);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                UTF32 *Decoded      = UTF8_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFold(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_Normalize(CaseFolded, NormalizationForm_KompatibleCompose);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, (PlatformIO_Immutable(UTF32**)) Arguments32);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            CommandLineIO_ShowHelp(CLI);
        }
    }
    
    void CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, PlatformIO_Immutable(UTF16 **) Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            UTF32 **Arguments32     = UTF32_StringSet_Init(NumArguments);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                UTF32 *Decoded      = UTF16_Decode(Arguments[Arg]);
                UTF32 *CaseFolded   = UTF32_CaseFold(Decoded);
                free(Decoded);
                UTF32 *Normalized   = UTF32_Normalize(CaseFolded, NormalizationForm_KompatibleCompose);
                free(CaseFolded);
                Arguments32[Arg]    = Normalized;
            }
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, (PlatformIO_Immutable(UTF32 **)) Arguments32);
            for (uint64_t Arg = 0ULL; Arg < NumArguments; Arg++) {
                free(Arguments32[Arg]);
            }
            free(Arguments32);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (CLI->MinOptions == 0 || NumArguments < CLI->MinOptions) {
            CommandLineIO_ShowHelp(CLI);
        }
    }
    
    static void CommandLineIO_Error(CommandLineIO *CLI) {
        if (CLI != NULL) {
            /*
             How do we handle erroring?
             
             We can generate some errors ourselves, like f a required switch isn't present, or something doesn't match our syntax, but for more complex things the user will just have to log.
             
             and on second thought I think that's ok
             
             Make sure all mandatory options are present, and a bunch of other stuff
             */
        } else {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    /*
     
     After we parse the Options from the tokens, we need to provide an API for users of ComandLineIO to query Arguments.
     
     Which makes sense, but what all information could a user want to know?
     
     theres a few status things users may be interested in, like is there an option that contains switch X? (also we should have required switches so we can automatically error)
     
     May CommandLineIO_Query, nahh that's too webbitch
     
     CommandLineIO_Option_Exists, and that's a simple bool for the return, it takes a SwitchID and any number of ChildIDs and sees if there's an option that matches that criteria
     
     Now we can have the API serve a dual purpose, where the function returns either the OptionID that matches the criteria if it exists, or an invalid value if it doesn't
     
     so CommandLineIO_Option_Exists should be CommandLineIO_Option_GetOptionID(CLI, SwitchID, NumChildren, Children[])
     
     */
    
    uint64_t CommandLineIO_Option_GetOptionID(CommandLineIO *CLI, uint64_t SwitchID, uint64_t NumChildren, uint64_t *ChildIDs) {
        uint64_t OptionID = 0xFFFFFFFFFFFFFFFF;
        if (CLI != NULL || (NumChildren > 1 && ChildIDs != NULL)) {
            for (uint64_t Option = 0ULL; Option < CLI->NumOptions; Option++) {
                if (SwitchID == CLI->Options[Option].SwitchID) {
                    if (NumChildren > 0 && (CLI->Switches[CLI->Options[Option].SwitchID].Status & SwitchType_Required) == SwitchType_Required) {
                        
                        // ok so the logic is: if there are param children, we need to see if the option contains children, if it does not that may be an error so check if the child is required, if it's not then whatever, if it is error
                        uint64_t OptionChild = 0ULL;
                        uint64_t ParamChild  = 0ULL;
                        
                        while (OptionChild < CLI->Options[Option].NumChildren && ParamChild < NumChildren) {
                            // ok so what do we do?
                            
                            OptionChild += 1;
                            ParamChild  += 1;
                        }
                        
                        /*
                         Old nonsense incase of hidden gems:
                         
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
                         */
                    }
                }
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
        return OptionID;
    }
    
    int8_t CommandLineIO_ConvertBoolString(CommandLineIO *CLI, uint64_t OptionID) {
        int8_t Value = -1;
        if (CLI != NULL && OptionID < CLI->NumOptions) {
            switch (CLI->Options[OptionID].Argument[0]) {
                case U'0':
                    Value = 0;
                    break;
                case U'1':
                    Value = 1;
                    break;
                case U'N':
                case U'n':
                    // check for the rest of No
                    if (CLI->Options[OptionID].Argument[1] != PlatformIO_NULLTerminator &&
                        (CLI->Options[OptionID].Argument[1] == UTF32Character('O') || CLI->Options[OptionID].Argument[1] == UTF32Character('o'))) {
                        Value = 0;
                    }
                    break;
                case U'Y':
                case U'y':
                    // check for the rest of Yes
                    if (CLI->Options[OptionID].Argument[1] != PlatformIO_NULLTerminator &&
                        (CLI->Options[OptionID].Argument[1] == UTF32Character('E') || CLI->Options[OptionID].Argument[1] == UTF32Character('e'))) {
                        if (CLI->Options[OptionID].Argument[2] != PlatformIO_NULLTerminator &&
                            (CLI->Options[OptionID].Argument[2] == UTF32Character('S') || CLI->Options[OptionID].Argument[2] == UTF32Character('s'))) {
                            Value = 1;
                        }
                    }
                    break;
                case U'T':
                case U't':
                    // check for the rest of True
                    if (CLI->Options[OptionID].Argument[1] != PlatformIO_NULLTerminator &&
                        (CLI->Options[OptionID].Argument[1] == UTF32Character('R') || CLI->Options[OptionID].Argument[1] == UTF32Character('r'))) {
                        if (CLI->Options[OptionID].Argument[2] != PlatformIO_NULLTerminator &&
                            (CLI->Options[OptionID].Argument[2] == UTF32Character('U') || CLI->Options[OptionID].Argument[2] == UTF32Character('u'))) {
                            if (CLI->Options[OptionID].Argument[3] != PlatformIO_NULLTerminator &&
                                (CLI->Options[OptionID].Argument[3] == UTF32Character('E') || CLI->Options[OptionID].Argument[3] == UTF32Character('E'))) {
                                Value = 1;
                            }
                        }
                    }
                    break;
                case U'F':
                case U'f':
                    // check for the rest of False
                    if (CLI->Options[OptionID].Argument[1] != PlatformIO_NULLTerminator &&
                        (CLI->Options[OptionID].Argument[1] == UTF32Character('A') || CLI->Options[OptionID].Argument[1] == UTF32Character('a'))) {
                        if (CLI->Options[OptionID].Argument[2] != PlatformIO_NULLTerminator &&
                            (CLI->Options[OptionID].Argument[2] == UTF32Character('L') || CLI->Options[OptionID].Argument[2] == UTF32Character('l'))) {
                            if (CLI->Options[OptionID].Argument[3] != PlatformIO_NULLTerminator &&
                                (CLI->Options[OptionID].Argument[3] == UTF32Character('S') || CLI->Options[OptionID].Argument[3] == UTF32Character('s'))) {
                                if (CLI->Options[OptionID].Argument[3] != PlatformIO_NULLTerminator &&
                                    (CLI->Options[OptionID].Argument[3] == UTF32Character('E') || CLI->Options[OptionID].Argument[3] == UTF32Character('E'))) {
                                    Value = 0;
                                }
                            }
                        }
                    }
                    break;
            }
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionID >= CLI->NumOptions) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OptionID: %llu is invalid"), OptionID);
        }
        return Value;
    }
    
    UTF8 *CommandLineIO_UTF8_GetOptionResult(CommandLineIO *CLI, uint64_t OptionID) {
        UTF8 *Result = NULL;
        if (CLI != NULL && OptionID <= CLI->NumOptions - 1) {
            Result = UTF8_Encode(CLI->Options[OptionID].Argument);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Option %lld is outside the range 0 - %lld"), OptionID, CLI->NumOptions - 1);
        }
        return Result;
    }
    
    UTF16 *CommandLineIO_UTF16_GetOptionResult(CommandLineIO *CLI, uint64_t OptionID) {
        UTF16 *Result = NULL;
        if (CLI != NULL && OptionID <= CLI->NumOptions - 1) {
            Result = UTF16_Encode(CLI->Options[OptionID].Argument);
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (OptionID > CLI->NumOptions - 1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Option %lld is outside the range 0 - %lld"), OptionID, CLI->NumOptions - 1);
        }
        return Result;
    }
    
    UTF8 *CommandLineIO_UTF8_Colorize(PlatformIO_Immutable(UTF8 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF8 *Colorized    = NULL;
        UTF32 *String32    = UTF8_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF8_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF16 *CommandLineIO_UTF16_Colorize(PlatformIO_Immutable(UTF16 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF16 *Colorized   = NULL;
        UTF32 *String32    = UTF16_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF16_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF32 *CommandLineIO_UTF32_Colorize(PlatformIO_Immutable(UTF32 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF32 *Colorized = NULL;
        uint8_t DigitSize = 0;
        DigitSize       += (uint8_t) Logarithm(10, Red);
        DigitSize       += (uint8_t) Logarithm(10, Green);
        DigitSize       += (uint8_t) Logarithm(10, Blue);
        UTF32 *IntegerR  = UTF32_Integer2String(Red,   Base_Integer | Base_Radix10);
        UTF32 *IntegerG  = UTF32_Integer2String(Green, Base_Integer | Base_Radix10);
        UTF32 *IntegerB  = UTF32_Integer2String(Blue,  Base_Integer | Base_Radix10);
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
    
    UTF8 *CommandLineIO_UTF8_Decolorize(PlatformIO_Immutable(UTF8 *) String) {
        UTF8  *Decolorized   = NULL;
        UTF32 *String32      = UTF8_Decode(String);
        UTF32 *Decolorized32 = CommandLineIO_UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF8_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF16 *CommandLineIO_UTF16_Decolorize(PlatformIO_Immutable(UTF16 *) String) {
        UTF16 *Decolorized   = NULL;
        UTF32 *String32      = UTF16_Decode(String);
        UTF32 *Decolorized32 = CommandLineIO_UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF16_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF32 *CommandLineIO_UTF32_Decolorize(PlatformIO_Immutable(UTF32 *) String) {
        UTF32    *Decolorized          = NULL;
        uint64_t  EscapeSequenceSize   = 0;
        uint64_t  EscapeSequenceOffset = 0;
        uint64_t  CodePoint            = 0;
        while (String[CodePoint] != PlatformIO_NULLTerminator) {
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
                free(CLI->Switches[Option].Name);
                free(CLI->Switches[Option].Description);
                free(CLI->Switches[Option].Children);
                free(CLI->Switches[Option].Argument);
            }
            free(CLI->Switches);
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
