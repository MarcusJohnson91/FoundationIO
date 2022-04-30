#include "../../include/TextIO/FormatIO.h"    /* Included for our declarations */

#include "../../include/AssertIO.h"           /* Included for Assertions */
#include "../../include/MathIO.h"             /* Included for Logarithm */
#include "../../include/TextIO/StringIO.h"    /* Included for StringIO */
#include "../../include/TextIO/StringSetIO.h" /* Included for StringSet support */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @enum         FormatIO_BaseTypes
     @abstract                                    Defines the base type of each format specifier.
     @constant     BaseType_Unspecified           Invalid base type.
     @constant     BaseType_Integer               The specifier is an Integer.
     @constant     BaseType_Decimal               The specifier is a floating point number.
     @constant     BaseType_CodeUnit              The specifier is a single value encoded as UTF 8/16/32 bits max, otherwise use String.
     @constant     BaseType_String                The specifier is a null terminated array encoded as UTF 8/16/32.
     @constant     BaseType_LiteralPercent        The specifier is a double percent token to print a single percent symbol.
     @constant     BaseType_Pointer               The specifier is a Pointer address.
     @constant     BaseType_RemoveN               The specifier is N, we remove it for interoperability.
     */
    typedef enum FormatIO_BaseTypes {
        BaseType_Unspecified           = 0,
        BaseType_Integer               = 1,
        BaseType_Decimal               = 2,
        BaseType_CodeUnit              = 4,
        BaseType_String                = 8,
        BaseType_Pointer               = 16,
        BaseType_RemoveN               = 32,
        BaseType_LiteralPercent        = 64,
    } FormatIO_BaseTypes;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FormatIO_BaseTypes operator | (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            return static_cast<FormatIO_BaseTypes>(static_cast<uint8_t>(A) | static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_BaseTypes operator & (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            return static_cast<FormatIO_BaseTypes>(static_cast<uint8_t>(A) & static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_BaseTypes operator |= (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_BaseTypes>(A1 | B1);
        }
        
        constexpr inline FormatIO_BaseTypes operator &= (FormatIO_BaseTypes A, FormatIO_BaseTypes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_BaseTypes>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_ModifierTypes
     @abstract                                    Defines the modifiers of each format specifier.
     @constant     ModifierType_Unspecified       Invalid modifier type.
     @constant     ModifierType_UTF8              The modifier is UTF-8 encoded CodeUnit or String.
     @constant     ModifierType_UTF16             The modifier is UTF-16 encoded CodeUnit or String.
     @constant     ModifierType_UTF32             The modifier is UTF-32 encoded CodeUnit or String.
     @constant     ModifierType_Radix8            The modifier is base-8.
     @constant     ModifierType_Radix10           The modifier is base-10, either Integer or Decimal.
     @constant     ModifierType_Radix16           The modifier is base-16, either Integer or Decimal.
     @constant     ModifierType_Uppercase         The modifier is Uppercase.
     @constant     ModifierType_Lowercase         The modifier is Lowercase.
     @constant     ModifierType_Scientific        The modifier is Scientific format Decimal.
     @constant     ModifierType_Shortest          The modifier is Shortest Decimal, either base-10 format or Scientific.
     @constant     ModifierType_Long              The modifier is Long, depends on the Base type.
     @constant     ModifierType_Signed            The modifier is a Signed Integer.
     @constant     ModifierType_Unsigned          The modifier is a Unsigned Integer.
     */
    typedef enum FormatIO_ModifierTypes {
        ModifierType_Unspecified       = 0,
        ModifierType_UTF8              = 1,
        ModifierType_UTF16             = 2,
        ModifierType_UTF32             = 4,
        ModifierType_Radix8            = 8,
        ModifierType_Radix10           = 16,
        ModifierType_Radix16           = 32,
        ModifierType_Uppercase         = 64,
        ModifierType_Lowercase         = 128,
        ModifierType_Scientific        = 256,
        ModifierType_Shortest          = 512,
        ModifierType_Long              = 1024,
        ModifierType_Signed            = 2048,
        ModifierType_Unsigned          = 4096,
    } FormatIO_ModifierTypes;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FormatIO_ModifierTypes operator | (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            return static_cast<FormatIO_ModifierTypes>(static_cast<uint32_t>(A) | static_cast<uint32_t>(B));
        }
        
        constexpr inline FormatIO_ModifierTypes operator & (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            return static_cast<FormatIO_ModifierTypes>(static_cast<uint32_t>(A) & static_cast<uint32_t>(B));
        }
        
        constexpr inline FormatIO_ModifierTypes operator |= (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            uint32_t A1 = static_cast<uint32_t>(A);
            uint32_t B1 = static_cast<uint32_t>(B);
            return static_cast<FormatIO_ModifierTypes>(A1 | B1);
        }
        
        constexpr inline FormatIO_ModifierTypes operator &= (FormatIO_ModifierTypes A, FormatIO_ModifierTypes B) {
            uint32_t A1 = static_cast<uint32_t>(A);
            uint32_t B1 = static_cast<uint32_t>(B);
            return static_cast<FormatIO_ModifierTypes>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_ModifierLengths
     @abstract                                    Defines the lengths of each format specifier, generally Integer or Decimal sizes.
     @constant     ModifierLength_Unspecified     Invalid modifier length.
     @constant     ModifierLength_8Bit            The length is 8 bit.
     @constant     ModifierLength_16Bit           The length is 16 bit.
     @constant     ModifierLength_32Bit           The length is 32 bit.
     @constant     ModifierLength_64Bit           The length is 64 bit.
     @constant     ModifierLength_SizeType        The length is equal to size_t, generally 32 or 64 bit depending on platform.
     @constant     ModifierLength_PointerDiff     The length is Pointer Diff as in ptrdiff_t/uptrdiff_t.
     @constant     ModifierLength_IntMax          The length is IntMax as in intmax_t.
     */
    typedef enum FormatIO_ModifierLengths {
        ModifierLength_Unspecified     = 0,
        ModifierLength_8Bit            = 1,
        ModifierLength_16Bit           = 2,
        ModifierLength_32Bit           = 4,
        ModifierLength_64Bit           = 8,
        ModifierLength_SizeType        = 16,
        ModifierLength_PointerDiff     = 32,
        ModifierLength_IntMax          = 64,
    } FormatIO_ModifierLengths;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FormatIO_ModifierLengths operator | (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            return static_cast<FormatIO_ModifierLengths>(static_cast<uint8_t>(A) | static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_ModifierLengths operator & (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            return static_cast<FormatIO_ModifierLengths>(static_cast<uint8_t>(A) & static_cast<uint8_t>(B));
        }
        
        constexpr inline FormatIO_ModifierLengths operator |= (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_ModifierLengths>(A1 | B1);
        }
        
        constexpr inline FormatIO_ModifierLengths operator &= (FormatIO_ModifierLengths A, FormatIO_ModifierLengths B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FormatIO_ModifierLengths>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum         FormatIO_Flags
     @abstract                                    Defines the Flags of each format specifier.
     @constant     Flag_Unspecified               There is no Flag.
     @constant     Flag_Zero_Pad                  Pad the number with leading zeros.
     @constant     Flag_Space_Pad                 Pad the string with spaces or Integer with a prefixed space for positive integers.
     @constant     Flag_Plus_AddSign              Add the + sign to positive Integers.
     @constant     Flag_Minus_LeftJustify         Left justify a field, right justify is default.
     @constant     Flag_Pound_PrefixBase          Prefix '0' for radix8, 0x/0X for radix16 on non-zero Integers.
     @constant     Flag_Pound_DecimalSuffix       The length is IntMax as in intmax_t.
     @constant     Flag_Apostrophe_ExponentGroup  The Integer or Decimals Exponent has grouping seperators applied.
     */
    typedef enum FormatIO_Flags {
        Flag_Unspecified               = 0,
        Flag_Zero_Pad                  = 1,
        Flag_Space_Pad                 = 2,
        Flag_Plus_AddSign              = 4,
        Flag_Minus_LeftJustify         = 8,
        Flag_Pound_PrefixBase          = 16,
        Flag_Pound_DecimalSuffix       = 32,
        Flag_Apostrophe_ExponentGroup  = 64,
    } FormatIO_Flags;
    
    /*!
     @enum         FormatIO_MinWidths
     @abstract                                    The Minimum Widths of each format specifier.
     @constant     MinWidth_Unspecified           There is no Flag.
     @constant     MinWidth_Asterisk_NextArg      The minimum width is the next argument.
     @constant     MinWidth_Inline_Digits         The minimum width is in the format specifier.
     */
    typedef enum FormatIO_MinWidths {
        MinWidth_Unspecified           = 0,
        MinWidth_Asterisk_NextArg      = 1,
        MinWidth_Inline_Digits         = 2,
    } FormatIO_MinWidths;
    
    /*!
     @enum         FormatIO_Positions
     @abstract                                    The Position of each format specifier.
     @constant     Position_Unspecified           The format specifier is not Positional.
     @constant     Position_Asterisk_NextArg      The Position index is the next argument.
     @constant     MinWidth_Inline_Digits         The Position index is in the format specifier.
     */
    typedef enum FormatIO_Positions {
        Position_Unspecified           = 0,
        Position_Asterisk_NextArg      = 1,
        Position_Inline_Digits         = 2,
    } FormatIO_Positions;
    
    /*!
     @enum         FormatIO_Precisions
     @abstract                                    The Precision of each format specifier.
     @constant     Precision_Unspecified          The Precision is unspecified.
     @constant     Position_Asterisk_NextArg      The Precision is the next argument.
     @constant     MinWidth_Inline_Digits         The Precision is in the format specifier.
     */
    typedef enum FormatIO_Precisions {
        Precision_Unspecified          = 0,
        Precision_Asterisk_NextArg     = 1,
        Precision_Inline_Digits        = 2,
    } FormatIO_Precisions;
    
    typedef enum FormatIO_ArgOrders {
        ArgOrder_Unused    = 0,
        ArgOrder_MinWidth  = 1,
        ArgOrder_Precision = 2,
        ArgOrder_Position  = 3,
    } FormatIO_ArgOrders;
    
    typedef struct FormatSpecifier {
        UTF32                   *Argument;
        size_t                   FormatStart; // Start=FormatStart
        size_t                   FormatModifierStart;
        size_t                   FormatEnd; // End=FormatEnd
        size_t                   FormattedStart;
        size_t                   FormattedEnd;
        size_t                   MinWidth;
        size_t                   Precision;
        size_t                   Position;
        FormatIO_ModifierTypes   ModifierType;
        FormatIO_Flags           Flag;
        FormatIO_BaseTypes       BaseType;
        FormatIO_ModifierLengths LengthModifier;
        FormatIO_MinWidths       MinWidthFlag;
        FormatIO_Precisions      PrecisionFlag;
        FormatIO_Positions       PositionFlag;
        FormatIO_ArgOrders       Order1;
        FormatIO_ArgOrders       Order2;
        FormatIO_ArgOrders       Order3;
    } FormatSpecifier;
    
    typedef struct FormatSpecifiers {
        FormatSpecifier         *Specifiers;
        size_t                  *UniqueSpecifiers;
        size_t                   NumSpecifiers;
        size_t                   NumUniqueSpecifiers;
        TextIO_StringTypes       StringType;
    } FormatSpecifiers;
    
    static void FormatSpecifiers_Deinit(FormatSpecifiers *Specifiers) {
        AssertIO(Specifiers != NULL);
        for (size_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers && Specifiers->Specifiers[Specifier].PositionFlag != Position_Unspecified) {
                size_t Pos = Specifiers->Specifiers[Specifier].Position;
                UTF32_Deinit(Specifiers->Specifiers[Pos].Argument);
            } else {
                UTF32_Deinit(Specifiers->Specifiers[Specifier].Argument);
            }
        }
        free(Specifiers->UniqueSpecifiers);
        free(Specifiers->Specifiers);
        free(Specifiers);
    }
    
    static FormatSpecifiers *FormatSpecifiers_Init(size_t NumSpecifiers) {
        AssertIO(NumSpecifiers >= 1);
        FormatSpecifiers *Specifiers                                       = calloc(1, sizeof(FormatSpecifiers));
        AssertIO(Specifiers != NULL);

        Specifiers->Specifiers                                         = calloc(NumSpecifiers, sizeof(FormatSpecifier));
        AssertIO(Specifiers->Specifiers);

        Specifiers->NumSpecifiers                                  = NumSpecifiers;
        for (size_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
            Specifiers->Specifiers[Specifier].FormatStart          = 0x7FFFFFFFFFFFFFFE;
            Specifiers->Specifiers[Specifier].FormatEnd            = 0x7FFFFFFFFFFFFFFE;
            Specifiers->Specifiers[Specifier].FormatModifierStart  = 0x7FFFFFFFFFFFFFFE;
            Specifiers->Specifiers[Specifier].FormattedStart       = 0x7FFFFFFFFFFFFFFE;
            Specifiers->Specifiers[Specifier].FormattedEnd         = 0x7FFFFFFFFFFFFFFE;
            Specifiers->Specifiers[Specifier].MinWidth             = 0;
            Specifiers->Specifiers[Specifier].Precision            = 0;
            Specifiers->Specifiers[Specifier].Position             = 0;
            Specifiers->Specifiers[Specifier].ModifierType         = ModifierType_Unspecified;
            Specifiers->Specifiers[Specifier].Flag                 = Flag_Unspecified;
            Specifiers->Specifiers[Specifier].BaseType             = BaseType_Unspecified;
            Specifiers->Specifiers[Specifier].LengthModifier       = ModifierLength_Unspecified;
            Specifiers->Specifiers[Specifier].MinWidthFlag         = MinWidth_Unspecified;
            Specifiers->Specifiers[Specifier].PrecisionFlag        = Precision_Unspecified;
            Specifiers->Specifiers[Specifier].PositionFlag         = Position_Unspecified;
            Specifiers->Specifiers[Specifier].Order1               = ArgOrder_Unused;
            Specifiers->Specifiers[Specifier].Order2               = ArgOrder_Unused;
            Specifiers->Specifiers[Specifier].Order3               = ArgOrder_Unused;
        }
        return Specifiers;
    }
    
    static TextIO_Bases ConvertModifierType2Base(FormatSpecifier *Specifier) {
        AssertIO(Specifier != NULL);
        TextIO_Bases           Base         = Base_Unspecified;
        FormatIO_BaseTypes     BaseType     = Specifier->BaseType;
        FormatIO_ModifierTypes ModifierType = Specifier->ModifierType;
        
        if ((BaseType & BaseType_Integer) == BaseType_Integer) {
            Base                           |= Base_Integer;
            if ((ModifierType & ModifierType_Radix8) == ModifierType_Radix8) {
                Base                       |= Base_Radix8;
            } else if ((ModifierType & ModifierType_Radix10) == ModifierType_Radix10) {
                Base                       |= Base_Radix10;
            } else if ((ModifierType & ModifierType_Radix16) == ModifierType_Radix16) {
                Base                       |= Base_Radix16;
            }
        } else if ((BaseType & BaseType_Decimal) == BaseType_Decimal) {
            Base                           |= Base_Decimal;
            if ((ModifierType & ModifierType_Radix10) == ModifierType_Radix10) {
                Base                       |= Base_Radix10;
            } else if ((ModifierType & ModifierType_Scientific) == ModifierType_Scientific) {
                Base                       |= Base_Scientific;
            } else if ((ModifierType & ModifierType_Shortest) == ModifierType_Shortest) {
                Base                       |= Base_Shortest;
            } else if ((ModifierType & ModifierType_Radix16) == ModifierType_Radix16) {
                Base                       |= Base_Radix16;
            }
        }
        if ((ModifierType & ModifierType_Uppercase) == ModifierType_Uppercase) {
            Base                           |= Base_Uppercase;
        } else if ((ModifierType & ModifierType_Lowercase) == ModifierType_Lowercase) {
            Base                           |= Base_Lowercase;
        }
        return Base;
    }
    
    static void FormatSpecifiers_GetNumUniqueSpecifiers(FormatSpecifiers *Specifiers) {
        AssertIO(Specifiers != NULL);
        if (Specifiers->NumSpecifiers > 1) {
            for (size_t Specifier1 = 0; Specifier1 < Specifiers->NumSpecifiers; Specifier1++) {
                for (size_t Specifier2 = 0; Specifier2 < Specifier1; Specifier2++) {
                    if (Specifier1 == Specifier2) {
                        Specifiers->NumUniqueSpecifiers += 1;
                    }
                    if (Specifiers->Specifiers[Specifier1].PositionFlag != Position_Unspecified && Specifiers->Specifiers[Specifier1].Position == Specifiers->Specifiers[Specifier2].Position) {
                        break;
                    }
                }
            }

            Specifiers->UniqueSpecifiers                                  = calloc(Specifiers->NumUniqueSpecifiers, sizeof(size_t));

            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
                size_t Index                                              = 1ULL;
                for (size_t UniqueSpecifier = 0ULL; UniqueSpecifier < Specifiers->NumUniqueSpecifiers; UniqueSpecifier++) {
                    for (size_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                        if (Specifiers->Specifiers[Specifier].PositionFlag != Position_Unspecified && Specifiers->Specifiers[Specifier].Position == Index) {
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
    }
    
    static void FormatIO_StringTypeIsDefault(FormatSpecifiers *Specifiers, size_t Specifier) {
        AssertIO(Specifiers != NULL);
        AssertIO(Specifier <= Specifiers->NumSpecifiers);
        switch (Specifiers->StringType) {
            case StringType_UTF8:
                Specifiers->Specifiers[Specifier].ModifierType |= ModifierType_UTF8;
                break;
            case StringType_UTF16:
                Specifiers->Specifiers[Specifier].ModifierType |= ModifierType_UTF16;
                break;
            case StringType_UTF32:
                Specifiers->Specifiers[Specifier].ModifierType |= ModifierType_UTF32;
                break;
            case StringType_Unspecified: // Don't do anything
                break;
        }
    }
    
    static void FormatIO_FindSpecifierBoundaries(ImmutableString_UTF32 Format, FormatSpecifiers *Specifiers) {
        AssertIO(Format != NULL);
        AssertIO(Specifiers != NULL);

        for (size_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
            size_t CodePoint = 0ULL;
            if (Specifier > 0) { // Don't re-parse previously visited Specifiers
                CodePoint      = Specifiers->Specifiers[Specifier - 1].FormatEnd + 1;
            }
            while (Specifiers->Specifiers[Specifier].FormatStart == 0x7FFFFFFFFFFFFFFE && Format[CodePoint] != TextIO_NULLTerminator) {
                if (Format[CodePoint] == U'%') {
                    Specifiers->Specifiers[Specifier].FormatStart = CodePoint;
                    CodePoint += 1;
                    break;
                } else {
                    CodePoint += 1;
                }
            }
            while (Specifiers->Specifiers[Specifier].FormatEnd == 0x7FFFFFFFFFFFFFFE && Format[CodePoint] != TextIO_NULLTerminator) {
                switch (Format[CodePoint]) {
                    case U'%':
                    case U'a':
                    case U'A':
                    case U'C':
                    case U'c':
                    case U'd':
                    case U'e':
                    case U'E':
                    case U'F':
                    case U'f':
                    case U'g':
                    case U'G':
                    case U'i':
                    case U'n':
                    case U'o':
                    case U'P':
                    case U'p':
                    case U's':
                    case U'S':
                    case U'u':
                    case U'X':
                    case U'x':
                        Specifiers->Specifiers[Specifier].FormatEnd = CodePoint;
                        break;
                    default:
                        CodePoint += 1;
                        break;
                }
            }
        }
    }
    
    static void UTF32_ParseFormatSpecifiers(ImmutableString_UTF32 Format, FormatSpecifiers *Specifiers, TextIO_StringTypes StringType) {
        AssertIO(Format != NULL);
        AssertIO(Specifiers != NULL);
        AssertIO(StringType != StringType_Unspecified);

        Specifiers->StringType                                                    = StringType;
        size_t CodePoint                                                          = 0;
        size_t Specifier                                                          = 0;

        while (Specifier < Specifiers->NumSpecifiers && Format[CodePoint] != TextIO_NULLTerminator && Specifiers->Specifiers[Specifier].FormatStart == 0x7FFFFFFFFFFFFFFE) {
            FormatIO_FindSpecifierBoundaries(Format, Specifiers);

            CodePoint                                                             = Specifiers->Specifiers[Specifier].FormatEnd;

            switch (Format[CodePoint]) {
                case U'a':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Radix16 | ModifierType_Lowercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'A':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Radix16 | ModifierType_Uppercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'e':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Scientific | ModifierType_Lowercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'E':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Scientific | ModifierType_Uppercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'f':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Lowercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'F':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Uppercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'g':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Shortest | ModifierType_Lowercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'G':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Decimal;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Shortest | ModifierType_Uppercase);
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= (ModifierType_Long);
                            break;
                        }
                    }
                    break;
                case U'c':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_CodeUnit;
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u16"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
#endif
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("w"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF8;
                            break;
                        }
                    } else {
                        FormatIO_StringTypeIsDefault(Specifiers, Specifier);
                    }
                    break;
                case U'C':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_String;
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u16"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
#endif
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("w"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF8;
                            break;
                        }
                    } else {
                        FormatIO_StringTypeIsDefault(Specifiers, Specifier);
                    }
                    break;
                case U's':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_String;
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u16"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
#endif
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("w"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF8;
                            break;
                        }
                    } else {
                        FormatIO_StringTypeIsDefault(Specifiers, Specifier);
                    }
                    break;
                case U'S':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_String;
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("u16"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF32;
#elif PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsWindows)
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
#endif
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("w"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF16;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].ModifierType       |= ModifierType_UTF8;
                            break;
                        }
                    } else {
                        FormatIO_StringTypeIsDefault(Specifiers, Specifier);
                    }
                    break;
                case U'd':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Integer;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Signed | ModifierType_Radix10);
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I64"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        }
                    } else if (CodePoint - 2 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("ll"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("hh"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_8Bit;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_16Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("j"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_IntMax;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("z"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_SizeType;
                            break;
                        }
                    }
                    break;
                case U'i':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Integer;
                    Specifiers->Specifiers[Specifier].ModifierType                = (ModifierType_Signed | ModifierType_Radix10);
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I64"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        }
                    } else if (CodePoint - 2 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("ll"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("hh"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_8Bit;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_16Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("j"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_IntMax;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("z"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_SizeType;
                            break;
                        }
                    }
                    break;
                case U'o':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Integer;
                    Specifiers->Specifiers[Specifier].ModifierType                = (ModifierType_Unsigned | ModifierType_Radix8);
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I64"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        }
                    } else if (CodePoint - 2 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("ll"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("hh"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_8Bit;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_16Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("j"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_IntMax;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("z"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_SizeType;
                            break;
                        }
                    }
                    break;
                case U'x':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Integer;
                    Specifiers->Specifiers[Specifier].ModifierType                = (ModifierType_Unsigned | ModifierType_Radix16 | ModifierType_Lowercase);
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I64"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        }
                    } else if (CodePoint - 2 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("ll"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("hh"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_8Bit;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_16Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("j"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_IntMax;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("z"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_SizeType;
                            break;
                        }
                    }
                    break;
                case U'X':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Integer;
                    Specifiers->Specifiers[Specifier].ModifierType                = (ModifierType_Unsigned | ModifierType_Radix16 | ModifierType_Uppercase);
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I64"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        }
                    } else if (CodePoint - 2 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("ll"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_64Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("hh"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_8Bit;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_16Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("j"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_IntMax;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("z"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_SizeType;
                            break;
                        }
                    }
                    break;
                case U'u':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Integer;
                    Specifiers->Specifiers[Specifier].ModifierType               |= (ModifierType_Unsigned | ModifierType_Radix10);
                    if (CodePoint - 3 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I32"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 3], UTF32String("I64"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 3;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_64Bit;
                            break;
                        }
                    } else if (CodePoint - 2 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("ll"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_64Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 2], UTF32String("hh"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 2;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_8Bit;
                            break;
                        }
                    } else if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("l"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_32Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("h"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_16Bit;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("j"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_IntMax;
                            break;
                        } else if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("z"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_SizeType;
                            break;
                        }
                    }
                    break;
                case U'p':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Pointer;
                    Specifiers->Specifiers[Specifier].ModifierType                = ModifierType_Lowercase;
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("t"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier     |= ModifierLength_PointerDiff;
                            break;
                        }
                    }
                    break;
                case U'P':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_Pointer;
                    Specifiers->Specifiers[Specifier].ModifierType                = ModifierType_Uppercase;
                    if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                        if (UTF32_Compare(&Format[CodePoint - 1], UTF32String("t"))) {
                            Specifiers->Specifiers[Specifier].FormatModifierStart = CodePoint - 1;
                            Specifiers->Specifiers[Specifier].LengthModifier      = ModifierLength_PointerDiff;
                            break;
                        }
                    }
                    break;
                case U'%':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_LiteralPercent;
                    break;
                case U'n':
                    Specifiers->Specifiers[Specifier].BaseType                    = BaseType_RemoveN;
                    break;
            }

            if (Specifiers->Specifiers[Specifier].FormatModifierStart != 0x7FFFFFFFFFFFFFFE && Specifiers->Specifiers[Specifier].FormatModifierStart != Specifiers->Specifiers[Specifier].FormatStart + 1) {
                size_t FlagStart  = Specifiers->Specifiers[Specifier].FormatStart + 1;
                size_t FlagEnd    = Specifiers->Specifiers[Specifier].FormatEnd   - 1;
                size_t CodePoint2 = FlagStart;
                while (CodePoint2 < FlagEnd) {
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
                            if (CodePoint + 1 >= Specifiers->Specifiers[Specifier].FormatStart && CodePoint + 1 <= Specifiers->Specifiers[Specifier].FormatEnd) {
                                if (Format[CodePoint + 1] == U'*') {
                                    Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Asterisk_NextArg;
                                    break;
                                } else if (Format[CodePoint + 1] >= U'0' && Format[CodePoint + 1] <= U'9') {
                                    Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Inline_Digits;
                                    Specifiers->Specifiers[Specifier].Precision = UTF32_String2Integer(Base_Integer | Base_Radix10, &Format[CodePoint + 1]);
                                    CodePoint += UTF32_GetNumDigits(Base_Integer | Base_Radix10, &Format[CodePoint + 1]);
                                    break;
                                }
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
                                break;
                            } else if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal)  {
                                if ((Specifiers->Specifiers[Specifier].ModifierType & ModifierType_Shortest) == ModifierType_Shortest) {
                                    Specifiers->Specifiers[Specifier].Flag  = Flag_Pound_DecimalSuffix;
                                    break;
                                }
                            }
                            break;
                        case U'$':
                            if (CodePoint + 1 > Specifiers->Specifiers[Specifier].FormatStart) {
                                if (Format[CodePoint - 1] == U'*') {
                                    Specifiers->Specifiers[Specifier].PositionFlag = Position_Asterisk_NextArg;
                                    break;
                                } else if (Format[CodePoint - 1] >= U'0' && Format[CodePoint - 1] <= U'9') {
                                    Specifiers->Specifiers[Specifier].PositionFlag = Position_Inline_Digits;
                                    // We need to loop backwards until we stop finding digits
                                    uint64_t DigitStart = CodePoint - 1;
                                    while (Format[DigitStart] >= UTF32Character('0') && Format[DigitStart] <= UTF32Character('9')) {
                                        DigitStart -= 1;
                                    }
                                    Specifiers->Specifiers[Specifier].Position = UTF32_String2Integer(Base_Integer | Base_Radix10, &Format[DigitStart]);
                                    break;
                                }
                            }
                            break;
                        case U'*':
                            if (CodePoint - 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                                if (Format[CodePoint - 1] == U'.') {
                                    Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Asterisk_NextArg;
                                }
                            } else if (CodePoint + 1 >= Specifiers->Specifiers[Specifier].FormatStart) {
                                if (Format[CodePoint + 1] == U'$') {
                                    Specifiers->Specifiers[Specifier].PositionFlag  = Position_Asterisk_NextArg;
                                }
                            } else {
                                Specifiers->Specifiers[Specifier].MinWidthFlag      = MinWidth_Asterisk_NextArg;
                            }
                            break;
                    }
                    CodePoint += 1;
                }
                CodePoint      = Specifiers->Specifiers[Specifier].FormatEnd + 1;
                Specifier     += 1;
            }
        }

        FormatSpecifiers_GetNumUniqueSpecifiers(Specifiers);
    }
    
    static UTF8 *UTF8_CodeUnit2String(UTF8 CodeUnit) {
        UTF8 *String  = UTF8_Init(1);
        AssertIO(String != NULL);

        String[0] = CodeUnit;
        return String;
    }
    
    static UTF16 *UTF16_CodeUnit2String(UTF16 CodeUnit) {
        UTF16 *String  = UTF16_Init(1);
        AssertIO(String != NULL);

        String[0] = CodeUnit;
        return String;
    }
    
    static UTF32 *UTF32_CodeUnit2String(UTF32 CodeUnit) {
        UTF32 *String  = UTF32_Init(1);
        AssertIO(String != NULL);

        String[0] = CodeUnit;
        return String;
    }
    
    static void Format_Specifiers_RetrieveArguments(FormatSpecifiers *Specifiers, va_list Arguments) { // This is where we need to set the Formatted start and end
        AssertIO(Specifiers != NULL);
        uint64_t                 Position                              = 0ULL;
        FormatIO_BaseTypes       BaseType                              = BaseType_Unspecified;
        FormatIO_ModifierTypes   ModifierType                          = ModifierType_Unspecified;
        FormatIO_ModifierLengths LengthType                            = ModifierLength_Unspecified;
        FormatIO_MinWidths       MinWidthType                          = MinWidth_Unspecified;
        FormatIO_Positions       PositionType                          = Position_Unspecified;
        FormatIO_Precisions      PrecisionType                         = Precision_Unspecified;
        FormatIO_Flags           FlagType                              = Flag_Unspecified;
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

            TextIO_Bases Base                                          = ConvertModifierType2Base(&Specifiers->Specifiers[Position]);

            if ((BaseType & BaseType_Integer) == BaseType_Integer) {
                if ((ModifierType & ModifierType_Unsigned) == ModifierType_Unsigned) {
                    if ((LengthType & ModifierLength_8Bit) == ModifierLength_8Bit) {
                        uint8_t   Arg                                  = (uint8_t) va_arg(Arguments, uint32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else if ((LengthType & ModifierLength_16Bit) == ModifierLength_16Bit) {
                        uint16_t  Arg                                  = (uint16_t) va_arg(Arguments, uint32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else if ((LengthType & ModifierLength_32Bit) == ModifierLength_32Bit) {
                        uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else if ((LengthType & ModifierLength_64Bit) == ModifierLength_64Bit) {
                        uint64_t  Arg                                  = va_arg(Arguments, uint64_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else {
                        uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    }
                } else if ((ModifierType & ModifierType_Signed) == ModifierType_Signed) {
                    if ((LengthType & ModifierLength_8Bit) == ModifierLength_8Bit) {
                        int8_t    Arg                                  = (int8_t) va_arg(Arguments, int32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else if ((LengthType & ModifierLength_16Bit) == ModifierLength_16Bit) {
                        int16_t   Arg                                  = (int16_t) va_arg(Arguments, int32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else if ((LengthType & ModifierLength_32Bit) == ModifierLength_32Bit) {
                        int32_t   Arg                                  = va_arg(Arguments, int32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else if ((LengthType & ModifierLength_64Bit) == ModifierLength_64Bit) {
                        int64_t   Arg                                  = va_arg(Arguments, int64_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    } else {
                        int32_t   Arg                                  = va_arg(Arguments, int32_t);
                        Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                    }
                }
            } else if ((BaseType & BaseType_Decimal) == BaseType_Decimal) {
                if ((LengthType & ModifierLength_32Bit) == ModifierLength_32Bit) {
                    float    Arg                                       = (float) va_arg(Arguments, double);
                    Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, Arg);
                } else if ((LengthType & ModifierLength_64Bit) == ModifierLength_64Bit) {
                    double   Arg                                       = va_arg(Arguments, double);
                    Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, Arg);
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
                    Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Value, (Base_Integer | Base_Radix16 | Base_Uppercase));
                } else if ((ModifierType & ModifierType_Lowercase) == ModifierType_Lowercase) {
                    uint64_t Value                                     = va_arg(Arguments, uint64_t);
                    Specifiers->Specifiers[Position].Argument          = UTF32_Integer2String(Value, (Base_Integer | Base_Radix16| Base_Lowercase));
                }
            }
            if (Specifiers->NumSpecifiers == Specifiers->NumUniqueSpecifiers) {
                Position += 1;
            }
        }

        if (MinWidthType == MinWidth_Asterisk_NextArg) { // We need to store the order of MinWidth/Precision/Position as well...
            Specifiers->Specifiers[Position].MinWidth              = va_arg(Arguments, uint32_t);
        }
        if (PrecisionType == Precision_Asterisk_NextArg) {
            Specifiers->Specifiers[Position].Precision             = va_arg(Arguments, uint32_t);
        }
        if (PositionType == Position_Asterisk_NextArg) {
            Specifiers->Specifiers[Position].Position              = va_arg(Arguments, uint32_t);
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

        // Okay then, so now we need to Start worrying about the Formatted Start and End
        uint64_t Specifier = 0ULL;
        while (Specifiers != NULL & Specifier < Specifiers->NumSpecifiers) {
            if (Specifier == 0) {
                Specifiers->Specifiers[Specifier].FormattedStart = Specifiers->Specifiers[Specifier].FormatStart;
                Specifiers->Specifiers[Specifier].FormatEnd      = Specifiers->Specifiers[Specifier].FormatEnd;
            } else {
                /*
                 Format:    "%s: %s: %c"
                 Formatted: "123: 64: 1"
                 
                 
                 [0] = {
                 FormatStart    = 0;
                 FormatEnd      = 1;
                 FormattedStart = 0;
                 FormattedEnd   = 2;
                 }
                 [1] = {
                 FormatStart    = 4;
                 FormatEnd      = 5;
                 FormattedStart = 5;
                 FormattedEnd   = 6;
                 }
                 [2] = {
                 FormatStart    = 8;
                 FormatEnd      = 9;
                 FormattedStart = 9;
                 FormattedEnd   = 9;
                 }
                 So, Copies:
                 Format: 2-3, 6-7
                 Arguments:
                 */
            }
            Specifier += 1;
        }
    }
    
    static uint64_t UTF32_LocateNextPercent(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        uint64_t NextPercent     = -1;

        uint64_t CodePoint   = 0ULL;
        while (String[CodePoint] != TextIO_NULLTerminator && String[CodePoint] != UTF32Character('%')) {
            CodePoint       += 1;
        }
        NextPercent          = CodePoint;

        return NextPercent;
    }
    
    static UTF32 *FormatString_UTF32(ImmutableString_UTF32 Format, FormatSpecifiers *Specifiers, size_t FormattedSize) {
        AssertIO(Format != NULL);
        AssertIO(Specifiers != NULL);
        AssertIO(FormattedSize > 0);
        UTF32 *Formatted                 = NULL;
        Formatted                    = UTF32_Init(FormattedSize);
        AssertIO(Formatted != NULL);
        /*
         Ok, so the string has been allocated, what do we need to do next?
         Copy the string until we find the %
         Loop over the String a codepoint at a time, and loop over the specifiers, if codepoint is between Start and End of a specifier, copy the replacement.

         but that gets complicated because we can't rely on the string position, we need some external reference

         I think the ultimate issue is we don't have enough information.

         We need to know the start and end location in both the FormatString AND the FormattedString.

         So scan for specifiers in the Format string, and then adjust them based on the actual formatted arguments
         */

        /*
         TODO: Use UTF32_ShiftCodePoints and UTF32_ReplaceInPlace to make the replacement's work without repeatedly allocating new strings

         TODO: Start replacing at the end of the string working towards the beginning, to minimize the amount of work it takes
         */
        uint64_t Specifier             = 0ULL;
        uint64_t FormattedCodePoint    = 0ULL;
        uint64_t FormatCodePoint       = 0ULL;
        uint64_t SubstitutionCodePoint = 0ULL;
        uint64_t SubstitutionSize      = 0ULL;
        while (Specifier < Specifiers->NumSpecifiers && FormattedCodePoint < FormattedSize) {
            // Well for each Specifier we need to load the offset and make sure that it matches, so we need 2 while loops then, one for the specifier, one for the offset

            UTF32_LocateNextPercent(&Format[FormatCodePoint]);


            SubstitutionSize           = UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Specifier].Argument);
            if (FormattedCodePoint >= Specifiers->Specifiers[Specifier].FormatStart && FormattedCodePoint <= Specifiers->Specifiers[Specifier].FormatEnd) {
                while (Specifiers->Specifiers[Specifier].Argument[SubstitutionCodePoint] != TextIO_NULLTerminator) {
                    Formatted[FormattedCodePoint] = Specifiers->Specifiers[Specifier].Argument[SubstitutionCodePoint];
                    FormatCodePoint              += 1;
                    SubstitutionCodePoint        += 1;
                    FormattedCodePoint           += 1;
                }
                if ((Specifiers->Specifiers[Specifier].FormatEnd - Specifiers->Specifiers[Specifier].FormatStart) < SubstitutionSize) {
                    FormatCodePoint              += (Specifiers->Specifiers[Specifier].FormatEnd - Specifiers->Specifiers[Specifier].FormatStart) - SubstitutionSize;
                }
            }
        }
































        for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
            /*
             Deduplicate the intermediate allocations, just allocate the final string.

             Also, deduplicate positional arguments
             */

            UTF32                        *Argument                      = NULL;

            if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers && Specifiers->Specifiers[Specifier].PositionFlag != Position_Unspecified) {
                uint64_t Pos                                            = Specifiers->UniqueSpecifiers[Specifiers->Specifiers[Specifier].Position - 1];
                Argument                                                = Specifiers->Specifiers[Pos].Argument;
            } else {
                Argument                                                = Specifiers->Specifiers[Specifier].Argument;
            }

            int64_t                       CurrentStart                  = (int64_t) Specifiers->Specifiers[Specifier].FormatStart;
            int64_t                       NewStart                      = (int64_t) UTF32_LocateNextPercent(Format);
            int64_t                       StartEndDifference            = (int64_t) (NewStart - CurrentStart);
            int64_t                       NewEnd                        = (int64_t) (Specifiers->Specifiers[Specifier].FormatEnd + StartEndDifference);
            Specifiers->Specifiers[Specifier].FormatStart                     = NewStart;
            Specifiers->Specifiers[Specifier].FormatEnd                       = NewEnd;
            uint64_t                      FormatStart                         = Specifiers->Specifiers[Specifier].FormatStart;
            uint64_t                      FormatEnd                           = Specifiers->Specifiers[Specifier].FormatEnd;

            FormatIO_BaseTypes     BaseType                      = Specifiers->Specifiers[Specifier].BaseType;
            FormatIO_Precisions    PrecisionType                 = Specifiers->Specifiers[Specifier].PrecisionFlag;
            FormatIO_ModifierTypes ModifierType                  = Specifiers->Specifiers[Specifier].ModifierType;
            FormatIO_MinWidths     MinWidthType                  = Specifiers->Specifiers[Specifier].MinWidthFlag;
            FormatIO_Flags         FlagType                      = Specifiers->Specifiers[Specifier].Flag;

            if ((BaseType & BaseType_RemoveN) == BaseType_RemoveN) {
                // Remove %n
            } else if ((BaseType & BaseType_LiteralPercent) == BaseType_LiteralPercent) {
                // Print just a single %
            } else {
                if (MinWidthType != MinWidth_Unspecified) {
                    uint64_t MinWidth                                  = Specifiers->Specifiers[Specifier].MinWidth;
                    uint64_t ArgSize                                   = UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Specifier].Argument);
                    uint64_t PadSize                                   = 0ULL;
                    UTF32   *PadString                                 = NULL;
                    if (ArgSize < MinWidth) {
                        PadSize                                        = MinWidth - ArgSize;
                        if (FlagType == Flag_Zero_Pad) {
                            PadString                                  = UTF32_Create(UTF32String("0"), PadSize);
                            UTF32 *OriginalArg                         = Specifiers->Specifiers[Specifier].Argument;
                            Specifiers->Specifiers[Specifier].Argument = UTF32_Insert(OriginalArg, PadString, 0);
                            UTF32_Deinit(OriginalArg);
                        } else if (FlagType == Flag_Space_Pad) {
                            PadString                                  = UTF32_Create(UTF32String(" "), PadSize);
                            UTF32 *OriginalArg                         = Specifiers->Specifiers[Specifier].Argument;
                            Specifiers->Specifiers[Specifier].Argument = UTF32_Insert(OriginalArg, PadString, 0);
                            UTF32_Deinit(OriginalArg);
                        }
                    }
                } else if (PrecisionType != Precision_Unspecified) {
                    // Cut or extend the string so it fits
                }

                UTF32 *Argument                                        = Specifiers->Specifiers[Specifier].Argument;
            }
        }
        return Formatted;
    }
    
    static UTF32 **DeformatString_UTF32(ImmutableString_UTF32 Format, FormatSpecifiers *Specifiers, ImmutableString_UTF32 Formatted) {
        AssertIO(Format != NULL);
        AssertIO(Specifiers != NULL);
        AssertIO(Formatted > 0);
        UTF32 **Deformatted                            = UTF32_StringSet_Init(Specifiers->NumSpecifiers);
        AssertIO(Deformatted != NULL);
        for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
            /*
             Example:
             "Daddy'll buy you a %sing b%s.", "mock", "ird"
             "Daddy'll buy you a mocking bird."
             2 specifiers,
             0:
             start: 19
             end: 20
             1:
             start: 26
             end: 27

             Format string length:    28
             Formatted string length: 32
             specifiers total size = 4
             32 - 28 = 4, so the formatting = 8

             but theres an easier way.

             read from the end of the format specifier until the start of the next specifier, or until the string is done.
             comparing codepoints as you go.

             copy until they start matching

             So, we need to create a StringSet of the number of specifiers

             ----

             %u32s is for type specific formatters
             */
            FormatIO_BaseTypes     BaseType = Specifiers->Specifiers[Specifier].BaseType;
            FormatIO_ModifierTypes Modifier = Specifiers->Specifiers[Specifier].ModifierType;
            uint64_t               FormatStart    = Specifiers->Specifiers[Specifier].FormatStart + 1;

            if ((BaseType & BaseType_Integer) == BaseType_Integer || (BaseType & BaseType_Decimal) == BaseType_Decimal || (BaseType & BaseType_Pointer) == BaseType_Pointer) {
                TextIO_Bases       Base            = ConvertModifierType2Base(&Specifiers->Specifiers[Specifier]);
                ImmutableString_UTF32 Formatted2 = &Formatted[FormatStart];
                uint64_t           SubStringLength = UTF32_GetNumDigits(Base, Formatted2);
                Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, FormatStart, SubStringLength);
            } else if ((BaseType & BaseType_CodeUnit) == BaseType_CodeUnit) {
                Deformatted[Specifier]             = UTF32_CodeUnit2String(Formatted[FormatStart]);
            } else if ((BaseType & BaseType_String) == BaseType_String) {
                uint64_t SubStringLength           = UTF32_GetSubStringLength(Format, Formatted, FormatStart);
                Deformatted[Specifier]             = UTF32_ExtractSubString(Formatted, FormatStart, SubStringLength);
            } else if ((BaseType & BaseType_LiteralPercent) == BaseType_LiteralPercent) {
                Deformatted[Specifier]             = UTF32_Clone(UTF32String("%"));
            }
        }
        return Deformatted;
    }
    
    uint64_t UTF8_GetNumFormatSpecifiers(ImmutableString_UTF8 String) {
        AssertIO(String != NULL);
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodeUnit              = 0ULL;

        while (String[CodeUnit] != TextIO_NULLTerminator) {
            if (String[CodeUnit] == '%') {
                NumSpecifiers     += 1;
            }
            CodeUnit              += 1;
        }

        return NumSpecifiers;
    }
    
    uint64_t UTF16_GetNumFormatSpecifiers(ImmutableString_UTF16 String) {
        AssertIO(String != NULL);
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodeUnit              = 0ULL;

        while (String[CodeUnit] != TextIO_NULLTerminator) {
            if (String[CodeUnit] == UTF16Character('%')) {
                NumSpecifiers     += 1;
            }
            CodeUnit              += 1;
        }
        return NumSpecifiers;
    }
    
    uint64_t UTF32_GetNumFormatSpecifiers(ImmutableString_UTF32 String) {
        AssertIO(String != NULL);
        uint64_t NumSpecifiers         = 0ULL;
        uint64_t CodePoint             = 0ULL;

        while (String[CodePoint] != TextIO_NULLTerminator) {
            if (String[CodePoint] == UTF32Character('%')) {
                NumSpecifiers     += 1;
            }
            CodePoint             += 1;
        }
        return NumSpecifiers;
    }
    
    static uint64_t UTF32_GetFormattedStringSize(ImmutableString_UTF32 Format, FormatSpecifiers *Specifiers) {
        AssertIO(Format != NULL);
        AssertIO(Specifiers != NULL);
        uint64_t FormattedStringSize     = 0ULL;
        FormattedStringSize          = UTF32_GetStringSizeInCodePoints(Format);
        uint64_t SpecifierLength     = 0ULL;
        if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumUniqueSpecifiers; Specifier++) {
                uint64_t Pos         = Specifiers->UniqueSpecifiers[Specifier];
                SpecifierLength      = Specifiers->Specifiers[Pos].FormatEnd - Specifiers->Specifiers[Pos].FormatStart;
                FormattedStringSize -= SpecifierLength;
            }
        } else {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                SpecifierLength      = Specifiers->Specifiers[Specifier].FormatEnd - Specifiers->Specifiers[Specifier].FormatStart;
                FormattedStringSize -= SpecifierLength;
            }
        }
        // Now add the length of each replacement string, which needs to be duplicated
        // We need to know the number of times each positional parameter is used, basically a histogram.
        if (Specifiers->NumUniqueSpecifiers < Specifiers->NumSpecifiers) {
            Specifiers->UniqueSpecifiers = calloc(Specifiers->NumUniqueSpecifiers, sizeof(size_t));
            // Loop over all specifiers, increment .Position in PositionalSpecifierCount
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                Specifiers->UniqueSpecifiers[Specifiers->Specifiers[Specifier].Position] += 1;
            }
            for (uint64_t Unique = 0ULL; Unique < Specifiers->NumUniqueSpecifiers; Unique++) {
                uint64_t Pos          = Specifiers->UniqueSpecifiers[Unique];
                FormattedStringSize  += (UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Pos].Argument) * Specifiers->UniqueSpecifiers[Unique]);
            }
        } else {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                FormattedStringSize += UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Specifier].Argument);
            }
        }
        return FormattedStringSize;
    }
    
    UTF8 *UTF8_Format(ImmutableString_UTF8 Format, ...) {
        AssertIO(Format != NULL);
        UTF8 *Format8                        = NULL;
        uint64_t NumSpecifiers           = UTF8_GetNumFormatSpecifiers(Format);
        if (NumSpecifiers > 0) {
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
        return Format8;
    }
    
    UTF16 *UTF16_Format(ImmutableString_UTF16 Format, ...) {
        AssertIO(Format != NULL);
        UTF16 *Format16                      = NULL;
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
        return Format16;
    }
    
    UTF32 *UTF32_Format(ImmutableString_UTF32 Format, ...) {
        AssertIO(Format != NULL);
        UTF32 *FormattedString               = NULL;
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
        return FormattedString;
    }
    
    UTF8 **UTF8_Deformat(ImmutableString_UTF8 Format, ImmutableString_UTF8 Formatted) {
        AssertIO(Format != NULL);
        AssertIO(Formatted != NULL);
        UTF8 **StringSet                            = NULL;

        uint64_t NumSpecifiers                  = UTF8_GetNumFormatSpecifiers(Format);
        if (NumSpecifiers > 0) {
            ImmutableString_UTF32 Format32      = UTF8_Decode(Format);
            FormatSpecifiers *Specifiers        = FormatSpecifiers_Init(NumSpecifiers);
            UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF8);
            ImmutableString_UTF32 Formatted32   = UTF8_Decode(Formatted);
            UTF32 **Strings32                   = DeformatString_UTF32(Format32, Specifiers, Formatted32);
            FormatSpecifiers_Deinit(Specifiers);
            UTF32_Deinit((UTF32*) Format32);
            UTF32_Deinit((UTF32*) Formatted32);
            StringSet                           = UTF8_StringSet_Encode((const UTF32**) Strings32);
            UTF32_StringSet_Deinit(Strings32);
        }
        return StringSet;
    }
    
    UTF16 **UTF16_Deformat(ImmutableString_UTF16 Format, ImmutableString_UTF16 Formatted) {
        AssertIO(Format != NULL);
        AssertIO(Formatted != NULL);
        UTF16 **StringSet                    = NULL;

        size_t NumSpecifiers             = UTF16_GetNumFormatSpecifiers(Format);
        if (NumSpecifiers > 0) {
            UTF32 *Format32              = UTF16_Decode(Format);
            FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
            UTF32_ParseFormatSpecifiers(Format32, Specifiers, StringType_UTF16);
            UTF32 *Formatted32           = UTF16_Decode(Formatted);
            UTF32 **Strings32            = DeformatString_UTF32(Format32, Specifiers, Formatted32);
            FormatSpecifiers_Deinit(Specifiers);
            UTF32_Deinit(Format32);
            UTF32_Deinit(Formatted32);
            StringSet                    = UTF16_StringSet_Encode((const UTF32**) Strings32);
            UTF32_StringSet_Deinit(Strings32);
        }
        return StringSet;
    }
    
    UTF32 **UTF32_Deformat(ImmutableString_UTF32 Format, ImmutableString_UTF32 Formatted) {
        AssertIO(Format != NULL);
        AssertIO(Formatted != NULL);
        UTF32 **StringSet                    = NULL;

        size_t NumSpecifiers             = UTF32_GetNumFormatSpecifiers(Format);
        if (NumSpecifiers > 0) {
            FormatSpecifiers *Specifiers = FormatSpecifiers_Init(NumSpecifiers);
            UTF32_ParseFormatSpecifiers(Format, Specifiers, StringType_UTF32);
            StringSet                    = DeformatString_UTF32(Format, Specifiers, Formatted);
            FormatSpecifiers_Deinit(Specifiers);
        }
        return StringSet;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
