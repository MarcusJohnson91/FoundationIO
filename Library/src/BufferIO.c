#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
	/* Start Anciliary Functions */
	int BitIOCurrentArgument = 1;
	
	uint16_t SwapEndian16(uint16_t Data2Swap) {
		return ((Data2Swap & 0xFF00) >> 8) || \
		       ((Data2Swap & 0x00FF) << 8);
	}
	
	uint32_t SwapEndian32(uint32_t Data2Swap) {
		return ((Data2Swap & 0xFF000000) >> 24) || \
		       ((Data2Swap & 0x00FF0000) >>  8) || \
			   ((Data2Swap & 0x0000FF00) <<  8) || \
		       ((Data2Swap & 0x000000FF) << 24);
	}
	
	uint64_t SwapEndian64(uint64_t Data2Swap) {
		return (((Data2Swap & 0xFF00000000000000) >> 56) || \
				((Data2Swap & 0x00FF000000000000) >> 40) || \
				((Data2Swap & 0x0000FF0000000000) >> 24) || \
				((Data2Swap & 0x000000FF00000000) >>  8) || \
				((Data2Swap & 0x00000000FF000000) <<  8) || \
				((Data2Swap & 0x0000000000FF0000) << 24) || \
				((Data2Swap & 0x000000000000FF00) << 40) || \
				((Data2Swap & 0x00000000000000FF) << 56));
	}
	
	bool IsOdd(int64_t Input) {
		return Input % 2;
	}
	
	uint64_t Bits2Bytes(uint64_t Bits) {
		return Bits / 8;
	}
	
	uint64_t Bytes2Bits(uint64_t Bytes) {
		return Bytes * 8;
	}
	
	uint8_t BitsRemaining(uint64_t BitsAvailable) {
		return 8 - (BitsAvailable % 8);
	}
	
	uint64_t Signed2Unsigned(int64_t Signed) {
		return (uint64_t)Signed;
	}
	
	int64_t Unsigned2Signed(uint64_t Unsigned) {
		return (int64_t)Unsigned;
	}
	
	uint64_t Power2Mask(uint8_t Exponent) {
		if (Exponent > 64) {
			return 0;
		}
		uint64_t Mask1 = 0, Mask2 = 0;
		Mask1 = 1 << (Exponent - 1);
		Mask2 = Mask1 - 1;
		return Mask1 + Mask2;
		// Shift 1 (Exponent - 1) times, then take the result, subtract one, and add that to the result.
	}
	
	uint64_t OnesCompliment2TwosCompliment(uint64_t Input) { // All unset bits ABOVE the set bit are set, including those originally set
		// If 1 was already set, it's set as well.
		return ~Input + 1;
	}
	
	uint64_t TwosCompliment2OnesCompliment(uint64_t Input) { // All unset bits become set, except those originally set
		return Input ^ 0xFFFFFFFFFFFFFFFF;
	}
	
	/*
	uint8_t FindHighestBitSet(uint64_t InputData) {
		// Just return 0... that should never appear, so you're good...
		uint8_t HighestBitSet = 0;
		for (uint8_t Bit = 64; Bit > 0; Bit--) { // stop at the first 1 bit, and return that.
			if ((InputData & (1 << (Bit - 1))) == 1) {
				HighestBitSet = Bit;
			}
		}
		return HighestBitSet;
	}
	 */
	/*
	uint8_t CountBits(uint64_t Input) {
		if (Input == 0) {
			return 0;
		}
		uint8_t Count = 0;
		for (size_t Bit = 64; Bit > 0; Bit--) {
			if ((Input & (1 << (Bit - 1))) == 1) {
				Count++;
			}
		}
		return Count;
	}
	*/
	
	bool IsStreamByteAligned(uint64_t BitsUnavailable) {
		if ((BitsUnavailable % 8) == 0) {
			return true;
		}
		return false;
	}
	
	void AlignInputBits2Byte(BitInput *BitI) {
		uint8_t Bits2Add       = BitsRemaining(BitI->BitsAvailable);
		BitI->BitsAvailable   -= Bits2Add;
		BitI->BitsUnavailable += Bits2Add;
	}
	
	void AlignInputBits2Bytes(BitInput *BitI, uint8_t Bytes2Align) { // FIXME: This needs a LOT of work
		uint8_t Bits2Add       = BitsRemaining(BitI->BitsAvailable);
		BitI->BitsAvailable   -= Bits2Add;
		BitI->BitsUnavailable += Bits2Add;
		if (Bytes2Align > 1) {
			uint64_t Wat = Bytes2Bits(Bytes2Align -1);
			BitI->BitsAvailable   -= Wat;
			BitI->BitsUnavailable += Wat;
		}
	}
	
	void AlignOutputBits2Byte(BitOutput *BitO) {
		uint8_t Bits2Add       = BitsRemaining(BitO->BitsAvailable);
		BitO->BitsAvailable   -= Bits2Add;
		BitO->BitsUnavailable += Bits2Add;
	}
	
	void AlignOutputBits2Bytes(BitOutput *BitO, uint8_t Bytes2Align) { // FIXME: This needs a LOT of work
		uint8_t Bits2Add       = BitsRemaining(BitO->BitsAvailable);
		BitO->BitsAvailable   += Bits2Add;
		BitO->BitsUnavailable -= Bits2Add;
		if (Bytes2Align > 1) {
			uint64_t Wat           = Bytes2Bits(Bytes2Align -1);
			BitO->BitsAvailable   -= Wat;
			BitO->BitsUnavailable += Wat;
		}
	}
	
	/* Start Obselete Path processing functions */
	static void PrintHelp(void) {
		fprintf(stderr, "Usage: -i <input> -o <output>\n");
	}
	
	void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]) {
		// I need to add some variables to BitInput to split the path into 3, one for the base path, the second for the format specifier number, and the third for the extension.
		while (BitI->File == NULL) {
			for (int Argument = BitIOCurrentArgument; Argument < argc; Argument++) {
				char Path[BitIOPathSize];
				snprintf(Path, strlen(argv[Argument]), "%s", argv[Argument]);
				
				if (strcasecmp(Path, "-i") == 0) {
					Argument += 1;
					snprintf(Path, strlen(argv[Argument]), "%s", argv[Argument]);
					if (strcasecmp(Path, "-") == 0) {
						BitI->File = freopen(Path, "rb", stdin);
					} else {
						BitI->File = fopen(Path, "rb");
						if (BitI->File == NULL) {
							Log(SYSCritical, &BitI->ErrorStatus->ParseInputOptions, FopenFailed, "BitIO", "ParseInputOptions", strerror(errno));
						}
					}
				} else {
					Argument += 1;
				}
				BitIOCurrentArgument += Argument - 1;
				if ((Argument >= argc) && (BitI->File == NULL)) { // end of the argument list with no file, so start back at the top.
					BitIOCurrentArgument = 1;
				}
			}
		}
	}
	
	void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]) {
		while (BitO->File == NULL) {
			for (int Argument = BitIOCurrentArgument; Argument < argc; Argument++) {
				int64_t SpecifierOffset = 0;
				char Path[BitIOPathSize];
				snprintf(Path, strlen(argv[Argument]), "%s", argv[Argument]);
				
				
				
				if (strcasecmp(Path, "-s") == 0) { // Specifier Offset
					Argument += 1; 
					snprintf(SpecifierOffset, 1, "%s", argv[Argument]);
				}
				
				if (strcasecmp(argv[Argument], "-o")             == 0) {
					Argument += 1;
					if (strcasecmp(argv[Argument], "-")          == 0) {
						BitO->File = freopen(argv[Argument], "wb", stdout);
					}
					/*
					else if (strcasecmp(argv[Argument], "*")   == 0) {
						glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
					} else if (strcasecmp(argv[Argument], "?")   == 0) {
						glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
					} 
					 */
					 else {
						BitO->File = fopen(argv[Argument], "wb");
						if (BitO->File == NULL) {
							Log(SYSCritical, &BitO->ErrorStatus->ParseOutputOptions, FopenFailed, "BitIO", "ParseOutputOptions", strerror(errno));
						}
					}
				}
				BitIOCurrentArgument += Argument - 1;
				if ((Argument >= argc) && (BitO->File == NULL)) { // end of the argument list with no file, so start back at the top.
					BitIOCurrentArgument = 1;
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
			setvbuf(BitI->File, NULL, _IONBF, BitInputBufferSize);
			// This needs to be called before the buffer is filled?
			ParseInputOptions(BitI, argc, argv);
			
			fseek(BitI->File, 0, SEEK_END);
			BitI->FileSize         = (uint64_t)ftell(BitI->File);
			fseek(BitI->File, 0, SEEK_SET);
			uint64_t Bytes2Read    = BitI->FileSize > BitInputBufferSize ? BitInputBufferSize : BitI->FileSize;
			uint64_t BytesRead     = fread(BitI->Buffer, 1, BitInputBufferSize, BitI->File);
			if (BytesRead < Bytes2Read) {
				Log(SYSCritical, &BitI->ErrorStatus->InitBitInput, FreadReturnedTooLittleData, "BitIO", "InitBitInput", strerror(errno));
			}
			BitI->BitsAvailable = Bytes2Bits(BytesRead);
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
			ParseOutputOptions(BitO, argc, argv);
			BitO->BitsAvailable    = BitOutputBufferSizeInBits;
			BitO->BitsUnavailable  = 0;
		}
	}
	
	void ExitBitIO(BitInput *Input, BitOutput *Output) {
		CloseBitInput(Input);
		CloseBitOutput(Output);
		closelog();
	}
	
	void CloseBitInput(BitInput *BitI) {
		fclose(BitI->File);
		memset(BitI->Buffer, 0, Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable));
		free(BitI);
	}
	
	void CloseBitOutput(BitOutput *BitO) {
		fclose(BitO->File);
		memset(BitO->Buffer, 0, Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable));
		free(BitO);
	}
	
	void CloseBitBuffer(BitBuffer *Bits) {
		Bits->BitsAvailable   = 0;
		Bits->BitsUnavailable = 0;
		Bits->Buffer          = NULL;
		Bits->ES              = NULL;
	}
	
	/* End Input parsing functions */
	
	static void UpdateInputBuffer(BitInput *BitI, int64_t RelativeOffset) {
		if (RelativeOffset == 0) {
			Log(SYSCritical, &BitI->ErrorStatus->UpdateInputBuffer, NumberNotInRange, "BitIO", "UpdateInputBuffer", NULL);
		}
		uint64_t Bytes2Read = BitI->FileSize - BitI->FilePosition > Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable) ? Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable) : BitI->FileSize - BitI->FilePosition;
		fseek(BitI->File, RelativeOffset, SEEK_CUR);
		memset(BitI, 0, Bytes2Read);
		uint64_t BytesRead = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
		if (BytesRead != Bytes2Read) {
			Log(SYSWarning, &BitI->ErrorStatus->UpdateInputBuffer, FreadReturnedTooLittleData, "BitIO", "UpdateInputBuffer", NULL);
		}
	}
	
	uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read) {
		uint64_t OutputData = 0;
		
		if (Bits2Read <= 0) {
			char Description[BitIOPathSize];
			snprintf(Description, BitIOPathSize, "Read too few bits: %d", Bits2Read);
			Log(SYSCritical, &BitI->ErrorStatus->ReadBits, NumberNotInRange, "BitIO", "ReadBits", Description);
		} else if (Bits2Read > 64) {
			char Description[BitIOPathSize];
			snprintf(Description, BitIOPathSize, "Read too many bits: %d", Bits2Read);
			Log(SYSCritical, &BitI->ErrorStatus->ReadBits, NumberNotInRange, "BitIO", "ReadBits", Description);
		} else {
			OutputData             = PeekBits(BitI, Bits2Read);
			if (BitI->ErrorStatus->PeekBits != Success) {
				BitI->ErrorStatus->ReadBits  = BitI->ErrorStatus->PeekBits;
			} else {
				BitI->BitsUnavailable += Bits2Read;
				BitI->BitsAvailable   -= Bits2Read;
			}
		}
		return OutputData;
	}
	
	uint64_t ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated) {
		uint64_t Zeros = 0;
		uint64_t  Data = 0;
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
		if (StopBit != (0|1)) {
			BitI->ErrorStatus->ReadRICE = NumberNotInRange;
		} else {
			while (PeekBits(BitI, 1) != StopBit) {
				SkipBits(BitI, 1);
				BitCount += 1;
			}
		}
		return BitCount;
	}
	
	void WriteRICE(BitOutput *BitO, bool StopBit, uint64_t Data2Write) {
		if ((StopBit != 0) || (StopBit != 1)) {
			BitO->ErrorStatus->WriteRICE = NumberNotInRange;
		} else {
			for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
				WriteBits(BitO, (1 ^ StopBit), 1);
			}
			WriteBits(BitO, StopBit, 1);
		}
	}
	
	uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek) {
	//WARNING: You CAN'T read byte by byte, it simply won't work so stop trying. Also, this is faster, instruction wise.
		uint64_t OutputData = 0;
		uint8_t  Data       = 0;
		uint8_t  BitsLeft   = Bits2Peek;
		uint8_t  BitMask    = 0;
		
		if ((Bits2Peek > 64) || (Bits2Peek <= 0)) {
			char Description[BitIOStringSize] = {0};
			snprintf(Description, BitIOStringSize, "Tried to peek %d bits, only 1-64 is valid\n", Bits2Peek);
			Log(SYSError, &BitI->ErrorStatus->PeekBits, NumberNotInRange, "BitIO", "PeekBits", Description);
		} else if (BitI->BitsAvailable < Bits2Peek) {
			// InputBuffer can't completely satisfy the request, update it.
			UpdateInputBuffer(BitI, 0);
		} else {
			while (BitsLeft > 0) {
				OutputData           <<= 1;
				BitMask                = 1 << (BitsRemaining(BitI->BitsUnavailable) - 1); // 1 << ((8 - (BitI->BitsUnavailable % 8)) -1);
				Data                   = BitI->Buffer[Bits2Bytes(BitI->BitsUnavailable)] & BitMask;
				Data                 >>= BitsRemaining(BitI->BitsUnavailable) - 1; // ((8 - (BitI->BitsUnavailable % 8)) -1);
				OutputData            += Data;
				BitI->BitsAvailable   -= 1;
				BitI->BitsUnavailable += 1;
				BitsLeft              -= 1;
			}
			BitI->BitsAvailable       += Bits2Peek;
			BitI->BitsUnavailable     -= Bits2Peek;
		}
		return OutputData;
	}

	uint64_t PeekBits2(BitInput2 *Input, uint8_t Bits2Peek) { // REBASED ON BitBuffer
		//WARNING: You CAN'T read byte by byte, it simply won't work so stop trying. Also, this is faster, instruction wise.
		uint64_t OutputData = 0;
		uint8_t  Data       = 0;
		uint8_t  BitsLeft   = Bits2Peek;
		uint8_t  BitMask    = 0;

		if ((Bits2Peek > 64) || (Bits2Peek <= 0)) {
			char Description[BitIOStringSize] = {0};
			snprintf(Description, BitIOStringSize, "Tried to peek %d bits, only 1-64 is valid\n", Bits2Peek);
			Log(SYSError, &Input->Error->PeekBits, NumberNotInRange, "BitIO", "PeekBits", Description);
		} else if (Input->Data->BitsAvailable < Bits2Peek) {
			// InputBuffer can't completely satisfy the request, update it.
			UpdateInputBuffer(Input->Source, 0);
		} else {
			while (BitsLeft > 0) {
				OutputData                  <<= 1;
				BitMask                       = 1 << (BitsRemaining(Input->Data->BitsUnavailable) - 1); // 1 << ((8 - (BitI->BitsUnavailable % 8)) -1);
				Data                          = Input->Data->Buffer[Bits2Bytes(Input->Data->BitsUnavailable)] & BitMask;
				Data                        >>= BitsRemaining(Input->Data->BitsUnavailable) - 1; // ((8 - (BitI->BitsUnavailable % 8)) -1);
				OutputData                   += Data;
				Input->Data->BitsAvailable   -= 1;
				Input->Data->BitsUnavailable += 1;
				BitsLeft                     -= 1;
			}
			Input->Data->BitsAvailable       += Bits2Peek;
			Input->Data->BitsUnavailable     -= Bits2Peek;
		}
		return OutputData;
	}
	
	void WriteBits(BitOutput *BitO, uint64_t Data2Write, size_t NumBits) {
		if (NumBits <= 0) {
			BitO->ErrorStatus->WriteBits = TriedWritingTooFewBits;
		} else {
			
			if (BitO->BitsAvailable < NumBits) {
				// Write the completed bytes out, save the uncompleted ones in the array.
			}
			while (BitO->BitsUnavailable >= Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable)) {
				fwrite(BitO->Buffer, 1, Bits2Bytes(BitO->BitsUnavailable), BitO->File);
				fflush(BitO->File);
				memcpy(&BitO->Buffer, &BitO->Buffer + Bits2Bytes(BitO->BitsUnavailable), (Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable) - Bits2Bytes(BitO->BitsUnavailable)));
				memset(BitO->Buffer, 0, Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable));
				BitO->BitsUnavailable = (BitO->BitsUnavailable - ((BitO->BitsUnavailable / 8) * 8));
			}
			while (NumBits > 0) {
				uint64_t X = Data2Write & BitsRemaining(BitO->BitsUnavailable);
				BitO->Buffer[Bits2Bytes(BitO->BitsUnavailable)] += X;
				BitO->BitsUnavailable++;
				NumBits -= 1;
			}
			// FIXME: We need to just enlarge the buffer, the call a function to flush it all out to disk.
		}
	}
	
	void InitBitBuffer(BitBuffer *Bits, uint8_t *Buffer, size_t BufferSize) {
		Bits->BitsAvailable   = Bytes2Bits(BufferSize);
		Bits->BitsUnavailable = 0;
		Bits->Buffer = Buffer;
	}
	
	uint64_t ReadBitBuffer(BitBuffer *Bits, uint8_t Bits2Read) {
		uint64_t OutputData = 0, Data = 0, BitMask = 0;
		if ((Bits2Read <= 0) || (Bits2Read > 64)) {
			char ErrorDescription[BitIOPathSize] = {0};
			snprintf(ErrorDescription, BitIOPathSize, "You requested %d bits, ReadBuffer can only read 1-64 bits at a time\n", Bits2Read);
			Log(SYSError, &Bits->ES->ReadBitBuffer, NumberNotInRange, "BitIO", "ReadBitBuffer", ErrorDescription);
		} else {
			while ((Bits2Read > 0) && (Bits->BitsAvailable >= Bits2Read)) {
				OutputData           <<= 1;
				BitMask                = 1 << (BitsRemaining(Bits->BitsUnavailable) - 1); // 1 << ((8 - (BitI->BitsUnavailable % 8)) -1);
				Data                   = Bits->Buffer[Bits2Bytes(Bits->BitsUnavailable)] & BitMask;
				Data                 >>= BitsRemaining(Bits->BitsUnavailable) - 1; // ((8 - (BitI->BitsUnavailable % 8)) -1);
				OutputData            += Data;
				Bits->BitsAvailable   -= 1;
				Bits->BitsUnavailable += 1;
				Bits2Read             -= 1;
			}
		}
		return OutputData;
	}
	
	void WriteBitBuffer(BitOutput *BitO, uint8_t *Buffer2Write, size_t BufferSize) { // Use BitBuffer as the backend
		uint8_t  Bits2Keep          = 0;
		uint8_t  Byte2Keep          = 0;
		uint64_t ExpandedBufferSize = 0;
		if (BitO->BitsAvailable + BitO->BitsUnavailable < Bytes2Bits(BufferSize)) {
			// It won't fit, so we have to enlarge the array.
			
			/* This part is just WriteBits */
			Bits2Keep = BitsRemaining(BitO->BitsUnavailable); // BitO->BitsUnavailable % 8;
			fwrite(BitO->Buffer, 1, Bits2Bytes(BitO->BitsUnavailable - Bits2Keep), BitO->File); // Write all but the uncompleted byte
			fflush(BitO->File);
			for (uint64_t Byte = Bits2Bytes(BitO->BitsUnavailable); Byte <Bits2Bytes(BitO->BitsUnavailable)+1; Byte++) {
				Byte2Keep              = BitO->Buffer[Byte];
			}
			realloc(BitO->Buffer, BufferSize);
			ExpandedBufferSize     = Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable) - BufferSize; // FIXME: is this correct?
			for (uint64_t Byte = 0; Byte < Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable); Byte++) {
				BitO->Buffer[Byte] = 0;
			}
			BitO->Buffer[0]        = Byte2Keep; // Replace the unfinished byte in the first element.
			BitO->BitsAvailable    = BitOutputBufferSizeInBits - Bits2Keep;
			BitO->BitsUnavailable  = Bits2Keep;
			
			for (uint64_t Byte = 0; Byte < BufferSize; Byte++) {
				for (uint8_t Bit = 0; Bit < 8; Bit++) {
					uint8_t BitData = Buffer2Write[Byte] & (1 << Bit);
					WriteBits(BitO, BitData, 1);
				}
			}
		}
	}
	
	void SkipBits(BitInput *BitI, int64_t Bits) { 
		// The point of this is to seek around in the file, in case you need to jumpover parts of memory
		// 
		BitI->BitsAvailable   -= Bits;
		BitI->BitsUnavailable += Bits;
		
		/*
		if ((BitI->BitsUnavailable + Bits) > BitI->BitsAvailable) {
			UpdateInputBuffer(BitI, 0);
		} else {
			uint64_t Bytes2Read = BitI->BitsAvailable < BitOutputBufferSizeInBits ? BitI->BitsAvailable : BitOutputBufferSizeInBits;
			
			uint64_t Bytes2Read2 = BitI->BitsAvailable < Bytes2Bits(BitI->BufferSize) ? BitI->BitsAvailable : BitOutputBufferSizeInBits;
			
			
			BitI->FilePosition = (uint64_t)ftell(BitI->File);
			fseek(BitI->File, BitOutputBufferSizeInBits - (BitI->BitsUnavailable + Bits), SEEK_CUR);
			memset(BitI->Buffer, 0, Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable));
			fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
		}
		 */
	}
	
	uint64_t GenerateCRC(uint8_t *DataBuffer, size_t BufferSize, uint64_t Poly, uint64_t Init, uint8_t CRCSize) {
		uint64_t Output = ~Init;
		uint64_t Polynomial = 1 << CRCSize; // Implicit bit
		Polynomial += (Poly & Power2Mask(CRCSize));
		
		// Create a BitInput version of the data buffer, to readbits from the buffer.
		BitBuffer *CRCBits = calloc(CRCSize, 1);
		InitBitBuffer(CRCBits, DataBuffer, BufferSize);
		
		Init > 1 ? Power2Mask(CRCSize) : 1;
		
		for (size_t Byte = 0; Byte < BufferSize; Byte++) {
			uint64_t Bits2XOR = ReadBitBuffer(CRCBits, CRCSize);
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
	
	void ReadUUID(BitInput *BitI, char *UUIDString[BitIOUUIDSize]) {
		//static char UUIDString[21] = {0};
		for (uint8_t Character = 0; Character < BitIOUUIDSize; Character++) {
			if (Character == 21) {
				*UUIDString[Character] = 0;
			} else if ((Character == 4) || (Character == 7) || (Character == 10) || (Character == 13)) {
				*UUIDString[Character] = 0x2D; // hyphen
			} else {
				*UUIDString[Character] = ReadBits(BitI, 8);
			}
		}
	}

	void ReadUUIDAsGUID(BitInput *Input, char *UUIDString[BitIOUUIDSize]) { // We will flip endian to a standard UUID
		for (uint8_t Character = 0; Character < BitIOGUIDSize; Character++) {
			if (Character == 21) {
				*UUIDString[Character] = 0;
			} else if ((Character == 4) || (Character == 7) || (Character == 10) || (Character == 13)) {
				*UUIDString[Character] = 0x2D;
			} else {
				uint32_t Section1 = SwapEndian32(ReadBits(Input, 32));
				// Extract bytes from int into characters
				uint16_t Section2 = SwapEndian16(ReadBits(Input, 16));
				uint16_t Section3 = SwapEndian16(ReadBits(Input, 16));
			}
		}
	}
	
	void WriteUUID(BitOutput *BitO, char UUIDString[BitIOUUIDSize]) {
		if (strlen(UUIDString) != BitIOUUIDSize) {
			BitO->ErrorStatus->WriteUUID = WrongStringSize;
		}
		for (uint8_t Character = 0; Character < BitIOUUIDSize; Character++) {
			if ((Character == 4) || (Character == 7) || (Character == 10) || (Character == 13) || (Character == 21)) {
				// Character 21 is the NULL terminator, the rest are the hyphens.
				WriteBits(BitO, UUIDString[Character], 8);
			}
			
		}
	}

	/*
	void ConvertUUID2GUID(char UUIDString[BitIOUUIDSize]) {
		// Maybe I should just use ReadBitBuffer, pop it all into various sized ints, and then use SwapEndianX.
		BitBuffer *UUID = calloc(sizeof(UUIDString), 1);
		InitBitBuffer(UUID, UUIDString, BitIOUUIDSize); // I do need to rebase BitInput and BitOutput on top of BitBuffer, so SkipBits and whatnot will work with it.
		uint32_t UUIDGroup1  = ReadBitBuffer(UUID, 32);
		SkipBits(UUID, 8); // FIXME: Remove this once SkipBits can with with BitBuffers
		uint16_t UUIDGroup2  = ReadBitBuffer(UUID, 16);
		SkipBits(UUID, 8);
		uint16_t UUIDGroup3  = ReadBitBuffer(UUID, 16);
		SkipBits(UUID, 8);
		uint16_t UUIDGroup4  = ReadBitBuffer(UUID, 16);
		SkipBits(UUID, 8);
		uint64_t UUIDGroup5  = ReadBitBuffer(UUID, 48);





		uint32_t GUIDGroup1  = SwapEndian32(UUIDGroup1);
		uint16_t GUIDGroup2  = SwapEndian16(UUIDGroup2);
		uint16_t GUIDGroup3  = SwapEndian16(UUIDGroup3);









		// Break the string into it's constituant parts, then swap the endian of the first 3 fields.
		uint8_t Data1[4] = {0}; // NULL terminating
		uint8_t Data2[2] = {0};
		uint8_t Data3[2] = {0};
		uint8_t Data4[8] = 0;

		char Flipped1[4] = {0};
		char Flipped2[2] = {0};
		char Flipped3[2] = {0};

		for (uint8_t Byte = 0; Byte < 20; Byte++) {
			if ((Byte == 0) || (Byte == 1) || (Byte == 2) || (Byte == 3)) {
				Data1[Byte] = UUIDString[Byte];
			} else if ((Byte == 5) || (Byte == 6)) {
				Data2[Byte] = UUIDString[Byte];
			} else if ((Byte == 8) || (Byte == 9)) {
				Data3[Byte] = UUIDString[Byte];
			} else if ((Byte == 11) || (Byte == 12) || (Byte == 13) || (Byte == 14) || (Byte == 15) || (Byte == 16) || (Byte == 17) || (Byte == 18)) {
				Data4[Byte] = UUIDString[Byte];
			}
		}
		// Now we Swap endian. Manually.
		for (uint8_t Byte = 0; Byte < 20; Byte++) {
			if ((Byte == 0) || (Byte == 1) || (Byte == 2) || (Byte == 3)) {
				uint8_t Byte1 = 0, Byte2 = 0, Byte3 = 0, Byte4 = 0;
				Byte1 = Data1[1];
				Byte2 = Data1[2];
				Byte3 = Data1[3];
				Byte4 = Data1[4];



				Data1[1] = Data1[4];
				Data1[4] = Data1[1];
			} else if ((Byte == 5) || (Byte == 6)) {
				Data2[Byte] = UUIDString[Byte];
			} else if ((Byte == 8) || (Byte == 9)) {
				Data3[Byte] = UUIDString[Byte];
			} else if ((Byte == 11) || (Byte == 12) || (Byte == 13) || (Byte == 14) || (Byte == 15) || (Byte == 16) || (Byte == 17) || (Byte == 18)) {
				Data4[Byte] = UUIDString[Byte];
			}
		}
	}
	 */

	void ConvertGUID2UUID(char GUIDString[BitIOGUIDSize]) {
		// merely a wrapper around ConvertUUID2GUID
	}
	
	void Log(int64_t SYSError, int64_t *ES, int64_t ESError, char Library[BitIOStringSize], char Function[BitIOStringSize], char Description[BitIOStringSize]) {
		if ((SYSError == SYSPanic) || (SYSError == SYSCritical)) {
			openlog(Library, SYSError, (LOG_PERROR|LOG_MAIL|LOG_USER));
		} else {
			openlog(Library, SYSError, (LOG_PERROR|LOG_USER));
		}
		
		time_t Time;
		char CurrentTime[26];
		time(&Time);
		strftime(CurrentTime, 26, "%A, %B %e, %g+1000: %I:%M:%S %p %Z", Time);
		
		*ES = ESError;
		syslog(SYSError, "%s: %s - %s: %s\n", CurrentTime, Library, Function, Description);
		
		free(&CurrentTime);
	}
	
	/* Default Huffman table for reading the embedded table */
	
	
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
			char String2Print[BitIOStringSize];
			snprintf(String2Print, BitIOStringSize, "One's Compliment of Length: %d != Length %d", OnesComplimentOfLength, DataLength);
			Log(SYSWarning, &BitI->ErrorStatus->DecodeHuffman, InvalidData, "BitIO", "DecodeHuffman", String2Print);
		}
		
		if (IsLastHuffmanBlock == true) {
			
		}
		
		if (HuffmanCompressionType == 0) { // No compression.
			AlignInputBits2Byte(BitI); // Skip the rest of the current byte
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
	
	uint32_t GenerateAdler32(uint8_t *Data, size_t DataSize) {
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
	
	bool VerifyAdler32(uint8_t *Data, size_t DataSize, uint32_t EmbeddedAdler32) {
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

	uint64_t ReadArithmetic(BitInput *Input, uint64_t *MaximumTable, uint64_t *MinimumTable, size_t TableSize, uint64_t Bits2Decode) {
		double Maximum = 1.0;
		double Minimum = 0.0;
		double Range   = 0.0;

		uint64_t Symbol = 0;

		double Probability = ReadBits(Input, 1); // this is just a mockup.


		while (Bits2Decode > 0) { // No, this needs to be rethought
			Range = Maximum - Minimum;
			uint64_t Symbol = FindSymbolFromProbability(Probability, MaximumTable, MinimumTable, TableSize);
			Maximum = Minimum + Range; // * p.second
			Minimum = Minimum + Range; // * p.first
		}
		return Symbol;
	}

	void WriteArithmetic(BitOutput *BitO, Probabilities *Probability, uint64_t Bits2Encode) { // Use the least precision you can get away with to be as efficent as possible.
		double Maximum = 1.0;
		double Minimum = 0.0;
		double Range   = 0.0;

		FindProbabilityFromSymbol(Probability, NULL, NULL, 65535, 0xAB37);

		while (Bits2Encode > 0) {
			Range   = Probability->Maximum - Probability->Minimum;
			Maximum = Probability->Minimum + Range;
			Minimum = Probability->Minimum + Range;
		}
		
	}

	void NaiveFizzBuzzTest(void) {
		for (uint8_t Number = 1; Number < 100; Number++) {
			if ((Number % 3 == 0) && (Number % 5 == 0)) {
				printf("FizzBuzz\n");
			} else if (Number % 5 == 0) {
				printf("Buzz\n");
			} else if (Number % 3 == 0) {
				printf("Fizz\n");
			} else {
				printf("Number: %d\n", Number);
			}
		}
	}
	
#ifdef __cplusplus
}
#endif
