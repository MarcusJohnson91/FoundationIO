#include "../../include/TextIO/CommandLineIO.h" /* Included for our declarations */
#include "../../include/MathIO.h"               /* Included for Logarithm */
#include "../../include/TextIO/FormatIO.h"      /* Included for Formatter */
#include "../../include/TextIO/LogIO.h"         /* Included for Logging */
#include "../../include/TextIO/StringIO.h"      /* Included for StringIO's declarations */

#if ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#include <signal.h>                             /* Included for SIGWINCH handling */
#include <sys/ioctl.h>                          /* Included for the terminal size */
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsLinux) == PlatformIO_TargetOSIsLinux)
#include <pty.h>                                /* Included for winsize, TIOCGWINSZ */
#else
#endif /* PlatformIO_TargetOSIsLinux */
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
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
        UTF32    *Argument;
        uint64_t  SwitchID;
        uint64_t  NumChildren;
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
        uint64_t                      NumChildren;
        CommandLineIO_SwitchArguments ArgumentType;
        CommandLineIO_SwitchTypes     SwitchType;
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
        CommandLineSwitch          *Switches;
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
                    CLI->MinOptions  = 1;
                } else {
                    CommandLineIO_Deinit(CLI);
                    CLI              = NULL;
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
    
    uint64_t CommandLineIO_GetTerminalWidth(void) {
        uint64_t Width = 0ULL;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Width          = WindowSize.ws_row;
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Width          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Width;
    }
    
    uint64_t CommandLineIO_GetTerminalHeight(void) {
        uint64_t Height = 0ULL;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Height          = WindowSize.ws_row;
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Height          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Height;
    }
    
    bool CommandLineIO_TerminalWasResized(void) {
        bool SizeChanged = No;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
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
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        SizeChanged      = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return SizeChanged;
    }
    
    void CommandLineIO_SetName(CommandLineIO *CLI, ImmutableString_UTF32 Name) {
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
    
    void CommandLineIO_SetVersion(CommandLineIO *CLI, ImmutableString_UTF32 Version) {
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
    
    void CommandLineIO_SetDescription(CommandLineIO *CLI, ImmutableString_UTF32 Description) {
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
    
    void CommandLineIO_SetAuthor(CommandLineIO *CLI, ImmutableString_UTF32 Author) {
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
    
    void CommandLineIO_SetCopyright(CommandLineIO *CLI, ImmutableString_UTF32 Copyright) {
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
    
    void CommandLineIO_SetLicense(CommandLineIO *CLI, CommandLineIO_LicenseTypes LicenseType, ImmutableString_UTF32 Name, ImmutableString_UTF32 LicenseURL) {
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
    
    void CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, ImmutableStringSet_UTF32 Strings, PlatformIO_Immutable(uint64_t *) Numerator, PlatformIO_Immutable(uint64_t *) Denominator) {
        if (CLI != NULL) {
            uint64_t *StringSize = (uint64_t*) calloc(NumItems2Display + PlatformIO_NULLTerminatorSize, sizeof(uint64_t));
            for (uint8_t Item = 0; Item < NumItems2Display; Item++) {
                StringSize[Item] = UTF32_GetStringSizeInCodePoints(Strings[Item]);
            }
            
            uint64_t *NumProgressIndicatorsPerString = calloc(NumItems2Display, sizeof(uint64_t));
            UTF8    **ActualStrings2Print            = UTF8_StringSet_Init(NumItems2Display);
            for (uint8_t String = 0; String < NumItems2Display; String++) {
                NumProgressIndicatorsPerString[String] = CommandLineIO_GetTerminalWidth() - (2 + StringSize[String]);
                uint64_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
                uint64_t TerminalWidth       = CommandLineIO_GetTerminalWidth() / 2;
                UTF8    *Indicator           = UTF8_Init(TerminalWidth);
                UTF8_Set(Indicator, '-', TerminalWidth);
                UTF8    *FormattedString     = UTF8_Format(UTF8String("[%s%l32s %llu/%llu %llu/%llu%s]%s"), Indicator, Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, PlatformIO_NewLine8);
                UTF8_File_WriteString(stdout, FormattedString);
                free(Indicator);
            }
            free(StringSize);
            free(NumProgressIndicatorsPerString);
            UTF8_Deinit(ActualStrings2Print);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    static void CommandLineIO_ShowHelp(CommandLineIO *CLI) {
        if (CLI != NULL) {
            UTF8 *Name = UTF8_Encode(CLI->ProgramName);
            
            UTF8 *ProgramsOptions = UTF8_Format(UTF8String("%s's Options (-|--|/):%s"), Name, PlatformIO_NewLine8);
            UTF8_File_WriteString(stdout, ProgramsOptions);
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
                GeneratedHelp[Switch]   = UTF32_Format(UTF32String("%l32s: %l32s%l32s"), CLI->Switches[Switch].Name, CLI->Switches[Switch].Description, PlatformIO_NewLine32);
                
                if (CurrentSwitchType == SwitchType_Parent && CLI->Switches[Switch].NumChildren > 0) {
                    for (uint64_t Child = 0ULL; Child < CLI->Switches[Switch].NumChildren; Child++) {
                        GeneratedHelp[Switch + Child] = UTF32_Format(UTF32String("\t%l32s: %l32s%l32s"), CLI->Switches[Child].Name, CLI->Switches[Child].Description, PlatformIO_NewLine32);
                    }
                }
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
                UTF8 **GeneratedHelp8 = UTF8_StringSet_Encode((ImmutableStringSet_UTF32) GeneratedHelp);
                for (uint64_t String = 0; String < StringSetSize; String++) {
                    UTF8_File_WriteString(stdout, GeneratedHelp8[String]);
                }
                UTF8_StringSet_Deinit(GeneratedHelp8);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
                UTF16 **GeneratedHelp16 = UTF16_StringSet_Encode((ImmutableStringSet_UTF32) GeneratedHelp);
                for (uint64_t String = 0; String < StringSetSize; String++) {
                    UTF16_File_WriteString(stdout, GeneratedHelp16[String]);
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
                License = UTF32_Format(UTF32String("Released under the \"%l32s\" license, you can see the details of this license at: %l32s"), CLI->ProgramLicenseName != NULL ? CLI->ProgramLicenseName : PlatformIO_InvisibleString32, CLI->ProgramLicenseURL != NULL ? CLI->ProgramLicenseURL : PlatformIO_InvisibleString32);
            } else if (CLI->LicenseType == LicenseType_Proprietary) {
                License = UTF32_Format(UTF32String("By using this software, you agree to the End User License Agreement:%l32s%l32s%l32s"), PlatformIO_NewLine32, PlatformIO_NewLine32, CLI->ProgramLicenseURL != NULL ? CLI->ProgramLicenseURL : PlatformIO_InvisibleString32);
            } else {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("LicenseType isn't set"));
            }
            
            UTF32 *Banner32 = UTF32_Format(UTF32String("%l32s, v. %l32s by %l32s © %l32s, %l32s, %l32s"),
                                           CLI->ProgramName        != NULL ? CLI->ProgramName        : PlatformIO_InvisibleString32,
                                           CLI->ProgramVersion     != NULL ? CLI->ProgramVersion     : PlatformIO_InvisibleString32,
                                           CLI->ProgramAuthor      != NULL ? CLI->ProgramAuthor      : PlatformIO_InvisibleString32,
                                           CLI->ProgramCopyright   != NULL ? CLI->ProgramCopyright   : PlatformIO_InvisibleString32,
                                           CLI->ProgramDescription != NULL ? CLI->ProgramDescription : PlatformIO_InvisibleString32,
                                           License                 != NULL ? License                 : PlatformIO_InvisibleString32
                                           );
            UTF32_Deinit(License);
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            UTF8 *Banner8 = UTF8_Encode(Banner32);
            UTF8_File_WriteString(stdout, Banner8);
            UTF8_Deinit(Banner8);
#elif  (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            UTF16 *Banner16 = UTF16_Encode(Banner32);
            UTF16_File_WriteString(stdout, Banner16);
            UTF16_Deinit(Banner16);
#endif
            UTF32_Deinit(Banner32);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        }
    }
    
    static UTF32 *ArgumentString2OptionFlag(ImmutableString_UTF32 ArgumentString) {
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
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
        NumArguments          = (uint64_t) argc;
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        NumArguments          = (uint64_t) __argc;
#endif
        return NumArguments;
    }
    
    UTF32 **CommandLineIO_GetArgumentStringSet(void **Arguments) {
        UTF32 **StringSet               = NULL;
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#if   (PlatformIO_Language == PlatformIO_LanguageIsC)
        StringSet                       = (UTF32**) UTF8_StringSet_Decode((ImmutableStringSet_UTF8) Arguments);
#elif (PlatformIO_Language == PlatformIO_LanguageIsCXX)
        StringSet                       = (UTF32**) UTF8_StringSet_Decode(reinterpret_cast<ImmutableStringSet_UTF8>( const_cast<PlatformIO_Immutable(void **)>(Arguments)));
#endif
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
        uint64_t NumArguments           = (uint64_t) __argc;
        StringSet                       = UTF32_StringSet_Init(NumArguments);
        if (Arguments != NULL) {
            for (uint64_t Argument = 0ULL; Argument < NumArguments; Argument++) {
                StringSet[Argument]     = UTF16_Decode((UTF16*) Arguments);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate %llu arguments"), NumArguments);
        }
#endif
        return StringSet;
    }
    
    void CommandLineIO_Switch_SetName(CommandLineIO *CLI, uint64_t SwitchID, ImmutableString_UTF32 Name) {
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
    
    void CommandLineIO_Switch_SetDescription(CommandLineIO *CLI, uint64_t SwitchID, ImmutableString_UTF32 Description) {
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
                CLI->Switches[ParentID].Children           = (uint64_t*) realloc(CLI->Switches[ParentID].Children, CLI->Switches[ParentID].NumChildren * sizeof(uint64_t));
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
        if (CLI != NULL && SwitchID < CLI->NumSwitches && ArgumentType != SwitchArgument_Unspecified) {
            CLI->Switches[SwitchID].ArgumentType = ArgumentType;
        } else if (CLI == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
        } else if (SwitchID > CLI->NumSwitches) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("SwitchID %lld is invalid, it should be between 0 and %lld"), SwitchID, CLI->NumSwitches);
        } else if (ArgumentType == SwitchArgument_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("You can not set SwitchID %lld to SwitchArgument_Unspecified"), SwitchID);
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
    
    static uint64_t CommandLineIO_UTF32_GetNumTokens(uint64_t NumArguments, ImmutableStringSet_UTF32 Arguments) {
        uint64_t NumTokens = 0;
        if (Arguments != NULL) {
            uint64_t *StringSizes           = UTF32_StringSet_GetStringSizesInCodePoints(Arguments);
            for (uint64_t Argument = 1ULL; Argument < NumArguments - 1; Argument++) {
                for (uint64_t CodePoint = 1ULL; CodePoint < StringSizes[Argument]; CodePoint++) {
                    if (Arguments[Argument][CodePoint - 1] == UTF32Character('-') && Arguments[Argument][CodePoint] == UTF32Character('-')) {
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('-') && Arguments[Argument][CodePoint] != UTF32Character('-')) {
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('/') && Arguments[Argument][CodePoint] != UTF32Character('/')) {
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('\\') && Arguments[Argument][CodePoint] != UTF32Character('\\')) {
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character(' ')) {
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character('=')) {
                        NumTokens += 1;
                    } else if (Arguments[Argument][CodePoint - 1] == UTF32Character(':')) {
                        NumTokens += 1;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Arguments Pointer is NULL"));
        }
        return NumTokens;
    }
    
    static void CommandLineIO_UTF32_TokenizeArguments(CommandLineIO *CLI, uint64_t NumArguments, ImmutableStringSet_UTF32 Arguments) {
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
    
    static void CommandLineIO_UTF32_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, ImmutableStringSet_UTF32 Arguments) {
        if (CLI != NULL && (CLI->MinOptions >= 1 && NumArguments >= CLI->MinOptions)) {
            uint64_t     CurrentArgument  = 1LL;
            if (NumArguments < CLI->MinOptions) {
                CommandLineIO_ShowBanner(CLI);
            }
            while (CurrentArgument < NumArguments) {
                UTF32   *Argument         = (UTF32*) Arguments[CurrentArgument];
                UTF32   *ArgumentFlag     = ArgumentString2OptionFlag(Argument);
                
                for (uint64_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
                    if (UTF32_CompareSubString(ArgumentFlag, CLI->Switches[Switch].Name, 0, 0) == Yes) {
                        
                        CLI->NumOptions   += 1;
                        CLI->Options       = (CommandLineOption*) realloc(CLI->Options, CLI->NumOptions * sizeof(CommandLineOption));
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
    
    void CommandLineIO_UTF8_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, ImmutableStringSet_UTF8 Arguments) {
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
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, (ImmutableStringSet_UTF32) Arguments32);
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
    
    void CommandLineIO_UTF16_ParseOptions(CommandLineIO *CLI, uint64_t NumArguments, ImmutableStringSet_UTF16 Arguments) {
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
            CommandLineIO_UTF32_ParseOptions(CLI, NumArguments, (ImmutableStringSet_UTF32) Arguments32);
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommandLineIO Pointer is NULL"));
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
        if (CLI != NULL && (NumChildren > 1 && ChildIDs != NULL)) {
            for (uint64_t Option = 0ULL; Option < CLI->NumOptions; Option++) {
                if (SwitchID == CLI->Options[Option].SwitchID) {
                    if (NumChildren > 0 && (CLI->Switches[CLI->Options[Option].SwitchID].Status & SwitchType_Required) == SwitchType_Required) {
                        uint64_t OptionChild = 0ULL;
                        uint64_t ParamChild  = 0ULL;
                        
                        while (OptionChild < CLI->Options[Option].NumChildren && ParamChild < NumChildren) {
                            
                            OptionChild += 1;
                            ParamChild  += 1;
                        }
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
            bool Bool = UTF32_String2Bool(CLI->Options[OptionID].Argument);
            Value = Bool == false ? -1 : true;
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
    
    UTF8 *CommandLineIO_UTF8_Colorize(ImmutableString_UTF8 String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF8 *Colorized    = NULL;
        UTF32 *String32    = UTF8_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF8_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF16 *CommandLineIO_UTF16_Colorize(ImmutableString_UTF16 String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF16 *Colorized   = NULL;
        UTF32 *String32    = UTF16_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF16_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF32 *CommandLineIO_UTF32_Colorize(ImmutableString_UTF32 String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        UTF32 *Colorized = NULL;
        uint8_t DigitSize = 0;
        DigitSize       += (uint8_t) Logarithm(10, Red);
        DigitSize       += (uint8_t) Logarithm(10, Green);
        DigitSize       += (uint8_t) Logarithm(10, Blue);
        UTF32 *IntegerR  = UTF32_Integer2String(Base_Integer | Base_Radix10, Red);
        UTF32 *IntegerG  = UTF32_Integer2String(Base_Integer | Base_Radix10, Green);
        UTF32 *IntegerB  = UTF32_Integer2String(Base_Integer | Base_Radix10, Blue);
        DigitSize       += 8;
        if ((ColorType & ColorType_Foreground) == ColorType_Foreground) {
            UTF32 *Formatted = UTF32_Format(UTF32String("%l32c[38;2;%l32s;%l32s;%l32s;m"), U'\x1B', IntegerR, IntegerG, IntegerB);
            Colorized        = UTF32_Insert(String, Formatted, 0);
            UTF32_Deinit(Formatted);
        } else if ((ColorType & ColorType_Background) == ColorType_Background) {
            UTF32 *Formatted = UTF32_Format(UTF32String("%l32c[48;2;%l32s;%l32s;%l32s:m"), UTF32Character('\x1B'), IntegerR, IntegerG, IntegerB);
            Colorized        = UTF32_Insert(String, Formatted, 0);
            UTF32_Deinit(Formatted);
        }
        UTF32_Deinit(IntegerR);
        UTF32_Deinit(IntegerG);
        UTF32_Deinit(IntegerB);
        return Colorized;
    }
    
    UTF8 *CommandLineIO_UTF8_Decolorize(ImmutableString_UTF8 String) {
        UTF8  *Decolorized   = NULL;
        UTF32 *String32      = UTF8_Decode(String);
        UTF32 *Decolorized32 = CommandLineIO_UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF8_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF16 *CommandLineIO_UTF16_Decolorize(ImmutableString_UTF16 String) {
        UTF16 *Decolorized   = NULL;
        UTF32 *String32      = UTF16_Decode(String);
        UTF32 *Decolorized32 = CommandLineIO_UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF16_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF32 *CommandLineIO_UTF32_Decolorize(ImmutableString_UTF32 String) {
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
