#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	uint64_t BitInputCurrentArgument   = 1;
	uint64_t BitOutputCurrentArgument  = 1;
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
	
	uint8_t CountBitsSet(uint64_t Data) {
		uint8_t DataBit = 0, BitCount = 0;
		for (uint8_t Bit = 0; Bit < Bits2Bytes(sizeof(Data)); Bit++) {
			DataBit = (Data & (1 << Bit)) >> Bit;
			if (DataBit == 1) {
				BitCount += 1;
			}
		}
		return BitCount;
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
	
	bool IsOdd(int64_t X) {
		return X % 2 == 0 ? false : true;
	}
	
	uint8_t  FindHighestBitSet(uint64_t X) {
		uint8_t HighestBitSet = 0;
		// use sizeof to get the size of the included bit (check to see if its always cast to 64 tho, if so just start at 64)
		// then count down from 64 to 0, stopping when you first find a 1 bit, mark the number of loops. that's the highest bit set.
		for (uint8_t Bit = sizeof(X); Bit > 0; Bit--) {
			if (((X & Bit) >> (sizeof(X) - Bit)) == 1) {
				HighestBitSet = Bit;
				break;
			}
		}
		return HighestBitSet;
	}
	
	void OpenInputFile(BitInput *BitI, CommandLineOptions *CMD) {
		
	}
	
	void DisplayHelp(CommandLineOptions *CMD) {
		printf("%s:\t", CMD->ProgramName);
		printf("%s\n",  CMD->ProgramDescription);
		printf("%s\n",  CMD->AuthorCopyrightLicense);
		printf("\n\n");
		printf("Options:\n");
		for (uint8_t Option = 0; Option < CMD->NumSwitches; Option++) {
			printf("%s\t", CMD->Switch[Option]->Switch);
			printf("%s\n", CMD->Switch[Option]->SwitchDescription);
		}
	}
	
	void ParseCommandLineArguments(int argc, char *argv[], CommandLineOptions *CMD) {
		char Argument[BitIOStringSize];
		size_t EqualsLocation = 0;
		
		if (argc < CMD->NumSwitches + 1) {
			DisplayHelp(CMD);
		} else {
			// Scan for equals signs as well, if found, after after the equal sign is the result, everything before is the switch.
			for (int Index = BitIOCurrentArgument; Index < argc; Index++) {
				if (strcasecmp(Argument, "-h") == 0 || strcasecmp(Argument, "--help") == 0 || strcasecmp(Argument, "/?")) {
					DisplayHelp(CMD);
				} else {
					EqualsLocation = strchr(Argument, 0x3D); // 0x3D = "="
					if (EqualsLocation != NULL) { // found
												 // Start reading the result of the switch from EqualLocation +1.
						for (size_t Byte = EqualsLocation + 1; Byte < sizeof(argv[Index]); Byte++) {
							for (size_t SwitchByte = 0; SwitchByte < sizeof(argv[Index]); SwitchByte++) {
								CMD->Switch[Index]->SwitchResult[SwitchByte] = *argv[Index];
							}
						}
					}
					
					if (strcasecmp(CMD->Switch[Index]->Switch, argv[Index]) == 0) { // If the current switch matches one of the switches, set the IsFound bool to true.
						CMD->Switch[Index]->SwitchFound = true;
						snprintf(CMD->Switch[Index]->SwitchResult, BitIOStringSize, "%s", argv[Index]);
					}
				}
			}
		}
	}

	static void PrintHelp(void) {
		fprintf(stdout, "Usage: -i <input> -o <output> (-s is to specify the start of a file sequence, and should be before the argument it's intended for)\n");
		fprintf(stdout, "Usage: -ia <input address>, -ias <input buffer size>, -oa <output address>, -oas <output buffer size>\n");
	}

	void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]) {
		while (BitI->File == NULL) {
			for (int Index = BitInputCurrentArgument; Index < argc; Index++) {
				char Argument[BitIOPathSize];
				snprintf(Argument, BitIOPathSize, "%s", argv[Index]);
				
				if ((strcasecmp(Argument, "-s") == 0) && (BitInputCurrentSpecifier == 0)) {
					Index += 1;
					sscanf(Argument, "%lld", &BitInputCurrentSpecifier);
				}
				
				if (strcasecmp(Argument, "-i") == 0) {
					Index += 1;
					if (BitInputCurrentSpecifier != 0) { // I have absolutely no idea why this is here
						snprintf(Argument, BitIOPathSize, "%s", argv[Index]);
					} else {
						snprintf(Argument, BitIOPathSize, "%s", argv[Index]);
					}
					
					if (strcasecmp(Argument, "-") == 0) {
						BitI->File = stdin;
					} else {
						BitI->File = fopen(Argument, "rb");
						//errno = setvbuf(BitI->File, &BitI->Buffer, _IONBF, BitInputBufferSize);
						if (BitI->File == NULL) {
							BitI->ErrorStatus->ParseInputOptions = FopenFailed;
							Log(SYSCritical, "BitIO", "ParseInputOptions", strerror(errno));
						}
					}
					BitInputCurrentArgument = Index + 1;
				}
				
				if (strcasecmp(Argument, "-ia")) { // input address
					Index += 1;
					BitI->StartReadAddress = (uint64_t)Argument;
					BitI->IsFileBased      = true;
				}
				if (strcasecmp(Argument, "-ias")) { // input address size
					Index += 1;
					BitI->ExternalBufferSize = (uint64_t)Argument;
				}
			}
		}
	}

	void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]) {
		while (BitO->File == NULL) {
			for (int Index = BitOutputCurrentArgument; Index < argc; Index++) { // The problem is that InputBuffer read past where it should.
				int64_t SpecifierOffset = 0;
				char Argument[BitIOPathSize];
				snprintf(Argument, BitIOPathSize, "%s", argv[Index]);
				
				if (strcasecmp(Argument, "-s")    == 0) { // Specifier Offset
					Index += 1;
					snprintf((char*)SpecifierOffset, 1, "%s", argv[Index]);
				}
				
				if (strcasecmp(Argument, "-o")    == 0) {
					Index += 1;
					snprintf(Argument, BitIOPathSize, "%s", argv[Index]);
					if (strcasecmp(Argument, "-") == 0) {
						BitO->File = stdout;
					} else {
						BitO->File = fopen(Argument, "wb");
						//errno = setvbuf(BitO->File, BitO->Buffer, _IONBF, BitOutputBufferSize);
						if (BitO->File == NULL) {
							BitO->ErrorStatus->ParseOutputOptions = FopenFailed;
							Log(SYSCritical, "BitIO", "ParseOutputOptions", strerror(errno));
						}
					}
					BitOutputCurrentArgument = Index + 1;
				}
				
				if (strcasecmp(Argument, "-oa")) { // Output address
					Index += 1;
					BitO->StartWriteAddress = Argument;
					BitO->IsFileBased       = true;
				}
				
				if (strcasecmp(Argument, "-oas")) { // Output address size
					Index += 1;
					BitO->ExternalBufferSize = (uint64_t)Argument;
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
			// Do file related shit here.
			ParseInputOptions(BitI, argc, argv);
			fseek(BitI->File, 0, SEEK_END);
			BitI->FileSize         = (uint64_t)ftell(BitI->File);
			fseek(BitI->File, 0, SEEK_SET);
			BitI->FilePosition     = ftell(BitI->File);
			uint64_t Bytes2Read    = BitI->FileSize > BitInputBufferSize ? BitInputBufferSize : BitI->FileSize;
			uint64_t BytesRead     = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
			if (BitI->FilePosition + BytesRead < BitI->FileSize && BytesRead < BitInputBufferSize) { // Bytes2Read
				BitI->ErrorStatus->InitBitInput = FreadReturnedTooLittleData;
				Log(SYSCritical, "BitIO", "InitBitInput", strerror(errno));
			}
			BitI->BitsAvailable    = Bytes2Bits(BytesRead);
			BitI->BitsUnavailable  = 0;
			
			// Do memory related shit here.
			// Hmm, how do we accept memory address stuff?
			// Well, we should skip creating a buffer when one already exists.
			// BUUUT the buffer has already been initiated, so we have to use it.
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
        memset(BitI->Buffer, 0, sizeof(BitI->Buffer));
        Bytes2Read = BitI->FileSize - BitI->FilePosition >= BitInputBufferSize ? BitInputBufferSize : BitI->FileSize - BitI->FilePosition;
        BytesRead = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
        if (BytesRead != Bytes2Read) { // Bytes2Read
            BitI->ErrorStatus->UpdateInputBuffer = FreadReturnedTooLittleData;
			Log(SYSWarning, "BitIO", "UpdateInputBuffer", NULL);
		}
        uint64_t NEWBitsUnavailable = BitI->BitsUnavailable % 8; // FIXME: This assumes UpdateBuffer was called with at most 7 unread bits...

        BitI->BitsUnavailable = NEWBitsUnavailable;
        BitI->BitsAvailable   = Bytes2Bits(BytesRead);
	}
	
	uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read) { // Set this up so it can read from memory addresses, to support running on machines without an OS.
		uint8_t Bits = Bits2Read, UserBits = 0, SystemBits = 0, Mask = 0, Data = 0, Mask2Shift = 0;
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
	
	int64_t  ReadRICECoding(BitInput *BitI, bool Truncated, bool Signed, uint8_t StopBit) {
		if (Truncated == true) {
			// count the zero bit
		} else {
			// dont count the 0 bit
		}
		
		if (Signed == true) {
			// subtract the first bit, if it's set to 1 its negative
		}
		
		return 0;
	}
	
	uint64_t ReadRICE(BitInput *BitI, bool IsTruncated, bool StopBit) {
		uint64_t BitCount = 0; // 2
		
		if (IsTruncated == true) {
			// Truncated RICE code
			while (ReadBits(BitI, 1) != StopBit) {
				BitCount += 1;
			}
		} else {
			while (ReadBits(BitI, 1) != StopBit) {
				BitCount += 1;
			}
			
			BitCount += 1; // The StopBit needs to be included in the count.
		}
		return BitCount;
	}

	int64_t ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated) {
		uint64_t Zeros   = 0;
		uint64_t CodeNum = 0;
		int64_t  Temp    = 0;
		int64_t  Final   = 0;

		while (ReadBits(BitI, 1) != 1) {
			Zeros += 1;
		}
		
		if (IsSigned == false && IsTruncated == false) { // Unsigned
			CodeNum  = (1ULL << Zeros);
			CodeNum += ReadBits(BitI, Zeros);
		} else if (IsSigned == true && IsTruncated == false) { // Signed
			// Find out if it's negative, if so, read it normally, then use Unsigned2Signed.
			// If CodeNum is odd it's positive, even numbers are negative
			if (IsOdd(CodeNum) == true) { // Positive
				Final = CodeNum;
			} else {
				Final = -CodeNum;
			}
		} else if (IsTruncated == true && IsSigned == false) { // Truncated
			
		}
		return Final; // FIXME: CodeNum is an intemediary number, not final
	}
	
	void WriteExpGolomb(BitOutput *BitO, bool IsTruncated, bool IsMapped, uint64_t Data2Write) {
		uint64_t Data = 0;
		
		if (IsTruncated == true) {
			// Do truncated stuff here.
			// if x = M, the terminating 0 is omitted.
		} else {
			// Do signed stuff here.
			// tldr write Data2Write 1s, followed by a 0.
			for (uint8_t Bit = 0; Bit < Data2Write - 1; Bit++) {
				WriteBits(BitO, 0, 1);
			}
			WriteBits(BitO, 1, 1); // Trailing 1 bit
		}
	}

	void WriteRICE(BitOutput *BitO, bool IsTruncated, bool StopBit, uint64_t Data2Write) {
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

	uint64_t GenerateCRC(BitInput *BitI, size_t DataSize, CRC *CRCData) { // uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, bool PolyType, uint64_t Init, uint8_t CRCSize
		// So, first append Z zero bits to the original message, for who knows why.
		// Then, long divide the data by the poly, all subtractions in long division are done modulo 2. Modulo 2 subtraction = XOR
		
		// Loop over the whole data block, to calculate the CRC. use BitInput directly, instead of relying on data being submitted
		// PNG Polynomial in reversed representation:
		// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
		// x^178 + x + 1 % 4294967296;
		// Add any padding if nessicary?
		// CRC Data Input: 49484452 00000058 00000058 08060000 00
		// CRC in base 10: 1229472850 88 88 2054
		// CRC result: 0x71953034
		uint16_t CRCResult = 0;
		for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
			CRCResult = CRCData->Polynomial ^ BitI->Buffer[BitI->BitsUnavailable / 8] << 8;
			for (uint8_t Bit = 0; Bit < 8; Bit++) {
				if ((CRCResult & 0x8000) == true) {
					CRCResult = ((CRCResult <<= 1) ^ CRCData->Polynomial);
				} else {
					CRCResult <<= 1;
				}
			}
		}
		/*
		uint64_t Output = ~CRCData->Initalization;
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
		 */
		return 0;
	}

	bool VerifyCRC(BitInput *BitI, size_t DataSize, CRC *CRCData) { // uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, bool PolyType, uint64_t Init, uint8_t CRCSize, uint64_t EmbeddedCRC
		/*
		uint64_t GeneratedCRC = GenerateCRC(CRCData); // DataBuffer, BufferSize, Poly, PolyType, Init, CRCSize
		if (GeneratedCRC == EmbeddedCRC) {
			return true;
		} else {
			return false;
		}
		 */
		return false;
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
		if ((StringSize <= 0) || (StringSize > BitIOStringSize)) {
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

#ifdef __cplusplus
}
#endif
