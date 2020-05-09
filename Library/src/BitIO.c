#include "../include/BitIO.h"                /* Included for our declarations */
#include "../include/Constants.h"            /* Included for BitMaskTables */
#include "../include/CryptographyIO.h"       /* Included for Entropy_GenerateInteger for GUUID_Generate */
#include "../include/MathIO.h"               /* Included for Integer functions */
#include "../include/UnicodeIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/UnicodeIO/LogIO.h"      /* Included for Logging */
#include "../include/UnicodeIO/StringIO.h"   /* Included for StringIO's declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum BitIO_Constants {
        GUUIDString_Size                = 20,
        BinaryGUUID_Size                = 16,
    } BitIO_Constants;
    
    /* Start BitBuffer section */
    typedef struct BitBuffer {
        uint8_t   *Buffer;
        uint64_t   BitOffset;
        uint64_t   NumBits;
    } BitBuffer;
    
    typedef struct BitInput {
        FILE            *File;
        uint64_t         FilePosition;
        uint64_t         FileSize;
        int              Socket;
        BitIO_FileTypes  FileType;
    } BitInput;
    
    typedef struct BitOutput {
        FILE            *File;
        uint64_t         FilePosition;
        int              Socket;
        BitIO_FileTypes  FileType;
    } BitOutput;
    
    BitBuffer *BitBuffer_Init(uint64_t BitBufferSize) {
        BitBuffer *BitB                  = (BitBuffer*) calloc(1, sizeof(BitBuffer));
        if (BitB != NULL && BitBufferSize > 0) {
            BitB->Buffer                 = (uint8_t*) calloc(BitBufferSize, sizeof(uint8_t));
            if (BitB->Buffer != NULL) {
                BitB->NumBits            = Bytes2Bits(BitBufferSize);
            } else {
                BitBuffer_Deinit(BitB);
                BitB                     = NULL;
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate %lld bits for BitBuffer's buffer"), BitBufferSize);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate BitBuffer"));
        }
        return BitB;
    }
    
    void BitBuffer_Read(BitBuffer *BitB, BitInput *BitI) {
        if (BitB != NULL && BitI != NULL) {
            uint64_t Bytes2Read    = Bits2Bytes(BitB->NumBits - BitB->BitOffset, RoundingType_Down);
            uint8_t  Bits2Save     = BitB->BitOffset % 8;
            if (Bits2Save > 0) {
                BitB->Buffer[0]    = 0;
                uint8_t Saved      = BitB->Buffer[Bytes2Read + 1] & BitMaskTable[Bits2Save - 1];
                BitB->Buffer[0]    = Saved;
                BitB->BitOffset    = Bits2Save;
                for (uint64_t Byte = (uint64_t) Bits2Bytes(BitB->BitOffset, RoundingType_Up); Byte < (uint64_t) Bits2Bytes(BitB->NumBits, RoundingType_Down); Byte++) {
                    BitB->Buffer[Byte] = 0;
                }
            }
            uint64_t BytesRead     = 0ULL;
            if (BitI->FileType == FileType_File) {
                BytesRead          = PlatformIO_File_Read(BitB->Buffer, 1, Bytes2Read, BitI->File);
            } else if (BitI->FileType == FileType_Socket) {
                BytesRead          = PlatformIO_Socket_Read(BitI->Socket, BitB->Buffer, Bytes2Read);
            }
            if (BytesRead == Bytes2Read) {
                BitB->NumBits      = (BytesRead * 8) + BitB->BitOffset;
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Num bytes read %llu does not match num bytes requested %llu"), BytesRead, Bytes2Read);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (BitI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetSize(BitBuffer *BitB) {
        uint64_t BitBufferSize = 0ULL;
        if (BitB != NULL) {
            BitBufferSize      = BitB->NumBits;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return BitBufferSize;
    }
    
    void BitBuffer_SetSize(BitBuffer *BitB, uint64_t Bits) {
        if (BitB != NULL) {
            BitB->NumBits = Bits;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetPosition(BitBuffer *BitB) {
        uint64_t Position = 0ULL;
        if (BitB != NULL) {
            Position = BitB->BitOffset;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Position;
    }
    
    void BitBuffer_SetPosition(BitBuffer *BitB, uint64_t Offset) {
        if (BitB != NULL) {
            BitB->BitOffset = Offset;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    uint64_t BitBuffer_GetBitsFree(BitBuffer *BitB) {
        uint64_t BitsFree = 0ULL;
        if (BitB != NULL) {
            BitsFree      = BitB->NumBits - BitB->BitOffset;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return BitsFree;
    }
    
    bool BitBuffer_IsAligned(BitBuffer *BitB, uint8_t AlignmentSize) {
        bool AlignmentStatus    = No;
        if (BitB != NULL && (AlignmentSize != 1 || AlignmentSize % 2 != 0)) {
            if (BitB->BitOffset % (AlignmentSize * 8) == 0) {
                AlignmentStatus = Yes;
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize != 1 || AlignmentSize % 2 != 0) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("AlignmentSize: %d isn't 1 or an integer power of 2"), AlignmentSize);
        }
        return AlignmentStatus;
    }
    
    void BitBuffer_Align(BitBuffer *BitB, uint8_t AlignmentSize) {
        if (BitB != NULL && (AlignmentSize == 1 || AlignmentSize % 2 == 0)) {
            int64_t  AlignmentSizeInBits = Bytes2Bits(AlignmentSize);
            int64_t  Bits2Align          = AlignmentSizeInBits - (BitB->BitOffset % AlignmentSizeInBits);
            if (BitB->BitOffset + Bits2Align > BitB->NumBits) {
                uint8_t *Buffer_Old      = BitB->Buffer;
                BitB->Buffer             = (uint8_t*) realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, RoundingType_Up));
                if (BitB->Buffer != NULL) {
                    free(Buffer_Old);
                } else {
                    BitB->Buffer         = Buffer_Old;
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Realloc failed"));
                }
                BitB->NumBits           += Bits2Align;
            }
            BitB->BitOffset             += Bits2Align;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (AlignmentSize == 1 || AlignmentSize % 2 == 0) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("AlignmentSize: %d isn't a multiple of 2"), AlignmentSize);
        }
    }
    
    void BitBuffer_Seek(BitBuffer *BitB, int64_t Bits2Seek) {
        if (BitB != NULL) {
            BitB->BitOffset += Bits2Seek;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    uint8_t BitBuffer_Erase(BitBuffer *BitB, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (BitB != NULL) {
            uint64_t BufferSize = Bits2Bytes(BitB->NumBits, RoundingType_Up);
            for (uint64_t Byte = 0ULL; Byte < BufferSize; Byte++) {
                BitB->Buffer[Byte] = NewValue;
            }
            Verification = BitB->Buffer[0];
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Verification;
    }
    
    void BitBuffer_Resize(BitBuffer *BitB, uint64_t NewSize) {
        if (BitB != NULL && NewSize * 8 >= BitB->BitOffset) {
            uint8_t *Buffer_Old = BitB->Buffer;
            BitB->Buffer        = (uint8_t*) realloc(BitB->Buffer, NewSize);
            if (BitB->Buffer != NULL) {
                BitB->BitOffset = 0;
                BitB->NumBits   = NewSize * 8;
                free(Buffer_Old);
            } else {
                BitB->Buffer    = Buffer_Old;
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Realloc failed"));
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_Refresh(BitBuffer *BitB, BitInput *BitI) {
        if (BitB != NULL && BitI != NULL) {
            uint64_t Bytes2Read      = BitB->NumBits / 8;
            uint64_t BytesRead       = 0ULL;
            if (BitI->FileType == FileType_File) {
                BytesRead            = PlatformIO_File_Read(BitB->Buffer, 1, Bytes2Read, BitI->File);
            } else if (BitI->FileType == FileType_Socket) {
                BytesRead            = PlatformIO_Socket_Read(BitI->Socket, BitB->Buffer, Bytes2Read);
            }
            if (BytesRead != Bytes2Read) {
                uint8_t *Buffer_Old  = BitB->Buffer;
                BitB->Buffer         = (uint8_t*) realloc(BitB->Buffer, BytesRead);
                if (BitB->Buffer != NULL) {
                    BitB->BitOffset  = 0;
                    BitB->NumBits    = BytesRead * 8;
                    free(Buffer_Old);
                } else {
                    BitB->Buffer    = Buffer_Old;
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Realloc failed"));
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (BitI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
    }
    
    void BitBuffer_Copy(BitBuffer *Source, BitBuffer *Destination, uint64_t BitStart, uint64_t BitEnd) {
        if (Source != NULL && Destination != NULL && BitStart < BitEnd && BitStart <= Source->NumBits && BitEnd <= Source->NumBits) {
            uint64_t NumBits2Copy = BitEnd - BitStart;
            if (BitStart % 8 == 0 && BitEnd % 8 == 0 && NumBits2Copy % 8 == 0) {
                Destination->NumBits = NumBits2Copy;
                for (uint64_t Byte = BitStart / 8; Byte < BitEnd / 8; Byte++) {
                    Destination->Buffer[Byte - (BitStart / 8)] = Source->Buffer[Byte];
                }
            } else {
                for (uint64_t Bit = BitStart; Bit < BitEnd / 8; Bit++) {
                    Destination->Buffer[Bit / 8] = Source->Buffer[Bit / 8];
                }
            }
        } else if (Source == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Source Pointer is NULL"));
        } else if (Destination == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Destination Pointer is NULL"));
        } else if (BitStart >= BitEnd) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitStart %lld is greater than or equal to BitEnd %lld"), BitStart, BitEnd);
        } else if (BitStart >= Source->NumBits || BitEnd >= Source->NumBits) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitStart %lld or BitEnd %lld is greater than there are bits in Source %lld"), BitStart, BitEnd, Source->NumBits);
        }
    }
    
    static uint64_t BitBuffer_Extract_LSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            uint8_t Bits2Read                     = NumBits2Extract;
            while (Bits2Read > 0) {
                uint8_t  BitsInCurrentByte        = Bits2ExtractFromByte(BitB->BitOffset + Bits2Read);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInCurrentByte, Bits2Read);
                uint8_t  Shift                    = BitsInCurrentByte - Bits2Get;
                uint8_t  BitMask                  = BitMaskTable[Bits2Get - 1] << Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset + Bits2Read, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                uint8_t  ApplyBits                = ExtractedBits >> Shift;
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ApplyBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            }
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_LSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            uint8_t Bits2Read                     = NumBits2Extract;
            while (Bits2Read > 0) {
                uint8_t  BitsInCurrentByte        = Bits2ExtractFromByte(BitB->BitOffset + Bits2Read);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInCurrentByte, Bits2Read);
                uint8_t  Shift                    = BitsInCurrentByte - Bits2Get;
                uint8_t  BitMask                  = BitMaskTable[Bits2Get - 1] << Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset + Bits2Read, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ApplyBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            }
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_MSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            uint8_t Bits2Read                     = NumBits2Extract;
            while (Bits2Read > 0) {
                uint8_t  BitsInCurrentByte        = Bits2ExtractFromByte(BitB->BitOffset);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInCurrentByte, Bits2Read);
                uint8_t  Shift                    = BitsInCurrentByte - Bits2Get;
                uint8_t  BitMask                  = BitMaskTable[Bits2Get - 1] << Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                uint8_t  ApplyBits                = ExtractedBits >> Shift;
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ApplyBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            }
        }
        return Extracted;
    }
    
    static uint64_t BitBuffer_Extract_MSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Extract) {
        uint64_t Extracted = 0ULL;
        if (NumBits2Extract > 0) {
            uint8_t Bits2Read                     = NumBits2Extract;
            while (Bits2Read > 0) {
                uint8_t  BitsInCurrentByte        = Bits2ExtractFromByte(BitB->BitOffset);
                uint8_t  Bits2Get                 = (uint8_t) Minimum(BitsInCurrentByte, Bits2Read);
                uint8_t  Shift                    = BitsInCurrentByte - Bits2Get;
                uint8_t  BitMask                  = BitMaskTable[Bits2Get - 1] << Shift;
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                uint8_t  ExtractedBits            = BitB->Buffer[Byte] & BitMask;
                ExtractedBits                   >>= Shift;
                
                Extracted                       <<= Bits2Get;
                Extracted                        |= ExtractedBits;
                
                Bits2Read                        -= Bits2Get;
                BitB->BitOffset                  += Bits2Get;
            }
        }
        return Extracted;
    }
    
    static void BitBuffer_Append_LSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t  Bits2Write                   = NumBits2Append;
            while (Bits2Write > 0) {
                uint8_t  BitsInCurrentByte        = 8 - (BitB->BitOffset % 8);
                uint8_t  Bits2Append2CurrentByte  = (uint8_t) Minimum(BitsInCurrentByte, Bits2Write);
                uint8_t  Shift                    = BitsInCurrentByte - Bits2Append2CurrentByte;
                uint8_t  ExtractBitMask           = BitMaskTable[Bits2Append2CurrentByte - 1] << Shift;
                uint8_t  ExtractedBits            = Data2Append & ExtractBitMask;
                uint8_t  ApplyBits                = (uint8_t) (ExtractedBits << Shift);
                uint64_t Byte                     = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                BitB->Buffer[Byte]               |= ApplyBits;
                
                Bits2Write                       -= Bits2Append2CurrentByte;
                BitB->BitOffset                  += Bits2Append2CurrentByte;
            }
        }
    }
    
    static void BitBuffer_Append_LSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t  Bits2Write                    = NumBits2Append;
            uint64_t Data2Write                    = Data2Append;
            while (Bits2Write > 0) {
                uint8_t  BitsInCurrentByte         = 8 - (BitB->BitOffset % 8);
                uint8_t  Bits2Append2CurrentByte   = (uint8_t) Minimum(BitsInCurrentByte, Bits2Write);
                uint8_t  Shift                     = BitsInCurrentByte - Bits2Append2CurrentByte;
                uint64_t ExtractBitMask            = BitMaskTable[Bits2Append2CurrentByte - 1] << Shift;
                uint8_t  ExtractedBits             = (Data2Write & ExtractBitMask) >> (Bits2Write - Bits2Append2CurrentByte);
                if (BitsInCurrentByte > Bits2Append2CurrentByte) {
                    ExtractedBits                <<= (BitsInCurrentByte - Bits2Append2CurrentByte) - 1;
                }
                uint64_t Byte                      = Bits2Bytes(BitB->BitOffset, RoundingType_Down);
                BitB->Buffer[Byte]                |= ExtractedBits;
                
                Bits2Write                        -= Bits2Append2CurrentByte;
                BitB->BitOffset                   += Bits2Append2CurrentByte;
            }
        }
    }
    
    static void BitBuffer_Append_MSByteLSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t  Bits2Write                    = NumBits2Append;
            uint64_t Data2Write                    = Data2Append;
            while (Bits2Write > 0) {
                uint8_t  BitsInCurrentByte         = 8 - (BitB->BitOffset % 8);
                uint8_t  Bits2Append2CurrentByte   = (uint8_t) Minimum(BitsInCurrentByte, Bits2Write);
                uint8_t  Shift                     = BitsInCurrentByte - Bits2Append2CurrentByte;
                uint64_t ExtractBitMask            = BitMaskTable[Bits2Append2CurrentByte - 1] << Shift;
                uint8_t  ExtractedBits             = Data2Write & ExtractBitMask;
                if (BitsInCurrentByte > Bits2Append2CurrentByte) {
                    ExtractedBits                <<= (BitsInCurrentByte - Bits2Append2CurrentByte) - 1;
                }
                BitB->Buffer[BitB->BitOffset / 8] |= ExtractedBits;
                
                Data2Write                       >>= Bits2Append2CurrentByte;
                Bits2Write                        -= Bits2Append2CurrentByte;
                BitB->BitOffset                   += Bits2Append2CurrentByte;
            }
        }
    }
    
    static void BitBuffer_Append_MSByteMSBit(BitBuffer *BitB, uint8_t NumBits2Append, uint64_t Data2Append) {
        if (NumBits2Append > 0) {
            uint8_t  Bits2Write                    = NumBits2Append;
            while (Bits2Write > 0) {
                uint8_t  BitsByteCanContain        = 8 - (BitB->BitOffset % 8);
                uint8_t  Bits2Append2CurrentByte   = (uint8_t) Minimum(BitsByteCanContain, Bits2Write);
                uint8_t  ExtractionShift           = Bits2Write - Bits2Append2CurrentByte;
                uint64_t ExtractBitMask            = BitMaskTable[Bits2Append2CurrentByte - 1] << ExtractionShift;
                uint64_t ExtractedBits             = (Data2Append & ExtractBitMask) >> ExtractionShift;
                if (BitsByteCanContain > Bits2Append2CurrentByte) {
                    ExtractedBits                <<= (BitsByteCanContain - Bits2Append2CurrentByte) - 1;
                }
                BitB->Buffer[BitB->BitOffset / 8] |= ExtractedBits;
                
                Bits2Write                        -= Bits2Append2CurrentByte;
                BitB->BitOffset                   += Bits2Append2CurrentByte;
                
                /*
                 Bits2Apend = 12
                 BitOffset  = 7
                 
                 BitsByteCanContain      = 1
                 Bits2Append2CurrentByte = 1
                 ExtractionShift         = 11
                 ExtractBitMask          = 0x80 << 11 = 0x40000, should be 0x400
                 
                 ExtractBitMask2         = (0x80 >> 7) << 11 = 0x800
                 
                 There's a few ways we can handle this tho.
                 
                 We can do this math and shifting, or we can just use the LSBit table to begin with
                 
                 So this is the cause of the bugs?
                 
                 MaskShift needs to be what? well in this case it's 1 bit aka 0x80 >> 7 aka 8 - Bits2Append2CurrentByte
                 ----
                 
                 Using LSBitMaskTable:
                 
                 0x01 << 11 = 0x800
                 */
                
                
                
                
                /*
                 
                 There's 2 levels of writing the bits:
                 
                 The first level is making sure that we shift the mask to 0 BEFORE shifting to it's actual position, that way it aligns with the bits correctly.
                 
                 
                 */
                
                // 0xFBBEA -> 0x1BEA
                
                /* Data2Write can not be shifted, since we're removing the top bits, not the bottom.
                 
                 Wait tho.
                 
                 Bits2Write WAS 21, it's now 13, so we don't actually need to shift a damn thing?
                 */
            }
        }
    }
    
    static UTF32 *Format_BitBuffer(BitBuffer *BitB, uint8_t Length) {
        UTF32 *BitBufferString = NULL;
        if (BitB != NULL && Length >= 1 && Length <= 64) {
            BitBuffer_Seek(BitB, BitB->BitOffset - Length);
            BitBufferString  = UTF32_Format(UTF32String("BitBuffer: %P, NumBits: %llu, BitOffset: %llu, Buffer: %llX"), BitB, BitB->NumBits, BitB->BitOffset, BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, Length));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Length == 0) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Length is zero, less than the minimum of 1"));
        } else if (Length > 64) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Length: %u is greater than 64 bits, the maximum"), Length);
        }
        return BitBufferString;
    }
    
    uint64_t BitBuffer_PeekBits(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Peek) {
        uint64_t OutputData  = 0ULL;
        if (BitB != NULL && (Bits2Peek >= 1 && Bits2Peek <= 64) && (Bits2Peek <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == BitIO_ByteOrder_LSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData          = BitBuffer_Extract_LSByteLSBit(BitB, Bits2Peek);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData          = BitBuffer_Extract_LSByteMSBit(BitB, Bits2Peek);
                }
            } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData         = BitBuffer_Extract_MSByteLSBit(BitB, Bits2Peek);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData         = BitBuffer_Extract_MSByteMSBit(BitB, Bits2Peek);
                }
            }
            BitB->BitOffset -= Bits2Peek;
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if ((Bits2Peek == 0 || Bits2Peek > 64) || (Bits2Peek > (BitB->BitOffset - BitB->NumBits))) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Bits2Peek %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_PeekBits can satisfy 1-64"), Bits2Peek, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint8_t BitBuffer_ReadBits8(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read) {
        uint8_t OutputData    = 0U;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == BitIO_ByteOrder_LSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData          = (uint8_t) BitBuffer_Extract_LSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData          = (uint8_t) BitBuffer_Extract_LSByteMSBit(BitB, Bits2Read);
                }
            } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData         = (uint8_t) BitBuffer_Extract_MSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData         = (uint8_t) BitBuffer_Extract_MSByteMSBit(BitB, Bits2Read);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if ((Bits2Read == 0 || Bits2Read > 8) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Bits2Read %d is greater than BitBuffer can provide %lld, or greater than can be satisfied 1-8"), Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint16_t BitBuffer_ReadBits16(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read) {
        uint16_t OutputData    = 0U;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == BitIO_ByteOrder_LSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData          = (uint16_t) BitBuffer_Extract_LSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData          = (uint16_t) BitBuffer_Extract_LSByteMSBit(BitB, Bits2Read);
                }
            } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData         = (uint16_t) BitBuffer_Extract_MSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData         = (uint16_t) BitBuffer_Extract_MSByteMSBit(BitB, Bits2Read);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if ((Bits2Read == 0 || Bits2Read > 16) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Bits2Read %d is greater than BitBuffer can provide %lld, or greater than can be satisfied 1-16"), Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint32_t BitBuffer_ReadBits32(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read) {
        uint32_t OutputData    = 0UL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == BitIO_ByteOrder_LSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData          = (uint32_t) BitBuffer_Extract_LSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData          = (uint32_t) BitBuffer_Extract_LSByteMSBit(BitB, Bits2Read);
                }
            } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData         = (uint32_t) BitBuffer_Extract_MSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData         = (uint32_t) BitBuffer_Extract_MSByteMSBit(BitB, Bits2Read);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if ((Bits2Read == 0 || Bits2Read > 32) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Bits2Read %d is greater than BitBuffer can provide %lld, or greater than can be satisfied 1-32"), Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t BitBuffer_ReadBits64(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t Bits2Read) {
        uint64_t OutputData    = 0UL;
        if (BitB != NULL && (Bits2Read >= 1 && Bits2Read <= 64) && (Bits2Read <= (BitB->BitOffset - BitB->NumBits))) {
            if (ByteOrder == BitIO_ByteOrder_LSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData          = (uint32_t) BitBuffer_Extract_LSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData          = (uint32_t) BitBuffer_Extract_LSByteMSBit(BitB, Bits2Read);
                }
            } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    OutputData         = (uint32_t) BitBuffer_Extract_MSByteLSBit(BitB, Bits2Read);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    OutputData         = (uint32_t) BitBuffer_Extract_MSByteMSBit(BitB, Bits2Read);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if ((Bits2Read == 0 || Bits2Read > 64) || (Bits2Read > (BitB->BitOffset - BitB->NumBits))) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Bits2Read %d is greater than BitBuffer can provide %lld, or greater than can be satisfied 1-64"), Bits2Read, BitB->BitOffset);
        }
        return OutputData;
    }
    
    uint64_t BitBuffer_ReadUnary(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, BitIO_UnaryTypes UnaryType, BitIO_UnaryTerminatorTypes StopBit) {
        uint64_t OutputData    = 0ULL;
        if (BitB != NULL) {
            uint8_t CurrentBit = StopBit ^ 1;
            do {
                if (ByteOrder == BitIO_ByteOrder_LSByte) {
                    if (BitOrder == BitIO_BitOrder_LSBit) {
                        CurrentBit          = (uint8_t) BitBuffer_Extract_LSByteLSBit(BitB, 1);
                    } else if (BitOrder == BitIO_BitOrder_MSBit) {
                        CurrentBit          = (uint8_t) BitBuffer_Extract_LSByteMSBit(BitB, 1);
                    }
                } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                    if (BitOrder == BitIO_BitOrder_LSBit) {
                        CurrentBit         = (uint8_t) BitBuffer_Extract_MSByteLSBit(BitB, 1);
                    } else if (BitOrder == BitIO_BitOrder_MSBit) {
                        CurrentBit         = (uint8_t) BitBuffer_Extract_MSByteMSBit(BitB, 1);
                    }
                }
                OutputData    += 1;
            } while (CurrentBit != StopBit);
            BitBuffer_Seek(BitB, 1);
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return (UnaryType == UnaryType_Count ? OutputData + 1 : OutputData);
    }
    
    uint64_t BitBuffer_GetUTF8StringSize(BitBuffer *BitB) {
        uint64_t StringSize           = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset   = BitBuffer_GetPosition(BitB);
            UTF8     CodeUnit8        = 1;
            while (CodeUnit8 != PlatformIO_NULLTerminator) {
                CodeUnit8             = (UTF8) BitBuffer_Extract_LSByteLSBit(BitB, 8);
                uint8_t  CodeUnitSize = UTF8_GetCodePointSizeInCodeUnits(CodeUnit8);
                StringSize           += CodeUnitSize;
            }
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF8 *BitBuffer_ReadUTF8(BitBuffer *BitB, uint64_t StringSize) {
        UTF8 *ExtractedString             = UTF8_Init(StringSize);
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                ExtractedString[CodeUnit] = (UTF8) BitBuffer_Extract_LSByteLSBit(BitB, 8);
            }
        }
        return ExtractedString;
    }
    
    uint64_t BitBuffer_GetUTF16StringSize(BitBuffer *BitB) {
        uint64_t StringSize           = 0ULL;
        if (BitB != NULL) {
            int64_t  OriginalOffset   = BitBuffer_GetPosition(BitB);
            UTF16    CodeUnit16       = 1;
            while (CodeUnit16 != PlatformIO_NULLTerminator) {
                CodeUnit16            = (UTF16) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                uint8_t  CodeUnitSize = UTF16_GetCodePointSizeInCodeUnits(CodeUnit16);
                StringSize           += CodeUnitSize;
            }
            BitBuffer_SetPosition(BitB, OriginalOffset);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return StringSize;
    }
    
    UTF16 *BitBuffer_ReadUTF16(BitBuffer *BitB, uint64_t StringSize) {
        UTF16 *ExtractedString            = UTF16_Init(StringSize);
        if (BitB != NULL && ExtractedString != NULL) {
            for (uint64_t CodeUnit = 0ULL; CodeUnit < StringSize; CodeUnit++) {
                ExtractedString[CodeUnit] = (UTF16) BitBuffer_Extract_LSByteLSBit(BitB, 16);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return ExtractedString;
    }
    
    uint8_t *BitBuffer_ReadGUUID(BitBuffer *BitB, BitIO_GUUIDTypes GUUID2Read) {
        uint8_t *GUUID = NULL;
        if (GUUID2Read != GUUIDType_Unknown && BitB != NULL && (BitBuffer_GetSize(BitB) - BitBuffer_GetPosition(BitB)) >= BinaryGUUID_Size) {
            if (GUUID2Read == GUUIDType_BinaryUUID || GUUID2Read == GUUIDType_BinaryGUID) {
                GUUID                    = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
                if (GUUID != NULL) {
                    for (uint8_t Byte = 0; Byte < BinaryGUUID_Size; Byte++) {
                        GUUID[Byte]      = (uint8_t) BitBuffer_Extract_LSByteLSBit(BitB, 8);
                    }
                } else {
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate GUUIDType_GUIDString"));
                }
            } else if (GUUID2Read == GUUIDType_UUIDString || GUUID2Read == GUUIDType_GUIDString) {
                if (GUUID != NULL) {
                    uint32_t Section1    = (uint32_t) BitBuffer_Extract_LSByteLSBit(BitB, 32);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section2    = (uint16_t) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section3    = (uint16_t) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint16_t Section4    = (uint16_t) BitBuffer_Extract_LSByteLSBit(BitB, 16);
                    BitBuffer_Seek(BitB, 8);
                    uint64_t Section5    = (uint64_t) BitBuffer_Extract_LSByteLSBit(BitB, 48);
                    for (uint8_t Byte = 0; Byte < GUUIDString_Size; Byte++) {
                        if (Byte <= 3) {
                            GUUID[Byte]  = Section1 & (0xFF << (Byte * 8));
                        } else if (Byte >= 5 && Byte <= 7) {
                            GUUID[Byte]  = Section2 & ((0xFF << (Byte / 4) * 8));
                        } else if (Byte >= 9 && Byte <= 11) {
                            GUUID[Byte]  = Section3 & ((0xFF << (Byte / 8) * 8));
                        } else if (Byte >= 13 && Byte <= 15) {
                            GUUID[Byte]  = Section4 & ((0xFF << (Byte / 12) * 8));
                        } else if (Byte >= 17 && Byte <= 21) {
                            GUUID[Byte]  = Section5 & ((0xFF << (Byte / 16) * 8));
                        } else {
                            GUUID[Byte]  = '-';
                        }
                    }
                } else {
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate GUUIDType_UUIDString"));
                }
            }
        }
        return GUUID;
    }
    
    void BitBuffer_WriteBits(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, uint8_t NumBits2Write, uint64_t Bits2Write) {
        if (BitB != NULL) {
            if (ByteOrder == BitIO_ByteOrder_LSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    BitBuffer_Append_LSByteLSBit(BitB, NumBits2Write, Bits2Write);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    BitBuffer_Append_LSByteMSBit(BitB, NumBits2Write, Bits2Write);
                }
            } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    BitBuffer_Append_MSByteLSBit(BitB, NumBits2Write, Bits2Write);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    BitBuffer_Append_MSByteMSBit(BitB, NumBits2Write, Bits2Write);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (NumBits2Write <= 0 || NumBits2Write > 64) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("NumBits2Write %d is greater than BitBuffer can provide %lld, or greater than BitBuffer_WriteBits can satisfy 1-64"), NumBits2Write, BitB->NumBits);
        }
    }
    
    void BitBuffer_WriteUnary(BitBuffer *BitB, BitIO_ByteOrders ByteOrder, BitIO_BitOrders BitOrder, BitIO_UnaryTypes UnaryType, BitIO_UnaryTerminatorTypes StopBit, uint8_t UnaryBits2Write) {
        if (BitB != NULL) {
            uint8_t EndBit      = StopBit == UnaryTerminator_Zero ? 0 : 1;
            uint8_t Field2Write = UnaryBits2Write;
            if (UnaryType == UnaryType_Count) {
                Field2Write -= 1;
            }
            
            if (ByteOrder == BitIO_ByteOrder_LSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    BitBuffer_Append_LSByteLSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_LSByteLSBit(BitB, 1, EndBit);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    BitBuffer_Append_LSByteMSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_LSByteMSBit(BitB, 1, EndBit);
                }
            } else if (ByteOrder == BitIO_ByteOrder_MSByte) {
                if (BitOrder == BitIO_BitOrder_LSBit) {
                    BitBuffer_Append_MSByteLSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_MSByteLSBit(BitB, 1, EndBit);
                } else if (BitOrder == BitIO_BitOrder_MSBit) {
                    BitBuffer_Append_MSByteMSBit(BitB, (uint8_t) Logarithm(2, Field2Write), StopBit ^ 1);
                    BitBuffer_Append_MSByteMSBit(BitB, 1, EndBit);
                }
            }
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteUTF8(BitBuffer *BitB, UTF8 *String2Write, BitIO_StringTerminatorTypes WriteType) {
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize = UTF8_GetStringSizeInCodeUnits(String2Write);
            if (WriteType == StringTerminator_NULL) {
                StringSize     += UTF8CodeUnitSizeInBits;
            }

            int64_t  BitsAvailable = BitBuffer_GetBitsFree(BitB);
            uint64_t CodeUnit      = 0ULL;
            if (BitsAvailable >= Bytes2Bits(StringSize)) {
                while (String2Write[CodeUnit] != PlatformIO_NULLTerminator) {
                    BitBuffer_Append_MSByteLSBit(BitB, UTF8CodeUnitSizeInBits, String2Write[CodeUnit]);
                    CodeUnit         += 1;
                }
                if (WriteType == StringTerminator_NULL) {
                    BitBuffer_Append_MSByteLSBit(BitB, UTF8CodeUnitSizeInBits, 0); // NULL Terminator
                }
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteUTF16(BitBuffer *BitB, UTF16 *String2Write, BitIO_StringTerminatorTypes WriteType) {
        if (BitB != NULL && String2Write != NULL) {
            uint64_t StringSize = UTF16_GetStringSizeInCodeUnits(String2Write);
            if (WriteType == StringTerminator_NULL) {
                StringSize += UTF16CodeUnitSizeInBits / 8; // Size in bytes, not bits
            }

            int64_t  BitsAvailable = BitBuffer_GetBitsFree(BitB);
            uint64_t CodeUnit = 0ULL;
            if (BitsAvailable >= Bytes2Bits(StringSize)) {
                while (String2Write[CodeUnit] != PlatformIO_NULLTerminator) {
                    BitBuffer_Append_LSByteLSBit(BitB, UTF16CodeUnitSizeInBits, String2Write[CodeUnit]);
                    CodeUnit += 1;
                }
                if (WriteType == StringTerminator_NULL) {
                    BitBuffer_Append_LSByteLSBit(BitB, UTF16CodeUnitSizeInBits, 0); // NULL Terminator
                }
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("StringSize: %lld bits is bigger than the buffer can contain: %lld"), Bytes2Bits(StringSize), BitsAvailable);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (String2Write == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitBuffer_WriteGUUID(BitBuffer *BitB, BitIO_GUUIDTypes GUUIDType, uint8_t *GUUID2Write) {
        if (BitB != NULL && GUUID2Write != NULL) {
            static const uint8_t GUUIDSizeInBits[4] = {168, 168, 128, 128};
            if (BitB->BitOffset + GUUIDSizeInBits[GUUIDType] <= BitB->NumBits) {
                uint8_t GUUIDSize = ((GUUIDType == GUUIDType_GUIDString || GUUIDType == GUUIDType_UUIDString) ? GUUIDString_Size : BinaryGUUID_Size);
                for (uint8_t Byte = 0; Byte < GUUIDSize; Byte++) {
                    BitBuffer_Append_LSByteLSBit(BitB, 8, GUUID2Write[Byte]);
                }
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (GUUID2Write == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUID2Write Pointer is NULL"));
        }
    }
    
    void BitBuffer_Write(BitBuffer *BitB, BitOutput *BitO) {
        if (BitB != NULL && BitO != NULL) {
            uint64_t Bytes2Write  = Bits2Bytes(BitBuffer_GetPosition(BitB), RoundingType_Down);
            uint64_t Bits2Keep    = BitB->BitOffset % 8;
            uint64_t BytesWritten = 0ULL;
            if (BitO->FileType == FileType_File) {
                BytesWritten = PlatformIO_File_Write(BitB->Buffer, 1, Bytes2Write, BitO->File);
            } else if (BitO->FileType == FileType_Socket) {
                BytesWritten = PlatformIO_Socket_Write(BitO->Socket, BitB->Buffer, Bytes2Write);
            }
            if (BytesWritten == Bytes2Write) {
                BitB->Buffer[0] = 0;
                BitB->Buffer[0] = BitB->Buffer[Bytes2Write + 1] & (Exponentiate(2, Bits2Keep) << (8 - Bits2Keep));
                BitB->BitOffset = Bits2Keep + 1;
                for (uint64_t Byte = (uint64_t) Bits2Bytes(BitB->BitOffset, RoundingType_Up); Byte < (uint64_t) Bits2Bytes(BitB->NumBits, RoundingType_Down); Byte++) {
                    BitB->Buffer[Byte] = 0;
                }
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Wrote %lld of %lld bits"), BytesWritten * 8, Bytes2Write * 8);
            }
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (BitO == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitOutput Pointer is NULL"));
        }
    }
    
    void BitBuffer_Deinit(BitBuffer *BitB) {
        if (BitB != NULL) {
            free(BitB->Buffer);
            free(BitB);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    /* BitBuffer Resource Management */
    /* End BitBuffer section */
    
    /* BitInput */
    BitInput *BitInput_Init(void) {
        BitInput *BitI = (BitInput*) calloc(1, sizeof(BitInput));
        if (BitI == NULL) {
            BitInput_Deinit(BitI);
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate BitInput"));
        }
        return BitI;
    }
    
    void BitInput_UTF8_OpenFile(BitInput *BitI, UTF8 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType                   = FileType_File;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
            bool PathHasBOM                  = UTF8_HasBOM(Path2Open);
            if (PathHasBOM == No) {
                BitI->File                   = PlatformIO_File_Open(Path2Open, UTF8String("rb"));
            } else {
                const UTF8 *const UTF8BOMString = UTF8String("\xEF\xBB\xBF");
                int64_t GoodCodeUnit         = UTF8_FindSubString(Path2Open, UTF8BOMString, 0, 3) + 3;
                if (GoodCodeUnit >= 0) {
                    BitI->File               = PlatformIO_File_Open(&Path2Open[GoodCodeUnit], UTF8String("rb"));
                } else {
                    UTF8 *BOMLess            = UTF8_RemoveBOM(Path2Open);
                    BitI->File               = PlatformIO_File_Open(BOMLess, UTF8String("rb"));
                    free(BOMLess);
                }
            }
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
            bool  StringHasBOM               = UTF8_HasBOM(Path2Open);
            UTF32 *WinPath32                 = UTF8_Decode(Path2Open);
            UTF32 *WinPathLong32             = UTF32_Insert(WinPath32, PlatformIO_UNCPathPrefix32, StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16                 = UTF16_Encode(WinPathLong32);
            free(WinPathLong32);
            BitI->File                       = PlatformIO_File_Open(WinPath16, UTF16String("rb"));
            free(WinPath16);
#endif
            if (BitI->File != NULL) {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't open file \"%s\": Check that the file exists and the permissions are correct"), Path2Open);
            }
        } else if (BitI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitInput_UTF16_OpenFile(BitInput *BitI, UTF16 *Path2Open) {
        if (BitI != NULL && Path2Open != NULL) {
            BitI->FileType                   = FileType_File;
            UTF32 *Path32                    = NULL;
            UTF8  *Path8                     = NULL;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
            Path32                           = UTF16_Decode(Path2Open);
            bool   PathHasBOM                = UTF16_HasBOM(Path2Open);
            bool   PathHasWinPrefix          = UTF16_IsUNCPath(Path2Open);
            
            if (PathHasBOM == Yes && PathHasWinPrefix == Yes) {
                UTF32 *BOMLess               = UTF32_RemoveBOM(Path32);
                free(Path32);
                Path32                       = BOMLess;
            } else if (PathHasBOM == Yes && PathHasWinPrefix == No) {
                UTF32 *BOMLess               = UTF32_RemoveBOM(Path32);
                UTF32 *Prefix                = UTF32_Insert(BOMLess, PlatformIO_UNCPathPrefix32, 0);
                free(Path32);
                Path32                       = Prefix;
            } else if (PathHasBOM == No && PathHasWinPrefix == No) {
                UTF32 *Prefix                = UTF32_Insert(Path32, PlatformIO_UNCPathPrefix32, 0);
                free(Path32);
                Path32                       = Prefix;
            }
            Path8                            = UTF8_Encode(Path32);
            BitI->File                       = PlatformIO_File_Open(Path8, UTF8String("rb"));
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
            bool  StringHasBOM               = UTF16_HasBOM(Path2Open);
            UTF32 *WinPath32                 = UTF16_Decode(Path2Open);
            UTF32 *WinPathLong32             = UTF32_Insert(WinPath32, PlatformIO_UNCPathPrefix32, StringHasBOM == Yes ? UTF8BOMSizeInCodeUnits : 0);
            free(WinPath32);
            UTF16 *WinPath16                 = UTF16_Encode(WinPathLong32);
            free(WinPathLong32);
            BitI->File                       = PlatformIO_File_Open(WinPath16, UTF16String("rb"));
            free(WinPath16);
#endif
            if (BitI->File != NULL) {
                setvbuf(BitI->File, NULL, _IONBF, 0);
            } else {
                Path32                       = UTF16_Decode(Path2Open);
                Path8                        = UTF8_Encode(Path32);
                free(Path32);
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't open file \"%s\": Check that the file exists and the permissions are correct"), Path8);
                free(Path8);
            }
        } else if (BitI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("String Pointer is NULL"));
        }
    }
    
    void BitInput_OpenSocket(BitInput *BitI, int Domain, int Type, int Protocol) {
        if (BitI != NULL) {
            BitI->FileType = FileType_Socket;
            BitI->Socket   = PlatformIO_Socket_Create(Domain, Type, Protocol);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
    }
    
    void BitInput_ConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, uint32_t SocketSize) {
        if (BitI != NULL && SocketAddress != NULL) {
            BitI->FileType = FileType_Socket;
            PlatformIO_Socket_Connect(BitI->Socket, SocketAddress, SocketSize);
        } else if (BitI == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("SocketAddress Pointer is NULL"));
        }
    }
    
    static void BitInput_FindFileSize(BitInput *BitI) {
        if (BitI != NULL) {
            PlatformIO_File_Seek(BitI->File, 0, SEEK_END);
            BitI->FileSize     = (uint64_t) PlatformIO_File_GetSize(BitI->File);
            PlatformIO_File_Seek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition = (uint64_t) PlatformIO_File_GetSize(BitI->File);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
    }
    
    uint64_t BitInput_GetFileSize(BitInput *BitI) {
        uint64_t InputSize = 0ULL;
        if (BitI != NULL) {
            if (BitI->FileSize == 0) {
                BitInput_FindFileSize(BitI);
            }
            InputSize     = BitI->FileSize;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
        return InputSize;
    }
    
    uint64_t BitInput_GetFilePosition(BitInput *BitI) {
        uint64_t Position = 0ULL;
        if (BitI != NULL) {
            if (BitI->FilePosition == 0) {
                BitInput_FindFileSize(BitI);
            }
            Position    = BitI->FilePosition;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
        return Position;
    }
    
    uint64_t BitInput_BytesRemaining(BitInput *BitI) {
        uint64_t BytesLeft = 0ULL;
        if (BitI != NULL) {
            if (BitI->FilePosition == 0) {
                BitInput_FindFileSize(BitI);
            }
            BytesLeft    = BitI->FileSize - BitI->FilePosition;
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
        return BytesLeft;
    }
    
    void BitInput_Deinit(BitInput *BitI) {
        if (BitI != NULL) {
            if (BitI->FileType == FileType_File) {
                PlatformIO_File_Close(BitI->File);
            } else if (BitI->FileType == FileType_Socket) {
                PlatformIO_Socket_Close(BitI->Socket);
            }
            free(BitI);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
    }
    /* BitInput */
    
    
    /* BitOutput */
    BitOutput *BitOutput_Init(void) {
        BitOutput *BitO = (BitOutput*) calloc(1, sizeof(BitOutput));
        if (BitO == NULL) {
            BitOutput_Deinit(BitO);
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate BitOutput"));
        }
        return BitO;
    }
    
    void BitOutput_UTF8_OpenFile(BitOutput *BitO, UTF8 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType                   = FileType_File;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
            bool PathHasBOM                  = UTF8_HasBOM(Path2Open);
            if (PathHasBOM == No) {
                BitO->File                   = PlatformIO_File_Open(Path2Open, UTF8String("rb"));
            } else {
                UTF8 *BOMLess                = UTF8_RemoveBOM(Path2Open);
                BitO->File                   = PlatformIO_File_Open(BOMLess, UTF8String("rb"));
                free(BOMLess);
            }
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
            bool   PathIsAbsolute       = UTF8_IsAbsolutePath(Path2Open);
            bool   PathHasUNCPrefix     = UTF8_IsUNCPath(Path2Open);
            UTF32 *Path32               = UTF8_Decode(Path2Open);
            UTF16 *Path16               = NULL;
            if (PathIsAbsolute == Yes && PathHasUNCPrefix == No) {
                UTF32 *UNCPrefixed      = UTF32_Insert(Path32, PlatformIO_UNCPathPrefix32, 0);
                Path16                  = UTF16_Encode(UNCPrefixed);
                free(UNCPrefixed);
                PlatformIO_File_Open(Path16, UTF16String("rb"));
            } else if (PathIsAbsolute == No && PathHasUNCPrefix == Yes) {
                UTF32 *Removed          = UTF32_RemoveSubString(Path32, PlatformIO_UNCPathPrefix32, 1);
                Path16                  = UTF16_Encode(Removed);
                free(Removed);
                PlatformIO_File_Open(Path16, UTF16String("rb"));
            } else {
                Path16                  = UTF16_Encode(Path32);
                PlatformIO_File_Open(Path16, UTF16String("rb"));
            }
#endif
        } else if (BitO == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Path2Open Pointer is NULL"));
        }
    }
    
    void BitOutput_UTF16_OpenFile(BitOutput *BitO, UTF16 *Path2Open) {
        if (BitO != NULL && Path2Open != NULL) {
            BitO->FileType              = FileType_File;
            bool  PathHasBOM            = No;
#if   ((PlatformIO_TargetOS & PlatformIO_POSIXOS) == PlatformIO_POSIXOS)
            UTF32 *Decoded              = UTF16_Decode(Path2Open);
            PathHasBOM                  = UTF32_HasBOM(Decoded);
            UTF8 *Fixed                 = NULL;
            if (PathHasBOM == Yes) {
                UTF32 *Fixed32          = UTF32_RemoveBOM(Decoded);
                Fixed                   = UTF8_Encode(Fixed32);
            }
            PlatformIO_File_Open(Fixed, UTF8String("rb"));
#elif (PlatformIO_TargetOS == PlatformIO_WindowsOS)
            bool   StringHasPathPrefix  = UTF16_IsUNCPath(Path2Open);
            
            UTF32 *Path32               = UTF16_Decode(Path2Open);
            UTF16 *Path16               = NULL;
            if (StringHasPathPrefix == No) {
                bool   StringHasBOM     = UTF16_HasBOM(Path2Open);
                UTF32 *PrefixPath       = UTF32_Insert(Path32, PlatformIO_UNCPathPrefix32, StringHasBOM == Yes ? UTF16BOMSizeInCodeUnits : 0);
                Path16                  = UTF16_Encode(PrefixPath);
            } else {
                Path16                  = UTF16_Encode(Path32);
            }
#endif
        } else if (BitO == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitOutput Pointer is NULL"));
        } else if (Path2Open == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Path2Open Pointer is NULL"));
        }
    }
    
    void BitOutput_OpenSocket(BitOutput *BitO, int Domain, int Type, int Protocol) {
        if (BitO != NULL) {
            BitO->FileType  = FileType_Socket;
            BitO->Socket    = PlatformIO_Socket_Create(Domain, Type, Protocol);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitInput Pointer is NULL"));
        }
    }
    
    void BitOutput_ConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, uint64_t SocketSize) {
        if (BitO != NULL && SocketAddress != NULL) {
            BitO->FileType = FileType_Socket;
            PlatformIO_Socket_Connect(BitO->Socket, SocketAddress, SocketSize);
        } else if (BitO == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitOutput Pointer is NULL"));
        } else if (SocketAddress == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("SocketAddress Pointer is NULL"));
        }
    }
    
    void BitOutput_Deinit(BitOutput *BitO) {
        if (BitO != NULL) {
            fflush(BitO->File);
            if (BitO->FileType == FileType_File) {
                PlatformIO_File_Close(BitO->File);
            } else if (BitO->FileType == FileType_Socket) {
                PlatformIO_Socket_Close(BitO->Socket);
            }
            free(BitO);
        } else {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("BitOutput Pointer is NULL"));
        }
    }
    /* BitOutput */
    
    /* GUUID */
    uint8_t *GUUID_Generate(Entropy *Random, BitIO_GUUIDTypes GUUIDType) {
        uint8_t *GUUID                   = 0;
        if (Random != NULL && GUUIDType != GUUIDType_Unknown) {
            uint64_t LowBits             = Entropy_GenerateInteger(Random, 64);
            uint64_t HighBits            = Entropy_GenerateInteger(Random, 64);
            uint8_t *BinaryGUUIDData     = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
            if (GUUID != NULL) {
                for (uint8_t GUUIDByte = 0; GUUIDByte < BinaryGUUID_Size; GUUIDByte++) {
                    if (GUUIDByte < 8) {
                        uint8_t Byte     = (LowBits  & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                        BinaryGUUIDData[GUUIDByte] = Byte;
                    } else {
                        uint8_t Byte     = (HighBits & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                        BinaryGUUIDData[GUUIDByte] = Byte;
                    }
                }
                if (GUUIDType == GUUIDType_GUIDString || GUUIDType == GUUIDType_UUIDString) {
                    GUUID                = GUUID_Convert(GUUIDType_BinaryGUID, GUUIDType_GUIDString, BinaryGUUIDData);
                } else {
                    GUUID                = BinaryGUUIDData;
                }
            } else {
                Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate GUUID"));
            }
        } else if (Random == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Entropy Pointer is NULL"));
        } else if (GUUIDType == GUUIDType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUIDType_Unknown is an invalid GUUIDType"));
        }
        return GUUID;
    }
    
    bool GUUID_Compare(BitIO_GUUIDTypes Type2Compare, uint8_t *GUUID1, uint8_t *GUUID2) {
        uint8_t GUUIDSize       = ((Type2Compare == GUUIDType_GUIDString || Type2Compare == GUUIDType_UUIDString) ? BinaryGUUID_Size : BinaryGUUID_Size);
        bool GUUIDsMatch        = Yes;
        if (GUUID1 != NULL && GUUID2 != NULL && Type2Compare != GUUIDType_Unknown) {
            for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < GUUIDSize; BinaryGUUIDByte++) {
                if (GUUID1[BinaryGUUIDByte] != GUUID2[BinaryGUUIDByte]) {
                    GUUIDsMatch = No;
                }
            }
        } else if (GUUID1 == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUID1 Pointer is NULL"));
        } else if (GUUID2 == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUID2 Pointer is NULL"));
        } else if (Type2Compare == GUUIDType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUIDType_Unknown is an invalid GUUID type"));
        }
        return GUUIDsMatch;
    }
    
    uint8_t *GUUID_Convert(BitIO_GUUIDTypes InputType, BitIO_GUUIDTypes OutputType, uint8_t *GUUID2Convert) {
        uint8_t  Dash = '-';
        uint8_t  OutputGUUIDSize = ((OutputType == GUUIDType_GUIDString || OutputType == GUUIDType_UUIDString) ? GUUIDString_Size : BinaryGUUID_Size);
        uint8_t *ConvertedGUUID  = (uint8_t*) calloc(OutputGUUIDSize, sizeof(uint8_t));
        if (ConvertedGUUID != NULL && GUUID2Convert != NULL && InputType != GUUIDType_Unknown && OutputType != GUUIDType_Unknown) {
            bool ByteOrderDiffers = (((InputType == GUUIDType_GUIDString && OutputType == GUUIDType_UUIDString) || (InputType == GUUIDType_UUIDString && OutputType == GUUIDType_GUIDString) || (InputType == GUUIDType_BinaryUUID && OutputType == GUUIDType_BinaryGUID) || (InputType == GUUIDType_BinaryGUID && OutputType == GUUIDType_BinaryUUID)) ? Yes : No);
            
            bool TypeDiffers      = (((InputType == GUUIDType_GUIDString && OutputType == GUUIDType_BinaryGUID) || (InputType == GUUIDType_BinaryGUID && OutputType == GUUIDType_GUIDString) || (InputType == GUUIDType_UUIDString && OutputType == GUUIDType_BinaryUUID) || (InputType == GUUIDType_BinaryUUID && OutputType == GUUIDType_UUIDString)) ? Yes : No);
            
            if (ByteOrderDiffers == Yes) {
                GUUID_Swap(InputType, GUUID2Convert);
            }
            
            if (TypeDiffers == Yes) {
                if ((InputType == GUUIDType_UUIDString || InputType == GUUIDType_GUIDString) && (OutputType == GUUIDType_BinaryUUID || OutputType == GUUIDType_BinaryGUID)) {
                    for (uint8_t StringByte = 0; StringByte < BinaryGUUID_Size; StringByte++) {
                        for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUID_Size; BinaryByte++) {
                            if (GUUID2Convert[StringByte] != Dash) {
                                ConvertedGUUID[BinaryByte] = GUUID2Convert[StringByte];
                            }
                        }
                    }
                } else if ((InputType == GUUIDType_BinaryUUID || InputType == GUUIDType_BinaryGUID) || (OutputType == GUUIDType_UUIDString || OutputType == GUUIDType_GUIDString)) {
                    for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUID_Size; BinaryByte++) {
                        for (uint8_t StringByte = 0; StringByte < BinaryGUUID_Size; StringByte++) {
                            if (BinaryByte != 4 && BinaryByte != 7 && BinaryByte != 10 && BinaryByte != 13) {
                                ConvertedGUUID[StringByte]  = GUUID2Convert[BinaryByte];
                            } else {
                                ConvertedGUUID[StringByte]  = Dash;
                            }
                        }
                    }
                }
            }
        } else if (ConvertedGUUID == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("Insufficent memory to allocate ConvertedGUUID"));
        } else if (GUUID2Convert == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUID Pointer is NULL"));
        } else if (InputType == GUUIDType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("InputType is invalid"));
        } else if (OutputType == GUUIDType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("OutputType is invalid"));
        }
        return ConvertedGUUID;
    }
    
    uint8_t *GUUID_Swap(BitIO_GUUIDTypes GUUIDType, uint8_t *GUUID2Swap) {
        uint8_t *SwappedGUUID = NULL;
        if (GUUID2Swap != NULL && GUUIDType != GUUIDType_Unknown) {
            uint8_t Dash = '-';
            if (GUUIDType == GUUIDType_UUIDString || GUUIDType == GUUIDType_GUIDString) {
                SwappedGUUID          = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
                if (SwappedGUUID != NULL) {
                    SwappedGUUID[0]   = GUUID2Swap[3];
                    SwappedGUUID[1]   = GUUID2Swap[2];
                    SwappedGUUID[2]   = GUUID2Swap[1];
                    SwappedGUUID[3]   = GUUID2Swap[0];
                    
                    SwappedGUUID[4]   = Dash;
                    
                    SwappedGUUID[5]   = GUUID2Swap[6];
                    SwappedGUUID[6]   = GUUID2Swap[5];
                    
                    SwappedGUUID[7]   = Dash;
                    
                    SwappedGUUID[8]   = GUUID2Swap[9];
                    SwappedGUUID[9]   = GUUID2Swap[8];
                    
                    SwappedGUUID[10]  = Dash;
                    
                    SwappedGUUID[11]  = GUUID2Swap[12];
                    SwappedGUUID[12]  = GUUID2Swap[11];
                    
                    SwappedGUUID[13]  = Dash;
                    
                    for (uint8_t EndBytes = 13; EndBytes < GUUIDString_Size; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("SwappedGUUID Pointer is NULL"));
                }
            } else if (GUUIDType == GUUIDType_BinaryUUID || GUUIDType == GUUIDType_BinaryGUID) {
                SwappedGUUID          = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
                if (SwappedGUUID != NULL) {
                    SwappedGUUID[0]   = GUUID2Swap[3];
                    SwappedGUUID[1]   = GUUID2Swap[2];
                    SwappedGUUID[2]   = GUUID2Swap[1];
                    SwappedGUUID[3]   = GUUID2Swap[0];
                    
                    SwappedGUUID[4]   = GUUID2Swap[5];
                    SwappedGUUID[5]   = GUUID2Swap[4];
                    
                    SwappedGUUID[6]   = GUUID2Swap[7];
                    SwappedGUUID[7]   = GUUID2Swap[6];
                    
                    SwappedGUUID[8]   = GUUID2Swap[9];
                    SwappedGUUID[9]   = GUUID2Swap[8];
                    for (uint8_t EndBytes = 10; EndBytes < BinaryGUUID_Size; EndBytes++) {
                        SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
                    }
                } else {
                    Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("SwappedGUUID Pointer is NULL"));
                }
            }
        } else if (GUUID2Swap == NULL) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUID2Swap Pointer is NULL"));
        } else if (GUUIDType == GUUIDType_Unknown) {
            Log(Severity_DEBUG, FoundationIOFunctionName, UTF8String("GUUIDType_Unknown is an invalid GUUID type"));
        }
        return SwappedGUUID;
    }
    
    void GUUID_Deinit(uint8_t *GUUID) {
        if (GUUID != NULL) {
            free(GUUID);
        }
    }
    /* GUUID */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
