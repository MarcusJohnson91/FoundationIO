#include <stdarg.h>                    /* Included for va_list, va_copy, va_start, va_end */

#include "../include/StringIO.h"       /* Included for StringIOBases */
#include "../include/FormatIO.h"
#include "../include/Log.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum FormatSpecifier_Flags {
        Flag_Unknown                   = 0,
        Flag_Minus_LeftJustify         = 1,
        Flag_Plus_AddSign              = 2,
        Flag_Space_Pad                 = 4,
        Flag_Pound1_PrefixBase         = 8,
        Flag_Pound2_SuffixDecimal      = 16,
        Flag_Zero_Pad                  = 32,
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
        BaseType_Character             = 4,
        BaseType_String                = 8,
        BaseType_Literal               = 16,
        BaseType_Pointer               = 32,
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
    } FormatSpecifier_TypeModifiers;
    
    typedef enum FormatSpecifier_LengthModifiers {
        Length_Unknown                 = 0,
        Length_8Bit                    = 1,
        Length_16Bit                   = 2,
        Length_32Bit                   = 4,
        Length_64Bit                   = 8,
        Length_SizeType                = 16, // Modifier_Size
        Length_PointerDiff             = 32, // Modifier_PointerDiff
        Length_IntMax                  = 64,
    } FormatSpecifier_LengthModifiers;
    
    typedef struct FormatSpecifier {
        UTF32                          *Argument;        // The actual argument contained in the va_list
        uint64_t                        Offset; // Start location in the format string
        uint64_t                        Length; // Start - Size
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
        bool                            IsPositional;
    } FormatSpecifier;
    
    typedef struct FormatSpecifiers {
        FormatSpecifier             *Specifiers;
        uint64_t                     NumSpecifiers;
        FormatSpecifier_StringTypes  StringType;
    } FormatSpecifiers;
    
    void FormatSpecifiers_Deinit(FormatSpecifiers *Details) {
        if (Details != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                free(Details->Specifiers[Specifier].Argument);
            }
            free(Details->Specifiers);
            free(Details);
        }
    }
    
    FormatSpecifiers *FormatSpecifiers_Init(uint64_t NumSpecifiers) {
        FormatSpecifiers *NewFormatSpecifiers      = calloc(1, sizeof(FormatSpecifiers));
        if (NewFormatSpecifiers != NULL) {
            NewFormatSpecifiers->Specifiers        = calloc(NumSpecifiers, sizeof(FormatSpecifier));
            if (NewFormatSpecifiers->Specifiers != NULL) {
                NewFormatSpecifiers->NumSpecifiers = NumSpecifiers;
                for (uint64_t Specifier = 0ULL; Specifier < NumSpecifiers; Specifier++) {
                    NewFormatSpecifiers->Specifiers[Specifier].Offset = 0xFFFFFFFFFFFFFFFF;
                    NewFormatSpecifiers->Specifiers[Specifier].Length = 0xFFFFFFFFFFFFFFFF;
                }
            } else {
                FormatSpecifiers_Deinit(NewFormatSpecifiers);
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %lld Specifiers"), NumSpecifiers);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate FormatSpecifiers"));
        }
        return NewFormatSpecifiers;
    }
    
#define FormatIOIntegerTableBase2Size        2
#define FormatIOIntegerTableBase8Size        8
#define FormatIOIntegerTableBase10Size      10
#define FormatIOIntegerTableBase16Size      16
#define FormatIODecimalTableSize            11
#define FormatIODecimalTableScientificSize  14
#define FormatIODecimalTableHexadecimalSize 21
    
    
    static const UTF32 FormatIOIntegerTableBase2[FormatIOIntegerTableBase2Size] = {
        U32('0'), U32('1')
    };
    
    static const UTF32 FormatIOIntegerTableBase8[FormatIOIntegerTableBase8Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7')
    };
    
    static const UTF32 FormatIOIntegerTableBase10[FormatIOIntegerTableBase10Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9')
    };
    
    static const UTF32 FormatIOIntegerTableUppercaseBase16[FormatIOIntegerTableBase16Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'), U32('E'), U32('F')
    };
    
    static const UTF32 FormatIOIntegerTableLowercaseBase16[FormatIOIntegerTableBase16Size] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'), U32('e'), U32('f')
    };
    
    static const UTF32 FormatIODecimalTable[FormatIODecimalTableSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('.')
    };
    
    static const UTF32 FormatIODecimalScientificUppercase[FormatIODecimalTableScientificSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('E'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 FormatIODecimalScientificLowercase[FormatIODecimalTableScientificSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('e'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 FormatIODecimalHexUppercase[FormatIODecimalTableHexadecimalSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('A'), U32('B'), U32('C'), U32('D'),
        U32('E'), U32('F'), U32('P'), U32('X'), U32('.'), U32('+'), U32('-')
    };
    
    static const UTF32 FormatIODecimalHexLowercase[FormatIODecimalTableHexadecimalSize] = {
        U32('0'), U32('1'), U32('2'), U32('3'), U32('4'), U32('5'), U32('6'), U32('7'), U32('8'), U32('9'), U32('a'), U32('b'), U32('c'), U32('d'),
        U32('e'), U32('f'), U32('p'), U32('x'), U32('.'), U32('+'), U32('-')
    };
    
    static StringIOBases ConvertTypeModifier2Base(FormatSpecifier_TypeModifiers TypeModifier) {
        StringIOBases Base = UnknownBase;
        if ((TypeModifier & Modifier_Decimal) == Modifier_Decimal) {
            Base           = Decimal;
        } else if ((TypeModifier & Modifier_Scientific) == Modifier_Scientific) {
            Base          |= Decimal | Scientific;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        } else if ((TypeModifier & Modifier_Shortest) == Modifier_Shortest) {
            Base          |= Decimal | Shortest;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        } else if ((TypeModifier & Modifier_Hex) == Modifier_Hex) {
            Base          |= Decimal | Hex;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        } else if ((TypeModifier & Modifier_Base2) == Modifier_Base2) {
            Base          |= Integer | Base2;
        } else if ((TypeModifier & Modifier_Base8) == Modifier_Base8) {
            Base          |= Integer | Base8;
        } else if ((TypeModifier & Modifier_Base10) == Modifier_Base10) {
            Base          |= Integer | Base10;
        } else if ((TypeModifier & Modifier_Base16) == Modifier_Base16) {
            Base          |= Integer | Base16;
            if ((TypeModifier & Modifier_Uppercase) == Modifier_Uppercase) {
                Base      |= Uppercase;
            } else if ((TypeModifier & Modifier_Lowercase) == Modifier_Lowercase) {
                Base      |= Lowercase;
            }
        }
        return Base;
    }
    
    static FormatSpecifiers *UTF32_Specifiers_GetOffsetAndLength(UTF32 *Format, uint64_t NumSpecifiers, FormatSpecifier_StringTypes Type) {
        FormatSpecifiers *Specifiers      = NULL;
        if (Format != NULL) {
            Specifiers                    = FormatSpecifiers_Init(NumSpecifiers);
            if (Specifiers != NULL) {
                Specifiers->StringType    = Type;
                uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(Format);
                uint64_t CodePoint        = 0ULL;
                uint64_t Specifier        = 0ULL;
                while (CodePoint < StringSize) {
                    while (Specifier < Specifiers->NumSpecifiers) {
                        while (Specifiers->Specifiers[Specifier].Offset == 0xFFFFFFFFFFFFFFFF) {
                            if (Format[CodePoint] == U32('%')) {
                                Specifiers->Specifiers[Specifier].Offset = CodePoint;
                            }
                            CodePoint    += 1;
                        }
                        while (Specifiers->Specifiers[Specifier].Length == 0xFFFFFFFFFFFFFFFF) {
                            switch (Format[CodePoint]) {
                                case U32('%'):
                                case U32('a'):
                                case U32('A'):
                                case U32('c'):
                                case U32('C'):
                                case U32('d'):
                                case U32('e'):
                                case U32('E'):
                                case U32('f'):
                                case U32('F'):
                                case U32('g'):
                                case U32('G'):
                                case U32('i'):
                                case U32('o'):
                                case U32('p'):
                                case U32('P'):
                                case U32('s'):
                                case U32('S'):
                                case U32('u'):
                                case U32('x'):
                                case U32('X'):
                                    Specifiers->Specifiers[Specifier].Length = CodePoint - Specifiers->Specifiers[Specifier].Offset;
                                    break;
                            }
                            CodePoint    += 1;
                        }
                        Specifier        += 1;
                    }
                    break;
                }
            } else {
                Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Specifiers;
    }
    
    static void UTF32_Specifiers_GetType(UTF32 *Format, FormatSpecifiers *Specifiers) {
        if (Format != NULL && Specifiers != NULL) {
            uint64_t Specifier = 0ULL;
            while (Specifier < Specifiers->NumSpecifiers) {
                switch (Format[Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length]) {
                    case U32('d'): // Fallthrough, bitches!
                    case U32('i'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Signed | Modifier_Base10);
                        break;
                    case U32('u'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base10);
                        break;
                    case U32('o'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base8);
                        break;
                    case U32('x'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Lowercase);
                        break;
                    case U32('X'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Uppercase);
                        break;
                    case U32('f'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Decimal | Modifier_Lowercase);
                        break;
                    case U32('F'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Decimal | Modifier_Uppercase);
                        break;
                    case U32('e'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Scientific | Modifier_Lowercase);
                        break;
                    case U32('E'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Scientific | Modifier_Uppercase);
                        break;
                    case U32('g'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Shortest | Modifier_Lowercase);
                        break;
                    case U32('G'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Shortest | Modifier_Uppercase);
                        break;
                    case U32('a'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Hex | Modifier_Lowercase);
                        break;
                    case U32('A'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Hex | Modifier_Uppercase);
                        break;
                    case U32('c'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Character;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            }
                        } else {
                            if (Specifiers->StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (Specifiers->StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (Specifiers->StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('C'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Character;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            }
                        } else {
                            if (Specifiers->StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (Specifiers->StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (Specifiers->StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('s'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_String;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            }
                        } else {
                            if (Specifiers->StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (Specifiers->StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (Specifiers->StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('S'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_String;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            }
                        } else {
                            if (Specifiers->StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (Specifiers->StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (Specifiers->StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('p'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Pointer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Lowercase;
                        break;
                    case U32('P'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Pointer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Uppercase;
                        break;
                    case U32('%'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Literal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Percent;
                        break;
                    default:
                        Log(Log_DEBUG, __func__, U8("Unknown Format Specifier %s"), Format[Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length]);
                        break;
                }
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    static void UTF32_Specifiers_GetModifiers(UTF32 *Format, FormatSpecifiers *Specifiers) {
        if (Format != NULL && Specifiers != NULL) {
            uint64_t Specifier = 0ULL;
            uint64_t CodePoint = (Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 2;
            while (Specifier < Specifiers->NumSpecifiers) {
                while (CodePoint < Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) {
                    if (Format[CodePoint] == U32('l') && Format[CodePoint + 1] == U32('l')) {
                        Specifiers->Specifiers[Specifier].LengthModifier |= Length_64Bit;
                        Specifiers->Specifiers[Specifier].Length         += 2;
                        CodePoint                                        += 2;
                    } else if (Format[CodePoint] == U32('h') && Format[CodePoint + 1] == U32('h')) {
                        Specifiers->Specifiers[Specifier].LengthModifier |= Length_8Bit;
                        Specifiers->Specifiers[Specifier].Length         += 2;
                        CodePoint                                        += 2;
                    }
                    
                    if (Format[CodePoint] == U32('l') && (Specifiers->Specifiers[Specifier].LengthModifier & Length_64Bit) != Length_64Bit) {
                        if (((Specifiers->Specifiers[Specifier].BaseType & BaseType_Character) == BaseType_Character) || (Specifiers->Specifiers[Specifier].BaseType & BaseType_String) == BaseType_String) {
                            Specifiers->Specifiers[Specifier].TypeModifier   |= Modifier_UTF16;
                        }
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_32Bit;
                        Specifiers->Specifiers[Specifier].Length             += 1;
                        CodePoint                                            += 1;
                    } else if (Format[CodePoint] == U32('h') && (Specifiers->Specifiers[Specifier].LengthModifier & Length_8Bit) != Length_8Bit) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_16Bit;
                        Specifiers->Specifiers[Specifier].Length             += 1;
                        CodePoint                                            += 1;
                    } else if (Format[CodePoint] == U32('j')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_IntMax;
                        Specifiers->Specifiers[Specifier].Length             += 1;
                        CodePoint                                            += 1;
                    } else if (Format[CodePoint] == U32('z')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_SizeType;
                        Specifiers->Specifiers[Specifier].Length             += 1;
                        CodePoint                                            += 1;
                    } else if (Format[CodePoint] == U32('t')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_PointerDiff;
                        Specifiers->Specifiers[Specifier].Length             += 1;
                        CodePoint                                            += 1;
                    } else if (Format[CodePoint] == U32('L')) {
                        Specifiers->Specifiers[Specifier].TypeModifier       |= Modifier_UTF16;
                        Specifiers->Specifiers[Specifier].Length             += 1;
                        CodePoint                                            += 1;
                    }
                    CodePoint                                                += 1;
                }
                Specifier                                                    += 1;
            }
            
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    void UTF32_Specifiers_GetFlagsWidthPrecisionPosition(UTF32 *Format, FormatSpecifiers *Specifiers) {
        if (Format != NULL && Specifiers != NULL) {
            // Search for Flags, then Numbers as wel as Asterisks
            // if a Number is found then go through and get the number of digits and blah blah blah
            uint64_t Specifier = 0ULL;
            while (Specifier < Specifiers->NumSpecifiers) {
                uint64_t Offset = Specifiers->Specifiers[Specifier].Offset;
                uint64_t Length = Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length;
                
                uint64_t Core   = Offset;
                while (Core < Offset + Length) {
                    if (Format[Core] == U32('0')) { // Flags
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Zero_Pad;
                    } else if (Format[Core] == U32('+')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Plus_AddSign;
                    } else if (Format[Core] == U32('-')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Minus_LeftJustify;
                    } else if (Format[Core] == U32(' ')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Space_Pad;
                    } else if (Format[Core] == U32('#')) {
                        if ((Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base8) == Modifier_Base8 || (Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base16) == Modifier_Base16) {
                            Specifiers->Specifiers[Specifier].Flag |= Flag_Pound1_PrefixBase;
                        } else if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                            Specifiers->Specifiers[Specifier].Flag |= Flag_Pound2_SuffixDecimal;
                        }
                    }
                    
                    if (Format[Core] == U32('*')) {
                        // Make sure there isn't a . in front or a $ behind
                        if (Format[Core - 1] != U32('.')) {
                            // We found a MinWidth!
                            Specifiers->Specifiers[Specifier].MinWidthFlag = MinWidth_Asterisk_NextArg;
                        } else if (Format[Core - 1] == U32('.')) {
                            // We found a Precision!
                            Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Asterisk_NextArg;
                        }
                    }
                    
                    if (
                        Format[Core] == U32('1') ||
                        Format[Core] == U32('2') ||
                        Format[Core] == U32('3') ||
                        Format[Core] == U32('4') ||
                        Format[Core] == U32('5') ||
                        Format[Core] == U32('6') ||
                        Format[Core] == U32('7') ||
                        Format[Core] == U32('8') ||
                        Format[Core] == U32('9')
                        ) {
                        // Get the number of digits followed by checking the end and beginning for $ and . respectively.
                        Core  -= 1; // Go to the first digit
                                    // Check the one before the first digit for a .
                    }
                    
                    if (Format[Core] == U32('.')) { // Precision
                        if (Core + 1 < Offset + Length) {
                            if (Format[Core + 1] == U32('*')) {
                                Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Asterisk_NextArg;
                            } else {
                                Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Number;
                                uint64_t NumDigits = UTF32_GetNumDigits(Integer | Base10, Format, Core + 1);
                                UTF32   *Digits    = UTF32_ExtractSubString(Format, Core + 1, NumDigits);
                                Specifiers->Specifiers[Specifier].Precision = UTF32_String2Integer(Integer | Base10, Digits);
                            }
                        }
                    }
                    
                    if (Format[Core] == U32('$')) {// Position
                        Specifiers->Specifiers[Specifier].IsPositional  = Yes;
                        uint64_t FirstDigit = 0ULL;
                        while (Core > Offset && Core < Offset + Length) {
                            if (
                                Format[Core] == U32('0') ||
                                Format[Core] == U32('1') ||
                                Format[Core] == U32('2') ||
                                Format[Core] == U32('3') ||
                                Format[Core] == U32('4') ||
                                Format[Core] == U32('5') ||
                                Format[Core] == U32('6') ||
                                Format[Core] == U32('7') ||
                                Format[Core] == U32('8') ||
                                Format[Core] == U32('9')) {
                                Core        -= 1;
                            } else {
                                FirstDigit   = Core + 1;
                            }
                        }
                        uint64_t  NumDigits                             = UTF32_GetNumDigits(Integer | Base10, Format, FirstDigit);
                        UTF32    *Digits                                = UTF32_ExtractSubString(Format, FirstDigit, NumDigits);
                        Specifiers->Specifiers[Specifier].PositionalArg = UTF32_String2Integer(Integer | Base10, Digits) - 1;
                    }
                    Core       += 1;
                }
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    FormatSpecifiers *UTF32_ParseFormatString(UTF32 *Format, uint64_t NumSpecifiers, FormatSpecifier_StringTypes StringType) {
        FormatSpecifiers *Specifiers      = NULL;
        if (Format != NULL) {
            Specifiers                    = FormatSpecifiers_Init(NumSpecifiers);
            if (Specifiers != NULL) {
                Specifiers->StringType    = StringType;
                uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(Format);
                uint64_t CodePoint        = 0ULL;
                uint64_t Specifier        = 0ULL;
                while (CodePoint < StringSize) { // Get Offset and Length
                    while (Specifier < Specifiers->NumSpecifiers) {
                        while (Specifiers->Specifiers[Specifier].Offset == 0xFFFFFFFFFFFFFFFF) {
                            if (Format[CodePoint] == U32('%')) {
                                Specifiers->Specifiers[Specifier].Offset = CodePoint;
                            }
                            CodePoint    += 1;
                        }
                        while (Specifiers->Specifiers[Specifier].Length == 0xFFFFFFFFFFFFFFFF) {
                            switch (Format[CodePoint]) {
                                case U32('%'):
                                case U32('a'):
                                case U32('A'):
                                case U32('c'):
                                case U32('C'):
                                case U32('d'):
                                case U32('e'):
                                case U32('E'):
                                case U32('f'):
                                case U32('F'):
                                case U32('g'):
                                case U32('G'):
                                case U32('i'):
                                case U32('o'):
                                case U32('p'):
                                case U32('P'):
                                case U32('s'):
                                case U32('S'):
                                case U32('u'):
                                case U32('x'):
                                case U32('X'):
                                    Specifiers->Specifiers[Specifier].Length = CodePoint - Specifiers->Specifiers[Specifier].Offset;
                                    break;
                            }
                            CodePoint    += 1;
                        }
                        Specifier        += 1;
                    }
                    break;
                }
                
                switch (Format[Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length]) {
                    case U32('d'): // Fallthrough, bitches!
                    case U32('i'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Signed | Modifier_Base10);
                        break;
                    case U32('u'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base10);
                        break;
                    case U32('o'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base8);
                        break;
                    case U32('x'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Lowercase);
                        break;
                    case U32('X'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Uppercase);
                        break;
                    case U32('f'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Decimal | Modifier_Lowercase);
                        break;
                    case U32('F'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Decimal | Modifier_Uppercase);
                        break;
                    case U32('e'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Scientific | Modifier_Lowercase);
                        break;
                    case U32('E'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Scientific | Modifier_Uppercase);
                        break;
                    case U32('g'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Shortest | Modifier_Lowercase);
                        break;
                    case U32('G'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Shortest | Modifier_Uppercase);
                        break;
                    case U32('a'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Hex | Modifier_Lowercase);
                        break;
                    case U32('A'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Hex | Modifier_Uppercase);
                        break;
                    case U32('c'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Character;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            }
                        } else {
                            if (StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('C'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Character;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            }
                        } else {
                            if (StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('s'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_String;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            }
                        } else {
                            if (StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('S'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_String;
                        if (Specifiers->Specifiers[Specifier].Length > 2) {
                            if (Format[(Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length) - 1] == U32('l')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            }
                        } else {
                            if (StringType == UTF8Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            } else if (StringType == UTF16Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            } else if (StringType == UTF32Format) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                            }
                        }
                        break;
                    case U32('p'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Pointer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Lowercase;
                        break;
                    case U32('P'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Pointer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Uppercase;
                        break;
                    case U32('%'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Literal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Percent;
                        break;
                    default:
                        Log(Log_DEBUG, __func__, U8("Unknown Format Specifier %s"), Format[Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length]);
                        break;
                }
                
                for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                    uint64_t Offset = Specifiers->Specifiers[Specifier].Offset;
                    uint64_t Length = Specifiers->Specifiers[Specifier].Length;
                    for (uint64_t Symbol = Offset + 1; Symbol < Length; Symbol++) {
                        if (Format[Symbol] == U32('$')) {
                            Specifiers->Specifiers[Specifier].IsPositional = Yes;
                        } else if (Format[Symbol] == U32('.')) {
                            Specifiers->Specifiers[Specifier].MinWidthFlag = MinWidth_Digits;
                        }
                    }
                }
                
                /*
                 Now what the hell do I do?
                 Well, we need to start parsing the format specifiers
                 
                 
                 
                 
                 Precision: Must be followed by either an asterisk, or decimal digits.
                 MinWidth:  Must be preceded by either an asterisk or decimal digits.
                 Position:  Must be preceded by decimal digits
                 
                 lc: UTF-16
                 ls: UTF-16
                 */
                /*
                 uint64_t Offset = Specifiers->Specifiers[Specifier].Offset;
                 uint64_t Length = Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length;
                 
                 uint64_t Core   = Offset;
                 while (Core < Offset + Length) {
                 if (Format[Core] == U32('0')) { // Flags
                 Specifiers->Specifiers[Specifier].Flag |= Flag_Zero_Pad;
                 } else if (Format[Core] == U32('+')) {
                 Specifiers->Specifiers[Specifier].Flag |= Flag_Plus_AddSign;
                 } else if (Format[Core] == U32('-')) {
                 Specifiers->Specifiers[Specifier].Flag |= Flag_Minus_LeftJustify;
                 } else if (Format[Core] == U32(' ')) {
                 Specifiers->Specifiers[Specifier].Flag |= Flag_Space_Pad;
                 } else if (Format[Core] == U32('#')) {
                 if ((Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base8) == Modifier_Base8 || (Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base16) == Modifier_Base16) { // We need to know the BaseType first
                 Specifiers->Specifiers[Specifier].Flag |= Flag_Pound1_PrefixBase;
                 } else if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                 Specifiers->Specifiers[Specifier].Flag |= Flag_Pound2_SuffixDecimal;
                 }
                 }
                 
                 if (Format[Core] == U32('*')) {
                 // Make sure there isn't a . in front or a $ behind
                 if (Format[Core - 1] != U32('.')) {
                 // We found a MinWidth!
                 Specifiers->Specifiers[Specifier].MinWidthFlag = MinWidth_Asterisk_NextArg;
                 } else if (Format[Core - 1] == U32('.')) {
                 // We found a Precision!
                 Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Asterisk_NextArg;
                 }
                 }
                 */
                /*
                 if (
                 Format[Core] == U32('1') ||
                 Format[Core] == U32('2') ||
                 Format[Core] == U32('3') ||
                 Format[Core] == U32('4') ||
                 Format[Core] == U32('5') ||
                 Format[Core] == U32('6') ||
                 Format[Core] == U32('7') ||
                 Format[Core] == U32('8') ||
                 Format[Core] == U32('9')
                 ) {
                 // Get the number of digits followed by checking the end and beginning for $ and . respectively.
                 Core  -= 1; // Go to the first digit
                 // Check the one before the first digit for a .
                 }
                 */
                /*
                 if (Format[Core] == U32('.')) { // Precision
                 if (Core + 1 < Offset + Length) {
                 if (Format[Core + 1] == U32('*')) {
                 Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Asterisk_NextArg;
                 } else {
                 Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Number;
                 uint64_t NumDigits = UTF32_GetNumDigits(Integer | Base10, Format, Core + 1);
                 UTF32   *Digits    = UTF32_ExtractSubString(Format, Core + 1, NumDigits);
                 Specifiers->Specifiers[Specifier].Precision = UTF32_String2Integer(Integer | Base10, Digits);
                 }
                 }
                 }
                 
                 if (Format[Core] == U32('$')) {// Position
                 Specifiers->Specifiers[Specifier].IsPositional  = Yes;
                 uint64_t FirstDigit = 0ULL;
                 while (Core > Offset && Core < Offset + Length) {
                 if (
                 Format[Core] == U32('0') ||
                 Format[Core] == U32('1') ||
                 Format[Core] == U32('2') ||
                 Format[Core] == U32('3') ||
                 Format[Core] == U32('4') ||
                 Format[Core] == U32('5') ||
                 Format[Core] == U32('6') ||
                 Format[Core] == U32('7') ||
                 Format[Core] == U32('8') ||
                 Format[Core] == U32('9')) {
                 Core        -= 1;
                 } else {
                 FirstDigit   = Core + 1;
                 }
                 }
                 uint64_t  NumDigits                             = UTF32_GetNumDigits(Integer | Base10, Format, FirstDigit);
                 UTF32    *Digits                                = UTF32_ExtractSubString(Format, FirstDigit, NumDigits);
                 Specifiers->Specifiers[Specifier].PositionalArg = UTF32_String2Integer(Integer | Base10, Digits) - 1;
                 }
                 Core       += 1;
                 }
                 */
            } else {
                Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Specifiers;
    }
    
    FormatSpecifiers *UTF32_ParseFormatSpecifiers(UTF32 *Format, uint64_t NumSpecifiers, FormatSpecifier_StringTypes StringType) {
        FormatSpecifiers *Details         = NULL;
        if (Format != NULL && StringType != StringType_Unknown) {
            Details                       = FormatSpecifiers_Init(NumSpecifiers);
            if (Details != NULL) {
                Details->StringType       = StringType;
                uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(Format);
                uint64_t Specifier        = 0ULL;
                uint64_t CodePoint        = 0ULL;
                uint64_t CodePoint2       = 0ULL;
                while (Specifier < NumSpecifiers) {
                    while (CodePoint < StringSize) {
                        if (Format[CodePoint] == U32('%')) { // Found a Format Specifier
                            Details->Specifiers[Specifier].Offset = CodePoint;
                            Details->Specifiers[Specifier].Length = 1; // 1 to account for the percent
                            CodePoint2                                     = CodePoint + 1;
                            /*
                             We have to disambiguate between Flag 0, and Octal, or even Hex input.
                             
                             Specifier is the only Mandatory field, everything else is Optional.
                             */
                            if (CodePoint2 < StringSize) {
                                /* Flags, may be multiple */
                                if (Format[CodePoint2] == U32('+')) {
                                    Details->Specifiers[Specifier].Flag     |= Flag_Plus_AddSign;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32('-')) {
                                    Details->Specifiers[Specifier].Flag     |= Flag_Minus_LeftJustify;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32(' ')) { // If Space and + are both used, Ignore the Space
                                    Details->Specifiers[Specifier].Flag     |= Flag_Space_Pad;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32('0')) {
                                    Details->Specifiers[Specifier].Flag     |= Flag_Zero_Pad;
                                    CodePoint2                              += 1;
                                }
                                if (Format[CodePoint2] == U32('#')) {
                                    if ((Details->Specifiers[Specifier].TypeModifier & Modifier_Base8) == Modifier_Base8 || (Details->Specifiers[Specifier].TypeModifier & Modifier_Base16) == Modifier_Base16) {
                                        Details->Specifiers[Specifier].Flag |= Flag_Pound1_PrefixBase;
                                    } else if ((Details->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                                        Details->Specifiers[Specifier].Flag |= Flag_Pound2_SuffixDecimal;
                                    }
                                    CodePoint2                              += 1;
                                }
                                /* Flags */
                                
                                /* Positional, MinWidth */
                                uint64_t NumDigits   = 0ULL;
                                int64_t  Digits      = 0LL;
                                UTF32   *DigitString = NULL;
                                switch (Format[CodePoint2]) {
                                    case U32('0'):
                                    case U32('1'):
                                    case U32('2'):
                                    case U32('3'):
                                    case U32('4'):
                                    case U32('5'):
                                    case U32('6'):
                                    case U32('7'):
                                    case U32('8'):
                                    case U32('9'):
                                        NumDigits   = UTF32_GetNumDigits(Integer | Base10, Format, CodePoint2);
                                        DigitString = UTF32_ExtractSubString(Format, CodePoint2, NumDigits);
                                        Digits      = UTF32_String2Integer(Integer | Base10, DigitString);
                                        Details->Specifiers[Specifier].Length += NumDigits;
                                        CodePoint2 += NumDigits;
                                        break;
                                }
                                
                                if (CodePoint2 < StringSize && NumDigits > 0) {
                                    if (Format[CodePoint2] == U32('$')) { // Positional
                                        Details->Specifiers[Specifier].IsPositional     = Yes;
                                        Details->Specifiers[Specifier].PositionalArg    = Digits - 1;
                                        CodePoint2                                     += 1;
                                        Details->Specifiers[Specifier].Length += 1;
                                    } else {
                                        Details->Specifiers[Specifier].MinWidthFlag     = MinWidth_Digits;
                                        Details->Specifiers[Specifier].MinWidth         = Digits;
                                    }
                                } else {
                                    if (Format[CodePoint2] == U32('*')) { // MinWidth
                                        Details->Specifiers[Specifier].MinWidthFlag     = MinWidth_Asterisk_NextArg;
                                        CodePoint2                                     += 1;
                                        Details->Specifiers[Specifier].Length += 1;
                                    }
                                }
                                /* Positional,MinWidth */
                                
                                /* Precision */
                                if (Format[CodePoint2] == U32('.')) {
                                    Details->Specifiers[Specifier].Length             += 1;
                                    CodePoint2                                                 += 1;
                                    if (Format[CodePoint2] < StringSize) {
                                        uint64_t NumDigits                                      = 0ULL;
                                        UTF32   *DigitString                                    = NULL;
                                        int64_t  Digits                                         = 0LL;
                                        switch (Format[CodePoint2]) {
                                            case U32('*'):
                                                Details->Specifiers[Specifier].PrecisionFlag   |= Precision_Dot_Asterisk_NextArg;
                                                Details->Specifiers[Specifier].Length += 1;
                                                CodePoint2                                     += 1;
                                                break;
                                            case U32('0'):
                                            case U32('1'):
                                            case U32('2'):
                                            case U32('3'):
                                            case U32('4'):
                                            case U32('5'):
                                            case U32('6'):
                                            case U32('7'):
                                            case U32('8'):
                                            case U32('9'):
                                                Details->Specifiers[Specifier].PrecisionFlag   |= Precision_Dot_Number;
                                                NumDigits                                       = UTF32_GetNumDigits(Integer | Base10, Format, CodePoint2);
                                                DigitString                                     = UTF32_ExtractSubString(Format, CodePoint2, NumDigits);
                                                Digits                                          = UTF32_String2Integer(Integer | Base10, DigitString);
                                                
                                                Details->Specifiers[Specifier].MinWidth         = Digits;
                                                CodePoint2                                     += NumDigits;
                                                Details->Specifiers[Specifier].Length += NumDigits;
                                                break;
                                        }
                                    }
                                }
                                /* Precision */
                                
                                /* Length Modifiers */
                                if (CodePoint2 + 1 < StringSize) {
                                    if (Format[CodePoint2] == U32('l') && Format[CodePoint2 + 1] == U32('l')) {
                                        Details->Specifiers[Specifier].LengthModifier  |= Length_64Bit;
                                        Details->Specifiers[Specifier].Length += 2;
                                        CodePoint2                                     += 2;
                                    } else if (Format[CodePoint2] == U32('h') && Format[CodePoint2 + 1] == U32('h')) {
                                        Details->Specifiers[Specifier].LengthModifier  |= Length_8Bit;
                                        Details->Specifiers[Specifier].Length += 2;
                                        CodePoint2                                     += 2;
                                    }
                                }
                                
                                if (Format[CodePoint2] == U32('l') && (Details->Specifiers[Specifier].LengthModifier & Length_64Bit) != Length_64Bit) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_32Bit;
                                    Details->Specifiers[Specifier].Length  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('h') && (Details->Specifiers[Specifier].LengthModifier & Length_8Bit) != Length_8Bit) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_16Bit;
                                    Details->Specifiers[Specifier].Length  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('j')) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_IntMax;
                                    Details->Specifiers[Specifier].Length  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('z')) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_SizeType;
                                    Details->Specifiers[Specifier].Length  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('t')) {
                                    Details->Specifiers[Specifier].LengthModifier   |= Length_PointerDiff;
                                    Details->Specifiers[Specifier].Length  += 1;
                                    CodePoint2                                      += 1;
                                } else if (Format[CodePoint2] == U32('L')) {
                                    Details->Specifiers[Specifier].TypeModifier     |= Modifier_UTF16;
                                    Details->Specifiers[Specifier].Length  += 1;
                                    CodePoint2                                      += 1;
                                }
                                /* Length Modifiers */
                                
                                /* Type */
                                switch (Format[CodePoint2]) {
                                    case U32('d'): // Fallthrough, bitches!
                                    case U32('i'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Signed | Modifier_Base10);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('u'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base10);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('o'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base8);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('x'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('X'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Integer;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('f'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Decimal | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('F'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Decimal | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('e'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Scientific | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('E'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Scientific | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('g'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Shortest | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('G'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Shortest | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('a'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Hex | Modifier_Lowercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('A'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Decimal;
                                        Details->Specifiers[Specifier].TypeModifier     |= (Modifier_Hex | Modifier_Uppercase);
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('c'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Character;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('C'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Character;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('s'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_String;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('S'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_String;
                                        if (StringType == UTF8Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                        } else if (StringType == UTF16Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                        } else if (StringType == UTF32Format) {
                                            Details->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                        }
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('%'):
                                        Details->Specifiers[Specifier].Length  += 1;
                                        Details->Specifiers[Specifier].BaseType         |= BaseType_Literal;
                                        Details->Specifiers[Specifier].TypeModifier     |= Modifier_Percent;
                                        CodePoint2                                      += 1;
                                        CodePoint                                        = CodePoint2;
                                        break;
                                    case U32('p'):
                                        Details->Specifiers[Specifier].BaseType          |= BaseType_Pointer;
                                    default:
                                        Log(Log_DEBUG, __func__, U8("Unknown Format Specifier %s"), Format[CodePoint]);
                                        break;
                                }
                                Specifier  += 1;
                            }
                        }
                        CodePoint          += 1;
                    }
                }
                /*
                 Lets reorganize these CodePoint and Specifier loops that way we don't do unnessicary work.
                 
                 Specifier should just be a vriable that gets incremented each time a specifier is found, we can use a while loop for it.
                 
                 while(Specifier < NumSpecifiers)
                 
                 Now, should the CodePoint lop be inside or outside of the Dowhile
                 */
                
                // Now we need to get the number of Positional arguments
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate FormatSpecifiers"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Details;
    }
    
    static UTF8 *UTF8_CodeUnit2String(UTF8 CodeUnit) {
        UTF8 *String  = calloc(1 + FoundationIONULLTerminatorSize, sizeof(UTF8));
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    static UTF16 *UTF16_CodeUnit2String(UTF16 CodeUnit) {
        UTF16 *String  = calloc(1 + FoundationIONULLTerminatorSize, sizeof(UTF16));
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    static UTF32 *UTF32_CodeUnit2String(UTF32 CodeUnit) {
        UTF32 *String  = calloc(1 + FoundationIONULLTerminatorSize, sizeof(UTF32));
        if (String != NULL) {
            String[0] = CodeUnit;
        }
        return String;
    }
    
    UTF32 *FormatString_UTF32(UTF32 *Format, FormatSpecifiers *Details, va_list VariadicArguments) {
        UTF32 *Formatted                                                       = NULL;
        UTF32 *FormatTemp                                                      = Format;
        if (Format != NULL && Details != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                if (Details->Specifiers[Specifier].IsPositional == No) {
                    FormatSpecifier_BaseTypes       Base                       = Details->Specifiers[Specifier].BaseType;
                    FormatSpecifier_TypeModifiers   Type                       = Details->Specifiers[Specifier].TypeModifier;
                    FormatSpecifier_LengthModifiers Length                     = Details->Specifiers[Specifier].LengthModifier;
                    uint64_t                        Position                   = Specifier;
                    
                    if ((Base & BaseType_Literal) == BaseType_Literal && Type == Modifier_Percent) {
                        Details->Specifiers[Position].Argument                 = UTF32_Clone(U32("%"));
                    } else if ((Base & BaseType_Character) == BaseType_Character) {
                        if ((Type & Modifier_UTF8) == Modifier_UTF8) {
                            UTF8  VariadicArgument                             = va_arg(VariadicArguments, UTF8);
                            UTF8 *String                                       = UTF8_CodeUnit2String(VariadicArgument);
                            Details->Specifiers[Position].Argument             = UTF8_Decode(String);
                            free(String);
                        } else if ((Type & Modifier_UTF16) == Modifier_UTF16) {
                            UTF16  VariadicArgument                            = va_arg(VariadicArguments, UTF16);
                            UTF16 *String                                      = UTF16_CodeUnit2String(VariadicArgument);
                            Details->Specifiers[Position].Argument             = UTF16_Decode(String);
                            free(String);
                        } else if ((Type & Modifier_UTF32) == Modifier_UTF32) {
                            UTF32  VariadicArgument                            = va_arg(VariadicArguments, UTF32);
                            Details->Specifiers[Position].Argument             = UTF32_CodeUnit2String(VariadicArgument);
                        }
                    } else if ((Base & BaseType_String) == BaseType_String) {
                        if ((Type & Modifier_UTF8) == Modifier_UTF8) {
                            UTF8  *VariadicArgument                            = va_arg(VariadicArguments, UTF8*);
                            UTF32 *VariadicArgument32                          = UTF8_Decode(VariadicArgument);
                            Details->Specifiers[Position].Argument             = VariadicArgument32;
                            free(VariadicArgument);
                        } else if ((Type & Modifier_UTF16) == Modifier_UTF16) {
                            UTF16 *VariadicArgument                            = va_arg(VariadicArguments, UTF16*);
                            UTF32 *VariadicArgument32                          = UTF16_Decode(VariadicArgument);
                            Details->Specifiers[Position].Argument             = VariadicArgument32;
                            free(VariadicArgument);
                        } else if ((Type & Modifier_UTF32) == Modifier_UTF32) {
                            Details->Specifiers[Position].Argument             = va_arg(VariadicArguments, UTF32*);
                        }
                    } else if ((Base & BaseType_Integer) == BaseType_Integer) {
                        if ((Type & Modifier_Unsigned) == Modifier_Unsigned) {
                            if ((Length & Length_8Bit) == Length_8Bit) {
                                uint8_t  Arg                                   = va_arg(VariadicArguments, uint8_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            } else if ((Length & Length_16Bit) == Length_16Bit) {
                                uint16_t  Arg                                  = va_arg(VariadicArguments, uint16_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            } else if ((Length & Length_32Bit) == Length_32Bit) {
                                uint32_t  Arg                                  = va_arg(VariadicArguments, uint32_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            } else if ((Length & Length_64Bit) == Length_64Bit) {
                                uint64_t  Arg                                  = va_arg(VariadicArguments, uint64_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            }
                        } else if ((Type & Modifier_Signed) == Modifier_Signed) {
                            if ((Length & Length_8Bit) == Length_8Bit) {
                                int8_t  Arg                                    = va_arg(VariadicArguments, int8_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            } else if ((Length & Length_16Bit) == Length_16Bit) {
                                int16_t  Arg                                   = va_arg(VariadicArguments, int16_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            } else if ((Length & Length_32Bit) == Length_32Bit) {
                                int32_t  Arg                                   = va_arg(VariadicArguments, int32_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            } else if ((Length & Length_64Bit) == Length_64Bit) {
                                int64_t  Arg                                   = va_arg(VariadicArguments, int64_t);
                                Details->Specifiers[Position].Argument         = UTF32_Integer2String(ConvertTypeModifier2Base(Type), Arg);
                            }
                        }
                    } else if ((Base & BaseType_Decimal) == BaseType_Decimal) {
                        if ((Length & Length_32Bit) == Length_32Bit) {
                            float    Arg                                       = va_arg(VariadicArguments, float);
                            Details->Specifiers[Position].Argument             = UTF32_Decimal2String(ConvertTypeModifier2Base(Type), (double) Arg);
                        } else if ((Length & Length_64Bit) == Length_64Bit) {
                            double   Arg                                       = va_arg(VariadicArguments, double);
                            Details->Specifiers[Position].Argument             = UTF32_Decimal2String(ConvertTypeModifier2Base(Type), Arg);
                        }
                    }
                }
            }
            
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                if (Details->Specifiers[Specifier].IsPositional == Yes) {
                    uint64_t Position                                          = Details->Specifiers[Specifier].PositionalArg;
                    if (Position < Details->NumSpecifiers) {
                        if (Details->Specifiers[Specifier].Argument != NULL) {
                            Details->Specifiers[Specifier].Argument            = UTF32_Clone(Details->Specifiers[Position].Argument);
                        }
                    } else {
                        Log(Log_DEBUG, __func__, U8("Position %llu is greater than there are Specifiers %llu, invalid"), Position, Details->NumSpecifiers);
                    }
                }
            }
            
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
                UTF32    *OriginalTemp = FormatTemp;
                UTF32    *Argument     = NULL;
                uint64_t  Offset       = Details->Specifiers[Specifier].Offset;
                uint64_t  Length       = Details->Specifiers[Specifier].Length;
                Argument               = Details->Specifiers[Specifier].Argument;
                
                FormatTemp             = UTF32_ReplaceSubString(FormatTemp, Argument, Offset, Length);
                
                for (uint64_t Specifier2 = Specifier + 1; Specifier2 < Details->NumSpecifiers; Specifier2++) {
                    uint64_t ArgumentSize                            = UTF32_GetStringSizeInCodePoints(Details->Specifiers[Specifier2].Argument);
                    uint64_t Length                                  = Details->Specifiers[Specifier2].Length;
                    Details->Specifiers[Specifier2].Offset          += Minimum(Length, ArgumentSize);
                    
                }
                
                if (OriginalTemp != Format) {
                    free(OriginalTemp);
                }
            }
            Formatted                  = FormatTemp;
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("Format Pointer is NULL"));
        } else if (Details == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
        return Formatted;
    }
    
    UTF32 **DeformatString_UTF32(UTF32 *Format, UTF32 *Result, FormatSpecifiers *Details) {
        UTF32 **Deformatted                            = NULL;
        uint64_t NumStrings                            = 0ULL;
        for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) {
            if (Details->Specifiers[Specifier].BaseType != BaseType_Literal && Details->Specifiers[Specifier].TypeModifier != Modifier_Percent) {
                NumStrings                            += 1;
            }
        }
        Deformatted                                    = UTF32_StringArray_Init(NumStrings);
        if (Deformatted != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Details->NumSpecifiers; Specifier++) { // Stringify each specifier
                FormatSpecifier_BaseTypes     BaseType = Details->Specifiers[Specifier].BaseType;
                FormatSpecifier_TypeModifiers Modifier = Details->Specifiers[Specifier].TypeModifier;
                uint64_t                      Offset   = Details->Specifiers[Specifier].Offset;
                uint64_t                      Length   = 0ULL;
                if (BaseType == BaseType_Integer || BaseType == BaseType_Decimal) {
                    StringIOBases             Base     = ConvertTypeModifier2Base(Modifier);
                    Length                             = UTF32_GetNumDigits(Base, Result, Offset);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Result, Offset, Length);
                    Length                             = 0ULL;
                } else if (BaseType == BaseType_Character) { // Character = Grapheme
                    Deformatted[Specifier]             = UTF32_ExtractGrapheme(Result, Offset);
                } else if (BaseType == BaseType_String) {
                    uint64_t SubstringStart            = Details->Specifiers[Specifier].Offset + Details->Specifiers[Specifier].Length;
                    uint64_t SubstringEnd              = Details->Specifiers[Specifier].Offset;
                    
                    UTF32 *SubString                   = UTF32_ExtractSubString(Format, SubstringStart, SubstringEnd);
                    uint64_t EndOffset                 = UTF32_FindSubString(Result, SubString, Details->Specifiers[Specifier].Offset, -1);
                    free(SubString);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Result, SubstringStart, EndOffset);
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
        return Deformatted;
    }
    
#ifdef __cplusplus
}
#endif
