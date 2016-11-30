#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	uint64_t BitIOCurrentArgument      = 1;
	uint64_t BitInputCurrentSpecifier  = 0;
	uint64_t BitOutputCurrentSpecifier = 0;

	uint16_t SwapEndian16(uint16_t Data2Swap) { // In UnitTest
		return ((Data2Swap & 0xFF00) >> 8) | ((Data2Swap & 0x00FF) << 8);
	}

	uint32_t SwapEndian32(uint32_t Data2Swap) { // In UnitTest
		return ((Data2Swap & 0xFF000000) >> 24) | ((Data2Swap & 0x00FF0000) >> 8) | ((Data2Swap & 0x0000FF00) << 8) | ((Data2Swap & 0x000000FF) << 24);
	}

	uint64_t SwapEndian64(uint64_t Data2Swap) { // In UnitTest
		return (((Data2Swap & 0xFF00000000000000) >> 56) | ((Data2Swap & 0x00FF000000000000) >> 40) | \
				((Data2Swap & 0x0000FF0000000000) >> 24) | ((Data2Swap & 0x000000FF00000000) >>  8) | \
				((Data2Swap & 0x00000000FF000000) <<  8) | ((Data2Swap & 0x0000000000FF0000) << 24) | \
				((Data2Swap & 0x000000000000FF00) << 40) | ((Data2Swap & 0x00000000000000FF) << 56));
	}

	uint64_t Bits2Bytes(uint64_t Bits) { // In UnitTest
		return (Bits / 8);
	}

	uint64_t Bytes2Bits(uint64_t Bytes) { // In UnitTest
		return (Bytes * 8);
	}

    uint8_t BitsRemaining(uint64_t BitsAvailable) { // In UnitTest
		return BitsAvailable > 8 ? 8 : BitsAvailable;
	}

	uint64_t Signed2Unsigned(int64_t Signed) { // In UnitTest
		return (uint64_t)Signed;
	}

	int64_t Unsigned2Signed(uint64_t Unsigned) {  // In UnitTest
		return (int64_t)Unsigned;
	}

    uint64_t Power2Mask(uint8_t Exponent) { // In UnitTest
        if ((Exponent <= 0) || (Exponent > 64)) { // Exponent = 1
            return EXIT_FAILURE;
        } else {
            if (Exponent == 1) {
                return 1;
            } else if (Exponent == 64) {
                return ((1ULL << Exponent) - 1) + (((1ULL << Exponent) - 1) - 1);
            } else {
                return (1ULL << Exponent) - 1; // Exponent = 6 // ((1ULL << (Exponent - 1)) - 1)
            } // (1ULL << (Exponent - 1)) + ((1ULL << Exponent -1) -1);
        }
    }

	uint64_t OnesCompliment2TwosCompliment(int64_t Input) { // All unset bits ABOVE the set bit are set, including those originally set
		return ~Input + 1;
	}

	uint64_t TwosCompliment2OnesCompliment(int64_t Input) { // All unset bits become set, except those originally set
		return Input ^ 0xFFFFFFFFFFFFFFFF;
	}

    uint8_t DetectSystemEndian(void) {
        uint8_t SystemEndian = 0;
        uint16_t Endian = 0xFFFE;
        if (Endian == 0xFFFE) {
            SystemEndian = LittleEndian;
        } else if (Endian == 0xFEFF) {
            SystemEndian = BigEndian;
        } else {
            SystemEndian = UnknownEndian;
        }
        return SystemEndian;
    }

	bool IsStreamByteAligned(uint64_t BitsUsed, uint8_t BytesOfAlignment) {
        if ((BitsUsed % Bytes2Bits(BytesOfAlignment)) == 0) {
            return true;
        } else {
            return false;
        }
	}

	void AlignInput(BitInput *BitI, uint8_t BytesOfAlignment) {
        uint8_t Bits2Align = BitI->BitsUnavailable % Bytes2Bits(BytesOfAlignment);

        if (Bits2Align != 0) { // NOT aligned
            BitI->BitsAvailable   -= Bits2Align;
            BitI->BitsUnavailable += Bits2Align;
        }
	}

	void AlignOutput(BitOutput *BitO, uint8_t BytesOfAlignment) {
        uint8_t Bits2Align = BitO->BitsUnavailable % Bytes2Bits(BytesOfAlignment);
        if (Bits2Align != 0) {
            BitO->BitsAvailable    -= Bits2Align;
            BitO->BitsUnavailable  += Bits2Align;
        }
	}

	static void PrintHelp(void) {
		fprintf(stderr, "Usage: -i <input> -o <output> (-s is to specify the start of a file sequence, and should be before the argument it's intended for)\n");
	}

    void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]) {
        while (BitI->File == NULL) {
            for (int Argument = BitIOCurrentArgument; Argument < argc; Argument++) {
                char Path[BitIOPathSize];
                snprintf(Path, BitIOPathSize, "%s", argv[Argument]);

                if ((strcasecmp(Path, "-s") == 0) && (BitInputCurrentSpecifier == 0)) {
                    Argument += 1;
                    sscanf(Path, "%lld", &BitInputCurrentSpecifier);
                }

                if (strcasecmp(Path, "-i") == 0) {
                    Argument += 1;
                    if (BitInputCurrentSpecifier != 0) {
                        snprintf(Path, BitIOPathSize, "%s", argv[Argument]);
                    } else {
                        snprintf(Path, BitIOPathSize, "%s", argv[Argument]);
                    }

                    if (strcasecmp(Path, "-") == 0) {
                        BitI->File = freopen(Path, "rb", stdin);
                        setvbuf(BitI->File, BitI->Buffer, _IONBF, BitInputBufferSize);
                    } else {
                        BitI->File = fopen(Path, "rb");
                        setvbuf(BitI->File, BitI->Buffer, _IONBF, BitInputBufferSize);
                        if (BitI->File == NULL) {
                            BitI->ErrorStatus->ParseInputOptions = FopenFailed;
                            Log(SYSCritical, "BitIO", "ParseInputOptions", strerror(errno));
                        }
                    }
                    BitIOCurrentArgument = Argument + 1;
                }
            }
        }
    }

    void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]) {
        while (BitO->File == NULL) {
            for (int Argument = BitIOCurrentArgument; Argument < argc; Argument++) { // The problem is that InputBuffer read past where it should.
                int64_t SpecifierOffset = 0;
                char Path[BitIOPathSize];
                snprintf(Path, BitIOPathSize, "%s", argv[Argument]);

                if (strcasecmp(Path, "-s")    == 0) { // Specifier Offset
                    Argument += 1;
                    snprintf((char*)SpecifierOffset, 1, "%s", argv[Argument]);
                }

                if (strcasecmp(Path, "-o")    == 0) {
                    Argument += 1;
                    snprintf(Path, BitIOPathSize, "%s", argv[Argument]);
                    if (strcasecmp(Path, "-") == 0) {
                        BitO->File = freopen(Path, "wb", stdout);
                        setvbuf(BitO->File, BitO->Buffer, _IONBF, BitOutputBufferSize);
                    } else {
                        BitO->File = fopen(Path, "wb");
                        setvbuf(BitO->File, BitO->Buffer, _IONBF, BitOutputBufferSize);
                        if (BitO->File == NULL) {
                            BitO->ErrorStatus->ParseOutputOptions = FopenFailed;
                            Log(SYSCritical, "BitIO", "ParseOutputOptions", strerror(errno));
                        }
                    }
                    BitIOCurrentArgument = Argument + 1;
                }
            }
        }
	}

	void InitBitInput(BitInput *BitI, ErrorStatus *ES, int argc, const char *argv[]) {
		// FIXME: Remove any quotes on the path, or the issue could be that the -i and location are specified together...
		if (argc < 3) {
			PrintHelp();
		} else {
			if (BitI->ErrorStatus == NULL) {
				BitI->ErrorStatus  = ES;
			}
			BitI->SystemEndian = DetectSystemEndian();
			ParseInputOptions(BitI, argc, argv);

			fseek(BitI->File, 0, SEEK_END);
			BitI->FileSize         = (uint64_t)ftell(BitI->File);
			fseek(BitI->File, 0, SEEK_SET);
            BitI->FilePosition     = ftell(BitI->File);
			//uint64_t Bytes2Read    = BitI->FileSize > BitInputBufferSize ? BitInputBufferSize : BitI->FileSize;
			uint64_t BytesRead     = fread(BitI->Buffer, 1, BitInputBufferSize, BitI->File);
			if ((BitI->FilePosition + BytesRead < BitI->FileSize) && (BytesRead < BitInputBufferSize)) { // Bytes2Read
                BitI->ErrorStatus->InitBitInput = FreadReturnedTooLittleData;
				Log(SYSCritical, "BitIO", "InitBitInput", strerror(errno));
			}
			BitI->BitsAvailable    = Bytes2Bits(BytesRead);
			BitI->BitsUnavailable  = 0;
		}
	}

	void InitBitOutput(BitOutput *BitO, ErrorStatus *ES, int argc, const char *argv[]) {
		if (argc < 3) {
			PrintHelp();
		} else {
			if (BitO->ErrorStatus == NULL) {
				BitO->ErrorStatus = ES;
			}
			BitO->SystemEndian     = DetectSystemEndian();
			ParseOutputOptions(BitO, argc, argv);
			BitO->BitsAvailable    = BitOutputBufferSizeInBits;
			BitO->BitsUnavailable  = 0;
		}
	}

	void CloseBitInput(BitInput *BitI) {
		fclose(BitI->File);
		memset(BitI->Buffer, 0, Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable));
		free(BitI);
	}

	void CloseBitOutput(BitOutput *BitO) {
        fwrite(BitO->Buffer, BitO->BitsUnavailable % 8, 1, BitO->File); // Make sure it's all written to disk
        fflush(BitO->File);
		fclose(BitO->File);
		memset(BitO->Buffer, 0, Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable));
		free(BitO);
	}

	void UpdateInputBuffer(BitInput *BitI, int64_t RelativeOffsetInBytes) {
        uint64_t Bytes2Read = 0, BytesRead = 0;

        fseek(BitI->File, RelativeOffsetInBytes, SEEK_CUR);
        BitI->FilePosition = ftell(BitI->File);
        //uint64_t Bytes2Read = BitI->FileSize - BitI->FilePosition > Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable) ? Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable) : BitI->FileSize - BitI->FilePosition;
        memset(BitI->Buffer, 0, BitInputBufferSize); // Bytes2Read
        Bytes2Read = BitI->FileSize - BitI->FilePosition >= BitInputBufferSize ? BitInputBufferSize : BitI->FileSize - BitI->FilePosition;
        BytesRead = fread(BitI->Buffer, 1, Bytes2Read, BitI->File); // Bytes2Read
        if (BytesRead != Bytes2Read) { // Bytes2Read
            BitI->ErrorStatus->UpdateInputBuffer = FreadReturnedTooLittleData;
			Log(SYSWarning, "BitIO", "UpdateInputBuffer", NULL);
		}
        uint64_t NEWBitsUnavailable = BitI->BitsUnavailable % 8; // FIXME: This assumes UpdateBuffer was called with at most 7 unread bits...

        BitI->BitsUnavailable = NEWBitsUnavailable;
        BitI->BitsAvailable   = Bytes2Bits(BytesRead);
	}
	
	uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read) { // Set this up so it can read from memory addresses, to support running on machines without an OS.
		uint8_t Bits = Bits2Read, UserBits = 0, SystemBits = 0, LeftShift = 0, RightShift = 0, Mask = 0, Data = 0, Mask2Shift = 0;
		uint64_t OutputData = 0;
		
		if ((Bits2Read <= 0) || (Bits2Read > 64)) {
			BitI->ErrorStatus->PeekBits = NumberNotInRange;
			char Description[BitIOPathSize];
			snprintf(Description, BitIOPathSize, "ReadBits only supports reading 1-64 bits at a time, you tried reading: %d bits\n", Bits2Read);
			Log(SYSCritical, "BitIO", "ReadBits", Description);
			exit(EXIT_FAILURE);
		} else {
			if (BitI->BitsAvailable < Bits) {
				UpdateInputBuffer(BitI, 0);
			}
			while (Bits > 0) {
				SystemBits             = 8 - (BitI->BitsUnavailable % 8);
				UserBits               = BitsRemaining(Bits);
				Bits2Read              = SystemBits >= UserBits  ? UserBits : SystemBits;
				Mask2Shift             = SystemBits <= UserBits  ? 0 : SystemBits - UserBits;
				Mask                   = (Power2Mask(Bits2Read) << Mask2Shift);
				Data                   = BitI->Buffer[BitI->BitsUnavailable / 8] & Mask;
				Data                 >>= Mask2Shift;
				
				OutputData           <<= SystemBits >= UserBits ? UserBits : SystemBits;
				OutputData            += Data;
				
				BitI->BitsAvailable   -= SystemBits >= UserBits ? UserBits : SystemBits;
				BitI->BitsUnavailable += SystemBits >= UserBits ? UserBits : SystemBits;
				Bits                  -= SystemBits >= UserBits ? UserBits : SystemBits;
			}
		}
		return OutputData;
	}

	/*
	uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read) { // We need to make this PeekBits basically.
		uint64_t OutputData = 0;

		if ((Bits2Read <= 0) && (Bits2Read > 64)) {
            BitI->ErrorStatus->ReadBits = NumberNotInRange;
			char Description[BitIOPathSize];
			snprintf(Description, BitIOPathSize, "ReadBits only supports reading 1-64 bits at a time, you tried reading: %d bits\n", Bits2Read);
			Log(SYSCritical, "BitIO", "ReadBits", Description);
		} else {
			OutputData             = PeekBits(BitI, Bits2Read); // , InputEndian
			if (BitI->ErrorStatus->PeekBits != Success) {
				BitI->ErrorStatus->ReadBits  = BitI->ErrorStatus->PeekBits;
			}
			BitI->BitsUnavailable += Bits2Read;
			BitI->BitsAvailable   -= Bits2Read;
		}
		return OutputData;
	}
	 */

	void ReadRLEData(BitInput *BitI, size_t BufferSize, uint8_t *DecodedBuffer, size_t RLESize) {
        
	}

	void WriteRLEData(BitOutput *BitO, size_t BufferSize, uint8_t *Buffer2Encode, size_t RLESize) {

	}

	uint64_t ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated) {
		uint64_t Zeros = 0;
		uint64_t  Data = 0;

        while (ReadBits(BitI, 1) != 1) {
            Zeros += 1;
        }
        Data  = (1LLU << Zeros);
        Data += ReadBits(BitI, Zeros);

        /*
         Data = ReadBits(BitI, 1);

		if ((Zeros == 0) && (Data == 1)) { // If this is the first loop?
			while (ReadBits(BitI, 1) == 0) {
				Zeros += 1;
			}
			Data  = (1 << Zeros);
			Data += ReadBits(BitI, Zeros);
		}
		if (Zeros == 0) { // Zero case
			Data = 0;
		}
         */
		if (IsSigned == true) {
			Data  = Unsigned2Signed(Data);
		}
		if (IsTruncated == true) {
			Data  = 2 * Zeros - 1; // Left bit first
			Data += ReadBits(BitI, Zeros);
		}
		return Data;
	}

	uint64_t ReadRICE(BitInput *BitI, bool StopBit) {
		uint64_t BitCount = 0;
		if ((StopBit != 0) || (StopBit != 1)) {
			BitI->ErrorStatus->ReadRICE = NumberNotInRange;
		} else {
			while (PeekBits(BitI, 1) != StopBit) { // The StopBit needs to be included in the count.
				SkipBits(BitI, 1);
				BitCount += 1;
			}
		}
		return BitCount + 1;
	}

	void WriteRICE(BitOutput *BitO, bool StopBit, uint64_t Data2Write) {
		if ((StopBit < 0) || (StopBit > 1)) {
			BitO->ErrorStatus->WriteRICE = NumberNotInRange;
		} else {
			for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
                WriteBits(BitO, (1 ^ StopBit), 1); // FIXME: XOR?
			}
			WriteBits(BitO, StopBit, 1);
		}
	}
	
	uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek) {
		uint64_t OutputData = 0ULL;
		OutputData = ReadBits(BitI, Bits2Peek);
		
		BitI->BitsAvailable       += Bits2Peek;
		BitI->BitsUnavailable     -= Bits2Peek;
		
		return OutputData;
	}

	/*
    uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek) { // 26, 22,215
		uint8_t Bits = Bits2Peek, UserBits = 0, SystemBits = 0, LeftShift = 0, RightShift = 0, Mask = 0, Data = 0, Bits2Read = 0, Mask2Shift = 0;
        uint64_t OutputData = 0;

        if ((Bits2Read <= 0) && (Bits2Read > 64)) {
            BitI->ErrorStatus->PeekBits = NumberNotInRange;
            char Description[BitIOPathSize];
            snprintf(Description, BitIOPathSize, "ReadBits only supports reading 1-64 bits at a time, you tried reading: %d bits\n", Bits2Read);
            Log(SYSCritical, "BitIO", "ReadBits", Description);
            exit(EXIT_FAILURE);
        }

		if (BitI->BitsAvailable < Bits) {
			UpdateInputBuffer(BitI, 0);
		}

		while (Bits > 0) {
            SystemBits             = 8 - (BitI->BitsUnavailable % 8); // 1
            UserBits               = BitsRemaining(Bits); // 8

            Bits2Read              = SystemBits >= UserBits  ? UserBits : SystemBits; // 1
            Mask2Shift             = SystemBits <= UserBits  ? 0 : SystemBits - UserBits; // 0
            Mask                   = (Power2Mask(Bits2Read) << Mask2Shift); // 0x1F
            Data                   = BitI->Buffer[BitI->BitsUnavailable / 8] & Mask; // 0x1F
            Data                 >>= Mask2Shift; // 0x1F

            OutputData           <<= SystemBits >= UserBits ? UserBits : SystemBits;
            OutputData            += Data; // The issue is that I need to mask this addition as well.
            
            BitI->BitsAvailable   -= SystemBits >= UserBits ? UserBits : SystemBits;
			BitI->BitsUnavailable += SystemBits >= UserBits ? UserBits : SystemBits;
            Bits                  -= SystemBits >= UserBits ? UserBits : SystemBits;
		}

		BitI->BitsAvailable       += Bits2Peek;
		BitI->BitsUnavailable     -= Bits2Peek;

		return OutputData;
	}
	 */
	
	
	
	void WriteBits(BitOutput *BitO, uint64_t Data2Write, uint8_t NumBits) { // 12 bits 2 write, 0xFFF
		uint8_t BitsLeft = NumBits, InputMask = 0, OutputMask = 0, Bits2Write = 0;
		if (BitO->BitsAvailable < NumBits) {
			fwrite(BitO->Buffer, Bits2Bytes(BitO->BitsUnavailable), 1, BitO->File);
			// Save unused bits, memset, and recopy them to the start of the buffer
		}
		// in order to write bits to the buffer, I need to mask Data2Write, and apply it to the output buffer, also, with a mask.
		while (BitsLeft > 0) {
			// if BitsLeft is greater than 8, the input mask needs to be 8, otherwise, set it to BitsLeft
			Bits2Write   = BitsLeft > 8 ? 8 : BitsLeft;
			InputMask    = Power2Mask(Bits2Write);
			BitO->Buffer[BitO->BitsUnavailable / 8] = Data2Write & InputMask;
			Data2Write >>= Bits2Write;
			BitsLeft    -= Bits2Write;
		}
	}
	
	/*
	void WriteBits(BitOutput *BitO, uint64_t Data2Write, size_t NumBits) {
        if (NumBits > BitO->BitsAvailable) {
            fwrite(BitO->Buffer, sizeof(BitO->Buffer), 1, BitO->File);
            memset(BitO->Buffer, 0xFF, sizeof(BitO->Buffer));
        } else {

        }







		if (NumBits <= 0) {
			BitO->ErrorStatus->WriteBits = TriedWritingTooFewBits;
            char Error[BitIOStringSize];
            snprintf(Error, BitIOStringSize, "Tried writing %zu bits\n", NumBits);
            Log(SYSWarning, "BitIO", "WriteBits", "");
		} else {
			if (BitO->BitsAvailable < NumBits) {
				// Write the completed bytes out, save the uncompleted ones in the array.
			}

            if (BitO->BitsAvailable < BitOutputBufferSize) { // while (BitO->BitsUnavailable >= BitOutputBufferSize) {
                fwrite(BitO->Buffer, 1, Bits2Bytes(BitO->BitsUnavailable), BitO->File);
                fflush(BitO->File);
                memcpy(&BitO->Buffer, &BitO->Buffer + Bits2Bytes(BitO->BitsUnavailable), (Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable) - Bits2Bytes(BitO->BitsUnavailable)));
                memset(BitO->Buffer, 0, Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable));
                BitO->BitsUnavailable = (BitO->BitsUnavailable - ((BitO->BitsUnavailable / 8) * 8));
            }

            for (uint64_t Bit = 0; Bit < NumBits; Bit++) {
                uint64_t X = Data2Write & BitsRemaining(BitO->BitsUnavailable);
                BitO->Buffer[Bits2Bytes(BitO->BitsUnavailable)] += X;
                BitO->BitsUnavailable++;
            }
			// FIXME: We need to just enlarge the buffer, the call a function to flush it all out to disk.
		}
	}
	 */

	/*
    void WriteBuffer(BitOutput *BitI, uint8_t *Buffer2Write, size_t BufferSize) {

    }
	 */

	void SkipBits(BitInput *BitI, int64_t Bits) {
		if (Bits <= BitI->BitsAvailable) {
			BitI->BitsAvailable   -= Bits;
			BitI->BitsUnavailable += Bits;
		} else {
			fseek(BitI->File, Bits2Bytes(Bits - BitI->BitsAvailable), SEEK_CUR);
            BitI->BitsAvailable   = 0;
            BitI->BitsUnavailable = 0;
            UpdateInputBuffer(BitI, 0); // Bits2Bytes(Bits)
		}
	}

	uint64_t GenerateCRC(uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, uint64_t Init, uint8_t CRCSize) {
		uint64_t Output = ~Init;
		uint64_t Polynomial = 1 << CRCSize; // Implicit bit
		Polynomial += (Poly & Power2Mask(CRCSize));

		// Create a BitInput version of the data buffer, to readbits from the buffer.
        //BitBuffer *CRCBits = calloc(CRCSize, 1);
        //InitBitBuffer(CRCBits, DataBuffer, BufferSize);

		Init > 1 ? Power2Mask(CRCSize) : 1;

		for (size_t Byte = 0; Byte < BufferSize; Byte++) {
			uint64_t Bits2XOR = 0;//ReadBitBuffer(CRCBits, CRCSize);
			// if there aren't enough bits, simply shift to MSB to append 0s.

		}
		return Output;
	}

	bool VerifyCRC(uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, uint64_t Init, uint8_t CRCSize, uint64_t EmbeddedCRC) {
		uint64_t GeneratedCRC = GenerateCRC(DataBuffer, BufferSize, Poly, Init, CRCSize);
		if (GeneratedCRC == EmbeddedCRC) {
			return true;
		} else {
			return false;
		}
	}

	void Log(int64_t SYSError, char Library[BitIOStringSize], char Function[BitIOStringSize], char Description[BitIOStringSize]) {
		char ComputerName[BitIOStringSize] = {0};
		size_t StringSize = 0;

		if ((SYSError == SYSPanic) || (SYSError == SYSCritical)) {
			openlog(Library, SYSError, (LOG_PERROR|LOG_MAIL|LOG_USER));
		} else {
			openlog(Library, SYSError, (LOG_PERROR|LOG_USER));
		}

		//time_t *Time;
		struct tm *Time = calloc(sizeof(struct tm), 1);
        char CurrentTime[26] = {0};
		//time(Time);
		StringSize = strftime(CurrentTime, 26, "%A, %B %e, %g+1000: %I:%M:%S %p %Z", Time);
		if ((StringSize < 0) || (StringSize > BitIOStringSize)) {
			fprintf(stderr, "BitIO - Log: String too big %zu\n", StringSize);
		}

		// ADD SYSTEM NAME TO THE BEGINNING OF EACH LOG FILE AND LOGFILE NAME.
		errno = gethostname(ComputerName, BitIOStringSize);
		if (errno != 0) {
			fprintf(stderr, "Log error: %d\n", errno);
		}
		syslog(SYSError, "%s - %s: %s - %s: %s\n", ComputerName, CurrentTime, Library, Function, Description);

        free(Time);
	}

	void CreateHuffmanTree(uint16_t *SymbolOccurance) {
		
	}

	void SortArrayByValue(uint16_t *Symbols[], uint16_t *Probability[], uint16_t *SortedArray, size_t NumSymbols) {
		uint16_t PreviousProbability = 0;
		uint16_t CurrentProbabiity   = 0;
		uint16_t CurrentSymbol       = 0;
		uint16_t PreviousSymbol      = 0;

		// Symbols and probabilities have the same index. So that means we just need the probability table, "symbols" are generated by the for loop.
		// So that means we generate a table of sorted symbols, so the index of the generated symbols is their commonness, and the value is the symbol.
		// In order to do that we need to loop through each symbol, and place it in the approperiate index.
		// To do THAT we need an exmple.

		/*
		 Lets say that symbol 0x7C57 = highest probability, at 192 occurances.
		 So once the for loop hit that number, we'd need to put that 
		 */


		for (uint16_t PotentialSymbol = 0; PotentialSymbol < NumSymbols; PotentialSymbol++) {
			if (PotentialSymbol == *Symbols[PotentialSymbol]) { // Symbol is actually in the table
				CurrentSymbol = PotentialSymbol;
				if (CurrentSymbol) {

				}
			}
		}






		for (uint16_t Symbol = 0; Symbol < NumSymbols; Symbol++) {
			// Go through all the probabilities and look for any runs, if any symbols have the same probability, put them in numerical order of the symbol. aka Y before Z but after X.
			CurrentProbabiity = *Probability[Symbol];
			CurrentSymbol     = Symbol;
			if (CurrentProbabiity < PreviousProbability) {
				// Move the current Probability AND symbol up a notch, otherwise down.
				SortedArray[Symbol] = *Symbols[Symbol - 1];

			} else if ((CurrentProbabiity == PreviousProbability) && (Symbol)) {
				if (Symbols[Symbol] > Symbols[Symbol - 1]) { // if the current symbol is less than the previous symbol numerically, move it down
					SortedArray[Symbol] = *Symbols[Symbol - 1];
				}
			} else {
				SortedArray[Symbol] = *Symbols[Symbol + 1];
			}
		}
	}

	/* Huffman Decoding functions */
	void DecodeHuffman(BitInput *BitI, size_t HuffmanSize) {
		// 3 alphabets, literal, "alphabet of bytes", or <length 8, distance 15> the first 2 are combined, 0-255 = literal, 256 = End of Block, 257-285 = length
		// FIXME: The Tilde ~ symbol is the negation symbol in C!!!!! XOR = ^

		uint8_t  DecodedData[32768] = {0};
		/* Parse Huffman block header */
		bool     IsLastHuffmanBlock     = ReadBits(BitI, 1);
		uint8_t  HuffmanCompressionType = ReadBits(BitI, 2); // 0 = none, 1 = fixed, 2 = dynamic, 3 = invalid.
		int32_t  DataLength             = 0;
		int32_t  OnesComplimentOfLength = 0; // Ones Compliment of DataLength

		if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != HuffmanSize) { // Make sure the numbers match up
            BitI->ErrorStatus->DecodeHuffman = InvalidData;
			char String2Print[BitIOStringSize];
			snprintf(String2Print, BitIOStringSize, "One's Compliment of Length: %d != Length %d", OnesComplimentOfLength, DataLength);
			Log(SYSWarning, "BitIO", "DecodeHuffman", String2Print);
		}

		if (IsLastHuffmanBlock == true) {

		}

		if (HuffmanCompressionType == 0) { // No compression.
			AlignInput(BitI, 1); // Skip the rest of the current byte
			DataLength             = ReadBits(BitI, 32);
			OnesComplimentOfLength = ReadBits(BitI, 32);
			if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != DataLength) {
				// Exit because there's an issue.
			}
			for (size_t Byte = 0; Byte < DataLength; Byte++) {
				DecodedData[Byte] = ReadBits(BitI, 8);
			}
		} else if (HuffmanCompressionType == 1) { // Static Huffman.
			uint8_t  Length   = (ReadBits(BitI, 8) - 254);
			uint16_t Distance = ReadBits(BitI, 5);

		} else if (HuffmanCompressionType == 2) { // Dynamic Huffman.
			/*
			 Huff->Dynamic->Length     = ReadBits(BitI, 5) + 257;
			 Huff->Dynamic->Distance   = ReadBits(BitI, 5) + 1;
			 Huff->Dynamic->CodeLength = ReadBits(BitI, 4) + 4;
			 */
		} else { // Invalid.
				 // Reject the stream.
		}
		/*
		 if compressed with dynamic Huffman codes
		 read representation of code trees (see
		 subsection below)
		 loop (until end of block code recognized)
		 decode literal/length value from input stream
		 if value < 256
		 copy value (literal byte) to output stream
		 otherwise
		 if value = end of block (256)
		 break from loop
		 otherwise (value = 257..285)
		 decode distance from input stream

		 move backwards distance bytes in the output
		 stream, and copy length bytes from this
		 position to the output stream.
		 end loop
		 while not last block
		 */
	}

	void ParseDeflate(BitInput *BitI) {
		uint8_t CompressionInfo    = ReadBits(BitI, 4); // 7 = LZ77 window size 32k
		uint8_t CompressionMethod  = ReadBits(BitI, 4); // 8 = DEFLATE
		uint8_t CheckCode          = ReadBits(BitI, 5); // for the previous 2 fields, MUST be multiple of 31
		bool    DictionaryPresent  = ReadBits(BitI, 1); // false
		uint8_t CompressionLevel   = ReadBits(BitI, 2); // Fixed Huffman
		if (DictionaryPresent == true) {
			uint16_t Dictionary    = ReadBits(BitI, 16);
		}
	}

	uint32_t GenerateAdler32(uint8_t *Data, size_t DataSize) { // In UnitTest
		// Add all values up, then modulo it by 65521 for Sum1. byte bound.
		// Sum2 is Sum1 ran through the algorithm again.
		// Sum2 is stored before Sum1. Big Endian.

		uint32_t Adler  = 1;
		uint32_t Sum1   = Adler & 0xFFFF;
		uint32_t Sum2   = (Adler >> 16) & 0xFFFF;

		for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
			Sum1 += Data[Byte] % 65521;
			Sum2 += Sum1 % 65521;
		}
		return (Sum2 << 16) + Sum1;
	}

	bool VerifyAdler32(uint8_t *Data, size_t DataSize, uint32_t EmbeddedAdler32) { // In UnitTest
		uint32_t GeneratedAdler32 = GenerateAdler32(Data, DataSize);
		if (GeneratedAdler32 != EmbeddedAdler32) {
			return false;
		} else {
			return true;
		}
	}

	Probabilities FindProbabilityFromSymbol(Probabilities *Probability, double *MaximumTable, double *MinimumTable, size_t TableSize, uint64_t Symbol2Lookup) {
		if (Symbol2Lookup > TableSize) {
			// Not good.
		} else {
			Probability->Maximum = MaximumTable[Symbol2Lookup];
			Probability->Minimum = MinimumTable[Symbol2Lookup];
		}
		return *Probability;
	}

	// Create a function to lookup the symbol from the probabilities
	uint16_t FindSymbolFromProbability(double Probability, uint64_t	*MaximumTable, uint64_t *MinimumTable, size_t TableSize) {
		uint16_t Symbol = 0; // there is a SINGLE probability, not two...
							 // If the probability is closer to 1 than 0, start the loop at 1, instead of 0. otherwise, start it at 0. to ensure it takes half the time to traverse it.

		bool WhichEnd = round(Probability);

		if (WhichEnd == 0) {
			for (uint64_t Index = 0; Index < TableSize; Index++) {
				uint64_t MaxProb   = MaximumTable[Index];
				uint64_t MinProb   = MinimumTable[Index];
				if ((Probability  >= MinProb) && (Probability <= MaxProb)) { // You found the symbol!
					Symbol = Index;
				}
			}
		} else {
			for (uint64_t Index = TableSize; Index > 0; Index--) {
				uint64_t MaxProb   = MaximumTable[Index];
				uint64_t MinProb   = MinimumTable[Index];
				if ((Probability  >= MinProb) && (Probability <= MaxProb)) { // You found the symbol!
					Symbol = Index;
				}
			}
		}
		return Symbol;
	}

	typedef struct Probability {
		double Low;
		double High;
	} Probability;

	uint64_t ReadArithmetic(BitInput *Input, uint64_t *MaximumTable, uint64_t *MinimumTable, size_t TableSize, uint64_t Bits2Decode) {
		// Read a bit at a time.
		uint64_t High = 0xFFFFFFFFFFFFFFFFULL, Low = 0ULL; // Decimal point is implied before the highest bit.
		return 0;
	}

	void WriteArithmetic(BitOutput *BitO, double *ProbabilityTable[], size_t TableSize, uint64_t Bits2Encode) { // Use the least precision you can get away with to be as efficent as possible.
		uint64_t High = 0xFFFFFFFFFFFFFFFFULL, Low = 0ULL, Range = 0ULL, Probability = 0ULL;
		while ((Bits2Encode >= High) && (Bits2Encode <= Low)) {
			Range = (High - Low) + 1;
			Probability = ProbabilityTable[Range]; // Probability should be an int table ordered on how often a symbol shows up, not it's quantized probability.
			
		}
	}

#ifdef __cplusplus
}
#endif
