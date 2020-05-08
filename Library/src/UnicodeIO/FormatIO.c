#include "../../include/Constants.h"                   /* Included for the Number tables */
#include "../../include/MathIO.h"                      /* Included for Logarithm */
#include "../../include/UnicodeIO/FormatIO.h"          /* Included for our declarations */
#include "../../include/UnicodeIO/LogIO.h"             /* Included for Logging */
#include "../../include/UnicodeIO/StringIO.h"          /* Included for StringIO */

#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
extern "C" {
#endif
    
    typedef struct FormatSpecifier {
        UTF32                   *Argument;
        uint64_t                 Start;
        uint64_t                 ModifierStart;
        uint64_t                 End;
        uint64_t                 MinWidth;
        uint64_t                 Precision;
        uint64_t                 Position;
        FormatIO_ModifierTypes   ModifierType;
        FormatIO_Flags           Flag;
        FormatIO_BaseTypes       BaseType;
        FormatIO_ModifierLengths LengthModifier;
        FormatIO_MinWidths       MinWidthFlag;
        FormatIO_Precisions      PrecisionFlag;
        FormatIO_Positions       PositionFlag;
    } FormatSpecifier;
    
    typedef struct FormatSpecifiers {
        FormatSpecifier         *Specifiers;
        uint64_t                *UniqueSpecifiers;
        uint64_t                 NumSpecifiers;
        uint64_t                 NumUniqueSpecifiers;
        FoundationIO_StringTypes StringType;
    } FormatSpecifiers;
    
    void FormatSpecifiers_Deinit(FormatSpecifiers *Specifiers) {
        if (Specifiers != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers && Specifiers->Specifiers[Specifier].PositionFlag != Position_Unknown) {
                    // Find the actual argument from the Position by looking up the Position -1 in UniqueSpecifiers, then going to that argument and freeing it.
                    uint64_t Pos = Specifiers->Specifiers[Specifier].Position;
                    UTF32_Deinit(Specifiers->Specifiers[Pos].Argument);
                    // I'm expecting this to break
                } else {
                    UTF32_Deinit(Specifiers->Specifiers[Specifier].Argument);
                }
            }
            free(Specifiers->UniqueSpecifiers);
            free(Specifiers->Specifiers);
            free(Specifiers);
        }
    }
    
    FormatSpecifiers *FormatSpecifiers_Init(uint64_t NumSpecifiers) {
        FormatSpecifiers *Specifiers                                = (FormatSpecifiers*) calloc(1, sizeof(FormatSpecifiers));
        if (Specifiers != NULL) {
            Specifiers->Specifiers                                  = (FormatSpecifier*) calloc(NumSpecifiers, sizeof(Specifiers->Specifiers));
            if (Specifiers->Specifiers != NULL) {
                Specifiers->NumSpecifiers                           = NumSpecifiers;
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    Specifiers->Specifiers[Specifier].Start         = 0x7FFFFFFFFFFFFFFE;
                    Specifiers->Specifiers[Specifier].End           = 0x7FFFFFFFFFFFFFFE;
                    Specifiers->Specifiers[Specifier].ModifierStart = 0x7FFFFFFFFFFFFFFE;
                }
            } else {
                FormatSpecifiers_Deinit(Specifiers);
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate %llu Specifiers"), NumSpecifiers);
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate FormatSpecifiers"));
        }
        return Specifiers;
    }
    
    static FoundationIO_Bases ConvertModifierType2Base(FormatIO_ModifierTypes ModifierType) {
        FoundationIO_Bases Base = Base_Unknown;
        if ((ModifierType & ModifierType_Integer) == ModifierType_Integer) {
            Base               |= Base_Integer;
            if ((ModifierType & ModifierType_Radix2) == ModifierType_Radix2) {
                Base           |= Base_Radix2;
            } else if ((ModifierType & ModifierType_Radix8) == ModifierType_Radix8) {
                Base           |= Base_Radix8;
            } else if ((ModifierType & ModifierType_Radix10) == ModifierType_Radix10) {
                Base           |= Base_Radix10;
            } else if ((ModifierType & ModifierType_Radix16) == ModifierType_Radix16) {
                Base           |= Base_Radix16;
            }
        } else if ((ModifierType & ModifierType_Decimal) == ModifierType_Decimal) {
            Base               |= Base_Decimal;
            if ((ModifierType & ModifierType_Radix10) == ModifierType_Radix10) {
                Base           |= Base_Radix10;
            } else if ((ModifierType & ModifierType_Scientific) == ModifierType_Scientific) {
                Base           |= Base_Scientific;
            } else if ((ModifierType & ModifierType_Shortest) == ModifierType_Shortest) {
                 Base          |= Base_Shortest;
            } else if ((ModifierType & ModifierType_Radix16) == ModifierType_Radix16) {
                Base           |= Base_Radix16;
            }
        }
        if ((ModifierType & ModifierType_Uppercase) == ModifierType_Uppercase) {
            Base               |= Base_Uppercase;
        } else if ((ModifierType & ModifierType_Lowercase) == ModifierType_Lowercase) {
            Base               |= Base_Lowercase;
        }
        return Base;
    }
    
    static void FormatSpecifiers_GetNumUniqueSpecifiers(FormatSpecifiers *Specifiers) {
        if (Specifiers != NULL) {
            if (Specifiers->NumSpecifiers > 1) {
                uint64_t Specifier1 = 0ULL;
                uint64_t Specifier2 = 0ULL;
                for (Specifier1 = 0ULL; Specifier1 < Specifiers->NumSpecifiers; Specifier1++) {
                    for (Specifier2 = 0ULL; Specifier2 < Specifier1; Specifier2++) {
                        if (Specifiers->Specifiers[Specifier1].PositionFlag != Position_Unknown && Specifiers->Specifiers[Specifier1].Position == Specifiers->Specifiers[Specifier2].Position) {
                            break;
                        }
                    }
                    if (Specifier1 == Specifier2) {
                        Specifiers->NumUniqueSpecifiers += 1;
                    }
                }
                
                Specifiers->UniqueSpecifiers                                  = (uint64_t*) calloc(Specifiers->NumUniqueSpecifiers, sizeof(Specifiers->UniqueSpecifiers));
                
                if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                    uint64_t Index                                            = 1ULL;
                    uint64_t Specifier                                        = 0ULL;
                    for (uint64_t UniqueSpecifier = 0ULL; UniqueSpecifier < Specifiers->NumUniqueSpecifiers; UniqueSpecifier++) {
                        for (Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                            if (Specifiers->Specifiers[Specifier].PositionFlag != Position_Unknown && Specifiers->Specifiers[Specifier].Position == Index) {
                                Specifiers->UniqueSpecifiers[UniqueSpecifier] = Specifier;
                                Index += 1;
                                break;
                            }
                        }
                    }
                }
            } else {
                Specifiers->NumUniqueSpecifiers = 1;
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    void UTF32_ParseFormatSpecifiers(FoundationIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, FoundationIO_StringTypes StringType) {
        if (Format != NULL && Specifiers != NULL) {
            Specifiers->StringType                               = StringType;
            uint64_t CodePoint                                   = 0ULL;
            uint64_t Specifier                                   = 0ULL;
            while (Specifier < Specifiers->NumSpecifiers && Format[CodePoint] != FoundationIONULLTerminator && Specifiers->Specifiers[Specifier].Start == 0x7FFFFFFFFFFFFFFE) {
                while (Format[CodePoint] != FoundationIONULLTerminator && Specifiers->Specifiers[Specifier].Start == 0x7FFFFFFFFFFFFFFE) {
                    if (Format[CodePoint] == UTF32Character('%')) {
                        Specifiers->Specifiers[Specifier].Start  = CodePoint;
                    }
                    CodePoint                                   += 1;
                }
                
                /* Look for the Type specifier, Find End */
                while (Format[CodePoint] != FoundationIONULLTerminator && Specifiers->Specifiers[Specifier].End == 0x7FFFFFFFFFFFFFFE && Specifier < Specifiers->NumSpecifiers) {
                    switch (Format[CodePoint]) {
                        case U'a':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Radix16 | ModifierType_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'A':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Radix16 | ModifierType_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'e':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Scientific | ModifierType_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'E':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Scientific | ModifierType_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'f':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'F':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'g':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Shortest | ModifierType_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'G':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Decimal | ModifierType_Shortest | ModifierType_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = (ModifierType_Long | ModifierType_Decimal);
                            }
                            break;
                        case U'c':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_CodeUnit;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF8;
                            } else if (CodePoint >= 3 && Format[CodePoint - 3] == UTF32Character('U')) { // %U32s s = 4, 2 = 3, 3 = 2, U = 1
                                if (Format[CodePoint - 2] == UTF32Character('1') && Format[CodePoint - 1] == UTF32Character('6')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF16;
                                } else if (Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF32;
                                }
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF32;
                                        break;
                                }
                            }
                            break;
                        case U'C':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_String;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF8;
                            } else if (CodePoint >= 3 && Format[Specifiers->Specifiers[Specifier].End - 3] == UTF32Character('U')) { // %U32s s = 4, 2 = 3, 3 = 2, U = 1
                                if (Format[Specifiers->Specifiers[Specifier].End - 2] == UTF32Character('1') && Format[Specifiers->Specifiers[Specifier].End - 1] == UTF32Character('6')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF16;
                                } else if (Format[Specifiers->Specifiers[Specifier].End - 2] == UTF32Character('3') && Format[Specifiers->Specifiers[Specifier].End - 1] == UTF32Character('2')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF32;
                                }
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF32;
                                        break;
                                }
                            }
                            break;
                        case U's':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_String;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF8;
                            } else if (CodePoint >= 3 && Format[CodePoint - 3] == UTF32Character('U')) { // %U32s s = 4, 2 = 3, 3 = 2, U = 1
                                if (Format[CodePoint - 2] == UTF32Character('1') && Format[CodePoint - 1] == UTF32Character('6')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF16;
                                } else if (Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF32;
                                }
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF32;
                                        break;
                                }
                            }
                            break;
                        case U'S':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_String;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].ModifierType           = ModifierType_UTF8;
                            } else if (CodePoint >= 3 && Format[CodePoint - 3] == UTF32Character('U')) { // %U32s s = 4, 2 = 3, 3 = 2, U = 1
                                if (Format[CodePoint - 2] == UTF32Character('1') && Format[CodePoint - 1] == UTF32Character('6')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF16;
                                } else if (Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) {
                                    Specifiers->Specifiers[Specifier].ModifierStart      = CodePoint - 3;
                                    Specifiers->Specifiers[Specifier].ModifierType       = ModifierType_UTF32;
                                }
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].ModifierType   = ModifierType_UTF32;
                                        break;
                                }
                            }
                            break;
                        case U'b':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Unsigned | ModifierType_Radix2 | ModifierType_Lowercase);
                            break;
                        case U'B':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Unsigned | ModifierType_Radix2 | ModifierType_Uppercase);
                            break;
                        case U'd':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Signed | ModifierType_Radix10);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_SizeType;
                            }
                            break;
                        case U'i': // "%s in" = i? what the fuck
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Signed | ModifierType_Radix10);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_SizeType;
                            }
                            break;
                        case U'o':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Unsigned | ModifierType_Radix8);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_SizeType;
                            }
                            break;
                        case U'x':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Unsigned | ModifierType_Radix16 | ModifierType_Lowercase);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_SizeType;
                            }
                            break;
                        case U'X':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Unsigned | ModifierType_Radix16 | ModifierType_Uppercase);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_SizeType;
                            }
                            break;
                        case U'u':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].ModifierType               = (ModifierType_Integer | ModifierType_Unsigned | ModifierType_Radix10);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 3;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 2;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_SizeType;
                            }
                            break;
                        case U'p':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Pointer;
                            Specifiers->Specifiers[Specifier].ModifierType               = ModifierType_Lowercase;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('t')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_PointerDiff;
                            }
                            break;
                        case U'P':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Pointer;
                            Specifiers->Specifiers[Specifier].ModifierType               = ModifierType_Uppercase;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('t')) {
                                Specifiers->Specifiers[Specifier].ModifierStart          = CodePoint - 1;
                                Specifiers->Specifiers[Specifier].LengthModifier         = ModifierLength_PointerDiff;
                            }
                            break;
                        case U'%':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Literal;
                            Specifiers->Specifiers[Specifier].ModifierType               = ModifierType_Percent;
                            break;
                        case U'n':
                            Specifiers->Specifiers[Specifier].End                        = CodePoint;
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Remove;
                            Specifiers->Specifiers[Specifier].ModifierType               = ModifierType_N;
                            break;
                    }
                    CodePoint                                                           += 1;
                }
                
                uint64_t FlagStart  = 0ULL;
                uint64_t FlagEnd    = 0ULL;
                
                if (Specifiers->Specifiers[Specifier].ModifierStart == 0x7FFFFFFFFFFFFFFE) {
                    FlagStart       = Specifiers->Specifiers[Specifier].Start         + 1;
                    FlagEnd         = Specifiers->Specifiers[Specifier].End           - 1;
                } else {
                    FlagStart       = Specifiers->Specifiers[Specifier].Start         + 1;
                    FlagEnd         = Specifiers->Specifiers[Specifier].ModifierStart;
                }
                
                uint64_t CodePoint2 = FlagStart;
                
                while (CodePoint2 != FlagEnd) {
                    uint64_t NumDigits                                      = 0ULL;
                    switch (Format[CodePoint2]) {
                        case U' ':
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Space_Pad;
                            break;
                        case U'\'':
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Apostrophe_ExponentGroup;
                            break;
                        case U'+':
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Plus_AddSign;
                            break;
                        case U'-':
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Minus_LeftJustify;
                            break;
                        case U'.':
                            if (Format[CodePoint + 1] != FoundationIONULLTerminator && Format[CodePoint + 1] == U'*') {
                                Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Asterisk_NextArg;
                            } else if (Format[CodePoint + 1] != FoundationIONULLTerminator &&
                                      (Format[CodePoint + 1] >= UTF32Character('1') && Format[CodePoint+1] <= UTF32Character('9'))) {
                                Specifiers->Specifiers[Specifier].PrecisionFlag  = Precision_Inline_Digits;
                            }
                            break;
                        case U'0':
                            if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Integer) == BaseType_Integer || (Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                                Specifiers->Specifiers[Specifier].Flag  = Flag_Zero_Pad;
                            }
                            break;
                        case U'#':
                            if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Integer) == BaseType_Integer) {
                                Specifiers->Specifiers[Specifier].Flag      = Flag_Pound_PrefixBase;
                            } else if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal)  {
                                if ((Specifiers->Specifiers[Specifier].ModifierType & ModifierType_Shortest) == ModifierType_Shortest) {
                                    Specifiers->Specifiers[Specifier].Flag  = Flag_Pound_DecimalSuffix;
                                }
                            }
                            break;
                        case U'$':
                            if (CodePoint >= 1 && Format[CodePoint - 1] == U'*') {
                                Specifiers->Specifiers[Specifier].PositionFlag = Position_Asterisk_NextArg;
                            } else if (CodePoint >= 1 && Format[CodePoint - 1] >= UTF32Character('0') && Format[CodePoint - 1] <= UTF32Character('9')) {
                                Specifiers->Specifiers[Specifier].PositionFlag = Position_Inline_Digits;
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                uint8_t  MaxDigits              = Logarithm(10, NL_ARGMAX);
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                uint8_t  MaxDigits              = Logarithm(10, _ARGMAX);
#endif
                                uint64_t PotentialDigitLocation = CodePoint - 1;
                                while (Format[PotentialDigitLocation] != UTF32Character('%')) {
                                    PotentialDigitLocation     -= 1;
                                }
                                PotentialDigitLocation         += 1;
                                uint64_t NumPossibleCodePoints  = CodePoint - PotentialDigitLocation;
                                uint8_t  NumDigits2Read         = Minimum(NumPossibleCodePoints, MaxDigits);
                                uint64_t Value                  = UTF32_String2Integer(&Format[CodePoint - NumDigits2Read], Base_Integer | Base_Radix10);
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                if (Value <= NL_ARGMAX) {
                                    Specifiers->Specifiers[Specifier].Position = Value;
                                } else {
                                    Log(Severity_USER, FoundationIOFunctionName, UTF8String("Positional Argument: %llu is greater than NL_ARGMAX: %d"), Value, NL_ARGMAX);
                                }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                if (Value <= _ARGMAX) {
                                    Specifiers->Specifiers[Specifier].Position = Value;
                                } else {
                                    Log(Severity_USER, FoundationIOFunctionName, UTF8String("Positional Argument: %llu is greater than _ARGMAX: %d"), Value, _ARGMAX);
                                }
#endif
                            }
                            break;
                        case U'*':
                            if (CodePoint - 1 >= 0 && Format[CodePoint - 1] == U'.') {
                                Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Asterisk_NextArg;
                            } else if (Format[CodePoint + 1] != FoundationIONULLTerminator && Format[CodePoint + 1] == U'$') {
                                Specifiers->Specifiers[Specifier].PositionFlag  = Position_Asterisk_NextArg;
                            } else {
                                Specifiers->Specifiers[Specifier].MinWidthFlag  = MinWidth_Asterisk_NextArg;
                            }
                            break;
                        case U'1':
                        case U'2':
                        case U'3':
                        case U'4':
                        case U'5':
                        case U'6':
                        case U'7':
                        case U'8':
                        case U'9':
                            // Shit, we gotta account for U16 and U32 now fuck.
                            
                            
                            
                            // you know, it'd be a lot easier if we just set a variable that says where type modifiers begin so we don't have to get into the weeds on stuff like this.
                            /*
                             So basically only scan for flags, minwidth, precision, etc if Start +1 is not TypeModifierStart
                             
                             */
                            
                            
                            
                            while (Format[CodePoint2] != FoundationIONULLTerminator && Format[CodePoint2] >= UTF32Character('0') && Format[CodePoint2] <= UTF32Character('9')) {
                                CodePoint2 += 1;
                                NumDigits  += 1;
                            }
                            
                            if (CodePoint - 1 > 0 && Format[CodePoint - 1] == U'.') {
                                Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Inline_Digits;
                                Specifiers->Specifiers[Specifier].Precision     = (uint64_t) UTF32_String2Integer(&Format[CodePoint - NumDigits], Base_Integer | Base_Radix10);
                            } else if (Format[CodePoint + NumDigits + 1] != FoundationIONULLTerminator && Format[CodePoint + NumDigits + 1] == U'$') {
                                Specifiers->Specifiers[Specifier].PositionFlag  = Position_Inline_Digits;
                                Specifiers->Specifiers[Specifier].Position      = (uint64_t) UTF32_String2Integer(&Format[CodePoint - NumDigits], Base_Integer | Base_Radix10);
                            } else {
                                Specifiers->Specifiers[Specifier].MinWidthFlag  = MinWidth_Inline_Digits;
                                Specifiers->Specifiers[Specifier].MinWidth      = (uint64_t) UTF32_String2Integer(&Format[CodePoint - NumDigits], Base_Integer | Base_Radix10);
                            }
                            break;
                    }
                    CodePoint += 1;
                }
                CodePoint      = Specifiers->Specifiers[Specifier].End + 1;
                Specifier     += 1;
            }
            
            FormatSpecifiers_GetNumUniqueSpecifiers(Specifiers);
        } else if (Specifiers == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        } else if (Format == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    static UTF8 *UTF8_CodeUnit2String(UTF8 CodeUnit) {
        UTF8 *String  = UTF8_Init(1);
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    static UTF16 *UTF16_CodeUnit2String(UTF16 CodeUnit) {
        UTF16 *String  = UTF16_Init(1);
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    static UTF32 *UTF32_CodeUnit2String(UTF32 CodeUnit) {
        UTF32 *String  = UTF32_Init(1);
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    void Format_Specifiers_RetrieveArguments(FormatSpecifiers *Specifiers, va_list Arguments) {
        if (Specifiers != NULL) {
            uint64_t                 Position                              = 0ULL;
            uint64_t                 NumSpecifiers                         = 0ULL;
            FormatIO_BaseTypes       BaseType                              = BaseType_Unknown;
            FormatIO_ModifierTypes   ModifierType                          = ModifierType_Unknown;
            FormatIO_ModifierLengths LengthType                            = ModifierLength_Unknown;
            FormatIO_MinWidths       MinWidthType                          = MinWidth_Unknown;
            FormatIO_Positions       PositionType                          = Position_Unknown;
            FormatIO_Precisions      PrecisionType                         = Precision_Unknown;
            FormatIO_Flags           FlagType                              = Flag_Unknown;
            uint64_t                 UniqueSpecifier2                      = 0ULL;
            
            while (Position < Specifiers->NumUniqueSpecifiers) {
                if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                    Position                                               = Specifiers->UniqueSpecifiers[UniqueSpecifier2];
                    UniqueSpecifier2                                      += 1;
                }
                BaseType                                                   = Specifiers->Specifiers[Position].BaseType;
                ModifierType                                               = Specifiers->Specifiers[Position].ModifierType;
                LengthType                                                 = Specifiers->Specifiers[Position].LengthModifier;
                MinWidthType                                               = Specifiers->Specifiers[Position].MinWidthFlag;
                PositionType                                               = Specifiers->Specifiers[Position].PositionFlag;
                PrecisionType                                              = Specifiers->Specifiers[Position].PrecisionFlag;
                FlagType                                                   = Specifiers->Specifiers[Position].Flag;
                
                if (MinWidthType == MinWidth_Asterisk_NextArg) {
                    Specifiers->Specifiers[Position].MinWidth              = va_arg(Arguments, uint32_t);
                }
                if (PrecisionType == Precision_Asterisk_NextArg) {
                    Specifiers->Specifiers[Position].Precision             = va_arg(Arguments, uint32_t);
                }
                
                FoundationIO_Bases Base                                    = ConvertModifierType2Base(ModifierType);
                
                if ((BaseType & BaseType_Integer) == BaseType_Integer) {
                    if ((ModifierType & ModifierType_Unsigned) == ModifierType_Unsigned) {
                        if ((LengthType & ModifierLength_8Bit) == ModifierLength_8Bit) {
                            uint8_t   Arg                                  = (uint8_t) va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else if ((LengthType & ModifierLength_16Bit) == ModifierLength_16Bit) {
                            uint16_t  Arg                                  = (uint16_t) va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else if ((LengthType & ModifierLength_32Bit) == ModifierLength_32Bit) {
                            uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else if ((LengthType & ModifierLength_64Bit) == ModifierLength_64Bit) {
                            uint64_t  Arg                                  = va_arg(Arguments, uint64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else {
                            uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        }
                    } else if ((ModifierType & ModifierType_Signed) == ModifierType_Signed) {
                        if ((LengthType & ModifierLength_8Bit) == ModifierLength_8Bit) {
                            int8_t    Arg                                  = (int8_t) va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else if ((LengthType & ModifierLength_16Bit) == ModifierLength_16Bit) {
                            int16_t   Arg                                  = (int16_t) va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else if ((LengthType & ModifierLength_32Bit) == ModifierLength_32Bit) {
                            int32_t   Arg                                  = va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else if ((LengthType & ModifierLength_64Bit) == ModifierLength_64Bit) {
                            int64_t   Arg                                  = va_arg(Arguments, int64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        } else {
                            int32_t   Arg                                  = va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Arg, Base);
                        }
                    }
                } else if ((BaseType & BaseType_Decimal) == BaseType_Decimal) {
                    if ((LengthType & ModifierLength_32Bit) == ModifierLength_32Bit) {
                        float    Arg                                       = (float) va_arg(Arguments, double);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Arg, Base);
                    } else if ((LengthType & ModifierLength_64Bit) == ModifierLength_64Bit) {
                        double   Arg                                       = va_arg(Arguments, double);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Arg, Base);
                    }
                } else if ((BaseType & BaseType_CodeUnit) == BaseType_CodeUnit) {
                    if ((ModifierType & ModifierType_UTF8) == ModifierType_UTF8) {
                        UTF8  VariadicArgument                             = (UTF8) va_arg(Arguments, uint32_t);
                        UTF8 *String                                       = UTF8_CodeUnit2String(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = UTF8_Decode(String);
                        UTF8_Deinit(String);
                    } else if ((ModifierType & ModifierType_UTF16) == ModifierType_UTF16) {
                        UTF16  VariadicArgument                            = (UTF16) va_arg(Arguments, uint32_t);
                        UTF16 *String                                      = UTF16_CodeUnit2String(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = UTF16_Decode(String);
                        UTF16_Deinit(String);
                    } else if ((ModifierType & ModifierType_UTF32) == ModifierType_UTF32) {
                        UTF32  VariadicArgument                            = va_arg(Arguments, UTF32);
                        Specifiers->Specifiers[Position].Argument          = UTF32_CodeUnit2String(VariadicArgument);
                    }
                } else if ((BaseType & BaseType_String) == BaseType_String) {
                    if ((ModifierType & ModifierType_UTF8) == ModifierType_UTF8) {
                        UTF8  *VariadicArgument                            = va_arg(Arguments, UTF8*);
                        UTF32 *VariadicArgument32                          = UTF8_Decode(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = VariadicArgument32;
                    } else if ((ModifierType & ModifierType_UTF16) == ModifierType_UTF16) {
                        UTF16 *VariadicArgument                            = va_arg(Arguments, UTF16*);
                        UTF32 *VariadicArgument32                          = UTF16_Decode(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = VariadicArgument32;
                    } else if ((ModifierType & ModifierType_UTF32) == ModifierType_UTF32) {
                        Specifiers->Specifiers[Position].Argument          = va_arg(Arguments, UTF32*);
                    }
                } else if ((BaseType & BaseType_Pointer) == BaseType_Pointer) {
                    if ((ModifierType & ModifierType_Radix16) == ModifierType_Radix16) {
                        uint64_t Value                                     = va_arg(Arguments, uint64_t);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Value, Base_Integer | Base_Radix16 | Base_Uppercase);
                    } else if ((ModifierType & ModifierType_Lowercase) == ModifierType_Lowercase) {
                        uint64_t Value                                     = va_arg(Arguments, uint64_t);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Value, Base_Integer | Base_Radix16 | Base_Lowercase);
                    }
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && ModifierType == ModifierType_Percent) {
                    Specifiers->Specifiers[Position].Argument              = UTF32String("%");
                }
                if (Specifiers->NumSpecifiers == Specifiers->NumUniqueSpecifiers) {
                    Position += 1;
                }
            }
            
            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                for (uint64_t Unique = 0; Unique < Specifiers->NumUniqueSpecifiers; Unique++) {
                    uint64_t ValidArgument = Specifiers->Specifiers[Specifiers->UniqueSpecifiers[Unique]].Position;
                    for (uint64_t InvalidArgument = 0ULL; InvalidArgument < Specifiers->NumSpecifiers; InvalidArgument++) {
                        if (Specifiers->Specifiers[InvalidArgument].Position == ValidArgument && Specifiers->Specifiers[InvalidArgument].Argument == NULL) {
                            Specifiers->Specifiers[InvalidArgument].Argument = UTF32_Clone(Specifiers->Specifiers[Specifiers->UniqueSpecifiers[Unique]].Argument);
                        }
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    static uint64_t UTF32_LocateNextPercent(FoundationIO_Immutable(UTF32 *) String) {
        uint64_t NextPercent     = -1;
        if (String != FoundationIONULLTerminator) {
            uint64_t CodePoint   = 0ULL;
            while (String[CodePoint] != FoundationIONULLTerminator && String[CodePoint] != UTF32Character('%')) {
                CodePoint       += 1;
            }
            NextPercent          = CodePoint;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return NextPercent;
    }
    
    UTF32 *FormatString_UTF32(FoundationIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, uint64_t FormattedSize) {
        UTF32 *Formatted                 = NULL;
        if (Format != NULL && Specifiers != NULL) {
            Formatted                    = UTF32_Init(FormattedSize);
            if (Formatted != NULL) {
                /*
                 Ok, so the string has been allocated, what do we need to do next?
                 Copy the string until we find the %
                 */
                uint64_t Specifier             = 0ULL;
                uint64_t FormattedCodePoint    = 0ULL;
                uint64_t FormatCodePoint       = 0ULL;
                uint64_t SubstitutionCodePoint = 0ULL;
                uint64_t SubstitutionSize      = 0ULL;
                while (Specifier < Specifiers->NumSpecifiers && FormattedCodePoint < FormattedSize) {
                    // Well for each Specifier we need to load the offset and make sure that it matches, so we need 2 while loops then, one for the specifier, one for the offset
                    
                    
                    
                    UTF32_LocateNextPercent(&Format[FormatCodePoint]);
                    
                    
                    /*
                     
                     How do we support Positional Specifiers?
                     
                     */
                    
                    SubstitutionSize           = UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Specifier].Argument);
                    if (FormattedCodePoint >= Specifiers->Specifiers[Specifier].Start && FormattedCodePoint <= Specifiers->Specifiers[Specifier].End) {
                        while (Specifiers->Specifiers[Specifier].Argument[SubstitutionCodePoint] != FoundationIONULLTerminator) {
                            Formatted[FormattedCodePoint] = Specifiers->Specifiers[Specifier].Argument[SubstitutionCodePoint];
                            FormatCodePoint              += 1;
                            SubstitutionCodePoint        += 1;
                            FormattedCodePoint           += 1;
                        }
                        if ((Specifiers->Specifiers[Specifier].End - Specifiers->Specifiers[Specifier].Start) < SubstitutionSize) {
                            FormatCodePoint              += (Specifiers->Specifiers[Specifier].End - Specifiers->Specifiers[Specifier].Start) - SubstitutionSize;
                        }
                    }
                }
                
                
                
                /*
                while (FormattedCodePoint < FormattedStringSize) {
                    if (FormattedCodePoint == Offset) {
                        while (SubstitutionCodePoint < SubstitutionSize) {
                            NewString[NewCodePoint] = Substitution[SubstitutionCodePoint];
                            NewCodePoint           += 1;
                            SubstitutionCodePoint  += 1;
                        }
                        StringCodePoint            += Length;
                    } else {
                        NewString[NewCodePoint]     = String[StringCodePoint];
                        NewCodePoint               += 1;
                        StringCodePoint            += 1;
                    }
                }
                 */
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate %llu CodePoints"), FormattedSize);
            }
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            /*
            UTF32 *FormattedStrings[Specifiers->NumSpecifiers + 2];
            FormattedStrings[0]        = Format;
             */
            
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                /*
                 Deduplicate the intermediate allocations, just allocate the final string.
                 
                 Also, deduplicate positional arguments
                 */
                
                UTF32                        *Argument                      = NULL;
                
                if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers && Specifiers->Specifiers[Specifier].PositionFlag != Position_Unknown) {
                    uint64_t Pos                                            = Specifiers->UniqueSpecifiers[Specifiers->Specifiers[Specifier].Position - 1];
                    Argument                                                = Specifiers->Specifiers[Pos].Argument;
                } else {
                    Argument                                                = Specifiers->Specifiers[Specifier].Argument;
                }
                
                int64_t                       CurrentStart                  = (int64_t) Specifiers->Specifiers[Specifier].Start;
                int64_t                       NewStart                      = (int64_t) UTF32_LocateNextPercent(Format);
                int64_t                       StartEndDifference            = (int64_t) (NewStart - CurrentStart);
                int64_t                       NewEnd                        = (int64_t) (Specifiers->Specifiers[Specifier].End + StartEndDifference);
                Specifiers->Specifiers[Specifier].Start                     = NewStart;
                Specifiers->Specifiers[Specifier].End                       = NewEnd;
                uint64_t                      Start                         = Specifiers->Specifiers[Specifier].Start;
                uint64_t                      End                           = Specifiers->Specifiers[Specifier].End;
                
                FormatIO_BaseTypes     BaseType                      = Specifiers->Specifiers[Specifier].BaseType;
                FormatIO_Precisions    PrecisionType                 = Specifiers->Specifiers[Specifier].PrecisionFlag;
                FormatIO_ModifierTypes ModifierType                  = Specifiers->Specifiers[Specifier].ModifierType;
                FormatIO_MinWidths     MinWidthType                  = Specifiers->Specifiers[Specifier].MinWidthFlag;
                FormatIO_Flags         FlagType                      = Specifiers->Specifiers[Specifier].Flag;
                
                if ((BaseType & BaseType_Remove) == BaseType_Remove && (ModifierType & ModifierType_N) == ModifierType_N) {
                    //FormattedStrings[Specifier + 1]                        = UTF32_StitchSubString(FormattedStrings[Specifier], Start, End - Start);
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && (ModifierType & ModifierType_Percent) == ModifierType_Percent) {
                    //FormattedStrings[Specifier + 1]                        = UTF32_SubstituteSubString(FormattedStrings[Specifier], UTF32String("%"), Start, End - Start);
                } else {
                    if (MinWidthType != MinWidth_Unknown) {
                        uint64_t MinWidth                                  = Specifiers->Specifiers[Specifier].MinWidth;
                        uint64_t ArgSize                                   = UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Specifier].Argument);
                        uint64_t PadSize                                   = 0ULL;
                        if (ArgSize < MinWidth) {
                            PadSize                                        = MinWidth - ArgSize;
                            if (FlagType == Flag_Zero_Pad) {
                                UTF32 *OriginalArg                         = Specifiers->Specifiers[Specifier].Argument;
                                Specifiers->Specifiers[Specifier].Argument = UTF32_PadString(OriginalArg, UTF32String("0"), PadSize);
                                UTF32_Deinit(OriginalArg);
                            } else if (FlagType == Flag_Space_Pad) {
                                UTF32 *OriginalArg                         = Specifiers->Specifiers[Specifier].Argument;
                                UTF32 *Padded                              = UTF32_PadString(OriginalArg, UTF32String(" "), PadSize);
                                Specifiers->Specifiers[Specifier].Argument = UTF32_Insert(OriginalArg, Padded, 0);
                                UTF32_Deinit(OriginalArg);
                                UTF32_Deinit(Padded);
                            }
                        }
                    } else if (PrecisionType != Precision_Unknown) {
                        // Cut or extend the string so it fits
                    }
                    
                    /*
                     The second half of reducing our memory waste is we need to allocate the final string, so figure out the total size
                     
                     and the substitute each specifier, and copy from the main string up to each specifier hen quit
                     
                     So, UTF32_GetFormattedStringSize
                     */
                    
                    UTF32 *Argument                                        = Specifiers->Specifiers[Specifier].Argument;
                    
                    //FormattedStrings[Specifier + 1]                        = UTF32_SubstituteSubString(FormattedStrings[Specifier], Argument, Start, End - Start);
                   // printf("Formatted: \"%ls\"\n", (wchar_t*) FormattedStrings[Specifier + 1]);
                }
            }
            
            for (uint64_t Specifier = 1ULL; Specifier < Specifiers->NumSpecifiers - 1; Specifier++) {
                //UTF32_Deinit(FormattedStrings[Specifier]);
            }
            
            //Formatted                                   = FormattedStrings[Specifiers->NumSpecifiers];
        } else if (Specifiers == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        } else if (Format == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        }
        return Formatted;
    }
    
    UTF32 **DeformatString_UTF32(FoundationIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers, FoundationIO_Immutable(UTF32 *) Formatted) {
        UTF32 **Deformatted                            = UTF32_StringSet_Init(Specifiers->NumSpecifiers);
        if (Format != NULL && Specifiers != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                /*
                 How do wwe support Positional Specifiers?
                 */
                FormatIO_BaseTypes     BaseType = Specifiers->Specifiers[Specifier].BaseType;
                FormatIO_ModifierTypes Modifier = Specifiers->Specifiers[Specifier].ModifierType;
                uint64_t               Start    = Specifiers->Specifiers[Specifier].Start + 1;
                
                if ((BaseType & BaseType_Integer) == BaseType_Integer || (BaseType & BaseType_Decimal) == BaseType_Decimal || (BaseType & BaseType_Pointer) == BaseType_Pointer) {
                    FoundationIO_Bases  Base           = ConvertModifierType2Base(Modifier);
                    uint64_t           SubStringLength = UTF32_GetNumDigits(Formatted, Start, Base);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, Start, SubStringLength);
                } else if ((BaseType & BaseType_CodeUnit) == BaseType_CodeUnit) {
                    Deformatted[Specifier]             = UTF32_CodeUnit2String(Formatted[Start]);
                } else if ((BaseType & BaseType_String) == BaseType_String) {
                    uint64_t SubStringLength           = UTF32_GetSubStringLength(Format, Formatted, Start);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, Start, SubStringLength);
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && Modifier == ModifierType_Percent) {
                    Deformatted[Specifier]             = UTF32_Clone(UTF32String("%"));
                }
            }
        } else if (Format == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
        return Deformatted;
    }
    
    uint64_t UTF8_GetNumFormatSpecifiers(FoundationIO_Immutable(UTF8 *) String) {
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodeUnit              = 0ULL;
        if (String != NULL) {
            while (String[CodeUnit] != FoundationIONULLTerminator) {
                if (String[CodeUnit] == '%') {
                    NumSpecifiers     += 1;
                }
                CodeUnit              += 1;
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF16_GetNumFormatSpecifiers(FoundationIO_Immutable(UTF16 *) String) {
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodeUnit              = 0ULL;
        if (String != NULL) {
            while (String[CodeUnit] != FoundationIONULLTerminator) {
                if (String[CodeUnit] == UTF16Character('%')) {
                    NumSpecifiers     += 1;
                }
                CodeUnit              += 1;
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF32_GetNumFormatSpecifiers(FoundationIO_Immutable(UTF32 *) String) {
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodePoint             = 0ULL;
        if (String != NULL) {
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (String[CodePoint] == UTF32Character('%')) {
                    NumSpecifiers     += 1;
                }
                CodePoint             += 1;
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF32_GetFormattedStringSize(FoundationIO_Immutable(UTF32 *) Format, FormatSpecifiers *Specifiers) {
        uint64_t FormattedStringSize     = 0ULL;
        if (Format != NULL && Specifiers != NULL) {
            FormattedStringSize          = UTF32_GetStringSizeInCodePoints(Format);
            uint64_t SpecifierLength     = 0ULL;
            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumUniqueSpecifiers; Specifier++) {
                    uint64_t Pos         = Specifiers->UniqueSpecifiers[Specifier];
                    SpecifierLength      = Specifiers->Specifiers[Pos].End - Specifiers->Specifiers[Pos].Start;
                    FormattedStringSize -= SpecifierLength;
                }
            } else {
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    SpecifierLength      = Specifiers->Specifiers[Specifier].End - Specifiers->Specifiers[Specifier].Start;
                    FormattedStringSize -= SpecifierLength;
                }
            }
            // Now add the length of each replacement string, which needs to be duplicated
            // We need to know the number of times each positional parameter is used, basically a histogram.
            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                Specifiers->UniqueSpecifiers = (uint64_t*) calloc(Specifiers->NumUniqueSpecifiers, sizeof(Specifiers->UniqueSpecifiers));
                // Loop over all specifiers, increment .Position in PositionalSpecifierCount
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    Specifiers->UniqueSpecifiers[Specifiers->Specifiers[Specifier].Position] += 1;
                }
                for (uint64_t Unique = 0ULL; Unique < Specifiers->NumUniqueSpecifiers; Unique++) {
                    uint64_t Pos          = Specifiers->UniqueSpecifiers[Unique];
                    FormattedStringSize  += (UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Pos].Argument) * Specifiers->UniqueSpecifiers[Unique]); // Feels like this is wrong
                }
            } else {
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    FormattedStringSize += UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Specifier].Argument);
                }
            }
        } else if (Format == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
        return FormattedStringSize;
    }
    
    UTF8 *UTF8_Format(FoundationIO_Immutable(UTF8 *) Format, ...) {
        UTF8 *Format8                        = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers           = UTF8_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                /*
                Lets try to minimize allocations
                 FormatString: "Hush little baby don't say a word, %1$s's gonna buy you a Mockingbird; And if that Mockingbird don't s%2$s, %1$s's gonna buy you a Diamond r%2$s; and if that Diamond r%2$s turns br%3$s, %1$s's gonna buy you a looking gl%3$s"
                 FormatStringSize: 223
                 NumSpecifiers: 8
                 SpecifierSizes: {4, 4, 4, 4, 4, 4, 4, 4} = 32
                 
                 ArgumentStrings: {"Popa", "ing", "ass"}
                 Arguments: {1, 2, 1, 2, 2, 3, 1, 3}
                 ArgumentSizes: 12 + 9 + 6 = 27
                 32 - 27 = 5
                 FormattedStringSize: 223 - 5 = 218
                 So allocate a single string 218 codepoints in size, and manually copy in the parts needed when they're needed
                 
                 Let's wait until we know we need to optimize allocations before we do so
                */
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32 *Format32              = UTF8_Decode(Format);
                UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF8);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                Format_Specifiers_RetrieveArguments(Specifiers, VariadicArguments);
                va_end(VariadicArguments);
                uint64_t FormattedStringSize = UTF32_GetFormattedStringSize(Format32, Specifiers);
                UTF32 *FormattedString       = FormatString_UTF32(Format32, Specifiers, FormattedStringSize);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                Format8                      = UTF8_Encode(FormattedString);
                printf("UTF-8: \"%s\"\n", Format8);
                UTF32_Deinit(FormattedString);
            } else {
                Format8                      = UTF8_Clone(Format);
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return Format8;
    }
    
    UTF16 *UTF16_Format(FoundationIO_Immutable(UTF16 *) Format, ...) {
        UTF16 *Format16                      = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers           = UTF16_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32              = UTF16_Decode(Format);
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF16);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                Format_Specifiers_RetrieveArguments(Specifiers, VariadicArguments);
                va_end(VariadicArguments);
                uint64_t FormattedStringSize = UTF32_GetFormattedStringSize(Format32, Specifiers);
                UTF32 *FormattedString       = FormatString_UTF32(Format32, Specifiers, FormattedStringSize);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                Format16                     = UTF16_Encode(FormattedString);
                UTF32_Deinit(FormattedString);
            } else {
                Format16                     = UTF16_Clone(Format);
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return Format16;
    }
    
    UTF32 *UTF32_Format(FoundationIO_Immutable(UTF32 *) Format, ...) {
        UTF32 *FormattedString               = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers           = UTF32_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format, Specifiers, StringType_UTF32);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                Format_Specifiers_RetrieveArguments(Specifiers, VariadicArguments);
                va_end(VariadicArguments);
                uint64_t FormattedStringSize = UTF32_GetFormattedStringSize(Format, Specifiers);
                FormattedString              = FormatString_UTF32(Format, Specifiers, FormattedStringSize);
                FormatSpecifiers_Deinit(Specifiers);
            } else {
                FormattedString              = UTF32_Clone(Format);
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return FormattedString;
    }
    
    UTF8 **UTF8_Deformat(UTF8 *Format, UTF8 *Formatted) {
        UTF8 **StringSet                   = NULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t NumSpecifiers           = UTF8_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FoundationIO_Immutable(UTF32 *) Format32    = UTF8_Decode(Format);
                FormatSpecifiers *Specifiers                = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF8);
                FoundationIO_Immutable(UTF32 *) Formatted32 = UTF8_Decode(Formatted);
                UTF32 **Strings32                           = DeformatString_UTF32(Format32, Specifiers, Formatted32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit((UTF32*) Format32);
                UTF32_Deinit((UTF32*) Formatted32);
                StringSet                  = UTF8_StringSet_Encode((FoundationIO_Immutable(UTF32**)) Strings32);
                UTF32_StringSet_Deinit(Strings32);
            }
        } else if (Format == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Formatted String is NULL"));
        }
        return StringSet;
    }
    
    UTF16 **UTF16_Deformat(UTF16 *Format, UTF16 *Formatted) {
        UTF16 **StringSet                    = NULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t NumSpecifiers           = UTF16_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32              = UTF16_Decode(Format);
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF16);
                UTF32 *Formatted32           = UTF16_Decode(Formatted);
                UTF32 **Strings32            = DeformatString_UTF32(Format32, Specifiers, Formatted32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                UTF32_Deinit(Formatted32);
                StringSet                  = UTF16_StringSet_Encode((FoundationIO_Immutable(UTF32**)) Strings32);
                UTF32_StringSet_Deinit(Strings32);
            }
        } else if (Format == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Formatted String is NULL"));
        }
        return StringSet;
    }
    
    UTF32 **UTF32_Deformat(FoundationIO_Immutable(UTF32 *) Format, FoundationIO_Immutable(UTF32 *) Formatted) {
        UTF32 **StringSet                  = NULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t NumSpecifiers           = UTF32_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format, Specifiers, StringType_UTF32);
                StringSet                    = DeformatString_UTF32(Format, Specifiers, Formatted);
                FormatSpecifiers_Deinit(Specifiers);
            }
        } else if (Format == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Formatted String is NULL"));
        }
        return StringSet;
    }
    
#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
}
#endif
