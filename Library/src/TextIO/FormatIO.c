#include <stdarg.h>                          /* Included for va_list, va_copy, va_start, va_end */

#include "../include/StringIO.h"             /* Included for StringIOBases */
#include "../include/Private/FormatIO.h"
#include "../include/Log.h"
#include "../include/Private/NumberTables.h" /* Included for the Number tables */

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
        BaseType_Character             = 4,
        BaseType_String                = 8,
        BaseType_Literal               = 16,
        BaseType_Pointer               = 32,
        BaseType_Unsupported           = 64,
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
        uint64_t                        Offset;          // Start location in the format string
        uint64_t                        Length;          // Start - Size
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
                    Specifiers->Specifiers[Specifier].Offset          = 0x7FFFFFFFFFFFFFFE;
                    Specifiers->Specifiers[Specifier].Length          = 0x7FFFFFFFFFFFFFFE;
                }
            } else {
                FormatSpecifiers_Deinit(Specifiers);
                Log(Log_DEBUG, __func__, U8("Couldn't allocate %llu Specifiers"), NumSpecifiers);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate FormatSpecifiers"));
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
    
    static FormatSpecifiers *UTF32_Specifiers_GetOffsetAndLength(UTF32 *Format, uint64_t NumSpecifiers, StringIOStringTypes Type) {
        FormatSpecifiers *Specifiers      = NULL;
        if (Format != NULL) {
            Specifiers                    = FormatSpecifiers_Init(NumSpecifiers);
            if (Specifiers != NULL) {
                Specifiers->StringType    = Type;
                uint64_t StringSize       = UTF32_GetStringSizeInCodePoints(Format);
                uint64_t CodePoint        = 0ULL;
                uint64_t CodePoint2       = 0ULL;
                uint64_t Specifier        = 0ULL;
                
                while (Specifier < Specifiers->NumSpecifiers) {
                    while (CodePoint < StringSize) {
                        if (Format[CodePoint] == U32('%')) {
                            Specifiers->Specifiers[Specifier].Offset             = CodePoint;
                            CodePoint2                                           = CodePoint + 1;
                            while (CodePoint2 < StringSize) {
                                switch (Format[CodePoint2]) {
                                    case U32('%'):
                                    case U32('a'):
                                    case U32('A'):
                                    case U32('b'):
                                    case U32('B'):
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
                                    case U32('n'):
                                    case U32('o'):
                                    case U32('p'):
                                    case U32('P'):
                                    case U32('s'):
                                    case U32('S'):
                                    case U32('u'):
                                    case U32('x'):
                                    case U32('X'):
                                        Specifiers->Specifiers[Specifier].Length = CodePoint2 - CodePoint;
                                        break;
                                }
                                CodePoint2                                      += 1;
                            }
                        }
                        CodePoint                                               += 1;
                    }
                    Specifier                                                   += 1;
                }
            } else {
                Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Specifiers;
    }
    
    /*
     FormatIO Custom Formatter:
     
     Let's say that we want to dump the BitBuffer struct, we'd take in a pointer to the bitbuffer, we'd then extract the NumBits variable, the BitOffset variable, and the Buffer variable printing X number of bytes.
     
     Something like: "BitBuffer: NumBits: %llu, BitOffset: %llu, Buffer[BitOffset - 64: BitOffset]: 0x80 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     
     Now we'd need a wrapper function to do all that and write it to a string.
     
     then that String could be appended via Format to an error string.
     
     Really all we need is to have a specifier to specify UTF-32, once we have an unambigious way to do that, we're golden.
     
     and we could drop the UTF8 and UTF16 versions and just write the UTF-32 version that would then convert that string to whatever the output format should be
     
     So: UTF32 *Format_BitBuffer(BitBuffer *BitB, uint8_t Length);
     
     So then in all the places were we might want to dump the contents of a BitBuffer we could simply call Format_BitBuffer as part of the Log arguments.
     */
    
    static void UTF32_Specifiers_GetType(UTF32 *Format, FormatSpecifiers *Specifiers) {
        if (Format != NULL && Specifiers != NULL) {
            uint64_t Specifier = 0ULL;
            while (Specifier < Specifiers->NumSpecifiers) {
                uint64_t Offset    = Specifiers->Specifiers[Specifier].Offset;
                uint64_t Length    = Specifiers->Specifiers[Specifier].Length;
                uint64_t CodePoint = Offset + Length;
                
                switch (Format[CodePoint]) {
                    case U32('a'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Hex | Modifier_Lowercase);
                        break;
                    case U32('A'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Hex | Modifier_Uppercase);
                        break;
                    case U32('b'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base2 | Modifier_Lowercase);
                        break;
                    case U32('B'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base2 | Modifier_Uppercase);
                        break;
                    case U32('c'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Character;
                        if (Length > 2) {
                            uint64_t PreviousCodePoint = (Offset + Length) - 1;
                            if (Format[PreviousCodePoint] == U32('l') || Format[PreviousCodePoint] == U32('L')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else if (Format[PreviousCodePoint] == U32('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            }
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
                    case U32('C'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Character;
                        if (Length > 2) {
                            uint64_t PreviousCodePoint = (Offset + Length) - 1;
                            if (Format[PreviousCodePoint] == U32('l') || Format[PreviousCodePoint] == U32('L')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else if (Format[PreviousCodePoint] == U32('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            }
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
                    case U32('d'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Signed | Modifier_Base10);
                        break;
                    case U32('e'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Scientific | Modifier_Lowercase);
                        break;
                    case U32('E'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Scientific | Modifier_Uppercase);
                        break;
                    case U32('f'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Decimal | Modifier_Lowercase);
                        break;
                    case U32('F'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Decimal | Modifier_Uppercase);
                        break;
                    case U32('g'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Shortest | Modifier_Lowercase);
                        break;
                    case U32('G'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Decimal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Shortest | Modifier_Uppercase);
                        break;
                    case U32('i'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Signed | Modifier_Base10);
                        break;
                    case U32('n'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Unsupported;
                        break;
                    case U32('o'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base8);
                        break;
                    case U32('p'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Pointer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Lowercase;
                        break;
                    case U32('P'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Pointer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Uppercase;
                        break;
                    case U32('s'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_String;
                        if (Length > 2) {
                            uint64_t PreviousCodePoint = (Offset + Length) - 1;
                            if (Format[PreviousCodePoint] == U32('l') || Format[PreviousCodePoint] == U32('L')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else if (Format[PreviousCodePoint] == U32('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                            }
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
                    case U32('S'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_String;
                        if (Length > 2) {
                            uint64_t PreviousCodePoint = (Offset + Length) - 1;
                            if (Format[PreviousCodePoint] == U32('l') || Format[PreviousCodePoint] == U32('L')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF8;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else if (Format[PreviousCodePoint] == U32('U')) {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF32;
                                Specifiers->Specifiers[Specifier].ModifierSize += 1;
                            } else {
                                Specifiers->Specifiers[Specifier].TypeModifier |= Modifier_UTF16;
                            }
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
                    case U32('u'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base10);
                        break;
                    case U32('x'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Lowercase);
                        break;
                    case U32('X'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Integer;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= (Modifier_Unsigned | Modifier_Base16 | Modifier_Uppercase);
                        break;
                    case U32('%'):
                        Specifiers->Specifiers[Specifier].BaseType             |= BaseType_Literal;
                        Specifiers->Specifiers[Specifier].TypeModifier         |= Modifier_Percent;
                        break;
                    default:
                        Log(Log_DEBUG, __func__, U8("Unknown Format Specifier %s"), Format[Offset + Length]);
                        break;
                }
                Specifier += 1;
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
            while (Specifier < Specifiers->NumSpecifiers) {
                uint64_t Offset    = Specifiers->Specifiers[Specifier].Offset;
                uint64_t Length    = Specifiers->Specifiers[Specifier].Length;
                uint64_t CodePoint = Offset + Length;
                
                if (Length > 2) {
                    CodePoint     -= 2;
                } else {
                    CodePoint     -= 1;
                }
                
                if (Length > 2) {
                    if (Format[CodePoint] == U32('l') && Format[CodePoint + 1] == U32('l')) {
                        Specifiers->Specifiers[Specifier].LengthModifier |= Length_64Bit;
                        Specifiers->Specifiers[Specifier].ModifierSize   += 2;
                    } else if (Format[CodePoint] == U32('h') && Format[CodePoint + 1] == U32('h')) {
                        Specifiers->Specifiers[Specifier].LengthModifier |= Length_8Bit;
                        Specifiers->Specifiers[Specifier].ModifierSize   += 2;
                    } else if (Format[CodePoint] == U32('l') && (Specifiers->Specifiers[Specifier].LengthModifier & Length_64Bit) != Length_64Bit) {
                        if (((Specifiers->Specifiers[Specifier].BaseType & BaseType_Character) == BaseType_Character) || (Specifiers->Specifiers[Specifier].BaseType & BaseType_String) == BaseType_String) {
                            Specifiers->Specifiers[Specifier].TypeModifier   |= Modifier_UTF16;
                        } else {
                            Specifiers->Specifiers[Specifier].LengthModifier |= Length_32Bit;
                            Specifiers->Specifiers[Specifier].ModifierSize   += 1;
                        }
                    } else if (Format[CodePoint] == U32('h') && (Specifiers->Specifiers[Specifier].LengthModifier & Length_8Bit) != Length_8Bit) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_16Bit;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('j')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_IntMax;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('z')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_SizeType;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('t')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_PointerDiff;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('L')) {
                        Specifiers->Specifiers[Specifier].TypeModifier       |= Modifier_UTF16;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('U')) {
                        if (((Specifiers->Specifiers[Specifier].BaseType & BaseType_Character) == BaseType_Character) || (Specifiers->Specifiers[Specifier].BaseType & BaseType_String) == BaseType_String) {
                            Specifiers->Specifiers[Specifier].TypeModifier   |= Modifier_UTF32;
                        }
                    }
                    Specifier                                                += 1;
                } else {
                    if (Format[CodePoint] == U32('l') && (Specifiers->Specifiers[Specifier].LengthModifier & Length_64Bit) != Length_64Bit) {
                        if (((Specifiers->Specifiers[Specifier].BaseType & BaseType_Character) == BaseType_Character) || (Specifiers->Specifiers[Specifier].BaseType & BaseType_String) == BaseType_String) {
                            Specifiers->Specifiers[Specifier].TypeModifier   |= Modifier_UTF16;
                        }
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_32Bit;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('h') && (Specifiers->Specifiers[Specifier].LengthModifier & Length_8Bit) != Length_8Bit) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_16Bit;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('j')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_IntMax;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('z')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_SizeType;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('t')) {
                        Specifiers->Specifiers[Specifier].LengthModifier     |= Length_PointerDiff;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('L')) {
                        Specifiers->Specifiers[Specifier].TypeModifier       |= Modifier_UTF16;
                        Specifiers->Specifiers[Specifier].ModifierSize       += 1;
                    } else if (Format[CodePoint] == U32('U')) {
                        if (((Specifiers->Specifiers[Specifier].BaseType & BaseType_Character) == BaseType_Character) || (Specifiers->Specifiers[Specifier].BaseType & BaseType_String) == BaseType_String) {
                            Specifiers->Specifiers[Specifier].TypeModifier   |= Modifier_UTF32;
                        }
                    }
                    Specifier                                                += 1;
                }
            }
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
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
                    if (Format[CodePoint] == U32('0')) {
                        if (Format[CodePoint + 1] != FoundationIONULLTerminator) {
                            if ((Format[CodePoint - 1] < U32('0') || Format[CodePoint - 1] > U32('9')) && (Format[CodePoint + 1] < U32('0') || Format[CodePoint + 1] > U32('9'))) { // The zero is standalone, therefore it's a flag
                                Specifiers->Specifiers[Specifier].Flag |= Flag_Zero_Pad;
                            }
                        } else { // Then just check that the revious codepoint isn't a digit, and if its not then you have a valid 0 flag
                            if (Format[CodePoint - 1] < U32('0') || Format[CodePoint] > U32('9')) {
                                Specifiers->Specifiers[Specifier].Flag |= Flag_Zero_Pad;
                            }
                        }
                    } else if (Format[CodePoint] == U32(' ')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Space_Pad;
                    } else if (Format[CodePoint] == U32('+')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Plus_AddSign;
                    } else if (Format[CodePoint] == U32('-')) {
                        Specifiers->Specifiers[Specifier].Flag |= Flag_Minus_LeftJustify;
                    } else if (Format[CodePoint] == U32('#')) {
                        if ((Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base8) == Modifier_Base8 || (Specifiers->Specifiers[Specifier].TypeModifier & Modifier_Base16) == Modifier_Base16) {
                            Specifiers->Specifiers[Specifier].Flag |= Flag_Pound1_PrefixBase;
                        } else if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Decimal) == BaseType_Decimal) {
                            Specifiers->Specifiers[Specifier].Flag |= Flag_Pound2_SuffixDecimal;
                        }
                    } else if (Format[CodePoint] >= U32('0') && Format[CodePoint] <= U32('9')) {
                        uint64_t DigitStart           = CodePoint;
                        while (Format[CodePoint] >= U32('0') && Format[CodePoint] <= U32('9') && (CodePoint > Offset) && (CodePoint < Offset + Length)) {
                            DigitStart               -= 1;
                            CodePoint                -= 1;
                        }
                        DigitStart                   += 1;
                        CodePoint                    += 1;
                        uint64_t NumConsecutiveDigits = UTF32_GetNumDigits(Base_Integer_Radix10, Format, CodePoint);
                        UTF32   *DigitString          = UTF32_ExtractSubString(Format, DigitStart, NumConsecutiveDigits);
                        uint64_t Number               = UTF32_String2Integer(Base_Integer_Radix10, DigitString);
                        if (Format[DigitStart - 1] == U32('.')) { // Precision
                            Specifiers->Specifiers[Specifier].PrecisionFlag = Precision_Dot_Number;
                            Specifiers->Specifiers[Specifier].Precision     = Number;
                        } else if (Format[DigitStart + NumConsecutiveDigits] == U32('$')) { // Positional
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
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
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
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    FormatSpecifiers *UTF32_ParseFormatString(UTF32 *Format, uint64_t NumSpecifiers, StringIOStringTypes StringType) {
        FormatSpecifiers *Specifiers      = NULL;
        if (Format != NULL) {
            Specifiers                    = UTF32_Specifiers_GetOffsetAndLength(Format, NumSpecifiers, StringType);
            UTF32_Specifiers_GetType(Format, Specifiers);
            UTF32_Specifiers_GetModifiers(Format, Specifiers);
            UTF32_Specifiers_GetFlagsWidthPrecisionPosition(Format, Specifiers);
            UTF32_Specifiers_GetDuplicateSpecifiers(Specifiers);
        } else {
            Log(Log_DEBUG, __func__, U8("String Pointer is NULL"));
        }
        return Specifiers;
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
    
    void Format_Specifiers_RetrieveArguments(FormatSpecifiers *Specifiers, va_list Arguments) __attribute__((no_sanitize("signed-integer-overflow", "integer"))) {
        if (Specifiers != NULL) {
            uint64_t                        Specifier0 = 0ULL;
            uint64_t                        Position   = 0x7FFFFFFFFFFFFFFE;
            FormatSpecifier_BaseTypes       BaseType   = BaseType_Unknown;
            FormatSpecifier_TypeModifiers   Modifier   = Modifier_Unknown;
            FormatSpecifier_LengthModifiers Length     = Length_Unknown;
            while (Specifier0 < Specifiers->NumSpecifiers - Specifiers->NumDuplicateSpecifiers) {
                if (Specifiers->Specifiers[Specifier0].IsPositional == Yes) {
                    Position                           = Specifiers->Specifiers[Specifier0].PositionalArg;
                    BaseType                           = Specifiers->Specifiers[Position].BaseType;
                    Modifier                           = Specifiers->Specifiers[Position].TypeModifier;
                    Length                             = Specifiers->Specifiers[Position].LengthModifier;
                } else {
                    Position                           = Specifier0;
                    BaseType                           = Specifiers->Specifiers[Position].BaseType;
                    Modifier                           = Specifiers->Specifiers[Position].TypeModifier;
                    Length                             = Specifiers->Specifiers[Position].LengthModifier;
                }
                
                /*
                 Specifier                               = 0
                 Specifiers->Specifiers[0].IsPositional  = Yes
                 Specifiers->Specifiers[0].PositionalArg = 1
                 
                 Specifiers->Specifiers[1].BaseType      = BaseType_Integer
                 Specifiers->Specifiers[1].TypeModifier  = Modifier_Unsigned | Modifier_64Bit
                 
                 
                 */
                
                if (Specifiers->Specifiers[Position].MinWidthFlag == MinWidth_Asterisk_NextArg) {
                    Specifiers->Specifiers[Position].MinWidth              = va_arg(Arguments, uint32_t);
                }
                if (Specifiers->Specifiers[Position].PrecisionFlag == Precision_Dot_Asterisk_NextArg) {
                    Specifiers->Specifiers[Position].Precision             = va_arg(Arguments, uint32_t);
                }
                
                StringIOBases Base                                         = ConvertTypeModifier2Base(Modifier);
                
                if ((BaseType & BaseType_Integer) == BaseType_Integer) {
                    if ((Modifier & Modifier_Unsigned) == Modifier_Unsigned) {
                        if ((Length & Length_8Bit) == Length_8Bit) {
                            uint8_t   Arg                                  = va_arg(Arguments, uint8_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_16Bit) == Length_16Bit) {
                            uint16_t  Arg                                  = va_arg(Arguments, uint16_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_32Bit) == Length_32Bit) {
                            uint32_t  Arg                                  = va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_64Bit) == Length_64Bit) {
                            uint64_t  Arg                                  = va_arg(Arguments, uint64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        }
                    } else if ((Modifier & Modifier_Signed) == Modifier_Signed) {
                        if ((Length & Length_8Bit) == Length_8Bit) {
                            int8_t    Arg                                  = va_arg(Arguments, uint8_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_16Bit) == Length_16Bit) {
                            int16_t   Arg                                  = va_arg(Arguments, uint16_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_32Bit) == Length_32Bit) {
                            int32_t   Arg                                  = va_arg(Arguments, uint32_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        } else if ((Length & Length_64Bit) == Length_64Bit) {
                            int64_t   Arg                                  = va_arg(Arguments, uint64_t);
                            Specifiers->Specifiers[Position].Argument      = UTF32_Integer2String(Base, Arg);
                        }
                    }
                } else if ((BaseType & BaseType_Decimal) == BaseType_Decimal) {
                    if ((Length & Length_32Bit) == Length_32Bit) {
                        float    Arg                                       = va_arg(Arguments, float);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, (double) Arg);
                    } else if ((Length & Length_64Bit) == Length_64Bit) {
                        double   Arg                                       = va_arg(Arguments, double);
                        Specifiers->Specifiers[Position].Argument          = UTF32_Decimal2String(Base, Arg);
                    }
                } else if ((BaseType & BaseType_Character) == BaseType_Character) {
                    if ((Modifier & Modifier_UTF8) == Modifier_UTF8) {
                        UTF8  VariadicArgument                             = va_arg(Arguments, UTF8);
                        UTF8 *String                                       = UTF8_CodeUnit2String(VariadicArgument);
                        Specifiers->Specifiers[Position].Argument          = UTF8_Decode(String);
                        UTF8_Deinit(String);
                    } else if ((Modifier & Modifier_UTF16) == Modifier_UTF16) {
                        UTF16  VariadicArgument                            = va_arg(Arguments, UTF16);
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
                    Specifiers->Specifiers[Position].Argument              = UTF32_Clone(U32("%"));
                }
                Specifier0                                                 += 1;
            } // This works fine
            
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
            
            uint64_t Specifier1                                = 0ULL;
            while (Specifier1 < Specifiers->NumSpecifiers) { // This is where it's going wrong.
                uint64_t Position                              = 0ULL;
                if (Specifiers->Specifiers[Specifier1].IsPositional == Yes) {
                    Position                                   = Specifiers->Specifiers[Specifier1].PositionalArg;
                } else {
                    Position                                   = Specifier1;
                }
                
                uint64_t ReplacementSize                       = UTF32_GetStringSizeInCodePoints(Specifiers->Specifiers[Position].Argument);
                uint64_t MinimumWidth                          = Specifiers->Specifiers[Position].MinWidth;
                uint64_t Precision                             = Specifiers->Specifiers[Position].Precision;
                uint64_t Length                                = Specifiers->Specifiers[Position].Length;
                int64_t  Skip                                  = 0;
                if (Specifiers->Specifiers[Position].MinWidthFlag != MinWidth_Unknown) {
                    if (ReplacementSize < MinimumWidth) {
                        Skip                                  += MinimumWidth - ReplacementSize;
                    }
                }
                if (Specifiers->Specifiers[Position].PrecisionFlag != Precision_Unknown) {
                    if (ReplacementSize > Precision) {
                        Skip                                  -= ReplacementSize - Precision;
                    }
                }
                
                if (ReplacementSize < Length + 1) {
                    Skip                                      += (ReplacementSize - Length) - 1;
                } else {
                    Skip                                      += (ReplacementSize - (Length + 1));
                }
                
                uint64_t Specifier2                            = Specifier1 + 1;
                while (Specifier2 < Specifiers->NumSpecifiers) {
                    Specifiers->Specifiers[Specifier2].Offset += Skip;
                    Specifier2                                += 1;
                }
                Specifier1                                    += 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
    }
    
    UTF32 *FormatString_UTF32(UTF32 *Format, FormatSpecifiers *Specifiers) {
        UTF32 *Formatted                                                       = NULL;
        UTF32 *FormatTemp                                                      = Format;
        if (Format != NULL && Specifiers != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
                UTF32    *OriginalTemp = FormatTemp;
                UTF32    *Argument     = Specifiers->Specifiers[Specifier].Argument;
                uint64_t  Offset       = Specifiers->Specifiers[Specifier].Offset;
                uint64_t  Length       = Specifiers->Specifiers[Specifier].Length;
                
                // Wait, it needs to substitute arguments in just Specifier order tho...
                /*
                if (Specifiers->Specifiers[Specifier].IsPositional == Yes) {
                    uint64_t Position  = Specifiers->Specifiers[Specifier].PositionalArg;
                    Argument           = Specifiers->Specifiers[Position].Argument;
                    Offset             = Specifiers->Specifiers[Specifier].Offset;
                    Length             = Specifiers->Specifiers[Specifier].Length;
                } else {
                    Argument           = Specifiers->Specifiers[Specifier].Argument;
                    Offset             = Specifiers->Specifiers[Specifier].Offset;
                    Length             = Specifiers->Specifiers[Specifier].Length;
                }
                */
                if ((Specifiers->Specifiers[Specifier].BaseType & BaseType_Unsupported) != BaseType_Unsupported) {
                    FormatTemp         = UTF32_ReplaceSubString(OriginalTemp, Argument, Offset, Length);
                } else if (Specifiers->Specifiers[Specifier].BaseType == BaseType_Unsupported) {
                    if (Offset > 0 && Offset + Length + 1 != FoundationIONULLTerminator) {
                        if (OriginalTemp[Offset - 1] == U32(' ') && OriginalTemp[Offset + Length + 1] == U32(' ')) {
                            Length    += 1;
                        }
                    }
                    FormatTemp         = UTF32_Stitch(OriginalTemp, Offset, Length);
                }
                
                if (OriginalTemp != Format) {
                    UTF32_Deinit(OriginalTemp);
                }
            }
            Formatted                  = FormatTemp;
        } else if (Format == NULL) {
            Log(Log_DEBUG, __func__, U8("Format Pointer is NULL"));
        } else if (Specifiers == NULL) {
            Log(Log_DEBUG, __func__, U8("FormatSpecifiers Pointer is NULL"));
        }
        return Formatted;
    }
    
    UTF32 **DeformatString_UTF32(UTF32 *Format, UTF32 *Result, FormatSpecifiers *Specifiers) {
        UTF32 **Deformatted                            = NULL;
        uint64_t NumStrings                            = 0ULL;
        for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) {
            if (Specifiers->Specifiers[Specifier].BaseType != BaseType_Literal && Specifiers->Specifiers[Specifier].TypeModifier != Modifier_Percent) {
                NumStrings                            += 1;
            }
        }
        Deformatted                                    = UTF32_StringArray_Init(NumStrings);
        if (Deformatted != NULL) {
            for (uint64_t Specifier = 0ULL; Specifier < Specifiers->NumSpecifiers; Specifier++) { // Stringify each specifier
                FormatSpecifier_BaseTypes     BaseType = Specifiers->Specifiers[Specifier].BaseType;
                FormatSpecifier_TypeModifiers Modifier = Specifiers->Specifiers[Specifier].TypeModifier;
                uint64_t                      Offset   = Specifiers->Specifiers[Specifier].Offset;
                uint64_t                      Length   = 0ULL;
                if (BaseType == BaseType_Integer || BaseType == BaseType_Decimal) {
                    StringIOBases             Base     = ConvertTypeModifier2Base(Modifier);
                    Length                             = UTF32_GetNumDigits(Base, Result, Offset);
                    Deformatted[Specifier]             = UTF32_ExtractSubString(Result, Offset, Length);
                    Length                             = 0ULL;
                } else if (BaseType == BaseType_Character) { // Character = Grapheme
                    Deformatted[Specifier]             = UTF32_ExtractGrapheme(Result, Offset);
                } else if (BaseType == BaseType_String) {
                    uint64_t SubstringStart            = Specifiers->Specifiers[Specifier].Offset + Specifiers->Specifiers[Specifier].Length;
                    uint64_t SubstringEnd              = Specifiers->Specifiers[Specifier].Offset;
                    
                    UTF32 *SubString                   = UTF32_ExtractSubString(Format, SubstringStart, SubstringEnd);
                    uint64_t EndOffset                 = UTF32_FindSubString(Result, SubString, Specifiers->Specifiers[Specifier].Offset, -1);
                    UTF32_Deinit(SubString);
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
