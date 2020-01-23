#include "../../include/Private/Constants.h"           /* Included for the Number tables */
#include "../../include/UnicodeIO/FormatIO.h"          /* Included for our declarations */
#include "../../include/UnicodeIO/LogIO.h"             /* Included for Logging */
#include "../../include/UnicodeIO/StringIO.h"          /* Included for StringIOBases */
#include <stdarg.h>                                    /* Included for va_list, va_copy, va_start, va_end */
#include <stdlib.h>                                    /* Included for the EXIT_FAILURE and EXIT_SUCCESS macros, calloc, realloc, and free */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum FormatSpecifier_Flags {
        Flag_Unknown                   = 0,
        Flag_Zero_Pad                  = 1,
        Flag_Space_Pad                 = 2,
        Flag_Plus_AddSign              = 4,
        Flag_Minus_LeftJustify         = 8,
        Flag_Pound1_PrefixBase         = 16,
        Flag_Pound2_SuffixDecimal      = 32,
    } FormatSpecifier_Flags;
    
    typedef enum FormatSpecifier_MinWidths {
        MinWidth_Unknown               = 0,
        MinWidth_Asterisk_NextArg      = 1,
        MinWidth_Digits                = 2,
    } FormatSpecifier_MinWidths;
    
    typedef enum FormatSpecifier_Precisions {
        Precision_Unknown              = 0,
        Precision_Dot_Number           = 1,
        Precision_Dot_Asterisk_NextArg = 2,
    } FormatSpecifier_Precisions;
    
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
    
    typedef enum FormatSpecifier_TypeModifiers { // MSVC supports capital C and S for "wide" aka UTF-16 characters/strings
        Modifier_Unknown               = 0,
        Modifier_Percent               = 1,
        Modifier_UTF8                  = 2,
        Modifier_UTF16                 = 4,
        Modifier_UTF32                 = 8,
        Modifier_Base2                 = 16,
        Modifier_Base8                 = 32,
        Modifier_Base10                = 64,
        Modifier_Base16                = 128,
        Modifier_Decimal               = 256,  // XX.YYY
        Modifier_Scientific            = 512, // XX.YYYE(+|-)Z
        Modifier_Shortest              = 1024, // Scientific, or Decimal, whichever is  shorter
        Modifier_Hex                   = 2048, // 0XF.FFFFFFFFFFFFFFFP+60
        Modifier_Uppercase             = 4096,
        Modifier_Lowercase             = 8192,
        Modifier_Long                  = 16384,
        Modifier_Signed                = 131072,
        Modifier_Unsigned              = 262144,
        Modifier_N                     = 524288,
    } FormatSpecifier_TypeModifiers;
    
    typedef enum FormatSpecifier_LengthModifiers {
        Length_Unknown                 = 0,
        Length_8Bit                    = 1,
        Length_16Bit                   = 2,
        Length_32Bit                   = 4,
        Length_64Bit                   = 8,
        Length_SizeType                = 16, // Modifier_Size
        Length_PointerDiff             = 32, // Modifier_PointerDiff
        Length_IntMax                  = 64, // Largest Integer type, 64 bit
    } FormatSpecifier_LengthModifiers;
    
    typedef struct FormatSpecifier {
        UTF32                          *Argument;        // The actual argument contained in the va_list
        uint64_t                        Offset;          // Start location in the format string
        uint64_t                        Length;          // (CurrentCodePoint - Offset) + 1
        uint64_t                        MinWidth;        // Actual Width
        uint64_t                        Precision;       // Actual Precision
        uint64_t                        PositionalArg;   // Argument number to substitute
        FormatSpecifier_TypeModifiers   TypeModifier;
        FormatSpecifier_LengthModifiers LengthModifier;
        FormatSpecifier_Flags           Flag;
        FormatSpecifier_MinWidths       MinWidthFlag;
        FormatSpecifier_Precisions      PrecisionFlag;
        FormatSpecifier_BaseTypes       BaseType;
        uint8_t                         OctalSeqSize;    // Octal Sequence Size, Max 3
        uint8_t                         ModifierSize;    // Num CodePoints for the Type Modifier
        bool                            IsPositional;
    } FormatSpecifier;
    
    typedef struct FormatSpecifiers {
        FormatSpecifier     *Specifiers;
        uint64_t             NumSpecifiers;
        uint64_t             NumDuplicateSpecifiers;
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
        FormatSpecifiers *Specifiers                         = calloc(1, sizeof(FormatSpecifiers));
        if (Specifiers != NULL) {
            Specifiers->Specifiers                           = calloc(NumSpecifiers, sizeof(FormatSpecifier));
            if (Specifiers->Specifiers != NULL) {
                Specifiers->NumSpecifiers                    = NumSpecifiers;
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    Specifiers->Specifiers[Specifier].Offset = 0x7FFFFFFFFFFFFFFE;
                    Specifiers->Specifiers[Specifier].Length = 0x7FFFFFFFFFFFFFFE;
                }
            } else {
                FormatSpecifiers_Deinit(Specifiers);
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate %llu Specifiers"), NumSpecifiers);
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate FormatSpecifiers"));
        }
        return Specifiers;
    }
    
    static StringIOBases ConvertTypeModifier2Base(FormatSpecifier_TypeModifiers TypeModifier) {
        StringIOBases Base = Base_Unknown;
        if ((TypeModifier & Modifier_Decimal) == Modifier_Decimal) {
            Base           = Base_Decimal_Radix10;
        } else if ((TypeModifier & Modifier_Scientific) == Modifier_Scientific) {
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base       = Base_Decimal_Scientific_Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base       = Base_Decimal_Scientific_Lowercase;
            }
        } else if ((TypeModifier & Modifier_Shortest) == Modifier_Shortest) {
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base       = Base_Decimal_Shortest_Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base       = Base_Decimal_Shortest_Lowercase;
            }
        } else if ((TypeModifier & Modifier_Hex) == Modifier_Hex) {
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base       = Base_Decimal_Hex_Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base       = Base_Decimal_Hex_Lowercase;
            }
        } else if ((TypeModifier & Modifier_Base2) == Modifier_Base2) {
            Base           = Base_Integer_Radix2;
        } else if ((TypeModifier & Modifier_Base8) == Modifier_Base8) {
            Base           = Base_Integer_Radix8;
        } else if ((TypeModifier & Modifier_Base10) == Modifier_Base10) {
            Base           = Base_Integer_Radix10;
        } else if ((TypeModifier & Modifier_Base16) == Modifier_Base16) {
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base       = Base_Integer_Radix16_Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base       = Base_Integer_Radix16_Lowercase;
            }
        }
        return Base;
    }
    
    static FormatSpecifiers *UTF32_Specifiers_GetOffsetAndLength(FormatSpecifiers *Specifiers, UTF32 *Format, StringIOStringTypes Type) {
        if (Specifiers != NULL && Format != NULL) {
            Specifiers->StringType    = Type;
            uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(Format);
            uint64_t CodePoint        = 0ULL;
            uint64_t CodePoint2       = 0ULL;
            uint64_t Specifier        = 0ULL;
            
            while (Specifier < Specifiers->NumSpecifiers) {
                while (CodePoint < StringSize) {
                    if (Format[CodePoint] == UTF32Character('%')) {
                        Specifiers->Specifiers[Specifier].Offset             = CodePoint;
                        CodePoint2                                           = CodePoint + 1;
                        while (CodePoint2 < StringSize && Specifiers->Specifiers[Specifier].Length == 0x7FFFFFFFFFFFFFFE) {
                            switch (Format[CodePoint2]) {
                                case UTF32Character('%'):
                                case UTF32Character('a'):
                                case UTF32Character('A'):
                                case UTF32Character('b'):
                                case UTF32Character('B'):
                                case UTF32Character('c'):
                                case UTF32Character('C'):
                                case UTF32Character('d'):
                                case UTF32Character('e'):
                                case UTF32Character('E'):
                                case UTF32Character('f'):
                                case UTF32Character('F'):
                                case UTF32Character('g'):
                                case UTF32Character('G'):
                                case UTF32Character('i'):
                                case UTF32Character('n'):
                                case UTF32Character('o'):
                                case UTF32Character('p'):
                                case UTF32Character('P'):
                                case UTF32Character('s'):
                                case UTF32Character('S'):
                                case UTF32Character('u'):
                                case UTF32Character('x'):
                                case UTF32Character('X'):
                                    Specifiers->Specifiers[Specifier].Length = (CodePoint2 - CodePoint) + 1; // Add 1 for the %
                                    CodePoint                                = CodePoint2;
                                    break;
                            }
                            CodePoint2                                      += 1;
                        }
                        Specifier                                           += 1;
                    }
                    CodePoint                                               += 1;
                }
            }
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        return Specifiers;
    }
    
    static void UTF32_Specifiers_GetType(UTF32 *Format, FormatSpecifiers *Specifiers) {
        if (Format != NULL && Specifiers != NULL) {
            uint64_t Specifier = 0ULL;
            while (Specifier < Specifiers->NumSpecifiers) {
                uint64_t Offset    = Specifiers->Specifiers[Specifier].Offset;
                uint64_t Length    = Specifiers->Specifiers[Specifier].Length;
                uint64_t CodePoint = (Offset + Length) - 1;
                
                if (Offset != 0x7FFFFFFFFFFFFFFE && Length != 0x7FFFFFFFFFFFFFFE) {
                    /* TODO: Rewrite this so it can support overlapping modifiers and base types */
                    switch (Format[CodePoint]) {
                        case UTF32Character('a'):
                        case UTF32Character('A'):
                        case UTF32Character('e'):
                        case UTF32Character('E'):
                        case UTF32Character('f'):
                        case UTF32Character('F'):
                        case UTF32Character('g'):
                        case UTF32Character('G'):
                            Specifiers->Specifiers[Specifier].BaseType |= BaseType_Decimal;
                            break;
                        case UTF32Character('b'):
                        case UTF32Character('B'):
                        case UTF32Character('d'):
                        case UTF32Character('i'):
                        case UTF32Character('o'):
                        case UTF32Character('u'):
                        case UTF32Character('x'):
                        case UTF32Character('X'):
                            Specifiers->Specifiers[Specifier].BaseType |= BaseType_Integer;
                            break;
                        case UTF32Character('c'):
                        case UTF32Character('C'):
                            Specifiers->Specifiers[Specifier].BaseType |= BaseType_CodeUnit;
                            break;
                        case UTF32Character('s'):
                        case UTF32Character('S'):
                            Specifiers->Specifiers[Specifier].BaseType |= BaseType_String;
                            break;
                        case UTF32Character('p'):
                        case UTF32Character('P'):
                            Specifiers->Specifiers[Specifier].BaseType |= BaseType_Pointer;
                            break;
                        case UTF32Character('%'):
                            Specifiers->Specifiers[Specifier].BaseType |= BaseType_Literal;
                        case UTF32Character('n'):
                            Specifiers->Specifiers[Specifier].BaseType |= BaseType_Remove;
                            break;
                    }
                }
                Specifier                                              += 1;
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    static void UTF32_Specifiers_GetModifiers(UTF32 *Format, FormatSpecifiers *Specifiers) {
        if (Format != NULL && Specifiers != NULL) {
            uint64_t Specifier = 0ULL;
            while (Specifier < Specifiers->NumSpecifiers) {
                uint64_t Offset    = Specifiers->Specifiers[Specifier].Offset;
                uint64_t Length    = Specifiers->Specifiers[Specifier].Length;
                uint64_t CodePoint = (Offset + Length) - 2;
                
                switch (Specifiers->Specifiers[Specifier].BaseType) {
                    case BaseType_Decimal:
                        if (Format[CodePoint] == UTF32Character('L')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= (Modifier_Long | Modifier_Decimal);
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        }
                        break;
                    case BaseType_Integer:
                        if (Format[CodePoint - 1] == UTF32Character('l') && Format[CodePoint] == UTF32Character('l')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= Length_64Bit;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 2;
                        } else if (Format[CodePoint - 1] != UTF32Character('l') && Format[CodePoint] == UTF32Character('l')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= Length_32Bit;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        } else if (Format[CodePoint - 1] == UTF32Character('h') && Format[CodePoint] == UTF32Character('h')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= Length_8Bit;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 2;
                        } else if (Format[CodePoint - 1] != UTF32Character('h') && Format[CodePoint] == UTF32Character('h')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= Length_16Bit;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        } else if (Format[CodePoint] == UTF32Character('j')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= Length_IntMax;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        } else if (Format[CodePoint] == UTF32Character('z')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= Length_SizeType;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        }
                        break;
                    case BaseType_Pointer:
                        if (Format[CodePoint] == UTF32Character('t')) {
                            Specifiers->Specifiers[Specifier].LengthModifier   |= Length_PointerDiff;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        }
                        break;
                    case BaseType_CodeUnit:
                    case BaseType_String:
                        if (Format[CodePoint] == UTF32Character('l')) {
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS || FoundationIOTargetOS == FoundationIOAppleOS)
                            Specifiers->Specifiers[Specifier].TypeModifier     |= Modifier_UTF32;
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
                            Specifiers->Specifiers[Specifier].TypeModifier     |= Modifier_UTF16;
#endif
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        } else if (Format[CodePoint] == UTF32Character('u')) {
                            Specifiers->Specifiers[Specifier].TypeModifier     |= Modifier_UTF16;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        } else if (Format[CodePoint] == UTF32Character('U')) {
                            Specifiers->Specifiers[Specifier].TypeModifier     |= Modifier_UTF32;
                            Specifiers->Specifiers[Specifier].ModifierSize     += 1;
                        } else {
                            if (Specifiers->StringType == StringType_UTF8) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (Specifiers->StringType == StringType_UTF16) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (Specifiers->StringType == StringType_UTF32) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case BaseType_Literal:
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Percent;
                        break;
                    case BaseType_Remove:
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_N;
                        break;
                    case BaseType_Unknown: // Only here to get rid of the warning about missing cases
                    default:
                        break;
                }
                
                if (Length > 2) { // 2 means no modifiers, %%, %s, etc
                    switch (Format[CodePoint + 1]) {
                        case UTF32Character('a'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Hex | Modifier_Lowercase);
                            break;
                        case UTF32Character('A'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Hex | Modifier_Uppercase);
                            break;
                        case UTF32Character('e'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Scientific | Modifier_Lowercase);
                            break;
                        case UTF32Character('E'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Scientific | Modifier_Uppercase);
                            break;
                        case UTF32Character('f'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Decimal | Modifier_Lowercase);
                            break;
                        case UTF32Character('F'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Decimal | Modifier_Uppercase);
                            break;
                        case UTF32Character('g'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Shortest | Modifier_Lowercase);
                            break;
                        case UTF32Character('G'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Shortest | Modifier_Uppercase);
                            break;
                        case UTF32Character('b'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Unsigned | Modifier_Base2 | Modifier_Lowercase);
                            break;
                        case UTF32Character('B'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Unsigned | Modifier_Base2 | Modifier_Uppercase);
                            break;
                        case UTF32Character('d'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Signed | Modifier_Base10);
                            break;
                        case UTF32Character('i'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Signed | Modifier_Base10);
                            break;
                        case UTF32Character('u'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Unsigned | Modifier_Base10);
                            break;
                        case UTF32Character('o'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Unsigned | Modifier_Base10);
                            break;
                        case UTF32Character('x'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Lowercase);
                            break;
                        case UTF32Character('X'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Uppercase);
                            break;
                        case UTF32Character('p'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_Lowercase;
                            break;
                        case UTF32Character('P'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_Uppercase;
                            break;
                        case UTF32Character('n'):
                            Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_N;
                            break;
                    }
                }
                Specifier                                                  += 1;
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    void UTF32_Specifiers_GetFlagsWidthPrecisionPosition(UTF32 *Format, FormatSpecifiers *Specifiers) {
        if (Format != NULL && Specifiers != NULL) {
            uint64_t Specifier     = 0ULL;
            while (Specifier < Specifiers->NumSpecifiers) {
                uint64_t Offset    = Specifiers->Specifiers[Specifier].Offset;
                uint64_t Length    = Specifiers->Specifiers[Specifier].Length;
                
                uint64_t CodePoint = Offset + 1;
                while (CodePoint < Offset + Length) {
                    if (Format[CodePoint] == UTF32Character('0')) {
                        if (Format[CodePoint + 1] != FoundationIONULLTerminator) {
                            if ((Format[CodePoint - 1] < UTF32Character('0') || Format[CodePoint - 1] > UTF32Character('9')) && (Format[CodePoint + 1] < UTF32Character('0') || Format[CodePoint + 1] > UTF32Character('9'))) { // The zero is standalone, therefore it's a flag
                                Specifiers->Specifiers[Specifier].Flag |= Flag_Zero_Pad;
                            }
                        } else { // Then just check that the revious codepoint isn't a digit, and if its not then you have a valid 0 flag
                            if (Format[CodePoint - 1] < UTF32Character('0') || Format[CodePoint] > UTF32Character('9')) {
                                Specifiers->Specifiers[Specifier].Flag |= Flag_Zero_Pad;
                            }
                        }
                    } else if (Format[CodePoint] == UTF32Character(' ')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Space_Pad;
                    } else if (Format[CodePoint] == UTF32Character('+')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Plus_AddSign;
                    } else if (Format[CodePoint] == UTF32Character('-')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Minus_LeftJustify;
                    } else if (Format[CodePoint] == UTF32Character('#')) {
                        if ((Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base8) == Modifier_Base8 || (Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base16) == Modifier_Base16) {
                            Specifiers->Specifiers[Specifier].Flag |= Flag_Pound1_PrefixBase;
                        } else if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                            Specifiers->Specifiers[Specifier].Flag |= Flag_Pound2_SuffixDecimal;
                        }
                    } else if (Format[CodePoint] >= UTF32Character('0') && Format[CodePoint] <= UTF32Character('9')) {
                        uint64_t DigitStart           = CodePoint;
                        while (Format[CodePoint] >= UTF32Character('0') && Format[CodePoint] <= UTF32Character('9') && (CodePoint > Offset) && (CodePoint < Offset + Length)) {
                            DigitStart               -= 1;
                            CodePoint                -= 1;
                        }
                        DigitStart                   += 1;
                        CodePoint                    += 1;
                        uint64_t NumConsecutiveDigits = UTF32_GetNumDigits(Base_Integer_Radix10, Format, CodePoint);
                        UTF32   *DigitString          = UTF32_ExtractSubString(Format, DigitStart, NumConsecutiveDigits);
                        uint64_t Number               = UTF32_String2Integer(Base_Integer_Radix10, DigitString);
                        if (Format[DigitStart - 1] == UTF32Character('.')) { // Precision
                            Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Number;
                            Specifiers->Specifiers[Specifier].Precision     = Number;
                        } else if (Format[DigitStart + NumConsecutiveDigits] == UTF32Character('$')) { // Positional
                            Specifiers->Specifiers[Specifier].IsPositional  = Yes;
                            Specifiers->Specifiers[Specifier].PositionalArg = Number - 1;
                        } else { // MinWidth
                            Specifiers->Specifiers[Specifier].MinWidthFlag  = MinWidth_Digits;
                            Specifiers->Specifiers[Specifier].MinWidth      = Number;
                        }
                    }
                    CodePoint     += 1;
                }
                Specifier += 1;
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    static void UTF32_Specifiers_GetDuplicateSpecifiers(FormatSpecifiers *Specifiers) {
        if (Specifiers != NULL) {
            uint64_t  NumDuplicateSpecifiers      = 0ULL;
            uint64_t *SpecifierCounts             = calloc(Specifiers->NumSpecifiers, sizeof(uint64_t));
            if (SpecifierCounts != NULL) {
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    if (Specifiers->Specifiers[Specifier].IsPositional == Yes) {
                        uint64_t Position           = Specifiers->Specifiers[Specifier].PositionalArg;
                        SpecifierCounts[Position]  += 1;
                    }
                }
                
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    if (SpecifierCounts[Specifier] >= 2) {
                        NumDuplicateSpecifiers += 1;
                    }
                }
            }
            
            Specifiers->NumDuplicateSpecifiers = NumDuplicateSpecifiers;
            free(SpecifierCounts);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    void UTF32_ParseFormatString(FormatSpecifiers *Specifiers, UTF32 *Format, StringIOStringTypes StringType) {
        if (Format != NULL) {
            UTF32_Specifiers_GetOffsetAndLength(Specifiers, Format, StringType);
            UTF32_Specifiers_GetType(Format, Specifiers);
            UTF32_Specifiers_GetModifiers(Format, Specifiers);
            UTF32_Specifiers_GetFlagsWidthPrecisionPosition(Format, Specifiers);
            UTF32_Specifiers_GetDuplicateSpecifiers(Specifiers);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
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
    
	static void Specifiers_CorrectOffset(FormatSpecifiers *Specifiers, UTF32 *Format, uint64_t CurrentSpecifier) {
        if (Specifiers != NULL && Format != NULL && CurrentSpecifier < Specifiers->NumSpecifiers) {
            uint64_t CodePoint                                      = 0ULL;
            while (Format[CodePoint] != FoundationIONULLTerminator) {
                CodePoint                                          += 1;
                if (Format[CodePoint] == UTF32Character('%')) {
                    Specifiers->Specifiers[CurrentSpecifier].Offset = CodePoint;
                    break;
                }
            }
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Format Pointer is NULL"));
        } else if (CurrentSpecifier >= Specifiers->NumSpecifiers) {
            Log(Log_DEBUG, __func__, UTF8String("CurrentSpecifier: %llu is not a valid index"), CurrentSpecifier);
        }
    }
    
    static void Format_Specifiers_RetrieveArguments(FormatSpecifiers *Specifiers, va_list Arguments) {
        if (Specifiers != NULL) {
            uint64_t                        Specifier0                     = 0ULL;
            uint64_t                        Position                       = 0x7FFFFFFFFFFFFFFE;
            FormatSpecifier_BaseTypes       BaseType                       = BaseType_Unknown;
            FormatSpecifier_TypeModifiers   Modifier                       = Modifier_Unknown;
            FormatSpecifier_LengthModifiers Length                         = Length_Unknown;
            while (Specifier0 < Specifiers->NumSpecifiers - Specifiers->NumDuplicateSpecifiers) {
                if (Specifiers->Specifiers[Specifier0].IsPositional == Yes) {
                    Position                                               = Specifiers->Specifiers[Specifier0].PositionalArg;
                    BaseType                                               = Specifiers->Specifiers[Position].BaseType;
                    Modifier                                               = Specifiers->Specifiers[Position].TypeModifier;
                    Length                                                 = Specifiers->Specifiers[Position].LengthModifier;
                } else {
                    Position                                               = Specifier0;
                    BaseType                                               = Specifiers->Specifiers[Position].BaseType;
                    Modifier                                               = Specifiers->Specifiers[Position].TypeModifier;
                    Length                                                 = Specifiers->Specifiers[Position].LengthModifier;
                }
                
                if (Specifiers->Specifiers[Position].MinWidthFlag == MinWidth_Asterisk_NextArg) {
                    Specifiers->Specifiers[Position].MinWidth              = va_arg(Arguments, uint32_t);
                }
                if (Specifiers->Specifiers[Position].PrecisionFlag == Precision_Dot_Asterisk_NextArg) {
                    Specifiers->Specifiers[Position].Precision             = va_arg(Arguments, uint32_t);
                }
                
                StringIOBases Base                                         = ConvertTypeModifier2Base(Modifier);
                
                if ((BaseType & BaseType_Integer) == BaseType_Integer) {
                    if ((Modifier & Modifier_Unsigned) == Modifier_Unsigned) {
                        /* 8 and 16 bit types are promoted automatically */
                        if ((Length & Length_8Bit) == Length_8Bit) {
                            uint8_t   Arg                                  = (uint8_t) va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_16Bit) == Length_16Bit) {
                            uint16_t  Arg                                  = (uint16_t) va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_32Bit) == Length_32Bit) {
                            uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_64Bit) == Length_64Bit) {
                            uint64_t  Arg                                  = va_arg(Arguments, uint64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        }
                    } else if ((Modifier & Modifier_Signed) == Modifier_Signed) {
                        /* 8 and 16 bit types are promoted automatically */
                        if ((Length & Length_8Bit) == Length_8Bit) {
                            int8_t    Arg                                  = (int8_t) va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_16Bit) == Length_16Bit) {
                            int16_t   Arg                                  = (int16_t) va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_32Bit) == Length_32Bit) {
                            int32_t   Arg                                  = va_arg(Arguments, int32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_64Bit) == Length_64Bit) {
                            int64_t   Arg                                  = va_arg(Arguments, int64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        }
                    }
                } else if ((BaseType & BaseType_Decimal) == BaseType_Decimal) {
                    if ((Length & Length_32Bit) == Length_32Bit) {
                        float    Arg                                       = (float) va_arg(Arguments, double);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, Arg);
                    }
                    if ((Length & Length_64Bit) == Length_64Bit) {
                        double   Arg                                       = va_arg(Arguments, double);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, Arg);
                    }
                } else if ((BaseType & BaseType_CodeUnit) == BaseType_CodeUnit) {
                    if ((Modifier & Modifier_UTF8) == Modifier_UTF8) {
                        UTF8  VariadicArgument                             = (UTF8) va_arg(Arguments, uint32_t);
                        UTF8 *String                                       = UTF8_CodeUnit2String(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = UTF8_Decode(String);
                        UTF8_Deinit(String);
                    } else if ((Modifier & Modifier_UTF16) == Modifier_UTF16) {
                        UTF16  VariadicArgument                            = (UTF16) va_arg(Arguments, uint32_t);
                        UTF16 *String                                      = UTF16_CodeUnit2String(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = UTF16_Decode(String);
                        UTF16_Deinit(String);
                    } else if ((Modifier & Modifier_UTF32) == Modifier_UTF32) {
                        UTF32  VariadicArgument                            = va_arg(Arguments, UTF32);
                        Specifiers->Specifiers[Position].Argument          = UTF32_CodeUnit2String(VariadicArgument);
                    }
                } else if ((BaseType & BaseType_String) == BaseType_String) {
                    if ((Modifier & Modifier_UTF8) == Modifier_UTF8) {
                        UTF8  *VariadicArgument                            = va_arg(Arguments, UTF8*);
                        UTF32 *VariadicArgument32                          = UTF8_Decode(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = VariadicArgument32;
                    } else if ((Modifier & Modifier_UTF16) == Modifier_UTF16) {
                        UTF16 *VariadicArgument                            = va_arg(Arguments, UTF16*);
                        UTF32 *VariadicArgument32                          = UTF16_Decode(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = VariadicArgument32;
                    } else if ((Modifier & Modifier_UTF32) == Modifier_UTF32) {
                        Specifiers->Specifiers[Position].Argument          = va_arg(Arguments, UTF32*);
                    }
                } else if ((BaseType & BaseType_Pointer) == BaseType_Pointer) {
                    if ((Modifier & Modifier_Lowercase) == Modifier_Lowercase) {
                        uint64_t Value                                     = va_arg(Arguments, uint64_t);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Base_Integer_Radix16_Lowercase, Value);
                    } else if ((Modifier & Modifier_Uppercase) == Modifier_Uppercase) {
                        uint64_t Value                                     = va_arg(Arguments, uint64_t);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Base_Integer_Radix16_Uppercase, Value);
                    }
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && Modifier == Modifier_Percent) {
                    Specifiers->Specifiers[Position].Argument              = UTF32String("%");
                }
                Specifier0                                                 += 1;
            }
            
            if (Specifiers->NumDuplicateSpecifiers > 0) {
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    if (Specifiers->Specifiers[Specifier].IsPositional == Yes && Specifiers->Specifiers[Specifier].Argument == NULL) {
                        uint64_t Position                                  = Specifiers->Specifiers[Specifier].PositionalArg;
                        
                        Specifiers->Specifiers[Specifier].MinWidthFlag     = Specifiers->Specifiers[Position].MinWidthFlag;
                        Specifiers->Specifiers[Specifier].MinWidth         = Specifiers->Specifiers[Position].MinWidth;
                        
                        Specifiers->Specifiers[Specifier].PrecisionFlag    = Specifiers->Specifiers[Position].PrecisionFlag;
                        Specifiers->Specifiers[Specifier].Precision        = Specifiers->Specifiers[Position].Precision;
                        
                        Specifiers->Specifiers[Specifier].Argument         = UTF32_Clone(Specifiers->Specifiers[Position].Argument);
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    UTF32 *FormatString_UTF32(FormatSpecifiers *Specifiers, UTF32 *Format) {
        UTF32 *Formatted               = NULL;
        UTF32 *FormatTemp              = Format;
        UTF32 *FormatTemp2             = Format;
        if (Format != NULL && Specifiers != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                if (Specifier > 0) {
                    Specifiers_CorrectOffset(Specifiers, Format, Specifier);
                }
                UTF32    *Argument     = Specifiers->Specifiers[Specifier].Argument;
                uint64_t  Offset       = Specifiers->Specifiers[Specifier].Offset + 1;
                uint64_t  Length       = Specifiers->Specifiers[Specifier].Length;
                
                if (Specifiers->Specifiers[Specifier].BaseType == BaseType_Remove) {
                    FormatTemp2        = UTF32_RemoveSubString(FormatTemp, UTF32String("%n"), 1);
                } else if (Specifiers->Specifiers[Specifier].BaseType == BaseType_Literal) {
                    FormatTemp2        = UTF32_ReplaceSubString(FormatTemp, UTF32String("%"), Offset, Length);
                } else {
                    FormatTemp2        = UTF32_ReplaceSubString(FormatTemp, Argument, Offset, Length);
                }
                
                /* Deallocate  */
                /*
                if (FormatTemp2 != FormatTemp) {
                    UTF32_Deinit(FormatTemp);
                    FormatTemp = FormatTemp2;
                }
                 */
            }
            Formatted                  = FormatTemp;
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Format Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
        }
        return Formatted;
    }
    
    UTF32 **DeformatString_UTF32(FormatSpecifiers *Specifiers, UTF32 *Format, UTF32 *Formatted) {
        UTF32 **Deformatted                            = UTF32_StringArray_Init(Specifiers->NumSpecifiers);
        if (Deformatted != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) { // Stringify each specifier
                FormatSpecifier_BaseTypes     BaseType = Specifiers->Specifiers[Specifier].BaseType;
                FormatSpecifier_TypeModifiers Modifier = Specifiers->Specifiers[Specifier].TypeModifier;
                uint64_t                      Offset   = Specifiers->Specifiers[Specifier].Offset + 1; // Offset always starts at the Percent so always add 1
                
                if ((BaseType & BaseType_Integer) == BaseType_Integer || (BaseType & BaseType_Decimal) == BaseType_Decimal || (BaseType & BaseType_Pointer) == BaseType_Pointer) {
                    StringIOBases      Base            = ConvertTypeModifier2Base(Modifier);
                    uint64_t           SubStringLength = UTF32_GetNumDigits(Base, Formatted, Offset);
                    // Offset is 1 long  for Specifier 0
                    // Offset is 2 short for Specifier 1
                    // Offset is 5 short for Specifier 2
                    // Cascading failure, CorrectOffset is wrong
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, Offset, SubStringLength);
                } else if ((BaseType & BaseType_CodeUnit) == BaseType_CodeUnit) {
                    Deformatted[Specifier]             = UTF32_CodeUnit2String(Formatted[Offset]);
                } else if ((BaseType & BaseType_String) == BaseType_String) {
                    uint64_t SubStringLength           = UTF32_GetSubStringLength(Format, Formatted, Offset);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, Offset, SubStringLength);
                } else if ((BaseType & BaseType_Literal) == BaseType_Literal && Modifier == Modifier_Percent) {
                    Deformatted[Specifier]             = UTF32_Clone(UTF32String("%"));
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("FormatSpecifiers Pointer is NULL"));
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
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
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
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
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
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
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
                UTF32_ParseFormatString(Specifiers, Format32, StringType_UTF8);
                va_list VariadicArguments;
                va_start(VariadicArguments, Format);
                Format_Specifiers_RetrieveArguments(Specifiers, VariadicArguments);
                va_end(VariadicArguments);
                UTF32 *FormattedString       = FormatString_UTF32(Specifiers, Format32);
                printf("%ls\n", (wchar_t*) FormattedString);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                Format8                      = UTF8_Encode(FormattedString);
                UTF32_Deinit(FormattedString);
            } else {
                Format8                      = Format;
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
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
                UTF32_ParseFormatString(Specifiers, Format32, StringType_UTF16);
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
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
        }
        return Format16;
    }
    
    UTF32 *UTF32_Format(UTF32 *Format, ...) {
        UTF32 *FormattedString               = NULL;
        if (Format != NULL) {
            uint64_t NumSpecifiers           = UTF32_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatString(Specifiers, Format, StringType_UTF32);
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
            Log(Log_DEBUG, __func__, UTF8String("String Pointer is NULL"));
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
                UTF32_ParseFormatString(Specifiers, Format32, StringType_UTF8);
                UTF32 *Formatted32           = UTF8_Decode(Formatted);
                UTF32 **Strings32            = DeformatString_UTF32(Specifiers, Format32, Formatted32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                UTF32_Deinit(Formatted32);
                StringArray                  = UTF8_StringArray_Encode(Strings32);
                UTF32_StringArray_Deinit(Strings32);
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Formatted String is NULL"));
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
                UTF32_ParseFormatString(Specifiers, Format32, StringType_UTF16);
                UTF32 *Formatted32           = UTF16_Decode(Formatted);
                UTF32 **Strings32            = DeformatString_UTF32(Specifiers, Format32, Formatted32);
                FormatSpecifiers_Deinit(Specifiers);
                UTF32_Deinit(Format32);
                UTF32_Deinit(Formatted32);
                StringArray                  = UTF16_StringArray_Encode(Strings32);
                UTF32_StringArray_Deinit(Strings32);
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Formatted String is NULL"));
        }
        return StringArray;
    }
    
    UTF32 **UTF32_Deformat(UTF32 *Format, UTF32 *Formatted) {
        UTF32 **StringArray                  = NULL;
        if (Format != NULL && Formatted != NULL) {
            uint64_t NumSpecifiers           = UTF32_GetNumFormatSpecifiers(Format);
            if (NumSpecifiers > 0) {
                FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
                UTF32_ParseFormatString(Specifiers, Format, StringType_UTF32);
                StringArray                  = DeformatString_UTF32(Specifiers, Format, Formatted);
                FormatSpecifiers_Deinit(Specifiers);
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Format Pointer is NULL"));
        } else if (Formatted == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Formatted String is NULL"));
        }
        return StringArray;
    }
    
#ifdef __cplusplus
}
#endif
