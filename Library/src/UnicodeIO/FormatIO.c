#include "../../include/MathIO.h"                      /* Included for Logarithm */
#include "../../include/Constants.h"                   /* Included for the Number tables */
#include "../../include/UnicodeIO/FormatIO.h"          /* Included for our declarations */
#include "../../include/UnicodeIO/LogIO.h"             /* Included for Logging */
#include "../../include/UnicodeIO/StringIO.h"          /* Included for StringIO */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum FormatSpecifier_TypeModifiers {
        Modifier_Unknown               = 0,
        Modifier_Percent               = 1,
        Modifier_UTF8                  = 2,
        Modifier_UTF16                 = 4,
        Modifier_UTF32                 = 8,
        Modifier_Integer               = 16,
        Modifier_Radix2                = 32,
        Modifier_Radix8                = 64,
        Modifier_Decimal               = 128,
        Modifier_Radix10               = 256,
        Modifier_Radix16               = 512,
        Modifier_Uppercase             = 1024,
        Modifier_Lowercase             = 2048,
        Modifier_Scientific            = 4096,
        Modifier_Shortest              = 8192,
        Modifier_Long                  = 16384,
        Modifier_Signed                = 32768,
        Modifier_Unsigned              = 65536,
        Modifier_N                     = 131072,
    } FormatSpecifier_TypeModifiers;
    
    typedef enum FormatSpecifier_Flags {
        Flag_Unknown                   = 0,
        Flag_Zero_Pad                  = 1,
        Flag_Space_Pad                 = 2,
        Flag_Plus_AddSign              = 4,
        Flag_Pound_Present             = 8,
        Flag_Minus_LeftJustify         = 16,
        Flag_Pound_PrefixOctal         = 32,
        Flag_Pound_PrefixBase          = 64,
        Flag_Pound_DecimalSuffix       = 128,
        Flag_Apostrophe_Present        = 256,
        Flag_Apostrophe_ExponentGroup  = 512,
    } FormatSpecifier_Flags;
    
    typedef enum FormatSpecifier_BaseTypes {
        BaseType_Unknown               = 0,
        BaseType_Integer               = 1,
        BaseType_Decimal               = 2,
        BaseType_CodeUnit              = 4,
        BaseType_String                = 8,
        BaseType_Literal               = 16,
        BaseType_Pointer               = 32,
        BaseType_Remove                = 64,
    } FormatSpecifier_BaseTypes;
    
    typedef enum FormatSpecifier_LengthModifiers {
        Length_Unknown                 = 0,
        Length_8Bit                    = 1,
        Length_16Bit                   = 2,
        Length_32Bit                   = 4,
        Length_64Bit                   = 8,
        Length_SizeType                = 16,
        Length_PointerDiff             = 32,
        Length_IntMax                  = 64,
    } FormatSpecifier_LengthModifiers;
    
    typedef enum FormatSpecifier_MinWidths {
        MinWidth_Unknown               = 0,
        MinWidth_Asterisk_NextArg      = 1,
        MinWidth_Inline_Digits         = 2,
    } FormatSpecifier_MinWidths;
    
    typedef enum FormatSpecifier_Positions {
        Position_Unknown               = 0,
        Position_Asterisk_NextArg      = 1,
        Position_Inline_Digits         = 2,
    } FormatSpecifier_Positions;
    
    typedef enum FormatSpecifier_Precisions {
        Precision_Unknown              = 0,
        Precision_Asterisk_NextArg     = 1,
        Precision_Inline_Digits        = 2,
    } FormatSpecifier_Precisions;
    
    typedef struct FormatSpecifier {
        UTF32                          *Argument;
        uint64_t                        Start;
        uint64_t                        End;
        uint64_t                        MinWidth;
        uint64_t                        Precision;
        uint64_t                        Position;
        FormatSpecifier_TypeModifiers   TypeModifier;
        FormatSpecifier_Flags           Flag;
        FormatSpecifier_BaseTypes       BaseType;
        FormatSpecifier_LengthModifiers LengthModifier;
        FormatSpecifier_MinWidths       MinWidthFlag;
        FormatSpecifier_Precisions      PrecisionFlag;
        FormatSpecifier_Positions       PositionFlag;
    } FormatSpecifier;
    
    typedef struct FormatSpecifiers {
        FormatSpecifier     *Specifiers;
        uint64_t             NumSpecifiers;
        uint64_t             NumUniqueSpecifiers;
        StringIOStringTypes  StringType;
    } FormatSpecifiers;
    
    void FormatSpecifiers_Deinit(FormatSpecifiers *Specifiers) {
        if (Specifiers != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                UTF32_Deinit(Specifiers->Specifiers[Specifier].Argument);
            }
            free(Specifiers->Specifiers);
            free(Specifiers);
        }
    }
    
    FormatSpecifiers *FormatSpecifiers_Init(uint64_t NumSpecifiers) {
        FormatSpecifiers *Specifiers                         = (FormatSpecifiers*) calloc(1, sizeof(FormatSpecifiers));
        if (Specifiers != NULL) {
            Specifiers->Specifiers                           = (FormatSpecifier*) calloc(NumSpecifiers, sizeof(FormatSpecifier));
            if (Specifiers->Specifiers != NULL) {
                Specifiers->NumSpecifiers                    = NumSpecifiers;
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    Specifiers->Specifiers[Specifier].Start = 0x7FFFFFFFFFFFFFFE;
                    Specifiers->Specifiers[Specifier].End   = 0x7FFFFFFFFFFFFFFE;
                }
            } else {
                FormatSpecifiers_Deinit(Specifiers);
                Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate %llu Specifiers"), NumSpecifiers);
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate FormatSpecifiers"));
        }
        return Specifiers;
    }
    
    static FoundationIOBases ConvertTypeModifier2Base(FormatSpecifier_TypeModifiers TypeModifier) {
        FoundationIOBases Base  = Base_Unknown;
        if ((TypeModifier & Modifier_Integer) == Modifier_Integer) {
            Base               |= Base_Integer;
            if ((TypeModifier & Modifier_Radix2) == Modifier_Radix2) {
                Base           |= Base_Radix2;
            } else if ((TypeModifier & Modifier_Radix8) == Modifier_Radix8) {
                Base           |= Base_Radix8;
            } else if ((TypeModifier & Modifier_Radix10) == Modifier_Radix10) {
                Base           |= Base_Radix10;
            } else if ((TypeModifier & Modifier_Radix16) == Modifier_Radix16) {
                Base           |= Base_Radix16;
            }
        } else if ((TypeModifier & Modifier_Decimal) == Modifier_Decimal) {
            Base               |= Base_Decimal;
            if ((TypeModifier & Modifier_Radix10) == Modifier_Radix10) {
                Base           |= Base_Radix10;
            } else if ((TypeModifier & Modifier_Scientific) == Modifier_Scientific) {
                Base           |= Base_Scientific;
            } else if ((TypeModifier & Modifier_Shortest) == Modifier_Shortest) {
                 Base          |= Base_Shortest;
            } else if ((TypeModifier & Modifier_Radix16) == Modifier_Radix16) {
                Base           |= Base_Radix16;
            }
        }
        if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
            Base               |= Base_Uppercase;
        } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
            Base               |= Base_Lowercase;
        }
        return Base;
    }
    
    void UTF32_ParseFormatSpecifiers(UTF32 *Format, FormatSpecifiers *Specifiers, StringIOStringTypes StringType) {
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
                while (Format[CodePoint] != FoundationIONULLTerminator && Specifiers->Specifiers[Specifier].End == 0x7FFFFFFFFFFFFFFE) {
                    switch (Format[CodePoint]) {
                        case UTF32Character('a'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Radix16 | Modifier_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('A'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Radix16 | Modifier_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('e'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Scientific | Modifier_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('E'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Scientific | Modifier_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('f'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('F'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('g'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Shortest | Modifier_Lowercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('G'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Decimal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Decimal | Modifier_Shortest | Modifier_Uppercase);
                            if (CodePoint > 1 && (Format[CodePoint - 1] == UTF32Character('L') || Format[CodePoint - 1] == UTF32Character('l'))) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = (FormatSpecifier_LengthModifiers) (Modifier_Long | Modifier_Decimal);
                            }
                            break;
                        case UTF32Character('c'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_CodeUnit;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF8;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('u')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF32;
                                        break;
                                }
                            }
                            break;
                        case UTF32Character('C'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_String;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF8;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('u')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF32;
                                        break;
                                }
                            }
                            break;
                        case UTF32Character('s'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_String;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF8;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('u')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF32;
                                        break;
                                }
                            }
                            break;
                        case UTF32Character('S'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_String;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('l')) {
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
#endif
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('w')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('h')) { // Windows
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF8;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('u')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF16;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier           = Modifier_UTF32;
                            } else {
                                switch (Specifiers->StringType) {
                                    case StringType_Unknown: // Literally can't happen, here just to get rid of the warning
                                    case StringType_UTF8:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF8;
                                        break;
                                    case StringType_UTF16:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF16;
                                        break;
                                    case StringType_UTF32:
                                        Specifiers->Specifiers[Specifier].TypeModifier   = Modifier_UTF32;
                                        break;
                                }
                            }
                            break;
                        case UTF32Character('b'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Unsigned | Modifier_Radix2 | Modifier_Lowercase);
                            break;
                        case UTF32Character('B'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Unsigned | Modifier_Radix2 | Modifier_Uppercase);
                            break;
                        case UTF32Character('d'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Signed | Modifier_Radix10);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_SizeType;
                            }
                            break;
                        case UTF32Character('i'): // "%s in" = i? what the fuck
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Signed | Modifier_Radix10);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_SizeType;
                            }
                            break;
                        case UTF32Character('o'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Unsigned | Modifier_Radix8);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_SizeType;
                            }
                            break;
                        case UTF32Character('x'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Unsigned | Modifier_Radix16 | Modifier_Lowercase);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_SizeType;
                            }
                            break;
                        case UTF32Character('X'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Integer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Unsigned | Modifier_Radix16 | Modifier_Uppercase);
                            if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_SizeType;
                            }
                            break;
                        case UTF32Character('u'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            if (Format[CodePoint + 1] != FoundationIONULLTerminator && (Format[CodePoint + 1] == UTF32Character('c') || Format[CodePoint + 1] == UTF32Character('C'))) {
                                Specifiers->Specifiers[Specifier].End               += 1;
                                Specifiers->Specifiers[Specifier].BaseType           = BaseType_CodeUnit;
                                Specifiers->Specifiers[Specifier].TypeModifier       = Modifier_UTF16;
                            } else if (Format[CodePoint + 1] != FoundationIONULLTerminator && (Format[CodePoint + 1] == UTF32Character('s') || Format[CodePoint + 1] == UTF32Character('S'))) {
                                Specifiers->Specifiers[Specifier].End               += 1;
                                Specifiers->Specifiers[Specifier].BaseType           = BaseType_String;
                                Specifiers->Specifiers[Specifier].TypeModifier       = Modifier_UTF16;
                            } else {
                                Specifiers->Specifiers[Specifier].BaseType           = BaseType_Integer;
                                Specifiers->Specifiers[Specifier].TypeModifier       = (FormatSpecifier_TypeModifiers) (Modifier_Integer | Modifier_Unsigned | Modifier_Radix10);
                                if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('3') && Format[CodePoint - 1] == UTF32Character('2')) { // Microsoft Extension
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_32Bit;
                                } else if (CodePoint > 3 && Format[CodePoint - 3] == UTF32Character('I') && Format[CodePoint - 2] == UTF32Character('6') && Format[CodePoint - 1] == UTF32Character('4')) { // Microsoft Extension
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_64Bit;
                                } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_64Bit;
                                } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_32Bit;
                                } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_16Bit;
                                } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_8Bit;
                                } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_IntMax;
                                } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                    Specifiers->Specifiers[Specifier].LengthModifier = Length_SizeType;
                                }
                            }
                            break;
                        case UTF32Character('p'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Pointer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = Modifier_Lowercase;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('t')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_PointerDiff;
                            }
                            break;
                        case UTF32Character('P'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Pointer;
                            Specifiers->Specifiers[Specifier].TypeModifier               = Modifier_Uppercase;
                            if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('t')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_PointerDiff;
                            }
                            break;
                        case UTF32Character('%'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Literal;
                            Specifiers->Specifiers[Specifier].TypeModifier               = Modifier_Percent;
                            break;
                        case UTF32Character('n'):
                            Specifiers->Specifiers[Specifier].End                        = CodePoint + 1; // +1 to account for the %
                            Specifiers->Specifiers[Specifier].BaseType                   = BaseType_Remove;
                            Specifiers->Specifiers[Specifier].TypeModifier               = Modifier_N;
                            if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_64Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('l') && Format[CodePoint - 1] == UTF32Character('l')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_32Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] != UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_16Bit;
                            } else if (CodePoint > 2 && Format[CodePoint - 2] == UTF32Character('h') && Format[CodePoint - 1] == UTF32Character('h')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_8Bit;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('j')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_IntMax;
                            } else if (CodePoint > 1 && Format[CodePoint - 1] == UTF32Character('z')) {
                                Specifiers->Specifiers[Specifier].LengthModifier         = Length_SizeType;
                            }
                            break;
                    }
                    CodePoint                                                           += 1;
                }
                
                uint64_t CodePoint2        = 0ULL;
                uint64_t NumDigits         = 0ULL;
                CodePoint                  = Specifiers->Specifiers[Specifier].Start + 1;
                while (CodePoint < 0x7FFFFFFFFFFFFFFE && Format[CodePoint] != FoundationIONULLTerminator && CodePoint < Specifiers->Specifiers[Specifier].End) {
                    switch (Format[CodePoint]) {
                        case UTF32Character(' '):
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Space_Pad;
                            break;
                        case UTF32Character('\''):
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Apostrophe_ExponentGroup;
                            break;
                        case UTF32Character('+'):
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Plus_AddSign;
                            break;
                        case UTF32Character('-'):
                            Specifiers->Specifiers[Specifier].Flag          = Flag_Minus_LeftJustify;
                            break;
                        case UTF32Character('.'):
                            Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Inline_Digits;
                            break;
                        case UTF32Character('0'):
                            if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Integer) == BaseType_Integer || (Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                                Specifiers->Specifiers[Specifier].Flag  = Flag_Zero_Pad;
                            }
                            break;
                        case UTF32Character('#'):
                            if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Integer) == BaseType_Integer) {
                                Specifiers->Specifiers[Specifier].Flag      = Flag_Pound_PrefixBase;
                            } else if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal)  {
                                if ((Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Shortest) == Modifier_Shortest) {
                                    Specifiers->Specifiers[Specifier].Flag  = Flag_Pound_DecimalSuffix;
                                }
                            }
                            break;
                        case UTF32Character('$'):
                            if (CodePoint >= 1 && Format[CodePoint - 1] == UTF32Character('*')) {
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
                                uint64_t Value                  = UTF32_String2Integer(Base_Integer | Base_Radix10, &Format[CodePoint - NumDigits2Read]);
#if   ((FoundationIOTargetOS & FoundationIOPOSIXOS) == FoundationIOPOSIXOS)
                                if (Value <= NL_ARGMAX) {
                                    Specifiers->Specifiers[Specifier].Position = Value;
                                } else {
                                    Log(Log_USER, FoundationIOFunctionName, UTF8String("Positional Argument: %llu is greater than NL_ARGMAX: %d"), Value, NL_ARGMAX);
                                }
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                                if (Value <= _ARGMAX) {
                                    Specifiers->Specifiers[Specifier].Position = Value;
                                } else {
                                    Log(Log_USER, FoundationIOFunctionName, UTF8String("Positional Argument: %llu is greater than _ARGMAX: %d"), Value, _ARGMAX);
                                }
#endif
                            }
                            break;
                        case UTF32Character('*'):
                            if (CodePoint - 1 >= 0 && Format[CodePoint - 1] == UTF32Character('.')) {
                                Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Asterisk_NextArg;
                            } else if (Format[CodePoint + 1] != FoundationIONULLTerminator && Format[CodePoint + 1] == UTF32Character('$')) {
                                Specifiers->Specifiers[Specifier].PositionFlag  = Position_Asterisk_NextArg;
                            } else {
                                Specifiers->Specifiers[Specifier].MinWidthFlag  = MinWidth_Asterisk_NextArg;
                            }
                            break;
                        case UTF32Character('1'):
                        case UTF32Character('2'):
                        case UTF32Character('3'):
                        case UTF32Character('4'):
                        case UTF32Character('5'):
                        case UTF32Character('6'):
                        case UTF32Character('7'):
                        case UTF32Character('8'):
                        case UTF32Character('9'):
                            while (Format[CodePoint2] != FoundationIONULLTerminator && Format[CodePoint2] >= UTF32Character('0') && Format[CodePoint2] <= UTF32Character('9')) {
                                CodePoint2 += 1;
                                NumDigits  += 1;
                            }
                            
                            if (CodePoint - 1 > 0 && Format[CodePoint - 1] == UTF32Character('.')) {
                                Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Inline_Digits;
                                Specifiers->Specifiers[Specifier].Precision     = (uint64_t) UTF32_String2Integer(Base_Integer | Base_Radix10, &Format[CodePoint - NumDigits]);
                            } else if (Format[CodePoint + NumDigits + 1] != FoundationIONULLTerminator && Format[CodePoint + NumDigits + 1] == UTF32Character('$')) {
                                Specifiers->Specifiers[Specifier].PositionFlag  = Position_Inline_Digits;
                                Specifiers->Specifiers[Specifier].Position      = (uint64_t) UTF32_String2Integer(Base_Integer | Base_Radix10, &Format[CodePoint - NumDigits]);
                            } else {
                                Specifiers->Specifiers[Specifier].MinWidthFlag  = MinWidth_Inline_Digits;
                                Specifiers->Specifiers[Specifier].MinWidth      = (uint64_t) UTF32_String2Integer(Base_Integer | Base_Radix10, &Format[CodePoint - NumDigits]);
                            }
                            break;
                    }
                    CodePoint += 1;
                }
                CodePoint      = Specifiers->Specifiers[Specifier].End + 1;
                Specifier     += 1;
            }
            
            uint64_t Specifier1 = 0;
            uint64_t Specifier2 = 0;
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
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        } else if (Format == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
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
            uint64_t                        Position                       = 0ULL;
            uint64_t                        NumSpecifiers                  = 0ULL;
            FormatSpecifier_BaseTypes       BaseType                       = BaseType_Unknown;
            FormatSpecifier_TypeModifiers   ModifierType                   = Modifier_Unknown;
            FormatSpecifier_LengthModifiers LengthType                     = Length_Unknown;
            FormatSpecifier_MinWidths       MinWidthType                   = MinWidth_Unknown;
            FormatSpecifier_Positions       PositionType                   = Position_Unknown;
            FormatSpecifier_Precisions      PrecisionType                  = Precision_Unknown;
            FormatSpecifier_Flags           FlagType                       = Flag_Unknown;
            uint64_t UniqueSpecifier2                                      = 0ULL;
            uint64_t UniqueSpecifiers[Specifiers->NumUniqueSpecifiers];
            
            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                NumSpecifiers                                              = Specifiers->NumUniqueSpecifiers;
                uint64_t Index = 1ULL;
                uint64_t Specifier = 0ULL;
                for (uint64_t UniqueSpecifier = 0ULL; UniqueSpecifier < Specifiers->NumUniqueSpecifiers; UniqueSpecifier++) {
                    for (Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                        if (Specifiers->Specifiers[Specifier].PositionFlag != Position_Unknown && Specifiers->Specifiers[Specifier].Position == Index) {
                            UniqueSpecifiers[UniqueSpecifier] = Specifier;
                            Index += 1;
                            break;
                        }
                    }
                }
            }
            
            while (Position < Specifiers->NumUniqueSpecifiers) {
                if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                    Position                                               = UniqueSpecifiers[UniqueSpecifier2];
                    UniqueSpecifier2                                      += 1;
                }
                BaseType                                                   = Specifiers->Specifiers[Position].BaseType;
                ModifierType                                               = Specifiers->Specifiers[Position].TypeModifier;
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
                
                FoundationIOBases Base                                     = ConvertTypeModifier2Base(ModifierType);
                
                if ((BaseType & BaseType_Integer) == BaseType_Integer) {
                    if ((ModifierType & Modifier_Unsigned) == Modifier_Unsigned) {
                        if ((LengthType & Length_8Bit) == Length_8Bit) {
                            uint8_t   Arg                                  = (uint8_t) va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((LengthType & Length_16Bit) == Length_16Bit) {
                            uint16_t  Arg                                  = (uint16_t) va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((LengthType & Length_32Bit) == Length_32Bit) {
                            uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((LengthType & Length_64Bit) == Length_64Bit) {
                            uint64_t  Arg                                  = va_arg(Arguments, uint64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else {
                            uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        }
                    } else if ((ModifierType & Modifier_Signed) == Modifier_Signed) {
                        if ((LengthType & Length_8Bit) == Length_8Bit) {
                            int8_t    Arg                                  = (int8_t) va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((LengthType & Length_16Bit) == Length_16Bit) {
                            int16_t   Arg                                  = (int16_t) va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((LengthType & Length_32Bit) == Length_32Bit) {
                            int32_t   Arg                                  = va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((LengthType & Length_64Bit) == Length_64Bit) {
                            int64_t   Arg                                  = va_arg(Arguments, int64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else {
                            int32_t   Arg                                  = va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        }
                    }
                } else if ((BaseType & BaseType_Decimal) == BaseType_Decimal) {
                    if ((LengthType & Length_32Bit) == Length_32Bit) {
                        float    Arg                                       = (float) va_arg(Arguments, double);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, Arg);
                    } else if ((LengthType & Length_64Bit) == Length_64Bit) {
                        double   Arg                                       = va_arg(Arguments, double);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, Arg);
                    }
                } else if ((BaseType & BaseType_CodeUnit) == BaseType_CodeUnit) {
                    if ((ModifierType & Modifier_UTF8) == Modifier_UTF8) {
                        UTF8  VariadicArgument                             = (UTF8) va_arg(Arguments, uint32_t);
                        UTF8 *String                                       = UTF8_CodeUnit2String(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = UTF8_Decode(String);
                        UTF8_Deinit(String);
                    } else if ((ModifierType & Modifier_UTF16) == Modifier_UTF16) {
                        UTF16  VariadicArgument                            = (UTF16) va_arg(Arguments, uint32_t);
                        UTF16 *String                                      = UTF16_CodeUnit2String(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = UTF16_Decode(String);
                        UTF16_Deinit(String);
                    } else if ((ModifierType & Modifier_UTF32) == Modifier_UTF32) {
                        UTF32  VariadicArgument                            = va_arg(Arguments, UTF32);
                        Specifiers->Specifiers[Position].Argument          = UTF32_CodeUnit2String(VariadicArgument);
                    }
                } else if ((BaseType & BaseType_String) == BaseType_String) {
                    if ((ModifierType & Modifier_UTF8) == Modifier_UTF8) {
                        UTF8  *VariadicArgument                            = va_arg(Arguments, UTF8*);
                        UTF32 *VariadicArgument32                          = UTF8_Decode(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = VariadicArgument32;
                    } else if ((ModifierType & Modifier_UTF16) == Modifier_UTF16) {
                        UTF16 *VariadicArgument                            = va_arg(Arguments, UTF16*);
                        UTF32 *VariadicArgument32                          = UTF16_Decode(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = VariadicArgument32;
                    } else if ((ModifierType & Modifier_UTF32) == Modifier_UTF32) {
                        Specifiers->Specifiers[Position].Argument          = va_arg(Arguments, UTF32*);
                    }
                } else if ((BaseType & BaseType_Pointer) == BaseType_Pointer) {
                    if ((ModifierType & Modifier_Uppercase) == Modifier_Uppercase) {
                        uint64_t Value                                     = va_arg(Arguments, uint64_t);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Base_Integer | Base_Radix16 | Base_Uppercase, Value);
                    } else if ((ModifierType & Modifier_Lowercase) == Modifier_Lowercase) {
                        uint64_t Value                                     = va_arg(Arguments, uint64_t);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Base_Integer | Base_Radix16 | Base_Lowercase, Value);
                    }
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && ModifierType == Modifier_Percent) {
                    Specifiers->Specifiers[Position].Argument              = UTF32String("%");
                }
                if (Specifiers->NumSpecifiers == Specifiers->NumUniqueSpecifiers) {
                    Position += 1;
                }
            }
            
            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                for (uint64_t Unique = 0; Unique < Specifiers->NumUniqueSpecifiers; Unique++) {
                    uint64_t ValidArgument = Specifiers->Specifiers[UniqueSpecifiers[Unique]].Position;
                    for (uint64_t InvalidArgument = 0ULL; InvalidArgument < Specifiers->NumSpecifiers; InvalidArgument++) {
                        if (Specifiers->Specifiers[InvalidArgument].Position == ValidArgument && Specifiers->Specifiers[InvalidArgument].Argument == NULL) {
                            Specifiers->Specifiers[InvalidArgument].Argument = UTF32_Clone(Specifiers->Specifiers[UniqueSpecifiers[Unique]].Argument);
                        }
                    }
                }
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    static uint64_t UTF32_LocateFirstPercent(UTF32 *String) {
        uint64_t FirstPercent    = -1;
        if (String != FoundationIONULLTerminator) {
            uint64_t CodePoint   = 0ULL;
            while (String[CodePoint] != FoundationIONULLTerminator) {
                if (String[CodePoint] == UTF32Character('%')) {
                    FirstPercent = CodePoint;
                    break;
                }
                CodePoint       += 1;
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return FirstPercent;
    }
    
    UTF32 *FormatString_UTF32(FormatSpecifiers *Specifiers, UTF32 *Format) {
        UTF32 *Formatted               = NULL;
        if (Format != NULL && Specifiers != NULL) {
            UTF32 *FormattedStrings[Specifiers->NumSpecifiers + 2];
            FormattedStrings[0]        = Format;
            
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                /*
                 Find the codepoint that contains '%', record the location.
                 
                 
                 
                 */
                int64_t                       CurrentStart       = (int64_t) Specifiers->Specifiers[Specifier].Start;
                int64_t                       NewStart           = (int64_t) UTF32_LocateFirstPercent(FormattedStrings[Specifier]);
                int64_t                       StartEndDifference = (int64_t) (NewStart - CurrentStart);
                Specifiers->Specifiers[Specifier].Start          = NewStart;
                Specifiers->Specifiers[Specifier].End           += (int64_t) StartEndDifference;
                uint64_t                      Start              = Specifiers->Specifiers[Specifier].Start;
                uint64_t                      End                = Specifiers->Specifiers[Specifier].End;
                
                FormatSpecifier_BaseTypes     BaseType           = Specifiers->Specifiers[Specifier].BaseType;
                FormatSpecifier_Precisions    PrecisionType      = Specifiers->Specifiers[Specifier].PrecisionFlag;
                FormatSpecifier_TypeModifiers ModifierType       = Specifiers->Specifiers[Specifier].TypeModifier;
                FormatSpecifier_MinWidths     MinWidthType       = Specifiers->Specifiers[Specifier].MinWidthFlag;
                FormatSpecifier_Flags         FlagType           = Specifiers->Specifiers[Specifier].Flag;
                
                if ((BaseType & BaseType_Remove) == BaseType_Remove && (ModifierType & Modifier_N) == Modifier_N) {
                    FormattedStrings[Specifier + 1]             = UTF32_StitchSubString(FormattedStrings[Specifier], Start, End - Start);
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && (ModifierType & Modifier_Percent) == Modifier_Percent) {
                    FormattedStrings[Specifier + 1]             = UTF32_SubstituteSubString(FormattedStrings[Specifier], UTF32String("%"), Start, End - Start);
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
                    
                    UTF32 *Argument                                        = Specifiers->Specifiers[Specifier].Argument;
                    
                    FormattedStrings[Specifier + 1]                        = UTF32_SubstituteSubString(FormattedStrings[Specifier], Argument, Start, End - Start);
                    printf("Formatted: \"%ls\"\n", (wchar_t*) FormattedStrings[Specifier + 1]);
                }
            }
            
            for (uint64_t Specifier = 1ULL; Specifier < Specifiers->NumSpecifiers - 1; Specifier++) {
                UTF32_Deinit(FormattedStrings[Specifier]);
            }
            
            Formatted                                   = FormattedStrings[Specifiers->NumSpecifiers];
        } else if (Format == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
        return Formatted;
    }
    
    UTF32 **DeformatString_UTF32(FormatSpecifiers *Specifiers, UTF32 *Format, UTF32 *Formatted) {
        UTF32 **Deformatted                            = UTF32_StringArray_Init(Specifiers->NumSpecifiers);
        if (Deformatted != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                FormatSpecifier_BaseTypes     BaseType = Specifiers->Specifiers[Specifier].BaseType;
                FormatSpecifier_TypeModifiers Modifier = Specifiers->Specifiers[Specifier].TypeModifier;
                uint64_t                      Start    = Specifiers->Specifiers[Specifier].Start + 1;
                
                if ((BaseType & BaseType_Integer) == BaseType_Integer || (BaseType & BaseType_Decimal) == BaseType_Decimal || (BaseType & BaseType_Pointer) == BaseType_Pointer) {
                    FoundationIOBases  Base            = ConvertTypeModifier2Base(Modifier);
                    uint64_t           SubStringLength = UTF32_GetNumDigits(Base, Formatted, Start);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, Start, SubStringLength);
                } else if ((BaseType & BaseType_CodeUnit) == BaseType_CodeUnit) {
                    Deformatted[Specifier]             = UTF32_CodeUnit2String(Formatted[Start]);
                } else if ((BaseType & BaseType_String) == BaseType_String) {
                    uint64_t SubStringLength           = UTF32_GetSubStringLength(Format, Formatted, Start);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, Start, SubStringLength);
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && Modifier == Modifier_Percent) {
                    Deformatted[Specifier]             = UTF32_Clone(UTF32String("%"));
                }
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
        return Deformatted;
    }
    
    uint64_t UTF8_GetNumFormatSpecifiers(UTF8 *String) {
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
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF16_GetNumFormatSpecifiers(UTF16 *String) {
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
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF32_GetNumFormatSpecifiers(UTF32 *String) {
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
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return NumSpecifiers;
    }
    
    UTF8 *UTF8_Format(UTF8 *Format, ...) {
        UTF8 *Format8                        = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers           = UTF8_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32 *Format32              = UTF8_Decode(Format);
                UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF8);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                Format_Specifiers_RetrieveArguments(Specifiers, VariadicArguments);
                va_end(VariadicArguments);
                UTF32 *FormattedString       = FormatString_UTF32(Specifiers, Format32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                Format8                      = UTF8_Encode(FormattedString);
                printf("UTF-8: \"%s\"\n", Format8);
                UTF32_Deinit(FormattedString);
            } else {
                Format8                      = Format;
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return Format8;
    }
    
    UTF16 *UTF16_Format(UTF16 *Format, ...) {
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
                UTF32 *FormattedString       = FormatString_UTF32(Specifiers, Format32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                Format16                     = UTF16_Encode(FormattedString);
                UTF32_Deinit(FormattedString);
            } else {
                Format16                     = Format;
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return Format16;
    }
    
    UTF32 *UTF32_Format(UTF32 *Format, ...) {
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
                FormattedString              = FormatString_UTF32(Specifiers, Format);
                FormatSpecifiers_Deinit(Specifiers);
            } else {
                FormattedString              = Format;
            }
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
        return FormattedString;
    }
    
    UTF8 **UTF8_Deformat(UTF8 *Format, UTF8 *Formatted) {
        UTF8 **StringArray                   = NULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t NumSpecifiers           = UTF8_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32              = UTF8_Decode(Format);
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF8);
                UTF32 *Formatted32           = UTF8_Decode(Formatted);
                UTF32 **Strings32            = DeformatString_UTF32(Specifiers, Format32, Formatted32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                UTF32_Deinit(Formatted32);
                StringArray                  = UTF8_StringArray_Encode(Strings32);
                UTF32_StringArray_Deinit(Strings32);
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Formatted String is NULL"));
        }
        return StringArray;
    }
    
    UTF16 **UTF16_Deformat(UTF16 *Format, UTF16 *Formatted) {
        UTF16 **StringArray                  = NULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t NumSpecifiers           = UTF16_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                UTF32 *Format32              = UTF16_Decode(Format);
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF16);
                UTF32 *Formatted32           = UTF16_Decode(Formatted);
                UTF32 **Strings32            = DeformatString_UTF32(Specifiers, Format32, Formatted32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                UTF32_Deinit(Formatted32);
                StringArray                  = UTF16_StringArray_Encode(Strings32);
                UTF32_StringArray_Deinit(Strings32);
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Formatted String is NULL"));
        }
        return StringArray;
    }
    
    UTF32 **UTF32_Deformat(UTF32 *Format, UTF32 *Formatted) {
        UTF32 **StringArray                  = NULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t NumSpecifiers           = UTF32_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatSpecifiers(Format, Specifiers, StringType_UTF32);
                StringArray                  = DeformatString_UTF32(Specifiers, Format, Formatted);
                FormatSpecifiers_Deinit(Specifiers);
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Formatted String is NULL"));
        }
        return StringArray;
    }
    
#ifdef __cplusplus
}
#endif
