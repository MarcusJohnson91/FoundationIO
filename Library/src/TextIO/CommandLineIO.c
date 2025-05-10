#include "../../include/TextIO/CommandLineIO.h" /* Included for our declarations */

#include "../../include/AssertIO.h"             /* Included for Assertions */
#include "../../include/MathIO.h"               /* Included for Logarithm */
#include "../../include/TextIO/FormatIO.h"      /* Included for Formatter */
#include "../../include/TextIO/StringIO.h"      /* Included for string handling */
#include "../../include/TextIO/StringSetIO.h"   /* Included for StringSet support */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#include <signal.h>                             /* Included for SIGWINCH handling */
#include <sys/ioctl.h>                          /* Included for the terminal size */
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsLinux)
#include <pty.h>                                /* Included for winsize, TIOCGWINSZ */
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
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
        size_t   *Children;
        UTF32    *Argument;
        size_t    SwitchID;
        size_t    NumChildren;
    } CommandLineOption;
    
    typedef struct CommandLineSwitch {
        size_t                       *IncompatibleOptions;
        size_t                       *Children;
        UTF32                        *Name;
        UTF32                        *Description;
        UTF32                        *Argument;
        size_t                        OptionID;
        size_t                        NumIncompatibleOptions;
        size_t                        NumChildren;
        SwitchArguments ArgumentType;
        SwitchTypes     SwitchType;
        SwitchStatuses  Status;
    } CommandLineSwitch;
    
    typedef struct CommandLineIO {
        CommandLineSwitch          *Switches;
        CommandLineOption          *Options;
        UTF32                     **Tokens;
        UTF32                      *ProgramName;
        UTF32                      *ProgramAuthor;
        UTF32                      *ProgramDescription;
        UTF32                      *ProgramVersion;
        UTF32                      *ProgramCopyright;
        UTF32                      *ProgramLicenseName;
        UTF32                      *ProgramLicenseDescription;
        UTF32                      *ProgramLicenseURL;
        size_t                      NumSwitches;
        size_t                      NumOptions;
        size_t                      MinOptions;
        size_t                      HelpOption;
        CommandLineIO_LicenseTypes  LicenseType;
    } CommandLineIO;
    
    CommandLineIO *CommandLineIO_Init(size_t NumSwitches) {
        AssertIO(NumSwitches > 0);
        CommandLineIO *CLI           = NULL;
        CLI                      = calloc(1, sizeof(CommandLineIO));
        AssertIO(CLI != NULL);

        CLI->Switches        = calloc(NumSwitches, sizeof(CommandLineSwitch));
        AssertIO(CLI->Switches != NULL);
        CLI->NumSwitches = NumSwitches;
        CLI->MinOptions  = 1;
        return CLI;
    }
    
    size_t GetTerminalWidth(void) {
        size_t Width = 0ULL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Width          = WindowSize.ws_row;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Width          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Width;
    }
    
    size_t GetTerminalHeight(void) {
        size_t Height = 0ULL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        struct winsize       WindowSize;
        ioctl(0, TIOCGWINSZ, &WindowSize);
        Height          = WindowSize.ws_row;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        Height          = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return Height;
    }
    
    bool TerminalWasResized(void) {
        bool SizeChanged = No;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
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
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenBufferInfo);
        SizeChanged      = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
#endif
        return SizeChanged;
    }
    
    void CommandLineIO_SetName(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Name) {
        AssertIO(CLI != NULL);
        AssertIO(Name != NULL);

        UTF32 *Normalized    = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFolded    = UTF32_CaseFold(Normalized);
        UTF32_Deinit(Normalized);
        CLI->ProgramName     = CaseFolded;
    }
    
    void CommandLineIO_SetVersion(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Version) {
        AssertIO(CLI != NULL);
        AssertIO(Version != NULL);

        UTF32 *Normalized    = UTF32_Normalize(Version, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFolded    = UTF32_CaseFold(Normalized);
        UTF32_Deinit(Normalized);
        CLI->ProgramVersion  = CaseFolded;
    }
    
    void CommandLineIO_SetDescription(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Description) {
        AssertIO(CLI != NULL);
        AssertIO(Description != NULL);

        UTF32 *Normalized        = UTF32_Normalize(Description, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFolded        = UTF32_CaseFold(Normalized);
        UTF32_Deinit(Normalized);
        CLI->ProgramDescription  = CaseFolded;
    }
    
    void CommandLineIO_SetAuthor(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Author) {
        AssertIO(CLI != NULL);
        AssertIO(Author != NULL);

        UTF32 *Normalized   = UTF32_Normalize(Author, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFolded   = UTF32_CaseFold(Normalized);
        UTF32_Deinit(Normalized);
        CLI->ProgramAuthor  = CaseFolded;
    }
    
    void CommandLineIO_SetCopyright(CommandLineIO *CLI, PlatformIO_Immutable(UTF32 *) Copyright) {
        AssertIO(CLI != NULL);
        AssertIO(Copyright != NULL);

        UTF32 *Normalized     = UTF32_Normalize(Copyright, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFolded     = UTF32_CaseFold(Normalized);
        UTF32_Deinit(Normalized);
        CLI->ProgramCopyright = CaseFolded;
    }
    
    void CommandLineIO_SetLicense(CommandLineIO *CLI, CommandLineIO_LicenseTypes LicenseType, PlatformIO_Immutable(UTF32 *) Name, PlatformIO_Immutable(UTF32 *) LicenseURL) {
        AssertIO(CLI != NULL);
        AssertIO(LicenseType != LicenseType_Unspecified);
        AssertIO(Name != NULL);
        AssertIO(LicenseURL != NULL);

        CLI->LicenseType                = LicenseType;
        UTF32 *NormalizedName           = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFoldedName           = UTF32_CaseFold(NormalizedName);
        UTF32_Deinit(NormalizedName);
        CLI->ProgramLicenseName         = CaseFoldedName;

        UTF32 *NormalizedLicenseURL     = UTF32_Normalize(LicenseURL, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFoldedLicenseURL     = UTF32_CaseFold(NormalizedLicenseURL);
        UTF32_Deinit(NormalizedLicenseURL);
        CLI->ProgramLicenseURL          = CaseFoldedLicenseURL;
    }
    
    void CommandLineIO_SetMinOptions(CommandLineIO *CLI, size_t MinOptions) {
        AssertIO(CLI != NULL);
        AssertIO(MinOptions < CLI->NumSwitches);

        CLI->MinOptions = MinOptions;
    }
    
    void CommandLineIO_SetHelpOption(CommandLineIO *CLI, size_t HelpOption) {
        AssertIO(CLI != NULL);
        AssertIO(HelpOption < CLI->NumSwitches);

        CLI->HelpOption = HelpOption;
    }
    
    void CommandLineIO_ShowProgress(CommandLineIO *CLI, uint8_t NumItems2Display, PlatformIO_Immutable(UTF32 **)  Strings, PlatformIO_Immutable(uint64_t *) Numerator, PlatformIO_Immutable(uint64_t *) Denominator) {
        AssertIO(CLI != NULL);
        AssertIO(Strings != NULL);

        size_t *StringSize   = calloc(NumItems2Display + TextIO_NULLTerminatorSize, sizeof(size_t));
        for (uint8_t Item = 0; Item < NumItems2Display; Item++) {
            StringSize[Item] = UTF32_GetStringSizeInCodePoints(Strings[Item]);
        }

        size_t *NumProgressIndicatorsPerString   = calloc(NumItems2Display, sizeof(size_t));
        UTF8    **ActualStrings2Print            = UTF8_StringSet_Init(NumItems2Display);
        for (uint8_t String = 0; String < NumItems2Display; String++) {
            NumProgressIndicatorsPerString[String] = CommandLineIO_GetTerminalWidth() - (2 + StringSize[String]);
            uint64_t PercentComplete     = ((Numerator[String] / Denominator[String]) % 100);
            size_t   TerminalWidth       = CommandLineIO_GetTerminalWidth() / 2;
            UTF8    *Indicator           = UTF8_Init(TerminalWidth);
            UTF8_Set(Indicator, '-', TerminalWidth);
            UTF8    *FormattedString     = UTF8_Format(UTF8String("%s[%U32s %llu/%llu %llu]%s%s"), Indicator, Strings[String], Numerator[String], Denominator[String], PercentComplete, Indicator, TextIO_NewLine8);
            UTF8_File_WriteString(stdout, FormattedString);
            free(Indicator);
        }
        free(StringSize);
        free(NumProgressIndicatorsPerString);
        UTF8_StringSet_Deinit(ActualStrings2Print);
    }
    
    static void CommandLineIO_ShowHelp(CommandLineIO *CLI) {
        AssertIO(CLI != NULL);

        UTF8 *Name = UTF8_Encode(CLI->ProgramName);

        UTF8 *ProgramsOptions = UTF8_Format(UTF8String("%s's Options (-|--|/):%s"), Name, TextIO_NewLine8);
        UTF8_File_WriteString(stdout, ProgramsOptions);
        free(Name);
        free(ProgramsOptions);

        size_t StringSetSize = 0;
        for (size_t Switch = 0; Switch < CLI->NumSwitches; Switch++) {
            if (CLI->Switches[Switch].SwitchType == SwitchType_Existential || CLI->Switches[Switch].SwitchType == SwitchType_Standalone) {
                StringSetSize += 1;
            } else if (CLI->Switches[Switch].SwitchType == SwitchType_Parent) {
                StringSetSize += CLI->Switches[Switch].NumChildren;
            }
        }

        UTF32 **GeneratedHelp = UTF32_StringSet_Init(StringSetSize);

        for (size_t Switch = 0ULL; Switch < CLI->NumSwitches; Switch++) {
            SwitchTypes CurrentSwitchType = CLI->Switches[Switch].SwitchType;
            GeneratedHelp[Switch]   = UTF32_Format(UTF32String("%U32s: %U32s%U32s"), CLI->Switches[Switch].Name, CLI->Switches[Switch].Description, TextIO_NewLine32);

            if (CurrentSwitchType == SwitchType_Parent && CLI->Switches[Switch].NumChildren > 0) {
                for (size_t Child = 0ULL; Child < CLI->Switches[Switch].NumChildren; Child++) {
                    GeneratedHelp[Switch + Child] = UTF32_Format(UTF32String("\t%U32s: %U32s%U32s"), CLI->Switches[Child].Name, CLI->Switches[Child].Description, TextIO_NewLine32);
                }
            }
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
            UTF8 **GeneratedHelp8 = UTF8_StringSet_Encode((const UTF32**) GeneratedHelp);
            for (size_t String = 0; String < StringSetSize; String++) {
                UTF8_File_WriteString(stdout, GeneratedHelp8[String]);
            }
            UTF8_StringSet_Deinit(GeneratedHelp8);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
            UTF16 **GeneratedHelp16 = UTF16_StringSet_Encode((const UTF32**) GeneratedHelp);
            for (size_t String = 0; String < StringSetSize; String++) {
                UTF16_File_WriteString(stdout, GeneratedHelp16[String]);
            }
            UTF16_StringSet_Deinit(GeneratedHelp16);
#endif
            UTF32_StringSet_Deinit(GeneratedHelp);
        }
    }
    
    static void ShowBanner(CommandLineIO *CLI) {
        AssertIO(CLI != NULL);
        AssertIO(CLI->LicenseType != LicenseType_Unspecified);
        UTF32 *License = NULL;
        if (CLI->LicenseType == LicenseType_Permissive || CLI->LicenseType == LicenseType_Copyleft) {
            License = UTF32_Format(UTF32String("Released under the \"%U32s\" license, you can see the details of this license at: %U32s"), CLI->ProgramLicenseName != NULL ? CLI->ProgramLicenseName : TextIO_InvisibleString32, CLI->ProgramLicenseURL != NULL ? CLI->ProgramLicenseURL : TextIO_InvisibleString32);
        } else if (CLI->LicenseType == LicenseType_Proprietary) {
            License = UTF32_Format(UTF32String("By using this software, you agree to the End User License Agreement:%U32s%U32s%U32s"), TextIO_NewLine32, TextIO_NewLine32, CLI->ProgramLicenseURL != NULL ? CLI->ProgramLicenseURL : TextIO_InvisibleString32);
        }

        UTF32 *Banner32 = UTF32_Format(UTF32String("%U32s, v. %U32s by %U32s © %U32s, %U32s, %U32s"),
                                       CLI->ProgramName        != NULL ? CLI->ProgramName        : TextIO_InvisibleString32,
                                       CLI->ProgramVersion     != NULL ? CLI->ProgramVersion     : TextIO_InvisibleString32,
                                       CLI->ProgramAuthor      != NULL ? CLI->ProgramAuthor      : TextIO_InvisibleString32,
                                       CLI->ProgramCopyright   != NULL ? CLI->ProgramCopyright   : TextIO_InvisibleString32,
                                       CLI->ProgramDescription != NULL ? CLI->ProgramDescription : TextIO_InvisibleString32,
                                       License                 != NULL ? License                 : TextIO_InvisibleString32
                                       );
        UTF32_Deinit(License);
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        UTF8 *Banner8 = UTF8_Encode(Banner32);
        UTF8_File_WriteString(stdout, Banner8);
        UTF8_Deinit(Banner8);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        UTF16 *Banner16 = UTF16_Encode(Banner32);
        UTF16_File_WriteString(stdout, Banner16);
        UTF16_Deinit(Banner16);
#endif
        UTF32_Deinit(Banner32);
    }
    
    static UTF32 *ArgumentString2OptionFlag(PlatformIO_Immutable(UTF32 *) ArgumentString) {
        AssertIO(ArgumentString != NULL);
        UTF32 *ArgumentSwitch = NULL;

        uint8_t  ArgumentStringPrefixSize = 0;
        size_t   ArgumentStringSize       = UTF32_GetStringSizeInCodePoints(ArgumentString);
        AssertIO(ArgumentStringSize >= 2);
        if (ArgumentString[0] == UTF32Character('-') && ArgumentString[1] == UTF32Character('-')) {
            ArgumentStringPrefixSize  = 2;
        } else if (ArgumentString[0] == UTF32Character('/') || ArgumentString[0] == UTF32Character('\\') || ArgumentString[0] == UTF32Character('-')) {
            ArgumentStringPrefixSize  = 1;
        }
        size_t ArgumentSwitchSize      = ArgumentStringSize - ArgumentStringPrefixSize;
        ArgumentSwitch                 = UTF32_ExtractSubString(UTF32_CaseFold(ArgumentString), ArgumentStringPrefixSize, ArgumentSwitchSize);

        return ArgumentSwitch;
    }
    
    size_t CommandLineIO_GetNumArguments(int argc) {
        size_t NumArguments   = 0ULL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        NumArguments          = (size_t) argc;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        NumArguments          = (size_t) __argc;
#endif
        return NumArguments;
    }
    
    UTF32 **CommandLineIO_GetArgumentStringSet(void **Arguments) {
        AssertIO(Arguments != NULL);
        UTF32 **StringSet               = NULL;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
        StringSet                       = UTF8_StringSet_Decode((const UTF8**) Arguments);
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
        size_t   NumArguments           = (size_t) __argc;
        StringSet                       = UTF32_StringSet_Init(NumArguments);

        for (size_t Argument = 0ULL; Argument < NumArguments; Argument++) {
            StringSet[Argument]     = UTF16_Decode((UTF16*) Arguments);
        }
#endif
        return StringSet;
    }
    
    void Switch_SetName(CommandLineIO *CLI, size_t SwitchID, PlatformIO_Immutable(UTF32 *) Name) {
        AssertIO(CLI != NULL);
        AssertIO(Name != NULL);
        AssertIO(SwitchID < CLI->NumSwitches);

        UTF32 *Normalized             = UTF32_Normalize(Name, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFolded             = UTF32_CaseFold(Normalized);
        UTF32_Deinit(Normalized);
        CLI->Switches[SwitchID].Name  = CaseFolded;
    }
    
    void Switch_SetDescription(CommandLineIO *CLI, size_t SwitchID, PlatformIO_Immutable(UTF32 *) Description) {
        AssertIO(CLI != NULL);
        AssertIO(Description != NULL);
        AssertIO(SwitchID < CLI->NumSwitches);

        UTF32 *Normalized                    = UTF32_Normalize(Description, NormalizationForm_KompatibleCompose);
        UTF32 *CaseFolded                    = UTF32_CaseFold(Normalized);
        UTF32_Deinit(Normalized);
        CLI->Switches[SwitchID].Description  = CaseFolded;
    }
    
    void Switch_SetChild(CommandLineIO *CLI, size_t ParentID, size_t ChildID) {
        AssertIO(CLI != NULL);
        AssertIO(ParentID < CLI->NumSwitches);
        AssertIO(ChildID < CLI->NumSwitches);
        AssertIO(CLI->Switches[ParentID].SwitchType == SwitchType_Parent);
        AssertIO(CLI->Switches[ChildID].SwitchType == SwitchType_Child);

        CLI->Switches[ParentID].NumChildren       += 1;
        CLI->Switches[ParentID].Children           = realloc(CLI->Switches[ParentID].Children, CLI->Switches[ParentID].NumChildren * sizeof(size_t));
    }
    
    void Switch_SetType(CommandLineIO *CLI, size_t SwitchID, CommandLineIO_SwitchTypes SwitchType) {
        AssertIO(CLI != NULL);
        AssertIO(SwitchID < CLI->NumSwitches);
        AssertIO(SwitchType != SwitchType_Unspecified);

        CLI->Switches[SwitchID].SwitchType = SwitchType;
    }
    
    void Switch_SetArgumentType(CommandLineIO *CLI, size_t SwitchID, CommandLineIO_SwitchArguments ArgumentType) {
        AssertIO(CLI != NULL);
        AssertIO(SwitchID < CLI->NumSwitches);
        AssertIO(ArgumentType != SwitchArgument_Unspecified);

        /* TODO: Parser - If there is Children, the Argument is after the Child */
        CLI->Switches[SwitchID].ArgumentType = ArgumentType;
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
    
    static size_t UTF32_GetNumTokens(uint64_t NumArguments, PlatformIO_Immutable(UTF32 **)  Arguments) {
        AssertIO(Arguments != NULL);
        size_t NumTokens = 0;

        size_t *StringSizes           = UTF32_StringSet_GetStringSizesInCodePoints(Arguments);
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
        return NumTokens;
    }
    
    static void UTF32_TokenizeArguments(CommandLineIO *CLI, size_t NumArguments, PlatformIO_Immutable(UTF32 **)  Arguments) {
        AssertIO(CLI != NULL);
        AssertIO(Arguments != NULL);

        size_t NumTokens   = CommandLineIO_UTF32_GetNumTokens(NumArguments, Arguments);
        CLI->Tokens        = UTF32_StringSet_Init(NumTokens);
        for (uint64_t Argument = 1ULL; Argument < NumArguments - 1; Argument++) {


        }
    }
    
    static void UTF32_ParseOptions(CommandLineIO *CLI, size_t NumArguments, PlatformIO_Immutable(UTF32 **)  Arguments) {
        AssertIO(CLI != NULL);
        AssertIO(CLI->MinOptions > 0);
        AssertIO(CLI->MinOptions < CLI->NumOptions);


        size_t       CurrentArgument  = 1LL;
        if (NumArguments < CLI->MinOptions) {
            CommandLineIO_ShowBanner(CLI);
        }
        while (CurrentArgument < NumArguments) {
            UTF32   *Argument         = (UTF32*) Arguments[CurrentArgument];
            UTF32   *ArgumentFlag     = ArgumentString2OptionFlag(Argument);

            for (size_t Switch = 0; Switch < CLI->NumSwitches; Switch++) {
                if (UTF32_CompareSubString(ArgumentFlag, CLI->Switches[Switch].Name, 0, 0) == Yes) {

                    CLI->NumOptions   += 1;
                    CLI->Options       = (CommandLineOption*) realloc(CLI->Options, CLI->NumOptions * sizeof(CommandLineOption));
                    CLI->Options[Switch].SwitchID      = Switch;

                    for (size_t Child = 0; Child < CLI->Options[Switch].NumChildren; Child++) {
                        UTF32 *PotentialChildArg        = (UTF32 *) Arguments[CurrentArgument + Child];
                        UTF32 *PotentialChildFlag       = CLI->Switches[Child].Name;
                        if (UTF32_CompareSubString(PotentialChildArg, PotentialChildFlag, 0, 0) == Yes) {
                            CLI->Switches[CLI->NumOptions - 1].NumChildren += 1;
                            CLI->Switches[CLI->NumOptions - 1].Children[CLI->Switches[CLI->NumSwitches - 1].NumChildren - 1] = CLI->Switches[Switch].Children[Child];
                        }
                        UTF32_Deinit(PotentialChildArg);
                    }
                } else {
                    // Tell the user that their string wasn't found as a valid argument, or suggest a similar one, or support argument compaction automtically.
                }
            }
            UTF32_Deinit(Argument);
            UTF32_Deinit(ArgumentFlag);
        }
    }
    
    void UTF8_ParseOptions(CommandLineIO *CLI, size_t NumArguments, PlatformIO_Immutable(UTF8 **) Arguments) {
        AssertIO(CLI != NULL);
        AssertIO(Arguments != NULL);

        UTF32 **Arguments32     = UTF32_StringSet_Init(NumArguments);
        for (size_t Arg = 0ULL; Arg < NumArguments; Arg++) {
            UTF32 *Decoded      = UTF8_Decode(Arguments[Arg]);
            UTF32 *CaseFolded   = UTF32_CaseFold(Decoded);
            UTF32_Deinit(Decoded);
            UTF32 *Normalized   = UTF32_Normalize(CaseFolded, NormalizationForm_KompatibleCompose);
            UTF32_Deinit(CaseFolded);
            Arguments32[Arg]    = Normalized;
        }
        UTF32_ParseOptions(CLI, NumArguments, (const UTF32 *const *) Arguments32);
        UTF32_StringSet_Deinit(Arguments32);
    }
    
    void UTF16_ParseOptions(CommandLineIO *CLI, size_t NumArguments, PlatformIO_Immutable(UTF16 **) Arguments) {
        AssertIO(CLI != NULL);
        AssertIO(Arguments != NULL);

        UTF32 **Arguments32     = UTF32_StringSet_Init(NumArguments);
        for (size_t Arg = 0ULL; Arg < NumArguments; Arg++) {
            UTF32 *Decoded      = UTF16_Decode(Arguments[Arg]);
            UTF32 *CaseFolded   = UTF32_CaseFold(Decoded);
            UTF32_Deinit(Decoded);
            UTF32 *Normalized   = UTF32_Normalize(CaseFolded, NormalizationForm_KompatibleCompose);
            UTF32_Deinit(CaseFolded);
            Arguments32[Arg]    = Normalized;
        }
        UTF32_ParseOptions(CLI, NumArguments, (const UTF32 *const *) Arguments32);
        UTF32_StringSet_Deinit(Arguments32);
    }
    
    static void Error(CommandLineIO *CLI) {
        AssertIO(CLI != NULL);
        /*
         How do we handle erroring?

         We can generate some errors ourselves, like if a required switch isn't present, or something doesn't match our syntax, but for more complex things the user will just have to log.

         and on second thought I think that's ok

         Make sure all mandatory options are present, and a bunch of other stuff
         */
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
    
    size_t Option_GetOptionID(CommandLineIO *CLI, size_t SwitchID, size_t NumChildren, size_t *ChildIDs) {
        AssertIO(CLI != NULL);
        AssertIO(ChildIDs != NULL);
        AssertIO(SwitchID < CLI->NumSwitches);
        AssertIO(NumChildren > 1);

        size_t OptionID = 0xFFFFFFFFFFFFFFFF;
        for (size_t Option = 0ULL; Option < CLI->NumOptions; Option++) {
            if (SwitchID == CLI->Options[Option].SwitchID) {
                if (NumChildren > 0 && PlatformIO_Is(CLI->Switches[CLI->Options[Option].SwitchID].Status, SwitchType_Required)) {
                    size_t OptionChild = 0ULL;
                    size_t ParamChild  = 0ULL;

                    while (OptionChild < CLI->Options[Option].NumChildren && ParamChild < NumChildren) {
                        OptionChild += 1;
                        ParamChild  += 1;
                    }
                }
            }
        }
        return OptionID;
    }
    
    int8_t ConvertBoolString(CommandLineIO *CLI, size_t OptionID) {
        AssertIO(CLI != NULL);
        AssertIO(OptionID < CLI->NumOptions);

        int8_t Value = -1;
        bool Bool = UTF32_String2Bool(CLI->Options[OptionID].Argument);
        Value = Bool == false ? -1 : true;
        return Value;
    }
    
    UTF8 *UTF8_GetOptionResult(CommandLineIO *CLI, size_t OptionID) {
        AssertIO(CLI != NULL);
        AssertIO(OptionID < CLI->NumOptions);

        return UTF8_Encode(CLI->Options[OptionID].Argument);
    }
    
    UTF16 *UTF16_GetOptionResult(CommandLineIO *CLI, size_t OptionID) {
        AssertIO(CLI != NULL);
        AssertIO(OptionID < CLI->NumOptions);

        return UTF16_Encode(CLI->Options[OptionID].Argument);
    }
    
    UTF8 *UTF8_Colorize(PlatformIO_Immutable(UTF8 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        AssertIO(String != NULL);
        AssertIO(ColorType != ColorType_Unspecified);

        UTF8 *Colorized    = NULL;
        UTF32 *String32    = UTF8_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF8_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF16 *UTF16_Colorize(PlatformIO_Immutable(UTF16 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        AssertIO(String != NULL);
        AssertIO(ColorType != ColorType_Unspecified);

        UTF16 *Colorized   = NULL;
        UTF32 *String32    = UTF16_Decode(String);
        UTF32 *Colorized32 = CommandLineIO_UTF32_Colorize(String32, ColorType, Red, Green, Blue);
        UTF32_Deinit(String32);
        Colorized          = UTF16_Encode(Colorized32);
        UTF32_Deinit(Colorized32);
        return Colorized;
    }
    
    UTF32 *UTF32_Colorize(PlatformIO_Immutable(UTF32 *) String, CommandLineIO_ColorTypes ColorType, uint8_t Red, uint8_t Green, uint8_t Blue) {
        AssertIO(String != NULL);
        AssertIO(ColorType != ColorType_Unspecified);

        UTF32 *Colorized = NULL;
        uint8_t DigitSize = 0;
        DigitSize       += (uint8_t) Logarithm(10, Red);
        DigitSize       += (uint8_t) Logarithm(10, Green);
        DigitSize       += (uint8_t) Logarithm(10, Blue);
        UTF32 *IntegerR  = UTF32_Integer2String(Base_Integer | Base_Radix10, Red);
        UTF32 *IntegerG  = UTF32_Integer2String(Base_Integer | Base_Radix10, Green);
        UTF32 *IntegerB  = UTF32_Integer2String(Base_Integer | Base_Radix10, Blue);
        DigitSize       += 8;
        if (PlatformIO_Is(ColorType, ColorType_Foreground)) {
            UTF32 *Formatted = UTF32_Format(UTF32String("%U32c[38;2;%U32s;%U32s;%U32s;m"), U'\x1B', IntegerR, IntegerG, IntegerB);
            Colorized        = UTF32_Insert(String, Formatted, 0);
            UTF32_Deinit(Formatted);
        } else if (PlatformIO_Is(ColorType, ColorType_Background)) {
            UTF32 *Formatted = UTF32_Format(UTF32String("%U32c[48;2;%U32s;%U32s;%U32s:m"), UTF32Character('\x1B'), IntegerR, IntegerG, IntegerB);
            Colorized        = UTF32_Insert(String, Formatted, 0);
            UTF32_Deinit(Formatted);
        }
        UTF32_Deinit(IntegerR);
        UTF32_Deinit(IntegerG);
        UTF32_Deinit(IntegerB);
        return Colorized;
    }
    
    UTF8 *UTF8_Decolorize(PlatformIO_Immutable(UTF8 *) String) {
        AssertIO(String != NULL);

        UTF8  *Decolorized   = NULL;
        UTF32 *String32      = UTF8_Decode(String);
        UTF32 *Decolorized32 = UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF8_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF16 *UTF16_Decolorize(PlatformIO_Immutable(UTF16 *) String) {
        AssertIO(String != NULL);

        UTF16 *Decolorized   = NULL;
        UTF32 *String32      = UTF16_Decode(String);
        UTF32 *Decolorized32 = CommandLineIO_UTF32_Decolorize(String32);
        UTF32_Deinit(String32);
        Decolorized          = UTF16_Encode(Decolorized32);
        UTF32_Deinit(Decolorized32);
        return Decolorized;
    }
    
    UTF32 *UTF32_Decolorize(PlatformIO_Immutable(UTF32 *) String) {
        AssertIO(String != NULL);

        UTF32    *Decolorized          = NULL;
        uint64_t  EscapeSequenceSize   = 0;
        uint64_t  EscapeSequenceOffset = 0;
        uint64_t  CodePoint            = 0;
        while (String[CodePoint] != TextIO_NULLTerminator) {
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
        AssertIO(CLI != NULL);

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
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
