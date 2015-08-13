#include "BitIO.h"

uint64_t FileSequence(int argc, char *argv, int WhichArg) {
    // small function to return the filename sequence specifier, aka take %03d and return leading 0, 3 digit, decimal, etc.
    if (argv) {
    }
    strcmp(&argv[WhichArg -1], "%03d");
    
    return 0;
};

/*!
 *  Check if argv[x] is a file or folder, if a folder work on shit.
 */
uint64_t IdFileOrFolder() {
    
    return 0;
};


/*
void OptionParser(int argc, char *argv, const char *OptString) {
    int GetOptCount = 0, InputArg = 0, OutputArg = 0;
    while ((GetOptCount = getopt(argc, &argv, OptString) != -1)) {
        switch (GetOptCount) {
            case 'i':
                // Input
                InputArg  = GetOptCount;
                break;
            case 'I':
                // Input
                InputArg  = GetOptCount;
                break;
            case 'f':
                // Folder input
                InputArg  = GetOptCount;
                FileSequence(argc, argv, InputArg);
                DIR *InputFolder;
                break;
            case 'F':
                // Folder input
                InputArg  = GetOptCount;
                break;
            case 'o':
                // Output
                OutputArg = GetOptCount;
                break;
            case 'O':
                // Output
                OutputArg = GetOptCount;
                break;
            default:
                break;
        }
        Init_BitIO(&argv[InputArg], "rb", &argv[OutputArg], "wb");
    }
};

/*!
 *  Initalize BitIO for use. this must be done first
 */
int64_t Init_BitIO(char *InputFile, const char *InputMode, char *OutputFile, const char *OutputMode) {
    BitIO.InputFP                              = fopen(InputFile, InputMode);
    BitIO.OutputFP                             = fopen(OutputFile, OutputMode);
    
    if (BitIO.InputFP == NULL) {
        printf("InputFile is NULL\n");
        return -2;
    }
    if (BitIO.OutputFP == NULL) {
        printf("OutputFile couldn't be opened\n");
        return -3;
    }
    fseek(BitIO.InputFP, 0, SEEK_END);
    BitIO.InputFPSize = ftell(BitIO.InputFP);
    rewind(BitIO.InputFP);
    
    uint64_t BytesRead = 0;
    InputBufferManager(64); // TODO set this to the filesize if less than 64
    
    if (BitIO.InputFPSize > BufferSize) {
        BytesRead = fread(BitIO.InputBuffer, 1, BufferSize, BitIO.InputFP);
        BitIO.InputBitOffset = 0;
        if (BytesRead < BufferSize) {
            // Retry reading in the rest of the missed data
            fprintf(stderr, "Init_BitIO read %llu of %d bytes\n", BytesRead, BufferSize);
        }
    } else {
        BytesRead = fread(BitIO.InputBuffer, 1, BitIO.InputFPSize, BitIO.InputFP);
        BitIO.InputBitOffset = 0;
        if (BytesRead < BitIO.InputFPSize) {
            // Retry reading in the rest of the missed data
            fprintf(stderr, "Init_BitIO read %llu of %llu bytes\n", BytesRead, BitIO.InputFPSize);
        }
    }
    
    // Endian Detection
    uint8_t NumberArray[2] = {0xFF,0x00};
    if (NumberArray[1] == 0) {
        BitIO.NativeEndian = Little;
    } else {
        BitIO.NativeEndian = Big;
    }
    
    return 0;
};

/*!
 *  The purpose of this function is to fread in data, and output the requested data to the requesting functions.
 *  This function is INTERNAL to BitIO.c, no externel function should call it.
 */
uint64_t InputBufferManager(uint64_t RequestedBits) {
    uint64_t OutputBits = 0, RightShift = 0, BytesRead = 0, ErrorCode = 0;
    
    if (BitIO.InputFPSize - ftell(BitIO.InputFP) > BufferSize) { // buffer will be full.
        BitIO.BufferIsFull = true;
    }
    
    if (RequestedBits > 0 || RequestedBits <= 64) { // Basic sanity check, maybe this should be removed and just handle the buffer for Get* functions?
        if ((BufferSizeInBits - BitIO.InputBitOffset) < RequestedBits) { // There are NOT enough bits to fulfill the request.
            if (BitIO.InputFPSize - ftell(BitIO.InputFP) >= Bits2Bytes(BufferSizeInBits - BitIO.InputBitOffset)) { // Make sure there are enough bits in the file
                memset(BitIO.InputBuffer, 0, Bits2Bytes(BitIO.InputBitOffset)); // FIXME: the last array element is still polluted tho
                for (uint64_t i = Bits2Bytes(BitIO.InputBitOffset); BufferSize; i++) {
                    BytesRead = fread(BitIO.InputBuffer, 1, 1, BitIO.InputFP);
                }
                if ((BytesRead * 8) < RequestedBits) { // fread failed somewhere.
                    ErrorCode = ferror(BitIO.InputFP);
                    if (ErrorCode != 0) {
                        fprintf(stderr, "Error %llu reading from InputFile\n", ErrorCode);
                    }
                    fprintf(stderr, "Couldn't read all the bits requested, read %llu of %llu\n", BytesRead, Bits2Bytes(RequestedBits));
                }
            } else { // Not enough bits left in InputFP, read until EOF
                // memset unused bits at the top to zero, set the unused value to the top of the usable thing.
                memset(BitIO.InputBuffer, 0, Bits2Bytes(BitIO.InputBitOffset)); // FixME: only set unused portion of buffer.
            }
        } else { // There ARE enough bits to fulfill the request
            
        }
    } else {
        fprintf(stderr, "Requested less than 0 or more than 64 bits.\n");
    }
    
    return OutputBits;
};

/*!
 *  Reads bits into buffer, and breaks them into just the requested bits
 *  Reading backwards isn't possible so don't even try.
 *  If you need to skip backwards use fseek then flush BitIO
 */
uint64_t ReadBits(uint64_t Bits2Read) {
    uint64_t BitsRead = 0, SelectedBits = 0;
    bool BufferIsFull = 0;
    
    if (Bits2Read <= 0 || Bits2Read > 64) {
        sprintf(stderr, "ReadBits failed. %llu bits were requested", Bits2Read);
    } else {
        if ((BufferSizeInBits - BitIO.InputBitOffset) < Bits2Read) {
            
        }
    }
    
    
    
    
    /*
    if (Bits2Read == 0 || Bits2Read > 64) {
        fprintf(stderr, "%llu bits requested, You have to read at least 1, and the max is 64 bits.\n", Bits2Read);
        return -1;
    } else if (Bits2Read < BitIO.InputBitOffset) { // BitIO.InputBitIndex
        int64_t BitsUpdated = UpdateInputBuffer(Bits2Read);
        if (BitsUpdated != (Bits2Read - BitIO.InputBitOffset)) { // BitIO.InputBitIndex
            printf("UpdateInputBuffer couldn't read all the bits requested: %lld\n", BitsUpdated);
        }
    } else {
        BitsRead = BufferManagment(Bits2Read); // if negative, an error has occured
    }
    */
    return BitsRead;
};

/*!
 *  Read in 512 bits and scan for the specified stop bit, and return how many bits were counted before the stop bit was reached
 */
uint64_t ReadRICE(int Bits2RICE, bool StopBit) {
    uint64_t ArrayOffset = 0, BitOffset = 0, Output = 0;
    
    ArrayOffset = BitIO.InputBitOffset / 8; // MUST round down.
    BitOffset   = BitIO.InputBitOffset - (ArrayOffset * 8); // difference between ArrayOffset and InputBitOffset
    
    
    // for loop that reads all the bits available, and when done updates the buffer.
    for (uint64_t i = 0; i < (BufferSizeInBits - BitIO.InputBitOffset); i++) {
        if (((BitIO.InputBuffer[ArrayOffset] << BitOffset) & 0x7F) == StopBit)  { // Found the stop bit. report num bits.
            Output = i;
        }
        BitIO.InputBitOffset += i;
    }
    
    BufferSizeInBits + (BitIO.InputFPSize * 8);
    
    /*
    uint64_t Bits = ReadBits(Bits2RICE);
    for (int i = 0; i < Bits2RICE; i++) {
        if (((Bits & 0x8000000000000000)) == StopBit) {
            Bits << 1;
            return x = i;
        }
    }
    return x;
     */
    return Output;
};

/*!
 *  Function to Skip X number of bits
 */
void SkipBits(uint64_t Bits2Skip) {
    uint64_t Trash = ReadBits(Bits2Skip); /// TODO: This is horrible, fix this... like, today.
    UpdateBitsRead(Bits2Skip);
};

/*!
 *  Write bits to buffer, and when full write OutputBuffer to disk
 */
/*
uint64_t WriteBits(uint64_t Bits2Write) {
    // convert Bits2Write to array
    for (int i = 0; i < 8; i++) {
        BitIO.OutputBuffer[i] = (Bits2Write & 0x00000000000000FF);
    }
    
    uint64_t Times2Loop = Bits2Bytes(CountBits(Bits2Write));
    uint64_t ArrayOffset = Bits2Bytes(CountBits(BitIO.BitsUnwritten)); // BitIO.BitsUnwritten
    
    for (uint64_t i = ArrayOffset; i < Times2Loop; i++) {
        BitIO.OutputBuffer[i] = (Bits2Write & 0x00FF);
        Bits2Write >>= 8;
    }
    fwrite(BitIO.OutputBuffer, 1, 4096, BitIO.OutputFP);
    
    
    /*
     for (int i = 0; i < Bits2Bytes(CountBits(Bits2Write)); i++) {
     uint64_t SelectedBits = Bits2Write << 8;
     BitIO.OutputBuffer[i] = SelectedBits;
     }
     
     if (BitIO.BitsUnwritten == BufferSizeInBits) { // if Outputbuffer is full, write data to file, and reset UnwrittenBits
     fwrite(BitIO.OutputBuffer, 1, 4096, BitIO.OutputFP);
     BitIO.BitsUnwritten -= BufferSizeInBits;
     }
     */
/*
    return 0;
};
*/

/*!
 *  Returns pointer to UUID struct containing UUID
 */
uintptr_t ReadUUID() {
    uint64_t BitsActuallyRead = 0;
    
    UUID.TimeLow                = ReadBits(32);
    UUID.TimeMid                = ReadBits(16);
    UUID.TimeHigh_Version       = ReadBits(16);
    UUID.ClockHigh_Version      = ReadBits(8);
    UUID.ClockLow               = ReadBits(8);
    BitsActuallyRead = fread(UUID.Node, 1, 8, BitIO.InputFP);
    UpdateBitsRead(BitsActuallyRead);
    
    return &UUID;
};

/*!
 *  Internel function to verify if X is a multiple of 8.
 */
bool MultipleOf8(uint64_t IsMultiple) {
    bool Multiple = 0;
    
    if (IsMultiple == 0) {
        Multiple = false;
    } else if ((IsMultiple % 8) == 0) {
        Multiple = true;
    } else {
        Multiple = false;
    }
    
    return Multiple;
};

/*!
 *  convert bits to bytes, round up if not divisible by 8
 */
uint64_t Bits2Bytes(uint64_t Bits) {
    uint64_t x = Bits / 8;
    if (MultipleOf8(Bits) == false) {
        x++;
    }
    return x;
}

/*!
 *  Internal: Divide Bytes by 8 to get number of bits
 */
uint64_t Bytes2Bits(uint64_t Bytes) {
    uint64_t x = Bytes * 8;
    return x;
}

/*!
 *  Internal: for inverting the sign of X, for use in Byte Rounding calculations
 */
int64_t InvertSign(int64_t Number2Invert) {
    int64_t InvertedNumber = Number2Invert * -1;
    return InvertedNumber;
}

/*!
 *  Internal to count the number of bits in X
 */
uint64_t CountBits(uint64_t Bits2Count) {
    uint64_t NumberOfBits = 0;
    while (Bits2Count) {
        Bits2Count &= (Bits2Count -1);
        NumberOfBits++;
    }
    return NumberOfBits;
};

/*!
 *  Internal: Remove leading bits
 */
int64_t PruneBits(int64_t Data2Prune) {
    uint64_t NumberOfBits = CountBits(Data2Prune);
    uint64_t PrunedData = 0;
    PrunedData = Data2Prune << (64 - NumberOfBits);
    return PrunedData;
};

/*
void UpdateBitsRead(uint64_t BitsRead) {
    BitIO.InputBitsLeft += BitsRead;
    BitIO.InputBitIndex -= BitsRead;
};
 */

/*!
 *  Deallocate BitIO
 */
void Deallocate_BitIO(void) {
    free(&BitIO);
    fclose(BitIO.InputFP);
    fclose(BitIO.OutputFP);
    closedir(BitIO.InputDir);
    closedir(BitIO.OutputDir);
};

/*!
 *  Internal: Converts a ones compliment value to twos compliment
 *  Keep in mind that the internal representation in BitIO is twos compliment.
 */
uint64_t OnesCompliment2TwosCompliment() {
    
    return 0;
}

/*!
 *  Internal: Converts an Signed int to a Unsigned int.
 *  Keep in mind that the internal representation in BitIO is unsigned.
 */
uint64_t Signed2Unsigned(int64_t Signed) {
    
    return 0;
};

/*!
 *  Internal: Converts an Unsigned int to a Signed int.
 *  Keep in mind that the internal representation in BitIO is unsigned.
 */
int64_t Unsigned2Signed(uint64_t Unsigned) {
  
    return 0;
};

/*!
 *  Swap endian of 2 byte integers
 */
uint16_t EndianSwap16(uint16_t X) { // Officially works.
    return ((X & 0xff00) >> 8) | ((X & 0x00ff) << 8);
}

/*!
 *  Swap endian of 4 byte integers
 */
uint32_t EndianSwap32(uint32_t X) { // Officially works
    return ((X & 0xff000000) >> 24) | ((X & 0x00ff0000) >> 8) | ((X & 0x0000ff00) << 8) | ((X & 0x000000ff) << 24);
}

/*!
 *  Swap endian of 8 byte integers
 */
uint64_t EndianSwap64(uint64_t X) { // Offically works
    return
    (((X & 0xff00000000000000) >> 56)|((X & 0x00ff000000000000) >> 40)|((X & 0x0000ff0000000000) >> 24)|((X & 0x000000ff00000000) >> 8)
    |((X & 0x00000000ff000000) << 8)|((X & 0x0000000000ff0000) << 24)|((X & 0x000000000000ff00) << 40)|((X & 0x00000000000000ff) << 56));
}
